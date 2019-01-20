#!/usr/bin/env python

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
