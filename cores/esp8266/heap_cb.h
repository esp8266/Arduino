#ifndef HEAP_CB_H
#define HEAP_CB_H
// Experimental debug macros
/*
 * HEAP_DEBUG_PROBE_PSFLC_CB - A debug build option. User-defined "C" function
 * callback for Heap related failures. It can range from a simple breakpoint to
 * a complicated backtrace or other supplemental information. The callback
 * context is very restrictive: no Heap calls, no PROGMEM, IRAM code only, no
 * flash strings, and limited to ROM-based printf functions (like
 * ets_uart_printf(...)).
 *
 * Example:
 *   extern "C" void my_user_bp(const char* f, int l, const void* c) {
 *     (void)f; (void)l; (void)c;
 *     __asm__ __volatile__("break 1, 0;" ::: "memory");
 *   }
 *
 *   -DHEAP_DEBUG_PROBE_PSFLC_CB='my_user_bp'
 */
enum heap_api_cb_id {
    heap_oom_cb_id = 0,
    heap_malloc_cb_id = 1,
    heap_calloc_cb_id,
    heap_realloc_in_cb_id,
    heap_realloc_out_cb_id,
    heap_free_cb_id,
    heap_abi_malloc_cb_id,
    heap_abi_delete_cb_id,
    heap_poison_cb_id,
    heap_poison_lite_cb_id,
    heap_poison_lite_neighbor_cb_id,
    heap_poison_lite_addr_cb_id,
    heap_integrity_cb_id
};

#if defined(_HEAP_DEBUG_PROBE_PSFLC_CB)
// Set a callback macro
// #define _HEAP_DEBUG_PROBE_PSFLC_CB(id, ptr, size, file, line, caller) ...

#elif defined(HEAP_DEBUG_PROBE_PSFLC_CB)
// Set a callback function
extern "C" void *HEAP_DEBUG_PROBE_PSFLC_CB(heap_api_cb_id id, void *ptr, size_t size, const char* file, int line, const void *caller);
#define _HEAP_DEBUG_PROBE_PSFLC_CB HEAP_DEBUG_PROBE_PSFLC_CB

#else
#define _HEAP_DEBUG_PROBE_PSFLC_CB(i, p, s, f, l, c) ({ (void)i; (void)s; (void)f; (void)l; (void)c; p; })
#endif


#if defined(_HEAP_DEBUG_PROBE_ADD_OVERHEAD)
// Set allocation size adjustment macro
// #define _HEAP_DEBUG_PROBE_ADD_OVERHEAD(id, size) ...

#elif defined(HEAP_DEBUG_PROBE_ADD_OVERHEAD)
// Set allocation size adjustment function
extern "C" size_t HEAP_DEBUG_PROBE_ADD_OVERHEAD(heap_api_cb_id id, size_t size);
#define _HEAP_DEBUG_PROBE_ADD_OVERHEAD HEAP_DEBUG_PROBE_ADD_OVERHEAD

#else
#define _HEAP_DEBUG_PROBE_ADD_OVERHEAD(id, size) (size);
#endif


// #define VOID_PLUS(p, i) ((void*)umm_uadd_sat(((uintptr_t)(p)) + (i) ))

#endif

#if defined(HEAP_DEBUG_PROBE_PSFLC_CB_EXAMPLE) && defined(HEAP_DEBUG_PROBE_PSFLC_CB)
extern "C" {
void *HEAP_DEBUG_PROBE_PSFLC_CB(heap_api_cb_id id, void *ptr, size_t size, const char* file, int line, const void *caller)
{
    switch (id) {
// Heap API events for tracking
        case heap_malloc_cb_id:
            break;

        case heap_calloc_cb_id:
            break;

        case heap_realloc_in_cb_id:
            break;

        case heap_realloc_out_cb_id:
            break;

        case heap_free_cb_id:
            break;

        case heap_abi_malloc_cb_id:
            break;

        case heap_abi_delete_cb_id:
            break;

// Failed Heap events
        case heap_poison_cb_id:
            break;

        case heap_poison_lite_cb_id:
            break;

        case heap_integrity_cb_id:
            break;

        case heap_oom_cb_id:
            break;
    }
    return ptr;
}

extern "C" size_t umm_uadd_sat(const size_t a, const size_t b);
const size_t extra_space = 4;
size_t HEAP_DEBUG_PROBE_ADD_OVERHEAD(heap_api_cb_id id, size_t size) {
    return umm_uadd_sat(size, extra_space);
}

}
#endif
