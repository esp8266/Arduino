#!/usr/bin/env python3

# Wrapper for Arduino core / others that can call esptool.py possibly multiple times
# Adds pyserial to sys.path automatically based on the path of the current file

# First parameter is pyserial path, second is esptool path, then a series of command arguments
# i.e. upload.py tools/pyserial tools/esptool write_flash file 0x0


import argparse
import sys
import tempfile

from pathlib import Path
from typing  import List, Optional, Tuple

TOOLS_PATH    = Path(__file__).resolve().parent
PYSERIAL_PATH = TOOLS_PATH / "pyserial"
ESPTOOL_PATH  = TOOLS_PATH / "esptool"

DEFAULT_BAUD_RATE  = "460800"
DEFAULT_WRITE_ADDR = "0x0"

def setup_path() -> None:
    sys.path.insert(0, str(PYSERIAL_PATH))
    sys.path.insert(0, str(ESPTOOL_PATH))

def import_esptool():
    try:
        import esptool
        return esptool
    
    except ImportError:
        sys.stderr.write("PySerial or ESPtool directories not found next to this upload.py tool.\n")
        sys.exit(1)

def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Wrapper for Arduino core / others that can call esptool.py")

    # I like to space these out to better see what each argument does.

    parser.add_argument("--baud"         , default=DEFAULT_BAUD_RATE,                         help="Baud rate for serial communication")
    parser.add_argument("--erase-flash"  , action="store_true",                               help="Erase the entire flash")
    parser.add_argument("--erase-region" , nargs=2,                 metavar=("ADDR", "LEN"),  help="Erase a specific region of flash")
    parser.add_argument("--write-flash"  , nargs=2,                 metavar=("ADDR", "FILE"), help="Write a file to flash")
    parser.add_argument("additional_args", nargs=argparse.REMAINDER,                          help="Additional arguments to pass to esptool.py")
    
    return parser.parse_args()

def process_baud_rate(baud_rate: str) -> str:
    return "460800" if baud_rate == "921600" else baud_rate

def create_erase_file(erase_len: str) -> Path:

    with tempfile.NamedTemporaryFile(delete=False) as temp_file:

        temp_file.write(bytearray([0xff] * int(erase_len, 0)))

    return Path(temp_file.name)

def build_command_line(args: argparse.Namespace) -> Tuple[List[str], Optional[Path]]:

    cmdline = args.additional_args + ["write_flash"]
    
    if args.erase_flash:

        cmdline.append("--erase-all")
    
    cmdline.extend(["--flash_size", "detect"])
    
    if args.write_flash:

        cmdline.extend([args.write_flash[0], args.write_flash[1]])
    
    erase_file = None

    if args.erase_region:

        erase_file = create_erase_file(args.erase_region[1])
        
        cmdline.extend([args.erase_region[0], str(erase_file)])
    
    return cmdline, erase_file

def main():

    setup_path()

    esptool = import_esptool()
    
    args = parse_arguments()
    args.baud = process_baud_rate(args.baud)
    
    cmdline, erase_file = build_command_line(args)
    
    try:
        esptool.main(cmdline)
    except Exception as e:
        sys.stderr.write(f'\nA fatal esptool.py error occurred: {e}')
    finally:
        if erase_file:
            erase_file.unlink()
        if any(sys.exc_info()):
            sys.exit(2)

if __name__ == "__main__":
    main()