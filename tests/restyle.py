#!/usr/bin/env python

import argparse
import os
import sys
import pathlib
import subprocess
import contextlib

from dataclasses import dataclass


GIT_ROOT = pathlib.Path(
    subprocess.check_output(
        ["git", "rev-parse", "--show-toplevel"], universal_newlines=True
    ).strip()
)


def clang_format(clang_format, config, files):
    if not files:
        raise ValueError("Files list cannot be empty")

    cmd = [clang_format, "--verbose", f"--style=file:{config.as_posix()}", "-i"]
    cmd.extend(files)

    subprocess.run(cmd, check=True)


def ls_files(patterns):
    """Git-only search, but rather poor at matching complex patterns (at least w/ <=py3.12)"""
    proc = subprocess.run(
        ["git", "--no-pager", "ls-files"],
        capture_output=True,
        check=True,
        universal_newlines=True,
    )

    out = []
    for line in proc.stdout.split("\n"):
        path = pathlib.Path(line.strip())
        if any(path.match(pattern) for pattern in patterns):
            out.append(path)

    return out


def diff_lines():
    proc = subprocess.run(
        ["git", "--no-pager", "diff", "--ignore-submodules"],
        capture_output=True,
        check=True,
        universal_newlines=True,
    )

    return proc.stdout.split("\n")


def find_files(patterns):
    """Filesystem search, matches both git and non-git files"""
    return [
        file
        for pattern in patterns
        for file in [found for found in GIT_ROOT.rglob(pattern)]
    ]


def find_core_files():
    """Returns a subset of Core files that should be formatted"""
    return [
        file
        for file in find_files(
            (
                "cores/esp8266/Lwip*",
                "libraries/ESP8266mDNS/**/*",
                "libraries/Wire/**/*",
                "libraries/lwIP*/**/*",
                "cores/esp8266/debug*",
                "cores/esp8266/core_esp8266_si2c*",
                "cores/esp8266/StreamString*",
                "cores/esp8266/StreamSend*",
                "libraries/Netdump/**/*",
                "tests/**/*",
            )
        )
        if file.is_file()
        and file.suffix in (".c", ".cpp", ".h", ".hpp")
        and not GIT_ROOT / "tests/device/test_libc" in file.parents
        and not GIT_ROOT / "tests/host/bin" in file.parents
        and not GIT_ROOT / "tests/host/common/catch.hpp" == file
    ]


def find_arduino_files():
    """Returns every .ino file available in the repository, excluding submodule ones"""
    return [
        ino
        for library in find_files(("libraries/*",))
        if library.is_dir() and not (library / ".git").exists()
        for ino in library.rglob("**/*.ino")
    ]


FILES_PRESETS = {
    "core": find_core_files,
    "arduino": find_arduino_files,
}


@dataclass
class Changed:
    file: str
    hunk: str
    lines: list[int]


class Context:
    def __init__(self):
        self.append_hunk = False
        self.deleted = False
        self.file = ""
        self.hunk = []
        self.markers = []

    def reset(self):
        self.__init__()

    def reset_with_line(self, line):
        self.reset()
        self.hunk.append(line)

    def pop(self, out, line):
        if self.file and self.hunk and self.markers:
            out.append(
                Changed(file=self.file, hunk="\n".join(self.hunk), lines=self.markers)
            )

        self.reset_with_line(line)


def changed_files_for_diff(lines: list[str] | str) -> list[Changed]:
    """
    Naive git-diff output parser. Generates list of objects for every file changed after clang-format.
    """
    match lines:
        case str():
            lines = lines.split("\n")
        case list():
            pass
        case _:
            raise ValueError("Unknown 'lines' type, can be either list[str] or str")

    ctx = Context()
    out = []

    # TODO: pygit2?
    # ref. https://github.com/cpp-linter/cpp-linter/blob/main/cpp_linter/git/__init__.py ::parse_diff
    # ref. https://github.com/libgit2/pygit2/blob/master/src/diff.c ::parse_diff
    for line in lines:
        # '--- a/path/to/changed/file' most likely
        # '--- /dev/null' aka created file. should be ignored, same as removed ones
        if line.startswith("---"):
            ctx.pop(out, line)

            _, file = line.split(" ")
            ctx.deleted = "/dev/null" in file

        # '+++ b/path/to/changed/file' most likely
        # '+++ /dev/null' aka removed file
        elif not ctx.deleted and line.startswith("+++"):
            ctx.hunk.append(line)

            _, file = line.split(" ")
            ctx.deleted = "/dev/null" in file
            if not ctx.deleted:
                ctx.file = file[2:]

        # @@ from-file-line-numbers to-file-line-numbers @@
        elif not ctx.deleted and line.startswith("@@"):
            ctx.hunk.append(line)

            _, _, numbers, _ = line.split(" ", 3)
            if "," in numbers:
                numbers, _ = numbers.split(",")  # drop count

            numbers = numbers.replace("+", "")
            numbers = numbers.replace("-", "")

            ctx.markers.append(int(numbers))
            ctx.append_hunk = True

        # capture diff for the summary
        elif ctx.append_hunk and line.startswith(("+", "-", " ")):
            ctx.hunk.append(line)

    ctx.pop(out, line)

    return out


def changed_files() -> list[Changed]:
    return changed_files_for_diff(diff_lines())


def errors_changed(changed: Changed):
    all_lines = ", ".join(str(x) for x in changed.lines)
    for line in changed.lines:
        print(
            f"::error file={changed.file},title=Run tests/restyle.sh and re-commit {changed.file},line={line}::File {changed.file} failed clang-format style check. (lines {all_lines})"
        )


SUMMARY_PATH = pathlib.Path(os.environ.get("GITHUB_STEP_SUMMARY", os.devnull))
SUMMARY_OUTPUT = SUMMARY_PATH.open("a")


def summary_diff(changed: Changed):
    with contextlib.redirect_stdout(SUMMARY_OUTPUT):
        print(f"# {changed.file} (suggested change)")
        print("```diff")
        print(changed.hunk)
        print("```")


def stdout_diff():
    subprocess.run(["git", "--no-pager", "diff", "--ignore-submodules"])


def assert_unchanged():
    subprocess.run(
        ["git", "diff", "--ignore-submodules", "--exit-code"],
        check=True,
        stdout=subprocess.DEVNULL,
    )


def run_format(args):
    targets = []

    for include in args.include:
        targets.append(
            (GIT_ROOT / f"tests/clang-format-{include}.yaml", FILES_PRESETS[include]())
        )

    if not targets:
        targets.append((args.config, args.files))

    for target in targets:
        clang_format(args.clang_format, *target)


def run_assert(args):
    for changed in changed_files():
        if args.with_errors:
            errors_changed(changed)
        if args.with_summary:
            summary_diff(changed)

    if args.with_diff:
        stdout_diff()

    assert_unchanged()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    cmd = parser.add_subparsers(required=True)
    format_ = cmd.add_parser("format")
    format_.set_defaults(func=run_format)
    format_.add_argument("--clang-format", default="clang-format")

    fmt = format_.add_subparsers(required=True)

    preset = fmt.add_parser("preset")
    preset.add_argument(
        "--include", action="append", required=True, choices=tuple(FILES_PRESETS.keys())
    )

    files = fmt.add_parser("files")
    files.add_argument("--config", type=pathlib.Path, required=True)
    files.add_argument("files", type=pathlib.Path, nargs="+")

    assert_ = cmd.add_parser("assert")
    assert_.set_defaults(func=run_assert)
    assert_.add_argument("--with-diff", action="store_true")
    assert_.add_argument("--with-errors", action="store_true")
    assert_.add_argument("--with-summary", action="store_true")

    args = parser.parse_args()
    args.func(args)
