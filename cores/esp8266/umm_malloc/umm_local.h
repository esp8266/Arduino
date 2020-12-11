#ifndef _UMM_LOCAL_H
#define _UMM_LOCAL_H
/*
 * A home for local items exclusive to umm_malloc.c and not to be shared in
 * umm_malloc_cfg.h. And, not for upstream version.
 * Also used to redefine defines made in upstream files we donet want to edit.
 *
 */

#undef memcpy
#undef memmove
#undef memset
#define memcpy ets_memcpy
#define memmove ets_memmove
#define memset ets_memset


/*
 * This redefines DBGLOG_FORCE defined in dbglog/dbglog.h
 * Just for printing from umm_info() which is assumed to always be called from
 * non-ISR. Thus SPI bus is available to handle cache-miss and reading a flash
 * string while INTLEVEL is non-zero.
 */
#undef DBGLOG_FORCE
#define DBGLOG_FORCE(force, format, ...) {if(force) {UMM_INFO_PRINTF(format, ## __VA_ARGS__);}}
// #define DBGLOG_FORCE(force, format, ...) {if(force) {::printf(PSTR(format), ## __VA_ARGS__);}}


#if defined(DEBUG_ESP_OOM) || defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE) || defined(UMM_INTEGRITY_CHECK)
#else

#define umm_malloc(s)    malloc(s)
#define umm_calloc(n,s)  calloc(n,s)
#define umm_realloc(p,s) realloc(p,s)
#define umm_free(p)      free(p)
#endif


#if defined(UMM_POISON_CHECK_LITE)
static bool check_poison_neighbors( umm_heap_context_t *_context, uint16_t cur );
#endif


#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
void ICACHE_FLASH_ATTR umm_print_stats(int force);
#endif



int ICACHE_FLASH_ATTR umm_info_safe_printf_P(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#define UMM_INFO_PRINTF(fmt, ...) umm_info_safe_printf_P(PSTR4(fmt), ##__VA_ARGS__)
// use PSTR4() instead of PSTR() to ensure 4-bytes alignment in Flash, whatever the default alignment of PSTR_ALIGN


typedef struct umm_block_t umm_block;

struct UMM_HEAP_CONTEXT {
  umm_block *heap;
  void *heap_end;
#if (!defined(UMM_INLINE_METRICS) && defined(UMM_STATS)) || defined(UMM_STATS_FULL)
  UMM_STATISTICS stats;
#endif
#ifdef UMM_INFO
  UMM_HEAP_INFO info;
#endif
  unsigned short int numblocks;
  unsigned char id;
};


#endif
