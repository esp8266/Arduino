#!/bin/bash

set -e
export PKG_URL_PREFIX=https://dl.bintray.com/igrr/arduino-esp8266/:
commit=`git rev-parse --short HEAD`

./build_boards_manager_package.sh

ver=`ls -1 versions`
bintray_slug=igrr/arduino-esp8266/arduino-esp8266-core

# Upload to bintray
# URL to the file will look like this: https://dl.bintray.com/igrr/arduino-esp8266/:esp8266-2.4.0-nightly+20170218.zip
curl --progress-bar \
	-T versions/$ver/esp8266-$ver.zip \
	-uigrr:$BINTRAY_API_KEY \
	-o curl.out \
	https://api.bintray.com/content/$bintray_slug/$ver/esp8266-$ver.zip


# Publish the uploaded file
curl -uigrr:$BINTRAY_API_KEY \
	-X POST \
	https://api.bintray.com/content/$bintray_slug/$ver/publish

# Load deploy key
echo -n $ESP8266_ARDUINO_DEPLOY_KEY_B64 > ~/.ssh/esp8266_arduino_deploy_b64
base64 --decode --ignore-garbage ~/.ssh/esp8266_arduino_deploy_b64 > ~/.ssh/esp8266_arduino_deploy
chmod 600 ~/.ssh/esp8266_arduino_deploy
echo -e "Host $DEPLOY_HOST_NAME\n\tUser $DEPLOY_USER_NAME\n\tStrictHostKeyChecking no\n\tIdentityFile ~/.ssh/esp8266_arduino_deploy" >> ~/.ssh/config

# Generate the badge (used in Readme.md)
release_date=$(date "+%b_%d,_%Y")
curl -o versions/$ver/badge.svg https://img.shields.io/badge/updated-$release_date-blue.svg

# Check old version
oldver=$(ssh $DEPLOY_HOST_NAME "cat $DEPLOY_PATH_NIGHTLY/version")

if [ "$oldver" = "$ver" ]; then
	echo "Nightly version hasn't changed, not updating"
else
	# Upload new json, version file, and the badge
	echo $ver > versions/$ver/version
	scp versions/$ver/{package_esp8266com_index.json,version,badge.svg} $DEPLOY_HOST_NAME:$DEPLOY_PATH_NIGHTLY/
	curl --data-urlencode "message=Updating **nightly** package from $oldver to $ver" $GITTER_WEBHOOK
fi
