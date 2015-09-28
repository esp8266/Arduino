#!/bin/bash
#

ver=`git describe --tags`
package_name=esp8266-$ver
echo "Version: $ver"
echo "Package name: $package_name"
outdir=versions/$ver/$package_name
srcdir=$PWD/../hardware/esp8266com/esp8266/

rm -rf versions/$ver
mkdir -p $outdir

cp -R $srcdir/* $outdir/

cp -R ../libraries/SD $outdir/libraries/
cp -R ../libraries/Adafruit_ILI9341 $outdir/libraries/
cp -R ../libraries/OneWire $outdir/libraries/

wget -O SoftwareSerial.zip https://github.com/plerup/espsoftwareserial/archive/38259afcab9c291dbb6216f827d9a3738abf1868.zip
unzip SoftwareSerial.zip
rm -rf SoftwareSerial.zip
mv espsoftwareserial-* SoftwareSerial
mv SoftwareSerial $outdir/libraries

cat $srcdir/platform.txt | \
gsed 's/runtime.tools.xtensa-lx106-elf-gcc.path={runtime.platform.path}\/tools\/xtensa-lx106-elf//g' | \
gsed 's/runtime.tools.esptool.path={runtime.platform.path}\/tools//g' | \
gsed 's/tools.esptool.path={runtime.platform.path}\/tools/tools.esptool.path=\{runtime.tools.esptool.path\}/g' \
 > $outdir/platform.txt

pushd versions/$ver
echo "Making $package_name.zip"
zip -qr $package_name.zip $package_name
rm -rf $package_name
sha=`shasum -a 256 $package_name.zip | cut -f 1 -d ' '`
size=`/bin/ls -l $package_name.zip | awk '{print $5}'`
echo Size: $size
echo SHA-256: $sha

if [ -z "$REMOTE_URL" ]; then
    REMOTE_URL="http://localhost:8000"
    echo "REMOTE_URL not defined, using default: $REMOTE_URL"
fi

echo "Making package_esp8266com_index.json"
cat << EOF > package_esp8266com_index.json
{
  "packages": [ {
    "name":"esp8266",
    "maintainer":"ESP8266 Community",
    "websiteURL":"https://github.com/esp8266/Arduino",
    "email":"ivan@esp8266.com",
    "help":{
      "online":"$REMOTE_URL/versions/$ver/doc/reference.html"
    },

    "platforms": [ {
      "name":"esp8266",
      "architecture":"esp8266",
      "version":"$ver",
      "category":"ESP8266",
      "url":"$REMOTE_URL/versions/$ver/$package_name.zip",
      "archiveFileName":"$package_name.zip",
      "checksum":"SHA-256:$sha",
      "size":"$size",
      "help":{
        "online":"$REMOTE_URL/versions/$ver/doc/reference.html"
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
          "name":"SparkFun Thing"
        },
        {
          "name":"SweetPea ESP-210"
        }
      ],
      "toolsDependencies":[ {
        "packager":"esp8266",
        "name":"esptool",
        "version":"0.4.6"
      },
      {
        "packager":"esp8266",
        "name":"xtensa-lx106-elf-gcc",
        "version":"1.20.0-26-gb404fb9-2"
      },
      {
        "packager":"esp8266",
        "name":"mkspiffs",
        "version":"0.1.1"
      } ]
    } ],

    "tools": [ {
      "name":"esptool",
      "version":"0.4.6",
      "systems": [
        {
            "host":"i686-mingw32",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.6/esptool-0.4.6-win32.zip",
            "archiveFileName":"esptool-0.4.6-win32.zip",
            "checksum":"SHA-256:0248bf78514a3195f583e29218ca7828a66e13c6e5545a078f1c1257033e4927",
            "size":"17481"
        },
        {
            "host":"x86_64-apple-darwin",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.6/esptool-0.4.6-osx.tar.gz",
            "archiveFileName":"esptool-0.4.6-osx.tar.gz",
            "checksum":"SHA-256:0fe87ba7e29ee90a9fc72492aada8c0796f9e8f8a1c594b6b26cee2610d09bb3",
            "size":"20926"
        },
        {
            "host":"i386-apple-darwin",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.6/esptool-0.4.6-osx.tar.gz",
            "archiveFileName":"esptool-0.4.6-osx.tar.gz",
            "checksum":"SHA-256:0fe87ba7e29ee90a9fc72492aada8c0796f9e8f8a1c594b6b26cee2610d09bb3",
            "size":"20926"
        },
        {
            "host":"x86_64-pc-linux-gnu",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.6/esptool-0.4.6-linux64.tar.gz",
            "archiveFileName":"esptool-0.4.6-linux64.tar.gz",
            "checksum":"SHA-256:f9f456e9a42bb2597126c513cb8865f923fb978865d4838b9623d322216b74d0",
            "size":"12885"
        },
        {
            "host":"i686-pc-linux-gnu",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.6/esptool-0.4.6-linux32.tar.gz",
            "archiveFileName":"esptool-0.4.6-linux32.tar.gz",
            "checksum":"SHA-256:85275ca03a82bfc456f5a84e86962ca1e470ea2e168829c38ca29ee668831d93",
            "size":"13417"
        }
      ]
    },
    {
      "name":"xtensa-lx106-elf-gcc",
      "version":"1.20.0-26-gb404fb9-2",
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
           "host":"i386-apple-darwin",
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
    },
    {
      "name":"mkspiffs",
      "version":"0.1.1",
      "systems": [
        {
            "host":"i686-mingw32",
            "url":"https://github.com/igrr/mkspiffs/releases/download/0.1.1/mkspiffs-0.1.1-windows.zip",
            "archiveFileName":"mkspiffs-0.1.1-windows.zip",
            "checksum":"SHA-256:421532b415f2227c15ea8b7c633b50a15bdcb40b61d1d223b155b9b5cff886a7",
            "size":"228989"
        },
        {
            "host":"x86_64-apple-darwin",
            "url":"https://github.com/igrr/mkspiffs/releases/download/0.1.1/mkspiffs-0.1.1-osx.tar.gz",
            "archiveFileName":"mkspiffs-0.1.1-osx.tar.gz",
            "checksum":"SHA-256:673cbc3027ca461f6b928bb910953b4e78cec2d4b638c872410461296c54aeaf",
            "size":"123544"
        },
        {
            "host":"i386-apple-darwin",
            "url":"https://github.com/igrr/mkspiffs/releases/download/0.1.1/mkspiffs-0.1.1-osx.tar.gz",
            "archiveFileName":"mkspiffs-0.1.1-osx.tar.gz",
            "checksum":"SHA-256:673cbc3027ca461f6b928bb910953b4e78cec2d4b638c872410461296c54aeaf",
            "size":"123544"
        },
        {
            "host":"x86_64-pc-linux-gnu",
            "url":"https://github.com/igrr/mkspiffs/releases/download/0.1.1/mkspiffs-0.1.1-linux64.tar.gz",
            "archiveFileName":"mkspiffs-0.1.1-linux64.tar.gz",
            "checksum":"SHA-256:afea5786cf60c8cfdae5d2665323ce281c05c0516590ba2bf6282e396e3fa76a",
            "size":"44373"
        },
        {
            "host":"i686-pc-linux-gnu",
            "url":"https://github.com/igrr/mkspiffs/releases/download/0.1.1/mkspiffs-0.1.1-linux32.tar.gz",
            "archiveFileName":"mkspiffs-0.1.1-linux32.tar.gz",
            "checksum":"SHA-256:46a646a9e9070912234c9a7d5618d9787047b07b44a4aef5e05f16f9100d37ce",
            "size":"43304"
        }
      ]
    } ]
  } ]
}
EOF

popd
