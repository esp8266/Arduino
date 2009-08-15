#!/bin/sh

#REVISION=`head -c 4 ../../todo.txt`
REVISION=`head -1 ../../todo.txt | awk '{print $1}'`

if [ $1 ]
then
  RELEASE=$1
  echo Creating Arduino release $RELEASE...
else 
  RELEASE=$REVISION
  echo Creating Arduino distribution for revision $REVISION...
fi

ARCH=`uname -m`
if [ $ARCH != "i686" ]
then
  echo At present, the Linux distribution can only be built on i686 \(32-bit\).
  exit
fi

./make.sh

# remove any old boogers
rm -rf arduino
rm -rf arduino-*

mkdir arduino
cp -r ../shared/lib arduino/
cp -r ../shared/libraries arduino/
cp -r ../shared/tools arduino/
cp ../../app/lib/antlr.jar arduino/lib/
cp ../../app/lib/ecj.jar arduino/lib/
cp ../../app/lib/jna.jar arduino/lib/
cp ../../app/lib/oro.jar arduino/lib/
cp ../../app/lib/RXTXcomm.jar arduino/lib/
cp ../../readme.txt arduino/

cp -r ../../hardware arduino/

cp -r dist/tools arduino/hardware
cp dist/lib/librxtxSerial.so arduino/lib

if [ $1 ]
then
  # write the release version number into the output directory
  echo $1 > arduino/lib/version.txt
fi

echo Copying examples...
cp -r ../shared/examples arduino/

echo Extracting reference...
unzip -q -d arduino/ ../shared/reference.zip

# add java (jre) files
#tar --extract --file=jre.tgz --ungzip --directory=arduino

# grab pde.jar and export from the working dir
cp work/lib/pde.jar arduino/lib/
cp work/lib/core.jar arduino/lib/

# get platform-specific goodies from the dist dir
install -m 755 dist/arduino arduino/arduino

# make sure notes.txt is unix LFs
# the 2> is because the app is a little chatty
dos2unix arduino/readme.txt 2> /dev/null
dos2unix arduino/lib/preferences.txt 2> /dev/null

# remove boogers
find arduino -name "*~" -exec rm -f {} ';'
find arduino -name ".DS_Store" -exec rm -f {} ';'
find arduino -name "._*" -exec rm -f {} ';'
find arduino -name "Thumbs.db" -exec rm -f {} ';'

# clean out the cvs entries
find arduino -name "CVS" -exec rm -rf {} ';' 2> /dev/null
find arduino -name ".cvsignore" -exec rm -rf {} ';'
find arduino -name ".svn" -exec rm -rf {} 2> /dev/null ';'

# zip it all up for release
echo Creating tarball and finishing...
P5=arduino-$RELEASE
mv arduino $P5

tar cfz $P5.tgz $P5
# nah, keep the new directory around
#rm -rf $P5

echo Done.
