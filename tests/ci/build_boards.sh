#!/bin/bash
#
# CI job which checks that files generated with boards.txt.py are up to date

set -ev

cd $TRAVIS_BUILD_DIR

tools/boards.txt.py test
tools/boards.txt.py generate --all

git diff --exit-code -- boards.txt \
                        doc/boards.rst \
                        tools/sdk/ld/
git diff --exit-code -w -- package/package_esp8266com_index.boards.json
