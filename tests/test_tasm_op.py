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
from tasm.parser import Parser, calculate_type_size
import tasm.proc
from tasm.proc import Proc
import traceback
import unittest


class ParserTest(unittest.TestCase):
    '''
    def test_convert_data_to_blob(self):
        parser_instance = Parser([])
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'00000000'", '0Dh', '0Ah', "'$'"]), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'07/02/95 12:26:42'", '0']), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'100% assembler!'"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'1024'", '0']), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'13/02/95 21:15:58'", '0']), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'1 Thru 0'"]), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'2:284/116.8'"]), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' 24bit Interpolation'"]), 20)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'256'", '0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'512'", '0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'768'", '0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Adlib SoundCard'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Adlib SoundCard'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' and '"]), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' and write following text in your message:'"]), 42)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Arpeggio       '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' at'", '0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Auto TonePorta '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'BackSpace'"]), 9)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' base port '", '0']), 12)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'BMOD2STM'"]), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'CD81'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'CH'"]), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Channels      :'"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'CHN'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Config file not found. Run ISETUP first'", '0Dh', '0Ah', "'$'"]), 42)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Copyright (c) 1994,1995 by Stefan Danes and Ramon van Gorkom'", '0']), 61)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Could not find the Gravis UltraSound at the specified port addres'"]), 65)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Could not find the ULTRASND environment string'", '0Dh', '0Ah', '0']), 49)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Covox'", '0']), 6)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Covox'", '0']), 6)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Dh', '0Ah']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Ctrl Del'"]), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Dh', "'Current Soundcard settings:'", '0Dh', '0Ah']), 30)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Current Track :'"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Fh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Cursor '", '1Bh', "' '"]), 9)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Cursor '"]), 7)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Eh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Dec/Inc amplify'"]), 17)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Eh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Dec/Inc volume'"]), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Del'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Delete all files which are marked to delete'"]), 43)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Delete marked files? [Y/N]'", '0']), 27)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Deleting file: '"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Device not initialised!'", '0']), 24)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Eh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' Disable BPM on/off'"]), 19)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["', DMA '"]), 6)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Eh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'DOS Shell (Type EXIT to return)'"]), 31)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  DOS Shell (Type EXIT to return)'"]), 33)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["', DRAM-DMA '"]), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'E_Command      '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'E.G.'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'End'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  End pattern'"]), 13)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'End'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Enter'"]), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Error: Could not find IRQ/DMA!'", '0Dh', '0Ah', '0']), 33)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Error: Could not find IRQ!'", '0Dh', '0Ah', '0']), 29)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Error: Could not find DMA!'", '0Dh', '0Ah', '0']), 29)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Error: Soundcard not found!'", '0Dh', '0Ah', "'$'", '0']), 31)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'ESC'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'EXIT'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-1'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Fh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-10'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-10'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Fh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-11'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-11'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Fh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-12'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-12'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-2'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-3'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-4'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-5'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-8'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-8'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' [F-9]              '", '0']), 21)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' [F-9]'", '0']), 7)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Fh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-9'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-9'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'F-9'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FAR■'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FAR Fine Tempo '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FAR Tempo      '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Fast(er) forward'"]), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Fast(er) rewind'"]), 17)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  FastFourier Frequency Analysis'"]), 32)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FidoNet  : '"]), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'File'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'File Selector Help'"]), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Filename      : '"]), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FileName.Ext'"]), 12)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Fine Panning   '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Fine Port+VolSl'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Fine Porta Down'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Fine Porta Up  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Fine Tone Porta'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Fine Vibr+VolSl'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Fine Vibrato   '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Fine Vol Slide '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FineSlide Down '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FineSlide Up   '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FineVolume Down'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FineVolume Up  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FLT4'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'FLT8'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'General MIDI'", '0']), 13)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'General MIDI'", '0']), 13)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Glissando Ctrl '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Graphical scopes, one for each channel'"]), 40)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Gravis MAX Codec'", '0']), 17)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Gravis UltraSound'", '0']), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Gravis UltraSound'", '0']), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Fh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Gray - +'"]), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'GSFT'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Guess...'"]), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'h, GF1-IRQ '"]), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'h, IRQ '"]), 7)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Hit backspace to return to playmode, F-1 for help, QuickRead='"]), 61)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Home'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Hope you liked using the '"]), 25)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'if'"]), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'If you have bug-reports, suggestions or comments send a message t'"]), 65)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' Ignore BPM changes'"]), 19)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Inertia Mailinglists'"]), 20)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Inertia Module: '", '0']), 17)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Inertia Module: '", '0']), 17)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Inertia Module: '"]), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Inertia Player'"]), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Inertia Player V1.22 written by Stefan Danes and Ramon van Gorkom'"]), 65)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Inertia Player V1.22 Assembly '", '27h', "'94 CD Edition by Sound Solution'"]), 62)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Inertia Player'", '0']), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Inertia Sample: '"]), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Internet : '"]), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Invert Loop    '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'   JanFebMarAprMayJunJulAugSepOctNovDec'"]), 39)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'JN'"]), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Jump To Loop   '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'KB'", '0']), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'kHz'", '0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'List file not found.'", '0Dh', '0Ah', "'$'"]), 23)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'listserver@oliver.sun.ac.za'"]), 27)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Loading module'", '0']), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Eh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' Loop Module when done'"]), 22)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' Loop module'"]), 12)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Loop pattern'"]), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'M&K!'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'M!K!'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'M.K.'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Main Volume   :'"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Mark file to delete'"]), 19)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'<Marked to Delete>    '", '0']), 23)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'MAS_UTrack_V'"]), 12)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["', mixed at '", '0']), 12)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Module is corrupt!'", '0']), 19)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Module load error.'", '0Dh', '0Ah', "'$'"]), 21)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Module not found.'", '0Dh', '0Ah', "'$'"]), 20)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Module Type   : '"]), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'MTM'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'<Mute>                '", '0']), 23)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Mute channel'"]), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'name'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Not enough DRAM on UltraSound'", '0Dh', '0Ah', '0']), 32)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Not enough DRAM on your UltraSound to load all samples!'", '0']), 56)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Not enough memory available to load all samples!'", '0']), 49)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Not enough memory.'", '0Dh', '0Ah', "'$'"]), 21)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Not enough Memory available'", '0Dh', '0Ah', '0']), 30)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Note Cut       '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Note Delay     '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'(NTSC)'", '0']), 7)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'OCTA'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'(PAL) '", '0']), 7)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Pattern Break  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Pattern Delay  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Pause'"]), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'PC Honker'", '0']), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'PC Honker'", '0']), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'PgDn'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'PgUp'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Player: '"]), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' Playing in Stereo, Free:'"]), 25)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'PlayPausLoop'"]), 12)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Port + VolSlide'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Portamento Down'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Portamento Up  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Position Jump  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Press '"]), 6)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Press any key to return to the fileselector'", '0']), 44)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'                 Press F-1 for help, QuickRead='"]), 47)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Pro Audio Spectrum 16'", '0']), 22)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Pro Audio Spectrum 16'", '0']), 22)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Eh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  ProTracker 1.0 compatibility on/off'"]), 37)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' ProTracker 1.0'"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'PSM■'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Quit IPLAY'"]), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Realtime VU meters'"]), 20)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Retrig+Volume  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Retrigger Note '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Return to playmode [Only if the music is playing]'"]), 49)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'# SampleName   '"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Samples Used  :'"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'!Scream!'"]), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'SCRM'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'ScrollLock'"]), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'sdanes@marvels.hacktic.nl'"]), 25)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Send email to '"]), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Set Amplify    '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Set Filter     '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Set FineTune   '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Set Loop Point '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Set Panning    '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Set Sample Ofs '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Set Speed      '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Set Speed/BPM  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Set STM Speed  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Shell: 13/02/95 21:15:58'"]), 24)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Shelling to Operating System...'"]), 31)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'~   Size Vol Mode  C-2 Tune LoopPos LoopEnd'", '0']), 44)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'So you wanted some help?'"]), 24)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Some functions of the UltraSound do not work!'", '0Dh', '0Ah']), 47)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Sound Blaster'", '0']), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Sound Blaster 16/16ASP'", '0']), 23)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Sound Blaster 16/16ASP'", '0']), 23)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Sound Blaster Pro'", '0']), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Sound Blaster Pro'", '0']), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Sound Blaster'", '0']), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Speed'"]), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Stereo-On-1'", '0']), 12)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Stereo-On-1'", '0']), 12)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'subscribe inertia-list YourRealName'"]), 35)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'subscribe inertia-talk YourRealName'", '0']), 36)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Tab'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Tab'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'TDZ'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'the '"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'This help screen, but I guess you already found it...'"]), 53)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'This program requires the soundcards device driver.'", '0Dh', '0Ah', '0']), 54)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'To connect to Binary Inertia releases: '"]), 39)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'To connect to Discussion Mailing list: '"]), 39)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' to move the highlighted bar'"]), 28)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' to play the module or select the drive/directory'"]), 49)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' to return to '"]), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' to subscribe to one or both of'"]), 31)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Eh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' Toggle 24bit Interpolation'"]), 27)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  Toggle PAL/NTSC'", '0']), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Toggle QuickReading of module name'"]), 34)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Tone Portamento'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Track Position:'"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Tremolo        '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Tremolo Control'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Tremor         '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Triller        '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Type '"]), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'\x7f Unused'"]), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Use '"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Vibr + VolSlide'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Vibrato        '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Vibrato Control'", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  View sample names (twice for more)'"]), 36)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Volume Amplify:'"]), 15)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Volume Change  '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Volume Sliding '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' which is written in '"]), 21)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Windows Sound System'", '0']), 21)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Windows Sound System'", '0']), 21)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'xPress F-4 for more'"]), 19)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'.Ext'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'.M.K'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'.MOD.NST.669.STM.S3M.MTM.PSM.WOW.INR.FAR.ULT.OKT.OCT'", '0', '0', '0', '0']), 56)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['100']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=[['offset', '_mysprintf_0_nop']]), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['152h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['8']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0Ah']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '9', '12h', '1Bh', '24h', '2Dh', '36h', '40h', '40h', '4Ah', '53h', '5Ch', '65h', '6Eh']), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['78h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['20h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['20h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['62h','dup','(',['0'],')']), 98)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0FFh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['14h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['4Bh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['20h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['10h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['20h dup( ?)']), 32)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['21h dup( ?)']), 33)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['4']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['?']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['218Bh']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0FFh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['10524E49h']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['?']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Module not found'", '0Dh', '0Ah', '0']), 19)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['3F8h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'      ██████╔╗╚╝═║┌┐└┘─│╓╖╙╜─║╒╕╘╛═│'", '0']), 37)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['22050']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2Ch']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['1']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['3C6h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0FFh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['20202020h']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'    '"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Ah dup(0)']), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Searching directory for modules  '", '0']), 34)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Dh', '0Ah', "'$'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['?']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['42h dup(0)']), 66)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'  C-C#D-D#E-F-F#G-G#A-A#B-'"]), 26)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['40h', '40h', '40h', '40h', '40h', '40h', '40h', '40h', '40h', '40h', '3Fh', '3Fh', '3Fh']), 13)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'IPLAY.CFG'", '0']), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'─\\\\|/─\\\\|/'"]), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0FFFFh']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['3']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['offset _aGravisUltrasoun']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['140', '50', '25', '15', '10', '7', '6', '4', '3', '3', '2', '2', '2', '2', '1', '1']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0FFh', '80h', '40h', '2Ah', '20h', '19h', '15h', '12h', '10h', '0Eh', '0Ch', '0Bh', '0Ah']), 13)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['8363', '8422', '8482', '8543', '8604', '8667', '8730', '8794', '7901', '7954', '8007']), 22)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['8000h', '9000h', '0A000h', '0A952h', '0B000h', '0B521h', '0B952h', '0BCDEh']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['8000h', '9800h', '0A000h', '0A800h', '0B000h', '0B400h', '0B800h', '0BC00h']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['8000h', '9800h', '0A000h', '0A800h', '0B000h', '0B400h', '0B800h', '0BC00h']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh', '1Eh']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['1712', '1616', '1524', '1440', '1356', '1280', '1208', '1140', '1076', '1016', '960', '906', '856', '808', '762', '720', '678', '640', '604', '570', '538', '508', '480', '453']), 48)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '18h', '31h', '4Ah', '61h', '78h', '8Dh', '0A1h', '0B4h', '0C5h', '0D4h', '0E0h']), 12)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '15h', '20h', '29h', '30h', '37h', '3Dh', '44h', '49h', '4Fh', '54h', '59h', '5Eh']), 13)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '4', '8', '0Ch', '10h', '14h', '18h', '1Ch', '20h', '24h', '28h', '2Ch', '30h', '34h']), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '4', '8', '0Ch', '10h', '14h', '18h', '1Ch', '20h', '24h', '28h', '2Ch', '30h', '34h']), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0', '65536', '46340', '25079', '12785', '6423', '3215', '1608', '804', '402']), 40)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['-131072', '-65536', '-19196', '-4989', '-1260', '-316', '-79', '-20', '-5']), 36)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '0', '0']), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['100h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['536h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['22Fh']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['22Eh']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['1000h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['1234h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['378h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['37Ah']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['1234h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['1234h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['1000h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['1234h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['3F2h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['49Eh']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['2020h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['2020h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['6B00h', '6500h', '5F40h', '5A00h', '54C0h', '5000h', '4B80h', '4740h', '4340h']), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['6B00h', '6500h', '5F40h', '5A00h', '54C0h', '5000h', '4B80h', '4740h', '4340h']), 18)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['4']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['?']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['5513']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['1', '19D7h', '0Fh', '1F40h', '0', '2580h', '0Eh', '2B11h', '3', '3E80h', '2', '49D4h']), 24)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0']), 34)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0ffh', '0dfh', '0h']), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '18h', '0Bh', '0Dh', '0Ah']), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '0', '1', '3', '0', '2', '0', '4', '0', '0', '0', '5', '6', '0', '0', '7']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '1', '0', '2', '0', '3', '4', '5']), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'                              '"]), 30)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'               '", '0']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'                      '", '0']), 23)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'                                '"]), 32)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'                                '", '0Dh', '0Ah', '1Ah']), 35)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['2']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['4']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['3']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['4']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Ah']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['20h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['20h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2Ch']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['80h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['8Ah']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' '"]), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["' /?  This help screen'", '0Dh', '0Ah']), 23)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["','"]), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'- +'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["':'"]), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'ABC'", '0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['\'Close this DOS session first with the "EXIT" command.\'', '0Dh', '0Ah']), 55)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'OKOKOKOK'"]), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'OKOKOKOK'", '10', '13']), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Try changing the AT-BUS Clock in the CMOS Setup.'", '0Dh', '0Ah', '0']), 51)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Usage: IPLAY [Switches] [FileName.Ext|@FileList.Ext]'", '0Dh', '0Ah']), 54)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'[ ]'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'[ ]'", '0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'ed again.'", '0Dh', '0Ah']), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'h'"]), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'o:'"]), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'s'"]), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'s'", '0Dh', '0Ah', '0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'─asdkweorjwoerj3434'", '13', '10', '92']), 22)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0', '2Ah', '2Ah']), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0A0h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0A0h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0A0h', '0A4h', '0A8h', '0ACh', '0B0h', '0B4h', '0B8h', '0BCh', '0C0h', '0C4h', '0C8h']), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0A1h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0A1h', '0A5h', '0AAh', '0AEh', '0B2h', '0B6h', '0BAh', '0BEh', '0C2h', '0C6h', '0CAh']), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0AAh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Ah']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Ah', "'$'"]), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Ah', '0Bh', '1Bh']), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0B8h', '0BBh', '0BEh', '0C1h', '0C3h', '0C6h', '0C9h', '0CCh', '0CFh', '0D1h', '0D4h']), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0C5h', '0B4h', '0A1h', '8Dh', '78h', '61h', '4Ah', '31h', '18h']), 9)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Dh', '0Ah']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0Dh', '0Ah', "'$'"]), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['1']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['1', '1', '1', '1', '1']), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['1', '2', '3', '4']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['10h', '11h', '2Ah']), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['12']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['141']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['7Fh']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['8', '8', '8', '7', '7', '7', '7', '6', '6', '6', '6', '6', '6', '5', '5', '5']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['80h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['9', '9', '8']), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['-2', '-1', '-1', '-1', '-1', '0']), 24)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['201', '100', '50', '25', '12']), 20)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['111', '1']), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['unk_24453']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'ab''cd'", '"e"']), 7)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0']), 32)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['5', '5622h', '7', '6B25h', '4', '7D00h', '6', '8133h', '0Dh', '93A8h', '9', '0AC44h']), 24)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0A06h']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0BE0h', '0B40h', '0AA0h', '0A00h', '970h', '8F0h', '870h', '7F0h', '780h', '710h']), 20)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0Bh', '0BB80h', '0Ch']), 6)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['32Ah']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['5']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['4']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['6']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'file1.txt'", '0']), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['12345']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['-1']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['1']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['offset label2']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['000', '000', '000', '000', '000', '021', '000', '000', '042', '000', '000', '063', '009', '000', '000', '009']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'NOT enought memory for VGA display, controls work for network games'", '13', '10', "'$'"]), 70)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'probleme dans allocation de descriptor..'", '13', '10', "'$'"]), 43)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'probleme dans dans definition de la taille du segment'", '13', '10', "'$'"]), 56)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'abcde'"]), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'abcde'"]), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'cdeab'"]), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['0']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14']), 14)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['20h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['0']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['49h']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['?']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['4 dup (5)']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['10 dup (?)']), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['1', '2', '3']), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['5 dup (0)']), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['5*5 dup (0', 'testEqu*2', '2*2', '3)']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['131']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'abcd'"]), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['9', '8', '7', '1']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['5*5 dup (0', 'testEqu*2', '2*2', '3)']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['000', '009', '000', '000', '009', '021', '000', '009', '042', '000', '009', '063', '009', '009', '000', '009']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'00000000'", '0Dh', '0Ah', "'$'"]), 11)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'Hello World From Protected Mode!'", '10', '13', "'$'"]), 35)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'OKOKOKOK'", '10', '13']), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'OKOKOKOK'"]), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'ab''cd'"]), 6)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=["'file.txt'", '0']), 9)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['1']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['10 dup (?)']), 10)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['100 dup (1)']), 100)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['12']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['131']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['141']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2', '5', '6']), 3)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['2']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['4 dup (5)']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['4']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['5 dup (0)']), 5)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['5*5 dup (0', 'testEqu*2', '2*2', '3)']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=1, data=['6']), 1)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['11']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['2', '5', '0']), 6)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['2']), 2)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['223', '22']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=2, data=['4', '6', '9']), 6)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['0']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['10 dup (?)']), 40)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['11', '-11', '2', '4']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['11', '-11', '2', '4000000']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['111', '1']), 8)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['3']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['34']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['9', '8', '7', '1']), 16)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['offset var5']), 4)
        self.assertEqual(parser_instance.calculate_data_binary_size(width=4, data=['test2']), 4)
        #self.assertEqual(parser_instance.convert_data_to_blob(width=1,data=[u'2*2 dup (0,testEqu*2,2*2,3)']),[0, 0, 0, 0]))


    @patch.object(logging, 'debug')
    @patch.object(logging, 'warning')
    def test_convert_data_to_c(self, mock_warning, mock_debug):
        mock_warning.return_value = None
        mock_debug.return_value = None
        parser_instance = Parser([])

        #self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'offset var5'],label=''),(['', 'offset(_data,var5)', ', // dummy1\n'], ['dw dummy1', ';\n'], 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'11', u'-11', u'2', u'4'],label=u'var3'),(u'{11,4294967285,2,4}, // var3\n', u'dd var3[4];\n', 4))

        parser_instance = Parser([])
        #self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'offset var5'],label=''),(u'0, // dummy1\n', u'dw dummy1;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'34'],label=u'var3'),(u'34, // var3\n', u'dd var3;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=2,data=[u'2'],label=u'var2'),(u'2, // var2\n', u'dw var2;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=["'*'", '10', '11', '3 * 15 DUP(0)'],label=u'var3'),(u'"*\\n\\x0b", // var3\n', u'char var3[4];\n', 3))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u"'OKOKOKOK'", u'10', u'13'],label=''),(u"{'O','K','O','K','O','K','O','K','\\n','\\r'}, // dummy1\n", u'char dummy1[10];\n', 2))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'111', u'1'],label=''),(u'{111,1}, // dummy1\n', u'dd dummy1[2];\n', 2))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'3'],label=u'var3'),(u'3, // var3\n', u'dd var3;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[[u'100', 'dup', '(',['1'],')']],label=u'var4'),(u'{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // var4\n', u'db var4[100];\n', 100))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u"'00000000'", u'0Dh', u'0Ah', u"'$'"],label=u'ASCII'),(u"{'0','0','0','0','0','0','0','0','\\r','\\n','$'}, // ASCII\n", u'char ASCII[11];\n', 2))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'1'],label=u'var1'),(u'1, // var1\n', u'db var1;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u"'Hello World From Protected Mode!'", u'10', u'13', u"'$'"],label=u'_msg'),(u"{'H','e','l','l','o',' ','W','o','r','l','d',' ','F','r','o','m',' ','P','r','o','t','e','c','t','e','d',' ','M','o','d','e','!','\\n','\\r','$'}, // _msg\n", u'char _msg[35];\n', 2))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=2,data=[u'11'],label=u'var2'),(u'11, // var2\n', u'dw var2;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u"'OKOKOKOK'"],label=''),(u"{'O','K','O','K','O','K','O','K'}, // dummy1\n", u'char dummy1[8];\n', 0))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u"'file.txt'", u'0'],label=u'fileName'),(u'"file.txt", // fileName\n', u'char fileName[9];\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=2,data=[u'223', u'22'],label=''),(u'{223,22}, // dummy1\n', u'dw dummy1[2];\n', 2))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'test2'],label=u'var3'),(u'0, // var3\n', u'dd var3;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'12'],label=''),(u'12, // dummy1\n', u'db dummy1;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'2'],label=u'var1'),(u'2, // var1\n', u'db var1;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'6'],label=u'var1'),(u'6, // var1\n', u'db var1;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=2,data=[u'2', u'5', u'0'],label=u'var5'),(u'{2,5,0}, // var5\n', u'dw var5[3];\n', 3))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'4'],label=u'enddata'),(u'4, // enddata\n', u'db enddata;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'11', u'-11', u'2', u'4000000'],label=u'var3'),(u'{11,4294967285,2,4000000}, // var3\n', u'dd var3[4];\n', 4))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'4 dup (5)'],label=''),(u'{5,5,5,5}, // dummy1\n', u'db dummy1[4];\n', 4))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'5*5 dup (0', u'testEqu*2', u'2*2', u'3)'],label=u'var3'),(u'{0,0,4,0}, // var3\n', u'db var3[4];\n', 4))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=["'abcde\x00\x00'"],label=u'var5'),("{'a','b','c','d','e','\\0','\\0'}, // var5\n", 'char var5[7];\n', 0))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'9', u'8', u'7', u'1'],label=u'var6'),(u'{9,8,7,1}, // var6\n', u'dd var6[4];\n', 4))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'4 dup (5)'],label=u'var'),(u'{5,5,5,5}, // var\n', u'db var[4];\n', 4))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'131'],label=u'var4'),(u'131, // var4\n', u'db var4;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'5 dup (0)'],label=u'var2'),(u'{0,0,0,0,0}, // var2\n', u'db var2[5];\n', 5))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'4'],label=u'beginningdata'),(u'4, // beginningdata\n', u'db beginningdata;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'10 dup (?)'],label=u'var5'),(u'{0,0,0,0,0,0,0,0,0,0}, // var5\n', u'dd var5[10];\n', 10))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=4,data=[u'0'],label=u'load_handle'),(u'0, // load_handle\n', u'dd load_handle;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'141'],label=''),(u'141, // dummy1\n', u'db dummy1;\n', 1))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=["'.MOD.'", '0', '0', '0', '0'],label='_a_mod_nst_669_s'),('".MOD.\\0\\0\\0", // _a_mod_nst_669_s\n', u'char _a_mod_nst_669_s[9];\n', 4))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=2,data=[u'4', u'6', u'9'],label=u'var2'),(u'{4,6,9}, // var2\n', u'dw var2[3];\n', 3))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'2', u'5', u'6'],label=u'var1'),(u'{2,5,6}, // var1\n', u'db var1[3];\n', 3))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'10 dup (?)'],label=u'var0'),(u'{0,0,0,0,0,0,0,0,0,0}, // var0\n', u'db var0[10];\n', 10))

        parser_instance = Parser([])
        self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u"'ab''cd'"],label=u'doublequote'),(u"{'a','b','\\'','\\'','c','d'}, // doublequote\n", u'char doublequote[6];\n', 0))

        #parser_instance = Parser([])
        #self.assertEqual(parser_instance.convert_data_to_c(width=1,data=[u'2*2 dup (0,testEqu*2,2*2,3)']),[0, 0, 0, 0])
    '''

    @patch.object(logging, 'debug')
    @patch.object(logging, 'info')
    # @patch.object(parser, 'get_global')
    def test_convert_data(self, mock_info, mock_debug):
        # mock_get_global.return_value = var()
        mock_info.return_value = None
        mock_debug.return_value = None
        parser_instance = Parser([])

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=256, v=u'2*2')

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=256, v=u'3)')

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=256, v=u'5*5 dup (0')

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=256, v=u'testEqu*2')

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=4294967296, v=u'test2')

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=65536, v=u'2*2')

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=65536, v=u'3)')

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=65536, v=u'5*5 dup (0')

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=65536, v=u'test2')

        with self.assertRaises(KeyError):
            parser_instance.get_global_value(base=65536, v=u'testEqu*2')

        # self.assertEqual(parser_instance.convert_data(base=4294967296,v=u'var5'),u'offset(_data,var5)')

    @patch.object(logging, 'debug')
    def test_fix_dollar(self, mock_debug):
        mock_debug.return_value = None
        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='3'), '3')

        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='1'), '1')

        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='-13'), '-13')

        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='13'), '13')

        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='4'), '4')

        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='var1'), 'var1')

        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='1'), '1')

        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='2'), '2')

        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='(00+38*3)*320+1/2+33*(3-1)'), '(00+38*3)*320+1/2+33*(3-1)')

        parser_instance = Parser([])
        self.assertEqual(parser_instance.fix_dollar(v='1500 ; 8*2*3 ;+1 +19*13*2*4'), '1500 ; 8*2*3 ;+1 +19*13*2*4')

    def test_parse_int(self):
        parser_instance = Parser([])

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u"'Z' - 'A' +1")

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u"'a'")

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u"'c'")

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u"'d'")

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u"'tseT'")

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'3)')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'5*5 dup (0')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'B')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'CC')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'DDD')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'OFFSET ASCiI')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'OFFSET AsCii')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[doublequote+4]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[edi+1]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[edi]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[load_handle]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var+3]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var+4]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var-1]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var0+5]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var1+1]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var1]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var2+2]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var2-1]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var2]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var3+3*4]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var3+ebp]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var3]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'[var]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'_data')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'al')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'beginningdata')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'bl')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'buffer')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'bx')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'byte ptr [edi+1]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'byte ptr [edi+7]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'byte ptr dl')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'cl')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'cx')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'dl')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'ds')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'ds:[edi]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'dword ptr buffer')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'dx')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'eax')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'ebp')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'ebx')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'ecx')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'edi')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'edx')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'enddata')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'es')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'esi')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'fileName')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'offset _msg')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'offset var1')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'offset var2')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'offset var5')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'teST2')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'test2')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'testEqu*2')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'var1')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'var1[1]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'var1[bx+si]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'var1[bx]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'var2')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'var3')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'var3+3*4')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'var3+ebp')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'word ptr [var5+2]')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'word ptr var5')

        with self.assertRaises(ValueError):
            parser_instance.parse_int(v=u'ah')

        self.assertEqual(parser_instance.parse_int(v=u'14*320'), 4480)
        self.assertEqual(parser_instance.parse_int(v=u'2*2'), 4)
        self.assertEqual(parser_instance.parse_int(v=u'3*4'), 12)
        self.assertEqual(parser_instance.parse_int(v=u'-1-(-2+3)'), -2)
        self.assertEqual(parser_instance.parse_int(v=u'-1'), -1)
        self.assertEqual(parser_instance.parse_int(v=u'-11'), -11)
        self.assertEqual(parser_instance.parse_int(v=u'-2'), -2)
        self.assertEqual(parser_instance.parse_int(v=u'0'), 0)
        self.assertEqual(parser_instance.parse_int(v=u'00h'), 0)
        self.assertEqual(parser_instance.parse_int(v=u'03dh'), 61)
        self.assertEqual(parser_instance.parse_int(v=u'03eh'), 62)
        self.assertEqual(parser_instance.parse_int(v=u'03fh'), 63)
        self.assertEqual(parser_instance.parse_int(v=u'042h'), 66)
        self.assertEqual(parser_instance.parse_int(v=u'0Ah'), 10)
        self.assertEqual(parser_instance.parse_int(v=u'0Dh'), 13)
        self.assertEqual(parser_instance.parse_int(v=u'0Fh'), 15)
        self.assertEqual(parser_instance.parse_int(v=u'0ffffff00h'), 4294967040)
        self.assertEqual(parser_instance.parse_int(v=u'1'), 1)
        self.assertEqual(parser_instance.parse_int(v=u'10'), 10)
        self.assertEqual(parser_instance.parse_int(v=u'100'), 100)
        self.assertEqual(parser_instance.parse_int(v=u'1000h'), 4096)
        self.assertEqual(parser_instance.parse_int(v=u'11'), 11)
        self.assertEqual(parser_instance.parse_int(v=u'111'), 111)
        self.assertEqual(parser_instance.parse_int(v=u'12'), 12)
        self.assertEqual(parser_instance.parse_int(v=u'13'), 13)
        self.assertEqual(parser_instance.parse_int(v=u'131'), 131)
        self.assertEqual(parser_instance.parse_int(v=u'16'), 16)
        self.assertEqual(parser_instance.parse_int(v=u'2'), 2)
        self.assertEqual(parser_instance.parse_int(v=u'21h'), 33)
        self.assertEqual(parser_instance.parse_int(v=u'22'), 22)
        self.assertEqual(parser_instance.parse_int(v=u'223'), 223)
        self.assertEqual(parser_instance.parse_int(v=u'25'), 25)
        self.assertEqual(parser_instance.parse_int(v=u'3'), 3)
        self.assertEqual(parser_instance.parse_int(v=u'30h'), 48)
        self.assertEqual(parser_instance.parse_int(v=u'34'), 34)
        self.assertEqual(parser_instance.parse_int(v=u'35'), 35)
        self.assertEqual(parser_instance.parse_int(v=u'37'), 37)
        self.assertEqual(parser_instance.parse_int(v=u'39h'), 57)
        self.assertEqual(parser_instance.parse_int(v=u'4'), 4)
        self.assertEqual(parser_instance.parse_int(v=u'4000000'), 4000000)
        self.assertEqual(parser_instance.parse_int(v=u'4ch'), 76)
        self.assertEqual(parser_instance.parse_int(v=u'5'), 5)
        self.assertEqual(parser_instance.parse_int(v=u'50'), 50)
        self.assertEqual(parser_instance.parse_int(v=u'6'), 6)
        self.assertEqual(parser_instance.parse_int(v=u'64000'), 64000)
        self.assertEqual(parser_instance.parse_int(v=u'7'), 7)
        self.assertEqual(parser_instance.parse_int(v=u'0h'), 0)
        self.assertEqual(parser_instance.parse_int(v=u'0b'), 0)

    def test_calculate_data_size(self):
        #parser_instance = Parser([])
        self.assertEqual(calculate_type_size(type='db'), 1)
        self.assertEqual(calculate_type_size(type='dd'), 4)
        self.assertEqual(calculate_type_size(type='dq'), 8)
        self.assertEqual(calculate_type_size(type='dw'), 2)
        self.assertEqual(calculate_type_size(type='dt'), 10)


    @patch.object(logging, 'debug')
    @patch.object(logging, 'info')
    # @patch.object(parser, 'get_global')
    def test_equ(self, mock_info, mock_debug):
        # mock_get_global.return_value = var()
        mock_info.return_value = None
        mock_debug.return_value = None
        parser_instance = Parser([])

        cpp_instance = cpp.Cpp(parser_instance)
        proc_instance = Proc('mainproc', False)
        cpp_instance.proc = proc_instance
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'aaaa = 1')), '#undef aaaa\n#define aaaa 1\n')

        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'B = 1')), '#undef B\n#define B 1\n')
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'DDD = var1 ; actually it is address of var1')), '#undef DDD\n#define DDD var1\n')

        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'left equ 0')), '#define left 0\n')
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'col equ 40')), '#define col 40\n')
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'right equ left+col')), '#define right left + col\n')
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'scale_mod equ -19*32*4; ')), '#define scale_mod -19 * 32 * 4\n')
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'tempo equ 1193182/256/targetFPS')), '#define tempo 1193182 / 256 / targetFPS\n')
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'sierp_color equ 2Ah')), '#define sierp_color 0x2A\n')
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'tilt_plate_pattern equ 4+8+16')), '#define tilt_plate_pattern 4 +8 +16\n')

        # wrong
        # TODO
        # self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'l equ byte ptr aaa')), '#define l byte aaa\n')
        self.assertEqual(proc_instance.generate_c_cmd(cpp_instance, parser_instance.action_data(u'res = edx ; int')), '#undef res\n#define res edx\n')



if __name__ == "__main__":
    unittest.main()
