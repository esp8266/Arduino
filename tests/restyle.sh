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
libraries/lwIP*
cores/esp8266/Lwip*
cores/esp8266/debug*
cores/esp8266/core_esp8266_si2c.cpp
cores/esp8266/StreamString.*
cores/esp8266/StreamSend.*
libraries/Netdump
tests
"

# core

cp tests/clang-format-core .clang-format
for d in $all; do
    if [ -d "$d" ]; then
        echo "-------- directory $d:"
        for e in c cpp h; do
            find $d -name "*.$e" -exec clang-format-12 -i {} \;
        done
    else
        echo "-------- file $d:"
        clang-format-12 -i ${d}
    fi
done

# examples

cp tests/clang-format-arduino .clang-format
for d in libraries; do
    echo "-------- examples in $d:"
    find $d -name "*.ino" -exec clang-format-12 -i {} \;
done
