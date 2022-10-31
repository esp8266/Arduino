#!/bin/bash
#
# CI job which builds the boards manager package

set -ev

root=$(git rev-parse --show-toplevel)
tag=$ESP8266_ARDUINO_RELEASE_TAG

export PKG_URL=https://github.com/esp8266/Arduino/releases/download/$tag/esp8266-$tag.zip
export DOC_URL=https://arduino-esp8266.readthedocs.io/en/$tag/

if [ -z "$CI_GITHUB_API_KEY" ]; then
    echo "Github API key not set. Skip building the package."
    exit 0
fi

cd $root/package
./build_boards_manager_package.sh
