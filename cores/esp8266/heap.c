/* heap.c - overrides of SDK heap handling functions
 * Copyright (c) 2016 Ivan Grokhotkov. All rights reserved.
 * This file is distributed under MIT license.
 */

#include <stdlib.h>
#include "umm_malloc/umm_malloc.h"
#include <c_types.h>
#include <sys/reent.h>

void* _malloc_r(struct _reent* unused, size_t size)
{
    (void) unused;
    return malloc(size);
}

void _free_r(struct _reent* unused, void* ptr)
{
    (void) unused;
    return free(ptr);
}

void* _realloc_r(struct _reent* unused, void* ptr, size_t size)
{
    (void) unused;
    return realloc(ptr, size);
}

void* _calloc_r(struct _reent* unused, size_t count, size_t size)
{
    (void) unused;
    return calloc(count, size);
}

void* ICACHE_RAM_ATTR pvPortMalloc(size_t size, const char* file, int line)
{
    (void) file;
    (void) line;
    return malloc(size);
}

void ICACHE_RAM_ATTR vPortFree(void *ptr, const char* file, int line)
{
    (void) file;
    (void) line;
    free(ptr);
}

void* ICACHE_RAM_ATTR pvPortCalloc(size_t count, size_t size, const char* file, int line)
{
    (void) file;
    (void) line;
    return calloc(count, size);
}

void* ICACHE_RAM_ATTR pvPortRealloc(void *ptr, size_t size, const char* file, int line)
{
    (void) file;
    (void) line;
    return realloc(ptr, size);
}

void* ICACHE_RAM_ATTR pvPortZalloc(size_t size, const char* file, int line)
{
    (void) file;
    (void) line;
    return calloc(1, size);
}

size_t xPortGetFreeHeapSize(void)
{
    return umm_free_heap_size();
}

size_t ICACHE_RAM_ATTR xPortWantedSizeAlign(size_t size)
{
    return (size + 3) & ~((size_t) 3);
}

void system_show_malloc(void)
{
    umm_info(NULL, 1);
}
