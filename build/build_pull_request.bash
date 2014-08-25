#!/bin/bash -e

if [ "x${ghprbPullId}" == "x" ]
then
	exit 1
fi

ant -Djava.net.preferIPv4Stack=true -Dplatform=linux64 -Dlinux64=1 clean build test

ERRORS=`grep '<error' ../app/test-bin/TEST-*.xml | wc -l`
if [ $ERRORS -ne 0 ] ;
then
	exit $ERRORS
fi

VERSION="PR-${ghprbPullId}"

ant -Djava.net.preferIPv4Stack=true -Dplatform=linux32 -Dlinux32=1 -Dversion="${VERSION}" dist
ant -Djava.net.preferIPv4Stack=true -Dplatform=linux64 -Dlinux64=1 -Dversion="${VERSION}" dist
ant -Djava.net.preferIPv4Stack=true -Dplatform=windows -Dwindows=1 -Dversion="${VERSION}" dist
ant -Djava.net.preferIPv4Stack=true -Dplatform=macosx  -Dmacosx=1  -Dversion="${VERSION}" dist

echo "http://downloads.arduino.cc/javaide/pull_requests/arduino-${VERSION}-linux32.zip"
echo "http://downloads.arduino.cc/javaide/pull_requests/arduino-${VERSION}-linux64.zip"
echo "http://downloads.arduino.cc/javaide/pull_requests/arduino-${VERSION}-windows.zip"
echo "http://downloads.arduino.cc/javaide/pull_requests/arduino-${VERSION}-macosx.zip"
