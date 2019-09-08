/*
 * Local Additions/Enhancements
 *
 */
#if defined(BUILD_UMM_MALLOC_C)

#if defined(UMM_CRITICAL_METRICS)
/*
 * umm_malloc performance measurments for critical sections
 */
UMM_TIME_STATS time_stats = {
    {0xFFFFFFFF, 0U, 0U, 0U},
    {0xFFFFFFFF, 0U, 0U, 0U},
    {0xFFFFFFFF, 0U, 0U, 0U},
#ifdef UMM_INFO
    {0xFFFFFFFF, 0U, 0U, 0U},
#endif
#ifdef UMM_POISON_CHECK
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

static int check_poison_neighbors( unsigned short cur ) {
  unsigned short int c;

  if ( 0 == cur )
    return 1;

  c = UMM_PBLOCK(cur) & UMM_BLOCKNO_MASK;
  while( c && (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) ) {
    /*
       There can be up to 1 free block neighbor in either direction.
       This loop should self limit to 2 passes, due to heap design.
       i.e. Adjacent free space is always consolidated.
     */
    if ( !(UMM_NBLOCK(c) & UMM_FREELIST_MASK) ) {
      if ( !check_poison_block(&UMM_BLOCK(c)) )
        return 0;

      break;
    }

    c = UMM_PBLOCK(c) & UMM_BLOCKNO_MASK;
  }

  c = UMM_NBLOCK(cur) & UMM_BLOCKNO_MASK;
  while( (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) ) {
    if ( !(UMM_NBLOCK(c) & UMM_FREELIST_MASK) ) {
      if ( !check_poison_block(&UMM_BLOCK(c)) )
        return 0;

      break;
    }

    c = UMM_NBLOCK(c) & UMM_BLOCKNO_MASK;
  }

  return 1;
}
#endif

#if defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE)

/* ------------------------------------------------------------------------ */

static void *get_unpoisoned_check_neighbors( void *v_ptr, const char* file, int line ) {
  unsigned char *ptr = (unsigned char *)v_ptr;

  if (ptr != NULL) {

    ptr -= (sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE);

#if defined(UMM_POISON_CHECK_LITE)
    UMM_CRITICAL_DECL(id_poison);
    unsigned short int c;
    bool poison = false;

    /* Figure out which block we're in. Note the use of truncated division... */
    c = (((char *)ptr)-(char *)(&(umm_heap[0])))/sizeof(umm_block);

    UMM_CRITICAL_ENTRY(id_poison);
    poison = check_poison_block(&UMM_BLOCK(c)) && check_poison_neighbors(c);
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

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
size_t umm_free_heap_size( void ) __attribute__ ((alias("umm_free_heap_size_lw")));
size_t xPortGetFreeHeapSize(void) __attribute__ ((alias("umm_free_heap_size_lw")));
#elif defined(UMM_INFO)
size_t umm_free_heap_size( void ) __attribute__ ((alias("umm_free_heap_size_info")));
size_t xPortGetFreeHeapSize(void) __attribute__ ((alias("umm_free_heap_size_info")));
#endif

#endif
