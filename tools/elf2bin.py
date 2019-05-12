#!/usr/bin/env python

# Generate an Arduino compatible BIN file from bootloader and sketch ELF
# Replaces esptool-ck.exe and emulates its behavior.
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
import re
import os
import subprocess
import sys
import tempfile

fmodeb = { 'dout': 3, 'dio': 2, 'quot': 1, 'qio': 0 }
ffreqb = { '40': 0, '26': 1, '20': 2, '80': 15 }
fsizeb = { '512K': 0, '256K': 1, '1M': 2, '2M': 3, '4M': 4, '8M': 8, '16M': 9 }

def get_elf_entry(elf, path):
    p = subprocess.Popen([path + "/xtensa-lx106-elf-readelf", '-h', elf], stdout=subprocess.PIPE, universal_newlines=True )
    lines = p.stdout.readlines()
    for line in lines:
        if 'Entry point address' in line:
            words = re.split('\s+', line)
            entry_point = words[-2]
            return int(entry_point, 16)
    raise Exception('Unable to find entry point in file "' + elf + '"')

def get_segment_size_addr(elf, segment, path):
    p = subprocess.Popen([path + '/xtensa-lx106-elf-objdump', '-h', '-j', segment,  elf], stdout=subprocess.PIPE, universal_newlines=True )
    lines = p.stdout.readlines()
    for line in lines:
        if segment in line:
            words = re.split('\s+', line)
            size = int(words[3], 16)
            addr = int(words[4], 16)
            return [ size, addr ]
    raise Exception('Unable to find size and start point in file "' + elf + '" for "' + segment + '"')

def read_segment(elf, segment, path):
    tmpfile, dumpfile = tempfile.mkstemp()
    os.close(tmpfile)
    p = subprocess.check_call([path + "/xtensa-lx106-elf-objcopy", '-O', 'binary', '--only-section=' + segment, elf, dumpfile], stdout=subprocess.PIPE)
    binfile = open(dumpfile, "rb")
    raw = binfile.read()
    binfile.close()
    return raw

def write_bin(out, elf, segments, to_addr, flash_mode, flash_size, flash_freq, path):
    entry = int(get_elf_entry( elf, path ))
    header = [ 0xe9, len(segments), fmodeb[flash_mode], ffreqb[flash_freq] + 16 * fsizeb[flash_size],
               entry & 255, (entry>>8) & 255, (entry>>16) & 255, (entry>>24) & 255 ]
    out.write(bytearray(header))
    total_size = 8
    checksum = 0xef
    for segment in segments:
        [size, addr] = get_segment_size_addr(elf, segment, path)
        seghdr = [ addr & 255, (addr>>8) & 255, (addr>>16) & 255, (addr>>24) & 255,
                   size & 255, (size>>8) & 255, (size>>16) & 255, (size>>24) & 255]
        out.write(bytearray(seghdr));
        total_size += 8;
        raw = read_segment(elf, segment, path)
        if len(raw) != size:
            raise Exception('Segment size doesn\'t match read data for "' + segment + '" in "' + elf + '"')
        out.write(raw)
        total_size += len(raw)
        try:
            for data in raw:
                checksum = checksum ^ ord(data)
        except:
            for data in raw:
                checksum = checksum ^ data
    total_size += 1
    while total_size & 15:
        total_size += 1
        out.write(bytearray([0]))
    out.write(bytearray([checksum]))
    if to_addr != 0:
        while total_size < to_addr:
            out.write(bytearray([0xaa]))
            total_size += 1

def main():
    parser = argparse.ArgumentParser(description='Create a BIN file from eboot.elf and Arduino sketch.elf for upload by esptool.py')
    parser.add_argument('-e', '--eboot', action='store', required=True, help='Path to the Arduino eboot.elf bootloader')
    parser.add_argument('-a', '--app', action='store', required=True, help='Path to the Arduino sketch ELF')
    parser.add_argument('-m', '--flash_mode', action='store', required=True, choices=['dout', 'dio', 'qout', 'qio'], help='SPI flash mode')
    parser.add_argument('-f', '--flash_freq', action='store', required=True, choices=['20', '26', '40', '80'], help='SPI flash speed')
    parser.add_argument('-s', '--flash_size', action='store', required=True, choices=['256K', '512K', '1M', '2M', '4M', '8M', '16M'], help='SPI flash size')
    parser.add_argument('-o', '--out', action='store', required=True, help='Output BIN filename')
    parser.add_argument('-p', '--path', action='store', required=True, help='Path to Xtensa toolchain binaries')

    args = parser.parse_args()

    print('Creating BIN file "' + args.out + '" using "' + args.app + '"')

    out = open(args.out, "wb")
    write_bin(out, args.eboot, ['.text'], 4096, args.flash_mode, args.flash_size, args.flash_freq, args.path)
    write_bin(out, args.app, ['.irom0.text', '.text', '.text1', '.data', '.rodata'], 0, args.flash_mode, args.flash_size, args.flash_freq, args.path)
    out.close()

    return 0


if __name__ == '__main__':
    sys.exit(main())
