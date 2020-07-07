.386p

_DATA   segment use32 dword public 'DATA' ;IGNORE
var1 db 2,5,6
var2 dw 4,6,9
var3 dd 11,-11,2,4000000
_DATA   ends ;IGNORE

_text segment byte public 'CODE' use16
assume ds:_DATA
_start:
mov al,0
mov ah,4ch                    ; AH=4Ch - Exit To DOS
int 21h                       ; DOS INT 21h                     ; DOS INT 21h

cmp var1[1],2
cmp var1[bx],2
cmp var1[bx+si],2
label1:
		mov	es:[di+8], eax

myoffs		dw offset label2
		dw offset label3
		dw offset _myout
		jmp	cs:[myoffs+bx]

		jmp	short $+2
		mul cs:[_slider+di]
		movzx cx, byte ptr [bx+4]
		mov eax, [dword ptr _slider]
		mov	al, _slider[eax+eax*2] ; "	JanFebMarAprMayJunJulAugSepOctNovDec"
		lea	si, _slider[eax+eax*2] ; "	JanFebMarAprMayJunJulAugSepOctNovDec"
		jmp $+2
		mov ax, offset label1
		jmp label1
label2:
		div	_word_245D4
label3:
		mov si, (offset _slider+10h)

_text ends

; ---------------------------------------------------------------------------
; ===========================================================================
; Segment type:	Regular
seg003 segment byte public 'UNK' use16
		assume cs:seg003
		assume es:nothing, ss:nothing, ds:dseg,	fs:nothing, gs:nothing
_a_mod_nst_669_s	db '.MOD.NST.669.STM.S3M.MTM.PSM.WOW.INR.FAR.ULT.OKT.OCT',0,0,0,0
_slider		db 'Ä\|/Ä\|/'           ; DATA XREF: _modules_search+7Fr
		db 'Äasdkweorjwoerj3434',13,10,92

_myout		db 18C0h dup(?)		; DATA XREF: _mod_1024A+3o

_word_245D4	dw 0			; DATA XREF: _moduleread+81r
seg003 ends

; ===========================================================================

; Segment type:	Uninitialized
seg004 segment byte stack 'STACK' use16
		assume cs:seg004
		assume es:nothing, ss:nothing, ds:dseg,	fs:nothing, gs:nothing
_byte_34510	db 1000h dup(?)
seg004 ends


		end _start
 