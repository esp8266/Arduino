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

#ifdef UMM_HEAP_EXTERNAL
void  umm_init_vm( void *vmaddr, unsigned int vmsize );
#endif
#ifdef UMM_HEAP_IRAM
void umm_init_iram(void);
void umm_init_iram_ex( void *addr, unsigned int size, bool zero );
#endif

void *umm_malloc( size_t size );
void *umm_calloc( size_t num, size_t size );
void *umm_realloc( void *ptr, size_t size );
void  umm_free( void *ptr );

umm_heap_context_t *umm_push_heap( size_t heap_number );
umm_heap_context_t *umm_pop_heap( void );
umm_heap_context_t *umm_set_heap_by_id( size_t which );
size_t umm_get_current_heap_id(void);


#ifdef __cplusplus
}
#endif
/*
  This is pattered after interrupts.h

  HeapSelectIram is used to temporarily select an alternate Heap.

  {
      {
        HeapSelectIram lock;
        // allocate memory here
      }
      allocations here are from the old Heap selection
  }
 */

class HeapSelect {
public:
#if (UMM_NUM_HEAPS == 1)
  HeapSelect(size_t id) { (void)id; }
  ~HeapSelect() {}
#else
  HeapSelect(size_t id) : _heap_id(umm_get_current_heap_id()) {
    umm_set_heap_by_id(id);
  }

  ~HeapSelect() {
    umm_set_heap_by_id(_heap_id);
  }

protected:
    size_t _heap_id;
#endif
};

class HeapSelectIram {
public:
#ifdef UMM_HEAP_IRAM
  HeapSelectIram() : _heap_id(umm_get_current_heap_id()) {
    umm_set_heap_by_id(UMM_HEAP_IRAM);
  }

  ~HeapSelectIram() {
    umm_set_heap_by_id(_heap_id);
  }

protected:
    size_t _heap_id;

#else
  HeapSelectIram() {}
  ~HeapSelectIram() {}
#endif
};

class HeapSelectDram {
public:
#if (UMM_NUM_HEAPS == 1)
  HeapSelectDram() {}
  ~HeapSelectDram() {}
#else
  HeapSelectDram() : _heap_id(umm_get_current_heap_id()) {
    umm_set_heap_by_id(UMM_HEAP_DRAM);
  }

  ~HeapSelectDram() {
    umm_set_heap_by_id(_heap_id);
  }

protected:
    size_t _heap_id;
#endif
};


/* ------------------------------------------------------------------------ */

#endif /* UMM_MALLOC_H */
