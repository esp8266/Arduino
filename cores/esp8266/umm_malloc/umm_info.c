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

// UMM_HEAP_INFO ummHeapInfo;

void *umm_info(void *ptr, bool force) {
    UMM_CRITICAL_DECL(id_info);

    UMM_CHECK_INITIALIZED();

    uint16_t blockNo = 0;

    /* Protect the critical section... */
    UMM_CRITICAL_ENTRY(id_info);

    umm_heap_context_t *_context = umm_get_current_heap();

    /*
     * Clear out all of the entries in the ummHeapInfo structure before doing
     * any calculations..
     */
    memset(&_context->info, 0, sizeof(_context->info));

    DBGLOG_FORCE(force, "\n");
    DBGLOG_FORCE(force, "+----------+-------+--------+--------+-------+--------+--------+\n");
    DBGLOG_FORCE(force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5d|NF %5d|PF %5d|\n",
        DBGLOG_32_BIT_PTR(&UMM_BLOCK(blockNo)),
        blockNo,
        UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
        UMM_PBLOCK(blockNo),
        (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK) - blockNo,
        UMM_NFREE(blockNo),
        UMM_PFREE(blockNo));

    /*
     * Now loop through the block lists, and keep track of the number and size
     * of used and free blocks. The terminating condition is an nb pointer with
     * a value of zero...
     */

    blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;

    while (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK) {
        size_t curBlocks = (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK) - blockNo;

        ++_context->info.totalEntries;
        _context->info.totalBlocks += curBlocks;

        /* Is this a free block? */

        if (UMM_NBLOCK(blockNo) & UMM_FREELIST_MASK) {
            ++_context->info.freeEntries;
            _context->info.freeBlocks += curBlocks;
            _context->info.freeBlocksSquared += (curBlocks * curBlocks);

            if (_context->info.maxFreeContiguousBlocks < curBlocks) {
                _context->info.maxFreeContiguousBlocks = curBlocks;
            }

            DBGLOG_FORCE(force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5u|NF %5d|PF %5d|\n",
                DBGLOG_32_BIT_PTR(&UMM_BLOCK(blockNo)),
                blockNo,
                UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
                UMM_PBLOCK(blockNo),
                (uint16_t)curBlocks,
                UMM_NFREE(blockNo),
                UMM_PFREE(blockNo));

            /* Does this block address match the ptr we may be trying to free? */

            if (ptr == &UMM_BLOCK(blockNo)) {

                /* Release the critical section... */
                UMM_CRITICAL_EXIT(id_info);

                return ptr;
            }
        } else {
            ++_context->info.usedEntries;
            _context->info.usedBlocks += curBlocks;

            DBGLOG_FORCE(force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5u|\n",
                DBGLOG_32_BIT_PTR(&UMM_BLOCK(blockNo)),
                blockNo,
                UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
                UMM_PBLOCK(blockNo),
                (uint16_t)curBlocks);
        }

        blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;
    }

    /*
     * The very last block is used as a placeholder to indicate that
     * there are no more blocks in the heap, so it cannot be used
     * for anything - at the same time, the size of this block must
     * ALWAYS be exactly 1 !
     */

    DBGLOG_FORCE(force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5d|NF %5d|PF %5d|\n",
        DBGLOG_32_BIT_PTR(&UMM_BLOCK(blockNo)),
        blockNo,
        UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
        UMM_PBLOCK(blockNo),
        UMM_NUMBLOCKS - blockNo,
        UMM_NFREE(blockNo),
        UMM_PFREE(blockNo));

    DBGLOG_FORCE(force, "+----------+-------+--------+--------+-------+--------+--------+\n");

    DBGLOG_FORCE(force, "Total Entries %5d    Used Entries %5d    Free Entries %5d\n",
        _context->info.totalEntries,
        _context->info.usedEntries,
        _context->info.freeEntries);

    DBGLOG_FORCE(force, "Total Blocks  %5d    Used Blocks  %5d    Free Blocks  %5d\n",
        _context->info.totalBlocks,
        _context->info.usedBlocks,
        _context->info.freeBlocks);

    DBGLOG_FORCE(force, "+--------------------------------------------------------------+\n");

    DBGLOG_FORCE(force, "Usage Metric:               %5d\n", umm_usage_metric_core(_context));
    DBGLOG_FORCE(force, "Fragmentation Metric:       %5d\n", umm_fragmentation_metric_core(_context));

    DBGLOG_FORCE(force, "+--------------------------------------------------------------+\n");

    #if defined(UMM_STATS) || defined(UMM_STATS_FULL)
    #if !defined(UMM_INLINE_METRICS)
    if (_context->info.freeBlocks == _context->stats.free_blocks) {
        DBGLOG_FORCE(force, "heap info Free blocks and heap statistics Free blocks match.\n");
    } else {
        DBGLOG_FORCE(force, "\nheap info Free blocks  %5d != heap statistics Free Blocks  %5d\n\n",
            _context->info.freeBlocks,
            _context->stats.free_blocks);
    }
    DBGLOG_FORCE(force, "+--------------------------------------------------------------+\n");
    #endif

    umm_print_stats(force);
    #endif

    /* Release the critical section... */
    UMM_CRITICAL_EXIT(id_info);

    return NULL;
}

/* ------------------------------------------------------------------------ */

size_t umm_free_heap_size_core(umm_heap_context_t *_context) {
    return (size_t)_context->info.freeBlocks * sizeof(umm_block);
}

/*
  Need to expose a function for getting the current free heap size
  Using umm_free_heap_size for this purpose.

  For an expanded discussion see Notes.h, entry dated "Sep 26, 2022"
*/
#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
/*
  For this build option, see umm_free_heap_size in umm_local.c
*/

/*
  Make upstream logic available under a new name.
  May be useful for sanity checks between methods.
  Only takes .bin space if used.
*/
size_t umm_free_heap_size_info(void)

#else
/*
  Not our default build path. For this path to function well with WiFi enabled,
  you must use the build option UMM_INLINE_METRICS. Otherwise, umm_info() is
  used to complete the operation, which uses a time-consuming method for getting
  free Heap and runs with interrupts off. Also, it cannot support calls from
  ISRs, `umm_info()` runs from flash.

  Current build test in umm_local.c fails the build option that uses the
  umm_info() method.
*/
size_t umm_free_heap_size(void)
#endif
{
    #ifndef UMM_INLINE_METRICS
    umm_info(NULL, false);
    #endif

    return umm_free_heap_size_core(umm_get_current_heap());
}

// C Breaking change in upstream umm_max_block_size() was changed to
// C umm_max_free_block_size() keeping old function name for (dot) releases.
// C TODO: update at next major release.
// C size_t umm_max_free_block_size( void ) {
size_t umm_max_block_size_core(umm_heap_context_t *_context) {
    return _context->info.maxFreeContiguousBlocks * sizeof(umm_block);
}

size_t umm_max_block_size(void) {
    umm_info(NULL, false);
    return umm_max_block_size_core(umm_get_current_heap());
}

/*
  Without build option UMM_INLINE_METRICS, calls to umm_usage_metric() or
  umm_fragmentation_metric() must to be preceded by a call to umm_info(NULL, false)
  for updated results.
*/
int umm_usage_metric_core(umm_heap_context_t *_context) {
// C Note, umm_metrics also appears in the upstrean w/o definition. I suspect it is suppose to be ummHeapInfo.
    // DBGLOG_DEBUG( "usedBlocks %d totalBlocks %d\n", umm_metrics.usedBlocks, ummHeapInfo.totalBlocks);
    DBGLOG_DEBUG("usedBlocks %d totalBlocks %d\n", _context->info.usedBlocks, _context->info.totalBlocks);
    if (_context->info.freeBlocks) {
        return (int)((_context->info.usedBlocks * 100) / (_context->info.freeBlocks));
    }

    return -1; // no freeBlocks
}

int umm_usage_metric(void) {
    #ifndef UMM_INLINE_METRICS
    umm_info(NULL, false);
    #endif

    return umm_usage_metric_core(umm_get_current_heap());
}
uint32_t sqrt32(uint32_t n);

int umm_fragmentation_metric_core(umm_heap_context_t *_context) {
    // DBGLOG_DEBUG( "freeBlocks %d freeBlocksSquared %d\n", umm_metrics.freeBlocks, ummHeapInfo.freeBlocksSquared);
    DBGLOG_DEBUG("freeBlocks %d freeBlocksSquared %d\n", _context->info.freeBlocks, _context->info.freeBlocksSquared);
    if (0 == _context->info.freeBlocks) {
        return 0;
    } else {
        // upstream version: return (100 - (((uint32_t)(sqrtf(ummHeapInfo.freeBlocksSquared)) * 100)/(ummHeapInfo.freeBlocks)));
        return 100 - (((uint32_t)(sqrt32(_context->info.freeBlocksSquared)) * 100) / (_context->info.freeBlocks));
    }
}

int umm_fragmentation_metric(void) {
    #ifndef UMM_INLINE_METRICS
    umm_info(NULL, false);
    #endif

    return umm_fragmentation_metric_core(umm_get_current_heap());
}

#ifdef UMM_INLINE_METRICS
static void umm_fragmentation_metric_init(umm_heap_context_t *_context) {
    _context->info.freeBlocks = UMM_NUMBLOCKS - 2;
    _context->info.freeBlocksSquared = _context->info.freeBlocks * _context->info.freeBlocks;
}

static void umm_fragmentation_metric_add(umm_heap_context_t *_context, uint16_t c) {
    uint16_t blocks = (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) - c;
    DBGLOG_DEBUG("Add block %d size %d to free metric\n", c, blocks);
    _context->info.freeBlocks += blocks;
    _context->info.freeBlocksSquared += (blocks * blocks);
}

static void umm_fragmentation_metric_remove(umm_heap_context_t *_context, uint16_t c) {
    uint16_t blocks = (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) - c;
    DBGLOG_DEBUG("Remove block %d size %d from free metric\n", c, blocks);
    _context->info.freeBlocks -= blocks;
    _context->info.freeBlocksSquared -= (blocks * blocks);
}
#endif // UMM_INLINE_METRICS

/* ------------------------------------------------------------------------ */
#endif

#endif  // defined(BUILD_UMM_MALLOC_C)
