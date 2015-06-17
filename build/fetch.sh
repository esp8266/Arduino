#!/bin/sh

# fetch.sh
# David A. Mellis and Tom Pollard
# Script to download reference pages from Arduino website and change links
# to point to local copies of the pages. 

die () { echo ERROR: $*; exit 1; }

mkdir reference  || die 'unable to create reference directory'

cd reference
perl ../create_reference.pl || die 'unable to create local reference pages'

mkdir img
curl http://www.arduino.cc/en/pub/skins/arduinoUno/img/logo.png > img/logo.png

cd ..
zip -r shared/reference.zip reference || die 'unable to create reference.zip archive'

rm -rf reference
