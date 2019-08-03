#!/usr/bin/env python3
# This script merges two Arduino Board Manager package json files.
# Usage:
#   python3 merge_packages.py package_esp8266com_index.json version/new/package_esp8266com_index.json
# Written by Ivan Grokhotkov, 2015
#
import json
import sys

def load_package(filename):
    pkg = json.load(open(filename))['packages'][0]
    print("Loaded package {0} from {1}".format(pkg['name'], filename), file=sys.stderr)
    print("{0} platform(s), {1} tools".format(len(pkg['platforms']), len(pkg['tools'])), file=sys.stderr)
    return pkg

def merge_objects(versions, obj):
    for o in obj:
        name = o['name'].encode('ascii')
        ver = o['version'].encode('ascii')
        if not name in versions:
            print("found new object, {0}".format(name), file=sys.stderr)
            versions[name] = {}
        if not ver in versions[name]:
            print("found new version {0} for object {1}".format(ver, name), file=sys.stderr)
            versions[name][ver] = o
    return versions


def main(args):
    if len(args) < 3:
        print("Usage: {0} <package1> <package2>".format(args[0]), file=sys.stderr)
        return 1

    tools = {}
    platforms = {}
    pkg1 = load_package(args[1])
    tools = merge_objects(tools, pkg1['tools']);
    platforms = merge_objects(platforms, pkg1['platforms']);
    pkg2 = load_package(args[2])
    tools = merge_objects(tools, pkg2['tools']);
    platforms = merge_objects(platforms, pkg2['platforms']);

    pkg1['tools'] = []
    pkg1['platforms'] = []

    for name in tools:
        for version in tools[name]:
            print("Adding tool {0}-{1}".format(name, version), file=sys.stderr)
            pkg1['tools'].append(tools[name][version])

    for name in platforms:
        for version in platforms[name]:
            print("Adding platform {0}-{1}".format(name, version), file=sys.stderr)
            pkg1['platforms'].append(platforms[name][version])

    json.dump({'packages':[pkg1]}, sys.stdout, indent=2)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
