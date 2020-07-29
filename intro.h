#ifndef TASMRECOVER_INTRO_STUBS_H__
#define TASMRECOVER_INTRO_STUBS_H__

/* PLEASE DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE LOST! LOOK FOR README FOR DETAILS */

/* 
 *
 */

#include "asm.h"

//namespace intro {

#define kbegin 0x1001
#define kmaketopoloop 0x1111
#define kmainloop 0x1112
#define kangleloop 0x1113
#define kradiusloop 0x1114
#define knowall 0x1115
#define klineloop 0x1116
#define kprojectloop 0x1117
#define kplotloop 0x1118
#define kdontdraw 0x1119

struct MYPACKED Memory{
 db dummy1[4096]; // protective
 db code[16]; // segment code

                        db stack[STACK_SIZE];
                        db heap[HEAP_SIZE];
                };

//class introContext {
//public:
//      introContext() {}

//      void _start();
//};

//} // End of namespace DreamGen

#endif
