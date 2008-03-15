#!/bin/sh

# Part of the Arduino project
# http://arduino.berlios.de
#
# this is derived from the processing project
# http://www.processing.org
#
# This file is subjected to the GPL License

# NOTE: before running this script, you must set CLASSPATH to
# your standard Java classpath, and DIRAVR to the root of your
# avr-gcc installation.


### -- SETUP WORK DIR -------------------------------------------

if test -d work 
then
  BUILD_PREPROC=false
else
  BUILD_PREPROC=true

  # needs to make the dir because of packaging goofiness
  echo Setting up directories to build under Linux
  mkdir -p work/classes/processing/app/preproc
  mkdir -p work/classes/processing/app/syntax
  mkdir -p work/classes/processing/app/tools
  mkdir -p work/lib/build

  echo Unzipping reference...
  unzip -q -d work ../shared/reference.zip

  cp dist/arduino work/
fi

echo Copying shared and core files...
cp -r ../shared/* work
rm -rf work/dist
cp -r ../../hardware work
rm work/reference.zip

echo Copying examples...
cp -r ../shared/dist/examples work/

echo Copying dist files...
cp -r dist/lib work/
cp -r dist/tools work/hardware/

### -- START BUILDING -------------------------------------------

# move to root 'arduino' directory
cd ../..

### -- BUILD PDE ------------------------------------------------

cd app

echo Building the PDE...

# compile the code as java 1.3, so that the application will run and
# show the user an error, rather than crapping out with some strange
# "class not found" crap
#jikes -classpath ../build/linux/work/classes:../build/linux/work/lib/antlr.jar:../build/linux/work/lib/oro.jar:../build/linux/work/lib/registry.jar:../build/linux/work/lib/RXTXcomm.jar:../build/linux/work/lib/mrj.jar:$CLASSPATH -d ../build/linux/work/classes tools/*.java preproc/*.java syntax/*.java *.java 
javac -source 1.4 -target 1.4 -classpath ../build/linux/work/class:../build/linux/work/lib/antlr.jar:../build/linux/work/lib/oro.jar:../build/linux/work/lib/registry.jar:../build/linux/work/lib/RXTXcomm.jar:../build/linux/work/lib/mrj.jar:$CLASSPATH -d ../build/linux/work/classes ../core/*.java tools/*.java preproc/*.java syntax/*java *.java

cd ../build/linux/work/classes
rm -f ../lib/pde.jar
zip -0rq ../lib/pde.jar .
cd ../..

echo
echo Done.
