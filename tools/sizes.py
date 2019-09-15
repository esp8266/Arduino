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

from __future__ import print_function
import argparse
import os
import subprocess
import sys

def get_segment_sizes(elf, path):
    sizes = {}
    sizes['IROM'] = 0
    sizes['IRAM'] = 0
    sizes['DATA'] = 0
    sizes['RODATA'] = 0
    sizes['BSS'] = 0
    p = subprocess.Popen([path + "/xtensa-lx106-elf-size", '-A', elf], stdout=subprocess.PIPE, universal_newlines=True )
    lines = p.stdout.readlines()
    for line in lines:
        words = line.split()
        if line.startswith('.irom0.text'):
            sizes['IROM'] = sizes['IROM'] + int(words[1])
        elif line.startswith('.text'): # Gets .text and .text1
            sizes['IRAM'] = sizes['IRAM'] + int(words[1])
        elif line.startswith('.data'): # Gets .text and .text1
            sizes['DATA'] = sizes['DATA'] + int(words[1])
        elif line.startswith('.rodata'): # Gets .text and .text1
            sizes['RODATA'] = sizes['RODATA'] + int(words[1])
        elif line.startswith('.bss'): # Gets .text and .text1
            sizes['BSS'] = sizes['BSS'] + int(words[1])
    return sizes

def main():
    parser = argparse.ArgumentParser(description='Report the different segment sizes of a compiled ELF file')
    parser.add_argument('-e', '--elf', action='store', required=True, help='Path to the Arduino sketch ELF')
    parser.add_argument('-p', '--path', action='store', required=True, help='Path to Xtensa toolchain binaries')

    args = parser.parse_args()
    sizes = get_segment_sizes(args.elf, args.path)

    sys.stderr.write("Executable segment sizes:" + os.linesep)
    for k in sizes.keys():
        sys.stderr.write("%-7s: %d%s" % (k, sizes[k], os.linesep))

    return 0

if __name__ == '__main__':
    sys.exit(main())
