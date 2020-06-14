#if defined(BUILD_UMM_MALLOC_C)

/* poisoning (UMM_POISON_CHECK) {{{ */
#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
#define POISON_BYTE (0xa5)

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Yields a size of the poison for the block of size `s`.
 * If `s` is 0, returns 0.
 */
static size_t poison_size(size_t s) {
    return(s ? (UMM_POISON_SIZE_BEFORE +
                sizeof(UMM_POISONED_BLOCK_LEN_TYPE) +
                UMM_POISON_SIZE_AFTER)
             : 0);
}

/*
 * Print memory contents starting from given `ptr`
 */
static void dump_mem ( const void *vptr, size_t len ) {
  const uint8_t *ptr = (const uint8_t *)vptr;
  while (len--) {
    DBGLOG_ERROR(" 0x%.2x", (unsigned int)(*ptr++));
  }
}

/*
 * Put poison data at given `ptr` and `poison_size`
 */
static void put_poison( void *ptr, size_t poison_size ) {
  memset(ptr, POISON_BYTE, poison_size);
}

/*
 * Check poison data at given `ptr` and `poison_size`. `where` is a pointer to
 * a string, either "before" or "after", meaning, before or after the block.
 *
 * If poison is there, returns 1.
 * Otherwise, prints the appropriate message, and returns 0.
 */
static bool check_poison( const void *ptr, size_t poison_size,
    const char *where) {
  size_t i;
  bool ok = true;

  for (i = 0; i < poison_size; i++) {
    if (((const uint8_t *)ptr)[i] != POISON_BYTE) {
      ok = false;
      break;
    }
  }

  if (!ok) {
    DBGLOG_ERROR( "No poison %s block at: 0x%lx, actual data:", where, (unsigned long)ptr);
    dump_mem(ptr, poison_size);
    DBGLOG_ERROR( "\n" );
  }

  return ok;
}

/*
 * Check if a block is properly poisoned. Must be called only for non-free
 * blocks.
 */
static bool check_poison_block( umm_block *pblock ) {
  int ok = true;

  if (pblock->header.used.next & UMM_FREELIST_MASK) {
    DBGLOG_ERROR( "check_poison_block is called for free block 0x%lx\n", (unsigned long)pblock);
  } else {
    /* the block is used; let's check poison */
    unsigned char *pc = (unsigned char *)pblock->body.data;
    unsigned char *pc_cur;

    pc_cur = pc + sizeof(UMM_POISONED_BLOCK_LEN_TYPE);
    if (!check_poison(pc_cur, UMM_POISON_SIZE_BEFORE, "before")) {
      ok = false;
      goto clean;
    }

    pc_cur = pc + *((UMM_POISONED_BLOCK_LEN_TYPE *)pc) - UMM_POISON_SIZE_AFTER;
    if (!check_poison(pc_cur, UMM_POISON_SIZE_AFTER, "after")) {
      ok = false;
      goto clean;
    }
  }

clean:
  return ok;
}

/*
 * Takes a pointer returned by actual allocator function (`umm_malloc` or
 * `umm_realloc`), puts appropriate poison, and returns adjusted pointer that
 * should be returned to the user.
 *
 * `size_w_poison` is a size of the whole block, including a poison.
 */
static void *get_poisoned( void *vptr, size_t size_w_poison ) {
  unsigned char *ptr = (unsigned char *)vptr;

  if (size_w_poison != 0 && ptr != NULL) {

    /* Poison beginning and the end of the allocated chunk */
    put_poison(ptr + sizeof(UMM_POISONED_BLOCK_LEN_TYPE),
        UMM_POISON_SIZE_BEFORE);
    put_poison(ptr + size_w_poison - UMM_POISON_SIZE_AFTER,
        UMM_POISON_SIZE_AFTER);

    /* Put exact length of the user's chunk of memory */
    *(UMM_POISONED_BLOCK_LEN_TYPE *)ptr = (UMM_POISONED_BLOCK_LEN_TYPE)size_w_poison;

    /* Return pointer at the first non-poisoned byte */
    ptr += sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE;
  }

  return (void *)ptr;
}

/*
 * Takes "poisoned" pointer (i.e. pointer returned from `get_poisoned()`),
 * and checks that the poison of this particular block is still there.
 *
 * Returns unpoisoned pointer, i.e. actual pointer to the allocated memory.
 */
static void *get_unpoisoned( void *vptr ) {
   uintptr_t ptr = (uintptr_t)vptr;

  if (ptr != 0) {
    uint16_t c;

    ptr -= (sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE);

    umm_heap_context_t *_context = umm_get_ptr_context( vptr );
    if (NULL == _context) {
      panic();
      return NULL;
    }
    /* Figure out which block we're in. Note the use of truncated division... */
    c = (ptr - (uintptr_t)(&(_context->heap[0])))/sizeof(umm_block);

    check_poison_block(&UMM_BLOCK(c));
  }

  return (void *)ptr;
}

/* }}} */

/* ------------------------------------------------------------------------ */

void *umm_poison_malloc( size_t size ) {
  void *ret;

  size += poison_size(size);

  ret = umm_malloc( size );

  ret = get_poisoned(ret, size);

  return ret;
}

/* ------------------------------------------------------------------------ */

void *umm_poison_calloc( size_t num, size_t item_size ) {
  void *ret;
  size_t size = item_size * num;

  size += poison_size(size);

  ret = umm_malloc(size);

  if (NULL != ret)
      memset(ret, 0x00, size);

  ret = get_poisoned(ret, size);

  return ret;
}

/* ------------------------------------------------------------------------ */

void *umm_poison_realloc( void *ptr, size_t size ) {
  void *ret;

  ptr = get_unpoisoned(ptr);

  size += poison_size(size);
  ret = umm_realloc( ptr, size );

  ret = get_poisoned(ret, size);

  return ret;
}

/* ------------------------------------------------------------------------ */

void umm_poison_free( void *ptr ) {

  ptr = get_unpoisoned(ptr);

  umm_free( ptr );
}

/*
 * Iterates through all blocks in the heap, and checks poison for all used
 * blocks.
 */

bool umm_poison_check(void) {
  UMM_CRITICAL_DECL(id_poison);
  bool ok = true;
  uint16_t cur;

  UMM_INIT_HEAP;

  UMM_CRITICAL_ENTRY(id_poison);
  umm_heap_context_t *_context = umm_get_current_heap();

  /* Now iterate through the blocks list */
  cur = UMM_NBLOCK(0) & UMM_BLOCKNO_MASK;

  while( UMM_NBLOCK(cur) & UMM_BLOCKNO_MASK ) {
    if ( !(UMM_NBLOCK(cur) & UMM_FREELIST_MASK) ) {
      /* This is a used block (not free), so, check its poison */
      ok = check_poison_block(&UMM_BLOCK(cur));
      if (!ok){
        break;
      }
    }

    cur = UMM_NBLOCK(cur) & UMM_BLOCKNO_MASK;
  }
  UMM_CRITICAL_EXIT(id_poison);

  return ok;
}

/* ------------------------------------------------------------------------ */

#endif

#endif  // defined(BUILD_UMM_MALLOC_C)
