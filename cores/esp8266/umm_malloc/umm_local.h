#ifndef _UMM_LOCAL_H
#define _UMM_LOCAL_H
/*
 * A home for local items exclusive to umm_malloc.c and not to be shared in
 * umm_malloc_cfg.h. And, not for upstream version.
 *
 */
#if defined(DEBUG_ESP_OOM) || defined(UMM_CHECK_POISON) || defined(UMM_POISON_CHECK_LITE) || defined(UMM_INTEGRITY_CHECK)
#else

#define umm_malloc(s)    malloc(s)
#define umm_calloc(n,s)  calloc(n,s)
#define umm_realloc(p,s) realloc(p,s)
#define umm_free(p)      free(p)
#endif

#endif
