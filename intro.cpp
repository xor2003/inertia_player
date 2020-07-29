/* PLEASE DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE LOST! LOOK FOR README FOR DETAILS */

/* 
 *
 */

#include "intro.h"
#include <curses.h>
#include <SDL2/SDL.h>

//namespace intro {


int init(struct _STATE* _state)
{
X86_REGREF

_state->_indent=0;
logDebug=fopen("intro.log","w");
ecx=0;

initscr();
resize_term(25, 80);
 cbreak(); // put keys directly to program
    noecho(); // do not echo
    keypad(stdscr, TRUE); // provide keypad buttons

    if (!has_colors())
    {
        printw("Unable to use colors");
    }
        start_color();

        realtocurs();
        curs_set(0);

        refresh();

  log_debug("~~~ heap_size=%d para=%d heap_ofs=%d", HEAP_SIZE, (HEAP_SIZE >> 4), seg_offset(heap) );
  /* We expect ram_top as Kbytes, so convert to paragraphs */
  mcb_init(seg_offset(heap), (HEAP_SIZE >> 4) - seg_offset(heap) - 1, MCB_LAST);

  R(MOV(ss, seg_offset(stack)));
#if _BITS == 32
  esp = ((dd)(db*)&m.stack[STACK_SIZE - 4]);
#else
  esp=0;
  sp = STACK_SIZE - 4;
  es=0;
 *(dw*)(raddr(0,0x408)) = 0x378; //LPT
#endif

        return(0);
}

void mainproc(_offsets _i, struct _STATE* _state){
X86_REGREF
__disp=_i;
if (__disp==kbegin) goto begin;
else goto __dispatch_call;
begin:
	R(ORG(0x100));	// 6 org 100h
	R(MOV(al, 0x13));	// 8 mov al,13h
	R(_INT(0x10));	// 9 int 10h
	R(PUSH(0x8000));	// 11 push 0x8000
	R(POP(ds));	// 12 pop ds
	R(PUSH(0x7000));	// 13 push 0x7000
	R(POP(fs));	// 14 pop fs
	R(PUSH(0x6000));	// 15 push 0x6000
	R(POP(gs));	// 16 pop gs
	R(PUSH(0x0a000));	// 17 push 0a000h
	R(POP(es));	// 18 pop es
	bx = 0;AFFECT_ZF(0); AFFECT_SF(bx,0);	// 20 xor bx,bx
maketopoloop:
	R(MOV(ax, bx));	// 22 mov ax,bx
	R(SHL(ax, 2));	// 23 shl ax,2
	R(AND(al, ah));	// 24 and al,ah
	R(CMP(al, 128+64+32));	// 25 cmp al,128+64+32
	R(SETB(al));	// 26 setb al
	R(SHL(al, 7));	// 27 shl al,7
	R(MOV(*(raddr(fs,bx)), al));	// 28 mov byte ptr [fs:bx],al
	R(MOV(ax, bx));	// 30 mov ax,bx
	R(ADD(ax, 64));	// 31 add ax,64
	R(CBW);	// 32 cbw
	R(CWD);	// 33 cwd
	R(MOV(ax, bx));	// 34 mov ax,bx
	R(SHL(ax, 1));	// 35 shl ax,1
	R(MOV(di, ax));	// 36 mov di,ax
	R(IMUL1_1(al));	// 37 imul al
	R(SUB(ah, 64));	// 38 sub ah,64
	R(XOR(ax, dx));	// 39 xor ax,dx
	R(SAR(ax, 6));	// 40 sar ax,6
	R(MOV(*(dw*)(raddr(gs,di)), ax));	// 41 mov [gs:di],ax
	R(INC(bx));	// 42 inc bx
		R(JNZ(maketopoloop));	// 43 jnz MakeTopoLoop
mainloop:
LODSW;	// 47 lodsw
	R(MOV(di, 512));	// 50 mov di,512
angleloop:
	R(MOV(bp, *(dw*)(raddr(gs,si))));	// 52 mov bp,[gs:si]
	R(SHL(bp, 6));	// 53 shl bp,6
	R(MOV(dx, si));	// 54 mov dx,si
	R(SHL(dx, 7));	// 55 shl dx,7
	R(ADD(dh, 89));	// 56 add dh,89
	R(MOV(ah, 255));	// 57 mov ah,255
	R(MOV(cx, 128));	// 58 mov cx,128
radiusloop:
	R(MOV(bx, bp));	// 60 mov bx,bp
	R(MOV(bl, dh));	// 61 mov bl,dh
	R(MOV(al, cl));	// 62 mov al,cl
	R(AND(al, ah));	// 63 and al,ah
	R(CMP(*(raddr(fs,bx)), ch));	// 64 cmp byte ptr [fs:bx],ch
		R(JNZ(nowall));	// 65 jne NoWall
	ax = 0;AFFECT_ZF(0); AFFECT_SF(ax,0);	// 66 xor ax,ax
nowall:
	R(ADD(*(raddr(ds,bx)), al));	// 68 add [bx],al
	R(SHR(*(raddr(ds,bx)), 1));	// 69 shr byte ptr [bx],1
	R(ADD(dx, *(dw*)(raddr(gs,di+128))));	// 70 add dx,[gs:di+128]
	R(ADD(bp, *(dw*)(raddr(gs,di))));	// 71 add bp,[gs:di]
		R(LOOP(radiusloop));	// 72 loop RadiusLoop
	R(DEC(di));	// 73 dec di
	R(DEC(di));	// 74 dec di
		R(JNZ(angleloop));	// 75 jnz AngleLoop
	R(MOV(bp, 320));	// 78 mov bp,320
lineloop:
	R(MOV(dx, *(dw*)(raddr(gs,si))));	// 80 mov dx,[gs:si]
	R(SAL(dx, 5));	// 81 sal dx,5
	R(ADD(dh, 16));	// 82 add dh,16
	R(MOV(bl, 199));	// 84 mov bl,199
	cx = 0;AFFECT_ZF(0); AFFECT_SF(cx,0);	// 85 xor cx,cx
projectloop:
	R(INC(cx));	// 87 inc cx
	R(PUSH(si));	// 88 push si
	R(PUSH(dx));	// 89 push dx
	R(MOV(ax, si));	// 91 mov ax,si
	R(SHR(ax, 1));	// 92 shr ax,1
	R(ADD(al, cl));	// 93 add al,cl
	R(MOV(ah, dh));	// 94 mov ah,dh
	R(XCHG(si, ax));	// 95 xchg si,ax
	R(MOV(al, *(raddr(fs,si))));	// 97 mov al,[fs:si]
	R(CBW);	// 98 cbw
	R(NEG(ax));	// 99 neg ax
	R(SHL(ax, 5));	// 100 shl ax,5
	R(CWD);	// 101 cwd
	R(IDIV2(cx));	// 102 idiv cx
	R(ADD(ax, 100));	// 103 add ax,100
	R(CMP(ax, 199));	// 104 cmp ax,199
		R(JG(dontdraw));	// 105 jg DontDraw
	R(CMP(al, bl));	// 106 cmp al,bl
		R(JNC(dontdraw));	// 107 jae DontDraw
	R(MOV(dl, bl));	// 108 mov dl,bl
	R(XCHG(bx, ax));	// 109 xchg bx,ax
	R(SUB(dl, bl));	// 110 sub dl,bl
	R(IMUL3_2(di,bx,320));	// 111 imul di,bx,320
	R(MOV(ax, si));	// 112 mov ax,si
	R(XOR(al, ah));	// 113 xor al,ah
	R(SHR(al, 2));	// 114 shr al,2
	R(ADD(al, *(raddr(ds,si-3))));	// 115 add al,[si-3]
	R(SUB(al, 64));	// 116 sub al,64
	R(CBW);	// 117 cbw
	R(NOT(ah));	// 118 not ah
	R(AND(al, ah));	// 119 and al,ah
	R(SHR(al, 3));	// 120 shr al,3
	R(ADD(al, 16));	// 121 add al,16
plotloop:
//	R(MOV(*(raddr(es,di+bp)), al));	// 123 mov [es:di+bp],al

	  SDL_SetRenderDrawColor(renderer, vgaPalette[3*al+2], vgaPalette[3*al+1], vgaPalette[3*al], 255);
          SDL_RenderDrawPoint(renderer, (di+bp)%320, (di+bp)/320);
  	  SDL_RenderPresent(renderer);
	log_debug("plot(%d,%d) %d\n",(di+bp)%320,(di+bp)/320,al);

	R(NOT(di));	// 124 not di
//	R(MOV(*(raddr(es,di+bp+65)), al));	// 125 mov [es:di+bp+65],al
	  SDL_SetRenderDrawColor(renderer, vgaPalette[3*al+2], vgaPalette[3*al+1], vgaPalette[3*al], 255);
          SDL_RenderDrawPoint(renderer, (di+bp+65)%320, (di+bp+65)/320);
  	  SDL_RenderPresent(renderer);
	log_debug("plot(%d,%d) %d\n",(di+bp+65)%320,(di+bp+65)/320,al);

	R(NOT(di));	// 126 not di
	R(ADD(di, 320));	// 127 add di,320
	R(DEC(dl));	// 128 dec dl
		R(JNZ(plotloop));	// 129 jnz PlotLoop
dontdraw:
	R(POP(dx));	// 132 pop dx
	R(POP(si));	// 133 pop si
	R(ADD(dx, bp));	// 135 add dx,bp
	R(ADD(dx, bp));	// 136 add dx,bp
	R(SUB(dx, 320));	// 137 sub dx,320
	R(CMP(ch, 1));	// 139 cmp ch,1
		R(JC(projectloop));	// 140 jb ProjectLoop
	R(DEC(bp));	// 141 dec bp
		R(JNZ(lineloop));	// 142 jnz LineLoop
	R(IN(al, 0x60));	// 144 in al,60h
	R(CMP(al, 1));	// 145 cmp al,1
		R(JNZ(mainloop));	// 146 jne MainLoop
	R(RETN);	// 147 ret


return;
__dispatch_call:
switch (__disp) {
case kangleloop: 	goto angleloop;
case kdontdraw: 	goto dontdraw;
case klineloop: 	goto lineloop;
case kmainloop: 	goto mainloop;
case kmaketopoloop: 	goto maketopoloop;
case knowall: 	goto nowall;
case kplotloop: 	goto plotloop;
case kprojectloop: 	goto projectloop;
case kradiusloop: 	goto radiusloop;
default: log_error("Jump/call to nothere %d\n", __disp);stackDump(_state); abort();
};
}

 
struct Memory m = {
{0}, // padding
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // segment code

                {0}
                };



//} // End of namespace DreamGen
