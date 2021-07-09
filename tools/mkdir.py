#!/usr/bin/env python3

# Platform-independent `mkdir`

import argparse
import pathlib
import sys

def IsPythonVersionBelow3_5():
    v = sys.version_info
    if v[ 0 ] > 3 or ( v[ 0 ] == 3 and v[ 1 ] > 4 ):
      return False
    return True

def mkdir_3_4( p, parents_ ):
    try:
      pathlib.Path( p ).mkdir( parents = parents_ )
    except FileExistsError:
      pass

def mkdir_3_5( p, parents_ ):
    pathlib.Path( p ).mkdir( parents = parents_, exist_ok = True )

def main():
    parser = argparse.ArgumentParser(description='Platform-independent `mkdir`')
    parser.add_argument('-p', '--parents', action='store_true', required=False, help='no error if existing, make parent directories as needed')
    parser.add_argument('dir', action='store', nargs='+')
    ns = parser.parse_args()
    if IsPythonVersionBelow3_5():
      mkdir_ = mkdir_3_4
    else:
      mkdir_ = mkdir_3_5
    for p in ns.dir:
      mkdir_( p, ns.parents )
    return 0

if __name__ == '__main__':
    sys.exit(main())
