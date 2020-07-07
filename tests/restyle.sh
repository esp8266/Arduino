#!/bin/sh

set -e

org=$(cd ${0%/*}; pwd)
cd ${org}/..
pwd
test -d cores/esp8266
test -d libraries

# should be: all="cores/esp8266 libraries"

all="
libraries/ESP8266mDNS
libraries/Wire
cores/esp8266/core_esp8266_si2c.cpp
"

# core

for d in $all; do
    if [ -d "$d" ]; then
        echo "-------- directory $d:"
        for e in c cpp h; do
            find $d -name "*.$e" -exec \
                astyle \
                    --suffix=none \
                    --options=${org}/astyle_core.conf {} \;
        done
    else
        echo "-------- file $d:"
        astyle --suffix=none --options=${org}/astyle_core.conf "$d"
    fi
done

# examples

for d in libraries; do
    echo "-------- examples in $d:"
    find $d -name "*.ino" -exec \
        astyle \
            --suffix=none \
            --options=${org}/astyle_examples.conf {} \;
done
