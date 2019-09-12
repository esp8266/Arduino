#!/usr/bin/env python3
# This script drops one or multiple versions of a release
#
from __future__ import print_function
import json
import sys
from collections import OrderedDict

def load_package(filename):
    if filename == "-":
        pkg = json.load(sys.stdin, object_pairs_hook=OrderedDict)['packages'][0]
    else:
        pkg = json.load(open(filename), object_pairs_hook=OrderedDict)['packages'][0]
    print("Loaded package {0} from {1}".format(pkg['name'], filename), file=sys.stderr)
    print("{0} platform(s), {1} tools".format(len(pkg['platforms']), len(pkg['tools'])), file=sys.stderr)
    return pkg

# There's probably a lambda way of doing this, but I can't figure it out...
def drop_version(todrop, obj):
    out = [];
    for o in obj:
        version = o['version'].encode('ascii')
        if version == todrop:
            print("Dropping version {0}".format(todrop), file=sys.stderr)
        else:
            out.append(o)
    return out

def main(args):
    if len(args) < 3:
        print("Usage: {0} <inpackage> <section> <version-to-remove> ...".format(args[0]), file=sys.stderr)
        return 1

    pkg = load_package(args[1])
    section = args[2]
    sub = pkg[section]
    for ver in args[3:]:
        sub = drop_version(ver, sub)
    pkg[section] = sub

    json.dump({'packages':[pkg]}, sys.stdout, indent=2)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
