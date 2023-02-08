/* ----------------------------------------------------------------------------
 * umm_malloc.h - a memory allocator for embedded systems (microcontrollers)
 *
 * See copyright notice in LICENSE.TXT
 * ----------------------------------------------------------------------------
 */

#ifndef UMM_MALLOC_H
#define UMM_MALLOC_H

#include <stdint.h>

// C These includes are not in the upstream
#include "umm_malloc_cfg.h"   /* user-dependent */
#include <osapi.h>
#include <heap_api_debug.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef UMM_HEAP_EXTERNAL
extern void  umm_init_vm(void *vmaddr, unsigned int vmsize);
#endif
#ifdef UMM_HEAP_IRAM
extern void umm_init_iram(void);
extern void umm_init_iram_ex(void *addr, unsigned int size, bool zero);
#endif
/* ------------------------------------------------------------------------ */

extern void  umm_init(void);
extern void *umm_malloc(size_t size);
extern void *umm_calloc(size_t num, size_t size);
extern void *umm_realloc(void *ptr, size_t size);
extern void  umm_free(void *ptr);

/* ------------------------------------------------------------------------ */

extern umm_heap_context_t *umm_push_heap(size_t heap_number);
extern umm_heap_context_t *umm_pop_heap(void);
extern int umm_get_heap_stack_index(void);
extern umm_heap_context_t *umm_set_heap_by_id(size_t which);
extern size_t umm_get_current_heap_id(void);
extern umm_heap_context_t *umm_get_current_heap(void);

#ifdef __cplusplus
}
#endif

#endif /* UMM_MALLOC_H */
