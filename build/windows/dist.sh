#!/bin/sh

REVISION=`head -c 4 ../../todo.txt`

# check to see if the version number in the app is correct
# so that mikkel doesn't kick my ass
VERSIONED=`cat ../../app/Base.java | grep $REVISION`
if [ -z "$VERSIONED" ]
then
  echo Fix the revision number in Base.java
  exit
fi

./make.sh

echo Creating P5 distribution for revision $REVISION...
echo

# remove any old boogers
rm -rf processing
rm -rf processing-*

# use 'shared' files as starting point
cp -r ../shared processing

# add the libraries folder with source
cp -r ../../net processing/libraries/
cp -r ../../opengl processing/libraries/
cp -r ../../serial processing/libraries/
cp -r ../../video processing/libraries/

# new style examples thing ala reas
cd processing
unzip -q examples.zip
rm examples.zip
cd ..

# new style reference
cd processing
unzip -q reference.zip
# necessary for launching reference from shell/command prompt
# which is done internally to view reference
chmod +x reference/*.html
# needed by 'help' menu
chmod +x reference/environment/*.html
# get rid of the zip file
rm reference.zip
cd ..

# add java (jre) files
unzip -q -d processing jre.zip

# directories used by the app
#mkdir processing/lib/build

# grab pde.jar and export from the working dir
cp work/lib/pde.jar processing/lib/
cp work/lib/core.jar processing/lib/
#cp -r work/lib/export processing/lib/
#rm -rf processing/lib/export/CVS

# get jikes and depedencies
#gunzip < dist/jikes.gz > processing/jikes.exe
cp dist/jikes.exe processing/
chmod +x processing/jikes.exe

cp dist/ICE_JNIRegistry.dll processing/
chmod +x processing/ICE_JNIRegistry.dll

# get platform-specific goodies from the dist dir
cp launcher/processing.exe processing/
cp dist/run.bat processing/

# convert notes.txt to windows LFs
# the 2> is because the app is a little chatty
unix2dos processing/revisions.txt 2> /dev/null
unix2dos processing/lib/preferences.txt 2> /dev/null
unix2dos processing/lib/keywords.txt 2> /dev/null
rm -f processing/*.bak
rm -f processing/lib/*.bak

# remove boogers
find processing -name "*~" -exec rm -f {} ';'
find processing -name ".DS_Store" -exec rm -f {} ';'
find processing -name "._*" -exec rm -f {} ';'
find processing -name "Thumbs.db" -exec rm -f {} ';'

# chmod +x the crew
find processing -name "*.dll" -exec chmod +x {} ';'
find processing -name "*.exe" -exec chmod +x {} ';'
find processing -name "*.html" -exec chmod +x {} ';'

# clean out the cvs entries
find processing -name "CVS" -exec rm -rf {} ';' 2> /dev/null
find processing -name ".cvsignore" -exec rm -rf {} ';'

# zip it all up for release
echo Packaging standard release...
echo
P5=processing-$REVISION
mv processing $P5
zip -rq $P5.zip $P5
# nah, keep the new directory around
#rm -rf $P5

# zip up another for experts
echo Packaging expert release...
echo

cp -a $P5 $P5-expert

# can't use the run.bat that's tied to a local jre
rm $P5-expert/run.bat
cp dist/run-expert.bat $P5-expert/

# remove enormous java runtime
rm -rf $P5-expert/java
zip -rq $P5-expert.zip $P5-expert

echo Done.

