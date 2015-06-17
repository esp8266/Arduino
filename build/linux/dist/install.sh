#!/bin/sh

CURDIR=`pwd`
APPDIR="$(dirname -- "$(readlink -f -- "${0}")" )"

cd "$APPDIR"

# Set arduino.desktop absolute path workaround
mv arduino.desktop arduino.desktop-bak
sed -e "s,FULL_PATH,$PWD,g" arduino.desktop-bak > arduino.desktop
rm arduino.desktop-bak

cp arduino.desktop ~/.local/share/applications/arduino.desktop
cp arduino.desktop ~/Desktop/arduino.desktop

echo "Installed Arduino IDE icons on menu and desktop !"

