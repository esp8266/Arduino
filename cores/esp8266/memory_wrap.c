/**
 memory_wrap.c

 Copyright (c) 2015 Markus Sattler. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <stdlib.h>
#include <debug.h>
#include <Arduino.h>

void ICACHE_RAM_ATTR __wrap_vPortFree(void *ptr, const char* file, int line) {
    free(ptr);
}

void * ICACHE_RAM_ATTR __wrap_pvPortMalloc(size_t size, const char* file, int line) {
    return malloc(size);
}

void * ICACHE_RAM_ATTR __wrap_pvPortCalloc(size_t num, size_t size, const char* file, int line) {
    return calloc(num, size);
}

void * ICACHE_RAM_ATTR __wrap_pvPortRealloc(void *ptr, size_t size, const char* file, int line) {
    return realloc(ptr, size);
}

void * ICACHE_RAM_ATTR __wrap_pvPortZalloc(size_t size, const char* file, int line) {
    return calloc(1, size);
}

void ICACHE_RAM_ATTR __wrap_mem_free(void *ptr) {
    free(ptr);
}

void * ICACHE_RAM_ATTR __wrap_mem_malloc(size_t size) {
    return malloc(size);
}

void * ICACHE_RAM_ATTR __wrap_mem_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

void * ICACHE_RAM_ATTR __wrap_mem_zalloc(size_t size) {
    return calloc(1, size);
}

size_t ICACHE_RAM_ATTR __wrap_xPortGetFreeHeapSize(void) {
    return umm_free_heap_size();
}
