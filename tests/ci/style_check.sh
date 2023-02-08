#!/bin/bash
#
# CI job for checking examples style

set -e -x

git --version || true
root=$(git rev-parse --show-toplevel)

# Run formatter and compare what changed in the git tree.
# Also revert changes which formatter might have done to the submodules,
# as we don't want to fail the build because of the 3rd party libraries

cd $root
./tests/restyle.sh

git submodule foreach --recursive 'git reset --hard'
git diff --exit-code
