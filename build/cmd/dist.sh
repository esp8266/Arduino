#!/bin/sh

# only needed for core.jar and pde.jar, hmm
ARCH=`uname`
if [ $ARCH == "Darwin" ]
then
    BUILD=../macosx
    REVISION=`head -1 ../../todo.txt | cut -c 1-4`
elif [ $ARCH == "Cygwin" ]
then
    BUILD=../windows
    REVISION=`head -c 4 ../../todo.txt`
else 
    BUILD=../linux
    REVISION=`head -c 4 ../../todo.txt`
fi

echo Creating command-line distribution for revision $REVISION...

# remove any old boogers
rm -rf processing
rm -rf processing-*

mkdir processing
cp -r ../shared/lib processing/
cp -r ../shared/libraries processing/
cp ../../app/lib/antlr.jar processing/lib/
cp ../../app/lib/ecj.jar processing/lib/
cp ../../app/lib/jna.jar processing/lib/
cp ../shared/revisions.txt processing/

# add the libraries folder with source
cp -r ../../net processing/libraries/
cp -r ../../opengl processing/libraries/
cp -r ../../serial processing/libraries/
cp -r ../../pdf processing/libraries/
cp -r ../../dxf processing/libraries/
cp -r ../../xml processing/libraries/
cp -r ../../candy processing/libraries/
cp -r ../../video processing/libraries/

# grab pde.jar and export from the working dir
cp $BUILD/work/lib/pde.jar processing/lib/
cp $BUILD/work/lib/core.jar processing/lib/

# get platform-specific goodies from the dist dir
install -m 755 dist/processing processing/processing

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
P5=processing-cmd-$REVISION
mv processing $P5

zip -rq $P5.zip $P5
#tar cfz $P5.tgz $P5
# nah, keep the new directory around
#rm -rf $P5

#echo Done.
