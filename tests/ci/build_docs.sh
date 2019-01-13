#!/bin/bash
#
# CI job to run the documentation build

set -ev

cd $TRAVIS_BUILD_DIR/doc

SPHINXOPTS="-W" make html
