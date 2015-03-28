# Copyright (C) 2010-2012 Free Software Foundation, Inc.

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

import traceback

# Auto-load all functions/commands.

# Modules to auto-load, and the paths where those modules exist.

module_dict = {
  'gdb.function': os.path.join(gdb.PYTHONDIR, 'gdb', 'function'),
  'gdb.command': os.path.join(gdb.PYTHONDIR, 'gdb', 'command')
}

# Iterate the dictionary, collating the Python files in each module
# path.  Construct the module name, and import.

for module, location in module_dict.iteritems():
  if os.path.exists(location):
     py_files = filter(lambda x: x.endswith('.py') and x != '__init__.py',
                       os.listdir(location))

     for py_file in py_files:
       # Construct from foo.py, gdb.module.foo
       py_file = module + '.' + py_file[:-3]
       try:
         exec('import ' + py_file)
       except:
         print >> sys.stderr, traceback.format_exc()
