#!/bin/bash

# Check that no file in the bootloader directory is newer than the eboot.elf
# (indicating that eboot.elf needs to be rebuilt)

cd "$TRAVIS_BUILD_DIR"/bootloaders/eboot
fail=0
eboot=$(git log -1 --date=unix --format=%at eboot.elf)
for i in $(git ls-files | grep -v ^eboot.elf); do
    fdate=$(git log -1 --date=unix --format=%at $i)
    if [ $fdate -gt $eboot ]; then
        echo FAILURE: $i is newer than eboot.elf
        fail=1
    fi
done
if [ $fail -ne 0 ]; then
    exit 1
else
    exit 0
fi
