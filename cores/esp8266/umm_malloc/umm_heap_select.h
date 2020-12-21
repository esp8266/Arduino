#ifndef UMM_MALLOC_SELECT_H
#define UMM_MALLOC_SELECT_H

#include <umm_malloc/umm_malloc.h>

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE inline __attribute__ ((always_inline))
#endif

// Use FORCE_ALWAYS_INLINE to ensure HeapSelect... construtor/deconstructor
// are placed in IRAM
#ifdef FORCE_ALWAYS_INLINE
#define MAYBE_ALWAYS_INLINE ALWAYS_INLINE
#else
#define MAYBE_ALWAYS_INLINE
#endif

/*
  This class is modeled after interrupts.h

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
  MAYBE_ALWAYS_INLINE
  HeapSelect(size_t id) { (void)id; }
  MAYBE_ALWAYS_INLINE
  ~HeapSelect() {}
#else
  MAYBE_ALWAYS_INLINE
  HeapSelect(size_t id) : _heap_id(umm_get_current_heap_id()) {
    umm_set_heap_by_id(id);
  }

  MAYBE_ALWAYS_INLINE
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
  MAYBE_ALWAYS_INLINE
  HeapSelectIram() : _heap_id(umm_get_current_heap_id()) {
    umm_set_heap_by_id(UMM_HEAP_IRAM);
  }

  MAYBE_ALWAYS_INLINE
  ~HeapSelectIram() {
    umm_set_heap_by_id(_heap_id);
  }

protected:
    size_t _heap_id;

#else
  MAYBE_ALWAYS_INLINE
  HeapSelectIram() {}
  MAYBE_ALWAYS_INLINE
  ~HeapSelectIram() {}
#endif
};

class HeapSelectDram {
public:
#if (UMM_NUM_HEAPS == 1)
  MAYBE_ALWAYS_INLINE
  HeapSelectDram() {}
  MAYBE_ALWAYS_INLINE
  ~HeapSelectDram() {}
#else
  MAYBE_ALWAYS_INLINE
  HeapSelectDram() : _heap_id(umm_get_current_heap_id()) {
    umm_set_heap_by_id(UMM_HEAP_DRAM);
  }

  MAYBE_ALWAYS_INLINE
  ~HeapSelectDram() {
    umm_set_heap_by_id(_heap_id);
  }

protected:
    size_t _heap_id;
#endif
};

#endif // UMM_MALLOC_SELECT_H
