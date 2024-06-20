#!/bin/sh
# requires clang-format, git, python3 with pyyaml

set -e -x

root=$(git rev-parse --show-toplevel)
test -d ${root}/cores/esp8266
test -d ${root}/libraries

# allow `env CLANG_FORMAT=clang-format-N`, or some other version
# default to v15, latest stable version from ubuntu-latest Github Actions image
CLANG_FORMAT=${CLANG_FORMAT:-clang-format-15}

#########################################
# 'all' variable should be "cores/esp8266 libraries"

all=${1:-"
cores/esp8266/Lwip*
libraries/ESP8266mDNS
libraries/Wire
libraries/lwIP*
cores/esp8266/debug*
cores/esp8266/core_esp8266_si2c.cpp
cores/esp8266/StreamString.*
cores/esp8266/StreamSend.*
libraries/Netdump
tests
"}

#########################################
# restyling core & libraries

cd $root

style=${root}/tests/clang-format-core.yaml
for target in $all; do
    if [ -d "$target" ]; then
        find $target \
            '(' -name "*.cpp" -o -name "*.c" -o -name "*.h" ')' \
            -exec $CLANG_FORMAT --verbose --style="file:$style" -i {} \;
    else
        $CLANG_FORMAT --verbose --style="file:$style" -i $target
    fi
done

#########################################
# restyling arduino examples

# TODO should not be matched, these are formatted externally
# exclude=$(git submodule --quiet foreach git rev-parse --show-toplevel | grep libraries)

if [ -z "$1" ] ; then
    style=${root}/tests/clang-format-arduino.yaml
    find libraries \
        -path libraries/ESP8266SdFat -prune -o \
        -path libraries/Ethernet -prune -o \
        -path libraries/SoftwareSerial -prune -o \
        -name '*.ino' -exec $CLANG_FORMAT --verbose --style="file:$style" -i {} \;
fi

#########################################
