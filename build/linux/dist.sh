#!/bin/sh

REVISION=`head -c 4 ../../todo.txt`

./make.sh

echo Creating linux distribution for revision $REVISION...

# remove any old boogers
rm -rf processing
rm -rf processing-*

# use 'shared' files as starting point
cp -r ../shared processing

# add the libraries folder with source
cp -r ../../net processing/libraries/
cp -r ../../opengl processing/libraries/
cp -r ../../serial processing/libraries/

# new style examples thing ala reas
cd processing
unzip -q examples.zip
rm examples.zip
cd ..

cd processing
unzip -q reference.zip
rm reference.zip
cd ..

# add java (jre) files
#tar --extract --verbose --file=jre.tgz --ungzip --directory=processing
tar --extract --file=jre.tgz --ungzip --directory=processing

# directories used by the app
#mkdir processing/lib/build

# grab pde.jar and export from the working dir
cp work/lib/pde.jar processing/lib/
cp work/lib/core.jar processing/lib/

# get platform-specific goodies from the dist dir
install -m 755 dist/processing processing/processing
cp dist/jikes processing/
chmod +x processing/jikes

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
#find processing -name "CVS" -exec echo {} ';'

# zip it all up for release
echo Creating tarball and finishing...
P5=processing-$REVISION
mv processing $P5

tar cfz $P5.tgz $P5
# nah, keep the new directory around
#rm -rf $P5

#echo Done.
