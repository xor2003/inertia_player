#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys, os
from parglare import Grammar

file_name = os.path.dirname(os.path.realpath(__file__))+"/_masm61.pg"
grammar = Grammar.from_file(file_name, ignore_case=True)

from parglare import Parser
#parser = Parser(grammar, debug=True, debug_trace=True)
parser = Parser(grammar, debug=True)
#parser = Parser(grammar)

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
