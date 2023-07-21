/*
 * Arduino ESP8266 core umm_malloc port config
 */

#ifdef _UMM_MALLOC_CFG_H
// Additional includes for "umm_malloc_cfg.h" only
#include <pgmspace.h>
#include <mmu_iram.h>
#include "../debug.h"
#include "../esp8266_undocumented.h"

#include <core_esp8266_features.h>
#include <stdlib.h>
#include <osapi.h>

#include "c_types.h"
#endif


#ifndef _UMM_MALLOC_CFGPORT_H
#define _UMM_MALLOC_CFGPORT_H

/*
 * Between UMM_BEST_FIT or UMM_FIRST_FIT, UMM_BEST_FIT is the better option for
 * reducing heap fragmentation. With no selection made, UMM_BEST_FIT is used.
 * See umm_malloc_cfg.h for more information.
 */

/*
 * -DUMM_INIT_USE_ICACHE
 *
 * Historically, the umm_init() call path has been in IRAM. The umm_init() call
 * path is now in ICACHE (flash). Use the build option UMM_INIT_USE_IRAM to
 * restore the legacy behavor.
 *
 * If you have your own app_entry_redefinable() function, see
 * app_entry_redefinable() in core_esp8266_app_entry_noextra4k.cpp for an
 * example of how to toggle between ICACHE and IRAM in your build.
 *
 * ~The default is to use ICACHE.~
 * For now revert default back to IRAM
 * define UMM_INIT_USE_ICACHE to use ICACHE/IROM
 */
#ifdef UMM_INIT_USE_ICACHE
#undef UMM_INIT_USE_IRAM
#else
#undef UMM_INIT_USE_IRAM
#define UMM_INIT_USE_IRAM 1
#endif

/*
 * Start addresses and the size of the heap
 */
extern char _heap_start[];
#define UMM_HEAP_END_ADDR          0x3FFFC000UL
#define UMM_MALLOC_CFG_HEAP_ADDR   ((uint32_t)&_heap_start[0])
#define UMM_MALLOC_CFG_HEAP_SIZE   ((size_t)(UMM_HEAP_END_ADDR - UMM_MALLOC_CFG_HEAP_ADDR))

/*
 * Define active Heaps
 */
#if defined(MMU_IRAM_HEAP)
#define UMM_HEAP_IRAM
#else
#undef UMM_HEAP_IRAM
#endif

#if defined(MMU_EXTERNAL_HEAP)
#define UMM_HEAP_EXTERNAL
#else
#undef UMM_HEAP_EXTERNAL
#endif

/*
 * Assign IDs to active Heaps and tally. DRAM is always active.
 */
#define UMM_HEAP_DRAM 0
#define UMM_HEAP_DRAM_DEFINED 1

#ifdef UMM_HEAP_IRAM
#undef UMM_HEAP_IRAM
#define UMM_HEAP_IRAM_DEFINED 1
#define UMM_HEAP_IRAM UMM_HEAP_DRAM_DEFINED
#else
#define UMM_HEAP_IRAM_DEFINED 0
#endif

#ifdef UMM_HEAP_EXTERNAL
#undef UMM_HEAP_EXTERNAL
#define UMM_HEAP_EXTERNAL_DEFINED 1
#define UMM_HEAP_EXTERNAL (UMM_HEAP_DRAM_DEFINED + UMM_HEAP_IRAM_DEFINED)
#else
#define UMM_HEAP_EXTERNAL_DEFINED 0
#endif

#define UMM_NUM_HEAPS (UMM_HEAP_DRAM_DEFINED + UMM_HEAP_IRAM_DEFINED + UMM_HEAP_EXTERNAL_DEFINED)

#if (UMM_NUM_HEAPS == 1)
#else
#define UMM_HEAP_STACK_DEPTH 32
#endif

/*
 * The NONOS SDK API requires function `umm_info()` for implementing
 * `system_show_malloc()`. Build option `-DUMM_INFO` enables this support.
 *
 * Also, `-DUMM_INFO` is needed to support several EspClass methods.
 * Partial EspClass method list:
 *   `uint32_t EspClass::getMaxFreeBlockSize()`
 *   `void EspClass::getHeapStats(uint32_t* hfree, uint32_t* hmax, uint8_t* hfrag)`
 *   `uint8_t EspClass::getHeapFragmentation()`
 *
 * The NONOS SDK API requires an ISR safe function to call for implementing
 * `xPortGetFreeHeapSize()`. Use one of these options:
 *  1) `-DUMM_STATS` or `-DUMM_STATS_FULL`
 *  2) `-DUMM_INLINE_METRICS` (implicitly includes `-DUMM_INFO`)
 *
 * If frequent calls are made to `ESP.getHeapFragmentation()`, using build
 * option `-DUMM_INLINE_METRICS` would reduce long periods of interrupts
 * disabled caused by frequent calls to `umm_info().` Instead, the computations
 * get distributed across each malloc, realloc, and free. Requires approximately
 * 116 more bytes of IRAM when compared to the build option `-DUMM_STATS` with
 * `-DUMM_INFO.`
 *
 * When both `-DUMM_STATS` and `-DUMM_INLINE_METRICS` are defined, macros and
 * structures are optimized to reduce duplications.
 *
 * You can use `-DUMM_INFO` with `-DUMM_INLINE_METRICS` and drop
 * `-DUMM_STATS(_FULL)` gaining back some IROM at the expense of IRAM.
 *
 * If you don't require the methods in EspClass that are dependent on functions
 * from the `-DUMM_INFO` build option, you can use only `-DUMM_STATS` and save
 * on IROM and a little IRAM.
 *
 */
#if defined(UMM_STATS) || defined(UMM_STATS_FULL) || defined(UMM_INLINE_METRICS) || defined(UMM_INFO)
/*
  User defined via build options eg. Sketch.ino.globals.h
*/
#else
/*
  Set expected/implicit defaults for complete support of EspClass methods.
*/
#define UMM_INFO 1
#define UMM_STATS 1
#endif

/*
  For `-Dname`, gcc assigns a value of 1 and this works fine; however,
  if `-Dname=0` is used, the intended results will not be obtained.

  Make value and valueless defines compliant with their usage in umm_malloc:
    `#define name`   => #define name 1
    `#define name 0` => #undef name
*/
#if ((1 - UMM_BEST_FIT - 1) == 2)
// When UMM_BEST_FIT is defined w/o value, the computation becomes
// (1 - - 1) == 2    =>     (1 + 1) == 2
#undef UMM_BEST_FIT
#define UMM_BEST_FIT 1
#elif ((1 - UMM_BEST_FIT - 1) == 0)
#undef UMM_BEST_FIT
#endif
#if ((1 - UMM_FIRST_FIT - 1) == 2)
#undef UMM_FIRST_FIT
#define UMM_FIRST_FIT 1
#elif ((1 - UMM_FIRST_FIT - 1) == 0)
#undef UMM_FIRST_FIT
#endif

#if ((1 - UMM_INFO - 1) == 2)
#undef UMM_INFO
#define UMM_INFO 1
#elif ((1 - UMM_INFO - 1) == 0)
#undef UMM_INFO
#endif
#if ((1 - UMM_INLINE_METRICS - 1) == 2)
#undef UMM_INLINE_METRICS
#define UMM_INLINE_METRICS 1
#elif ((1 - UMM_INLINE_METRICS - 1) == 0)
#undef UMM_INLINE_METRICS
#endif

#if ((1 - UMM_STATS - 1) == 2)
#undef UMM_STATS
#define UMM_STATS 1
#elif ((1 - UMM_STATS - 1) == 0)
#undef UMM_STATS
#endif
#if ((1 - UMM_STATS_FULL - 1) == 2)
#undef UMM_STATS_FULL
#define UMM_STATS_FULL 1
#elif ((1 - UMM_STATS_FULL - 1) == 0)
#undef UMM_STATS_FULL
#endif


#if defined(UMM_INLINE_METRICS)
// Dependent on UMM_INFO if missing enable.
#ifndef UMM_INFO
#define UMM_INFO 1
#endif
#endif

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
// We have support for free Heap size
#if defined(UMM_STATS) && defined(UMM_STATS_FULL)
#error "Build option conflict, specify either UMM_STATS or UMM_STATS_FULL."
#endif
#elif defined(UMM_INFO)
// ensure fallback support for free Heap size
#ifndef UMM_INLINE_METRICS
#define UMM_INLINE_METRICS 1
#endif
#else
#error "Specify at least one of these build options: (UMM_STATS or UMM_STATS_FULL) and/or UMM_INFO and/or UMM_INLINE_METRICS"
#endif


////////////////////////////////////////////////////////////////////////////////
/*
 * -D UMM_POISON_CHECK :
 * -D UMM_POISON_CHECK_LITE :
 * -D UMM_POISON_NONE
 *
 * Enables heap poisoning: add predefined value (poison) before and after each
 * allocation, and check before each heap operation that no poison is
 * corrupted.
 *
 * Other than the poison itself, we need to store exact user-requested length
 * for each buffer, so that overrun by just 1 byte will be always noticed.
 *
 * Customizations:
 *
 *    UMM_POISON_SIZE_BEFORE:
 *      Number of poison bytes before each block, e.g. 4
 *    UMM_POISON_SIZE_AFTER:
 *      Number of poison bytes after each block e.g. 4
 *    UMM_POISONED_BLOCK_LEN_TYPE
 *      Type of the exact buffer length, e.g. `uint16_t`
 *
 * NOTE: each allocated buffer is aligned by 4 bytes. But when poisoning is
 * enabled, actual pointer returned to user is shifted by
 * `(sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE)`.
 * It's your responsibility to make resulting pointers aligned appropriately.
 *
 * If poison corruption is detected, the message is printed and user-provided
 * callback is called: `UMM_HEAP_CORRUPTION_CB()`
 *
 * UMM_POISON_CHECK - does a global heap check on all active allocation at
 * every alloc API call. May exceed 10us due to critical section with IRQs
 * disabled.
 *
 * UMM_POISON_CHECK_LITE - checks the allocation presented at realloc()
 * and free(). Expands the poison check on the current allocation to
 * include its nearest allocated neighbors in the heap.
 * umm_malloc() will also checks the neighbors of the selected allocation
 * before use.
 *
 * UMM_POISON_NONE - No UMM_POISON... checking.
 *
 * Status: TODO?: UMM_POISON_CHECK_LITE is a new option. We could propose for
 * upstream; however, the upstream version has much of the framework for calling
 * poison check on each alloc call refactored out. Not sure how this will be
 * received.
 */

/*
 * Compatibility for deprecated UMM_POISON
 */
#if defined(UMM_POISON) && !defined(UMM_POISON_CHECK) && !defined(UMM_POISON_NONE)
#define UMM_POISON_CHECK_LITE
#endif

#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_CORE)
#if !defined(UMM_POISON_CHECK) && !defined(UMM_POISON_CHECK_LITE) && !defined(UMM_POISON_NONE)
/*
#define UMM_POISON_CHECK
 */
#define UMM_POISON_CHECK_LITE
#endif
#endif

#if defined(UMM_POISON_CHECK) && defined(UMM_POISON_CHECK_LITE)
// There can only be one.
#error "Build options UMM_POISON_NONE, UMM_POISON_CHECK and UMM_POISON_CHECK_LITE are mutually exclusive."
#endif

#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
#if defined(UMM_POISON_NONE)
#error "Build options UMM_POISON_NONE, UMM_POISON_CHECK and UMM_POISON_CHECK_LITE are mutually exclusive."
#endif

#define UMM_POISON_SIZE_BEFORE (4)
#define UMM_POISON_SIZE_AFTER (4)
#define UMM_POISONED_BLOCK_LEN_TYPE uint32_t

extern void *umm_poison_malloc(size_t size);
extern void *umm_poison_calloc(size_t num, size_t size);
#endif

#if defined(UMM_POISON_CHECK_LITE)
/*
 * Local Additions to better report location in code of the caller.
 *
 * We can safely do individual poison checks at free and realloc and stay
 * under 10us or close.
 */
extern void *umm_poison_realloc_flc(void *ptr, size_t size, const char *file, int line, const void *caller);
extern void  umm_poison_free_flc(void *ptr, const char *file, int line, const void *caller);
#define POISON_CHECK_SET_POISON(p, s) get_poisoned(p, s)
#define POISON_CHECK_SET_POISON_BLOCKS(p, s) \
    do { \
        size_t super_size = (s * sizeof(umm_block)) - (sizeof(((umm_block *)0)->header)); \
        get_poisoned(p, super_size); \
    } while (false)
#define UMM_POISON_SKETCH_PTR(p) ((void *)((uintptr_t)p + sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE))
#define UMM_POISON_SKETCH_PTRSZ(p) (*(UMM_POISONED_BLOCK_LEN_TYPE *)p)
#define UMM_POISON_MEMMOVE(t, p, s) memmove(UMM_POISON_SKETCH_PTR(t), UMM_POISON_SKETCH_PTR(p), UMM_POISON_SKETCH_PTRSZ(p))
#define UMM_POISON_MEMCPY(t, p, s) memcpy(UMM_POISON_SKETCH_PTR(t), UMM_POISON_SKETCH_PTR(p), UMM_POISON_SKETCH_PTRSZ(p))

// No meaningful information is conveyed with panic() for fail. Save space used abort().
#define POISON_CHECK_NEIGHBORS(c) \
    do { \
        if (!check_poison_neighbors(_context, c)) { \
            DBGLOG_ERROR("This bad block is in a neighbor allocation near free memory %p\n", (void *)&UMM_BLOCK(c)); \
            abort(); \
        } \
    } while (false)
/*
 * Nullify any POISON_CHECK for UMM_POISON_CHECK builds.
 */
#define POISON_CHECK() 1

#elif defined(UMM_POISON_CHECK)
extern void *umm_poison_realloc(void *ptr, size_t size);
extern void  umm_poison_free(void *ptr);
extern bool  umm_poison_check(void);
#define POISON_CHECK_SET_POISON(p, s) get_poisoned(p, s)
#define POISON_CHECK_SET_POISON_BLOCKS(p, s) \
    do { \
        size_t super_size = (s * sizeof(umm_block)) - (sizeof(((umm_block *)0)->header)); \
        get_poisoned(p, super_size); \
    } while (false)
#define UMM_POISON_SKETCH_PTR(p) ((void *)((uintptr_t)p + sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE))
#define UMM_POISON_SKETCH_PTRSZ(p) (*(UMM_POISONED_BLOCK_LEN_TYPE *)p)
#define UMM_POISON_MEMMOVE(t, p, s) memmove(UMM_POISON_SKETCH_PTR(t), UMM_POISON_SKETCH_PTR(p), UMM_POISON_SKETCH_PTRSZ(p))
#define UMM_POISON_MEMCPY(t, p, s) memcpy(UMM_POISON_SKETCH_PTR(t), UMM_POISON_SKETCH_PTR(p), UMM_POISON_SKETCH_PTRSZ(p))

/* Not normally enabled. A full heap poison check may exceed 10us. */
#define POISON_CHECK() umm_poison_check()
#define POISON_CHECK_NEIGHBORS(c) do {} while (false)

#else
#define POISON_CHECK() 1
#define POISON_CHECK_NEIGHBORS(c) do {} while (false)
#define POISON_CHECK_SET_POISON(p, s) (p)
#define POISON_CHECK_SET_POISON_BLOCKS(p, s)
#define UMM_POISON_MEMMOVE(t, p, s) memmove((t), (p), (s))
#define UMM_POISON_MEMCPY(t, p, s) memcpy((t), (p), (s))
#endif


#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
/*
 * Overhead adjustments needed for free_blocks to express the number of bytes
 * that can actually be allocated.
 */
#define UMM_OVERHEAD_ADJUST ( \
    umm_block_size() / 2 + \
    UMM_POISON_SIZE_BEFORE + \
    UMM_POISON_SIZE_AFTER + \
    sizeof(UMM_POISONED_BLOCK_LEN_TYPE))

#else
#define UMM_OVERHEAD_ADJUST  (umm_block_size() / 2)
#endif

#endif
