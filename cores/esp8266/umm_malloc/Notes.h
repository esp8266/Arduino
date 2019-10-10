#if 0
/*
PR text description:

upstream version of `umm_malloc` customized for Arduino ESP8266 Core

This updates the heap management library, umm_malloc, to the current upstream
version at https://github.com/rhempel/umm_malloc. Some reorganizing and new code
was needed to use the new version.

This is a list of noteworthy changes:

UMM_POISON - now has a lite option as well as the previous intensive check
option. The code for running the full poison test at the call of the various
alloc functions was removed in the upstream version. In this port, the missing
code was added to heap.cpp and umm_local.cpp.
* UMM_POISON - appears to have been partially changed to UMM_POISON_CHECK,
  I treat it as deprecated and used UMM_POISON_CHECK when needed.
  However, the Arduino Core's references to UMM_POISON were replaced with
  UMM_POISON_CHECK_LITE.
* UMM_POISON_CHECK_LITE - Less intense, it just checks poison on active
  neighboring allocations.
* UMM_POISON_CHECK - Full heap intensive check of poison

A cautionary note, on the use of UMM_INTEGRITY_CHECK, UMM_POISON_CHECK, and
UMM_INFO_PRINT. All of these run with IRQs disabled, for periods that can go
into 100's of us. With umm_info(NULL, true) that may go into seconds, depending
on the serial interface speed and the number of memory allocations present. Use
UMM_INTEGRITY_CHECK, UMM_POISON_CHECK, and UMM_INFO_PRINT sparingly.
If you want to see numbers for the disabled time, explore using
UMM_CRITICAL_METRICS in umm_malloc_cfg.h.


===============================================================================

 New upstream umm_malloc feature delta's from the old umm_malloc we were using:

   umm_posion check for a given *alloc - failure - no longer panics.

   option to run full poison check at each *alloc call, not present

   option to run full interity check at each *alloc call, not present

   upstream code does not call panic from poison_check_block.

   Defragmenting effect of realloc is gone. It now minimizes copy. This
   may have been an accident during code cleanup.

 In one form or another these features have been restored in the
 reintegration of the upstream umm_malloc into the Arduino ESP8266 Core.

===============================================================================

 A list of changes made for local adaptation of newer upstream umm_malloc.
 
 In umm_malloc.c
     Renamed to umm_malloc.cpp
     Added `extern "C" { ... };` around code.
     Surround DBGLOG_LEVEL with #ifndef... Define value of DBGLOG_LEVEL from
     umm_malloc_cfg.h
     umm_realloc() - Added UMM_CRITICAL_SUSPEND()/UMM_CRITICAL_RESUME() for when
     lightweight locks are available. eg. sti/cli. Single threaded single CPU
     case.
     umm_realloc() - appears to have been refactored to minimize memmove and
     memcpy. The old version would always combine an adjacent block in the
     direction of the start of the heap when available and do a memmove. This
     had a defragging effect. This appears to have been replaced with an attempt
     to minimize copy when possible.
     Added heap stats tracking.
     
 In umm_info.c
    umm_info() - Added UMM_CRITICAL_DECL(id_info), updated critical sections
    with tag.
    Carried forward: Added NULL ptr check at beginning (umm_malloc.c).
 
 In umm_poison.c:
    Resolved C++ compiler error reported on get_poisoned(), and get_unpoisoned().
    They now take in void * arg instead of unsigned char *.
    Added  #if ... || defined(UMM_POISON_CHECK_LITE) to the conditional.
  
 In umm_integrity.c:
    Replaced printf with DBGLOG_FUNCTION. This needs to be a malloc free
    function and ISR safe.
    Added critical sections.
  
 In umm_malloc_cfg.h:
    Added macro UMM_CRITICAL_SUSPEND()/UMM_CRITICAL_RESUME()
  
 Globally change across all files %i to %d: umm_info.c, umm_malloc.c,
 Added a #ifdef BUILD_UMM_MALLOC_C fence to prevent Arduino IDE from building
 the various .c files that are #included into umm_malloc.cpp. They are
 normally enabled by #define <feature name>  in umm_malloc_cfg.h. In this
 case it builds fine; however, if the define is global, the IDE will try and
 build the .c by itself.

 Notes,
   umm_integrity_check() is called by macro INTEGRITY_CHECK which returns 1
   on success. No corruption. Does a time consuming scan of the whole heap.
   It will call UMM_HEAP_CORRUPTION_CB if an error is found.
   
   umm_poison_check(), formerly known as check_poison_all_blocks(),
   is called by macro POISON_CHECK which returns 1 on success for no
   corruption. Does a time consuming scan of all active allocations for
   modified poison. The new upstream version does *NOT* call
   UMM_HEAP_CORRUPTION_CB if an error is found. The option description says
   it does!
   
   umm_poison_realloc() and umm_poison_free() no longer call the macro
   UMM_HEAP_CORRUPTION_CB on poison error. Just a printf message is
   generated. I have added alternative functions umm_poison_free_fl,
   umm_poison_realloc_fl, and get_unpoisoned_check_neighbors in
   umm_local.cpp. These expand the poison check on the current allocation to
   include its nearest allocated neighbors in the heap.
   
   umm_malloc() has been extended to call check_poison_neighbors for the
   allocation it selects, conditionally for UMM_POISON_CHECK_LITE.
   
   For upstream umm_malloc "#  define POISON_CHECK() 0" should have been 1
   add to list to report.


==============================================================================

Notes from searching for the best print option

Printing from the malloc routines is tricky. Since a print library
might call *alloc. Then recusion may follow as each error call may fail
into another error and so on.

Objective:  To be able to print "last gasp" diagnostic messages
when interrupts are disabled and w/o availability of heap resources.

It turns out things are more complicated than that. There are three cases for
printing:

  1. Printing detailed heap info through `umm_info(NULL, 1);`. This function
  resides in flash and can only be called from non-ISR context. It can use
  PROGMEM strings. Because SPI bus will not be busy when called from foreground.
  Because printing is done from IRQ_LEVEL 15, printf function must be in iRAM or
  ROM.
  > I don't think I full understand how this can work. If `umm_info` is called
  > and not full cached and we raise IRQ_LEVEL 15, how is a cache-miss handled?
  > Interrupts? Some hidden internal interrupt? If we can handle cache-miss then 
  > other flash based functions can be used for printing here.
  
  2. Printing diagnostic messages possibly from from ISR context.
  
  3. Printing diagnostic messages from `heap.cpp` these printf's need to check
  `system_get_os_print()` to confirm debug-output is enabled just as
  `os_printf()` did.

Considerations:
* can be called from ISR
* can be called from malloc code, cannot use malloc
* can be called from malloc code that was called from an ISR
* can be called from with in a critical section, eg. xt_rsil(15);
  * this may be effectively the same as being called from an ISR?

Knowns:
* ets_printf - For RTOS SDK they replaced this function with one in the SDK.
  Most of the problems I can see with ets_printf center around not being
  able to maintain a port to thread context. That is you cannot have one
  thread using one port while another thread uses the other. In the no OS
  case we cannot have one area of code using one port and another area of
  code using the other port. Most of the ROM printf functions are not built
  to support this kind of usage. Things get especially dangerous when you
  try to use the ets_external_printf stuff.
* ets_vprintf - by itself is safe.
* newlibc printf - not safe - lives in flash.
* newlibc snprintf - not safe - lives in flash.
* builtin putc1 print function - Is installed when you use
  ets_install_uart_printf. Which calls ets_install_putc1. The selection of UART
  is performed by calling uart_buff_switch with 0 for UART0 and 1 for UART1.
  This should work for our purpose here, if handled as follows:
  * call uart_buff_switch at each printf call to reselect UART
  * use a stack buffer to hold a copy the PROGMEM string to print from.
  * use ets_vprintf for printing with putc1 function.
* os_printf_plus looks interesting. It is in IRAM. If no heap is available it
  will use up to 64 bytes of stack space to copy a PROGMEM fmt for printing.
  Issues:
  * Printing is turned off by system_set_os_print
  * putc1 needs to be in IRAM - this is a uart.cpp issue
  * Need to force system_get_free_heap_size to return 0 during critical periods.
  * won't work for umm_info it prints over 64 characters.
  * along with umm_info there are other debug messages that exceed 64 characters.
* ets_uart_printf - Appears safe. Just no PROGMEM support. Uses
  uart_buff_switch to select UART.

*/
#endif
