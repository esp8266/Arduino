#/bin/sh

# Processing script to optionally reduce filesystem use by miniying, gzipping and preparing index.htm for embedding in code.
# Please see readme.md for more information.

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

xxd -i index.htm.gz > index_htm.h
if [ $? -ne 0 ]
then
  echo "Error creating include file for index.htm"
  exit -1
fi

mv index.htm.gz ../data/edit
if [ $? -ne 0 ]
then
  echo "Error moving gzipped version back to data/edit"
  exit -1
fi

echo Reduce complete.

