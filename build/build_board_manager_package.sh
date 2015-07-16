#!/bin/bash
#
# for platform in windows linux macosx; do pushd $platform; ls -l esptool-*; shasum -a 256 esptool-*; popd; done;
#
#

ver=`git describe --tags`
outdir=esp8266-$ver
srcdir=../hardware/esp8266com/esp8266/
mkdir -p $outdir
cp -R $srcdir/* $outdir/

cp -R ../libraries/SD $outdir/libraries/
cp -R ../libraries/Adafruit_ILI9341 $outdir/libraries/
cp -R ../libraries/OneWire $outdir/libraries/

cat $srcdir/platform.txt | \
gsed 's/runtime.tools.xtensa-lx106-elf-gcc.path={runtime.platform.path}\/tools\/xtensa-lx106-elf//g' | \
gsed 's/runtime.tools.esptool.path={runtime.platform.path}\/tools//g' | \
gsed 's/tools.esptool.path={runtime.platform.path}\/tools/tools.esptool.path=\{runtime.tools.esptool.path\}/g' \
 > $outdir/platform.txt

zip -r $outdir.zip $outdir
rm -rf $outdir
sha=`shasum -a 256 $outdir.zip | cut -f 1 -d ' '`
size=`/bin/ls -l $outdir.zip | awk '{print $5}'`
echo Size: $size
echo SHA-256: $sha

if [ "$upload" == "stable" ]; then
    badge_title="stable"
    badge_color="blue"
    path=""
elif [ "$upload" == "staging" ]; then
    badge_title="staging"
    badge_color="yellow"
    path="staging/"
elif [ "$upload" == "test" ]; then
    badge_title="test"
    badge_color="red"
    path="test/"
else
    upload=""
    remote="http://localhost:8000"
fi

if [ ! -z "$upload" ]; then
  remote="http://arduino.esp8266.com"
  release_date=$(date "+%b_%d,_%Y")
  wget -O badge.svg https://img.shields.io/badge/$badge_title-$release_date-$badge_color.svg
fi

cat << EOF > package_esp8266com_index.json
{
  "packages": [ {
    "name":"esp8266",
    "maintainer":"ESP8266 Community",
    "websiteURL":"https://github.com/esp8266/Arduino",
    "email":"ivan@esp8266.com",
    "help":{
      "online":"http://esp8266.com"
    },

    "platforms": [ {
      "name":"esp8266",
      "architecture":"esp8266",
      "version":"$ver",
      "category":"ESP8266",
      "url":"$remote/$path/$outdir.zip",
      "archiveFileName":"$outdir.zip",
      "checksum":"SHA-256:$sha",
      "size":"$size",
      "help":{
        "online":"http://esp8266.com"
      },
      "boards":[
        {
          "name":"Generic ESP8266 Module"
        },
        {
          "name":"Olimex MOD-WIFI-ESP8266(-DEV)"
        },
        {
          "name":"NodeMCU 0.9 (ESP-12 Module)"
        },
        {
          "name":"NodeMCU 1.0 (ESP-12E Module)"
        },
        {
          "name":"Adafruit HUZZAH ESP8266 (ESP-12)"
        },
        {
          "name":"SweetPea ESP-210"
        }
      ],
      "toolsDependencies":[ {
        "packager":"esp8266",
        "name":"esptool",
        "version":"0.4.5"
      },
      {
        "packager":"esp8266",
        "name":"xtensa-lx106-elf-gcc",
        "version":"1.20.0-26-gb404fb9"
      } ]
    } ],

    "tools": [ {
      "name":"esptool",
      "version":"0.4.5",
      "systems": [
        {
            "host":"i686-mingw32",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.5/esptool-0.4.5-win32.zip",
            "archiveFileName":"esptool-0.4.5-win32.zip",
            "checksum":"SHA-256:1b0a7d254e74942d820a09281aa5dc2af1c8314ae5ee1a5abb0653d0580e531b",
            "size":"17408"
        },
        {
            "host":"x86_64-apple-darwin",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.5/esptool-0.4.5-osx.tar.gz",
            "archiveFileName":"esptool-0.4.5-osx.tar.gz",
            "checksum":"SHA-256:924d31c64f4bb9f748e70806dafbabb15e5eb80afcdde33715f3ec884be1652d",
            "size":"11359"
        },
        {
            "host":"x86_64-pc-linux-gnu",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.5/esptool-0.4.5-linux64.tar.gz",
            "archiveFileName":"esptool-0.4.5-linux64.tar.gz",
            "checksum":"SHA-256:4ce799e13fbd89f8a8f08a08db77dc3b1362c4486306fe1b3801dee80cfa3203",
            "size":"12789"
        },
        {
            "host":"i686-pc-linux-gnu",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.5/esptool-0.4.5-linux32.tar.gz",
            "archiveFileName":"esptool-0.4.5-linux32.tar.gz",
            "checksum":"SHA-256:4aa81b97a470641771cf371e5d470ac92d3b177adbe8263c4aae66e607b67755",
            "size":"12044"
        }
      ]
    },
    {
      "name":"xtensa-lx106-elf-gcc",
      "version":"1.20.0-26-gb404fb9",
      "systems": [
        {
           "host":"i686-mingw32",
           "url":"http://arduino.esp8266.com/win32-xtensa-lx106-elf-gb404fb9-2.tar.gz",
           "archiveFileName":"win32-xtensa-lx106-elf-gb404fb9-2.tar.gz",
           "checksum":"SHA-256:10476b9c11a7a90f40883413ddfb409f505b20692e316c4e597c4c175b4be09c",
           "size":"153527527"
        },
        {
           "host":"x86_64-apple-darwin",
           "url":"http://arduino.esp8266.com/osx-xtensa-lx106-elf-gb404fb9-2.tar.gz",
           "archiveFileName":"osx-xtensa-lx106-elf-gb404fb9-2.tar.gz",
           "checksum":"SHA-256:0cf150193997bd1355e0f49d3d49711730035257bc1aee1eaaad619e56b9e4e6",
           "size":"35385382"
        },
        {
           "host":"x86_64-pc-linux-gnu",
           "url":"http://arduino.esp8266.com/linux64-xtensa-lx106-elf-gb404fb9.tar.gz",
           "archiveFileName":"linux64-xtensa-lx106-elf-gb404fb9.tar.gz",
           "checksum":"SHA-256:46f057fbd8b320889a26167daf325038912096d09940b2a95489db92431473b7",
           "size":"30262903"
        },
        {
           "host":"i686-pc-linux-gnu",
           "url":"http://arduino.esp8266.com/linux32-xtensa-lx106-elf.tar.gz",
           "archiveFileName":"linux32-xtensa-lx106-elf.tar.gz",
           "checksum":"SHA-256:b24817819f0078fb05895a640e806e0aca9aa96b47b80d2390ac8e2d9ddc955a",
           "size":"32734156"
        }
      ]
    } ]
  } ]
}
EOF


if [ ! -z "$upload" ]; then
    remote_path=dl:apps/download_files/download/$path
    scp $outdir.zip $remote_path
    scp package_esp8266com_index.json $remote_path
    scp -r $srcdir/doc $remote_path
    scp badge.svg $remote_path
else
    python -m SimpleHTTPServer
fi
