#!/bin/sh


### -- SETUP WORK DIR -------------------------------------------

RESOURCES=`pwd`/work/Processing.app/Contents/Resources/Java
#echo $RESOURCES
#exit

if test -d work
then
  BUILD_PREPROC=false
else
  echo Setting up directories to build under Mac OS X
  BUILD_PREPROC=true

  mkdir work

  # to have a copy of this guy around for messing with
  echo Copying Processing.app...
  #cp -a dist/Processing.app work/   # #@$(* bsd switches
  #/sw/bin/cp -a dist/Processing.app work/
  cp -pRX dist/Processing.app work/
  # cvs doesn't seem to want to honor the +x bit 
  chmod +x work/Processing.app/Contents/MacOS/JavaApplicationStub

  cp -rX ../shared/lib "$RESOURCES/"
  cp -rX ../shared/libraries "$RESOURCES/"
  cp -rX ../shared/tools "$RESOURCES/"

  cp -X ../../app/lib/antlr.jar "$RESOURCES/"
  cp -X ../../app/lib/ecj.jar "$RESOURCES/"
  cp -X ../../app/lib/jna.jar "$RESOURCES/"

  echo Extracting examples...
  unzip -q -d "$RESOURCES/" ../shared/examples.zip

  echo Extracting reference...
  unzip -q -d "$RESOURCES/" ../shared/reference.zip

  LIBRARIES=$RESOURCES/libraries/
  cp -rX ../../net "$LIBRARIES"
  cp -rX ../../opengl "$LIBRARIES"
  cp -rX ../../serial "$LIBRARIES"
  cp -rX ../../video "$LIBRARIES"
  cp -rX ../../pdf "$LIBRARIES"
  cp -rX ../../dxf "$LIBRARIES"
fi


### -- START BUILDING -------------------------------------------

# move to root 'processing' directory
cd ../..


### -- BUILD CORE ----------------------------------------------

echo Building processing.core...

cd core

#CLASSPATH=/System/Library/Frameworks/JavaVM.framework/Classes/classes.jar:/System/Library/Frameworks/JavaVM.framework/Classes/ui.jar:/System/Library/Java/Extensions/QTJava.zip
#export CLASSPATH

perl preproc.pl

mkdir -p bin
javac -source 1.5 -target 1.5 -d bin \
  src/processing/core/*.java \
  src/processing/xml/*.java

rm -f "$RESOURCES/core.jar"

cd bin && \
  zip -rq "$RESOURCES/core.jar" \
  processing/core/*.class \
  processing/xml/*.class \
  && cd ..

# head back to "processing/app"
cd ../app



### -- BUILD PARSER ---------------------------------------------

#BUILD_PREPROC=true

if $BUILD_PREPROC
then
  # build classes/grammar for preprocessor
  echo Building antlr grammar code...
  # first build the default java goop
  java -cp "$RESOURCES/antlr.jar" antlr.Tool \
    -o src/antlr/java \
    src/antlr/java/java.g

  # hack to get around path mess
  cp -X src/antlr/java/JavaTokenTypes.txt src/processing/app/preproc/

  # now build the pde stuff that extends the java classes
  java -cp "$RESOURCES/antlr.jar" antlr.Tool \
    -o src/processing/app/preproc \
    -glib src/antlr/java/java.g src/processing/app/preproc/pde.g
fi

### -- BUILD PDE ------------------------------------------------

echo Building the PDE...

# For some reason, javac really wants this folder to exist beforehand.
rm -rf ../build/macosx/work/classes
mkdir ../build/macosx/work/classes
# Intentionally keeping this separate from the 'bin' folder
# used by eclipse so that they don't cause conflicts.

javac \
    -Xlint:deprecation \
    -source 1.5 -target 1.5 \
    -classpath "$RESOURCES/core.jar:$RESOURCES/antlr.jar:$RESOURCES/ecj.jar:$RESOURCES/jna.jar" \
    -d ../build/macosx/work/classes \
    src/processing/app/*.java \
    src/processing/app/debug/*.java \
    src/processing/app/macosx/*.java \
    src/processing/app/preproc/*.java \
    src/processing/app/syntax/*.java \
    src/processing/app/tools/*.java \
    src/antlr/*.java \
    src/antlr/java/*.java 

cd ../build/macosx/work/classes
rm -f "$RESOURCES/pde.jar"
zip -0rq "$RESOURCES/pde.jar" .
cd ../..

# get updated core.jar and pde.jar; also antlr.jar and others
#mkdir -p work/Processing.app/Contents/Resources/Java/
#cp work/lib/*.jar work/Processing.app/Contents/Resources/Java/


### -- BUILD LIBRARIES ------------------------------------------------

PLATFORM=macosx


CLASSPATH=$RESOURCES/core.jar
JAVAC="javac -source 1.5 -target 1.5"
LIBRARIES=$RESOURCES/libraries

# move to processing/build 
cd ..


# SERIAL LIBRARY
echo Building serial library...
cd ../serial
mkdir -p bin
$JAVAC \
    -classpath "library/RXTXcomm.jar:$CLASSPATH" \
    -d bin src/processing/serial/*.java 
rm -f library/serial.jar
#find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/serial.jar processing/serial/*.class && cd ..
mkdir -p "$LIBRARIES/serial/library/"
cp library/serial.jar "$LIBRARIES/serial/library/"


# NET LIBRARY
echo Building net library...
cd ../net
mkdir -p bin
$JAVAC \
    -classpath "$CLASSPATH" \
    -d bin src/processing/net/*.java 
rm -f library/net.jar
#find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/net.jar processing/net/*.class && cd ..
mkdir -p "$LIBRARIES/net/library/"
cp library/net.jar "$LIBRARIES/net/library/"


# VIDEO LIBRARY
echo Building video library...
QTJAVA=/System/Library/Java/Extensions/QTJava.zip
if test -f "${QTJAVA}"
then
  echo "Found QuickTime for Java at $QTJAVA"
else 
  echo "QuickTime for Java must be installed before building."
  exit 1;
fi
cd ../video
mkdir -p bin
$JAVAC \
    -classpath "$QTJAVA:$CLASSPATH" \
    -d bin src/processing/video/*.java 
rm -f library/video.jar
#find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/video.jar processing/video/*.class && cd ..
mkdir -p "$LIBRARIES/video/library/"
cp library/video.jar "$LIBRARIES/video/library/"


# OPENGL LIBRARY
echo Building OpenGL library...
cd ../opengl
mkdir -p bin
$JAVAC \
    -classpath "library/jogl.jar:$CLASSPATH" \
    -d bin src/processing/opengl/*.java 
rm -f library/opengl.jar
#find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/opengl.jar processing/opengl/*.class && cd ..
mkdir -p "$LIBRARIES/opengl/library/"
cp library/opengl.jar "$LIBRARIES/opengl/library/"


# PDF LIBRARY
echo Building PDF library...
cd ../pdf
mkdir -p bin
$JAVAC \
    -classpath "library/itext.jar:$CLASSPATH" \
    -d bin src/processing/pdf/*.java 
rm -f library/pdf.jar
#find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/pdf.jar processing/pdf/*.class && cd ..
mkdir -p "$LIBRARIES/pdf/library/"
cp library/pdf.jar "$LIBRARIES/pdf/library/"


# DXF LIBRARY
echo Building DXF library...
cd ../dxf
mkdir -p bin
$JAVAC \
    -classpath "$CLASSPATH" \
    -d bin src/processing/dxf/*.java 
rm -f library/dxf.jar
#find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/dxf.jar processing/dxf/*.class && cd ..
mkdir -p "$LIBRARIES/dxf/library/"
cp library/dxf.jar "$LIBRARIES/dxf/library/"


echo
echo Done.