#!/bin/bash -ex

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

VERSION="PR-${ghprbPullId}-BUILD-${BUILD_NUMBER}"

ant -Djava.net.preferIPv4Stack=true -Dplatform=linux32 -Dversion="${VERSION}" clean dist
mv linux/arduino-*$VERSION*.tar.xz ../

ant -Djava.net.preferIPv4Stack=true -Dplatform=linux64 -Dversion="${VERSION}" clean dist
mv linux/arduino-*$VERSION*.tar.xz ../

ant -Djava.net.preferIPv4Stack=true -Dplatform=windows -Dversion="${VERSION}" clean dist
mv windows/arduino-*$VERSION*.zip ../

ant -Djava.net.preferIPv4Stack=true -Dplatform=macosx -Dversion="${VERSION}" clean dist
mv macosx/arduino-*$VERSION*.zip ../

ant -Djava.net.preferIPv4Stack=true -Dplatform=macosx-java-latest -Dversion="${VERSION}" clean dist
mv macosx/arduino-*$VERSION*.zip ../

