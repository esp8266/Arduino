#!/bin/sh

set -e

org=$(cd ${0%/*}; pwd)
cd ${org}/..
pwd
test -d cores/esp8266
test -d libraries

# this warning question will be removed after restyle-all.sh is renamed to restyle.sh
echo "This is dangerous if you have modified your local repository"
echo "type iknowwhatido to continue"
read ans
test "$ans" = iknowwhatido || exit 1

for d in cores/esp8266 libraries; do
    for e in c cpp h; do
        find $d -name "*.$e" -exec \
            astyle \
                --suffix=none \
                --options=${org}/astyle_core.conf {} \;
        done
done

for d in libraries; do
    find $d -name "*.ino" -exec \
        astyle \
            --suffix=none \
            --options=${org}/astyle_examples.conf {} \;
done
