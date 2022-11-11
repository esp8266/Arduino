/* heap.c - overrides of SDK heap handling functions
 * Copyright (c) 2016 Ivan Grokhotkov. All rights reserved.
 * This file is distributed under MIT license.
 */
/*
 * On the Arduino ESP8266 platform, there are four heap API families in use:
 *   * The C++ `new`/`delete` operators
 *   * The legacy `malloc`, ... often used by "C" programs
 *   * An internal LIBC library `_malloc_r`, ...
 *   * The portable heap APIs, `pvPortMalloc`, ... for embedded platforms.
 *     Used by NONOS SDK and LWIP.
 *
 * Their is only one Heap. A thin wrapper or shim is used to access the
 * underlying heap library. The underlying heap library is a local port of
 * `umm_malloc` from https://github.com/rhempel/umm_malloc/.
 *
 * This module supplies a convergence point and a redirect to umm_malloc for
 * Heap APIs families malloc, pvPortMalloc, "new", and _malloc_r. It builds as
 * either a "thin wrapper" or a "thick wrapper" to capture Heap debug info and
 * diagnostics.
 *
 * Protect ISR call paths to malloc APIs and pvPortMalloc APIs with IRAM_ATTR.
 * "new" and _malloc_r (LIBC) are not expected to be called from an ISR and are
 * not safe.
 *
 * Iventory of debug options supported by this modules
 *
 *  * DEBUG_ESP_OOM - Monitors all "allocating" Heap API families for failure
 *    and saves the last failure for postmortem to display. Additionally if
 *    system OS print is enabled (system_get_os_print == true), a diagnostic
 *    message can be printed at the time of the OOM event. To furthor assist in
 *    debugging, "fancy macros" redefine malloc, calloc, and realloc to their
 *    matching cousins in the portable malloc family, These allow the capturing
 *    of the file name and line number of the caller that had the OOM event.
 *
 *    When DEBUG_ESP_OOM is not selected, use a minimized version to monitor for
 *    OOM from LIBC and the C++ operator "new".  These wrappers check for an
 *    out-of-memory result, saving the caller address of the last fail. Report
 *    last fail at Postmortem.
 *
 *  * UMM_POISON_CHECK_LITE - A much lighter version of UMM_POISON_CHECK.
 *    Adds and presets an extra 4 bytes of poison at the beginning and end of
 *    each allocation. On each call to free or realloc, test the current
 *    allocation's poison areas, then each active allocation's neighbor is
 *    tested. This option is enabled when Tools->Debug: Serial is selected or
 *    Tools->Debug level: "CORE" is selected. While coverage is not 100%, a
 *    sketch is less likely to have strange behavior from heavy heap access with
 *    interrupts disabled.
 *
 *  * UMM_POISON_CHECK - Adds and presets 4 bytes of poison at the beginning and
 *    end of each allocation. For each Heap API call, a complete sweep through
 *    every allocation is performed, verifying that all poison bytes are
 *    correct. This check runs with interrupts disabled and may affect WiFi
 *    performance and maybe system stability.
 *
 *    As the number of active heap allocations grows, this option will cause
 *    increasingly long periods with interrupts disabled, adversely affecting
 *    time-critical sketches.
 *
 *    Enable via a build option define.
 *
 *    UMM_POISON_CHECK_LITE is a better alternative.
 *
 *  * UMM_INTEGRITY_CHECK - will verify that the Heap is semantically correct
 *    and that all the block indexes make sense. While it can catch errors
 *    quickly, the check runs with interrupts disabled and may affect WiFi
 *    performance and maybe system stability.
 *
 *    As the number of active heap allocations grows, this option will cause
 *    increasingly long periods with interrupts disabled, adversely affecting
 *    time-critical sketches.
 *
 *    Enable via a build option define.
 *
 *    IMHO, UMM_INTEGRITY_CHECK is best suited for heap library verification
 *    rather than general debugging. It will detect heap corruption; however, it
 *    offers little aid in determining who did it.
 *
 *    UMM_POISON_CHECK_LITE should detect most would-be heap corruptions with
 *    lower overhead.
 */

#include <stdlib.h>
#include "umm_malloc/umm_malloc.h"

extern "C" size_t umm_uadd_sat(const size_t a, const size_t b);
extern "C" size_t umm_umul_sat(const size_t a, const size_t b);

// z2EapFree: See wpa2_eap_patch.cpp for details
extern "C" void z2EapFree(void *ptr, const char* file, int line) __attribute__((weak, alias("vPortFree"), nothrow));
// I don't understand all the compiler noise around this alias.
// Adding "__attribute__ ((nothrow))" seems to resolve the issue.
// This may be relevant: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81824

// For "pvPortMalloc" API wrappers, use FORCE_ALWAYS_INLINE to put HeapSelect
// class constructor/deconstructor in IRAM.
#define FORCE_ALWAYS_INLINE_HEAP_SELECT
#include "umm_malloc/umm_heap_select.h"

#include <c_types.h>
#include <sys/reent.h>
#include <user_interface.h>

extern "C" {

///////////////////////////////////////////////////////////////////////////////
// Select from various heap function renames that facilitate inserting debug
// wrappers, and tradditional names for the non-debug case.
//

/*
 * With any debug options listed above, umm_malloc changes its heap API names
 * from malloc, calloc, realloc, and free to umm_malloc, umm_calloc,
 * umm_realloc,  and umm_free.
 *
 */
#undef STATIC_ALWAYS_INLINE
#undef ENABLE_THICK_DEBUG_WRAPPERS

#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
/*
 * With either of these defines, umm_malloc will build with umm_poison_*
 * wrappers for each Heap API.
 *
 * Support debug wrappers that need to include handling poison
 */
#define UMM_MALLOC_FL(s,f,l,c)    umm_poison_malloc(s)
#define UMM_CALLOC_FL(n,s,f,l,c)  umm_poison_calloc(n,s)
#define UMM_REALLOC_FL(p,s,f,l,c) umm_poison_realloc_flc(p,s,f,l,c)
#define UMM_FREE_FL(p,f,l,c)      umm_poison_free_flc(p,f,l,c)
#define ENABLE_THICK_DEBUG_WRAPPERS

#undef realloc
#undef free

#elif defined(DEBUG_ESP_OOM) || defined(UMM_INTEGRITY_CHECK)
// All other debug wrappers that do not require handling poison
#define UMM_MALLOC_FL(s,f,l,c)    umm_malloc(s)
#define UMM_CALLOC_FL(n,s,f,l,c)  umm_calloc(n,s)
#define UMM_REALLOC_FL(p,s,f,l,c) umm_realloc(p,s)
#define UMM_FREE_FL(p,f,l,c)      umm_free(p)
#define ENABLE_THICK_DEBUG_WRAPPERS

#undef realloc
#undef free

#else  // ! UMM_POISON_CHECK && ! DEBUG_ESP_OOM
// Used to create thin heap wrappers not for debugging.
#define UMM_MALLOC(s)             malloc(s)
#define UMM_CALLOC(n,s)           calloc(n,s)
#define UMM_REALLOC(p,s)          realloc(p,s)
#define UMM_FREE(p)               free(p)
#endif


///////////////////////////////////////////////////////////////////////////////
// UMM_POISON_CHECK wrapper macros
//
// Take care not to blame the messenger; the function (file/line) that resulted
// in the discovery may not be directly responsible for the damage. We could use
// abort; however, using panic may provide some hints of the location of the
// problem.
//
// Note well, the failure is a discovery of an error that could have occurred at
// any time between calls to POISON_CHECK.
//
#if defined(UMM_POISON_CHECK)
  #define POISON_CHECK__PANIC_FL(f, l, c) \
      do { \
          if ( ! POISON_CHECK() ) { \
              __panic_func(f, l, ""); \
          } \
      } while(0)

#else
  // Disable full heap poison checking.
  #define POISON_CHECK__PANIC_FL(f, l, c) do { (void)f; (void)l; (void)c; } while(0)
#endif


///////////////////////////////////////////////////////////////////////////////
// UMM_INTEGRITY_CHECK wrapper macros
//
// (Caution notes of UMM_POISON_CHECK also apply here.)
//
#ifdef UMM_INTEGRITY_CHECK
#define INTEGRITY_CHECK__PANIC_FL(f, l, c) \
    do { \
        if ( ! INTEGRITY_CHECK() ) { \
            __panic_func(f, l, ""); \
        } \
    } while(0)

#else  // ! UMM_INTEGRITY_CHECK
#define INTEGRITY_CHECK__PANIC_FL(f, l, c) do { (void)f; (void)l; (void)c; } while(0)
#endif //   UMM_INTEGRITY_CHECK


///////////////////////////////////////////////////////////////////////////////
// OOM - this structure variable is always in use by abi.cpp
//
// Always track last failed caller and size requested
//
#if defined(DEBUG_ESP_OOM)
struct umm_last_fail_alloc {
    const void *addr;
    size_t size;
    const char *file;
    int line;
} _umm_last_fail_alloc = {NULL, 0, NULL, 0};

#else
struct umm_last_fail_alloc {
    const void *addr;
    size_t size;
} _umm_last_fail_alloc = {NULL, 0};
#endif


///////////////////////////////////////////////////////////////////////////////
// OOM - DEBUG_ESP_OOM extends monitoring for OOM to capture caller information
// across various Heap entry points and their aliases.
//
// data capture wrapper macros and defines
// Debugging helper, save the last caller address that got a NULL pointer
// response. And when available, the file and line number.
#if defined(DEBUG_ESP_OOM)

// OOM - Debug printing
//
// IRQ/ISR safe printing macros. Printing is controled according to the results
// of system_get_os_print(). Also, being in a IRQ will prevent the printing of
// file names stored in PROGMEM. The PROGMEM address to the string is printed in
// its place.
#define DEBUG_HEAP_PRINTF ets_uart_printf
static void IRAM_ATTR print_loc(size_t size, const char* file, int line, const void* caller)
{
    if (system_get_os_print()) {
        DEBUG_HEAP_PRINTF(":oom(%d)@", (int)size);

        if (file) {
            bool inISR = ETS_INTR_WITHINISR();
            if (inISR && (uint32_t)file >= 0x40200000) {
                DEBUG_HEAP_PRINTF("%p, File: %p", caller, file);
            } else if (!inISR && (uint32_t)file >= 0x40200000) {
                char buf[strlen_P(file) + 1];
                strcpy_P(buf, file);
                DEBUG_HEAP_PRINTF("%p, File: %s", caller, buf);
            } else {
                DEBUG_HEAP_PRINTF(file);
            }
            DEBUG_HEAP_PRINTF(":%d\n", line);
        } else {
            DEBUG_HEAP_PRINTF("%p\n", caller);
        }
    }
}

static bool IRAM_ATTR oom_check__log_last_fail_atomic_psflc(void *ptr, size_t size, const char* file, int line, const void* caller) {
    if (0 != (size) && 0 == ptr) {
        // Ensure changes to umm_last_fail_alloc are atomic. Otherwise, with a
        // poorly timed interrupt, OOM details would mix between foreground and
        // IRQ paths.
        uint32_t saved_ps = xt_rsil(DEFAULT_CRITICAL_SECTION_INTLEVEL);
        _umm_last_fail_alloc.addr = caller;
        _umm_last_fail_alloc.size = size;
        _umm_last_fail_alloc.file = file;
        _umm_last_fail_alloc.line = line;
        print_loc(size, file, line, caller);
        xt_wsr_ps(saved_ps);
        return false;
    }
    return true;
}
#define OOM_CHECK__LOG_LAST_FAIL_FL(p, s, f, l, c) oom_check__log_last_fail_atomic_psflc(p, s, f, l, c)
#define OOM_CHECK__LOG_LAST_FAIL_LITE_FL(p, s, f, l, c) ({ (void)p, (void)s, (void)f; (void)l; (void)c; true; })

#elif defined(ENABLE_THICK_DEBUG_WRAPPERS)
static bool IRAM_ATTR oom_check__log_last_fail_psc(void *ptr, size_t size, const void* caller) {
    if (0 != (size) && 0 == ptr) {
        // Need to ensure changes to umm_last_fail_alloc are atomic.
        uint32_t saved_ps = xt_rsil(DEFAULT_CRITICAL_SECTION_INTLEVEL);
        _umm_last_fail_alloc.addr = caller;
        _umm_last_fail_alloc.size = size;
        xt_wsr_ps(saved_ps);
        return false;
    }
    return true;
}
#define OOM_CHECK__LOG_LAST_FAIL_FL(p, s, f, l, c) oom_check__log_last_fail_psc(p, s, c)
#define OOM_CHECK__LOG_LAST_FAIL_LITE_FL(p, s, f, l, c) ({ (void)p, (void)s, (void)f; (void)l; (void)c; true; })

#else
// At this location, the macro is only used by Heap API families "new" and
// _malloc_r, which are not called from ISRs. Thus, the non-debug OOM call path
// does not require the IRAM_ATTR attribute.
static bool oom_check__log_last_fail_psc(void *ptr, size_t size, const void* caller) {
    if (0 != (size) && 0 == ptr) {
        _umm_last_fail_alloc.addr = caller;
        _umm_last_fail_alloc.size = size;
        return false;
    }
    return true;
}
// Used to capture minumum OOM info for "new" and LIBC
#define OOM_CHECK__LOG_LAST_FAIL_LITE_FL(p, s, f, l, c) oom_check__log_last_fail_psc(p, s, c)
#define OOM_CHECK__LOG_LAST_FAIL_FL(p, s, f, l, c) ({ (void)p, (void)s, (void)f; (void)l; (void)c; true; })
#endif


#ifdef ENABLE_THICK_DEBUG_WRAPPERS
///////////////////////////////////////////////////////////////////////////////
// Thick Heap API wrapper for debugging: malloc, pvPortMalloc, "new", and
// _malloc_r families of heap APIs.
//
// While UMM_INTEGRITY_CHECK and UMM_POISON_CHECK are included, the Arduino IDE
// has no selection to build with them. Both are CPU intensive and can adversly
// effect the WiFi operation. We use option UMM_POISON_CHECK_LITE instead of
// UMM_POISON_CHECK. This is include in the debug build when you select the
// Debug Port. For completeness they are all included in the list below. Both
// UMM_INTEGRITY_CHECK and UMM_POISON_CHECK can be enabled by a build define.
//
// The thinking behind the ordering of Integrity Check, Full Poison Check, and
// the specific *alloc function.
//
// 1. Integrity Check - verifies the heap management information is not corrupt.
//    This allows any other testing, that walks the heap, to run safely.
//
// 2. Place Full Poison Check before or after a specific *alloc function?
//    a. After, when the *alloc function operates on an existing allocation.
//    b. Before, when the *alloc function creates a new, not modified, allocation.
//
//    In a free() or realloc() call, the focus is on their allocation. It is
//    checked 1st and reported on 1ST if an error exists. Full Poison Check is
//    done after.
//
//    For malloc(), calloc(), and zalloc() Full Poison Check is done 1st since
//    these functions do not modify an existing allocation.
//
#undef malloc
#undef calloc
#undef realloc
#undef free

///////////////////////////////////////////////////////////////////////////////
// Common Heap debug helper functions for each alloc operation
//
// Used by debug wrapper for:
//   * portable malloc API, pvPortMalloc, ...
//   * LIBC variation, _malloc_r, ...
//   * "fancy macros" that call heap_pvPortMalloc, ...
//   * Fallback for uncapture malloc API calls, malloc, ...
//
void* IRAM_ATTR _heap_pvPortMalloc(size_t size, const char* file, int line, const void *caller)
{
    INTEGRITY_CHECK__PANIC_FL(file, line, caller);
    POISON_CHECK__PANIC_FL(file, line, caller);
    void* ret = UMM_MALLOC_FL(size, file, line, caller);
    OOM_CHECK__LOG_LAST_FAIL_FL(ret, size, file, line, caller);
    return ret;
}

void* IRAM_ATTR _heap_pvPortCalloc(size_t count, size_t size, const char* file, int line, const void *caller)
{
    INTEGRITY_CHECK__PANIC_FL(file, line, caller);
    POISON_CHECK__PANIC_FL(file, line, caller);
    size_t total_size = umm_umul_sat(count, size);
    void* ret = UMM_CALLOC_FL(1, total_size, file, line, caller);
    OOM_CHECK__LOG_LAST_FAIL_FL(ret, total_size, file, line, caller);
    return ret;
}

void* IRAM_ATTR _heap_pvPortRealloc(void *ptr, size_t size, const char* file, int line, const void *caller)
{
    INTEGRITY_CHECK__PANIC_FL(file, line, caller);
    void* ret = UMM_REALLOC_FL(ptr, size, file, line, caller);
    POISON_CHECK__PANIC_FL(file, line, caller);
    OOM_CHECK__LOG_LAST_FAIL_FL(ret, size, file, line, caller);
    return ret;
}

void IRAM_ATTR _heap_vPortFree(void *ptr, const char* file, int line, [[maybe_unused]] const void *caller)
{
    INTEGRITY_CHECK__PANIC_FL(file, line, caller);
    UMM_FREE_FL(ptr, file, line, caller);
    POISON_CHECK__PANIC_FL(file, line, caller);
}


///////////////////////////////////////////////////////////////////////////////
// Heap debug wrappers used by "fancy debug macros" to capture caller's context:
// module name, line no. and caller return address.
// The "fancy debug macros" are defined in `heap_api_debug.h`
void* IRAM_ATTR heap_pvPortMalloc(size_t size, const char* file, int line)
{
    return _heap_pvPortMalloc(size,  file, line, __builtin_return_address(0));
}

void* IRAM_ATTR heap_pvPortCalloc(size_t count, size_t size, const char* file, int line)
{
    return _heap_pvPortCalloc(count, size,  file, line, __builtin_return_address(0));
}

void* IRAM_ATTR heap_pvPortRealloc(void *ptr, size_t size, const char* file, int line)
{
    return _heap_pvPortRealloc(ptr, size,  file, line, __builtin_return_address(0));
}

void IRAM_ATTR heap_vPortFree(void *ptr, const char* file, int line)
{
    return _heap_vPortFree(ptr, file, line, __builtin_return_address(0));
}


///////////////////////////////////////////////////////////////////////////////
// Heap debug wrappers used to captured any remaining standard heap api calls
void* IRAM_ATTR malloc(size_t size)
{
    return _heap_pvPortMalloc(size, NULL, 0, __builtin_return_address(0));
}

void* IRAM_ATTR calloc(size_t count, size_t size)
{
    return _heap_pvPortCalloc(count, size, NULL, 0, __builtin_return_address(0));
}

void* IRAM_ATTR realloc(void* ptr, size_t size)
{
    return _heap_pvPortRealloc(ptr, size, NULL, 0, __builtin_return_address(0));
}

void IRAM_ATTR free(void* ptr)
{
    _heap_vPortFree(ptr, NULL, 0, __builtin_return_address(0));
}

#else
///////////////////////////////////////////////////////////////////////////////
// Non-debug path
//
// Make Non-debug Portable Heap wrappers ultra thin with STATIC_ALWAYS_INLINE
#define STATIC_ALWAYS_INLINE static ALWAYS_INLINE

STATIC_ALWAYS_INLINE
void* IRAM_ATTR _heap_pvPortMalloc(size_t size, const char* file, int line, const void *caller)
{
    (void)file;
    (void)line;
    (void)caller;
    return UMM_MALLOC(size);
}

STATIC_ALWAYS_INLINE
void* IRAM_ATTR _heap_pvPortCalloc(size_t count, size_t size, const char* file, int line, const void *caller)
{
    (void)file;
    (void)line;
    (void)caller;
    return UMM_CALLOC(count, size);
}

STATIC_ALWAYS_INLINE
void* IRAM_ATTR _heap_pvPortRealloc(void *ptr, size_t size, const char* file, int line, const void *caller)
{
    (void)file;
    (void)line;
    (void)caller;
    return UMM_REALLOC(ptr, size);
}

STATIC_ALWAYS_INLINE
void IRAM_ATTR _heap_vPortFree(void *ptr, const char* file, int line, const void *caller)
{
    (void)file;
    (void)line;
    (void)caller;
    UMM_FREE(ptr);
}
#endif


///////////////////////////////////////////////////////////////////////////////
// Heap debug wrappers used by LIBC
// capture caller return address at exposed API
void* _malloc_r(struct _reent* unused, size_t size)
{
    (void) unused;
    void* ret = _heap_pvPortMalloc(size, NULL, 0, __builtin_return_address(0));
    OOM_CHECK__LOG_LAST_FAIL_LITE_FL(ret, size, NULL, 0, __builtin_return_address(0));
    return ret;
}

void* _calloc_r(struct _reent* unused, size_t count, size_t size)
{
    (void) unused;
    void* ret = _heap_pvPortCalloc(count, size, NULL, 0, __builtin_return_address(0));
    OOM_CHECK__LOG_LAST_FAIL_LITE_FL(ret, size, NULL, 0, __builtin_return_address(0));
    return ret;
}

void* _realloc_r(struct _reent* unused, void* ptr, size_t size)
{
    (void) unused;
    void* ret = _heap_pvPortRealloc(ptr, size, NULL, 0, __builtin_return_address(0));
    OOM_CHECK__LOG_LAST_FAIL_LITE_FL(ret, size, NULL, 0, __builtin_return_address(0));
    return ret;
}

void _free_r(struct _reent* unused, void* ptr)
{
    (void) unused;
    _heap_vPortFree(ptr, NULL, 0, __builtin_return_address(0));
}


/*
  NONOS SDK and lwIP expect DRAM memory. Ensure they don't get something else
  like an IRAM Heap allocation. Since they also use portable malloc calls
  pvPortMalloc, ... we can leverage that for this solution.
  Force pvPortMalloc, ... APIs to serve DRAM only.

  _heap_xxx() functions will be inline for non-debug builds.

  capture caller return address at exposed API
*/
void* IRAM_ATTR pvPortMalloc(size_t size, const char* file, int line)
{
    HeapSelectDram ephemeral;
    return _heap_pvPortMalloc(size,  file, line, __builtin_return_address(0));
}

void* IRAM_ATTR pvPortCalloc(size_t count, size_t size, const char* file, int line)
{
    HeapSelectDram ephemeral;
    return _heap_pvPortCalloc(count, size,  file, line, __builtin_return_address(0));
}

void* IRAM_ATTR pvPortRealloc(void *ptr, size_t size, const char* file, int line)
{
    HeapSelectDram ephemeral;
    return _heap_pvPortRealloc(ptr, size,  file, line, __builtin_return_address(0));
}

void* IRAM_ATTR pvPortZalloc(size_t size, const char* file, int line)
{
    HeapSelectDram ephemeral;
    return _heap_pvPortCalloc(1, size, file, line, __builtin_return_address(0));
}

void IRAM_ATTR vPortFree(void *ptr, const char* file, int line)
{
#if defined(UMM_POISON_CHECK) || defined(UMM_INTEGRITY_CHECK)
    // While umm_free internally determines the correct heap, UMM_POISON_CHECK
    // and UMM_INTEGRITY_CHECK do not have arguments. They will use the
    // current heap to identify which one to analyze.
    //
    // This is not needed for UMM_POISON_CHECK_LITE, it directly handles
    // multiple heaps.
    //
    // DEBUG_ESP_OOM is not tied to any one heap.
    HeapSelectDram ephemeral;
#endif
    return _heap_vPortFree(ptr, file, line, __builtin_return_address(0));
}

///////////////////////////////////////////////////////////////////////////////
// NONOS SDK - Replacement functions
//
size_t IRAM_ATTR xPortWantedSizeAlign(size_t size)
{
    return (size + 3) & ~((size_t) 3);
}

void system_show_malloc(void)
{
#ifdef UMM_INFO
    HeapSelectDram ephemeral;
    umm_info(NULL, true);
#endif
}

#if !defined(__cpp_exceptions)
///////////////////////////////////////////////////////////////////////////////
// heap allocator for "new" (ABI) - To support collecting OOM info, always defined
void* _heap_abi_malloc(size_t size, bool unhandle, const void* caller)
{
    [[maybe_unused]] const char *file = NULL;
    [[maybe_unused]] const int line = 0;

    #ifdef ENABLE_THICK_DEBUG_WRAPPERS
    INTEGRITY_CHECK__PANIC_FL(file, line, caller);
    POISON_CHECK__PANIC_FL(file, line, caller);
    void* ret = UMM_MALLOC_FL(size, file, line, caller);
    if (!OOM_CHECK__LOG_LAST_FAIL_FL(ret, size, file, line, caller) && unhandle) {
        __unhandled_exception(PSTR("OOM"));
    }
    #else
    void* ret = UMM_MALLOC(size);
    // Always do some level of OOM check
    if (!OOM_CHECK__LOG_LAST_FAIL_LITE_FL(ret, size, file, line, caller) && unhandle) {
        __unhandled_exception(PSTR("OOM"));
    }
    #endif
    return ret;
}
#endif

};

#if defined(ENABLE_THICK_DEBUG_WRAPPERS)
///////////////////////////////////////////////////////////////////////////////
//C Note I just threw this together from files from the Internet
//C Is this the proper way to supply replacement "delete" operator?
//

///////////////////////////////////////////////////////////////////////////////
// Replacement C++ delete operator to capture callers address
//
#include <bits/c++config.h>

#if !_GLIBCXX_HOSTED
// A freestanding C runtime may not provide "free" -- but there is no
// other reasonable way to implement "operator delete".
namespace std
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION
  extern "C" void free(void*);
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace
#pragma message("!_GLIBCXX_HOSTED")
#else
// #pragma message("_GLIBCXX_HOSTED")
// This is the path taken
#include <cstdlib>
#endif

#include "new"

// The sized deletes are defined in other files.
#pragma GCC diagnostic ignored "-Wsized-deallocation"

// These function replace their weak counterparts tagged with _GLIBCXX_WEAK_DEFINITION
void operator delete(void* ptr) noexcept
{
    _heap_vPortFree(ptr, NULL, 0, __builtin_return_address(0));
}

void operator delete(void* ptr, std::size_t) noexcept
{
    _heap_vPortFree(ptr, NULL, 0, __builtin_return_address(0));
}
#endif
