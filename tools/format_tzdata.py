#!/usr/bin/env python3
# /// script
# dependencies = [
#   "tzdata",
# ]
# requires-python = ">=3.12"
# ///

# this script refreshes world timezone definitions in
#   cores/esp8266/TZ.h
#
# use the file output argument or stdout redirect to overwrite the target file

import argparse
import contextlib
import itertools
import datetime
import pathlib
import sys

from dataclasses import dataclass
from operator import attrgetter

from importlib import resources
from importlib.resources.abc import Traversable

from collections.abc import Iterable, Callable

import typing
from typing import TextIO, Self

import tzdata  # https://tzdata.readthedocs.io/en/latest/


class NotZoneInfo(Exception):
    pass


def tz_from_zonefile(zonefile: Traversable) -> str:
    with zonefile.open("rb") as f:
        magic = f.read(4)
        if magic != b"TZif":
            raise NotZoneInfo()

        *_, tail = f.readlines()
        tz = tail.strip()
        tz = tz.decode("ascii")

    return tz


@dataclass(frozen=True)
class Tz:
    name: str
    value: str


@dataclass(frozen=True)
class File:
    """
    Newline-delimited file listing IANA keys. Zone information files listed MUST exist.
    """

    file: Traversable

    @classmethod
    def fromString(cls, s: str) -> Self:
        return cls(pathlib.Path(s))


def multiple_from_file(root: Traversable, zones: File) -> Iterable[Tz]:
    with zones.file.open("r") as f:
        names = [name.strip() for name in f.readlines()]

    for name in names:
        zonefile = root.joinpath(name)
        tz = tz_from_zonefile(zonefile)
        yield Tz(name, tz)


@dataclass(frozen=True)
class Area:
    """
    Area name of one or multiple IANA keys.
    --area Europe

    This source is ignored when given an invalid area name.
    --area DoesntExist"""

    name: str


def maybe_multiple_for_area(root: Traversable, area: Area) -> list[Tz]:
    out: list[Tz] = []
    root = root.joinpath(area.name)

    if root.is_dir():
        for location in root.iterdir():
            if location.is_dir():
                continue

            try:
                tz = tz_from_zonefile(location)
                out.append(Tz(f"{area.name}/{location.name}", tz))
            except NotZoneInfo:
                pass

    return out


def split_kv(s: str) -> tuple[str, str]:
    key, _, val = s.partition("=")
    return (key, val)


@dataclass(frozen=True)
class Link:
    """
    Named link to an existing IANA key.
    --link Foo/Bar=Europe/London

    This source is ignored if named zone information file already exists.
    --link Europe/Isle_of_Man=Europe/London

    This source is ignored if target zone information file doesn't exist.
    --link Europe/Isle_of_Man=Doesnt/Exist

    Intended to be combined with either --file or --key"""

    name: str
    target: str

    @classmethod
    def fromString(cls, s: str) -> Self:
        return cls(*split_kv(s))


def maybe_one_link(root: Traversable, link: Link) -> tuple[Tz] | tuple[()]:
    if not root.joinpath(link.name).is_file() and root.joinpath(link.target).is_file():
        tz = tz_from_zonefile(root.joinpath(link.target))
        return (Tz(link.name, tz),)

    return tuple()


@dataclass(frozen=True)
class Key:
    """
    IANA key name. This source is ignored if zone information file doesn't exist."""

    name: str


def maybe_one_key(root: Traversable, key: Key) -> tuple[Tz] | tuple[()]:
    if root.joinpath(key.name).is_file():
        tz = tz_from_zonefile(root.joinpath(key.name))
        return (Tz(key.name, tz),)

    return tuple()


@dataclass(frozen=True)
class Raw:
    """
    Inject a named TZ variable."""

    name: str
    value: str

    @classmethod
    def fromString(cls, s: str) -> Self:
        return cls(*split_kv(s))


def safe_macro_name(name: str) -> str:
    swaps = [["-", "m"], ["+", "p"], ["/", "_"]]

    for lhs, rhs in swaps:
        name = name.replace(lhs, rhs)

    return name


def header(info: Iterable[Tz]):
    print("// ! ! ! DO NOT EDIT, AUTOMATICALLY GENERATED ! ! !")
    print(f"// File created {datetime.datetime.now(tz=datetime.timezone.utc)}")
    print(f"// Based on IANA database {tzdata.IANA_VERSION}")
    print(
        f"// Re-run <esp8266 arduino core>/tools/{pathlib.Path(__file__).name} to update"
    )
    print()
    print("#pragma once")
    print()
    for tz in info:
        print(f'#define TZ_{safe_macro_name(tz.name)}\tPSTR("{tz.value}")')


UTC_NAMES: tuple[str, ...] = (
    "Universal",
    "UTC",
    "UCT",
    "Zulu",
    "GMT",
    "GMT+0",
    "GMT-0",
    "GMT0",
    "Greenwich",
)


def markdown_group(tz: Tz) -> str:
    if tz.name in UTC_NAMES:
        return "Universal Coordinated Time"

    if not "/" in tz.name:
        return "Other"

    prefix, _, _ = tz.name.partition("/")
    return prefix


def markdown(info: Iterable[Tz]):
    print(f"> [!NOTE]")
    print(f"> Based on IANA database {tzdata.IANA_VERSION}")
    print()
    print(f"> [!TIP]")
    print(f"> Re-run {pathlib.Path(__file__).name} to update")
    print()
    print("---")
    print()

    info = sorted(info, key=markdown_group)
    grouped = itertools.groupby(info, key=markdown_group)

    last = ""
    for heading, group in grouped:
        if heading != last:
            print(f"# {heading}")
            print()
            last = heading
        print("|Name|Value|")
        print("|---|---|")
        for tz in group:
            print(f"|{tz.name}|`{tz.value}`|")
        print()

    if last == "":
        print()


FORMATTERS = {
    "header": header,
    "markdown": markdown,
}

type Source = File | Link | Key | Raw


DEFAULT_SOURCES: tuple[Source, ...] = (
    File(resources.files("tzdata").joinpath("zones")),
    Link("Europe/Zaporizhzhia", "Europe/Zaporozhye"),
    Link("Europe/Uzhhorod", "Europe/Uzhgorod"),
)


def format_argparse_source(src: Source):
    match src:
        case File() as file:
            return f"--file {file.file}"
        case Area() as area:
            return f"--area {area.name}"
        case Link() as link:
            return f"--link {link.name}={link.target}"
        case Key() as key:
            return f"--key {key.name}"
        case Raw() as raw:
            return f"--raw {raw.name}={raw.value}"
        case _:
            raise ValueError(f"Unknown type {type(src)}")


# default=... is awkward to use with groups of arguments
def format_argparse_default_sources(sources: Iterable[Source]) -> str:
    return "\n".join(f"  `{format_argparse_source(src)}`" for src in sources)


class Formatter(
    argparse.ArgumentDefaultsHelpFormatter,
    argparse.RawTextHelpFormatter,
):
    pass


VERSION = f"""
version:
  {tzdata.IANA_VERSION=}
  {tzdata.__version__=}
"""

EPILOG = f"""
default sources:
{format_argparse_default_sources(DEFAULT_SOURCES)}

{VERSION}
"""


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        epilog=EPILOG,
        formatter_class=Formatter,
    )

    sources_group = parser.add_argument_group("iana zone key sources")
    sources_group.add_argument(
        "--file",
        type=File.fromString,
        metavar="FILE",
        dest="sources",
        action="append",
        help=File.__doc__,
        default=argparse.SUPPRESS,
    )

    sources_group.add_argument(
        "--area",
        type=Area,
        metavar="NAME",
        dest="sources",
        action="append",
        help=Area.__doc__,
        default=argparse.SUPPRESS,
    )

    sources_group.add_argument(
        "--link",
        type=Link.fromString,
        metavar="NAME=TARGET",
        dest="sources",
        action="append",
        help=Link.__doc__,
        default=argparse.SUPPRESS,
    )

    sources_group.add_argument(
        "--key",
        type=Key,
        metavar="NAME",
        dest="sources",
        action="append",
        help=Key.__doc__,
        default=argparse.SUPPRESS,
    )

    sources_group.add_argument(
        "--raw",
        type=Raw.fromString,
        metavar="NAME=TZ",
        dest="sources",
        action="append",
        help=Raw.__doc__,
        default=argparse.SUPPRESS,
    )

    parser.add_argument(
        "--format",
        choices=tuple(FORMATTERS.keys()),
        default="header",
        help="Choose output formatter",
    )

    parser.add_argument(
        "--root",
        help="Where do we get raw zoneinfo files from",
        type=pathlib.Path,
        default=resources.files("tzdata.zoneinfo"),
    )

    parser.add_argument(
        "--version",
        action="version",
        version=VERSION,
    )

    parser.add_argument(
        "output",
        type=str,
        nargs="?",
        default="-",
    )

    args = parser.parse_args()
    if not getattr(args, "sources", None):
        args.sources = DEFAULT_SOURCES

    root: Traversable = args.root
    mapping: dict[str, Tz] = {}

    def update(items: Iterable[Tz]):
        for item in items:
            if not item.name in mapping:
                mapping[item.name] = item

    for src in args.sources:
        match src:
            case File() as file:
                update(multiple_from_file(root, file))
            case Area() as area:
                update(maybe_multiple_for_area(root, area))
            case Link() as link:
                update(maybe_one_link(root, link))
            case Key() as key:
                update(maybe_one_key(root, key))
            case Raw() as raw:
                update(Tz(raw.name, raw.value))

    info = sorted(mapping.values(), key=attrgetter("name"))

    def maybe_stdout(s: str) -> TextIO:
        if s == "-":
            return sys.stdout

        return pathlib.Path(s).open("w")

    with contextlib.redirect_stdout(maybe_stdout(args.output)):
        FORMATTERS[args.format](info)
