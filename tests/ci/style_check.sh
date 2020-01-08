#!/bin/bash
#
# CI job for checking examples style

set -ev

org=$(cd ${0%/*}; pwd)
${org}/../restyle.sh

# Revert changes which astyle might have done to the submodules,
# as we don't want to fail the build because of the 3rd party libraries
git --version || true
git submodule foreach --recursive 'git reset --hard'

git diff --exit-code -- $TRAVIS_BUILD_DIR/libraries
