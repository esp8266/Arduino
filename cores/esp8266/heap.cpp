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
 * Even though ISRs should not perform Heap API calls, we protect the call path
 * with IRAM_ATTR for malloc APIs and pvPortMalloc APIs. "new" and _malloc_r
 * (LIBC) are unprotected.
 *
 * Inventory of debug options supported by this modules
 *
 *  * DEBUG_ESP_OOM - Monitors all "allocating" Heap API families for an
 *    out-of-memory result and saves the Last OOM for Postmortem to display.
 *    Additionally, if system OS print is enabled (system_get_os_print() ==
 *    true) , print a diagnostic message at the time of the OOM event. To
 *    further assist in debugging, "fancy macros" redefine malloc, calloc, and
 *    realloc to their matching cousins in the portable malloc family.
 *    Identifies the file name and line number of the caller where the OOM event
 *    occurred.
 *
 *    When DEBUG_ESP_OOM is not selected, use a minimized Last OOM wrapper to
 *    track LIBC and the C++ operator "new".  These wrappers only save the
 *    caller address and size of the Last OOM - and report details at
 *    Postmortem. No Last OOM tracking for the "new" operator with the non-debug
 *    build and option C++ Exceptions: "enabled".
 *
 *    You may select DEBUG_ESP_OOM through the Arduino IDE, Tools->Debug level:
 *    "OOM". Or, enable via a build option define.
 *
 *    DEBUG_ESP_WITHINISR - Monitors in-flash Heap APIs for calls from ISRs.
 *    If they occur, print a message with the address of the caller.
 *
 *    Considerations:
 *    * There may be some rare special case where 'new', 'delete', or LIBC's
 *      _malloc_r APIs are called with interrupts disabled.
 *    * If called from within an ISR, we could have crash before reaching
 *      this code.
 *
 *    Enable via a build option define.
 *
 *  * UMM_POISON_CHECK_LITE - A much lighter version of UMM_POISON_CHECK.
 *    Adds and presets an extra 4 bytes of poison at the beginning and end of
 *    each allocation. On each call to free or realloc, test the current
 *    allocation's poison areas, then each active allocation's neighbor is
 *    tested. During each successful malloc/calloc, check the neighbors of the
 *    free block before resizing to fulfill the request.
 *
 *    In the absence of other UMM_POISON_... options, this option assumes
 *    "enabled" when Tools->Debug: Serial is selected or Tools->Debug level:
 *    "CORE" is selected. Otherwise, you may enable it via a build option
 *    definition.
 *
 *    While coverage is not 100%, a sketch is less likely to have strange
 *    behavior from heavy heap access with interrupts disabled. Also, with
 *    UMM_POISON_CHECK_LITE, more caller context is available at "poison fail."
 *    If you need more perspective, continue reading "UMM_POISON_CHECK."
 *
 *  * UMM_POISON_CHECK - Adds and presets 4 bytes of poison at the beginning
 *    and end of each allocation. At each Heap API call, performs a global Heap
 *    poison data verification. This check runs with interrupts disabled and may
 *    affect WiFi performance and possibly system stability.
 *
 *    As the number of active heap allocations grows, this option will cause
 *    increasingly long periods with interrupts disabled, adversely affecting
 *    time-critical sketches.
 *
 *    Enable via a build option define.
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
 *    While not as comprehensive as UMM_INTEGRITY_CHECK, using
 *    UMM_POISON_CHECK_LITE should reveal most heap corruptions with lower
 *    overhead.
 *
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

#include "heap_cb.h"

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

#if defined(UMM_POISON_CHECK_LITE)
/*
 * umm_malloc will build with umm_poison_* wrappers for each Heap API.
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

#elif defined(UMM_POISON_CHECK)
/*
 * umm_malloc will build with umm_poison_* wrappers for each Heap API.
 *
 * Support debug wrappers that need to include handling poison
 */
#define UMM_MALLOC_FL(s,f,l,c)    umm_poison_malloc(s)
#define UMM_CALLOC_FL(n,s,f,l,c)  umm_poison_calloc(n,s)
#define UMM_REALLOC_FL(p,s,f,l,c) umm_poison_realloc(p,s)
#define UMM_FREE_FL(p,f,l,c)      umm_poison_free(p)
#define ENABLE_THICK_DEBUG_WRAPPERS

#undef realloc
#undef free

#elif defined(DEBUG_ESP_OOM) || defined(UMM_INTEGRITY_CHECK) || defined(DEBUG_ESP_WITHINISR) || defined(HEAP_DEBUG_PROBE_PSFLC_CB)
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
// OOM - this structure variable is always in use by abi.cpp - except for
// C++ Exceptions "enabled"  builds.
//
// When building with C++ Exceptions "disabled" or debug build,
// always track last failed caller and size requested
#if defined(DEBUG_ESP_OOM)
struct umm_last_fail_alloc {
    const void *addr;
    size_t size;
    const char *file;
    int line;
} _umm_last_fail_alloc = {NULL, 0, NULL, 0};

#else
// Note for the least used case "(defined(__cpp_exceptions) &&
// !defined(DEBUG_ESP_OOM))", we only capture details for LIBC calls.
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

static ALWAYS_INLINE bool withinISR(uint32_t ps) {
  return (0 != (ps & 0x0fu));
}

#if 0
/*
  ICACHE should be accessable from an ISR "IF" it has not been disabled for a
  SPI bus transfer. TODO investagate further - `if (inISR && ! isCacheReady())`
 */
#define SPIRDY              ESP8266_DREG(0x0C)  // CACHE_FLASH_CTRL_REG
#define CACHE_READ_EN_BIT   BIT8                // eagle_soc.h in RTOS_SDK
static ALWAYS_INLINE bool isCacheReady(void) {
    return 0 != (SPIRDY & CACHE_READ_EN_BIT);
}
#endif

static void IRAM_ATTR print_loc(bool inISR, size_t size, const char* file, int line, const void* caller) {
    if (system_get_os_print()) {
        DEBUG_HEAP_PRINTF(":oom %p(%d), File: ", caller, (int)size);
        if (file) {
            if ((uint32_t)file >= 0x40200000) {
                if (inISR) {
                    DEBUG_HEAP_PRINTF("%p", file);
                } else {
                    char buf[strlen_P(file) + 1];
                    strcpy_P(buf, file);
                    DEBUG_HEAP_PRINTF(buf);
                }
            } else {
                DEBUG_HEAP_PRINTF(file);
            }
        } else {
            DEBUG_HEAP_PRINTF("??");
        }
        DEBUG_HEAP_PRINTF(":%d\n", line);
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
        print_loc(withinISR(saved_ps), size, file, line, caller);
        xt_wsr_ps(saved_ps);
        _HEAP_DEBUG_PROBE_PSFLC_CB(heap_oom_cb_id, ptr, size, file, line, caller);
        return false;
    }
    return true;
}
#define OOM_CHECK__LOG_LAST_FAIL_FL(p, s, f, l, c) oom_check__log_last_fail_atomic_psflc(p, s, f, l, c)
#define OOM_CHECK__LOG_LAST_FAIL_LITE_FL(p, s, f, l, c) ({ (void)p, (void)s, (void)f; (void)l; (void)c; true; })

#elif defined(ENABLE_THICK_DEBUG_WRAPPERS)
static bool IRAM_ATTR oom_check__log_last_fail_atomic_psc(void *ptr, size_t size, const void* caller) {
    if (0 != (size) && 0 == ptr) {
        // Need to ensure changes to umm_last_fail_alloc are atomic.
        uint32_t saved_ps = xt_rsil(DEFAULT_CRITICAL_SECTION_INTLEVEL);
        _umm_last_fail_alloc.addr = caller;
        _umm_last_fail_alloc.size = size;
        xt_wsr_ps(saved_ps);
        _HEAP_DEBUG_PROBE_PSFLC_CB(heap_oom_cb_id, ptr, size, NULL, 0, caller);
        return false;
    }
    return true;
}
#define OOM_CHECK__LOG_LAST_FAIL_FL(p, s, f, l, c) oom_check__log_last_fail_atomic_psc(p, s, c)
#define OOM_CHECK__LOG_LAST_FAIL_LITE_FL(p, s, f, l, c) ({ (void)p, (void)s, (void)f; (void)l; (void)c; true; })

#else
// At this location, the macro is only used by Heap API families "new" and
// _malloc_r, which are not called from ISRs. Thus, the non-debug OOM call path
// does not require the IRAM_ATTR attribute.
static bool oom_check__log_last_fail_psc(void *ptr, size_t size, const void* caller) {
    if (0 != (size) && 0 == ptr) {
        _umm_last_fail_alloc.addr = caller;
        _umm_last_fail_alloc.size = size;
        _HEAP_DEBUG_PROBE_PSFLC_CB(heap_oom_cb_id, ptr, size, NULL, 0, caller);
        return false;
    }
    return true;
}
// Used to capture minumum OOM info for "new" and LIBC
#define OOM_CHECK__LOG_LAST_FAIL_LITE_FL(p, s, f, l, c) oom_check__log_last_fail_psc(p, s, c)
#define OOM_CHECK__LOG_LAST_FAIL_FL(p, s, f, l, c) ({ (void)p, (void)s, (void)f; (void)l; (void)c; true; })
#endif


///////////////////////////////////////////////////////////////////////////////
// Monitor Heap APIs in flash for calls from ISRs
//
#if DEBUG_ESP_WITHINISR
#define DEBUG_HEAP_PRINTF ets_uart_printf
static void isr_check__flash_not_safe(const void *caller) {
    if (ETS_INTR_WITHINISR()) { // Assumes, non-zero INTLEVEL means in ISR
        DEBUG_HEAP_PRINTF("\nIn-flash, Heap API call from %p with Interrupts Disabled.\n", caller);
    }
}
#define ISR_CHECK__LOG_NOT_SAFE(c) isr_check__flash_not_safe(c)
#else
#define ISR_CHECK__LOG_NOT_SAFE(c) do { (void)c; } while(false)
#endif


#ifdef ENABLE_THICK_DEBUG_WRAPPERS
///////////////////////////////////////////////////////////////////////////////
// Thick Heap API wrapper for debugging: malloc, pvPortMalloc, "new", and
// _malloc_r families of heap APIs.
//
// While UMM_INTEGRITY_CHECK and UMM_POISON_CHECK are included, the Arduino IDE
// has no selection to build with them. Both are CPU intensive and can adversly
// effect the WiFi operation. For completeness they are all included in the
// list below. Both UMM_INTEGRITY_CHECK and UMM_POISON_CHECK can be enabled by
// build defines.
//
// A debug build will use option UMM_POISON_CHECK_LITE by default. If explicitly
// specifying UMM_POISON_CHECK_LITE or UMM_POISON_CHECK, only one is permitted
// in a Build.
//
// When selected, do Integrity Check first. Verifies the heap management
// information is not corrupt. Followed by Full Poison Check before *alloc
// operation.
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
    ret = _HEAP_DEBUG_PROBE_PSFLC_CB(heap_malloc_cb_id, ret, size, file, line, caller);
    OOM_CHECK__LOG_LAST_FAIL_FL(ret, size, file, line, caller);
    return ret;
}

void* IRAM_ATTR _heap_pvPortCalloc(size_t count, size_t size, const char* file, int line, const void *caller)
{
    INTEGRITY_CHECK__PANIC_FL(file, line, caller);
    POISON_CHECK__PANIC_FL(file, line, caller);
    size_t total_size = umm_umul_sat(count, size);
    void* ret = UMM_CALLOC_FL(1, total_size, file, line, caller);
    ret = _HEAP_DEBUG_PROBE_PSFLC_CB(heap_calloc_cb_id, ret, size, file, line, caller);
    OOM_CHECK__LOG_LAST_FAIL_FL(ret, total_size, file, line, caller);
    return ret;
}

void* IRAM_ATTR _heap_pvPortRealloc(void *ptr, size_t size, const char* file, int line, const void *caller)
{
    INTEGRITY_CHECK__PANIC_FL(file, line, caller);
    POISON_CHECK__PANIC_FL(file, line, caller);
    ptr = _HEAP_DEBUG_PROBE_PSFLC_CB(heap_realloc_in_cb_id, ptr, size, file, line, caller);
    void* ret = UMM_REALLOC_FL(ptr, size, file, line, caller);
    ret = _HEAP_DEBUG_PROBE_PSFLC_CB(heap_realloc_out_cb_id, ret, size, file, line, caller);
    OOM_CHECK__LOG_LAST_FAIL_FL(ret, size, file, line, caller);
    return ret;
}

void IRAM_ATTR _heap_vPortFree(void *ptr, const char* file, int line, [[maybe_unused]] const void *caller)
{
    INTEGRITY_CHECK__PANIC_FL(file, line, caller);
    POISON_CHECK__PANIC_FL(file, line, caller);
    ptr = _HEAP_DEBUG_PROBE_PSFLC_CB(heap_free_cb_id, ptr, 0, file, line, caller);
    UMM_FREE_FL(ptr, file, line, caller);
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
    void *caller = __builtin_return_address(0);
    ISR_CHECK__LOG_NOT_SAFE(caller);
    void* ret = _heap_pvPortMalloc(size, NULL, 0, caller);
    OOM_CHECK__LOG_LAST_FAIL_LITE_FL(ret, size, NULL, 0, caller);
    return ret;
}

void* _calloc_r(struct _reent* unused, size_t count, size_t size)
{
    (void) unused;
    void *caller = __builtin_return_address(0);
    ISR_CHECK__LOG_NOT_SAFE(caller);
    void* ret = _heap_pvPortCalloc(count, size, NULL, 0, caller);
    OOM_CHECK__LOG_LAST_FAIL_LITE_FL(ret, size, NULL, 0, caller);
    return ret;
}

void* _realloc_r(struct _reent* unused, void* ptr, size_t size)
{
    (void) unused;
    void *caller = __builtin_return_address(0);
    ISR_CHECK__LOG_NOT_SAFE(caller);
    void* ret = _heap_pvPortRealloc(ptr, size, NULL, 0, caller);
    OOM_CHECK__LOG_LAST_FAIL_LITE_FL(ret, size, NULL, 0, caller);
    return ret;
}

void _free_r(struct _reent* unused, void* ptr)
{
    (void) unused;
    void *caller = __builtin_return_address(0);
    ISR_CHECK__LOG_NOT_SAFE(caller);
    _heap_vPortFree(ptr, NULL, 0, caller);
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

#if defined(DEBUG_ESP_OOM) || !defined(__cpp_exceptions) || defined(DEBUG_ESP_PORT)
///////////////////////////////////////////////////////////////////////////////
// heap allocator for "new" (ABI) - To support collecting OOM info, always defined
void* _heap_abi_malloc(size_t size, bool unhandled, const void* caller)
{
    [[maybe_unused]] const char *file = NULL;
    [[maybe_unused]] const int line = 0;

    #ifdef ENABLE_THICK_DEBUG_WRAPPERS
    ISR_CHECK__LOG_NOT_SAFE(caller);
    INTEGRITY_CHECK__PANIC_FL(file, line, caller);
    POISON_CHECK__PANIC_FL(file, line, caller);
    void* ret = UMM_MALLOC_FL(size, file, line, caller);
    ret = _HEAP_DEBUG_PROBE_PSFLC_CB(heap_abi_malloc_cb_id, ret, size, file, line, caller);
    bool ok = OOM_CHECK__LOG_LAST_FAIL_FL(ret, size, file, line, caller);
    #else
    void* ret = UMM_MALLOC(size);
    // minimum OOM check
    bool ok = OOM_CHECK__LOG_LAST_FAIL_LITE_FL(ret, size, file, line, caller);
    #endif
    if (!ok && unhandled) {
        __unhandled_exception(PSTR("OOM"));
    }
    return ret;
}
#endif

#if (NONOSDK >= (0x30000))
////////////////////////////////////////////////////////////////////////////////
/*
  New for NON-OS SDK 3.0.0 and up
  Needed for WPA2 Enterprise support. This was not present in SDK pre 3.0

  The NON-OS SDK 3.0.x has breaking changes to pvPortMalloc. They added one more
  argument for selecting a heap. To avoid breaking the build, I renamed their
  breaking version to sdk3_pvPortMalloc. To complete the fix, the LIBS need to
  be edited.

  Also in the release are low-level functions pvPortZallocIram and
  pvPortCallocIram, which are not documented in the Espressif NONOS SDK manual.
  No issues in providing replacements. For the non-Arduino ESP8266 applications,
  pvPortZallocIram and pvPortCallocIram would have been selected through the
  macros like os_malloc defined in `mem.h`.

  OOM - Implementation strategy - Native v3.0 SDK
  * For functions `pvPortMalloc(,,,true);` and `pvPortMallocIram(,,,);` on a
    failed IRAM alloc, try DRAM.
  * For function `pvPortMalloc(,,,false);` use DRAM only - on fail, do not
    try IRAM.

  WPA2 Enterprise connect crashing is fixed at v3.0.2 and up.
*/
#ifdef UMM_HEAP_IRAM
void* IRAM_ATTR sdk3_pvPortMalloc(size_t size, const char* file, int line, bool iram)
{
    void* caller = __builtin_return_address(0);
    if (iram) {
        HeapSelectIram ephemeral;
        void* ret = _heap_pvPortMalloc(size, file, line, caller);
        if (ret) return ret;
    }
    {
        HeapSelectDram ephemeral;
        return _heap_pvPortMalloc(size, file, line, caller);
    }
}

void* IRAM_ATTR pvPortCallocIram(size_t count, size_t size, const char* file, int line)
{
    void* caller = __builtin_return_address(0);
    {
        HeapSelectIram ephemeral;
        void* ret = _heap_pvPortCalloc(count, size, file, line, caller);
        if (ret) return ret;
    }
    {
        HeapSelectDram ephemeral;
        return _heap_pvPortCalloc(count, size, file, line, caller);
    }
}

void* IRAM_ATTR pvPortZallocIram(size_t size, const char* file, int line)
{
    void* caller = __builtin_return_address(0);
    {
        HeapSelectIram ephemeral;
        void* ret = _heap_pvPortCalloc(1, size, file, line, caller);
        if (ret) return ret;
    }
    {
        HeapSelectDram ephemeral;
        return _heap_pvPortCalloc(1, size, file, line, caller);
    }
}
#define CONFIG_IRAM_MEMORY 1

#else
// For sdk3_pvPortMalloc, the bool argument is ignored and intentionally omitted.
extern "C" void* sdk3_pvPortMalloc(size_t size, const char* file, int line) __attribute__ ((alloc_size(1), malloc, nothrow, alias("pvPortMalloc")));
extern "C" void* pvPortCallocIram(size_t count, size_t size, const char* file, int line) __attribute__((alloc_size(1, 2), malloc, nothrow, alias("pvPortCalloc")));
extern "C" void* pvPortZallocIram(size_t size, const char* file, int line) __attribute__((alloc_size(1), malloc, nothrow, alias("pvPortZalloc")));
#define CONFIG_IRAM_MEMORY 0
#endif  // #ifdef UMM_HEAP_IRAM

/*
  We do not need the function user_iram_memory_is_enabled().
  1. It was used by mem_manager.o which was replaced with this custom heap
     implementation. IRAM memory selection is handled differently for
     Arduino ESP8266.
  2. In libmain.a, Cache_Read_Enable_New uses it for cache size. However, When
     using IRAM for memory or running with 48K IRAM for code, we use a
     replacement Cache_Read_Enable to correct the cache size ignoring
     Cache_Read_Enable_New's selected value.
  3. Create a linker conflicts in the event the sketch author tries to control
     IRAM heap through this method.
*/
uint32 IRAM_ATTR user_iram_memory_is_enabled(void)
{
    return  CONFIG_IRAM_MEMORY;
}
#endif // #if (NONOSDK >= (0x30000))
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
#include <cstdlib>
#include "new"

// The sized deletes are defined in other files.
#pragma GCC diagnostic ignored "-Wsized-deallocation"

// These function replace their weak counterparts tagged with _GLIBCXX_WEAK_DEFINITION
void operator delete(void* ptr) noexcept
{
    void *caller = __builtin_return_address(0);
    ISR_CHECK__LOG_NOT_SAFE(caller);
    _heap_vPortFree(ptr, NULL, 0, caller);
}

void operator delete(void* ptr, std::size_t) noexcept
{
    void *caller = __builtin_return_address(0);
    ISR_CHECK__LOG_NOT_SAFE(caller);
    _heap_vPortFree(ptr, NULL, 0, caller);
}
#endif
