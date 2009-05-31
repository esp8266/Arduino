#!/bin/sh

#REVISION=`head -c 4 ../../todo.txt`
REVISION=`head -1 ../../todo.txt | awk '{print $1}'`

if [ $1 ]
then
  RELEASE=$1
  echo Creating Processing release $RELEASE...
else 
  RELEASE=$REVISION
  echo Creating Processing distribution for revision $REVISION...
fi

ARCH=`uname -m`
if [ $ARCH != "i686" ]
then
  echo At present, the Linux distribution can only be built on i686 \(32-bit\).
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
cp ../../app/lib/antlr.jar processing/lib/
cp ../../app/lib/ecj.jar processing/lib/
cp ../../app/lib/jna.jar processing/lib/
cp ../shared/revisions.txt processing/

if [ $1 ]
then
  # write the release version number into the output directory
  echo $1 > processing/lib/version.txt
fi

echo Extracting examples...
unzip -q -d processing/ ../shared/examples.zip

echo Extracting reference...
unzip -q -d processing/ ../shared/reference.zip

# add the libraries folder with source
cp -r ../../net processing/libraries/
cp -r ../../opengl processing/libraries/
cp -r ../../serial processing/libraries/
cp -r ../../pdf processing/libraries/
cp -r ../../dxf processing/libraries/

# add java (jre) files
tar --extract --file=jre.tgz --ungzip --directory=processing

# grab pde.jar and export from the working dir
cp work/lib/pde.jar processing/lib/
cp work/lib/core.jar processing/lib/

# get platform-specific goodies from the dist dir
install -m 755 dist/processing processing/processing

# make sure notes.txt is unix LFs
# the 2> is because the app is a little chatty
dos2unix processing/revisions.txt 2> /dev/null
dos2unix processing/lib/preferences.txt 2> /dev/null

# remove boogers
find processing -name "*~" -exec rm -f {} ';'
find processing -name ".DS_Store" -exec rm -f {} ';'
find processing -name "._*" -exec rm -f {} ';'
find processing -name "Thumbs.db" -exec rm -f {} ';'

# clean out the cvs entries
find processing -name "CVS" -exec rm -rf {} ';' 2> /dev/null
find processing -name ".cvsignore" -exec rm -rf {} ';'
find processing -name ".svn" -exec rm -rf {} 2> /dev/null ';'

# zip it all up for release
echo Creating tarball and finishing...
P5=processing-$RELEASE
mv processing $P5

tar cfz $P5.tgz $P5
# nah, keep the new directory around
#rm -rf $P5

echo Done.
