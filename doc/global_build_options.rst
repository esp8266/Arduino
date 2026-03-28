Global build defines and options
================================

Basics
------

To create globally usable macro definitions for a Sketch, create a file
with a name based on your Sketch’s file name followed by ``.globals.h``
in the Sketch folder. For example, if the main Sketch file is named
``LowWatermark.ino``, its global ``.h`` file would be
``LowWatermark.ino.globals.h``. This file will be implicitly included
with every module built for your Sketch. Do not directly include it in
any of your sketch files or in any other source files. There is no need
to create empty/dummy files, when not used.

This global ``.h`` also supports embedding compiler command-line options
in a unique “C” block comment. Compiler options are placed in a “C”
block comment starting with ``/*@create-file:build.opt@``. This
signature line must be alone on a single line. The block comment ending
``*/`` should also be alone on a single line. In between, place your
compiler command-line options just as you would have for the GCC @file
command option.

Actions taken in processing comment block to create ``build.opt``

-  for each line, white space is trimmed
-  blank lines are skipped
-  lines starting with ``*``, ``//``, or ``#`` are skipped
-  the remaining results are written to arduino-cli build cache directory
-  multiple ``/*@create-file:build.opt@`` ``*/`` comment blocks are
   allowed and would be merged in order they are written in the file
-  ``build.opt`` is finished with a ``-include ...`` command, which
   references the global .h its contents were extracted from.

Example
-------

Example Sketch: ``LowWatermark.ino``

.. code:: cpp

   #include <umm_malloc/umm_malloc.h>  // has prototype for umm_free_heap_size_min()

   void setup() {
     Serial.begin(115200);
     delay(200);
   #ifdef MYTITLE1
     Serial.printf("\r\n" MYTITLE1 MYTITLE2 "\r\n");
   #else
     Serial.println("ERROR:  MYTITLE1 not present");
   #endif
     Serial.printf("Heap Low Watermark %u\r\n", umm_free_heap_size_min());
   }

   void loop() {}

Global ``.h`` file: ``LowWatermark.ino.globals.h``

.. code:: cpp

   /*@create-file:build.opt@
   // An embedded build.opt file using a "C" block comment. The starting signature
   // must be on a line by itself. The closing block comment pattern should be on a
   // line by itself. Each line within the block comment will be space trimmed and
   // written to build.opt, skipping blank lines and lines starting with '//', '*'
   // or '#'.

    * this line is ignored
    # this line is ignored
   -DMYTITLE1="\"Running on \""
     -O3
   //-fanalyzer
   -DUMM_STATS_FULL=1
   */

   #ifndef LOWWATERMARK_INO_GLOBALS_H
   #define LOWWATERMARK_INO_GLOBALS_H

   #if !defined(__ASSEMBLER__)
   // Defines kept away from assembler modules
   // i.e. Defines for .cpp, .ino, .c ... modules
   #endif

   #if defined(__cplusplus)
   // Defines kept private to .cpp and .ino modules
   //#pragma message("__cplusplus has been seen")
   #define MYTITLE2 "Empty"
   #endif

   #if !defined(__cplusplus) && !defined(__ASSEMBLER__)
   // Defines kept private to .c modules
   #define MYTITLE2 "Full"
   #endif

   #if defined(__ASSEMBLER__)
   // Defines kept private to assembler modules
   #endif

   #endif

Separate production and debug build options
-------------------------------------------

If your production and debug build option requirements are different,
adding ``mkbuildoptglobals.extra_flags={build.debug_port}`` to
``platform.local.txt`` will create separate build option groups for
debugging and production. For the production build option group, the “C”
block comment starts with ``/*@create-file:build.opt@``, as previously
defined. For the debugging group, the new “C” block comment starts with
``/*@create-file:build.opt:debug@``. You make your group selection
through “Arduino->Tools->Debug port” by selecting or disabling the
“Debug port.”

Options common to both debug and production builds must be included in
both groups. Neither of the groups is required. You may also omit either
or both.

Reminder with this change, any old “sketch” with only a “C” block
comment starting with ``/*@create-file:build.opt@`` would not use a
``build.opt`` file for the debug case. Update old sketches as needed.

Updated Global ``.h`` file: ``LowWatermark.ino.globals.h``

.. code:: cpp

   /*@create-file:build.opt:debug@
   // Debug build options
   -DMYTITLE1="\"Running on \""
   -DUMM_STATS_FULL=1

   //-fanalyzer

   // Removing the optimization for "sibling and tail recursive calls" may fill
   // in some gaps in the stack decoder report. Preserves the stack frames
   // created at each level as you call down to the next.
   -fno-optimize-sibling-calls
   */

   /*@create-file:build.opt@
   // Production build options
   -DMYTITLE1="\"Running on \""
   -DUMM_STATS_FULL=1
   -O3
   */

   #ifndef LOWWATERMARK_INO_GLOBALS_H
   #define LOWWATERMARK_INO_GLOBALS_H

   #if defined(__cplusplus)
   #define MYTITLE2 "Empty"
   #endif

   #if !defined(__cplusplus) && !defined(__ASSEMBLER__)
   #define MYTITLE2 "Full"
   #endif

   #ifdef DEBUG_ESP_PORT
   // Global Debug defines
   // ...
   #else
   // Global Production defines
   // ...
   #endif

   #endif


Custom build environments
-------------------------

Some custom build environments may have already addressed this issue by
other means. If you have a custom build environment that does not
require this feature and would like to turn it off, you can add the
following lines to the ``platform.local.txt`` used in your build
environment:

.. code-block:: ini

   recipe.hooks.prebuild.2.pattern=
   build.opt.flags=

Source Code
-----------

https://github.com/esp8266/Arduino/blob/master/tools/mkbuildoptglobals.py


Internals
---------

:doc:`/buildopt/internals`

IDE 1.x aggressive caching
--------------------------

.. attention::

    This article applies ONLY to IDE 1.x and original version
    of the mkbuildoptglobals.py script shipped with Core 3.1.x

:doc:`/buildopt/aggressive-caching`

