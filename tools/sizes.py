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


def get_segment_hints():
    return {
        "ICACHE": "flash instruction cache",
        "IROM": "code in flash (default, ICACHE_FLASH_ATTR)",
        "IRAM": "code in IRAM (IRAM_ATTR, ICACHE_RAM_ATTR)",
        "DATA": "initialized variables (global, static) in RAM",
        "RODATA": "constants (global, static) in RAM",
        "BSS": "zeroed variables (global, static) in RAM",
    }


def get_segment_sizes(elf, path, mmu):
    sizes = {
        "ICACHE": [32768, 32768],
        "IROM": [0, 1048576],
        "IRAM": [0, 32768],
        "DATA": [0, 81920],
        "RODATA": [0, 81920],
        "BSS": [0, 81920],
    }

    cmd = [os.path.join(path, "xtensa-lx106-elf-size"), "-A", elf]
    with subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True) as proc:
        lines = proc.stdout.readlines()
        for line in lines:
            words = line.split()
            if line.startswith(".irom0.text"):
                sizes["IROM"][0] += int(words[1])
            elif line.startswith(".text"):
                sizes["IRAM"][0] += int(words[1])
            elif line.startswith(".data"):
                sizes["DATA"][0] += int(words[1])
            elif line.startswith(".rodata"):
                sizes["RODATA"][0] += int(words[1])
            elif line.startswith(".bss"):
                sizes["BSS"][0] += int(words[1])

    for line in mmu.split():
        words = line.split("=")
        if line.startswith("-DMMU_IRAM_SIZE"):
            sizes["IRAM"][1] = int(words[1], 16)
        elif line.startswith("-DMMU_ICACHE_SIZE"):
            sizes["ICACHE"][0] = sizes["ICACHE"][1] = int(words[1], 16)

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
    hints = get_segment_hints()

    template = "{:<8} {:<8} {:<8} {:<8} {:<16}"
    header = template.format("SEGMENT", "USED", "TOTAL", "PERCENT", "DESCRIPTION")

    with contextlib.redirect_stdout(sys.stderr):
        print(header)
        print(len(header) * "-")
        for key, (used, total) in sizes.items():
            print(
                template.format(key, used, total, percentage(used, total), hints[key])
            )

    return 0


if __name__ == "__main__":
    main()
