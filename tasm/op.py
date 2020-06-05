from __future__ import absolute_import
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
from builtins import str
from builtins import object

from tasm import lex


# import traceback
# import sys

class Unsupported(Exception):
    pass


class var(object):

    def __init__(self, size, offset, name="", segment="", issegment=False, elements=1):
        #logging.debug("op.var(%d, %d, %s, %s, %s, %d)" %(size, offset, name, segment, issegment, elements))
        self.size = size
        self.offset = offset
        self.name = name
        self.segment = segment
        self.issegment = issegment
        self.elements = elements
        #logging.debug("op.var(%s)" %(str(self.__dict__).replace('\n',' ')))


class equ(object):
    def __init__(self, value, size=0):
        self.value = value
        if size:
            self.size = size


class assignment(object):
    def __init__(self, value, size=0):
        self.value = value
        if size:
            self.size = size


class reg(object):
    def __init__(self, name):
        self.name = name

    def size(self):
        return 2 if self.name[1] == 'x' else 1

    def __str__(self):
        return "<register %s>" % self.name


class unref(object):
    def __init__(self, exp):
        self.exp = exp

    def __str__(self):
        return "<unref %s>" % self.exp


class ref(object):
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return "<ref %s>" % self.name


class glob(object):
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return "<global %s>" % self.name


class segment(object):
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return "<segment %s>" % self.name


class baseop(object):
    cmd = ""
    line = ""
    line_number = 0
    elements = 1

    # def __str__(self):
    #        return self.cmd+" "+self.command+" "+str(self.line_number)

    def parse_arg(self, arg):
        args = self.split(arg)
        if len(args) > 0:
            return args[0]
        return ""

    def split(self, text):
        # print "text %s" %text
        #               traceback.print_stack(file=sys.stdout)
        return lex.parse_args(text)
        # a, b = lex.parse_args(text)

    #               logging.info "a %s b %s" %(a, b)
    # return self.parse_arg(a), self.parse_arg(b)
    def __str__(self):
        return str(self.__class__)


class basejmp(baseop):
    pass


class _call(baseop):
    def __init__(self, arg):
        self.name = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._call(self.name)

    def __str__(self):
        return "call(%s)" % self.name


class _rep(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._rep(self.arg)


class _sub(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._sub(self.dst, self.src)

class _mul(baseop):
    def __init__(self, arg):
        self.arg = self.split(arg)

    def visit(self, visitor):
        return visitor._mul(self.arg)


class _div(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._div(self.arg)


class _xor(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._xor(self.dst, self.src)

class _jne(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jnz(self.label)

class _je(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jz(self.label)

class _jb(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jc(self.label)

class _jae(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jnc(self.label)


class _jnb(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jnc(self.label)

class _push(baseop):
    def __init__(self, arg):
        self.regs = []
        for r in lex.parse_args(arg)[0].split():
            self.regs.append(self.parse_arg(r))

    def visit(self, visitor):
        return visitor._push(self.regs)

class _pop(baseop):
    def __init__(self, arg):
        self.regs = []
        for r in lex.parse_args(arg)[0].split():
            self.regs.append(self.parse_arg(r))

    def visit(self, visitor):
        return visitor._pop(self.regs)

class _ret(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._ret()

class _retn(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._ret()

class _lodsb(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._lodsb()

class _scasb(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._scasb()

class _cmpsb(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._cmpsb()

class _lodsw(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._lodsw()

class _lodsd(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._lodsd()

class _stosd(baseop):
    def __init__(self, arg):
        self.repeat = 1
        self.clear_cx = False

    def visit(self, visitor):
        return visitor._stosd(self.repeat, self.clear_cx)

class _stosw(baseop):
    def __init__(self, arg):
        self.repeat = 1
        self.clear_cx = False

    def visit(self, visitor):
        return visitor._stosw(self.repeat, self.clear_cx)


class _stosb(baseop):
    def __init__(self, arg):
        self.repeat = 1
        self.clear_cx = False

    def visit(self, visitor):
        return visitor._stosb(self.repeat, self.clear_cx)

class _movsw(baseop):
    def __init__(self, arg):
        self.repeat = 1
        self.clear_cx = False

    def visit(self, visitor):
        return visitor._movsw(self.repeat, self.clear_cx)

class _movsd(baseop):
    def __init__(self, arg):
        self.repeat = 1
        self.clear_cx = False

    def visit(self, visitor):
        return visitor._movsd(self.repeat, self.clear_cx)


class _movsb(baseop):
    def __init__(self, arg):
        self.repeat = 1
        self.clear_cx = False

    def visit(self, visitor):
        return visitor._movsb(self.repeat, self.clear_cx)

class _int(baseop):
    def __init__(self, arg):
        self.dst = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._int(self.dst)

class _nop(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        pass

class label(baseop):

    def __init__(self, name, proc, line_number=0, far=False):
        self.name = name
        self.line_number = line_number
        self.far = far
        self.proc = proc

    def visit(self, visitor):
        return visitor._label(self.name, self.proc)

class _lea(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._lea(self.dst, self.src)

class _repe(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._repe(self.arg)

class _repne(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._repne(self.arg)

class _jna(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jbe(self.label)

class _jnbe(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._ja(self.label)

class _imul(baseop):
    def __init__(self, arg):
        self.arg = self.split(arg)

    def visit(self, visitor):
        return visitor._imul(self.arg)

class _movs(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._movs(self.dst, self.src)

class _lods(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._lods(self.arg)

class _scas(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._scas(self.arg)

class _leave(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._leave()

class _idiv(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._idiv(self.arg)

class _instruction0(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._instruction0(self.cmd)

class _instruction1(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._instruction1(self.cmd, self.arg)

class _jump(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._jump(self.cmd, self.arg)

class _instruction2(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._instruction2(self.cmd, self.dst, self.src)

class _instruction3(baseop):
    def __init__(self, arg):
        self.dst, self.src, self.c = lex.parse_args(arg)

    def visit(self, visitor):
        return visitor._instruction3(self.cmd, self.dst, self.src, self.c)

class _equ(baseop):
    def __init__(self, dst, src):
        self.dst, self.src = dst, src

    def visit(self, visitor):
        return visitor._equ(self.dst, self.src)

class _assignment(baseop):
    def __init__(self, dst, src):
        self.dst, self.src = dst, src

    def visit(self, visitor):
        logging.debug("~ %s = %s" % (self.dst, self.src))
        return visitor._assignment(self.dst, self.src)
