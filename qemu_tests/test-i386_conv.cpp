/*
 *  x86 CPU test
 *
 *  Copyright (c) 2003 Fabrice Bellard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <errno.h>

#include "asm.h"

struct Memory{
db stack[1024];
};

struct Memory m = {
{0}
};

_STATE sstate;
_STATE* _state=&sstate;
X86_REGREF

void log_debug(const char *fmt, ...){printf("unimp ");}

#define CC_C   	0x0001
#define CC_P 	0x0004
#define CC_A	0x0010
#define CC_Z	0x0040
#define CC_S    0x0080
#define CC_O    0x0800


//#define CC_MASK (CC_C | CC_P | CC_Z | CC_S | CC_O | CC_A)
#define CC_MASK (CC_C | CC_Z | CC_S)

static inline long i2l(long v)
{
    return v;
}





void exec_addl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags; 
    PUSH(iflags);POPF;ADD(*(dd*)&res, (dd)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "addl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_addw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ADD(*(dw*)&res, (dw)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "addw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_addb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ADD(*(db*)&res, (db)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "addb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}


void exec_add(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_addl(s0, s1, 0);
    exec_addw(s0, s1, 0);
    exec_addb(s0, s1, 0);

}

void test_add(void)
{
    exec_add(0x12345678, 0x812FADA);
    exec_add(0x12341, 0x12341);
    exec_add(0x12341, -0x12341);
    exec_add(0xffffffff, 0);
    exec_add(0xffffffff, -1);
    exec_add(0xffffffff, 1);
    exec_add(0xffffffff, 2);
    exec_add(0x7fffffff, 0);
    exec_add(0x7fffffff, 1);
    exec_add(0x7fffffff, -1);
    exec_add(0x80000000, -1);
    exec_add(0x80000000, 1);
    exec_add(0x80000000, -2);
    exec_add(0x12347fff, 0);
    exec_add(0x12347fff, 1);
    exec_add(0x12347fff, -1);
    exec_add(0x12348000, -1);
    exec_add(0x12348000, 1);
    exec_add(0x12348000, -2);
    exec_add(0x12347f7f, 0);
    exec_add(0x12347f7f, 1);
    exec_add(0x12347f7f, -1);
    exec_add(0x12348080, -1);
    exec_add(0x12348080, 1);
    exec_add(0x12348080, -2);
}

void *_test_add __attribute__ ((unused,__section__ ("initcall"))) = test_add;





void exec_subl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SUB(*(dd*)&res, (dd)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "subl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_subw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SUB(*(dw*)&res, (dw)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "subw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_subb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SUB(*(db*)&res, (db)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "subb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}


void exec_sub(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_subl(s0, s1, 0);
    exec_subw(s0, s1, 0);
    exec_subb(s0, s1, 0);

}

void test_sub(void)
{
    exec_sub(0x12345678, 0x812FADA);
    exec_sub(0x12341, 0x12341);
    exec_sub(0x12341, -0x12341);
    exec_sub(0xffffffff, 0);
    exec_sub(0xffffffff, -1);
    exec_sub(0xffffffff, 1);
    exec_sub(0xffffffff, 2);
    exec_sub(0x7fffffff, 0);
    exec_sub(0x7fffffff, 1);
    exec_sub(0x7fffffff, -1);
    exec_sub(0x80000000, -1);
    exec_sub(0x80000000, 1);
    exec_sub(0x80000000, -2);
    exec_sub(0x12347fff, 0);
    exec_sub(0x12347fff, 1);
    exec_sub(0x12347fff, -1);
    exec_sub(0x12348000, -1);
    exec_sub(0x12348000, 1);
    exec_sub(0x12348000, -2);
    exec_sub(0x12347f7f, 0);
    exec_sub(0x12347f7f, 1);
    exec_sub(0x12347f7f, -1);
    exec_sub(0x12348080, -1);
    exec_sub(0x12348080, 1);
    exec_sub(0x12348080, -2);
}

void *_test_sub __attribute__ ((unused,__section__ ("initcall"))) = test_sub;





void exec_xorl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;XOR(*(dd*)&res, (dd)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "xorl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_xorw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;XOR(*(dw*)&res, (dw)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "xorw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_xorb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;XOR(*(db*)&res, (db)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "xorb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}


void exec_xor(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_xorl(s0, s1, 0);
    exec_xorw(s0, s1, 0);
    exec_xorb(s0, s1, 0);

}

void test_xor(void)
{
    exec_xor(0x12345678, 0x812FADA);
    exec_xor(0x12341, 0x12341);
    exec_xor(0x12341, -0x12341);
    exec_xor(0xffffffff, 0);
    exec_xor(0xffffffff, -1);
    exec_xor(0xffffffff, 1);
    exec_xor(0xffffffff, 2);
    exec_xor(0x7fffffff, 0);
    exec_xor(0x7fffffff, 1);
    exec_xor(0x7fffffff, -1);
    exec_xor(0x80000000, -1);
    exec_xor(0x80000000, 1);
    exec_xor(0x80000000, -2);
    exec_xor(0x12347fff, 0);
    exec_xor(0x12347fff, 1);
    exec_xor(0x12347fff, -1);
    exec_xor(0x12348000, -1);
    exec_xor(0x12348000, 1);
    exec_xor(0x12348000, -2);
    exec_xor(0x12347f7f, 0);
    exec_xor(0x12347f7f, 1);
    exec_xor(0x12347f7f, -1);
    exec_xor(0x12348080, -1);
    exec_xor(0x12348080, 1);
    exec_xor(0x12348080, -2);
}

void *_test_xor __attribute__ ((unused,__section__ ("initcall"))) = test_xor;





void exec_andl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;AND(*(dd*)&res, (dd)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "andl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_andw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;AND(*(dw*)&res, (dw)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "andw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_andb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;AND(*(db*)&res, (db)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "andb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}


void exec_and(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_andl(s0, s1, 0);
    exec_andw(s0, s1, 0);
    exec_andb(s0, s1, 0);

}

void test_and(void)
{
    exec_and(0x12345678, 0x812FADA);
    exec_and(0x12341, 0x12341);
    exec_and(0x12341, -0x12341);
    exec_and(0xffffffff, 0);
    exec_and(0xffffffff, -1);
    exec_and(0xffffffff, 1);
    exec_and(0xffffffff, 2);
    exec_and(0x7fffffff, 0);
    exec_and(0x7fffffff, 1);
    exec_and(0x7fffffff, -1);
    exec_and(0x80000000, -1);
    exec_and(0x80000000, 1);
    exec_and(0x80000000, -2);
    exec_and(0x12347fff, 0);
    exec_and(0x12347fff, 1);
    exec_and(0x12347fff, -1);
    exec_and(0x12348000, -1);
    exec_and(0x12348000, 1);
    exec_and(0x12348000, -2);
    exec_and(0x12347f7f, 0);
    exec_and(0x12347f7f, 1);
    exec_and(0x12347f7f, -1);
    exec_and(0x12348080, -1);
    exec_and(0x12348080, 1);
    exec_and(0x12348080, -2);
}

void *_test_and __attribute__ ((unused,__section__ ("initcall"))) = test_and;





void exec_orl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;OR(*(dd*)&res, (dd)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "orl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_orw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;OR(*(dw*)&res, (dw)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "orw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_orb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;OR(*(db*)&res, (db)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "orb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}


void exec_or(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_orl(s0, s1, 0);
    exec_orw(s0, s1, 0);
    exec_orb(s0, s1, 0);

}

void test_or(void)
{
    exec_or(0x12345678, 0x812FADA);
    exec_or(0x12341, 0x12341);
    exec_or(0x12341, -0x12341);
    exec_or(0xffffffff, 0);
    exec_or(0xffffffff, -1);
    exec_or(0xffffffff, 1);
    exec_or(0xffffffff, 2);
    exec_or(0x7fffffff, 0);
    exec_or(0x7fffffff, 1);
    exec_or(0x7fffffff, -1);
    exec_or(0x80000000, -1);
    exec_or(0x80000000, 1);
    exec_or(0x80000000, -2);
    exec_or(0x12347fff, 0);
    exec_or(0x12347fff, 1);
    exec_or(0x12347fff, -1);
    exec_or(0x12348000, -1);
    exec_or(0x12348000, 1);
    exec_or(0x12348000, -2);
    exec_or(0x12347f7f, 0);
    exec_or(0x12347f7f, 1);
    exec_or(0x12347f7f, -1);
    exec_or(0x12348080, -1);
    exec_or(0x12348080, 1);
    exec_or(0x12348080, -2);
}

void *_test_or __attribute__ ((unused,__section__ ("initcall"))) = test_or;





void exec_cmpl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;CMP(*(dd*)&res, (dd)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "cmpl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_cmpw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;CMP(*(dw*)&res, (dw)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "cmpw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_cmpb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;CMP(*(db*)&res, (db)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "cmpb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}


void exec_cmp(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_cmpl(s0, s1, 0);
    exec_cmpw(s0, s1, 0);
    exec_cmpb(s0, s1, 0);

}

void test_cmp(void)
{
    exec_cmp(0x12345678, 0x812FADA);
    exec_cmp(0x12341, 0x12341);
    exec_cmp(0x12341, -0x12341);
    exec_cmp(0xffffffff, 0);
    exec_cmp(0xffffffff, -1);
    exec_cmp(0xffffffff, 1);
    exec_cmp(0xffffffff, 2);
    exec_cmp(0x7fffffff, 0);
    exec_cmp(0x7fffffff, 1);
    exec_cmp(0x7fffffff, -1);
    exec_cmp(0x80000000, -1);
    exec_cmp(0x80000000, 1);
    exec_cmp(0x80000000, -2);
    exec_cmp(0x12347fff, 0);
    exec_cmp(0x12347fff, 1);
    exec_cmp(0x12347fff, -1);
    exec_cmp(0x12348000, -1);
    exec_cmp(0x12348000, 1);
    exec_cmp(0x12348000, -2);
    exec_cmp(0x12347f7f, 0);
    exec_cmp(0x12347f7f, 1);
    exec_cmp(0x12347f7f, -1);
    exec_cmp(0x12348080, -1);
    exec_cmp(0x12348080, 1);
    exec_cmp(0x12348080, -2);
}

void *_test_cmp __attribute__ ((unused,__section__ ("initcall"))) = test_cmp;






void exec_adcl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ADC(*(dd*)&res, (dd)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "adcl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_adcw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ADC(*(dw*)&res, (dw)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "adcw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_adcb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ADC(*(db*)&res, (db)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "adcb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}


void exec_adc(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_adcl(s0, s1, 0);
    exec_adcw(s0, s1, 0);
    exec_adcb(s0, s1, 0);




    exec_adcl(s0, s1, 0x0001);
    exec_adcw(s0, s1, 0x0001);
    exec_adcb(s0, s1, 0x0001);

}

void test_adc(void)
{
    exec_adc(0x12345678, 0x812FADA);
    exec_adc(0x12341, 0x12341);
    exec_adc(0x12341, -0x12341);
    exec_adc(0xffffffff, 0);
    exec_adc(0xffffffff, -1);
    exec_adc(0xffffffff, 1);
    exec_adc(0xffffffff, 2);
    exec_adc(0x7fffffff, 0);
    exec_adc(0x7fffffff, 1);
    exec_adc(0x7fffffff, -1);
    exec_adc(0x80000000, -1);
    exec_adc(0x80000000, 1);
    exec_adc(0x80000000, -2);
    exec_adc(0x12347fff, 0);
    exec_adc(0x12347fff, 1);
    exec_adc(0x12347fff, -1);
    exec_adc(0x12348000, -1);
    exec_adc(0x12348000, 1);
    exec_adc(0x12348000, -2);
    exec_adc(0x12347f7f, 0);
    exec_adc(0x12347f7f, 1);
    exec_adc(0x12347f7f, -1);
    exec_adc(0x12348080, -1);
    exec_adc(0x12348080, 1);
    exec_adc(0x12348080, -2);
}

void *_test_adc __attribute__ ((unused,__section__ ("initcall"))) = test_adc;






void exec_sbbl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SBB(*(dd*)&res, (dd)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "sbbl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_sbbw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SBB(*(dw*)&res, (dw)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "sbbw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_sbbb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SBB(*(db*)&res, (db)s1);PUSHF;POP(flags); printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "sbbb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}


void exec_sbb(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_sbbl(s0, s1, 0);
    exec_sbbw(s0, s1, 0);
    exec_sbbb(s0, s1, 0);




    exec_sbbl(s0, s1, 0x0001);
    exec_sbbw(s0, s1, 0x0001);
    exec_sbbb(s0, s1, 0x0001);

}

void test_sbb(void)
{
    exec_sbb(0x12345678, 0x812FADA);
    exec_sbb(0x12341, 0x12341);
    exec_sbb(0x12341, -0x12341);
    exec_sbb(0xffffffff, 0);
    exec_sbb(0xffffffff, -1);
    exec_sbb(0xffffffff, 1);
    exec_sbb(0xffffffff, 2);
    exec_sbb(0x7fffffff, 0);
    exec_sbb(0x7fffffff, 1);
    exec_sbb(0x7fffffff, -1);
    exec_sbb(0x80000000, -1);
    exec_sbb(0x80000000, 1);
    exec_sbb(0x80000000, -2);
    exec_sbb(0x12347fff, 0);
    exec_sbb(0x12347fff, 1);
    exec_sbb(0x12347fff, -1);
    exec_sbb(0x12348000, -1);
    exec_sbb(0x12348000, 1);
    exec_sbb(0x12348000, -2);
    exec_sbb(0x12347f7f, 0);
    exec_sbb(0x12347f7f, 1);
    exec_sbb(0x12347f7f, -1);
    exec_sbb(0x12348080, -1);
    exec_sbb(0x12348080, 1);
    exec_sbb(0x12348080, -2);
}

void *_test_sbb __attribute__ ((unused,__section__ ("initcall"))) = test_sbb;







void exec_incl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;INC(*(dd*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "incl", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_incw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;INC(*(dw*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "incw", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_incb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;INC(*(db*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "incb", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_inc(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_incl(s0, s1, 0);
    exec_incw(s0, s1, 0);
    exec_incb(s0, s1, 0);




    exec_incl(s0, s1, 0x0001);
    exec_incw(s0, s1, 0x0001);
    exec_incb(s0, s1, 0x0001);

}

void test_inc(void)
{
    exec_inc(0x12345678, 0x812FADA);
    exec_inc(0x12341, 0x12341);
    exec_inc(0x12341, -0x12341);
    exec_inc(0xffffffff, 0);
    exec_inc(0xffffffff, -1);
    exec_inc(0xffffffff, 1);
    exec_inc(0xffffffff, 2);
    exec_inc(0x7fffffff, 0);
    exec_inc(0x7fffffff, 1);
    exec_inc(0x7fffffff, -1);
    exec_inc(0x80000000, -1);
    exec_inc(0x80000000, 1);
    exec_inc(0x80000000, -2);
    exec_inc(0x12347fff, 0);
    exec_inc(0x12347fff, 1);
    exec_inc(0x12347fff, -1);
    exec_inc(0x12348000, -1);
    exec_inc(0x12348000, 1);
    exec_inc(0x12348000, -2);
    exec_inc(0x12347f7f, 0);
    exec_inc(0x12347f7f, 1);
    exec_inc(0x12347f7f, -1);
    exec_inc(0x12348080, -1);
    exec_inc(0x12348080, 1);
    exec_inc(0x12348080, -2);
}

void *_test_inc __attribute__ ((unused,__section__ ("initcall"))) = test_inc;







void exec_decl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;DEC(*(dd*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "decl", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_decw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;DEC(*(dw*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "decw", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_decb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;DEC(*(db*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "decb", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_dec(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_decl(s0, s1, 0);
    exec_decw(s0, s1, 0);
    exec_decb(s0, s1, 0);




    exec_decl(s0, s1, 0x0001);
    exec_decw(s0, s1, 0x0001);
    exec_decb(s0, s1, 0x0001);

}

void test_dec(void)
{
    exec_dec(0x12345678, 0x812FADA);
    exec_dec(0x12341, 0x12341);
    exec_dec(0x12341, -0x12341);
    exec_dec(0xffffffff, 0);
    exec_dec(0xffffffff, -1);
    exec_dec(0xffffffff, 1);
    exec_dec(0xffffffff, 2);
    exec_dec(0x7fffffff, 0);
    exec_dec(0x7fffffff, 1);
    exec_dec(0x7fffffff, -1);
    exec_dec(0x80000000, -1);
    exec_dec(0x80000000, 1);
    exec_dec(0x80000000, -2);
    exec_dec(0x12347fff, 0);
    exec_dec(0x12347fff, 1);
    exec_dec(0x12347fff, -1);
    exec_dec(0x12348000, -1);
    exec_dec(0x12348000, 1);
    exec_dec(0x12348000, -2);
    exec_dec(0x12347f7f, 0);
    exec_dec(0x12347f7f, 1);
    exec_dec(0x12347f7f, -1);
    exec_dec(0x12348080, -1);
    exec_dec(0x12348080, 1);
    exec_dec(0x12348080, -2);
}

void *_test_dec __attribute__ ((unused,__section__ ("initcall"))) = test_dec;







void exec_negl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;NEG(*(dd*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "negl", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_negw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;NEG(*(dw*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "negw", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_negb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;NEG(*(db*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "negb", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_neg(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_negl(s0, s1, 0);
    exec_negw(s0, s1, 0);
    exec_negb(s0, s1, 0);




    exec_negl(s0, s1, 0x0001);
    exec_negw(s0, s1, 0x0001);
    exec_negb(s0, s1, 0x0001);

}

void test_neg(void)
{
    exec_neg(0x12345678, 0x812FADA);
    exec_neg(0x12341, 0x12341);
    exec_neg(0x12341, -0x12341);
    exec_neg(0xffffffff, 0);
    exec_neg(0xffffffff, -1);
    exec_neg(0xffffffff, 1);
    exec_neg(0xffffffff, 2);
    exec_neg(0x7fffffff, 0);
    exec_neg(0x7fffffff, 1);
    exec_neg(0x7fffffff, -1);
    exec_neg(0x80000000, -1);
    exec_neg(0x80000000, 1);
    exec_neg(0x80000000, -2);
    exec_neg(0x12347fff, 0);
    exec_neg(0x12347fff, 1);
    exec_neg(0x12347fff, -1);
    exec_neg(0x12348000, -1);
    exec_neg(0x12348000, 1);
    exec_neg(0x12348000, -2);
    exec_neg(0x12347f7f, 0);
    exec_neg(0x12347f7f, 1);
    exec_neg(0x12347f7f, -1);
    exec_neg(0x12348080, -1);
    exec_neg(0x12348080, 1);
    exec_neg(0x12348080, -2);
}

void *_test_neg __attribute__ ((unused,__section__ ("initcall"))) = test_neg;







void exec_notl(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;NOT(*(dd*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "notl", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_notw(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;NOT(*(dw*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "notw", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_notb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;NOT(*(db*)&res);PUSHF;POP(flags); printf("%-10s A=" "%08lx R=%08lx CCIN=%04lx CC=%04lx\n", "notb", s0, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));;
}

void exec_not(long s0, long s1)
{
    s0 = i2l(s0);
    s1 = i2l(s1);



    exec_notl(s0, s1, 0);
    exec_notw(s0, s1, 0);
    exec_notb(s0, s1, 0);




    exec_notl(s0, s1, 0x0001);
    exec_notw(s0, s1, 0x0001);
    exec_notb(s0, s1, 0x0001);

}

void test_not(void)
{
    exec_not(0x12345678, 0x812FADA);
    exec_not(0x12341, 0x12341);
    exec_not(0x12341, -0x12341);
    exec_not(0xffffffff, 0);
    exec_not(0xffffffff, -1);
    exec_not(0xffffffff, 1);
    exec_not(0xffffffff, 2);
    exec_not(0x7fffffff, 0);
    exec_not(0x7fffffff, 1);
    exec_not(0x7fffffff, -1);
    exec_not(0x80000000, -1);
    exec_not(0x80000000, 1);
    exec_not(0x80000000, -2);
    exec_not(0x12347fff, 0);
    exec_not(0x12347fff, 1);
    exec_not(0x12347fff, -1);
    exec_not(0x12348000, -1);
    exec_not(0x12348000, 1);
    exec_not(0x12348000, -2);
    exec_not(0x12347f7f, 0);
    exec_not(0x12347f7f, 1);
    exec_not(0x12347f7f, -1);
    exec_not(0x12348080, -1);
    exec_not(0x12348080, 1);
    exec_not(0x12348080, -2);
}

void *_test_not __attribute__ ((unused,__section__ ("initcall"))) = test_not;








void exec_shll(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHL(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shll", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_shlw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHL(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shlw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_shlb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHL(*(db*)&res, (db)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shlb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}


void exec_shl(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_shll(s2, s0, s1, 0);



    exec_shlw(s2, s0, s1, 0);


    exec_shlb(s0, s1, 0);

}

void test_shl(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_shl(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_shl(0x813f3421, 0x82345679, i);
}

void *_test_shl __attribute__ ((unused,__section__ ("initcall"))) = test_shl;





void exec_shrl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHR(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shrl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_shrw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHR(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shrw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_shrb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHR(*(db*)&res, (db)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shrb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}


void exec_shr(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_shrl(s2, s0, s1, 0);



    exec_shrw(s2, s0, s1, 0);


    exec_shrb(s0, s1, 0);

}

void test_shr(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_shr(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_shr(0x813f3421, 0x82345679, i);
}

void *_test_shr __attribute__ ((unused,__section__ ("initcall"))) = test_shr;





void exec_sarl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SAR(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "sarl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_sarw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SAR(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "sarw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_sarb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SAR(*(db*)&res, (db)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "sarb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}


void exec_sar(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_sarl(s2, s0, s1, 0);



    exec_sarw(s2, s0, s1, 0);


    exec_sarb(s0, s1, 0);

}

void test_sar(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_sar(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_sar(0x813f3421, 0x82345679, i);
}

void *_test_sar __attribute__ ((unused,__section__ ("initcall"))) = test_sar;





void exec_roll(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ROL(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "roll", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_rolw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ROL(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rolw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_rolb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ROL(*(db*)&res, (db)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rolb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}


void exec_rol(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_roll(s2, s0, s1, 0);



    exec_rolw(s2, s0, s1, 0);


    exec_rolb(s0, s1, 0);

}

void test_rol(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_rol(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_rol(0x813f3421, 0x82345679, i);
}

void *_test_rol __attribute__ ((unused,__section__ ("initcall"))) = test_rol;





void exec_rorl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ROR(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rorl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_rorw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ROR(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rorw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_rorb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;ROR(*(db*)&res, (db)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rorb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}


void exec_ror(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_rorl(s2, s0, s1, 0);



    exec_rorw(s2, s0, s1, 0);


    exec_rorb(s0, s1, 0);

}

void test_ror(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_ror(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_ror(0x813f3421, 0x82345679, i);
}

void *_test_ror __attribute__ ((unused,__section__ ("initcall"))) = test_ror;






void exec_rcrl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;RCR(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rcrl", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_rcrw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;RCR(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rcrw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_rcrb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;RCR(*(db*)&res, (db)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rcrb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}


void exec_rcr(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_rcrl(s2, s0, s1, 0);



    exec_rcrw(s2, s0, s1, 0);


    exec_rcrb(s0, s1, 0);





    exec_rcrl(s2, s0, s1, 0x0001);
    exec_rcrw(s2, s0, s1, 0x0001);
    exec_rcrb(s0, s1, 0x0001);

}

void test_rcr(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_rcr(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_rcr(0x813f3421, 0x82345679, i);
}

void *_test_rcr __attribute__ ((unused,__section__ ("initcall"))) = test_rcr;






void exec_rcll(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;RCL(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rcll", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_rclw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;RCL(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rclw", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_rclb(long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;RCL(*(db*)&res, (db)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "rclb", s0, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}


void exec_rcl(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_rcll(s2, s0, s1, 0);



    exec_rclw(s2, s0, s1, 0);


    exec_rclb(s0, s1, 0);





    exec_rcll(s2, s0, s1, 0x0001);
    exec_rclw(s2, s0, s1, 0x0001);
    exec_rclb(s0, s1, 0x0001);

}

void test_rcl(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_rcl(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_rcl(0x813f3421, 0x82345679, i);
}

void *_test_rcl __attribute__ ((unused,__section__ ("initcall"))) = test_rcl;







void exec_shldl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHLD(*(dd*)&res, (dd)s2, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx C=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shldl", s0, s2, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_shldw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHLD(*(dw*)&res, (dw)s2, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx C=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shldw", s0, s2, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_shld(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_shldl(s2, s0, s1, 0);

    exec_shldw(s2, s0, s1, 0);

}

void test_shld(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_shld(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_shld(0x813f3421, 0x82345679, i);
}

void *_test_shld __attribute__ ((unused,__section__ ("initcall"))) = test_shld;







void exec_shrdl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHRD(*(dd*)&res, (dd)s2, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx C=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shrdl", s0, s2, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_shrdw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;SHRD(*(dw*)&res, (dw)s2, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx C=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "shrdw", s0, s2, s1, res, iflags, flags & (0x0001 | 0x0040 | 0x0080));
}

void exec_shrd(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_shrdl(s2, s0, s1, 0);

    exec_shrdw(s2, s0, s1, 0);

}

void test_shrd(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_shrd(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_shrd(0x813f3421, 0x82345679, i);
}

void *_test_shrd __attribute__ ((unused,__section__ ("initcall"))) = test_shrd;










void exec_btl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;BT(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "btl", s0, s1, res, iflags, flags & (0x0001));
}

void exec_btw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;BT(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "btw", s0, s1, res, iflags, flags & (0x0001));
}

void exec_bt(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_btl(s2, s0, s1, 0);



    exec_btw(s2, s0, s1, 0);

}

void test_bt(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_bt(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_bt(0x813f3421, 0x82345679, i);
}

void *_test_bt __attribute__ ((unused,__section__ ("initcall"))) = test_bt;






void exec_btsl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;BTS(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "btsl", s0, s1, res, iflags, flags & (0x0001));
}

void exec_btsw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;BTS(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "btsw", s0, s1, res, iflags, flags & (0x0001));
}

void exec_bts(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_btsl(s2, s0, s1, 0);



    exec_btsw(s2, s0, s1, 0);

}

void test_bts(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_bts(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_bts(0x813f3421, 0x82345679, i);
}

void *_test_bts __attribute__ ((unused,__section__ ("initcall"))) = test_bts;






void exec_btrl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;BTR(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "btrl", s0, s1, res, iflags, flags & (0x0001));
}

void exec_btrw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;BTR(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "btrw", s0, s1, res, iflags, flags & (0x0001));
}

void exec_btr(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_btrl(s2, s0, s1, 0);



    exec_btrw(s2, s0, s1, 0);

}

void test_btr(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_btr(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_btr(0x813f3421, 0x82345679, i);
}

void *_test_btr __attribute__ ((unused,__section__ ("initcall"))) = test_btr;






void exec_btcl(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;BTC(*(dd*)&res, (dd)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "btcl", s0, s1, res, iflags, flags & (0x0001));
}

void exec_btcw(long s2, long s0, long s1, long iflags)
{
    long res, flags;
    res = s0;
    flags = iflags;
    PUSH(iflags);POPF;BTC(*(dw*)&res, (dw)s1);PUSHF;POP(flags);

    if (s1 != 1)
      flags &= ~0x0800;
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CCIN=%04lx CC=%04lx\n",
           "btcw", s0, s1, res, iflags, flags & (0x0001));
}

void exec_btc(long s2, long s0, long s1)
{
    s2 = i2l(s2);
    s0 = i2l(s0);



    exec_btcl(s2, s0, s1, 0);



    exec_btcw(s2, s0, s1, 0);

}

void test_btc(void)
{
    int i, n;



    n = 32;

    for(i = 0; i < n; i++)
        exec_btc(0x21ad3d34, 0x12345678, i);
    for(i = 0; i < n; i++)
        exec_btc(0x813f3421, 0x82345679, i);
}

void *_test_btc __attribute__ ((unused,__section__ ("initcall"))) = test_btc;


void test_lea(void)
{
    long eax, ebx, ecx, edx, esi, edi, res;
    eax = i2l(0x0001);
    ebx = i2l(0x0002);
    ecx = i2l(0x0004);
    edx = i2l(0x0008);
    esi = i2l(0x0010);
    edi = i2l(0x0020);

    { asm("lea 0x4000, %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000", res);};

    { asm("lea (%%eax)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%eax)", res);};
    { asm("lea (%%ebx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%ebx)", res);};
    { asm("lea (%%ecx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%ecx)", res);};
    { asm("lea (%%edx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%edx)", res);};
    { asm("lea (%%esi)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%esi)", res);};
    { asm("lea (%%edi)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%edi)", res);};

    { asm("lea 0x40(%%eax)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%eax)", res);};
    { asm("lea 0x40(%%ebx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%ebx)", res);};
    { asm("lea 0x40(%%ecx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%ecx)", res);};
    { asm("lea 0x40(%%edx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%edx)", res);};
    { asm("lea 0x40(%%esi)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%esi)", res);};
    { asm("lea 0x40(%%edi)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%edi)", res);};

    { asm("lea 0x4000(%%eax)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%eax)", res);};
    { asm("lea 0x4000(%%ebx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%ebx)", res);};
    { asm("lea 0x4000(%%ecx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%ecx)", res);};
    { asm("lea 0x4000(%%edx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%edx)", res);};
    { asm("lea 0x4000(%%esi)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%esi)", res);};
    { asm("lea 0x4000(%%edi)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%edi)", res);};

    { asm("lea (%%eax, %%ecx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%eax, %%ecx)", res);};
    { asm("lea (%%ebx, %%edx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%ebx, %%edx)", res);};
    { asm("lea (%%ecx, %%ecx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%ecx, %%ecx)", res);};
    { asm("lea (%%edx, %%ecx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%edx, %%ecx)", res);};
    { asm("lea (%%esi, %%ecx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%esi, %%ecx)", res);};
    { asm("lea (%%edi, %%ecx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%edi, %%ecx)", res);};

    { asm("lea 0x40(%%eax, %%ecx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%eax, %%ecx)", res);};
    { asm("lea 0x4000(%%ebx, %%edx)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%ebx, %%edx)", res);};

    { asm("lea (%%ecx, %%ecx, 2)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%ecx, %%ecx, 2)", res);};
    { asm("lea (%%edx, %%ecx, 4)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%edx, %%ecx, 4)", res);};
    { asm("lea (%%esi, %%ecx, 8)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%esi, %%ecx, 8)", res);};

    { asm("lea (,%%eax, 2)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(,%%eax, 2)", res);};
    { asm("lea (,%%ebx, 4)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(,%%ebx, 4)", res);};
    { asm("lea (,%%ecx, 8)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(,%%ecx, 8)", res);};

    { asm("lea 0x40(,%%eax, 2)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(,%%eax, 2)", res);};
    { asm("lea 0x40(,%%ebx, 4)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(,%%ebx, 4)", res);};
    { asm("lea 0x40(,%%ecx, 8)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(,%%ecx, 8)", res);};


    { asm("lea -10(%%ecx, %%ecx, 2)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "-10(%%ecx, %%ecx, 2)", res);};
    { asm("lea -10(%%edx, %%ecx, 4)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "-10(%%edx, %%ecx, 4)", res);};
    { asm("lea -10(%%esi, %%ecx, 8)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "-10(%%esi, %%ecx, 8)", res);};

    { asm("lea 0x4000(%%ecx, %%ecx, 2)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%ecx, %%ecx, 2)", res);};
    { asm("lea 0x4000(%%edx, %%ecx, 4)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%edx, %%ecx, 4)", res);};
    { asm("lea 0x4000(%%esi, %%ecx, 8)" ", %0" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%esi, %%ecx, 8)", res);};

    { asm(".code16 ; .byte 0x67 ; leal " "0x4000, %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "(%%bx)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%bx)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "(%%si)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%si)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "(%%di)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%di)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "0x40(%%bx)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%bx)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "0x40(%%si)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%si)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "0x40(%%di)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%di)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "0x4000(%%bx)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%bx)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "0x4000(%%si)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%si)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "(%%bx,%%si)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%bx,%%si)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "(%%bx,%%di)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "(%%bx,%%di)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "0x40(%%bx,%%si)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%bx,%%si)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "0x40(%%bx,%%di)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x40(%%bx,%%di)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "0x4000(%%bx,%%si)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%bx,%%si)", res);};
    { asm(".code16 ; .byte 0x67 ; leal " "0x4000(%%bx,%%di)" ", %0 ; .code32" : "=r" (res) : "a" (eax), "b" (ebx), "c" (ecx), "d" (edx), "S" (esi), "D" (edi)); printf("lea %s = %08lx\n", "0x4000(%%bx,%%di)", res);};

}

void test_jcc(void)
{
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njne 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jne", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetne %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setne", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovnel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnel", res);
asm("cmpl %2, %1\ncmovnew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njne 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jne", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetne %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setne", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovnel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnel", res);
asm("cmpl %2, %1\ncmovnew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnew", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\nje 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "je", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsete %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "sete", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovel", res);
asm("cmpl %2, %1\ncmovew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\nje 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "je", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsete %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "sete", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovel", res);
asm("cmpl %2, %1\ncmovew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovew", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njl 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jl", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetl %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setl", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovll %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovll", res);
asm("cmpl %2, %1\ncmovlw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovlw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njl 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jl", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetl %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setl", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovll %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovll", res);
asm("cmpl %2, %1\ncmovlw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovlw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njl 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "jl", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetl %b0\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "setl", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovll %k3, %k0\n" : "=r" (res) : "r" (1), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovll", res);
asm("cmpl %2, %1\ncmovlw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovlw", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njle 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jle", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetle %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setle", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovlel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovlel", res);
asm("cmpl %2, %1\ncmovlew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovlew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njle 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jle", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetle %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setle", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovlel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovlel", res);
asm("cmpl %2, %1\ncmovlew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovlew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njle 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "jle", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetle %b0\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "setle", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovlel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovlel", res);
asm("cmpl %2, %1\ncmovlew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovlew", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njge 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jge", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetge %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setge", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovgel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgel", res);
asm("cmpl %2, %1\ncmovgew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njge 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jge", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetge %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setge", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovgel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgel", res);
asm("cmpl %2, %1\ncmovgew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njge 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (-1), "r" (1)); printf("%-10s %d\n", "jge", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetge %b0\n" : "=r" (res) : "r" (-1), "r" (1)); printf("%-10s %d\n", "setge", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovgel %k3, %k0\n" : "=r" (res) : "r" (-1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgel", res);
asm("cmpl %2, %1\ncmovgew %w3, %w0\n" : "=r" (res) : "r" (-1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgew", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njg 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jg", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetg %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setg", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovgl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgl", res);
asm("cmpl %2, %1\ncmovgw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njg 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jg", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetg %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setg", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovgl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgl", res);
asm("cmpl %2, %1\ncmovgw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njg 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "jg", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetg %b0\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "setg", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovgl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgl", res);
asm("cmpl %2, %1\ncmovgw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovgw", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njb 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jb", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetb %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setb", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovbl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbl", res);
asm("cmpl %2, %1\ncmovbw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njb 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jb", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetb %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setb", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovbl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbl", res);
asm("cmpl %2, %1\ncmovbw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njb 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "jb", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetb %b0\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "setb", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovbl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbl", res);
asm("cmpl %2, %1\ncmovbw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbw", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njbe 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jbe", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetbe %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setbe", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovbel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbel", res);
asm("cmpl %2, %1\ncmovbew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njbe 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jbe", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetbe %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setbe", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovbel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbel", res);
asm("cmpl %2, %1\ncmovbew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njbe 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "jbe", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetbe %b0\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "setbe", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovbel %k3, %k0\n" : "=r" (res) : "r" (1), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbel", res);
asm("cmpl %2, %1\ncmovbew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovbew", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njae 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jae", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetae %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setae", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovael %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovael", res);
asm("cmpl %2, %1\ncmovaew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovaew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njae 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jae", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetae %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setae", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovael %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovael", res);
asm("cmpl %2, %1\ncmovaew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovaew", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njae 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "jae", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetae %b0\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "setae", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovael %k3, %k0\n" : "=r" (res) : "r" (1), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovael", res);
asm("cmpl %2, %1\ncmovaew %w3, %w0\n" : "=r" (res) : "r" (1), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovaew", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\nja 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "ja", res);
asm("movl $0, %0\n" "cmpl %2, %1\nseta %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "seta", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmoval %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmoval", res);
asm("cmpl %2, %1\ncmovaw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovaw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\nja 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "ja", res);
asm("movl $0, %0\n" "cmpl %2, %1\nseta %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "seta", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmoval %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmoval", res);
asm("cmpl %2, %1\ncmovaw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovaw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\nja 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "ja", res);
asm("movl $0, %0\n" "cmpl %2, %1\nseta %b0\n" : "=r" (res) : "r" (1), "r" (-1)); printf("%-10s %d\n", "seta", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmoval %k3, %k0\n" : "=r" (res) : "r" (1), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmoval", res);
asm("cmpl %2, %1\ncmovaw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovaw", res); } };


    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njp 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jp", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetp %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setp", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovpl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovpl", res);
asm("cmpl %2, %1\ncmovpw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovpw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njp 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jp", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetp %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setp", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovpl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovpl", res);
asm("cmpl %2, %1\ncmovpw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovpw", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njnp 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "jnp", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetnp %b0\n" : "=r" (res) : "r" (1), "r" (1)); printf("%-10s %d\n", "setnp", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovnpl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnpl", res);
asm("cmpl %2, %1\ncmovnpw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnpw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njnp 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "jnp", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetnp %b0\n" : "=r" (res) : "r" (1), "r" (0)); printf("%-10s %d\n", "setnp", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovnpl %k3, %k0\n" : "=r" (res) : "r" (1), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnpl", res);
asm("cmpl %2, %1\ncmovnpw %w3, %w0\n" : "=r" (res) : "r" (1), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnpw", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njo 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0x7fffffff), "r" (0)); printf("%-10s %d\n", "jo", res);
asm("movl $0, %0\n" "cmpl %2, %1\nseto %b0\n" : "=r" (res) : "r" (0x7fffffff), "r" (0)); printf("%-10s %d\n", "seto", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovol %k3, %k0\n" : "=r" (res) : "r" (0x7fffffff), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovol", res);
asm("cmpl %2, %1\ncmovow %w3, %w0\n" : "=r" (res) : "r" (0x7fffffff), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovow", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njo 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0x7fffffff), "r" (-1)); printf("%-10s %d\n", "jo", res);
asm("movl $0, %0\n" "cmpl %2, %1\nseto %b0\n" : "=r" (res) : "r" (0x7fffffff), "r" (-1)); printf("%-10s %d\n", "seto", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovol %k3, %k0\n" : "=r" (res) : "r" (0x7fffffff), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovol", res);
asm("cmpl %2, %1\ncmovow %w3, %w0\n" : "=r" (res) : "r" (0x7fffffff), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovow", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njno 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0x7fffffff), "r" (0)); printf("%-10s %d\n", "jno", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetno %b0\n" : "=r" (res) : "r" (0x7fffffff), "r" (0)); printf("%-10s %d\n", "setno", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovnol %k3, %k0\n" : "=r" (res) : "r" (0x7fffffff), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnol", res);
asm("cmpl %2, %1\ncmovnow %w3, %w0\n" : "=r" (res) : "r" (0x7fffffff), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnow", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njno 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0x7fffffff), "r" (-1)); printf("%-10s %d\n", "jno", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetno %b0\n" : "=r" (res) : "r" (0x7fffffff), "r" (-1)); printf("%-10s %d\n", "setno", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovnol %k3, %k0\n" : "=r" (res) : "r" (0x7fffffff), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnol", res);
asm("cmpl %2, %1\ncmovnow %w3, %w0\n" : "=r" (res) : "r" (0x7fffffff), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnow", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njs 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0), "r" (1)); printf("%-10s %d\n", "js", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsets %b0\n" : "=r" (res) : "r" (0), "r" (1)); printf("%-10s %d\n", "sets", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovsl %k3, %k0\n" : "=r" (res) : "r" (0), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovsl", res);
asm("cmpl %2, %1\ncmovsw %w3, %w0\n" : "=r" (res) : "r" (0), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovsw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njs 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0), "r" (-1)); printf("%-10s %d\n", "js", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsets %b0\n" : "=r" (res) : "r" (0), "r" (-1)); printf("%-10s %d\n", "sets", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovsl %k3, %k0\n" : "=r" (res) : "r" (0), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovsl", res);
asm("cmpl %2, %1\ncmovsw %w3, %w0\n" : "=r" (res) : "r" (0), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovsw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njs 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0), "r" (0)); printf("%-10s %d\n", "js", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsets %b0\n" : "=r" (res) : "r" (0), "r" (0)); printf("%-10s %d\n", "sets", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovsl %k3, %k0\n" : "=r" (res) : "r" (0), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovsl", res);
asm("cmpl %2, %1\ncmovsw %w3, %w0\n" : "=r" (res) : "r" (0), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovsw", res); } };

    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njns 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0), "r" (1)); printf("%-10s %d\n", "jns", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetns %b0\n" : "=r" (res) : "r" (0), "r" (1)); printf("%-10s %d\n", "setns", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovnsl %k3, %k0\n" : "=r" (res) : "r" (0), "r" (1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnsl", res);
asm("cmpl %2, %1\ncmovnsw %w3, %w0\n" : "=r" (res) : "r" (0), "r" (1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnsw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njns 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0), "r" (-1)); printf("%-10s %d\n", "jns", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetns %b0\n" : "=r" (res) : "r" (0), "r" (-1)); printf("%-10s %d\n", "setns", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovnsl %k3, %k0\n" : "=r" (res) : "r" (0), "r" (-1), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnsl", res);
asm("cmpl %2, %1\ncmovnsw %w3, %w0\n" : "=r" (res) : "r" (0), "r" (-1), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnsw", res); } };
    { int res;
asm("movl $1, %0\n" "cmpl %2, %1\njns 1f\nmovl $0, %0\n" "1:\n" : "=r" (res) : "r" (0), "r" (0)); printf("%-10s %d\n", "jns", res);
asm("movl $0, %0\n" "cmpl %2, %1\nsetns %b0\n" : "=r" (res) : "r" (0), "r" (0)); printf("%-10s %d\n", "setns", res); if (1) { long val = i2l(1); long res = i2l(0x12345678);
asm("cmpl %2, %1\ncmovnsl %k3, %k0\n" : "=r" (res) : "r" (0), "r" (0), "m" (val), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnsl", res);
asm("cmpl %2, %1\ncmovnsw %w3, %w0\n" : "=r" (res) : "r" (0), "r" (0), "r" (1), "0" (res)); printf("%-10s R=" "%08lx\n", "cmovnsw", res); } };
}

void test_loop(void)
{
    long ecx, zf;
    const long ecx_vals[] = {
        0,
        1,
        0x10000,
        0x10001,




    };
    int i, res;


    { for(i = 0; i < sizeof(ecx_vals) / sizeof(long); i++) { ecx = ecx_vals[i]; for(zf = 0; zf < 2; zf++) { asm("test %2, %2\nmovl $1, %0\n" "jcxz 1f\nmovl $0, %0\n" "1:\n" : "=a" (res) : "c" (ecx), "b" (!zf)); printf("%-10s ECX=" "%08lx ZF=%ld r=%d\n", "jcxz", ecx, zf, res); } }};
    { for(i = 0; i < sizeof(ecx_vals) / sizeof(long); i++) { ecx = ecx_vals[i]; for(zf = 0; zf < 2; zf++) { asm("test %2, %2\nmovl $1, %0\n" "loopw 1f\nmovl $0, %0\n" "1:\n" : "=a" (res) : "c" (ecx), "b" (!zf)); printf("%-10s ECX=" "%08lx ZF=%ld r=%d\n", "loopw", ecx, zf, res); } }};
    { for(i = 0; i < sizeof(ecx_vals) / sizeof(long); i++) { ecx = ecx_vals[i]; for(zf = 0; zf < 2; zf++) { asm("test %2, %2\nmovl $1, %0\n" "loopzw 1f\nmovl $0, %0\n" "1:\n" : "=a" (res) : "c" (ecx), "b" (!zf)); printf("%-10s ECX=" "%08lx ZF=%ld r=%d\n", "loopzw", ecx, zf, res); } }};
    { for(i = 0; i < sizeof(ecx_vals) / sizeof(long); i++) { ecx = ecx_vals[i]; for(zf = 0; zf < 2; zf++) { asm("test %2, %2\nmovl $1, %0\n" "loopnzw 1f\nmovl $0, %0\n" "1:\n" : "=a" (res) : "c" (ecx), "b" (!zf)); printf("%-10s ECX=" "%08lx ZF=%ld r=%d\n", "loopnzw", ecx, zf, res); } }};


    { for(i = 0; i < sizeof(ecx_vals) / sizeof(long); i++) { ecx = ecx_vals[i]; for(zf = 0; zf < 2; zf++) { asm("test %2, %2\nmovl $1, %0\n" "jecxz 1f\nmovl $0, %0\n" "1:\n" : "=a" (res) : "c" (ecx), "b" (!zf)); printf("%-10s ECX=" "%08lx ZF=%ld r=%d\n", "jecxz", ecx, zf, res); } }};
    { for(i = 0; i < sizeof(ecx_vals) / sizeof(long); i++) { ecx = ecx_vals[i]; for(zf = 0; zf < 2; zf++) { asm("test %2, %2\nmovl $1, %0\n" "loopl 1f\nmovl $0, %0\n" "1:\n" : "=a" (res) : "c" (ecx), "b" (!zf)); printf("%-10s ECX=" "%08lx ZF=%ld r=%d\n", "loopl", ecx, zf, res); } }};
    { for(i = 0; i < sizeof(ecx_vals) / sizeof(long); i++) { ecx = ecx_vals[i]; for(zf = 0; zf < 2; zf++) { asm("test %2, %2\nmovl $1, %0\n" "loopzl 1f\nmovl $0, %0\n" "1:\n" : "=a" (res) : "c" (ecx), "b" (!zf)); printf("%-10s ECX=" "%08lx ZF=%ld r=%d\n", "loopzl", ecx, zf, res); } }};
    { for(i = 0; i < sizeof(ecx_vals) / sizeof(long); i++) { ecx = ecx_vals[i]; for(zf = 0; zf < 2; zf++) { asm("test %2, %2\nmovl $1, %0\n" "loopnzl 1f\nmovl $0, %0\n" "1:\n" : "=a" (res) : "c" (ecx), "b" (!zf)); printf("%-10s ECX=" "%08lx ZF=%ld r=%d\n", "loopnzl", ecx, zf, res); } }};
}



void test_mulb(long op0, long op1)
{
    long res, s1, s0, flags;
    s0 = op0;
    s1 = op1;
    res = s0;
    flags = 0;
    asm ("push %4\npopf\n"
         "mulb %b2\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags)
         : "q" (s1), "0" (res), "1" (flags));
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n",
           "mulb", s0, s1, res, flags & (0x0001));
}

void test_mulw(long op0h, long op0, long op1)
{
    long res, s1, flags, resh;
    s1 = op1;
    resh = op0h;
    res = op0;
    flags = 0;
    asm ("push %5\n"
         "popf\n"
         "mulw %w3\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags), "=d" (resh)
         : "q" (s1), "0" (res), "1" (flags), "2" (resh));
    printf("%-10s AH=" "%08lx AL=%08lx B=%08lx RH=%08lx RL=%08lx CC=%04lx\n",
           "mulw", op0h, op0, s1, resh, res, flags & (0x0001));
}

void test_mull(long op0h, long op0, long op1)
{
    long res, s1, flags, resh;
    s1 = op1;
    resh = op0h;
    res = op0;
    flags = 0;
    asm ("push %5\n"
         "popf\n"
         "mull %k3\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags), "=d" (resh)
         : "q" (s1), "0" (res), "1" (flags), "2" (resh));
    printf("%-10s AH=" "%08lx AL=%08lx B=%08lx RH=%08lx RL=%08lx CC=%04lx\n",
           "mull", op0h, op0, s1, resh, res, flags & (0x0001));
}





void test_imulb(long op0, long op1)
{
    long res, s1, s0, flags;
    s0 = op0;
    s1 = op1;
    res = s0;
    flags = 0;
    asm ("push %4\n"
         "popf\n"
         "imul""b %b2\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags)
         : "q" (s1), "0" (res), "1" (flags));
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n",
           "imulb", s0, s1, res, flags & (0x0001));
}

void test_imulw(long op0h, long op0, long op1)
{
    long res, s1, flags, resh;
    s1 = op1;
    resh = op0h;
    res = op0;
    flags = 0;
    asm ("push %5\n"
         "popf\n"
         "imulw %w3\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags), "=d" (resh)
         : "q" (s1), "0" (res), "1" (flags), "2" (resh));
    printf("%-10s AH=" "%08lx AL=%08lx B=%08lx RH=%08lx RL=%08lx CC=%04lx\n",
           "imulw", op0h, op0, s1, resh, res, flags & (0x0001));
}

void test_imull(long op0h, long op0, long op1)
{
    long res, s1, flags, resh;
    s1 = op1;
    resh = op0h;
    res = op0;
    flags = 0;
    asm ("push %5\n"
         "popf\n"
         "imull %k3\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags), "=d" (resh)
         : "q" (s1), "0" (res), "1" (flags), "2" (resh));
    printf("%-10s AH=" "%08lx AL=%08lx B=%08lx RH=%08lx RL=%08lx CC=%04lx\n",
           "imull", op0h, op0, s1, resh, res, flags & (0x0001));
}


void test_imulw2(long op0, long op1)
{
    long res, s1, s0, flags;
    s0 = op0;
    s1 = op1;
    res = s0;
    flags = 0;
    asm volatile ("push %4\n"
         "popf\n"
         "imulw %w2, %w0\n"
         "pushf\n"
         "pop %1\n"
         : "=q" (res), "=g" (flags)
         : "q" (s1), "0" (res), "1" (flags));
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n",
           "imulw", s0, s1, res, flags & (0x0001));
}

void test_imull2(long op0, long op1)
{
    long res, s1, s0, flags;
    s0 = op0;
    s1 = op1;
    res = s0;
    flags = 0;
    asm volatile ("push %4\n"
         "popf\n"
         "imull %k2, %k0\n"
         "pushf\n"
         "pop %1\n"
         : "=q" (res), "=g" (flags)
         : "q" (s1), "0" (res), "1" (flags));
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n",
           "imull", s0, s1, res, flags & (0x0001));
}



void test_divb(long op0, long op1)
{
    long res, s1, s0, flags;
    s0 = op0;
    s1 = op1;
    res = s0;
    flags = 0;
    asm ("push %4\n"
         "popf\n"
         "div""b %b2\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags)
         : "q" (s1), "0" (res), "1" (flags));
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n",
           "divb", s0, s1, res, flags & (0));
}

void test_divw(long op0h, long op0, long op1)
{
    long res, s1, flags, resh;
    s1 = op1;
    resh = op0h;
    res = op0;
    flags = 0;
    asm ("push %5\n"
         "popf\n"
         "divw %w3\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags), "=d" (resh)
         : "q" (s1), "0" (res), "1" (flags), "2" (resh));
    printf("%-10s AH=" "%08lx AL=%08lx B=%08lx RH=%08lx RL=%08lx CC=%04lx\n",
           "divw", op0h, op0, s1, resh, res, flags & (0));
}

void test_divl(long op0h, long op0, long op1)
{
    long res, s1, flags, resh;
    s1 = op1;
    resh = op0h;
    res = op0;
    flags = 0;
    asm ("push %5\n"
         "popf\n"
         "divl %k3\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags), "=d" (resh)
         : "q" (s1), "0" (res), "1" (flags), "2" (resh));
    printf("%-10s AH=" "%08lx AL=%08lx B=%08lx RH=%08lx RL=%08lx CC=%04lx\n",
           "divl", op0h, op0, s1, resh, res, flags & (0));
}





void test_idivb(long op0, long op1)
{
    long res, s1, s0, flags;
    s0 = op0;
    s1 = op1;
    res = s0;
    flags = 0;
    asm ("push %4\n"
         "popf\n"
         "idiv""b %b2\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags)
         : "q" (s1), "0" (res), "1" (flags));
    printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n",
           "idivb", s0, s1, res, flags & (0));
}

void test_idivw(long op0h, long op0, long op1)
{
    long res, s1, flags, resh;
    s1 = op1;
    resh = op0h;
    res = op0;
    flags = 0;
    asm ("push %5\n"
         "popf\n"
         "idivw %w3\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags), "=d" (resh)
         : "q" (s1), "0" (res), "1" (flags), "2" (resh));
    printf("%-10s AH=" "%08lx AL=%08lx B=%08lx RH=%08lx RL=%08lx CC=%04lx\n",
           "idivw", op0h, op0, s1, resh, res, flags & (0));
}

void test_idivl(long op0h, long op0, long op1)
{
    long res, s1, flags, resh;
    s1 = op1;
    resh = op0h;
    res = op0;
    flags = 0;
    asm ("push %5\n"
         "popf\n"
         "idivl %k3\n"
         "pushf\n"
         "pop %1\n"
         : "=a" (res), "=g" (flags), "=d" (resh)
         : "q" (s1), "0" (res), "1" (flags), "2" (resh));
    printf("%-10s AH=" "%08lx AL=%08lx B=%08lx RH=%08lx RL=%08lx CC=%04lx\n",
           "idivl", op0h, op0, s1, resh, res, flags & (0));
}


void test_mul(void)
{
    test_imulb(0x1234561d, 4);
    test_imulb(3, -4);
    test_imulb(0x80, 0x80);
    test_imulb(0x10, 0x10);

    test_imulw(0, 0x1234001d, 45);
    test_imulw(0, 23, -45);
    test_imulw(0, 0x8000, 0x8000);
    test_imulw(0, 0x100, 0x100);

    test_imull(0, 0x1234001d, 45);
    test_imull(0, 23, -45);
    test_imull(0, 0x80000000, 0x80000000);
    test_imull(0, 0x10000, 0x10000);

    test_mulb(0x1234561d, 4);
    test_mulb(3, -4);
    test_mulb(0x80, 0x80);
    test_mulb(0x10, 0x10);

    test_mulw(0, 0x1234001d, 45);
    test_mulw(0, 23, -45);
    test_mulw(0, 0x8000, 0x8000);
    test_mulw(0, 0x100, 0x100);

    test_mull(0, 0x1234001d, 45);
    test_mull(0, 23, -45);
    test_mull(0, 0x80000000, 0x80000000);
    test_mull(0, 0x10000, 0x10000);

    test_imulw2(0x1234001d, 45);
    test_imulw2(23, -45);
    test_imulw2(0x8000, 0x8000);
    test_imulw2(0x100, 0x100);

    test_imull2(0x1234001d, 45);
    test_imull2(23, -45);
    test_imull2(0x80000000, 0x80000000);
    test_imull2(0x10000, 0x10000);

    { long res, flags, s1; flags = 0; res = 0; s1 = 0x1234;
asm volatile ("push %3\npopf\nimulw $" "45, %w2, %w0\npushf\npop %1\n" : "=r" (res), "=g" (flags) : "r" (s1), "1" (flags), "0" (res)); printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n", "imulw im", (long)45, (long)0x1234, res, flags & (0));};
    { long res, flags, s1; flags = 0; res = 0; s1 = 23;
asm volatile ("push %3\npopf\nimulw $" "-45" ", %w2, %w0\npushf\npop %1\n" : "=r" (res), "=g" (flags) : "r" (s1), "1" (flags), "0" (res)); printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n", "imulw im", (long)-45, (long)23, res, flags & (0));};
    { long res, flags, s1; flags = 0; res = 0; s1 = 0x80000000;
asm volatile ("push %3\npopf\nimulw $" "0x8000, %w2, %w0\npushf\npop %1\n" : "=r" (res), "=g" (flags) : "r" (s1), "1" (flags), "0" (res)); printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n", "imulw im", (long)0x8000, (long)0x80000000, res, flags & (0));};
    { long res, flags, s1; flags = 0; res = 0; s1 = 0x1000;
asm volatile ("push %3\npopf\nimulw $" "0x7fff, %w2, %w0\npushf\npop %1\n" : "=r" (res), "=g" (flags) : "r" (s1), "1" (flags), "0" (res)); printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n", "imulw im", (long)0x7fff, (long)0x1000, res, flags & (0));};

    { long res, flags, s1; flags = 0; res = 0; s1 = 0x1234;
asm volatile ("push %3\npopf\nimull $" "45, %k2, %k0\npushf\npop %1\n" : "=r" (res), "=g" (flags) : "r" (s1), "1" (flags), "0" (res)); printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n", "imull im", (long)45, (long)0x1234, res, flags & (0));};
    { long res, flags, s1; flags = 0; res = 0; s1 = 23;
asm volatile ("push %3\npopf\nimull $" "-45" ", %k2, %k0\npushf\npop %1\n" : "=r" (res), "=g" (flags) : "r" (s1), "1" (flags), "0" (res)); printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n", "imull im", (long)-45, (long)23, res, flags & (0));};
    { long res, flags, s1; flags = 0; res = 0; s1 = 0x80000000;
asm volatile ("push %3\npopf\nimull $" "0x8000, %k2, %k0\npushf\npop %1\n" : "=r" (res), "=g" (flags) : "r" (s1), "1" (flags), "0" (res)); printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n", "imull im", (long)0x8000, (long)0x80000000, res, flags & (0));};
    { long res, flags, s1; flags = 0; res = 0; s1 = 0x1000;
asm volatile ("push %3\npopf\nimull $" "0x7fff, %k2, %k0\npushf\npop %1\n" : "=r" (res), "=g" (flags) : "r" (s1), "1" (flags), "0" (res)); printf("%-10s A=" "%08lx B=%08lx R=%08lx CC=%04lx\n", "imull im", (long)0x7fff, (long)0x1000, res, flags & (0));};

    test_idivb(0x12341678, 0x127e);
    test_idivb(0x43210123, -5);
    test_idivb(0x12340004, -1);

    test_idivw(0, 0x12345678, 12347);
    test_idivw(0, -23223, -45);
    test_idivw(0, 0x12348000, -1);
    test_idivw(0x12343, 0x12345678, 0x81238567);

    test_idivl(0, 0x12345678, 12347);
    test_idivl(0, -233223, -45);
    test_idivl(0, 0x80000000, -1);
    test_idivl(0x12343, 0x12345678, 0x81234567);

    test_divb(0x12341678, 0x127e);
    test_divb(0x43210123, -5);
    test_divb(0x12340004, -1);

    test_divw(0, 0x12345678, 12347);
    test_divw(0, -23223, -45);
    test_divw(0, 0x12348000, -1);
    test_divw(0x12343, 0x12345678, 0x81238567);

    test_divl(0, 0x12345678, 12347);
    test_divl(0, -233223, -45);
    test_divl(0, 0x80000000, -1);
    test_divl(0x12343, 0x12345678, 0x81234567);

}

void test_bsx(void)
{
    { long res, val, resz; val = 0;
asm("xor %1, %1\nmov $0x12345678, %0\n" "bsrw %w2, %w0 ; setz %b1" : "=&r" (res), "=&q" (resz) : "r" (val)); printf("%-10s A=" "%08lx R=%08lx %ld\n", "bsrw", val, res, resz);};
    { long res, val, resz; val = 0x12340128;
asm("xor %1, %1\nmov $0x12345678, %0\n" "bsrw %w2, %w0 ; setz %b1" : "=&r" (res), "=&q" (resz) : "r" (val)); printf("%-10s A=" "%08lx R=%08lx %ld\n", "bsrw", val, res, resz);};
    { long res, val, resz; val = 0;
asm("xor %1, %1\nmov $0x12345678, %0\n" "bsfw %w2, %w0 ; setz %b1" : "=&r" (res), "=&q" (resz) : "r" (val)); printf("%-10s A=" "%08lx R=%08lx %ld\n", "bsfw", val, res, resz);};
    { long res, val, resz; val = 0x12340128;
asm("xor %1, %1\nmov $0x12345678, %0\n" "bsfw %w2, %w0 ; setz %b1" : "=&r" (res), "=&q" (resz) : "r" (val)); printf("%-10s A=" "%08lx R=%08lx %ld\n", "bsfw", val, res, resz);};
    { long res, val, resz; val = 0;
asm("xor %1, %1\nmov $0x12345678, %0\n" "bsrl %k2, %k0 ; setz %b1" : "=&r" (res), "=&q" (resz) : "r" (val)); printf("%-10s A=" "%08lx R=%08lx %ld\n", "bsrl", val, res, resz);};
    { long res, val, resz; val = 0x00340128;
asm("xor %1, %1\nmov $0x12345678, %0\n" "bsrl %k2, %k0 ; setz %b1" : "=&r" (res), "=&q" (resz) : "r" (val)); printf("%-10s A=" "%08lx R=%08lx %ld\n", "bsrl", val, res, resz);};
    { long res, val, resz; val = 0;
asm("xor %1, %1\nmov $0x12345678, %0\n" "bsfl %k2, %k0 ; setz %b1" : "=&r" (res), "=&q" (resz) : "r" (val)); printf("%-10s A=" "%08lx R=%08lx %ld\n", "bsfl", val, res, resz);};
    { long res, val, resz; val = 0x00340128;
asm("xor %1, %1\nmov $0x12345678, %0\n" "bsfl %k2, %k0 ; setz %b1" : "=&r" (res), "=&q" (resz) : "r" (val)); printf("%-10s A=" "%08lx R=%08lx %ld\n", "bsfl", val, res, resz);};

}


#if 0
union float64u {
    double d;
    uint64_t l;
};

union float64u q_nan = { .l = 0xFFF8000000000000LL };
union float64u s_nan = { .l = 0xFFF0000000000000LL };

void test_fops(double a, double b)
{
    printf("a=%f b=%f a+b=%f\n", a, b, a + b);
    printf("a=%f b=%f a-b=%f\n", a, b, a - b);
    printf("a=%f b=%f a*b=%f\n", a, b, a * b);
    printf("a=%f b=%f a/b=%f\n", a, b, a / b);
    printf("a=%f b=%f fmod(a, b)=%f\n", a, b, fmod(a, b));
    printf("a=%f sqrt(a)=%f\n", a, sqrt(a));
    printf("a=%f sin(a)=%f\n", a, sin(a));
    printf("a=%f cos(a)=%f\n", a, cos(a));
    printf("a=%f tan(a)=%f\n", a, tan(a));
    printf("a=%f log(a)=%f\n", a, log(a));
    printf("a=%f exp(a)=%f\n", a, exp(a));
    printf("a=%f b=%f atan2(a, b)=%f\n", a, b, atan2(a, b));

    printf("a=%f asin(sin(a))=%f\n", a, asin(sin(a)));
    printf("a=%f acos(cos(a))=%f\n", a, acos(cos(a)));
    printf("a=%f atan(tan(a))=%f\n", a, atan(tan(a)));

}

void fpu_clear_exceptions(void)
{
    struct QEMU_PACKED {
        uint16_t fpuc;
        uint16_t dummy1;
        uint16_t fpus;
        uint16_t dummy2;
        uint16_t fptag;
        uint16_t dummy3;
        uint32_t ignored[4];
        long double fpregs[8];
    } float_env32;

    asm volatile ("fnstenv %0\n" : "=m" (float_env32));
    float_env32.fpus &= ~0x7f;
    asm volatile ("fldenv %0\n" : : "m" (float_env32));
}





void test_fcmp(double a, double b)
{
    long eflags, fpus;

    fpu_clear_exceptions();
    asm("fcom %2\n"
        "fstsw %%ax\n"
        : "=a" (fpus)
        : "t" (a), "u" (b));
    printf("fcom(%f %f)=%04lx\n",
           a, b, fpus & (0x4500 | 0x0000));
    fpu_clear_exceptions();
    asm("fucom %2\n"
        "fstsw %%ax\n"
        : "=a" (fpus)
        : "t" (a), "u" (b));
    printf("fucom(%f %f)=%04lx\n",
           a, b, fpus & (0x4500 | 0x0000));
    if (1) {

        fpu_clear_exceptions();
        asm("fcomi %3, %2\n"
            "fstsw %%ax\n"
            "pushf\n"
            "pop %0\n"
            : "=r" (eflags), "=a" (fpus)
            : "t" (a), "u" (b));
        printf("fcomi(%f %f)=%04lx %02lx\n",
               a, b, fpus & 0x0000, eflags & (0x0040 | 0x0004 | 0x0001));
        fpu_clear_exceptions();
        asm("fucomi %3, %2\n"
            "fstsw %%ax\n"
            "pushf\n"
            "pop %0\n"
            : "=r" (eflags), "=a" (fpus)
            : "t" (a), "u" (b));
        printf("fucomi(%f %f)=%04lx %02lx\n",
               a, b, fpus & 0x0000, eflags & (0x0040 | 0x0004 | 0x0001));
    }
    fpu_clear_exceptions();
    asm volatile("fxam\n"
                 "fstsw %%ax\n"
                 : "=a" (fpus)
                 : "t" (a));
    printf("fxam(%f)=%04lx\n", a, fpus & 0x4700);
    fpu_clear_exceptions();
}

void test_fcvt(double a)
{
    float fa;
    long double la;
    int16_t fpuc;
    int i;
    int64_t lla;
    int ia;
    int16_t wa;
    double ra;

    fa = a;
    la = a;
    printf("(float)%f = %f\n", a, fa);
    printf("(long double)%f = %Lf\n", a, la);
    printf("a=%016" 

               "ll" 

               "x\n", *(uint64_t *)&a);
    printf("la=%016" 

                "ll" 

                "x %04x\n", *(uint64_t *)&la,
           *(unsigned short *)((char *)(&la) + 8));


    asm volatile ("fstcw %0" : "=m" (fpuc));
    for(i=0;i<4;i++) {
        uint16_t val16;
        val16 = (fpuc & ~0x0c00) | (i << 10);
        asm volatile ("fldcw %0" : : "m" (val16));
        asm volatile ("fist %0" : "=m" (wa) : "t" (a));
        asm volatile ("fistl %0" : "=m" (ia) : "t" (a));
        asm volatile ("fistpll %0" : "=m" (lla) : "t" (a) : "st");
        asm volatile ("frndint ; fstl %0" : "=m" (ra) : "t" (a));
        asm volatile ("fldcw %0" : : "m" (fpuc));
        printf("(short)a = %d\n", wa);
        printf("(int)a = %d\n", ia);
        printf("(int64_t)a = " "%016" 

                              "ll" 

                              "x\n", lla);
        printf("rint(a) = %f\n", ra);
    }
}





void test_fconst(void)
{
    double a;
    asm("fld1" : "=t" (a)); printf("fld1= %f\n", a);;
    asm("fldl2t" : "=t" (a)); printf("fldl2t= %f\n", a);;
    asm("fldl2e" : "=t" (a)); printf("fldl2e= %f\n", a);;
    asm("fldpi" : "=t" (a)); printf("fldpi= %f\n", a);;
    asm("fldlg2" : "=t" (a)); printf("fldlg2= %f\n", a);;
    asm("fldln2" : "=t" (a)); printf("fldln2= %f\n", a);;
    asm("fldz" : "=t" (a)); printf("fldz= %f\n", a);;
}

void test_fbcd(double a)
{
    unsigned short bcd[5];
    double b;

    asm("fbstp %0" : "=m" (bcd[0]) : "t" (a) : "st");
    asm("fbld %1" : "=t" (b) : "m" (bcd[0]));
    printf("a=%f bcd=%04x%04x%04x%04x%04x b=%f\n",
           a, bcd[4], bcd[3], bcd[2], bcd[1], bcd[0], b);
}

void test_fenv(void)
{
    struct __attribute__((__packed__)) {
        uint16_t fpuc;
        uint16_t dummy1;
        uint16_t fpus;
        uint16_t dummy2;
        uint16_t fptag;
        uint16_t dummy3;
        uint32_t ignored[4];
        long double fpregs[8];
    } float_env32;
    struct __attribute__((__packed__)) {
        uint16_t fpuc;
        uint16_t fpus;
        uint16_t fptag;
        uint16_t ignored[4];
        long double fpregs[8];
    } float_env16;
    double dtab[8];
    double rtab[8];
    int i;

    for(i=0;i<8;i++)
        dtab[i] = i + 1;

    { memset((&float_env16), 0xaa, sizeof(*(&float_env16))); for(i=0;i<5;i++) asm volatile ("fldl %0" : : "m" (dtab[i]));
asm volatile ("data16 fnstenv %0\n" : : "m" (*(&float_env16)));
asm volatile ("data16 fldenv %0\n": : "m" (*(&float_env16))); for(i=0;i<5;i++) asm volatile ("fstpl %0" : "=m" (rtab[i])); for(i=0;i<5;i++) printf("res[%d]=%f\n", i, rtab[i]); printf("fpuc=%04x fpus=%04x fptag=%04x\n", (&float_env16)->fpuc, (&float_env16)->fpus & 0xff00, (&float_env16)->fptag);};
    { memset((&float_env16), 0xaa, sizeof(*(&float_env16))); for(i=0;i<5;i++) asm volatile ("fldl %0" : : "m" (dtab[i]));
asm volatile ("data16 fnsave %0\n" : : "m" (*(&float_env16)));
asm volatile ("data16 frstor %0\n": : "m" (*(&float_env16))); for(i=0;i<5;i++) asm volatile ("fstpl %0" : "=m" (rtab[i])); for(i=0;i<5;i++) printf("res[%d]=%f\n", i, rtab[i]); printf("fpuc=%04x fpus=%04x fptag=%04x\n", (&float_env16)->fpuc, (&float_env16)->fpus & 0xff00, (&float_env16)->fptag);};
    { memset((&float_env32), 0xaa, sizeof(*(&float_env32))); for(i=0;i<5;i++) asm volatile ("fldl %0" : : "m" (dtab[i]));
asm volatile ("fnstenv %0\n" : : "m" (*(&float_env32)));
asm volatile ("fldenv %0\n": : "m" (*(&float_env32))); for(i=0;i<5;i++) asm volatile ("fstpl %0" : "=m" (rtab[i])); for(i=0;i<5;i++) printf("res[%d]=%f\n", i, rtab[i]); printf("fpuc=%04x fpus=%04x fptag=%04x\n", (&float_env32)->fpuc, (&float_env32)->fpus & 0xff00, (&float_env32)->fptag);};
    { memset((&float_env32), 0xaa, sizeof(*(&float_env32))); for(i=0;i<5;i++) asm volatile ("fldl %0" : : "m" (dtab[i]));
asm volatile ("fnsave %0\n" : : "m" (*(&float_env32)));
asm volatile ("frstor %0\n": : "m" (*(&float_env32))); for(i=0;i<5;i++) asm volatile ("fstpl %0" : "=m" (rtab[i])); for(i=0;i<5;i++) printf("res[%d]=%f\n", i, rtab[i]); printf("fpuc=%04x fpus=%04x fptag=%04x\n", (&float_env32)->fpuc, (&float_env32)->fpus & 0xff00, (&float_env32)->fptag);};


    for(i=0;i<5;i++)
        asm volatile ("fldl %0" : : "m" (dtab[i]));
    asm volatile("ffree %st(2)");
    asm volatile ("fnstenv %0\n" : : "m" (float_env32));
    asm volatile ("fninit");
    printf("fptag=%04x\n", float_env32.fptag);
}

void test_fcmov(void)
{
    double a, b;
    long eflags, i;

    a = 1.0;
    b = 2.0;
    for(i = 0; i < 4; i++) {
        eflags = 0;
        if (i & 1)
            eflags |= 0x0001;
        if (i & 2)
            eflags |= 0x0040;
        { double res;
asm("push %3\npopf\nfcmovb %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (eflags)); printf("fcmov%s eflags=0x%04lx-> %f\n", "b", (long)eflags, res);};
        { double res;
asm("push %3\npopf\nfcmove %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (eflags)); printf("fcmov%s eflags=0x%04lx-> %f\n", "e", (long)eflags, res);};
        { double res;
asm("push %3\npopf\nfcmovbe %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (eflags)); printf("fcmov%s eflags=0x%04lx-> %f\n", "be", (long)eflags, res);};
        { double res;
asm("push %3\npopf\nfcmovnb %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (eflags)); printf("fcmov%s eflags=0x%04lx-> %f\n", "nb", (long)eflags, res);};
        { double res;
asm("push %3\npopf\nfcmovne %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (eflags)); printf("fcmov%s eflags=0x%04lx-> %f\n", "ne", (long)eflags, res);};
        { double res;
asm("push %3\npopf\nfcmovnbe %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (eflags)); printf("fcmov%s eflags=0x%04lx-> %f\n", "nbe", (long)eflags, res);};
    }
    { double res;
asm("push %3\npopf\nfcmovu %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (0)); printf("fcmov%s eflags=0x%04lx-> %f\n", "u", (long)0, res);};
    { double res;
asm("push %3\npopf\nfcmovu %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (0x0004)); printf("fcmov%s eflags=0x%04lx-> %f\n", "u", (long)0x0004, res);};
    { double res;
asm("push %3\npopf\nfcmovnu %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (0)); printf("fcmov%s eflags=0x%04lx-> %f\n", "nu", (long)0, res);};
    { double res;
asm("push %3\npopf\nfcmovnu %2, %0\n" : "=t" (res) : "0" (a), "u" (b), "g" (0x0004)); printf("fcmov%s eflags=0x%04lx-> %f\n", "nu", (long)0x0004, res);};
}

void test_floats(void)
{
    test_fops(2, 3);
    test_fops(1.4, -5);
    test_fcmp(2, -1);
    test_fcmp(2, 2);
    test_fcmp(2, 3);
    test_fcmp(2, q_nan.d);
    test_fcmp(q_nan.d, -1);
    test_fcmp(-1.0/0.0, -1);
    test_fcmp(1.0/0.0, -1);
    test_fcvt(0.5);
    test_fcvt(-0.5);
    test_fcvt(1.0/7.0);
    test_fcvt(-1.0/9.0);
    test_fcvt(32768);
    test_fcvt(-1e20);
    test_fcvt(-1.0/0.0);
    test_fcvt(1.0/0.0);
    test_fcvt(q_nan.d);
    test_fconst();
    test_fbcd(1234567890123456.0);
    test_fbcd(-123451234567890.0);
    test_fenv();
    if (1) {
        test_fcmov();
    }
}
#endif

void test_bcd(void)
{
    { int res, flags; res = 0x12340503; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340503, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340506; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340506, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340507; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340507, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340559; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340559, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340560; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340560, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x1234059f; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x1234059f, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x123405a0; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x123405a0, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340503; flags = 0;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340503, res, 0, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340506; flags = 0;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340506, res, 0, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340503; flags = 0x0001;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340503, res, 0x0001, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340506; flags = 0x0001;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340506, res, 0x0001, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340503; flags = 0x0001;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340503, res, 0x0001 | 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340506; flags = 0x0001;
    eax=res;PUSH(flags);POPF;DAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "daa", 0x12340506, res, 0x0001 | 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};

    { int res, flags; res = 0x12340503; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340503, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340506; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340506, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340507; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340507, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340559; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340559, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340560; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340560, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x1234059f; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x1234059f, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x123405a0; flags = 0x0010;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x123405a0, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340503; flags = 0;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340503, res, 0, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340506; flags = 0;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340506, res, 0, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340503; flags = 0x0001;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340503, res, 0x0001, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340506; flags = 0x0001;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340506, res, 0x0001, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340503; flags = 0x0001;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340503, res, 0x0001 | 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340506; flags = 0x0001;
    eax=res;PUSH(flags);POPF;DAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "das", 0x12340506, res, 0x0001 | 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};

    { int res, flags; res = 0x12340205; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aaa", 0x12340205, res, 0x0010, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x12340306; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aaa", 0x12340306, res, 0x0010, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x1234040a; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aaa", 0x1234040a, res, 0x0010, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x123405fa; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aaa", 0x123405fa, res, 0x0010, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x12340205; flags = 0;
    eax=res;PUSH(flags);POPF;AAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aaa", 0x12340205, res, 0, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x12340306; flags = 0;
    eax=res;PUSH(flags);POPF;AAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aaa", 0x12340306, res, 0, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x1234040a; flags = 0;
    eax=res;PUSH(flags);POPF;AAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aaa", 0x1234040a, res, 0, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x123405fa; flags = 0;
    eax=res;PUSH(flags);POPF;AAA;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aaa", 0x123405fa, res, 0, flags & (0x0001 | 0x0010));};

    { int res, flags; res = 0x12340205; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aas", 0x12340205, res, 0x0010, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x12340306; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aas", 0x12340306, res, 0x0010, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x1234040a; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aas", 0x1234040a, res, 0x0010, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x123405fa; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aas", 0x123405fa, res, 0x0010, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x12340205; flags = 0;
    eax=res;PUSH(flags);POPF;AAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aas", 0x12340205, res, 0, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x12340306; flags = 0;
    eax=res;PUSH(flags);POPF;AAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aas", 0x12340306, res, 0, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x1234040a; flags = 0;
    eax=res;PUSH(flags);POPF;AAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aas", 0x1234040a, res, 0, flags & (0x0001 | 0x0010));};
    { int res, flags; res = 0x123405fa; flags = 0;
    eax=res;PUSH(flags);POPF;AAS;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aas", 0x123405fa, res, 0, flags & (0x0001 | 0x0010));};

    { int res, flags; res = 0x12340547; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAM;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aam", 0x12340547, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
    { int res, flags; res = 0x12340407; flags = 0x0010;
    eax=res;PUSH(flags);POPF;AAD;PUSHF;POP(flags);res=eax;
printf("%-10s A=%08x R=%08x CCIN=%04x CC=%04x\n", "aad", 0x12340407, res, 0x0010, flags & (0x0001 | 0x0040 | 0x0080));};
}

void test_xchg(void)
{



    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xchgl %k0, %k1" : "=q" (op0), "+q" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xchgl", op0, op1);};
    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xchgw %w0, %w1" : "=q" (op0), "+q" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xchgw", op0, op1);};
    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xchgb %b0, %b1" : "=q" (op0), "+q" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xchgb", op0, op1);};




    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xchgl %k0, %k1" : "=q" (op0), "+m" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xchgl", op0, op1);};
    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xchgw %w0, %w1" : "=q" (op0), "+m" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xchgw", op0, op1);};
    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xchgb %b0, %b1" : "=q" (op0), "+m" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xchgb", op0, op1);};




    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xaddl %k0, %k1" : "=q" (op0), "+q" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xaddl", op0, op1);};
    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xaddw %w0, %w1" : "=q" (op0), "+q" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xaddw", op0, op1);};
    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xaddb %b0, %b1" : "=q" (op0), "+q" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xaddb", op0, op1);};

    {
        int res;
        res = 0x12345678;
        asm("xaddl %1, %0" : "=r" (res) : "0" (res));
        printf("xaddl same res=%08x\n", res);
    }




    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xaddl %k0, %k1" : "=q" (op0), "+m" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xaddl", op0, op1);};
    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xaddw %w0, %w1" : "=q" (op0), "+m" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xaddw", op0, op1);};
    { long op0, op1; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654);
asm("xaddb %b0, %b1" : "=q" (op0), "+m" (op1) : "0" (op0)); printf("%-10s A=" "%08lx B=%08lx\n", "xaddb", op0, op1);};




    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfbca7654);
asm("cmpxchgl %k0, %k1" : "=q" (op0), "+q" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgl", op2, op0, op1);};
    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfbca7654);
asm("cmpxchgw %w0, %w1" : "=q" (op0), "+q" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgw", op2, op0, op1);};
    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfbca7654);
asm("cmpxchgb %b0, %b1" : "=q" (op0), "+q" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgb", op2, op0, op1);};




    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfffefdfc);
asm("cmpxchgl %k0, %k1" : "=q" (op0), "+q" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgl", op2, op0, op1);};
    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfffefdfc);
asm("cmpxchgw %w0, %w1" : "=q" (op0), "+q" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgw", op2, op0, op1);};
    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfffefdfc);
asm("cmpxchgb %b0, %b1" : "=q" (op0), "+q" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgb", op2, op0, op1);};




    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfbca7654);
asm("cmpxchgl %k0, %k1" : "=q" (op0), "+m" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgl", op2, op0, op1);};
    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfbca7654);
asm("cmpxchgw %w0, %w1" : "=q" (op0), "+m" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgw", op2, op0, op1);};
    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfbca7654);
asm("cmpxchgb %b0, %b1" : "=q" (op0), "+m" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgb", op2, op0, op1);};




    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfffefdfc);
asm("cmpxchgl %k0, %k1" : "=q" (op0), "+m" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgl", op2, op0, op1);};
    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfffefdfc);
asm("cmpxchgw %w0, %w1" : "=q" (op0), "+m" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgw", op2, op0, op1);};
    { long op0, op1, op2; op0 = i2l(0x12345678); op1 = i2l(0xfbca7654); op2 = i2l(0xfffefdfc);
asm("cmpxchgb %b0, %b1" : "=q" (op0), "+m" (op1) : "0" (op0), "a" (op2)); printf("%-10s EAX=" "%08lx A=%08lx C=%08lx\n", "cmpxchgb", op2, op0, op1);};

    {
        uint64_t op0, op1, op2;
        long eax, edx;
        long i, eflags;

        for(i = 0; i < 2; i++) {
            op0 = 0x123456789abcdLL;
            eax = i2l(op0 & 0xffffffff);
            edx = i2l(op0 >> 32);
            if (i == 0)
                op1 = 0xfbca765423456LL;
            else
                op1 = op0;
            op2 = 0x6532432432434LL;
            asm("cmpxchg8b %2\n"
                "pushf\n"
                "pop %3\n"
                : "=a" (eax), "=d" (edx), "=m" (op1), "=g" (eflags)
                : "0" (eax), "1" (edx), "m" (op1), "b" ((int)op2), "c" ((int)(op2 >> 32)));
            printf("cmpxchg8b: eax=" "%08lx edx=%08lx op1=%016" 

                                                                "ll" 

                                                                "x CC=%02lx\n",
                   eax, edx, op1, eflags & 0x0040);
        }
    }
}

void test_misc(void)
{
    char table[256];
    long res, i;

    for(i=0;i<256;i++) table[i] = 256 - i;
    res = 0x12345678;
    // asm ("xlat" : "=a" (res) : "b" (table), "0" (res));
    printf("xlat: EAX=" "%08lx\n", res);

    // asm volatile ("pushl $12345432 ; pushl $0x9abcdef ; popl (%%esp) ; popl %0" : "=g" (res));
    printf("popl esp=%08lx\n", res);


    // asm volatile ("pushl $12345432 ; pushl $0x9abcdef ; popw (%%esp) ; addl $2, %%esp ; popl %0" : "=g" (res));
    printf("popw esp=%08lx\n", res);

}

uint8_t str_buffer[4096];

void test_string(void)
{
    int i;
    for(i = 0;i < sizeof(str_buffer); i++)
        str_buffer[i] = i + 0x56;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nstosb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "stosb", esi, edi, eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nstosw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "stosw", esi, edi, eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nstosl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "stosl", esi, edi, eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nstosb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "stosb", esi, edi, eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nstosw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "stosw", esi, edi, eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nstosl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "stosl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrep stosb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep stosb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrep stosw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep stosw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrep stosl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep stosl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrep stosb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep stosb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrep stosw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep stosw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrep stosl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep stosl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nlodsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nlodsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nlodsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nlodsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nlodsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nlodsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrep lodsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep lodsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrep lodsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep lodsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrep lodsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep lodsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrep lodsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep lodsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrep lodsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep lodsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrep lodsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep lodsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nmovsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "movsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nmovsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "movsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nmovsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "movsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nmovsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "movsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nmovsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "movsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nmovsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "movsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrep movsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep movsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrep movsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep movsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrep movsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep movsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrep movsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep movsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrep movsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep movsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrep movsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "rep movsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nlodsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nlodsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nlodsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nlodsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nlodsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nlodsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "lodsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;


   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nscasb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "scasb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nscasw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "scasw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nscasl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "scasl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nscasb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "scasb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nscasw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "scasw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nscasl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "scasl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepz scasb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz scasb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepz scasw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz scasw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepz scasl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz scasl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepz scasb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz scasb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepz scasw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz scasw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepz scasl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz scasl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepnz scasb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz scasb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepnz scasw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz scasw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepnz scasl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz scasl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepnz scasb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz scasb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepnz scasw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz scasw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepnz scasl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz scasl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\ncmpsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "cmpsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\ncmpsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "cmpsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\ncmpsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "cmpsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\ncmpsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "cmpsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\ncmpsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "cmpsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\ncmpsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "" "cmpsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepz cmpsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz cmpsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepz cmpsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz cmpsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepz cmpsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz cmpsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepz cmpsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz cmpsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepz cmpsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz cmpsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepz cmpsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repz cmpsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
   { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepnz cmpsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz cmpsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepnz cmpsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz cmpsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\n\nrepnz cmpsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz cmpsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepnz cmpsb\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz cmpsb", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepnz cmpsw\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz cmpsw", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; { long esi, edi, eax, ecx, eflags; esi = (long)(str_buffer + sizeof(str_buffer) / 2); edi = (long)(str_buffer + sizeof(str_buffer) / 2) + 16; eax = i2l(0x12345678); ecx = 17;
asm volatile ("push $0\n" "popf\nstd\nrepnz cmpsl\ncld\npushf\npop %4\n" : "=S" (esi), "=D" (edi), "=a" (eax), "=c" (ecx), "=g" (eflags) : "0" (esi), "1" (edi), "2" (eax), "3" (ecx)); printf("%-10s ESI=" "%08lx EDI=%08lx EAX=%08lx ECX=%08lx EFL=%04x\n", "repnz cmpsl", esi - (long)(str_buffer), edi - (long)(str_buffer), eax, ecx, (int)(eflags & (0x0001 | 0x0040 | 0x0080)));}; ;
}

long enter_stack[4096];

static void test_enter(void)
{






    ;
    ;
    ;
    ;


    ;
    ;
    ;
    ;
}

void test_conv(void)
{
    { unsigned long a, r; a = i2l(0x8234a6f8); r = a;
asm volatile("cbw" : "=a" (r) : "0" (r)); printf("%-10s A=" "%08lx R=%08lx\n", "cbw", a, r);};
    { unsigned long a, r; a = i2l(0x8234a6f8); r = a;
asm volatile("cwde" : "=a" (r) : "0" (r)); printf("%-10s A=" "%08lx R=%08lx\n", "cwde", a, r);};




    { unsigned long a, d, r, rh; a = i2l(0x8234a6f8); d = i2l(0x8345a1f2); r = a; rh = d;
asm volatile("cwd" : "=a" (r), "=d" (rh) : "0" (r), "1" (rh)); printf("%-10s A=" "%08lx R=%08lx:" "%08lx\n", "cwd", a, r, rh); };
    { unsigned long a, d, r, rh; a = i2l(0x8234a6f8); d = i2l(0x8345a1f2); r = a; rh = d;
asm volatile("cdq" : "=a" (r), "=d" (rh) : "0" (r), "1" (rh)); printf("%-10s A=" "%08lx R=%08lx:" "%08lx\n", "cdq", a, r, rh); };




    {
        unsigned long a, r;
        a = i2l(0x12345678);
        // asm volatile("bswapl %k0" : "=r" (r) : "0" (a));
        printf("%-10s: A=" "%08lx R=%08lx\n", "bswapl", a, r);
    }

}

extern void *__start_initcall;
extern void *__stop_initcall;


int main(int argc, char **argv)
{
    void **ptr;
    void (*func)(void);

    ptr = &__start_initcall;
    while (ptr != &__stop_initcall) {
        func = *ptr++;
        func();
    }
    test_bsx();
    test_mul();
    test_jcc();
    test_loop();
//    test_floats();

    test_bcd();

    test_xchg();
    test_string();
    test_misc();
    test_lea();







    test_enter();
    test_conv();




    return 0;
}
