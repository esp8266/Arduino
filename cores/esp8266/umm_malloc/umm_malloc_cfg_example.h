/*
 * Configuration for umm_malloc
 */

#ifndef _UMM_MALLOC_CFG_H
#define _UMM_MALLOC_CFG_H

/*
 * There are a number of defines you can set at compile time that affect how
 * the memory allocator will operate.
 * You can set them in your config file umm_malloc_cfg.h.
 * In GNU C, you also can set these compile time defines like this:
 *
 * -D UMM_TEST_MAIN
 *
 * Set this if you want to compile in the test suite
 *
 * -D UMM_BEST_FIT (defualt)
 *
 * Set this if you want to use a best-fit algorithm for allocating new
 * blocks
 *
 * -D UMM_FIRST_FIT
 *
 * Set this if you want to use a first-fit algorithm for allocating new
 * blocks
 *
 * -D UMM_DBG_LOG_LEVEL=n
 *
 * Set n to a value from 0 to 6 depending on how verbose you want the debug
 * log to be
 *
 * ----------------------------------------------------------------------------
 *
 * Support for this library in a multitasking environment is provided when
 * you add bodies to the UMM_CRITICAL_ENTRY and UMM_CRITICAL_EXIT macros
 * (see below)
 *
 * ----------------------------------------------------------------------------
 */


/* Start addresses and the size of the heap */
#define UMM_MALLOC_CFG_HEAP_ADDR   /* TODO */
#define UMM_MALLOC_CFG_HEAP_SIZE   /* TODO */

/* A couple of macros to make packing structures less compiler dependent */

#define UMM_H_ATTPACKPRE
#define UMM_H_ATTPACKSUF __attribute__((__packed__))

#define UMM_BEST_FIT
#undef  UMM_FIRST_FIT

/*
 * -D UMM_INFO :
 *
 * Enables a dup of the heap contents and a function to return the total
 * heap size that is unallocated - note this is not the same as the largest
 * unallocated block on the heap!
 */

/*
#define UMM_INFO
*/

#ifdef UMM_INFO
  typedef struct UMM_HEAP_INFO_t {
    unsigned short int totalEntries;
    unsigned short int usedEntries;
    unsigned short int freeEntries;

    unsigned short int totalBlocks;
    unsigned short int usedBlocks;
    unsigned short int freeBlocks;

    unsigned short int maxFreeContiguousBlocks;
  }
  UMM_HEAP_INFO;

  extern UMM_HEAP_INFO ummHeapInfo;

  void *umm_info( void *ptr, int force );
  size_t umm_free_heap_size( void );

#else
#endif

/*
 * A couple of macros to make it easier to protect the memory allocator
 * in a multitasking system. You should set these macros up to use whatever
 * your system uses for this purpose. You can disable interrupts entirely, or
 * just disable task switching - it's up to you
 *
 * NOTE WELL that these macros MUST be allowed to nest, because umm_free() is
 * called from within umm_malloc()
 */

#define UMM_CRITICAL_ENTRY()
#define UMM_CRITICAL_EXIT()

/*
 * -D UMM_INTEGRITY_CHECK :
 *
 * Enables heap integrity check before any heap operation. It affects
 * performance, but does NOT consume extra memory.
 *
 * If integrity violation is detected, the message is printed and user-provided
 * callback is called: `UMM_HEAP_CORRUPTION_CB()`
 *
 * Note that not all buffer overruns are detected: each buffer is aligned by
 * 4 bytes, so there might be some trailing "extra" bytes which are not checked
 * for corruption.
 */

/*
#define UMM_INTEGRITY_CHECK
*/

#ifdef UMM_INTEGRITY_CHECK
   int umm_integrity_check( void );
#  define INTEGRITY_CHECK() umm_integrity_check()
   extern void umm_corruption(void);
#  define UMM_HEAP_CORRUPTION_CB() printf( "Heap Corruption!" )
#else
#  define INTEGRITY_CHECK() 0
#endif

/*
 * -D UMM_POISON :
 *
 * Enables heap poisoning: add predefined value (poison) before and after each
 * allocation, and check before each heap operation that no poison is
 * corrupted.
 *
 * Other than the poison itself, we need to store exact user-requested length
 * for each buffer, so that overrun by just 1 byte will be always noticed.
 *
 * Customizations:
 *
 *    UMM_POISON_SIZE_BEFORE:
 *      Number of poison bytes before each block, e.g. 2
 *    UMM_POISON_SIZE_AFTER:
 *      Number of poison bytes after each block e.g. 2
 *    UMM_POISONED_BLOCK_LEN_TYPE
 *      Type of the exact buffer length, e.g. `short`
 *
 * NOTE: each allocated buffer is aligned by 4 bytes. But when poisoning is
 * enabled, actual pointer returned to user is shifted by
 * `(sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE)`.
 * It's your responsibility to make resulting pointers aligned appropriately.
 *
 * If poison corruption is detected, the message is printed and user-provided
 * callback is called: `UMM_HEAP_CORRUPTION_CB()`
 */

/*
#define UMM_POISON_CHECK
*/

#define UMM_POISON_SIZE_BEFORE 4
#define UMM_POISON_SIZE_AFTER 4
#define UMM_POISONED_BLOCK_LEN_TYPE short

#ifdef UMM_POISON_CHECK
   void *umm_poison_malloc( size_t size );
   void *umm_poison_calloc( size_t num, size_t size );
   void *umm_poison_realloc( void *ptr, size_t size );
   void  umm_poison_free( void *ptr );
   int   umm_poison_check( void );
#  define POISON_CHECK() umm_poison_check()
#else
#  define POISON_CHECK() 0
#endif

#endif /* _UMM_MALLOC_CFG_H */
