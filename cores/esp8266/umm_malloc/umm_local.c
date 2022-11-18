/*
 * Local Additions/Enhancements
 *
 */
#if defined(BUILD_UMM_MALLOC_C)

#if defined(UMM_CRITICAL_METRICS)
/*
 * umm_malloc performance measurements for critical sections
 */
UMM_TIME_STATS time_stats = {
    {0xFFFFFFFF, 0U, 0U, 0U},
    {0xFFFFFFFF, 0U, 0U, 0U},
    {0xFFFFFFFF, 0U, 0U, 0U},
    #ifdef UMM_INFO
    {0xFFFFFFFF, 0U, 0U, 0U},
    #endif
    #if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
    {0xFFFFFFFF, 0U, 0U, 0U},
    #endif
    #ifdef UMM_INTEGRITY_CHECK
    {0xFFFFFFFF, 0U, 0U, 0U},
    #endif
    {0xFFFFFFFF, 0U, 0U, 0U}
};

bool ICACHE_FLASH_ATTR get_umm_get_perf_data(UMM_TIME_STATS *p, size_t size) {
    UMM_CRITICAL_DECL(id_no_tag);
    if (p && sizeof(time_stats) == size) {
        UMM_CRITICAL_ENTRY(id_no_tag);
        memcpy(p, &time_stats, size);
        UMM_CRITICAL_EXIT(id_no_tag);
        return true;
    }
    return false;
}
#endif

// Alternate Poison functions

#if defined(UMM_POISON_CHECK_LITE)
// We skip this when doing the full poison check.

static bool check_poison_neighbors(umm_heap_context_t *_context, uint16_t cur) {
    uint16_t c;

    if (0 == cur) {
        return true;
    }

    c = UMM_PBLOCK(cur) & UMM_BLOCKNO_MASK;
    while (c && (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK)) {
        /*
           There can be up to 1 free block neighbor in either direction.
           This loop should self limit to 2 passes, due to heap design.
           i.e. Adjacent free space is always consolidated.
         */
        if (!(UMM_NBLOCK(c) & UMM_FREELIST_MASK)) {
            if (!check_poison_block(&UMM_BLOCK(c))) {
                return false;
            }

            break;
        }

        c = UMM_PBLOCK(c) & UMM_BLOCKNO_MASK;
    }

    c = UMM_NBLOCK(cur) & UMM_BLOCKNO_MASK;
    while ((UMM_NBLOCK(c) & UMM_BLOCKNO_MASK)) {
        if (!(UMM_NBLOCK(c) & UMM_FREELIST_MASK)) {
            if (!check_poison_block(&UMM_BLOCK(c))) {
                return false;
            }

            break;
        }

        c = UMM_NBLOCK(c) & UMM_BLOCKNO_MASK;
    }

    return true;
}
#endif

#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)

/* ------------------------------------------------------------------------ */
#include "heap_cb.h"

static void *get_unpoisoned_check_neighbors(const void *vptr, const char *file, int line, const void *caller) {
    uintptr_t ptr = (uintptr_t)vptr;

    if (ptr != 0) {

        ptr -= (sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE);

        #if defined(UMM_POISON_CHECK_LITE)
        UMM_CRITICAL_DECL(id_poison);
        uint16_t c;
        bool poison = true;
        umm_heap_context_t *_context = _umm_get_ptr_context((void *)ptr);
        if (_context) {

            /* Figure out which block we're in. Note the use of truncated division... */
            c = (ptr - (uintptr_t)(&(_context->heap[0]))) / sizeof(umm_block);

            UMM_CRITICAL_ENTRY(id_poison);
            if (! check_poison_block(&UMM_BLOCK(c))) {
                DBGLOG_ERROR("Allocation address %p\n", vptr);
                size_t size = *(size_t *)ptr;
                _HEAP_DEBUG_PROBE_PSFLC_CB(heap_poison_lite_cb_id, (void *)ptr, size, file, line, caller);
                poison = false;
            } else
            if (! check_poison_neighbors(_context, c)) {
                DBGLOG_ERROR("This bad block is in a neighbor allocation near: %p\n", vptr);
                _HEAP_DEBUG_PROBE_PSFLC_CB(heap_poison_lite_neighbor_cb_id, (void *)ptr, 0, file, line, caller);
                poison = false;
            }
            UMM_CRITICAL_EXIT(id_poison);
        } else {
            DBGLOG_ERROR("\nPointer %p is not a Heap address.\n", vptr);
            _HEAP_DEBUG_PROBE_PSFLC_CB(heap_poison_lite_addr_cb_id, (void *)ptr, 0, file, line, caller);
            poison = false;
        }

        if (!poison) {
            DBGLOG_ERROR("Called from %p\n", caller);
            if (file) {
                __panic_func(file, line, "");
            } else {
                abort();
            }
        }
        #else
        /*
         *  No need to check poison here. POISON_CHECK() has already done a
         *  full heap check.
         */
        (void)file;
        (void)line;
        #endif
    }

    return (void *)ptr;
}

/* ------------------------------------------------------------------------ */

void *umm_poison_realloc_flc(void *ptr, size_t size, const char *file, int line, const void *caller) {
    void *ret;

    ptr = get_unpoisoned_check_neighbors(ptr, file, line, caller);

    add_poison_size(&size);
    ret = umm_realloc(ptr, size);

    ret = get_poisoned(ret, size);

    return ret;
}

/* ------------------------------------------------------------------------ */

void umm_poison_free_flc(void *ptr, const char *file, int line, const void *caller) {

    ptr = get_unpoisoned_check_neighbors(ptr, file, line, caller);

    umm_free(ptr);
}
#endif

/* ------------------------------------------------------------------------ */

#if defined(UMM_STATS) || defined(UMM_STATS_FULL) || defined(UMM_INFO)
/*
  For internal, mainly used by UMM_STATS_FULL; exported so external components
  can perform Heap related calculations.
*/
size_t umm_block_size(void) {
    return sizeof(umm_block);
}
#endif

/*
  Need to expose a function to support getting the current free heap size.
  Export `size_t umm_free_heap_size_lw(void)` for this purpose.
  Used by ESP.getFreeHeap().

  For an expanded discussion see Notes.h, entry dated "Sep 26, 2022"
*/
#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
/*
  Default build option to support export.

  Keep complete call path in IRAM.
*/
size_t umm_free_heap_size_lw(void) {
    UMM_CHECK_INITIALIZED();

    umm_heap_context_t *_context = umm_get_current_heap();
    return (size_t)_context->UMM_FREE_BLOCKS * sizeof(umm_block);
}

#elif defined(UMM_INLINE_METRICS)
/*
  For the fallback option using `size_t umm_free_heap_size(void)`, we must have
  the UMM_INLINE_METRICS build option enabled to support free heap size
  reporting without the use of `umm_info()`.
*/
size_t umm_free_heap_size_lw(void) __attribute__ ((alias("umm_free_heap_size")));

#else
/*
  We require a resource to track and report free Heap size with low overhead.
  For an expanded discussion see Notes.h, entry dated "Sep 26, 2022"
*/
#error UMM_INLINE_METRICS, UMM_STATS, or UMM_STATS_FULL needs to be defined.
#endif

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
size_t umm_free_heap_size_core_lw(umm_heap_context_t *_context) {
    return (size_t)_context->UMM_FREE_BLOCKS * sizeof(umm_block);
}

#elif defined(UMM_INFO)
// Backfill support for umm_free_heap_size_core_lw()
size_t umm_free_heap_size_core_lw(umm_heap_context_t *_context) __attribute__ ((alias("umm_free_heap_size_core")));
#endif

/*
  This API is called by `system_get_free_heap_size()` which is in IRAM. Driving
  the assumption the callee may be in an ISR or Cache_Read_Disable state. Use
  IRAM to ensure that the complete call chain is in IRAM.

  To satisfy this requirement, we need UMM_STATS... or UMM_INLINE_METRICS
  defined. These support an always available without intense computation
  free-Heap value.

  Like the other vPort... APIs used by the SDK, this must always report on the
  DRAM Heap not the current Heap.
*/
#if (UMM_NUM_HEAPS == 1)
// Reduce IRAM usage for the single Heap case
#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
size_t xPortGetFreeHeapSize(void) __attribute__ ((alias("umm_free_heap_size_lw")));
#else
size_t xPortGetFreeHeapSize(void) __attribute__ ((alias("umm_free_heap_size")));
#endif

#else
size_t xPortGetFreeHeapSize(void) {
    #if defined(UMM_STATS) || defined(UMM_STATS_FULL) || defined(UMM_INLINE_METRICS)
    UMM_CHECK_INITIALIZED();
    umm_heap_context_t *_context = umm_get_heap_by_id(UMM_HEAP_DRAM);

    return umm_free_heap_size_core_lw(_context);
    #else
    // At this time, this build path is not reachable. In case things change,
    // keep build check.
    // Not in IRAM, umm_info() would have been used to complete this operation.
    #error "No ISR safe function available to implement xPortGetFreeHeapSize()"
    #endif
}
#endif

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
void umm_print_stats(int force) {
    umm_heap_context_t *_context = umm_get_current_heap();

    DBGLOG_FORCE(force, "umm heap statistics:\n");
    DBGLOG_FORCE(force,   "  Heap ID           %7u\n", _context->id);
    DBGLOG_FORCE(force,   "  Free Space        %7u\n", _context->UMM_FREE_BLOCKS * sizeof(umm_block));
    DBGLOG_FORCE(force,   "  OOM Count         %7u\n", _context->UMM_OOM_COUNT);
    #if defined(UMM_STATS_FULL)
    DBGLOG_FORCE(force,   "  Low Watermark     %7u\n", _context->stats.free_blocks_min * sizeof(umm_block));
    DBGLOG_FORCE(force,   "  Low Watermark ISR %7u\n", _context->stats.free_blocks_isr_min * sizeof(umm_block));
    DBGLOG_FORCE(force,   "  MAX Alloc Request %7u\n", _context->stats.alloc_max_size);
    #endif
    DBGLOG_FORCE(force,   "  Size of umm_block %7u\n", sizeof(umm_block));
    DBGLOG_FORCE(force, "+--------------------------------------------------------------+\n");
}
#endif

int ICACHE_FLASH_ATTR umm_info_safe_printf_P(const char *fmt, ...) {
    char ram_buf[strlen_P(fmt) + 1];
    strcpy_P(ram_buf, fmt);
    va_list argPtr;
    va_start(argPtr, fmt);
    int result = ets_vprintf(ets_uart_putc1, ram_buf, argPtr);
    va_end(argPtr);
    return result;
}

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
size_t ICACHE_FLASH_ATTR umm_get_oom_count(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->UMM_OOM_COUNT;
}
#endif

#ifdef UMM_STATS_FULL
// TODO - Did I mix something up
//
//   umm_free_heap_size_min      is the same code as
//   umm_free_heap_size_lw_min
//
// If this is correct use alias.
//
size_t ICACHE_FLASH_ATTR umm_free_heap_size_lw_min(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.free_blocks_min * umm_block_size();
}

size_t ICACHE_FLASH_ATTR umm_free_heap_size_min_reset(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    _context->stats.free_blocks_min = _context->UMM_FREE_BLOCKS;
    return _context->stats.free_blocks_min * umm_block_size();
}

#if 0 // TODO - Don't understand this why do both umm_free_heap_size_(lw_)min exist
size_t umm_free_heap_size_min(void) __attribute__ ((alias("umm_free_heap_size_lw_min")));
#else
size_t ICACHE_FLASH_ATTR umm_free_heap_size_min(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.free_blocks_min * umm_block_size();
}
#endif

size_t ICACHE_FLASH_ATTR umm_free_heap_size_isr_min(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.free_blocks_isr_min * umm_block_size();
}

size_t ICACHE_FLASH_ATTR umm_get_max_alloc_size(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.alloc_max_size;
}

size_t ICACHE_FLASH_ATTR umm_get_last_alloc_size(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.last_alloc_size;
}

size_t ICACHE_FLASH_ATTR umm_get_malloc_count(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.id_malloc_count;
}

size_t ICACHE_FLASH_ATTR umm_get_malloc_zero_count(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.id_malloc_zero_count;
}

size_t ICACHE_FLASH_ATTR umm_get_realloc_count(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.id_realloc_count;
}

size_t ICACHE_FLASH_ATTR umm_get_realloc_zero_count(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.id_realloc_zero_count;
}

size_t ICACHE_FLASH_ATTR umm_get_free_count(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.id_free_count;
}

size_t ICACHE_FLASH_ATTR umm_get_free_null_count(void) {
    umm_heap_context_t *_context = umm_get_current_heap();
    return _context->stats.id_free_null_count;
}
#endif // UMM_STATS_FULL

#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
/*
 * Saturated unsigned add
 * Poison added to allocation size requires overflow protection.
 */
size_t umm_uadd_sat(const size_t a, const size_t b) {
    size_t r = a + b;
    if (r < a) {
        return SIZE_MAX;
    }
    return r;
}
#endif

/*
 * Use platform-specific functions to protect against unsigned overflow/wrap by
 * implementing saturated unsigned multiply.
 * The function umm_calloc requires a saturated multiply function.
 */
size_t umm_umul_sat(const size_t a, const size_t b) {
    size_t r;
    if (__builtin_mul_overflow(a, b, &r)) {
        return SIZE_MAX;
    }
    return r;
}


#endif // BUILD_UMM_MALLOC_C
