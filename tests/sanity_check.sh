#!/usr/bin/env bash

root=$(git rev-parse --show-toplevel)
source "$root/tests/common.sh"

pushd "$root"/tools
python3 get.py -q
python3 makecorever.py --git-root "$root" "$root/cores/esp8266/core_version.h"
popd

pushd "$cache_dir"

cppflags=$"\
 -DARDUINO \
 -DARDUINO_ESP8266_GENERIC \
 -DF_CPU=80000000L \
 -DNONOSDK305=1 \
 -DLWIP_OPEN_SRC \
 -DTCP_MSS=1460 \
 -DLWIP_IPV4=1 \
 -DLWIP_IPV6=1 \
 -DLWIP_FEATURES=1 \
 -I$root/cores/esp8266 \
 -I$root/tools/sdk/lwip2/include \
 -I$root/tools/sdk/include \
 -I$root/variants/generic \
 -I$root/tools/sdk/libc/xtensa-lx106-elf"
cflags=$"\
 -Os \
 -g \
 -free \
 -fipa-pta \
 -Werror=return-type \
 -Wpointer-arith \
 -fno-inline-functions \
 -mtext-section-literals \
 -mlongcalls \
 -falign-functions=4 \
 -ffunction-sections \
 -fdata-sections"
gcc=$"$root/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
 $cppflags
 $cflags"

function build() {
    local f=$1
    $gcc -c -o "$f".o "$f"
}

$gcc --verbose

set -v -x

cp $root/cores/esp8266/libc_replacements.cpp ./

build libc_replacements.cpp

cat << EOF > arduino.c
#include <Arduino.h>
EOF

build arduino.c

cat << EOF > coredecls.c
#include <coredecls.h>
EOF

build coredecls.c

cat << EOF > features.c
#include <core_esp8266_features.h>
EOF

build features.c

cat << EOF > sdk.c
#include <version.h>
EOF

build sdk.c

cat << EOF > cmath.cpp
#include <cmath>

bool test_remainder(float x) {
  return fabs(std::remainder((float)15.123456, x) - (float)0.123456) < 1e-5;
}

bool test_remainder(double x) {
  return std::fabs(std::remainder((double)10.123456, x) - (double)0.123456) < 1e-5;
}
EOF

build cmath.cpp

cat << EOF > iostream.cpp
#include <iostream>
void test_iostream(bool val) {
  std::cout << (val ? "hello" : "world") << '\n';
}
EOF

build iostream.cpp

cat << EOF > regex.cpp
#include <string>
#include <regex>

bool test_regex(std::string v) {
  std::regex r("HELLO", std::regex_constants::ECMAScript | std::regex_constants::icase);
  if (std::regex_search(v, r))
    return true;
  return false;
}
EOF

build regex.cpp

cat << EOF > app_entry.cpp
#include <string>

bool test_remainder(float);
bool test_remainder(double);
bool test_regex(std::string);
void test_iostream(bool);

extern "C" void app_entry() {
  test_iostream(test_remainder(1.23f));
  test_iostream(test_remainder(4.56));
  test_iostream(test_regex("hello world"));
}
EOF

build app_entry.cpp

cp "$root/tools/sdk/ld/eagle.flash.1m.ld" "local.eagle.flash.ld.h"
preprocess=$"$gcc \
  -DFP_IN_IROM \
  -DVTABLES_IN_FLASH \
  -DMMU_IRAM_SIZE=0x8000 \
  -DMMU_ICACHE_SIZE=0x8000 \
  -CC -E -P"

$preprocess \
  "$root/tools/sdk/ld/eagle.app.v6.common.ld.h" \
  -o "local.eagle.app.v6.common.ld"

$preprocess \
  "local.eagle.flash.ld.h" \
  -o "local.eagle.flash.ld"

libs=$"-lhal -lphy -lpp -lnet80211 -llwip6-1460-feat -lwpa \
-lcrypto -lmain -lwps -lbearssl -lespnow -lsmartconfig \
-lairkiss -lwpa2 -lstdc++ -lm -lc -lgcc"
objects=$(find . -name '*.o' -printf ' %f')

link=$"$root/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
 -nostdlib
 -uapp_entry
 -mlongcalls
 -L$cache_dir
 -L$root/tools/sdk/lib/NONOSDK305
 -L$root/tools/sdk/lib
 -L$root/tools/sdk/ld
 -Wl,-EL
 -Wl,-T,local.eagle.flash.ld
 -Wl,--no-check-sections
 -Wl,-Map,xtensa.map
 -Wl,--gc-sections
 -Wl,--defsym,abort=0xfefefefe
 -Wl,--defsym,malloc=0xfefefefe
 -Wl,--defsym,free=0xfefefefe
 -Wl,--defsym,_read_r=0xfefefefe
 -Wl,--defsym,_lseek_r=0xfefefefe
 -Wl,--defsym,_write_r=0xfefefefe
 -Wl,--defsym,_close_r=0xfefefefe
 -Wl,--defsym,_free_r=0xfefefefe
 -Wl,--defsym,_malloc_r=0xfefefefe
 -Wl,--defsym,_realloc_r=0xfefefefe
 -Wl,--defsym,_calloc_r=0xfefefefe
 -Wl,--defsym,_fstat_r=0xfefefefe
 -Wl,--start-group $objects $libs -Wl,--end-group"

$link -o xtensa.elf
