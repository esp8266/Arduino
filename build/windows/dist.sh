#!/bin/sh

REVISION=`head -1 ../../todo.txt | awk '{print $1}'`

if [ $1 ]
then
  RELEASE=$1
  echo Creating Arduino release $RELEASE...
else 
  RELEASE=$REVISION
  echo Creating Arduino distribution for revision $REVISION...
fi

# check to see if the version number in the app is correct
# so that mikkel doesn't kick my ass
VERSIONED=`cat ../../app/src/processing/app/Base.java | grep $REVISION`
if [ -z "$VERSIONED" ]
then
  echo Fix the revision number in Base.java
  exit
fi

./make.sh

# remove any old boogers
rm -rf arduino
rm -rf arduino-*

mkdir arduino
cp -r ../shared/lib arduino/
cp -r ../shared/tools arduino/

cp dist/*.dll arduino/
cp -r dist/drivers arduino/

cp -r ../../hardware arduino/


if [ $1 ]
then
  # write the release version number into the output directory
  echo $1 > arduino/lib/version.txt
fi

cp ../../app/lib/antlr.jar arduino/lib/
cp ../../app/lib/ecj.jar arduino/lib/
cp ../../app/lib/jna.jar arduino/lib/
cp ../../app/lib/oro.jar arduino/lib/
cp ../../app/lib/RXTXcomm.jar arduino/lib/

cp ../../readme.txt arduino/

echo Copying examples...
cp -r ../shared/examples arduino/

echo Extracting reference...
unzip -q -d arduino/ ../shared/reference.zip

unzip -q -d arduino/hardware avr_tools.zip

# add java (jre) files
unzip -q -d arduino jre.zip

# get platform-specific goodies from the dist dir
cp launcher/arduino.exe arduino/

# grab pde.jar and export from the working dir
cp work/lib/pde.jar arduino/lib/
cp work/lib/core.jar arduino/lib/

# convert revisions.txt to windows LFs
# the 2> is because the app is a little chatty
unix2dos arduino/readme.txt 2> /dev/null
unix2dos arduino/lib/preferences.txt 2> /dev/null
unix2dos arduino/lib/keywords.txt 2> /dev/null

# remove boogers
find arduino -name "*.bak" -exec rm -f {} ';'
find arduino -name "*~" -exec rm -f {} ';'
find arduino -name ".DS_Store" -exec rm -f {} ';'
find arduino -name "._*" -exec rm -f {} ';'
find arduino -name "Thumbs.db" -exec rm -f {} ';'

# chmod +x the crew
find arduino -name "*.html" -exec chmod +x {} ';'
find arduino -name "*.dll" -exec chmod +x {} ';'
find arduino -name "*.exe" -exec chmod +x {} ';'
find arduino -name "*.html" -exec chmod +x {} ';'

# clean out the cvs entries
find arduino -name "CVS" -exec rm -rf {} ';' 2> /dev/null
find arduino -name ".cvsignore" -exec rm -rf {} ';'
find arduino -name ".svn" -exec rm -rf {} ';' 2> /dev/null

# zip it all up for release
echo Packaging standard release...
echo
P5=arduino-$RELEASE
mv arduino $P5
zip -rq $P5.zip $P5
# nah, keep the new directory around
#rm -rf $P5

# zip up another for experts
#echo Expert release is disabled until further notice.
echo Packaging expert release...
echo
cp -a $P5 $P5-expert
# remove enormous java runtime
rm -rf $P5-expert/java
zip -rq $P5-expert.zip $P5-expert

echo Done.

