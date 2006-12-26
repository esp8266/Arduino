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
  #mkdir -p work/examples

  #need to replace this with the linux native library for RXTX
  echo Unzipping reference...
  unzip -q -d work ../shared/reference.zip

  cp dist/arduino work/
 
  # copy the avr-gcc distribution
  #echo Copying tools \(this may take a minute\)...

  # for now, require all linux users to install avr-gcc themselves
  #cp -pR dist/tools.zip work/
  #cd work
  #unzip -oq tools.zip
  #rm tools.zip
  #cd ..
fi

echo Copying shared and core files...
cp -r ../shared/* work
rm -rf work/dist
cp -r ../../targets work/lib
rm work/reference.zip

echo Copying examples...
#unzip -d work/examples ../shared/dist/examples.zip
cp -r ../shared/dist/examples work/

echo Copying dist files...
cp -r dist/lib work/
#cp -r dist/core work/
cp -r dist/bootloader work/
cp -r dist/tools work/

### -- START BUILDING -------------------------------------------

# move to root 'arduino' directory
cd ../..

### -- BUILD GCC ------------------------------------------------
# in the future we will build avr-gcc and tools (if they don't exist)

### -- COPY BOOTLOADER  -----------------------------------------
cd bootloader
# don't make the bootloader, since it rarely changes and requires
# installation of make.
# make
cp ATmegaBOOT.hex ../build/linux/work/bootloader
cd ..

### -- BUILD PARSER ---------------------------------------------

cd app

if $BUILD_PREPROC
then
  cd preproc
  # build classes/grammar for preprocessor
  echo Building antlr grammar code...
  # first build the default java goop
  java -cp ../../build/linux/work/lib/antlr.jar antlr.Tool StdCParser.g
  # now build the pde stuff that extends the java classes
  java -cp ../../build/linux/work/lib/antlr.jar antlr.Tool -glib StdCParser.g WParser.g
  java -cp ../../build/linux/work/lib/antlr.jar antlr.Tool WTreeParser.g
  java -cp ../../build/linux/work/lib/antlr.jar antlr.Tool -glib WTreeParser.g WEmitter.g
  cd ..
fi

### -- BUILD PDE ------------------------------------------------

echo Building the PDE...

# compile the code as java 1.3, so that the application will run and
# show the user an error, rather than crapping out with some strange
# "class not found" crap
#jikes -classpath ../build/linux/work/classes:../build/linux/work/lib/antlr.jar:../build/linux/work/lib/oro.jar:../build/linux/work/lib/registry.jar:../build/linux/work/lib/RXTXcomm.jar:../build/linux/work/lib/mrj.jar:$CLASSPATH -d ../build/linux/work/classes tools/*.java preproc/*.java syntax/*.java *.java 
javac -source 1.4 -target 1.4 -classpath ../build/linux/work/class:../build/linux/work/lib/antlr.jar:../build/linux/work/lib/oro.jar:../build/linux/work/lib/registry.jar:../build/linux/work/lib/RXTXcomm.jar:../build/linux/work/lib/mrj.jar:$CLASSPATH -d ../build/linux/work/classes tools/*.java preproc/*.java syntax/*java *.java

cd ../build/linux/work/classes
rm -f ../lib/pde.jar
zip -0rq ../lib/pde.jar .
cd ../..

echo
echo Done.
