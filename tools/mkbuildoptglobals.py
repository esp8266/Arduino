#!/usr/bin/env python3

# Copyright (C) 2022 - M Hightower
#
# Updates & fixes for arduino-cli environment by Maxim Prokhorov
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

# This script manages the use of a file with a unique name, like
# `SKETCH.ino.globals.h`, in the SKETCH source directory to provide compiler
# command-line options (build options), sketch global macros, or inject code.
# The buildo ption data is encapsulated in an unique "C" comment block /* ... */
# and extracted into the build directory during prebuild.
#
# A Tip of the hat to:
#
# This PR continues the effort to get some form of global build support
# presented by brainelectronics' PR
# - https://github.com/esp8266/Arduino/pull/8095
#
# Used d-a-v's global name suggestion from arduino PR
# - https://github.com/arduino/arduino-cli/pull/1524

import argparse
import locale
import logging
import io
import re
import os
import dataclasses
import pathlib
import sys
import textwrap
import time

from typing import Optional, TextIO, Union, List, Tuple

from shutil import copystat


# Stay in sync with our bundled version
VERSION_MIN = (3, 7)

if sys.version_info < VERSION_MIN:
    raise SystemExit(
        f"{__file__}\nMinimal supported version of Python is {VERSION_MIN[0]}.{VERSION_MIN[1]}"
    )


# Like existing documentation methods, signature is embedded in the comment block
# Unlike existing documentation methods, only the first line contains any metadata

# Command-line options file, to be sourced by the compiler
BUILD_OPT_SIGNATURE_RE = re.compile(r"/[\*]@\s*?create-file:(?P<name>\S*?)\s*?@$")

# Script documentation & examples
DOCS_URL = (
    "https://arduino-esp8266.readthedocs.io/en/latest/faq/a06-global-build-options.html"
)
DOCS_EPILOG = f"""
Use platform.local.txt 'mkbuildoptglobals.extra_flags=...' to supply extra command line flags.
See {DOCS_URL} for more information.
"""


# Notify that custom build options exist for the other core, but not this one
OTHER_BUILD_OPTIONS = [
    "build_opt.h",
    "file_opt",
]


def other_build_options(p: pathlib.Path, sketch_header: pathlib.Path):
    return f"""Build options file '{p.name}' is not supported.")
Embed build options and code in '{sketch_header.name}' instead.
Create an empty '{sketch_header.name}' to silence this warning.

  See {DOCS_URL} for more information.
"""


def check_other_build_options(sketch_header: pathlib.Path) -> Optional[str]:
    if sketch_header.exists():
        return None

    for name in OTHER_BUILD_OPTIONS:
        p = sketch_header.parent / name
        if p.exists():
            return other_build_options(p, sketch_header)

    return None


# Retrieve *system* encoding, not the one used by python internally
#
# Given that GCC will handle lines from an @file as if they were on
# the command line. I assume that the contents of @file need to be encoded
# to match that of the shell running GCC runs. I am not 100% sure this API
# gives me that, but it appears to work.
#
# However, elsewhere when dealing with source code we continue to use 'utf-8',
# ref. https://gcc.gnu.org/onlinedocs/cpp/Character-sets.html
if sys.version_info >= (3, 11):
    DEFAULT_ENCODING = locale.getencoding()
else:
    DEFAULT_ENCODING = locale.getdefaultlocale()[1]

FILE_ENCODING = "utf-8"


# Issues trying to address through logging module & buffered printing
# 1. Arduino IDE 1.x / 2.0 print stderr with color red, allowing any
#    messages to stand out in an otherwise white on black console output.
# 2. With Arduino IDE preferences set for "no verbose output", you only see
#    stderr messages. Prior related prints are missing.
# 3. logging ensures that stdout & stderr buffers are flushed before exiting.
#    While it may not provide consistent output, no messages should be lost.
class LoggingFilter(logging.Filter):
    def __init__(self, *filter_only):
        self._filter_only = filter_only

    def filter(self, rec):
        return rec.levelno in self._filter_only


# Since default handler is not created, make sure only specific levels go through to stderr
TO_STDERR = logging.StreamHandler(sys.stderr)
TO_STDERR.setFormatter(
    logging.Formatter("*** %(filename)s %(funcName)s:%(lineno)d ***\n%(message)s\n")
)
TO_STDERR.setLevel(logging.NOTSET)
TO_STDERR.addFilter(
    LoggingFilter(
        logging.CRITICAL,
        logging.DEBUG,
        logging.ERROR,
        logging.FATAL,
        logging.WARNING,
    )
)

# Generic info messages should be on stdout (but, note the above, these are hidden by IDE defaults)
TO_STDOUT = logging.StreamHandler(sys.stdout)
TO_STDOUT.setFormatter(logging.Formatter("%(message)s"))
TO_STDOUT.setLevel(logging.INFO)
TO_STDOUT.addFilter(
    LoggingFilter(
        logging.INFO,
        logging.NOTSET,
    )
)

logging.basicConfig(level=logging.INFO, handlers=(TO_STDOUT, TO_STDERR))


class ParsingException(Exception):
    def __init__(self, file: Optional[str], lineno: int, line: str):
        self.file = file
        self.lineno = lineno
        self.line = line

    def __str__(self):
        out = ""

        if self.file:
            out += f"in {self.file}"

        lineno = f" {self.lineno}"
        out += f"\n\n{lineno} {self.line}"

        out += f'\n{" " * len(lineno)} '
        out += "^" * len(self.line.strip())

        return out


class InvalidSignature(ParsingException):
    pass


class InvalidSyntax(ParsingException):
    pass


def extract_build_opt(name: str, dst: TextIO, src: TextIO):
    """
    Read src line by line and extract matching 'create-file' directives.
    'name' can match multiple times

    Empty 'src' is always valid.
    Never matching anything for 'name' is also valid.

    Incorrectly written signatures always fail with an exception.

    C/C++ syntax validity isn't checked, that's up to the user
    """
    IN_RAW = 1
    IN_BUILD_OPT = 2
    IN_SKIP_OPT = 3

    state = IN_RAW
    block = []  # type: List[str]

    for n, raw_line in enumerate(src, start=1):
        line = raw_line.strip().rstrip()

        if state == IN_SKIP_OPT:
            if line.startswith("*/"):
                state = IN_RAW
                for line in block:
                    dst.write(line)
                block = []
            continue

        if line.startswith("/*@"):
            if not line.endswith("@"):
                raise InvalidSyntax(None, n, raw_line)

            result = BUILD_OPT_SIGNATURE_RE.search(line)
            if not result or state in (IN_BUILD_OPT, IN_SKIP_OPT):
                raise InvalidSignature(None, n, raw_line)

            if name == result.group("name"):
                state = IN_BUILD_OPT
            else:
                state = IN_SKIP_OPT

            continue

        if state == IN_BUILD_OPT:
            if line.startswith("*/"):
                state = IN_RAW
                continue

            if line.startswith(("#", "//", "*")):
                continue

            if not line:
                continue

            block.append(f"{line}\n")

    if state != IN_RAW:
        raise InvalidSyntax(None, n, raw_line)

    for line in block:
        dst.write(line)


def extract_build_opt_from_path(dst: TextIO, name: str, p: pathlib.Path):
    """
    Same as 'extract_build_opt', but use a file path
    """
    try:
        with p.open("r", encoding=FILE_ENCODING) as src:
            extract_build_opt(name, dst, src)
    except ParsingException as e:
        e.file = p.name
        raise e


def is_future_utime(p: pathlib.Path):
    return time.time_ns() < p.stat().st_mtime_ns


def as_stat_result(p: Union[os.stat_result, pathlib.Path]) -> Optional[os.stat_result]:
    if not isinstance(p, os.stat_result):
        if not p.exists():
            return None
        return p.stat()

    return p


def is_different_utime(
    p1: Union[os.stat_result, pathlib.Path],
    p2: Union[os.stat_result, pathlib.Path],
) -> bool:
    s1 = as_stat_result(p1)
    if not s1:
        return True

    s2 = as_stat_result(p2)
    if not s2:
        return True

    for attr in ("st_atime_ns", "st_mtime_ns"):
        if getattr(s1, attr) != getattr(s2, attr):
            return True

    return False


# Arduino IDE uses timestamps to determine whether the .o file should be rebuilt
def synchronize_utime(stat: Union[os.stat_result, pathlib.Path], *rest: pathlib.Path):
    """
    Retrieve stats from the first 'file' and apply to the 'rest'
    """
    if not isinstance(stat, os.stat_result):
        stat = stat.stat()
    logging.debug(
        "setting mtime=%d for:\n%s", stat.st_mtime, "\n".join(f"  {p}" for p in rest)
    )
    for p in rest:
        if is_different_utime(stat, p.stat()):
            os.utime(p, ns=(stat.st_atime_ns, stat.st_mtime_ns))


def as_include_line(p: pathlib.Path) -> str:
    out = p.absolute().as_posix()
    out = out.replace('"', '\\"')
    out = f'-include "{out}"'
    return out


def as_path_field(help: str):
    return dataclasses.field(
        default_factory=pathlib.Path,
        metadata={
            "help": help,
        },
    )


@dataclasses.dataclass
class Context:
    build_opt: pathlib.Path = as_path_field(
        "resulting options file, used in the gcc command line options"
    )

    source_sketch_header: pathlib.Path = as_path_field(
        ".globals.h located in the sketch directory"
    )
    build_sketch_header: pathlib.Path = as_path_field(
        ".globals.h located in the build directory"
    )

    common_header: pathlib.Path = as_path_field(
        "dependency file, copied into the core directory to trigger rebuilds"
    )


# build process requires some file to always exist, even when they are empty and thus unused
def ensure_exists_and_empty(*paths: pathlib.Path):
    """
    Create empty or replace existing files with an empty ones at 'paths'
    """
    for p in paths:
        if not p.parent.exists():
            p.parent.mkdir(parents=True, exist_ok=True)

        if not p.exists() or (p.exists() and p.stat().st_size):
            p.write_bytes(b"")
            logging.debug("%s is a placeholder", p.name)
        else:
            logging.debug("%s is up-to-date", p.name)


def ensure_normal_time(*paths: pathlib.Path):
    for p in paths:
        if p.exists() and is_future_utime(p):
            logging.debug("%s has timestamp in the future, fixing", p.name)
            p.touch()


# ref. https://gcc.gnu.org/onlinedocs/cpp/Line-Control.html
# arduino builder not just copies sketch files to the build directory,
# but also injects this line at the top to remember the source
def as_arduino_sketch_quoted_header(p: pathlib.Path):
    out = str(p.absolute())
    out = out.replace("\\", "\\\\")
    out = out.replace('"', '\\"')
    return f'#line 1 "{out}"\n'


def write_or_replace(p: pathlib.Path, contents: str, encoding=FILE_ENCODING) -> bool:
    actual = ""

    try:
        if p.exists():
            actual = p.read_text(encoding=encoding)
    except UnicodeDecodeError:
        logging.warning("cannot decode %s", p.name)

    if contents != actual:
        p.write_text(contents, encoding=encoding)
        logging.debug("%s contents written", p.name)
        return True

    logging.debug("%s is up-to-date", p.name)
    return False


# arduino builder would copy the file regardless
# prebuild stage has it missing though
def ensure_build_sketch_header_written(ctx: Context):
    """
    Sketch header copy or placeholder must always exist in the build directory (even when unused)
    """
    if not ctx.source_sketch_header.exists():
        ensure_exists_and_empty(ctx.build_sketch_header)
        return

    p = ctx.build_sketch_header
    if not p.parent.exists():
        p.parent.mkdir(parents=True, exist_ok=True)

    contents = ctx.source_sketch_header.read_text(encoding=FILE_ENCODING)
    contents = "".join(
        (
            as_arduino_sketch_quoted_header(ctx.source_sketch_header),
            contents,
            "\n",
        )
    )

    if write_or_replace(p, contents):
        copystat(ctx.source_sketch_header, p)


def ensure_common_header_bound(ctx: Context):
    """
    Record currently used command-line options file
    """
    write_or_replace(ctx.common_header, as_arduino_sketch_quoted_header(ctx.build_opt))


def make_build_opt_name(ctx: Context, debug: bool) -> str:
    name = ctx.build_opt.name
    if debug:
        name = f"{name}:debug"

    return name


def ensure_build_opt_written(ctx: Context, buffer: io.StringIO):
    """
    Make sure that 'build_opt' is written to the filesystem.

    '-include ...' lines are always appended at the end of the buffer.
    'build_opt' is not written when its contents remain unchanged.
    """
    includes = [
        ctx.common_header,
    ]

    if len(buffer.getvalue()):
        includes.append(ctx.build_sketch_header)

    for p in includes:
        buffer.write(f"{as_include_line(p)}\n")

    write_or_replace(ctx.build_opt, buffer.getvalue(), encoding=DEFAULT_ENCODING)


def maybe_empty_or_missing(p: pathlib.Path):
    return not p.exists() or not p.stat().st_size


def build_with_minimal_build_opt(ctx: Context):
    """
    When sketch header is empty or there were no opt files created
    """
    logging.debug("building with a minimal %s", ctx.build_opt.name)

    ensure_build_opt_written(ctx, buffer=io.StringIO())

    ensure_exists_and_empty(ctx.common_header)
    ensure_build_sketch_header_written(ctx)

    # sketch directory time ignored, stats are from the only persistent file
    synchronize_utime(
        ctx.common_header,
        ctx.build_opt,
        ctx.build_sketch_header,
    )


# Before synchronizing targets, find out which file was modified last
# by default, check 'st_mtime_ns' attribute of os.stat_result
def most_recent(
    *paths: pathlib.Path, attr="st_mtime_ns"
) -> Tuple[pathlib.Path, os.stat_result]:
    def make_pair(p: pathlib.Path):
        return (p, p.stat())

    def key(pair: Tuple[pathlib.Path, os.stat_result]) -> int:
        return getattr(pair[1], attr)

    if not paths:
        raise ValueError('"paths" cannot be empty')
    elif len(paths) == 1:
        return make_pair(paths[0])

    return max((make_pair(p) for p in paths), key=key)


def main_build(args: argparse.Namespace):
    ctx = Context(
        build_opt=args.build_opt,
        source_sketch_header=args.source_sketch_header,
        build_sketch_header=args.build_sketch_header,
        common_header=args.common_header,
    )

    if args.debug:
        logging.debug(
            "Build Context:\n%s",
            "".join(
                f'  "{field.name}" at {getattr(ctx, field.name)} - {field.metadata["help"]}\n'
                for field in dataclasses.fields(ctx)
            ),
        )

    # notify when other files similar to .globals.h are in the sketch directory
    other_build_options = check_other_build_options(ctx.source_sketch_header)
    if other_build_options:
        logging.warning(other_build_options)

    # future timestamps generally break build order.
    # before comparing or synchronizing time, make sure it is current
    ensure_normal_time(*dataclasses.astuple(ctx))

    # when .globals.h is missing, provide placeholder files for the build and exit
    if maybe_empty_or_missing(ctx.source_sketch_header):
        build_with_minimal_build_opt(ctx)
        return

    # when debug port is used, allow for a different set of command line options
    build_debug = args.build_debug or "DEBUG_SERIAL_PORT" in (
        args.D or []
    )  # type: bool
    name = make_build_opt_name(ctx, build_debug)

    # options file is not written immediately, buffer its contents before commiting
    build_opt_buffer = io.StringIO()

    try:
        extract_build_opt_from_path(build_opt_buffer, name, ctx.source_sketch_header)
    except ParsingException as e:
        raise

    # when command-line options were not created / found, it means the same thing as empty or missing .globals.h
    if not len(build_opt_buffer.getvalue()):
        build_with_minimal_build_opt(ctx)
        return

    logging.info(
        "\nExtra command-line options:\n%s",
        "\n".join(f"  {line}" for line in build_opt_buffer.getvalue().split("\n")),
    )

    # at this point, it is necessary to synchronize timestamps of every file
    ensure_build_opt_written(ctx, build_opt_buffer)
    ensure_build_sketch_header_written(ctx)
    ensure_common_header_bound(ctx)

    # stats are now based on the either active sketch or common header
    # (thus ensure core.a is rebuilt, even when sketch mtime is earlier)
    synchronize_utime(
        most_recent(ctx.common_header, ctx.source_sketch_header)[1],
        ctx.build_opt,
        ctx.build_sketch_header,
        ctx.common_header,
        ctx.source_sketch_header,
    )


def main_inspect(args: argparse.Namespace):
    p = args.path  # type: pathlib.Path

    buffer = io.StringIO()
    try:
        extract_build_opt_from_path(buffer, args.build_opt_name, p)
    except ParsingException as e:
        raise e from None

    logging.info(buffer.getvalue())


def main_placeholder(args: argparse.Namespace):
    paths = args.path  # type: List[pathlib.Path]
    ensure_exists_and_empty(*paths)


def main_synchronize(args: argparse.Namespace):
    first = args.first  # type: pathlib.Path
    rest = args.rest  # type: List[pathlib.Path]
    synchronize_utime(first, *rest)


def as_path(p: str) -> pathlib.Path:
    if p.startswith("{") or p.endswith("}"):
        raise ValueError(f'"{p}" was not resolved') from None

    return pathlib.Path(p)


def parse_args(args=None, namespace=None):
    parser = argparse.ArgumentParser(
        description="Handles sketch header containing command-line options, resulting build.opt and the shared core common header",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(DOCS_EPILOG),
    )

    def main_help(args: argparse.Namespace):
        parser.print_help()

    parser.set_defaults(func=main_help)

    parser.add_argument(
        "--debug", action="store_true", help=argparse.SUPPRESS
    )  # normal debug
    parser.add_argument(
        "--audit", action="store_true", help=argparse.SUPPRESS
    )  # extra noisy debug

    parser.add_argument(
        "--build-debug",
        action="store_true",
        help="Instead of build.opt, use build.opt:debug when searching for the comment signature match",
    )

    parser.add_argument(
        "-D",
        help="Intended to be used with mkbuildoptglobals.extra_flags={build.debug_port} in platform.local.txt)."
        " Only enable --build-debug when debug port is also enabled in the menu / fqbn options.",
    )

    subparsers = parser.add_subparsers()

    # "prebuild" hook recipe command, preparing all of the necessary build files

    build = subparsers.add_parser("build")

    build.add_argument(
        "--build-opt",
        type=as_path,
        required=True,
        help="Command-line options file FQFN aka Fully Qualified File Name "
        "(%build-path%/core/%build-opt%)",
    )
    build.add_argument(
        "--source-sketch-header",
        type=as_path,
        required=True,
        help="FQFN of the globals.h header, located in the sketch directory "
        "(%sketchname%/%sketchname%.ino.globals.h)",
    )
    build.add_argument(
        "--build-sketch-header",
        type=as_path,
        required=True,
        help="FQFN of the globals.h header, located in the build directory "
        "(%build-path%/sketch/%sketchname%.ino.globals.h)",
    )
    build.add_argument(
        "--common-header",
        type=as_path,
        required=True,
        help="FQFN of shared dependency header (%core-path%/%common-header%)",
    )

    build.set_defaults(func=main_build)

    # "postbuild" hook recipe command, in case some files have to be cleared

    placeholder = subparsers.add_parser(
        "placeholder",
        help=ensure_exists_and_empty.__doc__,
    )
    placeholder.add_argument(
        "path",
        action="append",
        type=as_path,
    )
    placeholder.set_defaults(func=main_placeholder)

    # Parse file path and discover any 'name' options inside

    inspect = subparsers.add_parser(
        "inspect",
        help=extract_build_opt.__doc__,
    )
    inspect.add_argument("--build-opt-name", type=str, default="build.opt")
    inspect.add_argument(
        "path",
        type=as_path,
    )
    inspect.set_defaults(func=main_inspect)

    # Retrieve stats from the first file and apply to the rest

    synchronize = subparsers.add_parser(
        "synchronize",
        help=synchronize_utime.__doc__,
    )
    synchronize.add_argument(
        "first",
        type=as_path,
        help="Any file",
    )
    synchronize.add_argument(
        "rest",
        type=as_path,
        nargs="+",
        help="Any file",
    )
    synchronize.set_defaults(func=main_synchronize)

    return parser.parse_args(args, namespace)


def main(args: argparse.Namespace):
    # mildly verbose logging, intended to notify about the steps taken
    if args.debug:
        logging.root.setLevel(logging.DEBUG)

    # very verbose logging from the python internals
    if args.audit and sys.version_info >= (3, 8):

        def hook(event, args):
            # note that logging module itself has audit calls,
            # logging.debug(...) here would deadlock output
            print(f"{event}:{args}", file=sys.stderr)

        sys.addaudithook(hook)

    return args.func(args)


if __name__ == "__main__":
    main(parse_args())
