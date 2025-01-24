#!/usr/bin/env python3

# Wrapper for Arduino core / others that can call esptool.py possibly multiple times
# Adds pyserial to sys.path automatically based on the path of the current file

# First parameter is pyserial path, second is esptool path, then a series of command arguments
# i.e. upload.py tools/pyserial tools/esptool write_flash file 0x0

import os
import sys
import tempfile

sys.argv.pop(0) # Remove executable name
toolspath = os.path.dirname(os.path.realpath(__file__))
try:
    sys.path.insert(0, os.path.join(toolspath, "pyserial")) # Add pyserial dir to search path
    sys.path.insert(0, os.path.join(toolspath, "esptool")) # Add esptool dir to search path
    import esptool # If this fails, we can't continue and will bomb below
except ImportError:
    sys.stderr.write("pyserial or esptool directories not found next to this upload.py tool.\n")
    sys.exit(1)

cmdline = []
write_option = ''
write_addr = '0x0'
erase_addr = ''
erase_len = ''

while sys.argv:
    thisarg = sys.argv.pop(0)

    # We silently replace the 921kbaud setting with 460k to enable backward
    # compatibility with the old esptool-ck.exe.  Esptool.py doesn't seem
    # work reliably at 921k, but is still significantly faster at 460kbaud.
    if thisarg == "921600":
        thisarg = "460800"

    # 'erase_flash' command is translated to the write_flash --erase-all option
    # https://github.com/esp8266/Arduino/issues/6755#issuecomment-553208688
    if thisarg == "erase_flash":
        write_option = '--erase-all'
    # 'erase_region' is using a temporary file filled with 0xff
    elif thisarg == 'erase_region':
        erase_addr = sys.argv.pop(0)
        erase_len = sys.argv.pop(0)
    # 'write_flash' and everything else is used as-is
    elif thisarg == 'write_flash':
        write_addr = sys.argv.pop(0)
        binary = sys.argv.pop(0)
    elif thisarg:
        cmdline = cmdline + [thisarg]

cmdline = cmdline + ['write_flash']
if write_option:
    cmdline = cmdline + [write_option]
cmdline = cmdline + ['--flash_size', 'detect']
cmdline = cmdline + [write_addr, binary]

erase_file = ''
if erase_addr:
    erase_fd, erase_file = tempfile.mkstemp()
    os.write(erase_fd, b"\xff" * int(erase_len, 0))
    os.close(erase_fd)
    cmdline = cmdline + [erase_addr, erase_file]

exit_code = 0

try:
    esptool.main(cmdline)
except Exception as e:
    sys.stderr.write(f"\nA fatal upload.py error occurred: {repr(e)}\n")
    exit_code = 2

if erase_file:
    try:
        os.remove(erase_file)
    except:
        pass

sys.exit(exit_code)
