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
  cp -r ../shared/tools work/

  cp dist/*.dll work/
  cp -r dist/drivers work/

  cp -r ../../hardware work/
  cp -r ../../libraries work/

  cp ../../app/lib/antlr.jar work/lib/
  cp ../../app/lib/ecj.jar work/lib/
  cp ../../app/lib/jna.jar work/lib/
  cp ../../app/lib/oro.jar work/lib/
  cp ../../app/lib/RXTXcomm.jar work/lib/

  echo Copying examples...
  cp -r ../shared/examples work/

  echo Extracting reference...
  unzip -q -d work/ ../shared/reference.zip

  echo Extracting avr tools...
  unzip -q -d work/hardware/ avr_tools.zip

  echo Extracting enormous JRE...
  unzip -q -d work/ jre.zip

  # build the processing.exe bundle
  # there are a few hacks in the source to launch4j-3.0.1
  # to build them, use the following:
  # cd head_src/gui_head && make -f Makefile.win
  cd launcher
  ./launch4j/launch4jc.exe config.xml
  cp arduino.exe ../work/
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



### -- BUILD PDE ------------------------------------------------

echo Building the PDE...

cd app

# has to be present, otherwise javac will complain of file writing errors
rm -rf ../build/windows/work/classes
mkdir ../build/windows/work/classes

../build/windows/work/java/bin/java \
    -classpath "..\\build\\windows\\work\\java\\lib\\tools.jar" \
    com.sun.tools.javac.Main \
    -source 1.5 -target 1.5 \
    -classpath "..\\build\\windows\\work\\lib\\core.jar;..\\build\\windows\\work\\lib\antlr.jar;..\\build\\windows\\work\\lib\\ecj.jar;..\\build\\windows\\work\\lib\\jna.jar;..\\build\\windows\\work\\lib\\oro.jar;..\\build\\windows\\work\\lib\\RXTXcomm.jar;..\\build\\windows\\work\\java\\lib\\tools.jar" \
    -d ..\\build\\windows\\work\\classes \
    src/processing/app/*.java \
    src/processing/app/debug/*.java \
    src/processing/app/syntax/*.java \
    src/processing/app/preproc/*.java \
    src/processing/app/tools/*.java \
    src/processing/app/windows/*.java

cd ../build/windows/work/classes
rm -f ../lib/pde.jar
zip -rq ../lib/pde.jar .
cd ../..


echo
echo Done.

