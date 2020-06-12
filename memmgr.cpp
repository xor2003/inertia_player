/****************************************************************/
/*                                                              */
/*                          memmgr.c                            */
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

#include "memmgr.h"
#include "error.h"

#include "asm.h"

//struct Memory;
//extern Memory m;

#ifdef VERSION_STRING
static BYTE *memmgrRcsId =
    "$Id: memmgr.c 1338 2007-07-20 20:52:33Z mceric $";
#endif


uint16_t first_mcb; //=FP_SEG(&m.heap);         /* Start of user memory                 */
db mem_access_mode=0;   /* memory allocation scheme             */

typedef mcb * mcb_p;

/*
 * Join any following unused MCBs to MCB 'p'.
 *  Return:
 *  SUCCESS: on success
 *  else: error number <<currently DE_MCBDESTRY only>>
 */
static int joinMCBs(seg para)
{
  mcb_p p = para2far(para);
  mcb_p q;

  /* loop as long as the current MCB is not the last one in the chain
     and the next MCB is unused */
  while (p->m_type == MCB_NORMAL)
  {
    q = nxtMCB(p);
    if (!mcbFree(q))
      break;
    if (!mcbValid(q))
      return DE_MCBDESTRY;
    /* join both MCBs */
    p->m_type = q->m_type;      /* possibly the next MCB is the last one */
    p->m_size += q->m_size + 1; /* one for q's MCB itself */
#if 0				/* this spoils QB4's habit to double-free: */
    q->m_type = 'K';            /* Invalidate the magic number (whole MCB) */
#else
    q->m_size = 0xffff;		/* mark the now unlinked MCB as "fake" */
#endif
  }

  return SUCCESS;
}


/* Allocate a new memory area. *para is assigned to the segment of the
   MCB rather then the segment of the data portion */
/* If mode == LARGEST, asize MUST be != NULL and will always recieve the
   largest available block, which is allocated.
   If mode != LARGEST, asize maybe NULL, but if not, it is assigned to the
   size of the largest available block only on failure.
   size is the minimum size of the block to search for,
   even if mode == LARGEST.
 */
int DosMemAlloc(UWORD size, int mode, seg *para, UWORD *asize)
{
   mcb_p p;
  mcb_p foundSeg;
  mcb_p biggestSeg;
  /* Initialize                                           */

searchAgain:

  p = para2far(first_mcb);

  biggestSeg = foundSeg = NULL;
/*
    Hack to the Umb Region direct for now. Save time and program space.

  if ((uppermem_link & 1) && uppermem_root != 0xffff)
  {
    int tmpmode = (mode == LARGEST ? mem_access_mode : mode);
    if ((mode != LARGEST || size == 0xffff) &&
        (tmpmode & (FIRST_FIT_UO | FIRST_FIT_U)))
      p = para2far(uppermem_root);
  }
*/
  /* Search through memory blocks                         */
  while (1)
  {
    /* check for corruption                         */
    if (!mcbValid(p))
      return DE_MCBDESTRY;

    if (mcbFree(p))
    {                           /* unused block, check if it applies to the rule */
      if (joinMCBs(FP_SEG(p)) != SUCCESS)       /* join following unused blocks */
        return DE_MCBDESTRY;    /* error */

      if (!biggestSeg || biggestSeg->m_size < p->m_size)
        biggestSeg = p;

      if (p->m_size >= size)
      {                         /* if the block is too small, ignore */
        /* this block has a "match" size, try the rule set */
        switch (mode)
        {
          case LAST_FIT:       /* search for last possible */
          case LAST_FIT_U:
          case LAST_FIT_UO:
          default:
            foundSeg = p;
            break;

          case LARGEST:        /* grab the biggest block */
            /* it is calculated when the MCB chain
               was completely checked */
            break;

          case BEST_FIT:       /* first, but smallest block */
          case BEST_FIT_U:
          case BEST_FIT_UO:
            if (!foundSeg || foundSeg->m_size > p->m_size)
              /* better match found */
              foundSeg = p;
            break;

          case FIRST_FIT:      /* first possible */
          case FIRST_FIT_U:
          case FIRST_FIT_UO:
            foundSeg = p;
            goto stopIt;        /* OK, rest of chain can be ignored */

        }
      }
    }

    if (p->m_type == MCB_LAST)
      break;                    /* end of chain reached */

    p = nxtMCB(p);              /* advance to next MCB */
  }

  if (mode == LARGEST && biggestSeg && biggestSeg->m_size >= size)
    *asize = (foundSeg = biggestSeg)->m_size;

  if (!foundSeg || !foundSeg->m_size)
  {                             /* no block to fullfill the request */
    if ((mode != LARGEST) && (mode & FIRST_FIT_U))
    {
      mode &= ~FIRST_FIT_U;
      goto searchAgain;
    }
    if (asize)
      *asize = biggestSeg ? biggestSeg->m_size : 0;
    return DE_NOMEM;
  }

stopIt:                        /* reached from FIRST_FIT on match */

  if (mode != LARGEST && size != foundSeg->m_size)
  {
    /* Split the found buffer because it is larger than requested */
    /* foundSeg := pointer to allocated block
       p := pointer to MCB that will form the rest of the block
     */
    if ((mode == LAST_FIT) || (mode == LAST_FIT_UO)
        || (mode == LAST_FIT_U))
    {
      /* allocate the block from the end of the found block */
      p = foundSeg;
      p->m_size -= size + 1;    /* size+1 paragraphes are allocated by
                                   the new segment (+1 for MCB itself) */
      foundSeg = nxtMCB(p);

      /* initialize stuff because foundSeg > p */
      foundSeg->m_type = p->m_type;
      p->m_type = MCB_NORMAL;
    }
    else
    {                           /* all other modes allocate from the beginning */
      p = nxtMCBsize(foundSeg, size);

      /* initialize stuff because p > foundSeg  */
      p->m_type = foundSeg->m_type;
      p->m_size = foundSeg->m_size - size - 1;
      foundSeg->m_type = MCB_NORMAL;
    }

    /* Already initialized:
       p->m_size, ->m_type, foundSeg->m_type
     */
    p->m_psp = FREE_PSP;        /* unused */

    foundSeg->m_size = size;
  }

  /* Already initialized:
     foundSeg->m_size, ->m_type
   */
  foundSeg->m_psp = 1; //cu_psp;     /* the new block is for current process */
  foundSeg->m_name[0] = '\0';

memset(((char*)foundSeg)+16,0,(foundSeg->m_size)*16);
  *para = FP_SEG(foundSeg);
  return SUCCESS;
}

/*
 * Unlike the name and the original prototype could suggest, this function
 * is used to return the _size_ of the largest available block rather than
 * the block itself.
 *
 * Known bug: a memory area with a size of the data area of 0 (zero) is
 * not considered a "largest" block. <<Perhaps this is a feature ;-)>>
 */
int DosMemLargest(UWORD *size)
{
  seg dummy;
  *size = 0;
  DosMemAlloc(0xffff, LARGEST, &dummy, size);
  if (mem_access_mode & 0x80) /* then the largest block is probably low! */
  {
    UWORD lowsize = 0;
    mem_access_mode &= ~0x80;
    DosMemAlloc(0xffff, LARGEST, &dummy, &lowsize);
    mem_access_mode |= 0x80;
    if (lowsize > *size)
      *size = lowsize;
  }
  return *size ? SUCCESS : DE_NOMEM;
}

/*
 * Deallocate a memory block. para is the segment of the MCB itself
 * This function can be called with para == 0, which eases other parts
 * of the kernel.
 */
int DosMemFree(UWORD para)
{
   mcb_p p;

  if (!para)                    /* let esp. the kernel call this fct with para==0 */
    return DE_INVLDMCB;

  /* Initialize                                           */
  p = para2far(para);

  /* check for corruption                         */
  if (!mcbFreeable(p))	/* does not have to be valid, freeable is enough */
    return DE_INVLDMCB;

  /* Mark the mcb as free so that we can later    */
  /* merge with other surrounding free MCBs       */
  p->m_psp = FREE_PSP;
//  memset(p->m_name, '\0', 8);

  return SUCCESS;
}

/*
 * Resize an allocated memory block.
 * para is the segment of the data portion of the block rather than
 * the segment of the MCB itself.
 *
 * If the block shall grow, it is resized to the maximal size less than
 * or equal to size. This is the way MS DOS is reported to work.
 */
int DosMemChange(UWORD para, UWORD size, UWORD * maxSize)
{
   mcb_p p, q;

  /* Initialize                                                   */
  p = para2far(para - 1);       /* pointer to MCB */

  /* check for corruption                                         */
  if (!mcbValid(p))
    return DE_MCBDESTRY;

  /* check if to grow the block                                   */
  if (size > p->m_size)
  {
    /* first try to make the MCB larger by joining with any following
       unused blocks */
    if (joinMCBs(FP_SEG(p)) != SUCCESS)
      return DE_MCBDESTRY;

    if (size > p->m_size)
    {                           /* block is still too small */
      if (maxSize)
        *maxSize = p->m_size;
      return DE_NOMEM;
    }
  }

  /*       shrink it down                                         */
  if (size < p->m_size)
  {
    /* make q a pointer to the new next block               */
    q = nxtMCBsize(p, size);
    /* reduce the size of p and add difference to q         */
    q->m_size = p->m_size - size - 1;
    q->m_type = p->m_type;

    p->m_size = size;

    /* Make certian the old psp is not last (if it was)     */
    p->m_type = MCB_NORMAL;

    /* Mark the mcb as free so that we can later    */
    /* merge with other surrounding free MCBs       */
    q->m_psp = FREE_PSP;
//    memset(q->m_name, '\0', 8);

    /* try to join q with the free MCBs following it if possible */
    if (joinMCBs(FP_SEG(q)) != SUCCESS)
      return DE_MCBDESTRY;
  }

  /* MS network client NET.EXE: DosMemChange sets the PSP              *
   *               not tested, if always, or only on success         TE*
   * only on success seems more logical to me - Bart                                                                                                                   */
  p->m_psp = 1;//cu_psp;

  return SUCCESS;
}

/*
 * Check the MCB chain for allocation corruption
 */
int DosMemCheck(void)
{
   mcb_p p;
   mcb_p pprev = 0;

  /* Initialize                                           */
  p = para2far(first_mcb);

  /* Search through memory blocks                         */
  while (p->m_type != MCB_LAST) /* not all MCBs touched */
  {
    /* check for corruption                         */
    if (p->m_type != MCB_NORMAL)
    {
      log_error("dos mem corrupt, first_mcb=%x \nprev %x notMZ %x",first_mcb, pprev, p);
      return DE_MCBDESTRY;
    }

    /* not corrupted - but not end, bump the pointer */
    pprev = p;
    p = nxtMCB(p);
  }
  return SUCCESS;
}


#ifdef DEBUG3
void show_chain(void)
{
  mcb_p p;
  p = para2far(first_mcb);

  for (;;)
  {
    mcb_print(p);
    if (p->m_type == MCB_LAST || p->m_type != MCB_NORMAL)
      return;
    else
      p = nxtMCB(p);
  }
}

void mcb_print(mcb_p mcbp)
{
  static BYTE buff[9];

  memcpy(buff, mcbp->m_name, 8);
  buff[8] = '\0';
/*
  printf
      ("%04x:%04x -> |%s| m_type = 0x%02x '%c'; m_psp = 0x%04x; m_size = 0x%04x\n",
       FP_SEG(mcbp), FP_OFF(mcbp), *buff == '\0' ? "*NO-ID*" : buff,
       mcbp->m_type, mcbp->m_type > ' ' ? mcbp->m_type : ' ', mcbp->m_psp,
       mcbp->m_size);
*/
}
#endif

static void mcb_init_copy(dw seg, dw size, mcb *near_mcb)
{
  near_mcb->m_size = size;
  memcpy(para2far(seg), near_mcb, sizeof(mcb));
}

void mcb_init(dw seg, dw size, BYTE type)
{
  static mcb near_mcb;
  first_mcb = seg;
  near_mcb.m_type = type;
  mcb_init_copy(seg, size, &near_mcb);
}
