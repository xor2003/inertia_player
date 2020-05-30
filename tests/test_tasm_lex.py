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

class LexTest(unittest.TestCase):
    def test_parse_args(self):
        self.assertEqual(tasm.lex.parse_args(text="'00000000',0Dh,0Ah,'$' ; buffer for ASCII string"),[u"'00000000'", u'0Dh', u'0Ah', u"'$'"])
        self.assertEqual(tasm.lex.parse_args(text="'Hello World From Protected Mode!',10,13,'$'"),[u"'Hello World From Protected Mode!'", u'10', u'13', u"'$'"])
        self.assertEqual(tasm.lex.parse_args(text="'OKOKOKOK'"),[u"'OKOKOKOK'"])
        self.assertEqual(tasm.lex.parse_args(text="'OKOKOKOK',10,13"),[u"'OKOKOKOK'", u'10', u'13'])
        self.assertEqual(tasm.lex.parse_args(text="'ab''cd'"),[u"'ab''cd'"])
        self.assertEqual(tasm.lex.parse_args(text="'file.txt',0"),[u"'file.txt'", u'0'])
        self.assertEqual(tasm.lex.parse_args(text="[doublequote+4],'d'"),[u'[doublequote+4]', u"'d'"])
        self.assertEqual(tasm.lex.parse_args(text="ah,9 ; DS:DX->'$'-terminated string"),[u'ah', u'9'])
        self.assertEqual(tasm.lex.parse_args(text="al, 'Z' - 'A' +1"),[u'al', u"'Z' - 'A' +1"])
        self.assertEqual(tasm.lex.parse_args(text="cx, 80h ; '\x80'"),[u'cx', u'80h'])
        self.assertEqual(tasm.lex.parse_args(text="dl,'a'"),[u'dl', u"'a'"])
        self.assertEqual(tasm.lex.parse_args(text="dl,'c'"),[u'dl', u"'c'"])
        self.assertEqual(tasm.lex.parse_args(text="dword ptr buffer,'tseT'"),[u'dword ptr buffer', u"'tseT'"])
        self.assertEqual(tasm.lex.parse_args(text=''),[])
        self.assertEqual(tasm.lex.parse_args(text='(00+38*3)*320+1/2+33*(3-1)'),[u'(00+38*3)*320+1/2+33*(3-1)'])
        self.assertEqual(tasm.lex.parse_args(text='-13'),[u'-13'])
        self.assertEqual(tasm.lex.parse_args(text='0'),[u'0'])
        self.assertEqual(tasm.lex.parse_args(text='1'),[u'1'])
        self.assertEqual(tasm.lex.parse_args(text='10 dup (?)'),[u'10 dup (?)'])
        self.assertEqual(tasm.lex.parse_args(text='100 dup (1)'),[u'100 dup (1)'])
        self.assertEqual(tasm.lex.parse_args(text='1000h dup(?) ;IGNORE'),[u'1000h dup(?)'])
        self.assertEqual(tasm.lex.parse_args(text='1000h dup(?)'),[u'1000h dup(?)'])
        self.assertEqual(tasm.lex.parse_args(text='11'),[u'11'])
        self.assertEqual(tasm.lex.parse_args(text='11,-11,2,4'),[u'11', u'-11', u'2', u'4'])
        self.assertEqual(tasm.lex.parse_args(text='11,-11,2,4000000'),[u'11', u'-11', u'2', u'4000000'])
        self.assertEqual(tasm.lex.parse_args(text='111,1'),[u'111', u'1'])
        self.assertEqual(tasm.lex.parse_args(text='12'),[u'12'])
        self.assertEqual(tasm.lex.parse_args(text='13'),[u'13'])
        self.assertEqual(tasm.lex.parse_args(text='131'),[u'131'])
        self.assertEqual(tasm.lex.parse_args(text='141'),[u'141'])
        self.assertEqual(tasm.lex.parse_args(text='1500 ; 8*2*3 ;+1 +19*13*2*4'),[u'1500'])
        self.assertEqual(tasm.lex.parse_args(text='2'),[u'2'])
        self.assertEqual(tasm.lex.parse_args(text='2,5,0'),[u'2', u'5', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='2,5,6'),[u'2', u'5', u'6'])
        self.assertEqual(tasm.lex.parse_args(text='21h ; DOS - 2+ - CLOSE A FILE WITH HANDLE'),[u'21h'])
        self.assertEqual(tasm.lex.parse_args(text='21h ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)'),[u'21h'])
        self.assertEqual(tasm.lex.parse_args(text='21h ; DOS - 2+ - OPEN DISK FILE WITH HANDLE'),[u'21h'])
        self.assertEqual(tasm.lex.parse_args(text='21h ; DOS INT 21h ; DOS INT 21h'),[u'21h'])
        self.assertEqual(tasm.lex.parse_args(text='21h ; DOS INT 21h'),[u'21h'])
        self.assertEqual(tasm.lex.parse_args(text='21h ; maybe redirected under DOS 2+ for output to file'),[u'21h'])
        self.assertEqual(tasm.lex.parse_args(text='21h'),[u'21h'])
        self.assertEqual(tasm.lex.parse_args(text='223,22'),[u'223', u'22'])
        self.assertEqual(tasm.lex.parse_args(text='3'),[u'3'])
        self.assertEqual(tasm.lex.parse_args(text='34'),[u'34'])
        self.assertEqual(tasm.lex.parse_args(text='4 dup (5)'),[u'4 dup (5)'])
        self.assertEqual(tasm.lex.parse_args(text='4'),[u'4'])
        self.assertEqual(tasm.lex.parse_args(text='4,6,9'),[u'4', u'6', u'9'])
        self.assertEqual(tasm.lex.parse_args(text='5 dup (0)'),[u'5 dup (0)'])
        self.assertEqual(tasm.lex.parse_args(text='5*5 dup (0,testEqu*2,2*2,3)'),[u'5*5 dup (0', u'testEqu*2', u'2*2', u'3)'])
        self.assertEqual(tasm.lex.parse_args(text='6'),[u'6'])
        self.assertEqual(tasm.lex.parse_args(text='64000 dup(0)'),[u'64000 dup(0)'])
        self.assertEqual(tasm.lex.parse_args(text='9,8,7,1'),[u'9', u'8', u'7', u'1'])
        self.assertEqual(tasm.lex.parse_args(text='P1 ; jump if cl is not equal 0 (zeroflag is not set)'),[u'P1'])
        self.assertEqual(tasm.lex.parse_args(text='[load_handle],eax'),[u'[load_handle]', u'eax'])
        self.assertEqual(tasm.lex.parse_args(text='[var+3],5'),[u'[var+3]', u'5'])
        self.assertEqual(tasm.lex.parse_args(text='[var+4],0'),[u'[var+4]', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='[var-1],0'),[u'[var-1]', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='[var0+5],0'),[u'[var0+5]', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='[var1+1],5'),[u'[var1+1]', u'5'])
        self.assertEqual(tasm.lex.parse_args(text='[var1]'),[u'[var1]'])
        self.assertEqual(tasm.lex.parse_args(text='[var1],111'),[u'[var1]', u'111'])
        self.assertEqual(tasm.lex.parse_args(text='[var1],2'),[u'[var1]', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='[var1],3'),[u'[var1]', u'3'])
        self.assertEqual(tasm.lex.parse_args(text='[var2+2],6'),[u'[var2+2]', u'6'])
        self.assertEqual(tasm.lex.parse_args(text='[var2-1],5'),[u'[var2-1]', u'5'])
        self.assertEqual(tasm.lex.parse_args(text='[var2]'),[u'[var2]'])
        self.assertEqual(tasm.lex.parse_args(text='[var2],0'),[u'[var2]', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='[var2],1'),[u'[var2]', u'1'])
        self.assertEqual(tasm.lex.parse_args(text='[var2],13'),[u'[var2]', u'13'])
        self.assertEqual(tasm.lex.parse_args(text='[var2],4'),[u'[var2]', u'4'])
        self.assertEqual(tasm.lex.parse_args(text='[var3+3*4],4000000'),[u'[var3+3*4]', u'4000000'])
        self.assertEqual(tasm.lex.parse_args(text='[var3+ebp],4000000'),[u'[var3+ebp]', u'4000000'])
        self.assertEqual(tasm.lex.parse_args(text='[var3]'),[u'[var3]'])
        self.assertEqual(tasm.lex.parse_args(text='[var3],11'),[u'[var3]', u'11'])
        self.assertEqual(tasm.lex.parse_args(text='[var3],35'),[u'[var3]', u'35'])
        self.assertEqual(tasm.lex.parse_args(text='[var3],37'),[u'[var3]', u'37'])
        self.assertEqual(tasm.lex.parse_args(text='[var],5'),[u'[var]', u'5'])
        self.assertEqual(tasm.lex.parse_args(text='_byte_2461B, 0'),[u'_byte_2461B', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='_byte_24665, 1'),[u'_byte_24665', u'1'])
        self.assertEqual(tasm.lex.parse_args(text='_dosfread'),[u'_dosfread'])
        self.assertEqual(tasm.lex.parse_args(text='_ems_restore_mapctx'),[u'_ems_restore_mapctx'])
        self.assertEqual(tasm.lex.parse_args(text='_ems_save_mapctx'),[u'_ems_save_mapctx'])
        self.assertEqual(tasm.lex.parse_args(text='_fhandle_module, ax'),[u'_fhandle_module', u'ax'])
        self.assertEqual(tasm.lex.parse_args(text='_mod_1021E'),[u'_mod_1021E'])
        self.assertEqual(tasm.lex.parse_args(text='_mod_102F5'),[u'_mod_102F5'])
        self.assertEqual(tasm.lex.parse_args(text='_mod_channels_number, 4'),[u'_mod_channels_number', u'4'])
        self.assertEqual(tasm.lex.parse_args(text='_mod_channels_number, 8'),[u'_mod_channels_number', u'8'])
        self.assertEqual(tasm.lex.parse_args(text='_module_type_text, 2E542E4Eh'),[u'_module_type_text', u'2E542E4Eh'])
        self.assertEqual(tasm.lex.parse_args(text='_module_type_text, 38544C46h ; FLT8'),[u'_module_type_text', u'38544C46h'])
        self.assertEqual(tasm.lex.parse_args(text='_moduleflag_246D0, 11b'),[u'_moduleflag_246D0', u'11b'])
        self.assertEqual(tasm.lex.parse_args(text='_savesp_245D0, sp'),[u'_savesp_245D0', u'sp'])
        self.assertEqual(tasm.lex.parse_args(text='_word_245D2, 0Fh'),[u'_word_245D2', u'0Fh'])
        self.assertEqual(tasm.lex.parse_args(text='_word_245D2, 1Fh'),[u'_word_245D2', u'1Fh'])
        self.assertEqual(tasm.lex.parse_args(text='_word_24662, 0'),[u'_word_24662', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='ah, 3Eh'),[u'ah', u'3Eh'])
        self.assertEqual(tasm.lex.parse_args(text='ah,03dh'),[u'ah', u'03dh'])
        self.assertEqual(tasm.lex.parse_args(text='ah,03eh'),[u'ah', u'03eh'])
        self.assertEqual(tasm.lex.parse_args(text='ah,03fh'),[u'ah', u'03fh'])
        self.assertEqual(tasm.lex.parse_args(text='ah,042h'),[u'ah', u'042h'])
        self.assertEqual(tasm.lex.parse_args(text='ah,4ch ; AH=4Ch - Exit To DOS'),[u'ah', u'4ch'])
        self.assertEqual(tasm.lex.parse_args(text='ah,7'),[u'ah', u'7'])
        self.assertEqual(tasm.lex.parse_args(text='ah,9 ; AH=09h - Print DOS Message'),[u'ah', u'9'])
        self.assertEqual(tasm.lex.parse_args(text='al,0'),[u'al', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='al,00h ;debut du fichier'),[u'al', u'00h'])
        self.assertEqual(tasm.lex.parse_args(text='al,00h ;ouverture du fichier pour lecture.'),[u'al', u'00h'])
        self.assertEqual(tasm.lex.parse_args(text='al,1'),[u'al', u'1'])
        self.assertEqual(tasm.lex.parse_args(text='al,7'),[u'al', u'7'])
        self.assertEqual(tasm.lex.parse_args(text='al,ah'),[u'al', u'ah'])
        self.assertEqual(tasm.lex.parse_args(text='ax'),[u'ax'])
        self.assertEqual(tasm.lex.parse_args(text='ax, 0FFFEh'),[u'ax', u'0FFFEh'])
        self.assertEqual(tasm.lex.parse_args(text='ax, 0FFFFh'),[u'ax', u'0FFFFh'])
        self.assertEqual(tasm.lex.parse_args(text='ax, 3D00h'),[u'ax', u'3D00h'])
        self.assertEqual(tasm.lex.parse_args(text='ax, 4200h'),[u'ax', u'4200h'])
        self.assertEqual(tasm.lex.parse_args(text='ax, [bx]'),[u'ax', u'[bx]'])
        self.assertEqual(tasm.lex.parse_args(text='ax, _byte_2461B'),[u'ax', u'_byte_2461B'])
        self.assertEqual(tasm.lex.parse_args(text='ax, _mod_channels_number'),[u'ax', u'_mod_channels_number'])
        self.assertEqual(tasm.lex.parse_args(text='ax, ax'),[u'ax', u'ax'])
        self.assertEqual(tasm.lex.parse_args(text='ax, ds'),[u'ax', u'ds'])
        self.assertEqual(tasm.lex.parse_args(text='ax, offset _mod_n_t_module ; N.T.'),[u'ax', u'offset _mod_n_t_module'])
        self.assertEqual(tasm.lex.parse_args(text='bl,0'),[u'bl', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='bl,0Fh ; only low-Nibble'),[u'bl', u'0Fh'])
        self.assertEqual(tasm.lex.parse_args(text='bl,30h ; convert to ASCII'),[u'bl', u'30h'])
        self.assertEqual(tasm.lex.parse_args(text='bl,39h ; above 9?'),[u'bl', u'39h'])
        self.assertEqual(tasm.lex.parse_args(text='bl,7 ; "A" to "F"'),[u'bl', u'7'])
        self.assertEqual(tasm.lex.parse_args(text='bl,al'),[u'bl', u'al'])
        self.assertEqual(tasm.lex.parse_args(text='bx, _fhandle_module'),[u'bx', u'_fhandle_module'])
        self.assertEqual(tasm.lex.parse_args(text='bx, cx'),[u'bx', u'cx'])
        self.assertEqual(tasm.lex.parse_args(text='bx, offset off_25326'),[u'bx', u'offset off_25326'])
        self.assertEqual(tasm.lex.parse_args(text='bx, seg003'),[u'bx', u'seg003'])
        self.assertEqual(tasm.lex.parse_args(text='bx, si'),[u'bx', u'si'])
        self.assertEqual(tasm.lex.parse_args(text='byte ptr [edi+1]'),[u'byte ptr [edi+1]'])
        self.assertEqual(tasm.lex.parse_args(text='byte ptr [edi+1],6'),[u'byte ptr [edi+1]', u'6'])
        self.assertEqual(tasm.lex.parse_args(text='byte ptr [edi+7]'),[u'byte ptr [edi+7]'])
        self.assertEqual(tasm.lex.parse_args(text='byte ptr [edi+7],132'),[u'byte ptr [edi+7]', u'132'])
        self.assertEqual(tasm.lex.parse_args(text='byte ptr dl,[edi]'),[u'byte ptr dl', u'[edi]'])
        self.assertEqual(tasm.lex.parse_args(text='cl ; decrease loop counter'),[u'cl'])
        self.assertEqual(tasm.lex.parse_args(text='cl,2'),[u'cl', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='cl,8 ; number of ASCII'),[u'cl', u'8'])
        self.assertEqual(tasm.lex.parse_args(text='cs'),[u'cs'])
        self.assertEqual(tasm.lex.parse_args(text='cx, 1084'),[u'cx', u'1084'])
        self.assertEqual(tasm.lex.parse_args(text='cx, byte ptr [bx+4]'),[u'cx', u'byte ptr [bx+4]'])
        self.assertEqual(tasm.lex.parse_args(text='cx, cx'),[u'cx', u'cx'])
        self.assertEqual(tasm.lex.parse_args(text='di, [bx+2]'),[u'di', u'[bx+2]'])
        self.assertEqual(tasm.lex.parse_args(text='di, offset _chrin'),[u'di', u'offset _chrin'])
        self.assertEqual(tasm.lex.parse_args(text='dl'),[u'dl'])
        self.assertEqual(tasm.lex.parse_args(text='dl, 23'),[u'dl', u'23'])
        self.assertEqual(tasm.lex.parse_args(text='dl,2'),[u'dl', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='dl,4'),[u'dl', u'4'])
        self.assertEqual(tasm.lex.parse_args(text='dl,5'),[u'dl', u'5'])
        self.assertEqual(tasm.lex.parse_args(text='dl,[edi+1]'),[u'dl', u'[edi+1]'])
        self.assertEqual(tasm.lex.parse_args(text='dl,[edi]'),[u'dl', u'[edi]'])
        self.assertEqual(tasm.lex.parse_args(text='dl,var1'),[u'dl', u'var1'])
        self.assertEqual(tasm.lex.parse_args(text='ds es'),[u'ds es'])
        self.assertEqual(tasm.lex.parse_args(text='ds'),[u'ds'])
        self.assertEqual(tasm.lex.parse_args(text='ds, _data'),[u'ds', u'_data'])
        self.assertEqual(tasm.lex.parse_args(text='ds, bx'),[u'ds', u'bx'])
        self.assertEqual(tasm.lex.parse_args(text='ds:[edi],cl'),[u'ds:[edi]', u'cl'])
        self.assertEqual(tasm.lex.parse_args(text='dx'),[u'dx'])
        self.assertEqual(tasm.lex.parse_args(text='dx, 258h'),[u'dx', u'258h'])
        self.assertEqual(tasm.lex.parse_args(text='dx, offset _aNotEnoughMemory ; "Not enough Memory available\\r\\n"'),[u'dx', u'offset _aNotEnoughMemory'])
        self.assertEqual(tasm.lex.parse_args(text='dx, offset _chrin'),[u'dx', u'offset _chrin'])
        self.assertEqual(tasm.lex.parse_args(text='dx, offset _eModuleNotFound ; "Module not found\\r\\n"'),[u'dx', u'offset _eModuleNotFound'])
        self.assertEqual(tasm.lex.parse_args(text='dx,11'),[u'dx', u'11'])
        self.assertEqual(tasm.lex.parse_args(text='dx,[edi+1]'),[u'dx', u'[edi+1]'])
        self.assertEqual(tasm.lex.parse_args(text='dx,cx'),[u'dx', u'cx'])
        self.assertEqual(tasm.lex.parse_args(text='eax'),[u'eax'])
        self.assertEqual(tasm.lex.parse_args(text='eax, B'),[u'eax', u'B'])
        self.assertEqual(tasm.lex.parse_args(text='eax, CC'),[u'eax', u'CC'])
        self.assertEqual(tasm.lex.parse_args(text='eax, DDD'),[u'eax', u'DDD'])
        self.assertEqual(tasm.lex.parse_args(text='eax,-1'),[u'eax', u'-1'])
        self.assertEqual(tasm.lex.parse_args(text='eax,-1-(-2+3)'),[u'eax', u'-1-(-2+3)'])
        self.assertEqual(tasm.lex.parse_args(text='eax,-13'),[u'eax', u'-13'])
        self.assertEqual(tasm.lex.parse_args(text='eax,-2'),[u'eax', u'-2'])
        self.assertEqual(tasm.lex.parse_args(text='eax,1'),[u'eax', u'1'])
        self.assertEqual(tasm.lex.parse_args(text='eax,133'),[u'eax', u'133'])
        self.assertEqual(tasm.lex.parse_args(text='eax,2'),[u'eax', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='eax,3'),[u'eax', u'3'])
        self.assertEqual(tasm.lex.parse_args(text='eax,4'),[u'eax', u'4'])
        self.assertEqual(tasm.lex.parse_args(text='eax,6'),[u'eax', u'6'])
        self.assertEqual(tasm.lex.parse_args(text='eax,eax'),[u'eax', u'eax'])
        self.assertEqual(tasm.lex.parse_args(text='eax,ebx'),[u'eax', u'ebx'])
        self.assertEqual(tasm.lex.parse_args(text='eax,enddata'),[u'eax', u'enddata'])
        self.assertEqual(tasm.lex.parse_args(text='eax,teST2'),[u'eax', u'teST2'])
        self.assertEqual(tasm.lex.parse_args(text='ebp,3*4'),[u'ebp', u'3*4'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,-1'),[u'ebx', u'-1'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,0'),[u'ebx', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,0ffffff00h'),[u'ebx', u'0ffffff00h'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,1'),[u'ebx', u'1'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,2'),[u'ebx', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,3'),[u'ebx', u'3'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,5'),[u'ebx', u'5'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,[load_handle]'),[u'ebx', u'[load_handle]'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,beginningdata'),[u'ebx', u'beginningdata'])
        self.assertEqual(tasm.lex.parse_args(text='ebx,eax'),[u'ebx', u'eax'])
        self.assertEqual(tasm.lex.parse_args(text='ecx,10'),[u'ecx', u'10'])
        self.assertEqual(tasm.lex.parse_args(text='ecx,16'),[u'ecx', u'16'])
        self.assertEqual(tasm.lex.parse_args(text='ecx,2'),[u'ecx', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='ecx,3'),[u'ecx', u'3'])
        self.assertEqual(tasm.lex.parse_args(text='edi ; increase target address'),[u'edi'])
        self.assertEqual(tasm.lex.parse_args(text='edi,14*320'),[u'edi', u'14*320'])
        self.assertEqual(tasm.lex.parse_args(text='edi,OFFSET AsCii ; get the offset address'),[u'edi', u'OFFSET AsCii'])
        self.assertEqual(tasm.lex.parse_args(text='edi,buffer'),[u'edi', u'buffer'])
        self.assertEqual(tasm.lex.parse_args(text='edi,edi'),[u'edi', u'edi'])
        self.assertEqual(tasm.lex.parse_args(text='edi,esi'),[u'edi', u'esi'])
        self.assertEqual(tasm.lex.parse_args(text='edi,offset var1'),[u'edi', u'offset var1'])
        self.assertEqual(tasm.lex.parse_args(text='edi,offset var2'),[u'edi', u'offset var2'])
        self.assertEqual(tasm.lex.parse_args(text='edx,0'),[u'edx', u'0'])
        self.assertEqual(tasm.lex.parse_args(text='edx,OFFSET ASCiI ; DOS 1+ WRITE STRING TO STANDARD OUTPUT'),[u'edx', u'OFFSET ASCiI'])
        self.assertEqual(tasm.lex.parse_args(text='edx,edi'),[u'edx', u'edi'])
        self.assertEqual(tasm.lex.parse_args(text='edx,fileName'),[u'edx', u'fileName'])
        self.assertEqual(tasm.lex.parse_args(text='edx,offset _msg ; DS:EDX -> $ Terminated String'),[u'edx', u'offset _msg'])
        self.assertEqual(tasm.lex.parse_args(text='es ds'),[u'es ds'])
        self.assertEqual(tasm.lex.parse_args(text='esi,offset var2'),[u'esi', u'offset var2'])
        self.assertEqual(tasm.lex.parse_args(text='esi,var2'),[u'esi', u'var2'])
        self.assertEqual(tasm.lex.parse_args(text='exitLabel'),[u'exitLabel'])
        self.assertEqual(tasm.lex.parse_args(text='failure'),[u'failure'])
        self.assertEqual(tasm.lex.parse_args(text='fs, ax'),[u'fs', u'ax'])
        self.assertEqual(tasm.lex.parse_args(text='incebx'),[u'incebx'])
        self.assertEqual(tasm.lex.parse_args(text='load_raw'),[u'load_raw'])
        self.assertEqual(tasm.lex.parse_args(text='loc_101B7'),[u'loc_101B7'])
        self.assertEqual(tasm.lex.parse_args(text='near ptr _clean_11C43'),[u'near ptr _clean_11C43'])
        self.assertEqual(tasm.lex.parse_args(text='near ptr _memfree_125DA'),[u'near ptr _memfree_125DA'])
        self.assertEqual(tasm.lex.parse_args(text='near ptr _snd_offx'),[u'near ptr _snd_offx'])
        self.assertEqual(tasm.lex.parse_args(text='near ptr sub_12B18'),[u'near ptr sub_12B18'])
        self.assertEqual(tasm.lex.parse_args(text='near ptr sub_12B83'),[u'near ptr sub_12B83'])
        self.assertEqual(tasm.lex.parse_args(text='noerror'),[u'noerror'])
        self.assertEqual(tasm.lex.parse_args(text='offset var5'),[u'offset var5'])
        self.assertEqual(tasm.lex.parse_args(text='printeax'),[u'printeax'])
        self.assertEqual(tasm.lex.parse_args(text='short P2'),[u'short P2'])
        self.assertEqual(tasm.lex.parse_args(text='short _lfreaderr'),[u'short _lfreaderr'])
        self.assertEqual(tasm.lex.parse_args(text='short loc_10045'),[u'short loc_10045'])
        self.assertEqual(tasm.lex.parse_args(text='short loc_10064'),[u'short loc_10064'])
        self.assertEqual(tasm.lex.parse_args(text='short loc_1008A'),[u'short loc_1008A'])
        self.assertEqual(tasm.lex.parse_args(text='si, [bx+5]'),[u'si', u'[bx+5]'])
        self.assertEqual(tasm.lex.parse_args(text='si, offset _byte_27FE8'),[u'si', u'offset _byte_27FE8'])
        self.assertEqual(tasm.lex.parse_args(text='si, offset _byte_306DE'),[u'si', u'offset _byte_306DE'])
        self.assertEqual(tasm.lex.parse_args(text='si, offset _byte_308BE'),[u'si', u'offset _byte_308BE'])
        self.assertEqual(tasm.lex.parse_args(text='si, offset _dword_27BC8'),[u'si', u'offset _dword_27BC8'])
        self.assertEqual(tasm.lex.parse_args(text='sp, _savesp_245D0'),[u'sp', u'_savesp_245D0'])
        self.assertEqual(tasm.lex.parse_args(text='test2'),[u'test2'])
        self.assertEqual(tasm.lex.parse_args(text='var1'),[u'var1'])
        self.assertEqual(tasm.lex.parse_args(text='var1,1'),[u'var1', u'1'])
        self.assertEqual(tasm.lex.parse_args(text='var1,2'),[u'var1', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='var1,al'),[u'var1', u'al'])
        self.assertEqual(tasm.lex.parse_args(text='var1[1],2'),[u'var1[1]', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='var1[bx+si],2'),[u'var1[bx+si]', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='var1[bx],2'),[u'var1[bx]', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='var2,2'),[u'var2', u'2'])
        self.assertEqual(tasm.lex.parse_args(text='var2,bx'),[u'var2', u'bx'])
        self.assertEqual(tasm.lex.parse_args(text='var3'),[u'var3'])
        self.assertEqual(tasm.lex.parse_args(text='var3+3*4,4000000'),[u'var3+3*4', u'4000000'])
        self.assertEqual(tasm.lex.parse_args(text='var3+ebp,4000000'),[u'var3+ebp', u'4000000'])
        self.assertEqual(tasm.lex.parse_args(text='var3,-12'),[u'var3', u'-12'])
        self.assertEqual(tasm.lex.parse_args(text='var3,-13'),[u'var3', u'-13'])
        self.assertEqual(tasm.lex.parse_args(text='var3,3'),[u'var3', u'3'])
        self.assertEqual(tasm.lex.parse_args(text='var3,ecx'),[u'var3', u'ecx'])
        self.assertEqual(tasm.lex.parse_args(text='word ptr [var5+2],25'),[u'word ptr [var5+2]', u'25'])
        self.assertEqual(tasm.lex.parse_args(text='word ptr [var5+2],50'),[u'word ptr [var5+2]', u'50'])
        self.assertEqual(tasm.lex.parse_args(text='word ptr var5,0'),[u'word ptr var5', u'0'])
        self.assertEqual(tasm.lex.parse_args(text=u'ax'),[u'ax'])
        self.assertEqual(tasm.lex.parse_args(text=u'cs'),[u'cs'])
        self.assertEqual(tasm.lex.parse_args(text=u'ds'),[u'ds'])
        self.assertEqual(tasm.lex.parse_args(text=u'dx'),[u'dx'])
        self.assertEqual(tasm.lex.parse_args(text=u'es'),[u'es'])

if __name__ == "__main__":
    unittest.main()
