#!/bin/bash

#
# @file esp8266-arudino-doc.bash
# @author Ivan Grokhotkov (https://github.com/igrr)
# @author Pascal Gollor (https://github.com/pgollor)
#
#
# This script build the documentation for a specific Arduino ESP8266 release version.
#
# Packages needed by this script:
# * linux commands: ln, cp, mkdir, rm, wget
# * git
# * jekyll
#
# ruby libraries:
# * redcarpet
# * rb-pygments
#
# gem install [lib]
#


# some variable definitions
arduinoESP_src=$PWD"/../"
version="$(git --work-tree=$arduinoESP_src describe --tags --always)"
release_date=$(date "+%b_%d,_%Y") # format for badge link
build_date=$(date "+%b %d, %Y")
destinaton_path="/tmp/doc"
doc_template_url="https://github.com/pgollor/esp8266-arduino-docs.git"
url="http://pgollor.github.io/Arduino"

# control output
echo "Arduino ESP8266 source dir: "$arduinoESP_src
echo "                   version: "$version
echo "              release date: "$release_date
echo "                build date: "$build_date
echo "    put documentation into: "$destinaton_path
echo "documentatino template url: "$doc_template_url
echo "                       url: "$url

# continue?
read -e -p "Dou you wish to continue (y/n)? " -n 1 -i "y" decision
if echo "$decision" | grep -iq "^y" ;then
	echo "okay"
else
	echo "bye bye"
	exit
fi


# delete old doc dir
rm -fR $destinaton_path

# create destination directories
mkdir -p $destinaton_path/src
mkdir -p $destinaton_path/$version

# copy doc files to destination soruce dir
cp -R $arduinoESP_src/doc/* $destinaton_path/src

# download doc template
git clone $doc_template_url $destinaton_path/build


#cur_dir=$PWD
#cd $destinaton_path/build
pushd $destinaton_path/build

# link documentation source
ln -s ../src doc

# link documentation destination
ln -s ../$version _site

# add subtitle and basurl
echo "url: \"$url\"" > _config_local.yml
echo "subtitle: \"ver. $version, built on $build_date\"" >> _config_local.yml
echo "baseurl: /Arduino/versions/$version" >> _config_local.yml

# build with jekyll
jekyll build --config _config.yml,_config_local.yml

popd
#cd $cur_dir


# grab badge
wget -q -O $destinaton_path/$version/badge.svg "https://img.shields.io/badge/updated-$release_date-blue.svg"

