#!/usr/bin/env python3

# Platform-independent single-file `cp`

import argparse
import shutil
import sys

def main():
    parser = argparse.ArgumentParser(description="Platform-independent single-file `cp`")
    parser.add_argument("src", action="store")
    parser.add_argument("dst", action="store")
    ns = parser.parse_args()
    shutil.copyfile(ns.src, ns.dst)
    return 0

if __name__ == '__main__':
    sys.exit(main())
