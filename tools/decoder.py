#!/usr/bin/env python3

# Baseline code from https://github.com/me-no-dev/EspExceptionDecoder by Hristo Gochkov (@me-no-dev)
# - https://github.com/me-no-dev/EspExceptionDecoder/blob/master/src/EspExceptionDecoder.java
# Stack line detection from https://github.com/platformio/platform-espressif8266/ monitor exception filter by Vojtěch Boček (@Tasssadar)
# - https://github.com/platformio/platform-espressif8266/commits?author=Tasssadar

import os
import argparse
import sys
import re
import subprocess
import shutil

# https://github.com/me-no-dev/EspExceptionDecoder/blob/349d17e4c9896306e2c00b4932be3ba510cad208/src/EspExceptionDecoder.java#L59-L90
EXCEPTION_CODES = (
    "Illegal instruction",
    "SYSCALL instruction",
    "InstructionFetchError: Processor internal physical address or data error during "
    "instruction fetch",
    "LoadStoreError: Processor internal physical address or data error during load or store",
    "Level1Interrupt: Level-1 interrupt as indicated by set level-1 bits in "
    "the INTERRUPT register",
    "Alloca: MOVSP instruction, if caller's registers are not in the register file",
    "IntegerDivideByZero: QUOS, QUOU, REMS, or REMU divisor operand is zero",
    "reserved",
    "Privileged: Attempt to execute a privileged operation when CRING ? 0",
    "LoadStoreAlignmentCause: Load or store to an unaligned address",
    "reserved",
    "reserved",
    "InstrPIFDataError: PIF data error during instruction fetch",
    "LoadStorePIFDataError: Synchronous PIF data error during LoadStore access",
    "InstrPIFAddrError: PIF address error during instruction fetch",
    "LoadStorePIFAddrError: Synchronous PIF address error during LoadStore access",
    "InstTLBMiss: Error during Instruction TLB refill",
    "InstTLBMultiHit: Multiple instruction TLB entries matched",
    "InstFetchPrivilege: An instruction fetch referenced a virtual address at a ring level "
    "less than CRING",
    "reserved",
    "InstFetchProhibited: An instruction fetch referenced a page mapped with an attribute "
    "that does not permit instruction fetch",
    "reserved",
    "reserved",
    "reserved",
    "LoadStoreTLBMiss: Error during TLB refill for a load or store",
    "LoadStoreTLBMultiHit: Multiple TLB entries matched for a load or store",
    "LoadStorePrivilege: A load or store referenced a virtual address at a ring level "
    "less than CRING",
    "reserved",
    "LoadProhibited: A load referenced a page mapped with an attribute that does not "
    "permit loads",
    "StoreProhibited: A store referenced a page mapped with an attribute that does not "
    "permit stores",
)


# similar to java version, which used `list` and re-formatted it
# instead, simply use an already short-format `info line`
# TODO `info symbol`? revert to `list`?
def addresses_gdb(gdb, elf, addresses):
    cmd = [gdb, "--batch"]
    for address in addresses:
        if not address.startswith("0x"):
            address = f"0x{address}"
        cmd.extend(["--ex", f"info line *{address}"])
    cmd.append(elf)

    with subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True) as proc:
        for line in proc.stdout.readlines():
            if "No line number" in line:
                continue
            yield line.strip()


# original approach using addr2line, which is pretty enough already
def addresses_addr2line(addr2line, elf, addresses):
    cmd = [
        addr2line,
        "--addresses",
        "--inlines",
        "--functions",
        "--pretty-print",
        "--demangle",
        "--exe",
        elf,
    ]

    for address in addresses:
        if not address.startswith("0x"):
            address = f"0x{address}"
        cmd.append(address)

    with subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True) as proc:
        for line in proc.stdout.readlines():
            if "??:0" in line:
                continue
            yield line.strip()


def decode_lines(format_addresses, elf, lines):
    ANY_ADDR_RE = re.compile(r"0x[0-9a-fA-F]{8}|[0-9a-fA-F]{8}")
    HEX_ADDR_RE = re.compile(r"0x[0-9a-f]{8}")

    MEM_ERR_LINE_RE = re.compile(r"^(Stack|last failed alloc call)")

    STACK_LINE_RE = re.compile(r"^[0-9a-f]{8}:\s\s+")

    IGNORE_FIRMWARE_RE = re.compile(r"^(epc1=0x........, |Fatal exception )")

    CUT_HERE_STRING = "CUT HERE FOR EXCEPTION DECODER"
    DECODE_IT = "DECODE IT"
    EXCEPTION_STRING = "Exception ("
    EPC_STRING = "epc1="

    # either print everything as-is, or cache current string and dump after stack contents end
    last_stack = None
    stack_addresses = {}

    in_stack = False

    def print_all_addresses(addresses):
        for ctx, addrs in addresses.items():
            print()
            print(ctx)
            for formatted in format_addresses(elf, addrs):
                print(formatted)
        return dict()

    def format_address(address):
        return "\n".join(format_addresses(elf, [address]))

    for line in lines:
        # ctx could happen multiple times. for the 2nd one, reset list
        # ctx: bearssl *or* ctx: cont *or* ctx: sys *or* ctx: whatever
        if in_stack and "ctx:" in line:
            stack_addresses = print_all_addresses(stack_addresses)
            last_stack = line.strip()
        # 3fffffb0:  feefeffe feefeffe 3ffe85d8 401004ed
        elif IGNORE_FIRMWARE_RE.match(line):
            continue
        elif in_stack and STACK_LINE_RE.match(line):
            _, addrs = line.split(":")
            addrs = ANY_ADDR_RE.findall(addrs)
            stack_addresses.setdefault(last_stack, [])
            stack_addresses[last_stack].extend(addrs)
        # epc1=0xfffefefe epc2=0xfefefefe epc3=0xefefefef excvaddr=0xfefefefe depc=0xfefefefe
        elif EPC_STRING in line:
            pairs = line.split()
            for pair in pairs:
                name, addr = pair.split("=")
                if name in ["epc1", "excvaddr"]:
                    output = format_address(addr)
                    if output:
                        print(f"{name}={output}")
        # Exception (123):
        # Other reasons coming before the guard shown as-is
        elif EXCEPTION_STRING in line:
            number = line.strip()[len(EXCEPTION_STRING) : -2]
            print(f"Exception ({number}) - {EXCEPTION_CODES[int(number)]}")
        # stack smashing detected at <ADDR>
        # last failed alloc call: <ADDR>(<NUMBER>)[@<maybe file loc>]
        elif MEM_ERR_LINE_RE.match(line):
            for addr in ANY_ADDR_RE.findall(line):
                line = line.replace(addr, format_address(addr))
            print()
            print(line.strip())
        # postmortem guards our actual stack dump values with these
        elif ">>>stack>>>" in line:
            in_stack = True
        # ignore
        elif "<<<stack<<<" in line:
            in_stack = False
            stack_addresses = print_all_addresses(stack_addresses)
            continue
        elif CUT_HERE_STRING in line or DECODE_IT in line:
            continue
        else:
            line = line.strip()
            if line:
                print(line)

    print_all_addresses(stack_addresses)


TOOLS = {"gdb": addresses_gdb, "addr2line": addresses_addr2line}


def select_tool(toolchain_path, tool, func):
    path = f"xtensa-lx106-elf-{tool}"
    if toolchain_path:
        path = os.path.join(toolchain_path, path)
        if not shutil.which(path):
            raise FileNotFoundError(path)

    def formatter(func, path):
        def wrapper(elf, addresses):
            return func(path, elf, addresses)

        return wrapper

    return formatter(func, path)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--tool", choices=TOOLS, default="addr2line")
    parser.add_argument(
        "--toolchain-path", help="Sets path to Xtensa tools, when they are not in PATH"
    )

    parser.add_argument("firmware_elf")
    parser.add_argument(
        "postmortem", nargs="?", type=argparse.FileType("r"), default=sys.stdin
    )

    args = parser.parse_args()
    decode_lines(
        select_tool(args.toolchain_path, args.tool, TOOLS[args.tool]),
        args.firmware_elf,
        args.postmortem,
    )
