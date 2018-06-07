# Copyright (c) 2014-present PlatformIO <contact@platformio.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Arduino

Arduino Wiring-based Framework allows writing cross-platform software to
control devices attached to a wide range of Arduino boards to create all
kinds of creative coding, interactive objects, spaces or physical experiences.

http://arduino.cc/en/Reference/HomePage
"""

# Extends: https://github.com/platformio/platform-espressif8266/blob/develop/builder/main.py

from os.path import isdir, join

from SCons import Builder, Util
from SCons.Script import DefaultEnvironment


def scons_patched_match_splitext(path, suffixes=None):
    """
    Patch SCons Builder, append $OBJSUFFIX to the end of each target
    """
    tokens = Util.splitext(path)
    if suffixes and tokens[1] and tokens[1] in suffixes:
        return (path, tokens[1])
    return tokens


Builder.match_splitext = scons_patched_match_splitext


env = DefaultEnvironment()
platform = env.PioPlatform()

FRAMEWORK_DIR = platform.get_package_dir("framework-arduinoespressif8266")
assert isdir(FRAMEWORK_DIR)


env.Append(
    CCFLAGS=[
        "-Wall"
    ],

    CPPDEFINES=[
        ("ARDUINO", 10805),
        ("ARDUINO_BOARD", '\\"PLATFORMIO_%s\\"'
            % env.BoardConfig().id.upper()),
        "LWIP_OPEN_SRC"
    ],

    CPPPATH=[
        join(FRAMEWORK_DIR, "tools", "sdk", "include"),
        join(FRAMEWORK_DIR, "tools", "sdk", "libc",
             "xtensa-lx106-elf", "include"),
        join(FRAMEWORK_DIR, "cores", env.BoardConfig().get("build.core"))
    ],

    LIBPATH=[
        join("$BUILD_DIR", "ld"),  # eagle.app.v6.common.ld
        join(FRAMEWORK_DIR, "tools", "sdk", "lib"),
        join(FRAMEWORK_DIR, "tools", "sdk", "ld"),
        join(FRAMEWORK_DIR, "tools", "sdk", "libc", "xtensa-lx106-elf", "lib")
    ],

    LIBS=[
        "hal", "phy", "pp", "net80211", "wpa", "crypto", "main",
        "wps", "bearssl", "axtls", "espnow", "smartconfig", "airkiss", "wpa2",
        "stdc++", "m", "c", "gcc"
    ],

    LIBSOURCE_DIRS=[
        join(FRAMEWORK_DIR, "libraries")
    ],

    LINKFLAGS=[
        "-Wl,-wrap,system_restart_local",
        "-Wl,-wrap,spi_flash_read",
        "-u", "app_entry"
    ]
)

# remove LINKFLAGS defined in main.py and keep user custom flags
try:
    index = env['LINKFLAGS'].index("call_user_start")
    if index > 0 and env['LINKFLAGS'][index - 1] == "-u":
        del env['LINKFLAGS'][index - 1]
        env['LINKFLAGS'].remove("call_user_start")
except IndexError:
    pass

flatten_cppdefines = env.Flatten(env['CPPDEFINES'])

#
# lwIP
#
if "PIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 536)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
        LIBS=["lwip2"]
    )
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_HIGHER_BANDWIDTH" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 1460)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
        LIBS=["lwip2_1460"]
    )
else:
    env.Append(
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip", "include")],
        LIBS=["lwip_gcc"]
    )

#
# VTables
#

current_vtables = None
for d in flatten_cppdefines:
    if str(d).startswith("VTABLES_IN_"):
        current_vtables = d
if not current_vtables:
    current_vtables = "VTABLES_IN_FLASH"
    env.Append(CPPDEFINES=[current_vtables])
assert current_vtables

# Build the eagle.app.v6.common.ld linker file
app_ld = env.Command(
    join("$BUILD_DIR", "ld", "eagle.app.v6.common.ld"),
    join(FRAMEWORK_DIR, "tools", "sdk", "ld", "eagle.app.v6.common.ld.h"),
    env.VerboseAction(
        "$CC -CC -E -P -D%s $SOURCE -o $TARGET" % current_vtables,
        "Generating LD script $TARGET"))
env.Depends("$BUILD_DIR/$PROGNAME$PROGSUFFIX", app_ld)


#
# Target: Build Core Library
#

libs = []

if "build.variant" in env.BoardConfig():
    env.Append(
        CPPPATH=[
            join(FRAMEWORK_DIR, "variants",
                 env.BoardConfig().get("build.variant"))
        ]
    )
    libs.append(env.BuildLibrary(
        join("$BUILD_DIR", "FrameworkArduinoVariant"),
        join(FRAMEWORK_DIR, "variants", env.BoardConfig().get("build.variant"))
    ))

libs.append(env.BuildLibrary(
    join("$BUILD_DIR", "FrameworkArduino"),
    join(FRAMEWORK_DIR, "cores", env.BoardConfig().get("build.core"))
))

env.Prepend(LIBS=libs)
