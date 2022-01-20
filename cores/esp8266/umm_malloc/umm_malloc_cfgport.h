#ifndef _UMM_MALLOC_CFGPORT_H
#define _UMM_MALLOC_CFGPORT_H

#ifndef _UMM_MALLOC_CFG_H
#error "This include file must be used with umm_malloc_cfg.h"
#endif

/*
 * Arduino ESP8266 core umm_malloc port config
 */
#include <pgmspace.h>
#include <mmu_iram.h>
#include "../debug.h"
#include "../esp8266_undocumented.h"

#include <core_esp8266_features.h>
#include <stdlib.h>
#include <osapi.h>

#include "c_types.h"

/*
 * -DUMM_INIT_USE_ICACHE
 *
 * Historically, the umm_init() call path has been in IRAM. This build option,
 * UMM_INIT_USE_ICACHE, moves it to IROM (flash). When this option is used,
 * changes are needed in functions app_entry_redefinable() for the way it calls
 * umm_init() to enable/disable ICACHE.
 *
 * The default is to preserve the historical IRAM use case.
 */
// #define UMM_INIT_USE_ICACHE

/*
 * Start addresses and the size of the heap
 */
extern char _heap_start[];
#define UMM_HEAP_END_ADDR          0x3FFFC000UL
#define UMM_MALLOC_CFG_HEAP_ADDR   ((uint32_t)&_heap_start[0])
#define UMM_MALLOC_CFG_HEAP_SIZE   ((size_t)(UMM_HEAP_END_ADDR - UMM_MALLOC_CFG_HEAP_ADDR))

/*
 * Define active Heaps
 */
#if defined(MMU_IRAM_HEAP)
#define UMM_HEAP_IRAM
#else
#undef UMM_HEAP_IRAM
#endif

#if defined(MMU_EXTERNAL_HEAP)
#define UMM_HEAP_EXTERNAL
#else
#undef UMM_HEAP_EXTERNAL
#endif

/*
 * Assign IDs to active Heaps and tally. DRAM is always active.
 */
#define UMM_HEAP_DRAM 0
#define UMM_HEAP_DRAM_DEFINED 1

#ifdef UMM_HEAP_IRAM
#undef UMM_HEAP_IRAM
#define UMM_HEAP_IRAM_DEFINED 1
#define UMM_HEAP_IRAM UMM_HEAP_DRAM_DEFINED
#else
#define UMM_HEAP_IRAM_DEFINED 0
#endif

#ifdef UMM_HEAP_EXTERNAL
#undef UMM_HEAP_EXTERNAL
#define UMM_HEAP_EXTERNAL_DEFINED 1
#define UMM_HEAP_EXTERNAL (UMM_HEAP_DRAM_DEFINED + UMM_HEAP_IRAM_DEFINED)
#else
#define UMM_HEAP_EXTERNAL_DEFINED 0
#endif

#define UMM_NUM_HEAPS (UMM_HEAP_DRAM_DEFINED + UMM_HEAP_IRAM_DEFINED + UMM_HEAP_EXTERNAL_DEFINED)

#if (UMM_NUM_HEAPS == 1)
#else
#define UMM_HEAP_STACK_DEPTH 32
#endif


#endif
