#!/bin/bash

set -ev

root=$(git rev-parse --show-toplevel)
READELF="$root/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-readelf"

cd $root/tools
python3 get.py -q

cd $root/bootloaders/eboot

"$READELF" -x .data -x .text eboot.elf > git.txt
make clean
make
"$READELF" -x .data -x .text eboot.elf > build.txt
diff git.txt build.txt
if [ $? -ne 0 ]; then
    echo ERROR:  eboot.elf in repo does not match output from compile.
    echo ERROR:  Need to rebuild and check in updated eboot.
    exit 1
fi
