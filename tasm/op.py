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
    elements = 1

    def __init__(self, size, offset, name="", segment="", issegment=False):
        self.size = size
        self.offset = offset
        self.name = name
        self.segment = segment
        self.issegment = issegment


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


class _mov(baseop):
    def __init__(self, arg):
        # print arg
        self.dst, self.src = self.split(arg)
        # print self.dst, self.src

    def visit(self, visitor):
        return visitor._mov(self.dst, self.src)

    def __str__(self):
        return "mov(%s, %s)" % (self.dst, self.src)


'''
class _mov2(baseop):
        def __init__(self, dst, src):
                self.dst, self.src = dst, src
        def visit(self, visitor):
                visitor._mov(self.dst, self.src)
'''


class _shr(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._shr(self.dst, self.src)


class _shl(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._shl(self.dst, self.src)


class _ror(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._ror(self.dst, self.src)


class _rol(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._rol(self.dst, self.src)


class _sar(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._sar(self.dst, self.src)


class _sal(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._sal(self.dst, self.src)


class _rcl(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._rcl(self.dst, self.src)


class _rcr(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._rcr(self.dst, self.src)


class _neg(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._neg(self.arg)


class _dec(baseop):
    def __init__(self, arg):
        self.dst = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._dec(self.dst)


class _inc(baseop):
    def __init__(self, arg):
        self.dst = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._inc(self.dst)


class _add(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._add(self.dst, self.src)


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


class _and(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._and(self.dst, self.src)


class _xor(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._xor(self.dst, self.src)


class _or(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._or(self.dst, self.src)


class _cmp(baseop):
    def __init__(self, arg):
        self.a, self.b = self.split(arg)

    def visit(self, visitor):
        return visitor._cmp(self.a, self.b)


class _test(baseop):
    def __init__(self, arg):
        self.a, self.b = self.split(arg)

    def visit(self, visitor):
        return visitor._test(self.a, self.b)


class _xchg(baseop):
    def __init__(self, arg):
        self.a, self.b = self.split(arg)

    def visit(self, visitor):
        return visitor._xchg(self.a, self.b)


class _jnz(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jnz(self.label)


class _jne(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jnz(self.label)


class _jz(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jz(self.label)


class _je(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jz(self.label)


class _jc(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jc(self.label)


class _jb(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jc(self.label)


class _jnc(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jnc(self.label)


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


class _js(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._js(self.label)


class _jns(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jns(self.label)


class _jl(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jl(self.label)


class _jg(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jg(self.label)


class _jle(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jle(self.label)


class _jge(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jge(self.label)


class _jmp(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jmp(self.label)


class _loop(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._loop(self.label)


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


class _retf(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._retf()


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


class _in(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._in(self.dst, self.src)


#               raise Unsupported("input from port: %s" %self.arg)

class _out(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._out(self.dst, self.src)


#               raise Unsupported("out to port: %s" %self.arg)

class _cli(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._cli()


#               raise Unsupported("cli")

class _sti(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._sti()


#               raise Unsupported("sli")

class _int(baseop):
    def __init__(self, arg):
        self.dst = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._int(self.dst)


#               raise Unsupported("interrupt: %s" %self.arg)

class _iret(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._iret()


#               raise Unsupported("interrupt return")

class _cbw(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._cbw()


class _nop(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        pass


class _stc(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._stc()


class _xlat(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._xlat()


class _clc(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._clc()


class label(baseop):

    def __init__(self, name, proc, line_number=0, far=False):
        self.name = name
        self.line_number = line_number
        self.far = far
        self.proc = proc

    def visit(self, visitor):
        return visitor._label(self.name, self.proc)


class _cld(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._cld()


class _std(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._std()


class _movzx(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._movzx(self.dst, self.src)

    def __str__(self):
        return "MOVZX(%s, %s)" % (self.dst, self.src)


class _movsx(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._movsx(self.dst, self.src)

    def __str__(self):
        return "MOVSX(%s, %s)" % (self.dst, self.src)


class _lea(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._lea(self.dst, self.src)

    def __str__(self):
        return "%s = %s" % (self.dst, self.src)


class _lds(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._lds(self.dst, self.src)

    def __str__(self):
        return "LDS(%s, %s)" % (self.dst, self.src)


class _les(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._les(self.dst, self.src)

    def __str__(self):
        return "LES(%s, %s)" % (self.dst, self.src)


class _lfs(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._lfs(self.dst, self.src)

    def __str__(self):
        return "LFS(%s, %s)" % (self.dst, self.src)


class _lgs(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._lgs(self.dst, self.src)

    def __str__(self):
        return "LGS(%s, %s)" % (self.dst, self.src)


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


class _adc(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._adc(self.dst, self.src)


class _jbe(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jbe(self.label)


class _jna(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jbe(self.label)


class _ja(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._ja(self.label)


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


class _cmc(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._cmc()


class _shrd(baseop):
    def __init__(self, arg):
        self.dst, self.src, self.c = lex.parse_args(arg)

    def visit(self, visitor):
        return visitor._shrd(self.dst, self.src, self.c)


class _shld(baseop):
    def __init__(self, arg):
        self.dst, self.src, self.c = lex.parse_args(arg)

    def visit(self, visitor):
        return visitor._shld(self.dst, self.src, self.c)


class _loope(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._loope(self.label)


class _loopne(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._loopne(self.label)


class _jcxz(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jcxz(self.label)


class _jecxz(basejmp):
    def __init__(self, label):
        self.label = self.parse_arg(label)

    def visit(self, visitor):
        return visitor._jecxz(self.label)


class _pushf(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._pushf()


class _popf(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._popf()


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


class _setnz(baseop):
    def __init__(self, arg):
        self.dst = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._setnz(self.dst)


class _setz(baseop):
    def __init__(self, arg):
        self.dst = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._setz(self.dst)


class _setb(baseop):
    def __init__(self, arg):
        self.dst = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._setb(self.dst)


class _sbb(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._sbb(self.dst, self.src)


class _bt(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._bt(self.dst, self.src)


class _btc(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._btc(self.dst, self.src)


class _btr(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._btr(self.dst, self.src)


class _bts(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._bts(self.dst, self.src)


class _bsr(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._bsr(self.dst, self.src)


class _bsf(baseop):
    def __init__(self, arg):
        self.dst, self.src = self.split(arg)

    def visit(self, visitor):
        return visitor._bsr(self.dst, self.src)


class _pushad(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._pushad()


class _popad(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._popad()


class _pusha(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._pusha()


class _popa(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._popa()


class _aad(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._aad()


class _daa(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._daa()


class _cwde(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._cwde()


class _cwd(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._cwd()


class _leave(baseop):
    def __init__(self, arg):
        pass

    def visit(self, visitor):
        return visitor._leave()


class _enter(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._enter(self.arg)


class _idiv(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._idiv(self.arg)


class _not(baseop):
    def __init__(self, arg):
        self.arg = self.parse_arg(arg)

    def visit(self, visitor):
        return visitor._not(self.arg)


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
