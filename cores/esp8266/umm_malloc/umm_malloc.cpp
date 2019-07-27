/* ----------------------------------------------------------------------------
 * umm_malloc.c - a memory allocator for embedded systems (microcontrollers)
 *
 * See copyright notice in LICENSE.TXT
 * ----------------------------------------------------------------------------
 *
 * R.Hempel 2007-09-22 - Original
 * R.Hempel 2008-12-11 - Added MIT License biolerplate
 *                     - realloc() now looks to see if previous block is free
 *                     - made common operations functions
 * R.Hempel 2009-03-02 - Added macros to disable tasking
 *                     - Added function to dump heap and check for valid free
 *                        pointer
 * R.Hempel 2009-03-09 - Changed name to umm_malloc to avoid conflicts with
 *                        the mm_malloc() library functions
 *                     - Added some test code to assimilate a free block
 *                        with the very block if possible. Complicated and
 *                        not worth the grief.
 * D.Frank 2014-04-02  - Fixed heap configuration when UMM_TEST_MAIN is NOT set,
 *                        added user-dependent configuration file umm_malloc_cfg.h
 * ----------------------------------------------------------------------------
 *
 *  Note: when upgrading this file with upstream code, replace all %i with %d in
 *        printf format strings. ets_printf doesn't handle %i.
 *
 * ----------------------------------------------------------------------------
 *
 * This is a memory management library specifically designed to work with the
 * ARM7 embedded processor, but it should work on many other 32 bit processors,
 * as well as 16 and 8 bit devices.
 *
 * ACKNOWLEDGEMENTS
 *
 * Joerg Wunsch and the avr-libc provided the first malloc() implementation
 * that I examined in detail.
 *
 * http: *www.nongnu.org/avr-libc
 *
 * Doug Lea's paper on malloc() was another excellent reference and provides
 * a lot of detail on advanced memory management techniques such as binning.
 *
 * http: *g.oswego.edu/dl/html/malloc.html
 *
 * Bill Dittman provided excellent suggestions, including macros to support
 * using these functions in critical sections, and for optimizing realloc()
 * further by checking to see if the previous block was free and could be
 * used for the new block size. This can help to reduce heap fragmentation
 * significantly.
 *
 * Yaniv Ankin suggested that a way to dump the current heap condition
 * might be useful. I combined this with an idea from plarroy to also
 * allow checking a free pointer to make sure it's valid.
 *
 * ----------------------------------------------------------------------------
 *
 * The memory manager assumes the following things:
 *
 * 1. The standard POSIX compliant malloc/realloc/free semantics are used
 * 2. All memory used by the manager is allocated at link time, it is aligned
 *    on a 32 bit boundary, it is contiguous, and its extent (start and end
 *    address) is filled in by the linker.
 * 3. All memory used by the manager is initialized to 0 as part of the
 *    runtime startup routine. No other initialization is required.
 *
 * The fastest linked list implementations use doubly linked lists so that
 * its possible to insert and delete blocks in constant time. This memory
 * manager keeps track of both free and used blocks in a doubly linked list.
 *
 * Most memory managers use some kind of list structure made up of pointers
 * to keep track of used - and sometimes free - blocks of memory. In an
 * embedded system, this can get pretty expensive as each pointer can use
 * up to 32 bits.
 *
 * In most embedded systems there is no need for managing large blocks
 * of memory dynamically, so a full 32 bit pointer based data structure
 * for the free and used block lists is wasteful. A block of memory on
 * the free list would use 16 bytes just for the pointers!
 *
 * This memory management library sees the malloc heap as an array of blocks,
 * and uses block numbers to keep track of locations. The block numbers are
 * 15 bits - which allows for up to 32767 blocks of memory. The high order
 * bit marks a block as being either free or in use, which will be explained
 * later.
 *
 * The result is that a block of memory on the free list uses just 8 bytes
 * instead of 16.
 *
 * In fact, we go even one step futher when we realize that the free block
 * index values are available to store data when the block is allocated.
 *
 * The overhead of an allocated block is therefore just 4 bytes.
 *
 * Each memory block holds 8 bytes, and there are up to 32767 blocks
 * available, for about 256K of heap space. If that's not enough, you
 * can always add more data bytes to the body of the memory block
 * at the expense of free block size overhead.
 *
 * There are a lot of little features and optimizations in this memory
 * management system that makes it especially suited to small embedded, but
 * the best way to appreciate them is to review the data structures and
 * algorithms used, so let's get started.
 *
 * ----------------------------------------------------------------------------
 *
 * We have a general notation for a block that we'll use to describe the
 * different scenarios that our memory allocation algorithm must deal with:
 *
 *    +----+----+----+----+
 * c  |* n |  p | nf | pf |
 *    +----+----+----+----+
 *
 * Where - c  is the index of this block
 *         *  is the indicator for a free block
 *         n  is the index of the next block in the heap
 *         p  is the index of the previous block in the heap
 *         nf is the index of the next block in the free list
 *         pf is the index of the previous block in the free list
 *
 * The fact that we have forward and backward links in the block descriptors
 * means that malloc() and free() operations can be very fast. It's easy
 * to either allocate the whole free item to a new block or to allocate part
 * of the free item and leave the rest on the free list without traversing
 * the list from front to back first.
 *
 * The entire block of memory used by the heap is assumed to be initialized
 * to 0. The very first block in the heap is special - it't the head of the
 * free block list. It is never assimilated with a free block (more on this
 * later).
 *
 * Once a block has been allocated to the application, it looks like this:
 *
 *    +----+----+----+----+
 *  c |  n |  p |   ...   |
 *    +----+----+----+----+
 *
 * Where - c  is the index of this block
 *         n  is the index of the next block in the heap
 *         p  is the index of the previous block in the heap
 *
 * Note that the free list information is gone, because it's now being used to
 * store actual data for the application. It would have been nice to store
 * the next and previous free list indexes as well, but that would be a waste
 * of space. If we had even 500 items in use, that would be 2,000 bytes for
 * free list information. We simply can't afford to waste that much.
 *
 * The address of the ... area is what is returned to the application
 * for data storage.
 *
 * The following sections describe the scenarios encountered during the
 * operation of the library. There are two additional notation conventions:
 *
 * ?? inside a pointer block means that the data is irrelevant. We don't care
 *    about it because we don't read or modify it in the scenario being
 *    described.
 *
 * ... between memory blocks indicates zero or more additional blocks are
 *     allocated for use by the upper block.
 *
 * And while we're talking about "upper" and "lower" blocks, we should make
 * a comment about adresses. In the diagrams, a block higher up in the
 * picture is at a lower address. And the blocks grow downwards their
 * block index increases as does their physical address.
 *
 * Finally, there's one very important characteristic of the individual
 * blocks that make up the heap - there can never be two consecutive free
 * memory blocks, but there can be consecutive used memory blocks.
 *
 * The reason is that we always want to have a short free list of the
 * largest possible block sizes. By always assimilating a newly freed block
 * with adjacent free blocks, we maximize the size of each free memory area.
 *
 *---------------------------------------------------------------------------
 *
 * Operation of malloc right after system startup
 *
 * As part of the system startup code, all of the heap has been cleared.
 *
 * During the very first malloc operation, we start traversing the free list
 * starting at index 0. The index of the next free block is 0, which means
 * we're at the end of the list!
 *
 * At this point, the malloc has a special test that checks if the current
 * block index is 0, which it is. This special case initializes the free
 * list to point at block index 1.
 *
 *    BEFORE                             AFTER
 *
 *    +----+----+----+----+              +----+----+----+----+
 * 0  |  0 |  0 |  0 |  0 |           0  |  1 |  0 |  1 |  0 |
 *    +----+----+----+----+              +----+----+----+----+
 *                                       +----+----+----+----+
 *                                    1  |  0 |  0 |  0 |  0 |
 *                                       +----+----+----+----+
 *
 * The heap is now ready to complete the first malloc operation.
 *
 * ----------------------------------------------------------------------------
 *
 * Operation of malloc when we have reached the end of the free list and
 * there is no block large enough to accommodate the request.
 *
 * This happens at the very first malloc operation, or any time the free
 * list is traversed and no free block large enough for the request is
 * found.
 *
 * The current block pointer will be at the end of the free list, and we
 * know we're at the end of the list because the nf index is 0, like this:
 *
 *    BEFORE                             AFTER
 *
 *    +----+----+----+----+              +----+----+----+----+
 * pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | lf | ?? |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 *  p | cf | ?? |   ...   |            p | cf | ?? |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+
 * cf |  0 |  p |  0 | pf |            c | lf |  p |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *                                       +----+----+----+----+
 *                                    lf |  0 | cf |  0 | pf |
 *                                       +----+----+----+----+
 *
 * As we walk the free list looking for a block of size b or larger, we get
 * to cf, which is the last item in the free list. We know this because the
 * next index is 0.
 *
 * So we're going to turn cf into the new block of memory, and then create
 * a new block that represents the last free entry (lf) and adjust the prev
 * index of lf to point at the  block we just created. We also need to adjust
 * the next index of the new block (c) to point to the last free block.
 *
 * Note that the next free index of the pf block must point to the new lf
 * because cf is no longer a free block!
 *
 * ----------------------------------------------------------------------------
 *
 * Operation of malloc when we have found a block (cf) that will fit the
 * current request of b units exactly.
 *
 * This one is pretty easy, just clear the free list bit in the current
 * block and unhook it from the free list.
 *
 *    BEFORE                             AFTER
 *
 *    +----+----+----+----+              +----+----+----+----+
 * pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | nf | ?? |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 *  p | cf | ?? |   ...   |            p | cf | ?? |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+  Clear the free
 * cf |* n |  p | nf | pf |           cf |  n |  p |   ..    |  list bit here
 *    +----+----+----+----+              +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+
 *  n | ?? | cf |   ...   |            n | ?? | cf |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 * nf |*?? | ?? | ?? | cf |           nf | ?? | ?? | ?? | pf |
 *    +----+----+----+----+              +----+----+----+----+
 *
 * Unhooking from the free list is accomplished by adjusting the next and
 * prev free list index values in the pf and nf blocks.
 *
 * ----------------------------------------------------------------------------
 *
 * Operation of malloc when we have found a block that will fit the current
 * request of b units with some left over.
 *
 * We'll allocate the new block at the END of the current free block so we
 * don't have to change ANY free list pointers.
 *
 *    BEFORE                             AFTER
 *
 *    +----+----+----+----+              +----+----+----+----+
 * pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | cf | ?? |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 *  p | cf | ?? |   ...   |            p | cf | ?? |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+
 * cf |* n |  p | nf | pf |           cf |* c |  p | nf | pf |
 *    +----+----+----+----+              +----+----+----+----+
 *                                       +----+----+----+----+ This is the new
 *                                     c |  n | cf |   ..    | block at cf+b
 *                                       +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+
 *  n | ?? | cf |   ...   |            n | ?? |  c |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 * nf |*?? | ?? | ?? | cf |           nf | ?? | ?? | ?? | pf |
 *    +----+----+----+----+              +----+----+----+----+
 *
 * This one is prety easy too, except we don't need to mess with the
 * free list indexes at all becasue we'll allocate the new block at the
 * end of the current free block. We do, however have to adjust the
 * indexes in cf, c, and n.
 *
 * ----------------------------------------------------------------------------
 *
 * That covers the initialization and all possible malloc scenarios, so now
 * we need to cover the free operation possibilities...
 *
 * The operation of free depends on the position of the current block being
 * freed relative to free list items immediately above or below it. The code
 * works like this:
 *
 * if next block is free
 *   assimilate with next block already on free list
 * if prev block is free
 *   assimilate with prev block already on free list
 * else
 *   put current block at head of free list
 *
 * ----------------------------------------------------------------------------
 *
 * Step 1 of the free operation checks if the next block is free, and if it
 * is then insert this block into the free list and assimilate the next block
 * with this one.
 *
 * Note that c is the block we are freeing up, cf is the free block that
 * follows it.
 *
 *    BEFORE                             AFTER
 *
 *    +----+----+----+----+              +----+----+----+----+
 * pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | nf | ?? |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 *  p |  c | ?? |   ...   |            p |  c | ?? |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+ This block is
 *  c | cf |  p |   ...   |            c | nn |  p |   ...   | disconnected
 *    +----+----+----+----+              +----+----+----+----+ from free list,
 *    +----+----+----+----+                                    assimilated with
 * cf |*nn |  c | nf | pf |                                    the next, and
 *    +----+----+----+----+                                    ready for step 2
 *    +----+----+----+----+              +----+----+----+----+
 * nn | ?? | cf | ?? | ?? |           nn | ?? |  c |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 * nf |*?? | ?? | ?? | cf |           nf |*?? | ?? | ?? | pf |
 *    +----+----+----+----+              +----+----+----+----+
 *
 * Take special note that the newly assimilated block (c) is completely
 * disconnected from the free list, and it does not have its free list
 * bit set. This is important as we move on to step 2 of the procedure...
 *
 * ----------------------------------------------------------------------------
 *
 * Step 2 of the free operation checks if the prev block is free, and if it
 * is then assimilate it with this block.
 *
 * Note that c is the block we are freeing up, pf is the free block that
 * precedes it.
 *
 *    BEFORE                             AFTER
 *
 *    +----+----+----+----+              +----+----+----+----+ This block has
 * pf |* c | ?? | nf | ?? |           pf |* n | ?? | nf | ?? | assimilated the
 *    +----+----+----+----+              +----+----+----+----+ current block
 *    +----+----+----+----+
 *  c |  n | pf |   ...   |
 *    +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+
 *  n | ?? |  c |   ...   |            n | ?? | pf | ?? | ?? |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 * nf |*?? | ?? | ?? | pf |           nf |*?? | ?? | ?? | pf |
 *    +----+----+----+----+              +----+----+----+----+
 *
 * Nothing magic here, except that when we're done, the current block (c)
 * is gone since it's been absorbed into the previous free block. Note that
 * the previous step guarantees that the next block (n) is not free.
 *
 * ----------------------------------------------------------------------------
 *
 * Step 3 of the free operation only runs if the previous block is not free.
 * it just inserts the current block to the head of the free list.
 *
 * Remember, 0 is always the first block in the memory heap, and it's always
 * head of the free list!
 *
 *    BEFORE                             AFTER
 *
 *    +----+----+----+----+              +----+----+----+----+
 *  0 | ?? | ?? | nf |  0 |            0 | ?? | ?? |  c |  0 |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 *  p |  c | ?? |   ...   |            p |  c | ?? |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+
 *  c |  n |  p |   ..    |            c |* n |  p | nf |  0 |
 *    +----+----+----+----+              +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+
 *  n | ?? |  c |   ...   |            n | ?? |  c |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 * nf |*?? | ?? | ?? |  0 |           nf |*?? | ?? | ?? |  c |
 *    +----+----+----+----+              +----+----+----+----+
 *
 * Again, nothing spectacular here, we're simply adjusting a few pointers
 * to make the most recently freed block the first item in the free list.
 *
 * That's because finding the previous free block would mean a reverse
 * traversal of blocks until we found a free one, and it's just easier to
 * put it at the head of the list. No traversal is needed.
 *
 * ----------------------------------------------------------------------------
 *
 * Finally, we can cover realloc, which has the following basic operation.
 *
 * The first thing we do is assimilate up with the next free block of
 * memory if possible. This step might help if we're resizing to a bigger
 * block of memory. It also helps if we're downsizing and creating a new
 * free block with the leftover memory.
 *
 * First we check to see if the next block is free, and we assimilate it
 * to this block if it is. If the previous block is also free, and if
 * combining it with the current block would satisfy the request, then we
 * assimilate with that block and move the current data down to the new
 * location.
 *
 * Assimilating with the previous free block and moving the data works
 * like this:
 *
 *    BEFORE                             AFTER
 *
 *    +----+----+----+----+              +----+----+----+----+
 * pf |*?? | ?? | cf | ?? |           pf |*?? | ?? | nf | ?? |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 * cf |* c | ?? | nf | pf |            c |  n | ?? |   ...   | The data gets
 *    +----+----+----+----+              +----+----+----+----+ moved from c to
 *    +----+----+----+----+                                    the new data area
 *  c |  n | cf |   ...   |                                    in cf, then c is
 *    +----+----+----+----+                                    adjusted to cf
 *    +----+----+----+----+              +----+----+----+----+
 *  n | ?? |  c |   ...   |            n | ?? |  c | ?? | ?? |
 *    +----+----+----+----+              +----+----+----+----+
 *             ...                                ...
 *    +----+----+----+----+              +----+----+----+----+
 * nf |*?? | ?? | ?? | cf |           nf |*?? | ?? | ?? | pf |
 *    +----+----+----+----+              +----+----+----+----+
 *
 *
 * Once we're done that, there are three scenarios to consider:
 *
 * 1. The current block size is exactly the right size, so no more work is
 *    needed.
 *
 * 2. The current block is bigger than the new required size, so carve off
 *    the excess and add it to the free list.
 *
 * 3. The current block is still smaller than the required size, so malloc
 *    a new block of the correct size and copy the current data into the new
 *    block before freeing the current block.
 *
 * The only one of these scenarios that involves an operation that has not
 * yet been described is the second one, and it's shown below:
 *
 *    BEFORE                             AFTER
 *
 *    +----+----+----+----+              +----+----+----+----+
 *  p |  c | ?? |   ...   |            p |  c | ?? |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *    +----+----+----+----+              +----+----+----+----+
 *  c |  n |  p |   ...   |            c |  s |  p |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *                                       +----+----+----+----+ This is the
 *                                     s |  n |  c |   ..    | new block at
 *                                       +----+----+----+----+ c+blocks
 *    +----+----+----+----+              +----+----+----+----+
 *  n | ?? |  c |   ...   |            n | ?? |  s |   ...   |
 *    +----+----+----+----+              +----+----+----+----+
 *
 * Then we call free() with the adress of the data portion of the new
 * block (s) which adds it to the free list.
 *
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <pgmspace.h>

#define _UMM_MALLOC_CPP
#include "umm_malloc.h"

#include "umm_malloc_cfg.h"   /* user-dependent */

extern "C" {

// From UMM, the last caller of a malloc/realloc/calloc which failed:
extern void *umm_last_fail_alloc_addr;
extern int umm_last_fail_alloc_size;

#ifndef UMM_FIRST_FIT
#  ifndef UMM_BEST_FIT
#    define UMM_BEST_FIT
#  endif
#endif

#ifndef DBG_LOG_LEVEL
#  undef  DBG_LOG_LEVEL
#  define DBG_LOG_LEVEL 0
#else
#  undef  DBG_LOG_LEVEL
#  define DBG_LOG_LEVEL DBG_LOG_LEVEL
#endif

/*
Changes for July 2019:

  Correct critical section with interrupt level preserving and nest support
  alternative. Replace ets_intr_lock()/ets_intr_unlock() with uint32_t
  oldValue=xt_rsil(3)/xt_wrs(oldValue). Added UMM_CRITICAL_DECL macro to define
  storage for current state. Expanded UMM_CRITICAL_... to  use unique
  identifiers. This helpt facilitate gather function specific timing
  information.

  Replace printf with something that is ROM or IRAM based so that a printf
  that occurs during an ISR malloc/new does not cause a crash. To avoid any
  reentry issue it should also avoid doing malloc lib calls.

  Refactor realloc to avoid memcpy/memmove while in critical section. This is
  only effective when realloc is called with interrupts enabled. The copy
  process alone can take over 10us (when copying more than ~498 bytes with a
  80MHz CPU clock). It would be good practice for an ISR to avoid realloc.
  Note, while doing this might initially sound scary, this appears to be very
  stable. It ran on my troublesome sketch for over 3 weeks until I got back from
  vacation and flashed an update. Troublesome sketch - runs ESPAsyncTCP, with
  modified fauxmo emulation for 10 devices. It receives lost of Network traffic
  related to uPnP scans, which includes lots of TCP connects disconnects RSTs
  related to uPnP discovery.

  Locking is no longer nested in realloc, due to refactoring for reduced IRQ
  off time.

  I have clocked umm_info critical lock time taking as much as 180us. A common
  use for the umm_info call is to get the free heap result. It is common
  to try and closely monitor free heap as a method to detect memory leaks.
  This may result in frequent calls to umm_info. There has not been a clear
  test case that shows an issue yet; however, I and others think they are or
  have had crashes related to this.

  I have added code that updates a current free heap value from _umm_malloc,
  _umm_realloc, and _umm_free. Removing the need to do a long interrupts
  disabled calculation via umm_info.

  Build optional, min/max time measurements for locks held while in info,
  malloc, realloc, and free. Also, maintains a count of how many times each is
  called with INTLEVEL set.

 */

/* -- dbglog {{{ */

/* ----------------------------------------------------------------------------
 *            A set of macros that cleans up code that needs to produce debug
 *            or log information.
 *
 * See copyright notice in LICENSE.TXT
 * ----------------------------------------------------------------------------
 *
 * There are macros to handle the following decreasing levels of detail:
 *
 * 6 = TRACE
 * 5 = DEBUG
 * 4 = CRITICAL
 * 3 = ERROR
 * 2 = WARNING
 * 1 = INFO
 * 0 = FORCE - The printf is always compiled in and is called only when
 *              the first parameter to the macro is non-0
 *
 * ----------------------------------------------------------------------------
 *
 * The following #define should be set up before this file is included so
 * that we can be sure that the correct macros are defined.
 *
 * #define DBG_LOG_LEVEL x
 * ----------------------------------------------------------------------------
 */

#undef DBG_LOG_TRACE
#undef DBG_LOG_DEBUG
#undef DBG_LOG_CRITICAL
#undef DBG_LOG_ERROR
#undef DBG_LOG_WARNING
#undef DBG_LOG_INFO
#undef DBG_LOG_FORCE

/* ------------------------------------------------------------------------- */

#if DBG_LOG_LEVEL >= 6
#  define DBG_LOG_TRACE( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_TRACE( format, ... )
#endif

#if DBG_LOG_LEVEL >= 5
#  define DBG_LOG_DEBUG( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_DEBUG( format, ... )
#endif

#if DBG_LOG_LEVEL >= 4
#  define DBG_LOG_CRITICAL( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_CRITICAL( format, ... )
#endif

#if DBG_LOG_LEVEL >= 3
#  define DBG_LOG_ERROR( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_ERROR( format, ... )
#endif

#if DBG_LOG_LEVEL >= 2
#  define DBG_LOG_WARNING( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_WARNING( format, ... )
#endif

#if DBG_LOG_LEVEL >= 1
#  define DBG_LOG_INFO( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_INFO( format, ... )
#endif

#define DBG_LOG_FORCE( force, format, ... ) {if(force) {printf( format, ## __VA_ARGS__  );}}

/* }}} */

/* ------------------------------------------------------------------------- */

UMM_H_ATTPACKPRE typedef struct umm_ptr_t {
  unsigned short int next;
  unsigned short int prev;
} UMM_H_ATTPACKSUF umm_ptr;


UMM_H_ATTPACKPRE typedef struct umm_block_t {
  union {
    umm_ptr used;
  } header;
  union {
    umm_ptr free;
    unsigned char data[4];
  } body;
} UMM_H_ATTPACKSUF umm_block;

#define UMM_FREELIST_MASK (0x8000)
#define UMM_BLOCKNO_MASK  (0x7FFF)

/* ------------------------------------------------------------------------- */

#ifdef UMM_REDEFINE_MEM_FUNCTIONS
#  define umm_free    free
#  define umm_malloc  malloc
#  define umm_calloc  calloc
#  define umm_realloc realloc
#endif

umm_block *umm_heap = NULL;
unsigned short int umm_numblocks = 0;

#define UMM_NUMBLOCKS (umm_numblocks)

/* ------------------------------------------------------------------------ */

#define UMM_BLOCK(b)  (umm_heap[b])

#define UMM_NBLOCK(b) (UMM_BLOCK(b).header.used.next)
#define UMM_PBLOCK(b) (UMM_BLOCK(b).header.used.prev)
#define UMM_NFREE(b)  (UMM_BLOCK(b).body.free.next)
#define UMM_PFREE(b)  (UMM_BLOCK(b).body.free.prev)
#define UMM_DATA(b)   (UMM_BLOCK(b).body.data)

/*
 * This does not look safe, no access locks. It currently is not being
 * built, so not an immediate issue. -  06/10/19
 */
/* integrity check (UMM_INTEGRITY_CHECK) {{{ */
#if defined(UMM_INTEGRITY_CHECK)
/*
 * Perform integrity check of the whole heap data. Returns 1 in case of
 * success, 0 otherwise.
 *
 * First of all, iterate through all free blocks, and check that all backlinks
 * match (i.e. if block X has next free block Y, then the block Y should have
 * previous free block set to X).
 *
 * Additionally, we check that each free block is correctly marked with
 * `UMM_FREELIST_MASK` on the `next` pointer: during iteration through free
 * list, we mark each free block by the same flag `UMM_FREELIST_MASK`, but
 * on `prev` pointer. We'll check and unmark it later.
 *
 * Then, we iterate through all blocks in the heap, and similarly check that
 * all backlinks match (i.e. if block X has next block Y, then the block Y
 * should have previous block set to X).
 *
 * But before checking each backlink, we check that the `next` and `prev`
 * pointers are both marked with `UMM_FREELIST_MASK`, or both unmarked.
 * This way, we ensure that the free flag is in sync with the free pointers
 * chain.
 */
static int integrity_check(void) {
  int ok = 1;
  unsigned short int prev;
  unsigned short int cur;

  if (umm_heap == NULL) {
    umm_init();
  }

  /* Iterate through all free blocks */
  prev = 0;
  while(1) {
    cur = UMM_NFREE(prev);

    /* Check that next free block number is valid */
    if (cur >= UMM_NUMBLOCKS) {
      printf("heap integrity broken: too large next free num: %d "
          "(in block %d, addr 0x%lx)\n", cur, prev,
          (unsigned long)&UMM_NBLOCK(prev));
      ok = 0;
      goto clean;
    }
    if (cur == 0) {
      /* No more free blocks */
      break;
    }

    /* Check if prev free block number matches */
    if (UMM_PFREE(cur) != prev) {
      printf("heap integrity broken: free links don't match: "
          "%d -> %d, but %d -> %d\n",
          prev, cur, cur, UMM_PFREE(cur));
      ok = 0;
      goto clean;
    }

    UMM_PBLOCK(cur) |= UMM_FREELIST_MASK;

    prev = cur;
  }

  /* Iterate through all blocks */
  prev = 0;
  while(1) {
    cur = UMM_NBLOCK(prev) & UMM_BLOCKNO_MASK;

    /* Check that next block number is valid */
    if (cur >= UMM_NUMBLOCKS) {
      printf("heap integrity broken: too large next block num: %d "
          "(in block %d, addr 0x%lx)\n", cur, prev,
          (unsigned long)&UMM_NBLOCK(prev));
      ok = 0;
      goto clean;
    }
    if (cur == 0) {
      /* No more blocks */
      break;
    }

    /* make sure the free mark is appropriate, and unmark it */
    if ((UMM_NBLOCK(cur) & UMM_FREELIST_MASK)
        != (UMM_PBLOCK(cur) & UMM_FREELIST_MASK))
    {
      printf("heap integrity broken: mask wrong at addr 0x%lx: n=0x%x, p=0x%x\n",
          (unsigned long)&UMM_NBLOCK(cur),
          (UMM_NBLOCK(cur) & UMM_FREELIST_MASK),
          (UMM_PBLOCK(cur) & UMM_FREELIST_MASK)
          );
      ok = 0;
      goto clean;
    }

    /* unmark */
    UMM_PBLOCK(cur) &= UMM_BLOCKNO_MASK;

    /* Check if prev block number matches */
    if (UMM_PBLOCK(cur) != prev) {
      printf("heap integrity broken: block links don't match: "
          "%d -> %d, but %d -> %d\n",
          prev, cur, cur, UMM_PBLOCK(cur));
      ok = 0;
      goto clean;
    }

    prev = cur;
  }

clean:
  if (!ok){
    UMM_HEAP_CORRUPTION_CB();
  }
  return ok;
}

#define INTEGRITY_CHECK() integrity_check()
#else
/*
 * Integrity check is disabled, so just define stub macro
 */
#define INTEGRITY_CHECK() 1
#endif
/* }}} */

/* poisoning (UMM_POISON) {{{ */
#if defined(UMM_POISON)
#define POISON_BYTE (0xa5)

/*
 * Yields a size of the poison for the block of size `s`.
 * If `s` is 0, returns 0.
 */
#define POISON_SIZE(s) (                                \
    (s) ?                                             \
    (UMM_POISON_SIZE_BEFORE + UMM_POISON_SIZE_AFTER + \
     sizeof(UMM_POISONED_BLOCK_LEN_TYPE)              \
    ) : 0                                             \
    )

/*
 * Print memory contents starting from given `ptr`
 */
static void dump_mem ( const unsigned char *ptr, size_t len ) {
  while (len--) {
    printf(" 0x%.2x", (unsigned int)(*ptr++));
  }
}

/*
 * Put poison data at given `ptr` and `poison_size`
 */
static void put_poison( unsigned char *ptr, size_t poison_size ) {
  memset(ptr, POISON_BYTE, poison_size);
}

/*
 * Check poison data at given `ptr` and `poison_size`. `where` is a pointer to
 * a string, either "before" or "after", meaning, before or after the block.
 *
 * If poison is there, returns 1.
 * Otherwise, prints the appropriate message, and returns 0.
 */
static int check_poison( const unsigned char *ptr, size_t poison_size,
    const char *where) {
  size_t i;
  int ok = 1;

  for (i = 0; i < poison_size; i++) {
    if (ptr[i] != POISON_BYTE) {
      ok = 0;
      break;
    }
  }

  if (!ok) {
    printf("there is no poison %s the block. "
        "Expected poison address: 0x%lx, actual data:",
        where, (unsigned long)ptr);
    dump_mem(ptr, poison_size);
    printf("\n");
  }

  return ok;
}

/*
 * Check if a block is properly poisoned. Must be called only for non-free
 * blocks.
 */
static int check_poison_block( umm_block *pblock ) {
  int ok = 1;

  if (pblock->header.used.next & UMM_FREELIST_MASK) {
    printf("check_poison_block is called for free block 0x%lx\n",
        (unsigned long)pblock);
  } else {
    /* the block is used; let's check poison */
    unsigned char *pc = (unsigned char *)pblock->body.data;
    unsigned char *pc_cur;

    pc_cur = pc + sizeof(UMM_POISONED_BLOCK_LEN_TYPE);
    if (!check_poison(pc_cur, UMM_POISON_SIZE_BEFORE, "before")) {
      printf("block start: %08x\n", pc + sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE);
      UMM_HEAP_CORRUPTION_CB();
      ok = 0;
      goto clean;
    }

    pc_cur = pc + *((UMM_POISONED_BLOCK_LEN_TYPE *)pc) - UMM_POISON_SIZE_AFTER;
    if (!check_poison(pc_cur, UMM_POISON_SIZE_AFTER, "after")) {
	  printf("block start: %08x\n", pc + sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE);
      UMM_HEAP_CORRUPTION_CB();
      ok = 0;
      goto clean;
    }
  }

clean:
  return ok;
}

/*
 * Iterates through all blocks in the heap, and checks poison for all used
 * blocks.
 */
static int check_poison_all_blocks(void) {
  int ok = 1;
  unsigned short int blockNo = 0;

  if (umm_heap == NULL) {
    umm_init();
  }

  /* Now iterate through the blocks list */
  blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;

  while( UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK ) {
    if ( !(UMM_NBLOCK(blockNo) & UMM_FREELIST_MASK) ) {
      /* This is a used block (not free), so, check its poison */
      ok = check_poison_block(&UMM_BLOCK(blockNo));
      if (!ok){
        break;
      }
    }

    blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;
  }

  return ok;
}

/*
 * Takes a pointer returned by actual allocator function (`_umm_malloc` or
 * `_umm_realloc`), puts appropriate poison, and returns adjusted pointer that
 * should be returned to the user.
 *
 * `size_w_poison` is a size of the whole block, including a poison.
 */
static void *get_poisoned( void *vptr, size_t size_w_poison ) {
  unsigned char *ptr = (unsigned char *)vptr;
  if (size_w_poison != 0 && ptr != NULL) {

    /* Put exact length of the user's chunk of memory */
    memcpy(ptr, &size_w_poison, sizeof(UMM_POISONED_BLOCK_LEN_TYPE));

    /* Poison beginning and the end of the allocated chunk */
    put_poison(ptr + sizeof(UMM_POISONED_BLOCK_LEN_TYPE),
        UMM_POISON_SIZE_BEFORE);
    put_poison(ptr + size_w_poison - UMM_POISON_SIZE_AFTER,
        UMM_POISON_SIZE_AFTER);

    /* Return pointer at the first non-poisoned byte */
    return ptr + sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE;
  } else {
    return ptr;
  }
}

/*
 * Takes "poisoned" pointer (i.e. pointer returned from `get_poisoned()`),
 * and checks that the poison of this particular block is still there.
 *
 * Returns unpoisoned pointer, i.e. actual pointer to the allocated memory.
 */
static void *get_unpoisoned( void *vptr ) {
  unsigned char *ptr = (unsigned char *)vptr;
  if (ptr != NULL) {
    unsigned short int c;

    ptr -= (sizeof(UMM_POISONED_BLOCK_LEN_TYPE) + UMM_POISON_SIZE_BEFORE);

    /* Figure out which block we're in. Note the use of truncated division... */
    c = (((char *)ptr)-(char *)(&(umm_heap[0])))/sizeof(umm_block);

    check_poison_block(&UMM_BLOCK(c));
  }

  return ptr;
}

#define CHECK_POISON_ALL_BLOCKS() check_poison_all_blocks()
#define GET_POISONED(ptr, size)   get_poisoned(ptr, size)
#define GET_UNPOISONED(ptr)       get_unpoisoned(ptr)

#else
/*
 * Integrity check is disabled, so just define stub macros
 */
#define POISON_SIZE(s)            0
#define CHECK_POISON_ALL_BLOCKS() 1
#define GET_POISONED(ptr, size)   (ptr)
#define GET_UNPOISONED(ptr)       (ptr)
#endif
/* }}} */

/* ----------------------------------------------------------------------------
 * One of the coolest things about this little library is that it's VERY
 * easy to get debug information about the memory heap by simply iterating
 * through all of the memory blocks.
 *
 * As you go through all the blocks, you can check to see if it's a free
 * block by looking at the high order bit of the next block index. You can
 * also see how big the block is by subtracting the next block index from
 * the current block number.
 *
 * The umm_info function does all of that and makes the results available
 * in the ummHeapInfo structure.
 * ----------------------------------------------------------------------------
 */

UMM_HEAP_INFO ummHeapInfo;
unsigned short int ummHeapFreeBlocks;

void ICACHE_FLASH_ATTR *umm_info( void *ptr, int force ) {
  UMM_CRITICAL_DECL(id_info);

  unsigned short int blockNo = 0;

  if (umm_heap == NULL) {
      umm_init();
  }

  /* Protect the critical section... */
  UMM_CRITICAL_ENTRY(id_info);

  /*
   * Clear out all of the entries in the ummHeapInfo structure before doing
   * any calculations..
   */
  memset( &ummHeapInfo, 0, sizeof( ummHeapInfo ) );

  DBG_LOG_FORCE( force, "\n\nDumping the umm_heap...\n" );

  DBG_LOG_FORCE( force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5d|NF %5d|PF %5d|\n",
      (unsigned long)(&UMM_BLOCK(blockNo)),
      blockNo,
      UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
      UMM_PBLOCK(blockNo),
      (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo,
      UMM_NFREE(blockNo),
      UMM_PFREE(blockNo) );

  /*
   * Now loop through the block lists, and keep track of the number and size
   * of used and free blocks. The terminating condition is an nb pointer with
   * a value of zero...
   */

  blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;

  while( UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK ) {
    size_t curBlocks = (UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK )-blockNo;

    ++ummHeapInfo.totalEntries;
    ummHeapInfo.totalBlocks += curBlocks;

    /* Is this a free block? */

    if( UMM_NBLOCK(blockNo) & UMM_FREELIST_MASK ) {
      ++ummHeapInfo.freeEntries;
      ummHeapInfo.freeBlocks += curBlocks;
      ummHeapInfo.freeSize2 += (unsigned int)curBlocks
                             * (unsigned int)sizeof(umm_block)
                             * (unsigned int)curBlocks
                             * (unsigned int)sizeof(umm_block);

      if (ummHeapInfo.maxFreeContiguousBlocks < curBlocks) {
        ummHeapInfo.maxFreeContiguousBlocks = curBlocks;
      }

      DBG_LOG_FORCE( force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5u|NF %5d|PF %5d|\n",
          (unsigned long)(&UMM_BLOCK(blockNo)),
          blockNo,
          UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
          UMM_PBLOCK(blockNo),
          (unsigned int)curBlocks,
          UMM_NFREE(blockNo),
          UMM_PFREE(blockNo) );

      /* Does this block address match the ptr we may be trying to free? */

      if( ptr == &UMM_BLOCK(blockNo) ) {

        /* Release the critical section... */
        UMM_CRITICAL_EXIT(id_info);

        return( ptr );
      }
    } else {
      ++ummHeapInfo.usedEntries;
      ummHeapInfo.usedBlocks += curBlocks;

      DBG_LOG_FORCE( force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5u|\n",
          (unsigned long)(&UMM_BLOCK(blockNo)),
          blockNo,
          UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
          UMM_PBLOCK(blockNo),
          (unsigned int)curBlocks );
    }

    blockNo = UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK;
  }

  /*
   * Update the accounting totals with information from the last block, the
   * rest must be free!
   */

  {
    size_t curBlocks = UMM_NUMBLOCKS-blockNo;
    ummHeapInfo.freeBlocks  += curBlocks;
    ummHeapInfo.totalBlocks += curBlocks;

    if (ummHeapInfo.maxFreeContiguousBlocks < curBlocks) {
      ummHeapInfo.maxFreeContiguousBlocks = curBlocks;
    }
  }

  DBG_LOG_FORCE( force, "|0x%08lx|B %5d|NB %5d|PB %5d|Z %5d|NF %5d|PF %5d|\n",
      (unsigned long)(&UMM_BLOCK(blockNo)),
      blockNo,
      UMM_NBLOCK(blockNo) & UMM_BLOCKNO_MASK,
      UMM_PBLOCK(blockNo),
      UMM_NUMBLOCKS-blockNo,
      UMM_NFREE(blockNo),
      UMM_PFREE(blockNo) );

  DBG_LOG_FORCE( force, "Total Entries %5d    Used Entries %5d    Free Entries %5d\n",
      ummHeapInfo.totalEntries,
      ummHeapInfo.usedEntries,
      ummHeapInfo.freeEntries );

  DBG_LOG_FORCE( force, "Total Blocks  %5d    Used Blocks  %5d    Free Blocks  %5d\n",
      ummHeapInfo.totalBlocks,
      ummHeapInfo.usedBlocks,
      ummHeapInfo.freeBlocks  );

  /* Release the critical section... */
  UMM_CRITICAL_EXIT(id_info);

  return( NULL );
}

/* ------------------------------------------------------------------------ */

static unsigned short int umm_blocks( size_t size ) {

  /*
   * The calculation of the block size is not too difficult, but there are
   * a few little things that we need to be mindful of.
   *
   * When a block removed from the free list, the space used by the free
   * pointers is available for data. That's what the first calculation
   * of size is doing.
   */

  if( size <= (sizeof(((umm_block *)0)->body)) )
    return( 1 );

  /*
   * If it's for more than that, then we need to figure out the number of
   * additional whole blocks the size of an umm_block are required.
   */

  size -= ( 1 + (sizeof(((umm_block *)0)->body)) );

  return( 2 + size/(sizeof(umm_block)) );
}

/* ------------------------------------------------------------------------ */

/*
 * Split the block `c` into two blocks: `c` and `c + blocks`.
 *
 * - `cur_freemask` should be `0` if `c` used, or `UMM_FREELIST_MASK`
 *   otherwise.
 * - `new_freemask` should be `0` if `c + blocks` used, or `UMM_FREELIST_MASK`
 *   otherwise.
 *
 * Note that free pointers are NOT modified by this function.
 */
static void umm_make_new_block( unsigned short int c,
    unsigned short int blocks,
    unsigned short int cur_freemask, unsigned short int new_freemask ) {

  UMM_NBLOCK(c+blocks) = (UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) | new_freemask;
  UMM_PBLOCK(c+blocks) = c;

  UMM_PBLOCK(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) = (c+blocks);
  UMM_NBLOCK(c)                                = (c+blocks) | cur_freemask;
}

/* ------------------------------------------------------------------------ */

static void umm_disconnect_from_free_list( unsigned short int c ) {
  /* Disconnect this block from the FREE list */

  UMM_NFREE(UMM_PFREE(c)) = UMM_NFREE(c);
  UMM_PFREE(UMM_NFREE(c)) = UMM_PFREE(c);

  /* And clear the free block indicator */

  UMM_NBLOCK(c) &= (~UMM_FREELIST_MASK);
}

/* ------------------------------------------------------------------------ */

static void umm_assimilate_up( unsigned short int c ) {

  if( UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_FREELIST_MASK ) {
    /*
     * The next block is a free block, so assimilate up and remove it from
     * the free list
     */

    DBG_LOG_DEBUG( "Assimilate up to next block, which is FREE\n" );

    /* Disconnect the next block from the FREE list */

    umm_disconnect_from_free_list( UMM_NBLOCK(c) );

    /* Assimilate the next block with this one */

    UMM_PBLOCK(UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_BLOCKNO_MASK) = c;
    UMM_NBLOCK(c) = UMM_NBLOCK(UMM_NBLOCK(c)) & UMM_BLOCKNO_MASK;
  }
}

/* ------------------------------------------------------------------------ */

static unsigned short int umm_assimilate_down( unsigned short int c, unsigned short int freemask ) {

  UMM_NBLOCK(UMM_PBLOCK(c)) = UMM_NBLOCK(c) | freemask;
  UMM_PBLOCK(UMM_NBLOCK(c)) = UMM_PBLOCK(c);

  return( UMM_PBLOCK(c) );
}

/* ------------------------------------------------------------------------- */
/* This function called only one time during OS startup after flash is */
/* enabled.  No need to keep it in IRAM. */
void ICACHE_FLASH_ATTR umm_init( void ) {
  /* init heap pointer and size, and memset it to 0 */
  umm_heap = (umm_block *)UMM_MALLOC_CFG__HEAP_ADDR;
  umm_numblocks = (UMM_MALLOC_CFG__HEAP_SIZE / sizeof(umm_block));
  memset(umm_heap, 0x00, UMM_MALLOC_CFG__HEAP_SIZE);

  /* setup initial blank heap structure */
  {
    /* index of the 0th `umm_block` */
    const unsigned short int block_0th = 0;
    /* index of the 1st `umm_block` */
    const unsigned short int block_1th = 1;
    /* index of the latest `umm_block` */
    const unsigned short int block_last = UMM_NUMBLOCKS - 1;

    /* init heapFreeBlocks */
    ummHeapFreeBlocks = block_last;

    /* setup the 0th `umm_block`, which just points to the 1st */
    UMM_NBLOCK(block_0th) = block_1th;
    UMM_NFREE(block_0th)  = block_1th;

    /*
     * Now, we need to set the whole heap space as a huge free block. We should
     * not touch the 0th `umm_block`, since it's special: the 0th `umm_block`
     * is the head of the free block list. It's a part of the heap invariant.
     *
     * See the detailed explanation at the beginning of the file.
     */

    /*
     * 1th `umm_block` has pointers:
     *
     * - next `umm_block`: the latest one
     * - prev `umm_block`: the 0th
     *
     * Plus, it's a free `umm_block`, so we need to apply `UMM_FREELIST_MASK`
     *
     * And it's the last free block, so the next free block is 0.
     */
    UMM_NBLOCK(block_1th) = block_last | UMM_FREELIST_MASK;
    UMM_NFREE(block_1th)  = 0;
    UMM_PBLOCK(block_1th) = block_0th;
    UMM_PFREE(block_1th)  = block_0th;

    /*
     * latest `umm_block` has pointers:
     *
     * - next `umm_block`: 0 (meaning, there are no more `umm_blocks`)
     * - prev `umm_block`: the 1st
     *
     * It's not a free block, so we don't touch NFREE / PFREE at all.
     */
    UMM_NBLOCK(block_last) = 0;
    UMM_PBLOCK(block_last) = block_1th;
  }
}

/* ------------------------------------------------------------------------ */

static void _umm_free( void *ptr ) {
  UMM_CRITICAL_DECL(id_free);

  unsigned short int c;

  /* If we're being asked to free a NULL pointer, well that's just silly! */

  if( (void *)0 == ptr ) {
    DBG_LOG_DEBUG( "free a null pointer -> do nothing\n" );

    return;
  }

  /*
   * FIXME: At some point it might be a good idea to add a check to make sure
   *        that the pointer we're being asked to free up is actually within
   *        the umm_heap!
   *
   * NOTE:  See the new umm_info() function that you can use to see if a ptr is
   *        on the free list!
   */

  /* Figure out which block we're in. Note the use of truncated division... */

  c = (((char *)ptr)-(char *)(&(umm_heap[0])))/sizeof(umm_block);

  DBG_LOG_DEBUG( "Freeing block %6d\n", c );

  /* Protect the critical section... */
  UMM_CRITICAL_ENTRY(id_free);

  /* Update dynamic Free Block count */
  ummHeapFreeBlocks += (UMM_NBLOCK(c) - c);

  /* Now let's assimilate this block with the next one if possible. */

  umm_assimilate_up( c );

  /* Then assimilate with the previous block if possible */

  if( UMM_NBLOCK(UMM_PBLOCK(c)) & UMM_FREELIST_MASK ) {

    DBG_LOG_DEBUG( "Assimilate down to next block, which is FREE\n" );

    c = umm_assimilate_down(c, UMM_FREELIST_MASK);
  } else {
    /*
     * The previous block is not a free block, so add this one to the head
     * of the free list
     */

    DBG_LOG_DEBUG( "Just add to head of free list\n" );

    UMM_PFREE(UMM_NFREE(0)) = c;
    UMM_NFREE(c)            = UMM_NFREE(0);
    UMM_PFREE(c)            = 0;
    UMM_NFREE(0)            = c;

    UMM_NBLOCK(c)          |= UMM_FREELIST_MASK;
  }

#if 0
  /*
   * The following is experimental code that checks to see if the block we just
   * freed can be assimilated with the very last block - it's pretty convoluted in
   * terms of block index manipulation, and has absolutely no effect on heap
   * fragmentation. I'm not sure that it's worth including but I've left it
   * here for posterity.
   */

  if( 0 == UMM_NBLOCK(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK ) ) {

    if( UMM_PBLOCK(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) != UMM_PFREE(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK) ) {
      UMM_NFREE(UMM_PFREE(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK)) = c;
      UMM_NFREE(UMM_PFREE(c))                                = UMM_NFREE(c);
      UMM_PFREE(UMM_NFREE(c))                                = UMM_PFREE(c);
      UMM_PFREE(c)                                           = UMM_PFREE(UMM_NBLOCK(c) & UMM_BLOCKNO_MASK);
    }

    UMM_NFREE(c)  = 0;
    UMM_NBLOCK(c) = 0;
  }
#endif

  /* Release the critical section... */
  UMM_CRITICAL_EXIT(id_free);
}

/* ------------------------------------------------------------------------ */

static void *_umm_malloc( size_t size ) {
  UMM_CRITICAL_DECL(id_malloc);

  unsigned short int blocks;
  unsigned short int blockSize = 0;

  unsigned short int bestSize;
  unsigned short int bestBlock;

  unsigned short int cf;

  if (umm_heap == NULL) {
    umm_init();
  }

  /*
   * the very first thing we do is figure out if we're being asked to allocate
   * a size of 0 - and if we are we'll simply return a null pointer. if not
   * then reduce the size by 1 byte so that the subsequent calculations on
   * the number of blocks to allocate are easier...
   */

  if( 0 == size ) {
    DBG_LOG_DEBUG( "malloc a block of 0 bytes -> do nothing\n" );

    return( (void *)NULL );
  }

  blocks = umm_blocks( size );

  /*
   * Now we can scan through the free list until we find a space that's big
   * enough to hold the number of blocks we need.
   *
   * This part may be customized to be a best-fit, worst-fit, or first-fit
   * algorithm
   */

  /* Protect the critical section... */
  UMM_CRITICAL_ENTRY(id_malloc);

  cf = UMM_NFREE(0);

  bestBlock = UMM_NFREE(0);
  bestSize  = 0x7FFF;

  while( cf ) {
    blockSize = (UMM_NBLOCK(cf) & UMM_BLOCKNO_MASK) - cf;

    DBG_LOG_TRACE( "Looking at block %6d size %6d\n", cf, blockSize );

#if defined UMM_FIRST_FIT
    /* This is the first block that fits! */
    if( (blockSize >= blocks) )
      break;
#elif defined UMM_BEST_FIT
    if( (blockSize >= blocks) && (blockSize < bestSize) ) {
      bestBlock = cf;
      bestSize  = blockSize;
    }
#endif

    cf = UMM_NFREE(cf);
  }

  if( 0x7FFF != bestSize ) {
    cf        = bestBlock;
    blockSize = bestSize;
  }

  if( UMM_NBLOCK(cf) & UMM_BLOCKNO_MASK && blockSize >= blocks ) {
    /*
     * This is an existing block in the memory heap, we just need to split off
     * what we need, unlink it from the free list and mark it as in use, and
     * link the rest of the block back into the freelist as if it was a new
     * block on the free list...
     */

    if( blockSize == blocks ) {
      /* It's an exact fit and we don't neet to split off a block. */
      DBG_LOG_DEBUG( "Allocating %6d blocks starting at %6d - exact\n", blocks, cf );

      /* Disconnect this block from the FREE list */

      umm_disconnect_from_free_list( cf );

    } else {
      /* It's not an exact fit and we need to split off a block. */
      DBG_LOG_DEBUG( "Allocating %6d blocks starting at %6d - existing\n", blocks, cf );

      /*
       * split current free block `cf` into two blocks. The first one will be
       * returned to user, so it's not free, and the second one will be free.
       */
      umm_make_new_block( cf, blocks,
          0/*`cf` is not free*/,
          UMM_FREELIST_MASK/*new block is free*/);

      /*
       * `umm_make_new_block()` does not update the free pointers (it affects
       * only free flags), but effectively we've just moved beginning of the
       * free block from `cf` to `cf + blocks`. So we have to adjust pointers
       * to and from adjacent free blocks.
       */

      /* previous free block */
      UMM_NFREE( UMM_PFREE(cf) ) = cf + blocks;
      UMM_PFREE( cf + blocks ) = UMM_PFREE(cf);

      /* next free block */
      UMM_PFREE( UMM_NFREE(cf) ) = cf + blocks;
      UMM_NFREE( cf + blocks ) = UMM_NFREE(cf);
    }

    /* Update dynamic Free Block count */
    ummHeapFreeBlocks -= blocks;

  } else {
    /* Release the critical section... */
    UMM_CRITICAL_EXIT(id_malloc);

    /* Out of memory */

    DBG_LOG_DEBUG(  "Can't allocate %5d blocks\n", blocks );

    return( (void *)NULL );
  }

  /* Release the critical section... */
  UMM_CRITICAL_EXIT(id_malloc);

  return( (void *)&UMM_DATA(cf) );
}

/* ------------------------------------------------------------------------ */

static void *_umm_realloc( void *ptr, size_t size ) {
  UMM_CRITICAL_DECL(id_realloc);

  unsigned short int blocks;
  unsigned short int blockSize;

  unsigned short int c;

  size_t curSize;

  if (umm_heap == NULL) {
    umm_init();
  }

  /*
   * This code looks after the case of a NULL value for ptr. The ANSI C
   * standard says that if ptr is NULL and size is non-zero, then we've
   * got to work the same a malloc(). If size is also 0, then our version
   * of malloc() returns a NULL pointer, which is OK as far as the ANSI C
   * standard is concerned.
   */

  if( ((void *)NULL == ptr) ) {
    DBG_LOG_DEBUG( "realloc the NULL pointer - call malloc()\n" );

    return( _umm_malloc(size) );
  }

  /*
   * Now we're sure that we have a non_NULL ptr, but we're not sure what
   * we should do with it. If the size is 0, then the ANSI C standard says that
   * we should operate the same as free.
   */

  if( 0 == size ) {
    DBG_LOG_DEBUG( "realloc to 0 size, just free the block\n" );

    _umm_free( ptr );

    return( (void *)NULL );
  }

  /*
   * Defer starting critical section.
   *
   * Initially we should be safe without a critical section as long as we are
   * referencing values that are within our allocation as constants.
   * And only reference values that will not change, while the redefintions of
   * the allocations around us change.
   *
   * Example UMM_PBLOCK() could be change by a call to malloc from an ISR.
   * On the other hand UMM_NBLOCK() is safe returns an address of the next
   * block. The calculation is all based on information within our allocation
   * that remains constant, until we change it.
   *
   * As long as we don't try to modify the next block or walk the chain of
   * blocks we are okay.
   *
   * When called by an "interrupts enabled" type caller, it bears the
   * responsibility to not call again, with the allocate we are currently
   * working on. I think this is a normal expectation. I could be wrong.
   * Such a situation would involve a function that is called from foreground
   * and ISR context. Such code would already have to be re-entrant. This
   * change may expand the corner cases for such a function.
   *
   */

  /*
   * Otherwise we need to actually do a reallocation. A naiive approach
   * would be to malloc() a new block of the correct size, copy the old data
   * to the new block, and then free the old block.
   *
   * While this will work, we end up doing a lot of possibly unnecessary
   * copying. So first, let's figure out how many blocks we'll need.
   */

  blocks = umm_blocks( size );

  /* Figure out which block we're in. Note the use of truncated division... */

  c = (((char *)ptr)-(char *)(&(umm_heap[0])))/sizeof(umm_block);

  /* Figure out how big this block is... */

  blockSize = (UMM_NBLOCK(c) - c);

  /* Figure out how many bytes are in this block */

  curSize   = (blockSize*sizeof(umm_block))-(sizeof(((umm_block *)0)->header));

  /*
   * Ok, now that we're here, we know the block number of the original chunk
   * of memory, and we know how much new memory we want, and we know the original
   * block size...
   */

  if( blockSize == blocks ) {
    /* This space intentionally left blank - return the original pointer! */

    DBG_LOG_DEBUG( "realloc the same size block - %d, do nothing\n", blocks );

    return( ptr );
  }

  /* Now we need a critical section... */
  UMM_CRITICAL_ENTRY(id_realloc);

  /*
   * Now we have a block size that could be bigger or smaller. Either
   * way, try to assimilate up to the next block before doing anything...
   *
   * If it's still too small, we have to free it anyways and it will save the
   * assimilation step later in free :-)
   */

  umm_assimilate_up( c );

  /*
   * Now check if it might help to assimilate down, but don't actually
   * do the downward assimilation unless the resulting block will hold the
   * new request! If this block of code runs, then the new block will
   * either fit the request exactly, or be larger than the request.
   */

  if( (UMM_NBLOCK(UMM_PBLOCK(c)) & UMM_FREELIST_MASK) &&
      (blocks <= (UMM_NBLOCK(c)-UMM_PBLOCK(c)))    ) {

    /* Check if the resulting block would be big enough... */

    DBG_LOG_DEBUG( "realloc() could assimilate down %d blocks - fits!\n\r", c-UMM_PBLOCK(c) );

    /* Disconnect the previous block from the FREE list */

    umm_disconnect_from_free_list( UMM_PBLOCK(c) );

    /*
     * Connect the previous block to the next block ... and then
     * realign the current block pointer
     */

    c = umm_assimilate_down(c, 0);

    /*
     * Move the bytes down to the new block we just created, but be sure to move
     * only the original bytes.
     */

    memmove( (void *)&UMM_DATA(c), ptr, curSize );

    /* And don't forget to adjust the pointer to the new block location! */

    ptr    = (void *)&UMM_DATA(c);
  }

  unsigned short int previousBlockSize = blockSize;

  /* Now calculate the block size again...and we'll have three cases */

  blockSize = (UMM_NBLOCK(c) - c);

  /* Update dynamic Free Block count */
  ummHeapFreeBlocks -= (blockSize - previousBlockSize);

  if( blockSize == blocks ) {
    /* This space intentionally left blank - return the original pointer! */

    DBG_LOG_DEBUG( "realloc the same size block - %d, do nothing\n", blocks );

  } else if (blockSize > blocks ) {
    /*
     * New block is smaller than the old block, so just make a new block
     * at the end of this one and put it up on the free list...
     */

    DBG_LOG_DEBUG( "realloc %d to a smaller block %d, shrink and free the leftover bits\n", blockSize, blocks );

    umm_make_new_block( c, blocks, 0, 0 );
    _umm_free( (void *)&UMM_DATA(c+blocks) );
  } else {
    /* New block is bigger than the old block... */

    void *oldptr = ptr;

    DBG_LOG_DEBUG( "realloc %d to a bigger block %d, make new, copy, and free the old\n", blockSize, blocks );

    /*
     * Now _umm_malloc() a new/ one, copy the old data to the new block, and
     * free up the old block, but only if the malloc was sucessful!
     */

    UMM_CRITICAL_EXIT(id_realloc);

    if( (ptr = _umm_malloc( size )) ) {
      memcpy( ptr, oldptr, curSize );
      _umm_free( oldptr );
    }
    return( ptr );

  }

  /* Release the critical section... */
  UMM_CRITICAL_EXIT(id_realloc);

  return( ptr );
}

/* ------------------------------------------------------------------------ */

void *umm_malloc( size_t size ) {
  void *ret;

  /* check poison of each blocks, if poisoning is enabled */
  if (!CHECK_POISON_ALL_BLOCKS()) {
    return NULL;
  }

  /* check full integrity of the heap, if this check is enabled */
  if (!INTEGRITY_CHECK()) {
    return NULL;
  }

  size += POISON_SIZE(size);

  ret = _umm_malloc( size );
  if (0 != size && 0 == ret) {
    umm_last_fail_alloc_addr = __builtin_return_address(0);
    umm_last_fail_alloc_size = size;
  }

  ret = GET_POISONED(ret, size);

  return ret;
}

/* ------------------------------------------------------------------------ */

void *umm_calloc( size_t num, size_t item_size ) {
  void *ret;
  size_t size = item_size * num;

  /* check poison of each blocks, if poisoning is enabled */
  if (!CHECK_POISON_ALL_BLOCKS()) {
    return NULL;
  }

  /* check full integrity of the heap, if this check is enabled */
  if (!INTEGRITY_CHECK()) {
    return NULL;
  }

  size += POISON_SIZE(size);
  ret = _umm_malloc(size);
  if (ret) {
    memset(ret, 0x00, size);
  }
  if (0 != size && 0 == ret) {
    umm_last_fail_alloc_addr = __builtin_return_address(0);
    umm_last_fail_alloc_size = size;
  }

  ret = GET_POISONED(ret, size);

  return ret;
}

/* ------------------------------------------------------------------------ */

void *umm_realloc( void *ptr, size_t size ) {
  void *ret;

  ptr = GET_UNPOISONED(ptr);

  /* check poison of each blocks, if poisoning is enabled */
  if (!CHECK_POISON_ALL_BLOCKS()) {
    return NULL;
  }

  /* check full integrity of the heap, if this check is enabled */
  if (!INTEGRITY_CHECK()) {
    return NULL;
  }

  size += POISON_SIZE(size);
  ret = _umm_realloc( ptr, size );
  if (0 != size && 0 == ret) {
    umm_last_fail_alloc_addr = __builtin_return_address(0);
    umm_last_fail_alloc_size = size;
  }

  ret = GET_POISONED(ret, size);

  return ret;
}

/* ------------------------------------------------------------------------ */

void umm_free( void *ptr ) {

  ptr = GET_UNPOISONED(ptr);

  /* check poison of each blocks, if poisoning is enabled */
  if (!CHECK_POISON_ALL_BLOCKS()) {
    return;
  }

  /* check full integrity of the heap, if this check is enabled */
  if (!INTEGRITY_CHECK()) {
    return;
  }

  _umm_free( ptr );
}

/* ------------------------------------------------------------------------ */

size_t ICACHE_FLASH_ATTR umm_free_heap_size( void ) {
  return (size_t)ummHeapFreeBlocks * sizeof(umm_block);
}

size_t ICACHE_FLASH_ATTR umm_max_block_size( void ) {
  umm_info(NULL, 0);
  return ummHeapInfo.maxFreeContiguousBlocks * sizeof(umm_block);
}

size_t ICACHE_FLASH_ATTR umm_block_size( void ) {
  return sizeof(umm_block);
}

};

/* ------------------------------------------------------------------------ */
