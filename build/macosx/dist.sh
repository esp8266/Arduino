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

VERSIONED=`cat ../../app/src/processing/app/Base.java | grep $REVISION`
if [ -z "$VERSIONED" ]
then
  echo Fix the revision number in Base.java
  exit
fi

# remove any unfinished builds or old builds
rm -rf processing
rm -rf Processing*
rm -rf processing-*
rm -rf work

./make.sh

if [ $1 ]
then
  # write the release version number into the output directory
  echo $1 > work/Processing.app/Contents/Resources/Java/lib/version.txt
fi

echo Cleaning file boogers...

# remove boogers
find work -name "*~" -exec rm -f {} ';'
# need to leave ds store stuff cuz one of those is important
#find processing -name ".DS_Store" -exec rm -f {} ';'
find work -name "._*" -exec rm -f {} ';'
find work -name "Thumbs.db" -exec rm -f {} ';'

# clean out the cvs entries
find work -name "CVS" -exec rm -rf {} ';' 2> /dev/null
find work -name ".cvsignore" -exec rm -rf {} ';'
find work -name ".svn" -exec rm -rf {} 2> /dev/null ';'


# the following was adopted from the makefile by Remko Troncon:
# http://el-tramo.be/guides/fancy-dmg

echo Creating disk image...

SOURCE_DIR="work"
SOURCE_FILES="Processing.app"
OUTPUT_DMG="processing-$RELEASE"
WORK_DMG="working.dmg"
WORK_DIR="working_dir"

gzip -cd template.dmg.gz > "$WORK_DMG"
mkdir -p "$WORK_DIR"
hdiutil attach "$WORK_DMG" -noautoopen -quiet -mountpoint "$WORK_DIR"
for i in "$SOURCE_FILES"; do
	rm -rf "$WORK_DIR/$i"
	ditto -rsrc "$SOURCE_DIR/$i" "$WORK_DIR/$i"
done
WC_DEV=`hdiutil info | grep "$WORK_DIR" | awk '{print $1}'` && hdiutil detach $WC_DEV -quiet -force
hdiutil convert "$WORK_DMG" -quiet -format UDZO -imagekey zlib-level=9 -o "$OUTPUT_DMG"
rm -rf "$WORK_DIR"
rm -f "$WORK_DMG"

# for later, if we need to resize, etc
#hdiutil resize -size 200mb -growonly -imageonly working.dmg

echo Done.