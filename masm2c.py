#!/usr/bin/python3
# -*- coding: utf-8 -*-
"""masm-recover

Usage:
  masm-recover <in>
"""
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

#from docopt import docopt


from __future__ import print_function
from tasm.parser import Parser
from tasm.cpp import Cpp
import sys, re, logging

def tracefunc(frame, event, arg, indent=[0]):
      if event == "call":
          indent[0] += 2
          print("-" * indent[0] + "> call function", frame.f_code.co_filename, frame.f_code.co_name)
      elif event == "return":
          print("<" + "-" * indent[0], "exit function", frame.f_code.co_name)
          indent[0] -= 2
      return tracefunc

def valid(params):
    inf = params.get('<in>', False)
    #outf = params.get('<out>', False)
    if not inf:
        return False
    #if not outf:
    #    return False

def main():
  #sys.settrace(tracefunc)

  if sys.version_info[0] >= 3:
     sys.stdout.reconfigure(encoding='utf-8')
     sys.stderr.reconfigure(encoding='utf-8')
  # assuming loglevel is bound to the string value obtained from the
  # command line argument. Convert to upper case to allow the user to
  # specify --log=DEBUG or --log=debug
  for i in sys.argv[1:]:
     process(i)

def process(i):
  name = i
  m = re.match(r'.*?([A-Za-z90-9_.-]+)\.asm', name.lower())
  outname=""
  if m is not None:
     outname = m.group(1).strip()

  '''
  numeric_level = getattr(logging, loglevel.upper(), None)
  if not isinstance(numeric_level, int):
  '''
  numeric_level = logging.INFO
  ##if i == '-d':
  #numeric_level = logging.DEBUG
  logging.basicConfig(
     handlers=[logging.FileHandler(name+'.log', 'w', 'utf-8')], 
     format="[%(filename)s:%(lineno)s - %(funcName)20s()] %(message)s",
     level=numeric_level
  )

  root = logging.getLogger()
  #if i == '-d':
  root.setLevel(logging.DEBUG)
  #   return

  handler = logging.StreamHandler(sys.stderr)
  handler.setLevel(logging.ERROR)
  ##formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
  ##handler.setFormatter(formatter)
  root.addHandler(handler)

  handler = logging.StreamHandler(sys.stdout)
  handler.setLevel(logging.INFO)
  ##formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
  ##handler.setFormatter(formatter)
  root.addHandler(handler)

  p = Parser(skip_binary_data = [
      # These data blobs are not output
      # dreamweb.asm
      #	'characterset1',
   ])
  p.strip_path = 3
  context = p.parse_file(name)
  p.link()

  generator = Cpp(context, outfile = outname, blacklist = [
      # These functions are not processed

      #	'aboutturn',
      ], skip_output = [
      # These functions are processed but not output
      ], skip_dispatch_call = True, skip_addr_constants = True,
                  header_omit_blacklisted = True,
                  function_name_remapping = {
      # This remaps the function naming at output for readability
      #	'_moduleread' : '_moduleread',
      })
  generator.generate('mainproc') #start routine

from tasm import op
from tasm import proc
from tasm import lex
from tasm import parser
from tasm import cpp

main()
'''
import auger
import re

if __name__ == "__main__":
  with auger.magic([parser, lex, op, cpp, proc]):   # this is the new line and invokes Auger
    main()
'''