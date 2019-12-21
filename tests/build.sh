#!/usr/bin/env bash

cache_dir=$(mktemp -d)

source "$TRAVIS_BUILD_DIR"/tests/common.sh

if [ -z "$BUILD_PARITY" ]; then
    mod=1
    rem=0
elif [ "$BUILD_PARITY" = "even" ]; then
    mod=2
    rem=0
elif [ "$BUILD_PARITY" = "odd" ]; then
    mod=2
    rem=1
fi

# Before anything, make sure the eboot.elf is up to date
"$TRAVIS_BUILD_DIR"/tests/check_eboot.sh
if [ $? -ne 0 ]; then
    exit 1
fi

install_arduino nodebug
build_sketches_with_arduino "$mod" "$rem" lm2f

rm -rf "$cache_dir"

