#!/usr/bin/env python3

# this script refreshes world timezone definitions in
#   cores/esp8266/TZ.h
#
# use the file output argument or stdout redirect to overwrite the target file

import argparse
import contextlib
import datetime
import mmap
import sys

from importlib import resources
from pathlib   import Path
from typing    import List, Tuple, Optional

import tzdata  # https://tzdata.readthedocs.io/en/latest/

# Let us use constants for defaults:

TZIF_MAGIC = b"TZif"
UTC_VALUE  = "UTC0"
ENCODING   = "utf-8"

def known_alias(entry: str) -> Optional[str]:
    swaps = {
        "Europe/Zaporozhye": "Europe/Zaporizhzhia",
        "Europe/Uzhgorod": "Europe/Uzhhorod",
    }

    return swaps.get(entry)


def fix_name(name: str) -> str:
    swaps = [["-", "m"], ["+", "p"], ["/", "_"]]

    for lhs, rhs in swaps:
        name = name.replace(lhs, rhs)

    return name


def utc_alias(zone: str) -> bool:
    return zone in (
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


def tzdata_resource_from_name(name: str) -> Path:
    pair = name.rsplit("/", 1)
    if len(pair) == 1:
        return resources.files("tzdata.zoneinfo") / pair[0]

    return resources.files(f'tzdata.zoneinfo.{pair[0].replace("/", ".")}') / pair[1]


def make_zones_list(f) -> List[str]:
    return [zone.strip() for zone in f.readlines()]


def make_zones(args: argparse.Namespace) -> List[Tuple[str, str]]:
    out = []

    for zone in make_zones_list(args.zones):
        if args.root:
            target = args.root / zone
        else:
            target = tzdata_resource_from_name(zone)

        with target.open("rb") as f:
            magic = f.read(4)
            if magic != TZIF_MAGIC:
                continue

            m = mmap.mmap(f.fileno(), 0, prot=mmap.PROT_READ)
            newline = m.rfind(b"\n", 0, len(m) - 1)
            if newline < 0:
                continue

            m.seek(newline + 1)
            tz = m.readline().strip().decode(ENCODING)

            if alias := known_alias(zone):
                out.append([alias, tz])

            out.append([zone, tz])

    out.sort(key=lambda x: x[0])
    return out


def markdown(zones: List[Tuple[str, str]]) -> None:
    utcs = []
    rows = []

    for name, value in zones:
        if utc_alias(name):
            utcs.append(name)
            continue

        rows.append(f"|{name}|`{UTC_VALUE}`|")

    print("|Name|Value|")
    print("|---|---|")
    for name in utcs:
        print(f"|{name}|UTC0|")

    last = ""
    for row in rows:
        prefix, _, _ = row.partition("/")
        if last != prefix:
            last = prefix
            print("|||")
        print(row)
    print()
    print("---")
    print()
    print(f"*Generated with *{tzdata.IANA_VERSION=} {tzdata.__version__=}*")


def header(zones: List[Tuple[str, str]]) -> None:
    print("// ! ! ! DO NOT EDIT, AUTOMATICALLY GENERATED ! ! !")
    print(f"// File created {datetime.datetime.now(tz=datetime.timezone.utc)}")
    print(f"// Based on IANA database {tzdata.IANA_VERSION}")
    print(f"// Re-run <esp8266 arduino core>/tools/{Path(sys.argv[0]).name} to update")
    print()
    print("#pragma once")
    print()
    for name, value in zones:
        print(f'#define TZ_{fix_name(name)}\tPSTR("{value}")')


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--output",
        type=argparse.FileType("w", encoding=ENCODING),
        default=sys.stdout,
    )
    parser.add_argument(
        "--format",
        default="header",
        choices=["header", "markdown"],
    )
    parser.add_argument(
        "--zones",
        type=argparse.FileType("r", encoding=ENCODING),
        help="Zone names file, one per line",
        default=Path(tzdata.__file__).parent / "zones",
    )
    parser.add_argument(
        "--root",
        help="Where do we get raw zoneinfo files from",
        type=Path,
    )

    args = parser.parse_args()
    zones = make_zones(args)

    with contextlib.redirect_stdout(args.output):
        if args.format == "markdown":
            markdown(zones)
        elif args.format == "header":
            header(zones)

