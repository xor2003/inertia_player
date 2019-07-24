; printf1.asm   call printf by replacing real stack with emulated stack
; Assemble:	nasm -f coff -g -l iprintf.lst  iprintf.asm
; Link:		gcc -o printf1  printf1.o
; Run:		printf1
; Output:	a=5, eax=7

; Equivalent C code
; /* printf1.c  print an int and an expression */
; #include 
; int main()
; {
;   int a=5;
;   printf("a=%d, eax=%d\n", a, a+2);
;   return 0;
; }

; Declare some external functions
;
        extern	_printf		; the C function, to be called
                
        SECTION .text                   ; Code section.

        global _iiprintf		; the standard gcc entry point
;extern stackPointer

_iiprintf:				; the program label for the entry point
        push    ebp		; set up stack frame
        mov     ebp,esp

	mov     esp, [ebp+8]
	add	esp, 4
        call    _printf		; Call C function

	mov	eax,esp

        mov     esp, ebp	; takedown stack frame
        pop     ebp		; same as "leave" op

;	mov	eax,0		;  normal, no error, return value
	ret			; return
