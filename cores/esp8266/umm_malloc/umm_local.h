#ifndef _UMM_LOCAL_H
#define _UMM_LOCAL_H
/*
 * A home for local items exclusive to umm_malloc.c and not to be shared in
 * umm_malloc_cfg.h. And, not for upstream version.
 *
 */

#undef memcpy
#undef memmove
#undef memset
#define memcpy ets_memcpy
#define memmove ets_memmove
#define memset ets_memset

#if !defined(UMM_INFO_PRINT)
/* This will stop umm_info from printing.
 * At this time DBGLOG_FORCE is only used by umm_info.
 */
#undef DBGLOG_FORCE
#define DBGLOG_FORCE(force, format, ...) {(void)force; (void)format;}
#endif

#if defined(DEBUG_ESP_OOM) || defined(UMM_POISON_CHECK) || defined(UMM_POISON_CHECK_LITE) || defined(UMM_INTEGRITY_CHECK)
#else

#define umm_malloc(s)    malloc(s)
#define umm_calloc(n,s)  calloc(n,s)
#define umm_realloc(p,s) realloc(p,s)
#define umm_free(p)      free(p)
#endif

#if defined(UMM_POISON_CHECK_LITE)
static int check_poison_neighbors( unsigned short cur );
#endif

#if defined(UMM_STATS) || defined(UMM_STATS_FULL)
void ICACHE_FLASH_ATTR print_stats(int force);
#endif

#endif
