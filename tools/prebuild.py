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
    """
    Create the sketch directory
    :param      build_path:  The path to the build directory
    :type       build_path:  str
    """
    sketch_build_path = os.path.join(build_path, "sketch")

    if not os.path.exists(sketch_build_path):
        os.makedirs(sketch_build_path)


def create_build_options_file(source_path, build_path, build_opt_name="build_opt.h"):
    """
    Create the build options file in the build directory.
    The modification time of the build options file is set to the sketch
    modification time in case the file did not exist or the users build options
    modification time to reflect changes which require a recompilation
    :param      source_path:     The path to the source directory
    :type       source_path:     str
    :param      build_path:      The path to the build directory
    :type       build_path:      str
    :param      build_opt_name:  The build option file name
    :type       build_opt_name:  str, optional
    """
    build_opt_source_path = os.path.join(source_path, build_opt_name)
    build_opt_build_path = os.path.join(build_path, "sketch", build_opt_name)

    # check for an existing build options file in the source directory
    if os.path.exists(build_opt_source_path):
        # user does have/use an build options file in it's sketch directory

        if os.path.exists(build_opt_build_path):
            # set build options file modification time to the same time as the
            # sketch modification time to avoid rebuilding libraries
            set_file_time_to_sketch_time(source_path, build_opt_build_path)
        else:
            # the build options file does not yet exist in the build directory
            # just continue as it will be copied by the IDE automatically on
            # time. This section is only entered on the very first run
            pass
    else:
        # user does not have a build options file, create empty one to avoid
        # compilation error due to missing file for compilation
        open(build_opt_build_path, 'a').close()

        # set build options file modification time to the same time as the
        # sketch modification time to avoid rebuilding libraries
        set_file_time_to_sketch_time(source_path, build_opt_build_path)


def set_file_time_to_sketch_time(source_path, destination_path):
    """
    Set the file ctime and mtime to the ctime and mtime of the sketch.
    :param      source_path:       The source path to the sketch
    :type       source_path:       str
    :param      destination_path:  The destination path to the file to modify
    :type       destination_path:  str
    """
    sketch_path = get_full_sketch_path(source_path)
    sketch_mtime = get_modification_time(sketch_path)
    destination_ctime = get_creation_time(destination_path)

    os.utime(destination_path, (destination_ctime, sketch_mtime))


def get_full_sketch_path(source_path):
    """
    Get the full sketch path.
    :param      source_path:  The source path
    :type       source_path:  str
    :returns:   The full sketch path including the '.ino' extension.
    :rtype:     str
    """
    base_name = os.path.basename(source_path)

    sketch_path = os.path.join(source_path, base_name + '.ino')

    return sketch_path


def get_modification_time(file_path):
    """
    Get the modification time of a file.
    :param      file_path:  The file path
    :type       file_path:  str
    :returns:   The modification time.
    :rtype:     float
    """
    return os.path.getmtime(file_path)


def get_creation_time(file_path):
    """
    Gets the creation time if a file.
    :param      file_path:  The file path
    :type       file_path:  str
    :returns:   The creation time.
    :rtype:     float
    """
    return os.path.getctime(file_path)


def main():
    if len(sys.argv) == 3:
        build_path = sys.argv[1]
        source_path = sys.argv[2]

        create_sketch_dir(build_path)
        create_build_options_file(source_path, build_path)


if __name__ == '__main__':
    sys.exit(main())
