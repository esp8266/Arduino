#!/bin/bash

set -ev

root=$(git rev-parse --show-toplevel)

fail=0
for i in $(cat "$root/package/package_esp8266com_index.template.json" | jq '.packages[0]."tools" | .[] | .systems[] | "\(.url) \(.checksum)"' | sort -u | sed 's/ /@/'); do
    url=$(echo $i | sed 's/@/ /' | cut -f2 -d\" | cut -f1 -d' ')
    sha=$(echo $i | sed 's/@/ /' | cut -f2 -d\" | cut -f2 -d' ' | cut -f2 -d:)
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
