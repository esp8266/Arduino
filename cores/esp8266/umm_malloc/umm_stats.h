/*
 * umm_malloc heap statistics
 */

#ifndef _UMM_STATS_H
#define _UMM_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UMM_STATS_t {
  unsigned short int free_blocks;
  unsigned short int free_blocks_min;
  size_t alloc_max_size;
  size_t oom_count;
} UMM_STATS;
extern UMM_STATS ummStats;

size_t ICACHE_FLASH_ATTR umm_free_heap_size_min( void );
size_t ICACHE_FLASH_ATTR umm_free_heap_size_min_reset( void );

size_t inline umm_get_max_alloc_size( void ) {
  return ummStats.alloc_max_size;
}

size_t inline umm_get_oom_count( void ) {
  return ummStats.oom_count;
}


#ifdef __cplusplus
}
#endif

#endif  /* _UMM_STATS_H */
