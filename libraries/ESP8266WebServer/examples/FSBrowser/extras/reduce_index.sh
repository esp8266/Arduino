#/bin/sh

# Requires xdd which is part of the VIM package
# Requires npm
#   sudo apt install npm
#   ln -s /usr/bin/nodejs /usr/bin/node
# Requires html-minifier
#   sudo npm install html-minifier -g

html-minifier \
 --case-sensitive \
 --collapse-boolean-attributes \
 --collapse-whitespace \
 --minify-css true \
 --minify-js true \
 --process-conditional-comments \
 --remove-attribute-quotes \
 --remove-comments \
 --remove-empty-attributes \
 --remove-optional-tags \
 --remove-redundant-attributes \
 --remove-script-type-attributes \
 --remove-style-link-type-attributes \
 -o index.htm \
 ../data/edit/index.htm

if [ $? -ne 0 ]
then
  echo "Error minifying index.htm"
  exit -1
fi

if [ -e index.htm.gz ]
then
  rm index.htm.gz
fi

gzip index.htm
if [ $? -ne 0 ]
then
  echo "Error gzipping minified index.htm"
  exit -1
fi

xxd index.htm.gz > index_htm.h
if [ $? -ne 0 ]
then
  echo "Error creating include file for index.htm"
  exit -1
fi

echo Reduce complete.
echo index.htm.gz can be used instead of data/edit/index.htm to save FileSystem space
echo Alternately, you can leave the filesystem untouched by embedding index.htm
echo inside the code. To do so, uncomment the line 
echo "  #define INCLUDE_FALLBACK_INDEX_HTM" 
echo in FSBrowser.ino
