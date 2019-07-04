#!/bin/sh

set -e

cd $(cd ${0%/*}; pwd)/host

make -j ../../libraries/ESP8266WebServer/examples/FSBrowser/FSBrowser
