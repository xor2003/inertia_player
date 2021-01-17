from __future__ import division
from __future__ import print_function

import logging
import re
import sys
from builtins import hex
from builtins import object
from builtins import range
from builtins import str
from copy import copy

import tasm.proc as proc_module
from tasm import op
from tasm.Token import Token


# ScummVM - Graphic Adventure Engine
#
# ScummVM is the legal property of its developers, whose names
# are too numerous to list here. Please refer to the COPYRIGHT
# file distributed with this source distribution.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

class CrossJump(Exception):
    pass


def parse_bin(s):
    b = s.group(1)
    v = hex(int(b, 2))
    # print "BINARY: %s -> %s" %(b, v)
    return v


def convert_number_to_c(expr):
    expr = re.sub(r'\b([0-9][0-9A-Fa-f]*)[Hh]', '0x\\1', expr)
    expr = re.sub(r'\b([0-1]+)[Bb]', parse_bin, expr)  # convert binary
    return expr


class Cpp(object):
    def __init__(self, context, outfile="", skip_first=0, blacklist=[], skip_output=[], skip_dispatch_call=False,
                 skip_addr_constants=False, header_omit_blacklisted=False, function_name_remapping={}):
        FORMAT = "%(filename)s:%(lineno)d %(message)s"
        logging.basicConfig(format=FORMAT)
        self.__logger = logging.getLogger('cpp')
        # self.logger.info('Protocol problem: %s', 'connection reset')

        self.__namespace = outfile
        self.__indirection = 0
        self.__current_size = 0
        self.__seg_prefix = ""
        self.__codeset = 'cp437'
        self.__context = context
        # self.data_seg = __context.binary_data
        self.__cdata_seg = context.c_data
        self.__hdata_seg = context.h_data
        self.__procs = context.proc_list
        self.__skip_first = skip_first
        self.__proc_queue = []
        self.__proc_done = []
        self.__blacklist = blacklist
        self.__failed = list(blacklist)
        self.__skip_output = skip_output
        self.__skip_dispatch_call = skip_dispatch_call
        self.__skip_addr_constants = skip_addr_constants
        self.__header_omit_blacklisted = header_omit_blacklisted
        self.__function_name_remapping = function_name_remapping
        self.__translated = list()  # []
        self.__proc_addr = []
        self.__used_data_offsets = set()
        self.__methods = []
        self.__temps_count = 0
        self.__pointer_flag = False
        self.lea = False
        self.__expr_size = 0
        self.__far = False
        self.body = ""
        self.__unbounded = []

    def expand_cb(self, match):
        name_original = match.group(0)
        return self.expand_cb_(name_original)

    def expand_cb_(self, name_original):
        name = name_original.lower()
        logging.debug("expand_cb name = %s indirection = %u" % (name, self.__indirection))
        if self.is_register(name) != 0:
            return name

        if self.__indirection == -1:
            try:
                offset, p, p = self.__context.get_offset(name)
            except:
                pass
            else:
                logging.debug("OFFSET = %s" % offset)
                self.__indirection = 0
                self.__used_data_offsets.add((name, offset))
                return "offset_%s" % (name,)

        try:
            g = self.__context.get_global(name)
        except:
            # logging.warning("expand_cb() global '%s' is missing" % name)
            return name_original

        # print g
        if isinstance(g, op._equ):
            logging.debug("it is equ")
            value = g.original_name
            # value = self.expand_equ(g.value)
            logging.debug("equ: %s -> %s" % (name, value))
        elif isinstance(g, op._assignment):
            logging.debug("it is assignment")
            value = g.original_name
            # value = self.expand_equ(g.value)
            logging.debug("assignment %s = %s" % (name, value))
        elif isinstance(g, proc_module.Proc):
            logging.debug("it is proc")
            if self.__indirection != -1:
                logging.error("proc %s offset %s" % (str(proc_module.Proc), str(g.offset)))
                raise Exception("invalid proc label usage")
            value = str(g.offset)
            self.__indirection = 0
        elif isinstance(g, op.var):
            logging.debug("it is var " + str(g.size))
            size = g.size
            self.__current_size = size
            if size == 0:
                raise Exception("invalid var '%s' size %u" % (name, size))
            if g.issegment:
                value = "seg_offset(%s)" % (name_original.lower())
                self.__indirection = 0
                return value
            else:
                value = "offset(%s,%s)" % (g.segment, g.name)
                if self.__seg_prefix == 'cs':
                    self.body += '\tcs=seg_offset(' + g.segment + ');\n'
            self.__indirection = 1
            #       self.indirection = 0
        elif isinstance(g, op.label):
            value = "k" + g.name.lower()  # .capitalize()
        else:
            size = g.size
            if size == 0:
                raise Exception("invalid var '%s' size %u" % (name, size))
            if self.__indirection == 0 or self.__indirection == 1:  # x0r self.indirection == 1 ??
                '''
                                if (self.__expr_size != 0 and self.__expr_size != size) or g.elements > 1:
                                        self.__pointer_flag = True
                                        if self.lea == False:
                                                value = "(db*)&m.%s" %(name_original)
                                        else:
                                                value = "offsetof(struct Mem,%s)" %(name_original)
                                        logging.debug "value %s" %value
                                else:
                                        if self.lea == False:
                                                value = "m." + name_original
                                        else:
                                                value = "offsetof(struct Mem,%s)" %(name_original)
                                '''
                value = "offsetof(struct Mem,%s)" % (name_original)
                if self.__indirection == 1:
                    self.__indirection = 0
            elif self.__indirection == -1:
                value = "%s" % g.offset
                self.__indirection = 0
            else:
                raise Exception("invalid indirection %d name '%s' size %u" % (self.__indirection, name, size))
        return value

    def is_register(self, expr):
        expr = expr.lower()
        if len(expr) == 2 and expr[0] in ['a', 'b', 'c', 'd'] and expr[1] in ['h', 'l']:
            logging.debug('is reg res 1')
            return 1
        elif expr in ['ax', 'bx', 'cx', 'dx', 'si', 'di', 'sp', 'bp', 'ds', 'cs', 'es', 'fs', 'gs', 'ss']:
            logging.debug('is reg res 2')
            return 2
        elif expr in ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'esp', 'ebp']:
            logging.debug('is reg res 4')
            return 4
        return 0

    def get_size(self, expr):
        logging.debug('get_size("%s")' % expr)
        # if isinstance(expr, string):
        #    expr = expr.strip()
        origexpr = expr

        if isinstance(expr, list) and any(i in ['[', ']'] for i in expr):
            return 0

        if isinstance(expr, list) and all(
                isinstance(i, str) or (isinstance(i, Token) and i.type == 'INTEGER') for i in expr):
            s = "".join([x.value if isinstance(x, Token) else x for x in expr])
            try:
                s = eval(s)
                expr = Token('INTEGER', str(s))
            except:
                pass

        if isinstance(expr, Token):
            if expr.type in ('register','segmentregister'):
                return self.is_register(expr.value)
            elif expr.type == 'INTEGER':
                try:
                    # v = self.__context.parse_int(expr.value)
                    v = int(expr.value)
                    size = 0
                    if v < 0:
                        v = -2 * v - 1
                    if v < 256:
                        size = 1
                    elif v < 65536:
                        size = 2
                    elif v < 4294967296:
                        size = 4
                    logging.debug('get_size res %d' % size)
                    return size
                except:
                    pass
            elif expr.type == 'STRING':
                m = re.match(r'\'(.+)\'$', expr.value)  # char constants
                if m is not None:
                    return len(m.group(1))
            elif expr.type == 'LABEL':
                name = expr.value
                logging.debug('name = %s' % name)
                try:
                    g = self.__context.get_global(name)
                    if isinstance(g, op._equ) or isinstance(g, op._assignment):
                        if g.value != origexpr:  # prevent loop
                            return self.get_size(g.value)
                        else:
                            return 0
                    logging.debug('get_size res %d' % g.size)
                    return g.size
                except:
                    pass
        elif isinstance(expr, list) and len(expr) > 2 and \
             isinstance(expr[1], str) and expr[1].lower() == 'ptr':
                expr[0] = expr[0].lower()
                #logging.debug('get_size res 1')
                return {'byte':1,'word':2,'dword':4,'qword':8,'tword':10}[expr[0]]
        elif isinstance(expr, list) and len(expr) > 1 and isinstance(expr[0], str):
            if expr[0].lower() == 'small':
                return 2
            elif expr[0].lower() == 'large':
                return 4

        if isinstance(expr, list):
            return max([self.get_size(i) for i in expr])

        if isinstance(expr, str) and expr.lower() == 'offset':
            return 2  # TODO 16 bit word size

        #if isinstance(expr, str):  # in ('+','-','*','(',')','/'):
        #    return 0

        return 0
        m = re.match(r'(cs|ss|ds|es|fs|gs):(.*)', expr)
        if m is not None:
            expr = m.group(2).strip()
            logging.debug('segment name removed ' + expr)

        m = re.match(r'\[([a-zA-Z_]\w*)\]', expr)
        if m is not None:
            expr = m.group(1).strip()
            logging.debug('square braces removed ' + expr)

        m = re.match(r'(cs|ss|ds|es|fs|gs):(.*)', expr)
        if m is not None:
            expr = m.group(2).strip()
            logging.debug('segment name removed ' + expr)

        m = re.match(r'[a-zA-Z_]\w*', expr)
        if m is not None:
            logging.debug('expr match some a-z')
            name = m.group(0)

        ex = expr
        ex.replace("\\\\", "\\")

        logging.debug('get_size res 0')
        return 0

    def expand_equ_cb(self, match):
        name = match.group(0).lower()
        logging.debug("expand_equ_cb %s" % name)
        try:
            g = self.__context.get_global(name)
            if isinstance(g, op.equ) or isinstance(g, op.assignment):
                return g.value
            return str(g.offset)
        except:
            logging.warning("some exception")
            return name

    def expand_equ(self, expr):
        n = 1
        logging.debug("expand_equ(%s)" % expr)
        # while n > 0:
        #       expr, n = re.subn(r'\b[a-zA-Z_][a-zA-Z0-9_]+\b', self.expand_equ_cb, expr)
        # while n > 0:
        expr = re.sub(r'\b[a-zA-Z_][a-zA-Z0-9_]*\b', self.expand_equ_cb, expr)
        expr = convert_number_to_c(expr)
        return "(%s)" % expr

    def expand(self, expr, def_size=0, destination=False):
        logging.debug("EXPAND(expr:\"%s\")" % expr)
        self.__seg_prefix = ""

        expr = expr.strip()
        origexpr = expr

        expr = re.sub(r'^\(\s*(.*?)\s*\)$', '\\1', expr)  # (expr)
        logging.debug("wo curls " + expr)
        expr = re.sub(r'\bnot\b', '~', expr)
        expr = re.sub(r'\band\b', '&', expr)

        size = self.get_size(expr) if def_size == 0 else def_size

        self.__expr_size = size
        # print "expr \"%s\" %d" %(expr, size)
        indirection = 0
        seg = None
        reg = True

        ex = expr
        ex.replace("\\\\", "\\")
        m = re.match(r'\'(.+)\'$', ex)  # char constants
        if m is not None:
            ex = m.group(1)
            if len(ex) == 4:
                expr = '0x'
                for i in range(0, 4):
                    # logging.debug("constant %s %d" %(ex,i))
                    ss = str(hex(ord(ex[i])))
                    # logging.debug("constant %s" %ss)
                    expr += ss[2:]
            return expr

        try:
            g = self.__context.get_global(expr)
            logging.debug("found global %s" % expr)
            if not self.lea and isinstance(g, op.var):
                logging.debug("it is not lea and it is var")
                if g.issegment:
                    return "seg_offset(%s)" % expr.lower()
                else:
                    if g.elements == 1:
                        # traceback.print_stack(file=sys.stdout)
                        return "m." + expr
                    else:
                        s = {1: "*(db*)", 2: "*(dw*)", 4: "*(dd*)"}[size]
                        return s + "&m." + expr
            if isinstance(g, op.equ) or isinstance(g, op.assignment):
                logging.debug("it is equ")
                return self.expand(g.value)

        except:
            pass

        ex = expr
        ex.replace("\\\\", "\\")
        m = re.match(r'\'(.+)\'$', ex)  # char constants
        if m is not None:
            return expr
            '''
                        s = ""
                        for c in m.group(1):
                                s = '{:02X}'.format(ord(c)) + s
                        expr = "0x" + s
                        '''

        m = re.match(r'seg\s+(.*?)$', expr)
        if m is not None:
            return m.group(1)

        expr = convert_number_to_c(expr)  # convert hex

        match_id = True
        # print "is it offset ~%s~" %expr
        prog = re.compile(r'offset\s+(.*?)$', re.I)
        m = prog.match(expr)
        if m is not None:
            indirection -= 1
            size = 2  # x0r dos 16 bit
            expr = m.group(1).strip()
            self.__current_size = 0
            expr = re.sub(r'\b[a-zA-Z_][a-zA-Z0-9_]*\b', self.expand_cb, expr)  # parse each item
            # expr = "offsetof(struct Mem,%s)" %expr

            logging.debug("after it is offset ~%s~" % expr)
            return expr

        logging.debug("1:\"%s\")" % expr)
        m = re.match(r'byte\s+ptr\s+(.*)', expr)
        if m is not None:
            expr = m.group(1).strip()
            size = 1

        m = re.match(r'dword\s+ptr\s+(.*)', expr)
        if m is not None:
            expr = m.group(1).strip()
            size = 4

        m = re.match(r'word\s+ptr\s+(.*)', expr)
        if m is not None:
            expr = m.group(1).strip()
            size = 2

        logging.debug("2:\"%s\")" % expr)

        m = re.match(r'\[(.*)\]$', expr)
        if m is not None:
            indirection += 1
            expr = m.group(1).strip()

        m = re.match(r'(cs|ss|ds|es|fs|gs):(.*)', expr)
        if m is not None:
            self.__seg_prefix = m.group(1)
            expr = m.group(2).strip()
            logging.debug("SEGMENT %s, remains: %s" % (self.__seg_prefix, expr))
        else:
            self.__seg_prefix = "ds"

        m = re.match(r'\[(.*)\]$', expr)
        if m is not None:
            indirection += 1
            expr = m.group(1).strip()

        m = re.match(r'(\[?e?([abcd][xhl])|si|di|bp|sp)([+-\]].*)?$', expr)  # var[bx+]
        if m is not None:
            reg = m.group(1)
            plus = m.group(3)
            if plus is not None and plus != ']':
                seg_prefix = self.__seg_prefix
                plus = self.expand(plus)
                self.__seg_prefix = seg_prefix
            else:
                plus = ""
            match_id = False
            # print "COMMON_REG: ", reg, plus
            expr = "%s%s" % (reg, plus)

        logging.debug("~~ " + expr)
        expr = re.sub(r'"(.)"', '\'\\1\'', expr)  # convert string
        expr = re.sub(r'\[((0x)?[0-9][a-fA-F0-9]*)\]', '+\\1', expr)  # convert [num]

        expr = re.sub(r'\[(((e?([abcd][xhl])|si|di|bp|sp)|([+-]))+)\]', '+\\1', expr)  # name[bs+si]
        expr = re.sub(r'\[(e?([abcd][xhl])|si|di|bp|sp)', '+\\1', expr)  # name[bs+si]
        expr = re.sub(r'\]', '', expr)  # name[bs+si]

        if match_id:
            expr = re.sub(r'\bbyte\s+ptr\s*', '', expr)
            expr = re.sub(r'\b[dqt]word\s+ptr\s*', '', expr)
            expr = re.sub(r'\bword\s+ptr\s*', '', expr)

            logging.debug("EXPAND() BEFORE: %d %s" % (indirection, expr))
            self.__indirection = indirection
            self.__pointer_flag = False
            self.__current_size = 0
            expr = re.sub(r'(?<![\'\"])\b[a-zA-Z_][a-zA-Z0-9_]*\b(?![\'\"])', self.expand_cb, expr)  # parse each item
            if size == 0 and self.__current_size != 0:
                size = self.__current_size
                self.__expr_size = size
            logging.debug("EXPAND() AFTER: %d %s" % (self.__indirection, expr))
            logging.debug("is a pointer %d __expr_size %d" % (self.__pointer_flag, self.__expr_size))
            '''
                        if destination and not self.lea:
                                if self.__expr_size == 1:  # x0r
                                        expr = "*(db*)(%s)" %(expr)
                                elif self.__expr_size == 2:
                                        expr = "*(dw*)(%s)" %(expr)
                                elif self.__expr_size == 4:
                                        expr = "*(dd*)(%s)" %(expr)
                        '''
            self.__pointer_flag = False
            indirection = self.__indirection
            logging.debug("AFTER: %d %s" % (indirection, expr))
            # traceback.print_stack(file=sys.stdout)

        if indirection == 1:
            if not (self.lea and not destination):
                if size == 1:
                    expr = "*(raddr(%s,%s))" % (self.__seg_prefix, expr)
                elif size == 2:
                    expr = "*(dw*)(raddr(%s,%s))" % (self.__seg_prefix, expr)
                elif size == 4:
                    expr = "*(dd*)(raddr(%s,%s))" % (self.__seg_prefix, expr)
                else:
                    logging.debug("~%s~ @invalid size 0" % expr)
                    expr = "*(dw*)(raddr(%s,%s))" % (self.__seg_prefix, expr)
                logging.debug("expr: %s" % expr)
        elif indirection == 0:
            pass
        elif indirection == -1:
            expr = "&%s" % expr
        else:
            raise Exception("invalid indirection %d" % indirection)
        return expr

    def mangle_label(self, name):
        name = name.lower()
        return re.sub(r'\$', '_tmp', name)

    def resolve_label(self, name):
        name = name.lower()
        name = re.sub(r'@', "arb", name)

        if not name in self.proc.labels:
            try:
                offset, proc, pos = self.__context.get_offset(name)
            except:
                logging.debug("no label %s, trying procedure" % name)
                try:
                    proc = self.__context.get_global(name)
                except:
                    logging.warning("resolve_label exception " + name)
                    return name

                pos = 0
                if not isinstance(proc, proc_module.Proc):
                    raise CrossJump("cross-procedure jump to non label and non procedure %s" % (name))
            self.proc.labels.add(name)
            for i in range(0, len(self.__unbounded)):
                u = self.__unbounded[i]
                if u[1] == proc:
                    if pos < u[2]:
                        self.__unbounded[i] = (name, proc, pos)
                return self.mangle_label(name)
            self.__unbounded.append((name, proc, pos))

        return self.mangle_label(name)

    def jump_to_label(self, name):
        logging.debug("jump_to_label(%s)" % name)
        name = name.strip()
        jump_proc = False

        self.__far = False
        name_original = name

        prog = re.compile(r'^\s*(near|far|short)\s*(ptr)?\s*', re.I)  # x0r TODO
        name = re.sub(prog, '', name)
        name = self.resolve_label(name)
        logging.debug("label %s" % name)
        hasglobal = False
        if name in self.__blacklist:
            jump_proc = True

        if self.__context.has_global(name):
            hasglobal = True
            g = self.__context.get_global(name)
            if isinstance(g, proc_module.Proc):
                jump_proc = True

        if jump_proc:
            if name in self.__function_name_remapping:
                return "%s" % self.__function_name_remapping[name]
            else:
                return "%s" % name
        else:
            # TODO: name or self.resolve_label(name) or self.mangle_label(name)??
            if name in self.proc.retlabels:
                return "return /* (%s) */" % (name)
            ## x0r return "goto %s" %self.resolve_label(name)
            if not hasglobal:
                name = self.expand(name, destination=True)
                self.body += "__disp = (dw)(" + name + ");\n"
                name = "__dispatch_call"
            else:
                if isinstance(g, op.label) and g.far:
                    self.__far = True  # make far calls to far procs

            m = re.match(r'far\s+ptr', name_original)
            if m is not None:
                self.__far = True
            m = re.match(r'near\s+ptr', name_original)
            if m is not None:
                self.__far = False

            return name

    def _label(self, name, proc):
        ret = ""
        if proc:
            ret += " // Procedure %s() start\n" % (name)
        ret += "%s:\n" % self.mangle_label(name)
        return ret

    def schedule(self, name):
        name = name.lower()
        if name in self.__proc_queue or name in self.__proc_done or name in self.__failed:
            return
        logging.debug("+scheduling function %s..." % name)
        self.__proc_queue.append(name)

    def _call(self, name):
        logging.debug("cpp._call(%s)" % name)
        ret = ""
        # dst = self.expand(name, destination = True)
        dst = self.jump_to_label(name)
        if dst != "__dispatch_call":
            dst = "k" + dst
        else:
            dst = "__disp"

        if self.__far:
            ret += "\tR(CALLF(%s));\n" % (dst)
        else:
            ret += "\tR(CALL(%s));\n" % (dst)
        '''
                name = name.lower()
                if self.is_register(name):
                        ret += "\t__dispatch_call(%s);\n" %self.expand(name, 2)
                        return
                if name in self.__function_name_remapping:
                        ret += "\tR(CALL(%s));\n" %self.__function_name_remapping[name]
                else:
                        ret += "\tR(CALL(%s));\n" %name
                self.schedule(name)
        '''
        return ret

    def _ret(self):
        return "\tR(RETN);\n"

    def parse2(self, dst, src):
        dst_size, src_size = self.get_size(dst), self.get_size(src)
        if dst_size == 0:
            if src_size == 0:
                logging.debug("parse2: %s %s both sizes are 0" % (dst, src))
                # raise Exception("both sizes are 0")
            dst_size = src_size
        if src_size == 0:
            src_size = dst_size

        dst = self.expand(dst, dst_size, destination=True)
        src = self.expand(src, src_size)
        return dst, src

    def _sub(self, dst, src):
        self.d, self.s = self.parse2(dst, src)
        if self.d == self.s:
            return "\t%s = 0;AFFECT_ZF(0); AFFECT_SF(%s,0);\n" % (self.d, self.d)
        else:
            return "\tR(SUB(%s, %s));\n" % (self.d, self.s)

    def _xor(self, dst, src):
        self.d, self.s = self.parse2(dst, src)
        if self.d == self.s:
            return "\t%s = 0;AFFECT_ZF(0); AFFECT_SF(%s,0);\n" % (self.d, self.d)
        else:
            return "\tR(XOR(%s, %s));\n" % (self.d, self.s)

    def _mul(self, src):
        res = []
        size = 0
        for i in src:
            if size == 0:
                size = self.get_size(i)
            else:
                break
        for i in src:
            res.append(self.expand(i, size))
        if size == 0:
            size = self.__current_size
        return "\tR(MUL%d_%d(%s));\n" % (len(src), size, ",".join(res))

    def _imul(self, src):
        res = []
        size = 0
        for i in src:
            if size == 0:
                size = self.get_size(i)
            else:
                break
        for i in src:
            res.append(self.expand(i, size))
        if size == 0:
            size = self.__current_size
        return "\tR(IMUL%d_%d(%s));\n" % (len(src), size, ",".join(res))

    def _div(self, src):
        size = self.get_size(src)
        src = self.expand(src)
        return "\tR(DIV%d(%s));\n" % (size, src)

    def _idiv(self, src):
        size = self.get_size(src)
        src = self.expand(src)
        return "\tR(IDIV%d(%s));\n" % (size, src)

    def _jz(self, label):
        if re.match('.*?(\$\+2)', label) is None:  # skip jz $+2
            label = self.jump_to_label(label)
            return "\t\tR(JZ(%s));\n" % label
        return "\n"

    def _jnz(self, label):
        label = self.jump_to_label(label)
        return "\t\tR(JNZ(%s));\n" % label

    def _jbe(self, label):
        label = self.jump_to_label(label)
        return "\t\tR(JBE(%s));\n" % label

    def _ja(self, label):
        label = self.jump_to_label(label)
        return "\t\tR(JA(%s));\n" % label

    def _jc(self, label):
        label = self.jump_to_label(label)
        return "\t\tR(JC(%s));\n" % label

    def _jnc(self, label):
        label = self.jump_to_label(label)
        return "\t\tR(JNC(%s));\n" % label

    def _push(self, regs):
        p = ""
        for r in regs:
            if self.get_size(r):
                self.__temps_count += 1
                r = self.expand(r)
                p += "\tR(PUSH(%s));\n" % (r)
        return p

    def _pop(self, regs):
        p = ""
        for r in regs:
            self.__temps_count -= 1
            r = self.expand(r)
            p += "\tR(POP(%s));\n" % r
        return p

    def _rep(self, arg):
        return "\tREP\n"

    def _cmpsb(self):
        return "CMPSB;\n"

    def _lodsb(self):
        return "LODSB;\n"

    def _lodsw(self):
        return "LODSW;\n"

    def _lodsd(self):
        return "LODSD;\n"

    def _stosb(self, n, clear_cx):
        return "STOSB;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _stosw(self, n, clear_cx):
        return "STOSW;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _stosd(self, n, clear_cx):
        return "STOSD;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _movsb(self, n, clear_cx):
        return "MOVSB;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _movsw(self, n, clear_cx):
        return "MOVSW;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _movsd(self, n, clear_cx):
        return "MOVSD;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _scasb(self):
        return "SCASB;\n"

    def _scasw(self):
        return "SCASW;\n"

    def _scasd(self):
        return "SCASD;\n"

    def _scas(self, src):
        size = self.get_size(src)
        src = self.expand(src)
        return "\tR(SCAS(%s,%d));\n" % (src, size)

    def __proc(self, name, def_skip=0):
        logging.debug("cpp::__proc(%s)" % name)
        # traceback.print_stack(file=sys.stdout)
        try:
            skip = def_skip
            self.__temps_count = 0
            self.temps_max = 0
            if self.__context.has_global(name):
                self.proc = self.__context.get_global(name)
            else:
                logging.debug("No procedure named %s, trying label" % name)
                off, src_proc, skip = self.__context.get_offset(name)

                self.proc = proc_module.Proc(name)
                self.proc.stmts = copy(src_proc.stmts)
                self.proc.labels = copy(src_proc.labels)
                self.proc.retlabels = copy(src_proc.retlabels)
                # for p in xrange(skip, len(self.proc.stmts)):
                #       s = self.proc.stmts[p]
                #       if isinstance(s, op.basejmp):
                #               o, p, s = self.__context.get_offset(s.label)
                #               if p == src_proc and s < skip:
                #                       skip = s

            self.__proc_addr.append((name, self.proc.offset))
            self.body = ""
            '''
                        if name in self.__function_name_remapping:
                                self.body += "void %sContext::%s() {\n" %(self.namespace, self.__function_name_remapping[name]);
                        else:
                                self.body += "void %sContext::%s() {\n" %(self.namespace, name);
                        '''
            if name in self.__function_name_remapping:
                self.body += "int %s() {\ngoto %s;\n" % (
                    self.__function_name_remapping[name], self.__context.entry_point)
            else:
                self.body += """
int init(struct _STATE* _state)
{
X86_REGREF

_state->_indent=0;
logDebug=fopen("%s.log","w");
ecx=0;

initscr();
resize_term(25, 80);
 cbreak(); // put keys directly to program
    noecho(); // do not echo
    keypad(stdscr, TRUE); // provide keypad buttons

    if (!has_colors())
    {
        printw("Unable to use colors");
    }
        start_color();

        realtocurs();
        curs_set(0);

        refresh();

  log_debug("~~~ heap_size=%%d para=%%d heap_ofs=%%d", HEAP_SIZE, (HEAP_SIZE >> 4), seg_offset(heap) );
  /* We expect ram_top as Kbytes, so convert to paragraphs */
  mcb_init(seg_offset(heap), (HEAP_SIZE >> 4) - seg_offset(heap) - 1, MCB_LAST);

  R(MOV(ss, seg_offset(stack)));
#if _BITS == 32
  esp = ((dd)(db*)&m.stack[STACK_SIZE - 4]);
#else
  esp=0;
  sp = STACK_SIZE - 4;
  es=0;
 *(dw*)(raddr(0,0x408)) = 0x378; //LPT
#endif

        return(0);
}

void %s(_offsets _i, struct _STATE* _state){
X86_REGREF
__disp=_i;
if (__disp==kbegin) goto %s;
else goto __dispatch_call;
""" % (self.__namespace, name, self.__context.entry_point)

            logging.info(name)
            self.proc.optimize()
            self.__unbounded = []
            self.proc.visit(self, skip)
            '''
                        #adding remaining labels:
                        for i in xrange(0, len(self.__unbounded)):
                                u = self.__unbounded[i]
                                logging.info "UNBOUNDED: ", u
                                proc = u[1]
                                for p in xrange(u[2], len(proc.stmts)):
                                        s = proc.stmts[p]
                                        if isinstance(s, op.basejmp):
                                                self.resolve_label(s.label)

                        #adding statements
                        #BIG FIXME: this is quite ugly to handle code analysis from the code generation. rewrite me!
                        for label, proc, offset in self.__unbounded:
                                self.body += "\tR(return);\n" #we need to return before calling code from the other proc
                                self.body += "/*continuing to __unbounded code: %s from %s:%d-%d*/\n" %(label, proc.name, offset, len(proc.stmts))
                                start = len(self.proc.stmts)
                                self.proc.add_label(label)
                                for s in proc.stmts[offset:]:
                                        if isinstance(s, op.label):
                                                self.proc.labels.add(s.name)
                                        self.proc.stmts.append(s)
                                self.proc.add("ret")
                                logging.info "skipping %d instructions, todo: %d" %(start, len(self.proc.stmts) - start)
                                logging.info "re-optimizing..."
                                self.proc.optimize(keep_labels=[label])
                                self.proc.visit(self, start)
                        '''
            # self.body += "}\n"; # x0r function end
            if name not in self.__skip_output:
                self.__translated.insert(0, self.body)
            self.proc = None
            if self.__temps_count > 0:
                logging.warning("temps count == %d at the exit of proc" % self.__temps_count)
            return True
        except (CrossJump, op.Unsupported) as e:
            logging.error("%s: ERROR: %s" % (name, e))
            self.__failed.append(name)
        except:
            raise

    def get_type(self, width):
        return "uint%d_t" % (width * 8)

    def write_stubs(self, fname, procs):
        if sys.version_info >= (3, 0):
            fd = open(fname, "wt", encoding=self.__codeset)
        else:
            fd = open(fname, "wt")

        fd.write("//namespace %s {\n" % self.__namespace)
        for p in procs:
            if p in self.__function_name_remapping:
                fd.write("void %sContext::%s() {\n\t::error(\"%s\");\n}\n\n" % (
                    self.__namespace, self.__function_name_remapping[p], self.__function_name_remapping[p]))
            else:
                fd.write("void %sContext::%s() {\n\t::error(\"%s\");\n}\n\n" % (self.__namespace, p, p))
        fd.write("//} // End of namespace  %s\n" % self.__namespace)
        fd.close()

    def generate(self, start):
        # print self.prologue()
        # print __context
        fname = self.__namespace.lower() + ".cpp"
        header = self.__namespace.lower() + ".h"
        banner = """/* PLEASE DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE LOST! LOOK FOR README FOR DETAILS */

/* 
 *
 */
"""
        if sys.version_info >= (3, 0):
            self.fd = open(fname, "wt", encoding=self.__codeset)
            self.hd = open(header, "wt", encoding=self.__codeset)
        else:
            self.fd = open(fname, "wt")
            self.hd = open(header, "wt")
        hid = "TASMRECOVER_%s_STUBS_H__" % self.__namespace.upper().replace('-', '_')
        self.hd.write("""#ifndef %s
#define %s

%s""" % (hid, hid, banner))
        self.fd.write("""%s
#include \"%s\"
#include <curses.h>

//namespace %s {
""" % (banner, header, self.__namespace))

        self.__proc_queue.append(start)
        while len(self.__proc_queue):
            name = self.__proc_queue.pop()
            if name in self.__failed or name in self.__proc_done:
                continue
            if len(self.__proc_queue) == 0 and len(self.__procs) > 0:
                logging.info("queue's empty, adding remaining __procs:")
                for p in self.__procs:
                    self.schedule(p)
                self.__procs = []
            logging.info("continuing on %s" % name)
            self.__proc_done.append(name)
            self.__proc(name)
            self.__methods.append(name)
        # self.write_stubs("_stubs.cpp", self.__failed)
        self.__methods += self.__failed
        done, failed = len(self.__proc_done), len(self.__failed)

        self.fd.write("\n")
        self.fd.write("\n".join(self.__translated))
        self.fd.write("\n")
        logging.info(
            "%d ok, %d failed of %d, %3g%% translated" % (done, failed, done + failed, 100.0 * done / (done + failed)))
        logging.info("\n".join(self.__failed))
        # data_bin = self.data_seg
        cdata_bin = self.__cdata_seg
        hdata_bin = self.__hdata_seg
        data_impl = ""
        n = 0
        # comment = ""

        self.fd.write("\nreturn;\n__dispatch_call:\nswitch (__disp) {\n")
        offsets = []
        for k, v in list(self.__context.get_globals().items()):
            k = re.sub(r'[^A-Za-z0-9_]', '_', k)
            if isinstance(v, op.label):
                offsets.append((k.lower(), k))

        offsets = sorted(offsets, key=lambda t: t[1])
        for o in offsets:
            logging.debug(o)
            self.fd.write("case k%s: \tgoto %s;\n" % o)
        self.fd.write("default: log_error(\"Jump/call to nothere %d\\n\", __disp);stackDump(_state); abort();\n")
        self.fd.write("};\n}\n")

        data_impl += "\nstruct Memory m = {\n"
        for v in cdata_bin:
            # data_impl += "0x%02x, " %v
            data_impl += "%s" % v
            n += 1
        data_impl += """
                {0}
                """
        data_impl += "};\n"
        '''
                data_impl += "\n\tuint8_t m[] = {\n\t\t"

                for v in data_bin:
                        data_impl += "0x%02x, " %v
                        n += 1

                        comment += chr(v) if (v >= 0x20 and v < 0x7f and v != ord('\\')) else "."
                        if (n & 0xf) == 0:
                                data_impl += "\n\t\t//0x%04x: %s\n\t\t" %(n - 16, comment)
                                comment = str()
                        #elif (n & 0x3) == 0:
                        #       comment += " "
                data_impl += "};\n\t//ds.assign(src, src + sizeof(src));\n"
                '''

        self.hd.write(
            """\n#include "asm.h"

//namespace %s {

"""
            % (self.__namespace))

        if self.__skip_addr_constants == False:
            for name, addr in self.__proc_addr:
                self.hd.write("static const uint16_t addr_%s = 0x%04x;\n" % (name, addr))

        # for name,addr in self.__used_data_offsets:
        #       self.hd.write("static const uint16_t offset_%s = 0x%04x;\n" %(name, addr))

        offsets = []
        for k, v in list(self.__context.get_globals().items()):
            k = re.sub(r'[^A-Za-z0-9_]', '_', k)
            if isinstance(v, op.var):
                pass  # offsets.append((k.capitalize(), hex(v.offset)))
            elif isinstance(v, op.equ) or isinstance(v, op.assignment):
                offsets.append((k.capitalize(), self.expand_equ(v.value)))  # fixme: try to save all constants here
            # elif isinstance(v, op.label):
            #       offsets.append((k.capitalize(), hex(v.line_number)))

        offsets = sorted(offsets, key=lambda t: t[1])
        for o in offsets:
            self.fd.write("static const uint16_t k%s = %s;\n" % o)
        self.fd.write("\n")

        # self.hd.write("\nenum _offsets MYINT_ENUM {\n")
        offsets = []
        for k, v in list(self.__context.get_globals().items()):
            k = re.sub(r'[^A-Za-z0-9_]', '_', k)
            if isinstance(v, op.var):
                pass  # offsets.append((k.capitalize(), hex(v.offset)))
            elif isinstance(v, op.equ) or isinstance(v, op.assignment):
                pass  # offsets.append((k.capitalize(), self.expand_equ(v.value))) #fixme: try to save all constants here
            elif isinstance(v, op.label):
                offsets.append((k.lower(), hex(v.line_number)))

        offsets = sorted(offsets, key=lambda t: t[1])
        self.hd.write("#define kbegin 0x1001\n")
        for o in offsets:
            self.hd.write(("#define k%s %s\n" % o))
        # self.hd.write("};\n")

        data_head = "\nstruct MYPACKED Memory{\n"
        for v in hdata_bin:
            # data_impl += "0x%02x, " %v
            data_head += "%s" % v
            # n += 1

        data_head += '''
                        db stack[STACK_SIZE];
                        db heap[HEAP_SIZE];
                '''

        data_head += "};\n"
        self.hd.write(data_head)

        self.hd.write(
            """
//class %sContext {
//public:
//      %sContext() {}

//      void _start();
"""
            % (self.__namespace, self.__namespace))
        if self.__skip_dispatch_call == False:
            self.hd.write(
                """     
""")
        '''
                for p in set(self.__methods):
                        if p in self.__blacklist:
                                if self.__header_omit_blacklisted == False:
                                        self.hd.write("\t//void %s();\n" %p)
                        else:
                                if p in self.__function_name_remapping:
                                        self.hd.write("\tvoid %s();\n" %self.__function_name_remapping[p])
                                else:
                                        self.hd.write("\tvoid %s();\n" %p)
                '''
        self.hd.write("//};\n\n//} // End of namespace DreamGen\n\n#endif\n")
        self.hd.close()

        # self.fd.write("void %sContext::__start() { %s\t%s(); \n}\n" %(self.namespace, data_impl, start))
        self.fd.write(" %s\n" % data_impl)

        if self.__skip_dispatch_call == False:
            self.fd.write("\nvoid %sContext::__dispatch_call(uint16_t addr) {\n\tswitch(addr) {\n" % self.__namespace)
            self.__proc_addr.sort(cmp=lambda x, y: x[1] - y[1])
            for name, addr in self.__proc_addr:
                self.fd.write("\t\tcase addr_%s: %s(); break;\n" % (name, name))
            self.fd.write("\t\tdefault: ::error(\"invalid call to %04x dispatched\", (uint16_t)ax);")
            self.fd.write("\n\t}\n}")

        self.fd.write("\n\n//} // End of namespace DreamGen\n")
        self.fd.close()

    def _lea(self, dst, src):
        self.lea = True
        r = "\tR(%s = %s);\n" % (dst, self.expand(src))
        self.lea = False
        return r

    def _movs(self, dst, src):
        size = self.get_size(dst)
        a, b = self.parse2(dst, src)
        return "MOVS(%s, %s, %d);\n" % (a, b, size)

    def _repe(self, arg):
        return "\tREPE\n"

    def _repne(self, arg):
        return "\tREPNE\n"

    def _lods(self, src):
        size = self.get_size(src)
        src = self.expand(src)
        return "\tR(LODS(%s,%d));\n" % (src, size)

    def _leave(self):
        return "\tR(MOV(esp, ebp));\nR(POP(ebp));\n"

    def _int(self, dst):
        dst = self.expand(dst)
        return "\tR(_INT(%s));\n" % (dst)

    def _instruction0(self, cmd):
        return "\tR(%s);\n" % (cmd.upper())

    def _instruction1(self, cmd, dst):
        dst = self.expand(dst)
        return "\tR(%s(%s));\n" % (cmd.upper(), dst)

    def _jump(self, cmd, label):
        if re.match('.*?(\$\+2)', label) is None:  # skip j* $+2
            label = self.jump_to_label(label)
            return "\t\tR(%s(%s));\n" % (cmd.upper(), label)
        return "\n"

    def _instruction2(self, cmd, dst, src):
        self.a, self.b = self.parse2(dst, src)
        return "\tR(%s(%s, %s));\n" % (cmd.upper(), self.a, self.b)

    def _instruction3(self, cmd, dst, src, c):
        self.a, self.b = self.parse2(dst, src)
        self.c = self.expand(c)
        return "\tR(%s(%s, %s, %s));\n" % (cmd.upper(), self.a, self.b, self.c)

    def _assignment(self, dst, src):
        return "#undef %s\n#define %s %s\n" % (dst, dst, self.expand(src))

    def _equ(self, dst, src):
        return "#define %s %s\n" % (dst, src)
