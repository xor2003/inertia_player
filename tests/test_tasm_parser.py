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
import tasm.lex
import tasm.op
from tasm.op import label
from tasm.op import var
import tasm.parser
from tasm.parser import Parser
import tasm.proc
from tasm.proc import Proc
import traceback
import unittest

class TestInitOnce:
    
    def __init__(self):
        self.parser = Parser([])
        self.cpp = cpp.Cpp(self.parser)
        self.proc = Proc('mainproc', False)
        self.cpp.proc = self.proc

        self.proc.add_assignment(line_number=0, label='B', value=u'1')
        self.proc.add_assignment(line_number=0, label='DDD', value=u'var1')
        self.proc.add_assignment(line_number=0, label='argc', value=u'8')
        self.proc.add_assignment(line_number=0, label='argv', value=u'0x0C')
        self.proc.add_assignment(line_number=0, label='eax_0', value=u'eax')
        self.proc.add_assignment(line_number=0, label='ecx_0', value=u'-0x2C')
        self.proc.add_assignment(line_number=0, label='ecx_0_0', value=u'ecx')
        self.proc.add_assignment(line_number=0, label='ecx_vals', value=u'-0x28')
        self.proc.add_assignment(line_number=0, label='edi_0', value=u'edi')
        self.proc.add_assignment(line_number=0, label='edx_0', value=u'-0x2C')
        self.proc.add_assignment(line_number=0, label='edx_0_0', value=u'edx')
        self.proc.add_assignment(line_number=0, label='eflags', value=u'eax')
        self.proc.add_assignment(line_number=0, label='esi_0', value=u'ebx')
        self.proc.add_assignment(line_number=0, label='esi_0', value=u'esi')
        self.proc.add_assignment(line_number=0, label='flags', value=u'eax')
        self.proc.add_assignment(line_number=0, label='i', value=u'eax')
        self.proc.add_assignment(line_number=0, label='iflags', value=u'0x10')
        self.proc.add_assignment(line_number=0, label='iflags', value=u'0x14')
        self.proc.add_assignment(line_number=0, label='op0', value=u'0x0C')
        self.proc.add_assignment(line_number=0, label='op0h', value=u'8')
        self.proc.add_assignment(line_number=0, label='op1', value=u'eax')
        self.proc.add_assignment(line_number=0, label='r', value=u'eax')
        self.proc.add_assignment(line_number=0, label='res', value=u'eax')
        self.proc.add_assignment(line_number=0, label='resh', value=u'ebx')
        self.proc.add_assignment(line_number=0, label='resz', value=u'ecx')
        self.proc.add_assignment(line_number=0, label='rh', value=u'edx')
        self.proc.add_assignment(line_number=0, label='s0', value=u'0x0C')
        self.proc.add_assignment(line_number=0, label='s0_0', value=u'ebx')
        self.proc.add_assignment(line_number=0, label='s1', value=u'0x0C')
        self.proc.add_assignment(line_number=0, label='s1_0', value=u'ecx')
        self.proc.add_assignment(line_number=0, label='s2', value=u'8')
        self.proc.add_assignment(line_number=0, label='table', value=u'-0x108')
        self.proc.add_assignment(line_number=0, label='val', value=u'-0x1C')
        self.proc.add_assignment(line_number=0, label='var_1C', value=u'-0x1C')
        self.proc.add_assignment(line_number=0, label='var_20', value=u'-0x20')
        self.proc.add_assignment(line_number=0, label='var_2C', value=u'-0x2C')
        self.proc.add_assignment(line_number=0, label='var_4', value=u'-4')
        self.proc.add_equ(line_number=0, label='CC', value=u'4')
        self.proc.add_equ(line_number=0, label='T', value=u'4')
        self.proc.add_equ(line_number=0, label='TEST2', value=u'-13')
        self.proc.add_equ(line_number=0, label='dubsize', value=u'13')
        self.proc.add_equ(line_number=0, label='tWO', value=u'2')
        self.proc.add_equ(line_number=0, label='taille_moire', value=u'((((2030080+64000*26)/4096)+1)*4096)-1')
        self.proc.add_equ(line_number=0, label='test1', value=u'(00+38*3)*320+1/2+33*(3-1)')
        self.proc.add_equ(line_number=0, label='test3', value=u'1500')
        self.proc.add_equ(line_number=0, label='testEqu', value=u'1')

        self.parser.set_global("_data", op.var(1, 0, issegment=True))
        self.parser.set_global("var1", op.var(size=1, offset=1, name="var1", segment="_data", elements=1))
        self.parser.set_global('_msg', op.var(elements=2, name=u'_msg', offset=1, segment=u'_data', size=1))
        # p.set_global('_msg', op.var(name=u'_msg', offset=1, segment=u'dseg', size=1))
        self.parser.set_global('_test_btc', op.var(name=u'_test_btc', offset=1, segment=u'initcall', size=4))
        self.parser.set_global('a', op.var(elements=3, name=u'a', offset=1, segment=u'_data', size=1))
        self.parser.set_global('a10sa08lxb08lx', op.var(elements=2, name=u'a10sA08lxB08lx', offset=1, segment=u'_rdata', size=1))
        self.parser.set_global('a10sah08lxal08l', op.var(elements=2, name=u'a10sAh08lxAl08l', offset=1, segment=u'_rdata', size=1))
        self.parser.set_global('a10sd', op.var(elements=2, name=u'a10sD', offset=1, segment=u'_rdata', size=1))
        self.parser.set_global('a10seax08lxa08l', op.var(elements=2, name=u'a10sEax08lxA08l', offset=1, segment=u'_rdata', size=1))
        self.parser.set_global('ascii', op.var(elements=2, name=u'ASCII', offset=1, segment=u'_data', size=1))
        self.parser.set_global('axorw', op.var(name=u'aXorw', offset=1, segment=u'_rdata', size=1))
        self.parser.set_global('b', op.var(name=u'b', offset=1, segment=u'_data', size=1))
        self.parser.set_global('beginningdata', op.var(name=u'beginningdata', offset=1, segment=u'_data', size=1))
        self.parser.set_global('buffer', op.var(elements=64000, name=u'buffer', offset=1, segment=u'_data', size=1))
        self.parser.set_global('byte_41411f', op.var(name=u'byte_41411F', offset=1, segment=u'_bss', size=1))
        self.parser.set_global('d', op.var(name=u'd', offset=1, segment=u'_data', size=1))
        self.parser.set_global('doublequote', op.var(elements=0, name=u'doublequote', offset=1, segment=u'_data', size=1))
        self.parser.set_global('e', op.var(name=u'e', offset=1, segment=u'_data', size=1))
        self.parser.set_global('enddata', op.var(name=u'enddata', offset=1, segment=u'_data', size=1))
        self.parser.set_global('f', op.var(name=u'f', offset=1, segment=u'_data', size=1))
        self.parser.set_global('filename', op.var(name=u'fileName', offset=1, segment=u'_data', size=1))
        self.parser.set_global('g', op.var(name=u'g', offset=1, segment=u'_data', size=4))
        self.parser.set_global('h', op.var(name=u'h', offset=1, segment=u'_data', size=1))
        self.parser.set_global('h2', op.var(name=u'h2', offset=1, segment=u'_data', size=1))
        self.parser.set_global('load_handle', op.var(name=u'load_handle', offset=1, segment=u'_data', size=4))
        self.parser.set_global('pal_jeu', op.var(elements=16, name=u'pal_jeu', offset=1, segment=u'_data', size=1))
        self.parser.set_global('str1', op.var(elements=0, name=u'str1', offset=1, segment=u'_data', size=1))
        self.parser.set_global('str2', op.var(elements=0, name=u'str2', offset=1, segment=u'_data', size=1))
        self.parser.set_global('str3', op.var(elements=0, name=u'str3', offset=1, segment=u'_data', size=1))
        self.parser.set_global('str_buffer', op.var(elements=4096, name=u'str_buffer', offset=1, segment=u'_bss', size=1))
        self.parser.set_global('table', op.var(name=u'table', offset=1, segment=u'_text', size=2))
        self.parser.set_global('testoverlap', op.var(elements=14, name=u'testOVerlap', offset=1, segment=u'_data', size=1))
        self.parser.set_global('unk_40e008', op.var(name=u'unk_40E008', offset=1, segment=u'_data', size=1))
        self.parser.set_global('unk_40f064', op.var(name=u'unk_40F064', offset=1, segment=u'initcall', size=1))
        self.parser.set_global('var', op.var(elements=4, name=u'var', offset=1, segment=u'_data', size=1))
        self.parser.set_global('var0', op.var(elements=10, name=u'var0', offset=1, segment=u'_data', size=1))
        self.parser.set_global('var2', op.var(elements=3, name=u'var2', offset=1, segment=u'_data', size=2))
        self.parser.set_global('var3', op.var(elements=4, name=u'var3', offset=1, segment=u'_data', size=1))
        self.parser.set_global('var4', op.var(elements=100, name=u'var4', offset=1, segment=u'_data', size=1))
        self.parser.set_global('var5', op.var(elements=0, name=u'var5', offset=1, segment=u'_data', size=1))
        self.parser.action_label(far=False, name='@@saaccvaaaax', isproc=False)
        self.parser.action_label(far=False, name='@VBL1', isproc=False)
        self.parser.action_label(far=False, name='@VBL12', isproc=False)
        self.parser.action_label(far=False, name='@VBL2', isproc=False)
        self.parser.action_label(far=False, name='@VBL22', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@1', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@2', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@3', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@4', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@5', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@6', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@7', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@8', isproc=False)
        self.parser.action_label(far=False, name='@df@@@@9', isproc=False)
        self.parser.action_label(far=False, name='OK', isproc=False)
        self.parser.action_label(far=False, name='P1', isproc=False)
        self.parser.action_label(far=False, name='P2', isproc=False)
        self.parser.action_label(far=False, name='dffd', isproc=False)
        self.parser.action_label(far=False, name='exitLabel', isproc=False)
        self.parser.action_label(far=False, name='failure', isproc=False)
        self.parser.action_label(far=False, name='finTest', isproc=False)
        self.parser.action_label(far=False, name='loc_40458F', isproc=False)
        self.parser.action_label(far=False, name='loc_4046D6', isproc=False)
        self.parser.action_label(far=False, name='loc_406B3F', isproc=False)
        self.parser.action_label(far=False, name='loc_406CF8', isproc=False)
        self.parser.action_label(far=False, name='loc_406EBA', isproc=False)
        self.parser.action_label(far=False, name='loc_40707C', isproc=False)
        self.parser.action_label(far=False, name='loc_40723E', isproc=False)
        self.parser.action_label(far=False, name='loc_40752C', isproc=False)
        self.parser.action_label(far=False, name='loc_4075C2', isproc=False)
        self.parser.action_label(far=False, name='loc_407658', isproc=False)
        self.parser.action_label(far=False, name='loc_4076EE', isproc=False)
        self.parser.action_label(far=False, name='loc_407784', isproc=False)
        self.parser.action_label(far=False, name='loc_407E46', isproc=False)
        self.parser.action_label(far=False, name='loc_407F72', isproc=False)
        self.parser.action_label(far=False, name='loc_408008', isproc=False)
        self.parser.action_label(far=False, name='loc_40809E', isproc=False)
        self.parser.action_label(far=False, name='loc_408139', isproc=False)
        self.parser.action_label(far=False, name='loc_4081F6', isproc=False)
        self.parser.action_label(far=False, name='loc_4083E9', isproc=False)
        self.parser.action_label(far=False, name='loc_408464', isproc=False)
        self.parser.action_label(far=False, name='loc_4084DF', isproc=False)
        self.parser.action_label(far=False, name='loc_40855A', isproc=False)
        self.parser.action_label(far=False, name='loc_409652', isproc=False)
        self.parser.action_label(far=False, name='loc_40D571', isproc=False)
        self.parser.action_label(far=False, name='next', isproc=False)
        self.parser.action_label(far=False, name='noerror', isproc=False)
        self.parser.action_label(far=False, name='toto1', isproc=False)
        self.parser.action_label(far=False, name=u'exec_adc', isproc=True)
        self.parser.action_label(far=False, name=u'exec_rclb', isproc=True)
        self.parser.action_label(far=False, name=u'printeax', isproc=True)
        self.parser.action_label(far=False, name=u'test_bcd', isproc=True)


class ParserTest(unittest.TestCase):
    def __new__(cls, *args, **kwargs):
        if not hasattr(cls, '_inst'):
            cls._inst = super(ParserTest, cls).__new__(cls)
            cls._inst.v = TestInitOnce()
        return cls._inst


    def test_instr_10(self):
        pass
        #self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, p.action_code('pop     small word ptr [esp]')), u'\tR(POP(small));ntR(POP(word));ntR(POP(ptr));ntR(POP(*(dw*)(raddr(ss,esp))));\n')

    def test_instr_20(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('call dx')), u'\tR(CALL(__disp));\n')

    def test_instr_30(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     dword ptr [esp] eax edx')), u'\tR(POP(*(dd*)(raddr(ss,esp))));ntR(POP(eax));ntR(POP(edx));\n')

    def test_instr_40(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pOp  ebx ebp    eax')), u'\tR(POP(ebx));ntR(POP(ebp));ntR(POP(eax));\n')

    def test_instr_50(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop  dx cx ; linear address of allocated memory block')), u'\tR(POP(dx));ntR(POP(cx));\n')

    def test_instr_60(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop ds es')), u'\tR(POP(ds));ntR(POP(es));\n')

    def test_instr_70(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push  eax ebp  ebx')), u'\tR(PUSH(eax));ntR(PUSH(ebp));ntR(PUSH(ebx));\n')

    def test_instr_80(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push bx cx ; linear address of allocated memory block')), u'\tR(PUSH(bx));ntR(PUSH(cx));\n')

    def test_instr_90(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push bx fs')), u'\tR(PUSH(bx));ntR(PUSH(fs));\n')

    def test_instr_100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push es ds')), u'\tR(PUSH(es));ntR(PUSH(ds));\n')

    def test_instr_110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp a,1')), u'\tR(CMP(*(raddr(ds,offset(_data,a))), 1));\n')

    def test_instr_120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [ebp+i+table], dl')), u'\tR(MOV(*(dw*)(raddr(ss,ebp+i+offset(_text,table))), dl));\n')

    def test_instr_130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('CMP [var2],1')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var2))), 1));\n')

    def test_instr_140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('CMP [var2],13')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var2))), 13));\n')

    def test_instr_150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('CMP [var3],35')), u'\tR(CMP(*(raddr(ds,offset(_data,var3))), 35));\n')

    def test_instr_160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('CMP eax,133')), u'\tR(CMP(eax, 133));\n')

    def test_instr_170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('CMP eax,2')), u'\tR(CMP(eax, 2));\n')

    def test_instr_180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('CMP eax,3')), u'\tR(CMP(eax, 3));\n')

    def test_instr_190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('INC [var1]')), u'\tR(INC(m.var1));\n')

    def test_instr_200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('INC [var2]')), u'\tR(INC(*(dw*)(raddr(ds,offset(_data,var2)))));\n')

    def test_instr_210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('INC [var3]')), u'\tR(INC(*(raddr(ds,offset(_data,var3)))));\n')

    def test_instr_220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('INC eax')), u'\tR(INC(eax));\n')

    def test_instr_230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('INC edx')), u'\tR(INC(edx));\n')

    def test_instr_240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp b,256+3')), u'\tR(CMP(m.b, 256+3));\n')

    def test_instr_250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('call [cs:table+ax]')), 'tR(CALL(__disp));\n')

    def test_instr_260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('CMP eax,1')), u'\tR(CMP(eax, 1));\n')

    def test_instr_270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code("cmp ebx,'dcba'")), u'\tR(CMP(ebx, 0x64636261));\n')

    def test_instr_280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code("sub dl,'a'")), u"tR(SUB(dl, 'a'));n")

    def test_instr_290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var1[bx],2')), u'\tR(CMP(*(raddr(ds,offset(_data,var1)+bx)), 2));\n')

    def test_instr_300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code("cmp [doublequote+4],'d'")), u"tR(CMP(*(raddr(ds,offset(_data,doublequote)+4)), 'd'));n")

    def test_instr_310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code("cmp dword ptr buffer,'tseT'")), u'\tR(CMP(*(dd*)(raddr(ds,offset(_data,buffer))), 0x74736554));\n')

    def test_instr_320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code("mov ah,9            ; DS:DX->'$'-terminated string")), u'\tR(MOV(ah, 9));\n')

    def test_instr_330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code("mov dl,'c'")), u"tR(MOV(dl, 'c'));n")

    def test_instr_340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code("mov ecx,'dcba'")), u'\tR(MOV(ecx, 0x64636261));\n')

    def test_instr_350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('CMP [var1],111')), u'\tR(CMP(m.var1, 111));\n')

    def test_instr_360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JA failure')), u'\ttR(JA(failure));\n')

    def test_instr_370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JAE failure')), u'\ttR(JNC(failure));\n')

    def test_instr_380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JB failure')), u'\ttR(JC(failure));\n')

    def test_instr_390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JE @VBL2    ;on attends la fin du retrace')), u'\ttR(JZ(arbvbl2));\n')

    def test_instr_400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JE @VBL22    ;on attends la fin du retrace')), u'\ttR(JZ(arbvbl22));\n')

    def test_instr_410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JE failure')), u'\ttR(JZ(failure));\n')

    def test_instr_420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JMP exitLabel')), u'\ttR(JMP(exitlabel));\n')

    def test_instr_430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JNE @VBL1    ;on attends le retrace')), u'\ttR(JNZ(arbvbl1));\n')

    def test_instr_440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JNE @VBL12    ;on attends le retrace')), u'\ttR(JNZ(arbvbl12));\n')

    def test_instr_450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JNE failure')), u'\ttR(JNZ(failure));\n')

    def test_instr_460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('JNZ  @@saaccvaaaax')), u'\ttR(JNZ(arbarbsaaccvaaaax));\n')

    def test_instr_470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('LODSB')), 'LODSB;\n')

    def test_instr_480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('MOV DX,3DAh')), u'\tR(MOV(dx, 0x3DA));\n')

    def test_instr_490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('MOV al,0')), u'\tR(MOV(al, 0));\n')

    def test_instr_500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('MOV ds, _data')), u'\tR(MOV(ds, seg_offset(_data)));\n')

    def test_instr_510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('SHL ch,2')), u'\tR(SHL(ch, 2));\n')

    def test_instr_520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('SHR bl,1')), u'\tR(SHR(bl, 1));\n')

    def test_instr_530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('SHR ch,1')), u'\tR(SHR(ch, 1));\n')

    def test_instr_540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('SHR eax,1')), u'\tR(SHR(eax, 1));\n')

    def test_instr_550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('TEST AL,8')), u'\tR(TEST(al, 8));\n')

    def test_instr_560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('XOR   al,al')), u'\tal = 0;AFFECT_ZF(0); AFFECT_SF(al,0);\n')

    def test_instr_570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('aaa')), 'tR(AAA);\n')

    def test_instr_580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('aad')), 'tR(AAD);\n')

    def test_instr_590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('aam')), 'tR(AAM);\n')

    def test_instr_600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('aas')), 'tR(AAS);\n')

    def test_instr_610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('adc     dl, cl')), u'\tR(ADC(dl, cl));\n')

    def test_instr_620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('adc     dx, cx')), u'\tR(ADC(dx, cx));\n')

    def test_instr_630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('adc     edx, ecx')), u'\tR(ADC(edx, ecx));\n')

    def test_instr_640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add     dl, cl')), u'\tR(ADD(dl, cl));\n')

    def test_instr_650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add     dx, cx')), u'\tR(ADD(dx, cx));\n')

    def test_instr_660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add     ebx, 4')), u'\tR(ADD(ebx, 4));\n')

    def test_instr_670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add     edx, ecx')), u'\tR(ADD(edx, ecx));\n')

    def test_instr_680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add     esp, 10h')), u'\tR(ADD(esp, 0x10));\n')

    def test_instr_690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add     esp, 114h')), u'\tR(ADD(esp, 0x114));\n')

    def test_instr_700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add     esp, 2')), u'\tR(ADD(esp, 2));\n')

    def test_instr_710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add     esp, 20h')), u'\tR(ADD(esp, 0x20));\n')

    def test_instr_720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add     esp, 4Ch')), u'\tR(ADD(esp, 0x4C));\n')

    def test_instr_730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add bl,30h          ; convert to ASCII')), u'\tR(ADD(bl, 0x30));\n')

    def test_instr_740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add bl,7            ; "A" to "F"')), u'\tR(ADD(bl, 7));\n')

    def test_instr_750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add edi,14*320')), u'\tR(ADD(edi, 14*320));\n')

    def test_instr_760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('add word ptr [var5+2],50')), u'\tR(ADD(*(dw*)(raddr(ds,offset(_data,var5)+2)), 50));\n')

    def test_instr_770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     ah, 0F7h')), u'\tR(AND(ah, 0x0F7));\n')

    def test_instr_780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     dl, cl')), u'\tR(AND(dl, cl));\n')

    def test_instr_790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     dx, cx')), u'\tR(AND(dx, cx));\n')

    def test_instr_800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     ecx, 40h')), u'\tR(AND(ecx, 0x40));\n')

    def test_instr_810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     edx, ecx')), u'\tR(AND(edx, ecx));\n')

    def test_instr_820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     eflags, 40h')), u'\tR(AND(eflags, 0x40));\n')

    def test_instr_830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     eflags, 8D5h')), u'\tR(AND(eflags, 0x8D5));\n')

    def test_instr_840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     esp, 0FFFFFFF0h')), u'\tR(AND(esp, 0x0FFFFFFF0));\n')

    def test_instr_850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     flags, 0D5h')), u'\tR(AND(flags, 0x0D5));\n')

    def test_instr_860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     flags, 1')), u'\tR(AND(flags, 1));\n')

    def test_instr_870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     flags, 11h')), u'\tR(AND(flags, 0x11));\n')

    def test_instr_880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     flags, 801h')), u'\tR(AND(flags, 0x801));\n')

    def test_instr_890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     flags, 8C5h')), u'\tR(AND(flags, 0x8C5));\n')

    def test_instr_900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and     flags, 8D5h')), u'\tR(AND(flags, 0x8D5));\n')

    def test_instr_910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('and bl,0Fh          ; only low-Nibble')), u'\tR(AND(bl, 0x0F));\n')

    def test_instr_920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsf     ax, bx')), u'\tR(BSF(ax, bx));\n')

    def test_instr_930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsf     ax, di')), u'\tR(BSF(ax, di));\n')

    def test_instr_940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsf     ax, si')), u'\tR(BSF(ax, si));\n')

    def test_instr_950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsf     eax, ebx')), u'\tR(BSF(eax, ebx));\n')

    def test_instr_960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsf     eax, edi')), u'\tR(BSF(eax, edi));\n')

    def test_instr_970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsr     ax, bx')), u'\tR(BSR(ax, bx));\n')

    def test_instr_980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsr     ax, di')), u'\tR(BSR(ax, di));\n')

    def test_instr_990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsr     eax, ebx')), u'\tR(BSR(eax, ebx));\n')

    def test_instr_1000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsr     eax, edx')), u'\tR(BSR(eax, edx));\n')

    def test_instr_1010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsr     eax, esi')), u'\tR(BSR(eax, esi));\n')

    def test_instr_1020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bsr     edx, eax')), u'\tR(BSR(edx, eax));\n')

    def test_instr_1030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bswap   eax')), u'\tR(BSWAP(eax));\n')

    def test_instr_1040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bt      cx, dx')), u'\tR(BT(cx, dx));\n')

    def test_instr_1050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bt      ecx, edx')), u'\tR(BT(ecx, edx));\n')

    def test_instr_1060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bt eax,0')), u'\tR(BT(eax, 0));\n')

    def test_instr_1070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bt eax,2')), u'\tR(BT(eax, 2));\n')

    def test_instr_1080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('btc     cx, dx')), u'\tR(BTC(cx, dx));\n')

    def test_instr_1090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('btc     ecx, edx')), u'\tR(BTC(ecx, edx));\n')

    def test_instr_1100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('btc eax,0')), u'\tR(BTC(eax, 0));\n')

    def test_instr_1110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('btc eax,2')), u'\tR(BTC(eax, 2));\n')

    def test_instr_1120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('btr     cx, dx')), u'\tR(BTR(cx, dx));\n')

    def test_instr_1130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('btr     ecx, edx')), u'\tR(BTR(ecx, edx));\n')

    def test_instr_1140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('btr eax,0')), u'\tR(BTR(eax, 0));\n')

    def test_instr_1150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('btr eax,2')), u'\tR(BTR(eax, 2));\n')

    def test_instr_1160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bts     cx, dx')), u'\tR(BTS(cx, dx));\n')

    def test_instr_1170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bts     ecx, edx')), u'\tR(BTS(ecx, edx));\n')

    def test_instr_1180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bts eax,0')), u'\tR(BTS(eax, 0));\n')

    def test_instr_1190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('bts eax,2')), u'\tR(BTS(eax, 2));\n')

    def test_instr_1200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cbw')), 'tR(CBW);\n')

    def test_instr_1210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cdq')), 'tR(CDQ);\n')

    def test_instr_1220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('clc')), 'tR(CLC);\n')

    def test_instr_1230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cld')), 'tR(CLD);\n')

    def test_instr_1240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmc')), 'tR(CMC);\n')

    def test_instr_1250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     dl, cl')), u'\tR(CMP(dl, cl));\n')

    def test_instr_1260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     dx, cx')), u'\tR(CMP(dx, cx));\n')

    def test_instr_1270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     ebx, ebx')), u'\tR(CMP(ebx, ebx));\n')

    def test_instr_1280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     ebx, edi')), u'\tR(CMP(ebx, edi));\n')

    def test_instr_1290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     ebx, esi')), u'\tR(CMP(ebx, esi));\n')

    def test_instr_1300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     ebx, offset unk_40F064')), u'\tR(CMP(ebx, offset(initcall,unk_40f064)));\n')

    def test_instr_1310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     ecx, 1')), u'\tR(CMP(ecx, 1));\n')

    def test_instr_1320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     edi, ebx')), u'\tR(CMP(edi, ebx));\n')

    def test_instr_1330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     edx, 1')), u'\tR(CMP(edx, 1));\n')

    def test_instr_1340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     edx, ecx')), u'\tR(CMP(edx, ecx));\n')

    def test_instr_1350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     esi, ebx')), u'\tR(CMP(esi, ebx));\n')

    def test_instr_1360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     esi, edi')), u'\tR(CMP(esi, edi));\n')

    def test_instr_1370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     esi, esi')), u'\tR(CMP(esi, esi));\n')

    def test_instr_1380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     i, 1000h')), u'\tR(CMP(i, 0x1000));\n')

    def test_instr_1390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     i, 100h')), u'\tR(CMP(i, 0x100));\n')

    def test_instr_1400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     i, 10h')), u'\tR(CMP(i, 0x10));\n')

    def test_instr_1410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     i, 20h')), u'\tR(CMP(i, 0x20));\n')

    def test_instr_1420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp     i, 4')), u'\tR(CMP(i, 4));\n')

    def test_instr_1430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [a],5')), u'\tR(CMP(*(raddr(ds,offset(_data,a))), 5));\n')

    def test_instr_1440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var+3],5')), u'\tR(CMP(*(raddr(ds,offset(_data,var)+3)), 5));\n')

    def test_instr_1450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var+4],0')), u'\tR(CMP(*(raddr(ds,offset(_data,var)+4)), 0));\n')

    def test_instr_1460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var-1],0')), u'\tR(CMP(*(raddr(ds,offset(_data,var)-1)), 0));\n')

    def test_instr_1470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var0+5],0')), u'\tR(CMP(*(raddr(ds,offset(_data,var0)+5)), 0));\n')

    def test_instr_1480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var1+1],5')), u'\tR(CMP(*(raddr(ds,offset(_data,var1)+1)), 5));\n')

    def test_instr_1490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var1],2')), u'\tR(CMP(m.var1, 2));\n')

    def test_instr_1500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var2+2],6')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var2)+2)), 6));\n')

    def test_instr_1510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var2-1],5')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var2)-1)), 5));\n')

    def test_instr_1520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var2],4')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var2))), 4));\n')

    def test_instr_1530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var3+3*4],4000000')), u'\tR(CMP(*(raddr(ds,offset(_data,var3)+3*4)), 4000000));\n')

    def test_instr_1540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var3+ebp],4000000')), u'\tR(CMP(*(raddr(ss,offset(_data,var3)+ebp)), 4000000));\n')

    def test_instr_1550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var4+t],1')), u'\tR(CMP(*(raddr(ds,offset(_data,var4)+t)), 1));\n')

    def test_instr_1560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var4],2')), u'\tR(CMP(*(raddr(ds,offset(_data,var4))), 2));\n')

    def test_instr_1570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp [var],5')), u'\tR(CMP(*(raddr(ds,offset(_data,var))), 5));\n')

    def test_instr_1580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ah,-1')), u'\tR(CMP(ah, -1));\n')

    def test_instr_1590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ah,0ffh')), u'\tR(CMP(ah, 0x0ff));\n')

    def test_instr_1600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp al,2')), u'\tR(CMP(al, 2));\n')

    def test_instr_1610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp al,ah')), u'\tR(CMP(al, ah));\n')

    def test_instr_1620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ax,-5')), u'\tR(CMP(ax, -5));\n')

    def test_instr_1630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bh,0cch')), u'\tR(CMP(bh, 0x0cc));\n')

    def test_instr_1640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bl,001111111B')), u'\tR(CMP(bl, 0x7f));\n')

    def test_instr_1650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bl,0ddh')), u'\tR(CMP(bl, 0x0dd));\n')

    def test_instr_1660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bl,192')), u'\tR(CMP(bl, 192));\n')

    def test_instr_1670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bl,193')), u'\tR(CMP(bl, 193));\n')

    def test_instr_1680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bl,39h          ; above 9?')), u'\tR(CMP(bl, 0x39));\n')

    def test_instr_1690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bl,al')), u'\tR(CMP(bl, al));\n')

    def test_instr_1700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bx,-1')), u'\tR(CMP(bx, -1));\n')

    def test_instr_1710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bx,1')), u'\tR(CMP(bx, 1));\n')

    def test_instr_1720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bx,4+5*256')), u'\tR(CMP(bx, 4+5*256));\n')

    def test_instr_1730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp bx,6*256+5')), u'\tR(CMP(bx, 6*256+5));\n')

    def test_instr_1740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr [a],5')), u'\tR(CMP(*(raddr(ds,offset(_data,a))), 5));\n')

    def test_instr_1750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr [edi+1],6')), u'\tR(CMP(*(raddr(ds,edi+1)), 6));\n')

    def test_instr_1760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr [edi+7],132')), u'\tR(CMP(*(raddr(ds,edi+7)), 132));\n')

    def test_instr_1770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr [esi],1')), u'\tR(CMP(*(raddr(ds,esi)), 1));\n')

    def test_instr_1780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr [esi],4')), u'\tR(CMP(*(raddr(ds,esi)), 4));\n')

    def test_instr_1790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr [testOVerlap+1],1')), u'\tR(CMP(*(raddr(ds,offset(_data,testoverlap)+1)), 1));\n')

    def test_instr_1800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr [var1+1],5')), u'\tR(CMP(*(raddr(ds,offset(_data,var1)+1)), 5));\n')

    def test_instr_1810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr [var1+2],5')), u'\tR(CMP(*(raddr(ds,offset(_data,var1)+2)), 5));\n')

    def test_instr_1820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr es:[0],55')), u'\tR(CMP(*(raddr(es,0)), 55));\n')

    def test_instr_1830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr es:[0],56')), u'\tR(CMP(*(raddr(es,0)), 56));\n')

    def test_instr_1840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp byte ptr es:[0],57')), u'\tR(CMP(*(raddr(es,0)), 57));\n')

    def test_instr_1850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ch,001111111B')), u'\tR(CMP(ch, 0x7f));\n')

    def test_instr_1860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ch,011111100B')), u'\tR(CMP(ch, 0xfc));\n')

    def test_instr_1870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp dl,2')), u'\tR(CMP(dl, 2));\n')

    def test_instr_1880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp dl,4')), u'\tR(CMP(dl, 4));\n')

    def test_instr_1890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp dl,5')), u'\tR(CMP(dl, 5));\n')

    def test_instr_1900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp dword ptr var4,11')), u'\tR(CMP(*(dd*)(raddr(ds,offset(_data,var4))), 11));\n')

    def test_instr_1910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp dx,-1')), u'\tR(CMP(dx, -1));\n')

    def test_instr_1920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp dx,0')), u'\tR(CMP(dx, 0));\n')

    def test_instr_1930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp dx,11')), u'\tR(CMP(dx, 11));\n')

    def test_instr_1940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp dx,5')), u'\tR(CMP(dx, 5));\n')

    def test_instr_1950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,-13')), u'\tR(CMP(eax, -13));\n')

    def test_instr_1960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,-2')), u'\tR(CMP(eax, -2));\n')

    def test_instr_1970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,-5')), u'\tR(CMP(eax, -5));\n')

    def test_instr_1980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,0')), u'\tR(CMP(eax, 0));\n')

    def test_instr_1990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,000f3h')), u'\tR(CMP(eax, 0x000f3));\n')

    def test_instr_2000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,0101010101010101b')), u'\tR(CMP(eax, 0x5555));\n')

    def test_instr_2010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,0101b')), u'\tR(CMP(eax, 0x5));\n')

    def test_instr_2020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,077123456h')), u'\tR(CMP(eax, 0x077123456));\n')

    def test_instr_2030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,0ffff0003h')), u'\tR(CMP(eax, 0x0ffff0003));\n')

    def test_instr_2040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,0ffff00f3h')), u'\tR(CMP(eax, 0x0ffff00f3));\n')

    def test_instr_2050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,0ffffff03h')), u'\tR(CMP(eax, 0x0ffffff03));\n')

    def test_instr_2060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,0fffffff3h')), u'\tR(CMP(eax, 0x0fffffff3));\n')

    def test_instr_2070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,1')), u'\tR(CMP(eax, 1));\n')

    def test_instr_2080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,2')), u'\tR(CMP(eax, 2));\n')

    def test_instr_2090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,256')), u'\tR(CMP(eax, 256));\n')

    def test_instr_2100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,3')), u'\tR(CMP(eax, 3));\n')

    def test_instr_2110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,4')), u'\tR(CMP(eax, 4));\n')

    def test_instr_2120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,5')), u'\tR(CMP(eax, 5));\n')

    def test_instr_2130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,6')), u'\tR(CMP(eax, 6));\n')

    def test_instr_2140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp eax,ebx')), u'\tR(CMP(eax, ebx));\n')

    def test_instr_2150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebp,9')), u'\tR(CMP(ebp, 9));\n')

    def test_instr_2160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebx,0')), u'\tR(CMP(ebx, 0));\n')

    def test_instr_2170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebx,010B')), u'\tR(CMP(ebx, 0x2));\n')

    def test_instr_2180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebx,0ffffff00h')), u'\tR(CMP(ebx, 0x0ffffff00));\n')

    def test_instr_2190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebx,1')), u'\tR(CMP(ebx, 1));\n')

    def test_instr_2200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebx,100h')), u'\tR(CMP(ebx, 0x100));\n')

    def test_instr_2210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebx,12345')), u'\tR(CMP(ebx, 12345));\n')

    def test_instr_2220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebx,2')), u'\tR(CMP(ebx, 2));\n')

    def test_instr_2230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebx,3')), u'\tR(CMP(ebx, 3));\n')

    def test_instr_2240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ebx,TWO')), u'\tR(CMP(ebx, TWO));\n')

    def test_instr_2250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ecx,-5')), u'\tR(CMP(ecx, -5));\n')

    def test_instr_2260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ecx,0af222h')), u'\tR(CMP(ecx, 0x0af222));\n')

    def test_instr_2270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ecx,0dff1h')), u'\tR(CMP(ecx, 0x0dff1));\n')

    def test_instr_2280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ecx,0ffffh')), u'\tR(CMP(ecx, 0x0ffff));\n')

    def test_instr_2290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ecx,3')), u'\tR(CMP(ecx, 3));\n')

    def test_instr_2300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp ecx,5')), u'\tR(CMP(ecx, 5));\n')

    def test_instr_2310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edi,0')), u'\tR(CMP(edi, 0));\n')

    def test_instr_2320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edi,esi')), u'\tR(CMP(edi, esi));\n')

    def test_instr_2330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edi,offset var4+1')), u'\tR(CMP(edi, offset(_data,var4)+1));\n')

    def test_instr_2340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edi,offset var4+4')), u'\tR(CMP(edi, offset(_data,var4)+4));\n')

    def test_instr_2350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edx,-2')), u'\tR(CMP(edx, -2));\n')

    def test_instr_2360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edx,0')), u'\tR(CMP(edx, 0));\n')

    def test_instr_2370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edx,0abcdef77h')), u'\tR(CMP(edx, 0x0abcdef77));\n')

    def test_instr_2380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edx,0ffffh')), u'\tR(CMP(edx, 0x0ffff));\n')

    def test_instr_2390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edx,1')), u'\tR(CMP(edx, 1));\n')

    def test_instr_2400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp edx,10')), u'\tR(CMP(edx, 10));\n')

    def test_instr_2410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp esi,0')), u'\tR(CMP(esi, 0));\n')

    def test_instr_2420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp esi,offset var1+1')), u'\tR(CMP(esi, offset(_data,var1)+1));\n')

    def test_instr_2430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp esi,offset var3+4')), u'\tR(CMP(esi, offset(_data,var3)+4));\n')

    def test_instr_2440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var1[1],2')), u'\tR(CMP(*(raddr(ds,offset(_data,var1)+1)), 2));\n')

    def test_instr_2450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var1[bx+si],2')), u'\tR(CMP(*(raddr(ds,offset(_data,var1)+bx+si)), 2));\n')

    def test_instr_2460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var1[bx],2')), u'\tR(CMP(*(raddr(ds,offset(_data,var1)+bx)), 2));\n')

    def test_instr_2470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var3+3*4,4000000')), u'\tR(CMP(*(raddr(ds,offset(_data,var3)+3*4)), 4000000));\n')

    def test_instr_2480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var3+ebp,4000000')), u'\tR(CMP(*(raddr(ds,offset(_data,var3)+ebp)), 4000000));\n')

    def test_instr_2490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp word ptr [var5+2],25')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var5)+2)), 25));\n')

    def test_instr_2500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp word ptr [var5+2],50')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var5)+2)), 50));\n')

    def test_instr_2510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp word ptr var5,0')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var5))), 0));\n')

    def test_instr_2520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpsb')), 'CMPSB;\n')

    def test_instr_2530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpsd')), 'tR(CMPSD);\n')

    def test_instr_2540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpsw')), 'tR(CMPSW);\n')

    def test_instr_2550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg al, dl')), u'\tR(CMPXCHG(al, dl));\n')

    def test_instr_2560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg ax, dx')), u'\tR(CMPXCHG(ax, dx));\n')

    def test_instr_2570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg byte ptr [ebp+var_20], bl')), u'\tR(CMPXCHG(*(raddr(ss,ebp+var_20)), bl));\n')

    def test_instr_2580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg byte ptr [ebp+var_20], dl')), u'\tR(CMPXCHG(*(raddr(ss,ebp+var_20)), dl));\n')

    def test_instr_2590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg cl, dl')), u'\tR(CMPXCHG(cl, dl));\n')

    def test_instr_2600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg cx, dx')), u'\tR(CMPXCHG(cx, dx));\n')

    def test_instr_2610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg dword ptr [ebp+var_20], edx')), u'\tR(CMPXCHG(*(dd*)(raddr(ss,ebp+var_20)), edx));\n')

    def test_instr_2620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg eax, edx')), u'\tR(CMPXCHG(eax, edx));\n')

    def test_instr_2630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg ecx, edx')), u'\tR(CMPXCHG(ecx, edx));\n')

    def test_instr_2640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg word ptr [ebp+var_20], dx')), u'\tR(CMPXCHG(*(dw*)(raddr(ss,ebp+var_20)), dx));\n')

    def test_instr_2650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmpxchg8b [ebp+var_20]')), u'\tR(CMPXCHG8B(*(dw*)(raddr(ss,ebp+var_20))));\n')

    def test_instr_2660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cwd')), 'tR(CWD);\n')

    def test_instr_2670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cwde')), 'tR(CWDE);\n')

    def test_instr_2680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('daa')), 'tR(DAA);\n')

    def test_instr_2690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('das')), 'tR(DAS);\n')

    def test_instr_2700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('dec     dl')), u'\tR(DEC(dl));\n')

    def test_instr_2710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('dec     dx')), u'\tR(DEC(dx));\n')

    def test_instr_2720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('dec     edx')), u'\tR(DEC(edx));\n')

    def test_instr_2730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('dec cl              ; decrease loop counter')), u'\tR(DEC(cl));\n')

    def test_instr_2740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('dec cx')), u'\tR(DEC(cx));\n')

    def test_instr_2750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('dec eax')), u'\tR(DEC(eax));\n')

    def test_instr_2760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('div     cx')), u'\tR(DIV2(cx));\n')

    def test_instr_2770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('div     dl')), u'\tR(DIV1(dl));\n')

    def test_instr_2780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('div     s1_0')), u'\tR(DIV0(s1_0));\n')

    def test_instr_2790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('idiv    cx')), u'\tR(IDIV2(cx));\n')

    def test_instr_2800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('idiv    dl')), u'\tR(IDIV1(dl));\n')

    def test_instr_2810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('idiv    s1_0')), u'\tR(IDIV0(s1_0));\n')

    def test_instr_2820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    ax, cx')), u'\tR(IMUL2_2(ax,cx));\n')

    def test_instr_2830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    ax, cx, 2Dh')), u'\tR(IMUL3_2(ax,cx,0x2D));\n')

    def test_instr_2840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    ax, di, 8000h')), u'\tR(IMUL3_2(ax,di,0x8000));\n')

    def test_instr_2850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    ax, dx, -2Dh')), u'\tR(IMUL3_2(ax,dx,-0x2D));\n')

    def test_instr_2860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    ax, si, 7FFFh')), u'\tR(IMUL3_2(ax,si,0x7FFF));\n')

    def test_instr_2870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    cl')), u'\tR(IMUL1_1(cl));\n')

    def test_instr_2880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    cx')), u'\tR(IMUL1_2(cx));\n')

    def test_instr_2890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    eax, ecx, 2Dh')), u'\tR(IMUL3_4(eax,ecx,0x2D));\n')

    def test_instr_2900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    eax, edi, 8000h')), u'\tR(IMUL3_4(eax,edi,0x8000));\n')

    def test_instr_2910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    eax, edx, -2Dh')), u'\tR(IMUL3_4(eax,edx,-0x2D));\n')

    def test_instr_2920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    eax, s1_0')), u'\tR(IMUL2_4(eax,s1_0));\n')

    def test_instr_2930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    ebx, esi, 7FFFh')), u'\tR(IMUL3_4(ebx,esi,0x7FFF));\n')

    def test_instr_2940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('imul    s1_0')), u'\tR(IMUL1_0(s1_0));\n')

    def test_instr_2950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc     dl')), u'\tR(INC(dl));\n')

    def test_instr_2960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc     dx')), u'\tR(INC(dx));\n')

    def test_instr_2970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc     edx')), u'\tR(INC(edx));\n')

    def test_instr_2980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc     i')), u'\tR(INC(i));\n')

    def test_instr_2990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc byte ptr [edi+1]')), u'\tR(INC(*(raddr(ds,edi+1))));\n')

    def test_instr_3000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc byte ptr [edi+7]')), u'\tR(INC(*(raddr(ds,edi+7))));\n')

    def test_instr_3010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc byte ptr es:[0]')), u'\tR(INC(*(raddr(es,0))));\n')

    def test_instr_3020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc eax')), u'\tR(INC(eax));\n')

    def test_instr_3030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc ebx')), u'\tR(INC(ebx));\n')

    def test_instr_3040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc ecx')), u'\tR(INC(ecx));\n')

    def test_instr_3050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc edi              ; increase target address')), u'\tR(INC(edi));\n')

    def test_instr_3060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc edi')), u'\tR(INC(edi));\n')

    def test_instr_3070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc edx')), u'\tR(INC(edx));\n')

    def test_instr_3080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('int 10h')), u'\tR(_INT(0x10));\n')

    def test_instr_3090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('int 21h                         ; DOS INT 21h')), u'\tR(_INT(0x21));\n')

    def test_instr_3100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('int 31h')), u'\tR(_INT(0x31));\n')

    def test_instr_3110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jC failure')), u'\ttR(JC(failure));\n')

    def test_instr_3120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jNC OK')), u'\ttR(JNC(ok));\n')

    def test_instr_3130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('ja      short loc_407784')), u'\ttR(JA(loc_407784));\n')

    def test_instr_3140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('ja failure')), u'\ttR(JA(failure));\n')

    def test_instr_3150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnbe failure')), u'\ttR(JA(failure));\n')

    def test_instr_3160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jb      short loc_40723E')), u'\ttR(JC(loc_40723e));\n')

    def test_instr_3170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jb failure  ; // because unsigned comparaiso\n')), u'\ttR(JC(failure));\n')

    def test_instr_3180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jbe     short loc_40752C')), u'\ttR(JBE(loc_40752c));\n')

    def test_instr_3190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jc failure')), u'\ttR(JC(failure));\n')

    def test_instr_3200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jcxz    loc_4081F6')), u'\ttR(JCXZ(loc_4081f6));\n')

    def test_instr_3210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jcxz @df@@@@7')), u'\ttR(JCXZ(arbdfarbarbarbarb7));\n')

    def test_instr_3220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jcxz failure')), u'\ttR(JCXZ(failure));\n')

    def test_instr_3230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('je failure ; http://blog.rewolf.pl/blog/?p=177')), u'\ttR(JZ(failure));\n')

    def test_instr_3240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('je failure')), u'\ttR(JZ(failure));\n')

    def test_instr_3250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('je ok')), u'\ttR(JZ(ok));\n')

    def test_instr_3260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jecxz   short loc_4083E9')), u'\ttR(JECXZ(loc_4083e9));\n')

    def test_instr_3270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jg      short loc_40707C')), u'\ttR(JG(loc_40707c));\n')

    def test_instr_3280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jg @df@@@@1')), u'\ttR(JG(arbdfarbarbarbarb1));\n')

    def test_instr_3290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jg failure')), u'\ttR(JG(failure));\n')

    def test_instr_3300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jge     short loc_406EBA')), u'\ttR(JGE(loc_406eba));\n')

    def test_instr_3310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jge @df@@@@2')), u'\ttR(JGE(arbdfarbarbarbarb2));\n')

    def test_instr_3320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jge failure')), u'\ttR(JGE(failure));\n')

    def test_instr_3330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jl      short loc_406B3F')), u'\ttR(JL(loc_406b3f));\n')

    def test_instr_3340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jl @df@@@@4')), u'\ttR(JL(arbdfarbarbarbarb4));\n')

    def test_instr_3350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jl failure')), u'\ttR(JL(failure));\n')

    def test_instr_3360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jle     short loc_406CF8')), u'\ttR(JLE(loc_406cf8));\n')

    def test_instr_3370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jle @df@@@@5')), u'\ttR(JLE(arbdfarbarbarbarb5));\n')

    def test_instr_3380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jle failure')), u'\ttR(JLE(failure));\n')

    def test_instr_3390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jmp     exec_rclb')), u'\ttR(JMP(exec_rclb));\n')

    def test_instr_3400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jmp     short loc_40D571')), u'\ttR(JMP(loc_40d571));\n')

    def test_instr_3410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jmp exitLabel')), u'\ttR(JMP(exitlabel));\n')

    def test_instr_3420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jmp failure')), u'\ttR(JMP(failure));\n')

    def test_instr_3430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jmp finTest')), u'\ttR(JMP(fintest));\n')

    def test_instr_3440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jmp next')), u'\ttR(JMP(next));\n')

    def test_instr_3450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnC failure')), u'\ttR(JNC(failure));\n')

    def test_instr_3460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jna short P2')), u'\ttR(JBE(p2));\n')

    def test_instr_3470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnb     short loc_4075C2')), u'\ttR(JNC(loc_4075c2));\n')

    def test_instr_3480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnb     short loc_407658')), u'\ttR(JNC(loc_407658));\n')

    def test_instr_3490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnb     short loc_4076EE')), u'\ttR(JNC(loc_4076ee));\n')

    def test_instr_3500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnb failure')), u'\ttR(JNC(failure));\n')

    def test_instr_3510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnc failure')), u'\ttR(JNC(failure));\n')

    def test_instr_3520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnc noerror')), u'\ttR(JNC(noerror));\n')

    def test_instr_3530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jne exitLabel')), u'\ttR(JNZ(exitlabel));\n')

    def test_instr_3540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jne failure')), u'\ttR(JNZ(failure));\n')

    def test_instr_3550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jns     short loc_408008')), u'\ttR(JNS(loc_408008));\n')

    def test_instr_3560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jns     short loc_40809E')), u'\ttR(JNS(loc_40809e));\n')

    def test_instr_3570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jns     short loc_408139')), u'\ttR(JNS(loc_408139));\n')

    def test_instr_3580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jns failure')), u'\ttR(JNS(failure));\n')

    def test_instr_3590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnz     loc_409652')), u'\ttR(JNZ(loc_409652));\n')

    def test_instr_3600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnz     short loc_4046D6')), u'\ttR(JNZ(loc_4046d6));\n')

    def test_instr_3610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnz P1              ; jump if cl is not equal 0 (zeroflag is not set)')), u'\ttR(JNZ(p1));\n')

    def test_instr_3620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jnz failure')), u'\ttR(JNZ(failure));\n')

    def test_instr_3630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('js      short loc_407E46')), u'\ttR(JS(loc_407e46));\n')

    def test_instr_3640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('js      short loc_407F72')), u'\ttR(JS(loc_407f72));\n')

    def test_instr_3650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('js @df@@@@')), u'\ttR(JS(arbdfarbarbarbarb));\n')

    def test_instr_3660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('js failure')), u'\ttR(JS(failure));\n')

    def test_instr_3670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jz      short loc_40458F')), u'\ttR(JZ(loc_40458f));\n')

    def test_instr_3680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jz failure')), u'\ttR(JZ(failure));\n')

    def test_instr_3690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [eax+4000h]')), u'\tR(eax = eax+0x4000);\n')

    def test_instr_3700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [eax+40h]')), u'\tR(eax = eax+0x40);\n')

    def test_instr_3710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [eax+ecx+40h]')), u'\tR(eax = eax+ecx+0x40);\n')

    def test_instr_3720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [eax+ecx]')), u'\tR(eax = eax+ecx);\n')

    def test_instr_3730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [eax]')), u'\tR(eax = eax);\n')

    def test_instr_3740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ebx+4000h]')), u'\tR(eax = ebx+0x4000);\n')

    def test_instr_3750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ebx+40h]')), u'\tR(eax = ebx+0x40);\n')

    def test_instr_3760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ebx+edx+4000h]')), u'\tR(eax = ebx+edx+0x4000);\n')

    def test_instr_3770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ebx+edx]')), u'\tR(eax = ebx+edx);\n')

    def test_instr_3780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ebx]')), u'\tR(eax = ebx);\n')

    def test_instr_3790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ecx+4000h]')), u'\tR(eax = ecx+0x4000);\n')

    def test_instr_3800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ecx+40h]')), u'\tR(eax = ecx+0x40);\n')

    def test_instr_3810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ecx+ecx*2+4000h]')), u'\tR(eax = ecx+ecx*2+0x4000);\n')

    def test_instr_3820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ecx+ecx*2-0Ah]')), u'\tR(eax = ecx+ecx*2-0x0A);\n')

    def test_instr_3830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ecx+ecx*2]')), u'\tR(eax = ecx+ecx*2);\n')

    def test_instr_3840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ecx+ecx]')), u'\tR(eax = ecx+ecx);\n')

    def test_instr_3850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [ecx]')), u'\tR(eax = ecx);\n')

    def test_instr_3860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edi+4000h]')), u'\tR(eax = edi+0x4000);\n')

    def test_instr_3870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edi+40h]')), u'\tR(eax = edi+0x40);\n')

    def test_instr_3880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edi+ecx]')), u'\tR(eax = edi+ecx);\n')

    def test_instr_3890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edi]')), u'\tR(eax = edi);\n')

    def test_instr_3900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edx+4000h]')), u'\tR(eax = edx+0x4000);\n')

    def test_instr_3910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edx+40h]')), u'\tR(eax = edx+0x40);\n')

    def test_instr_3920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edx+ecx*4+4000h]')), u'\tR(eax = edx+ecx*4+0x4000);\n')

    def test_instr_3930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edx+ecx*4-0Ah]')), u'\tR(eax = edx+ecx*4-0x0A);\n')

    def test_instr_3940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edx+ecx*4]')), u'\tR(eax = edx+ecx*4);\n')

    def test_instr_3950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edx+ecx]')), u'\tR(eax = edx+ecx);\n')

    def test_instr_3960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [edx]')), u'\tR(eax = edx);\n')

    def test_instr_3970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [esi+4000h]')), u'\tR(eax = esi+0x4000);\n')

    def test_instr_3980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [esi+40h]')), u'\tR(eax = esi+0x40);\n')

    def test_instr_3990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [esi+ecx*8-0Ah]')), u'\tR(eax = esi+ecx*8-0x0A);\n')

    def test_instr_4000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [esi+ecx*8]')), u'\tR(eax = esi+ecx*8);\n')

    def test_instr_4010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [esi+ecx]')), u'\tR(eax = esi+ecx);\n')

    def test_instr_4020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, [esi]')), u'\tR(eax = esi);\n')

    def test_instr_4030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, ds:0[eax*2]')), u'\tR(eax = 0+eax*2);\n')

    def test_instr_4040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, ds:0[ebx*4]')), u'\tR(eax = 0+ebx*4);\n')

    def test_instr_4050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, ds:0[ecx*8]')), u'\tR(eax = 0+ecx*8);\n')

    def test_instr_4060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     ebx, [ebp+table]')), u'\tR(ebx = ebp+offset(_text,table));\n')

    def test_instr_4070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     ebx, [esi+ecx*8+4000h]')), u'\tR(ebx = esi+ecx*8+0x4000);\n')

    def test_instr_4080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     edi, [ebp+ecx_vals]')), u'\tR(edi = ebp+ecx_vals);\n')

    def test_instr_4090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     edi, [i+2]')), u'\tR(edi = i+2);\n')

    def test_instr_4100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     edi, [i+3]')), u'\tR(edi = i+3);\n')

    def test_instr_4110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     edi, [i+4]')), u'\tR(edi = i+4);\n')

    def test_instr_4120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     edi, [i+5]')), u'\tR(edi = i+5);\n')

    def test_instr_4130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     edi, [i-10h]')), u'\tR(edi = i-0x10);\n')

    def test_instr_4140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     edx, [i+56h]')), u'\tR(edx = i+0x56);\n')

    def test_instr_4150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     esi, [i+1]')), u'\tR(esi = i+1);\n')

    def test_instr_4160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea eax,enddata')), u'\tR(eax = offset(_data,enddata));\n')

    def test_instr_4170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea ebx,beginningdata')), u'\tR(ebx = offset(_data,beginningdata));\n')

    def test_instr_4180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea edi,buffer')), u'\tR(edi = offset(_data,buffer));\n')

    def test_instr_4190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea edi,f')), u'\tR(edi = offset(_data,f));\n')

    def test_instr_4200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea edi,testOVerlap')), u'\tR(edi = offset(_data,testoverlap));\n')

    def test_instr_4210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea edi,var1')), u'\tR(edi = offset(_data,var1));\n')

    def test_instr_4220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea edx,fileName')), u'\tR(edx = offset(_data,filename));\n')

    def test_instr_4230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea esi,b')), u'\tR(esi = offset(_data,b));\n')

    def test_instr_4240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea esi,f')), u'\tR(esi = offset(_data,f));\n')

    def test_instr_4250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea esi,var2')), u'\tR(esi = offset(_data,var2));\n')

    def test_instr_4260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea esi,var5')), u'\tR(esi = offset(_data,var5));\n')

    def test_instr_4270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('leave')), 'tR(MOV(esp, ebp));nR(POP(ebp));\n')

    def test_instr_4280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lodsb')), 'LODSB;\n')

    def test_instr_4290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lodsd')), 'LODSD;\n')

    def test_instr_4300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lodsw')), 'LODSW;\n')

    def test_instr_4310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('loop    loc_408464')), u'\ttR(LOOP(loc_408464));\n')

    def test_instr_4320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('loop dffd')), u'\ttR(LOOP(dffd));\n')

    def test_instr_4330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('loope   loc_4084DF')), u'\ttR(LOOPE(loc_4084df));\n')

    def test_instr_4340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('loope toto1')), u'\ttR(LOOPE(toto1));\n')

    def test_instr_4350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('loopne  loc_40855A')), u'\ttR(LOOPNE(loc_40855a));\n')

    def test_instr_4360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [ebp+ecx_0], ecx_0_0')), u'\tR(MOV(*(dw*)(raddr(ss,ebp+ecx_0)), ecx_0_0));\n')

    def test_instr_4370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [ebp+edx_0], edx')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+edx_0)), edx));\n')

    def test_instr_4380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [ebp+s0], esi')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+s0)), esi));\n')

    def test_instr_4390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [ebp+s1], 0')), u'\tR(MOV(*(raddr(ss,ebp+s1)), 0));\n')

    def test_instr_4400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [ebp+s1], 1')), u'\tR(MOV(*(raddr(ss,ebp+s1)), 1));\n')

    def test_instr_4410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [ebp+s2], ebx')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+s2)), ebx));\n')

    def test_instr_4420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [ebp+var_1C], edx')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+var_1C)), edx));\n')

    def test_instr_4430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [ebp+var_20], ecx')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+var_20)), ecx));\n')

    def test_instr_4440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], ebx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), ebx));\n')

    def test_instr_4450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], ecx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), ecx));\n')

    def test_instr_4460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], edi')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), edi));\n')

    def test_instr_4470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], edi_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x0C)), edi_0));\n')

    def test_instr_4480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], edx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), edx));\n')

    def test_instr_4490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], op0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x0C)), op0));\n')

    def test_instr_4500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], op1')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x0C)), op1));\n')

    def test_instr_4510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], r')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x0C)), r));\n')

    def test_instr_4520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], res')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x0C)), res));\n')

    def test_instr_4530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], resz')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x0C)), resz));\n')

    def test_instr_4540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+0Ch], s1_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x0C)), s1_0));\n')

    def test_instr_4550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+10h], eax')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x10)), eax));\n')

    def test_instr_4560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+10h], eax_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x10)), eax_0));\n')

    def test_instr_4570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+10h], ebx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x10)), ebx));\n')

    def test_instr_4580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+10h], ecx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x10)), ecx));\n')

    def test_instr_4590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+10h], op1')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x10)), op1));\n')

    def test_instr_4600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+10h], res')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x10)), res));\n')

    def test_instr_4610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+10h], resz')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x10)), resz));\n')

    def test_instr_4620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+10h], rh')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x10)), rh));\n')

    def test_instr_4630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+10h], s1_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x10)), s1_0));\n')

    def test_instr_4640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], eax')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x14)), eax));\n')

    def test_instr_4650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], ebx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x14)), ebx));\n')

    def test_instr_4660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], ecx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x14)), ecx));\n')

    def test_instr_4670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], ecx_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x14)), ecx_0));\n')

    def test_instr_4680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], edi')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x14)), edi));\n')

    def test_instr_4690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], edx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x14)), edx));\n')

    def test_instr_4700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], esi')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x14)), esi));\n')

    def test_instr_4710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], flags')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x14)), flags));\n')

    def test_instr_4720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], res')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x14)), res));\n')

    def test_instr_4730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+14h], resh')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x14)), resh));\n')

    def test_instr_4740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+18h], eax')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x18)), eax));\n')

    def test_instr_4750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+18h], edi')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x18)), edi));\n')

    def test_instr_4760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+18h], edx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x18)), edx));\n')

    def test_instr_4770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+18h], res')), u'\tR(MOV(*(dw*)(raddr(ss,esp+0x18)), res));\n')

    def test_instr_4780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+1Ch], eax')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x1C)), eax));\n')

    def test_instr_4790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+1Ch], ebx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x1C)), ebx));\n')

    def test_instr_4800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+4], eax_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+4)), eax_0));\n')

    def test_instr_4810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+4], ebx    ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), ebx));\n')

    def test_instr_4820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+4], edi    ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), edi));\n')

    def test_instr_4830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+4], esi    ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), esi));\n')

    def test_instr_4840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+4], esi    ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), esi));\n')

    def test_instr_4850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+4], i      ; op0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+4)), i));\n')

    def test_instr_4860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+4], res')), u'\tR(MOV(*(dw*)(raddr(ss,esp+4)), res));\n')

    def test_instr_4870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], eax')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), eax));\n')

    def test_instr_4880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], ebx    ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), ebx));\n')

    def test_instr_4890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], ebx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), ebx));\n')

    def test_instr_4900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], ecx')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), ecx));\n')

    def test_instr_4910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], ecx_0_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+8)), ecx_0_0));\n')

    def test_instr_4920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], edi    ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), edi));\n')

    def test_instr_4930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], edi    ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), edi));\n')

    def test_instr_4940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], edx_0_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+8)), edx_0_0));\n')

    def test_instr_4950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], esi    ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), esi));\n')

    def test_instr_4960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], esi    ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), esi));\n')

    def test_instr_4970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], esi')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), esi));\n')

    def test_instr_4980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], esi_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+8)), esi_0));\n')

    def test_instr_4990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], i      ; s1')), u'\tR(MOV(*(dw*)(raddr(ss,esp+8)), i));\n')

    def test_instr_5000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], i')), u'\tR(MOV(*(dw*)(raddr(ss,esp+8)), i));\n')

    def test_instr_5010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], op0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+8)), op0));\n')

    def test_instr_5020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], res')), u'\tR(MOV(*(dw*)(raddr(ss,esp+8)), res));\n')

    def test_instr_5030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], resh')), u'\tR(MOV(*(dw*)(raddr(ss,esp+8)), resh));\n')

    def test_instr_5040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], s0_0')), u'\tR(MOV(*(dw*)(raddr(ss,esp+8)), s0_0));\n')

    def test_instr_5050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp], ebx      ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), ebx));\n')

    def test_instr_5060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp], ebx      ; s2')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), ebx));\n')

    def test_instr_5070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp], edi      ; s2')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), edi));\n')

    def test_instr_5080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dl, al')), u'\tR(MOV(dl, al));\n')

    def test_instr_5090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [ebp+var_20+4], 0FBCA7h')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+var_20+4)), 0x0FBCA7));\n')

    def test_instr_5100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [ebp+var_20+4], 12345h')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+var_20+4)), 0x12345));\n')

    def test_instr_5110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [ebp+var_20], 0FBCA7654h')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+var_20)), 0x0FBCA7654));\n')

    def test_instr_5120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [ebp+var_20], 65423456h')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+var_20)), 0x65423456));\n')

    def test_instr_5130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [ebp+var_20], 6789ABCDh')), u'\tR(MOV(*(dd*)(raddr(ss,ebp+var_20)), 0x6789ABCD));\n')

    def test_instr_5140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+0Ch], 0 ; iflags')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), 0));\n')

    def test_instr_5150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+0Ch], 0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), 0));\n')

    def test_instr_5160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+0Ch], 1 ; iflags')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), 1));\n')

    def test_instr_5170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+0Ch], 1000h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), 0x1000));\n')

    def test_instr_5180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+0Ch], 1234h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), 0x1234));\n')

    def test_instr_5190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+0Ch], 17h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), 0x17));\n')

    def test_instr_5200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+0Ch], 80000000h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x0C)), 0x80000000));\n')

    def test_instr_5210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+10h], 0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x10)), 0));\n')

    def test_instr_5220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+10h], 1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x10)), 1));\n')

    def test_instr_5230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+10h], 10h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x10)), 0x10));\n')

    def test_instr_5240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+10h], 11h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x10)), 0x11));\n')

    def test_instr_5250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+14h], 0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x14)), 0));\n')

    def test_instr_5260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+1Ch], 0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+0x1C)), 0));\n')

    def test_instr_5270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 0 ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0));\n')

    def test_instr_5280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 0 ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0));\n')

    def test_instr_5290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0));\n')

    def test_instr_5300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 0FFFC70F9h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x0FFFC70F9));\n')

    def test_instr_5310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 0FFFFFFD3h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x0FFFFFFD3));\n')

    def test_instr_5320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 0FFFFFFFFh ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x0FFFFFFFF));\n')

    def test_instr_5330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 1 ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 1));\n')

    def test_instr_5340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 1 ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 1));\n')

    def test_instr_5350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 10000h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x10000));\n')

    def test_instr_5360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 10000h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x10000));\n')

    def test_instr_5370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 10000h ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x10000));\n')

    def test_instr_5380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 100h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x100));\n')

    def test_instr_5390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 100h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x100));\n')

    def test_instr_5400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 10h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x10));\n')

    def test_instr_5410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 1234001Dh ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x1234001D));\n')

    def test_instr_5420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 12341h ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x12341));\n')

    def test_instr_5430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 12345678h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x12345678));\n')

    def test_instr_5440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 12345678h ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x12345678));\n')

    def test_instr_5450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 12348000h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x12348000));\n')

    def test_instr_5460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 127Eh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x127E));\n')

    def test_instr_5470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 17h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x17));\n')

    def test_instr_5480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 1FF7Fh ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x1FF7F));\n')

    def test_instr_5490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 1FF80h ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x1FF80));\n')

    def test_instr_5500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 1FF81h ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x1FF81));\n')

    def test_instr_5510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 1FFFFh ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x1FFFF));\n')

    def test_instr_5520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 2 ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 2));\n')

    def test_instr_5530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 2 ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 2));\n')

    def test_instr_5540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 20000h ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x20000));\n')

    def test_instr_5550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 2Dh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x2D));\n')

    def test_instr_5560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 3 ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 3));\n')

    def test_instr_5570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 3 ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 3));\n')

    def test_instr_5580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 4 ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 4));\n')

    def test_instr_5590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 7FFFFFFFh ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x7FFFFFFF));\n')

    def test_instr_5600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 80000000h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x80000000));\n')

    def test_instr_5610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 80000000h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x80000000));\n')

    def test_instr_5620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 80000001h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x80000001));\n')

    def test_instr_5630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 80008688h ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x80008688));\n')

    def test_instr_5640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 8000h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x8000));\n')

    def test_instr_5650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 8000h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x8000));\n')

    def test_instr_5660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 80h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x80));\n')

    def test_instr_5670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 80h ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x80));\n')

    def test_instr_5680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 812FADAh ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x812FADA));\n')

    def test_instr_5690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 81h ; s1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x81));\n')

    def test_instr_5700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], 82345679h ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), 0x82345679));\n')

    def test_instr_5710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+4], offset aXorw ; "xorw"')), u'\tR(MOV(*(dd*)(raddr(ss,esp+4)), offset(_rdata,axorw)));\n')

    def test_instr_5720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0 ; iflags')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0));\n')

    def test_instr_5730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0 ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0));\n')

    def test_instr_5740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0));\n')

    def test_instr_5750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0FBCA7654h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0FBCA7654));\n')

    def test_instr_5760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0FFFFFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0FFFFFFFF));\n')

    def test_instr_5770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0FFFFFFFFh')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0FFFFFFFF));\n')

    def test_instr_5780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0FFFFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0FFFFFFF));\n')

    def test_instr_5790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0FFFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0FFFFFF));\n')

    def test_instr_5800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0FFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0FFFFF));\n')

    def test_instr_5810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0FFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0FFFF));\n')

    def test_instr_5820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0FFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0FFF));\n')

    def test_instr_5830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0FFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0FF));\n')

    def test_instr_5840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 0Fh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x0F));\n')

    def test_instr_5850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 1 ; iflags')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 1));\n')

    def test_instr_5860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 1 ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 1));\n')

    def test_instr_5870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 10000h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x10000));\n')

    def test_instr_5880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 100h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x100));\n')

    def test_instr_5890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 12340128h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x12340128));\n')

    def test_instr_5900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 12Ch ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x12C));\n')

    def test_instr_5910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 1FFFFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x1FFFFFFF));\n')

    def test_instr_5920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 1FFFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x1FFFFFF));\n')

    def test_instr_5930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 1FFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x1FFFFF));\n')

    def test_instr_5940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 1FFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x1FFFF));\n')

    def test_instr_5950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 1FFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x1FFF));\n')

    def test_instr_5960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 1FFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x1FF));\n')

    def test_instr_5970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 1Fh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x1F));\n')

    def test_instr_5980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 2 ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 2));\n')

    def test_instr_5990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 2Dh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x2D));\n')

    def test_instr_6000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 2Dh')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x2D));\n')

    def test_instr_6010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 3 ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 3));\n')

    def test_instr_6020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 303Bh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x303B));\n')

    def test_instr_6030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 340128h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x340128));\n')

    def test_instr_6040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 3FFFFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x3FFFFFFF));\n')

    def test_instr_6050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 3FFFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x3FFFFFF));\n')

    def test_instr_6060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 3FFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x3FFFFF));\n')

    def test_instr_6070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 3FFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x3FFFF));\n')

    def test_instr_6080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 3FFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x3FFF));\n')

    def test_instr_6090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 3FFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x3FF));\n')

    def test_instr_6100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 3Fh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x3F));\n')

    def test_instr_6110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 7FFFFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x7FFFFFFF));\n')

    def test_instr_6120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 7FFFFFFFh')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x7FFFFFFF));\n')

    def test_instr_6130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 7FFFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x7FFFFFF));\n')

    def test_instr_6140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 7FFFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x7FFFFF));\n')

    def test_instr_6150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 7FFFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x7FFFF));\n')

    def test_instr_6160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 7FFFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x7FFF));\n')

    def test_instr_6170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 7FFFh')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x7FFF));\n')

    def test_instr_6180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 7FFh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x7FF));\n')

    def test_instr_6190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 7Fh ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x7F));\n')

    def test_instr_6200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 80000000h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x80000000));\n')

    def test_instr_6210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 8000h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x8000));\n')

    def test_instr_6220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 8000h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x8000));\n')

    def test_instr_6230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 81234567h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x81234567));\n')

    def test_instr_6240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 81238567h ; op1')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x81238567));\n')

    def test_instr_6250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp+8], 8234A6F8h')), u'\tR(MOV(*(dd*)(raddr(ss,esp+8)), 0x8234A6F8));\n')

    def test_instr_6260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0 ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0));\n')

    def test_instr_6270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0 ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0));\n')

    def test_instr_6280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0Eh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0E));\n')

    def test_instr_6290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FE));\n')

    def test_instr_6300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFE));\n')

    def test_instr_6310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFE0080h ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFE0080));\n')

    def test_instr_6320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFE0080h ; s2')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFE0080));\n')

    def test_instr_6330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFE));\n')

    def test_instr_6340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFE));\n')

    def test_instr_6350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFFE));\n')

    def test_instr_6360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFFFECh ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFFFEC));\n')

    def test_instr_6370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFFFECh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFFFEC));\n')

    def test_instr_6380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFFFE));\n')

    def test_instr_6390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFFFFDh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFFFFD));\n')

    def test_instr_6400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFFFFE));\n')

    def test_instr_6410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFFFFFh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFFFFF));\n')

    def test_instr_6420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFFFFFh ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFFFFF));\n')

    def test_instr_6430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 0FFFFFFFFh ; s2')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x0FFFFFFFF));\n')

    def test_instr_6440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 1 ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 1));\n')

    def test_instr_6450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 10000h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x10000));\n')

    def test_instr_6460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 100h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x100));\n')

    def test_instr_6470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 10h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x10));\n')

    def test_instr_6480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 12340004h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x12340004));\n')

    def test_instr_6490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 12341h ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x12341));\n')

    def test_instr_6500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 12343h ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x12343));\n')

    def test_instr_6510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 1234561Dh ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x1234561D));\n')

    def test_instr_6520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 14h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x14));\n')

    def test_instr_6530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 14h ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x14));\n')

    def test_instr_6540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 17h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x17));\n')

    def test_instr_6550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 1Eh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x1E));\n')

    def test_instr_6560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 1FEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x1FE));\n')

    def test_instr_6570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 1FFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x1FFE));\n')

    def test_instr_6580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 1FFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x1FFFE));\n')

    def test_instr_6590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 1FFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x1FFFFE));\n')

    def test_instr_6600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 1FFFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x1FFFFFE));\n')

    def test_instr_6610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 1FFFFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x1FFFFFFE));\n')

    def test_instr_6620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 2 ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 2));\n')

    def test_instr_6630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 21AD3D34h ; s2')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x21AD3D34));\n')

    def test_instr_6640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 3 ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 3));\n')

    def test_instr_6650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 3 ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 3));\n')

    def test_instr_6660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 3Eh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x3E));\n')

    def test_instr_6670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 3FEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x3FE));\n')

    def test_instr_6680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 3FFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x3FFE));\n')

    def test_instr_6690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 3FFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x3FFFE));\n')

    def test_instr_6700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 3FFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x3FFFFE));\n')

    def test_instr_6710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 3FFFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x3FFFFFE));\n')

    def test_instr_6720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 3FFFFFFEh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x3FFFFFFE));\n')

    def test_instr_6730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 4 ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 4));\n')

    def test_instr_6740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 43210123h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x43210123));\n')

    def test_instr_6750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 7Eh ; op0h')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x7E));\n')

    def test_instr_6760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 7FFFFFFFh ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x7FFFFFFF));\n')

    def test_instr_6770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 80000000h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x80000000));\n')

    def test_instr_6780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 80000000h ; s0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x80000000));\n')

    def test_instr_6790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 80008481h ; s2')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x80008481));\n')

    def test_instr_6800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 8000h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x8000));\n')

    def test_instr_6810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 80h ; op0')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x80));\n')

    def test_instr_6820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], 813F3421h ; s2')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), 0x813F3421));\n')

    def test_instr_6830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], offset a10sA08lxB08lx ; "%-10s A=%08lx B=%08lx\n"')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), offset(_rdata,a10sa08lxb08lx)));\n')

    def test_instr_6840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], offset a10sAh08lxAl08l ; "%-10s AH=%08lx AL=%08lx B=%08lx RH=%08l"...')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), offset(_rdata,a10sah08lxal08l)));\n')

    def test_instr_6850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], offset a10sD ; "%-10s %d\n"')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), offset(_rdata,a10sd)));\n')

    def test_instr_6860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     dword ptr [esp], offset a10sEax08lxA08l ; "%-10s EAX=%08lx A=%08lx C=%08lx CC=%02l"...')), u'\tR(MOV(*(dd*)(raddr(ss,esp)), offset(_rdata,a10seax08lxa08l)));\n')

    def test_instr_6870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, 0')), u'\tR(MOV(eax, 0));\n')

    def test_instr_6880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, 0FFFF7FFFh')), u'\tR(MOV(eax, 0x0FFFF7FFF));\n')

    def test_instr_6890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, 1')), u'\tR(MOV(eax, 1));\n')

    def test_instr_6900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, 12340407h')), u'\tR(MOV(eax, 0x12340407));\n')

    def test_instr_6910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, 7FFFFFFFh')), u'\tR(MOV(eax, 0x7FFFFFFF));\n')

    def test_instr_6920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, dword ptr [ebp+var_20]')), u'\tR(MOV(eax, *(dd*)(raddr(ss,ebp+var_20))));\n')

    def test_instr_6930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, ebx')), u'\tR(MOV(eax, ebx));\n')

    def test_instr_6940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, edi')), u'\tR(MOV(eax, edi));\n')

    def test_instr_6950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, edx')), u'\tR(MOV(eax, edx));\n')

    def test_instr_6960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, esi')), u'\tR(MOV(eax, esi));\n')

    def test_instr_6970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, flags')), u'\tR(MOV(eax, flags));\n')

    def test_instr_6980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, res')), u'\tR(MOV(eax, res));\n')

    def test_instr_6990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     eax, s0_0')), u'\tR(MOV(eax, s0_0));\n')

    def test_instr_7000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebp, esp')), u'\tR(MOV(ebp, esp));\n')

    def test_instr_7010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, (offset str_buffer+800h)')), u'\tR(MOV(ebx, offset(_bss,str_buffer)+0x800));\n')

    def test_instr_7020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, 1')), u'\tR(MOV(ebx, 1));\n')

    def test_instr_7030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, 1234040Ah')), u'\tR(MOV(ebx, 0x1234040A));\n')

    def test_instr_7040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, 12340506h')), u'\tR(MOV(ebx, 0x12340506));\n')

    def test_instr_7050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, 12345678h')), u'\tR(MOV(ebx, 0x12345678));\n')

    def test_instr_7060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, 2')), u'\tR(MOV(ebx, 2));\n')

    def test_instr_7070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, 8234A6F8h')), u'\tR(MOV(ebx, 0x8234A6F8));\n')

    def test_instr_7080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, [ebp+iflags]')), u'\tR(MOV(ebx, *(dd*)(raddr(ss,ebp+iflags))));\n')

    def test_instr_7090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, [ebp+op0h]')), u'\tR(MOV(ebx, *(dd*)(raddr(ss,ebp+op0h))));\n')

    def test_instr_7100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, [ebp+s0]')), u'\tR(MOV(ebx, *(dd*)(raddr(ss,ebp+s0))));\n')

    def test_instr_7110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, [ebp+s2]')), u'\tR(MOV(ebx, *(dd*)(raddr(ss,ebp+s2))));\n')

    def test_instr_7120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, [ebp+var_4]')), u'\tR(MOV(ebx, *(dd*)(raddr(ss,ebp+var_4))));\n')

    def test_instr_7130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, dword ptr [ebp+var_20+4]')), u'\tR(MOV(ebx, *(dd*)(raddr(ss,ebp+var_20+4))));\n')

    def test_instr_7140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, edi')), u'\tR(MOV(ebx, edi));\n')

    def test_instr_7150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, i')), u'\tR(MOV(ebx, i));\n')

    def test_instr_7160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, 1')), u'\tR(MOV(ecx, 1));\n')

    def test_instr_7170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, 10h')), u'\tR(MOV(ecx, 0x10));\n')

    def test_instr_7180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, 11h')), u'\tR(MOV(ecx, 0x11));\n')

    def test_instr_7190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, 1234h')), u'\tR(MOV(ecx, 0x1234));\n')

    def test_instr_7200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, 4')), u'\tR(MOV(ecx, 4));\n')

    def test_instr_7210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, 65324h')), u'\tR(MOV(ecx, 0x65324));\n')

    def test_instr_7220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, [ebp+ecx_0]')), u'\tR(MOV(ecx, *(dd*)(raddr(ss,ebp+ecx_0))));\n')

    def test_instr_7230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, [ebp+edx_0]')), u'\tR(MOV(ecx, *(dd*)(raddr(ss,ebp+edx_0))));\n')

    def test_instr_7240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, [ebp+i*4+ecx_vals]')), u'\tR(MOV(ecx, *(dd*)(raddr(ss,ebp+i*4+ecx_vals))));\n')

    def test_instr_7250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, [ebp+s0]')), u'\tR(MOV(ecx, *(dd*)(raddr(ss,ebp+s0))));\n')

    def test_instr_7260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, [ebp+s1]')), u'\tR(MOV(ecx, *(dd*)(raddr(ss,ebp+s1))));\n')

    def test_instr_7270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, [ebp+var_20]')), u'\tR(MOV(ecx, *(dd*)(raddr(ss,ebp+var_20))));\n')

    def test_instr_7280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, dword ptr [ebp+var_20+4]')), u'\tR(MOV(ecx, *(dd*)(raddr(ss,ebp+var_20+4))));\n')

    def test_instr_7290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, dword ptr [ebp+var_20]')), u'\tR(MOV(ecx, *(dd*)(raddr(ss,ebp+var_20))));\n')

    def test_instr_7300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, edi')), u'\tR(MOV(ecx, edi));\n')

    def test_instr_7310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ecx, res')), u'\tR(MOV(ecx, res));\n')

    def test_instr_7320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, (offset str_buffer+810h)')), u'\tR(MOV(edi, offset(_bss,str_buffer)+0x810));\n')

    def test_instr_7330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, 0FBCA7654h')), u'\tR(MOV(edi, 0x0FBCA7654));\n')

    def test_instr_7340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, 0FFFFFFF7h')), u'\tR(MOV(edi, 0x0FFFFFFF7));\n')

    def test_instr_7350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, 1')), u'\tR(MOV(edi, 1));\n')

    def test_instr_7360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, 12340128h')), u'\tR(MOV(edi, 0x12340128));\n')

    def test_instr_7370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, 12340205h')), u'\tR(MOV(edi, 0x12340205));\n')

    def test_instr_7380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, 123405A0h')), u'\tR(MOV(edi, 0x123405A0));\n')

    def test_instr_7390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, 12345h')), u'\tR(MOV(edi, 0x12345));\n')

    def test_instr_7400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, 20h')), u'\tR(MOV(edi, 0x20));\n')

    def test_instr_7410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, 80000000h')), u'\tR(MOV(edi, 0x80000000));\n')

    def test_instr_7420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, [ebp+iflags]')), u'\tR(MOV(edi, *(dd*)(raddr(ss,ebp+iflags))));\n')

    def test_instr_7430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, [ebp+op0]')), u'\tR(MOV(edi, *(dd*)(raddr(ss,ebp+op0))));\n')

    def test_instr_7440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, [ebp+s1]')), u'\tR(MOV(edi, *(dd*)(raddr(ss,ebp+s1))));\n')

    def test_instr_7450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi, [ebp+s2]')), u'\tR(MOV(edi, *(dd*)(raddr(ss,ebp+s2))));\n')

    def test_instr_7460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edi_0, (offset str_buffer+810h)')), u'\tR(MOV(edi_0, offset(_bss,str_buffer)+0x810));\n')

    def test_instr_7470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, 1')), u'\tR(MOV(edx, 1));\n')

    def test_instr_7480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, 10h')), u'\tR(MOV(edx, 0x10));\n')

    def test_instr_7490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, 11h')), u'\tR(MOV(edx, 0x11));\n')

    def test_instr_7500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, 12340507h')), u'\tR(MOV(edx, 0x12340507));\n')

    def test_instr_7510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, 12345678h')), u'\tR(MOV(edx, 0x12345678));\n')

    def test_instr_7520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, 17h')), u'\tR(MOV(edx, 0x17));\n')

    def test_instr_7530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, 340128h')), u'\tR(MOV(edx, 0x340128));\n')

    def test_instr_7540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, 8')), u'\tR(MOV(edx, 8));\n')

    def test_instr_7550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, [ebp+s1]')), u'\tR(MOV(edx, *(dd*)(raddr(ss,ebp+s1))));\n')

    def test_instr_7560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, [ebp+var_1C]')), u'\tR(MOV(edx, *(dd*)(raddr(ss,ebp+var_1C))));\n')

    def test_instr_7570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, dword ptr [ebp+var_20]')), u'\tR(MOV(edx, *(dd*)(raddr(ss,ebp+var_20))));\n')

    def test_instr_7580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, ebx')), u'\tR(MOV(edx, ebx));\n')

    def test_instr_7590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, edi')), u'\tR(MOV(edx, edi));\n')

    def test_instr_7600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, esi')), u'\tR(MOV(edx, esi));\n')

    def test_instr_7610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, res')), u'\tR(MOV(edx, res));\n')

    def test_instr_7620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, resh')), u'\tR(MOV(edx, resh));\n')

    def test_instr_7630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     edx, dword ptr [ebp+var_20]')), u'\tR(MOV(edx, *(dd*)(raddr(ss,ebp+var_20))));\n')

    def test_instr_7640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, 0FFFEFDFCh')), u'\tR(MOV(esi, 0x0FFFEFDFC));\n')

    def test_instr_7650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, 1000h')), u'\tR(MOV(esi, 0x1000));\n')

    def test_instr_7660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, 10h')), u'\tR(MOV(esi, 0x10));\n')

    def test_instr_7670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, 12340306h')), u'\tR(MOV(esi, 0x12340306));\n')

    def test_instr_7680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, [ebp+iflags]')), u'\tR(MOV(esi, *(dd*)(raddr(ss,ebp+iflags))));\n')

    def test_instr_7690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, [ebp+op0]')), u'\tR(MOV(esi, *(dd*)(raddr(ss,ebp+op0))));\n')

    def test_instr_7700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, [ebp+op0h]')), u'\tR(MOV(esi, *(dd*)(raddr(ss,ebp+op0h))));\n')

    def test_instr_7710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, [ebp+s0]')), u'\tR(MOV(esi, *(dd*)(raddr(ss,ebp+s0))));\n')

    def test_instr_7720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, [ebp+s1]')), u'\tR(MOV(esi, *(dd*)(raddr(ss,ebp+s1))));\n')

    def test_instr_7730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, esi_0')), u'\tR(MOV(esi, esi_0));\n')

    def test_instr_7740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi, offset unk_40E008')), u'\tR(MOV(esi, offset(_data,unk_40e008)));\n')

    def test_instr_7750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     esi_0, ebx')), u'\tR(MOV(esi_0, ebx));\n')

    def test_instr_7760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     i, 12345678h')), u'\tR(MOV(i, 0x12345678));\n')

    def test_instr_7770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     i, esi')), u'\tR(MOV(i, esi));\n')

    def test_instr_7780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     op0, 32432434h')), u'\tR(MOV(op0, 0x32432434));\n')

    def test_instr_7790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov   al,0')), u'\tR(MOV(al, 0));\n')

    def test_instr_7800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov   dx,3c8h')), u'\tR(MOV(dx, 0x3c8));\n')

    def test_instr_7810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov   dx,3c9h')), u'\tR(MOV(dx, 0x3c9));\n')

    def test_instr_7820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov   esi,offset pal_jeu')), u'\tR(MOV(esi, offset(_data,pal_jeu)));\n')

    def test_instr_7830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov  bx,ax')), u'\tR(MOV(bx, ax));\n')

    def test_instr_7840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov  fs,ax')), u'\tR(MOV(fs, ax));\n')

    def test_instr_7850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov [a],5')), u'\tR(MOV(*(raddr(ds,offset(_data,a))), 5));\n')

    def test_instr_7860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov [load_handle],eax')), u'\tR(MOV(m.load_handle, eax));\n')

    def test_instr_7870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ah,03dh')), u'\tR(MOV(ah, 0x03d));\n')

    def test_instr_7880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ah,48h')), u'\tR(MOV(ah, 0x48));\n')

    def test_instr_7890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ah,4Ah')), u'\tR(MOV(ah, 0x4A));\n')

    def test_instr_7900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ah,4ch                    ; AH=4Ch - Exit To DOS')), u'\tR(MOV(ah, 0x4c));\n')

    def test_instr_7910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ah,7')), u'\tR(MOV(ah, 7));\n')

    def test_instr_7920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ah,9                        ; AH=09h - Print DOS Message')), u'\tR(MOV(ah, 9));\n')

    def test_instr_7930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov al,-5')), u'\tR(MOV(al, -5));\n')

    def test_instr_7940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov al,00h  ;ouverture du fichier pour lecture.')), u'\tR(MOV(al, 0x00));\n')

    def test_instr_7950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov al,00h ;debut du fichier')), u'\tR(MOV(al, 0x00));\n')

    def test_instr_7960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov al,1')), u'\tR(MOV(al, 1));\n')

    def test_instr_7970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov al,7')), u'\tR(MOV(al, 7));\n')

    def test_instr_7980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov al,[a]')), u'\tR(MOV(al, *(raddr(ds,offset(_data,a)))));\n')

    def test_instr_7990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,-1')), u'\tR(MOV(ax, -1));\n')

    def test_instr_8000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,0002h')), u'\tR(MOV(ax, 0x0002));\n')

    def test_instr_8010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,0007')), u'\tR(MOV(ax, 0007));\n')

    def test_instr_8020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,01010101010101010b')), u'\tR(MOV(ax, 0xaaaa));\n')

    def test_instr_8030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,01111111111111111b')), u'\tR(MOV(ax, 0xffff));\n')

    def test_instr_8040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,08h')), u'\tR(MOV(ax, 0x08));\n')

    def test_instr_8050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,13h')), u'\tR(MOV(ax, 0x13));\n')

    def test_instr_8060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,3h')), u'\tR(MOV(ax, 0x3));\n')

    def test_instr_8070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,4')), u'\tR(MOV(ax, 4));\n')

    def test_instr_8080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,501h')), u'\tR(MOV(ax, 0x501));\n')

    def test_instr_8090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,6')), u'\tR(MOV(ax, 6));\n')

    def test_instr_8100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ax,bp')), u'\tR(MOV(ax, bp));\n')

    def test_instr_8110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bl,-1')), u'\tR(MOV(bl, -1));\n')

    def test_instr_8120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bl,0')), u'\tR(MOV(bl, 0));\n')

    def test_instr_8130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bl,011111111B')), u'\tR(MOV(bl, 0xff));\n')

    def test_instr_8140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bl,1')), u'\tR(MOV(bl, 1));\n')

    def test_instr_8150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bl,192')), u'\tR(MOV(bl, 192));\n')

    def test_instr_8160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bl,[a+1]')), u'\tR(MOV(bl, *(raddr(ds,offset(_data,a)+1))));\n')

    def test_instr_8170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bl,al')), u'\tR(MOV(bl, al));\n')

    def test_instr_8180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bx,(1024*10/16)+5')), u'\tR(MOV(bx, (1024*10/16)+5));\n')

    def test_instr_8190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bx,(1024*10/16)-1')), u'\tR(MOV(bx, (1024*10/16)-1));\n')

    def test_instr_8200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bx,10')), u'\tR(MOV(bx, 10));\n')

    def test_instr_8210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bx,1024*10/16')), u'\tR(MOV(bx, 1024*10/16));\n')

    def test_instr_8220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bx,5')), u'\tR(MOV(bx, 5));\n')

    def test_instr_8230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bx,ax')), u'\tR(MOV(bx, ax));\n')

    def test_instr_8240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bx,fs')), u'\tR(MOV(bx, fs));\n')

    def test_instr_8250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bx,word ptr [d]')), u'\tR(MOV(bx, *(dw*)(raddr(ds,offset(_data,d)))));\n')

    def test_instr_8260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov bx,word ptr [e]')), u'\tR(MOV(bx, *(dw*)(raddr(ds,offset(_data,e)))));\n')

    def test_instr_8270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov byte ptr [a],5')), u'\tR(MOV(*(raddr(ds,offset(_data,a))), 5));\n')

    def test_instr_8280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov byte ptr [esi],-2')), u'\tR(MOV(*(raddr(ds,esi)), -2));\n')

    def test_instr_8290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov byte ptr [var1+1],5')), u'\tR(MOV(*(raddr(ds,offset(_data,var1)+1)), 5));\n')

    def test_instr_8300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dl,byte ptr [edi]')), u'\tR(MOV(dl, *(raddr(ds,edi))));\n')

    def test_instr_8310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov byte ptr ds:[0],55')), u'\tR(MOV(*(raddr(ds,0)), 55));\n')

    def test_instr_8320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov byte ptr es:[0],55')), u'\tR(MOV(*(raddr(es,0)), 55));\n')

    def test_instr_8330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov byte ptr es:[0],56')), u'\tR(MOV(*(raddr(es,0)), 56));\n')

    def test_instr_8340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ch,011111111B')), u'\tR(MOV(ch, 0xff));\n')

    def test_instr_8350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov cl,2')), u'\tR(MOV(cl, 2));\n')

    def test_instr_8360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov cl,8            ; number of ASCII')), u'\tR(MOV(cl, 8));\n')

    def test_instr_8370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov cx,-1')), u'\tR(MOV(cx, -1));\n')

    def test_instr_8380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov cx,-5')), u'\tR(MOV(cx, -5));\n')

    def test_instr_8390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov cx,0')), u'\tR(MOV(cx, 0));\n')

    def test_instr_8400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov cx,1')), u'\tR(MOV(cx, 1));\n')

    def test_instr_8410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov cx,256*3')), u'\tR(MOV(cx, 256*3));\n')

    def test_instr_8420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov cx,ax')), u'\tR(MOV(cx, ax));\n')

    def test_instr_8430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dl,[edi+1]')), u'\tR(MOV(dl, *(raddr(ds,edi+1))));\n')

    def test_instr_8440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dl,[edi]')), u'\tR(MOV(dl, *(raddr(ds,edi))));\n')

    def test_instr_8450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ds, _data')), u'\tR(MOV(ds, seg_offset(_data)));\n')

    def test_instr_8460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ds:[edi],cl')), u'\tR(MOV(*(raddr(ds,edi)), cl));\n')

    def test_instr_8470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dword ptr es:[0],077aaFF00h')), u'\tR(MOV(*(dd*)(raddr(es,0)), 0x077aaFF00));\n')

    def test_instr_8480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dword ptr es:[20*320+160],077aaFF00h')), u'\tR(MOV(*(dd*)(raddr(es,20*320+160)), 0x077aaFF00));\n')

    def test_instr_8490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dx,-1')), u'\tR(MOV(dx, -1));\n')

    def test_instr_8500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dx,0')), u'\tR(MOV(dx, 0));\n')

    def test_instr_8510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dx,5')), u'\tR(MOV(dx, 5));\n')

    def test_instr_8520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dx,[edi+1]')), u'\tR(MOV(dx, *(dw*)(raddr(ds,edi+1))));\n')

    def test_instr_8530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dx,cx')), u'\tR(MOV(dx, cx));\n')

    def test_instr_8540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax, 0ffffffffh')), u'\tR(MOV(eax, 0x0ffffffff));\n')

    def test_instr_8550(self):
        pass
        # self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp,'',self.v.proc.add('mov eax, B')), u'\tR(MOV(eax, B));\n')

    def test_instr_8560(self):
        pass
        # self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp,'',self.v.proc.add('mov eax, CC')), u'\tR(MOV(eax, CC));\n')

    def test_instr_8570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,-1')), u'\tR(MOV(eax, -1));\n')

    def test_instr_8580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,-1-(-2+3)')), u'\tR(MOV(eax, -1-(-2+3)));\n')

    def test_instr_8590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,-4')), u'\tR(MOV(eax, -4));\n')

    def test_instr_8600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,-5')), u'\tR(MOV(eax, -5));\n')

    def test_instr_8610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,-8')), u'\tR(MOV(eax, -8));\n')

    def test_instr_8620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,0')), u'\tR(MOV(eax, 0));\n')

    def test_instr_8630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,0100b')), u'\tR(MOV(eax, 0x4));\n')

    def test_instr_8640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,011111111111111111111111111111111b')), u'\tR(MOV(eax, 0xffffffff));\n')

    def test_instr_8650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,012345678h')), u'\tR(MOV(eax, 0x012345678));\n')

    def test_instr_8660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,077aaFF00h')), u'\tR(MOV(eax, 0x077aaFF00));\n')

    def test_instr_8670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,0ffff00f3h')), u'\tR(MOV(eax, 0x0ffff00f3));\n')

    def test_instr_8680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,0ffffff03h')), u'\tR(MOV(eax, 0x0ffffff03));\n')

    def test_instr_8690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,1')), u'\tR(MOV(eax, 1));\n')

    def test_instr_8700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,1024*1024')), u'\tR(MOV(eax, 1024*1024));\n')

    def test_instr_8710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,10B')), u'\tR(MOV(eax, 0x2));\n')

    def test_instr_8720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,2')), u'\tR(MOV(eax, 2));\n')

    def test_instr_8730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,256+3+65536')), u'\tR(MOV(eax, 256+3+65536));\n')

    def test_instr_8740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,3')), u'\tR(MOV(eax, 3));\n')

    def test_instr_8750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,4')), u'\tR(MOV(eax, 4));\n')

    def test_instr_8760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,5')), u'\tR(MOV(eax, 5));\n')

    def test_instr_8770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,511')), u'\tR(MOV(eax, 511));\n')

    def test_instr_8780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,taille_moire  ;::!300000h-1 ;182400h-1 ;1582080 ;0300000h-1 ;2mega 182400h-1')), u'\tR(MOV(eax, taille_moire));\n')

    def test_instr_8790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov eax,teST2')), u'\tR(MOV(eax, teST2));\n')

    def test_instr_8800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebp,10')), u'\tR(MOV(ebp, 10));\n')

    def test_instr_8810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebp,2')), u'\tR(MOV(ebp, 2));\n')

    def test_instr_8820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebp,20')), u'\tR(MOV(ebp, 20));\n')

    def test_instr_8830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebp,3')), u'\tR(MOV(ebp, 3));\n')

    def test_instr_8840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebp,3*4')), u'\tR(MOV(ebp, 3*4));\n')

    def test_instr_8850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebp,30')), u'\tR(MOV(ebp, 30));\n')

    def test_instr_8860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,-1')), u'\tR(MOV(ebx, -1));\n')

    def test_instr_8870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,0')), u'\tR(MOV(ebx, 0));\n')

    def test_instr_8880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,00fffh')), u'\tR(MOV(ebx, 0x00fff));\n')

    def test_instr_8890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,01B')), u'\tR(MOV(ebx, 0x1));\n')

    def test_instr_8900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,0FFFFFFFFh')), u'\tR(MOV(ebx, 0x0FFFFFFFF));\n')

    def test_instr_8910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,0a000h')), u'\tR(MOV(ebx, 0x0a000));\n')

    def test_instr_8920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,0aabbccddh')), u'\tR(MOV(ebx, 0x0aabbccdd));\n')

    def test_instr_8930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,0f222h')), u'\tR(MOV(ebx, 0x0f222));\n')

    def test_instr_8940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,0ffff01ffh')), u'\tR(MOV(ebx, 0x0ffff01ff));\n')

    def test_instr_8950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,0ffffffffh')), u'\tR(MOV(ebx, 0x0ffffffff));\n')

    def test_instr_8960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,2')), u'\tR(MOV(ebx, 2));\n')

    def test_instr_8970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,255')), u'\tR(MOV(ebx, 255));\n')

    def test_instr_8980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,3')), u'\tR(MOV(ebx, 3));\n')

    def test_instr_8990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,5')), u'\tR(MOV(ebx, 5));\n')

    def test_instr_9000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,[g]')), u'\tR(MOV(ebx, m.g));\n')

    def test_instr_9010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,[load_handle]')), u'\tR(MOV(ebx, m.load_handle));\n')

    def test_instr_9020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,eax')), u'\tR(MOV(ebx, eax));\n')

    def test_instr_9030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,-1')), u'\tR(MOV(ecx, -1));\n')

    def test_instr_9040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,000ff00ffh')), u'\tR(MOV(ecx, 0x000ff00ff));\n')

    def test_instr_9050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,0a0000h')), u'\tR(MOV(ecx, 0x0a0000));\n')

    def test_instr_9060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,0df01h')), u'\tR(MOV(ecx, 0x0df01));\n')

    def test_instr_9070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,0f0ffh')), u'\tR(MOV(ecx, 0x0f0ff));\n')

    def test_instr_9080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,0ffffffffh')), u'\tR(MOV(ecx, 0x0ffffffff));\n')

    def test_instr_9090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,10')), u'\tR(MOV(ecx, 10));\n')

    def test_instr_9100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,2')), u'\tR(MOV(ecx, 2));\n')

    def test_instr_9110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,3')), u'\tR(MOV(ecx, 3));\n')

    def test_instr_9120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,320*200/4')), u'\tR(MOV(ecx, 320*200/4));\n')

    def test_instr_9130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,5')), u'\tR(MOV(ecx, 5));\n')

    def test_instr_9140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,60')), u'\tR(MOV(ecx, 60));\n')

    def test_instr_9150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ecx,t')), u'\tR(MOV(ecx, t));\n')

    def test_instr_9160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edi,1')), u'\tR(MOV(edi, 1));\n')

    def test_instr_9170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edi,8')), u'\tR(MOV(edi, 8));\n')

    def test_instr_9180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edi,OFFSET AsCii ; get the offset address')), u'\tR(MOV(edi, offset(_data,ascii)));\n')

    def test_instr_9190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edi,esi')), u'\tR(MOV(edi, esi));\n')

    def test_instr_9200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edi,offset str2')), u'\tR(MOV(edi, offset(_data,str2)));\n')

    def test_instr_9210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edi,offset str3')), u'\tR(MOV(edi, offset(_data,str3)));\n')

    def test_instr_9220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edi,offset var1')), u'\tR(MOV(edi, offset(_data,var1)));\n')

    def test_instr_9230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edi,offset var2')), u'\tR(MOV(edi, offset(_data,var2)));\n')

    def test_instr_9240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edi,offset var4')), u'\tR(MOV(edi, offset(_data,var4)));\n')

    def test_instr_9250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edx,0')), u'\tR(MOV(edx, 0));\n')

    def test_instr_9260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edx,0abcdef77h')), u'\tR(MOV(edx, 0x0abcdef77));\n')

    def test_instr_9270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edx,2')), u'\tR(MOV(edx, 2));\n')

    def test_instr_9280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edx,4')), u'\tR(MOV(edx, 4));\n')

    def test_instr_9290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edx,OFFSET ASCiI ; DOS 1+ WRITE STRING TO STANDARD OUTPUT')), u'\tR(MOV(edx, offset(_data,ascii)));\n')

    def test_instr_9300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edx,edi')), u'\tR(MOV(edx, edi));\n')

    def test_instr_9310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov edx,offset _msg             ; DS:EDX -> $ Terminated String')), u'\tR(MOV(edx, offset(_data,_msg)));\n')

    def test_instr_9320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov es,ax')), u'\tR(MOV(es, ax));\n')

    def test_instr_9330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov esi,2')), u'\tR(MOV(esi, 2));\n')

    def test_instr_9340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov esi,6')), u'\tR(MOV(esi, 6));\n')

    def test_instr_9350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov esi,offset str1')), u'\tR(MOV(esi, offset(_data,str1)));\n')

    def test_instr_9360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov esi,offset testOVerlap')), u'\tR(MOV(esi, offset(_data,testoverlap)));\n')

    def test_instr_9370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov esi,offset var1')), u'\tR(MOV(esi, offset(_data,var1)));\n')

    def test_instr_9380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov esi,offset var2')), u'\tR(MOV(esi, offset(_data,var2)));\n')

    def test_instr_9390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov esi,offset var3')), u'\tR(MOV(esi, offset(_data,var3)));\n')

    def test_instr_9400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movsb')), 'MOVSB;\n')

    def test_instr_9410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movsd')), 'MOVSD;\n')

    def test_instr_9420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movsw')), 'MOVSW;\n')

    def test_instr_9430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movsx bx,[h2]')), u'\tR(MOVSX(bx, m.h2));\n')

    def test_instr_9440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movsx bx,bl')), u'\tR(MOVSX(bx, bl));\n')

    def test_instr_9450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movsx bx,byte ptr [h2]')), u'\tR(MOVSX(bx, *(raddr(ds,offset(_data,h2)))));\n')

    def test_instr_9460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movsx bx,byte ptr [h]')), u'\tR(MOVSX(bx, *(raddr(ds,offset(_data,h)))));\n')

    def test_instr_9470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movsx ecx,cx')), u'\tR(MOVSX(ecx, cx));\n')

    def test_instr_9480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movzx eax, DDD')), u'\tR(MOVZX(eax, DDD));\n')  # ERROR

    def test_instr_9490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movzx ecx,bx')), u'\tR(MOVZX(ecx, bx));\n')

    def test_instr_9500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mul     cl')), u'\tR(MUL1_1(cl));\n')

    def test_instr_9510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mul     cx')), u'\tR(MUL1_2(cx));\n')

    def test_instr_9520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mul     s1_0')), u'\tR(MUL1_0(s1_0));\n')

    def test_instr_9530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('neg     dl')), u'\tR(NEG(dl));\n')

    def test_instr_9540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('neg     dx')), u'\tR(NEG(dx));\n')

    def test_instr_9550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('neg     ebx')), u'\tR(NEG(ebx));\n')

    def test_instr_9560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('neg     edx')), u'\tR(NEG(edx));\n')

    def test_instr_9570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('neg ebx')), u'\tR(NEG(ebx));\n')

    def test_instr_9580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('neg edx')), u'\tR(NEG(edx));\n')

    def test_instr_9590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('not     dl')), u'\tR(NOT(dl));\n')

    def test_instr_9600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('not     dx')), u'\tR(NOT(dx));\n')

    def test_instr_9610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('not     edx')), u'\tR(NOT(edx));\n')

    def test_instr_9620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('not ax')), u'\tR(NOT(ax));\n')

    def test_instr_9630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('not eax')), u'\tR(NOT(eax));\n')

    def test_instr_9640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('or      dl, cl')), u'\tR(OR(dl, cl));\n')

    def test_instr_9650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('or      dx, cx')), u'\tR(OR(dx, cx));\n')

    def test_instr_9660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('or      ebx, 0FFFFFFFFh')), u'\tR(OR(ebx, 0x0FFFFFFFF));\n')

    def test_instr_9670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('or      edx, ecx')), u'\tR(OR(edx, ecx));\n')

    def test_instr_9680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('or      res, 0FFFFFFFFh')), u'\tR(OR(res, 0x0FFFFFFFF));\n')

    def test_instr_9690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('or cl,0f0h')), u'\tR(OR(cl, 0x0f0));\n')

    def test_instr_9700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('or cx,cx')), u'\tR(OR(cx, cx));\n')

    def test_instr_9710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('or eax,eax')), u'\tR(OR(eax, eax));\n')

    def test_instr_9720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('out   dx,al')), u'\tR(OUT(dx, al));\n')

    def test_instr_9730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rcl     dl, cl')), u'\tR(RCL(dl, cl));\n')

    def test_instr_9740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rcl     dx, cl')), u'\tR(RCL(dx, cl));\n')

    def test_instr_9750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rcl     edx, cl')), u'\tR(RCL(edx, cl));\n')

    def test_instr_9760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rcr     dl, cl')), u'\tR(RCR(dl, cl));\n')

    def test_instr_9770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rcr     dx, cl')), u'\tR(RCR(dx, cl));\n')

    def test_instr_9780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rcr     edx, cl')), u'\tR(RCR(edx, cl));\n')

    def test_instr_9790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('ret')), 'tR(RETN);\n')

    def test_instr_9800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('ret\n')), 'tR(RETN);\n')

    def test_instr_9810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rol     dl, cl')), u'\tR(ROL(dl, cl));\n')

    def test_instr_9820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rol     dx, cl')), u'\tR(ROL(dx, cl));\n')

    def test_instr_9830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rol     edx, cl')), u'\tR(ROL(edx, cl));\n')

    def test_instr_9840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rol ebx,1')), u'\tR(ROL(ebx, 1));\n')

    def test_instr_9850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('rol ebx,31')), u'\tR(ROL(ebx, 31));\n')

    def test_instr_9860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('ror     dl, cl')), u'\tR(ROR(dl, cl));\n')

    def test_instr_9870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('ror     dx, cl')), u'\tR(ROR(dx, cl));\n')

    def test_instr_9880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('ror     edx, cl')), u'\tR(ROR(edx, cl));\n')

    def test_instr_9890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sar     dl, cl')), u'\tR(SAR(dl, cl));\n')

    def test_instr_9900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sal     dl, cl')), u'\tR(SAL(dl, cl));\n')

    def test_instr_9910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sar     dx, cl')), u'\tR(SAR(dx, cl));\n')

    def test_instr_9920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sar     edx, cl')), u'\tR(SAR(edx, cl));\n')

    def test_instr_9930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sar eax,1')), u'\tR(SAR(eax, 1));\n')

    def test_instr_9940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sar eax,2')), u'\tR(SAR(eax, 2));\n')

    def test_instr_9950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sbb     dl, cl')), u'\tR(SBB(dl, cl));\n')

    def test_instr_9960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sbb     dx, cx')), u'\tR(SBB(dx, cx));\n')

    def test_instr_9970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sbb     edx, ecx')), u'\tR(SBB(edx, ecx));\n')

    def test_instr_9980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('scasb')), 'SCASB;\n')

    def test_instr_9990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('scasd')), 'SCASD;\n')

    def test_instr_10000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('scasw')), 'SCASW;\n')

    def test_instr_10010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('setb    al')), u'\tR(SETB(al));\n')

    def test_instr_10020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('setnz bh')), u'\tR(SETNZ(bh));\n')

    def test_instr_10030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('setz    cl')), u'\tR(SETZ(cl));\n')

    def test_instr_10040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shl     dl, cl')), u'\tR(SHL(dl, cl));\n')

    def test_instr_10050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shl     dx, cl')), u'\tR(SHL(dx, cl));\n')

    def test_instr_10060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shl     edx, cl')), u'\tR(SHL(edx, cl));\n')

    def test_instr_10070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shld    dx, bx, cl')), u'\tR(SHLD(dx, bx, cl));\n')

    def test_instr_10080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shld    edx, ebx, cl')), u'\tR(SHLD(edx, ebx, cl));\n')

    def test_instr_10090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shr     dl, cl')), u'\tR(SHR(dl, cl));\n')

    def test_instr_10100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shr     dx, cl')), u'\tR(SHR(dx, cl));\n')

    def test_instr_10110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shr     edx, cl')), u'\tR(SHR(edx, cl));\n')

    def test_instr_10120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shr eax,16')), u'\tR(SHR(eax, 16));\n')

    def test_instr_10130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shr ecx,16')), u'\tR(SHR(ecx, 16));\n')

    def test_instr_10140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shrd    dx, bx, cl')), u'\tR(SHRD(dx, bx, cl));\n')

    def test_instr_10150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shrd    edx, ebx, cl')), u'\tR(SHRD(edx, ebx, cl));\n')

    def test_instr_10160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shrd eax, edx, 8')), u'\tR(SHRD(eax, edx, 8));\n')

    def test_instr_10170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('stc')), 'tR(STC);\n')

    def test_instr_10180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('std')), 'tR(STD);\n')

    def test_instr_10190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sti                             ; Set The Interrupt Flag')), 'tR(STI);\n')

    def test_instr_10200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('stosb')), 'STOSB;\n')

    def test_instr_10210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('stosd')), 'STOSD;\n')

    def test_instr_10220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('stosw')), 'STOSW;\n')

    def test_instr_10230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub     dl, cl')), u'\tR(SUB(dl, cl));\n')

    def test_instr_10240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub     dx, cx')), u'\tR(SUB(dx, cx));\n')

    def test_instr_10250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub     edx, ecx')), u'\tR(SUB(edx, ecx));\n')

    def test_instr_10260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub     esp, 10h')), u'\tR(SUB(esp, 0x10));\n')

    def test_instr_10270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub     esp, 114h')), u'\tR(SUB(esp, 0x114));\n')

    def test_instr_10280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub     esp, 14h')), u'\tR(SUB(esp, 0x14));\n')

    def test_instr_10290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub eax,eax')), u'\teax = 0;AFFECT_ZF(0); AFFECT_SF(eax,0);\n')

    def test_instr_10300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub eax,ebx')), u'\tR(SUB(eax, ebx));\n')

    def test_instr_10310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub ebx,eax')), u'\tR(SUB(ebx, eax));\n')

    def test_instr_10320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('sub word ptr [var5+2],25')), u'\tR(SUB(*(dw*)(raddr(ds,offset(_data,var5)+2)), 25));\n')

    def test_instr_10330(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('test    ebx, ebx')), u'\tR(TEST(ebx, ebx));\n')

    def test_instr_10340(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('test al,010B')), u'\tR(TEST(al, 0x2));\n')

    def test_instr_10350(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('test al,0B')), u'\tR(TEST(al, 0x0));\n')

    def test_instr_10360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('test ax,ax')), u'\tR(TEST(ax, ax));\n')

    def test_instr_10370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('test bh,01h')), u'\tR(TEST(bh, 0x01));\n')

    def test_instr_10380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('test bh,02h')), u'\tR(TEST(bh, 0x02));\n')

    def test_instr_10390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('test eax,eax')), u'\tR(TEST(eax, eax));\n')

    def test_instr_10400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xadd    byte ptr [ebp+var_20], al')), u'\tR(XADD(*(raddr(ss,ebp+var_20)), al));\n')

    def test_instr_10410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xadd    dl, al')), u'\tR(XADD(dl, al));\n')

    def test_instr_10420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xadd    dword ptr [ebp+var_20], eax')), u'\tR(XADD(*(dd*)(raddr(ss,ebp+var_20)), eax));\n')

    def test_instr_10430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xadd    dx, ax')), u'\tR(XADD(dx, ax));\n')

    def test_instr_10440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xadd    eax, eax')), u'\tR(XADD(eax, eax));\n')

    def test_instr_10450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xadd    edx, eax')), u'\tR(XADD(edx, eax));\n')

    def test_instr_10460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xadd    word ptr [ebp+var_20], ax')), u'\tR(XADD(*(dw*)(raddr(ss,ebp+var_20)), ax));\n')

    def test_instr_10470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xchg    al, byte ptr [ebp+var_20]')), u'\tR(XCHG(al, *(raddr(ss,ebp+var_20))));\n')

    def test_instr_10480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xchg    al, dl')), u'\tR(XCHG(al, dl));\n')

    def test_instr_10490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xchg    ax, dx')), u'\tR(XCHG(ax, dx));\n')

    def test_instr_10500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xchg    ax, word ptr [ebp+var_20]')), u'\tR(XCHG(ax, *(dw*)(raddr(ss,ebp+var_20))));\n')

    def test_instr_10510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xchg    eax, dword ptr [ebp+var_20]')), u'\tR(XCHG(eax, *(dd*)(raddr(ss,ebp+var_20))));\n')

    def test_instr_10520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xchg    eax, edx')), u'\tR(XCHG(eax, edx));\n')

    def test_instr_10530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xchg eax,ebx')), u'\tR(XCHG(eax, ebx));\n')

    def test_instr_10540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xlat')), 'tR(XLAT);\n')

    def test_instr_10550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     dl, cl')), u'\tR(XOR(dl, cl));\n')

    def test_instr_10560(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     dx, cx')), u'\tR(XOR(dx, cx));\n')

    def test_instr_10570(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     eax, eax')), u'\teax = 0;AFFECT_ZF(0); AFFECT_SF(eax,0);\n')

    def test_instr_10580(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     ebx, ebx')), u'\tebx = 0;AFFECT_ZF(0); AFFECT_SF(ebx,0);\n')

    def test_instr_10590(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     ecx, ecx')), u'\tecx = 0;AFFECT_ZF(0); AFFECT_SF(ecx,0);\n')

    def test_instr_10600(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     edi, edi')), u'\tedi = 0;AFFECT_ZF(0); AFFECT_SF(edi,0);\n')

    def test_instr_10610(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     edx, ecx')), u'\tR(XOR(edx, ecx));\n')

    def test_instr_10620(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     edx, edx')), u'\tedx = 0;AFFECT_ZF(0); AFFECT_SF(edx,0);\n')

    def test_instr_10630(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     esi, esi')), u'\tesi = 0;AFFECT_ZF(0); AFFECT_SF(esi,0);\n')

    def test_instr_10640(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     i, i')), u'\ti = 0;AFFECT_ZF(0); AFFECT_SF(i,0);\n')

    def test_instr_10650(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor     res, res')), u'\tres = 0;AFFECT_ZF(0); AFFECT_SF(res,0);\n')

    def test_instr_10660(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor al,bl')), u'\tR(XOR(al, bl));\n')

    def test_instr_10670(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor ax,ax')), u'\tax = 0;AFFECT_ZF(0); AFFECT_SF(ax,0);\n')

    def test_instr_10680(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor ax,bx')), u'\tR(XOR(ax, bx));\n')

    def test_instr_10690(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor ch,bh')), u'\tR(XOR(ch, bh));\n')

    def test_instr_10700(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor cx,cx')), u'\tcx = 0;AFFECT_ZF(0); AFFECT_SF(cx,0);\n')

    def test_instr_10710(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor eax,eax')), u'\teax = 0;AFFECT_ZF(0); AFFECT_SF(eax,0);\n')

    def test_instr_10720(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor eax,ebx')), u'\tR(XOR(eax, ebx));\n')

    def test_instr_10730(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor ebp,ebp')), u'\tebp = 0;AFFECT_ZF(0); AFFECT_SF(ebp,0);\n')

    def test_instr_10740(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor ebx,ebx')), u'\tebx = 0;AFFECT_ZF(0); AFFECT_SF(ebx,0);\n')

    def test_instr_10750(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor ecx,ecx')), u'\tecx = 0;AFFECT_ZF(0); AFFECT_SF(ecx,0);\n')

    def test_instr_10760(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor edi,edi')), u'\tedi = 0;AFFECT_ZF(0); AFFECT_SF(edi,0);\n')

    def test_instr_10770(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor edx,edx')), u'\tedx = 0;AFFECT_ZF(0); AFFECT_SF(edx,0);\n')

    def test_instr_10780(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('xor esi,esi')), u'\tesi = 0;AFFECT_ZF(0); AFFECT_SF(esi,0);\n')

    def test_instr_10790(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code(u'iret')), 'tR(IRET);\n')

    def test_instr_10800(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code(u'retf')), 'tR(RETF);\n')

    def test_instr_10810(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lds     bx, offset unk_40F064')), u'\tR(LDS(bx, offset(initcall,unk_40f064)));\n')

    def test_instr_10820(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('les     bx, offset unk_40F064')), u'\tR(LES(bx, offset(initcall,unk_40f064)));\n')

    def test_instr_10830(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lfs     bx, offset unk_40F064')), u'\tR(LFS(bx, offset(initcall,unk_40f064)));\n')

    def test_instr_10840(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lgs     bx, offset unk_40F064')), u'\tR(LGS(bx, offset(initcall,unk_40f064)));\n')

    def test_instr_10850(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code(u'pusha')), 'tR(PUSHA);\n')

    def test_instr_10860(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code(u'popa')), 'tR(POPA);\n')

    def test_instr_10870(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code(u'cli')), 'tR(CLI);\n')

    def test_instr_10880(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('in   dx,al')), u'\tR(IN(dx, al));\n')

    def test_instr_10890(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shrd ax, dx, 3')), u'\tR(SHRD(ax, dx, 3));\n')

    def test_instr_10900(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('shld ax, dx, 3')), u'\tR(SHLD(ax, dx, 3));\n')

    def test_instr_10910(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('enter 0,0')), u'\tR(ENTER(0, 0));\n')

    def test_instr_10920(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jmp $+2')), u'\n')

    def test_instr_10930(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code(u'nop')), None)

    def test_instr_10940(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lods	[byte ptr fs:si]')), u'\tR(LODS(*(raddr(fs,si)),1));\n')

    def test_instr_10950(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('scas	[word ptr fs:si]')), u'\tR(SCAS(*(dw*)(raddr(fs,si)),2));\n')

    def test_instr_10960(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('movs [dword ptr es:di], [dword ptr fs:si]')), u'MOVS(*(dd*)(raddr(es,di)), *(dd*)(raddr(fs,si)), 4);\n')

    def test_instr_10970(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code("mov al, 'Z' - 'A' +1")), u"tR(MOV(al, 'Z'-'A'+1));n")

    def test_instr_10980(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code(u'mov ax,  not 1')), 'tR(MOV(ax, ~1));\n')

    def test_instr_10990(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     [esp+8], eax')), 'tR(MOV(*(dd*)(raddr(ss,esp+8)), eax));\n')

    def test_instr_11000(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, ds:40h[eax*2]')), u'\tR(eax = 0x40+eax*2);\n')

    def test_instr_11010(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, ds:40h[ebx*4]')), u'\tR(eax = 0x40+ebx*4);\n')

    def test_instr_11020(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, ds:40h[ecx*8]')), u'\tR(eax = 0x40+ecx*8);\n')

    def test_instr_11030(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ds:byte_41411F[eax], dl')), u'\tR(MOV(*(raddr(ds,offset(_bss,byte_41411f)+eax)), dl));\n')

    def test_instr_11040(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('lea     eax, large ds:4000h')), u'\tR(eax = 0x4000);\n')

    def test_instr_11050(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov     ebx, offset _test_btc')), u'\tR(MOV(ebx, offset(initcall,_test_btc)));\n')

    def test_instr_11060(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     dword ptr [esp]')), u'\tR(POP(*(dd*)(raddr(ss,esp))));\n')

    def test_instr_11070(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     eax')), u'\tR(POP(eax));\n')

    def test_instr_11080(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     ebp')), u'\tR(POP(ebp));\n')

    def test_instr_11090(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     ebx')), u'\tR(POP(ebx));\n')

    def test_instr_11100(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     ecx')), u'\tR(POP(ecx));\n')

    def test_instr_11110(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     edi')), u'\tR(POP(edi));\n')

    def test_instr_11120(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     edi_0')), u'\tR(POP(edi_0));\n')

    def test_instr_11130(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     edx')), u'\tR(POP(edx));\n')

    def test_instr_11140(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     esi')), u'\tR(POP(esi));\n')

    def test_instr_11150(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     esi_0')), u'\tR(POP(esi_0));\n')

    def test_instr_11160(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     i')), u'\tR(POP(i));\n')

    def test_instr_11170(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     res')), u'\tR(POP(res));\n')

    def test_instr_11180(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop     s0_0')), u'\tR(POP(s0_0));\n')

    def test_instr_11190(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop ds')), u'\tR(POP(ds));\n')

    def test_instr_11200(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop eax')), u'\tR(POP(eax));\n')

    def test_instr_11210(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pop es')), u'\tR(POP(es));\n')

    def test_instr_11220(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('popad')), 'tR(POPAD);\n')

    def test_instr_11230(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('popf')), 'tR(POPF);\n')

    def test_instr_11240(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push    0')), u'\tR(PUSH(0));\n')

    def test_instr_11250(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push    0BC6058h')), u'\tR(PUSH(0x0BC6058));\n')

    def test_instr_11260(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push    9ABCDEFh')), u'\tR(PUSH(0x9ABCDEF));\n')

    def test_instr_11270(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push    esi')), u'\tR(PUSH(esi));\n')

    def test_instr_11280(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push ds')), u'\tR(PUSH(ds));\n')

    def test_instr_11290(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push ebx')), u'\tR(PUSH(ebx));\n')

    def test_instr_11300(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('push es')), u'\tR(PUSH(es));\n')

    def test_instr_11310(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pushad')), 'tR(PUSHAD);\n')

    def test_instr_11320(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('pushf')), 'tR(PUSHF);\n')

    def test_instr_11330(self):
        pass
        # self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, p.action_code(u'rep')), 'tREP\n')

    def test_instr_11340(self):
        pass
        # self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, p.action_code(u'repe')), 'tREPE\n')

    def test_instr_11350(self):
        pass
        # self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, p.action_code(u'repne')), 'tREPNE\n')

    def test_instr_11360(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code(u'repne lodsb')), 'LODSB;\n')

    def test_instr_11370(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('call    dword ptr [ebx-4]')), 'tR(CALL(__disp));\n')

    def test_instr_11380(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('call    exec_adc')), u'\tR(CALL(kexec_adc));\n')

    def test_instr_11390(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('call    printf')), 'tR(CALL(__disp));\n')

    def test_instr_11400(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('call    test_bcd')), u'\tR(CALL(ktest_bcd));\n')

    def test_instr_11410(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('call printeax')), u'\tR(CALL(kprinteax));\n')

    def test_instr_11420(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var2,2')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var2))), 2));\n')

    def test_instr_11430(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var2,bx')), u'\tR(CMP(*(dw*)(raddr(ds,offset(_data,var2))), bx));\n')

    def test_instr_11440(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var3,-12')), u'\tR(CMP(*(raddr(ds,offset(_data,var3))), -12));\n')

    def test_instr_11450(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var3,ecx')), u'\tR(CMP(*(raddr(ds,offset(_data,var3))), ecx));\n')

    def test_instr_11460(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('inc var3')), u'\tR(INC(*(raddr(ds,offset(_data,var3)))));\n')

    def test_instr_11470(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('jmp [cs:table+ax]')), 'ttR(JMP(__dispatch_call));\n')

    def test_instr_11480(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov a,5')), u'\tR(MOV(*(raddr(ds,offset(_data,a))), 5));\n')

    def test_instr_11490(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov a,ah')), u'\tR(MOV(*(raddr(ds,offset(_data,a))), ah));\n')

    def test_instr_11500(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp b,256+3+65536')), u'\tR(CMP(m.b, 256+3+65536));\n')

    def test_instr_11510(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var1,1')), u'\tR(CMP(m.var1, 1));\n')

    def test_instr_11520(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('cmp var1,al')), u'\tR(CMP(m.var1, al));\n')

    def test_instr_11530(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov b,ax')), u'\tR(MOV(m.b, ax));\n')

    def test_instr_11540(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov dl,var1')), u'\tR(MOV(dl, m.var1));\n')

    def test_instr_11550(self):
        self.assertEqual(self.v.proc.generate_c_cmd(self.v.cpp, self.v.parser.action_code('mov ebx,g')), u'\tR(MOV(ebx, m.g));\n')
        #r=list(filter(lambda x: not x.used, p.get_globals().values()))
        #print([g.name for g in r])


if __name__ == "__main__":
    unittest.main()
