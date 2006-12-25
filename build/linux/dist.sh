#!/bin/sh

REVISION=`head -c 4 ../../todo.txt`

rm -rf work

./make.sh

echo Creating linux distribution for revision $REVISION...

rm -rf work/classes

# remove any old boogers
rm -rf arduino
rm -rf arduino-*

mv work arduino

# remove boogers
find arduino -name "*~" -exec rm -f {} ';'
find arduino -name ".DS_Store" -exec rm -f {} ';'
find arduino -name "._*" -exec rm -f {} ';'
find arduino -name "Thumbs.db" -exec rm -f {} ';'

# clean out the cvs entries
find arduino -name ".svn" -exec rm -rf {} ';' 2> /dev/null

# zip it all up for release
echo Creating tarball and finishing...
P5=arduino-$REVISION
mv arduino $P5

tar cfz $P5.tgz $P5
# nah, keep the new directory around
#rm -rf $P5

#echo Done.
