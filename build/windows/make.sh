#!/bin/sh


### -- SETUP WORK DIR -------------------------------------------

if test -d work
then
  BUILD_PREPROC=false
else
  echo Setting up directories to build P5...
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

  echo Extracting enormous JRE...
  unzip -q -d work jre.zip

  # build the export launcher
  cd export
  make 
  cd ..

  # build the processing.exe bundle
  # there are a few hacks in the source to launch4j-3.0.1
  # to build them, use the following:
  # cd head_src/gui_head && make -f Makefile.win
  cd launcher
  ./launch4j/launch4jc.exe config.xml
  cp processing.exe ../work/
  cd ..

  # chmod +x the crew
  # cygwin requires this because of unknown weirdness
  # it was not formerly this anal retentive
  # with the html, it's necessary on windows for launching reference 
  # from shell/command prompt, which is done internally to view reference
  find work -name "*.html" -exec chmod +x {} ';'
  find work -name "*.dll" -exec chmod +x {} ';'
  find work -name "*.exe" -exec chmod +x {} ';'
  find work -name "*.html" -exec chmod +x {} ';'
fi

cd ../..


### -- BUILD CORE ----------------------------------------------

echo Building processing.core...

cd core

#CLASSPATH="..\\build\\windows\\work\\java\\lib\\rt.jar;..\\build\\windows\\work\\java\\lib\\tools.jar"
#CLASSPATH="..\\build\\windows\\work\\java\\lib\\tools.jar"
#export CLASSPATH

perl preproc.pl

mkdir -p bin
../build/windows/work/java/bin/java \
    -classpath "..\\build\\windows\\work\\java\\lib\\tools.jar" \
    com.sun.tools.javac.Main \
    -source 1.5 -target 1.5 -d bin \
    src/processing/core/*.java src/processing/xml/*.java

rm -f ../build/windows/work/lib/core.jar

# package this folder into core.jar
cd bin && zip -rq ../../build/windows/work/lib/core.jar \
  processing/core/*.class processing/xml/*.class && cd ..

# back to base processing dir
cd ..

### -- BUILD PREPROC ---------------------------------------------

if $BUILD_PREPROC
then

echo Building PDE for JDK 1.5

cd app

# first build the default java goop
../build/windows/work/java/bin/java \
    -cp "..\\build\\windows\\work\\lib\\antlr.jar" antlr.Tool \
    -o src/antlr/java \
    src/antlr/java/java.g

# now build the pde stuff that extends the java classes
#../../build/windows/work/java/bin/java \
#    -cp "..\\..\\build\\windows\\work\\lib\\antlr.jar" antlr.Tool \
#    -o src/processing/app/preproc \
#    -glib antlr/java/java.g processing/app/preproc/pde.g

# this is totally ugly and needs to be fixed
# the problem is that -glib doesn't set the main path properly, 
# so it's necessary to cd into the antlr/java folder, otherwise
# the JavaTokenTypes.txt file won't be found

# this is the eventual hack to make things work
# why this is required on windows and not the others is beyond me
cp src/antlr/java/JavaTokenTypes.txt src/processing/app/preproc/

# this is a total disaster...fix me soon!
cd src/processing/app/preproc
../../../../../build/windows/work/java/bin/java \
  -cp "..\\..\\..\\..\\..\\build\\windows\\work\\lib\\antlr.jar" antlr.Tool \
  -glib ../../../antlr/java/java.g \
  pde.g
cd ../../../..


# back to base processing dir
cd ..

fi


### -- BUILD PDE ------------------------------------------------

cd app

# has to be present, otherwise javac will complain of file writing errors
rm -rf ../build/windows/work/classes
mkdir ../build/windows/work/classes

../build/windows/work/java/bin/java \
    -classpath "..\\build\\windows\\work\\java\\lib\\tools.jar" \
    com.sun.tools.javac.Main \
    -source 1.5 -target 1.5 \
    -classpath "..\\build\\windows\\work\\lib\\core.jar;..\\build\\windows\\work\\lib\antlr.jar;..\\build\\windows\\work\\lib\\ecj.jar;..\\build\\windows\\work\\lib\\jna.jar;..\\build\\windows\\work\\java\\lib\\tools.jar" \
    -d ..\\build\\windows\\work\\classes \
    src/processing/app/*.java \
    src/processing/app/debug/*.java \
    src/processing/app/syntax/*.java \
    src/processing/app/preproc/*.java \
    src/processing/app/tools/*.java \
    src/processing/app/windows/*.java \
    src/antlr/*.java \
    src/antlr/java/*.java 

cd ../build/windows/work/classes
rm -f ../lib/pde.jar
zip -rq ../lib/pde.jar .
cd ../..


### -- BUILD LIBRARIES ------------------------------------------------

PLATFORM=windows


#CLASSPATH="..\\build\\$PLATFORM\\work\\lib\\core.jar;..\\build\\$PLATFORM\\work\\java\\lib\\rt.jar"
#CLASSPATH="..\\build\\$PLATFORM\\work\\lib\\core.jar"

JAVAC="../build/windows/work/java/bin/java -classpath ..\\build\\windows\\work\\java\\lib\\tools.jar com.sun.tools.javac.Main -source 1.5 -target 1.5"
CORE="..\\build\\$PLATFORM\\work\\lib\\core.jar"
LIBRARIES="..\\build\\$PLATFORM\\work\\libraries"

# move to processing/build 
cd ..


# SERIAL LIBRARY
echo Building serial library...
cd ../serial
mkdir -p bin
$JAVAC \
    -classpath "library\\RXTXcomm.jar;$CORE" \
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


# VIDEO LIBRARY
echo Building video library...
if test -f "${QTJAVA}"
then 
  echo "Found Quicktime 7 installation"
else
  QTJAVA="$WINDIR\\system32\\QTJava.zip"
  if test -f "${QTJAVA}"
  then
    echo "Found Quicktime 6 at $QTJAVA"
  else 
    echo "Could not find QuickTime for Java,"
    echo "you'll need to install it before building."
    exit 1;
  fi
fi
cd ../video
mkdir -p bin
$JAVAC \
    -classpath "$QTJAVA;$CORE" \
    -d bin src/processing/video/*.java 
rm -f library/video.jar
find bin -name "*~" -exec rm -f {} ';'
cd bin && zip -rq ../library/video.jar processing/video/*.class && cd ..
mkdir -p $LIBRARIES/video/library/
cp library/video.jar $LIBRARIES/video/library/


# OPENGL LIBRARY
echo Building OpenGL library...
cd ../opengl
mkdir -p bin
$JAVAC \
    -classpath "library\\jogl.jar;$CORE" \
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
    -classpath "library\\itext.jar;$CORE" \
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

