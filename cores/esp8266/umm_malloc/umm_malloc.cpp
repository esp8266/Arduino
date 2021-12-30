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
 * R.Hempel 2019-09-07 - Separate the malloc() and free() functionality into
 *                        wrappers that use critical section protection macros
 *                        and static core functions that assume they are
 *                        running in a protected con text. Thanks @devyte
 * R.Hempel 2020-01-07 - Add support for Fragmentation metric - See Issue 14
 * R.Hempel 2020-01-12 - Use explicitly sized values from stdint.h - See Issue 15
 * R.Hempel 2020-01-20 - Move metric functions back to umm_info - See Issue 29
 * R.Hempel 2020-02-01 - Macro functions are uppercased - See Issue 34
 * ----------------------------------------------------------------------------
 */

/*
 * This include is nothing but comments about thoughts and observations made
 * while updating the Arduino ESP8266 Core, with the new upstream umm_malloc.
 * It is added here as an include so that it does not get lost and to avoid
 * cluttering up the code with a huge block comment.
 */
 #include "Notes.h"
/*
 * Added for using with Arduino ESP8266 and handling renameing to umm_malloc.cpp
 */
#define BUILD_UMM_MALLOC_C

extern "C" {

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "umm_malloc_cfg.h"   /* user-dependent */
#include "umm_malloc.h"

/* Use the default DBGLOG_LEVEL and DBGLOG_FUNCTION */

#ifndef DBGLOG_LEVEL
#define DBGLOG_LEVEL 0
#endif

// Save 104 bytes by calling umm_init() early once from app_entry()
// Some minor UMM_CRITICAL_METRICS counts will be lost through CRT0 init.
// #define UMM_INIT_HEAP if (!umm_heap) { umm_init(); }
#define UMM_INIT_HEAP (void)0

#include "dbglog/dbglog.h"

// C This change is new in upstream umm_malloc.I think this would have created a
// C breaking change. Keeping the old #define method in umm_malloc_cfg.h.
// C I don't see a simple way of making it work. We would have to run code before
// C the SDK has run to set a value for uint32_t UMM_MALLOC_CFG_HEAP_SIZE.
// C On the other hand, a manual call to umm_init() before anything else has had a
// C chance to run would mean that all those calls testing to see if the heap has
// C been initialized at every umm_malloc API could be removed.
// C
// C before starting the NON OS SDK
// C extern void *UMM_MALLOC_CFG_HEAP_ADDR;
// C extern uint32_t UMM_MALLOC_CFG_HEAP_SIZE;

#include "umm_local.h"      // target-dependent supplemental

/* ------------------------------------------------------------------------- */

UMM_H_ATTPACKPRE typedef struct umm_ptr_t {
    uint16_t next;
    uint16_t prev;
} UMM_H_ATTPACKSUF umm_ptr;


UMM_H_ATTPACKPRE typedef struct umm_block_t {
    union {
        umm_ptr used;
    } header;
    union {
        umm_ptr free;
        uint8_t data[4];
    } body;
} UMM_H_ATTPACKSUF umm_block;

#define UMM_FREELIST_MASK ((uint16_t)(0x8000))
#define UMM_BLOCKNO_MASK  ((uint16_t)(0x7FFF))

/* ------------------------------------------------------------------------- */
umm_heap_context_t heap_context[UMM_NUM_HEAPS] __attribute__((section(".noinit")));
// void *umm_heap = NULL;

/* A stack allowing push/popping of heaps for library use */
#if (UMM_NUM_HEAPS == 1)

#else
static size_t umm_heap_cur = UMM_HEAP_DRAM;
static int umm_heap_stack_ptr = 0;
static unsigned char umm_heap_stack[UMM_HEAP_STACK_DEPTH];
#endif
/* ------------------------------------------------------------------------ */
/*
 * Methods to get heap id or context
 *
 */

#if (UMM_NUM_HEAPS == 1)
size_t umm_get_current_heap_id(void) {
    return 0;
}

umm_heap_context_t *umm_get_current_heap(void) {
    return &heap_context[0];
}

static umm_heap_context_t *umm_get_heap_by_id(size_t which) {
    (void)which;
    return &heap_context[0];
}

umm_heap_context_t *umm_set_heap_by_id(size_t which) {
    (void)which;
    return &heap_context[0];
}

#else
size_t umm_get_current_heap_id(void) {
    return umm_heap_cur;
}

umm_heap_context_t *umm_get_current_heap(void) {
    return &heap_context[umm_heap_cur];
}

static umm_heap_context_t *umm_get_heap_by_id(size_t which) {
    if (which < UMM_NUM_HEAPS) {
        return &heap_context[which];
    }
    return NULL;
}

umm_heap_context_t *umm_set_heap_by_id(size_t which) {
    umm_heap_context_t *_context = umm_get_heap_by_id(which);
    if (_context && _context->heap) {
        umm_heap_cur = which;
        return _context;
    }
    return NULL;
}
#endif

#if (UMM_NUM_HEAPS == 1)
umm_heap_context_t *umm_push_heap(size_t which) {
    (void)which;
    return &heap_context[0];
}

umm_heap_context_t *umm_pop_heap(void) {
    return &heap_context[0];
}

int umm_get_heap_stack_index(void) {
    return 0;
}
#else
/* ------------------------------------------------------------------------ */

umm_heap_context_t *umm_push_heap(size_t which) {
    if (umm_heap_stack_ptr < UMM_HEAP_STACK_DEPTH) {
        umm_heap_stack[umm_heap_stack_ptr++] = umm_heap_cur;
        return umm_set_heap_by_id(which);
    }
    return NULL;
}

/* ------------------------------------------------------------------------ */

umm_heap_context_t *umm_pop_heap(void) {
    if (umm_heap_stack_ptr > 0) {
        return umm_set_heap_by_id(umm_heap_stack[--umm_heap_stack_ptr]);
    }
    return NULL;
}

// Intended for diagnosic use
int umm_get_heap_stack_index(void) {
    return umm_heap_stack_ptr;
}
#endif
/* ------------------------------------------------------------------------ */
/*
 * Returns the correct heap context for a given pointer.  Useful for
 * realloc or free since you may not be in the right heap to handle it.
 *
 */
static bool test_ptr_context(size_t which, void *ptr) {
    return
        heap_context[which].heap &&
        ptr >= (void *)heap_context[which].heap &&
        ptr < heap_context[which].heap_end;
}

static umm_heap_context_t *umm_get_ptr_context(void *ptr) {
    for (size_t i = 0; i < UMM_NUM_HEAPS; i++) {
        if (test_ptr_context(i, ptr)) {
            return umm_get_heap_by_id(i);
        }
    }

    panic();
    return NULL;
}

#define UMM_NUMBLOCKS (_context->numblocks)
#define UMM_BLOCK_LAST (UMM_NUMBLOCKS - 1)

/* -------------------------------------------------------------------------
 * These macros evaluate to the address of the block and data respectively
 */

#define UMM_BLOCK(b)  (_context->heap[b])
#define UMM_DATA(b)   (UMM_BLOCK(b).body.data)

/* -------------------------------------------------------------------------
 * These macros evaluate to the index of the block - NOT the address!!!
 */

/* ------------------------------------------------------------------------ */

#define UMM_NBLOCK(b) (UMM_BLOCK(b).header.used.next)
#define UMM_PBLOCK(b) (UMM_BLOCK(b).header.used.prev)
#define UMM_NFREE(b)  (UMM_BLOCK(b).body.free.next)
#define UMM_PFREE(b)  (UMM_BLOCK(b).body.free.prev)

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

static uint16_t umm_blocks(size_t size) {

    /*
     * The calculation of the block size is not too difficult, but there are
     * a few little things that we need to be mindful of.
     *
     * When a block removed from the free list, the space used by the free
     * pointers is available for data. That's what the first calculation
     * of size is doing.
     */

    if (size <= (sizeof(((umm_block *)0)->body))) {
        return 1;
    }

    /*
     * If it's for more than that, then we need to figure out the number of
     * additional whole blocks the size of an umm_block are required.
     */

    size -= (1 + (sizeof(((umm_block *)0)->body)));

    return 2 + size / (sizeof(umm_block));
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
static void umm_split_block(
    umm_heap_context_t *_context,
    uint16_t c,
    uint16_t blocks,
    uint16_t new_freemask) {

    UMM_NBLOCK(c + blocks) = (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) | new_freemask;
    UMM_PBLOCK(c + blocks) = c;

    UMM_PBLOCK(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) = (c + blocks);
    UMM_NBLOCK(c) = (c + blocks);
}

/* ------------------------------------------------------------------------ */

static void umm_disconnect_from_free_list(umm_heap_context_t *_context, uint16_t c) {
    /* Disconnect this block from the FREE list */

    UMM_NFREE(UMM_PFREE(c)) = UMM_NFREE(c);
    UMM_PFREE(UMM_NFREE(c)) = UMM_PFREE(c);

    /* And clear the free block indicator */

    UMM_NBLOCK(c) &= (~UMM_FREELIST_MASK);
}

/* ------------------------------------------------------------------------
 * The umm_assimilate_up() function does not assume that UMM_NBLOCK(c)
 * has the UMM_FREELIST_MASK bit set. It only assimilates up if the
 * next block is free.
 */

static void umm_assimilate_up(umm_heap_context_t *_context, uint16_t c) {

    if (UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_FREELIST_MASK) {

        UMM_FRAGMENTATION_METRIC_REMOVE(UMM_NBLOCK(c));

        /*
         * The next block is a free block, so assimilate up and remove it from
         * the free list
         */

        DBGLOG_DEBUG("Assimilate up to next block, which is FREE\n");

        /* Disconnect the next block from the FREE list */

        umm_disconnect_from_free_list(_context, UMM_NBLOCK(c));

        /* Assimilate the next block with this one */

        UMM_PBLOCK(UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_BLOCKNO_MASK) = c;
        UMM_NBLOCK(c) = UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_BLOCKNO_MASK;
    }
}

/* ------------------------------------------------------------------------
 * The umm_assimilate_down() function assumes that UMM_NBLOCK(c) does NOT
 * have the UMM_FREELIST_MASK bit set. In other words, try to assimilate
 * up before assimilating down.
 */

static uint16_t umm_assimilate_down(umm_heap_context_t *_context, uint16_t c, uint16_t freemask) {

    // We are going to assimilate down to the previous block because
    // it was free, so remove it from the fragmentation metric

    UMM_FRAGMENTATION_METRIC_REMOVE(UMM_PBLOCK(c));

    UMM_NBLOCK(UMM_PBLOCK(c)) = UMM_NBLOCK(c) | freemask;
    UMM_PBLOCK(UMM_NBLOCK(c)) = UMM_PBLOCK(c);

    if (freemask) {
        // We are going to free the entire assimilated block
        // so add it to the fragmentation metric. A good
        // compiler will optimize away the empty if statement
        // when UMM_INFO is not defined, so don't worry about
        // guarding it.

        UMM_FRAGMENTATION_METRIC_ADD(UMM_PBLOCK(c));
    }

    return UMM_PBLOCK(c);
}

/* ------------------------------------------------------------------------- */

static void umm_init_stage_2(umm_heap_context_t *_context) {
    /* setup initial blank heap structure */
    UMM_FRAGMENTATION_METRIC_INIT();

    /* init stats.free_blocks */
    #if defined(UMM_STATS) || defined(UMM_STATS_FULL)
    #if defined(UMM_STATS_FULL)
    _context->stats.free_blocks_min =
        _context->stats.free_blocks_isr_min = UMM_NUMBLOCKS - 2;
    #endif
    #ifndef UMM_INLINE_METRICS
    _context->stats.free_blocks = UMM_NUMBLOCKS - 2;
    #endif
    #endif

    /* Set up umm_block[0], which just points to umm_block[1] */
    UMM_NBLOCK(0) = 1;
    UMM_NFREE(0) = 1;
    UMM_PFREE(0) = 1;

    /*
     * Now, we need to set the whole heap space as a huge free block. We should
     * not touch umm_block[0], since it's special: umm_block[0] is the head of
     * the free block list. It's a part of the heap invariant.
     *
     * See the detailed explanation at the beginning of the file.
     *
     * umm_block[1] has pointers:
     *
     * - next `umm_block`: the last one umm_block[n]
     * - prev `umm_block`: umm_block[0]
     *
     * Plus, it's a free `umm_block`, so we need to apply `UMM_FREELIST_MASK`
     *
     * And it's the last free block, so the next free block is 0 which marks
     * the end of the list. The previous block and free block pointer are 0
     * too, there is no need to initialize these values due to the init code
     * that memsets the entire umm_ space to 0.
     */
    UMM_NBLOCK(1) = UMM_BLOCK_LAST | UMM_FREELIST_MASK;

    /*
     * Last umm_block[n] has the next block index at 0, meaning it's
     * the end of the list, and the previous block is umm_block[1].
     *
     * The last block is a special block and can never be part of the
     * free list, so its pointers are left at 0 too.
     */

    UMM_PBLOCK(UMM_BLOCK_LAST) = 1;
}


void umm_init_common(size_t id, void *start_addr, size_t size, bool zero) {
    /* Preserve internal setup */
    umm_heap_context_t *_context = umm_get_heap_by_id(id);
    if (NULL == start_addr || NULL == _context || _context->heap) {
        return;
    }

    /* init heap pointer and size, and memset it to 0 */
    _context->id = id;
    _context->heap = (umm_block *)start_addr;
    _context->heap_end = (void *)((uintptr_t)start_addr + size);
    _context->numblocks = (size / sizeof(umm_block));

    // An option for blocking the zeroing of extra heaps allows for performing
    // post-crash discovery.
    if (zero) {
        memset(_context->heap, 0x00, size);
        #if (!defined(UMM_INLINE_METRICS) && defined(UMM_STATS)) || defined(UMM_STATS_FULL)
        memset(&_context->stats, 0x00, sizeof(_context->stats));
        #endif

        /* Set up internal data structures */
        umm_init_stage_2(_context);
    }
}

void umm_init(void) {
    // if (umm_heap) {
    //   return;
    // }
    for (size_t i = 0; i < UMM_NUM_HEAPS; i++) {
        heap_context[i].heap = NULL;
    }
    memset(&heap_context[0], 0, sizeof(heap_context));
    umm_init_common(UMM_HEAP_DRAM, (void *)UMM_MALLOC_CFG_HEAP_ADDR, UMM_MALLOC_CFG_HEAP_SIZE, true);
    // umm_heap = (void *)&heap_context;
}

#ifdef UMM_HEAP_IRAM
void umm_init_iram_ex(void *addr, unsigned int size, bool zero) {
    /* We need the main, internal heap set up first */
    UMM_INIT_HEAP;

    umm_init_common(UMM_HEAP_IRAM, addr, size, zero);
}

void _text_end(void);
void umm_init_iram(void) __attribute__((weak));

/*
  By using a weak link, it is possible to reduce the IRAM heap size with a
  user-supplied init function. This would allow the creation of a block of IRAM
  dedicated to a sketch and possibly used/preserved across reboots.
 */
void umm_init_iram(void) {
    umm_init_iram_ex(mmu_sec_heap(), mmu_sec_heap_size(), true);
}
#endif  // #ifdef UMM_HEAP_IRAM

#ifdef UMM_HEAP_EXTERNAL
void umm_init_vm(void *vmaddr, unsigned int vmsize) {
    /* We need the main, internal (DRAM) heap set up first */
    UMM_INIT_HEAP;

    umm_init_common(UMM_HEAP_EXTERNAL, vmaddr, vmsize, true);
}
#endif

/* ------------------------------------------------------------------------
 * Must be called only from within critical sections guarded by
 * UMM_CRITICAL_ENTRY() and UMM_CRITICAL_EXIT().
 */

static void umm_free_core(umm_heap_context_t *_context, void *ptr) {

    uint16_t c;

    if (NULL == _context) {
        panic();
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

    c = (((uintptr_t)ptr) - (uintptr_t)(&(_context->heap[0]))) / sizeof(umm_block);

    DBGLOG_DEBUG("Freeing block %6d\n", c);

    /* Update stats Free Block count */
    STATS__FREE_BLOCKS_UPDATE(UMM_NBLOCK(c) - c);

    /* Now let's assimilate this block with the next one if possible. */

    umm_assimilate_up(_context, c);

    /* Then assimilate with the previous block if possible */

    if (UMM_NBLOCK(UMM_PBLOCK(c)) & UMM_FREELIST_MASK) {

        DBGLOG_DEBUG("Assimilate down to previous block, which is FREE\n");

        c = umm_assimilate_down(_context, c, UMM_FREELIST_MASK);
    } else {
        /*
         * The previous block is not a free block, so add this one to the head
         * of the free list
         */
        UMM_FRAGMENTATION_METRIC_ADD(c);

        DBGLOG_DEBUG("Just add to head of free list\n");

        UMM_PFREE(UMM_NFREE(0)) = c;
        UMM_NFREE(c) = UMM_NFREE(0);
        UMM_PFREE(c)            = 0;
        UMM_NFREE(0) = c;

        UMM_NBLOCK(c) |= UMM_FREELIST_MASK;
    }
}

/* ------------------------------------------------------------------------ */

void umm_free(void *ptr) {
    UMM_CRITICAL_DECL(id_free);

    UMM_INIT_HEAP;

    /* If we're being asked to free a NULL pointer, well that's just silly! */

    if ((void *)0 == ptr) {
        DBGLOG_DEBUG("free a null pointer -> do nothing\n");
        STATS__NULL_FREE_REQUEST(id_free);

        return;
    }

    /* Free the memory within a protected critical section */

    UMM_CRITICAL_ENTRY(id_free);

    /* Need to be in the heap in which this block lives */
    umm_free_core(umm_get_ptr_context(ptr), ptr);

    UMM_CRITICAL_EXIT(id_free);
}

/* ------------------------------------------------------------------------
 * Must be called only from within critical sections guarded by
 * UMM_CRITICAL_ENTRY() and UMM_CRITICAL_EXIT().
 */

static void *umm_malloc_core(umm_heap_context_t *_context, size_t size) {
    uint16_t blocks;
    uint16_t blockSize = 0;

    uint16_t bestSize;
    uint16_t bestBlock;

    uint16_t cf;

    STATS__ALLOC_REQUEST(id_malloc, size);

    if (NULL == _context) {
        panic();
        return NULL;
    }

    blocks = umm_blocks(size);

    /*
     * Now we can scan through the free list until we find a space that's big
     * enough to hold the number of blocks we need.
     *
     * This part may be customized to be a best-fit, worst-fit, or first-fit
     * algorithm
     */

    cf = UMM_NFREE(0);

    bestBlock = UMM_NFREE(0);
    bestSize = 0x7FFF;

    while (cf) {
        blockSize = (UMM_NBLOCK(cf) & UMM_BLOCKNO_MASK) - cf;

        DBGLOG_TRACE("Looking at block %6d size %6d\n", cf, blockSize);

        #if defined UMM_BEST_FIT
        if ((blockSize >= blocks) && (blockSize < bestSize)) {
            bestBlock = cf;
            bestSize = blockSize;
        }
        #elif defined UMM_FIRST_FIT
        /* This is the first block that fits! */
        if ((blockSize >= blocks)) {
            break;
        }
        #else
        #error "No UMM_*_FIT is defined - check umm_malloc_cfg.h"
        #endif

        cf = UMM_NFREE(cf);
    }

    if (0x7FFF != bestSize) {
        cf = bestBlock;
        blockSize = bestSize;
    }

    POISON_CHECK_NEIGHBORS(cf);

    if (UMM_NBLOCK(cf) & UMM_BLOCKNO_MASK && blockSize >= blocks) {

        UMM_FRAGMENTATION_METRIC_REMOVE(cf);

        /*
         * This is an existing block in the memory heap, we just need to split off
         * what we need, unlink it from the free list and mark it as in use, and
         * link the rest of the block back into the freelist as if it was a new
         * block on the free list...
         */

        if (blockSize == blocks) {
            /* It's an exact fit and we don't need to split off a block. */
            DBGLOG_DEBUG("Allocating %6d blocks starting at %6d - exact\n", blocks, cf);

            /* Disconnect this block from the FREE list */

            umm_disconnect_from_free_list(_context, cf);

        } else {

            /* It's not an exact fit and we need to split off a block. */
            DBGLOG_DEBUG("Allocating %6d blocks starting at %6d - existing\n", blocks, cf);

            /*
             * split current free block `cf` into two blocks. The first one will be
             * returned to user, so it's not free, and the second one will be free.
             */
            umm_split_block(_context, cf, blocks, UMM_FREELIST_MASK /*new block is free*/);

            UMM_FRAGMENTATION_METRIC_ADD(UMM_NBLOCK(cf));

            /*
             * `umm_split_block()` does not update the free pointers (it affects
             * only free flags), but effectively we've just moved beginning of the
             * free block from `cf` to `cf + blocks`. So we have to adjust pointers
             * to and from adjacent free blocks.
             */

            /* previous free block */
            UMM_NFREE(UMM_PFREE(cf)) = cf + blocks;
            UMM_PFREE(cf + blocks) = UMM_PFREE(cf);

            /* next free block */
            UMM_PFREE(UMM_NFREE(cf)) = cf + blocks;
            UMM_NFREE(cf + blocks) = UMM_NFREE(cf);
        }

        STATS__FREE_BLOCKS_UPDATE(-blocks);
        STATS__FREE_BLOCKS_MIN();
    } else {
        /* Out of memory */
        STATS__OOM_UPDATE();

        DBGLOG_DEBUG("Can't allocate %5d blocks\n", blocks);

        return (void *)NULL;
    }

    return (void *)&UMM_DATA(cf);
}

/* ------------------------------------------------------------------------ */

void *umm_malloc(size_t size) {
    UMM_CRITICAL_DECL(id_malloc);

    void *ptr = NULL;

    UMM_INIT_HEAP;

    /*
     * "Is it safe"
     *
     * Is it safe to call from an ISR? Is there a point during a malloc that a
     * an interrupt and subsequent call to malloc result in undesired results?
     *
     * Heap selection in managed by the functions umm_push_heap, umm_pop_heap,
     * umm_get_current_heap_id, and umm_set_heap_by_id. These functions are
     * responsible for getting/setting the module static variable umm_heap_cur.
     * The umm_heap_cur variable is an index that is used to select the current
     * heap context. Depending on the situation this selection can be overriddened.
     *
     * All variables for a specific Heap are in a single structure. `heap_context`
     * is an array of these structures. Each heap API function uses a function
     * local variable `_context` to hold a pointer to the selected heap structure.
     * This local pointer is referenced for all the "selected heap" operations.
     * Coupled with critical sections around global data should allow the API
     * functions to be reentrant.
     *
     * Using the `_context` name throughout made it easy to incorporate the
     * context into existing macros.
     *
     * For allocating APIs `umm_heap_cur` is used to index and select a value for
     * `_context`. If an allocation is made from an ISR, this value is ignored and
     * the heap context for DRAM is loaded. For APIs that require operating on an
     * existing allocation such as realloc and free, the heap context selected is
     * done by matching the allocation's address with that of one of the heap
     * address ranges.
     *
     * I think we are safe with multiple heaps when the non32-bit exception
     * handler is used, as long as interrupts don't get enabled. There was a
     * window in the Boot ROM "C" Exception Wrapper that would enable interrupts
     * when running our non32-exception handler; however, that should be resolved
     * by our replacement wrapper. For more information on exception handling
     * issues for IRAM see comments above `_set_exception_handler_wrapper()` in
     * `core_esp8266_non32xfer.cpp`.
     *
     * ISRs should not try and change heaps. umm_malloc will ignore the change.
     * All should be fine as long as the caller puts the heap back the way it was.
     * On return, everything must be the same. The foreground thread will continue
     * with the same information that was there before the interrupt. All malloc()
     * requests made from an ISR are fulfilled with DRAM.
     *
     * For umm_malloc, heap selection involves changing a single variable that is
     * on the calling context stack. From the umm_mallac side, that variable is
     * used to load a context pointer by index, heap ID. While an umm_malloc API
     * function is running, all heap related variables are in the context variable
     * pointer, registers, or the current stack as the request is processed. With
     * a single variable to reference for heap selection, I think it is unlikely
     * that umm_malloc can be called, with things in an unusable transition state.
     */

    umm_heap_context_t *_context = umm_get_current_heap();

    /*
     * the very first thing we do is figure out if we're being asked to allocate
     * a size of 0 - and if we are we'll simply return a null pointer. if not
     * then reduce the size by 1 byte so that the subsequent calculations on
     * the number of blocks to allocate are easier...
     */

    if (0 == size) {
        DBGLOG_DEBUG("malloc a block of 0 bytes -> do nothing\n");
        STATS__ZERO_ALLOC_REQUEST(id_malloc, size);

        return ptr;
    }

    /* Allocate the memory within a protected critical section */

    UMM_CRITICAL_ENTRY(id_malloc);

    /*
     * We handle the realloc of an existing IRAM allocation from an ISR with IRAM,
     * while a new malloc from an ISR will always supply DRAM. That said, realloc
     * from an ISR is not generally safe without special locking mechanisms and is
     * not formally supported.
     *
     * Additionally, to avoid extending the IRQs disabled period, it is best to
     * use DRAM for an ISR. Each 16-bit access to IRAM that umm_malloc has to make
     * requires a pass through the exception handling logic.
     */
    if (UMM_CRITICAL_WITHINISR(id_malloc)) {
        _context = umm_get_heap_by_id(UMM_HEAP_DRAM);
    }

    ptr = umm_malloc_core(_context, size);

    UMM_CRITICAL_EXIT(id_malloc);

    return ptr;
}

/* ------------------------------------------------------------------------ */

void *umm_realloc(void *ptr, size_t size) {
    UMM_CRITICAL_DECL(id_realloc);

    uint16_t blocks;
    uint16_t blockSize;
    uint16_t prevBlockSize = 0;
    uint16_t nextBlockSize = 0;

    uint16_t c;

    size_t curSize;

    UMM_INIT_HEAP;

    /*
     * This code looks after the case of a NULL value for ptr. The ANSI C
     * standard says that if ptr is NULL and size is non-zero, then we've
     * got to work the same a malloc(). If size is also 0, then our version
     * of malloc() returns a NULL pointer, which is OK as far as the ANSI C
     * standard is concerned.
     */

    if (((void *)NULL == ptr)) {
        DBGLOG_DEBUG("realloc the NULL pointer - call malloc()\n");

        return umm_malloc(size);
    }

    /*
     * Now we're sure that we have a non_NULL ptr, but we're not sure what
     * we should do with it. If the size is 0, then the ANSI C standard says that
     * we should operate the same as free.
     */

    /* Need to be in the heap in which this block lives */
    umm_heap_context_t *_context = umm_get_ptr_context(ptr);
    if (NULL == _context) {
        panic();
        return NULL;
    }

    if (0 == size) {
        DBGLOG_DEBUG("realloc to 0 size, just free the block\n");
        STATS__ZERO_ALLOC_REQUEST(id_realloc, size);

        umm_free(ptr);

        return (void *)NULL;
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

    blocks = umm_blocks(size);

    /* Figure out which block we're in. Note the use of truncated division... */

    c = (((uintptr_t)ptr) - (uintptr_t)(&(_context->heap[0]))) / sizeof(umm_block);

    /* Figure out how big this block is ... the free bit is not set :-) */

    blockSize = (UMM_NBLOCK(c) - c);

    /* Figure out how many bytes are in this block */

    curSize = (blockSize * sizeof(umm_block)) - (sizeof(((umm_block *)0)->header));

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

    DBGLOG_DEBUG("realloc blocks %d blockSize %d nextBlockSize %d prevBlockSize %d\n", blocks, blockSize, nextBlockSize, prevBlockSize);

// C With each upstream update this section should be reevaluated.
/*C
 *
 * The `#if defined(UMM_REALLOC_MINIMIZE_COPY)` section tracks the content of
 * the upstream with some local macros added. Back when I made my 1st update to
 * umm_malloc PR, I found the upstream had been refactored and removed the
 * defragmenting properties that were originally present. It took some looking
 * to see the logic, it didn't have any comments to make it stand out.
 *
 * I added the `#elif defined(UMM_REALLOC_DEFRAG)` to recreate and preserve the
 * defragmenting functionality that was lost. This is the default build option
 * we have set in `umm_malloc_cfg.h`. I have not done any structured testing to
 * confirm; however, I think this to be the best option when considering the
 * amount of reallocates that can occur with the Strings library.
 */
    #if defined(UMM_REALLOC_MINIMIZE_COPY)
    /*
     * Ok, now that we're here we know how many blocks we want and the current
     * blockSize. The prevBlockSize and nextBlockSize are set and we can figure
     * out the best strategy for the new allocation as follows:
     *
     * 1. If the new block is the same size or smaller than the current block do
     *    nothing.
     * 2. If the next block is free and adding it to the current block gives us
     *    EXACTLY enough memory, assimilate the next block. This avoids unwanted
     *    fragmentation of free memory.
     *
     * The following cases may be better handled with memory copies to reduce
     * fragmentation
     *
     * 3. If the previous block is NOT free and the next block is free and
     *    adding it to the current block gives us enough memory, assimilate
     *    the next block. This may introduce a bit of fragmentation.
     * 4. If the prev block is free and adding it to the current block gives us
     *    enough memory, remove the previous block from the free list, assimilate
     *    it, copy to the new block.
     * 5. If the prev and next blocks are free and adding them to the current
     *    block gives us enough memory, assimilate the next block, remove the
     *    previous block from the free list, assimilate it, copy to the new block.
     * 6. Otherwise try to allocate an entirely new block of memory. If the
     *    allocation works free the old block and return the new pointer. If
     *    the allocation fails, return NULL and leave the old block intact.
     *
     * TODO: Add some conditional code to optimise for less fragmentation
     *       by simply allocating new memory if we need to copy anyways.
     *
     * All that's left to do is decide if the fit was exact or not. If the fit
     * was not exact, then split the memory block so that we use only the requested
     * number of blocks and add what's left to the free list.
     */

    //  Case 1 - block is same size or smaller
    if (blockSize >= blocks) {
        DBGLOG_DEBUG("realloc the same or smaller size block - %i, do nothing\n", blocks);
        /* This space intentionally left blank */

        //  Case 2 - block + next block fits EXACTLY
    } else if ((blockSize + nextBlockSize) == blocks) {
        DBGLOG_DEBUG("exact realloc using next block - %i\n", blocks);
        umm_assimilate_up(c);
        STATS__FREE_BLOCKS_UPDATE(-nextBlockSize);
        blockSize += nextBlockSize;

        //  Case 3 - prev block NOT free and block + next block fits
    } else if ((0 == prevBlockSize) && (blockSize + nextBlockSize) >= blocks) {
        DBGLOG_DEBUG("realloc using next block - %i\n", blocks);
        umm_assimilate_up(_context, c);
        STATS__FREE_BLOCKS_UPDATE(-nextBlockSize);
        blockSize += nextBlockSize;

        //  Case 4 - prev block + block fits
    } else if ((prevBlockSize + blockSize) >= blocks) {
        DBGLOG_DEBUG("realloc using prev block - %i\n", blocks);
        umm_disconnect_from_free_list(_context, UMM_PBLOCK(c));
        c = umm_assimilate_down(_context, c, 0);
        STATS__FREE_BLOCKS_UPDATE(-prevBlockSize);
        STATS__FREE_BLOCKS_ISR_MIN();
        blockSize += prevBlockSize;
        UMM_CRITICAL_SUSPEND(id_realloc);
        memmove((void *)&UMM_DATA(c), ptr, curSize);
        ptr = (void *)&UMM_DATA(c);
        UMM_CRITICAL_RESUME(id_realloc);
        //  Case 5 - prev block + block + next block fits
    } else if ((prevBlockSize + blockSize + nextBlockSize) >= blocks) {
        DBGLOG_DEBUG("realloc using prev and next block - %d\n", blocks);
        umm_assimilate_up(_context, c);
        umm_disconnect_from_free_list(_context, UMM_PBLOCK(c));
        c = umm_assimilate_down(_context, c, 0);
        STATS__FREE_BLOCKS_UPDATE(-prevBlockSize - nextBlockSize);
        #ifdef UMM_LIGHTWEIGHT_CPU
        if ((prevBlockSize + blockSize + nextBlockSize) > blocks) {
            umm_split_block(_context, c, blocks, 0);
            umm_free_core(_context, (void *)&UMM_DATA(c + blocks));
        }
        STATS__FREE_BLOCKS_ISR_MIN();
        blockSize = blocks;
        #else
        blockSize += (prevBlockSize + nextBlockSize);
        #endif
        UMM_CRITICAL_SUSPEND(id_realloc);
        memmove((void *)&UMM_DATA(c), ptr, curSize);
        ptr = (void *)&UMM_DATA(c);
        UMM_CRITICAL_RESUME(id_realloc);

        //  Case 6 - default is we need to realloc a new block
    } else {
        DBGLOG_DEBUG("realloc a completely new block %i\n", blocks);
        void *oldptr = ptr;
        if ((ptr = umm_malloc_core(_context, size))) {
            DBGLOG_DEBUG("realloc %i to a bigger block %i, copy, and free the old\n", blockSize, blocks);
            UMM_CRITICAL_SUSPEND(id_realloc);
            memcpy(ptr, oldptr, curSize);
            UMM_CRITICAL_RESUME(id_realloc);
            umm_free_core(_context, oldptr);
        } else {
            DBGLOG_DEBUG("realloc %i to a bigger block %i failed - return NULL and leave the old block!\n", blockSize, blocks);
            /* This space intentionally left blnk */
            /* STATS__OOM_UPDATE() has already been called by umm_malloc_core - don't duplicate count */
        }
        /* This is not accurate for OOM case; however, it will work for
         * stopping a call to free before return.
         */
        blockSize = blocks;
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
        umm_disconnect_from_free_list(_context, UMM_PBLOCK(c));
        c = umm_assimilate_down(_context, c, 0);
        STATS__FREE_BLOCKS_UPDATE(-prevBlockSize);
        blockSize += prevBlockSize;
        if (blockSize >= blocks) {
            DBGLOG_DEBUG("realloc using prev block - %d\n", blocks);
            STATS__FREE_BLOCKS_ISR_MIN();
        } else {
            DBGLOG_DEBUG("realloc using prev and next block - %d\n", blocks);
            umm_assimilate_up(_context, c);
            STATS__FREE_BLOCKS_UPDATE(-nextBlockSize);
            blockSize += nextBlockSize;
            #ifdef UMM_LIGHTWEIGHT_CPU
            if (blockSize > blocks) {
                umm_split_block(_context, c, blocks, 0);
                umm_free_core(_context, (void *)&UMM_DATA(c + blocks));
            }
            STATS__FREE_BLOCKS_ISR_MIN();
            blockSize = blocks;
            #endif
        }
        UMM_CRITICAL_SUSPEND(id_realloc);
        memmove((void *)&UMM_DATA(c), ptr, curSize);
        ptr = (void *)&UMM_DATA(c);
        UMM_CRITICAL_RESUME(id_realloc);
    } else if (blockSize >= blocks) { // 2
        DBGLOG_DEBUG("realloc the same or smaller size block - %d, do nothing\n", blocks);
        /* This space intentionally left blank */
    } else if ((blockSize + nextBlockSize) >= blocks) { // 3
        DBGLOG_DEBUG("realloc using next block - %d\n", blocks);
        umm_assimilate_up(_context, c);
        STATS__FREE_BLOCKS_UPDATE(-nextBlockSize);
        blockSize += nextBlockSize;
    } else { // 4
        DBGLOG_DEBUG("realloc a completely new block %d\n", blocks);
        void *oldptr = ptr;
        if ((ptr = umm_malloc_core(_context, size))) {
            DBGLOG_DEBUG("realloc %d to a bigger block %d, copy, and free the old\n", blockSize, blocks);
            UMM_CRITICAL_SUSPEND(id_realloc);
            memcpy(ptr, oldptr, curSize);
            UMM_CRITICAL_RESUME(id_realloc);
            umm_free_core(_context, oldptr);
        } else {
            DBGLOG_DEBUG("realloc %d to a bigger block %d failed - return NULL and leave the old block!\n", blockSize, blocks);
            /* This space intentionally left blnk */
            /* STATS__OOM_UPDATE() has already been called by umm_malloc_core - don't duplicate count */
        }
        /* This is not accurate for OOM case; however, it will work for
         * stopping a call to free before return.
         */
        blockSize = blocks;
    }
    #else
    #warning "Neither UMM_REALLOC_DEFRAG nor UMM_REALLOC_MINIMIZE_COPY is defined - check umm_malloc_cfg.h"
    /* An always copy option just for performance/fragmentation comparison */
    if (blockSize >= blocks) {
        DBGLOG_DEBUG("realloc the same or smaller size block - %d, do nothing\n", blocks);
        /* This space intentionally left blank */
    } else {
        DBGLOG_DEBUG("realloc a completely new block %d\n", blocks);
        void *oldptr = ptr;
        if ((ptr = umm_malloc_core(_context, size))) {
            DBGLOG_DEBUG("realloc %d to a bigger block %d, copy, and free the old\n", blockSize, blocks);
            UMM_CRITICAL_SUSPEND(id_realloc);
            memcpy(ptr, oldptr, curSize);
            UMM_CRITICAL_RESUME(id_realloc);
            umm_free_core(_context, oldptr);
        } else {
            DBGLOG_DEBUG("realloc %d to a bigger block %d failed - return NULL and leave the old block!\n", blockSize, blocks);
            /* This space intentionally left blnk */
            /* STATS__OOM_UPDATE() has already been called by umm_malloc_core - don't duplicate count */
        }
        /* This is not accurate for OOM case; however, it will work for
         * stopping a call to free before return.
         */
        blockSize = blocks;
    }
    #endif
    /* Now all we need to do is figure out if the block fit exactly or if we
     * need to split and free ...
     */

    if (blockSize > blocks) {
        DBGLOG_DEBUG("split and free %d blocks from %d\n", blocks, blockSize);
        umm_split_block(_context, c, blocks, 0);
        umm_free_core(_context, (void *)&UMM_DATA(c + blocks));
    }

    STATS__FREE_BLOCKS_MIN();

    /* Release the critical section... */
    UMM_CRITICAL_EXIT(id_realloc);

    return ptr;
}

/* ------------------------------------------------------------------------ */

void *umm_calloc(size_t num, size_t item_size) {
    void *ret;

    ret = umm_malloc((size_t)(item_size * num));

    if (ret) {
        memset(ret, 0x00, (size_t)(item_size * num));
    }

    return ret;
}

/* ------------------------------------------------------------------------ */

};
