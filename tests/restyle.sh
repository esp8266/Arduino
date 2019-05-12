#!/bin/sh

set -e

org=$(cd ${0%/*}; pwd)
cd ${org}/..
pwd
test -d cores/esp8266
test -d libraries

for d in cores/esp8266 libraries; do
    for e in c cpp h; do
        find $d -name "*.$e" -exec \
            astyle \
                --suffix=none \
                --options=${org}/astyle_core.conf {} \;
        done
done

for d in libraries; do
    find $d -name "*.ino" -exec \
        astyle \
            --suffix=none \
            --options=${org}/astyle_examples.conf {} \;
done
