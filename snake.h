#ifndef TASMRECOVER_SNAKE_STUBS_H__
#define TASMRECOVER_SNAKE_STUBS_H__

/* PLEASE DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE LOST! LOOK FOR README FOR DETAILS */

/* 
 *
 */

#include "asm.h"

//namespace snake {

#define kbegin 0x1001
#define kmain 0x1111
#define kmainloop 0x1112
#define kgameover_mainloop 0x1113
#define kquitpressed_mainloop 0x1114
#define kquit_mainloop 0x1115
#define kdelay 0x1116
#define kjmp_delay 0x1117
#define kfruitgeneration 0x1118
#define kregenerate 0x1119
#define knevermind 0x111a
#define kret_fruitactive 0x111b
#define kdispdigit 0x111c
#define kdispnum 0x111d
#define kretz 0x111e
#define ksetcursorpos 0x111f
#define kdraw 0x1120
#define kdraw_loop 0x1121
#define kout_draw 0x1122
#define kreadchar 0x1123
#define kkeybdpressed 0x1124
#define kkeyboardfunctions 0x1125
#define knext_11 0x1126
#define knext_12 0x1127
#define knext_13 0x1128
#define knext_14 0x1129
#define kquit_keyboardfunctions 0x112a
#define kshiftsnake 0x112b
#define kl 0x112c
#define koutside 0x112d
#define knext_1 0x112e
#define knext_2 0x112f
#define knext_3 0x1130
#define kdone_checking_the_head 0x1131
#define kgame_over 0x1132
#define ki_ate_fruit 0x1133
#define kprintbox 0x1134
#define kl1 0x1135
#define kl2 0x1136
#define kl3 0x1137
#define kl4 0x1138
#define kwritecharat 0x1139
#define kreadcharat 0x113a
#define kwritestringat 0x113b
#define kloop_writestringat 0x113c
#define kexit_writestringat 0x113d

struct MYPACKED Memory{
 db dummy1[4096]; // protective
 db data[16]; // segment data
char msg[28];
char instructions[92];
char quitmsg[37];
char gameovermsg[28];
char scoremsg[8];
char head[3];
char body[48];
db segmentcount;
db fruitactive;
db fruitx;
db fruity;
db gameover;
db quit;
db delaytime;
 db dummy2[1]; // padding
 db text[16]; // segment text

                        db stack[STACK_SIZE];
                        db heap[HEAP_SIZE];
                };

//class snakeContext {
//public:
//      snakeContext() {}

//      void _start();
//};

//} // End of namespace DreamGen

#endif
