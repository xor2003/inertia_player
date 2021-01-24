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

from parglare import Grammar, Parser as PGParser

import tasm.cpp
import tasm.proc
from tasm import op
from tasm.Token import Token


# from parglare.parser import Context as PGContext
# action = get_collector()
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


def escape(s):
    if isinstance(s, list):
        return [escape(i) for i in s]
    elif isinstance(s, Token):
        return escape(s.value)
    else:
        return s.translate(s.maketrans({"\\": r"\\"})).replace("''", "'").replace('""', '"')

def read_asm_file(file_name):
        logging.info("opening file %s..." % file_name)
        if sys.version_info >= (3, 0):
            fd = open(file_name, 'rt', encoding="cp437")
        else:
            fd = open(file_name, 'rt')
        content = fd.read()
        fd.close()
        return content


macroids = []
macroidre = re.compile(r'([A-Za-z@_\$\?][A-Za-z0-9@_\$\?]*)')


def get_line_number(context):
    #old_line, old_col = parglare.pos_to_line_col(context.input_str, context.start_position)
    new_line = context.input_str[: context.start_position].count('\n') + 1
    #line_start_pos = context.input_str.rfind('\n', 0, context.start_position)
    #if line_start_pos == -1:
    #    line_start_pos = 0
    #new_col = context.start_position - line_start_pos - 1
    #assert(old_line == new_line and old_col == new_col)
    return  new_line


def get_raw(context):
    return context.input_str[context.start_position: context.end_position]


'''
def build_ast(nodes, type=''):
    if isinstance(nodes, parglare.parser.NodeNonTerm) and nodes.children:
        if len(nodes.children) == 1:
            return build_ast(nodes.children[0], nodes.symbol.name)
        else:
            l = []
            for n in nodes.children:
                if not n.symbol.name in ['COMMA']:
                    l.append(build_ast(n, nodes.symbol.name))
            return l
    else:
        return Node(name=nodes.symbol.name, type=type, keyword=nodes.symbol.keyword, value=nodes.value)
'''

'''
def make_token(context, nodes):
    if len(nodes) == 1 and context.production.rhs[0].name not in ('type'):
        nodes = Token(context.production.rhs[0].name, nodes[0])
    if context.production.rhs[0].name in ('type'):
        nodes = nodes[0]
    return nodes
'''


def make_token(context, nodes):
    if len(nodes) == 1 and context.production.rhs[0].name not in (
    'type', 'e01', 'e02', 'e03', 'e04', 'e05', 'e06', 'e07', 'e08', 'e09', 'e10', 'e11'):
        nodes = Token(context.production.rhs[0].name, nodes[0])
    if context.production.rhs[0].name in (
    'type', 'e01', 'e02', 'e03', 'e04', 'e05', 'e06', 'e07', 'e08', 'e09', 'e10', 'e11'):
        nodes = nodes[0]
    return nodes


def segoverride(context, nodes):
    # global cur_segment
    if isinstance(nodes[0], list):
        cur_segment = nodes[0][-1]
        return nodes[0][:-1] + [Token('segoverride', nodes[0][-1]), nodes[2]]
    # cur_segment = nodes[0] #!
    return [Token('segoverride', nodes[0]), nodes[2]]


def ptrdir(context, nodes):
    if len(nodes) == 3:
        return [Token('ptrdir', nodes[0]), nodes[2]]
    else:
        return [Token('ptrdir', nodes[0]), nodes[1]]


def INTEGER(context, nodes):
    return Token('INTEGER', tasm.cpp.convert_number_to_c(nodes))


def expr(context, nodes):
    return make_token(context, nodes)


def macroid(head, s, pos):
    mtch = macroidre.match(s[pos:])
    if mtch:
        result = mtch.group().lower()
        # logging.debug ("matched ~^~" + result+"~^~")
        if result in macroids:
            logging.debug(" ~^~" + result + "~^~ in macroids")
            return result
        else:
            return None
    else:
        return None


def macrodir(_, nodes, name):
    macroids.insert(0, name.lower())
    logging.debug("macroid added ~~" + name + "~~")


def datadir(context, nodes, label, type, values):
    if label:
        label = label.value
    else:
        label = ""
    label = re.sub(r'@', "arb", label)

    # if isinstance(values, list) and len(values) == 1:
    #    values = values[0]

    if isinstance(values, list):
        s = []
        for x in values:
            # if isinstance(x, list):
            #    values = [i for i in values]
            if isinstance(x, Token):
                s = s + [x.value]
            else:  # if isinstance(x, str):
                s = s + [x]
        values = s
    elif isinstance(values, Token):
        values = values.value

    values = escape(values)
    # if isinstance(values, str):
    #    values = [values]

    logging.debug("datadir " + str(nodes) + " ~~")
    return context.extra.datadir_action(label.lower(), type, values)

def includedir(context, nodes, name):
    #context.parser.input_str = context.input_str[:context.end_position] + '\n' + read_asm_file(name) \
    #+ '\n' + context.input_str[context.end_position:]
    result = context.extra.parse_include_file_lines(name)
    return result

def segdir(context, nodes, type, name):
    logging.debug("segdir " + str(nodes) + " ~~")
    context.extra.action_simplesegment(type, name)
    return nodes


def segmentdir(context, nodes, name):
    logging.debug("segmentdir " + str(nodes) + " ~~")
    context.extra.action_segment(name.value)
    return nodes


def endsdir(context, nodes, name):
    logging.debug("ends " + str(nodes) + " ~~")
    context.extra.action_endseg()
    return nodes


def procdir(context, nodes, name, type):
    logging.debug("procdir " + str(nodes) + " ~~")
    context.extra.action_proc(name, type)
    return nodes


def endpdir(context, nodes, name):
    logging.debug("endp " + str(name) + " ~~")
    context.extra.action_endp()
    return nodes


def equdir(context, nodes, name, value):
    logging.debug("equdir " + str(nodes) + " ~~")
    return context.extra.action_equ(name, value)


def assdir(context, nodes, name, value):
    logging.debug("assdir " + str(nodes) + " ~~")
    return context.extra.action_assign(name, value)


def labeldef(context, nodes, name):
    logging.debug("labeldef " + str(nodes) + " ~~")
    return context.extra.action_label(name.value)


def instrprefix(context, nodes):
    logging.debug("instrprefix " + str(nodes) + " ~~")
    instruction = nodes[0]
    o = context.extra.proc.create_instruction_object(instruction)
    o.line = get_raw(context)
    o.line_number = get_line_number(context)
    context.extra.proc.stmts.append(o)
    return []  # nodes

def asminstruction(context, nodes, instruction, args):
    logging.debug("instruction " + str(nodes) + " ~~")
    # if args:
    #    args = [listtostring(i) for i in args] # TODO temporary workaround
    # args = build_ast(args)
    if not instruction:
        return nodes
    if args is None:
        args = []
    o = context.extra.proc.create_instruction_object(instruction, args)
    o.line = get_raw(context)
    o.line_number = get_line_number(context)
    context.extra.proc.stmts.append(o)
    return o


def enddir(context, nodes, label):
    logging.debug("end " + str(nodes) + " ~~")
    if label:
        context.extra.entry_point = label.value.lower()
    return nodes


def notdir(context, nodes):
    nodes[0] = '~'
    return nodes


def ordir(context, nodes):
    nodes[1] = '|'
    return nodes


def xordir(context, nodes):
    nodes[1] = '^'
    return nodes


def anddir(context, nodes):
    nodes[1] = ' & '
    return nodes


def register(context, nodes):
    return Token('register', nodes[0].lower())


def segmentregister(context, nodes):
    return Token('segmentregister', nodes[0].lower())


def sqexpr(context, nodes):
    logging.debug("/~"+str(nodes)+"~\\")
    res = nodes[1]
    return Token('sqexpr', res)


def offsetdir(context, nodes):
    logging.debug("offset /~"+str(nodes)+"~\\")
    return Token('offsetdir', nodes[1])


def segmdir(context, nodes):
    logging.debug("offset /~"+str(nodes)+"~\\")
    # global indirection
    # indirection = -1
    return Token('segmdir', nodes[1])


def LABEL(context, nodes):
    return Token('LABEL', nodes)


def STRING(context, nodes):
    return Token('STRING', nodes)


actions = {
    "includedir": includedir,
    "instrprefix": instrprefix,
    "INTEGER": INTEGER,
    "LABEL": LABEL,
    "STRING": STRING,
    "anddir": anddir,
    "asminstruction": asminstruction,
    "assdir": assdir,
    "datadir": datadir,
    "enddir": enddir,
    "endpdir": endpdir,
    "endsdir": endsdir,
    "equdir": equdir,
    "labeldef": labeldef,
    "macrodir": macrodir,
    "notdir": notdir,
    "offsetdir": offsetdir,
    "ordir": ordir,
    "procdir": procdir,
    "ptrdir": ptrdir,
    "register": register,
    "segdir": segdir,
    "segmdir": segmdir,
    "segmentdir": segmentdir,
    "segmentregister": segmentregister,
    "segoverride": segoverride,
    "sqexpr": sqexpr,
    "xordir": xordir,
    "expr": make_token,
    "aexpr": make_token,
    "cexpr": make_token,
    "cxzexpr": make_token,
    "flagname": make_token,
    "primary": make_token,
    "recordconst": make_token,
    "simpleexpr": make_token,
    "sizearg": make_token,
    "term": make_token
}

recognizers = {
    'macroid': macroid
}


class ParglareParser(object):
    def __new__(cls, *args, **kwargs):
        if not hasattr(cls, '_inst'):
            cls._inst = super(ParglareParser, cls).__new__(cls)
            logging.debug("Allocated ParglareParser instance")

            file_name = os.path.dirname(os.path.realpath(__file__)) + "/_masm61.pg"
            cls._inst.grammar = Grammar.from_file(file_name, ignore_case=True, recognizers=recognizers)
            ## cls._inst.parser = PGParser(grammar, debug=True, debug_trace=True, actions=action.all)
            ## cls._inst.parser = PGParser(grammar, debug=True, actions=action.all)
            cls._inst.parser = PGParser(cls._inst.grammar,
                                        actions=actions)  # , build_tree = True, call_actions_during_tree_build = True)

        return cls._inst


def dump_object(value):
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


def calculate_type_size(type):
    binary_width = {'b': 1, 'w': 2, 'd': 4, 'q': 8, 't': 10}[type[1].lower()]
    return binary_width


class Parser:
    def __init__(self, skip_binary_data=[]):
        self.__skip_binary_data = skip_binary_data
        self.strip_path = 0
        self.__globals = {}
        self.__offsets = {}
        self.__offset_id = 0x1111
        self.__stack = []
        self.proc_list = []

        self.entry_point = ""

        # self.proc = None
        nname = "mainproc"
        self.proc = tasm.proc.Proc(nname)
        self.proc_list.append(nname)
        self.set_global(nname, self.proc)

        self.__binary_data_size = 0
        self.c_data = []
        self.h_data = []
        self.__cur_seg_offset = 0
        self.__dummy_enum = 0
        self.__segment = "default_seg"

        self.__symbols = []
        self.__link_later = []
        # self.data_started = False
        # self.prev_data_type = 0
        # self.prev_data_ctype = 0
        self.line_number = 0
        self.__lex = ParglareParser()
        self.used = False
        # self.__pgcontext = PGContext(extra = self)

    def visible(self):
        for i in self.__stack:
            if not i or i == 0:
                return False
        return True

    def push_if(self, text):
        value = self.eval(text)
        # logging.debug "if %s -> %s" %(text, value)
        self.__stack.append(value)

    def push_else(self):
        # logging.debug "else"
        self.__stack[-1] = not self.__stack[-1]

    def pop_if(self):
        # logging.debug "endif"
        return self.__stack.pop()

    def set_global(self, name, value):
        if len(name) == 0:
            raise Exception("empty name is not allowed")
        value.original_name = name
        name = name.lower()

        stuff = dump_object(value)
        logging.debug("set_global(name='%s',value=%s)" % (name, stuff))
        if name in self.__globals:
            raise Exception("global %s was already defined", name)
        value.used = False
        self.__globals[name] = value

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
            logging.debug("get_global KeyError %s" % name)
            raise
        g.used = True
        assert self.__globals[name].used
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
        return self.__offsets[name.lower()]

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

    def fix_dollar(self, v):
        logging.debug("$ = %d" % self.__cur_seg_offset)
        return re.sub(r'\$', "%d" % self.__cur_seg_offset, v)

    def parse_int(self, v):
        # logging.debug "~1~ %s" %v
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
        # logging.debug "~2~ %s" %v
        if re.match(r'^[+-]?[0-9][0-9A-Fa-f]*[Hh]$', v):
            v = int(v[:-1], 16)
            # logging.debug "~3~ %i" %v
        elif re.match(r'^[01]+[Bb]$', v):
            v = int(v[:-1], 2)
            # logging.debug "~2~ %i" %v

        try:
            vv = eval(v)
            v = vv
        except:
            pass

        # logging.debug "~4~ %s" %v
        return int(v)

    def calculate_data_binary_size(self, width, data):
        logging.debug("calculate_data_binary_size %d %s" % (width, data))
        s = 0
        for v in data:
            # v = v.strip()
            if isinstance(v, str) and width == 1 and len(v) >= 2 and (v[0] in ["'", '"']) and v[-1] == v[0]:
                v.replace("''", "'")
                s += len(v) - 2
                continue

            if isinstance(v, list) and len(v) == 5 and v[1].lower() == 'dup':
                # logging.error(v)
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
        logging.debug("get_global_value(%s)" % v)
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
        original_label = label
        label = label.lower()
        is_string = False
        elements = 0
        data_ctype = {1: 'db', 2: 'dw', 4: 'dd', 8: 'dq', 10: 'dt'}[width]
        r = [""]
        rh = []
        base = 1 << (8 * width)

        for v in data:
            # v = v.strip()
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

            # check if dup
            if isinstance(v, list) and len(v) == 5 and v[1].lower() == 'dup':
                # we should parse that
                group1 = v[0]
                if isinstance(group1, list):
                    group1 = "".join(group1)
                values = v[3]
                # value = value.strip()
                n, res = self.action_dup(group1, values)
                r += res
                elements += n
                continue

            elements += 1
            if isinstance(v, int) or isinstance(v, str):
                try:  # just number or many
                    v = v.strip()
                    if v == '?':
                        v = '0'
                    v = self.parse_int(v)

                    if v < 0:  # negative values
                        v += base

                except:
                    # global name
                    # traceback.print_stack(file=sys.stdout)
                    # logging.debug "global/expr: ~%s~" %v
                    try:
                        v = self.get_global_value(v, base)
                    except KeyError:
                        v = 0

            # vv = v.split()
            # logging.warning(vv)
            # if vv[0] == "offset":  # pointer
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
                    logging.warning("unknown address %s" % v)
                    logging.warning(self.c_data)
                    logging.warning(r)
                    logging.warning(len(self.c_data) + len(r))
                    self.__link_later.append((len(self.c_data) + len(r), v))
                    v = 0

            r.append(v)

        # cur_data_type = 0
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
            self.__dummy_enum += 1
            label = "dummy" + str(self.__dummy_enum)

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
            r = ["", vv]

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
                    # logging.debug "~~ " + r[i] + str(ord(r[i]))
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
            r = ["", vv]

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

        r.append(", // " + label + "\n")  # TODO can put original_label
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
                    val = ""
                    for v in value:
                        try:
                            v = self.parse_int(v)
                        except ValueError:
                            pass
                        val += str(v)
                    value = val
                else:
                    value = self.parse_int(value)
            # logging.debug "n = %d value = %d" %(n, value)

            res.append(value)
        return n, res

    def action_label_(self, line, far=False, isproc=False):
        # logging.info(line)
        m = re.match('([@\w]+)\s*::?', line)
        name = m.group(1).strip()
        logging.debug(name)
        self.action_label(name=name, far=far, isproc=isproc)

    def action_label(self, name, far=False, isproc=False):
        # if self.visible():
        # name = m.group(1)
        # logging.debug "~name: %s" %name
        name = re.sub(r'@', "arb", name)
        # logging.debug "~~name: %s" %name
        if not (name.lower() in self.__skip_binary_data):
            logging.debug("offset %s -> %s" % (name, "&m." + name.lower() + " - &m." + self.__segment))
            '''
            if self.proc is None:
                    nname = "mainproc"
                    self.proc = proc(nname)
                    #logging.debug "procedure %s, #%d" %(name, len(self.proc_list))
                    self.proc_list.append(nname)
                    self.set_global(nname, self.proc)
            '''
            if self.proc:
                self.proc.add_label(name, isproc)
                # self.set_offset(name, ("&m." + name.lower() + " - &m." + self.segment, self.proc, len(self.proc.stmts)))
                self.set_offset(name, ("&m." + name.lower() + " - &m." + self.__segment, self.proc, self.__offset_id))
                self.set_global(name, op.label(name, tasm.proc.Proc, line_number=self.__offset_id, far=far))
                self.__offset_id += 1
            else:
                logging.error("!!! Label %s is outside the procedure" % name)
        else:
            logging.info("skipping binary data for %s" % (name,))

    def create_segment(self, name):
        binary_width = 1
        offset = self.__binary_data_size // 16
        logging.debug("segment %s %x" % (name, offset))
        self.__cur_seg_offset = 16

        num = (0x10 - (self.__binary_data_size & 0xf)) & 0xf
        if num:
            l = ['0'] * num
            self.__binary_data_size += num

            self.__dummy_enum += 1
            labell = "dummy" + str(self.__dummy_enum)

            self.c_data.append("{" + ",".join(l) + "}, // padding\n")
            self.h_data.append(" db " + labell + "[" + str(num) + "]; // padding\n")

        num = 0x10
        l = ['0'] * num
        self.__binary_data_size += num

        self.c_data.append("{" + ",".join(l) + "}, // segment " + name + "\n")
        self.h_data.append(" db " + name + "[" + str(num) + "]; // segment " + name + "\n")

        self.set_global(name, op.var(binary_width, offset, name, issegment=True))
        '''
        if self.proc == None:
                name = "mainproc"
                self.proc = proc(name)
                #logging.debug "procedure %s, #%d" %(name, len(self.proc_list))
                self.proc_list.append(name)
                self.set_global(name, self.proc)
        '''

    def parse_file(self, fname):
        self.line_number = 0
        skipping_binary_data = False
        num = 0x1000
        if num:
            self.__binary_data_size += num

            self.__dummy_enum += 1
            labell = "dummy" + str(self.__dummy_enum)

            self.c_data.append("{0}, // padding\n")
            self.h_data.append(" db " + labell + "[" + str(num) + "]; // protective\n")
        self.parse_file_lines(fname, skipping_binary_data)
        num = (0x10 - (self.__binary_data_size & 0xf)) & 0xf
        if num:
            l = num * ['0']
            self.__binary_data_size += num

            self.__dummy_enum += 1
            labell = "dummy" + str(self.__dummy_enum)

            self.c_data.append("{" + ",".join(l) + "}, // padding\n")
            self.h_data.append(" db " + labell + "[" + str(num) + "]; // padding\n")
        return self

    def parse_file_lines(self, file_name, skipping_binary_data):
        content = read_asm_file(file_name)
        self.parse_args_new_data(content, file_name=file_name)
        return

    def parse_include_file_lines(self, file_name):
        content = read_asm_file(file_name)
        return self.parse_file_inside(content, file_name=file_name)

        '''
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

            elif cmd0l == 'include':
                self.action_include(line)
                continue
    '''

    def tokenstostring(self, l):  # TODO remove
        if isinstance(l, str):
            return l
        elif isinstance(l, list):
            return " ".join([self.tokenstostring(i) for i in l])
        elif isinstance(l, Token):
            return l.value

    def action_assign(self, name, value):
        name = name.value
        value = self.tokenstostring(value)
        vv = self.get_equ_value(value)
        has_global = False
        if self.has_global(name):
            has_global = True
            name = self.get_global(name).original_name
        o = self.proc.add_assignment(name, vv, line_number=self.line_number)
        if not has_global:
            self.set_global(name, o)
        self.proc.stmts.append(o)
        return o

    def action_equ(self, name, value):
        name = name.value
        value = self.tokenstostring(value)
        vv = self.get_equ_value(value)
        proc = self.get_global("mainproc")
        o = proc.add_equ_(name, vv, line_number=self.line_number)
        self.set_global(name, o)
        proc.stmts.insert(0, o)
        return o

    def action_segment(self, name):
        name = name.lower()
        self.__segment = name
        self.create_segment(name)

    def action_proc(self, name, type):
        logging.info("procedure name %s" % name.value)
        name = name.value.lower()
        far = ''
        for i in type:
            if i and i.lower() == 'far':
                far = True
        '''
                                        name = cmd0l
                                        self.proc = proc(name)
                                        logging.debug "procedure %s, #%d" %(name, len(self.proc_list))
                                        self.proc_list.append(name)
                                        self.set_global(name, self.proc)
        '''

        self.action_label(name, far=far, isproc=True)

    def action_simplesegment(self, type, name):
        if name is None:
            name = ""
        else:
            type = type + "_"
        type = type[1:] + name
        self.action_segment(type)

    def action_end(self, line):
        cmd = line.split()
        if len(cmd) >= 2:
            self.entry_point = cmd[1].lower()

    def action_prefix(self, line):
        cmd = line.split()
        cmd0 = str(cmd[0])
        cmd0l = cmd0.lower()

        o = self.proc.action_instruction(cmd0l)
        o.line = cmd0l
        o.line_number = self.line_number
        self.proc.stmts.append(o)

        o = self.proc.action_instruction(" ".join(cmd[1:]))
        o.line = " ".join(cmd[1:])
        o.line_number = self.line_number
        self.proc.stmts.append(o)

    def action_endseg(self):
        logging.debug("segment %s ends" % self.__segment)
        self.__segment = "default_seg"

    def action_include(self, name):
        logging.info("including %s" % name)
        self.parse_file(name)

    def action_endp(self):
        self.proc = self.get_global('mainproc')

    def action_endif(self):
        self.pop_if()

    def action_else(self):
        self.push_else()

    def action_if(self, line):
        cmd = line.split()
        self.push_if(cmd[1])

    def action_code(self, line):
        result = self.parse_args_new_data_('''.model tiny
default_seg segment
    ''' + line + '''
default_seg ends
    end start
    ''').asminstruction
        del self.__globals['default_seg']
        return result

    def test_size(self, line):
        result = self.parse_args_new_data_('''.model tiny
    default_seg segment
    push    ''' + line + '''
    default_seg ends
        end start
        ''').asminstruction.arg
        del self.__globals['default_seg']
        return result

    def action_data(self, line):
        result = self.parse_args_new_data_('''.model tiny
default_seg segment
''' + line + '''
default_seg ends
end startd
''')
        del self.__globals['default_seg']
        return result

    def parse_arg(self, line):
        result = self.parse_args_new_data_('''.model tiny
    default_seg segment
    push ''' + line + '''
    default_seg ends
    end startd
    ''').asminstruction.arg
        del self.__globals['default_seg']
        return result

    def parse_include(self, line, file_name=None):
        parser = PGParser(self.__lex.grammar,
                                    actions=actions)  # , build_tree = True, call_actions_during_tree_build = True)
        result = parser.parse('''.model tiny
    some_seg segment
        ''' + line + '''
    some_seg ends
        end start
        ''', file_name=file_name, extra=self)  # context = self.__pgcontext)
        del self.__globals['some_seg']
        return result

    def datadir_action(self, name, type, args):
        offset = self.__cur_seg_offset
        logging.debug("data value %s offset %d" % (str(args), offset))
        binary_width = calculate_type_size(type)
        s = self.calculate_data_binary_size(binary_width, args)
        self.__binary_data_size += s
        self.__cur_seg_offset += s
        c, h, elements = self.convert_data_to_c(name, binary_width,
                                                args)
        self.c_data += c
        self.h_data += h
        logging.debug("~size %d elements %d" % (binary_width, elements))
        if name:
            self.set_global(name.lower(), op.var(binary_width, offset, name=name,
                                                 segment=self.__segment, elements=elements))
        # logging.debug("~~        self.assertEqual(parser_instance.parse_data_line_whole(line='"+str(line)+"'),"+str(("".join(c), "".join(h), offset2 - offset))+")")
        return c, h, s

    def get_equ_value(self, v):
        logging.debug("%s" % v)
        vv = self.fix_dollar(v)
        # ? vv = " ".join(self.parse_args(vv))
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
        # logging.debug self.c_data
        for addr, expr in self.__link_later:
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
        # logging.debug self.c_data

    def parse_args_new_data_(self, text):
        # self.__pgcontext = PGContext(extra = self)
        self.__binary_data_size = 0
        self.__dummy_enum = 0  # one dummy number is used for "default_seg" creation
        return self.parse_file_insideseg(text)

    def parse_file_insideseg(self, text):
        return self.parse_args_new_data(text)[0][1][1][0].insegmentdir

    def parse_file_inside(self, text, file_name=None):
        return self.parse_include(text, file_name)

    def parse_args_new_data(self, text, file_name=None):
        logging.debug("parsing: [%s]" % text)

        result = self.__lex.parser.parse(text, file_name=file_name, extra=self)  # context = self.__pgcontext)
        # result = self.__lex.parser.call_actions(tree)
        logging.debug(str(result))
        return result

