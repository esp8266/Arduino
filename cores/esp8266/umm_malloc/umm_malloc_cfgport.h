#ifndef _UMM_MALLOC_CFGPORT_H
#define _UMM_MALLOC_CFGPORT_H

#ifndef _UMM_MALLOC_CFG_H
#error "This include file must be used with umm_malloc_cfg.h"
#endif

/*
 * Arduino ESP8266 core umm_malloc port config
 */
#include <pgmspace.h>
#include <mmu_iram.h>
#include "../debug.h"
#include "../esp8266_undocumented.h"

#include <core_esp8266_features.h>
#include <stdlib.h>
#include <osapi.h>

#include "c_types.h"

/*
 * Between UMM_BEST_FIT or UMM_FIRST_FIT, UMM_BEST_FIT is the better option for
 * reducing heap fragmentation. With no selection made, UMM_BEST_FIT is used.
 * See umm_malloc_cfg.h for more information.
 */

/*
 * -DUMM_INIT_USE_IRAM
 *
 * Historically, the umm_init() call path has been in IRAM. The umm_init() call
 * path is now in ICACHE (flash). Use the build option UMM_INIT_USE_IRAM to
 * restore the legacy behavor.
 *
 * If you have your own app_entry_redefinable() function, see
 * app_entry_redefinable() in core_esp8266_app_entry_noextra4k.cpp for an
 * example of how to toggle between ICACHE and IRAM in your build.
 *
 * The default is to use ICACHE.
 */
// #define UMM_INIT_USE_IRAM 1


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

#endif
