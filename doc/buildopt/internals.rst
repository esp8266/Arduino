:orphan:

mkbuildoptglobals.py internals
==============================

Sketch header aka SKETCH.ino.globals.h
--------------------------------------

.. hint::

    ``SKETCH.ino.globals.h`` is expected to be created by the user.

It is always located in the root of the SKETCH directory, and must use the
the actual name of the sketch program (``SKETCH.ino``) in the its name.

Header file format is used because IDE only manages source files it actually
recognizes as valid C / C++ file formats. When building and re-building the
sketch, only valid file formats are taken into an account when building source
code dependencies tree.

Command-line options file
-------------------------

    .. hint::

        This file is created by the script.

Options file is generated based on the contents of the sketch header comment block,
and its contents are then used as gcc command-line options (``@file``)

    If file does not exist, or cannot be read, then the option will be treated literally, and not removed.
    
    Options in file are separated by whitespace. A whitespace character may be included
    in an option by surrounding the entire option in either single or double quotes.
    Any character (including a backslash) may be included by prefixing the character
    to be included with a backslash.
    The file may itself contain additional @file options; any such options will be processed recursively.

    --- https://gcc.gnu.org/onlinedocs/gcc/Overall-Options.html


Arduino build system uses timestamps to determine which files should be rebuilt.
``@file`` cannot be a direct dependency, only other source code files can.

Thus, command-line options file is *always* created with at least one ``-include``

.. code-block:: console

    -include "PLATFORM_PATH / COMMON_HEADER_PATH"

When matching directive is found in the sketch header, path to its copy in the build directory is also added

.. code-block:: console

    -include "BUILD_DIRECTORY / SKETCH_HEADER_COPY_PATH"

Common header
-------------

.. note::

    This file is also created by the script.

It is used as a means of triggering core rebuild, because modern Arduino build systems
are agressively caching it and attempt to re-use existing ``core.a`` whenever possible.

This file would contain path to the currently used command-line options file extracted
from the sketch header. It remains empty otherwise.

Build directory
---------------

Arduino build process copies every valid source file from the source (sketch)
directory into the build directory. This script is expected to be launched in
the "prebuild" stage. At that point, build directory should already exist, but
it may not yet contain any of the sketch source files.

Script would always attempt to copy sketch header from the source (sketch)
directory to the build one. If it does not exist, a placeholder would be created.

Script would always synchronize atime & mtime of every file. When sketch header
exists, stats are taken from it. When it doesn't, stats for the generated common
header are used instead.

Configuration
-------------

``platform.txt`` is expected to have this script listed as a tool

.. code-block:: ini

    runtime.tools.mkbuildoptglobals={runtime.platform.path}/tools/mkbuildoptglobals.py

Paths are always provided as Fully Qualified File Names (FQFNs):

.. code-block:: ini

    globals.h.source.fqfn={build.source.path}/{build.project_name}.globals.h
    globals.h.common.fqfn={build.core.path}/__common_globals.h
    build.opt.fqfn={build.path}/core/build.opt
    mkbuildoptglobals.extra_flags=

`"prebuild" hook <https://docs.arduino.cc/arduino-cli/platform-specification/#pre-and-post-build-hooks-since-arduino-ide-165>`__ must be used,
allowing this script to run *before* build process creates and / or copies them.  
Both Arduino IDE 1.x and 2.x generate
`prerequisite makefiles (files with a .d suffix) <https://www.gnu.org/software/make/manual/html_node/Automatic-Prerequisites.html>`__
at some point in "discovery phase".

.. code-block:: ini

    recipe.hooks.prebuild.#.pattern=
        "{runtime.tools.python3.path}/python3" -I
        "{runtime.tools.mkbuildoptglobals}" {mkbuildoptglobals.extra_flags} build
        --build-path "{build.path}"
        --build-opt "{build.opt.fqfn}"
        --sketch-header "{globals.h.source.fqfn}"
        --common-header "{commonhfile.fqfn}"

Command-line options file is then shared between other recipes by including it in
the "cpreprocessor" flags.

.. code-block:: ini

    compiler.cpreprocessor.flags=... @{build.opt.path} ...

After that point, prerequisite makefiles should contain either only the common header,
or both the common header and the build sketch header. When any of included headers is
modified, every file in the dependency chain would be rebuilt. This allows us to keep
existing ``core.a`` cache when command-line options file is not used by the sketch.

Example
-------

Sketch header file with embedded command-line options file might look like this

.. code-block:: c++
   :emphasize-lines: 1,2,3,4,5,6,7,8,9,10,11

    /*@create-file:build.opt@
    // An embedded "build.opt" file using a "C" block comment. The starting signature
    // must be on a line by itself. The closing block comment pattern should be on a
    // line by itself. Each line within the block comment will be space trimmed and
    // written to build.opt, skipping blank lines and lines starting with '//', '*'
    // or '#'.
    -DMYDEFINE="\"Chimichangas do not exist\""
    -O3
    -fanalyzer
    -DUMM_STATS=2
    */

    #ifndef SKETCH_INO_GLOBALS_H
    #define SKETCH_INO_GLOBALS_H

    #if defined(__cplusplus)
    // Defines kept private to .cpp modules
    //#pragma message("__cplusplus has been seen")
    #endif

    #if !defined(__cplusplus) && !defined(__ASSEMBLER__)
    // Defines kept private to .c modules
    #endif

    #if defined(__ASSEMBLER__)
    // Defines kept private to assembler modules
    #endif

    #endif


Caveats, Observations, and Ramblings
------------------------------------

1. Edits to ``platform.txt`` or ``platform.local.txt`` force a complete rebuild that
   removes the core folder. Not a problem, just something to be aware of when
   debugging this script. Similarly, changes on the IDE Tools selection cause a
   complete rebuild.

   In contrast, the core directory is not deleted when the rebuild occurs from
   changing a file with an established dependency (inspect .d in the build path)

2. Renaming files does not change the last modified timestamp, possibly causing
   issues when adding or replacing files by renaming and rebuilding.

   A good example of this problem is when you correct the spelling of sketch
   header file. You must update mtime (e.g. call touch) of the file.

3. ``-include ".../Sketch.ino.globals.h"`` is conditionally added to every compilation command,
   so it may be reasonable to expect that ``#include "Sketch.ino.globals.h"`` is no longer necessary.

   However, it may not be the case when `create-file:...` directive is missing or does not match.

   When explicit ``#include "Sketch.ino.globals.h"`` is used in the code, it must always be guarded against including it twice:

   .. code-block:: c++
      :emphasize-lines: 1

        #pragma once

   Or, by using classic header guards:

   .. code-block:: c++
      :emphasize-lines: 1,2,4

        #infdef SKETCH_GLOBALS_H
        #define SKETCH_GLOBALS_H
            ... file contents ...
        #endif

4. ``build.opt`` itself is not listed as a dependency in .d, .h files are used
   because this is the only obvious way to force arduino-builder / arduino-cli
   into tracking it.

5. When not using ``--build-path``, ``core.a`` is cached and shared.
   CI sometimes uses `ARDUINO_BUILD_CACHE_PATH environment variable <https://arduino.github.io/arduino-cli/1.2/configuration/>`__.
   This allows to have a private core cache, separate from the system one.

6. `Referencing upstream arduino-cli code (v1.2.2) <https://github.com/arduino/arduino-cli/blob/1.2.2/internal/arduino/builder/core.go#L88-L110>`__, ``core.a`` cache key is based on:

   * `ESP8266 Platform Path`, and depends on installation method

     * `Installing ESP8266 Core <../installing.rst>`__
     * `Arduino Platform Installation Directories <https://docs.arduino.cc/arduino-cli/platform-specification/#platform-installation-directories>`__

   * `FQBN`

     See `Arduino Custom Board Options <https://docs.arduino.cc/arduino-cli/platform-specification/#custom-board-options>`__).

   * `Optimization flags`

     .. attention::

        ``{compiler.optimization_flags}`` is not currently used in the ESP8266 Core

     See `Arduino Optimization Level setting <https://docs.arduino.cc/arduino-cli/platform-specification/#optimization-level-for-debugging>`__).

