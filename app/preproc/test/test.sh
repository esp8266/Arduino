#!/bin/sh

javac -classpath ../../../build/shared/lib/oro.jar test_PdePreprocessor.java
for i in data/t*.cpp; do echo $i; java -classpath ../../../build/shared/lib/oro.jar:. test_PdePreprocessor strip $i | diff - $i.out; done
for i in data/foo*.cpp; do echo $i; java -classpath ../../../build/shared/lib/oro.jar:. test_PdePreprocessor prototypes $i | diff - $i.out; done
