/* heap.c - overrides of SDK heap handling functions
 * Copyright (c) 2016 Ivan Grokhotkov. All rights reserved.
 * This file is distributed under MIT license.
 */

#include <stdlib.h>
#include "umm_malloc/umm_malloc.h"
#include <c_types.h>
#include <sys/reent.h>


extern "C" {

#if defined( UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
#define __umm_malloc(s)           umm_poison_malloc(s)
#define __umm_calloc(n,s)         umm_poison_calloc(n,s)
#define __umm_realloc_fl(p,s,f,l) umm_poison_realloc_fl(p,s,f,l)
#define __umm_free_fl(p,f,l)      umm_poison_free_fl(p,f,l)

#ifdef UMM_POISON_CHECK_LITE
#define POISON_CHECK__ABORT() do {} while(0)
#define POISON_CHECK__PANIC_FL(file, line) do { (void)file; (void)line; } while(0)

#else // Full heap poison check at every malloc libary call.
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
#endif

#undef realloc
#undef free

#else  // ! UMM_POISON_CHECK
#define __umm_malloc(s)           malloc(s)
#define __umm_calloc(n,s)         calloc(n,s)
#define __umm_realloc_fl(p,s,f,l) realloc(p,s)
#define __umm_free_fl(p,f,l)      free(p)

#define POISON_CHECK__ABORT() do {} while(0)
#define POISON_CHECK__PANIC_FL(file, line) do { (void)file; (void)line; } while(0)
#endif  // UMM_POISON_CHECK

// Debugging helper, last allocation which returned NULL
void *umm_last_fail_alloc_addr = NULL;
int umm_last_fail_alloc_size = 0;


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

#define PTR_CHECK__LOG_LAST_FAIL(p, s, a) \
    if(0 != (s) && 0 == p)\
    {\
      umm_last_fail_alloc_addr = a;\
      umm_last_fail_alloc_size = s;\
    }

void* _malloc_r(struct _reent* unused, size_t size)
{
    (void) unused;
    void *ret = malloc(size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0))
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
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0))
    return ret;
}

void* _calloc_r(struct _reent* unused, size_t count, size_t size)
{
    (void) unused;
    void *ret = calloc(count, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, count * size, __builtin_return_address(0))
    return ret;
}


#define PTR_CHECK__LOG_LAST_FAIL(p, s, a) \
    if(0 != (s) && 0 == p)\
    {\
      umm_last_fail_alloc_addr = a;\
      umm_last_fail_alloc_size = s;\
    }

#ifdef DEBUG_ESP_OOM
#undef malloc
#undef calloc
#undef realloc

static const char oom_fmt[]   PROGMEM STORE_ATTR = ":oom(%d)@?\n";
static const char oom_fmt_1[] PROGMEM STORE_ATTR = ":oom(%d)@";
static const char oom_fmt_2[] PROGMEM STORE_ATTR = ":%d\n";

void ICACHE_RAM_ATTR print_loc(size_t size, const char* file, int line)
{
        DBGLOG_FUNCTION_P(oom_fmt_1, (int)size);
        DBGLOG_FUNCTION_P(file);
        DBGLOG_FUNCTION_P(oom_fmt_2, line);
}

#define OOM_CHECK__PRINT_OOM(p, f, s) if (!p) DBGLOG_FUNCTION_P(f, s)
#define OOM_CHECK__PRINT_LOC(p, s, f, l) if (!p) print_loc(s, f, l)

#else  // ! DEBUG_ESP_OOM

#if 1
//C Skip OOM check of pvPort... . It cost 64 more bytes of IRAM to turn on.
//C Was not previously enabled.
#undef PTR_CHECK__LOG_LAST_FAIL
#define PTR_CHECK__LOG_LAST_FAIL(p, s, a)
#endif

#define OOM_CHECK__PRINT_OOM(p, f, s)
#define OOM_CHECK__PRINT_LOC(p, s, f, l)
#endif

#if defined(DEBUG_ESP_OOM) || defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE) || defined(UMM_INTEGRITY_CHECK)

void* ICACHE_RAM_ATTR malloc(size_t size)
{
    INTEGRITY_CHECK__ABORT();
    POISON_CHECK__ABORT();
    void* ret = __umm_malloc(size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0));
    OOM_CHECK__PRINT_OOM(ret, oom_fmt, (int)size);
    return ret;
}

void* ICACHE_RAM_ATTR calloc(size_t count, size_t size)
{
    INTEGRITY_CHECK__ABORT();
    POISON_CHECK__ABORT();
    void* ret = __umm_calloc(count, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, count * size, __builtin_return_address(0));
    OOM_CHECK__PRINT_OOM(ret, oom_fmt, (int)size);
    return ret;
}

void* ICACHE_RAM_ATTR realloc(void* ptr, size_t size)
{
    INTEGRITY_CHECK__ABORT();
    void* ret = __umm_realloc_fl(ptr, size, NULL, 0);
    POISON_CHECK__ABORT();
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0));
    OOM_CHECK__PRINT_OOM(ret, oom_fmt, (int)size);
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
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0));
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

void* ICACHE_RAM_ATTR pvPortCalloc(size_t count, size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    POISON_CHECK__PANIC_FL(file, line);
    void* ret = __umm_calloc(count, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, count * size, __builtin_return_address(0));
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

void* ICACHE_RAM_ATTR pvPortRealloc(void *ptr, size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    void* ret = __umm_realloc_fl(ptr, size, file, line);
    POISON_CHECK__PANIC_FL(file, line);
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0));
    OOM_CHECK__PRINT_LOC(ret, size, file, line);
    return ret;
}

void* ICACHE_RAM_ATTR pvPortZalloc(size_t size, const char* file, int line)
{
    INTEGRITY_CHECK__PANIC_FL(file, line);
    POISON_CHECK__PANIC_FL(file, line);
    void* ret = __umm_calloc(1, size);
    PTR_CHECK__LOG_LAST_FAIL(ret, size, __builtin_return_address(0));
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
