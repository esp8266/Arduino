/*
 * Issolated heap debug helper code from from umm_malloc/umm_malloc_cfg.h.
 * Updated umm_malloc/umm_malloc.h and Arduino.h to reference.
 * No #ifdef fenceing was used before. From its previous location, this content
 * was reassert multiple times through Arduino.h. In case there are legacy
 * projects that depend on the previous unfenced behavior, no fencing has been
 * added.
 */

/*
 * *alloc redefinition - Included from Arduino.h for DEBUG_ESP_OOM support.
 *
 * It can also be directly include by the sketch for UMM_POISON_CHECK or
 * UMM_POISON_CHECK_LITE builds to get more info about the caller when they
 * report on a fail.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_ESP_OOM
#define MEMLEAK_DEBUG

#include "umm_malloc/umm_malloc_cfg.h"

#include <pgmspace.h>
// Reuse pvPort* calls, since they already support passing location information.
// Specifically the debug version (heap_...) that does not force DRAM heap.
void *IRAM_ATTR heap_pvPortMalloc(size_t size, const char *file, int line);
void *IRAM_ATTR heap_pvPortCalloc(size_t count, size_t size, const char *file, int line);
void *IRAM_ATTR heap_pvPortRealloc(void *ptr, size_t size, const char *file, int line);
void *IRAM_ATTR heap_pvPortZalloc(size_t size, const char *file, int line);
void IRAM_ATTR heap_vPortFree(void *ptr, const char *file, int line);

#define malloc(s) ({ static const char mem_debug_file[] PROGMEM STORE_ATTR = __FILE__; heap_pvPortMalloc(s, mem_debug_file, __LINE__); })
#define calloc(n,s) ({ static const char mem_debug_file[] PROGMEM STORE_ATTR = __FILE__; heap_pvPortCalloc(n, s, mem_debug_file, __LINE__); })
#define realloc(p,s) ({ static const char mem_debug_file[] PROGMEM STORE_ATTR = __FILE__; heap_pvPortRealloc(p, s, mem_debug_file, __LINE__); })

#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
#define dbg_heap_free(p) ({ static const char mem_debug_file[] PROGMEM STORE_ATTR = __FILE__; heap_vPortFree(p, mem_debug_file, __LINE__); })
#else
#define dbg_heap_free(p) free(p)
#endif

#elif defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE) // #elif for #ifdef DEBUG_ESP_OOM
#include <pgmspace.h>
void *IRAM_ATTR heap_pvPortRealloc(void *ptr, size_t size, const char *file, int line);
#define realloc(p,s) ({ static const char mem_debug_file[] PROGMEM STORE_ATTR = __FILE__; heap_pvPortRealloc(p, s, mem_debug_file, __LINE__); })

void IRAM_ATTR heap_vPortFree(void *ptr, const char *file, int line);
// C - to be discussed
/*
  Problem, I would like to report the file and line number with the umm poison
  event as close as possible to the event. The #define method works for malloc,
  calloc, and realloc those names are not as generic as free. A #define free
  captures too much. Classes with methods called free are included :(
  Inline functions would report the address of the inline function in the .h
  not where they are called.

  Anybody know a trick to make this work?

  Create dbg_heap_free() as an alternative for free() when you need a little
  more help in debugging the more challenging problems.
*/
#define dbg_heap_free(p) ({ static const char mem_debug_file[] PROGMEM STORE_ATTR = __FILE__; heap_vPortFree(p, mem_debug_file, __LINE__); })

#else
#define dbg_heap_free(p) free(p)
#endif /* DEBUG_ESP_OOM */

#ifdef __cplusplus
}
#endif
