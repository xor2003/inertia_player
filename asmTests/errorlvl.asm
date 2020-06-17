_TEXT   segment use16 word public 'CODE' ;IGNORE
assume  cs:_TEXT,ds:_TEXT,ss:_TEXT
org 100h
_start proc near
start: ;IGNORE
MOV     AH,4Dh
INT     21h
push	ax
MOV     DL,AL
ADD     DL,30h
MOV     AH,02
INT     21h
pop	ax
MOV     AH,4Ch
INT     21h
_start endp

_TEXT   ends ;IGNORE

end _start ;IGNORE
