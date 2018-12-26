#!/bin/bash
#
# CI job which builds the boards manager package

set -ev

export PKG_URL=https://github.com/esp8266/Arduino/releases/download/$TRAVIS_TAG/esp8266-$TRAVIS_TAG.zip
export DOC_URL=https://arduino-esp8266.readthedocs.io/en/$TRAVIS_TAG/

if [ -z "$CI_GITHUB_API_KEY" ]; then
    echo "Github API key not set. Skip building the package."
    exit 0
fi

cd $TRAVIS_BUILD_DIR/package
./build_boards_manager_package.sh
