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

  cp ../../app/lib/antlr.jar work/lib/
  cp ../../app/lib/ecj.jar work/lib/
  cp ../../app/lib/jna.jar work/lib/

  echo Extracting examples...
  unzip -q -d work/ ../shared/examples.zip

  echo Extracting reference...
  unzip -q -d work/ ../shared/reference.zip

  cp -r ../../net work/libraries/
  cp -r ../../opengl work/libraries/
  cp -r ../../serial work/libraries/
  cp -r ../../video work/libraries/
  cp -r ../../pdf work/libraries/
  cp -r ../../dxf work/libraries/

  install -m 755 dist/processing work/processing

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


### -- BUILD PREPROC ------------------------------------------------

echo Building PDE for JDK 1.5...

cd app

# long path is to avoid requiring java to be in your PATH
  echo Building antlr grammar code...

  # first build the default java goop
../build/linux/work/java/bin/java \
  -cp ../build/linux/work/lib/antlr.jar antlr.Tool \
  -o src/antlr/java \
  src/antlr/java/java.g

  # hack to get around path mess
  cp src/antlr/java/JavaTokenTypes.txt src/processing/app/preproc/

# now build the pde stuff that extends the java classes
# this is totally ugly and needs to be fixed
# the problem is that -glib doesn't set the main path properly, 
# so it's necessary to cd into the antlr/java folder, otherwise
# the JavaTokenTypes.txt file won't be found
cd src/antlr/java
../../../../build/linux/work/java/bin/java \
  -cp ../../../../build/linux/work/lib/antlr.jar antlr.Tool \
  -o ../../processing/app/preproc \
  -glib java.g \
  ../../processing/app/preproc/pde.g
cd ../../..

# return to the root of the p5 folder
cd ..


### -- BUILD PDE ------------------------------------------------

cd app

rm -rf ../build/linux/work/classes
mkdir ../build/linux/work/classes

../build/linux/work/java/bin/java \
    -cp ../build/linux/work/java/lib/tools.jar \
    com.sun.tools.javac.Main \
    -source 1.5 -target 1.5 \
    -classpath ../build/linux/work/lib/core.jar:../build/linux/work/lib/antlr.jar:../build/linux/work/lib/ecj.jar:../build/linux/work/lib/jna.jar:../build/linux/work/java/lib/tools.jar \
    -d ../build/linux/work/classes \
    src/processing/app/*.java \
    src/processing/app/debug/*.java \
    src/processing/app/linux/*.java \
    src/processing/app/preproc/*.java \
    src/processing/app/syntax/*.java \
    src/processing/app/tools/*.java \
    src/antlr/*.java \
    src/antlr/java/*.java 

cd ../build/linux/work/classes
rm -f ../lib/pde.jar
zip -0rq ../lib/pde.jar .
cd ../../../..


### -- BUILD LIBRARIES ------------------------------------------------

cd build/linux

PLATFORM=linux

JAVAC="../build/linux/work/java/bin/java -cp ../build/linux/work/java/lib/tools.jar com.sun.tools.javac.Main -source 1.5 -target 1.5"
CORE=../build/$PLATFORM/work/lib/core.jar
LIBRARIES=../build/$PLATFORM/work/libraries

# move to processing/build 
cd ..

# SERIAL LIBRARY
echo Building serial library...
cd ../serial
mkdir -p bin
$JAVAC \
    -classpath "library/RXTXcomm.jar:$CORE" \
    -d bin src/processing/serial/*.java 
rm -f library/serial.jar
find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/serial.jar processing/serial/*.class && cd ..
mkdir -p $LIBRARIES/serial/library/
cp library/serial.jar $LIBRARIES/serial/library/


# NET LIBRARY
echo Building net library...
cd ../net
mkdir -p bin
$JAVAC \
    -classpath "$CORE" \
    -d bin src/processing/net/*.java 
rm -f library/net.jar
find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/net.jar processing/net/*.class && cd ..
mkdir -p $LIBRARIES/net/library/
cp library/net.jar $LIBRARIES/net/library/


# OPENGL LIBRARY
echo Building OpenGL library...
cd ../opengl
mkdir -p bin
$JAVAC \
    -classpath "library/jogl.jar:$CORE" \
    -d bin src/processing/opengl/*.java 
rm -f library/opengl.jar
find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/opengl.jar processing/opengl/*.class && cd ..
mkdir -p $LIBRARIES/opengl/library/
cp library/opengl.jar $LIBRARIES/opengl/library/


# PDF LIBRARY
echo Building PDF library...
cd ../pdf
mkdir -p bin
$JAVAC \
    -classpath "library/itext.jar:$CORE" \
    -d bin src/processing/pdf/*.java 
rm -f library/pdf.jar
find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/pdf.jar processing/pdf/*.class && cd ..
mkdir -p $LIBRARIES/pdf/library/
cp library/pdf.jar $LIBRARIES/pdf/library/


# DXF LIBRARY
echo Building DXF library...
cd ../dxf
mkdir -p bin
$JAVAC \
    -classpath "$CORE" \
    -d bin src/processing/dxf/*.java 
rm -f library/dxf.jar
find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/dxf.jar processing/dxf/*.class && cd ..
mkdir -p $LIBRARIES/dxf/library/
cp library/dxf.jar $LIBRARIES/dxf/library/


echo
echo Done.
