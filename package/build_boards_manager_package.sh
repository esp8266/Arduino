#!/bin/bash
#

#set -x

# Extract next version from platform.txt
next=`sed -n -E 's/version=([0-9.]+)/\1/p' ../platform.txt`

# Figure out how will the package be called
ver=`git describe --exact-match`
if [ $? -ne 0 ]; then
    # not tagged version; generate nightly package
    date_str=`date +"%Y%m%d"`
    is_nightly=1
    plain_ver="${next}-nightly"
    ver="${plain_ver}+${date_str}"
else
    plain_ver=$ver
fi

set -e

package_name=esp8266-$ver
echo "Version: $ver"
echo "Package name: $package_name"

# Set REMOTE_URL environment variable to the address where the package will be
# available for download. This gets written into package json file.
if [ -z "$REMOTE_URL" ]; then
    REMOTE_URL="http://localhost:8000"
    echo "REMOTE_URL not defined, using default"
fi
echo "Remote: $REMOTE_URL"

if [ -z "$PKG_URL" ]; then
    if [ -z "$PKG_URL_PREFIX" ]; then
        PKG_URL_PREFIX="$REMOTE_URL/versions/$ver"
    fi
    PKG_URL="$PKG_URL_PREFIX/$package_name.zip"
fi
echo "Package: $PKG_URL"
echo "Docs: $DOC_URL"

pushd ..
# Create directory for the package
outdir=package/versions/$ver/$package_name
srcdir=$PWD
rm -rf package/versions/$ver
mkdir -p $outdir

# Some files should be excluded from the package
cat << EOF > exclude.txt
.git
.gitignore
.gitmodules
.travis.yml
package
doc
EOF
# Also include all files which are ignored by git
git ls-files --other --directory >> exclude.txt
# Now copy files to $outdir
rsync -a --exclude-from 'exclude.txt' $srcdir/ $outdir/
rm exclude.txt

# For compatibility, on OS X we need GNU sed which is usually called 'gsed'
if [ "$(uname)" == "Darwin" ]; then
    SED=gsed
else
    SED=sed
fi

# Do some replacements in platform.txt file, which are required because IDE
# handles tool paths differently when package is installed in hardware folder
cat $srcdir/platform.txt | \
$SED 's/runtime.tools.xtensa-lx106-elf-gcc.path={runtime.platform.path}\/tools\/xtensa-lx106-elf//g' | \
$SED 's/runtime.tools.esptool.path={runtime.platform.path}\/tools\/esptool//g' | \
$SED 's/tools.esptool.path={runtime.platform.path}\/tools\/esptool/tools.esptool.path=\{runtime.tools.esptool.path\}/g' | \
$SED 's/tools.mkspiffs.path={runtime.platform.path}\/tools\/mkspiffs/tools.mkspiffs.path=\{runtime.tools.mkspiffs.path\}/g' |\
$SED 's/recipe.hooks.core.prebuild.2.pattern.*//g' |\
$SED "s/version=.*/version=$ver/g" |\
$SED -E "s/name=([a-zA-Z0-9\ -]+).*/name=\1($ver)/g"\
 > $outdir/platform.txt

# Put core version and short hash of git version into core_version.h
ver_define=`echo $plain_ver | tr "[:lower:].\055" "[:upper:]_"`
echo Ver define: $ver_define
echo \#define ARDUINO_ESP8266_GIT_VER 0x`git rev-parse --short=8 HEAD 2>/dev/null` >$outdir/cores/esp8266/core_version.h
echo \#define ARDUINO_ESP8266_GIT_DESC `git describe --tags 2>/dev/null` >>$outdir/cores/esp8266/core_version.h
echo \#define ARDUINO_ESP8266_RELEASE_$ver_define >>$outdir/cores/esp8266/core_version.h
echo \#define ARDUINO_ESP8266_RELEASE \"$ver_define\" >>$outdir/cores/esp8266/core_version.h

# Zip the package
pushd package/versions/$ver
echo "Making $package_name.zip"
zip -qr $package_name.zip $package_name
rm -rf $package_name

# Calculate SHA sum and size
sha=`shasum -a 256 $package_name.zip | cut -f 1 -d ' '`
size=`/bin/ls -l $package_name.zip | awk '{print $5}'`
echo Size: $size
echo SHA-256: $sha

echo "Making package_esp8266com_index.json"

jq_arg=".packages[0].platforms[0].version = \"$ver\" | \
    .packages[0].platforms[0].url = \"$PKG_URL\" |\
    .packages[0].platforms[0].archiveFileName = \"$package_name.zip\""

if [ -z "$is_nightly" ]; then
    jq_arg="$jq_arg |\
        .packages[0].platforms[0].size = \"$size\" |\
        .packages[0].platforms[0].checksum = \"SHA-256:$sha\""
fi

if [ ! -z "$DOC_URL" ]; then
    jq_arg="$jq_arg |\
        .packages[0].platforms[0].help.online = \"$DOC_URL\""
fi

cat $srcdir/package/package_esp8266com_index.template.json | \
    jq "$jq_arg" > package_esp8266com_index.json

# Use Github API token, if available
curl_gh_token_arg=()
if [ ! -z "$CI_GITHUB_API_KEY" ]; then
    curl_gh_token_arg=(-H "Authorization: token $CI_GITHUB_API_KEY")
fi
# Get previous release name
curl --silent "${curl_gh_token_arg[@]}" https://api.github.com/repos/esp8266/Arduino/releases > releases.json
# Previous final release (prerelase == false)
prev_release=$(jq -r '. | map(select(.draft == false and .prerelease == false)) | sort_by(.created_at | - fromdateiso8601) | .[0].tag_name' releases.json)
# Previous release (possibly a pre-release)
prev_any_release=$(jq -r '. | map(select(.draft == false)) | sort_by(.created_at | - fromdateiso8601)  | .[0].tag_name' releases.json)
# Previous pre-release
prev_pre_release=$(jq -r '. | map(select(.draft == false and .prerelease == true)) | sort_by(.created_at | - fromdateiso8601)  | .[0].tag_name' releases.json)

echo "Previous release: $prev_release"
echo "Previous (pre-?)release: $prev_any_release"
echo "Previous pre-release: $prev_pre_release"

# Make all released versions available in one package (i.e. don't separate stable/staging versions)
base_ver=$prev_any_release

# Download previous release
echo "Downloading base package: $base_ver"
old_json=package_esp8266com_index_stable.json
curl -L -o $old_json "https://github.com/esp8266/Arduino/releases/download/${base_ver}/package_esp8266com_index.json"
new_json=package_esp8266com_index.json

set +e
python ../../merge_packages.py $new_json $old_json >tmp && mv tmp $new_json && rm $old_json

popd
popd

echo "All done"
