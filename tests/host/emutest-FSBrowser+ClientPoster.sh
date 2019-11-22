#!/bin/sh

# Purpose: debugging, timing, optimizing the core data transfers (Stream, FS, network)
#
# instructions: simply run this script
#
# what it does:
#
# Run FSbrowser official example
# use it to upload a random (big) file to emulated FS
# Stop it, copy the emulated FS file for another HTTPClientPost sketch
# Restart the Server
# Start the Poster, it will upload a copy to the server
# end

set -e

sizekb=16000  # FS SIZE in KBytes (file size will be the third of this value)
#DEBUG="D=1"   # comment this or not
options="-f -b"

sleep=5
sizeb=$(($sizekb * 1024))
fileb=$(($sizeb / 3))

make -j $DEBUG ../../libraries/ESP8266WebServer/examples/FSBrowser/FSBrowser
make -j $DEBUG otherexamples/HttpClientPost/HttpClientPost

killall -9 FSBrowser HttpClientPost || true

# remove FS
rm -f ./bin/FSBrowser/FSBrowser-littlefs${sizekb}KB
./bin/FSBrowser/FSBrowser $options -S 0 -L ${sizekb} & PIDFSBrowser=$!

echo "------------------------------"
echo "    let server start..."
echo "------------------------------"

# make a big random file
dd if=/dev/urandom of=randfile bs=${fileb} count=1

sleep ${sleep}

echo "------------------------------"
echo "    Uploading $file to FS"
echo "------------------------------"
curl -F "file=@$PWD/randfile" 127.0.0.1:9080/edit
echo "------------------------------"
echo "    Uploading to FS (done)"
echo "     (wait&kill FSBrowser)    "
echo "------------------------------"
sleep ${sleep}
kill -INT $PIDFSBrowser
sleep ${sleep}

# FSBrowser has generated littlefs backup file, copy it for the other sketch
# This sketch will repost this file to the FSBrowser, so we can debug/tune

# copy FS to http client sketch
cp ./bin/FSBrowser/FSBrowser-littlefs${sizekb}KB ./bin/HttpClientPost/HttpClientPost-littlefs${sizekb}KB
ls -al ./bin/FSBrowser/FSBrowser-littlefs${sizekb}KB ./bin/HttpClientPost/HttpClientPost-littlefs${sizekb}KB

echo "------------------------------"
echo "    let server start again..."
echo "------------------------------"
./bin/FSBrowser/FSBrowser $options -S 0 -L ${sizekb} & PIDFSBrowser=$!
sleep ${sleep}

echo "------------------------------"
echo "    start uploader sketch"
echo "------------------------------"
# run http client poster sketch
./bin/HttpClientPost/HttpClientPost $options -S 0 -L ${sizekb} & PIDClient=$!

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
