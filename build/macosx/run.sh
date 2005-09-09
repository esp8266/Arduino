#!/bin/sh

CLASSPATH=:lib:lib/build:lib/pde.jar:lib/antlr.jar:lib/oro.jar:lib/registry.jar:lib/RXTXcomm.jar

export CLASSPATH

cd work && java -Dapple.laf.useScreenMenuBar=true -Dapple.awt.showGrowBox=false processing.app.Base
#./work/Arduino.app/Contents/MacOS/JavaApplicationStub 

