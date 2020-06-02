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
from tasm.parser import Parser
import tasm.proc
from tasm.proc import Proc
import traceback
import unittest


class ProcTest(unittest.TestCase):

    @patch.object(_jbe, '__init__')
    @patch.object(_lea, '__init__')
    @patch.object(_pushad, '__init__')
    @patch.object(_cmp, '__init__')
    @patch.object(_jmp, '__init__')
    @patch.object(_xor, '__init__')
    @patch.object(_shr, '__init__')
    @patch.object(_pop, '__init__')
    @patch.object(_je, '__init__')
    @patch.object(_push, '__init__')
    @patch.object(_ret, '__init__')
    @patch.object(_jne, '__init__')
    @patch.object(_sti, '__init__')
    @patch.object(_int, '__init__')
    @patch.object(_mov, '__init__')
    @patch.object(_movzx, '__init__')
    @patch.object(_jnc, '__init__')
    @patch.object(_popad, '__init__')
    @patch.object(_stc, '__init__')
    @patch.object(_and, '__init__')
    @patch.object(_add, '__init__')
    @patch.object(_cld, '__init__')
    @patch.object(_sub, '__init__')
    @patch.object(_jna, '__init__')
    @patch.object(_cmpsb, '__init__')
    @patch.object(_ja, '__init__')
    @patch.object(_repe, '__init__')
    @patch.object(_jz, '__init__')
    @patch.object(_call, '__init__')
    @patch.object(_retf, '__init__')
    @patch.object(_dec, '__init__')
    @patch.object(_adc, '__init__')
    @patch.object(_jb, '__init__')
    @patch.object(_jnz, '__init__')
    @patch.object(_inc, '__init__')
    def test_add_(self, mock_jbe__init__,   mock_lea__init__,    mock_pushad__init__,    mock_cmp__init__,    mock_jmp__init__,    mock_xor__init__,    mock_shr__init__,    mock_pop__init__,    mock_je__init__,    mock_push__init__,    mock_ret__init__,    mock_jne__init__,    mock_sti__init__,    mock_int__init__,    mock_mov__init__,    mock_movzx__init__,    mock_jnc__init__,    mock_popad__init__,    mock_stc__init__,    mock_and__init__,    mock_add__init__,    mock_cld__init__,    mock_sub__init__,    mock_jna__init__,    mock_cmpsb__init__,    mock_ja__init__,    mock_repe__init__,    mock_jz__init__,    mock_call__init__,    mock_retf__init__,    mock_dec__init__,    mock_adc__init__,    mock_jb__init__,    mock_jnz__init__,    mock_inc__init__):
        mock_jbe__init__.return_value = None
        mock_lea__init__.return_value = None
        mock_pushad__init__.return_value = None
        mock_cmp__init__.return_value = None
        mock_jmp__init__.return_value = None
        mock_xor__init__.return_value = None
        mock_shr__init__.return_value = None
        mock_pop__init__.return_value = None
        mock_je__init__.return_value = None
        mock_push__init__.return_value = None
        mock_ret__init__.return_value = None
        mock_jne__init__.return_value = None
        mock_sti__init__.return_value = None
        mock_int__init__.return_value = None
        mock_mov__init__.return_value = None
        mock_movzx__init__.return_value = None
        mock_jnc__init__.return_value = None
        mock_popad__init__.return_value = None
        mock_stc__init__.return_value = None
        mock_and__init__.return_value = None
        mock_add__init__.return_value = None
        mock_cld__init__.return_value = None
        mock_sub__init__.return_value = None
        mock_jna__init__.return_value = None
        mock_cmpsb__init__.return_value = None
        mock_ja__init__.return_value = None
        mock_repe__init__.return_value = None
        mock_jz__init__.return_value = None
        mock_call__init__.return_value = None
        mock_retf__init__.return_value = None
        mock_dec__init__.return_value = None
        mock_adc__init__.return_value = None
        mock_jb__init__.return_value = None
        mock_jnz__init__.return_value = None
        mock_inc__init__.return_value = None

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=78,line='MOV al,0',stmt='MOV al,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=83,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=80,line='add\tdi, [bx+2]',stmt='add\tdi, [bx+2]'),tasm.op._add)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=113,line='mov\tax, _mod_channels_number',stmt='mov\tax, _mod_channels_number'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=17,line='mov ebx,5',stmt='mov ebx,5'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=19,line='int 21h                         ; DOS INT 21h',stmt='int 21h                         ; DOS INT 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=72,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=40,line='CMP [var2],1',stmt='CMP [var2],1'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=21,line='cmp var1[bx+si],2',stmt='cmp var1[bx+si],2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=53,line='mov eax, B',stmt='mov eax, B'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=105,line='dec cl              ; decrease loop counter',stmt='dec cl              ; decrease loop counter'),tasm.op._dec)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=71,line='cmp dx,11',stmt='cmp dx,11'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=60,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=21,line='mov eax,1',stmt='mov eax,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=87,line='cld',stmt='cld'),tasm.op._cld)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=46,line='mov\t_savesp_245D0,\tsp',stmt='mov\t_savesp_245D0,\tsp'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=169,line='mov\tbx, _fhandle_module',stmt='mov\tbx, _fhandle_module'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=6,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=142,line='push\tcs',stmt='push\tcs'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=30,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=19,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=47,line='int 21h',stmt='int 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=46,line='cmp [var3+3*4],4000000',stmt='cmp [var3+3*4],4000000'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=47,line='JA failure',stmt='JA failure'),tasm.op._ja)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=102,line='add bl,7            ; "A" to "F"',stmt='add bl,7            ; "A" to "F"'),tasm.op._add)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=32,line='inc byte ptr [edi+1]',stmt='inc byte ptr [edi+1]'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=66,line='cmp dl,2',stmt='cmp dl,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=125,line='retf',stmt='retf'),tasm.op._retf)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=25,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=45,line='JNE failure',stmt='JNE failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=53,line='mov ah,042h',stmt='mov ah,042h'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=94,line='mov edi,OFFSET AsCii ; get the offset address',stmt='mov edi,OFFSET AsCii ; get the offset address'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='mov dl,[edi]',stmt='mov dl,[edi]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=33,line='CMP [var1],111',stmt='CMP [var1],111'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=56,line='shr ecx,16',stmt='shr ecx,16'),tasm.op._shr)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=136,line='push\tdx',stmt='push\tdx'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=55,line='mov dx,cx',stmt='mov dx,cx'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=19,line='call load_raw',stmt='call load_raw'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=30,line='push\tdx',stmt='push\tdx'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=70,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=52,line='mov ebx,[load_handle]',stmt='mov ebx,[load_handle]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=16,line='cmp var1[bx],2',stmt='cmp var1[bx],2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=104,line='inc edi              ; increase target address',stmt='inc edi              ; increase target address'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=33,line='cmp var3,3',stmt='cmp var3,3'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=23,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=40,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=56,line='cmp var3,-12',stmt='cmp var3,-12'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=28,line='jmp exitLabel',stmt='jmp exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=138,line='mov\tah, 3Eh',stmt='mov\tah, 3Eh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=104,line='call\t_ems_restore_mapctx',stmt='call\t_ems_restore_mapctx'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=29,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=81,line='lea\tsi, [bx+5]',stmt='lea\tsi, [bx+5]'),tasm.op._lea)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=96,line='call\tax',stmt='call\tax'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=60,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=23,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=36,line='INC [var2]',stmt='INC [var2]'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=18,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=110,line='inc\tax',stmt='inc\tax'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=54,line='cmp eax,3',stmt='cmp eax,3'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=25,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=33,line='int 21h                       ; DOS INT 21h                     ; DOS INT 21h',stmt='int 21h                       ; DOS INT 21h                     ; DOS INT 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=42,line='call printeax',stmt='call printeax'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=23,line='mov bl,0',stmt='mov bl,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=49,line='mov eax,-1-(-2+3)',stmt='mov eax,-1-(-2+3)'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=36,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=61,line='mov ebx,[load_handle]',stmt='mov ebx,[load_handle]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=46,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=44,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=23,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=22,line='JNE failure',stmt='JNE failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=178,line='mov\t_module_type_text, 38544C46h ;\tFLT8',stmt='mov\t_module_type_text, 38544C46h ;\tFLT8'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=63,line='mov edi,offset var1',stmt='mov edi,offset var1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=27,line='mov edi,offset var1',stmt='mov edi,offset var1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=25,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=15,line='cmp var1[1],2',stmt='cmp var1[1],2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=130,line='mov\tdx, offset _aNotEnoughMemory ; "Not enough Memory available\\r\\n"',stmt='mov\tdx, offset _aNotEnoughMemory ; "Not enough Memory available\\r\\n"'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=48,line='mov eax,teST2',stmt='mov eax,teST2'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=100,line='mov\tah, 3Eh',stmt='mov\tah, 3Eh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=46,line='CMP [var3],37',stmt='CMP [var3],37'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=20,line='cmp [var],5',stmt='cmp [var],5'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=123,line='xor\tax, ax',stmt='xor\tax, ax'),tasm.op._xor)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=39,line='JNE failure',stmt='JNE failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=23,line='cmp eax,2',stmt='cmp eax,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=29,line='push\tds',stmt='push\tds'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=40,line='lea edx,fileName',stmt='lea edx,fileName'),tasm.op._lea)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=76,line='movzx\tcx, byte ptr [bx+4]',stmt='movzx\tcx, byte ptr [bx+4]'),tasm.op._movzx)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=91,line='jnz\tshort loc_10045',stmt='jnz\tshort loc_10045'),tasm.op._jnz)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=46,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=174,line='jmp\tloc_101B7',stmt='jmp\tloc_101B7'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=33,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=30,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=53,line='call\t_ems_save_mapctx',stmt='call\t_ems_save_mapctx'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=31,line='CMP [var1],3',stmt='CMP [var1],3'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=39,line='cmp dl,4',stmt='cmp dl,4'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=90,line='dec\tdl',stmt='dec\tdl'),tasm.op._dec)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=17,line='CMP eax,1',stmt='CMP eax,1'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=79,line='int 21h',stmt='int 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=14,line='mov edx,0',stmt='mov edx,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=92,line='mov\tax, offset _mod_n_t_module ; N.T.',stmt='mov\tax, offset _mod_n_t_module ; N.T.'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=180,line='mov\t_word_245D2, 1Fh',stmt='mov\t_word_245D2, 1Fh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=17,line='cmp var1[bx+si],2',stmt='cmp var1[bx+si],2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=22,line='cmp var1,al',stmt='cmp var1,al'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=50,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=37,line='pop\tdx',stmt='pop\tdx'),tasm.op._pop)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=41,line='MOV al,0',stmt='MOV al,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=89,line='jz\tshort loc_10064',stmt='jz\tshort loc_10064'),tasm.op._jz)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=43,line='cmp [var3],11',stmt='cmp [var3],11'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=18,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=45,line='MOV al,0',stmt='MOV al,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=36,line="mov al, 'Z' - 'A' +1",stmt="mov al, 'Z' - 'A' +1"),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=49,line='MOV al,0',stmt='MOV al,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=21,line='xor eax,eax',stmt='xor eax,eax'),tasm.op._xor)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=75,line='cmp byte ptr [edi+7],132',stmt='cmp byte ptr [edi+7],132'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=43,line='cmp edi,esi',stmt='cmp edi,esi'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=2,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=40,line='cmp [var2+2],6',stmt='cmp [var2+2],6'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=101,line='int\t21h\t\t; DOS -\t2+ - CLOSE A FILE WITH HANDLE',stmt='int\t21h\t\t; DOS -\t2+ - CLOSE A FILE WITH HANDLE'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=17,line='cmp word ptr [var5+2],50',stmt='cmp word ptr [var5+2],50'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=55,line='jb failure',stmt='jb failure'),tasm.op._jb)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=31,line='sub ebx,eax',stmt='sub ebx,eax'),tasm.op._sub)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=54,line='cld',stmt='cld'),tasm.op._cld)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=51,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=57,line='mov\t_byte_2461B, 0',stmt='mov\t_byte_2461B, 0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=32,line='JA failure',stmt='JA failure'),tasm.op._ja)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='lea ebx,beginningdata',stmt='lea ebx,beginningdata'),tasm.op._lea)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=147,line='pop\tdx',stmt='pop\tdx'),tasm.op._pop)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=41,line='int 21h                       ; DOS INT 21h',stmt='int 21h                       ; DOS INT 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=24,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='CMP [var2],13',stmt='CMP [var2],13'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=111,line="mov ah,9            ; DS:DX->'$'-terminated string",stmt="mov ah,9            ; DS:DX->'$'-terminated string"),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=118,line='mov\tsi, offset _dword_27BC8',stmt='mov\tsi, offset _dword_27BC8'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=34,line='movzx eax, DDD',stmt='movzx eax, DDD'),tasm.op._movzx)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=20,line='sub word ptr [var5+2],25',stmt='sub word ptr [var5+2],25'),tasm.op._sub)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=20,line='INC eax',stmt='INC eax'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=22,line='mov ds:[edi],cl',stmt='mov ds:[edi],cl'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=77,line='mov ebx,[load_handle]',stmt='mov ebx,[load_handle]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=53,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=37,line='cmp [var2],4',stmt='cmp [var2],4'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=37,line='JB failure',stmt='JB failure'),tasm.op._jb)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=137,line='mov\tbx, _fhandle_module',stmt='mov\tbx, _fhandle_module'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=16,line='INC eax',stmt='INC eax'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=16,line='lea edi,buffer',stmt='lea edi,buffer'),tasm.op._lea)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=144,line='mov\tax, ds',stmt='mov\tax, ds'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=17,line='mov byte ptr dl,[edi]',stmt='mov byte ptr dl,[edi]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=60,line='mov\t_word_24662, 0',stmt='mov\t_word_24662, 0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=24,line='cmp ebx,0ffffff00h',stmt='cmp ebx,0ffffff00h'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=27,line='xor eax,eax',stmt='xor eax,eax'),tasm.op._xor)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=54,line='mov al,00h ;debut du fichier',stmt='mov al,00h ;debut du fichier'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=46,line='mov eax,1',stmt='mov eax,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=55,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=65,line='int 21h                       ; DOS INT 21h',stmt='int 21h                       ; DOS INT 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=149,line='mov\tsp, _savesp_245D0',stmt='mov\tsp, _savesp_245D0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=28,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=21,line='mov cl,2',stmt='mov cl,2'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=23,line='cmp var1,2',stmt='cmp var1,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=47,line='JE failure',stmt='JE failure'),tasm.op._je)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=59,line='ret',stmt='ret'),tasm.op._ret)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=165,line='call\t_mod_1021E',stmt='call\t_mod_1021E'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=183,line='call\t_mod_1021E',stmt='call\t_mod_1021E'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=43,line='mov ah,03dh',stmt='mov ah,03dh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=15,line='mov ecx,2',stmt='mov ecx,2'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=61,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='push es ds',stmt='push es ds'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=57,line='int 21h',stmt='int 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=35,line='cmp eax,6',stmt='cmp eax,6'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=37,line='cmp var3,ecx',stmt='cmp var3,ecx'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=64,line='mov esi,offset var2',stmt='mov esi,offset var2'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=27,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=59,line='mov dl,var1',stmt='mov dl,var1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=52,line='mov ebp,3*4',stmt='mov ebp,3*4'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=18,line='cmp dl,2',stmt='cmp dl,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=14,line='MOV ds, _data',stmt='MOV ds, _data'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=23,line='mov byte ptr dl,[edi]',stmt='mov byte ptr dl,[edi]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=73,line='mov\tdl, 23',stmt='mov\tdl, 23'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=37,line='INC [var2]',stmt='INC [var2]'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=30,line='mov ebx,2',stmt='mov ebx,2'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=100,line='cmp bl,39h          ; above 9?',stmt='cmp bl,39h          ; above 9?'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=112,line='jbe\tshort loc_1008A',stmt='jbe\tshort loc_1008A'),tasm.op._jbe)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=41,line='JE failure',stmt='JE failure'),tasm.op._je)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=50,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=35,line='push\tcs',stmt='push\tcs'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=70,line='mov dx,[edi+1]',stmt='mov dx,[edi+1]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=79,line='mov\tdi, offset _chrin',stmt='mov\tdi, offset _chrin'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=32,line='cmp [var1+1],5',stmt='cmp [var1+1],5'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=44,line='JB failure',stmt='JB failure'),tasm.op._jb)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=95,line='mov\t_byte_24665, 1',stmt='mov\t_byte_24665, 1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=151,line='stc',stmt='stc'),tasm.op._stc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=119,line='push\tcs',stmt='push\tcs'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=179,line='mov\t_moduleflag_246D0, 11b',stmt='mov\t_moduleflag_246D0, 11b'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=36,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=67,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=26,line='mov eax, CC',stmt='mov eax, CC'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=24,line='cmp [var+3],5',stmt='cmp [var+3],5'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=163,line='mov\t_mod_channels_number, 4',stmt='mov\t_mod_channels_number, 4'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=65,line='MOV al,0',stmt='MOV al,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=74,line='inc byte ptr [edi+7]',stmt='inc byte ptr [edi+7]'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=31,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=27,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=141,line='call\t_ems_restore_mapctx',stmt='call\t_ems_restore_mapctx'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=47,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=4,line='jmp failure',stmt='jmp failure'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=32,line='cmp [var-1],0',stmt='cmp [var-1],0'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=46,line='MOV al,0',stmt='MOV al,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=14,line='sti                             ; Set The Interrupt Flag',stmt='sti                             ; Set The Interrupt Flag'),tasm.op._sti)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=36,line='mov ecx,3',stmt='mov ecx,3'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=171,line='int\t21h\t\t; DOS -\t2+ - MOVE FILE READ/WRITE POINTER (LSEEK)',stmt='int\t21h\t\t; DOS -\t2+ - MOVE FILE READ/WRITE POINTER (LSEEK)'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=50,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=68,line='int 21h',stmt='int 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=26,line='JNE failure',stmt='JNE failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=99,line='mov\tbx, _fhandle_module',stmt='mov\tbx, _fhandle_module'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=16,line='mov eax, B',stmt='mov eax, B'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=25,line='cmp var2,2',stmt='cmp var2,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=131,line='mov\tax, 0FFFEh',stmt='mov\tax, 0FFFEh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=135,line='push\tax',stmt='push\tax'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=75,line='pop ds',stmt='pop ds'),tasm.op._pop)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=0,line=u'repe',stmt=u'repe'),tasm.op._repe)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=29,line='cmp dl,5',stmt='cmp dl,5'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=59,line='mov ecx,10',stmt='mov ecx,10'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=40,line='sub eax,ebx',stmt='sub eax,ebx'),tasm.op._sub)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=167,line='mov\tdx, 258h',stmt='mov\tdx, 258h'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=34,line='MOV ds, _data',stmt='MOV ds, _data'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=21,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=34,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=31,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=145,line='mov\tfs, ax',stmt='mov\tfs, ax'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=164,line='mov\tsi, offset _byte_306DE',stmt='mov\tsi, offset _byte_306DE'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=43,line='cmp ebx,0',stmt='cmp ebx,0'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=32,line='call\tnear ptr _snd_offx',stmt='call\tnear ptr _snd_offx'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=24,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=27,line='cmp eax,4',stmt='cmp eax,4'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=62,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=86,line='add\tbx, cx',stmt='add\tbx, cx'),tasm.op._add)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=69,line='call\tnear ptr _clean_11C43',stmt='call\tnear ptr _clean_11C43'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=15,line='sti                             ; Set The Interrupt Flag',stmt='sti                             ; Set The Interrupt Flag'),tasm.op._sti)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=148,line='pop\tax',stmt='pop\tax'),tasm.op._pop)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=42,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=51,line='int 21h                       ; DOS INT 21h',stmt='int 21h                       ; DOS INT 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=64,line='push ds',stmt='push ds'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=41,line='xor eax,eax',stmt='xor eax,eax'),tasm.op._xor)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=22,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=22,line='mov eax, B',stmt='mov eax, B'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=17,line='MOV ds, _data',stmt='MOV ds, _data'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=181,line='mov\t_mod_channels_number, 8',stmt='mov\t_mod_channels_number, 8'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=1,line='MOV al,0',stmt='MOV al,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=53,line='int 21h                       ; DOS INT 21h',stmt='int 21h                       ; DOS INT 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=161,line='mov\t_module_type_text, 2E542E4Eh',stmt='mov\t_module_type_text, 2E542E4Eh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=52,line='cmp var3,-13',stmt='cmp var3,-13'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=139,line='int\t21h\t\t; DOS -\t2+ - CLOSE A FILE WITH HANDLE',stmt='int\t21h\t\t; DOS -\t2+ - CLOSE A FILE WITH HANDLE'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=31,line='push\tcs',stmt='push\tcs'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=24,line='cmp word ptr var5,0',stmt='cmp word ptr var5,0'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=28,line='JE failure',stmt='JE failure'),tasm.op._je)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=18,line='JNE failure',stmt='JNE failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=44,line='CMP [var3],35',stmt='CMP [var3],35'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=44,line='mov\tds, bx',stmt='mov\tds, bx'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=81,line='pop ds es',stmt='pop ds es'),tasm.op._pop)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='cmp ebx,1',stmt='cmp ebx,1'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=15,line='cld                             ; Clear The Direction Flag',stmt='cld                             ; Clear The Direction Flag'),tasm.op._cld)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=27,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=48,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=170,line='mov\tax, 4200h',stmt='mov\tax, 4200h'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=17,line='cmp eax,1',stmt='cmp eax,1'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=124,line='pop\tds',stmt='pop\tds'),tasm.op._pop)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=56,line='cmp var3+ebp,4000000',stmt='cmp var3+ebp,4000000'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=82,line='popad',stmt='popad'),tasm.op._popad)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=26,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=50,line='cmp eax,-2',stmt='cmp eax,-2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=19,line='cmp var1[1],2',stmt='cmp var1[1],2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=110,line='mov edx,OFFSET ASCiI ; DOS 1+ WRITE STRING TO STANDARD OUTPUT',stmt='mov edx,OFFSET ASCiI ; DOS 1+ WRITE STRING TO STANDARD OUTPUT'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=51,line='mov\tax, 0FFFFh',stmt='mov\tax, 0FFFFh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=25,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=48,line='mov\tdx, offset _eModuleNotFound ; "Module not found\\r\\n"',stmt='mov\tdx, offset _eModuleNotFound ; "Module not found\\r\\n"'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=47,line='mov\t_fhandle_module, ax',stmt='mov\t_fhandle_module, ax'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=79,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=22,line='mov ebx,-1',stmt='mov ebx,-1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=27,line='CMP eax,133',stmt='CMP eax,133'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=49,line='cmp eax,-13',stmt='cmp eax,-13'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=68,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=46,line='JB failure',stmt='JB failure'),tasm.op._jb)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=37,line='pushad',stmt='pushad'),tasm.op._pushad)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=72,line='mov\tbx, offset off_25326',stmt='mov\tbx, offset off_25326'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=34,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=52,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=54,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=116,line='push\tcs',stmt='push\tcs'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=81,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=173,line='adc\t_word_24662, 0',stmt='adc\t_word_24662, 0'),tasm.op._adc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=85,line='mov\tbx, si',stmt='mov\tbx, si'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=62,line='mov ah,03fh',stmt='mov ah,03fh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=28,line='mov dl,[edi+1]',stmt='mov dl,[edi+1]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=50,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=98,line='and bl,0Fh          ; only low-Nibble',stmt='and bl,0Fh          ; only low-Nibble'),tasm.op._and)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=71,line='int 21h',stmt='int 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=9,line='int 21h',stmt='int 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=24,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=0,line='cmpsb',stmt='cmpsb'),tasm.op._cmpsb)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=42,line='cmp al,ah',stmt='cmp al,ah'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=29,line='mov ebx,2',stmt='mov ebx,2'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=53,line='cmp [var3+ebp],4000000',stmt='cmp [var3+ebp],4000000'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=16,line='cld                             ; Clear The Direction Flag',stmt='cld                             ; Clear The Direction Flag'),tasm.op._cld)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=54,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=41,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=58,line="sub dl,'a'",stmt="sub dl,'a'"),tasm.op._sub)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=16,line='mov edi,offset var1',stmt='mov edi,offset var1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=65,line='mov byte ptr dl,[edi]',stmt='mov byte ptr dl,[edi]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=33,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=18,line='cmp var1,1',stmt='cmp var1,1'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=45,line='jnc noerror',stmt='jnc noerror'),tasm.op._jnc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=36,line='call\tnear ptr _memfree_125DA',stmt='call\tnear ptr _memfree_125DA'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=45,line="cmp [doublequote+4],'d'",stmt="cmp [doublequote+4],'d'"),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=20,line='cmp var1[bx],2',stmt='cmp var1[bx],2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=41,line='cmp ebx,2',stmt='cmp ebx,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=45,line='cmp ebx,1',stmt='cmp ebx,1'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=39,line='int\t21h\t\t; DOS -\t2+ - OPEN DISK FILE WITH HANDLE',stmt='int\t21h\t\t; DOS -\t2+ - OPEN DISK FILE WITH HANDLE'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=42,line='JA failure',stmt='JA failure'),tasm.op._ja)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=37,line='mov edi,offset var2',stmt='mov edi,offset var2'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=143,line='call\tnear ptr _memfree_125DA',stmt='call\tnear ptr _memfree_125DA'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=62,line='xor edi,edi',stmt='xor edi,edi'),tasm.op._xor)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=66,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=59,line='cmp dl,2',stmt='cmp dl,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=52,line='jb\tshort _lfreaderr',stmt='jb\tshort _lfreaderr'),tasm.op._jb)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=65,line='call\t_dosfread',stmt='call\t_dosfread'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=67,line='mov edx,edi',stmt='mov edx,edi'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=182,line='mov\tsi, offset _byte_308BE',stmt='mov\tsi, offset _byte_308BE'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=101,line='jna short P2',stmt='jna short P2'),tasm.op._jna)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=106,line='jnz P1              ; jump if cl is not equal 0 (zeroflag is not set)',stmt='jnz P1              ; jump if cl is not equal 0 (zeroflag is not set)'),tasm.op._jnz)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=52,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=22,line="cmp dword ptr buffer,'tseT'",stmt="cmp dword ptr buffer,'tseT'"),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=29,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=34,line='JE failure',stmt='JE failure'),tasm.op._je)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=84,line='mov\tax, [bx]',stmt='mov\tax, [bx]'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=22,line='cmp [var2],0',stmt='cmp [var2],0'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=16,line='add word ptr [var5+2],50',stmt='add word ptr [var5+2],50'),tasm.op._add)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=162,line='mov\t_word_245D2, 0Fh',stmt='mov\t_word_245D2, 0Fh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=111,line='cmp\tax, _mod_channels_number',stmt='cmp\tax, _mod_channels_number'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=64,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=60,line='cmp dl,2',stmt='cmp dl,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=57,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=19,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=76,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=57,line="mov dl,'c'",stmt="mov dl,'c'"),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=21,line='CMP eax,2',stmt='CMP eax,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=53,line='mov ebx,2',stmt='mov ebx,2'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=40,line='mov al,7',stmt='mov al,7'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=83,line='ret',stmt='ret'),tasm.op._ret)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=122,line='call\tnear ptr sub_12B18',stmt='call\tnear ptr sub_12B18'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=150,line='pop\tds',stmt='pop\tds'),tasm.op._pop)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=30,line='INC [var1]',stmt='INC [var1]'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=42,line='mov al,00h  ;ouverture du fichier pour lecture.',stmt='mov al,00h  ;ouverture du fichier pour lecture.'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=35,line='MOV al,0',stmt='MOV al,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=114,line='ret',stmt='ret'),tasm.op._ret)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=44,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=17,line='mov ah,9                        ; AH=09h - Print DOS Message',stmt='mov ah,9                        ; AH=09h - Print DOS Message'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=50,line='mov [load_handle],eax',stmt='mov [load_handle],eax'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=33,line='cmp byte ptr [edi+1],6',stmt='cmp byte ptr [edi+1],6'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=36,line='sub ebx,eax',stmt='sub ebx,eax'),tasm.op._sub)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=99,line='add bl,30h          ; convert to ASCII',stmt='add bl,30h          ; convert to ASCII'),tasm.op._add)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=152,line='retf',stmt='retf'),tasm.op._retf)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=41,line='inc eax',stmt='inc eax'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=26,line='cmp [var1],2',stmt='cmp [var1],2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=97,line='mov bl,al',stmt='mov bl,al'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=59,line='MOV al,0',stmt='MOV al,0'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=42,line='lea esi,var2',stmt='lea esi,var2'),tasm.op._lea)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=69,line='mov edi,offset var1',stmt='mov edi,offset var1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=63,line='add edi,14*320',stmt='add edi,14*320'),tasm.op._add)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=64,line='mov\tcx, 1084',stmt='mov\tcx, 1084'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=55,line='int 21h',stmt='int 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=95,line='mov cl,8            ; number of ASCII',stmt='mov cl,8            ; number of ASCII'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=24,line='cmp dl,2',stmt='cmp dl,2'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=63,line='mov\tdx, offset _chrin',stmt='mov\tdx, offset _chrin'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=28,line='cmp [var2-1],5',stmt='cmp [var2-1],5'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=44,line='int 21h',stmt='int 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=43,line='INC [var3]',stmt='INC [var3]'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=112,line='int 21h             ; maybe redirected under DOS 2+ for output to file',stmt='int 21h             ; maybe redirected under DOS 2+ for output to file'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=8,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=34,line='mov eax,2',stmt='mov eax,2'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=15,line='cld                             ; Clear The Direction Flag',stmt='cld                             ; Clear The Direction Flag'),tasm.op._cld)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=43,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=35,line='mov ebx,3',stmt='mov ebx,3'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=11,line='mov eax,-1',stmt='mov eax,-1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=32,line='JNE failure',stmt='JNE failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=14,line='sti                             ; Set The Interrupt Flag',stmt='sti                             ; Set The Interrupt Flag'),tasm.op._sti)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=84,line='int 21h                       ; DOS INT 21h',stmt='int 21h                       ; DOS INT 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        #self.assertEqual(proc_instance.add_(line_number=185,line="mov\tcx, 80h\t; '\x80'",stmt="mov\tcx, 80h\t; '\x80'"),tasm.op._mov)
        self.assertIsInstance(proc_instance.add_(line_number=185,line="mov\tcx, 80h\t; '\x80'",stmt="mov\tcx, 80h\t; '\x80'"),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=14,line='mov ds, _data',stmt='mov ds, _data'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=168,line='xor\tcx, cx',stmt='xor\tcx, cx'),tasm.op._xor)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=47,line='ret',stmt='ret'),tasm.op._ret)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=49,line='cmp var3+3*4,4000000',stmt='cmp var3+3*4,4000000'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=57,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=38,line='mov\tax, 3D00h',stmt='mov\tax, 3D00h'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=184,line='mov\tsi, offset _byte_27FE8',stmt='mov\tsi, offset _byte_27FE8'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=66,line='push\tcs',stmt='push\tcs'),tasm.op._push)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=32,line='mov ah,4ch                    ; AH=4Ch - Exit To DOS',stmt='mov ah,4ch                    ; AH=4Ch - Exit To DOS'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=55,line='inc var3',stmt='inc var3'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=117,line='call\tnear ptr sub_12B83',stmt='call\tnear ptr sub_12B83'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=30,line='cmp var2,bx',stmt='cmp var2,bx'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=78,line='mov ah,03eh',stmt='mov ah,03eh'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=24,line='INC eax',stmt='INC eax'),tasm.op._inc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=103,line='adc\t_word_24662, 0',stmt='adc\t_word_24662, 0'),tasm.op._adc)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=25,line='CMP eax,3',stmt='CMP eax,3'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=40,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=48,line='mov al,1',stmt='mov al,1'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=50,line='JMP exitLabel',stmt='JMP exitLabel'),tasm.op._jmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=18,line='mov edx,offset _msg             ; DS:EDX -> $ Terminated String',stmt='mov edx,offset _msg             ; DS:EDX -> $ Terminated String'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=43,line='mov\tbx, seg003',stmt='mov\tbx, seg003'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=39,line='lea eax,enddata',stmt='lea eax,enddata'),tasm.op._lea)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=15,line='xor eax,eax',stmt='xor eax,eax'),tasm.op._xor)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=30,line='cmp [var0+5],0',stmt='cmp [var0+5],0'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=107,line='movzx\tax, _byte_2461B',stmt='movzx\tax, _byte_2461B'),tasm.op._movzx)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=39,line='jne failure',stmt='jne failure'),tasm.op._jne)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=41,line='mov ah,7',stmt='mov ah,7'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=31,line='call incebx',stmt='call incebx'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=29,line='mov eax,3',stmt='mov eax,3'),tasm.op._mov)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=166,line='call\t_mod_102F5',stmt='call\t_mod_102F5'),tasm.op._call)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=26,line='cmp [var+4],0',stmt='cmp [var+4],0'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=30,line='int 21h                       ; DOS INT 21h',stmt='int 21h                       ; DOS INT 21h'),tasm.op._int)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=21,line='cmp word ptr [var5+2],25',stmt='cmp word ptr [var5+2],25'),tasm.op._cmp)

        proc_instance = Proc('mainproc', 0, False)
        self.assertIsInstance(proc_instance.add_(line_number=54,line='cmp eax,ebx',stmt='cmp eax,ebx'),tasm.op._cmp)


    def test_generate_c_cmd(self):
        p = Parser([])
        cpp_instance = cpp.Cpp(p)
        proc_instance = Proc('mainproc', 0, False)

        p.set_global("_data", op.var(1, 0, issegment=True))
        p.set_global("var1", op.var(size=1, offset=1, name="var1", segment="_data"), 1)
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance,'',proc_instance.add_("",0,'cmp var1[bx],2')),u'\tR(CMP(*(raddr(ds,offset(_data,var1)+bx)), 2));\n')

if __name__ == "__main__":
    unittest.main()
