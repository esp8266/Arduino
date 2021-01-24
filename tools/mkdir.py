#!/usr/bin/env python3

# Platform-independent `mkdir`

import argparse
import pathlib
import sys

def main():
    parser = argparse.ArgumentParser(description='Platform-independent `mkdir`')
    parser.add_argument('-p', '--parents', action='store_true', required=False, help='no error if existing, make parent directories as needed')
    parser.add_argument('dir', action='store', nargs='+')
    ns = parser.parse_args()
    for p in ns.dir:
        pathlib.Path(p).mkdir(parents=ns.parents, exist_ok=True)
    return 0

if __name__ == '__main__':
    sys.exit(main())
