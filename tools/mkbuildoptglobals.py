#!/usr/bin/env python3

# This script manages the use of a file with a unique name, like
# `SketchName.ino.globals.h`, in the Sketch source directory to provide compiler
# command-line options (build options) and sketch global defines. The build
# option data is encapsulated in a unique "C" comment block and extracted into
# the build tree during prebuild.
#
# Copyright (C) 2022 - M Hightower
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# A Tip of the hat to:
#
# This PR continues the effort to get some form of global build support
# presented by brainelectronics' PR https://github.com/esp8266/Arduino/pull/8095
#
# Used d-a-v's global name suggestion from arduino PR
# https://github.com/arduino/arduino-cli/pull/1524
#
"""
Operation

"SketchName.ino.globals.h" - A global h file in the Source Sketch directory. The
string SketchName is the actual name of the sketch. A matching copy is kept in
the build path/core directory. The file is empty when it does not exist in the
source directory.

Using SketchName.ino.globals.h as a container to hold build.opt, gives implicit
dependency tracking for build.opt by way of SketchName.ino.globals.h's
dependencies.
Example:
  gcc ... @{build.path}/core/build.opt -include "{build.path}/core/{build.project_name}.globals.h" ...

In this implementation the '-include "{build.path}/core/{build.project_name}.globals.h"'
component is added to the build.opt file.
  gcc ... @{build.path}/core/build.opt ...

At each build cycle, "{build.project_name}.globals.h" is conditoinally copied to
"{build.path}/core/" at prebuild, and build.opt is extraction as needed. The
SketchName.ino.globals.h's dependencies will trigger "rebuild all" as needed.

If SketchName.ino.globals.h is not in the source sketch folder, an empty
versions is created in the build tree. The file build.opt always contains a
"-include ..." entry so that file dependencies are generated for
SketchName.ino.globals.h. This allows for change detection when the file is
added.
"""

"""
Arduino `preferences.txt` changes

"Aggressively cache compiled core" must be turned off for a reliable build process.
In ~/.arduino15/preferences.txt, to disable the feature:
  compiler.cache_core=false

Reference:
https://forum.arduino.cc/t/no-aggressively-cache-compiled-core-in-ide-1-8-15/878954/2
"""

"""
# Updates or Additions for platform.txt or platform.local.txt

runtime.tools.mkbuildoptglobals={runtime.platform.path}/tools/mkbuildoptglobals.py

# Fully qualified and relative file names for processing sketch global options
globals.h.source.fqfn={build.source.path}/{build.project_name}.globals.h
build.globals.path={build.path}/core
globals.h.fqfn={build.globals.path}/{build.project_name}.globals.h
build.opt.fqfn={build.globals.path}/build.opt

recipe.hooks.prebuild.2.pattern="{runtime.tools.python3.path}/python3" "{runtime.tools.mkbuildoptglobals}" "{globals.h.source.fqfn}" "{globals.h.fqfn}" "{build.opt.fqfn}"

compiler.cpreprocessor.flags=-D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ -D_GNU_SOURCE -DESP8266 @{build.opt.path} "-I{compiler.sdk.path}/include" "-I{compiler.sdk.path}/{build.lwip_include}" "-I{compiler.libc.path}/include" "-I{build.path}/core"
"""

"""
A Sketch.ino.globals.h file with embedded build.opt might look like this

/*@create-file:build.opt@
// An embedded build.opt file using a "C" block comment. The starting signature
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
"""

"""
Added 2) and 5) to docs

Caveats, Observations, and Ramblings

1) Edits to platform.txt or platform.local.txt force a complete rebuild that
removes the core folder. Not a problem, just something to be aware of when
debugging this script. Similarly, changes on the IDE Tools selection cause a
complete rebuild.

In contrast, the core directory is not deleted when the rebuild occurs from
changing a file with an established dependency.

2) Renaming files does not change the last modified timestamp, possibly causing
issues when replacing files by renaming and rebuilding.

A good example of this problem is when you correct the spelling of file
SketchName.ino.globals.h. You need to touch (update time stampt) the file so a
rebuild all is performed.

3) During the build two identical copies of SketchName.ino.globals.h will exist.
#ifndef fencing will be needed for non comment blocks in SketchName.ino.globals.h.

4) By using a .h file to encapsulate "build.opt" options, the information is not
lost after a save-as. Before with an individual "build.opt" file, the file was
missing in the saved copy.

5) When a .h file is renamed, a copy of the old file remains in the build
sketch folder. This can create confusion if you missed an edit in updating an
include in one or more of your modules. That module will continue to use the
stale version of the .h, until you restart the IDE or other major changes that
would cause the IDE to delete and recopy the contents from the source sketch.

This may be the culprit for "What! It built fine last night!"

6a) In The case of two Arduino IDE screens up with different programs, they can
share the same core archive file. Defines on one screen will change the core
archive, and a build on the 2nd screen will build with those changes.
The 2nd build will have the core built for the 1st screen. It gets uglier. With
the 2nd program, the newly built modules used headers processed with different
defines than the core.

6b) Problem: Once core has been build, changes to build.opt or globals.h will
not cause the core archive to be rebuild. You either have to change tool
settings or close and reopen the Arduino IDE. This is a variation on 6a) above.
I thought this was working for the single sketch case, but it does not! :(
That is because sometimes it does build properly. What is unknown are the
causes that will make it work and fail?
  * Fresh single Arduino IDE Window, open with file to build - works

I think these, 6a and 6b, are resolved by setting `compiler.cache_core=false`
in ~/.arduino15/preferences.txt, to disable the aggressive caching feature:
  https://forum.arduino.cc/t/no-aggressively-cache-compiled-core-in-ide-1-8-15/878954/2

Added workaround for `compiler.cache_core=true` case.
See `if use_aggressive_caching_workaround:` in main().

7) Suspected but not confirmed. A quick edit and rebuild don't always work well.
Build does not work as expected. This does not fail often. Maybe PIC NIC.
"""

import os
import sys
import filecmp
import time
import platform
from shutil import copyfile

# Need to work on signature line used for match to avoid conflicts with
# existing embedded documentation methods.
build_opt_signature = "/*@create-file:build.opt@"

docs_url = "https://arduino-esp8266.readthedocs.io/en/latest/faq/a06-global-build-options.html"

def print_msg(*args, **kwargs):
    print(*args, **kwargs)


# I prefer error messages to stand out; however, using stderr for a different
# color does not work on the new Arduino IDE 2.0 RC4. Also,  separate pipes,
# buffering, and multiple threads with output can create mixed-up messages.
# Bring attention to errors with a blank line and lines starting with "*** ".
# Let multiple prints buffer to aid them in staying together.
def print_err(*args, **kwargs):
    if (args[0])[0] != ' ':
        print("")
    print("*** ", end='')
    print(*args, **kwargs)


def copy_create_build_file(source_fqfn, build_target_fqfn):
    """
    Conditionally copy a newer file between the source directory and the build
    directory. When source file is missing, create an empty file in the build
    directory.
    return     True when file change detected.
    """
    if os.path.exists(source_fqfn):
        if (os.path.exists(build_target_fqfn)) and \
        (os.path.getmtime(build_target_fqfn) >= os.path.getmtime(source_fqfn)):
            # only copy newer files - do nothing, all is good
            return False
        else:
            # The new copy gets stamped with the current time, just as other
            # files copied by `arduino-builder`.
            copyfile(source_fqfn, build_target_fqfn)
    else:
        if os.path.exists(build_target_fqfn) and \
        os.path.getsize(build_target_fqfn) == 0:
            return False
        else:
            # Place holder - Must have an empty file to satisfy parameter list
            # specifications in platform.txt.
            with open(build_target_fqfn, 'w'):
                pass
    return True     # file changed


def add_include_line(build_opt_fqfn, include_fqfn):
    if not os.path.exists(include_fqfn):
        # If file is missing, we need an place holder
        with open(include_fqfn, 'w'):
            pass
        print("add_include_line: Created " + include_fqfn)
    with open(build_opt_fqfn, 'a') as build_opt:
        build_opt.write('-include "' + include_fqfn.replace('\\', '\\\\') + '"\n')


def extract_create_build_opt_file(globals_h_fqfn, file_name, build_opt_fqfn):
    """
    Extract the embedded build.opt from SketchName.ino.globals.h into build
    path/core/build.opt. The subdirectory path must already exist as well as the
    copy of SketchName.ino.globals.h.
    """
    global build_opt_signature

    build_opt = open(build_opt_fqfn, 'w')
    if not os.path.exists(globals_h_fqfn) or (0 == os.path.getsize(globals_h_fqfn)):
        build_opt.close()
        return

    complete_comment = False
    build_opt_error = False
    line_no = 0
    # If the source sketch did not have the file SketchName.ino.globals.h, an empty
    # file was created in the ./core/ folder.
    # By using the copy, open will always succeed.
    with open(globals_h_fqfn, 'r') as src:
        for line in src:
            line = line.strip()
            line_no += 1
            if line == build_opt_signature:
                if complete_comment:
                    build_opt_error = True
                    print_err("  Multiple embedded build.opt blocks in " + file_name + ":" + str(line_no))
                    continue
                print_msg("Extracting embedded compiler command-line options from " +  file_name + ":" + str(line_no))
                for line in src:
                    line = line.strip()
                    line_no += 1
                    if 0 == len(line):
                        continue
                    if line.startswith("*/"):
                        complete_comment = True
                        break
                    elif line.startswith("*"):      # these are so common - skip these should they occur
                        continue
                    elif line.startswith("#"):      # allow some embedded comments
                        continue
                    elif line.startswith("//"):
                        continue
                    # some consistency checking before writing - give some hints about what is wrong
                    elif line == build_opt_signature:
                        print_err("  Double begin before end for embedded build.opt block in " + file_name + ":" + str(line_no))
                        build_opt_error = True
                    elif line.startswith(build_opt_signature):
                        print_err("  build.opt signature block ignored, trailing character for embedded build.opt block in " + file_name + ":" + str(line_no))
                        build_opt_error = True
                    elif "/*" in line or "*/" in line :
                        print_err("  Nesting issue for embedded build.opt block in " + file_name + ":" + str(line_no))
                        build_opt_error = True
                    else:
                        build_opt.write(line + "\n")
            elif line.startswith(build_opt_signature):
                print_err("  build.opt signature block ignored, trailing character for embedded build.opt block in " + file_name + ":" + str(line_no))
                build_opt_error = True
    if not complete_comment or build_opt_error:
        build_opt.truncate(0)
        build_opt.close()
        if build_opt_error:
            # this will help the script start over when the issue is fixed
            os.remove(globals_h_fqfn)
            print_err("  Extraction failed")
            # Don't let the failure get hidden by a spew of nonsensical error
            # messages that will follow. Bring things to a halt.
            sys.exit(1)
            return False
    elif complete_comment:
        print_msg("Created compiler command-line options file " + build_opt_fqfn)
    build_opt.close()
    return complete_comment


def enable_override(enable, commonhfile_fqfn):
    with open(commonhfile_fqfn, 'w') as file:
        if enable:
            file.write("//Override aggressive caching\n")
    # enabled when getsize(commonhfile_fqfn) is non-zero, disabled when zero


def find_preferences_txt():
    platform_name = platform.system()
    # OS Path list from:
    #   https://www.arduino.cc/en/hacking/preferences
    if "Linux" == platform_name:
        # Test for portable 1ST
        # <Arduino IDE installation folder>/portable/preferences.txt (when used in portable mode)
        # For more on portable mode see https://docs.arduino.cc/software/ide-v1/tutorials/PortableIDE
        # Working directory must be set to the location of the Arduino IDE executable.
        fqfn = "./portable/preferences.txt" # Linux portable - verified
        if os.path.exists(fqfn):
            return fqfn
        fqfn = os.path.expanduser("~/.arduino15/preferences.txt") # Linux - verified
        if os.path.exists(fqfn):
            return fqfn
    elif "Windows" == platform_name:
        fqfn = ".\portable\preferences.txt"
        if os.path.exists(fqfn):
            return fqfn
        fqfn = os.path.expanduser("~\Documents\ArduinoData\preferences.txt") # Windows app version - verified
        if os.path.exists(fqfn):
            return fqfn
        fqfn = os.path.expanduser("~\Arduino15\preferences.txt") # Windows
        if os.path.exists(fqfn):
            return fqfn
    elif "Darwin" == platform_name:
        # Skip portable on Macs. Portable is not compatable with Macs
        # see https://docs.arduino.cc/software/ide-v1/tutorials/PortableIDE
        fqfn = os.path.expanduser("~/Library/Arduino15/preferences.txt") # Max OS X
        if os.path.exists(fqfn):
            return fqfn

    print_err("File preferences.txt not found on " + platform_name)
    return ""


def get_preferences_txt(file_fqfn, key):
    with open(file_fqfn) as file:
        for line in file:
            name, value = line.partition("=")[::2]
            if name.strip().lower() == key:
                if value.strip().lower() == 'true':
                    return True
                else:
                    return False
    print_err("Key " + key + " not found in preferences.txt. Default to true.")
    return True     # If we don't find it just assume it is set True


def check_preferences_txt():
    file_fqfn = find_preferences_txt()
    if file_fqfn == "":
        return True     # cannot find file assume enabled
    print_msg("Using preferences from " + file_fqfn)
    return get_preferences_txt(file_fqfn, "compiler.cache_core")


def touch(fname, times=None):
    with open(fname, 'a'):
        os.utime(fname, times)


def synchronous_touch(globals_h_fqfn, commonhfile_fqfn):
    with open(globals_h_fqfn, 'a'):
        os.utime(globals_h_fqfn)
        ts = os.stat(globals_h_fqfn)
        with open(commonhfile_fqfn, 'a'):
            os.utime(commonhfile_fqfn, ns=(ts.st_atime_ns, ts.st_mtime_ns))


def main():
    global build_opt_signature
    global docs_url
    num_include_lines = 1
    use_aggressive_caching_workaround = check_preferences_txt()

    if len(sys.argv) >= 5:
        source_globals_h_fqfn = os.path.normpath(sys.argv[1])
        globals_name = os.path.basename(source_globals_h_fqfn)
        globals_h_fqfn = os.path.normpath(sys.argv[2])
        build_path = os.path.dirname(globals_h_fqfn)
        # Assumption: globals_h_fqfn and build_opt_fqfn have the same dirname
        build_opt_fqfn = os.path.normpath(sys.argv[3])
        commonhfile_fqfn = os.path.normpath(sys.argv[4])

        if os.path.exists(commonhfile_fqfn):
            if os.path.getsize(commonhfile_fqfn) and \
            not use_aggressive_caching_workaround:
                enable_override(False, commonhfile_fqfn)
        else:
            enable_override(False, commonhfile_fqfn)

        if os.path.exists(globals_h_fqfn):
            # Check for signs of "Aggressive Caching core.a"
            # 1ST time run, build path/core will not exist or be nearly empty,
            # nothing can be learned. The presence of globals_h_fqfn in the
            # build path/core helps distinguish 1st time run from rebuild.
            # This method does not report in all scenarios; however, it does
            # report often enough to draw attention to the issue. Some aborted
            # builds with incomplete ./core compiles may later produce false
            # positives. Only report when globals.h is being used.
            if not use_aggressive_caching_workaround and \
            os.path.getsize(globals_h_fqfn) and \
            len(os.listdir(build_path)) < 20:
                print_err("Aggressive caching of core.a might be enabled. This may create build errors.")
                print_err("  Suggest turning off in preferences.txt: \"compiler.cache_core=false\"")
                print_err("  Read more at " + docs_url)
        else:
            # Info: When platform.txt, platform.local.txt, or IDE Tools board
            # settings are changed, our build path directory was cleaned. Note,
            # makecorever.py may have run before us and recreaded the directory.
            if not os.path.exists(build_path):
                os.makedirs(build_path)
                print_msg("Clean build, created dir " + build_path)

        if os.path.exists(source_globals_h_fqfn):
            print_msg("Using global defines from " + source_globals_h_fqfn)

        copy_create_build_file(source_globals_h_fqfn, globals_h_fqfn)

        # globals_h_fqfn timestamp was only updated if the source changed. This
        # controls the rebuild on change. We can always extract a new build.opt
        # w/o triggering a needless rebuild.
        embedded_options = extract_create_build_opt_file(globals_h_fqfn, globals_name, build_opt_fqfn)

        if use_aggressive_caching_workaround:
            # When a Sketch owns a "Sketch.ino.globals.h" file in the build tree
            # that exactly matches the timestamp of "CommonHFile.h" in the
            # platform source tree, it owns the core cache. If not, or
            # "Sketch.ino.globals.h" has changed, rebuild core.
            if os.path.getsize(commonhfile_fqfn):
                if (os.path.getmtime(globals_h_fqfn) != os.path.getmtime(commonhfile_fqfn)):
                    # Need to rebuild core.a
                    # touching commonhfile_fqfn in the source core tree will cause rebuild.
                    # Looks like touching or writing unrelated files in the source core tree will cause rebuild.
                    synchronous_touch(globals_h_fqfn, commonhfile_fqfn)
                    print_msg("Using 'aggressive caching' workaround.")
            elif os.path.getsize(globals_h_fqfn):
                enable_override(True, commonhfile_fqfn)
                synchronous_touch(globals_h_fqfn, commonhfile_fqfn)
                print_msg("Using 'aggressive caching' workaround.")

        add_include_line(build_opt_fqfn, commonhfile_fqfn)
        add_include_line(build_opt_fqfn, globals_h_fqfn)

        # Provide context help for build option support.
        source_build_opt_h_fqfn = os.path.join(os.path.dirname(source_globals_h_fqfn), "build_opt.h")
        if os.path.exists(source_build_opt_h_fqfn) and not embedded_options:
            print_err("Build options file '" + source_build_opt_h_fqfn + "' not supported.")
            print_err("  Add build option content to '" + source_globals_h_fqfn + "'.")
            print_err("  Embedd compiler command-line options in a block comment starting with '" + build_opt_signature + "'.")
            print_err("  Read more at " + docs_url)
        elif os.path.exists(source_globals_h_fqfn):
            if not embedded_options:
                print_msg("Tip: Embedd compiler command-line options in a block comment starting with '" + build_opt_signature + "'.")
                print_msg("  Read more at " + docs_url)

    else:
        print_err("Too few arguments. Add arguments:")
        print_err("  Source FQFN SketchName.ino.globals.h, Build FQFN SketchName.ino.globals.h, Build FQFN build.opt")

if __name__ == '__main__':
    sys.exit(main())
