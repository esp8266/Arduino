#!/bin/bash
#
# CI job which checks that boards.txt and package_esp8266com_index.template.json are up to date

set -ev

root=$(git rev-parse --show-toplevel)

cd $root
tools/boards.txt.py --boardsgen --ldgen --packagegen --docgen

git diff --exit-code -- \
    boards.txt \
    doc/boards.rst \
    tools/sdk/ld/
git diff --exit-code --ignore-all-space -- \
    package/package_esp8266com_index.template.json
