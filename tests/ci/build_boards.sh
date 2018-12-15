#!/bin/bash
#
# CI job which checks that boards.txt and package_esp8266com_index.template.json are up to date

set -ev

cd $TRAVIS_BUILD_DIR

tools/boards.txt.py --boardsgen --ldgen --packagegen --docgen

git diff --exit-code -- boards.txt \
                        doc/boards.rst \
                        tools/sdk/ld/
git diff --exit-code -w -- package/package_esp8266com_index.template.json
