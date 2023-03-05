#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# This script manages the use of a file with a unique name, like
# `Sketch.ino.globals.h`, in the Sketch source directory to provide compiler
# command-line options (build options) and sketch global macros. The build
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

"Sketch.ino.globals.h" - A global h file in the Source Sketch directory. The
string Sketch.ino is the actual name of the sketch program. A matching copy is
kept in the build path/core directory. The file is empty when it does not exist
in the source directory.

Using Sketch.ino.globals.h as a container to hold build.opt, gives implicit
dependency tracking for build.opt by way of Sketch.ino.globals.h's
dependencies.
Example:
  gcc ... @{build.path}/core/build.opt -include "{build.path}/core/{build.project_name}.globals.h" ...

In this implementation the '-include "{build.path}/core/{build.project_name}.globals.h"'
component is added to the build.opt file.
  gcc ... @{build.path}/core/build.opt ...

At each build cycle, "{build.project_name}.globals.h" is conditoinally copied to
"{build.path}/core/" at prebuild, and build.opt is extraction as needed. The
Sketch.ino.globals.h's dependencies will trigger "rebuild all" as needed.

If Sketch.ino.globals.h is not in the source sketch folder, an empty
versions is created in the build tree. The file build.opt always contains a
"-include ..." entry so that file dependencies are generated for
Sketch.ino.globals.h. This allows for change detection when the file is
added.
"""

"""
Arduino `preferences.txt` changes

"Aggressively cache compiled core" ideally should be turned off; however,
a workaround has been implimented.
In ~/.arduino15/preferences.txt, to disable the feature:
  compiler.cache_core=false

Reference:
https://forum.arduino.cc/t/no-aggressively-cache-compiled-core-in-ide-1-8-15/878954/2
"""

"""
# Updates or Additions for platform.txt or platform.local.txt

runtime.tools.mkbuildoptglobals={runtime.platform.path}/tools/mkbuildoptglobals.py

# Fully qualified file names for processing sketch global options
globals.h.source.fqfn={build.source.path}/{build.project_name}.globals.h
commonhfile.fqfn={build.core.path}/CommonHFile.h
build.opt.fqfn={build.path}/core/build.opt
mkbuildoptglobals.extra_flags=

recipe.hooks.prebuild.2.pattern="{runtime.tools.python3.path}/python3" -I "{runtime.tools.mkbuildoptglobals}" "{runtime.ide.path}" {runtime.ide.version} "{build.path}" "{build.opt.fqfn}" "{globals.h.source.fqfn}" "{commonhfile.fqfn}" {mkbuildoptglobals.extra_flags}

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
Sketch.ino.globals.h. You need to touch (update time stampt) the file so a
rebuild all is performed.

3) During the build two identical copies of Sketch.ino.globals.h will exist.
#ifndef fencing will be needed for non comment blocks in Sketch.ino.globals.h.

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

import argparse
from shutil import copyfile
import glob
import os
import platform
import traceback
import sys
import textwrap
import time

import locale

# Need to work on signature line used for match to avoid conflicts with
# existing embedded documentation methods.
build_opt_signature = "/*@create-file:build.opt@"

docs_url = "https://arduino-esp8266.readthedocs.io/en/latest/faq/a06-global-build-options.html"


err_print_flag = False
msg_print_buf = ""
debug_enabled = False
default_encoding = None

# Issues trying to address through buffered printing
# 1. Arduino IDE 2.0 RC5 does not show stderr text in color. Text printed does
#    not stand out from stdout messages.
# 2. Separate pipes, buffering, and multiple threads with output can create
#    mixed-up messages. "flush" helped but did not resolve. The Arduino IDE 2.0
#    somehow makes the problem worse.
# 3. With Arduino IDE preferences set for "no verbose output", you only see
#    stderr messages. Prior related prints are missing.
#
# Locally buffer and merge both stdout and stderr prints. This allows us to
# print a complete context when there is an error. When any buffered prints
# are targeted to stderr, print the whole buffer to stderr.

def print_msg(*args, **kwargs):
    global msg_print_buf
    if 'sep' in kwargs:
        sep = kwargs['sep']
    else:
        sep = ' '

    msg_print_buf += args[0]
    for arg in args[1:]:
        msg_print_buf += sep
        msg_print_buf += arg

    if 'end' in kwargs:
        msg_print_buf += kwargs['end']
    else:
        msg_print_buf += '\n'


# Bring attention to errors with a blank line and lines starting with "*** ".
def print_err(*args, **kwargs):
    global err_print_flag
    if (args[0])[0] != ' ':
        print_msg("")
    print_msg("***", *args, **kwargs)
    err_print_flag = True

def print_dbg(*args, **kwargs):
    global debug_enabled
    global err_print_flag
    if debug_enabled:
        print_msg("DEBUG:", *args, **kwargs)
        err_print_flag = True


def handle_error(err_no):
    # on err_no 0, commit print buffer to stderr or stdout
    # on err_no != 0, commit print buffer to stderr and sys exist with err_no
    global msg_print_buf
    global err_print_flag
    if len(msg_print_buf):
        if err_no or err_print_flag:
            fd = sys.stderr
        else:
            fd = sys.stdout
        print(msg_print_buf, file=fd, end='', flush=True)
        msg_print_buf = ""
        err_print_flag = False
    if err_no:
        sys.exit(err_no)


def copy_create_build_file(source_fqfn, build_target_fqfn):
    """
    Conditionally copy a newer file between the source directory and the build
    directory. When source file is missing, create an empty file in the build
    directory.
    return     True when file change detected.
    """
    if os.path.exists(source_fqfn):
        if os.path.exists(build_target_fqfn) and \
        os.path.getmtime(build_target_fqfn) >= os.path.getmtime(source_fqfn):
            # only copy newer files - do nothing, all is good
            print_dbg(f"up to date os.path.exists({source_fqfn}) ")
            return False
        else:
            # The new copy gets stamped with the current time, just as other
            # files copied by `arduino-builder`.
            copyfile(source_fqfn, build_target_fqfn)
            print_dbg(f"copyfile({source_fqfn}, {build_target_fqfn})")
    else:
        if os.path.exists(build_target_fqfn) and \
        os.path.getsize(build_target_fqfn) == 0:
            return False
        else:
            # Place holder - Must have an empty file to satisfy parameter list
            # specifications in platform.txt.
            with open(build_target_fqfn, 'w', encoding="utf-8"):
                pass
    return True     # file changed

def add_include_line(build_opt_fqfn, include_fqfn):
    global default_encoding
    if not os.path.exists(include_fqfn):
        # If file is missing, we need an place holder
        with open(include_fqfn, 'w', encoding=default_encoding):
            pass
        print_msg("add_include_line: Created " + include_fqfn)

    with open(build_opt_fqfn, 'a', encoding=default_encoding) as build_opt:
        build_opt.write('-include "' + include_fqfn.replace('\\', '\\\\') + '"\n')

def extract_create_build_opt_file(globals_h_fqfn, file_name, build_opt_fqfn):
    """
    Extract the embedded build.opt from Sketch.ino.globals.h into build
    path/core/build.opt. The subdirectory path must already exist as well as the
    copy of Sketch.ino.globals.h.
    """
    global build_opt_signature
    global default_encoding

    build_opt = open(build_opt_fqfn, 'w', encoding=default_encoding)
    if not os.path.exists(globals_h_fqfn) or (0 == os.path.getsize(globals_h_fqfn)):
        build_opt.close()
        return False

    complete_comment = False
    build_opt_error = False
    line_no = 0
    # If the source sketch did not have the file Sketch.ino.globals.h, an empty
    # file was created in the ./core/ folder.
    # By using the copy, open will always succeed.
    with open(globals_h_fqfn, 'r', encoding="utf-8") as src:
        for line in src:
            line = line.strip()
            line_no += 1
            if line == build_opt_signature:
                if complete_comment:
                    build_opt_error = True
                    print_err("  Multiple embedded build.opt blocks in", f'{file_name}:{line_no}')
                    continue
                print_msg("Extracting embedded compiler command-line options from", f'{file_name}:{line_no}')
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
                        print_err("  Double begin before end for embedded build.opt block in", f'{file_name}:{line_no}')
                        build_opt_error = True
                    elif line.startswith(build_opt_signature):
                        print_err("  build.opt signature block ignored, trailing character for embedded build.opt block in", f'{file_name}:{line_no}')
                        build_opt_error = True
                    elif "/*" in line or "*/" in line :
                        print_err("  Nesting issue for embedded build.opt block in", f'{file_name}:{line_no}')
                        build_opt_error = True
                    else:
                        print_msg("  ", f'{line_no:2}, Add command-line option: {line}', sep='')
                        build_opt.write(line + "\n")
            elif line.startswith(build_opt_signature):
                print_err("  build.opt signature block ignored, trailing character for embedded build.opt block in", f'{file_name}:{line_no}')
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
            handle_error(1)
            return False    # not reached
    elif complete_comment:
        print_msg("  Created compiler command-line options file " + build_opt_fqfn)
    build_opt.close()
    return complete_comment


def enable_override(enable, commonhfile_fqfn):
    # Reduce disk IO writes
    if os.path.exists(commonhfile_fqfn):
        if os.path.getsize(commonhfile_fqfn): # workaround active
            if enable:
                return
        elif not enable:
            return
    with open(commonhfile_fqfn, 'w', encoding="utf-8") as file:
        if enable:
            file.write("//Override aggressive caching\n")
    # enable workaround when getsize(commonhfile_fqfn) is non-zero, disabled when zero


def discover_1st_time_run(build_path):
    # Need to know if this is the 1ST compile of the Arduino IDE starting.
    # Use empty cache directory as an indicator for 1ST compile.
    # Arduino IDE 2.0 RC5 does not cleanup on exist like 1.6.19. Probably for
    # debugging like the irregular version number 10607. For RC5 this indicator
    # will be true after a reboot instead of a 1ST compile of the IDE starting.
    # Another issue for this technique, Windows does not clear the Temp directory. :(
    tmp_path, build = os.path.split(build_path)
    ide_2_0 = 'arduino-sketch-'
    if ide_2_0 == build[:len(ide_2_0)]:
        search_path =  os.path.join(tmp_path, 'arduino-core-cache/*') # Arduino IDE 2.0
    else:
        search_path = os.path.join(tmp_path, 'arduino_cache_*/*') # Arduino IDE 1.6.x and up

    count = 0
    for dirname in glob.glob(search_path):
        count += 1
    return 0 == count


def get_preferences_txt(file_fqfn, key):
    # Get Key Value, key is allowed to be missing.
    # We assume file file_fqfn exists
    basename = os.path.basename(file_fqfn)
    with open(file_fqfn, encoding="utf-8") as file:
        for line in file:
            name, value = line.partition("=")[::2]
            if name.strip().lower() == key:
                val = value.strip().lower()
                if val != 'true':
                    val = False
                print_msg(f"  {basename}: {key}={val}")
                return val
    print_err(f"  Key '{key}' not found in file {basename}. Default to true.")
    return True     # If we don't find it just assume it is set True


def check_preferences_txt(runtime_ide_path, preferences_file):
    key = "compiler.cache_core"
    # return the state of "compiler.cache_core" found in preferences.txt
    if preferences_file != None:
        if os.path.exists(preferences_file):
            print_msg(f"Using preferences from '{preferences_file}'")
            return get_preferences_txt(preferences_file, key)
        else:
            print_err(f"Override preferences file '{preferences_file}' not found.")

    # Referencing the preferences.txt for an indication of shared "core.a"
    # caching is unreliable. There are too many places reference.txt can be
    # stored and no hints of which the Arduino build might be using. Unless
    # directed otherwise, assume "core.a" caching true.
    print_msg(f"Assume aggressive 'core.a' caching enabled.")
    return True

def touch(fname, times=None):
    with open(fname, "ab") as file:
        file.close();
    os.utime(fname, times)

def synchronous_touch(globals_h_fqfn, commonhfile_fqfn):
    global debug_enabled
    # touch both files with the same timestamp
    touch(globals_h_fqfn)
    with open(globals_h_fqfn, "rb") as file:
        file.close()
    with open(commonhfile_fqfn, "ab") as file2:
        file2.close()
    ts = os.stat(globals_h_fqfn)
    os.utime(commonhfile_fqfn, ns=(ts.st_atime_ns, ts.st_mtime_ns))

    if debug_enabled:
        print_dbg("After synchronous_touch")
        ts = os.stat(globals_h_fqfn)
        print_dbg(f"  globals_h_fqfn ns_stamp   = {ts.st_mtime_ns}")
        print_dbg(f"  getmtime(globals_h_fqfn)    {os.path.getmtime(globals_h_fqfn)}")
        ts = os.stat(commonhfile_fqfn)
        print_dbg(f"  commonhfile_fqfn ns_stamp = {ts.st_mtime_ns}")
        print_dbg(f"  getmtime(commonhfile_fqfn)  {os.path.getmtime(commonhfile_fqfn)}")

def determine_cache_state(args, runtime_ide_path, source_globals_h_fqfn):
    global docs_url
    print_dbg(f"runtime_ide_version: {args.runtime_ide_version}")

    if args.cache_core != None:
        print_msg(f"Preferences override, this prebuild script assumes the 'compiler.cache_core' parameter is set to {args.cache_core}")
        print_msg(f"To change, modify 'mkbuildoptglobals.extra_flags=(--cache_core | --no_cache_core)' in 'platform.local.txt'")
        return args.cache_core
    else:
        ide_path = None
        preferences_fqfn = None
        if args.preferences_sketch != None:
            preferences_fqfn = os.path.join(
                os.path.dirname(source_globals_h_fqfn),
                os.path.normpath(args.preferences_sketch))
        else:
            if args.preferences_file != None:
                preferences_fqfn = args.preferences_file
            elif args.preferences_env != None:
                preferences_fqfn = args.preferences_env
            else:
                ide_path = runtime_ide_path

            if preferences_fqfn != None:
                preferences_fqfn = os.path.normpath(preferences_fqfn)
                root = False
                if 'Windows' == platform.system():
                    if preferences_fqfn[1:2] == ':\\':
                        root = True
                else:
                    if preferences_fqfn[0] == '/':
                        root = True
                if not root:
                    if preferences_fqfn[0] != '~':
                        preferences_fqfn = os.path.join("~", preferences_fqfn)
                    preferences_fqfn = os.path.expanduser(preferences_fqfn)
                print_dbg(f"determine_cache_state: preferences_fqfn: {preferences_fqfn}")

    return check_preferences_txt(ide_path, preferences_fqfn)


"""
TODO

aggressive caching workaround
========== ======= ==========
The question needs to be asked, is it a good idea?
With all this effort to aid in determining the cache state, it is rendered
usless when arduino command line switches are used that contradict our
settings.

Sort out which of these are imperfect solutions should stay in

Possible options for handling problems caused by:
    ./arduino --preferences-file other-preferences.txt
    ./arduino --pref compiler.cache_core=false

--cache_core
--no_cache_core
--preferences_file (relative to IDE or full path)
--preferences_sketch (default looks for preferences.txt or specify path relative to sketch folder)
--preferences_env, python docs say "Availability: most flavors of Unix, Windows."

 export ARDUINO15_PREFERENCES_FILE=$(realpath other-name-than-default-preferences.txt )
 ./arduino --preferences-file other-name-than-default-preferences.txt

 platform.local.txt: mkbuildoptglobals.extra_flags=--preferences_env

 Tested with:
 export ARDUINO15_PREFERENCES_FILE=$(realpath ~/projects/arduino/arduino-1.8.19/portable/preferences.txt)
 ~/projects/arduino/arduino-1.8.18/arduino


 Future Issues
 * "--preferences-file" does not work for Arduino IDE 2.0, they plan to address at a future release
 * Arduino IDE 2.0 does not support portable, they plan to address at a future release

"""


def check_env(env):
    system = platform.system()
    # From the docs:
    #   Availability: most flavors of Unix, Windows.
    #   “Availability: Unix” are supported on macOS
    # Because of the soft commitment, I used "help=argparse.SUPPRESS" to keep
    # the claim out of the help. The unavailable case is untested.
    val = os.getenv(env)
    if val == None:
        if "Linux" == system or "Windows" == system:
            raise argparse.ArgumentTypeError(f'Missing environment variable: {env}')
        else:
            # OS/Library limitation
            raise argparse.ArgumentTypeError('Not supported')
    return val


def parse_args():
    extra_txt = '''\
       Use platform.local.txt 'mkbuildoptglobals.extra_flags=...' to supply override options:
         --cache_core | --no_cache_core | --preferences_file PREFERENCES_FILE | ...

       more help at {}
       '''.format(docs_url)
    parser = argparse.ArgumentParser(
        description='Prebuild processing for globals.h and build.opt file',
        formatter_class=argparse.RawDescriptionHelpFormatter,
              epilog=textwrap.dedent(extra_txt))
    parser.add_argument('runtime_ide_path', help='Runtime IDE path, {runtime.ide.path}')
    parser.add_argument('runtime_ide_version', type=int, help='Runtime IDE Version, {runtime.ide.version}')
    parser.add_argument('build_path', help='Build path, {build.path}')
    parser.add_argument('build_opt_fqfn', help="Build FQFN to build.opt")
    parser.add_argument('source_globals_h_fqfn', help="Source FQFN Sketch.ino.globals.h")
    parser.add_argument('commonhfile_fqfn', help="Core Source FQFN CommonHFile.h")
    parser.add_argument('--debug', action='store_true', required=False, default=False)
    parser.add_argument('-DDEBUG_ESP_PORT', nargs='?', action='store', const="", default="", help='Add mkbuildoptglobals.extra_flags={build.debug_port} to platform.local.txt')
    parser.add_argument('--ci', action='store_true', required=False, default=False)
    group = parser.add_mutually_exclusive_group(required=False)
    group.add_argument('--cache_core', action='store_true', default=None, help='Assume a "compiler.cache_core" value of true')
    group.add_argument('--no_cache_core', dest='cache_core', action='store_false', help='Assume a "compiler.cache_core" value of false')
    group.add_argument('--preferences_file', help='Full path to preferences file')
    group.add_argument('--preferences_sketch', nargs='?', action='store', const="preferences.txt", help='Sketch relative path to preferences file')
    # Since the docs say most versions of Windows and Linux support the os.getenv method, suppress the help message.
    group.add_argument('--preferences_env', nargs='?', action='store', type=check_env, const="ARDUINO15_PREFERENCES_FILE", help=argparse.SUPPRESS)
    # ..., help='Use environment variable for path to preferences file')
    return parser.parse_args()
    # ref epilog, https://stackoverflow.com/a/50021771
    # ref nargs='*'', https://stackoverflow.com/a/4480202
    # ref no '--n' parameter, https://stackoverflow.com/a/21998252


# retrieve *system* encoding, not the one used by python internally
if sys.version_info >= (3, 11):
    def get_encoding():
        return locale.getencoding()
else:
    def get_encoding():
        return locale.getdefaultlocale()[1]


def show_value(desc, value):
    print_dbg(f'{desc:<40} {value}')
    return

def locale_dbg():
    show_value("get_encoding()", get_encoding())
    show_value("locale.getdefaultlocale()", locale.getdefaultlocale())
    show_value('sys.getfilesystemencoding()', sys.getfilesystemencoding())
    show_value("sys.getdefaultencoding()", sys.getdefaultencoding())
    show_value("locale.getpreferredencoding(False)", locale.getpreferredencoding(False))
    try:
        show_value("locale.getpreferredencoding()", locale.getpreferredencoding())
    except:
        pass
    show_value("sys.stdout.encoding", sys.stdout.encoding)

    # use current setting
    show_value("locale.setlocale(locale.LC_ALL, None)", locale.setlocale(locale.LC_ALL, None))
    try:
        show_value("locale.getencoding()", locale.getencoding())
    except:
        pass
    show_value("locale.getlocale()", locale.getlocale())

    # use user setting
    show_value("locale.setlocale(locale.LC_ALL, '')", locale.setlocale(locale.LC_ALL, ''))
    # show_value("locale.getencoding()", locale.getencoding())
    show_value("locale.getlocale()", locale.getlocale())
    return


def main():
    global build_opt_signature
    global docs_url
    global debug_enabled
    global default_encoding
    num_include_lines = 1

    # Given that GCC will handle lines from an @file as if they were on
    # the command line. I assume that the contents of @file need to be encoded
    # to match that of the shell running GCC runs. I am not 100% sure this API
    # gives me that, but it appears to work.
    #
    # However, elsewhere when dealing with source code we continue to use 'utf-8',
    # ref. https://gcc.gnu.org/onlinedocs/cpp/Character-sets.html
    default_encoding = get_encoding()

    args = parse_args()
    debug_enabled = args.debug
    runtime_ide_path = os.path.normpath(args.runtime_ide_path)
    build_path = os.path.normpath(args.build_path)
    build_opt_fqfn = os.path.normpath(args.build_opt_fqfn)
    source_globals_h_fqfn = os.path.normpath(args.source_globals_h_fqfn)
    commonhfile_fqfn = os.path.normpath(args.commonhfile_fqfn)

    globals_name = os.path.basename(source_globals_h_fqfn)
    build_path_core, build_opt_name = os.path.split(build_opt_fqfn)
    globals_h_fqfn = os.path.join(build_path_core, globals_name)

    if debug_enabled:
        locale_dbg()

    default_locale = locale.getdefaultlocale()
    print_msg(f'default locale:         {default_locale}')
    print_msg(f'default_encoding:       {default_encoding}')

    print_dbg(f"runtime_ide_path:       {runtime_ide_path}")
    print_dbg(f"runtime_ide_version:    {args.runtime_ide_version}")
    print_dbg(f"build_path:             {build_path}")
    print_dbg(f"build_opt_fqfn:         {build_opt_fqfn}")
    print_dbg(f"source_globals_h_fqfn:  {source_globals_h_fqfn}")
    print_dbg(f"commonhfile_fqfn:       {commonhfile_fqfn}")
    print_dbg(f"globals_name:           {globals_name}")
    print_dbg(f"build_path_core:        {build_path_core}")
    print_dbg(f"globals_h_fqfn:         {globals_h_fqfn}")
    print_dbg(f"DDEBUG_ESP_PORT:        {args.DDEBUG_ESP_PORT}")

    if len(args.DDEBUG_ESP_PORT):
        build_opt_signature = build_opt_signature[:-1] + ":debug@"

    print_dbg(f"build_opt_signature:    {build_opt_signature}")

    if args.ci:
        # Requires CommonHFile.h to never be checked in.
        if os.path.exists(commonhfile_fqfn):
            first_time = False
        else:
            first_time = True
    else:
        first_time = discover_1st_time_run(build_path)
        if first_time:
            print_dbg("First run since Arduino IDE started.")

    use_aggressive_caching_workaround = determine_cache_state(args, runtime_ide_path, source_globals_h_fqfn)

    print_dbg(f"first_time:             {first_time}")
    print_dbg(f"use_aggressive_caching_workaround: {use_aggressive_caching_workaround}")

    if not os.path.exists(build_path_core):
        os.makedirs(build_path_core)
        print_msg("Clean build, created dir " + build_path_core)

    if first_time or \
    not use_aggressive_caching_workaround or \
    not os.path.exists(commonhfile_fqfn):
        enable_override(False, commonhfile_fqfn)

    # A future timestamp on commonhfile_fqfn will cause everything to
    # rebuild. This occurred during development and may happen after
    # changing the system time.
    if time.time_ns() < os.stat(commonhfile_fqfn).st_mtime_ns:
        touch(commonhfile_fqfn)
        print_err(f"Neutralized future timestamp on build file: {commonhfile_fqfn}")

    if os.path.exists(source_globals_h_fqfn):
        print_msg("Using global include from " + source_globals_h_fqfn)

    copy_create_build_file(source_globals_h_fqfn, globals_h_fqfn)

    # globals_h_fqfn timestamp was only updated if the source changed. This
    # controls the rebuild on change. We can always extract a new build.opt
    # w/o triggering a needless rebuild.
    embedded_options = extract_create_build_opt_file(globals_h_fqfn, globals_name, build_opt_fqfn)

    if use_aggressive_caching_workaround:
        # commonhfile_fqfn encodes the following information
        # 1. When touched, it causes a rebuild of core.a
        # 2. When file size is non-zero, it indicates we are using the
        #    aggressive cache workaround. The workaround is set to true
        #    (active) when we discover a non-zero length global .h file in
        #    any sketch. The aggressive workaround is cleared on the 1ST
        #    compile by the Arduino IDE after starting.
        # 3. When the timestamp matches the build copy of globals.h
        #    (globals_h_fqfn), we know one two things:
        #    * The cached core.a matches up to the current build.opt and
        #      globals.h. The current sketch owns the cached copy of core.a.
        #    * globals.h has not changed, and no need to rebuild core.a
        # 4. When core.a's timestamp does not match the build copy of
        #    the global .h file, we only know we need to rebuild core.a, and
        #    that is enough.
        #
        # When the sketch build has a "Sketch.ino.globals.h" file in the
        # build tree that exactly matches the timestamp of "CommonHFile.h"
        # in the platform source tree, it owns the core.a cache copy. If
        # not, or "Sketch.ino.globals.h" has changed, rebuild core.
        # A non-zero file size for commonhfile_fqfn, means we have seen a
        # globals.h file before and workaround is active.
        if debug_enabled:
            print_dbg("Timestamps at start of check aggressive caching workaround")
            ts = os.stat(globals_h_fqfn)
            print_dbg(f"  globals_h_fqfn ns_stamp   = {ts.st_mtime_ns}")
            print_dbg(f"  getmtime(globals_h_fqfn)    {os.path.getmtime(globals_h_fqfn)}")
            ts = os.stat(commonhfile_fqfn)
            print_dbg(f"  commonhfile_fqfn ns_stamp = {ts.st_mtime_ns}")
            print_dbg(f"  getmtime(commonhfile_fqfn)  {os.path.getmtime(commonhfile_fqfn)}")

        if os.path.getsize(commonhfile_fqfn):
            if (os.path.getmtime(globals_h_fqfn) != os.path.getmtime(commonhfile_fqfn)):
                # Need to rebuild core.a
                # touching commonhfile_fqfn in the source core tree will cause rebuild.
                # Looks like touching or writing unrelated files in the source core tree will cause rebuild.
                synchronous_touch(globals_h_fqfn, commonhfile_fqfn)
                print_msg("Using 'aggressive caching' workaround, rebuild shared 'core.a' for current globals.")
            else:
                print_dbg(f"Using old cached 'core.a'")
        elif os.path.getsize(globals_h_fqfn):
            enable_override(True, commonhfile_fqfn)
            synchronous_touch(globals_h_fqfn, commonhfile_fqfn)
            print_msg("Using 'aggressive caching' workaround, rebuild shared 'core.a' for current globals.")
        else:
            print_dbg(f"Workaround not active/needed")

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
        print_msg("Note: optional global include file '" + source_globals_h_fqfn + "' does not exist.")
        print_msg("  Read more at " + docs_url)

    handle_error(0)   # commit print buffer

if __name__ == '__main__':
    rc = 1
    try:
        rc = main()
    except:
        print_err(traceback.format_exc())
        handle_error(0)
    sys.exit(rc)
