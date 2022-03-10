How to specify global build defines and options
===============================================

To create global defines for a Sketch, create a file with a name based
on your sketch’s file name followed by ``.globals.h`` in the Sketch
folder. For example, if the main Sketch file is named
``LowWatermark.ino``, its global defines file would be
``LowWatermark.ino.globals.h``. This file will be implicitly included
with every module built for your Sketch. Do not directly include it in
any of your sketch files or in any other source files. There is no need
to create empty/dummy files, when not used.

This global define ``.h`` also supports embedding compiler command-line
options in a unique “C” block comment. Compiler options are placed in a
“C” block comment starting with ``/*@create-file:build.opt@``. This
signature line must be alone on a single line. The block comment ending
``*/`` should also be alone on a single line. In between, place your
compiler command-line options just as you would have for the GCC @file
command option.

Actions taken in processing comment block to create ``build.opt`` \* for
each line, white space is trimmed \* blank lines are skipped \* lines
starting with ``*``, ``//``, or ``#`` are skipped \* the remaining
results are written to build tree\ ``/core/build.opt`` \* multiple
``/*@create-file:build.opt@`` ``*/`` comment blocks are not allowed \*
``build.opt`` is finished with a ``-include ...`` command, which
references the global .h its contents were extracted from.

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

Aggressive Caching of ``core.a``
================================

The feature “Aggressive Caching of core.a” refers to sharing a single
copy of ``core.a`` across all Arduino IDE Sketch windows. This feature
is on by default. ``core.a`` is an archive file containing the compiled
objects of ``./core/esp8266/*``. Created after your 1ST successful
compilation. All other open sketch builds use this shared file. When you
close all Arduino IDE windows, the core archive file is deleted.

Without mediation, using global defines or compiler command-line options
could lead to bad builds when the “Aggressively cache compiled core”
feature is enabled. When ``#define`` changes require rebuilding
``core.a`` and multiple Sketches are open, they can no longer reliably
share one cached ``core.a``. In a simple case: The 1st Sketch to be
built has its version of ``core.a`` cached. Other sketches will use this
cached version for their builds.

When the “Aggressively cache compiled core” feature is enabled and a
global define file is detected, a workaround will turn on and stay on.
When you switch between Sketch windows, core will be recompiled and the
cache updated.

If you think your workflow performance would benefit from keeping a per
Sketch copy of ``core.a``, you can turn off the “Aggressively cache
compiled core” feature. You need to find the location of
``preferences.txt``. From the Arduino IDE, go to *File->Preferences*.
Make note of the path to ``prefereces.txt``. You cannot edit the file
while the Arduino IDE is running. Close all Arduino IDE windows and edit
the file ``preferences.txt``. Change ``compiler.cache_core=true`` to
``compiler.cache_core=false`` and save. Then each sketch will maintain
its *own* copy of ``core.a`` built with the customization expressed by
their respective ``build.opt`` file.

Other build confusion
=====================

1. Renaming a file does not change the last modified timestamp, possibly
   causing issues when adding a file by renaming and rebuilding. A good
   example of this problem would be to have then fixed a typo in file
   name ``LowWatermark.ino.globals.h``. You need to touch (update
   timestamp) the file so a “rebuild all” is performed.

2. When a ``.h`` file is renamed in the sketch folder, a copy of the old
   file remains in the build sketch folder. This can create confusion if
   you missed an edit in updating an ``#include`` in one or more of your
   modules. That module will continue to use the stale version of the
   ``.h`` until you restart the IDE or other major changes that would
   cause the IDE to delete and recopy the contents from the source
   Sketch directory. Changes on the IDE Tools board settings may cause a
   complete rebuild, clearing the problem. This may be the culprit for
   “What! It built fine last night!”
