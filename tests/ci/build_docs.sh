#!/bin/bash
#
# CI job to run the documentation build

set -ev

root=$(git rev-parse --show-toplevel)
make SPHINXOPTS="--fail-on-warning" SPHINXBUILD="${SPHINXBUILD:?sphinx-build}" -C $root/doc html
