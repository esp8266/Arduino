#!/bin/sh


### -- SETUP WORK DIR -------------------------------------------

if test -d work
then
  BUILD_PREPROC=false
else
  echo Setting up directories to build arduino...
  BUILD_PREPROC=true
  cp -r ../shared work
  rm -rf work/CVS
  rm -f work/.DS_Store 
  
  # needs to make the dir because of packaging goofiness
  mkdir -p work/classes/arduino/app/preproc
  mkdir -p work/classes/arduino/app/syntax
  mkdir -p work/classes/arduino/app/tools


#  echo Extracting examples...
 # cd work
  #unzip -q examples.zip
 # rm examples.zip
 # cd ..

  echo Extracting reference...
#  cd work
#  unzip -q reference.zip
  # necessary for launching reference from shell/command prompt
  # which is done internally to view reference
  #chmod +x reference/*.html
  # needed by 'help' menu
  #chmod +x reference/environment/*.html
  # chmod -R +x *.html doesn't seem to work

#  rm reference.zip
 # cd ..

  echo Extracting enormous JRE...
  unzip -q  -d work jre.zip
  # cygwin requires this because of unknown weirdness
  # it was not formerly this anal retentive
  cd work/java/bin/
  #chmod +x *.exe *.dll 
  #chmod +x client/*.dll
  cd ../../..

  mkdir work/lib/build
  #mkdir work/classes

  echo Compiling arduino.exe
  cd launcher
  make && cp arduino.exe ../work/
  cd ..

  # get jikes and depedencies
  cp dist/jikes.exe work/
  #chmod +x work/jikes.exe

  cp dist/ICE_JNIRegistry.dll work/
  cp dist/serial/*.* work/lib/
  mkdir work/tools
  cp dist/avr_tools.zip .
  echo Extracting avr tools ...
  unzip -q  -d work avr.zip
  rm -f avr_tools.zip
  cp dist/tools/*.* work/tools
  cp dist/lib/makefile.win work/Makefile
  mkdir work/core
  cp ../../../core/*.* work/core
  cp dist/core/makefile.win work/core/Makefile

  # chmod +x the crew
  find work -name "*.dll" -exec chmod +x {} ';'
  find work -name "*.exe" -exec chmod +x {} ';'
  find work -name "*.html" -exec chmod +x {} ';'
fi

cd ../..


### -- BUILD PREPROC ---------------------------------------------

# i suck at shell scripting
#if [ $1 = "preproc" ] 
#then 
#BUILD_PREPROC=true
#fi

if $BUILD_PREPROC
then

echo Building PDE for JDK 1.4

cd app/preproc

# first build the default java goop
../../build/windows/work/java/bin/java \
    -cp "..\\..\\build\\windows\\work\\lib\\antlr.jar" antlr.Tool java.g

# now build the pde stuff that extends the java classes
../../build/windows/work/java/bin/java \
    -cp "..\\..\\build\\windows\\work\\lib\\antlr.jar" antlr.Tool \
    -glib java.g pde.g

# back to base arduino dir
cd ../..

fi


### -- BUILD PDE ------------------------------------------------

cd app

CLASSPATH="..\\build\\windows\\work\\lib\\RXTXcomm.jar;..\\build\\windows\\work\\lib\\core.jar;..\\build\\windows\\work\\lib\\mrj.jar;..\\build\\windows\\work\\lib\antlr.jar;..\\build\\windows\\work\\lib\\oro.jar;..\\build\\windows\\work\\lib\\registry.jar;..\\build\\windows\\work\\java\\lib\\rt.jar"

# compile the code as java 1.3, so that the application will run and
# show the user an error, rather than crapping out with some strange
# "class not found" crap
../build/windows/work/jikes -target 1.3 +D -classpath "$CLASSPATH;..\\build\\windows\\work\\classes" -d ..\\build\\windows\\work\\classes *.java preproc/*.java syntax/*.java tools/*.java
#/cygdrive/c/jdk-1.4.2_05/bin/javac.exe -classpath $CLASSPATH -d ..\\build\\windows\\work\\classes *.java jeditsyntax/*.java preprocessor/*.java

cd ../build/windows/work/classes
rm -f ../lib/pde.jar
zip -0rq ../lib/pde.jar .

# back to build/windows
cd ../..


### -- BUILD LIBRARIES ------------------------------------------------


PLATFORM=windows


CLASSPATH="..\\build\\$PLATFORM\\work\\lib\\core.jar;..\\build\\$PLATFORM\\work\\java\\lib\\rt.jar"
JIKES=../build/$PLATFORM/work/jikes
CORE="..\\build\\$PLATFORM\\work\\lib\\core.jar"
LIBRARIES="..\\build\\$PLATFORM\\work\\libraries"

# move to arduino/build 
cd ..



CLASSPATH="..\\..\\build\\$PLATFORM\\work\\lib\\core.jar;..\\..\\build\\$PLATFORM\\work\\java\\lib\\rt.jar"
JIKES=../../build/$PLATFORM/work/jikes
CORE=..\\..\\build\\$PLATFORM\\work\\lib\\core.jar
LIBRARIES=..\\..\\build\\$PLATFORM\\work\\libraries


# PARTICLES LIBRARY
#echo Build particles library...
#cd ../lib/particles
#$JIKES -target 1.1 +D -d . *.java 
#rm -f library/particles.jar
#zip -r0q library/particles.jar simong
#rm -rf simong
#mkdir -p $LIBRARIES/particles/library/
#cp library/particles.jar $LIBRARIES/particles/library/

echo
echo Done.

