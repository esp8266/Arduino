#!/bin/bash
#
# This script updates package index hosted on arduino.esp8266.com.
# Normally is run by Travis CI for tagged versions, as a deploy step.

tag=`git describe --tags`

cd $(dirname "$0")

# Decrypt and install SSH private key.
# "encrypted_xxx_key" and "encrypted_xxx_iv" are environment variables
# known to Travis CI builds.
openssl aes-256-cbc -K $encrypted_3f14690ceb9b_key -iv $encrypted_3f14690ceb9b_iv -in arduino-esp8266-travis.enc -out arduino-esp8266-travis -d
eval "$(ssh-agent -s)"
chmod 600 arduino-esp8266-travis
ssh-add arduino-esp8266-travis

# Set SSH server public key
echo "arduino.esp8266.com,104.131.82.128 ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBAvoxsdf1jJ1XX7RrCtAQyjvZ3b33bWYfB/XDvEMLtxnJhZr+P/wa7yuZ+UJJ1wuJc+wcIMBNZ2Zz/MbdRMey7A=" \
    >> $HOME/.ssh/known_hosts

branch=stable
ssh_dl_server=nodeuser@arduino.esp8266.com
base_dir=apps/download_files/download

# Upload package_esp8266com_index.json
ssh $ssh_dl_server "mkdir -p $base_dir/versions/$tag"
scp versions/$tag/package_esp8266com_index.json $ssh_dl_server:$base_dir/versions/$tag/

# Change symlink for stable version
oldver=$(ssh $ssh_dl_server "readlink $base_dir/$branch")
newver="versions/$tag"
echo "Changing version of $branch from $oldver to $newver"

ssh $ssh_dl_server "pushd apps/download_files/download && ln -snf versions/$tag $branch"
