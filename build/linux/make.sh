#!/bin/sh


### -- SETUP WORK DIR -------------------------------------------

if test -d work
then
  BUILD_PREPROC=false
else
  echo Setting up directories to build for linux...
  BUILD_PREPROC=true
  cp -r ../shared work

  # needs to make the dir because of packaging goofiness
  mkdir -p work/classes/processing/app/preproc
  mkdir -p work/classes/processing/app/syntax
  mkdir -p work/classes/processing/app/tools

  #cp -r ../../lib work/libraries
  cp -r ../../net work/libraries/
  cp -r ../../opengl work/libraries/
  cp -r ../../serial work/libraries/
  cp -r ../../video work/libraries/

  cd work
  unzip -q examples.zip
  rm examples.zip
  cd ..

  cd work
  unzip -q reference.zip
  rm reference.zip
  cd ..

  tar --extract --file=jre.tgz --ungzip --directory=work

  #mkdir work/lib/export
  mkdir work/lib/build
  #mkdir work/classes

  # get the serial stuff
  #echo Copying serial support from bagel dir
  #cp ../../bagel/serial/RXTXcomm.jar work/lib/
  #mkdir work/lib/i386
  #cp ../../bagel/serial/librxtxSerial.so work/lib/i386/libSerial.so
  #chmod +x work/librxtxSerial.so

  # get jikes and depedencies
  cp dist/jikes work/
  chmod +x work/jikes

  install -m 755 dist/processing work/processing
fi

cd ../..


### -- BUILD CORE ----------------------------------------------


echo Building processing.core

# move to bagel inside base 'processing' directory
cd core

# new regular version
CLASSPATH="../build/linux/work/java/lib/rt.jar"
export CLASSPATH

perl preproc.pl
../build/linux/work/jikes -d . +D -target 1.1 *.java
zip -rq ../build/linux/work/lib/core.jar processing
rm -rf processing


# back to base processing dir
cd ..


### -- BUILD PREPROC ------------------------------------------------

echo Building PDE for JDK 1.3

cd app/preproc

# first build the default java goop
# long path is to avoid requiring java to be in your PATH

../../build/linux/work/java/bin/java \
  -cp ../../build/linux/work/lib/antlr.jar antlr.Tool java.g

# now build the pde stuff that extends the java classes
../../build/linux/work/java/bin/java \
  -cp ../../build/linux/work/lib/antlr.jar antlr.Tool -glib java.g pde.g

cd ../..


### -- BUILD PDE ------------------------------------------------

cd app

CLASSPATH="../build/linux/work/lib/core.jar:../build/linux/work/lib/mrj.jar:../build/linux/work/lib/antlr.jar:../build/linux/work/lib/oro.jar:../build/linux/work/lib/registry.jar:../build/linux/work/java/lib/rt.jar"

../build/linux/work/jikes -target 1.3 +D -classpath $CLASSPATH:../build/linux/work/classes -d ../build/linux/work/classes *.java preproc/*.java syntax/*.java tools/*.java

cd ../build/linux/work/classes
rm -f ../lib/pde.jar
zip -0rq ../lib/pde.jar .
cd ../../../..


### -- BUILD LIBRARIES ------------------------------------------------

cd build/linux

PLATFORM=linux

#CLASSPATH="../../build/linux/work/lib/core.jar:../../build/linux/work/java/lib/rt.jar"
CLASSPATH=../build/$PLATFORM/work/lib/core.jar:$CLASSPATH
JIKES=../build/$PLATFORM/work/jikes
CORE=../build/$PLATFORM/work/lib/core.jar
LIBRARIES=../build/$PLATFORM/work/libraries

# move to processing/build 
cd ..


# SERIAL LIBRARY
echo Building serial library...
cd ../serial
$JIKES -target 1.1 +D -classpath "code/RXTXcomm.jar:$CORE:$CLASSPATH" -d . *.java 
rm -f library/serial.jar
zip -r0q library/serial.jar processing
rm -rf processing
mkdir -p $LIBRARIES/serial/library/
cp library/serial.jar $LIBRARIES/serial/library/


# NET LIBRARY
echo Building net library...
cd ../net
$JIKES -target 1.1 +D -d . *.java 
rm -f library/net.jar
zip -r0q library/net.jar processing
rm -rf processing
mkdir -p $LIBRARIES/net/library/
cp library/net.jar $LIBRARIES/net/library/


# OPENGL LIBRARY
echo Building OpenGL library...
cd ../opengl
$JIKES -target 1.1 +D -classpath "library/jogl.jar:$CLASSPATH" -d . *.java 
rm -f library/opengl.jar
zip -r0q library/opengl.jar processing
rm -rf processing
mkdir -p $LIBRARIES/opengl/library/
cp library/opengl.jar $LIBRARIES/opengl/library/
