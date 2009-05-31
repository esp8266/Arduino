#!/bin/sh

#javadoc -public -d doc *.java
#javadoc -private -d doc *.java
chmod +x preproc.pl
./preproc.pl
jikes -d . +D *.java
