How to specify global build defines and options
===============================================

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

Aggressively cache compiled core
================================

This feature appeared with the release of Arduino IDE 1.8.2. The feature
“Aggressively Cache Compiled core” refers to sharing a single copy of
``core.a`` across all Arduino IDE Sketch windows. This feature is on by
default. ``core.a`` is an archive file containing the compiled objects
of ``./core/esp8266/*``. Created after your 1ST successful compilation.
All other open sketch builds use this shared file. When you close all
Arduino IDE windows, the core archive file is deleted.

This feature is not compatible with using global defines or compiler
command-line options. Without mediation, bad builds could result, when
left enabled. When ``#define`` changes require rebuilding ``core.a`` and
multiple Sketches are open, they can no longer reliably share one cached
``core.a``. In a simple case: The 1st Sketch to be built has its version
of ``core.a`` cached. Other sketches will use this cached version for
their builds.

There are two solutions to this issue: 1. Turn off the “Aggressively
Cache Compiled core” feature, by setting ``compiler.cache_core=false``.
2. Rely on the not ideal fail-safe, aggressive cache workaround built
into the script.

Using “compiler.cache_core=false”
---------------------------------

There are two ways to turn off the “Aggressively Cache Compiled core”
feature: This can be done with the Arduino IDE command-line or a text
editor.

Using the Arduino IDE command-line from a system command line, enter the
following:

::

   arduino --pref compiler.cache_core=false --save-prefs

For the text editor, you need to find the location of
``preferences.txt``. From the Arduino IDE, go to *File->Preferences*.
Make note of the path to ``prefereces.txt``. You *cannot* edit the file
while the Arduino IDE is running. Close all Arduino IDE windows and edit
the file ``preferences.txt``. Change ``compiler.cache_core=true`` to
``compiler.cache_core=false`` and save. Then each sketch will maintain
its *own* copy of ``core.a`` built with the customization expressed by
their respective ``build.opt`` file.

The “workaround”
----------------

When the “Aggressively Cache Compiled core” feature is enabled and the
global define file is detected, a workaround will turn on and stay on.
When you switch between Sketch windows, core will be recompiled and the
cache updated. The workaround logic is reset when Arduino IDE is
completely shutdown and restarted.

The workaround is not perfect. These issues may be of concern: 1. Dirty
temp space. Arduino build cache files left over from a previous run or
boot. 2. Arduino command-line options: \* override default
preferences.txt file. \* override a preference, specifically
``compiler.cache_core``. 3. Multiple versions of the Arduino IDE running

**Dirty temp space**

A minor concern, the workaround is always on. Not an issue for build
accuracy, but ``core.a`` maybe rebuild more often than necessary.

Some operating systems are better at cleaning up their temp space than
others at reboot after a crash. At least for Windows®, you may need to
manually delete the Arduino temp files and directories after a crash.
Otherwise, the workaround logic may be left on. There is no harm in the
workaround being stuck on, the build will be correct; however, the core
files will occasionally be recompiled when not needed.

For some Windows® systems the temp directory can be found near
``C:\Users\<user id>\AppData\Local\Temp\arduino*``. Note ``AppData`` is
a hidden directory. For help with this do an Internet search on
``windows disk cleanup``. Or, type ``disk cleanup`` in the Windows®
taskbar search box.

With Linux, this problem could occur after an Arduino IDE crash. The
problem would be cleared after a reboot. Or you can manually cleanup the
``/tmp/`` directory before restarting the Arduino IDE.

**Arduino command-line option overrides**

The script needs to know the working value of ``compiler.cache_core``
that the Arduino IDE uses when building. This script can learn the state
through documented locations; however, the Arduino IDE has two
command-line options that can alter the results the Arduino IDE uses
internally. And, the Arduino IDE does not provide a means for a script
to learn the override value.

These two command-line options are the problem:

::

   ./arduino --preferences-file other-preferences.txt
   ./arduino --pref compiler.cache_core=false

Hints for discovering the value of ``compiler.cache_core``, can be
provided by specifying ``mkbuildoptglobals.extra_flags=...`` in
``platform.local.txt``.

Examples of hints:

::

   mkbuildoptglobals.extra_flags=--preferences_sketch            # assume file preferences.txt in the sketch folder
   mkbuildoptglobals.extra_flags=--preferences_sketch "pref.txt" # is relative to the sketch folder
   mkbuildoptglobals.extra_flags=--no_cache_core
   mkbuildoptglobals.extra_flags=--cache_core
   mkbuildoptglobals.extra_flags=--preferences_file "other-preferences.txt" # relative to IDE or full path

If required, remember to quote file or file paths.

**Multiple versions of the Arduino IDE running**

You can run multiple Arduino IDE windows as long as you run one version
of the Arduino IDE at a time. When testing different versions,
completely exit one before starting the next version. For example,
Arduino IDE 1.8.19 and Arduino IDE 2.0 work with different temp and
build paths. With this combination, the workaround logic sometimes fails
to enable.

At the time of this writing, when Arduino IDE 2.0 rc5 exits, it leaves
the temp space dirty. This keeps the workaround active the next time the
IDE is started. If this is an issue, manually delete the temp files.

Custom build environments
=========================

Some custom build environments may have already addressed this issue by
other means. If you have a custom build environment that does not
require this feature and would like to turn it off, you can add the
following lines to the ``platform.local.txt`` used in your build
environment:

::

   recipe.hooks.prebuild.2.pattern=
   build.opt.flags=

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
