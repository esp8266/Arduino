#!/bin/bash -ex

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR

rm -f ../arduino-*.tar.xz
rm -f ../arduino-*.zip

ant -Djava.net.preferIPv4Stack=true -Dplatform=linux32 $@ clean dist
mv linux/arduino-*-linux32.tar.xz ../

ant -Djava.net.preferIPv4Stack=true -Dplatform=linux64 $@ clean dist
mv linux/arduino-*-linux64.tar.xz ../

ant -Djava.net.preferIPv4Stack=true -Dplatform=windows $@ clean dist
mv windows/arduino-*-windows.zip ../

ant -Djava.net.preferIPv4Stack=true -Dplatform=macosx $@ clean dist
mv macosx/arduino-*-macosx.zip ../

