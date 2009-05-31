#!/bin/sh

REVISION=`head -1 ../../todo.txt | awk '{print $1}'`

if [ $1 ]
then
  RELEASE=$1
  echo Creating Processing release $RELEASE...
else 
  RELEASE=$REVISION
  echo Creating Processing distribution for revision $REVISION...
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
rm -rf processing
rm -rf processing-*

mkdir processing
cp -r ../shared/lib processing/
cp -r ../shared/libraries processing/
cp -r ../shared/tools processing/

if [ $1 ]
then
  # write the release version number into the output directory
  echo $1 > processing/lib/version.txt
fi

cp ../../app/lib/antlr.jar processing/lib/
cp ../../app/lib/ecj.jar processing/lib/
cp ../../app/lib/jna.jar processing/lib/

cp ../shared/revisions.txt processing/

echo Extracting examples...
unzip -q -d processing/ ../shared/examples.zip

echo Extracting reference...
unzip -q -d processing/ ../shared/reference.zip

# add the libraries folder with source
cp -r ../../net processing/libraries/
cp -r ../../opengl processing/libraries/
cp -r ../../serial processing/libraries/
cp -r ../../video processing/libraries/
cp -r ../../pdf processing/libraries/
cp -r ../../dxf processing/libraries/

# add java (jre) files
unzip -q -d processing jre.zip

# get platform-specific goodies from the dist dir
cp launcher/processing.exe processing/

# grab pde.jar and export from the working dir
cp work/lib/pde.jar processing/lib/
cp work/lib/core.jar processing/lib/

# convert revisions.txt to windows LFs
# the 2> is because the app is a little chatty
unix2dos processing/revisions.txt 2> /dev/null
unix2dos processing/lib/preferences.txt 2> /dev/null
unix2dos processing/lib/keywords.txt 2> /dev/null

# remove boogers
find processing -name "*.bak" -exec rm -f {} ';'
find processing -name "*~" -exec rm -f {} ';'
find processing -name ".DS_Store" -exec rm -f {} ';'
find processing -name "._*" -exec rm -f {} ';'
find processing -name "Thumbs.db" -exec rm -f {} ';'

# chmod +x the crew
find processing -name "*.html" -exec chmod +x {} ';'
find processing -name "*.dll" -exec chmod +x {} ';'
find processing -name "*.exe" -exec chmod +x {} ';'
find processing -name "*.html" -exec chmod +x {} ';'

# clean out the cvs entries
find processing -name "CVS" -exec rm -rf {} ';' 2> /dev/null
find processing -name ".cvsignore" -exec rm -rf {} ';'
find processing -name ".svn" -exec rm -rf {} ';' 2> /dev/null

# zip it all up for release
echo Packaging standard release...
echo
P5=processing-$RELEASE
mv processing $P5
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

