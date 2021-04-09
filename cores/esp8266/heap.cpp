/* heap.c - overrides of SDK heap handling functions
 * Copyright (c) 2016 Ivan Grokhotkov. All rights reserved.
 * This file is distributed under MIT license.
 */

#include <stdlib.h>
#include "umm_malloc/umm_malloc.h"

// Need FORCE_ALWAYS_INLINE to put HeapSelect class constructor/deconstructor in IRAM
#define FORCE_ALWAYS_INLINE_HEAP_SELECT
#include "umm_malloc/umm_heap_select.h"

#include <c_types.h>
#include <sys/reent.h>
#include <user_interface.h>

extern "C" {

#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
#define UMM_MALLOC(s)           umm_poison_malloc(s)
#define UMM_CALLOC(n,s)         umm_poison_calloc(n,s)
#define UMM_REALLOC_FL(p,s,f,l) umm_poison_realloc_fl(p,s,f,l)
#define UMM_FREE_FL(p,f,l)      umm_poison_free_fl(p,f,l)
#define STATIC_ALWAYS_INLINE

#undef realloc
#undef free

#elif defined(DEBUG_ESP_OOM) || defined(UMM_INTEGRITY_CHECK)
#define UMM_MALLOC(s)           umm_malloc(s)
#define UMM_CALLOC(n,s)         umm_calloc(n,s)
#define UMM_REALLOC_FL(p,s,f,l) umm_realloc(p,s)
#define UMM_FREE_FL(p,f,l)      umm_free(p)
#define STATIC_ALWAYS_INLINE

#undef realloc
#undef free

#else  // ! UMM_POISON_CHECK && ! DEBUG_ESP_OOM
#define UMM_MALLOC(s)           malloc(s)
#define UMM_CALLOC(n,s)         calloc(n,s)
#define UMM_REALLOC_FL(p,s,f,l) realloc(p,s)
#define UMM_FREE_FL(p,f,l)      free(p)

// STATIC_ALWAYS_INLINE only applys to the non-debug build path,
// it must not be enabled on the debug build path.
#define STATIC_ALWAYS_INLINE static ALWAYS_INLINE
#endif


#if defined(UMM_POISON_CHECK)
  #define POISON_CHECK__ABORT() \
      do { \
          if ( ! POISON_CHECK() ) \
              abort(); \
      } while(0)

  #define POISON_CHECK__PANIC_FL(file, line) \
      do { \
          if ( ! POISON_CHECK() ) \
              __panic_func(file, line, ""); \
      } while(0)

#else // No full heap poison checking.
  #define POISON_CHECK__ABORT() do {} while(0)
  #define POISON_CHECK__PANIC_FL(file, line) do { (void)file; (void)line; } while(0)
#endif

// Debugging helper, last allocation which returned NULL
void *umm_last_fail_alloc_addr = NULL;
int umm_last_fail_alloc_size = 0;
#if defined(DEBUG_ESP_OOM)
const char *umm_last_fail_alloc_file = NULL;
int umm_last_fail_alloc_line = 0;
#endif

#ifdef UMM_INTEGRITY_CHECK
#define INTEGRITY_CHECK__ABORT() \
    do { \
        if ( ! INTEGRITY_CHECK() ) \
            abort(); \
    } while(0)

#define INTEGRITY_CHECK__PANIC_FL(file, line) \
    do { \
        if ( ! INTEGRITY_CHECK() ) \
            __panic_func(file, line, ""); \
    } while(0)

#else  // ! UMM_INTEGRITY_CHECK
#define INTEGRITY_CHECK__ABORT() do {} while(0)
#define INTEGRITY_CHECK__PANIC_FL(file, line) do { (void)file; (void)line; } while(0)

#endif //   UMM_INTEGRITY_CHECK

#if defined(DEBUG_ESP_OOM)
#define PTR_CHECK__LOG_LAST_FAIL_FL(p, s, f, l) \
    if(0 != (s) && 0 == p)\
    {\
      umm_last_fail_alloc_addr = __builtin_return_address(0);\
      umm_last_fail_alloc_size = s;\
      umm_last_fail_alloc_file = f;\
      umm_last_fail_alloc_line = l;\
    }
#define PTR_CHECK__LOG_LAST_FAIL(p, s) \
    if(0 != (s) && 0 == p)\
    {\
      umm_last_fail_alloc_addr = __builtin_return_address(0);\
      umm_last_fail_alloc_size = s;\
      umm_last_fail_alloc_file = NULL;\
      umm_last_fail_alloc_line = 0;\
    }
#else
#define PTR_CHECK__LOG_LAST_FAIL_FL(p, s, f, l) \
    (void)f;\
    (void)l;\
    if(0 != (s) && 0 == p)\
    {\
      umm_last_fail_alloc_addr = __builtin_return_address(0);\
      umm_last_fail_alloc_size = s;\
    }
#define PTR_CHECK__LOG_LAST_FAIL(p, s) \
    if(0 != (s) && 0 == p)\
    {\
      umm_last_fail_alloc_addr = __builtin_return_address(0);\
      umm_last_fail_alloc_size = s;\
    }
#endif

void* _malloc_r(struct _reent* unused, size_t size)
{
    (void) unused;
    void *ret = malloc(size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size);
    return ret;
}

void _free_r(struct _reent* unused, void* ptr)
{
    (void) unused;
    free(ptr);
}

void* _realloc_r(struct _reent* unused, void* ptr, size_t size)
{
    (void) unused;
    void *ret = realloc(ptr, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size);
    return ret;
}

void* _calloc_r(struct _reent* unused, size_t count, size_t size)
{
    (void) unused;
    void *ret = calloc(count, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, count * size);
    return ret;
}

#ifdef DEBUG_ESP_OOM
#undef malloc
#undef calloc
#undef realloc

#define DEBUG_HEAP_PRINTF ets_uart_printf

void IRAM_ATTR print_loc(size_t size, const char* file, int line)
{
    (void)size;
    (void)line;
    if (system_get_os_print()) {
        DEBUG_HEAP_PRINTF(":oom(%d)@", (int)size);

        bool inISR = ETS_INTR_WITHINISR();
        if (inISR && (uint32_t)file >= 0x40200000) {
            DEBUG_HEAP_PRINTF("File: %p", file);
        } else if (!inISR && (uint32_t)file >= 0x40200000) {
            char buf[strlen_P(file) + 1];
            strcpy_P(buf, file);
            DEBUG_HEAP_PRINTF(buf);
        } else {
            DEBUG_HEAP_PRINTF(file);
        }

        DEBUG_HEAP_PRINTF(":%d\n", line);
    }
}

void IRAM_ATTR print_oom_size(size_t size)
{
    (void)size;
    if (system_get_os_print()) {
        DEBUG_HEAP_PRINTF(":oom(%d)@?\n", (int)size);
    }
}

#define OOM_CHECK__PRINT_OOM(p, s) if ((s) && !(p)) print_oom_size(s)
#define OOM_CHECK__PRINT_LOC(p, s, f, l) if ((s) && !(p)) print_loc(s, f, l)

#else  // ! DEBUG_ESP_OOM

#if 1
//C - to be discussed - is this what you want?
//C Skip OOM logging of last fail for malloc/... and pvPort... .
//C It cost 64 more bytes of IRAM to turn on. And was not previously enabled.
#undef PTR_CHECK__LOG_LAST_FAIL_FL
#define PTR_CHECK__LOG_LAST_FAIL_FL(p, s, f, l)
#undef PTR_CHECK__LOG_LAST_FAIL
#define PTR_CHECK__LOG_LAST_FAIL(p, s)
#endif

#define OOM_CHECK__PRINT_OOM(p, s)
#define OOM_CHECK__PRINT_LOC(p, s, f, l)
#endif

#if defined(DEBUG_ESP_OOM) || defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE) || defined(UMM_INTEGRITY_CHECK)
/*
  The thinking behind the ordering of Integrity Check, Full Poison Check, and
  the specific *alloc function.

  1. Integrity Check - verifies the heap management information is not corrupt.
     This allows any other testing, that walks the heap, to run safely.

  2. Place Full Poison Check before or after a specific *alloc function?
     a. After, when the *alloc function operates on an existing allocation.
     b. Before, when the *alloc function creates a new, not modified, allocation.

     In a free() or realloc() call, the focus is on their allocation. It is
     checked 1st and reported on 1ST if an error exists. Full Posion Check is
     done after.

     For malloc(), calloc(), and zalloc() Full Posion Check is done 1st since
     these functions do not modify an existing allocation.
*/
void* IRAM_ATTR malloc(size_t size)
{
    INTEGRITY_CHECK__ABORT();
    POISON_CHECK__ABORT();
    void* ret = UMM_MALLOC(size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size);
    OOM_CHECK__PRINT_OOM(ret, size);
    return ret;
}

void* IRAM_ATTR calloc(size_t count, size_t size)
{
    INTEGRITY_CHECK__ABORT();
    POISON_CHECK__ABORT();
    void* ret = UMM_CALLOC(count, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, count * size);
    OOM_CHECK__PRINT_OOM(ret, size);
    return ret;
}

void* IRAM_ATTR realloc(void* ptr, size_t size)
{
    INTEGRITY_CHECK__ABORT();
    void* ret = UMM_REALLOC_FL(ptr, size, NULL, 0);
    POISON_CHECK__ABORT();
    PTR_CHECK__LOG_LAST_FAIL(ret, size);
    OOM_CHECK__PRINT_OOM(ret, size);
    return ret;
}

void IRAM_ATTR free(void* p)
{
    INTEGRITY_CHECK__ABORT();
    UMM_FREE_FL(p, NULL, 0);
    POISON_CHECK__ABORT();
}
#endif

STATIC_ALWAYS_INLINE
void* IRAM_ATTR heap_pvPortMalloc(size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    POISON_CHECK__PANIC_FL(file, line);
    void* ret = UMM_MALLOC(size);
    PTR_CHECK__LOG_LAST_FAIL_FL(ret, size, file, line);
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

STATIC_ALWAYS_INLINE
void* IRAM_ATTR heap_pvPortCalloc(size_t count, size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    POISON_CHECK__PANIC_FL(file, line);
    void* ret = UMM_CALLOC(count, size);
    PTR_CHECK__LOG_LAST_FAIL_FL(ret, count * size, file, line);
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

STATIC_ALWAYS_INLINE
void* IRAM_ATTR heap_pvPortRealloc(void *ptr, size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    void* ret = UMM_REALLOC_FL(ptr, size, file, line);
    POISON_CHECK__PANIC_FL(file, line);
    PTR_CHECK__LOG_LAST_FAIL_FL(ret, size, file, line);
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

STATIC_ALWAYS_INLINE
void* IRAM_ATTR heap_pvPortZalloc(size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    POISON_CHECK__PANIC_FL(file, line);
    void* ret = UMM_CALLOC(1, size);
    PTR_CHECK__LOG_LAST_FAIL_FL(ret, size, file, line);
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

STATIC_ALWAYS_INLINE
void IRAM_ATTR heap_vPortFree(void *ptr, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    UMM_FREE_FL(ptr, file, line);
    POISON_CHECK__PANIC_FL(file, line);
}

size_t IRAM_ATTR xPortWantedSizeAlign(size_t size)
{
    return (size + 3) & ~((size_t) 3);
}

void system_show_malloc(void)
{
    HeapSelectDram ephemeral;
    umm_info(NULL, true);
}

/*
  NONOS SDK and lwIP do not handle IRAM heap well. Since they also use portable
  malloc calls pvPortMalloc, ... we can leverage that for this solution.
  Force pvPortMalloc, ... APIs to serve DRAM only.
*/
void* IRAM_ATTR pvPortMalloc(size_t size, const char* file, int line)
{
    HeapSelectDram ephemeral;
    return heap_pvPortMalloc(size,  file, line);;
}

void* IRAM_ATTR pvPortCalloc(size_t count, size_t size, const char* file, int line)
{
    HeapSelectDram ephemeral;
    return heap_pvPortCalloc(count, size,  file, line);
}

void* IRAM_ATTR pvPortRealloc(void *ptr, size_t size, const char* file, int line)
{
    HeapSelectDram ephemeral;
    return heap_pvPortRealloc(ptr, size,  file, line);
}

void* IRAM_ATTR pvPortZalloc(size_t size, const char* file, int line)
{
    HeapSelectDram ephemeral;
    return heap_pvPortZalloc(size,  file, line);
}

void IRAM_ATTR vPortFree(void *ptr, const char* file, int line)
{
#if defined(DEBUG_ESP_OOM) || defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE) || defined(UMM_INTEGRITY_CHECK)
    // This is only needed for debug checks to ensure they are performed in
    // correct context. umm_malloc free internally determines the correct heap.
    HeapSelectDram ephemeral;
#endif
    return heap_vPortFree(ptr,  file, line);
}

};
