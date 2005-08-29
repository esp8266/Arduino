#!/bin/sh

# part of the arduino project http://arduino.berlios.de
# take from processing http://www.processing.org
# by Ben Fry, Casey Reas et al
#
# the power of open source


REVISION=`head -1 ../../todo.txt | cut -c 1-4`
SHORT_REVISION=`head -1 ../../todo.txt | cut -c 3-4`

VERSIONED=`cat ../../app/Base.java | grep $REVISION`
if [ -z "$VERSIONED" ]
then
  echo Fix the revision number in Base.java
  exit
fi

echo REBUILDING ARDUINO
rm -rf work
./make.sh

echo CREATING ARDUINO $REVISION DISTRIBUTION

# remove any old boogers
rm -rf arduino 
rm -rf Arduino*
rm -rf arduino-*

mkdir arduino

# use 'work' files as starting point
cp -r work/* arduino

rm -rf arduino/classes

# remove boogers
find arduino -name "*~" -exec rm -f {} ';'
# need to leave ds store stuff cuz one of those is important
#find arduino -name ".DS_Store" -exec rm -f {} ';'
find arduino -name "._*" -exec rm -f {} ';'
find arduino -name "Thumbs.db" -exec rm -f {} ';'

# clean out the cvs entries
find arduino -name "CVS" -exec rm -rf {} ';' 2> /dev/null
find arduino -name ".cvsignore" -exec rm -rf {} ';'

# clean out the svn entries
find arduino -name ".svn" -exec rm -rf {} ';' 2> /dev/null

mv arduino/Arduino.app "arduino/Arduino $SHORT_REVISION.app"
mv arduino arduino-$REVISION
#zip -r arduino-$REVISION.zip arduino-$REVISION

rm -Rf tmp

mkdir tmp
mv arduino-$REVISION/ tmp/
hdiutil create -fs HFS+ -srcfolder "./tmp/" -volname "arduino-$REVISION" "arduino-$REVISION.dmg"

#` don't have deluxe on my laptop right now
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
