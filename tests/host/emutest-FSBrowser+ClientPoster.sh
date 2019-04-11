#!/bin/sh

# Purpose: debugging, timing, optimizing the core data transfers (Stream, FS, network)
#
# instructions: simply run this script
#
# what it does:
#
# Run FSbrowser official example
# use it to upload a random (big) file to emulated SPIFFS
# Stop it, copy the emulated SPIFFS file for another HTTPClientPost sketch
# Restart the Server
# Start the Poster, it will upload a copy to the server
# end

set -e

sizekb=16000  # SPIFFS SIZE in KBytes (file size will be the third of this value)
DEBUG="D=1"   # comment this or not
options="-f -b"

sleep=5
sizeb=$(($sizekb * 1024))
fileb=$(($sizeb / 3))

make -j $DEBUG ../../libraries/ESP8266WebServer/examples/FSBrowser/FSBrowser
make -j $DEBUG otherexamples/HttpClientPost/HttpClientPost

killall -9 FSBrowser HttpClientPost || true

# remove SPIFFS
rm -f FSBrowser-spiffs${sizekb}KB
./bin/FSBrowser/FSBrowser $options -S ${sizekb} & PIDFSBrowser=$!

echo "------------------------------"
echo "    let server start..."
echo "------------------------------"

# make a big random file
dd if=/dev/urandom of=randfile bs=${fileb} count=1

sleep ${sleep}

echo "------------------------------"
echo "    Uploading $file to SPIFFS"
echo "------------------------------"
curl -F "file=@$PWD/randfile" 127.0.0.1:9080/edit
echo "------------------------------"
echo "    Uploading to SPIFFS (done)"
echo "     (wait&kill FSBrowser)    "
echo "------------------------------"
sleep ${sleep}
kill -INT $PIDFSBrowser
sleep ${sleep}

# FSBrowser has generated SPIFFS backup file, copy it for the other sketch
# This sketch will repost this file to the FSBrowser, so we can debug/tune

# copy SPIFFS to http client sketch
cp ./bin/FSBrowser/FSBrowser-spiffs${sizekb}KB ./bin/HttpClientPost/HttpClientPost-spiffs${sizekb}KB
ls -al ./bin/FSBrowser/FSBrowser-spiffs${sizekb}KB ./bin/HttpClientPost/HttpClientPost-spiffs${sizekb}KB

echo "------------------------------"
echo "    let server start again..."
echo "------------------------------"
./bin/FSBrowser/FSBrowser $options -S ${sizekb} & PIDFSBrowser=$!
sleep ${sleep}

echo "------------------------------"
echo "    start uploader sketch"
echo "------------------------------"
# run http client poster sketch
./bin/HttpClientPost/HttpClientPost $options -S ${sizekb} & PIDClient=$!

echo "------------------------------"
echo "Let run the sketches, press enter to kill"
echo "------------------------------"
echo "upload duration:"
read junk

echo "killing everybody"
kill -INT $PIDClient $PIDFSBrowser || true
sleep 2
echo "hardkilling everybody"
kill -9 $PIDClient $PIDFSBrowser
