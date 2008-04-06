#!/bin/sh

# Since Windows and GNU/Linux use a script called 'dist.sh' to make the
# package, it makes it easy to script the nightly auto-builds if Mac OS X has
# a script called 'dist.sh' also.

xcodebuild -target Distribute


