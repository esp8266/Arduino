#!/usr/bin/env python3

# Generate the core_version.h header per-build
#
# Copyright (C) 2019 - Earle F. Philhower, III
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import argparse
import os
import subprocess


def generate(path, platform_path, version="unspecified", release = False):
    def git(*args):
        cmd = ["git", "-C", platform_path]
        cmd.extend(args)
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True, stderr=subprocess.DEVNULL)
        return proc.stdout.readlines()[0].strip()

    text = ""

    try:
        text = "#define ARDUINO_ESP8266_GIT_VER   0x{}\n".format(git("rev-parse", "--short=8", "HEAD"))
    except Exception:
        pass

    # version is
    # - using Arduino-CLI:
    #   - blah-5.6.7     (official release, coming from platform.txt)
    #   - blah-5.6.7-dev (intermediate / unofficial / testing release)
    # - using git:
    #   - 5.6.7            (from release script, official release)
    #   - 5.6.7-42-g00d1e5 (from release script, test release)
    git_desc = version
    try:
        # in any case, get a better version when git is around
        git_desc = git("describe", "--tags")
    except Exception:
        pass

    text += "#define ARDUINO_ESP8266_GIT_DESC  {}\n".format(git_desc)
    text += "#define ARDUINO_ESP8266_VERSION   {}\n".format(version)
    text += "\n"

    version_split = version.split(".")
    # major: if present, skip "unix-" in "unix-3"
    text += "#define ARDUINO_ESP8266_MAJOR     {}\n".format(version_split[0].split("-")[-1])
    text += "#define ARDUINO_ESP8266_MINOR     {}\n".format(version_split[1])
    # revision can be ".n" or ".n-dev" or ".n-42-g00d1e5"
    revision = version_split[2].split("-")
    text += "#define ARDUINO_ESP8266_REVISION  {}\n".format(revision[0])
    text += "\n"

    # release or dev ?
    if release:
        text += "#define ARDUINO_ESP8266_RELEASE   \"{}\"\n".format(git_desc)
        text += "#define ARDUINO_ESP8266_RELEASE_{}\n".format(git_desc.replace("-","_").replace(".","_"))
    else:
        text += "#define ARDUINO_ESP8266_DEV       1 // developpment version\n"

    try:
        with open(path, "r") as inp:
            old_text = inp.read()
        if old_text == text:
            return
    except Exception:
        pass

    with open(path, "w") as out:
        out.write(text)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate core_version.h")

    parser.add_argument(
        "-b", "--build_path", action="store", required=True, help="build.path variable"
    )
    parser.add_argument(
        "-p",
        "--platform_path",
        action="store",
        required=True,
        help="platform.path variable",
    )
    parser.add_argument(
        "-v", "--version", action="store", required=True, help="version variable"
    )
    parser.add_argument("-i", "--include_dir", default="core")
    parser.add_argument("-r", "--release", action="store_true", default=False)

    args = parser.parse_args()

    include_dir = os.path.join(args.build_path, args.include_dir)
    try:
        os.makedirs(include_dir)
    except Exception:
        pass

    generate(
        os.path.join(include_dir, "core_version.h"),
        args.platform_path,
        version=args.version,
        release=args.release
    )
