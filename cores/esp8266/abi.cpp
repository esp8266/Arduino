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
#include <bits/c++config.h>
#include <bit>

using __cxxabiv1::__guard;

// Debugging helper, last allocation which returned NULL
extern "C" void* _heap_abi_malloc(size_t size, bool unhandled, const void* const caller);
#if UMM_ENABLE_MEMALIGN
extern "C" void* _heap_abi_memalign(size_t alignment, size_t size, bool unhandled, const void* const caller);
#endif

extern "C" void __cxa_pure_virtual(void) __attribute__ ((__noreturn__));
extern "C" void __cxa_deleted_virtual(void) __attribute__ ((__noreturn__));


#if DEV_DEBUG_ABI_CPP
extern "C" void _dbg_abi_print_pstr(const char* op, const char *function_name);
#define DEBUG_NEW_OP_PRINTF() _dbg_abi_print_pstr("new_op", __PRETTY_FUNCTION__)
#else
#define DEBUG_NEW_OP_PRINTF() do { } while (false)
#endif
/*
  This is what I perceived to be the intent of the original code.

  Use C++ "Replaceable allocation functions" to install debug wrappers to catch
  additional information for debugging. The default C++ exception handlers use
  weak links.

  C++ Exceptions: "enabled" -
    * With debug (eg. "Debug port: Serial"), do full caller info capture and
      Heap debug checks. "Replaceable allocation functions" are in use by the
      debugging code. "Replaceable allocation functions" are not available to
      the Sketch.
    * Without debug, no OOM details captured. The C++ "Replaceable allocation
      functions" are available to the Sketch.

  C++ Exceptions: "disabled" -
    * C++ "Replaceable allocation functions" are always in use.
    * With debug, do full caller info capture and Heap debug checks.
    * Without debug, capture minimum OOM information. Calling address and size
      of last alloc failure.
*/

#if defined(__cpp_exceptions) && \
(defined(DEBUG_ESP_OOM) || defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_WITHINISR) || defined(MIN_ESP_OOM))

// Debug replacement adaptation from ".../new_op.cc".
using std::new_handler;
using std::bad_alloc;

#if defined(UMM_ENABLE_MEMALIGN)

// Includes C++ exceptions
// Includes C++17 operator new align variants

static void* _heap_new_align(std::size_t size, std::size_t alignment, const void* caller)
{
    /*
       "Alignment must be a power of two."
       The C++ sample code did this: if (__builtin_expect(!std::__has_single_bit(alignment), false)) throw(bad_alloc());

       From https://en.cppreference.com/w/cpp/memory/c/aligned_alloc
         "alignment - specifies the alignment. Must be a valid alignment
         supported by the implementation."

       I left the validation to the umm_malloc library. See umm_memalign() for
       details. Generally speaking, zero is handled as default, and the default
       is sizeof(umm_block), 8 bytes. Since the default is 8 bytes, the
       umm_malloc library is less strict about checking alignments less than 8
       bytes.
    */

    void* p;

    while (nullptr == (p = _heap_abi_memalign(alignment, size, false, caller))) {
        new_handler handler = std::get_new_handler();
        if (!handler) {
            throw(bad_alloc());
        }
        handler();
    }

    return p;
}


// new_opa
void* operator new (std::size_t size, std::align_val_t alignment)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_new_align(size, std::size_t(alignment), __builtin_return_address(0));
}

// new_opva
void* operator new[] (std::size_t size, std::align_val_t alignment)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_new_align(size, std::size_t(alignment), __builtin_return_address(0));
}

// new_opant
void* operator new (std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    DEBUG_NEW_OP_PRINTF();

    __try {
        return _heap_new_align(size, std::size_t(alignment), __builtin_return_address(0));
    }
    __catch(...) {
        return nullptr;
    }
}

// new_opvant
void* operator new[] (std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    DEBUG_NEW_OP_PRINTF();

    __try {
        return _heap_new_align(size, std::size_t(alignment), __builtin_return_address(0));
    }
    __catch(...) {
        return nullptr;
    }
}

// new_op
void* operator new (std::size_t size)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_new_align(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__, __builtin_return_address(0));
}

// new_opv
void* operator new[] (std::size_t size)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_new_align(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__, __builtin_return_address(0));
}

// new_opnt
void* operator new (size_t size, const std::nothrow_t&) noexcept
{
    DEBUG_NEW_OP_PRINTF();

    __try {
        return _heap_new_align(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__, __builtin_return_address(0));
    }
    __catch (...) {
        return nullptr;
    }
}

// new_opvnt
void* operator new[] (size_t size, const std::nothrow_t&) noexcept
{
    DEBUG_NEW_OP_PRINTF();

    __try {
        return _heap_new_align(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__, __builtin_return_address(0));
    }
    __catch (...) {
        return nullptr;
    }
}

#else // ! UMM_ENABLE_MEMALIGN

// Includes C++ exceptions
// Without C++17 operator new align variants

static void* _heap_new(std::size_t size, const void* caller)
{
    void* p;

    while (nullptr == (p = _heap_abi_malloc(size, false, caller))) {
        new_handler handler = std::get_new_handler();
        if (!handler) {
            throw(bad_alloc());
        }
        handler();
    }

    return p;
}

void* operator new (std::size_t size)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_new(size, __builtin_return_address(0));
}

void* operator new[] (std::size_t size)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_new(size, __builtin_return_address(0));
}

void* operator new (size_t size, const std::nothrow_t&) noexcept
{
    DEBUG_NEW_OP_PRINTF();

    __try {
        return _heap_new(size, __builtin_return_address(0));
    }
    __catch (...) {
        return nullptr;
    }
}

void* operator new[] (size_t size, const std::nothrow_t&) noexcept
{
    DEBUG_NEW_OP_PRINTF();

    __try {
        return _heap_new(size, __builtin_return_address(0));
    }
    __catch (...) {
        return nullptr;
    }
}
#endif // #if UMM_ENABLE_MEMALIGN

#elif !defined(__cpp_exceptions)
// When doing builds with C++ Exceptions "disabled", always save details of
// the last OOM event.

// overwrite weak operators new/new[] definitions

#if defined(UMM_ENABLE_MEMALIGN)

// Without C++ exceptions
// Includes C++17 operator new align variants

void* operator new (size_t size, std::align_val_t alignment)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_memalign(std::size_t(alignment), size, true, __builtin_return_address(0));
}

void* operator new[] (size_t size, std::align_val_t alignment)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_memalign(std::size_t(alignment), size, true, __builtin_return_address(0));
}

void* operator new (size_t size, std::align_val_t alignment, const std::nothrow_t&)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_memalign(std::size_t(alignment), size, false, __builtin_return_address(0));
}

void* operator new[] (size_t size, std::align_val_t alignment, const std::nothrow_t&)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_memalign(std::size_t(alignment), size, false, __builtin_return_address(0));
}

void* operator new (size_t size)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_memalign(__STDCPP_DEFAULT_NEW_ALIGNMENT__, size, true, __builtin_return_address(0));
}

void* operator new[] (size_t size)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_memalign(__STDCPP_DEFAULT_NEW_ALIGNMENT__, size, true, __builtin_return_address(0));
}

void* operator new (size_t size, const std::nothrow_t&)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_memalign(__STDCPP_DEFAULT_NEW_ALIGNMENT__, size, false, __builtin_return_address(0));
}

void* operator new[] (size_t size, const std::nothrow_t&)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_memalign(__STDCPP_DEFAULT_NEW_ALIGNMENT__, size, false, __builtin_return_address(0));
}

#else

// Without C++ exceptions
// Without C++17 operator new align variants

void* operator new (size_t size)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_malloc(size, true, __builtin_return_address(0));
}

void* operator new[] (size_t size)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_malloc(size, true, __builtin_return_address(0));
}

void* operator new (size_t size, const std::nothrow_t&)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_malloc(size, false, __builtin_return_address(0));
}

void* operator new[] (size_t size, const std::nothrow_t&)
{
    DEBUG_NEW_OP_PRINTF();

    return _heap_abi_malloc(size, false, __builtin_return_address(0));
}
#endif // #elif !defined(__cpp_exceptions)  #if defined(UMM_ENABLE_MEMALIGN)
#else
/*
  The C++ Exception handlers in libstdc are using weaklinks. The "new" operators
  that express alignment should work through libstdc via memalign() in the
  umm_malloc library. While not likely to ever be needed, the Sketch can replace
  the C++ "Replaceable allocation functions."

  Note that libstdc will fail errors in alignment value early. Thus, the
  UMM_STATS_FULL alignment error count will be zero.

  This saves about 20 bytes in the UMM_ENABLE_MEMALIGN=1 case and 32 bytes when
  UMM_ENABLE_MEMALIGN=0.

*/
//D <<
//C temporary pragmas remove before merge
#pragma message("Using weaklink C++ Exception handlers in libstdc")
#if UMM_ENABLE_MEMALIGN
#pragma message("The \"new\" operators that express alignment should work through libstdc via memalign() in the umm_malloc library.")
#endif
//D >>

#endif // #if defined(__cpp_exceptions)


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
