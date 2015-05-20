#!/bin/sh
ALIBDIR="/Users/ficeto/Documents/Arduino"
mkdir -p bin && \
javac -target 1.8 -cp "../../arduino-core.jar:../../pde.jar" -d bin src/ESP8266FS.java && \
cd bin && \
mkdir -p $ALIBDIR/tools && \
rm -rf $ALIBDIR/tools/ESP8266FS && \
mkdir -p $ALIBDIR/tools/ESP8266FS/tool && \
zip -r $ALIBDIR/tools/ESP8266FS/tool/esp8266fs.jar * && \
cd ..
