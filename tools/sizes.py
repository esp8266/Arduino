#!/usr/bin/env python3

import argparse
import subprocess
import sys
import locale
from pathlib import Path
from typing import List, Dict, Tuple, NamedTuple
from enum import Enum
from dataclasses import dataclass

# Constants for default sizes
ICACHE_DEFAULT_SIZE = 32168
DEFAULT_TOTAL_RAM =   80192
DEFAULT_TOTAL_IRAM =  65536
DEFAULT_TOTAL_IROM =  1048576

# Redirect stdout to stderr as per original design
sys.stdout = sys.stderr

# Enum to represent different sections in the ELF file
class Section(Enum):
    IROM =   ".irom0.text"
    IRAM =   ".text"
    DATA =   ".data"
    RODATA = ".rodata"
    BSS =    ".bss"

# Data structure to hold segment information
@dataclass
class SegmentInfo:
    name: str
    size: int
    total: int
    hint: str

# Named tuple for mapping sections to their targets
class SectionMapping(NamedTuple):
    section: Section
    target: str

# Define the mapping between sections and their targets
SECTION_MAPPING = [
    SectionMapping(Section.IROM,   "IROM"),
    SectionMapping(Section.IRAM,   "IRAM"),
    SectionMapping(Section.DATA,   "DATA"),
    SectionMapping(Section.RODATA, "RODATA"),
    SectionMapping(Section.BSS,    "BSS"),
]

# Descriptions for each segment type
HINTS = {
    "ICACHE": "reserved space for flash instruction cache",
    "IRAM":   "code in IRAM",
    "IHEAP":  "secondary heap space",
    "IROM":   "code in flash",
    "DATA":   "initialized variables",
    "RODATA": "constants",
    "BSS":    "zeroed variables",
}

def get_encoding():
    
    if sys.version_info >= (3, 11):
        return locale.getencoding()
    return locale.getdefaultlocale()[1]

def get_segment_sizes(elf: Path, toolchain_path: Path, mmu: str) -> List[Tuple[str, Dict[str, int], int]]:
    
    # Analyze the ELF file and return segment sizes.
    # This function parses MMU options and runs the xtensa-lx106-elf-size tool.
    
    iram_size   = 0
    iheap_size  = 0
    icache_size = ICACHE_DEFAULT_SIZE

    # Parse MMU options
    for line in mmu.split():
        words = line.split("=")
        if line.startswith("-DMMU_IRAM_SIZE"):
            iram_size = int(words[1], 16)
        elif line.startswith("-DMMU_ICACHE_SIZE"):
            icache_size = int(words[1], 16)
        elif line.startswith("-DMMU_SEC_HEAP_SIZE"):
            iheap_size = int(words[1], 16)

    # Initialize sizes for different memory segments
    sizes = [
        ("Variables and constants in RAM (global, static)",
         {"DATA": 0, "RODATA": 0, "BSS": 0},
         DEFAULT_TOTAL_RAM),
        ("Instruction RAM (IRAM_ATTR, ICACHE_RAM_ATTR)",
         {"ICACHE": icache_size, "IHEAP": iheap_size, "IRAM": 0},
         DEFAULT_TOTAL_IRAM),
        ("Code in flash (default, ICACHE_FLASH_ATTR)",
         {"IROM": 0},
         DEFAULT_TOTAL_IROM),
    ]

    # Run xtensa-lx106-elf-size and parse its output
    cmd = [toolchain_path / "xtensa-lx106-elf-size", "-A", str(elf)]
    
    try:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True, encoding=get_encoding())
        with proc:
            lines = proc.stdout.readlines()
            for line in lines:
                words = line.split()
                for section, target in SECTION_MAPPING:
                    if not line.startswith(section.value):
                        continue
                    for _, (segments, total) in sizes:
                        if target in segments:
                            segments[target] += int(words[1])
                            assert segments[target] <= total

    except subprocess.CalledProcessError as e:
        print(f"Error running xtensa-lx106-elf-size: {e}", file=sys.stderr)
        sys.exit(1)

    return sizes

def percentage(lhs: int, rhs: int) -> str:
    """Calculate and format percentage."""
    return f"{int(100.0 * float(lhs) / float(rhs))}%"

def safe_prefix(n: int, length: int) -> str:
    """Return a safe ASCII prefix for tree-like output."""
    return "`--" if n == length else "|--"

def prefix(n: int, length: int) -> str:
    """Return a Unicode prefix for tree-like output."""
    return "╚══" if n == length else "╠══"

def filter_segments(segments: Dict[str, int]) -> Tuple[int, int, List[Tuple[int, str, int]]]:
    """Filter and sort segments, calculating total used size."""
    used = sum(segments.values())
    available = [(i+1, segment, size) for i, (segment, size) in enumerate(segments.items()) if size]
    return (len(available), used, available)

def print_segment_info(group: str, segments: Dict[str, int], total: int):
    """Print formatted information about memory segments."""
    number, used, available_segments = filter_segments(segments)

    print(f". {group:<8}, used {used} / {total} bytes ({percentage(used, total)})")
    try:
        print("║   SEGMENT  BYTES    DESCRIPTION")
    except UnicodeEncodeError:
        print("|   SEGMENT  BYTES    DESCRIPTION")
    
    for n, segment, size in available_segments:
        try:
            prefix_str = prefix(n, number)
        except UnicodeEncodeError:
            prefix_str = safe_prefix(n, number)
        print(f"{prefix_str} {segment:<8} {size:<8} {HINTS[segment]:<16}")

def main():
    """Parse arguments and run the analysis."""
    parser = argparse.ArgumentParser(description="Report the different segment sizes of a compiled ELF file")

    parser.add_argument("-e", "--elf",  type=Path, required=True,              help="Path to the Arduino sketch ELF")
    parser.add_argument("-p", "--path", type=Path, required=True,              help="Path to Xtensa toolchain binaries")
    parser.add_argument("-i", "--mmu",  type=str,  required=False, default="", help="MMU build options")

    args = parser.parse_args()
    sizes = get_segment_sizes(args.elf, args.path, args.mmu)

    for group, segments, total in sizes:
        print_segment_info(group, segments, total)

if __name__ == "__main__":
    main()