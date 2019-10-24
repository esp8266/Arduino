/* ----------------------------------------------------------------------------
 * umm_malloc.h - a memory allocator for embedded systems (microcontrollers)
 *
 * See copyright notice in LICENSE.TXT
 * ----------------------------------------------------------------------------
 */

#ifndef UMM_MALLOC_H
#define UMM_MALLOC_H

/* ------------------------------------------------------------------------ */

//C This include is not in upstream neither are the #ifdef __cplusplus
#include "umm_malloc_cfg.h"   /* user-dependent */

#ifdef __cplusplus
extern "C" {
#endif

void  umm_init( void );
void *umm_malloc( size_t size );
void *umm_calloc( size_t num, size_t size );
void *umm_realloc( void *ptr, size_t size );
void  umm_free( void *ptr );

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------------ */

#endif /* UMM_MALLOC_H */
