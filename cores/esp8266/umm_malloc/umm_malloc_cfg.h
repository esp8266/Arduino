/*
 * Configuration for umm_malloc
 */

#ifndef _UMM_MALLOC_CFG_H
#define _UMM_MALLOC_CFG_H

#include <debug.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <osapi.h>

#include "c_types.h"
/*
 * There are a number of defines you can set at compile time that affect how
 * the memory allocator will operate.
 * You can set them in your config file umm_malloc_cfg.h.
 * In GNU C, you also can set these compile time defines like this:
 *
 * -D UMM_TEST_MAIN
 *
 * Set this if you want to compile in the test suite at the end of this file.
 *
 * If you leave this define unset, then you might want to set another one:
 *
 * -D UMM_REDEFINE_MEM_FUNCTIONS
 *
 * If you leave this define unset, then the function names are left alone as
 * umm_malloc() umm_free() and umm_realloc() so that they cannot be confused
 * with the C runtime functions malloc() free() and realloc()
 *
 * If you do set this define, then the function names become malloc()
 * free() and realloc() so that they can be used as the C runtime functions
 * in an embedded environment.
 *
 * -D UMM_BEST_FIT (defualt)
 *
 * Set this if you want to use a best-fit algorithm for allocating new
 * blocks
 *
 * -D UMM_FIRST_FIT
 *
 * Set this if you want to use a first-fit algorithm for allocating new
 * blocks
 *
 * -D UMM_DBG_LOG_LEVEL=n
 *
 * Set n to a value from 0 to 6 depending on how verbose you want the debug
 * log to be
 *
 * ----------------------------------------------------------------------------
 *
 * Support for this library in a multitasking environment is provided when
 * you add bodies to the UMM_CRITICAL_ENTRY and UMM_CRITICAL_EXIT macros
 * (see below)
 *
 * ----------------------------------------------------------------------------
 */

/////////////////////////////////////////////////
#ifdef DEBUG_ESP_OOM

#define MEMLEAK_DEBUG

// umm_*alloc are not renamed to *alloc

void *umm_malloc( size_t size );
void *umm_calloc( size_t num, size_t size );
void *umm_realloc( void *ptr, size_t size );
#define umm_free    free
#define umm_zalloc(s) umm_calloc(1,s)

void* malloc_loc (size_t s, const char* file, int line);
void* calloc_loc (size_t n, size_t s, const char* file, int line);
void* realloc_loc (void* p, size_t s, const char* file, int line);

// *alloc are macro calling *alloc_loc calling+checking umm_*alloc()
// they are defined at the bottom of this file

/////////////////////////////////////////////////
#else // !defined(ESP_DEBUG_OOM)

 // umm_*alloc are renamed to *alloc
 #define UMM_REDEFINE_MEM_FUNCTIONS

#endif

 #define UMM_BEST_FIT

/* Start addresses and the size of the heap */
extern char _heap_start;
#define UMM_MALLOC_CFG__HEAP_ADDR   ((uint32_t)&_heap_start)
#define UMM_MALLOC_CFG__HEAP_SIZE   ((size_t)(0x3fffc000 - UMM_MALLOC_CFG__HEAP_ADDR))

/* A couple of macros to make packing structures less compiler dependent */

#define UMM_H_ATTPACKPRE
#define UMM_H_ATTPACKSUF __attribute__((__packed__))

/*
 * A couple of macros to make it easier to protect the memory allocator
 * in a multitasking system. You should set these macros up to use whatever
 * your system uses for this purpose. You can disable interrupts entirely, or
 * just disable task switching - it's up to you
 *
 * NOTE WELL that these macros MUST be allowed to nest, because umm_free() is
 * called from within umm_malloc()
 */

/*
  Per Devyte, the core currently doesn't support masking a specific interrupt
  level. That doesn't mean it can't be implemented, only that at this time
  locking is implemented as all or nothing.
  https://github.com/esp8266/Arduino/issues/6246#issuecomment-508612609

  So for now we default to all, 15.
 */
#ifndef DEFAULT_CRITICAL_SECTION_INTLEVEL
#define DEFAULT_CRITICAL_SECTION_INTLEVEL 15
#endif

/*
 * -D UMM_CRITICAL_PERIOD_ANALYZE :
 *
 * Build option to collect timing usage data on critical section usage in
 * functions: info, malloc, realloc. Collects MIN, MAX, and number of time
 * IRQs were disabled at request time. Note, for realloc MAX disabled time
 * will not include the time from calling malloc and/or free.
 * Examine code for specifics on what info is available and how to access.
*/
// #define UMM_CRITICAL_PERIOD_ANALYZE

#ifndef __STRINGIFY
#define __STRINGIFY(a) #a
#endif
/*
  Copy paste xt_rsil and xt_wsr_ps from Arduino.h
 */
#ifndef xt_rsil
#define xt_rsil(level) (__extension__({uint32_t state; __asm__ __volatile__("rsil %0," __STRINGIFY(level) : "=a" (state) :: "memory"); state;}))
#endif
#ifndef xt_wsr_ps
#define xt_wsr_ps(state)  __asm__ __volatile__("wsr %0,ps; isync" :: "a" (state) : "memory")
#endif

#if !defined(UMM_CRITICAL_PERIOD_ANALYZE)
// This method preserves the intlevel on entry and restores the
// original intlevel at exit.
#define UMM_CRITICAL_DECL(tag) uint32_t _saved_ps_##tag
#define UMM_CRITICAL_ENTRY(tag) _saved_ps_##tag = xt_rsil(DEFAULT_CRITICAL_SECTION_INTLEVEL)
#define UMM_CRITICAL_EXIT(tag) xt_wsr_ps(_saved_ps_##tag)

#else
// This option adds support for gathering time locked data
typedef struct _TIME_STAT {
  uint32_t min;
  uint32_t max;
  uint32_t start;
  uint32_t intlevel;
} time_stat_t;

struct _UMM_TIME_STATS {
  time_stat_t id_malloc;
  time_stat_t id_realloc;
  time_stat_t id_free;
  time_stat_t id_info;
};

bool get_umm_get_perf_data(struct _UMM_TIME_STATS *p, size_t size);

static inline ICACHE_RAM_ATTR uint32_t _umm_get_cycle_count() {
  uint32_t ccount;
  // Not sure esync is needed before "rsr %0,CCOUNT". I don't see it in
  // Espressf SDK or Xtensa clock.S file.
  //  __asm__ __volatile__("esync; rsr %0,ccount":"=a"(ccount)::"memory");
  __asm__ __volatile__("rsr %0,ccount":"=a"(ccount)::"memory");
  return ccount;
}

static inline void _critical_entry(time_stat_t *p, uint32_t *saved_ps) {
    *saved_ps = xt_rsil(DEFAULT_CRITICAL_SECTION_INTLEVEL);
    if (0U != (*saved_ps & 0x0FU)) {
        p->intlevel += 1U;
    }

    p->start = _umm_get_cycle_count();
}

static inline void _critical_exit(time_stat_t *p, uint32_t *saved_ps) {
    uint32_t elapse = _umm_get_cycle_count() - p->start;
    if (elapse < p->min)
        p->min = elapse;

    if (elapse > p->max)
        p->max = elapse;

    xt_wsr_ps(*saved_ps);
}

#define UMM_CRITICAL_DECL(tag) uint32_t _saved_ps_##tag
#define UMM_CRITICAL_ENTRY(tag) _critical_entry(&time_stats.tag, &_saved_ps_##tag)
#define UMM_CRITICAL_EXIT(tag) _critical_exit(&time_stats.tag, &_saved_ps_##tag)

#endif

/*
 * -D UMM_INTEGRITY_CHECK :
 *
 * Enables heap integrity check before any heap operation. It affects
 * performance, but does NOT consume extra memory.
 *
 * If integrity violation is detected, the message is printed and user-provided
 * callback is called: `UMM_HEAP_CORRUPTION_CB()`
 *
 * Note that not all buffer overruns are detected: each buffer is aligned by
 * 4 bytes, so there might be some trailing "extra" bytes which are not checked
 * for corruption.
 */
/*
#define UMM_INTEGRITY_CHECK
*/

/*
 * -D UMM_POISON :
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
 *      Number of poison bytes before each block, e.g. 2
 *    UMM_POISON_SIZE_AFTER:
 *      Number of poison bytes after each block e.g. 2
 *    UMM_POISONED_BLOCK_LEN_TYPE
 *      Type of the exact buffer length, e.g. `short`
 *
 * NOTE: each allocated buffer is aligned by 4 bytes. But when poisoning is
 * enabled, actual pointer returned to user is shifted by
 * `(sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE)`.
 * It's your responsibility to make resulting pointers aligned appropriately.
 *
 * If poison corruption is detected, the message is printed and user-provided
 * callback is called: `UMM_HEAP_CORRUPTION_CB()`
 */

#if defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_CORE)
#define UMM_POISON
#endif

#define UMM_POISON_SIZE_BEFORE 4
#define UMM_POISON_SIZE_AFTER  4
#define UMM_POISONED_BLOCK_LEN_TYPE uint32_t

#define UMM_HEAP_CORRUPTION_CB() panic()

#ifdef __cplusplus
}
#endif

#endif /* _UMM_MALLOC_CFG_H */

#ifdef DEBUG_ESP_OOM
// this must be outside from "#ifndef _UMM_MALLOC_CFG_H"
// because Arduino.h's <cstdlib> does #undef *alloc
// Arduino.h recall us to redefine them
#include <pgmspace.h>
#define malloc(s) ({ static const char mem_debug_file[] PROGMEM STORE_ATTR = __FILE__; malloc_loc(s, mem_debug_file, __LINE__); })
#define calloc(n,s) ({ static const char mem_debug_file[] PROGMEM STORE_ATTR = __FILE__; calloc_loc(n, s, mem_debug_file, __LINE__); })
#define realloc(p,s) ({ static const char mem_debug_file[] PROGMEM STORE_ATTR = __FILE__; realloc_loc(p, s, mem_debug_file, __LINE__); })
#endif /* DEBUG_ESP_OOM */
