.386p

_DATA   segment use32 dword public 'DATA' ;IGNORE
var1 db 6
     db 12
     db 12
     db 12
_DATA   ends ;IGNORE

_TEXT   segment use32 dword public 'CODE' ;IGNORE
assume  cs:_TEXT,ds:_DATA
_start proc near
start:


;mov eax, E  TODO implement several passes
;cmp eax,1
;mov al,1
;jne failure
;E = 1

B = 2
mov eax, B
cmp eax,2
mov al,2
jne failure

mov eax, CC
cmp eax,4
mov al,3
jne failure
B = 1

call incebx

DDD = var1 ; actually it is address of var1
movzx eax, DDD
cmp eax,0
mov al,4
jne failure

MOV al,0
JMP exitLabel
failure::
;mov al,1
exitLabel:
mov ah,4ch                    ; AH=4Ch - Exit To DOS
int 21h
_start endp

incebx proc near

B = 3
mov eax, B
cmp eax,3
mov al,5
jne failure

CC equ 4

ret
inceBX endp


_TEXT   ends ;IGNORE

stackseg   segment para stack 'STACK' ;IGNORE
db 1000h dup(?) ;IGNORE
stackseg   ends ;IGNORE

end _start ;IGNORE
