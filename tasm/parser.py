from __future__ import absolute_import
from __future__ import print_function

import logging
import os
import re
import sys
from builtins import chr
from builtins import hex
from builtins import object
from builtins import range
from builtins import str

import tasm.cpp
import tasm.lex
import tasm.proc
from tasm import op


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

# from tasm.lex import parse_line_data, parse_line_name_data


class Parser(object):
    def __init__(self, skip_binary_data=[]):
        self.skip_binary_data = skip_binary_data
        self.strip_path = 0
        self.__globals = {}
        self.__offsets = {}
        self.offset_id = 0x1111
        self.__stack = []
        self.proc_list = []

        self.entry_point = ""

        # self.proc = None
        nname = "mainproc"
        self.proc = tasm.proc.Proc(nname)
        self.proc_list.append(nname)
        self.set_global(nname, self.proc)

        self.binary_data_size = 0
        self.c_data = []
        self.h_data = []
        self.cur_seg_offset = 0
        self.dummy_enum = 0
        self.segment = "default_seg"

        self.symbols = []
        self.link_later = []
        #self.data_started = False
        #self.prev_data_type = 0
        #self.prev_data_ctype = 0
        self.line_number = 0
        self.lex = tasm.lex.Lex()

    def visible(self):
        for i in self.__stack:
            if not i or i == 0:
                return False
        return True

    def push_if(self, text):
        value = self.eval(text)
        # print "if %s -> %s" %(text, value)
        self.__stack.append(value)

    def push_else(self):
        # print "else"
        self.__stack[-1] = not self.__stack[-1]

    def pop_if(self):
        # print "endif"
        return self.__stack.pop()

    def set_global(self, name, value):
        if len(name) == 0:
            raise Exception("empty name is not allowed")
        value.original_name = name
        name = name.lower()

        stuff = self.dump_object(value)
        logging.debug("set_global(name='%s',value=%s)" % (name, stuff))
        if name in self.__globals:
            raise Exception("global %s was already defined", name)
        self.__globals[name] = value

    def dump_object(self, value):
        stuff = str(value.__dict__)
        replacements = [
            (r'\n', ' '),
            (r'[{}]', ''),
            (r"'([A-Za-z_0-9]+)'\s*:\s*", '\g<1>=')
        ]
        for old, new in replacements:
            stuff = re.sub(old, new, stuff)
        stuff = value.__class__.__name__ + "(" + stuff + ")"
        return stuff

    '''
    def reset_global(self, name, value):
            if len(name) == 0:
                    raise Exception("empty name is not allowed")
            name = name.lower()
            logging.debug("adding global %s -> %s" %(name, value))
            self.__globals[name] = value
    '''

    def get_global(self, name):
        name = name.lower()
        logging.debug("get_global(%s)" % name)
        try:
            g = self.__globals[name]
            logging.debug(g)
        except KeyError:
            logging.debug("get_global KeyError %s" % (name))
            raise KeyError
        g.used = True
        return g

    def get_globals(self):
        return self.__globals

    def has_global(self, name):
        name = name.lower()
        return name in self.__globals

    def set_offset(self, name, value):
        if len(name) == 0:
            raise Exception("empty name is not allowed")
        name = name.lower()
        logging.debug("adding offset %s -> %s" % (name, value))
        if name in self.__offsets:
            raise Exception("offset %s was already defined", name)
        self.__offsets[name] = value

    def get_offset(self, name):
        name = name.lower()
        return self.__offsets[name]

    def include(self, basedir, fname):
        logging.info("file: %s" % (fname))
        # path = fname.split('\\')[self.strip_path:]
        path = fname
        # path = os.path.join(basedir, os.path.pathsep.join(path))
        logging.info("including %s" % (path))

        self.parse(path)

    def eval(self, stmt):
        try:
            return self.parse_int(stmt)
        except:
            pass
        value = self.__globals[stmt.lower()].value
        return int(value)

    def expr_callback(self, match):
        name = match.group(1).lower()
        g = self.get_global(name)
        if isinstance(g, op.equ) or isinstance(g, op.assignment):
            return g.value
        else:
            return "0x%04x" % g.offset

    def eval_expr(self, expr):
        n = 1
        while n > 0:
            expr, n = re.subn(r'\b([a-zA-Z_]+[a-zA-Z0-9_]*)', self.expr_callback, expr)
        # print "~%s~" %(expr)
        expr = expr.strip()
        # exprr = expr.lower()
        expr = cpp.convert_number_to_c(expr)
        # if exprr[-1] == 'h':
        #        logging.debug("eval_expr: %s" %(expr))
        #        expr = '0x'.expr[0:len(expr)-1]
        logging.debug("eval_expr: %s" % (expr))

        if expr == '?':
            return 0
        try:
            return eval(expr)
        except SyntaxError:
            logging.debug("eval_expr SyntaxError ~%s~" % (expr))
            return 0

    def expand_globals(self, text):
        return text

    def fix_dollar(self, v):
        logging.debug("$ = %d" % self.cur_seg_offset)
        return re.sub(r'\$', "%d" % self.cur_seg_offset, v)

    def parse_int(self, v):
        # print "~1~ %s" %v
        if isinstance(v, list):
            vv = ""
            for i in v:
                try:
                    i = self.parse_int(i)
                except:
                    pass
                vv += str(i)
            v = vv
        v = v.strip()
        # print "~2~ %s" %v
        if re.match(r'^[+-]?[0-9][0-9A-Fa-f]*[Hh]$', v):
            v = int(v[:-1], 16)
            # print "~3~ %i" %v
        elif re.match(r'^[01]+[Bb]$', v):
            v = int(v[:-1], 2)
            # print "~2~ %i" %v

        try:
            vv = eval(v)
            v = vv
        except:
            pass

        # print "~4~ %s" %v
        return int(v)

    def calculate_data_binary_size(self, width, data):
        logging.debug("calculate_data_binary_size %d %s" %(width, data))
        s = 0
        for v in data:
            #v = v.strip()
            if isinstance(v, str) and width == 1 and len(v) >= 2 and (v[0] in ["'", '"']) and v[-1] == v[0]:
                v.replace("''", "'")
                s += len(v) - 2
                continue

            if isinstance(v,list) and len(v) == 5 and v[1].lower() == 'dup':
                #logging.error(v)
                # we should parse that
                n = self.parse_int(v[0])
                '''
                value = m.group(2)
                value = value.strip()
                if value == '?':
                    value = 0
                else:
                    value = self.parse_int(value)
                '''
                s += n * width
                continue
            '''
            try:
                v = self.parse_int(v)
            except:
                try:
                    g = self.get_global(v)
                    v = g.offset
                except:
                    v = 0
            '''
            s += width
        return s

    def get_global_value(self, v, base):
        logging.info("get_global_value(%s)" % v)
        g = self.get_global(v)
        logging.debug(g)
        if isinstance(g, op._equ) or isinstance(g, op._assignment):
            v = g.original_name
        elif isinstance(g, op.var):
            v = "offset(%s,%s)" % (g.segment, g.name)
        elif isinstance(g, op.label):
            v = "k%s" % (g.name.lower())
        else:
            v = g.offset
        logging.debug(v)
        return v

    def convert_data_to_c(self, label, width, data):
        """ Generate C formated data """
        logging.debug("convert_data_to_c %s %d %s" % (label, width, data))
        is_string = False
        elements = 0
        data_ctype = {1: 'db', 2: 'dw', 4: 'dd', 8: 'dq', 10: 'dt'}[width]
        r = [""]
        rh = []
        base = 1 << (8 * width)

        for v in data:
            #v = v.strip()
            # check if there are strings
            if isinstance(v, str) and width == 1 and len(v) >= 2 and (v[0] in ["'", '"']) and v[-1] == v[0]:
                v.replace("''", "'")
                res = []
                for i in range(1, len(v) - 1):
                    res.append(v[i])
                r += res

                is_string = True
                continue

            logging.debug("is_string %d" % is_string)
            logging.debug("v ~%s~" % v)
            '''
            if is_string and v.isdigit():
                    v = "'\\" +str(hex(int(v)))[1:] + "'"
            '''

            #check if dup
            if isinstance(v,list) and len(v) == 5 and v[1].lower() == 'dup':
                # we should parse that
                group1 = v[0]
                if isinstance(group1,list):
                    group1="".join(group1)
                values = v[3]
                #value = value.strip()
                n, res = self.action_dup(group1, values)
                r += res
                elements += n
                continue

            elements += 1
            if isinstance(v, int) or isinstance(v, str):
                try: # just number or many
                    v = v.strip()
                    if v == '?':
                        v = '0'
                    v = self.parse_int(v)

                    if v < 0:  # negative values
                        v += base

                except:
                    # global name
                    # traceback.print_stack(file=sys.stdout)
                    # print "global/expr: ~%s~" %v
                    try:
                        v = self.get_global_value(v, base)
                    except KeyError:
                        v = 0

            #vv = v.split()
            #logging.warning(vv)
            #if vv[0] == "offset":  # pointer
            #    data_ctype = "dw" # TODO for 16 bit only
            #    v = vv[1]

            logging.debug("global/expr: ~%s~" % v)
            if isinstance(v, list) and len(v) == 2 and v[0].lower() in ['offset', 'seg']:
                try:
                    v = v[1]
                    data_ctype = "dw"  # TODO for 16 bit only
                    v = re.sub(r'@', "arb", v)
                    v = self.get_global_value(v, base)
                except KeyError:
                    logging.warning("unknown address %s" % (v))
                    logging.warning(self.c_data)
                    logging.warning(r)
                    logging.warning(len(self.c_data) + len(r))
                    self.link_later.append((len(self.c_data) + len(r), v))
                    v = 0


            r.append(v)


        #cur_data_type = 0
        if is_string:
            if len(r) >= 2 and r[-1] == 0:
                cur_data_type = 1  # 0 terminated string
            else:
                cur_data_type = 2  # array string

        else:
            cur_data_type = 3  # number
            if elements > 1:
                cur_data_type = 4  # array of numbers

        logging.debug("current data type = %d current data c type = %s" % (cur_data_type, data_ctype))

        if len(label) == 0:
            self.dummy_enum += 1
            label = "dummy" + str(self.dummy_enum)

        vh = ""
        vc = ""

        if cur_data_type == 1:  # 0 terminated string
            vh = "char " + label + "[" + str(len(r) - 1) + "]"

        elif cur_data_type == 2:  # array string
            vh = "char " + label + "[" + str(len(r) - 1) + "]"
            vc = "{"

        elif cur_data_type == 3:  # number
            vh = data_ctype + " " + label

        elif cur_data_type == 4:  # array
            vh = data_ctype + " " + label + "[" + str(elements) + "]"
            vc = "{"

        if cur_data_type == 1:  # string
            vv = "\""
            for i in range(1, len(r) - 1):
                if isinstance(r[i], int):
                    if r[i] == 13:
                        vv += r"\r"
                    elif r[i] == 10:
                        vv += r"\n"
                    elif r[i] < 10:
                        vv += "\\{:01x}".format(r[i])
                    elif r[i] < 32:
                        vv += "\\x{:02x}".format(r[i])
                    else:
                        vv += chr(r[i])
                else:
                    vv += r[i]
            vv += "\""
            r = [""]
            r.append(vv)

        elif cur_data_type == 2:  # array of char
            vv = ""
            logging.debug(r)
            for i in range(1, len(r)):
                if isinstance(r[i], int):
                    if r[i] == 13:
                        vv += r"'\r'"
                    elif r[i] == 10:
                        vv += r"'\n'"
                    else:
                        vv += str(r[i])
                elif isinstance(r[i], str):
                    # print "~~ " + r[i] + str(ord(r[i]))
                    if r[i] in ["\'", '\"', '\\']:
                        r[i] = "\\" + r[i]
                    elif ord(r[i]) > 127:
                        r[i] = hex(ord(r[i].encode('cp437', 'backslashreplace')))
                        r[i] = '\\' + r[i][1:]
                    elif r[i] == '\0':
                        r[i] = '\\0'
                    vv += "'" + r[i] + "'"
                if i != len(r) - 1:
                    vv += ","
            r = [""]
            r.append(vv)

        elif cur_data_type == 3:  # number
            r[1] = str(r[1])
            # r = []
            # r.append(vv)

        elif cur_data_type == 4:  # array of numbers
            # vv = ""
            for i in range(1, len(r)):
                r[i] = str(r[i])
                if i != len(r) - 1:
                    r[i] += ","
            # r = []
            # r.append(vv)

        r[0] = vc
        rh.insert(0, vh)
        # if it was array of numbers or array string
        if cur_data_type == 4 or cur_data_type == 2:
            r.append("}")

        r.append(", // " + label + "\n")
        rh.append(";\n")

        logging.debug(r)
        logging.debug(rh)
        logging.debug("returning")

        r = "".join(r)
        rh = "".join(rh)
        return r, rh, elements

    def action_dup(self, group1, values):
        n = self.parse_int(group1)
        res = []
        for i in range(0, n):
            value = values[i % len(values)]
            if value == '?':
                value = 0
            else:
                if isinstance(value, list):
                    val=""
                    for v in value:
                        try:
                            v = self.parse_int(v)
                        except ValueError:
                            pass
                        val += str(v)
                    value = val
                else:
                    value = self.parse_int(value)
            # print "n = %d value = %d" %(n, value)

            res.append(value)
        return n, res

    def action_label_(self, line, far="", isproc=False):
        #logging.info(line)
        m = re.match('([@\w]+)\s*::?', line)
        name = m.group(1).strip()
        logging.debug(name)
        self.action_label(name, far=far, isproc=isproc)

    def action_label(self, name, far="", isproc=False):
            #if self.visible():
            # name = m.group(1)
            # print "~name: %s" %name
            name = re.sub(r'@', "arb", name)
            # print "~~name: %s" %name
            if not (name.lower() in self.skip_binary_data):
                logging.debug("offset %s -> %s" % (name, "&m." + name.lower() + " - &m." + self.segment))
                '''
                if self.proc is None:
                        nname = "mainproc"
                        self.proc = proc(nname)
                        #print "procedure %s, #%d" %(name, len(self.proc_list))
                        self.proc_list.append(nname)
                        self.set_global(nname, self.proc)
                '''
                if self.proc is not None:
                    self.proc.add_label(name, isproc)
                    # self.set_offset(name, ("&m." + name.lower() + " - &m." + self.segment, self.proc, len(self.proc.stmts)))
                    self.set_offset(name, ("&m." + name.lower() + " - &m." + self.segment, self.proc, self.offset_id))
                    farb = False
                    if far == 'far':
                        farb = True
                    self.set_global(name, op.label(name, tasm.proc.Proc, line_number=self.offset_id, far=farb))
                    self.offset_id += 1
                else:
                    logging.error("!!! Label %s is outside the procedure" % name)
            else:
                logging.info("skipping binary data for %s" % (name,))

    def create_segment(self, name):
        binary_width = 1
        offset = self.binary_data_size // 16
        logging.debug("segment %s %x" % (name, offset))
        self.cur_seg_offset = 16

        num = (0x10 - (self.binary_data_size & 0xf)) & 0xf
        if num:
            l = ['0'] * num
            self.binary_data_size += num

            self.dummy_enum += 1
            labell = "dummy" + str(self.dummy_enum)

            self.c_data.append("{" + ",".join(l) + "}, // padding\n")
            self.h_data.append(" db " + labell + "[" + str(num) + "]; // padding\n")

        num = 0x10
        l = ['0'] * num
        self.binary_data_size += num

        self.c_data.append("{" + ",".join(l) + "}, // segment " + name + "\n")
        self.h_data.append(" db " + name + "[" + str(num) + "]; // segment " + name + "\n")

        self.set_global(name, op.var(binary_width, offset, name, issegment=True))
        '''
        if self.proc == None:
                name = "mainproc"
                self.proc = proc(name)
                #print "procedure %s, #%d" %(name, len(self.proc_list))
                self.proc_list.append(name)
                self.set_global(name, self.proc)
        '''

    def parse(self, fname):
        logging.info("opening file %s..." % (fname))
        if sys.version_info >= (3, 0):
            fd = open(fname, 'rt', encoding="cp437")
        else:
            fd = open(fname, 'rt')

        self.parse_(fd, fname)

        fd.close()

        return self

    def parse_(self, fd, fname):
        self.line_number = 0
        skipping_binary_data = False
        num = 0x1000
        if num:
            self.binary_data_size += num

            self.dummy_enum += 1
            labell = "dummy" + str(self.dummy_enum)

            self.c_data.append("{0}, // padding\n")
            self.h_data.append(" db " + labell + "[" + str(num) + "]; // protective\n")
        self.parse_file_lines(fd, fname, skipping_binary_data)

        num = (0x10 - (self.binary_data_size & 0xf)) & 0xf
        if num:
            l = num * ['0']
            self.binary_data_size += num

            self.dummy_enum += 1
            labell = "dummy" + str(self.dummy_enum)

            self.c_data.append("{" + ",".join(l) + "}, // padding\n")
            self.h_data.append(" db " + labell + "[" + str(num) + "]; // padding\n")

    def parse_file_lines(self, fd, fname, skipping_binary_data):
        for line in fd:
            self.line_number += 1
            # line = line.decode("cp1251")
            line = line.strip()
            if len(line) == 0 or line[0] == ';' or line[0] == chr(0x1a):
                continue

            logging.debug("%d:      %s" % (self.line_number, line))

            m = re.match('([@\w]+)\s*::?\s*(.*)', line)
            if m is not None:
                self.action_label_(line, isproc=False)
                line = m.group(2).strip()
                if len(line) != 0:
                    logging.debug("Line without label %s",line)

            cmd = line.split()
            if len(cmd) == 0:
                continue

            cmd0 = str(cmd[0])
            if cmd0 == ';':
                continue

            cmd0l = cmd0.lower()

            m = re.match('(\.\d86[prc]?)', line)
            if m is not None:
                line = m.group(1).strip()
                logging.debug(line)
                continue

            m = re.match('(\.model)', line)
            if m is not None:
                line = m.group(1).strip()
                logging.debug(line)
                continue
            elif cmd0l == 'align':
                continue
            elif cmd0l == 'assume':
                logging.info("skipping: %s" % line)
                continue

            if cmd0l == 'if':
                self.action_if(line)
                continue
            elif cmd0l == 'else':
                self.action_else()
                continue
            elif cmd0l == 'endif':
                self.action_endif()
                continue

            #if not self.visible():
            #    continue

            if cmd0l in ['db', 'dw', 'dd', 'dq']:
                self.action_data(line)
                continue
            elif cmd0l == 'include':
                self.action_include(line)
                continue
            elif cmd0l == 'endp' or (len(cmd) >= 2 and str(cmd[1].lower()) == 'endp'):
                self.action_endp()
                continue
            elif cmd0l == 'ends':
                self.action_endseg()
                continue
            elif cmd0l in ['rep', 'repe', 'repne', 'repz', 'repnz']:
                self.action_prefix(line)
                continue
            elif cmd0l == 'end':
                self.action_end(line)
                continue
            elif cmd0l == '.code' or cmd0l == '.data' or cmd0l == '.stack':
                self.action_simplesegment(line)
                continue
            if len(cmd) >= 2:
                cmd1l = cmd[1].lower()
                if cmd1l == 'proc':
                    self.action_proc(line)
                    continue
                elif cmd1l == 'segment':
                    self.action_segment(line)
                    continue
                elif cmd1l == 'ends':
                    self.action_endseg()
                    continue

            if len(cmd) >= 3:
                cmd1l = cmd[1].lower()
                if cmd1l in ['equ', '=']:
                    if cmd1l == 'equ':
                        self.action_equ(line)
                    elif cmd1l == '=':
                        self.action_assign(line)
                    continue
                elif cmd1l in ['db', 'dw', 'dd', 'dq', 'dt']:
                        self.action_data(line)
                        continue
            if (self.proc):
                self.proc.action_instruction(line, line_number=self.line_number)
            else:
                # print line
                pass


    def action_assign(self, line):
            cmd = line.split()
            cmd0 = str(cmd[0])
            vv = self.get_equ_value(cmd)
            name = cmd0

            has_global = False
            if self.has_global(name):
                has_global = True
                name = self.get_global(name).original_name
            o = self.proc.add_assignment(name, vv, line_number=self.line_number)
            if has_global == False:
                self.set_global(cmd0, o)
            self.proc.stmts.append(o)
            return o


    def action_equ(self, line):
            cmd = line.split()
            cmd0 = str(cmd[0])
            vv = self.get_equ_value(cmd)
            name = cmd0
            proc = self.get_global("mainproc")
            o = proc.add_equ_(name, vv, line_number=self.line_number)
            self.set_global(name, o)
            proc.stmts.insert(0, o)
            return o

    def action_segment(self, line):
            cmd = line.split()
            cmd0 = str(cmd[0])
            cmd0l = cmd0.lower()
            name = cmd0l
            self.segment = name
            self.create_segment(name)


    def action_proc(self, line):
            cmd = line.split()
            cmd0 = str(cmd[0])
            cmd0l = cmd0.lower()
            cmd2l = ""
            if len(cmd) >= 3:
                cmd2l = cmd[2].lower()
            logging.info("procedure name %s" % cmd0l)
            '''
                                            name = cmd0l
                                            self.proc = proc(name)
                                            logging.debug "procedure %s, #%d" %(name, len(self.proc_list))
                                            self.proc_list.append(name)
                                            self.set_global(name, self.proc)
                                            '''
            self.action_label_(cmd0+":", far=cmd2l, isproc=True)


    def action_simplesegment(self, line):
            line = line.lower()
            self.action_segment(line[1:])


    def action_end(self, line):
            cmd = line.split()
            if len(cmd) >= 2:
                self.entry_point = cmd[1].lower()


    def action_prefix(self, line):
            cmd = line.split()
            cmd0 = str(cmd[0])
            cmd0l = cmd0.lower()
            self.proc.action_instruction(cmd0l)
            self.proc.action_instruction(" ".join(cmd[1:]))


    def action_endseg(self):
            logging.debug("segment %s ends" % (self.segment))
            self.segment = "default_seg"


    def action_include(self, line):
            cmd = line.split()
            self.include(os.path.dirname(fname), cmd[1])


    def action_endp(self):
            self.proc = self.get_global('mainproc')


    def action_endif(self):
            self.pop_if()


    def action_else(self):
            self.push_else()


    def action_if(self, line):
            cmd = line.split()
            self.push_if(cmd[1])


    def action_data(self, line):
            name, type, args = self.lex.parse_line_data_new(line)

            offset = self.cur_seg_offset
            logging.debug("data value %s offset %d" % (str(args), offset))

            binary_width = self.calculate_type_size(type)
            s = self.calculate_data_binary_size(binary_width, args)
            self.binary_data_size += s
            self.cur_seg_offset += s

            c, h, elements = self.convert_data_to_c(name, binary_width,
                                                       args)
            self.c_data += c
            self.h_data += h

            logging.debug("~size %d elements %d" % (binary_width, elements))
            if name:
                self.set_global(name, op.var(binary_width, offset, name=name,
                                             segment=self.segment, elements=elements))
            # print("~~        self.assertEqual(parser_instance.parse_data_line_whole(line='"+str(line)+"'),"+str(("".join(c), "".join(h), offset2 - offset))+")")
            return c, h, s

    def calculate_type_size(self, type):
            binary_width = {'b': 1, 'w': 2, 'd': 4, 'q': 8, 't': 10}[type[1].lower()]
            return binary_width


    def get_equ_value(self, cmd):
            v = " ".join(cmd[2:])
            logging.debug("%s" % v)
            vv = self.fix_dollar(v)
            vv = " ".join(self.lex.parse_args(vv))
            vv = vv.strip()
            logging.debug("%s" % vv)
            m = re.match(r'\bbyte\s+ptr\s+(.*)', vv)
            if m is not None:
                vv = m.group(1).strip()
            m = re.match(r'\bdword\s+ptr\s+(.*)', vv)
            if m is not None:
                vv = m.group(1).strip()
            m = re.match(r'\bqword\s+ptr\s+(.*)', vv)
            if m is not None:
                vv = m.group(1).strip()
            m = re.match(r'\btword\s+ptr\s+(.*)', vv)
            if m is not None:
                vv = m.group(1).strip()
            m = re.match(r'\bword\s+ptr\s+(.*)', vv)
            if m is not None:
                vv = m.group(1).strip()
            vv = tasm.cpp.convert_number_to_c(vv)
            return vv


    def link(self):
            logging.debug("link()")
            # print self.c_data
            for addr, expr in self.link_later:
                logging.debug("addr %s expr %s" % (addr, expr))
                try:
                    # v = self.eval_expr(expr)
                    v = expr
                    # if self.has_global('k' + v):
                    #               v = 'k' + v
                    v = self.get_global_value(v, 0x10000)

                    logging.debug("link: patching %04x -> %s" % (addr, v))
                except:
                    logging.warning("link: Exception %s" % expr)
                    continue
                logging.debug("link: addr %s v %s" % (addr, v))
                self.c_data[addr] = str(v)
            # print self.c_data
