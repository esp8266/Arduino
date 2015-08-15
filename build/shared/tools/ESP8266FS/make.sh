#!/bin/sh

if [[ -z "$INSTALLDIR" ]]; then
    INSTALLDIR="$HOME/Documents/Arduino"
fi
echo "INSTALLDIR: $INSTALLDIR"

pde_path=`find ../../.. -name pde.jar`
core_path=`find ../../.. -name arduino-core.jar`
if [[ -z "$core_path" || -z "$pde_path" ]]; then
    echo "Some java libraries have not been built yet (did you run ant build?)"
    return 1
fi

set -e

mkdir -p bin
javac -target 1.8 -cp "$pde_path:$core_path" \
      -d bin src/ESP8266FS.java

pushd bin
mkdir -p $INSTALLDIR/tools
rm -rf $INSTALLDIR/tools/ESP8266FS
mkdir -p $INSTALLDIR/tools/ESP8266FS/tool
zip -r $INSTALLDIR/tools/ESP8266FS/tool/esp8266fs.jar *
popd

#pushd $INSTALLDIR/tools
#zip -r ESP8266FS-$(git describe --tags).zip ESP8266FS/
#popd
