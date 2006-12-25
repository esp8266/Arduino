#!/bin/sh

PATH=lib:$PATH
CLASSPATH=\"java\\lib\\rt.jar\;lib\\RXTXcomm.jar\;lib\;lib\\build\;lib\\pde.jar\;lib\\mrj.jar\;lib\\antlr.jar\;lib\\oro.jar\;lib\\registry.jar\"
export PATH
export CLASSPATH

#cd work && ./java/bin/java -Xint PdeBase
cd work && ./java/bin/java processing.app.Base
#cd work && /cygdrive/c/jdk-1.3.1_11/bin/java PdeBase
