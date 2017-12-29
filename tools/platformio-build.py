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

from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()
platform = env.PioPlatform()

FRAMEWORK_DIR = platform.get_package_dir("framework-arduinoespressif8266")
assert isdir(FRAMEWORK_DIR)


env.Prepend(
    CPPDEFINES=[
        ("ARDUINO", 10600),
        "LWIP_OPEN_SRC"
    ],
    CPPPATH=[
        join(FRAMEWORK_DIR, "tools", "sdk", "include"),
        join(FRAMEWORK_DIR, "tools", "sdk", "lwip", "include"),
        join(FRAMEWORK_DIR, "tools", "sdk", "libc",
             "xtensa-lx106-elf", "include"),
        join(FRAMEWORK_DIR, "cores", env.BoardConfig().get("build.core"))
    ],
    LIBPATH=[
        join(FRAMEWORK_DIR, "tools", "sdk", "lib"),
        join(FRAMEWORK_DIR, "tools", "sdk", "ld"),
        join(FRAMEWORK_DIR, "tools", "sdk", "libc", "xtensa-lx106-elf", "lib")
    ],
    LIBS=[
        "mesh", "wpa2", "smartconfig", "espnow", "pp", "main", "wpa", "lwip_gcc",
        "net80211", "wps", "crypto", "phy", "hal", "axtls", "gcc",
        "m", "c", "stdc++"
    ]
)

env.Append(
    LIBSOURCE_DIRS=[
        join(FRAMEWORK_DIR, "libraries")
    ],
    LINKFLAGS=[
        "-Wl,-wrap,system_restart_local",
        "-Wl,-wrap,spi_flash_read"
    ]
)

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