#!/bin/sh


### -- SETUP WORK DIR -------------------------------------------

if test -d work
then
  BUILD_PREPROC=false
else
  echo Setting up directories to build for Linux...
  BUILD_PREPROC=true

  mkdir work
  cp -r ../shared/lib work/
  cp -r ../shared/libraries work/
  cp -r ../shared/tools work/

  cp -r ../../hardware work/

  cp ../../app/lib/antlr.jar work/lib/
  cp ../../app/lib/ecj.jar work/lib/
  cp ../../app/lib/jna.jar work/lib/
  cp ../../app/lib/oro.jar work/lib/
  cp ../../app/lib/RXTXcomm.jar work/lib/

  echo Copying examples...
  cp -r ../shared/examples work/

  echo Extracting reference...
  unzip -q -d work/ ../shared/reference.zip

  cp -r dist/tools work/hardware/
  cp dist/lib/librxtxSerial.so work/lib/

  install -m 755 dist/arduino work/arduino

  ARCH=`uname -m`
  if [ $ARCH = "i686" ]
  then
    echo Extracting JRE...
    tar --extract --file=jre.tgz --ungzip --directory=work
  else 
#    echo This is not my beautiful house.
#    if [ $ARCH = "x86_64" ]
#    then 
#      echo You gots the 64.
#    fi
    echo "
The Java bundle that is included with Processing supports only i686 by default.
To build the code, you will need to install the Java 1.5.0_15 JDK (not a JRE,
and not any other version), and create a symlink to the directory where it is
installed. Create the symlink in the \"work\" directory, and named it \"java\":
ln -s /path/to/jdk1.5.0_15 `pwd`/work/java"
    exit
  fi
fi

cd ../..


### -- BUILD CORE ----------------------------------------------


echo Building processing.core

cd core

#CLASSPATH="../build/linux/work/java/lib/rt.jar"
#export CLASSPATH

perl preproc.pl
mkdir -p bin
../build/linux/work/java/bin/java \
    -cp ../build/linux/work/java/lib/tools.jar \
    com.sun.tools.javac.Main \
    -d bin -source 1.5 -target 1.5 \
    src/processing/core/*.java src/processing/xml/*.java
#find bin -name "*~" -exec rm -f {} ';'
rm -f ../build/linux/work/lib/core.jar
cd bin && zip -rq ../../build/linux/work/lib/core.jar \
  processing/core/*.class processing/xml/*.class && cd ..

# back to base processing dir
cd ..


### -- BUILD PDE ------------------------------------------------

cd app

rm -rf ../build/linux/work/classes
mkdir ../build/linux/work/classes

../build/linux/work/java/bin/java \
    -cp ../build/linux/work/java/lib/tools.jar \
    com.sun.tools.javac.Main \
    -source 1.5 -target 1.5 \
    -classpath ../build/linux/work/lib/core.jar:../build/linux/work/lib/antlr.jar:../build/linux/work/lib/ecj.jar:../build/linux/work/lib/jna.jar:../build/linux/work/lib/oro.jar:../build/linux/work/lib/RXTXcomm.jar:../build/linux/work/java/lib/tools.jar \
    -d ../build/linux/work/classes \
    src/processing/app/*.java \
    src/processing/app/debug/*.java \
    src/processing/app/linux/*.java \
    src/processing/app/preproc/*.java \
    src/processing/app/syntax/*.java \
    src/processing/app/tools/*.java

cd ../build/linux/work/classes
rm -f ../lib/pde.jar
zip -0rq ../lib/pde.jar .
cd ../../../..


echo
echo Done.
