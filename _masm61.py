#!/usr/bin/python3
# -*- coding: utf-8 -*-
import sys, os, re
from parglare import Grammar

macroids=[]
macroidre = re.compile(r'([A-Za-z@_\$\?][A-Za-z@_\$\?0-9]*)')

def macro_action(context, nodes, name):
    macroids.insert(0,name.lower())
    print ("added ~~" + name + "~~")

def macroid(head, input, pos):
    mtch = macroidre.match(input[pos:])
    if mtch:
        result = mtch.group().lower()
        print ("matched ~^~" + result+"~^~")
        if result in macroids:
           print (" ~^~ in macroids")
           return result
        else:
           return None
    else:
        return None

recognizers = {
    'macroid': macroid
}

actions = {
"macrodir": macro_action
}

file_name = os.path.dirname(os.path.realpath(__file__))+"/tasm/_masm61.pg"
grammar = Grammar.from_file(file_name, ignore_case=True, recognizers=recognizers)

from parglare import Parser
#parser = Parser(grammar, debug=True, debug_trace=True, actions={"macrodir": macro_action})
#parser = Parser(grammar, debug=True, actions={"macrodir": macro_action})
parser = Parser(grammar, actions=actions)

codeset = 'cp437'

if sys.version_info[0] >= 3:
     sys.stdout.reconfigure(encoding='utf-8')

for i in sys.argv[1:]:

  if sys.version_info >= (3, 0):
     f = open(i, "rt", encoding=codeset)
  else:
     f = open(i, "rt")


  input_str=f.read()

  result = parser.parse(input_str)
  f.close()

  print(result)

