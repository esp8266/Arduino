MMU - Adjust the Ratio of ICACHE to IRAM
========================================

Overview
--------

The ESP8266 has a total of 64K of instruction memory, IRAM. This 64K of
IRAM is composed of one dedicated 32K block of IRAM and two 16K blocks
of IRAM. The last two 16K blocks of IRAM are flexible in the sense that
it can be used as a transparent cache for external flash memory. These
blocks can either be used for IRAM or an instruction cache for executing
code out of flash, ICACHE.

The instructions generated for a sketch are divided up into two groups
ICACHE and IRAM. IRAM offers faster access/execution. It is commonly
used for interrupt service routines, exception handling, and
time-critical code. ICACHE allows for the execution of up to 1MB of code
in flash. On a cache miss, a delay occurs as the instructions are read
from flash via the SPI bus.

There is 96KB of DRAM space. This memory can be accessed as byte, short,
or a 32-bit word. Access must be aligned according to the data element
size. A 16bit short must be on a multiple of 2-byte address boundary,
likewise, a 32-bit word must be on a multiple of 4-byte address
boundary. In contrast, data access to IRAM or ICACHE must always be a
full 32-bit word and aligned. We will introduce a non32-bit exception
handler for this later.

Option Summary
--------------

**The Arduino IDE Tools menu option, ``MMU`` allows for the following
selections:**

1. ``32KB cache + 32KB IRAM (balanced)``

   -  This is the legacy ratio.
   -  Try this option 1st.

2. ``16KB cache + 48KB IRAM (IRAM)``

   -  With just 16KB cache, execution of code out of flash may be slowed
      by more cache misses when compared to 32KB. The slowness will vary
      with the sketch.
   -  Use this if you need a little more IRAM space and you are good on
      DRAM space.

3. ``16KB cache + 48KB IRAM and 2nd Heap (shared)``

   -  This option builds on the previous option and builds a 2nd Heap
      made with IRAM.
   -  The 2nd Heap size will vary with free IRAM.
   -  This option is flexible. IRAM usage for code can overflow into the
      additional 16KB IRAM region shrinking the 2nd Heap below 16KB. Or
      IRAM can be under 32KB allowing the 2nd Heap to be larger than
      16KB.
   -  Installs a Non-32-Bit Access handler for IRAM. This allows for
      byte and 16-bit aligned short access.
   -  This 2nd Heap is supported by the standard ``malloc`` APIs.
   -  Heap selection is handled through a ``HeapSelect`` class. This
      allows a specific heap selection for the duration of a scope.
   -  Use this option, if you are still running out of DRAM space after
      you have moved as many of your constant strings/data elements that
      you can to PROGMEM.

4. ``16KB cache + 32KB IRAM + 16KB 2nd Heap (not shared)``

   -  Not managed by the ``umm_malloc`` heap library
   -  Non-32-Bit Access for IRAM must be enabled separately
   -  A 16KB block of unmanaged memory.
   -  Data persist across reboots, but not deep sleep.
   -  This may be best for when you just need a large chunk of memory.
      This option will free up the additional resources needed by
      ``umm_malloc`` to support managing the 2nd Heap.

MMU related build defines and possible values.These values change as
indicated with the menu options above:

+-------------------------+--------------+--------------+------------------------------------+------------------------------+
| ``#define``             | balanced     | IRAM         | shared (IRAM and Heap)             | not shared (IRAM and Heap)   |
+=========================+==============+==============+====================================+==============================+
| ``MMU_IRAM_SIZE``       | ``0x8000``   | ``0xC000``   | ``0xC000``                         | ``0x8000``                   |
+-------------------------+--------------+--------------+------------------------------------+------------------------------+
| ``MMU_ICACHE_SIZE``     | ``0x8000``   | ``0x4000``   | ``0x4000``                         | ``0x4000``                   |
+-------------------------+--------------+--------------+------------------------------------+------------------------------+
| ``MMU_IRAM_HEAP``       | --           | --           | defined, enables\ ``umm_malloc``   | --                           |
+-------------------------+--------------+--------------+------------------------------------+------------------------------+
| ``MMU_SEC_HEAP``        | --           | \*\*         | \*\*                               | ``0x40108000``               |
+-------------------------+--------------+--------------+------------------------------------+------------------------------+
| ``MMU_SEC_HEAP_SIZE``   | --           | \*\*         | \*\*                               | ``0x4000``                   |
+-------------------------+--------------+--------------+------------------------------------+------------------------------+

\*\* This define is to an inline function that calculates the value,
based on unused code space.

**The Arduino IDE Tools menu option, ``Non-32-Bit Access`` allows for
the following selections:**

-  ``Use pgm_read macros for IRAM/PROGMEM``
-  ``Byte/Word access to IRAM/PROGMEM (very slow)``

   -  This option adds a non32-bit exception handler to your build.
   -  Handles read/writes to IRAM and reads to ICACHE.
   -  Supports short and byte access to IRAM.
   -  Not recommended for high-frequency access data, use DRAM if you
      can.
   -  Expect it to be slower than DRAM, each character access will
      require a complete save and restore of all 16+ registers.
   -  Processing an exception uses 256 bytes of stack space just to get
      started. The actual handler will add a little more.
   -  This option is implicitly enabled and required when you select MMU
      option ``16KB cache + 48KB IRAM and 2nd Heap (shared)``.

IRAM, unlike DRAM, must be accessed as aligned full 32-bit words, no
byte or short access. The pgm\_read macros are an option; however, the
store operation remains an issue. For a block copy ets\_memcpy - appears
to work well as long as the byte count is rounded up to be evenly
divided by 4 and source and destination addresses are 4 bytes aligned.

A word of caution, I have seen one case with the new toolchain 10.1
where code that reads a 32-bit word to extract a byte was optimized away
to be a byte read. Using ``volatile`` on the pointer stopped the
over-optimization.

To get a sense of how memory access time is effected, see examples
``MMU48K`` and ``irammem`` in ``ESP8266``.

Miscellaneous
-------------

For calls to ``umm_malloc`` with interrupts disabled.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  ``malloc`` will always allocate from the ``DRAM`` heap when called
   with interrupts disabled.
-  ``realloc`` with a NULL pointer will use ``malloc`` and return a
   ``DRAM`` heap allocation. Note, calling ``realloc`` with interrupts
   disabled is **not** officially supported. You are on your own if you
   do this.
-  If you must use IRAM memory in your ISR, allocate the memory ahead of
   time. And avoid non32-bit access that would trigger the exception
   handler.

    I think this block should go away and we always use
    ``USE_ISR_SAFE_EXC_WRAPPER`` with the exception handler.

    -  ``realloc`` will fail if not built with
       ``USE_ISR_SAFE_EXC_WRAPPER`` defined.
    -  The current build has ``USE_ISR_SAFE_EXC_WRAPPER`` defined in
       ``mmu_iram.h``.

    .. rubric:: ISR/Exception Handler Issue
       :name: isrexception-handler-issue

    The non-32-bit exception handler is called by a "C" wrapper function
    in ROM. This ROM function enables interrupts before calling our
    registered handler. Defining ``USE_ISR_SAFE_EXC_WRAPPER`` in
    ``mmu_iram.h`` will install a replacement that does not enable
    interrupts (now default). The effects on Network performance are
    unknown.

    To keep ISR execution time with interrupts disabled at a minimum,
    avoid the use of IRAM from ISRs. Especially the use of non-32-bit
    read/writes on IRAM.

How to Select Heap
~~~~~~~~~~~~~~~~~~

The ``MMU`` selection ``16KB cache + 48KB IRAM and 2nd Heap (shared)``
allows you to use the standard heap API function calls (``malloc``,
``calloc``, ``free``, ... ). to allocate memory from DRAM or IRAM. The
selection can be made by instantiating the class ``HeapSelectIram`` or
``HeapSelectDram``.The usage is similar to that of the ``InterruptLock``
class. The default/initial heap source is DRAM. The class is in
``umm_malloc/umm_heap_select.h``

::

      ...
        char *bufferDram;
        bufferDram = (char *)malloc(33);
        char *bufferIram;
        {
            HeapSelectIram ephemeral;
            bufferIram = (char *)malloc(33);
        }
      ...
        free(bufferIram);
        free(bufferDram);
      ...

``free`` will always return memory to the correct heap. There is no need
for tracking and selecting before freeing.

``realloc`` with a non-NULL pointer will always resize the allocation
from the original heap it was allocated from. When the supplied pointer
is NULL, then the current heap selection is used.

Low level primatives for selecting a heap. These are used by the above
Classes:

-  ``umm_get_current_heap_id()``
-  ``umm_set_heap_by_id( ID value )``
-  Possible ID values
-  ``UMM_HEAP_DRAM``
-  ``UMM_HEAP_IRAM``

Also, an alternate stack select based API is available. This is not as
easy as the class method; however, for some small set of cases it may
provide some additional control:

-  ``ESP.setIramHeap()`` Pushes current heap ID onto a stack and sets
   Heap API for an IRAM selection.
-  ``ESP.setDramHeap()`` Pushes current heap ID onto a stack and sets
   Heap API for a DRAM selection.
-  ``ESP.resetHeap()`` Restores previously pushed heap.
