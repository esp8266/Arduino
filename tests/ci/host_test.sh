#!/bin/bash
#
# CI job for running tests on the host

set -ev

cd $TRAVIS_BUILD_DIR/tests/host

make CI
make clean-objects
