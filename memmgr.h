/****************************************************************/
/*                                                              */
/*                          memmgr.h                            */
/*                                                              */
/*               Memory Manager for Core Allocation             */
/*                                                              */
/*                      Copyright (c) 1995                      */
/*                      Pasquale J. Villani                     */
/*                      All Rights Reserved                     */
/*                                                              */
/* This file is part of DOS-C.                                  */
/*                                                              */
/* DOS-C is free software; you can redistribute it and/or       */
/* modify it under the terms of the GNU General Public License  */
/* as published by the Free Software Foundation; either version */
/* 2, or (at your option) any later version.                    */
/*                                                              */
/* DOS-C is distributed in the hope that it will be useful, but */
/* WITHOUT ANY WARRANTY; without even the implied warranty of   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See    */
/* the GNU General Public License for more details.             */
/*                                                              */
/* You should have received a copy of the GNU General Public    */
/* License along with DOS-C; see the file COPYING.  If not,     */
/* write to the Free Software Foundation, 675 Mass Ave,         */
/* Cambridge, MA 02139, USA.                                    */
/****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "mcb.h"

typedef uint8_t db;
typedef uint16_t dw;

struct Memory;
extern Memory m;


//#define MK_FP(seg,ofs)      (&(raddr(seg,ofs))) //  ( (ULONG(seg)<<4)+(UWORD)(ofs) )
#define FP_SEG(fp)           ( (  ((uint8_t*)(fp)) - ((uint8_t*)(&m)) )>>4)// ( (UWORD) ((fp)>>4) )
//#define FP_OFF(fp)            ( (UWORD) (fp) )
#define SUCCESS 0
//#define NULL 0

extern uint16_t first_mcb; //=FP_SEG(&m.heap);         /* Start of user memory                 */
extern db mem_access_mode;

#define nxtMCBsize(mcbp,size) ((mcb*)(((db*)mcbp) + (size<<4) + 0x10))
#define nxtMCB(mcbp) nxtMCBsize((mcbp), (mcbp)->m_size)

#define mcbFree(mcbp) ((mcbp)->m_psp == FREE_PSP)
#define mcbValid(mcbp)	( ((mcbp)->m_size != 0xffff) && \
        ((mcbp)->m_type == MCB_NORMAL || (mcbp)->m_type == MCB_LAST) )
#define mcbFreeable(mcbp) \
	((mcbp)->m_type == MCB_NORMAL || (mcbp)->m_type == MCB_LAST)

#define para2far(seg) ( (mcb *)((db *)&m + ((seg)<<4)) ) //(seg<<4)

typedef mcb * mcb_p;

/*
 * Join any following unused MCBs to MCB 'p'.
 *  Return:
 *  SUCCESS: on success
 *  else: error number <<currently DE_MCBDESTRY only>>
 */
static int joinMCBs(seg para);

/* Allocate a new memory area. *para is assigned to the segment of the
   MCB rather then the segment of the data portion */
/* If mode == LARGEST, asize MUST be != NULL and will always recieve the
   largest available block, which is allocated.
   If mode != LARGEST, asize maybe NULL, but if not, it is assigned to the
   size of the largest available block only on failure.
   size is the minimum size of the block to search for,
   even if mode == LARGEST.
 */
int DosMemAlloc(UWORD size, int mode, seg *para, UWORD *asize);

/*
 * Unlike the name and the original prototype could suggest, this function
 * is used to return the _size_ of the largest available block rather than
 * the block itself.
 *
 * Known bug: a memory area with a size of the data area of 0 (zero) is
 * not considered a "largest" block. <<Perhaps this is a feature ;-)>>
 */
int DosMemLargest(UWORD *size);

/*
 * Deallocate a memory block. para is the segment of the MCB itself
 * This function can be called with para == 0, which eases other parts
 * of the kernel.
 */
int DosMemFree(UWORD para);

/*
 * Resize an allocated memory block.
 * para is the segment of the data portion of the block rather than
 * the segment of the MCB itself.
 *
 * If the block shall grow, it is resized to the maximal size less than
 * or equal to size. This is the way MS DOS is reported to work.
 */
int DosMemChange(UWORD para, UWORD size, UWORD * maxSize);

/*
 * Check the MCB chain for allocation corruption
 */
int DosMemCheck(void);


#ifdef DEBUG3
VOID show_chain(void);

VOID mcb_print(mcb_p mcbp);
#endif

static void mcb_init_copy(dw seg, dw size, mcb *near_mcb);

void mcb_init(dw seg, dw size, BYTE type);

#ifdef __cplusplus
}
#endif
