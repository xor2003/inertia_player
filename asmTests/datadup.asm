.386p

dubsize EQU 13
testEqu EQU 1

_DATA   segment use32 dword public 'DATA' ;IGNORE
var0 db 10 dup (?)
var db 4 dup (5)
var2 db 5 dup (0)
var3 db 5*5 dup (0,testEqu*2,2*2,3)

_DATA   ends ;IGNORE

_TEXT   segment use32 dword public 'CODE' ;IGNORE
assume  cs:_TEXT,ds:_DATA

_start proc near
start: ;IGNORE
mov ds,_DATA

cmp [var],5
mov al,1
jne failure
cmp [var2],0
mov al,2
jne failure
cmp [var+3],5
mov al,3
jne failure
cmp [var+4],0
mov al,4
jne failure
cmp [var2-1],5
mov al,5
jne failure
cmp [var0+5],0
mov al,6
jne failure
cmp [var-1],0
mov al,7
jne failure

MOV al,0
JMP exitLabel
failure:
;mov al,1
exitLabel:
mov ah,4ch                    ; AH=4Ch - Exit To DOS
int 21h                       ; DOS INT 21h
_start endp

_TEXT   ends ;IGNORE

stackseg   segment para stack 'STACK' ;IGNORE
db 1000h dup(?)
stackseg   ends ;IGNORE

end start ;IGNORE
