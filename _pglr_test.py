#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import re
import sys

import parglare
from parglare import Grammar

macroids = []
macroidre = re.compile(r'([A-Za-z@_\$\?][A-Za-z@_\$\?0-9]*)')


def macro_action(context, nodes, name):
    macroids.insert(0, name.lower())
    print("added ~~" + name + "~~")


def macroid(head, input, pos):
    mtch = macroidre.match(input[pos:])
    if mtch:
        result = mtch.group().lower()
        print("matched ~^~" + result + "~^~")
        if result in macroids:
            print(" ~^~ in macroids")
            return result
        else:
            return None
    else:
        return None


class Token:
    #__slots__ = ('name', 'type', 'keyword', 'value')

    def __init__(self, type, value):
        #self.name = name
        self.type = type
        #self.keyword = keyword
        self.value = value

    def __repr__(self):
        from pprint import pformat
        return pformat(vars(self))


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

def make_token(context, nodes):
    if len(nodes) == 1 and context.production.rhs[0].name not in ('type','e01','e02','e03','e04','e05','e06','e07','e08','e09','e10','e11'):
       nodes = Token(context.production.rhs[0].name, nodes[0])
    if context.production.rhs[0].name in ('type','e01','e02','e03','e04','e05','e06','e07','e08','e09','e10','e11'):
       nodes = nodes[0]
    print("~"+str(nodes)+"~")
    return nodes

cur_segment = 'ds' #!
indirection = 0 #!

def segoverride(context, nodes):
    global cur_segment
    if isinstance(nodes[0], list):
        cur_segment = nodes[0][-1]
        return nodes[0][:-1] + [Token('segoverride', nodes[0][-1]), nodes[2]]
    cur_segment = nodes[0] #!
    return [Token('segoverride', nodes[0]), nodes[2]]

def ptrdir(context, nodes):
    if len(nodes) == 3:
        return [Token('ptrdir', nodes[0]), nodes[2]]
    else:
        return [Token('ptrdir', nodes[0]), nodes[1]]


def asminstruction(context, nodes, instruction, args):
    global cur_segment
    global indirection
    print("instruction " + get_raw(context) + " "+ str(nodes) + " ~~ "+str(cur_segment)+" "+str(indirection))
    cur_segment = 'ds' #!
    indirection = 0 #!
    # if args != None:
    #    args = [listtostring(i) for i in args] # TODO temporary workaround
    #args = build_ast(args)
    #print(instruction.children[0].value, str(args))
    return nodes


def NOT(context, nodes):
    nodes[0] = '~'
    return nodes

def OR(context, nodes):
    nodes[1] = '|'
    return nodes

def XOR(context, nodes):
    nodes[1] = '^'
    return nodes

def AND(context, nodes):
    nodes[1] = ' & '
    return nodes

def register(context, nodes):
    return Token('register', nodes[0].lower())

def sqexpr(context, nodes):
    global indirection
    indirection = 1
    print("/~"+str(nodes)+"~\\")
    #print("/~"+str(nodes[1][1])+"~\\")
    res = nodes[1]
    #res = [i[1] for i in nodes[1]]

    #if isinstance(res, list):
    #   res = res[0]
    #else:
    #   res = sum([[i, '+'] for i in res.value], [])[:-1]
    #print("~"+str(res)+"~")
    return Token('sqexpr', res)

def offsetdir(context, nodes):
    #print("offset /~"+str(nodes)+"~\\")
    #global indirection
    #indirection = -1
    return Token('offsetdir', nodes[1])

def segmdir(context, nodes):
    #print("offset /~"+str(nodes)+"~\\")
    #global indirection
    #indirection = -1
    return Token('segmdir', nodes[1])

def LABEL(context, nodes):
    return Token('LABEL', nodes)

def STRING(context, nodes):
    return Token('STRING', nodes)

recognizers = {
    'macroid': macroid
}

actions = {
    "macrodir": macro_action,
    "asminstruction": asminstruction,
    "expr": make_token,
    "STRING": STRING,
    "LABEL": LABEL,
    "aexpr": make_token,
    "cexpr": make_token,
    "cxzexpr": make_token,
    "flagname": make_token,
    "primary": make_token,
    "recordconst": make_token,
    "simpleexpr": make_token,
    "sizearg": make_token,
    "term": make_token,
    "segoverride": segoverride,
    "ptrdir": ptrdir,
    "offsetdir": offsetdir,
    "segmdir": segmdir,
    "register": register,
    "sqexpr": sqexpr,
    "NOT": NOT,
    "OR": OR,
    "XOR": XOR,
    "AND": AND
}


file_name = os.path.dirname(os.path.realpath(__file__)) + "/tasm/_masm61.pg"
grammar = Grammar.from_file(file_name, ignore_case=True, recognizers=recognizers)

from parglare import Parser

# parser = Parser(grammar, debug=True, debug_trace=True, actions={"macrodir": macro_action})
# parser = Parser(grammar, debug=True, actions={"macrodir": macro_action})
#parser = Parser(grammar, debug=True, actions=actions, debug_colors=True)
parser = Parser(grammar, actions=actions) #, build_tree=True, call_actions_during_tree_build=True)

codeset = 'cp437'

if sys.version_info[0] >= 3:
    sys.stdout.reconfigure(encoding='utf-8')

for i in sys.argv[1:]:

    if sys.version_info >= (3, 0):
        f = open(i, "rt", encoding=codeset)
    else:
        f = open(i, "rt")

    input_str = f.read()

    result = parser.parse(input_str, file_name=file_name)
    f.close()

    from pprint import pprint

    pprint(result)
