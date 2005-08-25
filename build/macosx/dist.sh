#!/bin/sh

# part of the arduino project http://arduino.berlios.de
# take from processing http://www.processing.org
# by Ben Fry, Casey Reas et al
#
# the power of open source


# prefers that fink is intalled, but not required
if test -f /sw/bin/head
then
  # old 4 char version.. osx only uses the two chars
  #REVISION=`head -c 4 ../../todo.txt`
  # a more useful version of head than what's included with osx
  SHORT_REVISION=`/sw/bin/head -c 4 ../../todo.txt | tail -c 2`
  REVISION=`/sw/bin/head -c 4 ../../todo.txt`

  VERSIONED=`cat ../../app/Base.java | grep $REVISION`
  if [ -z "$VERSIONED" ]
  then
    echo Fix the revision number in Base.java
    exit
  fi

else
  # can't get four bytes of head (osx doesn't support -c)
  SHORT_REVISION=00
  REVISION=0000
fi


./make.sh

echo Creating Arduino distribution for revision $REVISION...

# remove any old boogers
rm -rf arduino 
rm -rf Arduino*
rm -rf arduino-*

mkdir arduino


# use 'shared' files as starting point
cp -r ../shared arduino

# add the libraries folder with source
#cp -r ../../lib arduino/libraries


# new style examples thing ala reas
# not there yet in arduino
# cd arduino
# unzip -q examples.zip
# rm examples.zip
# cd ..

# new style reference
# not there yet in arduino
# cd arduino
# unzip -q reference.zip
# rm reference.zip
# cd ..

# get ds_store file (!)
cp dist/DS_Store arduino/.DS_Store

# get package from the dist dir
cp -R dist/Arduino.app arduino/
chmod +x arduino/Arduino.app/Contents/MacOS/JavaApplicationStub

# put jar files into the resource dir, leave the rest in lib
RES=arduino/Arduino.app/Contents/Resources/Java
mkdir -p $RES
mv work/lib/*.jar $RES/

# directories used by the app
#mkdir arduino/lib/build

# grab pde.jar and export from the working dir
cp work/Arduino.app/Contents/Resources/Java/pde.jar $RES/

# removed dependecies from the processing core
#cp work/lib/core.jar arduino/lib/

# get platform-specific goodies from the dist dir
#cp `which jikes` arduino
#gunzip < dist/jikes.gz > arduino/jikes

# not needed in arduino
# cp dist/jikes arduino/
# chmod a+x arduino /jikes

chmod a+x arduino/Arduino.app/Contents/MacOS/JavaApplicationStub

#cd ../..
#javadoc -public -d doc app/*.java app/preproc/*.java app/syntax/*.java core/*.java opengl/*.java net/*.java video/*.java serial/*.java
#cd build/macosx

# remove boogers
find arduino -name "*~" -exec rm -f {} ';'
# need to leave ds store stuff cuz one of those is important
#find arduino -name ".DS_Store" -exec rm -f {} ';'
find arduino -name "._*" -exec rm -f {} ';'
find arduino -name "Thumbs.db" -exec rm -f {} ';'

# clean out the cvs entries
find arduino -name "CVS" -exec rm -rf {} ';' 2> /dev/null
find arduino -name ".cvsignore" -exec rm -rf {} ';'

mv arduino/Arduino.app "arduino/Arduino $SHORT_REVISION.app"
mv arduino arduino-$REVISION

# don't have deluxe on my laptop right now
#stuff -f sitx arduino-$REVISION

# zip it all up for release
#NICE_FOLDER="Arduino $SHORT_REVISION"
#DMG_NAME="arduino-$REVISION"
#mv arduino "$NICE_FOLDER"
#chmod +x mkdmg
#./mkdmg "$NICE_FOLDER" "Arduino"
#mv "$NICE_FOLDER.dmg" "$DMG_NAME.dmg"

# actually, could probably use:
# open arduino-uncomp.dmg
# rm -rf /Volumes/Arduino/Arduino*
# mv "Arduino $REVISION" /Volumes/Arduino
# umount /Volumes/Arduino

echo Done.
