#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
from parglare import Grammar

file_name = "_masm61.pg"
grammar = Grammar.from_file(file_name, ignore_case=True)

from parglare import Parser
#parser = Parser(grammar, debug=True, debug_trace=True)
#parser = Parser(grammar, debug=True)
parser = Parser(grammar)

codeset = 'cp437'
if sys.version_info >= (3, 0):
   f = open(sys.argv[1], "rt", encoding=codeset)
else:
   f = open(sys.argv[1], "rt")

if sys.version_info[0] >= 3:
     sys.stdout.reconfigure(encoding='utf-8')


input_str=f.read()

result = parser.parse(input_str)
f.close()

print(result)
