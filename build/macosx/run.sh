#!/bin/sh

# is qt java already included tho?
#CLASSPATH=/System/Library/Java/Extensions/QTJava.zip:lib:lib/build:lib/pde.jar:lib/core.jar:lib/antlr.jar:lib/oro.jar:lib/registry.jar

#export CLASSPATH

#cd work && /System/Library/Frameworks/JavaVM.framework/Versions/1.4.2/Commands/java -Dapple.laf.useScreenMenuBar=true -Dapple.awt.showGrowBox=false processing.app.Base
#cd work && java -Dapple.laf.useScreenMenuBar=true -Dapple.awt.showGrowBox=false processing.app.Base

# starting with work on release 0120, run directly from the .app
cd work && ./Processing.app/Contents/MacOS/JavaApplicationStub
