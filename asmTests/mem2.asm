.386p

_DATA   segment use32 dword public 'DATA' ;IGNORE
a db 1
;padding db 10241024 dup(?)

b db 2
cc db 3
d db 4
e db 5
f db 6
pas_de_mem  db 'NOT enought memory for VGA display, controls work for network games',13,10,'$'
pbs1        db 'probleme dans allocation de descriptor..',13,10,'$'
pbs2        db 'probleme dans dans definition de la taille du segment',13,10,'$'
ASCII DB '00000000',0Dh,0Ah,'$' ; buffer for ASCII string

_DATA   ends ;IGNORE

_TEXT   segment use32 dword public 'CODE' ;IGNORE
assume  cs:_TEXT,ds:_DATA
_start proc near

start: ;IGNORE

taille_moire equ ((((2030080+64000*26)/4096)+1)*4096)-1

;--------------------- rÇserve de la mÇmoire pour mettre les donnÇes. ----
;2.29 - Function 0501h - Allocate Memory Block:
;In:  AX     = 0501h
;  BX:CX  = size of block in bytes (must be non-zero)
;Out: if successful:
;    carry flag clear
;    BX:CX  = linear address of allocated memory block
;    SI:DI  = memory block handle (used to resize and free block)
mov bx,1024*10/16
mov ah,48h
int 21h
jc failure
mov es,ax

mov bx,(1024*10/16)-1
mov ah,4Ah
int 21h
jc failure

mov byte ptr es:[0],55
cmp byte ptr es:[0],55
jne failure

mov bx,(1024*10/16)+5
mov ah,4Ah
int 21h
jc failure

inc byte ptr es:[0]
cmp byte ptr es:[0],56
jne failure

mov ah,49h
int 21h
jc failure



mov bx,5
mov ah,4Ah
int 21h
jc failure
push es

mov bx,10
mov ah,4Ah
int 21h
jc failure
mov ah,49h
int 21h
jc failure

pop es
mov ah,49h
int 21h
jc failure

MOV al,0
JMP exitLabel
failure:
mov al,1
exitLabel:
mov ah,4ch                    ; AH=4Ch - Exit To DOS
int 21h                       ; DOS INT 21h
_start endp

_TEXT   ends ;IGNORE

stackseg   segment para stack 'STACK' ;IGNORE
db 1000h dup(?)
stackseg   ends ;IGNORE

end _start ;IGNORE
