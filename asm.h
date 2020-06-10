#ifndef __asm_h__
#define __asm_h__



#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#ifndef __BORLANDC__
 #include <stdint.h>
 #include <stdbool.h>
 #define MYPACKED __attribute__((__packed__))
 #define MYINT_ENUM : int
#else
 typedef unsigned long uint32_t;
 typedef long int32_t;
 typedef unsigned short int uint16_t;
 typedef short int int16_t;
 typedef unsigned char uint8_t;
 typedef char int8_t;
 struct uint64_t
 {
	long a;
	long b;
 };

 #define MYPACKED
 #define MYINT_ENUM
#endif
//#include <pthread.h>

typedef uint8_t db;
typedef uint16_t dw;
typedef uint32_t dd;
typedef uint64_t dq;


#define VGARAM_SIZE (320*200)

#ifdef __BORLANDC__
 #define STACK_SIZE 4096
 #define HEAP_SIZE 1024
#else
 #define STACK_SIZE (1024*64-16)
 #define HEAP_SIZE 1024*1024 - 16 - STACK_SIZE
#endif

#define NB_SELECTORS 128

#ifdef __cplusplus
//extern "C" {
#endif

static const uint32_t MASK[]={0, 0xff, 0xffff, 0xffffff, 0xffffffff};
#define bitsizeof(dest) (8*sizeof(dest))
#define shiftmodule(dest,shiftbit) (shiftbit&(bitsizeof(dest)-1))
#define bitget(dest,bit) ((bit>=0)?(( (dest) >> bit)&1):0)
#define nthbitone(dest,bit) ( (dq)1 << shiftmodule(dest,bit))
#define bitset(dest,src,bit) {dest=(bit>=0)?(( (dest) & (~nthbitone(dest,bit))) | ((src&1) << bit)):dest;}
#define LSB(a) ((a)&1)
#define MSB(a) (( (a)>>( bitsizeof(a)-1) )&1)

#if defined(_WIN32) || defined(__INTEL_COMPILER)
 #define INLINE __inline
#elif defined(__STDC_VERSION__) && __STDC_VERSION__>=199901L
 #define INLINE inline
#elif defined(__GNUC__)
 #define INLINE __inline__
#else
 #define INLINE
#endif

#if defined(_PROTECTED_MODE)
 #if _BITS == 32
  #define raddr(segment,offset) (reinterpret_cast<db *>(offset))
 #else
  #define raddr(segment,offset) ((db *)&m+(db)(offset)+selectors[segment])
 #endif
#else
 #define raddr(segment,offset) (((db *)&m + ((segment)<<4) + (offset) ))
#endif

#define realAddress(offset, segment) raddr(segment,offset)

#if _BITS == 32
 typedef dd MWORDSIZE;
 #define offset(segment,name) ((dd)(db*)&m.name)
#else
 typedef dw MWORDSIZE;
 #define offset(segment,name) offsetof(struct Memory,name)-offsetof(struct Memory,segment)
#endif

#define seg_offset(segment) ((offsetof(struct Memory,segment))>>4)

// DJGPP
#define MASK_LINEAR(addr)     (((size_t)addr) & 0x000FFFFF)
#define RM_TO_LINEAR(addr)    (((((size_t)addr) & 0xFFFF0000) >> 12) + (((size_t)addr) & 0xFFFF))
#define RM_OFFSET(addr)       (((size_t)addr) & 0xF)
#define RM_SEGMENT(addr)      ((((size_t)addr) >> 4) & 0xFFFF)

#if _BITS == 32
 #define MOVSS(a) {void * dest;void * src;src=realAddress(esi,ds); dest=realAddress(edi,es); \
		memmove(dest,src,a); edi+=a; esi+=a; }
 #define STOS(a,b) {memcpy (realAddress(edi,es), ((db *)&eax)+b, a); edi+=a;}

 #define REP ecx++;while (--ecx != 0)
 #define REPE AFFECT_ZF(0);ecx++;while (--ecx != 0 && ZF)
 #define REPNE AFFECT_ZF(1);ecx++;while (--ecx != 0 && !ZF)
 #define XLAT {al = *raddr(ds,ebx+al);}
 #define CMPS(a) \
	{  \
			db* src=realAddress(esi,ds); db* dest=realAddress(edi,es); \
			CMP(*dest, *src) ); edi+=a; esi+=a; \
	}

 #define SCASB \
	{  \
			db* dest=realAddress(edi,es); \
			CMP(*dest, al); edi+=(DF==0)?1:-1; \
	}
 #define SCASW \
	{  \
			dw* dest=realAddress(edi,es); \
			CMP(*dest, ax); edi+=(DF==0)?2:-2; \
	}
 #define SCASD \
	{  \
			dd* dest=realAddress(edi,es); \
			CMP(*dest, eax); edi+=(DF==0)?4:-4; \
	}

 #define LODS(addr,s) {memcpy (((db *)&eax), &(addr), s);; esi+=s;} // TODO not always si!!!
 #define LODSS(a,b) {memcpy (((db *)&eax)+b, realAddress(esi,ds), a); esi+=a;}

 #ifdef MSB_FIRST
  #define STOSB STOS(1,3)
  #define STOSW STOS(2,2)
 #else
  #define STOSB STOS(1,0)
  #define STOSW {if (es>=0xB800) {STOS(2,0);} else {attron(COLOR_PAIR(ah)); mvaddch(edi/160, (edi/2)%80, al); /*attroff(COLOR_PAIR(ah))*/;edi+=2;refresh();}}
  #define STOSD STOS(4,0)
 #endif

 #define INSB {db a = asm2C_IN(dx);*realAddress(edi,es)=a;edi+=1;}
 #define INSW {dw a = asm2C_INW(dx);*realAddress(edi,es)=a;edi+=2;}

 #define LOOP(label) DEC(ecx); JNZ(label)
 #define LOOPE(label) --ecx; if (ecx!=0 && ZF) GOTOLABEL(label) //TODO
 #define LOOPNE(label) --ecx; if (ecx!=0 && !ZF) GOTOLABEL(label) //TODO

#else // 16 bit
 #define MOVSS(a) {void * dest;void * src;src=realAddress(si,ds); dest=realAddress(di,es); \
		memmove(dest,src,a); di+=a; si+=a; }
 #define STOS(a,b) {memcpy (realAddress(di,es), ((db *)&eax)+b, a); di+=a;}

 #define REP cx++;while (--cx != 0)
 #define REPE AFFECT_ZF(0);cx++;while (--cx != 0 && ZF)
 #define REPNE AFFECT_ZF(1);cx++;while (--cx != 0 && !ZF)
 #define XLAT {al = *raddr(ds,bx+al);}
 #define CMPS(a) \
	{  \
			void * dest;void * src;src=realAddress(si,ds); dest=realAddress(di,es); \
			AFFECT_ZF( (*(char*)dest-*(char*)src) ); di+=a; si+=a; \
	}

 #define SCASB \
	{  \
			db* dest=realAddress(di,es); \
			CMP(*dest, al); edi+=(DF==0)?1:-1; \
	}
 #define SCASW \
	{  \
			dw* dest=realAddress(di,es); \
			CMP(*dest, ax); edi+=(DF==0)?2:-2; \
	}
 #define SCASD \
	{  \
			dd* dest=realAddress(di,es); \
			CMP(*dest, eax); edi+=(DF==0)?4:-4; \
	}

 #define LODS(addr,s) {memcpy (((db *)&eax), &(addr), s);; si+=s;} // TODO not always si!!!
 #define LODSS(a,b) {memcpy (((db *)&eax)+b, realAddress(si,ds), a); si+=a;}

 #ifdef MSB_FIRST
  #define STOSB STOS(1,3)
  #define STOSW STOS(2,2)
 #else

 //SDL2 VGA
  #if SDL_MAJOR_VERSION == 2
   #define STOSB { \
	if (es==0xa000)\
		{ \
	  SDL_SetRenderDrawColor(renderer, vgaPalette[3*al+2], vgaPalette[3*al+1], vgaPalette[3*al], 255); \
          SDL_RenderDrawPoint(renderer, di%320, di/320); \
  	  SDL_RenderPresent(renderer); \
	  di+=1;} \
	else \
		{STOS(1,0);} \
	}
  #else
   #define STOSB { \
		{STOS(1,0);} \
	}
  #endif

  #define STOSW { \
	if (es==0xB800)  \
		{dd t=(di>>1);attrset(COLOR_PAIR(ah)); mvaddch(t/80, t%80, al); /*attroff(COLOR_PAIR(ah))*/;di+=2;refresh();} \
	else \
		{STOS(2,0);} \
	}
//#define STOSW {if (es!=0xB800) {STOS(2,0);} else {di+=2;}}
  #define STOSD STOS(4,0)
 #endif

#define LOOP(label) DEC(cx); JNZ(label)
#define LOOPE(label) --cx; if (cx!=0 && ZF) GOTOLABEL(label) //TODO
#define LOOPNE(label) --cx; if (cx!=0 && !ZF) GOTOLABEL(label) //TODO

#endif

#ifdef MSB_FIRST
typedef struct dblReg {
	db v0;
	db v1;
	db v2;
	db val;
} dblReg;
typedef struct dbhReg {
	db v0;
	db v1;
	db val;
	db v2;
} dbhReg;
typedef struct dwReg {
	dw v0;
	dw val;
} dwReg;
typedef struct dblReg16 {
	db v0;
	db val;
} dblReg16;
typedef struct dbhReg16 {
	db val;
	db v0;
} dbhReg16;
#else
typedef struct dblReg {
	db val;
	db v0;
	db v1;
	db v2;
} dblReg;
typedef struct dbhReg {
	db v0;
	db val;
	db v1;
	db v2;
} dbhReg;
typedef struct dwReg {
	dw val;
	dw v0;
} dwReg;
typedef struct dblReg16 {
	db val;
	db v0;
} dblReg16;
typedef struct dbhReg16 {
	db v0;
	db val;
} dbhReg16;
#endif

typedef struct ddReg {
	dd val;
} ddReg;


typedef union registry32Bits
{
	struct dblReg dbl;
	struct dbhReg dbh;
	struct dwReg dw;
	struct ddReg dd;
} registry32Bits;


typedef struct dwReg16 {
	dw val;
} dwReg16;                                       



typedef union registry16Bits
{
	struct dblReg16 dbl;
	struct dbhReg16 dbh;
	struct dwReg16 dw;
} registry16Bits;




//#define PUSHAD memcpy (&m.stack[m.stackPointer], &m.eax.dd.val, sizeof (dd)*8); m.stackPointer+=sizeof(dd)*8; assert(m.stackPointer<STACK_SIZE)
//pusha AX, CX, DX, BX, SP, BP, SI, DI
//pushad EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
#define PUSHAD {PUSH(eax);PUSH(ecx);PUSH(edx);PUSH(ebx); PUSH(esp);PUSH(ebp);PUSH(esi);PUSH(edi);}
//#define POPAD m.stackPointer-=sizeof(dd)*8; memcpy (&m.eax.dd.val, &m.stack[m.stackPointer], sizeof (dd)*8)
#define POPAD {POP(edi);POP(esi);POP(ebp); POP(ebx); POP(ebx);POP(edx);POP(ecx);POP(eax); }

#define PUSHA {PUSH(ax);PUSH(cx);PUSH(dx);PUSH(bx); PUSH(sp);PUSH(bp);PUSH(si);PUSH(di);}
#define POPA {POP(di);POP(si);POP(bp); POP(bx); POP(bx);POP(dx);POP(cx);POP(ax); }

/*
#define PUSH(a) {memcpy (&stack[stackPointer], &a, sizeof (a)); stackPointer+=sizeof(a); assert(stackPointer<STACK_SIZE);\
	log_debug("after push %d\n",stackPointer);}

#define POP(a) {log_debug("before pop %d\n",stackPointer); \
	stackPointer-=sizeof(a); memcpy (&a, &stack[stackPointer], sizeof (a));}
#define PUSH(a) {stackPointer-=sizeof(a); memcpy (&m.stack[stackPointer], &a, sizeof (a));  assert(stackPointer>8);}
*/
#ifdef DEBUG
 #define PUSH(a) {dd t=a;stackPointer-=sizeof(a); \
		memcpy (raddr(ss,stackPointer), &t, sizeof (a)); \
		assert((raddr(ss,stackPointer) - ((db*)&m.stack))>8);log_debug("after push %x\n",stackPointer);}

 #define POP(a) { log_debug("before pop %x\n",stackPointer);memcpy (&a, raddr(ss,stackPointer), sizeof (a));stackPointer+=sizeof(a);}
#else
 #define PUSH(a) {dd t=a;stackPointer-=sizeof(a); \
		memcpy (raddr(ss,stackPointer), &t, sizeof (a));}

 #define POP(a) {memcpy (&a, raddr(ss,stackPointer), sizeof (a));stackPointer+=sizeof(a);}
#endif


#define AFFECT_ZF(a) {ZF=((a)==0);}
#define AFFECT_CF(a) {CF=(a);}
//#define ISNEGATIVE(a) (a & (1 << (sizeof(a)*8-1)))
//#define AFFECT_SF(a) {SF=ISNEGATIVE(a);}
#define ISNEGATIVE(f,a) ( (a) & (1 << (bitsizeof(f)-1)) )
#define AFFECT_SF(f, a) {SF=ISNEGATIVE(f,a);}

#define CMP(a,b) {dd t=((a)-(b))& MASK[sizeof(a)]; \
		AFFECT_CF((t)>(a)); \
		AFFECT_ZF(t); \
		AFFECT_SF(a,t);}

#define OR(a,b) {a=(a)|(b); \
		AFFECT_ZF(a); \
		AFFECT_SF(a,a); \
		AFFECT_CF(0);}

#define XOR(a,b) {a=(a)^(b); \
		AFFECT_ZF(a); \
		AFFECT_SF(a,a) \
		AFFECT_CF(0);}

#define AND(a,b) {a=(a)&(b); \
		AFFECT_ZF(a); \
		AFFECT_SF(a,a) \
		AFFECT_CF(0);}

#define NEG(a) {AFFECT_CF((a)!=0); \
		a=-a;\
		AFFECT_ZF(a); \
		AFFECT_SF(a,a)}

#define TEST(a,b) {AFFECT_ZF((a)&(b)); \
		AFFECT_CF(0); \
		AFFECT_SF(a,(a)&(b))}

#define SHR(a,b) {if (b) {CF=(a>>(b-1))&1;\
		a=a>>b;\
		AFFECT_ZF(a);\
		AFFECT_SF(a,a)}}

#define SHL(a,b) {if (b) {CF=(a) & (1 << (bitsizeof(a)-(b)));\
		a=a<<b;\
		AFFECT_ZF(a);\
		AFFECT_SF(a,a)}}

#define ROR(a,b) {if (b) {CF=((a)>>(shiftmodule(a,b)-1))&1;\
		a=((a)>>(shiftmodule(a,b)) | a<<(bitsizeof(a)-(shiftmodule(a,b))));}}

#define ROL(a,b) {if (b) {a=(((a)<<(shiftmodule(a,b))) | (a)>>(bitsizeof(a)-(shiftmodule(a,b))));\
		AFFECT_CF(LSB(a));}}

#define RCL(a, b) RCL_(a, b, CF)
template <class D, class C>
void RCL_(D& Destination, C Count, bool& CF_)
{ 
		int TemporaryCount = Count % (bitsizeof(Destination) + 1);
			while(TemporaryCount) {
				bool TemporaryCF = MSB(Destination);
				Destination = (Destination << 1) + CF_;
				CF_ = TemporaryCF;
				--TemporaryCount;
			}
}

#define RCR(a, b) RCR_(a, b, CF)
template <class D, class C>
void RCR_(D& Destination, C Count, bool& CF_)
{ 
		int TemporaryCount = Count % (bitsizeof(Destination) + 1);
			while(TemporaryCount != 0) {
				bool TemporaryCF = LSB(Destination);
				Destination = (Destination >> 1) + (CF_ << (bitsizeof(Destination)-1));
				CF_ = TemporaryCF;
				--TemporaryCount;
			}
}

template <class D>
void SHLD_(D& Destination, D Source, int Count, bool& CF_);

template <class D>
void SHRD_(D& Destination, D Source, int Count, bool& CF_)
{ 
 if(Count != 0) {
int TCount = Count&(2*bitsizeof(D)-1);

if (TCount>bitsizeof(D)) {SHLD_(Destination, Source, 2*bitsizeof(D)-TCount, CF_);} 
else
{
		CF_ = bitget(Destination,TCount-1); Destination>>=TCount;
		for(int i = bitsizeof(D) - TCount; i <= bitsizeof(D) - 1; ++i) 
			if (i>=0) {bitset(Destination,bitget(Source,i + TCount - bitsizeof(D)),i);}
		if (bitsizeof(D) - TCount<0)	 {CF_ = bitget(Source,TCount-bitsizeof(D)-1);}
}
 }
}

template <class D>
void SHLD_(D& Destination, D Source, int Count, bool& CF_)
{ 
 if(Count != 0) {
int TCount = Count&(2*bitsizeof(D)-1);
if (TCount>bitsizeof(D)) {SHRD_(Destination, Source, 2*bitsizeof(D)-TCount, CF_);} 
else
{
		CF_ = bitget(Destination,bitsizeof(D)-TCount); Destination<<=TCount;
		for(int i = 0; i < TCount; ++i) 
			if (i>=0) {bitset(Destination,bitget(Source,bitsizeof(D) - TCount + i),i);}
}
 }
}

#define SHRD(a, b, c) {SHRD_(a, b, c, CF);if (c) {AFFECT_ZF(a);AFFECT_SF(a,a);}}
#define SHLD(a, b, c) {SHLD_(a, b, c, CF);if (c) {AFFECT_ZF(a);AFFECT_SF(a,a);}}
/*
 //TODO CF=(a) & (1 << (sizeof(f)*8-b));
#define SHRD(a,b,c) {if(c) {\
			int shift = c&(2*bitsizeof(a)-1); \
			dd a1=a>>shift; \
			a=a1 | ( (b& ((1<<shift)-1) ) << (bitsizeof(a)-shift) ); \
		AFFECT_ZF(a|b);\
		AFFECT_SF(a,a);}} //TODO optimize
*/

#define SAR(a,b) {if (b) {bool sign = MSB(a);\
	 int shift = (bitsizeof(a)-b);\
         shift = shift>0?shift:0;\
	 dd sigg=shift<(bitsizeof(a))?( (sign?-1:0)<<shift):0;\
         a = b>bitsizeof(a)?0:a;\
	 AFFECT_CF((a >> (b-1))&1);\
	 a=sigg | (a >> b);\
		AFFECT_ZF(a);\
		AFFECT_SF(a,a);}} // TODO optimize


#define READDDp(a) ((dd *) &m.a)
#define READDWp(a) ((dw *) &m.a)
#define READDBp(a) ((db *) &m.a)

#define READDD(a) (a)

#ifdef MSB_FIRST
 #define READDBhW(a) (*(((db *) &a)+0))
 #define READDBhD(a) (*(((db *) &a)+2))
 #define READDBlW(a) (*(((db *) &a)+1))
 #define READDBlD(a) (*(((db *) &a)+3))
#else
 #define READDBhW(a) (*(((db *) &a)+1))
 #define READDBhD(a) (*(((db *) &a)+1))
 #define READDBlW(a) (*(((db *) &a)))
 #define READDBlD(a) (*(((db *) &a)))
#endif

#define READDW(a) (*((dw *) &m.a.dw.val))
#define READDBh(a) (*((db *) &m.a.dbh.val))
#define READDBl(a) (*((db *) &m.a.dbl.val))

#define AAD {al = al + (ah * 10) & 0xFF; ah = 0;} //TODO

#define ADD(a,b) {dq t=(dq)a+(dq)b; \
		AFFECT_CF((t)>MASK[sizeof(a)]); \
		a=t; \
		AFFECT_ZF(a); \
		AFFECT_SF(a,a);}

#define SUB(a,b) {dd t=(a-b)& MASK[sizeof(a)]; \
		AFFECT_CF((t)>(a)); \
		a=t; \
		AFFECT_ZF(a); \
		AFFECT_SF(a,a);}

#define ADC(a,b) {dq t=(dq)a+(dq)b+(dq)CF; \
		AFFECT_CF((t)>MASK[sizeof(a)]); \
		a=t; \
		AFFECT_ZF(a); \
		AFFECT_SF(a,a);}

#define SBB(a,b) {dq t=(dq)a-(dq)b-(dq)CF; \
		AFFECT_CF((t)>MASK[sizeof(a)]); \
		a=t; \
		AFFECT_ZF(a); \
		AFFECT_SF(a,a);} 

// TODO: should affects OF, SF, ZF, AF, and PF
#define INC(a) {a+=1; \
		AFFECT_ZF(a);\
		AFFECT_SF(a,a);} 

#define DEC(a) {a-=1; \
		AFFECT_ZF(a);\
		AFFECT_SF(a,a);} 

// #num_args _ #bytes
#define IMUL1_1(a) {ax=(int16_t)((int8_t)al)*((int8_t)(a));}
#define IMUL1_2(a) {int32_t t=(int32_t)((int16_t)ax)*((int16_t)(a));ax=t;dx=t>>16;}
#define IMUL1_4(a) {int64_t t=(int64_t)((int32_t)eax)*((int32_t)(a));eax=t;edx=t>>32;}
#define IMUL2_2(a,b) {a = ((int16_t)(a)) * ((int16_t)(b));}
#define IMUL2_4(a,b) {a = ((int32_t)(a)) * ((int32_t)(b));}
#define IMUL3_2(a,b,c) {a = ((int16_t)(b)) * ((int16_t)(c));}
#define IMUL3_4(a,b,c) {a = ((int32_t)(b)) * ((int32_t)(c));}

#define MUL1_1(a) {ax=(dw)al*(a);}
#define MUL1_2(a) {dd t=(dd)ax*(a);ax=t;dx=t>>16;}
#define MUL1_4(a) {uint64_t t=(dq)eax*(a);eax=t;edx=t>>32;}
#define MUL2_2(a,b) {a *= (b);}
#define MUL2_4(a,b) {a *= (b);}
#define MUL3_2(a,b,c) {a = (b) * (c);}
#define MUL3_4(a,b,c) {a = (b) * (c);}

#define IDIV1(a) {int16_t t=ax;al=t/((int8_t)a); ah=t%((int8_t)a);}
#define IDIV2(a) {int32_t t=(((int32_t)(int16_t)dx)<<16)|ax; ax=t/((int16_t)a);dx=t%((int16_t)a);}
#define IDIV4(a) {int64_t t=(((int64_t)(int32_t)edx)<<32)|eax;eax=t/((int32_t)a);edx=t%((int32_t)a);}

#define DIV1(a) {dw t=ax;al=t/(a);ah=t%(a);}
#define DIV2(a) {dd t=((((dd)dx)<<16)|ax);ax=t/(a);dx=t%(a);}
#define DIV4(a) {uint64_t t=((((dq)edx)<<32)|eax);eax=t/(a);edx=t%(a);}

#define NOT(a) a= ~(a);// AFFECT_ZF(a) //TODO

#define SETA(a) a=CF==0 && ZF==0;
#define SETNBE(a) a=CF==0 && ZF==0;
#define SETAE(a) a=CF==0;
#define SETNB(a) a=CF==0;
#define SETNC(a) a=CF==0;
#define SETBE(a) a=CF || ZF;
#define SETNA(a) a=CF || ZF;
#define SETB(a) a=CF;
#define SETNAE(a) a=CF;
#define SETC(a) a=CF;
#define SETL(a) a=SF!=OF;
#define SETNGE(a) a=SF!=OF;
#define SETNS(a) a=SF==0;
#define SETS(a) a=SF;
#define SETGE(a) a=SF==OF;
#define SETNL(a) a=SF==OF;
#define SETG(a) a=ZF==0 && SF==OF;
#define SETNLE(a) a=ZF==0 && SF==OF;
#define SETNE(a) a=ZF==0;
#define SETNZ(a) a=ZF==0;
#define SETLE(a) a=ZF || SF!=OF;
#define SETNG(a) a=ZF || SF!=OF;
#define SETE(a) a=ZF;
#define SETZ(a) a=ZF;
#define SETNO(x) a=!OF;
#define SETNP(x) a=!PF;
#define SETO(x) a=OF;
#define SETP(x) a=PF;


#define JE(label) if (ZF) GOTOLABEL(label)
#define JZ(label) JE(label)

#define JNE(label) if (!ZF) GOTOLABEL(label)
#define JNZ(label) JNE(label)

#define JNB(label) if (!CF) GOTOLABEL(label)
#define JAE(label) JNB(label)
#define JNC(label) JNB(label)

#define JB(label) if (CF) GOTOLABEL(label)
#define JC(label) JB(label)
#define JNAE(label) JB(label)

#define JA(label) if (!CF && !ZF) GOTOLABEL(label)
#define JNBE(label) JA(label)

#define JNA(label) if (CF || ZF) GOTOLABEL(label)
#define JBE(label) JNA(label)

#define JGE(label) if (!SF) GOTOLABEL(label)
#define JNL(label) JGE(label)

#define JG(label) if (!ZF && !SF) GOTOLABEL(label)
#define JNLE(label) JG(label)

#define JLE(label) if (ZF || SF) GOTOLABEL(label) // TODO
#define JNG(label) JLE(label)

#define JL(label) if (SF) GOTOLABEL(label) // TODO
#define JNGE(label) JL(label)

#define JCXZ(label) if (cx == 0) GOTOLABEL(label) // TODO
#define JECXZ(label) if (ecx == 0) GOTOLABEL(label) // TODO

#define JS(label) if (SF) GOTOLABEL(label)
#define JNS(label) if (!SF) GOTOLABEL(label)

/*
#if DEBUG >= 3
 #define MOV(dest,src) {log_debug("%s := %x\n",#dest, src); dest = src;}
#else
 #define MOV(dest,src) {dest = src;}
#endif
*/

#if DEBUG >= 3
#define MOV(dest,src) {log_debug("%x := (%x)\n",&dest, src); MOV_(&dest,src);}
#else
#define MOV(dest,src) {MOV_(&dest,src);}
#endif
template <class D, class S>
void MOV_(D* dest, const S& src)
{ *dest = static_cast<D>(src); }

#define LFS(dest,src) {dest = src; fs= *(dw*)((db*)&(src) + sizeof(dest));}
#define LES(dest,src) {dest = src; es = *(dw*)((db*)&(src) + sizeof(dest));}
#define LGS(dest,src) {dest = src; gs = *(dw*)((db*)&(src) + sizeof(dest));}
#define LDS(dest,src) {dest = src; ds = *(dw*)((db*)&(src) + sizeof(dest));}

#define MOVZX(dest,src) dest = src
#define MOVSX(dest,src) {if (ISNEGATIVE(src,src)) { dest = ((-1 ^ (( 1 << (bitsizeof(src)) )-1)) | src ); } else { dest = src; }}

#define BT(dest,src) {CF = dest & nthbitone(dest,src);} //TODO
#define BTS(dest,src) {CF = dest & nthbitone(dest,src); dest |= nthbitone(dest,src);}
#define BTC(dest,src) {CF = dest & nthbitone(dest,src); dest ^= nthbitone(dest,src);}
#define BTR(dest,src) {CF = dest & nthbitone(dest,src); dest &= ~(nthbitone(dest,src));}

// LEA - Load Effective Address
#define LEA(dest,src) dest = src

#define XCHG(dest,src) {dd t = (dd) dest; dest = src; src = t;}//std::swap(dest,src); TODO


#define MOVS(dest,src,s)  {dest=src; dest+=s; src+=s; }
//                        {memmove(dest,src,s); dest+=s; src+=s; } \


#define CMPSB CMPS(1)
#define CBW {ah = ((int8_t)al) < 0?-1:0;} // TODO
#define CWD {dx = ((int16_t)ax) < 0?-1:0;}
#define CWDE {*(((dw*)&eax)+1) = ((int16_t)ax) < 0?-1:0;}

// MOVSx (DF FLAG not implemented)

#define MOVSB MOVSS(1)
#define MOVSW MOVSS(2)
#define MOVSD MOVSS(4)

/*
#define REP_MOVSS(b) MOVSS(b,cx)
#define REP_MOVS(dest,src) while (cx-- > 0) {MOVS(dest,src);}
#define REP_MOVSB REP_MOVSS(1)
#define REP_MOVSW REP_MOVSS(2)
#define REP_MOVSD REP_MOVSS(4)
*/



//#define REP_STOSB while (cx>0) { STOSB; --cx;}
//#define REP_STOSW while (cx>0) { STOSW; --cx;}
//#define REP_STOSD while (cx>0) { STOSD; --cx;}


#ifdef MSB_FIRST
 #define LODSB LODSS(1,3)
 #define LODSW LODSS(2,2)
#else
 #define LODSB LODSS(1,0)
 #define LODSW LODSS(2,0)
#endif

#define LODSD LODSS(4,0)

/*
#define REP_LODS(a,b) for (i=0; i<ecx; i++) { LODS(a,b); }

#ifdef MSB_FIRST
#define REP_LODSB REP_LODS(1,3)
#define REP_LODSW REP_LODS(2,2)
#else
#define REP_LODSB REP_LODS(1,0)
#define REP_LODSW REP_LODS(2,0)
#define REP_LODSD REP_LODS(4,0)
#endif
*/

// JMP - Unconditional Jump
#define JMP(label) GOTOLABEL(label)
#define GOTOLABEL(a) {_source=__LINE__;goto a;}


#define CLD {DF=0;}
#define STD {DF=1;}

#define STC {CF=1;} //TODO
#define CLC {CF=0;} //TODO
#define CMC {CF ^= 1;} //TODO

//struct _STATE;

// directjeu nosetjmp,2
// directmenu
#define _INT(a) {asm2C_INT(_state,a); TESTJUMPTOBACKGROUND;}

#define TESTJUMPTOBACKGROUND  //if (jumpToBackGround) CALL(moveToBackGround);

void asm2C_OUT(int16_t address, int data);

#define OUT(a,b) asm2C_OUT(a,b)
int8_t asm2C_IN(int16_t data);
#define IN(a,b) a = asm2C_IN(b); TESTJUMPTOBACKGROUND

#define STI // TODO: STI not implemented
#define CLI // TODO: STI not implemented
//#define PUSHF {dd t = CF+(ZF<<1)+(DF<<2)+(SF<<3); PUSH(t);}
//#define POPF {dd t; POP(t); CF=t&1; ZF=(t>>1)&1; DF=(t>>2)&1; SF=(t>>3)&1;}

#define PUSHF {PUSH( (dd) ((CF?1:0)|(PF?4:0)|(AF?0x10:0)|(ZF?0x40:0)|(SF?0x80:0)|(DF?0x400:0)|(OF?0x800:0)) );}
#define POPF {dd t; POP(t); CF=t&1;  PF=(t&4);AF=(t&0x10);ZF=(t&0x40);SF=(t&0x80);DF=(t&0x400);OF=(t&0x800);}
#define NOP

/*
#define CALLF(label) {log_debug("before callf %d\n",stackPointer);PUSH(cs);CALL(label);}
#define CALL(label) \
	{ log_debug("before call %d\n",stackPointer); db tt='x';  \
	if (setjmp(jmpbuffer) == 0) { \
		PUSH(jmpbuffer); PUSH(tt);\
		JMP(label); \
	} }

#define RET {log_debug("before ret %d\n",stackPointer); db tt=0; POP(tt); if (tt!='x') {log_error("Stack corrupted.\n");exit(1);} \
 		POP(jmpbuffer); log_debug("after ret %d\n",stackPointer);longjmp(jmpbuffer, 0);}

#define RETN RET
#define RETF {log_debug("before ret %d\n",stackPointer); db tt=0; POP(tt); if (tt!='x') {log_error("Stack corrupted.\n");exit(1);} \
 		POP(jmpbuffer); stackPointer-=2; log_debug("after retf %d\n",stackPointer);longjmp(jmpbuffer, 0);}
*/
#define CALLF(label) {PUSH(cs);CALL(label);}
/*
#define CALL(label) \
	{ db tt='x';  \
	if (setjmp(jmpbuffer) == 0) { \
		PUSH(jmpbuffer); PUSH(tt);\
		JMP(label); \
	} }

#define RET {db tt=0; POP(tt); if (tt!='x') {log_error("Stack corrupted.\n");exit(1);} \
 		POP(jmpbuffer); longjmp(jmpbuffer, 0);}
#define RETF {db tt=0; POP(tt); if (tt!='x') {log_error("Stack corrupted.\n");exit(1);} \
 		POP(jmpbuffer); stackPointer-=2; longjmp(jmpbuffer, 0);}
*/
#if DEBUG
 #define CALL(label) \
	{ MWORDSIZE tt='xy'; PUSH(tt); \
	  log_debug("after call %x\n",stackPointer); \
	  ++_state->_indent;_state->_str=log_spaces(_state->_indent);\
	  mainproc(label, _state); \
	}

 #define RET {log_debug("before ret %x\n",stackPointer); MWORDSIZE tt=0; POP(tt); if (tt!='xy') {log_error("Stack corrupted.\n");exit(1);} \
	log_debug("after ret %x\n",stackPointer); \
	--_state->_indent;_state->_str=log_spaces(_state->_indent);return;}

 #define RETF {log_debug("before retf %x\n",stackPointer); MWORDSIZE tt=0; POP(tt); if (tt!='xy') {log_error("Stack corrupted.\n");exit(1);} \
	dw tmpp;POP(tmpp); \
	log_debug("after retf %x\n",stackPointer); \
	--_state->_indent;_state->_str=log_spaces(_state->_indent);return;}
#else
 #define CALL(label) \
	{ MWORDSIZE tt='xy'; PUSH(tt); \
	  mainproc(label, _state); \
	}

 #define RET {MWORDSIZE tt=0; POP(tt);  \
	return;}

 #define RETF {MWORDSIZE tt=0; POP(tt); \
	dw tmpp;POP(tmpp); \
	return;}
#endif

#define RETN RET
#define IRET RETF
//#define RETF {dw tt=0; POP(tt); RET;}


#ifdef __LIBSDL2__
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#endif

#ifdef __LIBRETRO__
#include "libretro.h"
extern retro_log_printf_t log_cb;
#else
extern FILE * logDebug;
#endif

void log_error(const char *fmt, ...);
void log_debug(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_debug2(const char *fmt, ...);

const char* log_spaces(int n);

#if DEBUG==2
    #define R(a) {log_debug("l:%s%d:%s\n",_state->_str,__LINE__,#a);}; a
#elif DEBUG>=3
    #define R(a) {log_debug("l:%s%x:%d:%s eax: %x ebx: %x ecx: %x edx: %x ebp: %x ds: %x esi: %x es: %x edi: %x fs: %x esp: %x\n",_state->_str,0/*pthread_self()*/,__LINE__,#a, \
eax, ebx, ecx, edx, ebp, ds, esi, es, edi, fs, esp);} \
	a 
#else
    #define R(a) a
#endif

bool is_little_endian();

#if defined(_MSC_VER)
 #define SWAP16 _byteswap_ushort
 #define SWAP32 _byteswap_ulong
#else
 #define SWAP16(x) ((uint16_t)(                  \
			   (((uint16_t)(x) & 0x00ff) << 8)      | \
			   (((uint16_t)(x) & 0xff00) >> 8)        \
			   ))
 #define SWAP32(x) ((uint32_t)(           \
			   (((uint32_t)(x) & 0x000000ff) << 24) | \
			   (((uint32_t)(x) & 0x0000ff00) <<  8) | \
			   (((uint32_t)(x) & 0x00ff0000) >>  8) | \
			   (((uint32_t)(x) & 0xff000000) >> 24)   \
			   ))
#endif

typedef unsigned short _offsets;
/*
#ifndef __BORLANDC__
enum  _offsets : int;
#else
enum  _offsets;
#endif
*/

struct _STATE{
dd eax;
dd ebx;
dd ecx;
dd edx;
dd esi;
dd edi;
dd esp;
dd ebp;
dd eip;

dw cs;         
dw ds;         
dw es;         
dw fs;         
dw gs;         
dw ss;         
                      
bool CF;       
bool PF;       
bool AF;       
bool ZF;       
bool SF;       
bool DF;       
bool OF;       
db _indent; 
const char *_str;
};

/*
#define eax (_state->_eax)
#define ax  (*(dw*)&(_state->_eax))
#define al  (*(db*)&(_state->_eax))
#define ah  (*((db*)&(_state->_eax)+1))

#define ebx (_state->_ebx)
#define bx  (*(dw*)&(_state->_ebx))
#define bl  (*(db*)&(_state->_ebx))
#define bh  (*((db*)&(_state->_ebx)+1))

#define ecx (_state->_ecx)
#define cx  (*(dw*)&(_state->_ecx))
#define cl  (*(db*)&(_state->_ecx))
#define ch  (*((db*)&(_state->_ecx) +1))

#define edx (_state->_edx)
#define dx  (*(dw*)&(_state->_edx))
#define dl  (*(db*)&(_state->_edx))
#define dh  (*((db*)&(_state->_edx)+1))

#define esi (_state->_esi)
#define si  (*(dw*)&(_state->_esi))
#define sil (*(db*)&(_state->_esi))

#define edi (_state->_edi)
#define di  (*(dw*)&(_state->_edi))
#define dil (*(db*)&(_state->_edi))

#define ebp (_state->_ebp)
#define bp  (*(dw*)&(_state->_ebp))
#define bpl (*(db*)&(_state->_ebp))

#define esp (_state->_esp)
#define sp  (*(dw*)&(_state->_esp))
#define spl (*(db*)&(_state->_esp))

#define eip (_state->_eip)
#define ip  (*(dw*)&(_state->_eip))

#define cs  (_state->_cs)
#define ds  (_state->_ds)
#define es  (_state->_es)
#define fs  (_state->_fs)
#define gs  (_state->_gs)
#define ss  (_state->_ss)
                      
#define CF  (_state->_CF)
#define PF  (_state->_PF)
#define AF  (_state->_AF)
#define ZF  (_state->_ZF)
#define SF  (_state->_SF)
#define DF  (_state->_DF)
#define OF  (_state->_OF)
#define stackPointer (_state->_esp)

*/
#define REGDEF_hl(Z)   \
uint32_t& e##Z##x = _state->e##Z##x; \
uint16_t& Z##x = *(uint16_t *)& e##Z##x; \
uint8_t& Z##l = *(uint8_t *)& e##Z##x; \
uint8_t& Z##h = *(((uint8_t *)& e##Z##x)+1); 

#define REGDEF_l(Z) \
uint32_t& e##Z = _state->e##Z; \
uint16_t& Z = *(uint16_t *)& e##Z ; \
uint8_t&  Z##l = *(uint8_t *)& e##Z ;

#define REGDEF_nol(Z) \
uint32_t& e##Z = _state->e##Z; \
uint16_t& Z = *(uint16_t *)& e##Z ;

#define X86_REGREF \
    REGDEF_hl(a);     \
    REGDEF_hl(b);     \
    REGDEF_hl(c);     \
    REGDEF_hl(d);     \
                      \
    REGDEF_l(si);     \
    REGDEF_l(di);     \
    REGDEF_l(sp);     \
    REGDEF_l(bp);     \
                      \
    REGDEF_nol(ip);   \
                      \
dw& cs = _state->cs;         \
dw& ds = _state->ds;         \
dw& es = _state->es;         \
dw& fs = _state->fs;         \
dw& gs = _state->gs;         \
dw& ss = _state->ss;         \
                      \
bool& CF = _state->CF;       \
bool& PF = _state->PF;       \
bool& AF = _state->AF;       \
bool& ZF = _state->ZF;       \
bool& SF = _state->SF;       \
bool& DF = _state->DF;       \
bool& OF = _state->OF;       \
dd& stackPointer = _state->esp;\
_offsets __disp; \
dw _source;

/*
#define X86_REGREF \
dw __disp; \
dw _source;
*/
void stackDump(struct _STATE* state);
void hexDump (void *addr, int len);
void asm2C_INT(struct _STATE* state, int a);
void asm2C_init();
void asm2C_printOffsets(unsigned int offset);

void realtocurs();
dw getscan();

// SDL2 VGA
//struct SDL_Renderer;
extern struct SDL_Renderer *renderer;

extern db vgaPalette[256*3];

//extern chtype vga_to_curses[256];
#include "memmgr.h"

#ifdef __cplusplus
//}
#endif

// ---------unimplemented
#define CMPXCHG8B(a) log_debug("unimplemented\n"); // not in dosbox
#define CMOVA(a,b) log_debug("unimplemented\n"); // not in dosbox
#define CMOVB(a,b) log_debug("unimplemented\n");
#define CMOVBE(a,b) log_debug("unimplemented\n");
#define CMOVG(a,b) log_debug("unimplemented\n");
#define CMOVGE(a,b) log_debug("unimplemented\n");
#define CMOVL(a,b) log_debug("unimplemented\n");
#define CMOVLE(a,b) log_debug("unimplemented\n");
#define CMOVNB(a,b) log_debug("unimplemented\n");
#define CMOVNO(a,b) log_debug("unimplemented\n");
#define CMOVNP(a,b) log_debug("unimplemented\n");
#define CMOVNS(a,b) log_debug("unimplemented\n");
#define CMOVNZ(a,b) log_debug("unimplemented\n");
#define CMOVO(a,b) log_debug("unimplemented\n");
#define CMOVP(a,b) log_debug("unimplemented\n");
#define CMOVS(a,b) log_debug("unimplemented\n");
#define CMOVZ(a,b) log_debug("unimplemented\n");

#define JNO(x) log_debug("unimplemented\n");
#define JNP(x) log_debug("unimplemented\n");
#define JO(x) log_debug("unimplemented\n");
#define JP(x) log_debug("unimplemented\n");

#define AAA log_debug("unimplemented\n");
#define AAM log_debug("unimplemented\n");
#define AAS log_debug("unimplemented\n");
#define BSR(a,b) log_debug("unimplemented\n");
#define BSWAP(a) log_debug("unimplemented\n");
#define CDQ log_debug("unimplemented\n");
#define CMPSD log_debug("unimplemented\n");
#define CMPSW log_debug("unimplemented\n");
#define CMPXCHG log_debug("unimplemented\n");
#define DAA log_debug("unimplemented\n");
#define DAS log_debug("unimplemented\n");
#define SCASD log_debug("unimplemented\n");
#define SCASW log_debug("unimplemented\n");
//#define SHLD(a,b,c) log_debug("unimplemented\n");
#define XADD(a,b) log_debug("unimplemented\n");

#define LOOPW(x) log_debug("unimplemented\n");
#define LOOPWE(x) log_debug("unimplemented\n");
#define LOOPWNE(x) log_debug("unimplemented\n");
// ---------

#endif

