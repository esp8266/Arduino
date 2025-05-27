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
import pathlib
import subprocess
import sys

from typing import Optional, TextIO


PWD = pathlib.Path(__file__).parent.absolute()

VERSION_UNSPECIFIED = "unspecified"
VERSION_DEFAULT = ("0", "0", "0")


def check_git(*args: str, cwd: Optional[str]):
    cmd = ["git"]
    if cwd:
        cmd.extend(["-C", cwd])
    cmd.extend(args)

    try:
        with subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            universal_newlines=True,
            stderr=subprocess.DEVNULL,
        ) as proc:
            if proc.stdout:
                lines = proc.stdout.readlines()
                return lines[0].strip()
    except IndexError:
        pass
    except FileNotFoundError:
        pass

    return ""


def generate(
    out: TextIO,
    *,
    git_root: pathlib.Path,
    hash_length: int = 8,
    release: bool,
    version: str,
):
    git_root = git_root.absolute()
    git_cwd = git_root.as_posix()

    def git(*args):
        return check_git(*args, cwd=git_cwd)

    git_ver = "0" * hash_length
    git_ver = git("rev-parse", f"--short={hash_length}", "HEAD") or git_ver

    # version is
    # - using Arduino-CLI:
    #   - blah-5.6.7     (official release, coming from platform.txt)
    #   - blah-5.6.7-dev (intermediate / unofficial / testing release)
    # - using git:
    #   - 5.6.7            (from release script, official release)
    #   - 5.6.7-42-g00d1e5 (from release script, test release)

    # in any case, get a better version when git is around
    git_desc = git("describe", "--tags") or version

    if version == VERSION_UNSPECIFIED:
        version = git_desc

    version_triple = list(VERSION_DEFAULT)

    if version != VERSION_UNSPECIFIED:
        try:
            version_triple = version.split(".", 2)
        except ValueError:
            pass

    major, minor, patch = version_triple

    major = major.split("-")[-1]
    revision = patch.split("-")[0]

    text = rf"""// ! ! ! DO NOT EDIT, AUTOMATICALLY GENERATED ! ! !
#define ARDUINO_ESP8266_GIT_VER   0x{git_ver}
#define ARDUINO_ESP8266_GIT_DESC  {git_desc}
#define ARDUINO_ESP8266_VERSION   {version}

#define ARDUINO_ESP8266_MAJOR     {major}
#define ARDUINO_ESP8266_MINOR     {minor}
#define ARDUINO_ESP8266_REVISION  {revision}
"""
    if release:
        text += rf"""
#define ARDUINO_ESP8266_RELEASE   \"{major}.{minor}.{revision}\"
#define ARDUINO_ESP8266_RELEASE_{major}_{minor}_{revision}
"""
    else:
        text += """
#define ARDUINO_ESP8266_DEV       1 // development version
"""

    out.write(text)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate core_version.h")

    parser.add_argument(
        "--git-root",
        action="store",
        help="ESP8266 Core Git root. In platform.txt, this is {platform.path}",
        type=pathlib.Path,
        default=PWD / "..",
    )
    parser.add_argument(
        "--hash-length",
        default=8,
        type=int,
        help="Used in git rev-parse --short=...",
    )
    parser.add_argument(
        "--version",
        action="store",
        default=VERSION_UNSPECIFIED,
        help="ESP8266 Core version string. In platform.txt, this is {version}",
    )
    parser.add_argument(
        "--release",
        action="store_true",
        default=False,
        help="In addition to numeric version, also provide ARDUINO_ESP8266_RELEASE{,_...} definitions",
    )
    parser.add_argument(
        "output",
        nargs="?",
        type=str,
        default="",
    )

    args = parser.parse_args()

    def select_output(s: str) -> TextIO:
        if not s:
            return sys.stdout

        out = pathlib.Path(s)
        out.parent.mkdir(parents=True, exist_ok=True)

        return out.open("w", encoding="utf-8")

    with select_output(args.output) as out:
        generate(
            out,
            git_root=args.git_root,
            hash_length=args.hash_length,
            release=args.release,
            version=args.version,
        )
