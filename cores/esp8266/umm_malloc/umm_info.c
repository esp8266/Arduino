#if defined(BUILD_UMM_MALLOC_C)

#ifdef UMM_INFO

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <math.h>

/* ----------------------------------------------------------------------------
 * One of the coolest things about this little library is that it's VERY
 * easy to get debug information about the memory heap by simply iterating
 * through all of the memory blocks.
 *
 * As you go through all the blocks, you can check to see if it's a free
 * block by looking at the high order bit of the next block index. You can
 * also see how big the block is by subtracting the next block index from
 * the current block number.
 *
 * The umm_info function does all of that and makes the results available
 * in the ummHeapInfo structure.
 * ----------------------------------------------------------------------------
 */

UMM_HEAP_INFO ummHeapInfo;

void *umm_info( void *ptr, bool force ) {
  UMM_CRITICAL_DECL(id_info);

  UMM_INIT_HEAP;

  uint16_t blockNo = 0;

  /* Protect the critical section... */
  UMM_CRITICAL_ENTRY(id_info);

  umm_heap_context_t *_context = umm_get_current_heap();

  /*
   * Clear out all of the entries in the ummHeapInfo structure before doing
   * any calculations..
   */
  memset( &ummHeapInfo, 0, sizeof( ummHeapInfo ) );

  DBGLOG_FORCE( force, "\n" );
  DBGLOG_FORCE( force, "+----------+-------+--------+--------+-------+--------+--------+\n" );
  DBGLOG_FORCE( force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5d|NF %5d|PF %5d|\n",
      DBGLOG_32_BIT_PTR(&UMM_BLOCK(blockNo)),
      blockNo,
      UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
      UMM_PBLOCK(blockNo),
      (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo,
      UMM_NFREE(blockNo),
      UMM_PFREE(blockNo) );

  /*
   * Now loop through the block lists, and keep track of the number and size
   * of used and free blocks. The terminating condition is an nb pointer with
   * a value of zero...
   */

  blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;

  while( UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK ) {
    size_t curBlocks = (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo;

    ++ummHeapInfo.totalEntries;
    ummHeapInfo.totalBlocks += curBlocks;

    /* Is this a free block? */

    if( UMM_NBLOCK(blockNo) & UMM_FREELIST_MASK ) {
      ++ummHeapInfo.freeEntries;
      ummHeapInfo.freeBlocks += curBlocks;
      ummHeapInfo.freeBlocksSquared += (curBlocks * curBlocks);

      if (ummHeapInfo.maxFreeContiguousBlocks < curBlocks) {
        ummHeapInfo.maxFreeContiguousBlocks = curBlocks;
      }

      DBGLOG_FORCE( force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5u|NF %5d|PF %5d|\n",
          DBGLOG_32_BIT_PTR(&UMM_BLOCK(blockNo)),
          blockNo,
          UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
          UMM_PBLOCK(blockNo),
          (uint16_t)curBlocks,
          UMM_NFREE(blockNo),
          UMM_PFREE(blockNo) );

      /* Does this block address match the ptr we may be trying to free? */

      if( ptr == &UMM_BLOCK(blockNo) ) {

        /* Release the critical section... */
        UMM_CRITICAL_EXIT(id_info);

        return( ptr );
      }
    } else {
      ++ummHeapInfo.usedEntries;
      ummHeapInfo.usedBlocks += curBlocks;

      DBGLOG_FORCE( force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5u|\n",
          DBGLOG_32_BIT_PTR(&UMM_BLOCK(blockNo)),
          blockNo,
          UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
          UMM_PBLOCK(blockNo),
          (uint16_t)curBlocks );
    }

    blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;
  }

  /*
   * The very last block is used as a placeholder to indicate that
   * there are no more blocks in the heap, so it cannot be used
   * for anything - at the same time, the size of this block must
   * ALWAYS be exactly 1 !
   */

  DBGLOG_FORCE( force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5d|NF %5d|PF %5d|\n",
      DBGLOG_32_BIT_PTR(&UMM_BLOCK(blockNo)),
      blockNo,
      UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
      UMM_PBLOCK(blockNo),
      UMM_NUMBLOCKS-blockNo,
      UMM_NFREE(blockNo),
      UMM_PFREE(blockNo) );

  DBGLOG_FORCE( force, "+----------+-------+--------+--------+-------+--------+--------+\n" );

  DBGLOG_FORCE( force, "Total Entries %5d    Used Entries %5d    Free Entries %5d\n",
      ummHeapInfo.totalEntries,
      ummHeapInfo.usedEntries,
      ummHeapInfo.freeEntries );

  DBGLOG_FORCE( force, "Total Blocks  %5d    Used Blocks  %5d    Free Blocks  %5d\n",
      ummHeapInfo.totalBlocks,
      ummHeapInfo.usedBlocks,
      ummHeapInfo.freeBlocks  );

  DBGLOG_FORCE( force, "+--------------------------------------------------------------+\n" );

  DBGLOG_FORCE( force, "Usage Metric:               %5d\n", umm_usage_metric());
  DBGLOG_FORCE( force, "Fragmentation Metric:       %5d\n", umm_fragmentation_metric());

  DBGLOG_FORCE( force, "+--------------------------------------------------------------+\n" );

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
#if !defined(UMM_INLINE_METRICS)
  if (ummHeapInfo.freeBlocks == _context->stats.free_blocks) {
      DBGLOG_FORCE( force, "heap info Free blocks and heap statistics Free blocks match.\n");
  } else {
      DBGLOG_FORCE( force, "\nheap info Free blocks  %5d != heap statistics Free Blocks  %5d\n\n",
          ummHeapInfo.freeBlocks,
          _context->stats.free_blocks  );
  }
  DBGLOG_FORCE( force, "+--------------------------------------------------------------+\n" );
#endif

  umm_print_stats(force);
#endif

  /* Release the critical section... */
  UMM_CRITICAL_EXIT(id_info);

  return( NULL );
}

/* ------------------------------------------------------------------------ */

size_t umm_free_heap_size( void ) {
#ifndef UMM_INLINE_METRICS
  umm_info(NULL, false);
#endif
  return (size_t)ummHeapInfo.freeBlocks * sizeof(umm_block);
}

//C Breaking change in upstream umm_max_block_size() was changed to
//C umm_max_free_block_size() keeping old function name for (dot) releases.
//C TODO: update at next major release.
//C size_t umm_max_free_block_size( void ) {
size_t umm_max_block_size( void ) {
  umm_info(NULL, false);
  return ummHeapInfo.maxFreeContiguousBlocks * sizeof(umm_block);
}

/*
  Without build option UMM_INLINE_METRICS, calls to umm_usage_metric() or
  umm_fragmentation_metric() must to be preceeded by a call to umm_info(NULL, false)
  for updated results.
*/
int umm_usage_metric( void ) {
#ifndef UMM_INLINE_METRICS
  umm_info(NULL, false);
#endif
  DBGLOG_DEBUG( "usedBlocks %d totalBlocks %d\n", umm_metrics.usedBlocks, ummHeapInfo.totalBlocks);
  if (ummHeapInfo.freeBlocks)
    return (int)((ummHeapInfo.usedBlocks * 100)/(ummHeapInfo.freeBlocks));

  return -1;  // no freeBlocks
}

uint32_t sqrt32 (uint32_t n);

int umm_fragmentation_metric( void ) {
#ifndef UMM_INLINE_METRICS
  umm_info(NULL, false);
#endif
  DBGLOG_DEBUG( "freeBlocks %d freeBlocksSquared %d\n", umm_metrics.freeBlocks, ummHeapInfo.freeBlocksSquared);
  if (0 == ummHeapInfo.freeBlocks) {
      return 0;
  } else {
      //upstream version: return (100 - (((uint32_t)(sqrtf(ummHeapInfo.freeBlocksSquared)) * 100)/(ummHeapInfo.freeBlocks)));
      return (100 - (((uint32_t)(sqrt32(ummHeapInfo.freeBlocksSquared)) * 100)/(ummHeapInfo.freeBlocks)));
  }
}

#ifdef UMM_INLINE_METRICS
static void umm_fragmentation_metric_init( void ) {
    ummHeapInfo.freeBlocks = UMM_NUMBLOCKS - 2;
    ummHeapInfo.freeBlocksSquared = ummHeapInfo.freeBlocks * ummHeapInfo.freeBlocks;
}

static void umm_fragmentation_metric_add( uint16_t c ) {
    uint16_t blocks = (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) - c;
    DBGLOG_DEBUG( "Add block %d size %d to free metric\n", c, blocks);
    ummHeapInfo.freeBlocks += blocks;
    ummHeapInfo.freeBlocksSquared += (blocks * blocks);
}

static void umm_fragmentation_metric_remove( uint16_t c ) {
    uint16_t blocks = (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) - c;
    DBGLOG_DEBUG( "Remove block %d size %d from free metric\n", c, blocks);
    ummHeapInfo.freeBlocks -= blocks;
    ummHeapInfo.freeBlocksSquared -= (blocks * blocks);
}
#endif // UMM_INLINE_METRICS

/* ------------------------------------------------------------------------ */
#endif

#endif  // defined(BUILD_UMM_MALLOC_C)
