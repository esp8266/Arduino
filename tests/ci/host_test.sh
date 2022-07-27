#!/bin/bash
#
# CI job for running tests on the host

set -ev

if [ -z "$TRAVIS_BUILD_DIR" ]; then
    export TRAVIS_BUILD_DIR=$(cd ${PWD%/*}/..; pwd)
fi

cd $TRAVIS_BUILD_DIR/tests/host


make -j2 FORCE32=0 ssl
for i in ../../libraries/ESP8266WiFi/examples/WiFiClient/WiFiClient \
	../../libraries/ESP8266WiFi/examples/BearSSL_Validation/BearSSL_Validation \
	../../libraries/ESP8266WebServer/examples/HelloServer/HelloServer \
	../../libraries/SD/examples/Files/Files \
	../../libraries/LittleFS/examples/LittleFS_Timestamp/LittleFS_Timestamp \
	../../libraries/LittleFS/examples/SpeedTest/SpeedTest \
	../../libraries/DNSServer/examples/DNSServer/DNSServer ; do
	make -j2 D=1 FORCE32=0 $i
	valgrind --leak-check=full --track-origins=yes --error-limit=no --show-leak-kinds=all --error-exitcode=999 bin/$(basename $i)/$(basename $i) -1
done

make -j2 CI

make clean-objects
