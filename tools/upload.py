#!/usr/bin/env python3

# Wrapper for Arduino core / others that can call esptool.py possibly multiple times
# Adds pyserial to sys.path automatically based on the path of the current file

# First parameter is pyserial path, second is esptool path, then a series of command arguments
# i.e. upload.py tools/pyserial tools/esptool write_flash file 0x0

import argparse
import sys
import tempfile

from pathlib import Path
from typing  import List, Any, Union, Tuple

TOOLS_PATH    = Path(__file__).resolve().parent
PYSERIAL_PATH = TOOLS_PATH / "pyserial"
ESPTOOL_PATH  = TOOLS_PATH / "esptool"

DEFAULT_BAUD_RATE  = "460800"
DEFAULT_WRITE_ADDR = "0x0"

def setup_path() -> None:
    sys.path.insert(0, str(PYSERIAL_PATH))
    sys.path.insert(0, str(ESPTOOL_PATH))

def import_esptool() -> Any:
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
     # Swapping out 921600 for 460800.
    return "460800" if baud_rate == "921600" else baud_rate

def create_erase_file(erase_len: str) -> Path:

    try:
        # Creates a temporary file filled with 0xFF bytes to overwrite the stipulated region.

        erase_length = int(erase_len, 0)

        with tempfile.NamedTemporaryFile(delete=False) as temp_file:
            temp_file.write(bytearray([0xff] * erase_length))

        return Path(temp_file.name)
    
    except ValueError:
        # Errors on a funky erase length.
        sys.stderr.write(f"Invalid erase length: {erase_len}\n")

        sys.exit(1)


def validate_write_flash_args(addr: str, file_path: str) -> None:
    try:
        int(addr, 0)  
    except ValueError:
        sys.stderr.write(f"Invalid flash address: {addr}\n")
        sys.exit(1)
    
    if not Path(file_path).is_file():
        sys.stderr.write(f"Flash file not found: {file_path}\n")
        sys.exit(1)

def build_command_line(args: argparse.Namespace) -> Tuple[List[str], Union[Path, None]]:
    cmdline = args.additional_args + ["write_flash"]
    
    if args.erase_flash:
        cmdline.append("--erase-all")

    # This tells esptool to figure out the flash size on its own.
    cmdline.extend(["--flash_size", "detect"])
    
    if args.write_flash:
        validate_write_flash_args(args.write_flash[0], args.write_flash[1])
        cmdline.extend(args.write_flash)
    
    erase_file = None
    if args.erase_region:
        erase_file = create_erase_file(args.erase_region[1])
        cmdline.extend([args.erase_region[0], str(erase_file)])
    
    return cmdline, erase_file

def main() -> None:
    # First things first, let's set up our path and import esptool
    setup_path()
    esptool = import_esptool()

    # Now we parse the arguments and adjust the baud rate if necessary    
    args = parse_arguments()
    args.baud = process_baud_rate(args.baud)
    
    # Build our command line arguments
    cmdline, erase_file = build_command_line(args)
    
    try:
        # Run esptool with our arguments
        esptool.main(cmdline)

    except Exception as e:
        # Something went wrong with esptool, prints error
        sys.stderr.write(f'\nA fatal esptool.py error occurred: {e}\n')
        sys.exit(2)

    finally:
        # If we created an erase file, let's remove it.
        # The 'missing_ok=True' means we won't get an error if the file is already gone.
        if erase_file:
            erase_file.unlink(missing_ok=True)

if __name__ == "__main__":
    main()
