#!/usr/bin/env bash

cache_dir=$(mktemp -d)

source "$TRAVIS_BUILD_DIR"/tests/common.sh

if [ "$BUILD_PARITY" = "even" ]; then
    mod=2
    rem=0
elif [ "$BUILD_PARITY" = "odd" ]; then
    mod=2
    rem=1
fi

install_arduino debug
build_sketches_with_arduino "$mod" "$rem" lm2f

rm -rf "$cache_dir"

