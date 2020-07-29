;searchlight
;y0bi / wAMMA
;26.1.2007
.486
code segment use16
org 100h
    
    mov al,13h
    int 10h

    push 0x8000
    pop ds
    push 0x7000
    pop fs
    push 0x6000
    pop gs
    push 0a000h
    pop es
;------------------
	xor bx,bx
MakeTopoLoop:
	mov ax,bx
	shl ax,2
	and al,ah	
	cmp al,128+64+32
	setb al
	shl al,7
	mov byte ptr [fs:bx],al
;-------------
    mov ax,bx
    add ax,64
    cbw
    cwd
    mov ax,bx
	shl ax,1
	mov di,ax
    imul al
    sub ah,64
    xor ax,dx
    sar ax,6
    mov [gs:di],ax
	inc bx
	jnz MakeTopoLoop

;------------------
MainLoop:
	lodsw


	mov di,512
AngleLoop:
	mov bp,[gs:si]
	shl bp,6
    mov dx,si
	shl dx,7
	add dh,89
	mov ah,255
    mov cx,128
RadiusLoop:
    mov bx,bp
    mov bl,dh
	mov al,cl
	and al,ah
	cmp byte ptr [fs:bx],ch
	jne NoWall
	xor ax,ax
NoWall:	
	add [bx],al
	shr byte ptr [bx],1
    add dx,[gs:di+128]
    add bp,[gs:di]
  	loop RadiusLoop
   	dec di
    dec di
    jnz AngleLoop

;----------
    mov bp,320
LineLoop:
	mov dx,[gs:si]
	sal dx,5
	add dh,16

    mov bl,199
    xor cx,cx
ProjectLoop:
    inc cx
    push si
    push dx
	
	mov ax,si
	shr ax,1
    add al,cl
    mov ah,dh
    xchg si,ax
    
    mov al,[fs:si]
	cbw
    neg ax
    shl ax,5
    cwd
    idiv cx
    add ax,100
    cmp ax,199
    jg DontDraw
	cmp al,bl
   	jae DontDraw
    mov dl,bl
    xchg bx,ax
	sub dl,bl
	imul di,bx,320	    
	mov ax,si
	xor al,ah
	shr al,2
	add al,[si-3]
	sub al,64
	cbw
	not ah
	and al,ah
	shr al,3
	add al,16
PlotLoop:
    mov [es:di+bp],al
    not di
    mov [es:di+bp+65],al
    not di
    add di,320
    dec dl
    jnz PlotLoop
DontDraw:    

    pop dx 
    pop si

	add dx,bp
	add dx,bp
	sub dx,320
    
    cmp ch,1
    jb ProjectLoop
    dec bp
    jnz LineLoop
    
    in al,60h
    cmp al,1
    jne MainLoop
    ret
    
    
code ends