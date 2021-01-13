#!/usr/bin/env python3

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

from __future__ import print_function
import argparse
import re
import os
import subprocess
import sys
import tempfile

fmodeb = { 'dout': 3, 'dio': 2, 'qout': 1, 'qio': 0 }
ffreqb = { '40': 0, '26': 1, '20': 2, '80': 15 }
fsizeb = { '512K': 0, '256K': 1, '1M': 2, '2M': 3, '4M': 4, '8M': 8, '16M': 9 }

crcsize_offset = 4088
crcval_offset = 4092

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
    fd, tmpfile = tempfile.mkstemp()
    os.close(fd)
    subprocess.check_call([path + "/xtensa-lx106-elf-objcopy", '-O', 'binary', '--only-section=' + segment, elf, tmpfile], stdout=subprocess.PIPE)
    with open(tmpfile, "rb") as f:
        raw = f.read()
    os.remove(tmpfile)

    return raw

def write_bin(out, args, elf, segments, to_addr):
    entry = int(get_elf_entry( elf, args.path ))
    header = [ 0xe9, len(segments), fmodeb[args.flash_mode], ffreqb[args.flash_freq] + 16 * fsizeb[args.flash_size],
               entry & 255, (entry>>8) & 255, (entry>>16) & 255, (entry>>24) & 255 ]
    out.write(bytearray(header))
    total_size = 8
    checksum = 0xef
    for segment in segments:
        [size, addr] = get_segment_size_addr(elf, segment, args.path)
        seghdr = [ addr & 255, (addr>>8) & 255, (addr>>16) & 255, (addr>>24) & 255,
                   size & 255, (size>>8) & 255, (size>>16) & 255, (size>>24) & 255]
        out.write(bytearray(seghdr));
        total_size += 8;
        raw = read_segment(elf, segment, args.path)
        if len(raw) != size:
            raise Exception('Segment size doesn\'t match read data for "' + segment + '" in "' + elf + '"')
        out.write(raw)
        total_size += len(raw)
        try:
            for data in raw:
                checksum = checksum ^ ord(data)
        except Exception:
            for data in raw:
                checksum = checksum ^ data
    total_size += 1
    while total_size & 15:
        total_size += 1
        out.write(bytearray([0]))
    out.write(bytearray([checksum]))
    if to_addr != 0:
        if total_size + 8 > to_addr:
            raise Exception('Bin image of ' + elf + ' is too big, actual size ' + str(total_size  + 8) + ', target size ' + str(to_addr) + '.')
        while total_size < to_addr:
            out.write(bytearray([0xaa]))
            total_size += 1

def crc8266(ldata):
    "Return the CRC of ldata using same algorithm as eboot"
    crc = 0xffffffff
    idx = 0
    while idx < len(ldata):
        byte = int(ldata[idx])
        idx = idx + 1
        for i in [0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01]:
            bit = crc & 0x80000000
            if (byte & i) != 0:
                if bit == 0:
                    bit = 1
                else:
                    bit = 0
            crc = int(crc << 1) & 0xffffffff
            if bit != 0:
                crc = int(crc ^ 0x04c11db7)
    return crc

def store_word(raw, offset, val):
    "Place a 4-byte word in 8266-dependent order in the raw image"
    raw[offset] = val & 255
    raw[offset + 1] = (val >> 8) & 255
    raw[offset + 2] = (val >> 16) & 255
    raw[offset + 3] = (val >> 24) & 255
    return raw

def add_crc(out):
    with open(out, "rb") as binfile:
        raw = bytearray(binfile.read())

    # Zero out the spots we're going to overwrite to be idempotent
    raw = store_word(raw, crcsize_offset, 0)
    raw = store_word(raw, crcval_offset, 0)
    crc = crc8266(raw)
    raw = store_word(raw, crcsize_offset, len(raw))
    raw = store_word(raw, crcval_offset, int(crc))

    with open(out, "wb") as binfile:
        binfile.write(raw)

def gzip_bin(mode, out):
    import gzip

    firmware_path = out
    gzip_path = firmware_path + '.gz'
    orig_path = firmware_path + '.orig'
    if os.path.exists(gzip_path):
        os.remove(gzip_path)
    print('GZipping firmware ' + firmware_path)
    with open(firmware_path, 'rb') as firmware_file, \
            gzip.open(gzip_path, 'wb') as dest:
        data = firmware_file.read()
        dest.write(data)
    orig_size = os.stat(firmware_path).st_size
    gzip_size = os.stat(gzip_path).st_size
    print("New FW size {:d} bytes vs old {:d} bytes".format(
        gzip_size, orig_size))

    if mode == "PIO":
        if os.path.exists(orig_path):
            os.remove(orig_path)
        print('Moving original firmware to ' + orig_path)
        os.rename(firmware_path, orig_path)
        os.rename(gzip_path, firmware_path)

def main():
    parser = argparse.ArgumentParser(description='Create a BIN file from eboot.elf and Arduino sketch.elf for upload by esptool.py')
    parser.add_argument('-e', '--eboot', action='store', required=True, help='Path to the Arduino eboot.elf bootloader')
    parser.add_argument('-a', '--app', action='store', required=True, help='Path to the Arduino sketch ELF')
    parser.add_argument('-m', '--flash_mode', action='store', required=True, choices=['dout', 'dio', 'qout', 'qio'], help='SPI flash mode')
    parser.add_argument('-f', '--flash_freq', action='store', required=True, choices=['20', '26', '40', '80'], help='SPI flash speed')
    parser.add_argument('-s', '--flash_size', action='store', required=True, choices=['256K', '512K', '1M', '2M', '4M', '8M', '16M'], help='SPI flash size')
    parser.add_argument('-o', '--out', action='store', required=True, help='Output BIN filename')
    parser.add_argument('-p', '--path', action='store', required=True, help='Path to Xtensa toolchain binaries')
    parser.add_argument('-g', '--gzip', choices=['PIO', 'Arduino'], help='PIO - generate gzipped BIN file, Arduino - generate BIN and BIN.gz')

    args = parser.parse_args()

    print('Creating BIN file "{out}" using "{eboot}" and "{app}"'.format(
        out=args.out, eboot=args.eboot, app=args.app))

    with open(args.out, "wb") as out:
        def wrapper(**kwargs):
            write_bin(out=out, args=args, **kwargs)

        wrapper(
            elf=args.eboot,
            segments=[".text"],
            to_addr=4096
        )

        wrapper(
            elf=args.app,
            segments=[".irom0.text", ".text", ".text1", ".data", ".rodata"],
            to_addr=0
        )

    # Because the CRC includes both eboot and app, can only calculate it after the entire BIN generated
    add_crc(args.out)

    if args.gzip:
        gzip_bin(args.gzip, args.out)

    return 0


if __name__ == '__main__':
    sys.exit(main())
