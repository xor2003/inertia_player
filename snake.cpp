/* PLEASE DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE LOST! LOOK FOR README FOR DETAILS */

/* 
 *
 */

#include "snake.h"

#include <curses.h>

#include <unistd.h>

//namespace snake {


int init(struct _STATE* _state)
{
X86_REGREF

_state->_indent=0;
logDebug=fopen("snake.log","w");
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
if (__disp==kbegin) goto main;
else goto __dispatch_call;
#define bottom top+row	// 0 17 bottom equ top+row
#define right left+col	// 0 16 right equ left+col
#define col 40	// 0 15 col equ 40
#define row 15	// 0 14 row equ 15
#define top 2	// 0 13 top equ 2
#define left 0	// 0 12 left equ 0
 // Procedure main() start
main:
	R(MOV(ax, seg_offset(data)));	// 40 mov ax, data
	R(MOV(ds, ax));	// 41 mov ds, ax
	R(MOV(ax, 0x0b800));	// 43 mov ax, 0b800H
	R(MOV(es, ax));	// 44 mov es, ax
	R(MOV(ax, 0x0003));	// 47 mov ax, 0003H
	R(_INT(0x10));	// 48 int 10H
	R(bx = offset(data,msg));	// 50 lea bx, msg
	R(MOV(dx, 00));	// 51 mov dx,00
	R(CALL(kwritestringat));	// 52 call writestringat
	R(bx = offset(data,instructions));	// 54 lea dx, instructions
	R(MOV(dx, 00));	// 51 mov dx,00
	R(CALL(kwritestringat));	// 52 call writestringat
//	R(MOV(ah, 0x09));	// 55 mov ah, 09H
//	R(_INT(0x21));	// 56 int 21h
	R(MOV(ah, 0x07));	// 58 mov ah, 07h
	R(_INT(0x21));	// 59 int 21h
	R(MOV(ax, 0x0003));	// 60 mov ax, 0003H
	R(_INT(0x10));	// 61 int 10H
	R(CALL(kprintbox));	// 62 call printbox
mainloop:
	R(CALL(kdelay));	// 66 call delay
	R(bx = offset(data,msg));	// 67 lea bx, msg
	R(MOV(dx, 00));	// 68 mov dx, 00
	R(CALL(kwritestringat));	// 69 call writestringat
	R(CALL(kshiftsnake));	// 70 call shiftsnake
	R(CMP(m.gameover, 1));	// 71 cmp gameover,1
		R(JZ(gameover_mainloop));	// 72 je gameover_mainloop
	R(CALL(kkeyboardfunctions));	// 74 call keyboardfunctions
	R(CMP(m.quit, 1));	// 75 cmp quit, 1
		R(JZ(quitpressed_mainloop));	// 76 je quitpressed_mainloop
	R(CALL(kfruitgeneration));	// 77 call fruitgeneration
	R(CALL(kdraw));	// 78 call draw
		R(JMP(mainloop));	// 82 jmp mainloop
gameover_mainloop:
	R(MOV(ax, 0x0003));	// 85 mov ax, 0003H
	R(_INT(0x10));	// 86 int 10H
	R(MOV(m.delaytime, 100));	// 87 mov delaytime, 100
	R(MOV(dx, 0x0000));	// 88 mov dx, 0000H
	R(bx = offset(data,gameovermsg));	// 89 lea bx, gameovermsg
	R(CALL(kwritestringat));	// 90 call writestringat
	R(CALL(kdelay));	// 91 call delay
		R(JMP(quit_mainloop));	// 92 jmp quit_mainloop
quitpressed_mainloop:
	R(MOV(ax, 0x0003));	// 95 mov ax, 0003H
	R(_INT(0x10));	// 96 int 10H
	R(MOV(m.delaytime, 100));	// 97 mov delaytime, 100
	R(MOV(dx, 0x0000));	// 98 mov dx, 0000H
	R(bx = offset(data,quitmsg));	// 99 lea bx, quitmsg
	R(CALL(kwritestringat));	// 100 call writestringat
	R(CALL(kdelay));	// 101 call delay
		R(JMP(quit_mainloop));	// 102 jmp quit_mainloop
quit_mainloop:
	R(MOV(ax, 0x0003));	// 109 mov ax, 0003H
	R(_INT(0x10));	// 110 int 10h
	R(MOV(ax, 0x4c00));	// 111 mov ax, 4c00h
	R(_INT(0x21));	// 112 int 21h
 // Procedure delay() start
delay:
	dx=realElapsedTime()*18;
	sleep(1);
//	RETN;
//	sleep(m.delaytime/18);
//	RETN;
	R(MOV(ah, 00));	// 123 mov ah, 00
//	R(_INT(0x1A));	// 124 int 1Ah
	R(MOV(bx, dx));	// 125 mov bx, dx
jmp_delay:
	dx=realElapsedTime()*18;
//	R(_INT(0x1A));	// 128 int 1Ah
	R(SUB(dx, bx));	// 129 sub dx, bx
	R(CMP(dl, m.delaytime));	// 131 cmp dl, delaytime
		R(JL(jmp_delay));	// 132 jl jmp_delay
	R(RETN);	// 133 ret
 // Procedure fruitgeneration() start
fruitgeneration:
	R(MOV(ch, m.fruity));	// 141 mov ch, fruity
	R(MOV(cl, m.fruitx));	// 142 mov cl, fruitx
regenerate:
	R(CMP(m.fruitactive, 1));	// 145 cmp fruitactive, 1
		R(JZ(ret_fruitactive));	// 146 je ret_fruitactive
	R(MOV(ah, 00));	// 147 mov ah, 00
//	R(INT(0x1A));	// 148 int 1Ah
	dx=realElapsedTime()*18;

	R(PUSH(dx));	// 150 push dx
	R(MOV(ax, dx));	// 151 mov ax, dx
	dx = 0;AFFECT_ZF(0); AFFECT_SF(dx,0);	// 152 xor dx, dx
	bh = 0;AFFECT_ZF(0); AFFECT_SF(bh,0);	// 153 xor bh, bh
	R(MOV(bl, row));	// 154 mov bl, row
	R(DEC(bl));	// 155 dec bl
	R(DIV2(bx));	// 156 div bx
	R(MOV(m.fruity, dl));	// 157 mov fruity, dl
	R(INC(m.fruity));	// 158 inc fruity
	R(POP(ax));	// 161 pop ax
	R(MOV(bl, col));	// 162 mov bl, col
	R(DEC(dl));	// 163 dec dl
	bh = 0;AFFECT_ZF(0); AFFECT_SF(bh,0);	// 164 xor bh, bh
	dx = 0;AFFECT_ZF(0); AFFECT_SF(dx,0);	// 165 xor dx, dx
	R(DIV2(bx));	// 166 div bx
	R(MOV(m.fruitx, dl));	// 167 mov fruitx, dl
	R(INC(m.fruitx));	// 168 inc fruitx
	R(CMP(m.fruitx, cl));	// 170 cmp fruitx, cl
		R(JNZ(nevermind));	// 171 jne nevermind
	R(CMP(m.fruity, ch));	// 172 cmp fruity, ch
		R(JNZ(nevermind));	// 173 jne nevermind
		R(JMP(regenerate));	// 174 jmp regenerate
nevermind:
	R(MOV(al, m.fruitx));	// 176 mov al, fruitx
	R(ROR(al, 1));	// 177 ror al,1
		R(JC(regenerate));	// 178 jc regenerate
	R(ADD(m.fruity, top));	// 181 add fruity, top
	R(ADD(m.fruitx, left));	// 182 add fruitx, left
	R(MOV(dh, m.fruity));	// 184 mov dh, fruity
	R(MOV(dl, m.fruitx));	// 185 mov dl, fruitx
	R(CALL(kreadcharat));	// 186 call readcharat
	R(CMP(bl, '*'));	// 187 cmp bl, '*'
		R(JZ(regenerate));	// 188 je regenerate
	R(CMP(bl, '^'));	// 189 cmp bl, '^'
		R(JZ(regenerate));	// 190 je regenerate
	R(CMP(bl, '<'));	// 191 cmp bl, '<'
		R(JZ(regenerate));	// 192 je regenerate
	R(CMP(bl, '>'));	// 193 cmp bl, '>'
		R(JZ(regenerate));	// 194 je regenerate
	R(CMP(bl, 'v'));	// 195 cmp bl, 'v'
		R(JZ(regenerate));	// 196 je regenerate
ret_fruitactive:
	R(RETN);	// 199 ret
 // Procedure dispdigit() start
dispdigit:
	R(ADD(dl, '0'));	// 204 add dl, '0'
	R(MOV(ah, 0x02));	// 205 mov ah, 02H
	R(_INT(0x21));	// 206 int 21H
	R(RETN);	// 207 ret
 // Procedure dispnum() start
dispnum:
	R(TEST(ax, ax));	// 211 test ax,ax
		R(JZ(retz));	// 212 jz retz
	dx = 0;AFFECT_ZF(0); AFFECT_SF(dx,0);	// 213 xor dx, dx
	R(MOV(bx, 10));	// 216 mov bx,10
	R(DIV2(bx));	// 217 div bx
	R(PUSH(dx));	// 219 push dx
	R(CALL(kdispnum));	// 220 call dispnum
	R(POP(dx));	// 221 pop dx
	R(CALL(kdispdigit));	// 222 call dispdigit
	R(RETN);	// 223 ret
retz:
	R(MOV(ah, 02));	// 225 mov ah, 02
	R(RETN);	// 226 ret
 // Procedure setcursorpos() start
setcursorpos:
	R(MOV(ah, 0x02));	// 234 mov ah, 02H
	R(PUSH(bx));	// 235 push bx
	R(MOV(bh, 0));	// 236 mov bh,0
	R(_INT(0x10));	// 237 int 10h
	R(POP(bx));	// 238 pop bx
	R(RETN);	// 239 ret
 // Procedure draw() start
draw:
	R(bx = offset(data,scoremsg));	// 245 lea bx, scoremsg
	R(MOV(dx, 0x0109));	// 246 mov dx, 0109
	R(CALL(kwritestringat));	// 247 call writestringat
	R(ADD(dx, 7));	// 250 add dx, 7
	R(CALL(ksetcursorpos));	// 251 call setcursorpos
	R(MOV(al, m.segmentcount));	// 252 mov al, segmentcount
	R(DEC(al));	// 253 dec al
	ah = 0;AFFECT_ZF(0); AFFECT_SF(ah,0);	// 254 xor ah, ah
	R(CALL(kdispnum));	// 255 call dispnum
	R(si = offset(data,head));	// 257 lea si, head
draw_loop:
	R(MOV(bl, *(raddr(ds,si))));	// 259 mov bl, ds:[si]
	R(TEST(bl, bl));	// 260 test bl, bl
		R(JZ(out_draw));	// 261 jz out_draw
	R(MOV(dx, *(dw*)(raddr(ds,si+1))));	// 262 mov dx, ds:[si+1]
	R(CALL(kwritecharat));	// 263 call writecharat
	R(ADD(si, 3));	// 264 add si,3
		R(JMP(draw_loop));	// 265 jmp draw_loop
out_draw:
	R(MOV(bl, 'F'));	// 268 mov bl, 'F'
	R(MOV(dh, m.fruity));	// 269 mov dh, fruity
	R(MOV(dl, m.fruitx));	// 270 mov dl, fruitx
	R(CALL(kwritecharat));	// 271 call writecharat
	R(MOV(m.fruitactive, 1));	// 272 mov fruitactive, 1
	R(RETN);	// 274 ret
 // Procedure readchar() start
readchar:
//	R(MOV(ah, 0x01));	// 285 mov ah, 01H
//	R(INT(0x16));	// 286 int 16H
notimeout(stdscr,true);
nodelay(stdscr,true);
dl=getch();
if (dl==ERR) dl=0;
	R(RETN);	// 289 ret
//if (_kbhit())
	goto keybdpressed;
		R(JNZ(keybdpressed));	// 287 jnz keybdpressed
	dl = 0;AFFECT_ZF(0); AFFECT_SF(dl,0);	// 288 xor dl, dl
	R(RETN);	// 289 ret
keybdpressed:
//	R(MOV(ah, 0x00));	// 292 mov ah, 00H
//	R(INT(0x16));	// 293 int 16H
//	R(MOV(dl, al));	// 294 mov dl,al
dl = wgetch(stdscr);
	R(RETN);	// 295 ret
 // Procedure keyboardfunctions() start
keyboardfunctions:
	R(CALL(kreadchar));	// 308 call readchar
	R(CMP(dl, 0));	// 309 cmp dl, 0
		R(JZ(next_14));	// 310 je next_14
	R(CMP(dl, 'w'));	// 313 cmp dl, 'w'
		R(JNZ(next_11));	// 314 jne next_11
	R(CMP(*(db*)&m.head, 'v'));	// 315 cmp head, 'v'
		R(JZ(next_14));	// 316 je next_14
	R(MOV(*(db*)&m.head, '^'));	// 317 mov head, '^'
	R(RETN);	// 318 ret
next_11:
	R(CMP(dl, 's'));	// 320 cmp dl, 's'
		R(JNZ(next_12));	// 321 jne next_12
	R(CMP(*(db*)&m.head, '^'));	// 322 cmp head, '^'
		R(JZ(next_14));	// 323 je next_14
	R(MOV(*(db*)&m.head, 'v'));	// 324 mov head, 'v'
	R(RETN);	// 325 ret
next_12:
	R(CMP(dl, 'a'));	// 327 cmp dl, 'a'
		R(JNZ(next_13));	// 328 jne next_13
	R(CMP(*(db*)&m.head, '>'));	// 329 cmp head, '>'
		R(JZ(next_14));	// 330 je next_14
	R(MOV(*(db*)&m.head, '<'));	// 331 mov head, '<'
	R(RETN);	// 332 ret
next_13:
	R(CMP(dl, 'd'));	// 334 cmp dl, 'd'
		R(JNZ(next_14));	// 335 jne next_14
	R(CMP(*(db*)&m.head, '<'));	// 336 cmp head, '<'
		R(JZ(next_14));	// 337 je next_14
	R(MOV(*(db*)&m.head, '>'));	// 338 mov head,'>'
next_14:
	R(CMP(dl, 'q'));	// 340 cmp dl, 'q'
		R(JZ(quit_keyboardfunctions));	// 341 je quit_keyboardfunctions
	R(RETN);	// 342 ret
quit_keyboardfunctions:
	R(INC(m.quit));	// 345 inc quit
	R(RETN);	// 346 ret
 // Procedure shiftsnake() start
shiftsnake:
	R(MOV(bx, offset(data,head)));	// 358 mov bx, offset head
	ax = 0;AFFECT_ZF(0); AFFECT_SF(ax,0);	// 362 xor ax, ax
	R(MOV(al, *(raddr(ds,bx))));	// 363 mov al, [bx]
	R(PUSH(ax));	// 364 push ax
	R(INC(bx));	// 365 inc bx
	R(MOV(ax, *(dw*)(raddr(ds,bx))));	// 366 mov ax, [bx]
	R(INC(bx));	// 367 inc bx
	R(INC(bx));	// 368 inc bx
	cx = 0;AFFECT_ZF(0); AFFECT_SF(cx,0);	// 369 xor cx, cx
l:
	R(MOV(si, *(dw*)(raddr(ds,bx))));	// 371 mov si, [bx]
	R(TEST(si, *(dw*)(raddr(ds,bx))));	// 372 test si, [bx]
		R(JZ(outside));	// 373 jz outside
	R(INC(cx));	// 374 inc cx
	R(INC(bx));	// 375 inc bx
	R(MOV(dx, *(dw*)(raddr(ds,bx))));	// 376 mov dx,[bx]
	R(MOV(*(dw*)(raddr(ds,bx)), ax));	// 377 mov [bx], ax
	R(MOV(ax, dx));	// 378 mov ax,dx
	R(INC(bx));	// 379 inc bx
	R(INC(bx));	// 380 inc bx
		R(JMP(l));	// 381 jmp l
outside:
	R(POP(ax));	// 389 pop ax
	R(PUSH(dx));	// 392 push dx
	R(bx = offset(data,head));	// 396 lea bx, head
	R(INC(bx));	// 397 inc bx
	R(MOV(dx, *(dw*)(raddr(ds,bx))));	// 398 mov dx, [bx]
	R(CMP(al, '<'));	// 400 cmp al, '<'
		R(JNZ(next_1));	// 401 jne next_1
	R(DEC(dl));	// 402 dec dl
	R(DEC(dl));	// 403 dec dl
		R(JMP(done_checking_the_head));	// 404 jmp done_checking_the_head
next_1:
	R(CMP(al, '>'));	// 406 cmp al, '>'
		R(JNZ(next_2));	// 407 jne next_2
	R(INC(dl));	// 408 inc dl
	R(INC(dl));	// 409 inc dl
		R(JMP(done_checking_the_head));	// 410 jmp done_checking_the_head
next_2:
	R(CMP(al, '^'));	// 413 cmp al, '^'
		R(JNZ(next_3));	// 414 jne next_3
	R(DEC(dh));	// 415 dec dh
		R(JMP(done_checking_the_head));	// 418 jmp done_checking_the_head
next_3:
	R(INC(dh));	// 422 inc dh
done_checking_the_head:
	R(MOV(*(dw*)(raddr(ds,bx)), dx));	// 425 mov [bx],dx
	R(CALL(kreadcharat));	// 427 call readcharat ;dx
	R(CMP(bl, 'F'));	// 430 cmp bl, 'F'
		R(JZ(i_ate_fruit));	// 431 je i_ate_fruit
	R(MOV(cx, dx));	// 435 mov cx, dx
	R(POP(dx));	// 436 pop dx
	R(CMP(bl, '*'));	// 437 cmp bl, '*'    ;the snake bit itself, gameover
		R(JZ(game_over));	// 438 je game_over
	R(MOV(bl, 0));	// 439 mov bl, 0
	R(CALL(kwritecharat));	// 440 call writecharat
//R(RETN);	// 461 ret
	R(MOV(dx, cx));	// 441 mov dx, cx
	R(CMP(dh, top));	// 448 cmp dh, top
		R(JZ(game_over));	// 449 je game_over
	R(CMP(dh, bottom));	// 450 cmp dh, bottom
		R(JZ(game_over));	// 451 je game_over
	R(CMP(dl, left));	// 452 cmp dl,left
		R(JZ(game_over));	// 453 je game_over
	R(CMP(dl, right));	// 454 cmp dl, right
		R(JZ(game_over));	// 455 je game_over
	R(RETN);	// 461 ret
game_over:
	R(INC(m.gameover));	// 463 inc gameover
	R(RETN);	// 464 ret
i_ate_fruit:
	R(MOV(al, m.segmentcount));	// 468 mov al, segmentcount
	ah = 0;AFFECT_ZF(0); AFFECT_SF(ah,0);	// 469 xor ah, ah
	R(bx = offset(data,body));	// 472 lea bx, body
	R(MOV(cx, 3));	// 473 mov cx, 3
	R(MUL1_2(cx));	// 474 mul cx
	R(POP(dx));	// 476 pop dx
	R(ADD(bx, ax));	// 477 add bx, ax
	R(MOV(*(raddr(ds,bx)), '*'));	// 478 mov byte ptr ds:[bx], '*'
	R(MOV(*(dw*)(raddr(ds,bx+1)), dx));	// 479 mov [bx+1], dx
	R(INC(m.segmentcount));	// 480 inc segmentcount
	R(MOV(dh, m.fruity));	// 481 mov dh, fruity
	R(MOV(dl, m.fruitx));	// 482 mov dl, fruitx
	R(MOV(bl, 0));	// 483 mov bl, 0
	R(CALL(kwritecharat));	// 484 call writecharat
	R(MOV(m.fruitactive, 0));	// 485 mov fruitactive, 0
	R(RETN);	// 486 ret
 // Procedure printbox() start
printbox:
	R(MOV(dh, top));	// 504 mov dh, top
	R(MOV(dl, left));	// 505 mov dl, left
	R(MOV(cx, col));	// 506 mov cx, col
	R(MOV(bl, '*'));	// 507 mov bl, '*'
l1:
	R(CALL(kwritecharat));	// 509 call writecharat
	R(INC(dl));	// 510 inc dl
		R(LOOP(l1));	// 511 loop l1
	R(MOV(cx, row));	// 513 mov cx, row
l2:
	R(CALL(kwritecharat));	// 515 call writecharat
	R(INC(dh));	// 516 inc dh
		R(LOOP(l2));	// 517 loop l2
	R(MOV(cx, col));	// 519 mov cx, col
l3:
	R(CALL(kwritecharat));	// 521 call writecharat
	R(DEC(dl));	// 522 dec dl
		R(LOOP(l3));	// 523 loop l3
	R(MOV(cx, row));	// 525 mov cx, row
l4:
	R(CALL(kwritecharat));	// 527 call writecharat
	R(DEC(dh));	// 528 dec dh
		R(LOOP(l4));	// 529 loop l4
	R(RETN);	// 531 ret
 // Procedure writecharat() start
writecharat:
	R(PUSH(dx));	// 547 push dx
	R(MOV(ax, dx));	// 548 mov ax, dx
	R(AND(ax, 0x0FF00));	// 549 and ax, 0FF00H
	R(SHR(ax, 1));	// 550 shr ax,1
	R(SHR(ax, 1));	// 551 shr ax,1
	R(SHR(ax, 1));	// 552 shr ax,1
	R(SHR(ax, 1));	// 553 shr ax,1
	R(SHR(ax, 1));	// 554 shr ax,1
	R(SHR(ax, 1));	// 555 shr ax,1
	R(SHR(ax, 1));	// 556 shr ax,1
	R(SHR(ax, 1));	// 557 shr ax,1
	R(PUSH(bx));	// 560 push bx
	R(MOV(bh, 160));	// 561 mov bh, 160
	R(MUL1_1(bh));	// 562 mul bh
	R(POP(bx));	// 563 pop bx
	R(AND(dx, 0x0FF));	// 564 and dx, 0FFH
	R(SHL(dx, 1));	// 565 shl dx,1
	R(ADD(ax, dx));	// 566 add ax, dx
	R(MOV(di, ax));	// 567 mov di, ax
//	R(MOV(*(raddr(es,di)), bl));	// 568 mov es:[di], bl
if (bl==0) bl=' ';
{mvaddch(di/160, (di/2)%80, bl);refresh();}
	R(POP(dx));	// 569 pop dx
	R(RETN);	// 570 ret
 // Procedure readcharat() start
readcharat:
	R(PUSH(dx));	// 585 push dx
	R(MOV(ax, dx));	// 586 mov ax, dx
	R(AND(ax, 0x0FF00));	// 587 and ax, 0FF00H
	R(SHR(ax, 1));	// 588 shr ax,1
	R(SHR(ax, 1));	// 589 shr ax,1
	R(SHR(ax, 1));	// 590 shr ax,1
	R(SHR(ax, 1));	// 591 shr ax,1
	R(SHR(ax, 1));	// 592 shr ax,1
	R(SHR(ax, 1));	// 593 shr ax,1
	R(SHR(ax, 1));	// 594 shr ax,1
	R(SHR(ax, 1));	// 595 shr ax,1
	R(PUSH(bx));	// 596 push bx
	R(MOV(bh, 160));	// 597 mov bh, 160
	R(MUL1_1(bh));	// 598 mul bh
	R(POP(bx));	// 599 pop bx
	R(AND(dx, 0x0FF));	// 600 and dx, 0FFH
	R(SHL(dx, 1));	// 601 shl dx,1
	R(ADD(ax, dx));	// 602 add ax, dx
	R(MOV(di, ax));	// 603 mov di, ax
//	R(MOV(bl, *(raddr(es,di))));	// 604 mov bl,es:[di]
bl=mvinch(di/160, (di/2)%80)&A_CHARTEXT;
	R(POP(dx));	// 605 pop dx
	R(RETN);	// 606 ret
 // Procedure writestringat() start
writestringat:
	R(PUSH(dx));	// 619 push dx
	R(MOV(ax, dx));	// 620 mov ax, dx
	R(AND(ax, 0x0FF00));	// 621 and ax, 0FF00H
	R(SHR(ax, 1));	// 622 shr ax,1
	R(SHR(ax, 1));	// 623 shr ax,1
	R(SHR(ax, 1));	// 624 shr ax,1
	R(SHR(ax, 1));	// 625 shr ax,1
	R(SHR(ax, 1));	// 626 shr ax,1
	R(SHR(ax, 1));	// 627 shr ax,1
	R(SHR(ax, 1));	// 628 shr ax,1
	R(SHR(ax, 1));	// 629 shr ax,1
	R(PUSH(bx));	// 631 push bx
	R(MOV(bh, 160));	// 632 mov bh, 160
	R(MUL1_1(bh));	// 633 mul bh
	R(POP(bx));	// 635 pop bx
	R(AND(dx, 0x0FF));	// 636 and dx, 0FFH
	R(SHL(dx, 1));	// 637 shl dx,1
	R(ADD(ax, dx));	// 638 add ax, dx
	R(MOV(di, ax));	// 639 mov di, ax
loop_writestringat:
	R(MOV(al, *(raddr(ds,bx))));	// 642 mov al, [bx]
	R(TEST(al, al));	// 643 test al, al
		R(JZ(exit_writestringat));	// 644 jz exit_writestringat
//	R(MOV(*(raddr(es,di)), al));	// 645 mov es:[di], al
//	R(INC(di));	// 646 inc di
//	R(INC(di));	// 647 inc di
{mvaddch(di/160, (di/2)%80, al); /*attroff(COLOR_PAIR(ah))*/;di+=2;refresh();}
	R(INC(bx));	// 648 inc bx
		R(JMP(loop_writestringat));	// 649 jmp loop_writestringat
exit_writestringat:
	R(POP(dx));	// 653 pop dx
	R(RETN);	// 654 ret


return;
__dispatch_call:
switch (__disp) {
case kdelay: 	goto delay;
case kdispdigit: 	goto dispdigit;
case kdispnum: 	goto dispnum;
case kdone_checking_the_head: 	goto done_checking_the_head;
case kdraw: 	goto draw;
case kdraw_loop: 	goto draw_loop;
case kexit_writestringat: 	goto exit_writestringat;
case kfruitgeneration: 	goto fruitgeneration;
case kgame_over: 	goto game_over;
case kgameover_mainloop: 	goto gameover_mainloop;
case ki_ate_fruit: 	goto i_ate_fruit;
case kjmp_delay: 	goto jmp_delay;
case kkeybdpressed: 	goto keybdpressed;
case kkeyboardfunctions: 	goto keyboardfunctions;
case kl: 	goto l;
case kl1: 	goto l1;
case kl2: 	goto l2;
case kl3: 	goto l3;
case kl4: 	goto l4;
case kloop_writestringat: 	goto loop_writestringat;
case kmain: 	goto main;
case kmainloop: 	goto mainloop;
case knevermind: 	goto nevermind;
case knext_1: 	goto next_1;
case knext_11: 	goto next_11;
case knext_12: 	goto next_12;
case knext_13: 	goto next_13;
case knext_14: 	goto next_14;
case knext_2: 	goto next_2;
case knext_3: 	goto next_3;
case kout_draw: 	goto out_draw;
case koutside: 	goto outside;
case kprintbox: 	goto printbox;
case kquit_keyboardfunctions: 	goto quit_keyboardfunctions;
case kquit_mainloop: 	goto quit_mainloop;
case kquitpressed_mainloop: 	goto quitpressed_mainloop;
case kreadchar: 	goto readchar;
case kreadcharat: 	goto readcharat;
case kregenerate: 	goto regenerate;
case kret_fruitactive: 	goto ret_fruitactive;
case kretz: 	goto retz;
case ksetcursorpos: 	goto setcursorpos;
case kshiftsnake: 	goto shiftsnake;
case kwritecharat: 	goto writecharat;
case kwritestringat: 	goto writestringat;
default: log_error("Jump/call to nothere %d\n", __disp);stackDump(_state); abort();
};
}

 
struct Memory m = {
{0}, // padding
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // segment data
"Welcome to the snake game!!", // msg
{'\n','\r','U','s','e',' ','a',',',' ','s',',',' ','d',' ','a','n','d',' ','f',' ','t','o',' ','c','o','n','t','r','o','l',' ','y','o','u','r',' ','s','n','a','k','e','\n','\r','U','s','e',' ','q',' ','a','n','y','t','i','m','e',' ','t','o',' ','q','u','i','t','\r','\n','P','r','e','s','s',' ','a','n','y',' ','k','e','y',' ','t','o',' ','c','o','n','t','i','n','u','e','$'}, // instructions
"Thanks for playing! hope you enjoyed", // quitmsg
"OOPS!! your snake died! :P ", // gameovermsg
"Score: ", // scoremsg
{'^','\n','\n'}, // head
"*\n\x0b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", // body
1, // segmentcount
1, // fruitactive
8, // fruitx
8, // fruity
0, // gameover
0, // quit
5, // delaytime
{0}, // padding
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // segment text

                {0}
                };



//} // End of namespace DreamGen
