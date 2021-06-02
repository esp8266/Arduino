#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# prebuild.py — create build_opt.h file in build directory if it does not exist
#
# This script will create a folder called "sketch" in the build path if it does
# not yet exist and create an empty build options file inside if this does also
# not exist yet
#
# Written by brainelectronics, 2021.
#

import os
import sys


def create_sketch_dir(build_path):
    sketch_build_path = os.path.join(build_path, "sketch")

    if not os.path.exists(sketch_build_path):
        os.makedirs(sketch_build_path)


def create_build_options_file(source_path, build_path):
    file_source_path = os.path.join(source_path, "build_opt.h")
    file_build_path = os.path.join(build_path, "sketch", "build_opt.h")

    if not os.path.exists(file_source_path):
        open(file_build_path, 'a').close()


def main():
    if len(sys.argv) == 3:
        build_path = sys.argv[1]
        source_path = sys.argv[2]

        create_sketch_dir(build_path)
        create_build_options_file(source_path, build_path)


if __name__ == '__main__':
    sys.exit(main())
