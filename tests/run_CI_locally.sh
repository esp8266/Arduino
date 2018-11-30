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
branch=$(git rev-parse --abbrev-ref HEAD)

echo ""
echo " -- CI directory: ${TMPCI} --"
echo ""
echo "Ensure your changes are committed in current branch ${branch}"
echo ""
echo "press return to run 'git diff'"
read junk
git diff
echo "press return to run CI, or ^C"
read junk

# clone or update this repository into ${TMPDIR}/${CIDIR}
if [ -d ${TMPCI} ]; then
	echo ""
	echo " -- updating CI directory in ${TMPCI} --"
	echo ""
	(cd ${TMPCI}; git checkout master; git branch -D ${branch}; git checkout -b ${branch}; git pull origin ${branch})
else
	echo ""
	echo " -- installing CI directory in ${TMPCI} --"
	echo ""
	(cd ${TMPDIR}; git clone ${ESP} ${CIDIR})
fi

cd ${TMPCI}
if [ "$branch" != "$branch" ]; then
	echo "branch ${cibranch} in ${TMPCI} not matching branch ${branch} in ${ESP}"
	exit 1
fi
rm -rf arduino_ide arduino-nightly Arduino/libraries/ArduinoJson
HOME=${TMPCI} TRAVIS_BUILD_DIR=${TMPCI} BUILD_TYPE=build tests/common.sh
