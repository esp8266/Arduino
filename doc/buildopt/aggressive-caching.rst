:orphan:

Aggressively cache compiled core
================================

.. attention::

    This article applies ONLY to IDE 1.x and original version
    of the mkbuildoptglobals.py script shipped with Core 3.1.x

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

There are two solutions to this issue:

1. Do nothing, and rely on aggressive cache workaround built into the
   script.
2. Turn off the “Aggressively Cache Compiled core” feature, by setting
   ``compiler.cache_core=false``.

Using “compiler.cache_core=false”
---------------------------------

There are two ways to turn off the “Aggressively Cache Compiled core”
feature: This can be done with the Arduino IDE command-line or a text
editor.

Using the Arduino IDE command-line from a system command line, enter the
following:

.. code-block:: console

   $ arduino --pref compiler.cache_core=false --save-prefs

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

The workaround is not perfect. These issues may be of concern:

1. Dirty temp space. Arduino build cache files left over from a previous
   run or boot.
2. Arduino command-line options:

   -  override default preferences.txt file.
   -  override a preference, specifically ``compiler.cache_core``.

3. Multiple versions of the Arduino IDE running

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

If you are building with ``compiler.cache_core=true`` no action is
needed. If ``false`` the script would benefit by knowing that.

When using either of these two command-line options:

.. code-block:: console

   $ arduino --preferences-file other-preferences.txt
   $ arduino --pref compiler.cache_core=false

Hints for discovering the value of ``compiler.cache_core``, can be
provided by specifying ``mkbuildoptglobals.extra_flags=...`` in
``platform.local.txt``.

Examples of hints:

.. code-block:: ini

   mkbuildoptglobals.extra_flags=--preferences_sketch            # assume file preferences.txt in the sketch folder
   mkbuildoptglobals.extra_flags=--preferences_sketch "pref.txt" # is relative to the sketch folder
   mkbuildoptglobals.extra_flags=--no_cache_core
   mkbuildoptglobals.extra_flags=--cache_core
   mkbuildoptglobals.extra_flags=--preferences_file "other-preferences.txt" # relative to IDE or full path

If required, remember to quote file or file paths.

Multiple versions of the Arduino IDE running
--------------------------------------------

You can run multiple Arduino IDE windows as long as you run one version
of the Arduino IDE at a time. When testing different versions,
completely exit one before starting the next version. For example,
Arduino IDE 1.8.19 and Arduino IDE 2.0 work with different temp and
build paths. With this combination, the workaround logic sometimes fails
to enable.

At the time of this writing, when Arduino IDE 2.0 rc5 exits, it leaves
the temp space dirty. This keeps the workaround active the next time the
IDE is started. If this is an issue, manually delete the temp files.

Also note when a ``.h`` file is renamed in the sketch folder, a copy of the old
file remains in the build sketch folder. This can create confusion if
you missed an edit in updating an ``#include`` in one or more of your
modules. That module will continue to use the stale version of the
``.h`` until you restart the IDE or other major changes that would
cause the IDE to delete and recopy the contents from the source
Sketch directory. Changes on the IDE Tools board settings may cause a
complete rebuild, clearing the problem. This may be the culprit for
“What! It built fine last night!”

