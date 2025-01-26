#!/usr/bin/env bash

root=$(git rev-parse --show-toplevel)
source "$root/tests/common.sh"

pushd "$root"/tools
python3 get.py -q

popd
pushd "$cache_dir"

gcc="$root/tools/xtensa-lx106-elf-gcc/bin/xtensa-lx106-elf-gcc"\
" -I$root/cores/esp8266"\
" -I$root/tools/sdk/include"\
" -I$root/variants/generic"\
" -I$root/tools/sdk/libc/xtensa-lx106-elf"

$gcc --verbose

set -v -x

cat << EOF > arduino.c
#include <Arduino.h>
EOF

$gcc -c arduino.c

cat << EOF > coredecls.c
#include <coredecls.h>
EOF

$gcc -c coredecls.c

cat << EOF > features.c
#include <core_esp8266_features.h>
EOF

$gcc -c features.c

cat << EOF > sdk.c
#include <version.h>
EOF

$gcc -c sdk.c
