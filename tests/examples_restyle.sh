#!/bin/sh
cd $(cd ${0%/*}; pwd)
echo astyle --options=examples_style.conf ../libraries/*/examples/*/*.ino
