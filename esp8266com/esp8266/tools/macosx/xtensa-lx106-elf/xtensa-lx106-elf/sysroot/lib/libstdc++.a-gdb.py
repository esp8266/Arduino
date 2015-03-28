# -*- python -*-
# Copyright (C) 2009-2013 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys
import gdb
import os
import os.path

pythondir = '/Users/igrokhotkov/projects/esp8266/esptools/crosstool-NG/builds/xtensa-lx106-elf/share/gcc-4.8.2/python'
libdir = '/Users/igrokhotkov/projects/esp8266/esptools/crosstool-NG/builds/xtensa-lx106-elf/xtensa-lx106-elf/lib'

# This file might be loaded when there is no current objfile.  This
# can happen if the user loads it manually.  In this case we don't
# update sys.path; instead we just hope the user managed to do that
# beforehand.
if gdb.current_objfile () is not None:
    # Update module path.  We want to find the relative path from libdir
    # to pythondir, and then we want to apply that relative path to the
    # directory holding the objfile with which this file is associated.
    # This preserves relocatability of the gcc tree.

    # Do a simple normalization that removes duplicate separators.
    pythondir = os.path.normpath (pythondir)
    libdir = os.path.normpath (libdir)

    prefix = os.path.commonprefix ([libdir, pythondir])
    # In some bizarre configuration we might have found a match in the
    # middle of a directory name.
    if prefix[-1] != '/':
        prefix = os.path.dirname (prefix) + '/'

    # Strip off the prefix.
    pythondir = pythondir[len (prefix):]
    libdir = libdir[len (prefix):]

    # Compute the ".."s needed to get from libdir to the prefix.
    dotdots = ('..' + os.sep) * len (libdir.split (os.sep))

    objfile = gdb.current_objfile ().filename
    dir_ = os.path.join (os.path.dirname (objfile), dotdots, pythondir)

    if not dir_ in sys.path:
        sys.path.insert(0, dir_)

# Load the pretty-printers.
from libstdcxx.v6.printers import register_libstdcxx_printers
register_libstdcxx_printers (gdb.current_objfile ())
