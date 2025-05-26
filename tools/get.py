#!/usr/bin/env python3
# This script will download and extract required tools into the current directory.
# Tools list is obtained from package/package_esp8266com_index.template.json file.
# Originally written by Ivan Grokhotkov, 2015.

import argparse
import shutil
import hashlib
import json
import pathlib
import platform
import sys
import tarfile
import zipfile
import re

from typing import Optional, Literal, List
from urllib.request import urlretrieve


PWD = pathlib.Path(__file__).parent

if sys.version_info >= (3, 12):
    TARFILE_EXTRACT_ARGS = {"filter": "data"}
else:
    TARFILE_EXTRACT_ARGS = {}

PLATFORMS = {
    "Darwin": {32: "i386-apple-darwin", 64: "x86_64-apple-darwin"},
    "DarwinARM": {32: "arm64-apple-darwin", 64: "arm64-apple-darwin"},
    "Linux": {32: "i686-pc-linux-gnu", 64: "x86_64-pc-linux-gnu"},
    "LinuxARM": {32: "arm-linux-gnueabihf", 64: "aarch64-linux-gnu"},
    "Windows": {32: "i686-mingw32", 64: "x86_64-mingw32"},
}


class HashMismatch(Exception):
    pass


def sha256sum(p: pathlib.Path, blocksize=65536):
    hasher = hashlib.sha256()
    with p.open("rb") as f:
        for block in iter(lambda: f.read(blocksize), b""):
            hasher.update(block)

    return hasher.hexdigest()


def report_progress(count, blockSize, totalSize):
    percent = int(count * blockSize * 100 / totalSize)
    percent = min(100, percent)
    print(f"\r{percent}%", end="", file=sys.stdout, flush=True)


def unpack(p: pathlib.Path, destination: pathlib.Path):
    outdir = None  # type: Optional[pathlib.Path]

    print(f"Extracting {p}")
    if p.suffix == ".zip":
        zfile = zipfile.ZipFile(p)
        zfile.extractall(destination)
        outdir = destination / zfile.namelist()[0]
    else:
        tfile = tarfile.open(p, "r:*")
        tfile.extractall(destination, **TARFILE_EXTRACT_ARGS)  # type: ignore
        outdir = destination / tfile.getnames()[0]

    if not outdir:
        raise NotImplementedError(f"Unsupported archive type {p.suffix}")

    # a little trick to rename tool directories so they don't contain version number
    match = re.match(r"^([a-zA-Z_][^\-]*\-*)+", outdir.name)
    if match:
        rename_to = match.group(0).strip("-")
    else:
        rename_to = outdir.name

    if outdir.name != rename_to:
        print(f"Renaming {outdir.name} to {rename_to}")
        destdir = destination / rename_to
        if destdir.is_dir():
            shutil.rmtree(destdir)
        shutil.move(outdir, destdir)


# ref. https://docs.arduino.cc/arduino-cli/package_index_json-specification/
def get_tool(tool: dict, *, dist_dir: pathlib.Path, quiet: bool, dry_run: bool):
    archive_name = tool["archiveFileName"]
    local_path = dist_dir / archive_name

    url = tool["url"]
    algorithm, real_hash = tool["checksum"].split(":", 1)
    if algorithm != "SHA-256":
        raise NotImplementedError(f"Unsupported hash algorithm {algorithm}")

    if dry_run:
        print(f'{archive_name} ({tool.get("size")} bytes): {url}')
    else:
        if not quiet:
            reporthook = report_progress
        else:
            reporthook = None

        if not local_path.is_file():
            print(f"Downloading {archive_name}")
            urlretrieve(url, local_path, reporthook)
            print("\rDone", file=sys.stdout, flush=True)
        else:
            print(
                f"Tool {archive_name} ({local_path.stat().st_size} bytes) already downloaded"
            )

    if not dry_run or (dry_run and local_path.exists()):
        local_hash = sha256sum(local_path)
        if local_hash != real_hash:
            raise HashMismatch(
                f"Expected {local_hash}, got {real_hash}. Delete {local_path} and try again"
            ) from None

    if not dry_run:
        unpack(local_path, PWD / ".")


def load_tools_list(package_index_json: pathlib.Path, hosts: List[str]):
    out = []

    with package_index_json.open("r") as f:
        root = json.load(f)

        package = root["packages"][0]
        tools = package["tools"]

        for info in tools:
            found = [p for p in info["systems"] for host in hosts if p["host"] == host]
            found.sort(key=lambda p: hosts.index(p["host"]))
            if found:
                out.append(found[0])

    return out


def select_host(
    sys_name: Optional[str],
    sys_platform: Optional[str],
    bits: Optional[Literal[32, 64]],
) -> List[str]:
    if not sys_name:
        sys_name = platform.system()

    if not sys_platform:
        sys_platform = platform.platform()

    if not bits:
        bits = 32
        if sys.maxsize > 2**32:
            bits = 64

    def maybe_arm(s: str) -> bool:
        return (s.find("arm") > 0) or (s.find("aarch64") > 0)

    if "Darwin" in sys_name and maybe_arm(sys_platform):
        sys_name = "DarwinARM"
    elif "Linux" in sys_name and maybe_arm(sys_platform):
        sys_name = "LinuxARM"
    elif "CYGWIN_NT" in sys_name or "MSYS_NT" in sys_name or "MINGW" in sys_name:
        sys_name = "Windows"

    out = [
        PLATFORMS[sys_name][bits],
    ]

    if sys_name == "DarwinARM":
        out.append(PLATFORMS["Darwin"][bits])

    return out


def main(args: argparse.Namespace):
    # #6960 - Remove a symlink generated in 2.6.3 which causes later issues since the tarball can't properly overwrite it
    py3symlink = PWD / "python3" / "python3"
    if py3symlink.is_symlink():
        py3symlink.unlink()

    host = args.host
    if not host:
        host = select_host(
            sys_name=args.system,
            sys_platform=args.platform,
            bits=args.bits,
        )

    print(f"Platform: {', '.join(host)}")

    tools_to_download = load_tools_list(args.package_index_json, host)
    if args.tool:
        tools_to_download = [
            tool
            for tool in tools_to_download
            for exclude in args.tool
            if exclude in tool["archiveFileName"]
        ]

    for tool in tools_to_download:
        get_tool(
            tool,
            dist_dir=args.dist_dir,
            quiet=args.quiet,
            dry_run=args.dry_run,
        )


def parse_args(args: Optional[str] = None, namespace=argparse.Namespace):
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )

    parser.add_argument("-q", "--quiet", action="store_true", default=False)
    parser.add_argument("-d", "--dry-run", action="store_true", default=False)
    parser.add_argument("-t", "--tool", action="append", type=str)

    parser.add_argument("--host", type=str, action="append")
    parser.add_argument("--system", type=str)
    parser.add_argument("--platform", type=str)
    parser.add_argument("--bits", type=int, choices=PLATFORMS["Linux"].keys())

    parser.add_argument(
        "--no-progress", dest="quiet", action="store_true", default=False
    )
    parser.add_argument("--dist-dir", type=pathlib.Path, default=PWD / "dist")
    parser.add_argument(
        "--package-index-json",
        type=pathlib.Path,
        default=PWD / ".." / "package/package_esp8266com_index.template.json",
    )

    return parser.parse_args(args, namespace)


if __name__ == "__main__":
    main(parse_args())
