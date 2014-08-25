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

VERSION="PR-${ghprbPullId}-BUILD-${BUILD_NUMBER}"

ant -Djava.net.preferIPv4Stack=true -Dplatform=linux32 -Dlinux32=1 -Dversion="${VERSION}" dist
ant -Djava.net.preferIPv4Stack=true -Dplatform=linux64 -Dlinux64=1 -Dversion="${VERSION}" dist
ant -Djava.net.preferIPv4Stack=true -Dplatform=windows -Dwindows=1 -Dversion="${VERSION}" dist
ant -Djava.net.preferIPv4Stack=true -Dplatform=macosx  -Dmacosx=1  -Dversion="${VERSION}" dist

