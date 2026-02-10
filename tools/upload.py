#!/usr/bin/env python3

# Wrapper for Arduino core / others that can call esptool.py possibly multiple times
# Adds pyserial & esptool that are in the same directory as the script to sys.path

import os
import atexit
import pathlib
import sys
import tempfile
import traceback

from typing import List

# Add neighbouring pyserial & esptool to search path
MODULES = [
    "pyserial",
    "esptool",
]

PWD = pathlib.Path(__file__).resolve().parent
for m in MODULES:
    sys.path.insert(0, (PWD / m).as_posix())

# If this fails, we can't continue and will bomb below
try:
    import esptool
except ImportError:
    sys.stderr.write(
        "\n*** pyserial or esptool directories not found next to upload.py tool (this script) ***\n"
    )
    traceback.print_exc(file=sys.stderr)
    sys.stderr.flush()

    sys.exit(1)


def make_erase_pair(addr: str, dest_size: int, block_size=2**16):
    dest, path = tempfile.mkstemp()

    buffer = bytearray(b"\xff" * block_size)
    while dest_size:
        remainder = dest_size % block_size

        if remainder:
            src = buffer[:remainder]
            src_size = remainder
        else:
            src = buffer
            src_size = block_size

        os.write(dest, src)
        dest_size -= src_size

    os.close(dest)

    def maybe_remove(path):
        try:
            os.remove(path)
        except Exception:
            pass

    atexit.register(maybe_remove, path)
    return [addr, path]


argv = sys.argv[1:]  # Remove executable name

cmdline: List[str] = []
write_options: List[str] = ["--flash_size", "detect"]
erase_options: List[str] = []

thisarg = ""
lastarg = ""
while argv:
    lastarg = thisarg
    thisarg = argv.pop(0)

    # We silently replace the high-speed setting with 460k to enable backward
    # compatibility with the old esptool-ck.exe.  Esptool.py doesn't seem
    # work reliably, but 460kbaud is still plenty fast.
    if lastarg == "--baud" and thisarg in ("921600", "3000000"):
        thisarg = "460800"

    # 'erase_flash' command is translated to the write_flash --erase-all option
    # https://github.com/esp8266/Arduino/issues/6755#issuecomment-553208688
    if thisarg == "erase_flash":
        write_options.append("--erase-all")

    # instead of providing esptool with separate targets,
    # everything below becomes 'write_flash' [<addr> <path>] pairs

    # 'erase_region' becomes a temporary file filled with 0xff
    # this pair is appended *after* 'write_flash' pairs
    elif thisarg == "erase_region":
        addr = argv.pop(0)
        size = int(argv.pop(0), 0)
        erase_options.extend(make_erase_pair(addr, size))

    # 'write_flash' pair taken in order it was specified
    elif thisarg == "write_flash":
        addr = argv.pop(0)
        path = argv.pop(0)
        write_options.extend([addr, path])

    # everything else is used as-is
    elif thisarg:
        cmdline.append(thisarg)


cmdline.append("write_flash")
for opts in (write_options, erase_options):
    if opts:
        cmdline.extend(opts)

try:
    esptool.main(cmdline)
except Exception:
    etype, evalue, _ = sys.exc_info()
    estring = "\n".join(traceback.format_exception_only(etype, value=evalue))

    sys.stderr.write("\n*** upload.py fatal error ***\n")
    sys.stderr.write(estring)
    sys.stderr.flush()

    sys.exit(2)
