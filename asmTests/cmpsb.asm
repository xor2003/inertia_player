
_DATA   segment use16 word public 'DATA' ;IGNORE
var1 db 1,2,3
var2 db 1,2,4
_DATA   ends ;IGNORE

_TEXT   segment use16 word public 'CODE' ;IGNORE
assume  cs:_TEXT,ds:_DATA

_start proc near
start: ;IGNORE
mov ax,_DATA
mov ds,ax
mov es,ax

cld                             ; Clear The Direction Flag

mov si, offset var1
mov di, offset var2
mov cx,2
repe cmpsb
mov al,1
jne failure
mov al,2
jc failure

mov si, offset var1
mov di, offset var2
mov cx,3
repe cmpsb
mov al,3
je failure
mov al,4
jnc failure

mov di, offset var1
mov cx,6
mov al,1
repe scasb
mov al,5
je failure
mov al,6
jnc failure

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

end _start ;IGNORE
