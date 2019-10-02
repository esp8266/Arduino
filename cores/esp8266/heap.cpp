/* heap.c - overrides of SDK heap handling functions
 * Copyright (c) 2016 Ivan Grokhotkov. All rights reserved.
 * This file is distributed under MIT license.
 */

#include <stdlib.h>
#include "umm_malloc/umm_malloc.h"
#include <c_types.h>
#include <sys/reent.h>
#include <user_interface.h>

extern "C" {

#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
#define __umm_malloc(s)           umm_poison_malloc(s)
#define __umm_calloc(n,s)         umm_poison_calloc(n,s)
#define __umm_realloc_fl(p,s,f,l) umm_poison_realloc_fl(p,s,f,l)
#define __umm_free_fl(p,f,l)      umm_poison_free_fl(p,f,l)

#undef realloc
#undef free

#elif defined(DEBUG_ESP_OOM)
#define __umm_malloc(s)           umm_malloc(s)
#define __umm_calloc(n,s)         umm_calloc(n,s)
#define __umm_realloc_fl(p,s,f,l) umm_realloc(p,s)
#define __umm_free_fl(p,f,l)      umm_free(p)

#undef realloc
#undef free

#else  // ! UMM_POISON_CHECK && ! DEBUG_ESP_OOM
#define __umm_malloc(s)           malloc(s)
#define __umm_calloc(n,s)         calloc(n,s)
#define __umm_realloc_fl(p,s,f,l) realloc(p,s)
#define __umm_free_fl(p,f,l)      free(p)
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
#define PTR_CHECK__LOG_LAST_FAIL(p, s, a, f, l) \
    if(0 != (s) && 0 == p)\
    {\
      umm_last_fail_alloc_addr = a;\
      umm_last_fail_alloc_size = s;\
      umm_last_fail_alloc_file = f;\
      umm_last_fail_alloc_line = l;\
    }
#else
#define PTR_CHECK__LOG_LAST_FAIL(p, s, a, f, l) \
    (void)f;\
    (void)l;\
    if(0 != (s) && 0 == p)\
    {\
      umm_last_fail_alloc_addr = a;\
      umm_last_fail_alloc_size = s;\
    }
#endif

void* _malloc_r(struct _reent* unused, size_t size)
{
    (void) unused;
    void *ret = malloc(size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0), NULL, 0);
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
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0), NULL, 0);
    return ret;
}

void* _calloc_r(struct _reent* unused, size_t count, size_t size)
{
    (void) unused;
    void *ret = calloc(count, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, count * size, __builtin_return_address(0), NULL, 0);
    return ret;
}

#ifdef DEBUG_ESP_OOM
#undef malloc
#undef calloc
#undef realloc

#define DEBUG_HEAP_PRINTF ets_uart_printf

void ICACHE_RAM_ATTR print_loc(size_t size, const char* file, int line)
{
    (void)size;
    (void)line;
    if (system_get_os_print()) {
        DEBUG_HEAP_PRINTF(":oom(%d)@", (int)size);

        bool inISR = ETS_INTR_WITHINISR();
        if (inISR && (uint32_t)file >= 0x40200000) {
            DEBUG_HEAP_PRINTF("File: %p", file);
        } else if (!inISR && (uint32_t)file >= 0x40200000) {
            char buf[ets_strlen(file)] __attribute__ ((aligned(4)));
            ets_strcpy(buf, file);
            DEBUG_HEAP_PRINTF(buf);
        } else {
            DEBUG_HEAP_PRINTF(file);
        }
        
        DEBUG_HEAP_PRINTF(":%d\n", line);
    }
}

void ICACHE_RAM_ATTR print_oom_size(size_t size)
{
    (void)size;
    if (system_get_os_print()) {
        DEBUG_HEAP_PRINTF(":oom(%d)@?\n", (int)size);
    }
}

#define OOM_CHECK__PRINT_OOM(p, s) if (!p) print_oom_size(s)
#define OOM_CHECK__PRINT_LOC(p, s, f, l) if (!p) print_loc(s, f, l)

#else  // ! DEBUG_ESP_OOM

#if 1
//C Skip OOM logging of last fail for malloc/... and pvPort... .
//C It cost 64 more bytes of IRAM to turn on. And was not previously enabled.
#undef PTR_CHECK__LOG_LAST_FAIL
#define PTR_CHECK__LOG_LAST_FAIL(p, s, a)
#endif

#define OOM_CHECK__PRINT_OOM(p, s)
#define OOM_CHECK__PRINT_LOC(p, s, f, l)
#endif

#if defined(DEBUG_ESP_OOM) || defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE) || defined(UMM_INTEGRITY_CHECK)

void* ICACHE_RAM_ATTR malloc(size_t size)
{
    INTEGRITY_CHECK__ABORT();
    POISON_CHECK__ABORT();
    void* ret = __umm_malloc(size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0), NULL, 0);
    OOM_CHECK__PRINT_OOM(ret, size);
    return ret;
}

void* ICACHE_RAM_ATTR calloc(size_t count, size_t size)
{
    INTEGRITY_CHECK__ABORT();
    POISON_CHECK__ABORT();
    void* ret = __umm_calloc(count, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, count * size, __builtin_return_address(0), NULL, 0);
    OOM_CHECK__PRINT_OOM(ret, size);
    return ret;
}

void* ICACHE_RAM_ATTR realloc(void* ptr, size_t size)
{
    INTEGRITY_CHECK__ABORT();
    void* ret = __umm_realloc_fl(ptr, size, NULL, 0);
    POISON_CHECK__ABORT();
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0), NULL, 0);
    OOM_CHECK__PRINT_OOM(ret, size);
    return ret;
}

void ICACHE_RAM_ATTR free(void* p)
{
    INTEGRITY_CHECK__ABORT();
    __umm_free_fl(p, NULL, 0);
    POISON_CHECK__ABORT();
}
#endif


void* ICACHE_RAM_ATTR pvPortMalloc(size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    POISON_CHECK__PANIC_FL(file, line);
    void* ret = __umm_malloc(size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0), file, line);
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

void* ICACHE_RAM_ATTR pvPortCalloc(size_t count, size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    POISON_CHECK__PANIC_FL(file, line);
    void* ret = __umm_calloc(count, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, count * size, __builtin_return_address(0), file, line);
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

void* ICACHE_RAM_ATTR pvPortRealloc(void *ptr, size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    void* ret = __umm_realloc_fl(ptr, size, file, line);
    POISON_CHECK__PANIC_FL(file, line);
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0), file, line);
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

void* ICACHE_RAM_ATTR pvPortZalloc(size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    POISON_CHECK__PANIC_FL(file, line);
    void* ret = __umm_calloc(1, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0), file, line);
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

void ICACHE_RAM_ATTR vPortFree(void *ptr, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    POISON_CHECK__PANIC_FL(file, line);
    __umm_free_fl(ptr, file, line);
}

size_t ICACHE_RAM_ATTR xPortWantedSizeAlign(size_t size)
{
    return (size + 3) & ~((size_t) 3);
}

void system_show_malloc(void)
{
    umm_info(NULL, 1);
}

};
