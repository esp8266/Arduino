#!/bin/bash
cd $(cd ${0%/*}; pwd)
astyle --options=examples_style.conf ../libraries/*/examples/*{,/*}/*.ino
