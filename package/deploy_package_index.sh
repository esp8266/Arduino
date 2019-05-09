#!/bin/bash
#
# This script updates package index hosted on esp8266.github.io (aka arduino.esp8266.com).
# Normally is run by Travis CI for tagged versions, as a deploy step.

tag=`git describe --tags`

cd $(dirname "$0")

# Decrypt and install SSH private key.
# "encrypted_xxx_key" and "encrypted_xxx_iv" are environment variables
# known to Travis CI builds.
openssl aes-256-cbc -K $encrypted_3a94a4db7dec_key -iv $encrypted_3a94a4db7dec_iv -in esp8266_github_io_deploy.enc -out esp8266_github_io_deploy -d
eval "$(ssh-agent -s)"
chmod 600 esp8266_github_io_deploy
ssh-add esp8266_github_io_deploy
echo -e "Host github.com\nStrictHostKeyChecking no\n" >> ~/.ssh/config

# Clone the Github pages repository
git clone git@github.com:esp8266/esp8266.github.io.git
pushd esp8266.github.io

# Update the package index
cp ../versions/$tag/package_esp8266com_index.json stable/package_esp8266com_index.json
git add stable/package_esp8266com_index.json

# Commit and push the changes
git config user.email "travis@travis-ci.org"
git config user.name "Travis CI"
git commit -m "update package index for release $tag"
git push origin master
popd
