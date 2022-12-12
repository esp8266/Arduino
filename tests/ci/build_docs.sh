#!/bin/bash
#
# CI job to run the documentation build

set -ev

root=$(git rev-parse --show-toplevel)
env SPHINXOPTS="-W" make -C $root/doc html
