#!/bin/sh

# fetch.sh
# David A. Mellis
# Script to download reference pages from Arduino website and change links
# to point to local copies of the pages.  A terrible hack.

REVISION=`head -1 ../todo.txt | cut -c 1-4`
mkdir reference
cd reference
curl http://www.arduino.cc/en/Guide/HomePage -o Guide_index.html
curl http://www.arduino.cc/en/Main/FAQ -o FAQ.html
curl http://arduino.cc/en/Main/Environment -o environment.html
curl http://www.arduino.cc/en/Reference/HomePage$REVISION -o index.html
curl http://www.arduino.cc/en/pub/skins/arduino/arduino.css -o arduino.css
for i in `grep -o "http://www.arduino.cc/en/Guide/[^']*" Guide_index.html | sort -u | grep -v '?' | cut -d '/' -f 6`; do curl http://www.arduino.cc/en/Guide/$i -o Guide_$i.html; done
for i in `grep -o "http://www.arduino.cc/en/Reference/[^']*" index.html | sort -u | grep -v '?' | cut -d '/' -f 6`; do curl http://www.arduino.cc/en/Reference/$i -o $i.html; done
for i in `grep -o "http://www.arduino.cc/en/Serial/[^']*" index.html | sort -u | grep -v '?' | cut -d '/' -f 6`; do curl http://www.arduino.cc/en/Serial/$i -o Serial_$i.html; done
perl -i -pe "s|http://www.arduino.cc/en/Reference/[^?\"']*\?[^'\"]*|#|g" *.html # replace links to unknown pages with links to '#'
perl -i -pe "s|http://www.arduino.cc/en/Guide/([^']*)|Guide_\1.html|g" *.html # replace links to remote guide with links to local guide
perl -i -pe "s|http://www.arduino.cc/en/Reference/([^']*)|\1.html|g" *.html # replace links to remote reference with links to local reference
perl -i -pe "s|http://www.arduino.cc/en/Serial/([^']*)|Serial_\1.html|g" *.html # replace links to remove serial reference with links to local serial reference
perl -i -pe "s|http://www.arduino.cc/en/pub/skins/arduino/arduino.css|arduino.css|g" *.html
perl -i -pe "s|HomePage.html|index.html|g" *.html
perl -i -pe "s|href=\"/\"|href=\"http://www.arduino.cc/\"|g" *.html
cd ..
zip -r shared/reference.zip reference
rm -rf reference
