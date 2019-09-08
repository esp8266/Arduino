/* ----------------------------------------------------------------------------
 * umm_malloc.c - a memory allocator for embedded systems (microcontrollers)
 *
 * See LICENSE for copyright notice
 * See README.md for acknowledgements and description of internals
 * ----------------------------------------------------------------------------
 *
 * R.Hempel 2007-09-22 - Original
 * R.Hempel 2008-12-11 - Added MIT License biolerplate
 *                     - realloc() now looks to see if previous block is free
 *                     - made common operations functions
 * R.Hempel 2009-03-02 - Added macros to disable tasking
 *                     - Added function to dump heap and check for valid free
 *                        pointer
 * R.Hempel 2009-03-09 - Changed name to umm_malloc to avoid conflicts with
 *                        the mm_malloc() library functions
 *                     - Added some test code to assimilate a free block
 *                        with the very block if possible. Complicated and
 *                        not worth the grief.
 * D.Frank 2014-04-02  - Fixed heap configuration when UMM_TEST_MAIN is NOT set,
 *                        added user-dependent configuration file umm_malloc_cfg.h
 * R.Hempel 2016-12-04 - Add support for Unity test framework
 *                     - Reorganize source files to avoid redundant content
 *                     - Move integrity and poison checking to separate file
 * R.Hempel 2017-12-29 - Fix bug in realloc when requesting a new block that
 *                        results in OOM error - see Issue 11
 * ----------------------------------------------------------------------------
 */

 /*****************************************************************************
  *
  * A log listing changes made for local adaptation of newer upstream
  * umm_malloc - started July 22, 2019.
  *
  * AUG 1, 2019 compiles with OOM, UMM_POISON_CHECK, and appears to be working.
  *
  * In umm_malloc.c
  *   Renamed to umm_malloc.cpp
  *
  *   Added `extern "C" { ...b};` around code.

  *   Surround DBGLOG_LEVEL with #ifndef... Now defined value in umm_malloc_cfg.h
  *
  *   umm_free() - moved critical section to start after safe calculations.
  *
  *   umm_malloc() - moved critical section to start after umm_blocks()
  *   computations are based on constants that don't change, calling
  *   argument excepted.
  *
  *   umm_realloc() - Added UMM_CRITICAL_SUSPEND()/UMM_CRITICAL_RESUME() for
  *   when lightweight locks are available. eg. sti/cli. Single threaded
  *   single CPU case.
  *
  *   umm_realloc() - appears to have been refactored to minimize memmove
  *   and memcpy. The old version would always combine an adjacent block in the
  *   direction of the start of the heap when available and do a memmove. This
  *   had a defragging effect. This appears to have been replaced with an
  *   attempt to minimize copy when possible.
  *
  *   Added heap stats tracking.
  *
  *
  * In umm_info.c
  *   umm_info() - Added UMM_CRITICAL_DECL(id_info), updated critical sections
  *   with tag.
  *   Carried forward: Added NULL ptr check at beginning (umm_malloc.c). - Mar 8, 2016
  *
  *
  * In umm_poison.c:
  *   Resolved C++ compiler error reported on get_poisoned(), and
  *   get_unpoisoned(). They now take in void * arg instead of unsigned char *.
  *   Added  #if ... || defined(UMM_POISON_CHECK_LITE) to the conditional.
  *
  * In umm_integrity.c:
  *   Replaced printf with DBGLOG_FUNCTION. This needs to be a malloc free
  *   function and ISR safe.
  *   Added critical sections.
  *
  * In umm_malloc_cfg.h:
  *   Added macro UMM_CRITICAL_SUSPEND()/UMM_CRITICAL_RESUME()
  *
  *
  * Globally change across all files %i to %d: umm_info.c, umm_malloc.c,
  *
  *
  * Notes,
  *
  *   umm_integrity_check() is called by macro INTEGRITY_CHECK which returns 1
  *   on success. No corruption. Does a time consuming scan of the whole heap.
  *   It will call UMM_HEAP_CORRUPTION_CB if an error is found.
  *
  *   umm_poison_check(), formerly known as check_poison_all_blocks(),
  *   is called by macro POISON_CHECK which returns 1 on success for no
  *   corruption. Does a time consuming scan of all active allocations for
  *   modified poison. The new upstream version does *NOT* call
  *   UMM_HEAP_CORRUPTION_CB if an  error is found. The option description says
  *   it does!
  *
  *   umm_poison_realloc() and umm_poison_free() no longer call the macro
  *   UMM_HEAP_CORRUPTION_CB on poison error. Just a printf message is
  *   generated. I have added alternative functions umm_poison_free_fl,
  *   umm_poison_realloc_fl, and get_unpoisoned_check_neighbors in
  *   umm_local.cpp. These expand the poison check on the current allocation to
  *   include its nearest allocated neighbors in the heap.
  *
  *   umm_malloc() has been extended to call check_poison_neighbors for the
  *   allocation it selects, conditionally for UMM_POISON_CHECK_LITE.
  *
  *   For upstream umm_malloc "#  define POISON_CHECK() 0" should have been 1
  *   add to list to report.
  */
 /*
  * Current Deltas from the old umm_malloc
  *
  *   umm_posion check for a given *alloc - failure - no longer panics.
  *
  *   option to run full poison check at each *alloc call, not present
  *
  *   option to run full interity check at each *alloc call, not present
  *
  *   upstream code does not call panic from poison_check_block.
  *
  *   Defragmenting effect of realloc is gone. It now minimizes copy. This
  *   may have been an accident during code cleanup.
  *
  */


/*
 * Added for using with Arduino ESP8266 and handling renameing to umm_malloc.cpp
 */

#define BUILD_UMM_MALLOC_C

extern "C" {

#include <stdio.h>
#include <string.h>

#include "umm_malloc.h"

#include "umm_malloc_cfg.h"   /* user-dependent */

/* Use the default DBGLOG_LEVEL and DBGLOG_FUNCTION */

#ifndef DBGLOG_LEVEL
#define DBGLOG_LEVEL 0
#endif

#include "dbglog/dbglog.h"

#include "umm_local.h"      // target-dependent supplemental

/* ------------------------------------------------------------------------- */

UMM_H_ATTPACKPRE typedef struct umm_ptr_t {
  unsigned short int next;
  unsigned short int prev;
} UMM_H_ATTPACKSUF umm_ptr;


UMM_H_ATTPACKPRE typedef struct umm_block_t {
  union {
    umm_ptr used;
  } header;
  union {
    umm_ptr free;
    unsigned char data[4];
  } body;
} UMM_H_ATTPACKSUF umm_block;

#define UMM_FREELIST_MASK (0x8000)
#define UMM_BLOCKNO_MASK  (0x7FFF)

/* ------------------------------------------------------------------------- */

umm_block *umm_heap = NULL;
unsigned short int umm_numblocks = 0;

#define UMM_NUMBLOCKS (umm_numblocks)

/* ------------------------------------------------------------------------ */

#define UMM_BLOCK(b)  (umm_heap[b])

#define UMM_NBLOCK(b) (UMM_BLOCK(b).header.used.next)
#define UMM_PBLOCK(b) (UMM_BLOCK(b).header.used.prev)
#define UMM_NFREE(b)  (UMM_BLOCK(b).body.free.next)
#define UMM_PFREE(b)  (UMM_BLOCK(b).body.free.prev)
#define UMM_DATA(b)   (UMM_BLOCK(b).body.data)

/* -------------------------------------------------------------------------
 * There are additional files that may be included here - normally it's
 * not a good idea to include .c files but in this case it keeps the
 * main umm_malloc file clear and prevents issues with exposing internal
 * data structures to other programs.
 * -------------------------------------------------------------------------
 */

#include "umm_integrity.c"
#include "umm_poison.c"
#include "umm_info.c"
#include "umm_local.c"      // target-dependent supplemental features

/* ------------------------------------------------------------------------ */

static unsigned short int umm_blocks( size_t size ) {

  /*
   * The calculation of the block size is not too difficult, but there are
   * a few little things that we need to be mindful of.
   *
   * When a block removed from the free list, the space used by the free
   * pointers is available for data. That's what the first calculation
   * of size is doing.
   */

  if( size <= (sizeof(((umm_block *)0)->body)) )
    return( 1 );

  /*
   * If it's for more than that, then we need to figure out the number of
   * additional whole blocks the size of an umm_block are required.
   */

  size -= ( 1 + (sizeof(((umm_block *)0)->body)) );

  return( 2 + size/(sizeof(umm_block)) );
}

/* ------------------------------------------------------------------------ */
/*
 * Split the block `c` into two blocks: `c` and `c + blocks`.
 *
 * - `new_freemask` should be `0` if `c + blocks` used, or `UMM_FREELIST_MASK`
 *   otherwise.
 *
 * Note that free pointers are NOT modified by this function.
 */
static void umm_split_block( unsigned short int c,
    unsigned short int blocks,
    unsigned short int new_freemask ) {

  UMM_NBLOCK(c+blocks) = (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) | new_freemask;
  UMM_PBLOCK(c+blocks) = c;

  UMM_PBLOCK(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) = (c+blocks);
  UMM_NBLOCK(c)                                = (c+blocks);
}

/* ------------------------------------------------------------------------ */

static void umm_disconnect_from_free_list( unsigned short int c ) {
  /* Disconnect this block from the FREE list */

  UMM_NFREE(UMM_PFREE(c)) = UMM_NFREE(c);
  UMM_PFREE(UMM_NFREE(c)) = UMM_PFREE(c);

  /* And clear the free block indicator */

  UMM_NBLOCK(c) &= (~UMM_FREELIST_MASK);
}

/* ------------------------------------------------------------------------
 * The umm_assimilate_up() function assumes that UMM_NBLOCK(c) does NOT
 * have the UMM_FREELIST_MASK bit set!
 */

static void umm_assimilate_up( unsigned short int c ) {

  if( UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_FREELIST_MASK ) {
    /*
     * The next block is a free block, so assimilate up and remove it from
     * the free list
     */

    DBGLOG_DEBUG( "Assimilate up to next block, which is FREE\n" );

    /* Disconnect the next block from the FREE list */

    umm_disconnect_from_free_list( UMM_NBLOCK(c) );

    /* Assimilate the next block with this one */

    UMM_PBLOCK(UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_BLOCKNO_MASK) = c;
    UMM_NBLOCK(c) = UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_BLOCKNO_MASK;
  }
}

/* ------------------------------------------------------------------------
 * The umm_assimilate_down() function assumes that UMM_NBLOCK(c) does NOT
 * have the UMM_FREELIST_MASK bit set!
 */

static unsigned short int umm_assimilate_down( unsigned short int c, unsigned short int freemask ) {

  UMM_NBLOCK(UMM_PBLOCK(c)) = UMM_NBLOCK(c) | freemask;
  UMM_PBLOCK(UMM_NBLOCK(c)) = UMM_PBLOCK(c);

  return( UMM_PBLOCK(c) );
}

/* ------------------------------------------------------------------------- */

void umm_init( void ) {
  /* init heap pointer and size, and memset it to 0 */
  umm_heap = (umm_block *)UMM_MALLOC_CFG_HEAP_ADDR;
  umm_numblocks = (UMM_MALLOC_CFG_HEAP_SIZE / sizeof(umm_block));
  memset(umm_heap, 0x00, UMM_MALLOC_CFG_HEAP_SIZE);

  /* setup initial blank heap structure */
  {
    /* index of the 0th `umm_block` */
    const unsigned short int block_0th = 0;
    /* index of the 1st `umm_block` */
    const unsigned short int block_1th = 1;
    /* index of the latest `umm_block` */
    const unsigned short int block_last = UMM_NUMBLOCKS - 1;

    /* init ummStats.free_blocks */
#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
#if defined(UMM_STATS_FULL)
    ummStats.free_blocks_min =
    ummStats.free_blocks_isr_min  =
#endif
    ummStats.free_blocks = block_last;
#endif

    /* setup the 0th `umm_block`, which just points to the 1st */
    UMM_NBLOCK(block_0th) = block_1th;
    UMM_NFREE(block_0th)  = block_1th;
    UMM_PFREE(block_0th)  = block_1th;

    /*
     * Now, we need to set the whole heap space as a huge free block. We should
     * not touch the 0th `umm_block`, since it's special: the 0th `umm_block`
     * is the head of the free block list. It's a part of the heap invariant.
     *
     * See the detailed explanation at the beginning of the file.
     */

    /*
     * 1th `umm_block` has pointers:
     *
     * - next `umm_block`: the latest one
     * - prev `umm_block`: the 0th
     *
     * Plus, it's a free `umm_block`, so we need to apply `UMM_FREELIST_MASK`
     *
     * And it's the last free block, so the next free block is 0.
     */
    UMM_NBLOCK(block_1th) = block_last | UMM_FREELIST_MASK;
    UMM_NFREE(block_1th)  = 0;
    UMM_PBLOCK(block_1th) = block_0th;
    UMM_PFREE(block_1th)  = block_0th;

    /*
     * latest `umm_block` has pointers:
     *
     * - next `umm_block`: 0 (meaning, there are no more `umm_blocks`)
     * - prev `umm_block`: the 1st
     *
     * It's not a free block, so we don't touch NFREE / PFREE at all.
     */
    UMM_NBLOCK(block_last) = 0;
    UMM_PBLOCK(block_last) = block_1th;
  }
}

/* ------------------------------------------------------------------------ */

void umm_free( void *ptr ) {
  UMM_CRITICAL_DECL(id_free);

  unsigned short int c;

  /* If we're being asked to free a NULL pointer, well that's just silly! */

  if( (void *)0 == ptr ) {
    DBGLOG_DEBUG( "free a null pointer -> do nothing\n" );
    STATS__NULL_FREE_REQUEST(id_free);

    return;
  }

  STATS__FREE_REQUEST(id_free);
  /*
   * FIXME: At some point it might be a good idea to add a check to make sure
   *        that the pointer we're being asked to free up is actually within
   *        the umm_heap!
   *
   * NOTE:  See the new umm_info() function that you can use to see if a ptr is
   *        on the free list!
   */

  /* Figure out which block we're in. Note the use of truncated division... */

  c = (((char *)ptr)-(char *)(&(umm_heap[0])))/sizeof(umm_block);

  DBGLOG_DEBUG( "Freeing block %6d\n", c );

  /* Protect the critical section... */
  UMM_CRITICAL_ENTRY(id_free);

  /* Update stats Free Block count */
  STATS__FREE_BLOCKS_UPDATE(UMM_NBLOCK(c) - c);

  /* Now let's assimilate this block with the next one if possible. */

  umm_assimilate_up( c );

  /* Then assimilate with the previous block if possible */

  if( UMM_NBLOCK(UMM_PBLOCK(c)) & UMM_FREELIST_MASK ) {

    DBGLOG_DEBUG( "Assimilate down to next block, which is FREE\n" );

    c = umm_assimilate_down(c, UMM_FREELIST_MASK);
  } else {
    /*
     * The previous block is not a free block, so add this one to the head
     * of the free list
     */

    DBGLOG_DEBUG( "Just add to head of free list\n" );

    UMM_PFREE(UMM_NFREE(0)) = c;
    UMM_NFREE(c)            = UMM_NFREE(0);
    UMM_PFREE(c)            = 0;
    UMM_NFREE(0)            = c;

    UMM_NBLOCK(c)          |= UMM_FREELIST_MASK;
  }

  /* Release the critical section... */
  UMM_CRITICAL_EXIT(id_free);
}

/* ------------------------------------------------------------------------ */

void *umm_malloc( size_t size ) {
  UMM_CRITICAL_DECL(id_malloc);

  unsigned short int blocks;
  unsigned short int blockSize = 0;

  unsigned short int bestSize;
  unsigned short int bestBlock;

  unsigned short int cf;

  if (umm_heap == NULL) {
    umm_init();
  }

  /*
   * the very first thing we do is figure out if we're being asked to allocate
   * a size of 0 - and if we are we'll simply return a null pointer. if not
   * then reduce the size by 1 byte so that the subsequent calculations on
   * the number of blocks to allocate are easier...
   */


  if( 0 == size ) {
    DBGLOG_DEBUG( "malloc a block of 0 bytes -> do nothing\n" );
    STATS__ZERO_ALLOC_REQUEST(id_malloc, size);

    return( (void *)NULL );
  }

  STATS__ALLOC_REQUEST(id_malloc, size);

  blocks = umm_blocks( size );

  /* Protect the critical section... */
  UMM_CRITICAL_ENTRY(id_malloc);

  /*
   * Now we can scan through the free list until we find a space that's big
   * enough to hold the number of blocks we need.
   *
   * This part may be customized to be a best-fit, worst-fit, or first-fit
   * algorithm
   */

  cf = UMM_NFREE(0);

  bestBlock = UMM_NFREE(0);
  bestSize  = 0x7FFF;

  while( cf ) {
    blockSize = (UMM_NBLOCK(cf) & UMM_BLOCKNO_MASK) - cf;

    DBGLOG_TRACE( "Looking at block %6d size %6d\n", cf, blockSize );

#if defined UMM_BEST_FIT
    if( (blockSize >= blocks) && (blockSize < bestSize) ) {
      bestBlock = cf;
      bestSize  = blockSize;
    }
#elif defined UMM_FIRST_FIT
    /* This is the first block that fits! */
    if( (blockSize >= blocks) )
      break;
#else
#  error "No UMM_*_FIT is defined - check umm_malloc_cfg.h"
#endif

    cf = UMM_NFREE(cf);
  }

  if( 0x7FFF != bestSize ) {
    cf        = bestBlock;
    blockSize = bestSize;
  }

  POISON_CHECK_NEIGHBORS(cf);

  if( UMM_NBLOCK(cf) & UMM_BLOCKNO_MASK && blockSize >= blocks ) {
    /*
     * This is an existing block in the memory heap, we just need to split off
     * what we need, unlink it from the free list and mark it as in use, and
     * link the rest of the block back into the freelist as if it was a new
     * block on the free list...
     */

    if( blockSize == blocks ) {
      /* It's an exact fit and we don't neet to split off a block. */
      DBGLOG_DEBUG( "Allocating %6d blocks starting at %6d - exact\n", blocks, cf );

      /* Disconnect this block from the FREE list */

      umm_disconnect_from_free_list( cf );

    } else {
      /* It's not an exact fit and we need to split off a block. */
      DBGLOG_DEBUG( "Allocating %6d blocks starting at %6d - existing\n", blocks, cf );

      /*
       * split current free block `cf` into two blocks. The first one will be
       * returned to user, so it's not free, and the second one will be free.
       */
      umm_split_block( cf, blocks, UMM_FREELIST_MASK /*new block is free*/ );

      /*
       * `umm_split_block()` does not update the free pointers (it affects
       * only free flags), but effectively we've just moved beginning of the
       * free block from `cf` to `cf + blocks`. So we have to adjust pointers
       * to and from adjacent free blocks.
       */

      /* previous free block */
      UMM_NFREE( UMM_PFREE(cf) ) = cf + blocks;
      UMM_PFREE( cf + blocks ) = UMM_PFREE(cf);

      /* next free block */
      UMM_PFREE( UMM_NFREE(cf) ) = cf + blocks;
      UMM_NFREE( cf + blocks ) = UMM_NFREE(cf);
    }

    STATS__FREE_BLOCKS_UPDATE( -blocks );
    STATS__FREE_BLOCKS_MIN();
  } else {
    /* Out of memory */
    STATS__OOM_UPDATE();

    DBGLOG_DEBUG(  "Can't allocate %5d blocks\n", blocks );

    /* Release the critical section... */
    UMM_CRITICAL_EXIT(id_malloc);

    return( (void *)NULL );
  }

  /* Release the critical section... */
  UMM_CRITICAL_EXIT(id_malloc);

  return( (void *)&UMM_DATA(cf) );
}

/* ------------------------------------------------------------------------ */

void *umm_realloc( void *ptr, size_t size ) {
  UMM_CRITICAL_DECL(id_realloc);

  unsigned short int blocks;
  unsigned short int blockSize;
  unsigned short int prevBlockSize = 0;
  unsigned short int nextBlockSize = 0;

  unsigned short int c;

  size_t curSize;

  if (umm_heap == NULL) {
    umm_init();
  }

  /*
   * This code looks after the case of a NULL value for ptr. The ANSI C
   * standard says that if ptr is NULL and size is non-zero, then we've
   * got to work the same a malloc(). If size is also 0, then our version
   * of malloc() returns a NULL pointer, which is OK as far as the ANSI C
   * standard is concerned.
   */

  if( ((void *)NULL == ptr) ) {
    DBGLOG_DEBUG( "realloc the NULL pointer - call malloc()\n" );

    return( umm_malloc(size) );
  }

  /*
   * Now we're sure that we have a non_NULL ptr, but we're not sure what
   * we should do with it. If the size is 0, then the ANSI C standard says that
   * we should operate the same as free.
   */


  if( 0 == size ) {
    DBGLOG_DEBUG( "realloc to 0 size, just free the block\n" );
    STATS__ZERO_ALLOC_REQUEST(id_realloc, size);

    umm_free( ptr );

    return( (void *)NULL );
  }

  STATS__ALLOC_REQUEST(id_realloc, size);

  /*
   * Otherwise we need to actually do a reallocation. A naiive approach
   * would be to malloc() a new block of the correct size, copy the old data
   * to the new block, and then free the old block.
   *
   * While this will work, we end up doing a lot of possibly unnecessary
   * copying. So first, let's figure out how many blocks we'll need.
   */

  blocks = umm_blocks( size );

  /* Figure out which block we're in. Note the use of truncated division... */

  c = (((char *)ptr)-(char *)(&(umm_heap[0])))/sizeof(umm_block);

  /* Figure out how big this block is ... the free bit is not set :-) */

  blockSize = (UMM_NBLOCK(c) - c);

  /* Figure out how many bytes are in this block */

  curSize   = (blockSize*sizeof(umm_block))-(sizeof(((umm_block *)0)->header));

  /* Protect the critical section... */
  UMM_CRITICAL_ENTRY(id_realloc);

  /* Now figure out if the previous and/or next blocks are free as well as
   * their sizes - this will help us to minimize special code later when we
   * decide if it's possible to use the adjacent blocks.
   *
   * We set prevBlockSize and nextBlockSize to non-zero values ONLY if they
   * are free!
   */

  if ((UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_FREELIST_MASK)) {
      nextBlockSize = (UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_BLOCKNO_MASK) - UMM_NBLOCK(c);
  }

  if ((UMM_NBLOCK(UMM_PBLOCK(c)) & UMM_FREELIST_MASK)) {
      prevBlockSize = (c - UMM_PBLOCK(c));
  }

  DBGLOG_DEBUG( "realloc blocks %d blockSize %d nextBlockSize %d prevBlockSize %d\n", blocks, blockSize, nextBlockSize, prevBlockSize );

#if defined(UMM_REALLOC_MINIMIZE_COPY)
  /*
   * Ok, now that we're here we know how many blocks we want and the current
   * blockSize. The prevBlockSize and nextBlockSize are set and we can figure
   * out the best strategy for the new allocation as follows:
   *
   * 1. If the new block is the same size or smaller than the current block do
   *    nothing.
   * 2. If the next block is free and adding it to the current block gives us
   *    enough memory, assimilate the next block.
   * 3. If the prev block is free and adding it to the current block gives us
   *    enough memory, remove the previous block from the free list, assimilate
   *    it, copy to the new block.
   * 4. If the prev and next blocks are free and adding them to the current
   *    block gives us enough memory, assimilate the next block, remove the
   *    previous block from the free list, assimilate it, copy to the new block.
   * 5. Otherwise try to allocate an entirely new block of memory. If the
   *    allocation works free the old block and return the new pointer. If
   *    the allocation fails, return NULL and leave the old block intact.
   *
   * All that's left to do is decide if the fit was exact or not. If the fit
   * was not exact, then split the memory block so that we use only the requested
   * number of blocks and add what's left to the free list.
   */
    if (blockSize >= blocks) {
        DBGLOG_DEBUG( "realloc the same or smaller size block - %i, do nothing\n", blocks );
        /* This space intentionally left blank */
    } else if ((blockSize + nextBlockSize) >= blocks) {
        DBGLOG_DEBUG( "realloc using next block - %i\n", blocks );
        umm_assimilate_up( c );
        STATS__FREE_BLOCKS_UPDATE( - nextBlockSize );
        blockSize += nextBlockSize;
    } else if ((prevBlockSize + blockSize) >= blocks) {
        DBGLOG_DEBUG( "realloc using prev block - %i\n", blocks );
        umm_disconnect_from_free_list( UMM_PBLOCK(c) );
        c = umm_assimilate_down(c, 0);
        STATS__FREE_BLOCKS_UPDATE( - prevBlockSize );
        STATS__FREE_BLOCKS_ISR_MIN();
        blockSize += prevBlockSize;
        UMM_CRITICAL_SUSPEND(id_realloc);
        memmove( (void *)&UMM_DATA(c), ptr, curSize );
        ptr = (void *)&UMM_DATA(c);
        UMM_CRITICAL_RESUME(id_realloc);
    } else if ((prevBlockSize + blockSize + nextBlockSize) >= blocks) {
        DBGLOG_DEBUG( "realloc using prev and next block - %d\n", blocks );
        umm_assimilate_up( c );
        umm_disconnect_from_free_list( UMM_PBLOCK(c) );
        c = umm_assimilate_down(c, 0);
        STATS__FREE_BLOCKS_UPDATE( - prevBlockSize - nextBlockSize );
#ifdef UMM_LIGHTWEIGHT_CPU
        if ((prevBlockSize + blockSize + nextBlockSize) > blocks) {
            umm_split_block( c, blocks, 0 );
            umm_free( (void *)&UMM_DATA(c+blocks) );
        }
        STATS__FREE_BLOCKS_ISR_MIN();
        blockSize = blocks;
#else
        blockSize += (prevBlockSize + nextBlockSize);
#endif
        UMM_CRITICAL_SUSPEND(id_realloc);
        memmove( (void *)&UMM_DATA(c), ptr, curSize );
        ptr = (void *)&UMM_DATA(c);
        UMM_CRITICAL_RESUME(id_realloc);
    } else {
        UMM_CRITICAL_SUSPEND(id_realloc);
        DBGLOG_DEBUG( "realloc a completely new block %i\n", blocks );
        void *oldptr = ptr;
        if( (ptr = umm_malloc( size )) ) {
            DBGLOG_DEBUG( "realloc %i to a bigger block %i, copy, and free the old\n", blockSize, blocks );
            memcpy( ptr, oldptr, curSize );
            umm_free( oldptr );
            blockSize = blocks;
            UMM_CRITICAL_RESUME(id_realloc);
        } else {
            DBGLOG_DEBUG( "realloc %i to a bigger block %i failed - return NULL and leave the old block!\n", blockSize, blocks );
            /* This space intentionally left blnk */
            UMM_CRITICAL_RESUME(id_realloc);
            STATS__OOM_UPDATE();
        }
    }
#elif defined(UMM_REALLOC_DEFRAG)
  /*
   * Ok, now that we're here we know how many blocks we want and the current
   * blockSize. The prevBlockSize and nextBlockSize are set and we can figure
   * out the best strategy for the new allocation. The following strategy is
   * focused on defragging the heap:
   *
   * 1. If the prev is free and adding it to the current, or current and next
   *    block, gives us enough memory, proceed. Note, that next block may not
   *    be available.
   *    a. Remove the previous block from the free list, assimilate it.
   *    b. If this new block gives enough memory, copy to the new block.
   *       Note, this includes the case of same size or smaller block.
   *    c. Else assimilate the next block, copy to the new block.
   * 2. If the new block is the same size or smaller than the current block do
   *    nothing.
   * 3. If the next block is free and adding it to the current block gives us
   *    enough memory, assimilate the next block.
   * 4. Otherwise try to allocate an entirely new block of memory. If the
   *    allocation works free the old block and return the new pointer. If
   *    the allocation fails, return NULL and leave the old block intact.
   *
   * All that's left to do is decide if the fit was exact or not. If the fit
   * was not exact, then split the memory block so that we use only the
   * requested number of blocks and add what's left to the free list.
   */
   if (prevBlockSize && (prevBlockSize + blockSize + nextBlockSize) >= blocks) { // 1
        umm_disconnect_from_free_list( UMM_PBLOCK(c) );
        c = umm_assimilate_down(c, 0);
        STATS__FREE_BLOCKS_UPDATE( - prevBlockSize );
        blockSize += prevBlockSize;
        if (blockSize >= blocks) {
            DBGLOG_DEBUG( "realloc using prev block - %d\n", blocks );
            STATS__FREE_BLOCKS_ISR_MIN();
        } else {
            DBGLOG_DEBUG( "realloc using prev and next block - %d\n", blocks );
            umm_assimilate_up( c );
            STATS__FREE_BLOCKS_UPDATE( - nextBlockSize );
            blockSize += nextBlockSize;
#ifdef UMM_LIGHTWEIGHT_CPU
            if (blockSize > blocks) {
                umm_split_block( c, blocks, 0 );
                umm_free( (void *)&UMM_DATA(c+blocks) );
            }
            STATS__FREE_BLOCKS_ISR_MIN();
            blockSize = blocks;
#endif
        }
        UMM_CRITICAL_SUSPEND(id_realloc);
        memmove( (void *)&UMM_DATA(c), ptr, curSize );
        ptr = (void *)&UMM_DATA(c);
        UMM_CRITICAL_RESUME(id_realloc);
    } else if (blockSize >= blocks) { // 2
        DBGLOG_DEBUG( "realloc the same or smaller size block - %d, do nothing\n", blocks );
        /* This space intentionally left blank */
    } else if ((blockSize + nextBlockSize) >= blocks) { // 3
        DBGLOG_DEBUG( "realloc using next block - %d\n", blocks );
        umm_assimilate_up( c );
        STATS__FREE_BLOCKS_UPDATE(-nextBlockSize);
        blockSize += nextBlockSize;
    } else { // 4
        UMM_CRITICAL_SUSPEND(id_realloc);
        DBGLOG_DEBUG( "realloc a completely new block %d\n", blocks );
        void *oldptr = ptr;
        if( (ptr = umm_malloc( size )) ) {
            DBGLOG_DEBUG( "realloc %d to a bigger block %d, copy, and free the old\n", blockSize, blocks );
            memcpy( ptr, oldptr, curSize );
            umm_free( oldptr);
            blockSize = blocks;
            UMM_CRITICAL_RESUME(id_realloc);
        } else {
            DBGLOG_DEBUG( "realloc %d to a bigger block %d failed - return NULL and leave the old block!\n", blockSize, blocks );
            /* This space intentionally left blnk */
            UMM_CRITICAL_RESUME(id_realloc);
            STATS__OOM_UPDATE();
        }
    }
#else
#warning "Neither UMM_REALLOC_DEFRAG nor UMM_REALLOC_MINIMIZE_COPY is defined - check umm_malloc_cfg.h"
    /* An always copy option just for performance/fragmentation comparison */
    if (blockSize >= blocks) {
        DBGLOG_DEBUG( "realloc the same or smaller size block - %d, do nothing\n", blocks );
        /* This space intentionally left blank */
    } else {
        UMM_CRITICAL_SUSPEND(id_realloc);
        DBGLOG_DEBUG( "realloc a completely new block %d\n", blocks );
        void *oldptr = ptr;
        if( (ptr = umm_malloc( size )) ) {
            DBGLOG_DEBUG( "realloc %d to a bigger block %d, copy, and free the old\n", blockSize, blocks );
            memcpy( ptr, oldptr, curSize );
            umm_free( oldptr );
            blockSize = blocks;
            UMM_CRITICAL_RESUME(id_realloc);
        } else {
            DBGLOG_DEBUG( "realloc %d to a bigger block %d failed - return NULL and leave the old block!\n", blockSize, blocks );
            /* This space intentionally left blnk */
            UMM_CRITICAL_RESUME(id_realloc);
            STATS__OOM_UPDATE();
        }
    }
#endif
    /* Now all we need to do is figure out if the block fit exactly or if we
     * need to split and free ...
     */

    if (blockSize > blocks ) {
        DBGLOG_DEBUG( "split and free %d blocks from %d\n", blocks, blockSize );
        umm_split_block( c, blocks, 0 );
        umm_free( (void *)&UMM_DATA(c+blocks) );
    }

    STATS__FREE_BLOCKS_MIN();

    /* Release the critical section... */
    UMM_CRITICAL_EXIT(id_realloc);

    return( ptr );
}

/* ------------------------------------------------------------------------ */

void *umm_calloc( size_t num, size_t item_size ) {
  void *ret;

  ret = umm_malloc((size_t)(item_size * num));

  if (ret)
      memset(ret, 0x00, (size_t)(item_size * num));

  return ret;
}

/* ------------------------------------------------------------------------ */

};
