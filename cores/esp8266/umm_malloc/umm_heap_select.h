#ifndef UMM_MALLOC_SELECT_H
#define UMM_MALLOC_SELECT_H

#include <umm_malloc/umm_malloc.h>

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

#endif // UMM_MALLOC_SELECT_H
