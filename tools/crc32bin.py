#!/usr/bin/env python3

"""
# Place a CRC32 checksum and length in a generated BIN file
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
"""

from __future__ import print_function
import argparse
import sys

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

def main():
    "Main CLI interface"
    parser = argparse.ArgumentParser(description='Embed CRC32 and length in a generated BIN image')
    parser.add_argument('-b', '--bin', action='store', required=True,
                        help='Path to the Arduino sketch.bin')
    parser.add_argument('-s', '--size', action='store', required=True,
                        help='Byte offset in bin to store the size')
    parser.add_argument('-c', '--crc', action='store', required=True,
                        help='Byte offset in bin to store the crc32')
    args = parser.parse_args()

    with open(args.bin, "rb") as binfile:
        raw = bytearray(binfile.read())

    # Zero out the spots we're going to overwrite to be idempotent
    raw = store_word(raw, int(args.size), 0)
    raw = store_word(raw, int(args.crc), 0)
    crc = crc8266(raw)
    raw = store_word(raw, int(args.size), len(raw))
    raw = store_word(raw, int(args.crc), int(crc))

    with open(args.bin, "wb") as binfile:
        binfile.write(raw)

    print("Inserted length of " + hex(len(raw)) + " and CRC of " + hex(crc)+ " into " + args.bin)

if __name__ == '__main__':
    sys.exit(main())
