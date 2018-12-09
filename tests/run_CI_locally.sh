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
        (cd ${TMPCI}; git checkout master; git branch -D ${branch} || true; git checkout -b ${branch}; git pull origin ${branch})
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

while true; do

	cat << EOF
Which build?
1. main
2. debug even
3. debug odd
4. platformio
5. package
6. host
7. style
EOF

	read ans

	BUILD_TYPE=""
	case "$ans" in
		1) BUILD_TYPE=build;;
		2) BUILD_TYPE=debug_even;;
		3) BUILD_TYPE=debug_odd;;
		4) BUILD_TYPE=platformio;;
		5) BUILD_TYPE=package;;
		6) BUILD_TYPE=host;;
		7) BUILD_TYPE=style;;
	esac
	test -z "$BUILD_TYPE" || break
done

# use pip2 for python2 with python3 is around, platformio doesn't like it
cp tests/common.sh tests/common-custom.sh
sed -i 's,pip ,pip2 ,g' tests/common-custom.sh

HOME=${TMPCI} TRAVIS_BUILD_DIR=${TMPCI} BUILD_TYPE=$BUILD_TYPE tests/common-custom.sh
