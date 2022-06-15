#!/bin/sh
# requires clang-format, git, python3 with pyyaml

set -e -x

root=$(git rev-parse --show-toplevel)
test -d ${root}/cores/esp8266
test -d ${root}/libraries

# allow `env CLANG_FORMAT=clang-format-13`, or some other version
# default to v13, latest stable version from https://apt.llvm.org
CLANG_FORMAT=${CLANG_FORMAT:-clang-format-13}

# TODO: waiting for llvm-14 to allow --style=file:<path-to-file>
makeClangFormatStyle() {
    python3 -c 'import sys,yaml; sys.stdout.write(yaml.dump(yaml.safe_load(open(sys.argv[1], "r")), default_flow_style=True)); sys.stdout.flush();' $1
}

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

style=$(makeClangFormatStyle ${root}/tests/clang-format-core.yaml)
for target in $all; do
    if [ -d "$target" ]; then
        find $target \
            '(' -name "*.cpp" -o -name "*.c" -o -name "*.h" ')' \
            -exec $CLANG_FORMAT --verbose --style="$style" -i {} \;
    else
        $CLANG_FORMAT --verbose --style="$style" -i $target
    fi
done

#########################################
# restyling arduino examples

# TODO should not be matched, these are formatted externally
# exclude=$(git submodule --quiet foreach git rev-parse --show-toplevel | grep libraries)

if [ -z $1 ] ; then
    style=$(makeClangFormatStyle ${root}/tests/clang-format-arduino.yaml)
    find libraries \
        -path libraries/ESP8266SdFat -prune -o \
        -path libraries/Ethernet -prune -o \
        -path libraries/SoftwareSerial -prune -o \
        -name '*.ino' -exec $CLANG_FORMAT --verbose --style="$style" -i {} \;
fi

#########################################
