# Copyright (c) 2014-present PlatformIO <contact@platformio.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
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

https://arduino.cc/en/Reference/HomePage
"""

# Extends: https://github.com/platformio/platform-espressif8266/blob/develop/builder/main.py

from os.path import isdir, join

from SCons import Util
from SCons.Script import Builder, DefaultEnvironment


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
    ASFLAGS=["-x", "assembler-with-cpp"],

    CFLAGS=[
        "-std=c17",
        "-Wpointer-arith",
        "-Wno-implicit-function-declaration",
        "-Wl,-EL",
        "-fno-inline-functions",
        "-nostdlib"
    ],

    CCFLAGS=[
        "-Os",  # optimize for size
        "-mlongcalls",
        "-mtext-section-literals",
        "-falign-functions=4",
        "-U__STRICT_ANSI__",
        "-ffunction-sections",
        "-fdata-sections",
        "-fno-exceptions",
        "-Wall"
    ],

    CXXFLAGS=[
        "-fno-rtti",
        "-std=gnu++17"
    ],

    LINKFLAGS=[
        "-Os",
        "-nostdlib",
        "-Wl,--no-check-sections",
        "-Wl,-static",
        "-Wl,--gc-sections",
        "-Wl,-wrap,system_restart_local",
        "-Wl,-wrap,spi_flash_read",
        "-u", "app_entry",
        "-u", "_printf_float",
        "-u", "_scanf_float",
        "-u", "_DebugExceptionVector",
        "-u", "_DoubleExceptionVector",
        "-u", "_KernelExceptionVector",
        "-u", "_NMIExceptionVector",
        "-u", "_UserExceptionVector"
    ],

    CPPDEFINES=[
        ("F_CPU", "$BOARD_F_CPU"),
        "__ets__",
        "ICACHE_FLASH",
        ("ARDUINO", 10805),
        ("ARDUINO_BOARD", '\\"PLATFORMIO_%s\\"' % env.BoardConfig().id.upper()),
        "FLASHMODE_${BOARD_FLASH_MODE.upper()}",
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

    BUILDERS=dict(
        ElfToBin=Builder(
            action=env.VerboseAction(" ".join([
                '"$PYTHONEXE"',
                '"%s"' % join(FRAMEWORK_DIR, "tools", "elf2bin.py"),
                "--eboot", '"%s"' % join(
                    FRAMEWORK_DIR, "bootloaders", "eboot", "eboot.elf"),
                "--app", "$SOURCE",
                "--flash_mode", "$BOARD_FLASH_MODE",
                "--flash_freq", "${__get_board_f_flash(__env__)}",
                "--flash_size", "${__get_flash_size(__env__)}",
                "--path", '"%s"' % join(
                    platform.get_package_dir("toolchain-xtensa"), "bin"),
                "--out", "$TARGET"
            ]), "Building $TARGET"),
            suffix=".bin"
        )
    )
)

# copy CCFLAGS to ASFLAGS (-x assembler-with-cpp mode)
env.Append(ASFLAGS=env.get("CCFLAGS", [])[:])

flatten_cppdefines = env.Flatten(env['CPPDEFINES'])

#
# SDK
#
if "PIO_FRAMEWORK_ARDUINO_ESPRESSIF_SDK3" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("NONOSDK3V0", 1)],
        LIBPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lib", "NONOSDK3V0")]
    )
elif "PIO_FRAMEWORK_ARDUINO_ESPRESSIF_SDK221" in flatten_cppdefines:
    #(previous default)
    env.Append(
        CPPDEFINES=[("NONOSDK221", 1)],
        LIBPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lib", "NONOSDK221")]
    )
elif "PIO_FRAMEWORK_ARDUINO_ESPRESSIF_SDK22x_190313" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("NONOSDK22x_190313", 1)],
        LIBPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lib", "NONOSDK22x_190313")]
    )
elif "PIO_FRAMEWORK_ARDUINO_ESPRESSIF_SDK22x_191024" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("NONOSDK22x_191024", 1)],
        LIBPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lib", "NONOSDK22x_191024")]
    )
elif "PIO_FRAMEWORK_ARDUINO_ESPRESSIF_SDK22x_191105" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("NONOSDK22x_191105", 1)],
        LIBPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lib", "NONOSDK22x_191105")]
    )
elif "PIO_FRAMEWORK_ARDUINO_ESPRESSIF_SDK22x_191122" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("NONOSDK22x_191122", 1)],
        LIBPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lib", "NONOSDK22x_191122")]
    )
else: #(default) if "PIO_FRAMEWORK_ARDUINO_ESPRESSIF_SDK22x_190703" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("NONOSDK22x_190703", 1)],
        LIBPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lib", "NONOSDK22x_190703")]
    )

#
# lwIP
#
if "PIO_FRAMEWORK_ARDUINO_LWIP_HIGHER_BANDWIDTH" in flatten_cppdefines:
    env.Append(
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip", "include")],
        LIBS=["lwip_gcc"]
    )
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_IPV6_LOW_MEMORY" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 536), ("LWIP_FEATURES", 1), ("LWIP_IPV6", 1)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
        LIBS=["lwip6-536-feat"]
    )
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_IPV6_HIGHER_BANDWIDTH" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 1460), ("LWIP_FEATURES", 1), ("LWIP_IPV6", 1)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
        LIBS=["lwip6-1460-feat"]
    )
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_HIGHER_BANDWIDTH" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 1460), ("LWIP_FEATURES", 1), ("LWIP_IPV6", 0)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
        LIBS=["lwip2-1460-feat"]
    )
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY_LOW_FLASH" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 536), ("LWIP_FEATURES", 0), ("LWIP_IPV6", 0)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
        LIBS=["lwip2-536"]
    )
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_HIGHER_BANDWIDTH_LOW_FLASH" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 1460), ("LWIP_FEATURES", 0), ("LWIP_IPV6", 0)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
        LIBS=["lwip2-1460"]
    )
# PIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY (default)
else:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 536), ("LWIP_FEATURES", 1), ("LWIP_IPV6", 0)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
        LIBS=["lwip2-536-feat"]
    )

#
# VTables
#

current_vtables = None
fp_in_irom = ""
for d in flatten_cppdefines:
    if str(d).startswith("VTABLES_IN_"):
        current_vtables = d
    if str(d) == "FP_IN_IROM":
        fp_in_irom = "-DFP_IN_IROM"
if not current_vtables:
    current_vtables = "VTABLES_IN_FLASH"
    env.Append(CPPDEFINES=[current_vtables])
assert current_vtables

# Build the eagle.app.v6.common.ld linker file
app_ld = env.Command(
    join("$BUILD_DIR", "ld", "local.eagle.app.v6.common.ld"),
    join(FRAMEWORK_DIR, "tools", "sdk", "ld", "eagle.app.v6.common.ld.h"),
    env.VerboseAction(
        "$CC -CC -E -P -D%s %s $SOURCE -o $TARGET" % (current_vtables, fp_in_irom),
        "Generating LD script $TARGET"))
env.Depends("$BUILD_DIR/$PROGNAME$PROGSUFFIX", app_ld)

if not env.BoardConfig().get("build.ldscript", ""):
    env.Replace(LDSCRIPT_PATH=env.BoardConfig().get("build.arduino.ldscript", "")) 

#
# Dynamic core_version.h for staging builds
#

def platform_txt_version(default):
    with open(join(FRAMEWORK_DIR, "platform.txt"), "r") as platform_txt:
        for line in platform_txt:
            if not line:
                continue
            k, delim, v = line.partition("=")
            if not delim:
                continue
            if k == "version":
                return v.strip()

    return default

if isdir(join(FRAMEWORK_DIR, ".git")):
    cmd = '"$PYTHONEXE" "{script}" -b "$BUILD_DIR" -p "{framework_dir}" -v {version}'
    fmt = {
        "script": join(FRAMEWORK_DIR, "tools", "makecorever.py"),
        "framework_dir": FRAMEWORK_DIR,
        "version": platform_txt_version("unspecified")
    }

    env.Prepend(CPPPATH=[
        join("$BUILD_DIR", "core")
    ])
    core_version = env.Command(
        join("$BUILD_DIR", "core", "core_version.h"),
        join(FRAMEWORK_DIR, ".git"),
        env.VerboseAction(cmd.format(**fmt), "Generating $TARGET")
    )
    env.Depends("$BUILD_DIR/$PROGNAME$PROGSUFFIX", core_version)


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
