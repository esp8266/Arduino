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
import contextlib


HINTS = {
    "ICACHE": "configured flash instruction cache",
    "IROM": "code in flash (default, ICACHE_FLASH_ATTR)",
    "IRAM": "code in IRAM (IRAM_ATTR, ICACHE_RAM_ATTR)",
    "DATA": "initialized variables (global, static)",
    "RODATA": "constants (global, static)",
    "BSS": "zeroed variables (global, static)",
}


def get_segment_sizes(elf, path, mmu):
    iram_size = 0
    icache_size = 32168

    for line in mmu.split():
        words = line.split("=")
        if line.startswith("-DMMU_IRAM_SIZE"):
            iram_size = int(words[1], 16)
        elif line.startswith("-DMMU_ICACHE_SIZE"):
            icache_size = int(words[1], 16)

    sizes = [
        ["Variables and constants in RAM", [{
            "DATA": 0,
            "RODATA": 0,
            "BSS": 0,
        }, 80192]],
        ["Instruction cache", [{
            "ICACHE": icache_size,
        }, icache_size]],
        ["Instruction RAM", [{
            "IRAM": 0,
        }, iram_size]],
        ["Code in flash", [{
            "IROM": 0
        }, 1048576]],
    ]

    mapping = [
        [".irom0.text", "IROM"],
        [".text", "IRAM"],
        [".data", "DATA"],
        [".rodata", "RODATA"],
        [".bss", "BSS"],
    ]

    cmd = [os.path.join(path, "xtensa-lx106-elf-size"), "-A", elf]
    with subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True) as proc:
        lines = proc.stdout.readlines()
        for line in lines:
            words = line.split()
            for section, target in mapping:
                if not line.startswith(section):
                    continue
                for group, (segments, total) in sizes:
                    if target in segments:
                        segments[target] += int(words[1])
                        assert segments[target] <= total

    return sizes


def percentage(lhs, rhs):
    return "{}%".format(int(100.0 * float(lhs) / float(rhs)))


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
        print(f". {group:<8} (total {total} bytes)")
        for n, (segment, size) in enumerate(segments.items(), start=1):
            if n == len(segments):
                prefix = "└──"
            else:
                prefix = "├──"
            print(f"{prefix} {segment:<8} {size:<8} - {HINTS[segment]:<16}")


if __name__ == "__main__":
    main()
