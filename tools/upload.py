#!/usr/bin/env python3

# Wrapper for Arduino core / others that can call esptool.py possibly multiple times
# Adds pyserial to sys.path automatically based on the path of the current file

# First parameter is pyserial path, second is esptool path, then a series of command arguments separated with --end
# i.e. upload.py tools/pyserial tools/esptool erase_flash --end write_flash file 0x0 --end

import sys
import os
import tempfile

sys.argv.pop(0) # Remove executable name
toolspath = os.path.dirname(os.path.realpath(__file__)).replace('\\', '/') # CWD in UNIX format
try:
    sys.path.insert(0, toolspath + "/pyserial") # Add pyserial dir to search path
    sys.path.insert(0, toolspath + "/esptool") # Add esptool dir to search path
    import esptool # If this fails, we can't continue and will bomb below
except:
    sys.stderr.write("pyserial or esptool directories not found next to this upload.py tool.\n")
    sys.exit(1)

cmdline = []
write_option = ''
write_addr = '0x0'
erase_addr = ''
erase_len = ''

while len(sys.argv):
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
    elif thisarg == 'erase_region':
        erase_addr = sys.argv.pop(0)
        erase_len = sys.argv.pop(0)
    elif thisarg == '--end':
        # Backwards compatibility with fs upload tools, eat --end
        pass
    elif thisarg == 'write_flash':
        write_addr = sys.argv.pop(0)
        binary = sys.argv.pop(0)
    elif len(thisarg):
        cmdline = cmdline + [thisarg]

cmdline = cmdline + ['write_flash']
if len(write_option):
    cmdline = cmdline + [write_option]
cmdline = cmdline + [write_addr, binary]

erase_file = ''
if len(erase_addr):
    # Generate temporary empty (0xff) file
    eraser = tempfile.mkstemp()
    erase_file = eraser[1]
    os.write(eraser[0], bytearray([255] * int(erase_len, 0)))
    os.close(eraser[0])
    cmdline = cmdline + [ erase_addr, erase_file ]

esptool.main(cmdline)

if len(erase_file):
    os.remove(erase_file)
