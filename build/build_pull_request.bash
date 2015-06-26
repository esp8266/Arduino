#!/bin/bash -ex

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR

if [ "x${ghprbPullId}" == "x" ]
then
	exit 1
fi

ant -Djava.net.preferIPv4Stack=true -Dplatform=linux64 -Dlinux64=1 clean build

ERRORS=`grep '<error' ../app/test-bin/TEST-*.xml | wc -l`
if [ $ERRORS -ne 0 ] ;
then
	exit $ERRORS
fi

VERSION="PR-${ghprbPullId}-BUILD-${BUILD_NUMBER}"

./build_all_dist.bash -Dversion="${VERSION}" -DMACOSX_BUNDLED_JVM=$MACOSX_BUNDLED_JVM -DWINDOWS_BUNDLED_JVM=$WINDOWS_BUNDLED_JVM -DLINUX32_BUNDLED_JVM=$LINUX32_BUNDLED_JVM -DLINUX64_BUNDLED_JVM=$LINUX64_BUNDLED_JVM

