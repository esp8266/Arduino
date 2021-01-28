#!/bin/bash

set -ev

fail=0
for i in $(cat "$TRAVIS_BUILD_DIR/package/package_esp8266com_index.template.json" | jq '.packages[0]."tools" | .[] | .systems[] | "\(.url) \(.checksum)"' | sort -u); do
    url=$(echo $i | cut -f2 -d\" | cut -f1 -d' ')
    sha=$(echo $i | cut -f2 -d\" | cut -f2 -d' ' | cut -f2 -d:)
    echo "INFO:  Checking $url"
    rm -f file.bin
    wget --quiet -O file.bin $url
    calc=$(sha256sum file.bin | cut -f1 -d" ")
    if [ "$sha" != "$calc" ]; then
        echo "ERROR:  Download failed or SHA mismatch for $url"
        echo "ERROR:  Expected $sha"
        echo "ERROR:  Received $calc"
        fail=1
    fi
done

if [ $fail -ne 0 ]; then
    echo ERROR:  Package file integrity check failed
    exit 1
fi
