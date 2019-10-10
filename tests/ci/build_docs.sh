#!/bin/bash
#
# CI job to run the documentation build

set -ev

sudo apt install python3-setuptools

cd $TRAVIS_BUILD_DIR/doc

SPHINXOPTS="-W" make html
