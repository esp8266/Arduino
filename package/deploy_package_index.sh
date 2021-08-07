#!/bin/bash
# This script updates package index hosted on esp8266.github.io (aka arduino.esp8266.com).

tag=$(jq -r '.release.tag_name' "$GITHUB_EVENT_PATH")
if [ "$tag" == "" ]; then
    tag=`git describe --tags`
fi

cd $(dirname "$0")

set -e # Abort with error if anything here does not go as expected!

# Install SSH private key from a GH Secret
echo $GHCI_DEPLOY_KEY | base64 -d > esp8266_github_io_deploy
eval "$(ssh-agent -s)"
chmod 600 esp8266_github_io_deploy
ssh-add esp8266_github_io_deploy
mkdir -p ~/.ssh
chmod go-w ~/.ssh
echo -e "Host github.com\nStrictHostKeyChecking no\n" >> ~/.ssh/config
chmod go-w  ~/.ssh/config

# Clone the Github pages repository
git clone git@github.com:esp8266/esp8266.github.io.git
pushd esp8266.github.io

# Copy from published release, ensure JSON valid
rm -f stable/package_esp8266com_index.json
wget https://github.com/esp8266/Arduino/releases/download/$tag/package_esp8266com_index.json -O stable/package_esp8266com_index.json
cat stable/package_esp8266com_index.json | jq empty

git add stable/package_esp8266com_index.json

# Commit and push the changes
git config user.email "github-ci-action@github.com"
git config user.name "GitHub CI Action"
git commit -m "Update package index for release $tag"
git push origin master
popd
