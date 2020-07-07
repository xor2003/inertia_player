.386p

_DATA   segment use16 word public 'DATA' ;IGNORE
a db 0ffh,0dfh,0h
b db 2

_DATA   ends ;IGNORE

_TEXT   segment use16 word public 'CODE' ;IGNORE
assume  cs:_TEXT,ds:_DATA
start proc near
_start: ;IGNORE

mov ax,_DATA
mov ds,ax

mov ebp,32
mov ax,4
jmp [cs:table+ax]
jmp failure
mov ebp,30
@df@@@@8:

mov ebp,2
mov ax,6
call [cs:table+ax]
jmp next
mov ebp,31
@df@@@@9:
ret

next:

mov ebp,3
mov eax,-1
test eax,eax
js @df@@@@
@df@@@@:
jns failure

mov ebp,4
xor eax,eax
js failure

mov ebp,5
mov ax,-1
test ax,ax
jns failure

mov ebp,6
mov al,[a]
mov bl,[a+1]
cmp bl,al
ja failure


mov ebp,7
mov bl,192
cmp bl,192
jb failure

mov ebp,8
cmp bl,193
jnb failure

mov ebp,9
mov dx,-1
cmp dx,0
jns failure

mov ebp,10
mov dx,1
cmp dx,0
js failure


mov ebp,11
mov ecx,000ff00ffh
mov cx,1
or cx,cx
jz failure

mov ebp,12
xor cx,cx
jnz failure

mov ebp,13
lea esi,b
cmp byte ptr [esi],1
jb failure

mov ebp,14
cmp byte ptr [esi],4
ja failure

mov ebp,15
mov byte ptr [esi],-2
cmp byte ptr [esi],1
jb failure  ; // because unsigned comparaison

mov ebp,16
mov dx,-1
cmp dx,0
jg failure

mov ebp,17
mov dx,5
cmp dx,5
jg failure

mov ebp,18
mov eax,5
cmp eax,4
jg @df@@@@1
jmp failure
@df@@@@1:

mov ebp,19
mov eax,4
cmp eax,4
jge @df@@@@2
jmp failure
@df@@@@2:

mov ebp,20
mov eax,5
cmp eax,4
jge @df@@@@3
jmp failure
@df@@@@3:

mov ebp,21
mov dx,-1
cmp dx,0
jge failure

mov ebp,22
mov dx,0
cmp dx,-1
jl failure

mov ebp,23
mov dx,5
cmp dx,5
jl failure

mov ebp,24
mov eax,4
cmp eax,5
jl @df@@@@4
jmp failure
@df@@@@4:

mov ebp,25
mov eax,4
cmp eax,4
jle @df@@@@5
jmp failure
@df@@@@5:

mov ebp,26
mov eax,4
cmp eax,5
jle @df@@@@6
jmp failure
@df@@@@6:

mov ebp,27
mov dx,0
cmp dx,-1
jle failure

mov ebp,28
mov cx,-1
jcxz failure

mov ebp,29
mov cx,0
jcxz @df@@@@7
jmp failure
@df@@@@7:


good:
MOV al,0
JMP exitLabel
failure:
mov ax,bp
exitLabel:
mov ah,4ch                    ; AH=4Ch - Exit To DOS
int 21h


table   dw 0
	dw offset failure
  dw @df@@@@8
  dw @df@@@@9

start endp

_TEXT   ends ;IGNORE

stackseg   segment para stack 'STACK' ;IGNORE
db 1000h dup(?)
stackseg   ends ;IGNORE

end start ;IGNORE
