#!/bin/sh

set -e

org=$(cd ${0%/*}; pwd)
cd ${org}/..
pwd
test -d cores/esp8266
test -d libraries

#all="cores/esp8266 libraries"
all="
libraries/ESP8266mDNS
libraries/Wire
libraries/lwIP*
cores/esp8266/core_esp8266_si2c.cpp
"

for d in $all; do
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
