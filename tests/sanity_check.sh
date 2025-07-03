#!/usr/bin/env bash

root=$(git rev-parse --show-toplevel)
source "$root/tests/common.sh"

pushd "$root"/tools
python3 get.py -q
python3 makecorever.py --git-root "$root" "$root/cores/esp8266/core_version.h"
popd

pushd "$cache_dir"

cflags="-mlongcalls -g -free -fipa-pta -Werror=return-type -mlongcalls -mtext-section-literals -falign-functions=4"
gcc=$"$root/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
 $cflags
 -I$root/cores/esp8266 \
 -I$root/tools/sdk/include \
 -I$root/variants/generic \
 -I$root/tools/sdk/libc/xtensa-lx106-elf"

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

cat << EOF > iostream.cpp
#include <iostream>
void foo() {
  std::cout << "hello world";
}
EOF

$gcc -c iostream.cpp

cat << EOF > regex.cpp
#include <string>
#include <regex>

bool bar(std::string v) {
  std::regex r("HELLO", std::regex_constants::ECMAScript | std::regex_constants::icase);
  if (std::regex_search(v, r))
    return true;
  return false;
}
EOF

$gcc -c regex.cpp

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
cat local.eagle.app.v6.common.ld

$preprocess \
  "local.eagle.flash.ld.h" \
  -o "local.eagle.flash.ld"
cat local.eagle.flash.ld

libs=$"-lhal -lphy -lpp -lnet80211 -llwip6-1460-feat -lwpa \
-lcrypto -lmain -lwps -lbearssl -lespnow -lsmartconfig \
-lairkiss -lwpa2 -lstdc++ -lm -lc -lgcc"
objects="arduino.o coredecls.o features.o sdk.o iostream.o"

link=$"$root/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
 -nostdlib
 -u app_entry
 -mlongcalls
 -L$cache_dir
 -L$root/tools/sdk/lib/NONOSDK305
 -L$root/tools/sdk/lib
 -L$root/tools/sdk/ld
 -Wl,-T,local.eagle.flash.ld
 -Wl,--no-check-sections
 -Wl,-Map,xtensa.map
 -Wl,--gc-sections
 -Wl,--defsym,app_entry=0xaaaaaaaa
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
cat xtensa.map
