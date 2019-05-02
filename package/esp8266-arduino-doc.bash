#!/usr/bin/env bash

#
# @file esp8266-arduino-doc.bash
# @author Ivan Grokhotkov (https://github.com/igrr)
# @author Pascal Gollor (https://github.com/pgollor)
#
#
# This script build the documentation for a specific Arduino ESP8266 release version.
#
# Packages needed by this script:
# * linux commands: ln, cp, mkdir, rm, wget
# * git
#
# ruby gems:
# * jekyll
# * redcarpet
# * rb-pygments
#
# gem install [lib]
#

set -e

# some variable definitions
tmp_path=$1
doc_src_path=$2
arduinoESP_src=$(cd $PWD/..; pwd)
version="$(git --work-tree=$arduinoESP_src --git-dir=$arduinoESP_src/.git describe --tags --always)"
release_date=$(date "+%b_%d,_%Y") # format for badge link
build_date=$(date "+%b %d, %Y")
destination_path="$tmp_path/doc"
doc_template_url="https://github.com/igrr/esp8266-arduino-docs.git"
url="https://esp8266.github.io/Arduino"

# control output
echo "Arduino ESP8266 source dir: "$arduinoESP_src
echo "                   version: "$version
echo "              release date: "$release_date
echo "                build date: "$build_date
echo "    put documentation into: "$destination_path
echo "documentation template url: "$doc_template_url
echo "                       url: "$url

# continue?
read -e -p "Dou you wish to continue (y/n)? " -n 1 decision
if echo "$decision" | grep -iq "^y" ;then
	echo "okay"
else
	echo "bye bye"
	exit
fi


# delete old doc dir
rm -fR $destination_path

# create destination directories
mkdir -p $destination_path/src
mkdir -p $destination_path/$version

# copy doc files to destination soruce dir
cp -R $arduinoESP_src/doc/* $destination_path/src

# download doc template
rsync -av $doc_src_path/ $destination_path/build/
# git clone $doc_template_url $destination_path/build

# create versions.html file

# ... read verions
pushd $arduinoESP_src
old_versions=$(git ls-tree -d --name-only remotes/origin/gh-pages versions/ | sed -e 's/versions\///g')
popd

echo -e "\nREAD old versions:"

found_current_version="false"
case "${old_versions[@]}" in *"$version"*) found_current_version="true" ;; esac

if [ "$found_current_version" = "false" ]; then
	old_versions=$version" "$old_versions
fi

# ... fill versions.html
for VER in $old_versions
do
	echo $VER
	echo "<li><a href=\"versions/$VER\">$VER</a></li>" >> $destination_path/build/_includes/versions.html
done
echo ""


# into build dir
pushd $destination_path/build

# link documentation source
ln -s ../src doc

# link documentation destination
ln -s ../$version _site

# add subtitle and basurl
echo "url: \"$url\"" > _config_local.yml
echo "version: $version"  >> _config_local.yml
echo "build_date: $build_date" >> _config_local.yml
echo "baseurl: /Arduino/versions/$version" >> _config_local.yml
mv doc/reference_items.yml _data/reference_items.yml

# build with jekyll
jekyll build --config _config.yml,_config_local.yml

popd


# grab badge
wget -q -O $destination_path/$version/badge.svg "https://img.shields.io/badge/updated-$release_date-blue.svg"
