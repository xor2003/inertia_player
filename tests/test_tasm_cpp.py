from __future__ import absolute_import
from __future__ import print_function

from tasm import parser
from tasm import cpp
from tasm import op
from tasm import proc
from tasm import lex

from builtins import chr
from builtins import hex
from builtins import object
from builtins import range
from builtins import str
from copy import copy
import encodings
from encodings import CodecRegistryError
import future.types
from future.types import Integral
import future.types.newobject
from future.types.newobject import newobject
import future.types.newrange
from future.types.newrange import newrange
import future.types.newstr
from future.types.newstr import BaseNewStr
from future.types.newstr import newstr
import logging
from logging import BufferingFormatter
from mock import patch
import ntpath
import re
from re import Scanner
import re, string, os
import sys
import tasm.cpp
from tasm.cpp import cpp
import tasm.lex
import tasm.op
from tasm.op import _aad
from tasm.op import _adc
from tasm.op import _add
from tasm.op import _and
from tasm.op import _assignment
from tasm.op import _call
from tasm.op import _cld
from tasm.op import _cmp
from tasm.op import _cmpsb
from tasm.op import _dec
from tasm.op import _equ
from tasm.op import _inc
from tasm.op import _int
from tasm.op import _ja
from tasm.op import _jb
from tasm.op import _jbe
from tasm.op import _je
from tasm.op import _jmp
from tasm.op import _jna
from tasm.op import _jnc
from tasm.op import _jne
from tasm.op import _jnz
from tasm.op import _jz
from tasm.op import _lea
from tasm.op import _mov
from tasm.op import _movzx
from tasm.op import _pop
from tasm.op import _popad
from tasm.op import _push
from tasm.op import _pushad
from tasm.op import _repe
from tasm.op import _ret
from tasm.op import _retf
from tasm.op import _shr
from tasm.op import _stc
from tasm.op import _sti
from tasm.op import _sub
from tasm.op import _xor
from tasm.op import label
from tasm.op import var
import tasm.parser
from tasm.parser import parser
import tasm.proc
from tasm.proc import proc
import traceback
import traceback, re, string, logging, sys
import unittest


class CppTest(unittest.TestCase):



    @patch.object(logging, 'debug')
    #@patch.object(parser, 'get_global')
    def test_expand(self, mock_debug):
        #mock_get_global.return_value = var()
        mock_debug.return_value = None
        #mock___getattribute__.return_value = <built-in method lower of newstr object at 0x0000000005D43C48>
        #mock_compile.return_value = <_sre.SRE_Pattern object at 0x0000000005B66830>
        #mock_match.return_value = None
        #mock_sub.return_value = u'doublequote+4'
        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='cl ',def_size=0,destination=False),
            u'cl'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'ah',def_size=1,destination=True),
            u'ah'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='0Fh ',def_size=1,destination=False),
            u'0x0F'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='39h ',def_size=1,destination=False),
            u'0x39'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'dl',def_size=1,destination=True),
            u'dl'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='132',def_size=1,destination=False),
            u'132'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='4ch ',def_size=1,destination=False),
            u'0x4c'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=" 'Z' - 'A' +1",def_size=1,destination=False),
            u"'Z' - 'A' +1"
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'edx',def_size=4,destination=True),
            u'edx'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'cl',def_size=1,destination=True),
            u'cl'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'dx',def_size=2,destination=True),
            u'dx'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='var3',def_size=0,destination=False),u'm.var3')

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'eax',def_size=4,destination=True),
            u'eax'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='offset var2',def_size=4,destination=False),u'offset(_data,var2)')

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'esi',def_size=4,destination=True),
            u'esi'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr=u'[doublequote+4]',def_size=1,destination=True),u'*(raddr(ds,offset(_data,doublequote)+4))')

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='al',def_size=1,destination=False),
            u'al'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'bl',def_size=1,destination=True),
            u'bl'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='eax',def_size=0,destination=False),
            u'eax'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='enddata',def_size=0,destination=False),u'offset(_data,enddata)')

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='0',def_size=1,destination=False),
            u'0'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'byte ptr dl',def_size=1,destination=True),
            u'dl'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='9 ',def_size=1,destination=False),
            u'9'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='[edi+1]',def_size=2,destination=False),
            u'*(dw*)(raddr(ds,edi+1))'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr=u'var3',def_size=4,destination=True),u'm.var3')

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='-12',def_size=4,destination=False),
            u'-12'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='-13',def_size=4,destination=False),
            u'-13'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='[edi]',def_size=1,destination=False),
            u'*(raddr(ds,edi))'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='var1',def_size=1,destination=False),u'm.var1')

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='offset var1',def_size=4,destination=False),u'offset(_data,var1)')

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='teST2',def_size=4,destination=False),
            u'teST2'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr="'d'",def_size=1,destination=False),
            u"'d'"
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'ds',def_size=2,destination=True),
            u'ds'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='ebx',def_size=4,destination=False),
            u'ebx'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='OFFSET ASCiI ',def_size=4,destination=False),u'offset(_data,ASCII)')

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='21h ',def_size=0,destination=False),
            u'0x21'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='1',def_size=1,destination=False),
            u'1'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='2',def_size=1,destination=False),
            u'2'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='edi ',def_size=0,destination=False),
            u'edi'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'edi',def_size=4,destination=True),
            u'edi'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='7 ',def_size=1,destination=False),
            u'7'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'byte ptr [edi+7]',def_size=1,destination=True),
            u'*(raddr(ds,edi+7))'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='11',def_size=1,destination=False),
            u'11'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'al',def_size=1,destination=True),
            u'al'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='8 ',def_size=1,destination=False),
            u'8'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='30h ',def_size=1,destination=False),
            u'0x30'
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='byte ptr [edi+7]',def_size=0,destination=False),
            u'*(raddr(ds,edi+7))'
        )


    @patch.object(logging, 'debug')
    #@patch.object(parser, 'get_global')
    def test_get_size(self, mock_debug):
        #mock_get_global.return_value = var()
        mock_debug.return_value = None

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'esi'),
            4
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='offset var1'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='offset var2'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=" 'Z' - 'A' +1"),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='ebx'),
            4
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'[doublequote+4]'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'dl'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='132'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='0Fh '),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='[edi]'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='0'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='2'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'ah'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'edi'),
            4
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='8 '),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='11'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='7 '),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'dx'),
            2
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'21h'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='teST2'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='al'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'cl'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='9 '),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='OFFSET AsCii '),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'edx'),
            4
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'al'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='-12'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='30h '),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'byte ptr dl'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'byte ptr [edi+7]'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr="'d'"),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'bl'),
            1
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.get_size(expr='var1'),1)

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='4ch '),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='1'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='-13'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='39h '),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='[edi+1]'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'ds'),
            2
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'eax'),
            4
        )

    @patch.object(logging, 'debug')
    def test_is_register(self, mock_debug):
        mock_debug.return_value = None
        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'_data'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'offset var1'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'var1'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'cl'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'edx'),
            4
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'[edi+1]'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'[doublequote+4]'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'dl'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'-12'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'teST2'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'var3'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u"'d'"),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'dx'),
            2
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'esi'),
            4
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'enddata'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u"'Z' - 'A' +1"),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'beginningdata'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'al'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'ebx'),
            4
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'offset var2'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'OFFSET ASCiI'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'bl'),
            1
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'OFFSET AsCii'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'[edi]'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'-13'),
            0
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'edi'),
            4
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'ds'),
            2
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'eax'),
            4
        )


    def test_parse2(self):
        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='ebx',dst=u'eax'),
            (u'eax', u'ebx')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src=" 'Z' - 'A' +1",dst=u'al'),
            (u'al', u"'Z' - 'A' +1")
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='11',dst=u'dx'),
            (u'dx', u'11')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='30h ',dst=u'bl'),
            (u'bl', u'0x30')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='1',dst=u'al'),
            (u'al', u'1')
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.parse2(src='-12',dst=u'var3'),(u'm.var3', u'-12'))

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='9 ',dst=u'ah'),
            (u'ah', u'9')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='8 ',dst=u'cl'),
            (u'cl', u'8')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='2',dst=u'dl'),
            (u'dl', u'2')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='0',dst=u'al'),
            (u'al', u'0')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='[edi]',dst=u'byte ptr dl'),
            (u'dl', u'*(raddr(ds,edi))')
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.parse2(src='OFFSET AsCii ',dst=u'edi'),(u'edi', u'offset(_data,ASCII)'))

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.parse2(src='-13',dst=u'var3'),(u'm.var3', u'-13'))

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.parse2(src="'d'",dst=u'[doublequote+4]'),(u'*(raddr(ds,offset(_data,doublequote)+4))', u"'d'"))

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='0Fh ',dst=u'bl'),
            (u'bl', u'0x0F')
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.parse2(src='OFFSET ASCiI ',dst=u'edx'),(u'edx', u'offset(_data,ASCII)'))

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.parse2(src='offset var2',dst=u'esi'),(u'esi', u'offset(_data,var2)'))

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='7 ',dst=u'bl'),
            (u'bl', u'7')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='[edi+1]',dst=u'dx'),
            (u'dx', u'*(dw*)(raddr(ds,edi+1))')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='4ch ',dst=u'ah'),
            (u'ah', u'0x4c')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='39h ',dst=u'bl'),
            (u'bl', u'0x39')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='-13',dst=u'eax'),
            (u'eax', u'-13')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='al',dst=u'bl'),
            (u'bl', u'al')
        )

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='132',dst=u'byte ptr [edi+7]'),
            (u'*(raddr(ds,edi+7))', u'132')
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.parse2(src='var1',dst=u'dl'),(u'dl', u'm.var1'))

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.parse2(src='offset var1',dst=u'edi'),(u'edi', u'offset(_data,var1)'))

        p = parser([])
        cpp_instance = cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='teST2',dst=u'eax'),
            (u'eax', u'teST2')
        )

if __name__ == "__main__":
    unittest.main()
