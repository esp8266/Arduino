/* This file is part of the CodeSourcery Common Startup Code Sequence (CS3).

   Copyright (c) 2007 - 2011 CodeSourcery, Inc.
 * Version:Sourcery G++ Lite 2011.03-42
 * BugURL:https://support.codesourcery.com/GNUToolchain/

   THIS FILE CONTAINS PROPRIETARY, CONFIDENTIAL, AND TRADE SECRET
   INFORMATION OF CODESOURCERY AND/OR ITS LICENSORS.

   You may not use, modify or distribute this file without the express
   written permission of CodeSourcery or its authorized
   distributor. Please consult your license agreement for the
   applicable terms and conditions.  */

#ifndef CSL_CS3_H
#define CSL_CS3_H

#include <stddef.h>

#if __cplusplus
extern "C" {
#endif

/* The __cs3_regions array is used by CS3's startup code to initialize
   the contents of memory regions during the C initialization phase.

   For each region descriptor __cs3_regions[i], __cs3_regions[i].init_size
   bytes beginning at __cs3_regions[i].data are initialized by copying from
   __cs3_regions[i].init.  (E.g., the data field is the VMA and the init
   field is the LMA.)  Then the following __cs3_regions[i].zero_size bytes
   are zero-initialized.

   __cs3_regions is normally defined in the linker script.  */

typedef unsigned char __cs3_byte_align8 __attribute ((aligned (8)));

struct __cs3_region
{
  unsigned flags;       /* Flags for this region.  None defined yet.  */
  __cs3_byte_align8 *init;  /* Initial contents of this region.  */
  __cs3_byte_align8 *data;  /* Start address of region.  */
  size_t init_size;     /* Size of initial data.  */
  size_t zero_size;     /* Additional size to be zeroed.  */
};

extern const struct __cs3_region __cs3_regions[];

/* The number of elements in __cs3_regions.  This is weak, so the
   compiler does not presume it is non-zero.  */

extern const char __cs3_region_num __attribute__((weak));
#define __cs3_region_num ((size_t)&__cs3_region_num)

/* __cs3_start_c is the entry point to the C initialization phase.
   CS3's library provides a default implementation, but it is possible
   for user code to override it by defining this function.  */

extern void __cs3_start_c (void) __attribute ((noreturn));

/* Space for the heap is designated by the array __cs3_heap_start.
   __cs3_heap_limit points at the end of the heap.  

   CS3 provides default definitions for __cs3_heap_start and
   __cs3_heap_end in the linker scripts (typically placed after the
   .data and .bss sections in RAM), and a default definition for
   __cs3_heap_limit pointing to __cs3_heap_end in the CS3 library.
   As a special case, in some profiles &__cs3_heap_end may be zero to
   indicate that the end of the heap must be determined some other way,
   such as by a supervisory operation on a simulator, or simply by
   treating the stack pointer as the limit.

   User programs may override these default definitions either by using
   a custom linker script, or by defining __cs3_heap_start and
   __cs3_heap_limit appropriately from C.  */

extern unsigned char __cs3_heap_start[] __attribute ((aligned (8)));
extern unsigned char __cs3_heap_end[] __attribute ((aligned (8)));
extern void *__cs3_heap_limit;

/* The default initial stack pointer.  This is normally defined by the
   linker script except in profiles where the stack pointer is
   initialized externally, e.g. by a simulator or boot monitor.
   Refer to the documentation for the Assembly Initialization Phase and Heap
   and Stack Placement.  */
extern unsigned char __cs3_stack[] __attribute ((aligned (16)));

/* The macro CS3_STACK can be used for creating a custom stack.  Refer to the
   documentation for Heap and Stack Placement.  */
#define CS3_STACK(size) \
  CS3_STACK_SYMBOL(__cs3_stack_block, (size), 16)
/* Create a custom stack with name SYMBOL, aligned to ALIGNMENT bytes, sized by
   SIZE bytes, but possibly shortened such that the initial stack pointer
   (symbol __cs3_stack) that points to the block's last extent is aligned to
   ALIGNMENT bytes, too.  */
#define CS3_STACK_SYMBOL(symbol, size, alignment) \
  static char __attribute__ ((aligned (alignment))) \
    symbol[(size - ((size) % (alignment)))]; \
  asm (".global __cs3_stack"); \
  asm ("__cs3_stack = " #symbol " + (" #size ") - (" #alignment ")" \
       " - ((" #size ") % (" #alignment "))")

/* Regions.  Some may not be present on particular boards or profiles. */

/* We use weak on objects that might be at address zero.
   The compiler is at liberty to presume that no non-weak
   object resides at address zero (because that's
   indistinguishable from the NULL pointer on the systems
   we care about).  */

/* itcm region */
extern unsigned char __cs3_region_start_itcm[] __attribute__((weak,aligned(8)));
extern unsigned char __cs3_region_size_itcm[] __attribute__((aligned(8)));
extern const unsigned char __cs3_region_init_itcm[] __attribute__((weak,aligned(8)));
extern const char __cs3_region_init_size_itcm __attribute__((weak,aligned(8)));
#define __cs3_region_init_size_itcm ((size_t)&__cs3_region_init_size_itcm)
extern const char __cs3_region_zero_size_itcm __attribute__((weak,aligned(8)));
#define __cs3_region_zero_size_itcm ((size_t)&__cs3_region_zero_size_itcm)

/* ram region */
extern unsigned char __cs3_region_start_ram[] __attribute__((weak,aligned(8)));
extern unsigned char __cs3_region_size_ram[] __attribute__((aligned(8)));
extern const unsigned char __cs3_region_init_ram[] __attribute__((weak,aligned(8)));
extern const char __cs3_region_init_size_ram __attribute__((weak,aligned(8)));
#define __cs3_region_init_size_ram ((size_t)&__cs3_region_init_size_ram)
extern const char __cs3_region_zero_size_ram __attribute__((weak,aligned(8)));
#define __cs3_region_zero_size_ram ((size_t)&__cs3_region_zero_size_ram)

/* rom region */
extern unsigned char __cs3_region_start_rom[] __attribute__((weak,aligned(8)));
extern unsigned char __cs3_region_size_rom[] __attribute__((aligned(8)));
extern const unsigned char __cs3_region_init_rom[] __attribute__((weak,aligned(8)));
extern const char __cs3_region_init_size_rom __attribute__((weak,aligned(8)));
#define __cs3_region_init_size_rom ((size_t)&__cs3_region_init_size_rom)
extern const char __cs3_region_zero_size_rom __attribute__((weak,aligned(8)));
#define __cs3_region_zero_size_rom ((size_t)&__cs3_region_zero_size_rom)

#if __cplusplus
}
#endif
#endif /* CSL_CS3_H */
