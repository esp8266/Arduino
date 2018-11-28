#!/bin/sh

# temporary directory

[ -z "${TMPCI}" ] && TMPCI=/tmp/ci

##################

set -e

TMPDIR=${TMPCI%/*}
CIDIR=${TMPCI##*/}

mkdir -p ${TMPDIR}

# set root directory into $ESP
ESP="$(cd ${0%/*}/..; pwd)"

# clone or update this repository into ${TMPDIR}/${CIDIR}
if test -d ${CIDIR}; then
	(cd ${TMPDIR}; git clone ${ESP} ${CIDIR})
else
	(cd ${TMPCI}; git pull)
fi

cd ${TMPCI}
rm -rf arduino_ide arduino-nightly Arduino/libraries/ArduinoJson
HOME=${TMPCI} TRAVIS_BUILD_DIR=${TMPCI} BUILD_TYPE=build tests/common.sh
