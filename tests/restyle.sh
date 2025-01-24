#!/bin/sh
# requires python3, git, and runnable clang-format (specified below)

set -e -x

root=$(git rev-parse --show-toplevel)
test -d ${root}/cores/esp8266
test -d ${root}/libraries

# allow `env CLANG_FORMAT=clang-format-N`, or some other version
CLANG_FORMAT=${CLANG_FORMAT:-clang-format}

cd $root
python $root/tests/restyle.py format --clang-format=$CLANG_FORMAT preset --include core --include arduino

if [ "$CI" = "true" ] ; then
    python $root/tests/restyle.py assert --with-summary --with-errors
else
    python $root/tests/restyle.py assert --with-diff
fi
