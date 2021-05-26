/*
 * Local Additions/Enhancements
 *
 */
#if defined(BUILD_UMM_MALLOC_C)

#if defined(UMM_CRITICAL_METRICS)
/*
 * umm_malloc performance measurements for critical sections
 */
UMM_TIME_STATS time_stats = {
    {0xFFFFFFFF, 0U, 0U, 0U},
    {0xFFFFFFFF, 0U, 0U, 0U},
    {0xFFFFFFFF, 0U, 0U, 0U},
#ifdef UMM_INFO
    {0xFFFFFFFF, 0U, 0U, 0U},
#endif
#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)
    {0xFFFFFFFF, 0U, 0U, 0U},
#endif
#ifdef UMM_INTEGRITY_CHECK
    {0xFFFFFFFF, 0U, 0U, 0U},
#endif
    {0xFFFFFFFF, 0U, 0U, 0U} };

bool ICACHE_FLASH_ATTR get_umm_get_perf_data(UMM_TIME_STATS *p, size_t size)
{
    UMM_CRITICAL_DECL(id_no_tag);
    if (p && sizeof(time_stats) == size)
    {
        UMM_CRITICAL_ENTRY(id_no_tag);
        memcpy(p, &time_stats, size);
        UMM_CRITICAL_EXIT(id_no_tag);
        return true;
    }
    return false;
}
#endif

// Alternate Poison functions

#if defined(UMM_POISON_CHECK_LITE)
// We skip this when doing the full poison check.

static bool check_poison_neighbors( umm_heap_context_t *_context, uint16_t cur ) {
  uint16_t c;

  if ( 0 == cur )
    return true;

  c = UMM_PBLOCK(cur) & UMM_BLOCKNO_MASK;
  while( c && (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) ) {
    /*
       There can be up to 1 free block neighbor in either direction.
       This loop should self limit to 2 passes, due to heap design.
       i.e. Adjacent free space is always consolidated.
     */
    if ( !(UMM_NBLOCK(c) & UMM_FREELIST_MASK) ) {
      if ( !check_poison_block(&UMM_BLOCK(c)) )
        return false;

      break;
    }

    c = UMM_PBLOCK(c) & UMM_BLOCKNO_MASK;
  }

  c = UMM_NBLOCK(cur) & UMM_BLOCKNO_MASK;
  while( (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) ) {
    if ( !(UMM_NBLOCK(c) & UMM_FREELIST_MASK) ) {
      if ( !check_poison_block(&UMM_BLOCK(c)) )
        return false;

      break;
    }

    c = UMM_NBLOCK(c) & UMM_BLOCKNO_MASK;
  }

  return true;
}
#endif

#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)

/* ------------------------------------------------------------------------ */

static void *get_unpoisoned_check_neighbors( void *vptr, const char* file, int line ) {
  uintptr_t ptr = (uintptr_t)vptr;

  if (ptr != 0) {

    ptr -= (sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE);

#if defined(UMM_POISON_CHECK_LITE)
    UMM_CRITICAL_DECL(id_poison);
    uint16_t c;
    bool poison = false;
    umm_heap_context_t *_context = umm_get_ptr_context( vptr );
    if (NULL == _context) {
      panic();
      return NULL;
    }
    /* Figure out which block we're in. Note the use of truncated division... */
    c = (ptr - (uintptr_t)(&(_context->heap[0])))/sizeof(umm_block);

    UMM_CRITICAL_ENTRY(id_poison);
    poison = check_poison_block(&UMM_BLOCK(c)) && check_poison_neighbors(_context, c);
    UMM_CRITICAL_EXIT(id_poison);

    if (!poison) {
      if (file) {
        __panic_func(file, line, "");
      } else {
        abort();
      }
    }
#else
    /*
     *  No need to check poison here. POISON_CHECK() has already done a
     *  full heap check.
     */
    (void)file;
    (void)line;
#endif
  }

  return (void *)ptr;
}

/* ------------------------------------------------------------------------ */

void *umm_poison_realloc_fl(void *ptr, size_t size, const char* file, int line) {
    void *ret;

    ptr = get_unpoisoned_check_neighbors(ptr, file, line);

    size += poison_size(size);
    ret = umm_realloc(ptr, size);

    ret = get_poisoned(ret, size);

    return ret;
}

/* ------------------------------------------------------------------------ */

void umm_poison_free_fl(void *ptr, const char* file, int line) {

    ptr = get_unpoisoned_check_neighbors(ptr, file, line);

    umm_free(ptr);
}
#endif

/* ------------------------------------------------------------------------ */

#if defined(UMM_STATS) || defined(UMM_STATS_FULL) || defined(UMM_INFO)
size_t umm_block_size( void ) {
  return sizeof(umm_block);
}
#endif

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
// Keep complete call path in IRAM
size_t umm_free_heap_size_lw( void ) {
  UMM_INIT_HEAP;

  umm_heap_context_t *_context = umm_get_current_heap();
  return (size_t)_context->UMM_FREE_BLOCKS * sizeof(umm_block);
}
#endif

/*
  I assume xPortGetFreeHeapSize needs to be in IRAM. Since
  system_get_free_heap_size is in IRAM. Which would mean, umm_free_heap_size()
  in flash, was not a safe alternative for returning the same information.
*/
#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
size_t xPortGetFreeHeapSize(void) __attribute__ ((alias("umm_free_heap_size_lw")));
#elif defined(UMM_INFO)
#ifndef UMM_INLINE_METRICS
#warning "No ISR safe function available to implement xPortGetFreeHeapSize()"
#endif
size_t xPortGetFreeHeapSize(void) __attribute__ ((alias("umm_free_heap_size")));
#endif

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
void umm_print_stats(int force) {
  umm_heap_context_t *_context = umm_get_current_heap();

  DBGLOG_FORCE( force, "umm heap statistics:\n");
  DBGLOG_FORCE( force,   "  Heap ID           %5u\n", _context->id);
  DBGLOG_FORCE( force,   "  Free Space        %5u\n", _context->UMM_FREE_BLOCKS * sizeof(umm_block));
  DBGLOG_FORCE( force,   "  OOM Count         %5u\n", _context->UMM_OOM_COUNT);
#if defined(UMM_STATS_FULL)
  DBGLOG_FORCE( force,   "  Low Watermark     %5u\n", _context->stats.free_blocks_min * sizeof(umm_block));
  DBGLOG_FORCE( force,   "  Low Watermark ISR %5u\n", _context->stats.free_blocks_isr_min * sizeof(umm_block));
  DBGLOG_FORCE( force,   "  MAX Alloc Request %5u\n", _context->stats.alloc_max_size);
#endif
  DBGLOG_FORCE( force,   "  Size of umm_block %5u\n", sizeof(umm_block));
  DBGLOG_FORCE( force, "+--------------------------------------------------------------+\n" );
}
#endif

int ICACHE_FLASH_ATTR umm_info_safe_printf_P(const char *fmt, ...) {
    char ram_buf[strlen_P(fmt) + 1];
    strcpy_P(ram_buf, fmt);
    va_list argPtr;
    va_start(argPtr, fmt);
    int result = ets_vprintf(ets_uart_putc1, ram_buf, argPtr);
    va_end(argPtr);
    return result;
}

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
size_t ICACHE_FLASH_ATTR umm_get_oom_count( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->UMM_OOM_COUNT;
}
#endif

#ifdef UMM_STATS_FULL
// TODO - Did I mix something up
//
//   umm_free_heap_size_min      is the same code as
//   umm_free_heap_size_lw_min
//
// If this is correct use alias.
//
size_t ICACHE_FLASH_ATTR umm_free_heap_size_lw_min( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.free_blocks_min * umm_block_size();
}

size_t ICACHE_FLASH_ATTR umm_free_heap_size_min_reset( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  _context->stats.free_blocks_min = _context->UMM_FREE_BLOCKS;
  return _context->stats.free_blocks_min * umm_block_size();
}

#if 0 // TODO - Don't understand this why do both umm_free_heap_size_(lw_)min exist
size_t umm_free_heap_size_min(void) __attribute__ ((alias("umm_free_heap_size_lw_min")));
#else
size_t ICACHE_FLASH_ATTR umm_free_heap_size_min( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.free_blocks_min * umm_block_size();
}
#endif

size_t ICACHE_FLASH_ATTR umm_free_heap_size_isr_min( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.free_blocks_isr_min * umm_block_size();
}

size_t ICACHE_FLASH_ATTR umm_get_max_alloc_size( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.alloc_max_size;
}

size_t ICACHE_FLASH_ATTR umm_get_last_alloc_size( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.last_alloc_size;
}

size_t ICACHE_FLASH_ATTR umm_get_malloc_count( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.id_malloc_count;
}

size_t ICACHE_FLASH_ATTR umm_get_malloc_zero_count( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.id_malloc_zero_count;
}

size_t ICACHE_FLASH_ATTR umm_get_realloc_count( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.id_realloc_count;
}

size_t ICACHE_FLASH_ATTR umm_get_realloc_zero_count( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.id_realloc_zero_count;
}

size_t ICACHE_FLASH_ATTR umm_get_free_count( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.id_free_count;
}

size_t ICACHE_FLASH_ATTR umm_get_free_null_count( void ) {
  umm_heap_context_t *_context = umm_get_current_heap();
  return _context->stats.id_free_null_count;
}
#endif // UMM_STATS_FULL

#endif // BUILD_UMM_MALLOC_C
