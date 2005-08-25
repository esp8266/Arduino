#!/bin/sh

# Part of the Arduino project
# http://arduino.berlios.de
#
# this is derived from the processing project
# http://www.processing.org
#
# This file is subjected to the GPL License
#

### -- SETUP WORK DIR -------------------------------------------

if test -d work 
then
  BUILD_PREPROC=false
else
#  if test -f /sw/bin/cp
#  then
#    echo
#  else
#    echo You need to install fink with fileutils, textutils, etc
#    exit
#  fi

  echo Setting up directories to build under Mac OS X
  BUILD_PREPROC=true

  echo Copying shared and dist files...
  cp -r ../shared work
  cp -r ../../core work

  # needs to make the dir because of packaging goofiness
  mkdir -p work/classes/processing/app/preproc
  mkdir -p work/classes/processing/app/syntax
  mkdir -p work/classes/processing/app/tools
  mkdir -p work/lib/build

  cp -r dist/lib work/
  cp -r dist/core work/

  # to have a copy of this guy around for messing with
  echo Copying Arduino.app...

  cp -pR dist/Arduino.app work/
  # cvs doesn't seem to want to honor the +x bit 
  chmod +x work/Arduino.app/Contents/MacOS/JavaApplicationStub
  
  # copy the avr-gcc distribution
  cp -pR dist/tools.zip work/
  cd work
  unzip -q tools.zip
  rm tools.zip
  cd ..

  # get jikes and depedencies
  echo Copying jikes...
  cp dist/jikes work/
  chmod +x work/jikes
fi


### -- START BUILDING -------------------------------------------

# move to root 'processing' directory
cd ../..


### -- BUILD GCC ----------------------------------------------
# in the future we will build avr-gcc and tools (if they don't exist)

# set classpath
CLASSPATH=/System/Library/Frameworks/JavaVM.framework/Classes/classes.jar:/System/Library/Frameworks/JavaVM.framework/Classes/ui.jar:/System/Library/Java/Extensions/MRJToolkit.jar
export CLASSPATH

cd app

### -- BUILD PARSER ---------------------------------------------

#BUILD_PREPROC=true

if $BUILD_PREPROC
then
  cd preproc
  # build classes/grammar for preprocessor
  echo Building antlr grammar code...
  # first build the default java goop
  java -cp ../../build/macosx/work/lib/antlr.jar antlr.Tool java.g
  # now build the pde stuff that extends the java classes
  java -cp ../../build/macosx/work/lib/antlr.jar antlr.Tool -glib java.g pde.g
  cd ..
fi

### -- BUILD PDE ------------------------------------------------

echo Building the PDE...

# compile the code as java 1.3, so that the application will run and
# show the user an error, rather than crapping out with some strange
# "class not found" crap
#../build/macosx/work/jikes -target 1.3 +D -classpath ../build/macosx/work/classes:../build/macosx/work/lib/core.jar:../build/macosx/work/lib/antlr.jar:../build/macosx/work/lib/oro.jar:../build/macosx/work/lib/registry.jar:$CLASSPATH -d ../build/macosx/work/classes *.java syntax/*.java preproc/*.java tools/*.java
../build/macosx/work/jikes -target 1.3 +D -classpath ../build/macosx/work/classes:../build/macosx/work/lib/antlr.jar:../build/macosx/work/lib/oro.jar:../build/macosx/work/lib/registry.jar:../build/macosx/work/lib/RXTXcomm.jar:$CLASSPATH -d ../build/macosx/work/classes tools/*.java preproc/*.java syntax/*.java  *.java 

cd ../build/macosx/work/classes
rm -f ../lib/pde.jar
zip -0rq ../lib/pde.jar .
cd ../..

# i fought the packages and the packages won
#cd ../..
#WORKLIB=processing/build/macosx/work/lib
#./processing/build/macosx/work/jikes -d . -target 1.3 +D -classpath $WORKLIB/core.jar:$WORKLIB/antlr.jar:$WORKLIB/oro.jar:$WORKLIB/registry.jar:$CLASSPATH -d . processing/app/*.java processing/app/preproc/*.java processing/app/syntax/*.java processing/app/tools/*.java
#zip -rq $WORKLIB/pde.jar processing/app/*.class processing/app/preproc/*.class processing/app/syntax/*.class processing/app/tools/*.class

# get the libs
mkdir -p work/Arduino.app/Contents/Resources/Java/
cp work/lib/*.jar work/Arduino.app/Contents/Resources/Java/


### -- BUILD LIBRARIES ------------------------------------------------


PLATFORM=macosx


CLASSPATH=../build/$PLATFORM/work/lib/core.jar:$CLASSPATH
JIKES=../build/$PLATFORM/work/jikes
CORE=../build/$PLATFORM/work/lib/core.jar
LIBRARIES=../build/$PLATFORM/work/libraries

# move to processing/build 
cd ..







CLASSPATH=../$CLASSPATH
JIKES=../../build/$PLATFORM/work/jikes
CORE=../../build/$PLATFORM/work/lib/core.jar
LIBRARIES=../../build/$PLATFORM/work/libraries



echo
echo Done.
