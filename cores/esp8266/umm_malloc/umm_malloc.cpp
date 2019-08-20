/*
 * Itemized changes started July 22, 2019
 * AUG 1, 2019 compiles with OOM, UMM_POISON_CHECK, and appears to be working.
 *
 * local changes for upstream umm_malloc:
 *
 * In malloc.c
 *   Added `#if defined(...)` to cover all of umm_malloc.c.
 *   Suround DBGLOG_LEVEL with #ifndef... Now defined value in umm_malloc_cfg.h
 *
 *   umm_free() - moved critical section to start after safe calculations.
 *
 *   umm_malloc() - moved critical section to start after umm_blocks()
 *   computations are based on contants that don't change, calling
 *   argument excepted.
 *
 *   umm_realloc() - Added UMM_CRITICAL_SUSPEND()/UMM_CRITICAL_RESUME() for
 *   when lightweight locks are available. eg. sti/cli. Single threaded
 *   single CPU case.
 *
 *   umm_realloc() - appears to have been refactored to minimize memmove
 *   and memcpy. The old version would always combine an adjacent block in the
 *   direction of the start of the heap when available and do a memmove. This
 *   had a defragging effect. This appears to have been replaced with an
 *   attempt to minimize copy when possible.
 *
 *   Added heap stats tracking.
 *
 *
 * In umm_info.c
 *   umm_info() - Added UMM_CRITICAL_DECL(id_info), updated critical sections
 *   with tag.
 *   Carried forward: Added NULL ptr check at beginning (umm_malloc.c). - Mar 8, 2016
 *
 *
 * In umm_poison.c:
 *   Resolved C++ compiler error reported on get_poisoned(), and
 *   get_unpoisoned(). They now take in void * arg instead of unsigned char *.
 *
 * In umm_integrity.c:
 *   Replaced printf with DBGLOG_FUNCTION. This needs to be a malloc free
 *   function and ISR safe.
 *   Add critical sections.
 *
 * In umm_malloc_cfg.h:
 *   Added macro UMM_CRITICAL_SUSPEND()/UMM_CRITICAL_RESUME()
 *
 *
 * Globally change across all files %i to %d: umm_info.c, umm_malloc.c,
 *
 *
 * Notes,
 *
 *   umm_integrity_check() is called by macro INTEGRITY_CHECK which returns 1
 *   on success. No corruption. Does a time consuming scan of the whole heap. It
 *   will call UMM_HEAP_CORRUPTION_CB if an error is found.
 *
 *   umm_poison_check(), formerly known as check_poison_all_blocks(),
 *   is called by macro POISON_CHECK which returns 1 on success. No corruption.
 *   Does a time consuming scan of all active allocations for modified poison.
 *   It does *NOT* call UMM_HEAP_CORRUPTION_CB if an error is found.
 *   The option description says it does!
 *
 *   umm_poison_realloc() and umm_poison_free() no longer call the macro
 *   UMM_HEAP_CORRUPTION_CB on poison error. Just a printf message is generated.
 *   ? What do we want to do here? I have added code for detecting the error
 *   and generating a panic; however, it creates more minor upstream changes.
 *   I am not sure what behavior you want?
 *
 *   For upstream umm_malloc "#  define POISON_CHECK() 0" should have been 1
 *   add to list to report.
 */
/*
 * Current Deltas from the old umm_malloc
 *
 *   umm_posion check for a given *alloc - failure no longer panics.
 *   option to run full poison check at each *alloc call, not present
 *   option to run full interity check at each *alloc call, not present
 *   upstream code does not call panic from poison_check_block.
 *
 */


extern "C" {

#define UMM_MALLOC_C

#if 1
#undef memcpy
#undef memmove
#undef memset
#define memcpy ets_memcpy
#define memmove ets_memmove
#define memset ets_memset
#endif

#include "umm_malloc.c"

};
