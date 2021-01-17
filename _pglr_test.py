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
    if len(nodes) == 1 and context.production.rhs[0].name not in ('type'):
       nodes = Token(context.production.rhs[0].name, nodes[0])
    if context.production.rhs[0].name in ('type'):
       nodes = nodes[0]
    return nodes

def addop(context, nodes):
    return make_token(context, nodes)

def expr(context, nodes):
    return make_token(context, nodes)

def asminstruction(context, nodes, instruction, args):
    print("instruction " + str(nodes) + " ~~")
    # if args != None:
    #    args = [listtostring(i) for i in args] # TODO temporary workaround
    #args = build_ast(args)
    #print(instruction.children[0].value, str(args))
    return nodes


recognizers = {
    'macroid': macroid
}

actions = {
    "macrodir": macro_action,
    "asminstruction": asminstruction,
    "expr": expr,
#    "addop": expr,
    "aexpr": expr,
#    "binaryop": expr,
    "cexpr": expr,
    "cxzexpr": expr,
    "flagname": expr,
#    "mulop": expr,
#    "orop": expr,
    "primary": expr,
    "recordconst": expr,
#    "relop": expr,
#    "shiftop": expr,
    "simpleexpr": expr,
    "sizearg": expr,
    "term": expr
}

file_name = os.path.dirname(os.path.realpath(__file__)) + "/tasm/_masm61.pg"
grammar = Grammar.from_file(file_name, ignore_case=True, recognizers=recognizers)

from parglare import Parser

# parser = Parser(grammar, debug=True, debug_trace=True, actions={"macrodir": macro_action})
# parser = Parser(grammar, debug=True, actions={"macrodir": macro_action})
#parser = Parser(grammar, debug=True, actions=actions)
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
