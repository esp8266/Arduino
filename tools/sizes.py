#!/usr/bin/env python3

# Display the segment sizes used by an ELF
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
import sys

sys.stdout = sys.stderr

def get_segment_sizes(elf, path, mmu):
    iram_size = 0
    iheap_size = 0
    icache_size = 32168

    for line in mmu.split():
        words = line.split("=")
        if line.startswith("-DMMU_IRAM_SIZE"):
            iram_size = int(words[1], 16)
        elif line.startswith("-DMMU_ICACHE_SIZE"):
            icache_size = int(words[1], 16)
        elif line.startswith("-DMMU_SEC_HEAP_SIZE"):
            iheap_size = int(words[1], 16)

    sizes = [
        [
            "Variables and constants in RAM (global, static)",
            [
                {
                    "DATA": 0,
                    "RODATA": 0,
                    "BSS": 0,
                },
                80192,
            ],
        ],
        [
            "Instruction RAM (IRAM_ATTR, ICACHE_RAM_ATTR)",
            [
                {
                    "ICACHE": icache_size,
                    "IHEAP": iheap_size,
                    "IRAM": 0,
                },
                65536,
            ],
        ],
        ["Code in flash (default, ICACHE_FLASH_ATTR)", [{"IROM": 0}, 1048576]],
    ]

    section_mapping = (
        (".irom0.text", "IROM"),
        (".text", "IRAM"),
        (".data", "DATA"),
        (".rodata", "RODATA"),
        (".bss", "BSS"),
    )

    import locale
    shell_encoding = locale.getdefaultlocale()[1]
    cmd = [os.path.join(path, "xtensa-lx106-elf-size"), "-A", elf]
    with subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True, encoding=shell_encoding) as proc:
        lines = proc.stdout.readlines()
        for line in lines:
            words = line.split()
            for section, target in section_mapping:
                if not line.startswith(section):
                    continue
                for group, (segments, total) in sizes:
                    if target in segments:
                        segments[target] += int(words[1])
                        assert segments[target] <= total

    return sizes


def percentage(lhs, rhs):
    return "{}%".format(int(100.0 * float(lhs) / float(rhs)))


HINTS = {
    "ICACHE": "reserved space for flash instruction cache",
    "IRAM": "code in IRAM",
    "IHEAP": "secondary heap space",
    "IROM": "code in flash",
    "DATA": "initialized variables",
    "RODATA": "constants",
    "BSS": "zeroed variables",
}


def safe_prefix(n, length):
    if n == length:
        return "`--"

    return "|--"


def prefix(n, length):
    if n == length:
        return "╚══"

    return "╠══"


def filter_segments(segments):
    used = 0
    number = 0
    available = []

    for (segment, size) in segments.items():
        if not size:
            continue
        used += size
        number += 1
        available.append((number, segment, size))

    return (number, used, available)


def main():
    parser = argparse.ArgumentParser(
        description="Report the different segment sizes of a compiled ELF file"
    )
    parser.add_argument(
        "-e",
        "--elf",
        action="store",
        required=True,
        help="Path to the Arduino sketch ELF",
    )
    parser.add_argument(
        "-p",
        "--path",
        action="store",
        required=True,
        help="Path to Xtensa toolchain binaries",
    )
    parser.add_argument(
        "-i", "--mmu", action="store", required=False, help="MMU build options"
    )

    args = parser.parse_args()
    sizes = get_segment_sizes(args.elf, args.path, args.mmu)

    for group, (segments, total) in sizes:
        number, used, segments = filter_segments(segments)

        print(f". {group:<8}, used {used} / {total} bytes ({percentage(used, total)})")
        try:
            print("║   SEGMENT  BYTES    DESCRIPTION")
        except UnicodeEncodeError:
            print("|   SEGMENT  BYTES    DESCRIPTION")
        for n, segment, size in segments:
            try:
                print(f"{prefix(n, number)} ", end="")
            except UnicodeEncodeError:
                print(f"{safe_prefix(n, number)} ", end="")
            print(f"{segment:<8} {size:<8} {HINTS[segment]:<16}")


if __name__ == "__main__":
    main()
