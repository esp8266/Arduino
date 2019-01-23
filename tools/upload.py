#!/usr/bin/env python

# Wrapper for Arduino core / others that can call esptool.py possibly multiple times
# Adds pyserial to sys.path automatically based on the path of the current file

# First patameter is pyserial path, then a series of command arguments separated with --end
# i.e. upload.py tools/pyserial tools/esptool/esptool.py erase_flash --end write_flash file 0x0 --end

import inspect
import os
import sys

sys.argv.pop(0) # Remove executable name
sys.path.append(sys.argv.pop(0).replace('\\', '/')) # Add pyserial dir to search path, in UNIX format
esptool = sys.argv.pop(0).replace('\\', '/') # Full path to esptool.py, in UNIX format

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
        fakeargs = fakeargs + [sys.argv.pop(0)]
