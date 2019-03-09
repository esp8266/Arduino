#!/usr/bin/env python

# Wrapper for Arduino core / others that can call esptool.py possibly multiple times
# Adds pyserial to sys.path automatically based on the path of the current file

# First parameter is pyserial path, second is esptool path, then a series of command arguments separated with --end
# i.e. upload.py tools/pyserial tools/esptool erase_flash --end write_flash file 0x0 --end

import sys
import os

sys.argv.pop(0) # Remove executable name
toolspath = os.path.dirname(os.path.realpath(__file__)).replace('\\', '/') # CWD in UNIX format
try:
    sys.path.insert(0, toolspath + "/pyserial") # Add pyserial dir to search path
    sys.path.insert(0, toolspath + "/esptool") # Add esptool dir to search path
    import esptool # If this fails, we can't continue and will bomb below
except:
    sys.stderr.write("Error in command line, need pyserial path as 1st arg and esptool path as 2nd.\n")
    sys.exit(1)

fakeargs = [];
while len(sys.argv):
    if sys.argv[0] == '--end':
        esptool.main(fakeargs)
        sys.argv.pop(0) # Remove --end
        fakeargs = []
    else:
        # We silently replace the 921kbaud setting with 460k to enable backward
        # compatibility with the old esptool-ck.exe.  Esptool.py doesn't seem
        # work reliably at 921k, but is still significantly faster at 460kbaud.
        thisarg = sys.argv.pop(0)
        if thisarg == "921600":
            thisarg = "460800"
        fakeargs = fakeargs + [thisarg]
