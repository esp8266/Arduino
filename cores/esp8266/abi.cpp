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
#include <Arduino.h>
#include <cxxabi.h>

using __cxxabiv1::__guard;

// Debugging helper, last allocation which returned NULL
extern "C" void *_heap_abi_malloc(size_t size, bool unhandled, const void* const caller);

extern "C" void __cxa_pure_virtual(void) __attribute__ ((__noreturn__));
extern "C" void __cxa_deleted_virtual(void) __attribute__ ((__noreturn__));

#if defined(__cpp_exceptions) && (defined(DEBUG_ESP_OOM) || defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_WITHINISR))
/*
  When built with C++ Exceptions: "enabled", track caller address of Last OOM.
  * For debug build, force enable Last OOM tracking.
  * With the option "DEBUG_ESP_OOM," always do Last OOM tracking.
  * Otherwise, disable Last OOM tracking. The build relies on the weak link to
    the default C++ exception handler.
*/

// Debug replacement adaptation from ".../new_op.cc".
using std::new_handler;
using std::bad_alloc;

void * operator new (std::size_t size)
{
    void *p;

    /* malloc (0) is unpredictable; avoid it.  */
    if (__builtin_expect(size == 0, false)) {
        size = 1;
    }

    while (0 == (p = _heap_abi_malloc(size, false, __builtin_return_address(0)))) {
        new_handler handler = std::get_new_handler();
        if (!handler) {
            throw(bad_alloc());
        }
        handler();
    }

    return p;
}
#elif !defined(__cpp_exceptions)
// When doing builds with C++ Exceptions "disabled", always save details of
// the last OOM event.

// overwrite weak operators new/new[] definitions

void* operator new(size_t size)
{
    return _heap_abi_malloc(size, true, __builtin_return_address(0));
}

void* operator new[](size_t size)
{
    return _heap_abi_malloc(size, true, __builtin_return_address(0));
}

void* operator new (size_t size, const std::nothrow_t&)
{
    return _heap_abi_malloc(size, false, __builtin_return_address(0));
}

void* operator new[] (size_t size, const std::nothrow_t&)
{
    return _heap_abi_malloc(size, false, __builtin_return_address(0));
}

#endif // !defined(__cpp_exceptions)

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
