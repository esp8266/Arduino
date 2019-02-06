/*  ----------------------------------------------------------------------------
    umm_malloc.h - a memory allocator for embedded systems (microcontrollers)

    See copyright notice in LICENSE.TXT
    ----------------------------------------------------------------------------
*/

#ifndef UMM_MALLOC_H
#define UMM_MALLOC_H

/* ------------------------------------------------------------------------ */

#include "umm_malloc_cfg.h"   /* user-dependent */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UMM_HEAP_INFO_t
{
    unsigned short int totalEntries;
    unsigned short int usedEntries;
    unsigned short int freeEntries;

    unsigned short int totalBlocks;
    unsigned short int usedBlocks;
    unsigned short int freeBlocks;

    unsigned short int maxFreeContiguousBlocks;

    unsigned int freeSize2;
}
UMM_HEAP_INFO;

extern UMM_HEAP_INFO ummHeapInfo;

void umm_init(void);

void *umm_info(void *ptr, int force);

void *umm_malloc(size_t size);
void *umm_calloc(size_t num, size_t size);
void *umm_realloc(void *ptr, size_t size);
void umm_free(void *ptr);

size_t umm_free_heap_size(void);
size_t umm_max_block_size(void);
size_t umm_block_size(void);

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------------ */

#endif /* UMM_MALLOC_H */
