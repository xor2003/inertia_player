from __future__ import print_function
from __future__ import division

import logging
import re
import sys
from builtins import hex
from builtins import object
from builtins import range
from builtins import str
from copy import copy

from tasm import op
import tasm.proc as proc_module


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


def convert_number(expr):
    expr = re.sub(r'\b([0-9][0-9A-Fa-f]*)[Hh]', '0x\\1', expr)
    expr = re.sub(r'\b([0-1]+)[Bb]', parse_bin, expr)  # convert binary
    return expr


class Cpp(object):
    def __init__(self, context, outfile="", skip_first=0, blacklist=[], skip_output=[], skip_dispatch_call=False,
                 skip_addr_constants=False, header_omit_blacklisted=False, function_name_remapping={}):
        FORMAT = "%(filename)s:%(lineno)d %(message)s"
        logging.basicConfig(format=FORMAT)
        self.logger = logging.getLogger('cpp')
        # self.logger.info('Protocol problem: %s', 'connection reset')

        self.namespace = outfile
        self.indirection = 0
        self.current_size = 0
        self.seg_prefix = ""
        self.codeset = 'cp437'
        self.context = context
        self.data_seg = context.binary_data
        self.cdata_seg = context.c_data
        self.hdata_seg = context.h_data
        self.procs = context.proc_list
        self.skip_first = skip_first
        self.proc_queue = []
        self.proc_done = []
        self.blacklist = blacklist
        self.failed = list(blacklist)
        self.skip_output = skip_output
        self.skip_dispatch_call = skip_dispatch_call
        self.skip_addr_constants = skip_addr_constants
        self.header_omit_blacklisted = header_omit_blacklisted
        self.function_name_remapping = function_name_remapping
        self.translated = list()  # []
        self.proc_addr = []
        self.used_data_offsets = set()
        self.methods = []
        self.pointer_flag = False
        self.lea = False
        self.expr_size = 0
        self.far = False

    def expand_cb(self, match):
        name_original = match.group(0)
        return self.expand_cb_(name_original)

    def expand_cb_(self, name_original):
        name = name_original.lower()
        logging.debug("expand_cb name = %s indirection = %u" % (name, self.indirection))
        if len(name) == 2 and \
                ((name[0] in ['a', 'b', 'c', 'd'] and name[1] in ['x', 'h', 'l']) or name in ['si', 'di', 'bp', 'es',
                                                                                              'ds', 'cs', 'fs', 'gs',
                                                                                              'ss']):
            return name

        elif len(name) == 3 and \
                (name in ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp', 'eip']):
            return name

        if self.indirection == -1:
            try:
                offset, p, p = self.context.get_offset(name)
            except:
                pass
            else:
                logging.debug("OFFSET = %s" % offset)
                self.indirection = 0
                self.used_data_offsets.add((name, offset))
                return "offset_%s" % (name,)

        try:
            g = self.context.get_global(name)
        except:
            logging.warning("expand_cb exception on name = %s" % name)
            return name_original

        # print g
        if isinstance(g, op.equ):
            logging.debug("it is equ")
            value = name.upper()
            # value = self.expand_equ(g.value)
            logging.debug("equ: %s -> %s" % (name, value))
        elif isinstance(g, op.assignment):
            logging.debug("it is assignment")
            value = name.upper()
            # value = self.expand_equ(g.value)
            logging.debug("assignment %s = %s" % (name, value))
        elif isinstance(g, proc_module.Proc):
            logging.debug("it is proc")
            if self.indirection != -1:
                logging.error("proc %s offset %s" % (str(proc_module.Proc), str(g.offset)))
                raise Exception("invalid proc label usage")
            value = str(g.offset)
            self.indirection = 0
        elif isinstance(g, op.var):
            logging.debug("it is var " + str(g.size))
            size = g.size
            self.current_size = size
            if size == 0:
                raise Exception("invalid var '%s' size %u" % (name, size))
            if g.issegment:
                value = "seg_offset(%s)" % (name_original.lower())
                self.indirection = 0
                return value
            else:
                value = "offset(%s,%s)" % (g.segment, g.name)
                if self.seg_prefix == 'cs':
                    self.body += '\tcs=seg_offset(' + g.segment + ');\n'
            self.indirection = 1
            #       self.indirection = 0
        elif isinstance(g, op.label):
            value = "k" + g.name.lower()  # .capitalize()
        else:
            size = g.size
            if size == 0:
                raise Exception("invalid var '%s' size %u" % (name, size))
            if self.indirection == 0 or self.indirection == 1:  # x0r self.indirection == 1 ??
                '''
                                if (self.expr_size != 0 and self.expr_size != size) or g.elements > 1:
                                        self.pointer_flag = True
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
                if self.indirection == 1:
                    self.indirection = 0
            elif self.indirection == -1:
                value = "%s" % g.offset
                self.indirection = 0
            else:
                raise Exception("invalid indirection %d name '%s' size %u" % (self.indirection, name, size))
        return value

    def is_register(self, expr):
        if len(expr) == 2 and expr[0] in ['a', 'b', 'c', 'd'] and expr[1] in ['h', 'l']:
            logging.debug('is reg res 1')
            return 1
        if expr in ['ax', 'bx', 'cx', 'dx', 'si', 'di', 'sp', 'bp', 'ds', 'cs', 'es', 'fs', 'gs', 'ss']:
            logging.debug('is reg res 2')
            return 2
        if expr in ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'esp', 'ebp']:
            logging.debug('is reg res 4')
            return 4
        return 0

    def get_size(self, expr):
        expr = expr.strip()
        origexpr = expr
        logging.debug('get_size("%s")' % expr)

        try:
            v = self.context.parse_int(expr)
            size = 0
            if v < 0:
                raise Exception("negative not handled yet")
            elif v < 256:
                size = 1
            elif v < 65536:
                size = 2
            elif v < 4294967296:
                size = 4
            logging.debug('get_size res %d' % size)
            return size
        except:
            pass

        if re.match(r'\bbyte\s+ptr\s', expr) is not None:
            logging.debug('get_size res 1')
            return 1

        if re.match(r'\bword\s+ptr\s', expr) is not None:
            logging.debug('get_size res 2')
            return 2

        if re.match(r'\bdword\s+ptr\s', expr) is not None:
            logging.debug('get_size res 4')
            return 4

        if re.match(r'\bqword\s+ptr\s', expr) is not None:
            logging.debug('get_size res 4')
            return 8

        if re.match(r'\btword\s+ptr\s', expr) is not None:
            logging.debug('get_size res 4')
            return 10

        size = self.is_register(expr)
        if size:
            return size

        m = re.match(r'(cs|ss|ds|es|fs|gs):(.*)', expr)
        if m is not None:
            expr = m.group(2).strip()

        m = re.match(r'\[([a-zA-Z_]\w*)\]', expr)
        if m is not None:
            expr = m.group(1).strip()

        m = re.match(r'(cs|ss|ds|es|fs|gs):(.*)', expr)
        if m is not None:
            expr = m.group(2).strip()

        m = re.match(r'[a-zA-Z_]\w*', expr)
        if m is not None:
            logging.debug('expr match some a-z')
            name = m.group(0)
            logging.debug('name = %s' % name)
            try:
                g = self.context.get_global(name)
                if isinstance(g, op.equ) or isinstance(g, op.assignment):
                    if g.value != origexpr:  # prevent loop
                        return self.get_size(g.value)
                    else:
                        return 0
                logging.debug('get_size res %d' % g.size)
                return g.size
            except:
                pass

        ex = expr
        ex.replace("\\\\", "\\")
        m = re.match(r'\'(.+)\'$', ex)  # char constants
        if m is not None:
            return len(m.group(1))

        logging.debug('get_size res 0')
        return 0

    def expand_equ_cb(self, match):
        name = match.group(0).lower()
        logging.debug("expand_equ_cb %s" % name)
        try:
            g = self.context.get_global(name)
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
        expr = convert_number(expr)
        return "(%s)" % expr

    def expand(self, expr, def_size=0, destination=False):
        logging.debug("EXPAND(expr:\"%s\")" % expr)
        self.seg_prefix = ""

        expr = expr.strip()
        origexpr = expr

        expr = re.sub(r'^\(\s*(.*?)\s*\)$', '\\1', expr)  # (expr)
        logging.debug("wo curls " + expr)

        size = self.get_size(expr) if def_size == 0 else def_size

        self.expr_size = size
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
            g = self.context.get_global(expr)
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

        expr = convert_number(expr)  # convert hex

        match_id = True
        # print "is it offset ~%s~" %expr
        prog = re.compile(r'offset\s+(.*?)$', re.I)
        m = prog.match(expr)
        if m is not None:
            indirection -= 1
            size = 2  # x0r dos 16 bit
            expr = m.group(1).strip()
            self.current_size = 0
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

        m = re.match(r'(\w{2,2}):(.*)$', expr)
        if m is not None:
            self.seg_prefix = m.group(1)
            expr = m.group(2).strip()
            logging.debug("SEGMENT %s, remains: %s" % (self.seg_prefix, expr))
        else:
            self.seg_prefix = "ds"

        m = re.match(r'\[(.*)\]$', expr)
        if m is not None:
            indirection += 1
            expr = m.group(1).strip()

        m = re.match(r'(\[?e?([abcd][xhl])|si|di|bp|sp)([+-\]].*)?$', expr)  # var[bx+]
        if m is not None:
            reg = m.group(1)
            plus = m.group(3)
            if plus is not None and plus != ']':
                seg_prefix = self.seg_prefix
                plus = self.expand(plus)
                self.seg_prefix = seg_prefix
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
            self.indirection = indirection
            self.pointer_flag = False
            self.current_size = 0
            expr = re.sub(r'\b[a-zA-Z_][a-zA-Z0-9_]*\b', self.expand_cb, expr)  # parse each item
            if size == 0 and self.current_size != 0:
                size = self.current_size
                self.expr_size = size
            logging.debug("EXPAND() AFTER: %d %s" % (self.indirection, expr))
            logging.debug("is a pointer %d expr_size %d" % (self.pointer_flag, self.expr_size))
            '''
                        if destination and not self.lea:
                                if self.expr_size == 1:  # x0r
                                        expr = "*(db*)(%s)" %(expr)
                                elif self.expr_size == 2:
                                        expr = "*(dw*)(%s)" %(expr)
                                elif self.expr_size == 4:
                                        expr = "*(dd*)(%s)" %(expr)
                        '''
            self.pointer_flag = False
            indirection = self.indirection
            logging.debug("AFTER: %d %s" % (indirection, expr))
            # traceback.print_stack(file=sys.stdout)

        if indirection == 1:
            if not (self.lea and not destination):
                if size == 1:
                    expr = "*(raddr(%s,%s))" % (self.seg_prefix, expr)
                elif size == 2:
                    expr = "*(dw*)(raddr(%s,%s))" % (self.seg_prefix, expr)
                elif size == 4:
                    expr = "*(dd*)(raddr(%s,%s))" % (self.seg_prefix, expr)
                else:
                    logging.debug("~%s~ @invalid size 0" % expr)
                    expr = "*(dw*)(raddr(%s,%s))" % (self.seg_prefix, expr)
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
                offset, proc, pos = self.context.get_offset(name)
            except:
                logging.debug("no label %s, trying procedure" % name)
                try:
                    proc = self.context.get_global(name)
                except:
                    logging.warning("resolve_label exception")
                    return name

                pos = 0
                if not isinstance(proc, proc_module.Proc):
                    raise CrossJump("cross-procedure jump to non label and non procedure %s" % (name))
            self.proc.labels.add(name)
            for i in range(0, len(self.unbounded)):
                u = self.unbounded[i]
                if u[1] == proc:
                    if pos < u[2]:
                        self.unbounded[i] = (name, proc, pos)
                return self.mangle_label(name)
            self.unbounded.append((name, proc, pos))

        return self.mangle_label(name)

    def jump_to_label(self, name):
        logging.debug("jump_to_label(%s)" % name)
        name = name.strip()
        jump_proc = False

        self.far = False
        name_original = name

        prog = re.compile(r'^\s*(near|far|short)\s*(ptr)?\s*', re.I)  # x0r TODO
        name = re.sub(prog, '', name)
        name = self.resolve_label(name)
        logging.debug("label %s" % name)
        hasglobal = False
        if name in self.blacklist:
            jump_proc = True

        if self.context.has_global(name):
            hasglobal = True
            g = self.context.get_global(name)
            if isinstance(g, proc_module.Proc):
                jump_proc = True

        if jump_proc:
            if name in self.function_name_remapping:
                return "%s" % self.function_name_remapping[name]
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
                    self.far = True  # make far calls to far procs

            m = re.match(r'far\s+ptr', name_original)
            if m is not None:
                self.far = True
            m = re.match(r'near\s+ptr', name_original)
            if m is not None:
                self.far = False

            return name

    def _label(self, name, proc):
        if proc:
            self.body += " // Procedure %s() start\n" % (name)
        self.body += "%s:\n" % self.mangle_label(name)

    def schedule(self, name):
        name = name.lower()
        if name in self.proc_queue or name in self.proc_done or name in self.failed:
            return
        logging.debug("+scheduling function %s..." % name)
        self.proc_queue.append(name)

    def _call(self, name):
        logging.debug("cpp._call(%s)" % name)
        # dst = self.expand(name, destination = True)
        dst = self.jump_to_label(name)
        if dst != "__dispatch_call":
            dst = "k" + dst
        else:
            dst = "__disp"

        if self.far:
            self.body += "\tR(CALLF(%s));\n" % (dst)
        else:
            self.body += "\tR(CALL(%s));\n" % (dst)
        '''
                name = name.lower()
                if self.is_register(name):
                        self.body += "\t__dispatch_call(%s);\n" %self.expand(name, 2)
                        return
                if name in self.function_name_remapping:
                        self.body += "\tR(CALL(%s));\n" %self.function_name_remapping[name]
                else:
                        self.body += "\tR(CALL(%s));\n" %name
                self.schedule(name)
                '''

    def _ret(self):
        self.body += "\tR(RETN);\n"

    def _iret(self):
        self.body += "\tR(IRET);\n"

    def _retf(self):
        self.body += "\tR(RETF);\n"

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

    def _mov(self, dst, src):
        self.body += "\tR(MOV(%s, %s));\n" % self.parse2(dst, src)

    def _movsx(self, dst, src):
        self.body += "\tR(MOVSX(%s, %s));\n" % (self.expand(dst), self.expand(src))

    def _movzx(self, dst, src):
        self.body += "\tR(MOVZX(%s, %s));\n" % (self.expand(dst), self.expand(src))

    def _add(self, dst, src):
        self.body += "\tR(ADD(%s, %s));\n" % self.parse2(dst, src)

    def _sub(self, dst, src):
        self.d, self.s = self.parse2(dst, src)
        if self.d == self.s:
            self.body += "\t%s = 0;AFFECT_ZF(0); AFFECT_SF(%s,0);\n" % (self.d, self.d)
        else:
            self.body += "\tR(SUB(%s, %s));\n" % (self.d, self.s)

    def _and(self, dst, src):
        self.body += "\tR(AND(%s, %s));\n" % self.parse2(dst, src)

    def _or(self, dst, src):
        self.body += "\tR(OR(%s, %s));\n" % self.parse2(dst, src)

    def _xor(self, dst, src):
        self.d, self.s = self.parse2(dst, src)
        if self.d == self.s:
            self.body += "\t%s = 0;AFFECT_ZF(0); AFFECT_SF(%s,0);\n" % (self.d, self.d)
        else:
            self.body += "\tR(XOR(%s, %s));\n" % (self.d, self.s)

    def _neg(self, dst):
        dst = self.expand(dst)
        self.body += "\tR(NEG(%s));\n" % (dst)

    def _cbw(self):
        self.body += "\tR(CBW);\n"

    def _daa(self):
        self.body += "\tR(DAA);\n"

    def _shr(self, dst, src):
        self.body += "\tR(SHR(%s, %s));\n" % self.parse2(dst, src)

    def _shl(self, dst, src):
        self.body += "\tR(SHL(%s, %s));\n" % self.parse2(dst, src)

    def _sar(self, dst, src):
        self.body += "\tR(SAR(%s, %s));\n" % self.parse2(dst, src)

    def _sal(self, dst, src):
        self.body += "\tR(SAL(%s, %s));\n" % self.parse2(dst, src)

    def _rcl(self, dst, src):
        self.body += "\tR(RCL(%s, %s));\n" % self.parse2(dst, src)

    def _rcr(self, dst, src):
        self.body += "\tR(RCR(%s, %s));\n" % self.parse2(dst, src)

    def _bsr(self, dst, src):
        self.body += "\tR(BSR(%s, %s));\n" % self.parse2(dst, src)

    def _bsf(self, dst, src):
        self.body += "\tR(BSF(%s, %s));\n" % self.parse2(dst, src)

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
            size = self.current_size
        self.body += "\tR(MUL%d_%d(%s));\n" % (len(src), size, ",".join(res))

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
            size = self.current_size
        self.body += "\tR(IMUL%d_%d(%s));\n" % (len(src), size, ",".join(res))

    def _div(self, src):
        size = self.get_size(src)
        src = self.expand(src)
        self.body += "\tR(DIV%d(%s));\n" % (size, src)

    def _idiv(self, src):
        size = self.get_size(src)
        src = self.expand(src)
        self.body += "\tR(IDIV%d(%s));\n" % (size, src)

    def _inc(self, dst):
        dst = self.expand(dst)
        self.body += "\tR(INC(%s));\n" % (dst)

    def _dec(self, dst):
        dst = self.expand(dst)
        self.body += "\tR(DEC(%s));\n" % (dst)

    def _cmp(self, a, b):
        self.body += "\tR(CMP(%s, %s));\n" % self.parse2(a, b)

    def _test(self, a, b):
        self.body += "\tR(TEST(%s, %s));\n" % self.parse2(a, b)

    def _js(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JS(%s));\n" % label

    def _jns(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JNS(%s));\n" % label

    def _jz(self, label):
        if re.match('.*?(\$\+2)', label) is None:
            label = self.jump_to_label(label)
            self.body += "\t\tR(JZ(%s));\n" % label

    def _jnz(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JNZ(%s));\n" % label

    def _jl(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JL(%s));\n" % label

    def _jg(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JG(%s));\n" % label

    def _jle(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JLE(%s));\n" % label

    def _jge(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JGE(%s));\n" % label

    def _jbe(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JBE(%s));\n" % label

    def _ja(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JA(%s));\n" % label

    def _jc(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JC(%s));\n" % label

    def _jb(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JB(%s));\n" % label

    def _jnc(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JNC(%s));\n" % label

    def _jmp(self, label):
        if re.match('.*?(\$\+2)', label) is None:
            label = self.jump_to_label(label)
            self.body += "\t\tR(JMP(%s));\n" % label

    def _loop(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(LOOP(%s));\n" % label

    def _jcxz(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JCXZ(%s));\n" % label

    def _jecxz(self, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(JECXZ(%s));\n" % label

    def _loope(self, label):
        label = self.jump_to_label(label)
        self.body += "\tR(LOOPE(%s));\n" % label

    def _loopne(self, label):
        label = self.jump_to_label(label)
        self.body += "\tR(LOOPNE(%s));\n" % label

    def _xchg(self, dst, src):
        self.body += "\tR(XCHG(%s, %s));\n" % self.parse2(dst, src)

    def _push(self, regs):
        p = str()
        for r in regs:
            if self.get_size(r):
                r = self.expand(r)
                p += "\tR(PUSH(%s));\n" % (r)
        self.body += p

    def _pop(self, regs):
        p = str()
        for r in regs:
            self.temps_count -= 1
            i = self.temps_count
            r = self.expand(r)
            p += "\tR(POP(%s));\n" % r
        self.body += p

    def _rep(self, arg):
        self.body += "\tREP\n"

    def _cmpsb(self):
        self.body += "CMPSB;\n"

    def _lodsb(self):
        self.body += "LODSB;\n"

    def _lodsw(self):
        self.body += "LODSW;\n"

    def _lodsd(self):
        self.body += "LODSD;\n"

    def _stosb(self, n, clear_cx):
        self.body += "STOSB;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _stosw(self, n, clear_cx):
        self.body += "STOSW;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _stosd(self, n, clear_cx):
        self.body += "STOSD;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _movsb(self, n, clear_cx):
        self.body += "MOVSB;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _movsw(self, n, clear_cx):
        self.body += "MOVSW;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _movsd(self, n, clear_cx):
        self.body += "MOVSD;\n"  # %("" if n == 1 else n, ", true" if clear_cx else "")

    def _xlat(self):
        self.body += "\tR(XLAT);\n"

    def _stc(self):
        self.body += "\tR(STC);\n"

    def _scasb(self):
        self.body += "\tR(SCASB);\n"

    def _clc(self):
        self.body += "\tR(CLC);\n"

    def _cld(self):
        self.body += "\tR(CLD);\n"

    def _std(self):
        self.body += "\tR(STD);\n"

    def _cmc(self):
        self.body += "\tR(CMC);\n"

    def __proc(self, name, def_skip=0):
        logging.debug("cpp::__proc(%s)" % name)
        # traceback.print_stack(file=sys.stdout)
        try:
            skip = def_skip
            self.temps_count = 0
            self.temps_max = 0
            if self.context.has_global(name):
                self.proc = self.context.get_global(name)
            else:
                logging.debug("No procedure named %s, trying label" % name)
                off, src_proc, skip = self.context.get_offset(name)

                self.proc = proc_module.Proc(name)
                self.proc.stmts = copy(src_proc.stmts)
                self.proc.labels = copy(src_proc.labels)
                self.proc.retlabels = copy(src_proc.retlabels)
                # for p in xrange(skip, len(self.proc.stmts)):
                #       s = self.proc.stmts[p]
                #       if isinstance(s, op.basejmp):
                #               o, p, s = self.context.get_offset(s.label)
                #               if p == src_proc and s < skip:
                #                       skip = s

            self.proc_addr.append((name, self.proc.offset))
            self.body = str()
            '''
                        if name in self.function_name_remapping:
                                self.body += "void %sContext::%s() {\n" %(self.namespace, self.function_name_remapping[name]);
                        else:
                                self.body += "void %sContext::%s() {\n" %(self.namespace, name);
                        '''
            if name in self.function_name_remapping:
                self.body += "int %s() {\ngoto %s;\n" % (self.function_name_remapping[name], self.context.entry_point)
            else:
                self.body += """
int init(struct _STATE* _state)
{
X86_REGREF

_state->_indent=0;
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
""" % (name, self.context.entry_point)

            logging.info(name)
            self.proc.optimize()
            self.unbounded = []
            self.proc.visit(self, skip)
            '''
                        #adding remaining labels:
                        for i in xrange(0, len(self.unbounded)):
                                u = self.unbounded[i]
                                logging.info "UNBOUNDED: ", u
                                proc = u[1]
                                for p in xrange(u[2], len(proc.stmts)):
                                        s = proc.stmts[p]
                                        if isinstance(s, op.basejmp):
                                                self.resolve_label(s.label)

                        #adding statements
                        #BIG FIXME: this is quite ugly to handle code analysis from the code generation. rewrite me!
                        for label, proc, offset in self.unbounded:
                                self.body += "\tR(return);\n" #we need to return before calling code from the other proc
                                self.body += "/*continuing to unbounded code: %s from %s:%d-%d*/\n" %(label, proc.name, offset, len(proc.stmts))
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
            if name not in self.skip_output:
                self.translated.insert(0, self.body)
            self.proc = None
            if self.temps_count > 0:
                raise Exception("temps count == %d at the exit of proc" % self.temps_count)
            return True
        except (CrossJump, op.Unsupported) as e:
            logging.error("%s: ERROR: %s" % (name, e))
            self.failed.append(name)
        except:
            raise

    def get_type(self, width):
        return "uint%d_t" % (width * 8)

    def write_stubs(self, fname, procs):
        if sys.version_info >= (3, 0):
            fd = open(fname, "wt", encoding=self.codeset)
        else:
            fd = open(fname, "wt")

        fd.write("//namespace %s {\n" % self.namespace)
        for p in procs:
            if p in self.function_name_remapping:
                fd.write("void %sContext::%s() {\n\t::error(\"%s\");\n}\n\n" % (
                    self.namespace, self.function_name_remapping[p], self.function_name_remapping[p]))
            else:
                fd.write("void %sContext::%s() {\n\t::error(\"%s\");\n}\n\n" % (self.namespace, p, p))
        fd.write("//} // End of namespace  %s\n" % self.namespace)
        fd.close()

    def generate(self, start):
        # print self.prologue()
        # print context
        fname = self.namespace.lower() + ".cpp"
        header = self.namespace.lower() + ".h"
        banner = """/* PLEASE DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE LOST! LOOK FOR README FOR DETAILS */

/* 
 *
 */
"""
        if sys.version_info >= (3, 0):
            self.fd = open(fname, "wt", encoding=self.codeset)
            self.hd = open(header, "wt", encoding=self.codeset)
        else:
            self.fd = open(fname, "wt")
            self.hd = open(header, "wt")
        hid = "TASMRECOVER_%s_STUBS_H__" % self.namespace.upper().replace('-', '_')
        self.hd.write("""#ifndef %s
#define %s

%s""" % (hid, hid, banner))
        self.fd.write("""%s
#include \"%s\"
#include <curses.h>

//namespace %s {
""" % (banner, header, self.namespace))

        self.proc_queue.append(start)
        while len(self.proc_queue):
            name = self.proc_queue.pop()
            if name in self.failed or name in self.proc_done:
                continue
            if len(self.proc_queue) == 0 and len(self.procs) > 0:
                logging.info("queue's empty, adding remaining procs:")
                for p in self.procs:
                    self.schedule(p)
                self.procs = []
            logging.info("continuing on %s" % name)
            self.proc_done.append(name)
            self.__proc(name)
            self.methods.append(name)
        # self.write_stubs("_stubs.cpp", self.failed)
        self.methods += self.failed
        done, failed = len(self.proc_done), len(self.failed)

        self.fd.write("\n")
        self.fd.write("\n".join(self.translated))
        self.fd.write("\n")
        logging.info(
            "%d ok, %d failed of %d, %3g%% translated" % (done, failed, done + failed, 100.0 * done / (done + failed)))
        logging.info("\n".join(self.failed))
        data_bin = self.data_seg
        cdata_bin = self.cdata_seg
        hdata_bin = self.hdata_seg
        data_impl = ""
        n = 0
        comment = str()

        self.fd.write("\nreturn;\n__dispatch_call:\nswitch (__disp) {\n")
        offsets = []
        for k, v in list(self.context.get_globals().items()):
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
            % (self.namespace))

        if self.skip_addr_constants == False:
            for name, addr in self.proc_addr:
                self.hd.write("static const uint16_t addr_%s = 0x%04x;\n" % (name, addr))

        # for name,addr in self.used_data_offsets:
        #       self.hd.write("static const uint16_t offset_%s = 0x%04x;\n" %(name, addr))

        offsets = []
        for k, v in list(self.context.get_globals().items()):
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
        for k, v in list(self.context.get_globals().items()):
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
            % (self.namespace, self.namespace))
        if self.skip_dispatch_call == False:
            self.hd.write(
                """     
""")
        '''
                for p in set(self.methods):
                        if p in self.blacklist:
                                if self.header_omit_blacklisted == False:
                                        self.hd.write("\t//void %s();\n" %p)
                        else:
                                if p in self.function_name_remapping:
                                        self.hd.write("\tvoid %s();\n" %self.function_name_remapping[p])
                                else:
                                        self.hd.write("\tvoid %s();\n" %p)
                '''
        self.hd.write("//};\n\n//} // End of namespace DreamGen\n\n#endif\n")
        self.hd.close()

        # self.fd.write("void %sContext::__start() { %s\t%s(); \n}\n" %(self.namespace, data_impl, start))
        self.fd.write(" %s\n" % data_impl)

        if self.skip_dispatch_call == False:
            self.fd.write("\nvoid %sContext::__dispatch_call(uint16_t addr) {\n\tswitch(addr) {\n" % self.namespace)
            self.proc_addr.sort(cmp=lambda x, y: x[1] - y[1])
            for name, addr in self.proc_addr:
                self.fd.write("\t\tcase addr_%s: %s(); break;\n" % (name, name))
            self.fd.write("\t\tdefault: ::error(\"invalid call to %04x dispatched\", (uint16_t)ax);")
            self.fd.write("\n\t}\n}")

        self.fd.write("\n\n//} // End of namespace DreamGen\n")
        self.fd.close()

    def _lea(self, dst, src):
        self.lea = True
        self.body += "\tR(%s = %s);\n" % (dst, self.expand(src))
        self.lea = False

    def _lds(self, dst, src):
        self.body += "\tR(LDS(%s, %s));\n" % self.parse2(dst, src)

    def _lgs(self, dst, src):
        self.body += "\tR(LGS(%s, %s));\n" % self.parse2(dst, src)

    def _lfs(self, dst, src):
        self.body += "\tR(LFS(%s, %s));\n" % self.parse2(dst, src)

    def _les(self, dst, src):
        self.body += "\tR(LES(%s, %s));\n" % self.parse2(dst, src)

    def _adc(self, dst, src):
        self.body += "\tR(ADC(%s, %s));\n" % self.parse2(dst, src)

    def _setnz(self, dst):
        dst = self.expand(dst)
        self.body += "\tR(SETNZ(%s))\n" % (dst)

    def _setz(self, dst):
        dst = self.expand(dst)
        self.body += "\tR(SETZ(%s))\n" % (dst)

    def _setb(self, dst):
        dst = self.expand(dst)
        self.body += "\tR(SETB(%s))\n" % (dst)

    def _sbb(self, dst, src):
        self.body += "\tR(SBB(%s, %s));\n" % self.parse2(dst, src)

    def _movs(self, dst, src):
        size = self.get_size(dst)
        a, b = self.parse2(dst, src)
        self.body += "MOVS(%s, %s, %d);\n" % (a, b, size)

    def _bt(self, dst, src):
        self.body += "\tR(BT(%s, %s));\n" % self.parse2(dst, src)

    def _btc(self, dst, src):
        self.body += "\tR(BTC(%s, %s));\n" % self.parse2(dst, src)

    def _btr(self, dst, src):
        self.body += "\tR(BTR(%s, %s));\n" % self.parse2(dst, src)

    def _bts(self, dst, src):
        self.a, self.b = self.parse2(dst, src)
        self.body += "\tR(BTS(%s, %s));\n" % (self.a, self.b)

    def _ror(self, dst, src):
        self.body += "\tR(ROR(%s, %s));\n" % self.parse2(dst, src)

    def _rol(self, dst, src):
        self.body += "\tR(ROL(%s, %s));\n" % self.parse2(dst, src)

    def _repe(self, arg):
        self.body += "\tREPE\n"

    def _repne(self, arg):
        self.body += "\tREPNE\n"

    def _shrd(self, dst, src, c):
        self.body += "\tR(SHRD(%s, %s, " % self.parse2(dst, src)
        self.body += "%s));\n" % self.expand(c)

    def _shld(self, dst, src, c):
        self.body += "\tR(SHLD(%s, %s, " % self.parse2(dst, src)
        self.body += "%s));\n" % self.expand(c)

    def _pushf(self):
        self.body += "\tR(PUSHF);\n"

    def _popf(self):
        self.body += "\tR(POPF);\n"

    def _pushad(self):
        self.body += "\tR(PUSHAD);\n"

    def _pusha(self):
        self.body += "\tR(PUSHA);\n"

    def _popad(self):
        self.body += "\tR(POPAD);\n"

    def _popa(self):
        self.body += "\tR(POPA);\n"

    def _aad(self):
        self.body += "\tR(AAD);\n"

    def _cwde(self):
        self.body += "\tR(CWDE);\n"

    def _cwd(self):
        self.body += "\tR(CWD);\n"

    def _lods(self, src):
        size = self.get_size(src)
        src = self.expand(src)
        self.body += "\tR(LODS(%s,%d));\n" % (src, size)

    def _cli(self):
        self.body += "\tR(CLI);\n"

    def _sti(self):
        self.body += "\tR(STI);\n"

    def _leave(self):
        self.body += "\tR(MOV(esp, ebp));\nR(POP(ebp));\n"

    def _in(self, dst, src):
        self.body += "\tR(IN(%s, %s));\n" % self.parse2(dst, src)

    def _out(self, dst, src):
        self.body += "\tR(OUT(%s, %s));\n" % self.parse2(dst, src)

    def _int(self, dst):
        dst = self.expand(dst)
        self.body += "\tR(_INT(%s));\n" % (dst)

    def _not(self, dst):
        dst = self.expand(dst)
        self.body += "\tR(NOT(%s));\n" % (dst)

    def _instruction0(self, cmd):
        self.body += "\tR(%s);\n" % (cmd.upper())

    def _instruction1(self, cmd, dst):
        dst = self.expand(dst)
        self.body += "\tR(%s(%s));\n" % (cmd.upper(), dst)

    def _jump(self, cmd, label):
        label = self.jump_to_label(label)
        self.body += "\t\tR(%s(%s));\n" % (cmd.upper(), label)

    def _instruction2(self, cmd, dst, src):
        self.a, self.b = self.parse2(dst, src)
        self.body += "\tR(%s(%s, %s));\n" % (cmd.upper(), self.a, self.b)

    def _instruction3(self, cmd, dst, src, c):
        self.a, self.b = self.parse2(dst, src)
        self.c = self.expand(c)
        self.body += "\tR(%s(%s, %s, %s));\n" % (cmd.upper(), self.a, self.b, self.c)

    def _assignment(self, dst, src):
        self.body += "#undef %s\n#define %s %s\n" % (dst.upper(), dst.upper(), self.expand(src))

    def _equ(self, dst, src):
        self.body += "#define %s %s\n" % (dst.upper(), src)
