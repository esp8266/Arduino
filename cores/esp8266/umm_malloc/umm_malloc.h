/* ----------------------------------------------------------------------------
 * umm_malloc.h - a memory allocator for embedded systems (microcontrollers)
 *
 * See copyright notice in LICENSE.TXT
 * ----------------------------------------------------------------------------
 */

#ifndef UMM_MALLOC_H
#define UMM_MALLOC_H

/* ------------------------------------------------------------------------ */

void  umm_init( void );
void *umm_malloc( size_t size );
void *umm_calloc( size_t num, size_t size );
void *umm_realloc( void *ptr, size_t size );
void  umm_free( void *ptr );


/* ------------------------------------------------------------------------ */

#endif /* UMM_MALLOC_H */
