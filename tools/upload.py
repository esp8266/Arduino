#!/usr/bin/env python

# Wrapper for Arduino core / others that can call esptool.py possibly multiple times
# Adds pyserial to sys.path automatically based on the path of the current file

# First patameter is pyserial path, then a series of command arguments separated with --end
# i.e. upload.py tools/pyserial tools/esptool/esptool.py erase_flash --end write_flash file 0x0 --end

import inspect
import os
import sys

sys.argv.pop(0) # Remove executable name
try:
    sys.path.append(sys.argv.pop(0).replace('\\', '/')) # Add pyserial dir to search path, in UNIX format
    esptool = sys.argv.pop(0).replace('\\', '/') # Full path to esptool.py, in UNIX format
except:
    sys.stderr.write("Error in command line, need pyserial path as 1st arg and esptool path as 2nd.\n")
    sys.exit(1)

fakeargs = [];
while len(sys.argv):
    if sys.argv[0] == '--end':
        pid  = os.fork()
        if pid == 0:
            sys.argv = ['esptool.py'] + fakeargs
            sys.stderr.write("Running: " + " ".join(sys.argv) + "\n")
            exec(open(esptool).read())
            sys.exit(0)
        else:
            os.waitpid(pid, 0)
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
