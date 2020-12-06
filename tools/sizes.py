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

def get_segment_hints(iram):
    hints = {}
    hints['ICACHE'] = '          - flash instruction cache'
    hints['IROM'] = '         - code in flash         (default or ICACHE_FLASH_ATTR)'
    hints['IRAM'] = '  / ' + str(iram) + ' - code in IRAM          (ICACHE_RAM_ATTR, ISRs...)'
    hints['DATA'] = ')         - initialized variables (global, static) in RAM/HEAP'
    hints['RODATA'] = ') / 81920 - constants             (global, static) in RAM/HEAP'
    hints['BSS'] = ')         - zeroed variables      (global, static) in RAM/HEAP'
    return hints

def get_segment_sizes(elf, path):
    sizes = {}
    sizes['ICACHE'] = 0
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

def get_mmu_sizes(mmu, sizes):
    iram = 0x8000
    sizes['ICACHE'] = 0x8000
    lines = mmu.split(' ')
    for line in lines:
        words = line.split('=')
        if line.startswith('-DMMU_IRAM_SIZE'):
            iram = int(words[1], 16)
        elif line.startswith('-DMMU_ICACHE_SIZE'):
            sizes['ICACHE'] = int(words[1], 16)
    return [iram, sizes]

def main():
    parser = argparse.ArgumentParser(description='Report the different segment sizes of a compiled ELF file')
    parser.add_argument('-e', '--elf', action='store', required=True, help='Path to the Arduino sketch ELF')
    parser.add_argument('-p', '--path', action='store', required=True, help='Path to Xtensa toolchain binaries')
    parser.add_argument('-i', '--mmu', action='store', required=False, help='MMU build options')

    args = parser.parse_args()
    sizes = get_segment_sizes(args.elf, args.path)
    [iram, sizes] = get_mmu_sizes(args.mmu, sizes)
    hints = get_segment_hints(iram)

    sys.stderr.write("Executable segment sizes:" + os.linesep)
    for k in sizes.keys():
        sys.stderr.write("%-7s: %-5d %s %s" % (k, sizes[k], hints[k], os.linesep))

    return 0

if __name__ == '__main__':
    sys.exit(main())
