/*
 Copyright (c) 2014 Arduino.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <assert.h>
#include <debug.h>
#include <Arduino.h>
#include <cxxabi.h>

using __cxxabiv1::__guard;

// Debugging helper, last allocation which returned NULL
extern void *umm_last_fail_alloc_addr;
extern int umm_last_fail_alloc_size;

extern "C" void __cxa_pure_virtual(void) __attribute__ ((__noreturn__));
extern "C" void __cxa_deleted_virtual(void) __attribute__ ((__noreturn__));


#ifndef __cpp_exceptions
void *operator new(size_t size)
{
    void *ret = malloc(size);
    if (0 != size && 0 == ret) {
        umm_last_fail_alloc_addr = __builtin_return_address(0);
        umm_last_fail_alloc_size = size;
    }
   return ret;
}

void *operator new[](size_t size)
{
    void *ret = malloc(size);
    if (0 != size && 0 == ret) {
        umm_last_fail_alloc_addr = __builtin_return_address(0);
        umm_last_fail_alloc_size = size;
    }
    return ret;
}
#endif

void __cxa_pure_virtual(void)
{
    panic();
}

void __cxa_deleted_virtual(void)
{
    panic();
}

typedef struct {
    uint8_t guard;
    uint8_t ps;
} guard_t;

extern "C" int __cxa_guard_acquire(__guard* pg)
{
    uint8_t ps = xt_rsil(15);
    if (reinterpret_cast<guard_t*>(pg)->guard) {
        xt_wsr_ps(ps);
        return 0;
    }
    reinterpret_cast<guard_t*>(pg)->ps = ps;
    return 1;
}

extern "C" void __cxa_guard_release(__guard* pg)
{
    reinterpret_cast<guard_t*>(pg)->guard = 1;
    xt_wsr_ps(reinterpret_cast<guard_t*>(pg)->ps);
}

extern "C" void __cxa_guard_abort(__guard* pg)
{
    xt_wsr_ps(reinterpret_cast<guard_t*>(pg)->ps);
}

// TODO: rebuild windows toolchain to make this unnecessary:
void* __dso_handle;
