#!/usr/bin/env python

# Generate the core_version.h header per-build
#
# Copyright (C) 2019 - Earle F. Philhower, III
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import argparse
import os
import subprocess

parser = argparse.ArgumentParser(description='Generate core_version.h')
parser.add_argument('-b', '--build_path', action='store', required=True, help='build.path variable')
parser.add_argument('-p', '--platform_path', action='store', required=True, help='platform.path variable')
parser.add_argument('-v', '--version', action='store', required=True, help='version variable')

args = parser.parse_args()

core = args.build_path + '/core'
try:
    os.makedirs(core)
except:
    pass

out = open(core + '/core_version.h', "w")

try:
    p = subprocess.Popen(['git', '--git-dir', args.platform_path + '/.git', 'rev-parse', '--short=8', 'HEAD'], stdout = subprocess.PIPE )
    git_ver = '0x' + p.stdout.readlines()[0].strip()
    p = subprocess.Popen(['git', '--git-dir', args.platform_path + '/.git', 'describe', '--tags'], stdout = subprocess.PIPE )
    git_desc = p.stdout.readlines()[0].strip()
except:
    git_ver = '0xffffffff'
    git_desc = args.version

out.write('#define ARDUINO_ESP8266_GIT_VER  ' + git_ver + '\n')
out.write('#define ARDUINO_ESP8266_GIT_DESC ' + git_desc + '\n')

out.close()
