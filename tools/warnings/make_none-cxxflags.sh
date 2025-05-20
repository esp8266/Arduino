#!/usr/bin/env sh

root=$(git rev-parse --show-toplevel)
${CC:-xtensa-lx106-elf-gcc} --help=warnings -Q |\
    grep '\[enabled\]' |\
    grep -v 'return-type' |\
    awk '{print $1}' |\
    sed 's/-W/-Wno-/' |\
    grep -v = |\
    grep -v -f ${root}/tools/warnings/patterns_none-cxxflags.txt |\
    sort -u > ${root}/tools/warnings/none-cxxflags
