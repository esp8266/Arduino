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

# For SCons documentation, see:
# https://scons.org/doc/latest

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
board = env.BoardConfig()
gzip_fw = board.get("build.gzip_fw", False)
gzip_switch = []

FRAMEWORK_DIR = platform.get_package_dir("framework-arduinoespressif8266")
assert isdir(FRAMEWORK_DIR)

if gzip_fw:
    gzip_switch = ["--gzip", "PIO"]

env.Append(
    ASFLAGS=["-x", "assembler-with-cpp"],

    # General options that are passed to the C compiler (C only; not C++)
    CFLAGS=[
        "-std=gnu17",
        "-Wpointer-arith",
        "-Wno-implicit-function-declaration",
        "-Wl,-EL",
        "-fno-inline-functions",
        "-nostdlib"
    ],

    # General options that are passed to the C and C++ compilers
    CCFLAGS=[
        "-Os",  # optimize for size
        "-mlongcalls",
        "-mtext-section-literals",
        "-falign-functions=4",
        "-U__STRICT_ANSI__",
        "-D_GNU_SOURCE",
        "-ffunction-sections",
        "-fdata-sections",
        "-Wall",
        "-Werror=return-type",
        "-free",
        "-fipa-pta"
    ],

    # General options that are passed to the C++ compiler
    CXXFLAGS=[
        "-fno-rtti",
        "-std=gnu++17"
    ],

    # General user options passed to the linker
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

    # A platform independent specification of C preprocessor definitions as either:
    # - -DFLAG as "FLAG"
    # - -DFLAG=VALUE as ("FLAG", "VALUE")
    CPPDEFINES=[
        ("F_CPU", "$BOARD_F_CPU"),
        "__ets__",
        "ICACHE_FLASH",
        ("ARDUINO", 10805),
        ("ARDUINO_BOARD", '\\"PLATFORMIO_%s\\"' % env.BoardConfig().id.upper()),
        "FLASHMODE_${BOARD_FLASH_MODE.upper()}",
        "LWIP_OPEN_SRC"
    ],

    # The list of directories that the C preprocessor will search for include directories
    CPPPATH=[
        join(FRAMEWORK_DIR, "tools", "sdk", "include"),
        join(FRAMEWORK_DIR, "cores", env.BoardConfig().get("build.core")),
        join(platform.get_package_dir("toolchain-xtensa"), "include")
    ],

    # The list of directories that will be searched for libraries
    LIBPATH=[
        join("$BUILD_DIR", "ld"),  # eagle.app.v6.common.ld
        join(FRAMEWORK_DIR, "tools", "sdk", "lib"),
        join(FRAMEWORK_DIR, "tools", "sdk", "ld")
    ],

    # A list of one or more libraries that will be linked with any executable programs created by this environment
    LIBS=[
        "hal", "phy", "pp", "net80211", "wpa", "crypto", "main",
        "wps", "bearssl", "espnow", "smartconfig", "airkiss", "wpa2",
        "m", "c", "gcc"
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
            ] + gzip_switch), "Building $TARGET"),
            suffix=".bin"
        )
    )
)

# helper functions to place a library at a specific position in the linking 
# order, either referenced by "n positions before the end" or "n positions after the start".
def inject_lib_at_position_from_back(env, lib, position):
    old_libs = env["LIBS"]
    old_libs.insert(len(old_libs) - position, lib)
    env["LIBS"] = old_libs


def inject_lib_at_position_from_front(env, lib, position):
    old_libs = env["LIBS"]
    old_libs.insert(position, lib)
    env["LIBS"] = old_libs

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
if "PIO_FRAMEWORK_ARDUINO_LWIP2_IPV6_LOW_MEMORY" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 536), ("LWIP_FEATURES", 1), ("LWIP_IPV6", 1)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
    )
    inject_lib_at_position_from_front(env, "lwip6-536-feat", 4)
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_IPV6_HIGHER_BANDWIDTH" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 1460), ("LWIP_FEATURES", 1), ("LWIP_IPV6", 1)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
    )
    inject_lib_at_position_from_front(env, "lwip6-1460-feat", 4)
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_HIGHER_BANDWIDTH" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 1460), ("LWIP_FEATURES", 1), ("LWIP_IPV6", 0)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
    )
    inject_lib_at_position_from_front(env, "lwip2-1460-feat", 4)
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY_LOW_FLASH" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 536), ("LWIP_FEATURES", 0), ("LWIP_IPV6", 0)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
    )
    inject_lib_at_position_from_front(env, "lwip2-536", 4)
elif "PIO_FRAMEWORK_ARDUINO_LWIP2_HIGHER_BANDWIDTH_LOW_FLASH" in flatten_cppdefines:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 1460), ("LWIP_FEATURES", 0), ("LWIP_IPV6", 0)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
    )
    inject_lib_at_position_from_front(env, "lwip2-1460", 4)
# PIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY (default)
else:
    env.Append(
        CPPDEFINES=[("TCP_MSS", 536), ("LWIP_FEATURES", 1), ("LWIP_IPV6", 0)],
        CPPPATH=[join(FRAMEWORK_DIR, "tools", "sdk", "lwip2", "include")],
    )
    inject_lib_at_position_from_front(env, "lwip2-536-feat", 4)

#
# Waveform
#
if "PIO_FRAMEWORK_ARDUINO_WAVEFORM_LOCKED_PHASE" in flatten_cppdefines:
    env.Append(CPPDEFINES=[("WAVEFORM_LOCKED_PHASE", 1)])
# PIO_FRAMEWORK_ARDUINO_WAVEFORM_LOCKED_PWM will be used by default

#
# Exceptions
#
if "PIO_FRAMEWORK_ARDUINO_ENABLE_EXCEPTIONS" in flatten_cppdefines:
    env.Append(
        CXXFLAGS=["-fexceptions"],
    )
    # we need to respect the original linking order of the libraries
    # and cannot just append it to the end, but at the specific correct 
    # position, offset from the back.
    inject_lib_at_position_from_back(env, "stdc++-exc", 3)
else:
    env.Append(
        CXXFLAGS=["-fno-exceptions"],
    )
    inject_lib_at_position_from_back(env, "stdc++", 3)
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

#
# MMU
#

mmu_flags = []
required_flags = ("MMU_IRAM_SIZE", "MMU_ICACHE_SIZE")
if "PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48" in flatten_cppdefines:
    mmu_flags = [("MMU_IRAM_SIZE", "0xC000"), ("MMU_ICACHE_SIZE", "0x4000")]
elif "PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED" in flatten_cppdefines:
    mmu_flags = [
        ("MMU_IRAM_SIZE", "0xC000"),
        ("MMU_ICACHE_SIZE", "0x4000"),
        "MMU_IRAM_HEAP",
    ]
elif "PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM32_SECHEAP_NOTSHARED" in flatten_cppdefines:
    mmu_flags = [
        ("MMU_IRAM_SIZE", "0x8000"),
        ("MMU_ICACHE_SIZE", "0x4000"),
        ("MMU_SEC_HEAP_SIZE", "0x4000"),
        ("MMU_SEC_HEAP", "0x40108000"),
    ]
elif "PIO_FRAMEWORK_ARDUINO_MMU_EXTERNAL_128K" in flatten_cppdefines:
    mmu_flags = [
        ("MMU_IRAM_SIZE", "0x8000"),
        ("MMU_ICACHE_SIZE", "0x8000"),
        ("MMU_EXTERNAL_HEAP", "128"),
    ]
elif "PIO_FRAMEWORK_ARDUINO_MMU_EXTERNAL_1024K" in flatten_cppdefines:
    mmu_flags = [
        ("MMU_IRAM_SIZE", "0x8000"),
        ("MMU_ICACHE_SIZE", "0x8000"),
        ("MMU_EXTERNAL_HEAP", "256"),
    ]
elif "PIO_FRAMEWORK_ARDUINO_MMU_CUSTOM" in flatten_cppdefines:
    if not all(d in flatten_cppdefines for d in required_flags):
        print(
            "Error: Missing custom MMU configuration flags (%s)!"
            % ", ".join(required_flags)
        )
        env.Exit(1)

    for flag in env["CPPDEFINES"]:
        define = flag
        if isinstance(flag, (tuple, list)):
            define, _ = flag
        if define.startswith("MMU_"):
            mmu_flags.append(flag)
# PIO_FRAMEWORK_ARDUINO_MMU_CACHE32_IRAM32 (default)
else:
    mmu_flags = [
        ("MMU_IRAM_SIZE", board.get("build.mmu_iram_size", "0x8000")),
        ("MMU_ICACHE_SIZE", board.get("build.mmu_icache_size", "0x8000"))]
    if any(f in flatten_cppdefines for f in required_flags):
        print(
            "Warning! Detected custom MMU flags. Please use the "
            "`-D PIO_FRAMEWORK_ARDUINO_MMU_CUSTOM` option to disable "
            "the default configuration."
        )

assert mmu_flags
env.Append(CPPDEFINES=mmu_flags)


# Build the eagle.app.v6.common.ld linker file
app_ld = env.Command(
    join("$BUILD_DIR", "ld", "local.eagle.app.v6.common.ld"),
    join(FRAMEWORK_DIR, "tools", "sdk", "ld", "eagle.app.v6.common.ld.h"),
    env.VerboseAction(
        "$CC -CC -E -P -D%s %s %s $SOURCE -o $TARGET"
        % (
            current_vtables,
            # String representation of MMU flags
            " ".join(
                [
                    "-D%s=%s" % f if isinstance(f, (tuple, list)) else "-D" + f
                    for f in mmu_flags
                ]
            ),
            fp_in_irom,
        ),
        "Generating LD script $TARGET",
    ),
)
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
