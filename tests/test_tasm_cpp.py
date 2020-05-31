from __future__ import absolute_import
from __future__ import print_function

from tasm import parser
from tasm import cpp
from mock import patch
from tasm.cpp import Cpp
from tasm.parser import Parser
import logging
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
        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='cl ',def_size=0,destination=False),
            u'cl'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'ah',def_size=1,destination=True),
            u'ah'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='0Fh ',def_size=1,destination=False),
            u'0x0F'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='39h ',def_size=1,destination=False),
            u'0x39'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'dl',def_size=1,destination=True),
            u'dl'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='132',def_size=1,destination=False),
            u'132'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='4ch ',def_size=1,destination=False),
            u'0x4c'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=" 'Z' - 'A' +1",def_size=1,destination=False),
            u"'Z' - 'A' +1"
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'edx',def_size=4,destination=True),
            u'edx'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'cl',def_size=1,destination=True),
            u'cl'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'dx',def_size=2,destination=True),
            u'dx'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='var3',def_size=0,destination=False),u'm.var3')

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'eax',def_size=4,destination=True),
            u'eax'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='offset var2',def_size=4,destination=False),u'offset(_data,var2)')

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'esi',def_size=4,destination=True),
            u'esi'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr=u'[doublequote+4]',def_size=1,destination=True),u'*(raddr(ds,offset(_data,doublequote)+4))')

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='al',def_size=1,destination=False),
            u'al'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'bl',def_size=1,destination=True),
            u'bl'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='eax',def_size=0,destination=False),
            u'eax'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='enddata',def_size=0,destination=False),u'offset(_data,enddata)')

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='0',def_size=1,destination=False),
            u'0'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'byte ptr dl',def_size=1,destination=True),
            u'dl'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='9 ',def_size=1,destination=False),
            u'9'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='[edi+1]',def_size=2,destination=False),
            u'*(dw*)(raddr(ds,edi+1))'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr=u'var3',def_size=4,destination=True),u'm.var3')

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='-12',def_size=4,destination=False),
            u'-12'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='-13',def_size=4,destination=False),
            u'-13'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
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

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='teST2',def_size=4,destination=False),
            u'teST2'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr="'d'",def_size=1,destination=False),
            u"'d'"
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'ds',def_size=2,destination=True),
            u'ds'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='ebx',def_size=4,destination=False),
            u'ebx'
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.expand(expr='OFFSET ASCiI ',def_size=4,destination=False),u'offset(_data,ASCII)')

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='21h ',def_size=0,destination=False),
            u'0x21'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='1',def_size=1,destination=False),
            u'1'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='2',def_size=1,destination=False),
            u'2'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='edi ',def_size=0,destination=False),
            u'edi'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'edi',def_size=4,destination=True),
            u'edi'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='7 ',def_size=1,destination=False),
            u'7'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'byte ptr [edi+7]',def_size=1,destination=True),
            u'*(raddr(ds,edi+7))'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='11',def_size=1,destination=False),
            u'11'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr=u'al',def_size=1,destination=True),
            u'al'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='8 ',def_size=1,destination=False),
            u'8'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='30h ',def_size=1,destination=False),
            u'0x30'
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.expand(expr='byte ptr [edi+7]',def_size=0,destination=False),
            u'*(raddr(ds,edi+7))'
        )


    @patch.object(logging, 'debug')
    #@patch.object(parser, 'get_global')
    def test_get_size(self, mock_debug):
        #mock_get_global.return_value = var()
        mock_debug.return_value = None

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'esi'),
            4
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='offset var1'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='offset var2'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=" 'Z' - 'A' +1"),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='ebx'),
            4
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'[doublequote+4]'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'dl'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='132'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='0Fh '),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='[edi]'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='0'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='2'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'ah'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'edi'),
            4
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='8 '),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='11'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='7 '),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'dx'),
            2
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'21h'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='teST2'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='al'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'cl'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='9 '),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='OFFSET AsCii '),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'edx'),
            4
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'al'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='-12'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='30h '),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'byte ptr dl'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'byte ptr [edi+7]'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr="'d'"),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'bl'),
            1
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.get_size(expr='var1'),1)

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='4ch '),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='1'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='-13'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='39h '),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr='[edi+1]'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'ds'),
            2
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.get_size(expr=u'eax'),
            4
        )

    @patch.object(logging, 'debug')
    def test_is_register(self, mock_debug):
        mock_debug.return_value = None
        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'_data'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'offset var1'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'var1'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'cl'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'edx'),
            4
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'[edi+1]'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'[doublequote+4]'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'dl'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'-12'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'teST2'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'var3'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u"'d'"),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'dx'),
            2
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'esi'),
            4
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'enddata'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u"'Z' - 'A' +1"),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'beginningdata'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'al'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'ebx'),
            4
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'offset var2'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'OFFSET ASCiI'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'bl'),
            1
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'OFFSET AsCii'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'[edi]'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'-13'),
            0
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'edi'),
            4
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'ds'),
            2
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.is_register(expr=u'eax'),
            4
        )


    def test_parse2(self):
        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='ebx',dst=u'eax'),
            (u'eax', u'ebx')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src=" 'Z' - 'A' +1",dst=u'al'),
            (u'al', u"'Z' - 'A' +1")
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='11',dst=u'dx'),
            (u'dx', u'11')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='30h ',dst=u'bl'),
            (u'bl', u'0x30')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='1',dst=u'al'),
            (u'al', u'1')
        )

        #p = parser([])
        #cpp_instance = cpp(p)
        #self.assertEqual(cpp_instance.parse2(src='-12',dst=u'var3'),(u'm.var3', u'-12'))

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='9 ',dst=u'ah'),
            (u'ah', u'9')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='8 ',dst=u'cl'),
            (u'cl', u'8')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='2',dst=u'dl'),
            (u'dl', u'2')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='0',dst=u'al'),
            (u'al', u'0')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
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

        p = Parser([])
        cpp_instance = Cpp(p)
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

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='7 ',dst=u'bl'),
            (u'bl', u'7')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='[edi+1]',dst=u'dx'),
            (u'dx', u'*(dw*)(raddr(ds,edi+1))')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='4ch ',dst=u'ah'),
            (u'ah', u'0x4c')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='39h ',dst=u'bl'),
            (u'bl', u'0x39')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='-13',dst=u'eax'),
            (u'eax', u'-13')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='al',dst=u'bl'),
            (u'bl', u'al')
        )

        p = Parser([])
        cpp_instance = Cpp(p)
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

        p = Parser([])
        cpp_instance = Cpp(p)
        self.assertEqual(
            cpp_instance.parse2(src='teST2',dst=u'eax'),
            (u'eax', u'teST2')
        )

    def test_convert_number(self):
        self.assertEqual(cpp.convert_number(expr=u"'Z' - 'A' +1"),u"'Z' - 'A' +1")
        self.assertEqual(cpp.convert_number(expr=u'((((2030080+64000*26)/4096)+1)*4096)-1'),u'((((2030080+64000*26)/4096)+1)*4096)-1')
        self.assertEqual(cpp.convert_number(expr=u'(00+38*3)*320+1/2+33*(3-1)'),u'(00+38*3)*320+1/2+33*(3-1)')
        self.assertEqual(cpp.convert_number(expr=u'(1024*10/16)+5'),u'(1024*10/16)+5')
        self.assertEqual(cpp.convert_number(expr=u'(1024*10/16)-1'),u'(1024*10/16)-1')
        self.assertEqual(cpp.convert_number(expr=u'+0x40'),u'+0x40')
        self.assertEqual(cpp.convert_number(expr=u'+0x4000'),u'+0x4000')
        self.assertEqual(cpp.convert_number(expr=u'+ecx'),u'+ecx')
        self.assertEqual(cpp.convert_number(expr=u'+ecx*2'),u'+ecx*2')
        self.assertEqual(cpp.convert_number(expr=u'+ecx*2+0x4000'),u'+ecx*2+0x4000')
        self.assertEqual(cpp.convert_number(expr=u'+ecx*2-0x0A'),u'+ecx*2-0x0A')
        self.assertEqual(cpp.convert_number(expr=u'+ecx*4'),u'+ecx*4')
        self.assertEqual(cpp.convert_number(expr=u'+ecx*4+0x4000'),u'+ecx*4+0x4000')
        self.assertEqual(cpp.convert_number(expr=u'+ecx*4-0x0A'),u'+ecx*4-0x0A')
        self.assertEqual(cpp.convert_number(expr=u'+ecx+0x40'),u'+ecx+0x40')
        self.assertEqual(cpp.convert_number(expr=u'+edx'),u'+edx')
        self.assertEqual(cpp.convert_number(expr=u'+edx+0x4000'),u'+edx+0x4000')
        self.assertEqual(cpp.convert_number(expr=u'-0x108'),u'-0x108')
        self.assertEqual(cpp.convert_number(expr=u'-0x1C'),u'-0x1C')
        self.assertEqual(cpp.convert_number(expr=u'-0x20'),u'-0x20')
        self.assertEqual(cpp.convert_number(expr=u'-0x28'),u'-0x28')
        self.assertEqual(cpp.convert_number(expr=u'-0x2C'),u'-0x2C')
        self.assertEqual(cpp.convert_number(expr=u'-1'),u'-1')
        self.assertEqual(cpp.convert_number(expr=u'-1-(-2+3)'),u'-1-(-2+3)')
        self.assertEqual(cpp.convert_number(expr=u'-108h'),u'-0x108')
        self.assertEqual(cpp.convert_number(expr=u'-12'),u'-12')
        self.assertEqual(cpp.convert_number(expr=u'-13'),u'-13')
        self.assertEqual(cpp.convert_number(expr=u'-1Ch'),u'-0x1C')
        self.assertEqual(cpp.convert_number(expr=u'-2'),u'-2')
        self.assertEqual(cpp.convert_number(expr=u'-20h'),u'-0x20')
        self.assertEqual(cpp.convert_number(expr=u'-28h'),u'-0x28')
        self.assertEqual(cpp.convert_number(expr=u'-2Ch'),u'-0x2C')
        self.assertEqual(cpp.convert_number(expr=u'-2Dh'),u'-0x2D')
        self.assertEqual(cpp.convert_number(expr=u'-4'),u'-4')
        self.assertEqual(cpp.convert_number(expr=u'-5'),u'-5')
        self.assertEqual(cpp.convert_number(expr=u'-8'),u'-8')
        self.assertEqual(cpp.convert_number(expr=u'0'),u'0')
        self.assertEqual(cpp.convert_number(expr=u'0002h'),u'0x0002')
        self.assertEqual(cpp.convert_number(expr=u'0007'),u'0007')
        self.assertEqual(cpp.convert_number(expr=u'000f3h'),u'0x000f3')
        self.assertEqual(cpp.convert_number(expr=u'000ff00ffh'),u'0x000ff00ff')
        self.assertEqual(cpp.convert_number(expr=u'001111111B'),u'0x7f')
        self.assertEqual(cpp.convert_number(expr=u'00fffh'),u'0x00fff')
        self.assertEqual(cpp.convert_number(expr=u'00h'),u'0x00')
        self.assertEqual(cpp.convert_number(expr=u'0100b'),u'0x4')
        self.assertEqual(cpp.convert_number(expr=u'01010101010101010b'),u'0xaaaa')
        self.assertEqual(cpp.convert_number(expr=u'0101010101010101b'),u'0x5555')
        self.assertEqual(cpp.convert_number(expr=u'0101b'),u'0x5')
        self.assertEqual(cpp.convert_number(expr=u'010B'),u'0x2')
        self.assertEqual(cpp.convert_number(expr=u'011111100B'),u'0xfc')
        self.assertEqual(cpp.convert_number(expr=u'011111111111111111111111111111111b'),u'0xffffffff')
        self.assertEqual(cpp.convert_number(expr=u'01111111111111111b'),u'0xffff')
        self.assertEqual(cpp.convert_number(expr=u'011111111B'),u'0xff')
        self.assertEqual(cpp.convert_number(expr=u'012345678h'),u'0x012345678')
        self.assertEqual(cpp.convert_number(expr=u'01B'),u'0x1')
        self.assertEqual(cpp.convert_number(expr=u'01h'),u'0x01')
        self.assertEqual(cpp.convert_number(expr=u'02h'),u'0x02')
        self.assertEqual(cpp.convert_number(expr=u'03dh'),u'0x03d')
        self.assertEqual(cpp.convert_number(expr=u'03eh'),u'0x03e')
        self.assertEqual(cpp.convert_number(expr=u'03fh'),u'0x03f')
        self.assertEqual(cpp.convert_number(expr=u'042h'),u'0x042')
        self.assertEqual(cpp.convert_number(expr=u'077123456h'),u'0x077123456')
        self.assertEqual(cpp.convert_number(expr=u'077aaFF00h'),u'0x077aaFF00')
        self.assertEqual(cpp.convert_number(expr=u'08h'),u'0x08')
        self.assertEqual(cpp.convert_number(expr=u'0B'),u'0x0')
        self.assertEqual(cpp.convert_number(expr=u'0BC6058h'),u'0x0BC6058')
        self.assertEqual(cpp.convert_number(expr=u'0Ch'),u'0x0C')
        self.assertEqual(cpp.convert_number(expr=u'0D5h'),u'0x0D5')
        self.assertEqual(cpp.convert_number(expr=u'0Eh'),u'0x0E')
        self.assertEqual(cpp.convert_number(expr=u'0F7h'),u'0x0F7')
        self.assertEqual(cpp.convert_number(expr=u'0FBCA7654h'),u'0x0FBCA7654')
        self.assertEqual(cpp.convert_number(expr=u'0FBCA7h'),u'0x0FBCA7')
        self.assertEqual(cpp.convert_number(expr=u'0FEh'),u'0x0FE')
        self.assertEqual(cpp.convert_number(expr=u'0FFEh'),u'0x0FFE')
        self.assertEqual(cpp.convert_number(expr=u'0FFFC70F9h'),u'0x0FFFC70F9')
        self.assertEqual(cpp.convert_number(expr=u'0FFFE0080h'),u'0x0FFFE0080')
        self.assertEqual(cpp.convert_number(expr=u'0FFFEDCBFh'),u'0x0FFFEDCBF')
        self.assertEqual(cpp.convert_number(expr=u'0FFFEFDFCh'),u'0x0FFFEFDFC')
        self.assertEqual(cpp.convert_number(expr=u'0FFFEh'),u'0x0FFFE')
        self.assertEqual(cpp.convert_number(expr=u'0FFFF7FFFh'),u'0x0FFFF7FFF')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFA549h'),u'0x0FFFFA549')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFEh'),u'0x0FFFFE')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFED4h'),u'0x0FFFFFED4')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFEh'),u'0x0FFFFFE')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFD3h'),u'0x0FFFFFFD3')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFECh'),u'0x0FFFFFFEC')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFEh'),u'0x0FFFFFFE')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFF0h'),u'0x0FFFFFFF0')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFF7h'),u'0x0FFFFFFF7')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFFAh'),u'0x0FFFFFFFA')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFFBh'),u'0x0FFFFFFFB')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFFCh'),u'0x0FFFFFFFC')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFFDh'),u'0x0FFFFFFFD')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFFEh'),u'0x0FFFFFFFE')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFFFh'),u'0x0FFFFFFFF')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFFh'),u'0x0FFFFFFF')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFFh'),u'0x0FFFFFF')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFFh'),u'0x0FFFFF')
        self.assertEqual(cpp.convert_number(expr=u'0FFFFh'),u'0x0FFFF')
        self.assertEqual(cpp.convert_number(expr=u'0FFFh'),u'0x0FFF')
        self.assertEqual(cpp.convert_number(expr=u'0FFh'),u'0x0FF')
        self.assertEqual(cpp.convert_number(expr=u'0Fh'),u'0x0F')
        self.assertEqual(cpp.convert_number(expr=u'0a0000h'),u'0x0a0000')
        self.assertEqual(cpp.convert_number(expr=u'0a000h'),u'0x0a000')
        self.assertEqual(cpp.convert_number(expr=u'0aabbccddh'),u'0x0aabbccdd')
        self.assertEqual(cpp.convert_number(expr=u'0abcdef77h'),u'0x0abcdef77')
        self.assertEqual(cpp.convert_number(expr=u'0af222h'),u'0x0af222')
        self.assertEqual(cpp.convert_number(expr=u'0cch'),u'0x0cc')
        self.assertEqual(cpp.convert_number(expr=u'0ddh'),u'0x0dd')
        self.assertEqual(cpp.convert_number(expr=u'0df01h'),u'0x0df01')
        self.assertEqual(cpp.convert_number(expr=u'0dff1h'),u'0x0dff1')
        self.assertEqual(cpp.convert_number(expr=u'0f0ffh'),u'0x0f0ff')
        self.assertEqual(cpp.convert_number(expr=u'0f0h'),u'0x0f0')
        self.assertEqual(cpp.convert_number(expr=u'0f222h'),u'0x0f222')
        self.assertEqual(cpp.convert_number(expr=u'0ffff0003h'),u'0x0ffff0003')
        self.assertEqual(cpp.convert_number(expr=u'0ffff00f3h'),u'0x0ffff00f3')
        self.assertEqual(cpp.convert_number(expr=u'0ffff01ffh'),u'0x0ffff01ff')
        self.assertEqual(cpp.convert_number(expr=u'0ffffff00h'),u'0x0ffffff00')
        self.assertEqual(cpp.convert_number(expr=u'0ffffff03h'),u'0x0ffffff03')
        self.assertEqual(cpp.convert_number(expr=u'0fffffff3h'),u'0x0fffffff3')
        self.assertEqual(cpp.convert_number(expr=u'0ffffffffh'),u'0x0ffffffff')
        self.assertEqual(cpp.convert_number(expr=u'0ffffh'),u'0x0ffff')
        self.assertEqual(cpp.convert_number(expr=u'0ffh'),u'0x0ff')
        self.assertEqual(cpp.convert_number(expr=u'0x0C'),u'0x0C')
        self.assertEqual(cpp.convert_number(expr=u'0x10'),u'0x10')
        self.assertEqual(cpp.convert_number(expr=u'0x14'),u'0x14')
        self.assertEqual(cpp.convert_number(expr=u'1'),u'1')
        self.assertEqual(cpp.convert_number(expr=u'10'),u'10')
        self.assertEqual(cpp.convert_number(expr=u'10000h'),u'0x10000')
        self.assertEqual(cpp.convert_number(expr=u'1000h'),u'0x1000')
        self.assertEqual(cpp.convert_number(expr=u'100h'),u'0x100')
        self.assertEqual(cpp.convert_number(expr=u'1024*10/16'),u'1024*10/16')
        self.assertEqual(cpp.convert_number(expr=u'1024*1024'),u'1024*1024')
        self.assertEqual(cpp.convert_number(expr=u'10B'),u'0x2')
        self.assertEqual(cpp.convert_number(expr=u'10h'),u'0x10')
        self.assertEqual(cpp.convert_number(expr=u'11'),u'11')
        self.assertEqual(cpp.convert_number(expr=u'111'),u'111')
        self.assertEqual(cpp.convert_number(expr=u'114h'),u'0x114')
        self.assertEqual(cpp.convert_number(expr=u'11h'),u'0x11')
        self.assertEqual(cpp.convert_number(expr=u'12'),u'12')
        self.assertEqual(cpp.convert_number(expr=u'12340004h'),u'0x12340004')
        self.assertEqual(cpp.convert_number(expr=u'1234001Dh'),u'0x1234001D')
        self.assertEqual(cpp.convert_number(expr=u'12340128h'),u'0x12340128')
        self.assertEqual(cpp.convert_number(expr=u'12340205h'),u'0x12340205')
        self.assertEqual(cpp.convert_number(expr=u'12340306h'),u'0x12340306')
        self.assertEqual(cpp.convert_number(expr=u'12340407h'),u'0x12340407')
        self.assertEqual(cpp.convert_number(expr=u'1234040Ah'),u'0x1234040A')
        self.assertEqual(cpp.convert_number(expr=u'12340503h'),u'0x12340503')
        self.assertEqual(cpp.convert_number(expr=u'12340506h'),u'0x12340506')
        self.assertEqual(cpp.convert_number(expr=u'12340507h'),u'0x12340507')
        self.assertEqual(cpp.convert_number(expr=u'12340547h'),u'0x12340547')
        self.assertEqual(cpp.convert_number(expr=u'12340559h'),u'0x12340559')
        self.assertEqual(cpp.convert_number(expr=u'12340560h'),u'0x12340560')
        self.assertEqual(cpp.convert_number(expr=u'1234059Fh'),u'0x1234059F')
        self.assertEqual(cpp.convert_number(expr=u'123405A0h'),u'0x123405A0')
        self.assertEqual(cpp.convert_number(expr=u'123405FAh'),u'0x123405FA')
        self.assertEqual(cpp.convert_number(expr=u'12341678h'),u'0x12341678')
        self.assertEqual(cpp.convert_number(expr=u'12341h'),u'0x12341')
        self.assertEqual(cpp.convert_number(expr=u'12343h'),u'0x12343')
        self.assertEqual(cpp.convert_number(expr=u'12345'),u'12345')
        self.assertEqual(cpp.convert_number(expr=u'1234561Dh'),u'0x1234561D')
        self.assertEqual(cpp.convert_number(expr=u'12345678h'),u'0x12345678')
        self.assertEqual(cpp.convert_number(expr=u'12345h'),u'0x12345')
        self.assertEqual(cpp.convert_number(expr=u'12347F7Fh'),u'0x12347F7F')
        self.assertEqual(cpp.convert_number(expr=u'12347FFFh'),u'0x12347FFF')
        self.assertEqual(cpp.convert_number(expr=u'12348000h'),u'0x12348000')
        self.assertEqual(cpp.convert_number(expr=u'12348080h'),u'0x12348080')
        self.assertEqual(cpp.convert_number(expr=u'1234h'),u'0x1234')
        self.assertEqual(cpp.convert_number(expr=u'127Eh'),u'0x127E')
        self.assertEqual(cpp.convert_number(expr=u'12Ch'),u'0x12C')
        self.assertEqual(cpp.convert_number(expr=u'13'),u'13')
        self.assertEqual(cpp.convert_number(expr=u'132'),u'132')
        self.assertEqual(cpp.convert_number(expr=u'133'),u'133')
        self.assertEqual(cpp.convert_number(expr=u'13h'),u'0x13')
        self.assertEqual(cpp.convert_number(expr=u'14'),u'14')
        self.assertEqual(cpp.convert_number(expr=u'14*320'),u'14*320')
        self.assertEqual(cpp.convert_number(expr=u'14h'),u'0x14')
        self.assertEqual(cpp.convert_number(expr=u'15'),u'15')
        self.assertEqual(cpp.convert_number(expr=u'1500'),u'1500')
        self.assertEqual(cpp.convert_number(expr=u'16'),u'16')
        self.assertEqual(cpp.convert_number(expr=u'17'),u'17')
        self.assertEqual(cpp.convert_number(expr=u'17h'),u'0x17')
        self.assertEqual(cpp.convert_number(expr=u'18'),u'18')
        self.assertEqual(cpp.convert_number(expr=u'18h'),u'0x18')
        self.assertEqual(cpp.convert_number(expr=u'19'),u'19')
        self.assertEqual(cpp.convert_number(expr=u'192'),u'192')
        self.assertEqual(cpp.convert_number(expr=u'193'),u'193')
        self.assertEqual(cpp.convert_number(expr=u'1Ch'),u'0x1C')
        self.assertEqual(cpp.convert_number(expr=u'1Eh'),u'0x1E')
        self.assertEqual(cpp.convert_number(expr=u'1FEh'),u'0x1FE')
        self.assertEqual(cpp.convert_number(expr=u'1FF7Fh'),u'0x1FF7F')
        self.assertEqual(cpp.convert_number(expr=u'1FF80h'),u'0x1FF80')
        self.assertEqual(cpp.convert_number(expr=u'1FF81h'),u'0x1FF81')
        self.assertEqual(cpp.convert_number(expr=u'1FFEh'),u'0x1FFE')
        self.assertEqual(cpp.convert_number(expr=u'1FFFEh'),u'0x1FFFE')
        self.assertEqual(cpp.convert_number(expr=u'1FFFFEh'),u'0x1FFFFE')
        self.assertEqual(cpp.convert_number(expr=u'1FFFFFEh'),u'0x1FFFFFE')
        self.assertEqual(cpp.convert_number(expr=u'1FFFFFFEh'),u'0x1FFFFFFE')
        self.assertEqual(cpp.convert_number(expr=u'1FFFFFFFh'),u'0x1FFFFFFF')
        self.assertEqual(cpp.convert_number(expr=u'1FFFFFFh'),u'0x1FFFFFF')
        self.assertEqual(cpp.convert_number(expr=u'1FFFFFh'),u'0x1FFFFF')
        self.assertEqual(cpp.convert_number(expr=u'1FFFFh'),u'0x1FFFF')
        self.assertEqual(cpp.convert_number(expr=u'1FFFh'),u'0x1FFF')
        self.assertEqual(cpp.convert_number(expr=u'1FFh'),u'0x1FF')
        self.assertEqual(cpp.convert_number(expr=u'1Fh'),u'0x1F')
        self.assertEqual(cpp.convert_number(expr=u'2'),u'2')
        self.assertEqual(cpp.convert_number(expr=u'20'),u'20')
        self.assertEqual(cpp.convert_number(expr=u'20000h'),u'0x20000')
        self.assertEqual(cpp.convert_number(expr=u'20h'),u'0x20')
        self.assertEqual(cpp.convert_number(expr=u'21'),u'21')
        self.assertEqual(cpp.convert_number(expr=u'21AD3D34h'),u'0x21AD3D34')
        self.assertEqual(cpp.convert_number(expr=u'21h'),u'0x21')
        self.assertEqual(cpp.convert_number(expr=u'22'),u'22')
        self.assertEqual(cpp.convert_number(expr=u'23'),u'23')
        self.assertEqual(cpp.convert_number(expr=u'24'),u'24')
        self.assertEqual(cpp.convert_number(expr=u'24h'),u'0x24')
        self.assertEqual(cpp.convert_number(expr=u'25'),u'25')
        self.assertEqual(cpp.convert_number(expr=u'255'),u'255')
        self.assertEqual(cpp.convert_number(expr=u'256'),u'256')
        self.assertEqual(cpp.convert_number(expr=u'256*3'),u'256*3')
        self.assertEqual(cpp.convert_number(expr=u'256+3'),u'256+3')
        self.assertEqual(cpp.convert_number(expr=u'256+3+65536'),u'256+3+65536')
        self.assertEqual(cpp.convert_number(expr=u'26'),u'26')
        self.assertEqual(cpp.convert_number(expr=u'27'),u'27')
        self.assertEqual(cpp.convert_number(expr=u'28'),u'28')
        self.assertEqual(cpp.convert_number(expr=u'29'),u'29')
        self.assertEqual(cpp.convert_number(expr=u'2Ch'),u'0x2C')
        self.assertEqual(cpp.convert_number(expr=u'2Dh'),u'0x2D')
        self.assertEqual(cpp.convert_number(expr=u'3'),u'3')
        self.assertEqual(cpp.convert_number(expr=u'3*4'),u'3*4')
        self.assertEqual(cpp.convert_number(expr=u'30'),u'30')
        self.assertEqual(cpp.convert_number(expr=u'303Bh'),u'0x303B')
        self.assertEqual(cpp.convert_number(expr=u'30h'),u'0x30')
        self.assertEqual(cpp.convert_number(expr=u'31'),u'31')
        self.assertEqual(cpp.convert_number(expr=u'31h'),u'0x31')
        self.assertEqual(cpp.convert_number(expr=u'32'),u'32')
        self.assertEqual(cpp.convert_number(expr=u'320*200/4'),u'320*200/4')
        self.assertEqual(cpp.convert_number(expr=u'32432434h'),u'0x32432434')
        self.assertEqual(cpp.convert_number(expr=u'340128h'),u'0x340128')
        self.assertEqual(cpp.convert_number(expr=u'35'),u'35')
        self.assertEqual(cpp.convert_number(expr=u'37'),u'37')
        self.assertEqual(cpp.convert_number(expr=u'39h'),u'0x39')
        self.assertEqual(cpp.convert_number(expr=u'3Ch'),u'0x3C')
        self.assertEqual(cpp.convert_number(expr=u'3DAh'),u'0x3DA')
        self.assertEqual(cpp.convert_number(expr=u'3Eh'),u'0x3E')
        self.assertEqual(cpp.convert_number(expr=u'3FEh'),u'0x3FE')
        self.assertEqual(cpp.convert_number(expr=u'3FFEh'),u'0x3FFE')
        self.assertEqual(cpp.convert_number(expr=u'3FFFEh'),u'0x3FFFE')
        self.assertEqual(cpp.convert_number(expr=u'3FFFFEh'),u'0x3FFFFE')
        self.assertEqual(cpp.convert_number(expr=u'3FFFFFEh'),u'0x3FFFFFE')
        self.assertEqual(cpp.convert_number(expr=u'3FFFFFFEh'),u'0x3FFFFFFE')
        self.assertEqual(cpp.convert_number(expr=u'3FFFFFFFh'),u'0x3FFFFFFF')
        self.assertEqual(cpp.convert_number(expr=u'3FFFFFFh'),u'0x3FFFFFF')
        self.assertEqual(cpp.convert_number(expr=u'3FFFFFh'),u'0x3FFFFF')
        self.assertEqual(cpp.convert_number(expr=u'3FFFFh'),u'0x3FFFF')
        self.assertEqual(cpp.convert_number(expr=u'3FFFh'),u'0x3FFF')
        self.assertEqual(cpp.convert_number(expr=u'3FFh'),u'0x3FF')
        self.assertEqual(cpp.convert_number(expr=u'3Fh'),u'0x3F')
        self.assertEqual(cpp.convert_number(expr=u'3c8h'),u'0x3c8')
        self.assertEqual(cpp.convert_number(expr=u'3c9h'),u'0x3c9')
        self.assertEqual(cpp.convert_number(expr=u'3h'),u'0x3')
        self.assertEqual(cpp.convert_number(expr=u'4'),u'4')
        self.assertEqual(cpp.convert_number(expr=u'4+5*256'),u'4+5*256')
        self.assertEqual(cpp.convert_number(expr=u'4000000'),u'4000000')
        self.assertEqual(cpp.convert_number(expr=u'40h'),u'0x40')
        self.assertEqual(cpp.convert_number(expr=u'43210123h'),u'0x43210123')
        self.assertEqual(cpp.convert_number(expr=u'48h'),u'0x48')
        self.assertEqual(cpp.convert_number(expr=u'49h'),u'0x49')
        self.assertEqual(cpp.convert_number(expr=u'4Ah'),u'0x4A')
        self.assertEqual(cpp.convert_number(expr=u'4Ch'),u'0x4C')
        self.assertEqual(cpp.convert_number(expr=u'4ch'),u'0x4c')
        self.assertEqual(cpp.convert_number(expr=u'5'),u'5')
        self.assertEqual(cpp.convert_number(expr=u'50'),u'50')
        self.assertEqual(cpp.convert_number(expr=u'501h'),u'0x501')
        self.assertEqual(cpp.convert_number(expr=u'511'),u'511')
        self.assertEqual(cpp.convert_number(expr=u'55'),u'55')
        self.assertEqual(cpp.convert_number(expr=u'56'),u'56')
        self.assertEqual(cpp.convert_number(expr=u'57'),u'57')
        self.assertEqual(cpp.convert_number(expr=u'6'),u'6')
        self.assertEqual(cpp.convert_number(expr=u'6*256+5'),u'6*256+5')
        self.assertEqual(cpp.convert_number(expr=u'60'),u'60')
        self.assertEqual(cpp.convert_number(expr=u'65324h'),u'0x65324')
        self.assertEqual(cpp.convert_number(expr=u'65423456h'),u'0x65423456')
        self.assertEqual(cpp.convert_number(expr=u'6789ABCDh'),u'0x6789ABCD')
        self.assertEqual(cpp.convert_number(expr=u'7'),u'7')
        self.assertEqual(cpp.convert_number(expr=u'7Eh'),u'0x7E')
        self.assertEqual(cpp.convert_number(expr=u'7FEh'),u'0x7FE')
        self.assertEqual(cpp.convert_number(expr=u'7FFEh'),u'0x7FFE')
        self.assertEqual(cpp.convert_number(expr=u'7FFFEh'),u'0x7FFFE')
        self.assertEqual(cpp.convert_number(expr=u'7FFFFEh'),u'0x7FFFFE')
        self.assertEqual(cpp.convert_number(expr=u'7FFFFFEh'),u'0x7FFFFFE')
        self.assertEqual(cpp.convert_number(expr=u'7FFFFFFEh'),u'0x7FFFFFFE')
        self.assertEqual(cpp.convert_number(expr=u'7FFFFFFFh'),u'0x7FFFFFFF')
        self.assertEqual(cpp.convert_number(expr=u'7FFFFFFh'),u'0x7FFFFFF')
        self.assertEqual(cpp.convert_number(expr=u'7FFFFFh'),u'0x7FFFFF')
        self.assertEqual(cpp.convert_number(expr=u'7FFFFh'),u'0x7FFFF')
        self.assertEqual(cpp.convert_number(expr=u'7FFFh'),u'0x7FFF')
        self.assertEqual(cpp.convert_number(expr=u'7FFh'),u'0x7FF')
        self.assertEqual(cpp.convert_number(expr=u'7Fh'),u'0x7F')
        self.assertEqual(cpp.convert_number(expr=u'8'),u'8')
        self.assertEqual(cpp.convert_number(expr=u'80000000h'),u'0x80000000')
        self.assertEqual(cpp.convert_number(expr=u'80000001h'),u'0x80000001')
        self.assertEqual(cpp.convert_number(expr=u'80008481h'),u'0x80008481')
        self.assertEqual(cpp.convert_number(expr=u'80008688h'),u'0x80008688')
        self.assertEqual(cpp.convert_number(expr=u'8000h'),u'0x8000')
        self.assertEqual(cpp.convert_number(expr=u'801h'),u'0x801')
        self.assertEqual(cpp.convert_number(expr=u'80h'),u'0x80')
        self.assertEqual(cpp.convert_number(expr=u'81234567h'),u'0x81234567')
        self.assertEqual(cpp.convert_number(expr=u'81238567h'),u'0x81238567')
        self.assertEqual(cpp.convert_number(expr=u'812FADAh'),u'0x812FADA')
        self.assertEqual(cpp.convert_number(expr=u'813F3421h'),u'0x813F3421')
        self.assertEqual(cpp.convert_number(expr=u'81h'),u'0x81')
        self.assertEqual(cpp.convert_number(expr=u'82345679h'),u'0x82345679')
        self.assertEqual(cpp.convert_number(expr=u'8234A6F8h'),u'0x8234A6F8')
        self.assertEqual(cpp.convert_number(expr=u'8345A1F2h'),u'0x8345A1F2')
        self.assertEqual(cpp.convert_number(expr=u'8C5h'),u'0x8C5')
        self.assertEqual(cpp.convert_number(expr=u'8D5h'),u'0x8D5')
        self.assertEqual(cpp.convert_number(expr=u'9'),u'9')
        self.assertEqual(cpp.convert_number(expr=u'9ABCDEFh'),u'0x9ABCDEF')
        self.assertEqual(cpp.convert_number(expr=u'AL'),u'AL')
        self.assertEqual(cpp.convert_number(expr=u'B'),u'B')
        self.assertEqual(cpp.convert_number(expr=u'CC'),u'CC')
        self.assertEqual(cpp.convert_number(expr=u'DDD'),u'DDD')
        self.assertEqual(cpp.convert_number(expr=u'DX'),u'DX')
        self.assertEqual(cpp.convert_number(expr=u'OFFSET ASCiI'),u'OFFSET ASCiI')
        self.assertEqual(cpp.convert_number(expr=u'OFFSET AsCii'),u'OFFSET AsCii')
        self.assertEqual(cpp.convert_number(expr=u'TWO'),u'TWO')
        self.assertEqual(cpp.convert_number(expr=u'[a+1]'),u'[a+1]')
        self.assertEqual(cpp.convert_number(expr=u'[a]'),u'[a]')
        self.assertEqual(cpp.convert_number(expr=u'[cs:table+ax]'),u'[cs:table+ax]')
        self.assertEqual(cpp.convert_number(expr=u'[doublequote+4]'),u'[doublequote+4]')
        self.assertEqual(cpp.convert_number(expr=u'[eax+4000h]'),u'[eax+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[eax+40h]'),u'[eax+0x40]')
        self.assertEqual(cpp.convert_number(expr=u'[eax+ecx+40h]'),u'[eax+ecx+0x40]')
        self.assertEqual(cpp.convert_number(expr=u'[eax+ecx]'),u'[eax+ecx]')
        self.assertEqual(cpp.convert_number(expr=u'[eax]'),u'[eax]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+ecx_0]'),u'[ebp+ecx_0]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+ecx_vals]'),u'[ebp+ecx_vals]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+edx_0]'),u'[ebp+edx_0]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+i*4+ecx_vals]'),u'[ebp+i*4+ecx_vals]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+i+table]'),u'[ebp+i+table]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+iflags]'),u'[ebp+iflags]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+op0]'),u'[ebp+op0]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+op0h]'),u'[ebp+op0h]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+s0]'),u'[ebp+s0]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+s1]'),u'[ebp+s1]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+s2]'),u'[ebp+s2]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+table]'),u'[ebp+table]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+var_1C]'),u'[ebp+var_1C]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+var_20]'),u'[ebp+var_20]')
        self.assertEqual(cpp.convert_number(expr=u'[ebp+var_4]'),u'[ebp+var_4]')
        self.assertEqual(cpp.convert_number(expr=u'[ebx+4000h]'),u'[ebx+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[ebx+40h]'),u'[ebx+0x40]')
        self.assertEqual(cpp.convert_number(expr=u'[ebx+edx+4000h]'),u'[ebx+edx+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[ebx+edx]'),u'[ebx+edx]')
        self.assertEqual(cpp.convert_number(expr=u'[ebx]'),u'[ebx]')
        self.assertEqual(cpp.convert_number(expr=u'[ecx+4000h]'),u'[ecx+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[ecx+40h]'),u'[ecx+0x40]')
        self.assertEqual(cpp.convert_number(expr=u'[ecx+ecx*2+4000h]'),u'[ecx+ecx*2+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[ecx+ecx*2-0Ah]'),u'[ecx+ecx*2-0x0A]')
        self.assertEqual(cpp.convert_number(expr=u'[ecx+ecx*2]'),u'[ecx+ecx*2]')
        self.assertEqual(cpp.convert_number(expr=u'[ecx+ecx]'),u'[ecx+ecx]')
        self.assertEqual(cpp.convert_number(expr=u'[ecx]'),u'[ecx]')
        self.assertEqual(cpp.convert_number(expr=u'[edi+1]'),u'[edi+1]')
        self.assertEqual(cpp.convert_number(expr=u'[edi+4000h]'),u'[edi+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[edi+40h]'),u'[edi+0x40]')
        self.assertEqual(cpp.convert_number(expr=u'[edi+ecx]'),u'[edi+ecx]')
        self.assertEqual(cpp.convert_number(expr=u'[edi]'),u'[edi]')
        self.assertEqual(cpp.convert_number(expr=u'[edx+4000h]'),u'[edx+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[edx+40h]'),u'[edx+0x40]')
        self.assertEqual(cpp.convert_number(expr=u'[edx+ecx*4+4000h]'),u'[edx+ecx*4+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[edx+ecx*4-0Ah]'),u'[edx+ecx*4-0x0A]')
        self.assertEqual(cpp.convert_number(expr=u'[edx+ecx*4]'),u'[edx+ecx*4]')
        self.assertEqual(cpp.convert_number(expr=u'[edx+ecx]'),u'[edx+ecx]')
        self.assertEqual(cpp.convert_number(expr=u'[edx]'),u'[edx]')
        self.assertEqual(cpp.convert_number(expr=u'[esi+4000h]'),u'[esi+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[esi+40h]'),u'[esi+0x40]')
        self.assertEqual(cpp.convert_number(expr=u'[esi+ecx*8+4000h]'),u'[esi+ecx*8+0x4000]')
        self.assertEqual(cpp.convert_number(expr=u'[esi+ecx*8-0Ah]'),u'[esi+ecx*8-0x0A]')
        self.assertEqual(cpp.convert_number(expr=u'[esi+ecx*8]'),u'[esi+ecx*8]')
        self.assertEqual(cpp.convert_number(expr=u'[esi+ecx]'),u'[esi+ecx]')
        self.assertEqual(cpp.convert_number(expr=u'[esi]'),u'[esi]')
        self.assertEqual(cpp.convert_number(expr=u'[esp+0Ch]'),u'[esp+0x0C]')
        self.assertEqual(cpp.convert_number(expr=u'[esp+10h]'),u'[esp+0x10]')
        self.assertEqual(cpp.convert_number(expr=u'[esp+14h]'),u'[esp+0x14]')
        self.assertEqual(cpp.convert_number(expr=u'[esp+18h]'),u'[esp+0x18]')
        self.assertEqual(cpp.convert_number(expr=u'[esp+1Ch]'),u'[esp+0x1C]')
        self.assertEqual(cpp.convert_number(expr=u'[esp+4]'),u'[esp+4]')
        self.assertEqual(cpp.convert_number(expr=u'[esp+8]'),u'[esp+8]')
        self.assertEqual(cpp.convert_number(expr=u'[esp]'),u'[esp]')
        self.assertEqual(cpp.convert_number(expr=u'[g]'),u'[g]')
        self.assertEqual(cpp.convert_number(expr=u'[h2]'),u'[h2]')
        self.assertEqual(cpp.convert_number(expr=u'[i+1]'),u'[i+1]')
        self.assertEqual(cpp.convert_number(expr=u'[i+2]'),u'[i+2]')
        self.assertEqual(cpp.convert_number(expr=u'[i+3]'),u'[i+3]')
        self.assertEqual(cpp.convert_number(expr=u'[i+4]'),u'[i+4]')
        self.assertEqual(cpp.convert_number(expr=u'[i+56h]'),u'[i+0x56]')
        self.assertEqual(cpp.convert_number(expr=u'[i+5]'),u'[i+5]')
        self.assertEqual(cpp.convert_number(expr=u'[i-10h]'),u'[i-0x10]')
        self.assertEqual(cpp.convert_number(expr=u'[load_handle]'),u'[load_handle]')
        self.assertEqual(cpp.convert_number(expr=u'[var+3]'),u'[var+3]')
        self.assertEqual(cpp.convert_number(expr=u'[var+4]'),u'[var+4]')
        self.assertEqual(cpp.convert_number(expr=u'[var-1]'),u'[var-1]')
        self.assertEqual(cpp.convert_number(expr=u'[var0+5]'),u'[var0+5]')
        self.assertEqual(cpp.convert_number(expr=u'[var1+1]'),u'[var1+1]')
        self.assertEqual(cpp.convert_number(expr=u'[var1]'),u'[var1]')
        self.assertEqual(cpp.convert_number(expr=u'[var2+2]'),u'[var2+2]')
        self.assertEqual(cpp.convert_number(expr=u'[var2-1]'),u'[var2-1]')
        self.assertEqual(cpp.convert_number(expr=u'[var2]'),u'[var2]')
        self.assertEqual(cpp.convert_number(expr=u'[var3+3*4]'),u'[var3+3*4]')
        self.assertEqual(cpp.convert_number(expr=u'[var3+ebp]'),u'[var3+ebp]')
        self.assertEqual(cpp.convert_number(expr=u'[var3]'),u'[var3]')
        self.assertEqual(cpp.convert_number(expr=u'[var4+t]'),u'[var4+t]')
        self.assertEqual(cpp.convert_number(expr=u'[var4]'),u'[var4]')
        self.assertEqual(cpp.convert_number(expr=u'[var]'),u'[var]')
        self.assertEqual(cpp.convert_number(expr=u'ah'),u'ah')
        self.assertEqual(cpp.convert_number(expr=u'al'),u'al')
        self.assertEqual(cpp.convert_number(expr=u'ax'),u'ax')
        self.assertEqual(cpp.convert_number(expr=u'b'),u'b')
        self.assertEqual(cpp.convert_number(expr=u'beginningdata'),u'beginningdata')
        self.assertEqual(cpp.convert_number(expr=u'bh'),u'bh')
        self.assertEqual(cpp.convert_number(expr=u'bl'),u'bl')
        self.assertEqual(cpp.convert_number(expr=u'bp'),u'bp')
        self.assertEqual(cpp.convert_number(expr=u'buffer'),u'buffer')
        self.assertEqual(cpp.convert_number(expr=u'bx'),u'bx')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [a]'),u'byte ptr [a]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [ebp+var_20]'),u'byte ptr [ebp+var_20]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [edi+1]'),u'byte ptr [edi+1]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [edi+7]'),u'byte ptr [edi+7]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [esi]'),u'byte ptr [esi]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [h2]'),u'byte ptr [h2]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [h]'),u'byte ptr [h]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [testOVerlap+1]'),u'byte ptr [testOVerlap+1]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [var1+1]'),u'byte ptr [var1+1]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr [var1+2]'),u'byte ptr [var1+2]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr dl'),u'byte ptr dl')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr ds:[0]'),u'byte ptr ds:[0]')
        self.assertEqual(cpp.convert_number(expr=u'byte ptr es:[0]'),u'byte ptr es:[0]')
        self.assertEqual(cpp.convert_number(expr=u'ch'),u'ch')
        self.assertEqual(cpp.convert_number(expr=u'cl'),u'cl')
        self.assertEqual(cpp.convert_number(expr=u'cx'),u'cx')
        self.assertEqual(cpp.convert_number(expr=u'di'),u'di')
        self.assertEqual(cpp.convert_number(expr=u'dl'),u'dl')
        self.assertEqual(cpp.convert_number(expr=u'ds'),u'ds')
        self.assertEqual(cpp.convert_number(expr=u'ds:0[eax*2]'),u'ds:0[eax*2]')
        self.assertEqual(cpp.convert_number(expr=u'ds:0[ebx*4]'),u'ds:0[ebx*4]')
        self.assertEqual(cpp.convert_number(expr=u'ds:0[ecx*8]'),u'ds:0[ecx*8]')
        self.assertEqual(cpp.convert_number(expr=u'ds:40h[eax*2]'),u'ds:0x40[eax*2]')
        self.assertEqual(cpp.convert_number(expr=u'ds:40h[ebx*4]'),u'ds:0x40[ebx*4]')
        self.assertEqual(cpp.convert_number(expr=u'ds:40h[ecx*8]'),u'ds:0x40[ecx*8]')
        self.assertEqual(cpp.convert_number(expr=u'ds:[edi]'),u'ds:[edi]')
        self.assertEqual(cpp.convert_number(expr=u'ds:byte_41411F[eax]'),u'ds:byte_41411F[eax]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [ebp+var_20+4]'),u'dword ptr [ebp+var_20+4]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [ebp+var_20]'),u'dword ptr [ebp+var_20]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [ebx-4]'),u'dword ptr [ebx-4]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [esp+0Ch]'),u'dword ptr [esp+0x0C]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [esp+10h]'),u'dword ptr [esp+0x10]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [esp+14h]'),u'dword ptr [esp+0x14]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [esp+1Ch]'),u'dword ptr [esp+0x1C]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [esp+4]'),u'dword ptr [esp+4]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [esp+8]'),u'dword ptr [esp+8]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr [esp]'),u'dword ptr [esp]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr buffer'),u'dword ptr buffer')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr es:[0]'),u'dword ptr es:[0]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr es:[20*320+160]'),u'dword ptr es:[20*320+160]')
        self.assertEqual(cpp.convert_number(expr=u'dword ptr var4'),u'dword ptr var4')
        self.assertEqual(cpp.convert_number(expr=u'dword'),u'dword')
        self.assertEqual(cpp.convert_number(expr=u'dx'),u'dx')
        self.assertEqual(cpp.convert_number(expr=u'eax'),u'eax')
        self.assertEqual(cpp.convert_number(expr=u'eax_0'),u'eax_0')
        self.assertEqual(cpp.convert_number(expr=u'ebp'),u'ebp')
        self.assertEqual(cpp.convert_number(expr=u'ebx'),u'ebx')
        self.assertEqual(cpp.convert_number(expr=u'ecx'),u'ecx')
        self.assertEqual(cpp.convert_number(expr=u'ecx_0'),u'ecx_0')
        self.assertEqual(cpp.convert_number(expr=u'ecx_0_0'),u'ecx_0_0')
        self.assertEqual(cpp.convert_number(expr=u'edi'),u'edi')
        self.assertEqual(cpp.convert_number(expr=u'edi_0'),u'edi_0')
        self.assertEqual(cpp.convert_number(expr=u'edx'),u'edx')
        self.assertEqual(cpp.convert_number(expr=u'edx_0_0'),u'edx_0_0')
        self.assertEqual(cpp.convert_number(expr=u'eflags'),u'eflags')
        self.assertEqual(cpp.convert_number(expr=u'enddata'),u'enddata')
        self.assertEqual(cpp.convert_number(expr=u'es'),u'es')
        self.assertEqual(cpp.convert_number(expr=u'esi'),u'esi')
        self.assertEqual(cpp.convert_number(expr=u'esi_0'),u'esi_0')
        self.assertEqual(cpp.convert_number(expr=u'esp'),u'esp')
        self.assertEqual(cpp.convert_number(expr=u'f'),u'f')
        self.assertEqual(cpp.convert_number(expr=u'fileName'),u'fileName')
        self.assertEqual(cpp.convert_number(expr=u'flags'),u'flags')
        self.assertEqual(cpp.convert_number(expr=u'fs'),u'fs')
        self.assertEqual(cpp.convert_number(expr=u'i'),u'i')
        self.assertEqual(cpp.convert_number(expr=u'large ds:4000h'),u'large ds:0x4000')
        self.assertEqual(cpp.convert_number(expr=u'offset _msg'),u'offset _msg')
        self.assertEqual(cpp.convert_number(expr=u'offset _test_btc'),u'offset _test_btc')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000'),u'offset a0x4000')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000Eax'),u'offset a0x4000Eax')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000Ebx'),u'offset a0x4000Ebx')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000EbxEdx'),u'offset a0x4000EbxEdx')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000Ecx'),u'offset a0x4000Ecx')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000EcxEcx2'),u'offset a0x4000EcxEcx2')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000Edi'),u'offset a0x4000Edi')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000Edx'),u'offset a0x4000Edx')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000EdxEcx4'),u'offset a0x4000EdxEcx4')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000Esi'),u'offset a0x4000Esi')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x4000EsiEcx8'),u'offset a0x4000EsiEcx8')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40Eax'),u'offset a0x40Eax')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40Eax2'),u'offset a0x40Eax2')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40EaxEcx'),u'offset a0x40EaxEcx')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40Ebx'),u'offset a0x40Ebx')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40Ebx4'),u'offset a0x40Ebx4')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40Ecx'),u'offset a0x40Ecx')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40Ecx8'),u'offset a0x40Ecx8')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40Edi'),u'offset a0x40Edi')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40Edx'),u'offset a0x40Edx')
        self.assertEqual(cpp.convert_number(expr=u'offset a0x40Esi'),u'offset a0x40Esi')
        self.assertEqual(cpp.convert_number(expr=u'offset a10EcxEcx2'),u'offset a10EcxEcx2')
        self.assertEqual(cpp.convert_number(expr=u'offset a10EdxEcx4'),u'offset a10EdxEcx4')
        self.assertEqual(cpp.convert_number(expr=u'offset a10EsiEcx8'),u'offset a10EsiEcx8')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08lxB08lx'),u'offset a10sA08lxB08lx')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08lxB08lxC'),u'offset a10sA08lxB08lxC')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08lxB08lxR'),u'offset a10sA08lxB08lxR')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08lxB08lxR_0'),u'offset a10sA08lxB08lxR_0')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08lxR08lx'),u'offset a10sA08lxR08lx')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08lxR08lx0'),u'offset a10sA08lxR08lx0')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08lxR08lxC'),u'offset a10sA08lxR08lxC')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08lxR08lxL'),u'offset a10sA08lxR08lxL')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08lxR08lx_0'),u'offset a10sA08lxR08lx_0')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sA08xR08xCci'),u'offset a10sA08xR08xCci')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sAh08lxAl08l'),u'offset a10sAh08lxAl08l')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sD'),u'offset a10sD')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sEax08lxA08l'),u'offset a10sEax08lxA08l')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sEcx08lxZfLd'),u'offset a10sEcx08lxZfLd')
        self.assertEqual(cpp.convert_number(expr=u'offset a10sEsi08lxEdi0'),u'offset a10sEsi08lxEdi0')
        self.assertEqual(cpp.convert_number(expr=u'offset aAaa'),u'offset aAaa')
        self.assertEqual(cpp.convert_number(expr=u'offset aAad'),u'offset aAad')
        self.assertEqual(cpp.convert_number(expr=u'offset aAam'),u'offset aAam')
        self.assertEqual(cpp.convert_number(expr=u'offset aAas'),u'offset aAas')
        self.assertEqual(cpp.convert_number(expr=u'offset aAdcb'),u'offset aAdcb')
        self.assertEqual(cpp.convert_number(expr=u'offset aAdcl'),u'offset aAdcl')
        self.assertEqual(cpp.convert_number(expr=u'offset aAdcw'),u'offset aAdcw')
        self.assertEqual(cpp.convert_number(expr=u'offset aAddb'),u'offset aAddb')
        self.assertEqual(cpp.convert_number(expr=u'offset aAddl'),u'offset aAddl')
        self.assertEqual(cpp.convert_number(expr=u'offset aAddw'),u'offset aAddw')
        self.assertEqual(cpp.convert_number(expr=u'offset aAndb'),u'offset aAndb')
        self.assertEqual(cpp.convert_number(expr=u'offset aAndl'),u'offset aAndl')
        self.assertEqual(cpp.convert_number(expr=u'offset aAndw'),u'offset aAndw')
        self.assertEqual(cpp.convert_number(expr=u'offset aBsfl'),u'offset aBsfl')
        self.assertEqual(cpp.convert_number(expr=u'offset aBsfw'),u'offset aBsfw')
        self.assertEqual(cpp.convert_number(expr=u'offset aBsrl'),u'offset aBsrl')
        self.assertEqual(cpp.convert_number(expr=u'offset aBsrw'),u'offset aBsrw')
        self.assertEqual(cpp.convert_number(expr=u'offset aBswapl'),u'offset aBswapl')
        self.assertEqual(cpp.convert_number(expr=u'offset aBtcl'),u'offset aBtcl')
        self.assertEqual(cpp.convert_number(expr=u'offset aBtcw'),u'offset aBtcw')
        self.assertEqual(cpp.convert_number(expr=u'offset aBtl'),u'offset aBtl')
        self.assertEqual(cpp.convert_number(expr=u'offset aBtrl'),u'offset aBtrl')
        self.assertEqual(cpp.convert_number(expr=u'offset aBtrw'),u'offset aBtrw')
        self.assertEqual(cpp.convert_number(expr=u'offset aBtsl'),u'offset aBtsl')
        self.assertEqual(cpp.convert_number(expr=u'offset aBtsw'),u'offset aBtsw')
        self.assertEqual(cpp.convert_number(expr=u'offset aBtw'),u'offset aBtw')
        self.assertEqual(cpp.convert_number(expr=u'offset aCbw'),u'offset aCbw')
        self.assertEqual(cpp.convert_number(expr=u'offset aCdq'),u'offset aCdq')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpb'),u'offset aCmpb')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpl'),u'offset aCmpl')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpsb'),u'offset aCmpsb')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpsl'),u'offset aCmpsl')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpsw'),u'offset aCmpsw')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpw'),u'offset aCmpw')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpxchg8bEax08'),u'offset aCmpxchg8bEax08')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpxchgb'),u'offset aCmpxchgb')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpxchgl'),u'offset aCmpxchgl')
        self.assertEqual(cpp.convert_number(expr=u'offset aCmpxchgw'),u'offset aCmpxchgw')
        self.assertEqual(cpp.convert_number(expr=u'offset aCwd'),u'offset aCwd')
        self.assertEqual(cpp.convert_number(expr=u'offset aCwde'),u'offset aCwde')
        self.assertEqual(cpp.convert_number(expr=u'offset aDaa'),u'offset aDaa')
        self.assertEqual(cpp.convert_number(expr=u'offset aDas'),u'offset aDas')
        self.assertEqual(cpp.convert_number(expr=u'offset aDecb'),u'offset aDecb')
        self.assertEqual(cpp.convert_number(expr=u'offset aDecl'),u'offset aDecl')
        self.assertEqual(cpp.convert_number(expr=u'offset aDecw'),u'offset aDecw')
        self.assertEqual(cpp.convert_number(expr=u'offset aDivb'),u'offset aDivb')
        self.assertEqual(cpp.convert_number(expr=u'offset aDivl'),u'offset aDivl')
        self.assertEqual(cpp.convert_number(expr=u'offset aDivw'),u'offset aDivw')
        self.assertEqual(cpp.convert_number(expr=u'offset aEax'),u'offset aEax')
        self.assertEqual(cpp.convert_number(expr=u'offset aEax2'),u'offset aEax2')
        self.assertEqual(cpp.convert_number(expr=u'offset aEaxEcx'),u'offset aEaxEcx')
        self.assertEqual(cpp.convert_number(expr=u'offset aEbx'),u'offset aEbx')
        self.assertEqual(cpp.convert_number(expr=u'offset aEbx4'),u'offset aEbx4')
        self.assertEqual(cpp.convert_number(expr=u'offset aEbxEdx'),u'offset aEbxEdx')
        self.assertEqual(cpp.convert_number(expr=u'offset aEcx'),u'offset aEcx')
        self.assertEqual(cpp.convert_number(expr=u'offset aEcx8'),u'offset aEcx8')
        self.assertEqual(cpp.convert_number(expr=u'offset aEcxEcx'),u'offset aEcxEcx')
        self.assertEqual(cpp.convert_number(expr=u'offset aEcxEcx2'),u'offset aEcxEcx2')
        self.assertEqual(cpp.convert_number(expr=u'offset aEdi'),u'offset aEdi')
        self.assertEqual(cpp.convert_number(expr=u'offset aEdiEcx'),u'offset aEdiEcx')
        self.assertEqual(cpp.convert_number(expr=u'offset aEdx'),u'offset aEdx')
        self.assertEqual(cpp.convert_number(expr=u'offset aEdxEcx'),u'offset aEdxEcx')
        self.assertEqual(cpp.convert_number(expr=u'offset aEdxEcx4'),u'offset aEdxEcx4')
        self.assertEqual(cpp.convert_number(expr=u'offset aEsi'),u'offset aEsi')
        self.assertEqual(cpp.convert_number(expr=u'offset aEsiEcx'),u'offset aEsiEcx')
        self.assertEqual(cpp.convert_number(expr=u'offset aEsiEcx8'),u'offset aEsiEcx8')
        self.assertEqual(cpp.convert_number(expr=u'offset aIdivb'),u'offset aIdivb')
        self.assertEqual(cpp.convert_number(expr=u'offset aIdivl'),u'offset aIdivl')
        self.assertEqual(cpp.convert_number(expr=u'offset aIdivw'),u'offset aIdivw')
        self.assertEqual(cpp.convert_number(expr=u'offset aImulb'),u'offset aImulb')
        self.assertEqual(cpp.convert_number(expr=u'offset aImull'),u'offset aImull')
        self.assertEqual(cpp.convert_number(expr=u'offset aImullIm'),u'offset aImullIm')
        self.assertEqual(cpp.convert_number(expr=u'offset aImulw'),u'offset aImulw')
        self.assertEqual(cpp.convert_number(expr=u'offset aImulwIm'),u'offset aImulwIm')
        self.assertEqual(cpp.convert_number(expr=u'offset aIncb'),u'offset aIncb')
        self.assertEqual(cpp.convert_number(expr=u'offset aIncl'),u'offset aIncl')
        self.assertEqual(cpp.convert_number(expr=u'offset aIncw'),u'offset aIncw')
        self.assertEqual(cpp.convert_number(expr=u'offset aJa'),u'offset aJa')
        self.assertEqual(cpp.convert_number(expr=u'offset aJae'),u'offset aJae')
        self.assertEqual(cpp.convert_number(expr=u'offset aJb'),u'offset aJb')
        self.assertEqual(cpp.convert_number(expr=u'offset aJbe'),u'offset aJbe')
        self.assertEqual(cpp.convert_number(expr=u'offset aJcxz'),u'offset aJcxz')
        self.assertEqual(cpp.convert_number(expr=u'offset aJe'),u'offset aJe')
        self.assertEqual(cpp.convert_number(expr=u'offset aJecxz'),u'offset aJecxz')
        self.assertEqual(cpp.convert_number(expr=u'offset aJg'),u'offset aJg')
        self.assertEqual(cpp.convert_number(expr=u'offset aJge'),u'offset aJge')
        self.assertEqual(cpp.convert_number(expr=u'offset aJl'),u'offset aJl')
        self.assertEqual(cpp.convert_number(expr=u'offset aJle'),u'offset aJle')
        self.assertEqual(cpp.convert_number(expr=u'offset aJne'),u'offset aJne')
        self.assertEqual(cpp.convert_number(expr=u'offset aJns'),u'offset aJns')
        self.assertEqual(cpp.convert_number(expr=u'offset aJs'),u'offset aJs')
        self.assertEqual(cpp.convert_number(expr=u'offset aLeaS08lx'),u'offset aLeaS08lx')
        self.assertEqual(cpp.convert_number(expr=u'offset aLodsb'),u'offset aLodsb')
        self.assertEqual(cpp.convert_number(expr=u'offset aLodsl'),u'offset aLodsl')
        self.assertEqual(cpp.convert_number(expr=u'offset aLodsw'),u'offset aLodsw')
        self.assertEqual(cpp.convert_number(expr=u'offset aLoopl'),u'offset aLoopl')
        self.assertEqual(cpp.convert_number(expr=u'offset aLoopnzl'),u'offset aLoopnzl')
        self.assertEqual(cpp.convert_number(expr=u'offset aLoopzl'),u'offset aLoopzl')
        self.assertEqual(cpp.convert_number(expr=u'offset aMovsb'),u'offset aMovsb')
        self.assertEqual(cpp.convert_number(expr=u'offset aMovsl'),u'offset aMovsl')
        self.assertEqual(cpp.convert_number(expr=u'offset aMovsw'),u'offset aMovsw')
        self.assertEqual(cpp.convert_number(expr=u'offset aMulb'),u'offset aMulb')
        self.assertEqual(cpp.convert_number(expr=u'offset aMull'),u'offset aMull')
        self.assertEqual(cpp.convert_number(expr=u'offset aMulw'),u'offset aMulw')
        self.assertEqual(cpp.convert_number(expr=u'offset aNegb'),u'offset aNegb')
        self.assertEqual(cpp.convert_number(expr=u'offset aNegl'),u'offset aNegl')
        self.assertEqual(cpp.convert_number(expr=u'offset aNegw'),u'offset aNegw')
        self.assertEqual(cpp.convert_number(expr=u'offset aNotb'),u'offset aNotb')
        self.assertEqual(cpp.convert_number(expr=u'offset aNotl'),u'offset aNotl')
        self.assertEqual(cpp.convert_number(expr=u'offset aNotw'),u'offset aNotw')
        self.assertEqual(cpp.convert_number(expr=u'offset aOrb'),u'offset aOrb')
        self.assertEqual(cpp.convert_number(expr=u'offset aOrl'),u'offset aOrl')
        self.assertEqual(cpp.convert_number(expr=u'offset aOrw'),u'offset aOrw')
        self.assertEqual(cpp.convert_number(expr=u'offset aPopcntA08lxR08'),u'offset aPopcntA08lxR08')
        self.assertEqual(cpp.convert_number(expr=u'offset aPoplEsp08lx'),u'offset aPoplEsp08lx')
        self.assertEqual(cpp.convert_number(expr=u'offset aPopwEsp08lx'),u'offset aPopwEsp08lx')
        self.assertEqual(cpp.convert_number(expr=u'offset aRclb'),u'offset aRclb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRcll'),u'offset aRcll')
        self.assertEqual(cpp.convert_number(expr=u'offset aRclw'),u'offset aRclw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRcrb'),u'offset aRcrb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRcrl'),u'offset aRcrl')
        self.assertEqual(cpp.convert_number(expr=u'offset aRcrw'),u'offset aRcrw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepLodsb'),u'offset aRepLodsb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepLodsl'),u'offset aRepLodsl')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepLodsw'),u'offset aRepLodsw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepMovsb'),u'offset aRepMovsb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepMovsl'),u'offset aRepMovsl')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepMovsw'),u'offset aRepMovsw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepStosb'),u'offset aRepStosb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepStosl'),u'offset aRepStosl')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepStosw'),u'offset aRepStosw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepnzCmpsb'),u'offset aRepnzCmpsb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepnzCmpsl'),u'offset aRepnzCmpsl')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepnzCmpsw'),u'offset aRepnzCmpsw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepnzScasb'),u'offset aRepnzScasb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepnzScasl'),u'offset aRepnzScasl')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepnzScasw'),u'offset aRepnzScasw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepzCmpsb'),u'offset aRepzCmpsb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepzCmpsl'),u'offset aRepzCmpsl')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepzCmpsw'),u'offset aRepzCmpsw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepzScasb'),u'offset aRepzScasb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepzScasl'),u'offset aRepzScasl')
        self.assertEqual(cpp.convert_number(expr=u'offset aRepzScasw'),u'offset aRepzScasw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRolb'),u'offset aRolb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRoll'),u'offset aRoll')
        self.assertEqual(cpp.convert_number(expr=u'offset aRolw'),u'offset aRolw')
        self.assertEqual(cpp.convert_number(expr=u'offset aRorb'),u'offset aRorb')
        self.assertEqual(cpp.convert_number(expr=u'offset aRorl'),u'offset aRorl')
        self.assertEqual(cpp.convert_number(expr=u'offset aRorw'),u'offset aRorw')
        self.assertEqual(cpp.convert_number(expr=u'offset aSarb'),u'offset aSarb')
        self.assertEqual(cpp.convert_number(expr=u'offset aSarl'),u'offset aSarl')
        self.assertEqual(cpp.convert_number(expr=u'offset aSarw'),u'offset aSarw')
        self.assertEqual(cpp.convert_number(expr=u'offset aSbbb'),u'offset aSbbb')
        self.assertEqual(cpp.convert_number(expr=u'offset aSbbl'),u'offset aSbbl')
        self.assertEqual(cpp.convert_number(expr=u'offset aSbbw'),u'offset aSbbw')
        self.assertEqual(cpp.convert_number(expr=u'offset aScasb'),u'offset aScasb')
        self.assertEqual(cpp.convert_number(expr=u'offset aScasl'),u'offset aScasl')
        self.assertEqual(cpp.convert_number(expr=u'offset aScasw'),u'offset aScasw')
        self.assertEqual(cpp.convert_number(expr=u'offset aSetb'),u'offset aSetb')
        self.assertEqual(cpp.convert_number(expr=u'offset aSete'),u'offset aSete')
        self.assertEqual(cpp.convert_number(expr=u'offset aSetne'),u'offset aSetne')
        self.assertEqual(cpp.convert_number(expr=u'offset aShlb'),u'offset aShlb')
        self.assertEqual(cpp.convert_number(expr=u'offset aShldl'),u'offset aShldl')
        self.assertEqual(cpp.convert_number(expr=u'offset aShldw'),u'offset aShldw')
        self.assertEqual(cpp.convert_number(expr=u'offset aShll'),u'offset aShll')
        self.assertEqual(cpp.convert_number(expr=u'offset aShlw'),u'offset aShlw')
        self.assertEqual(cpp.convert_number(expr=u'offset aShrb'),u'offset aShrb')
        self.assertEqual(cpp.convert_number(expr=u'offset aShrdl'),u'offset aShrdl')
        self.assertEqual(cpp.convert_number(expr=u'offset aShrdw'),u'offset aShrdw')
        self.assertEqual(cpp.convert_number(expr=u'offset aShrl'),u'offset aShrl')
        self.assertEqual(cpp.convert_number(expr=u'offset aShrw'),u'offset aShrw')
        self.assertEqual(cpp.convert_number(expr=u'offset aStosb'),u'offset aStosb')
        self.assertEqual(cpp.convert_number(expr=u'offset aStosl'),u'offset aStosl')
        self.assertEqual(cpp.convert_number(expr=u'offset aStosw'),u'offset aStosw')
        self.assertEqual(cpp.convert_number(expr=u'offset aSubb'),u'offset aSubb')
        self.assertEqual(cpp.convert_number(expr=u'offset aSubl'),u'offset aSubl')
        self.assertEqual(cpp.convert_number(expr=u'offset aSubw'),u'offset aSubw')
        self.assertEqual(cpp.convert_number(expr=u'offset aXaddb'),u'offset aXaddb')
        self.assertEqual(cpp.convert_number(expr=u'offset aXaddl'),u'offset aXaddl')
        self.assertEqual(cpp.convert_number(expr=u'offset aXaddlSameRes08'),u'offset aXaddlSameRes08')
        self.assertEqual(cpp.convert_number(expr=u'offset aXaddw'),u'offset aXaddw')
        self.assertEqual(cpp.convert_number(expr=u'offset aXchgb'),u'offset aXchgb')
        self.assertEqual(cpp.convert_number(expr=u'offset aXchgl'),u'offset aXchgl')
        self.assertEqual(cpp.convert_number(expr=u'offset aXchgw'),u'offset aXchgw')
        self.assertEqual(cpp.convert_number(expr=u'offset aXlatEax08lx'),u'offset aXlatEax08lx')
        self.assertEqual(cpp.convert_number(expr=u'offset aXorb'),u'offset aXorb')
        self.assertEqual(cpp.convert_number(expr=u'offset aXorl'),u'offset aXorl')
        self.assertEqual(cpp.convert_number(expr=u'offset aXorw'),u'offset aXorw')
        self.assertEqual(cpp.convert_number(expr=u'offset pal_jeu'),u'offset pal_jeu')
        self.assertEqual(cpp.convert_number(expr=u'offset str1'),u'offset str1')
        self.assertEqual(cpp.convert_number(expr=u'offset str2'),u'offset str2')
        self.assertEqual(cpp.convert_number(expr=u'offset str3'),u'offset str3')
        self.assertEqual(cpp.convert_number(expr=u'offset str_buffer+800h'),u'offset str_buffer+0x800')
        self.assertEqual(cpp.convert_number(expr=u'offset str_buffer+810h'),u'offset str_buffer+0x810')
        self.assertEqual(cpp.convert_number(expr=u'offset testOVerlap'),u'offset testOVerlap')
        self.assertEqual(cpp.convert_number(expr=u'offset unk_40E008'),u'offset unk_40E008')
        self.assertEqual(cpp.convert_number(expr=u'offset unk_40F064'),u'offset unk_40F064')
        self.assertEqual(cpp.convert_number(expr=u'offset var1'),u'offset var1')
        self.assertEqual(cpp.convert_number(expr=u'offset var1+1'),u'offset var1+1')
        self.assertEqual(cpp.convert_number(expr=u'offset var2'),u'offset var2')
        self.assertEqual(cpp.convert_number(expr=u'offset var3'),u'offset var3')
        self.assertEqual(cpp.convert_number(expr=u'offset var3+4'),u'offset var3+4')
        self.assertEqual(cpp.convert_number(expr=u'offset var4'),u'offset var4')
        self.assertEqual(cpp.convert_number(expr=u'offset var4+1'),u'offset var4+1')
        self.assertEqual(cpp.convert_number(expr=u'offset var4+4'),u'offset var4+4')
        self.assertEqual(cpp.convert_number(expr=u'op0'),u'op0')
        self.assertEqual(cpp.convert_number(expr=u'op1'),u'op1')
        self.assertEqual(cpp.convert_number(expr=u'printf'),u'printf')
        self.assertEqual(cpp.convert_number(expr=u'ptr'),u'ptr')
        self.assertEqual(cpp.convert_number(expr=u'r'),u'r')
        self.assertEqual(cpp.convert_number(expr=u'res'),u'res')
        self.assertEqual(cpp.convert_number(expr=u'resh'),u'resh')
        self.assertEqual(cpp.convert_number(expr=u'resz'),u'resz')
        self.assertEqual(cpp.convert_number(expr=u'rh'),u'rh')
        self.assertEqual(cpp.convert_number(expr=u's0_0'),u's0_0')
        self.assertEqual(cpp.convert_number(expr=u's1_0'),u's1_0')
        self.assertEqual(cpp.convert_number(expr=u'si'),u'si')
        self.assertEqual(cpp.convert_number(expr=u'small'),u'small')
        self.assertEqual(cpp.convert_number(expr=u't'),u't')
        self.assertEqual(cpp.convert_number(expr=u'taille_moire'),u'taille_moire')
        self.assertEqual(cpp.convert_number(expr=u'teST2'),u'teST2')
        self.assertEqual(cpp.convert_number(expr=u'testOVerlap'),u'testOVerlap')
        self.assertEqual(cpp.convert_number(expr=u'var1'),u'var1')
        self.assertEqual(cpp.convert_number(expr=u'var1[1]'),u'var1[1]')
        self.assertEqual(cpp.convert_number(expr=u'var1[bx+si]'),u'var1[bx+si]')
        self.assertEqual(cpp.convert_number(expr=u'var1[bx]'),u'var1[bx]')
        self.assertEqual(cpp.convert_number(expr=u'var2'),u'var2')
        self.assertEqual(cpp.convert_number(expr=u'var3+3*4'),u'var3+3*4')
        self.assertEqual(cpp.convert_number(expr=u'var3+ebp'),u'var3+ebp')
        self.assertEqual(cpp.convert_number(expr=u'var5'),u'var5')
        self.assertEqual(cpp.convert_number(expr=u'word ptr [d]'),u'word ptr [d]')
        self.assertEqual(cpp.convert_number(expr=u'word ptr [e]'),u'word ptr [e]')
        self.assertEqual(cpp.convert_number(expr=u'word ptr [ebp+var_20]'),u'word ptr [ebp+var_20]')
        self.assertEqual(cpp.convert_number(expr=u'word ptr [var5+2]'),u'word ptr [var5+2]')
        self.assertEqual(cpp.convert_number(expr=u'word ptr var5'),u'word ptr var5')
        self.assertEqual(cpp.convert_number(expr=u'word'),u'word')


if __name__ == "__main__":
    unittest.main()
