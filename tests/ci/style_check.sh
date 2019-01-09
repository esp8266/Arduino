#!/bin/bash
#
# CI job for checking examples style

set -ev

find $TRAVIS_BUILD_DIR/libraries -name '*.ino' -exec \
    astyle \
        --suffix=none \
        --options=$TRAVIS_BUILD_DIR/tests/examples_style.conf {} \;

# Revert changes which astyle might have done to the submodules,
# as we don't want to fail the build because of the 3rd party libraries
git submodule foreach --recursive git reset --hard

git diff --exit-code -- $TRAVIS_BUILD_DIR/libraries
