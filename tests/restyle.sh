#!/bin/sh

set -e

org=$(cd ${0%/*}; pwd)
cd ${org}/..
pwd
test -d cores/esp8266
test -d libraries

# in a near future, restyle-all.sh will be renamed to restyle.sh
# and will be checked against CI

for d in libraries; do
	find $d -name "*.ino" -exec \
	    astyle \
	        --suffix=none \
                --options=${org}/astyle_examples.conf {} \;
done
