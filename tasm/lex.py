from __future__ import print_function

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
import logging
import os
import re
from builtins import str

from parglare import Grammar, Parser

def escape(str):
    if isinstance(str,list):
        return [escape(i) for i in str]
    else:
        return str.translate(str.maketrans({"\\": r"\\"}))

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


class Lex(object):

    def __new__(cls,*args, **kwargs):
        if not hasattr(cls,'_inst'):
            cls._inst = super(Lex, cls).__new__(cls)
            logging.debug("Allocated Lex instance")

            file_name = os.path.dirname(os.path.realpath(__file__)) + "/_masm61.pg"
            grammar = Grammar.from_file(file_name, ignore_case=True, recognizers=recognizers)
            ## parser = Parser(grammar, debug=True, debug_trace=True)
            ## parser = Parser(grammar, debug=True)
            cls._inst.parser = Parser(grammar)

        return cls._inst


    def __init__(self):
        pass

    def parse_args_new_data(self, text):
        # print "parsing: [%s]" %text

        return self.parser.parse(text)

    def parse_args(self, text):
        # print "parsing: [%s]" %text
        escape = False
        string = False
        result = []
        token = ""
        # value = 0
        for c in text:
            # print "[%s]%s: %s: %s" %(token, c, escape, string)
            if c == '\\':
                token += c
            #               if c == '\\':
            #                       escape = True
            #                       continue

            if escape:
                if not string:
                    raise SyntaxError("escape found in no string: %s" % text)

                logging.debug("escaping[%s]" % c)
                escape = False
                token += c
                continue

            if string:
                if c == '\'' or c == '"':
                    string = False

                token += c
                continue

            if c == '\'' or c == '"':
                string = True
                token += c
                continue

            if c == ',':
                result.append(token.strip())
                token = ""
                continue

            if c == ';':  # comment, bailing out
                break

            token += c
        token = token.strip()
        if len(token):
            result.append(token)
        # print result
        return result

    # def compile(width, data):
    #        logging.debug(data)
    #        return data

    def parse_line_data(self, line):
            cmd = line.split()
            cmd1l = cmd[1].lower()
            cmd0 = str(cmd[0])
            cmd0 = cmd0.lower()
            if cmd1l in ['db', 'dw', 'dd', 'dq', 'dt']:
                name = cmd0
                cmd0l = cmd0
                name = re.sub(r'@', "arb", name)
                arg = line[len(cmd0l):].strip()
                arg = arg[len(cmd1l):].strip()
                args = self.parse_args(arg)
                return name, cmd1l, args
            else:
                arg = line[len(cmd0):].strip()
                args = self.parse_args(arg)
                return "", cmd0, args

    def parse_line_data_new(self, line):
            #logging.error(line)
            cmd = line.split()
            cmd1l = cmd[1].lower()
            cmd0 = str(cmd[0])
            cmd0 = cmd0.lower()
            if cmd1l in ['db', 'dw', 'dd', 'dq', 'dt']:
                name = cmd0
                cmd0l = cmd0
                arg = line[len(cmd0l):]
                #arg = arg[len(cmd1l):].strip()
                arg = name + " " + arg + '\n'
                #logging.error(arg)
                args = self.parse_args_new_data(line+'\n')
                #logging.error(args.values)
                argg = [escape(i) for i in args.values]
                args.name = re.sub(r'@', "arb", args.name).lower() # TODO lower
                '''
                j=[]
                for i in argg:
                    if isinstance(i,list):
                        i=" ".join(i)
                    j=j+[i]
                argg = j
                '''
                args = (args.name, args.type, argg)
                return args
            else:
                #arg = line[len(cmd0):].strip()
                args = self.parse_args_new_data(line+'\n')
                #logging.error(args.values)
                args = ("", args.type, args.values)
                return args

