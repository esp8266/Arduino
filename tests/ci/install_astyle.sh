#!/bin/bash
#
# $HOME/astyle directory is cached on Travis.
# If cached build is not present, download astyle and build it.
# Install built astyle binary into the home directory.
#

set -e

if [ ! -f $HOME/astyle/build/gcc/bin/astyle ]; then
	wget -O astyle_3.1_linux.tar.gz https://sourceforge.net/projects/astyle/files/astyle/astyle%203.1/astyle_3.1_linux.tar.gz/download
	tar -xf astyle_3.1_linux.tar.gz -C $HOME
	make -C $HOME/astyle/build/gcc
fi

make -C $HOME/astyle/build/gcc prefix=$HOME install
