#!/bin/bash

ver=`git describe --tags`
outdir=esp8266-$ver
srcdir=../hardware/esp8266com/esp8266/
mkdir -p $outdir
cp -R $srcdir/* $outdir/
cat $srcdir/platform.txt | \
gsed 's/runtime.tools.xtensa-lx106-elf-gcc.path={runtime.platform.path}\/tools\/xtensa-lx106-elf//g' | \
gsed 's/runtime.tools.esptool.path={runtime.platform.path}\/tools//g' | \
gsed 's/tools.esptool.path={runtime.platform.path}\/tools/tools.esptool.path=\{runtime.tools.esptool.path\}/g' \
 > $outdir/platform.txt

zip -r $outdir.zip $outdir
sha=`shasum -a 256 $outdir.zip | cut -f 1 -d ' '`
size=`/bin/ls -l $outdir.zip | awk '{print $5}'`
echo Size: $size
echo SHA-256: $sha

scp $outdir.zip dl:apps/download_files/download/


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
      "url":"http://arduino.esp8266.com/$outdir.zip",
      "archiveFileName":"$outdir.zip",
      "checksum":"SHA-256:$sha",
      "size":"$size",
      "help":{
        "online":"http://esp8266.com"
      },
      "boards":[ {
          "name":"Generic ESP8266 Module"
        }
      ],
      "toolsDependencies":[ {
        "packager":"esp8266",
        "name":"esptool",
        "version":"0.4.4"
      },
      {
        "packager":"esp8266",
        "name":"xtensa-lx106-elf-gcc",
        "version":"1.20.0-26-gb404fb9"
      } ]
    } ],

    "tools": [ {
      "name":"esptool",
      "version":"0.4.4",
      "systems": [
        {
            "host":"i686-mingw32",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.4/esptool-0.4.4-win32.zip",
            "archiveFileName":"esptool-0.4.4-win32.zip",
            "checksum":"SHA-256:bc52165c847b194d8f079add982eae1c4b4466784bff8c8494241de602a003b3",
            "size":"17262"
        },
        {
            "host":"x86_64-apple-darwin",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.4/esptool-0.4.4-osx.tar.gz",
            "archiveFileName":"esptool-0.4.4-osx.tar.gz",
            "checksum":"SHA-256:bb2a58c3583f9dcb0d3e7611531a0f3a29c21a4a1c442987bb29d07408824dfe",
            "size":"12145"
        },
        {
            "host":"x86_64-pc-linux-gnu",
            "url":"https://github.com/igrr/esptool-ck/releases/download/0.4.4/esptool-0.4.4-linux64.tar.gz",
            "archiveFileName":"esptool-0.4.4-linux64.tar.gz",
            "checksum":"SHA-256:beedf89db0bdce0bf6034232d86edebcfed0966ff1501545aca9cfbba1f92593",
            "size":"12513"
        },
        {
        	"host":"i686-pc-linux-gnu",
        	"url":"https://github.com/igrr/esptool-ck/releases/download/0.4.4/esptool-0.4.4-linux32.tar.gz",
            "archiveFileName":"esptool-0.4.4-linux32.tar.gz",
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
           "url":"http://arduino.esp8266.com/win32-xtensa-lx106-elf-gb404fb9.tar.gz",
           "archiveFileName":"win32-xtensa-lx106-elf-gb404fb9.tar.gz",
           "checksum":"SHA-256:1561ec85cc58cab35cc48bfdb0d0087809f89c043112a2c36b54251a13bf781f",
           "size":"153807368"
        },
        {
           "host":"x86_64-apple-darwin",
           "url":"http://arduino.esp8266.com/osx-xtensa-lx106-elf-gb404fb9.tar.gz",
           "archiveFileName":"osx-xtensa-lx106-elf-gb404fb9.tar.gz",
           "checksum":"SHA-256:540be2ca32637584463a4a4d04717f6c79c7355e336b8c3578b0947218cf92b9",
           "size":"35189446"
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

scp package_esp8266com_index.json dl:apps/download_files/download

