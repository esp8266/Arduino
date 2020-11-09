#!/usr/bin/env bash

cache_dir=$(mktemp -d)

source "$TRAVIS_BUILD_DIR"/tests/common.sh

function install_platformio()
{
    pip3 install -U platformio
    platformio platform install "https://github.com/platformio/platform-espressif8266.git"
    # Overwrite toolchain with this PR's toolset.  Probably better way to do this
    ( cd $TRAVIS_BUILD_DIR/tools && python3 get.py -q )
    mv $TRAVIS_BUILD_DIR/tools/xtensa-lx106-elf ~/.platformio/packages/toolchain-xtensa-latest
    mv ~/.platformio/packages/toolchain-xtensa/package.json ~/.platformio/packages/toolchain-xtensa/.piopm ~/.platformio/packages/toolchain-xtensa-latest/
    python -c "import json; import os; fp=open(os.path.expanduser('~/.platformio/platforms/espressif8266/platform.json'), 'r+'); data=json.load(fp); data['packages']['framework-arduinoespressif8266']['version'] = '*'; del data['packages']['framework-arduinoespressif8266']['owner'];fp.seek(0); fp.truncate(); json.dump(data, fp); fp.close()"
    ln -sf $TRAVIS_BUILD_DIR ~/.platformio/packages/framework-arduinoespressif8266
    # Install dependencies:
    # - esp8266/examples/ConfigFile
    pio lib --global install "ArduinoJson@^6.11.0"
}

function build_sketches_with_platformio()
{
    set +e
    local srcpath=$1
    local build_arg=$2
    local build_mod=$3
    local build_rem=$4
    local sketches=$(find $srcpath -name *.ino | sort)
    local testcnt=0
    for sketch in $sketches; do
        testcnt=$(( ($testcnt + 1) % $build_mod ))
        if [ $testcnt -ne $build_rem ]; then
            continue  # Not ours to do
        fi
        local sketchdir=$(dirname $sketch)
        local sketchdirname=$(basename $sketchdir)
        local sketchname=$(basename $sketch)
        if [[ "${sketchdirname}.ino" != "$sketchname" ]]; then
            echo "Skipping $sketch, because it is not the main sketch file";
            continue
        fi;
        if [[ -f "$sketchdir/.test.skip" ]]; then
            echo -e "\n ------------ Skipping $sketch ------------ \n";
            continue
        fi
        if [[ $(skip_ino $sketch) = 1 ]]; then
            echo -e "\n ------------ Skipping $sketch ------------ \n";
            continue
        fi
        local build_cmd="pio ci $sketchdir $build_arg"
        echo -e "\n ------------ Building $sketch ------------ \n";
        echo "$build_cmd"
        time ($build_cmd >build.log)
        local result=$?
        if [ $result -ne 0 ]; then
            echo "Build failed ($1)"
            echo "Build log:"
            cat build.log
            set -e
            return $result
        fi
        rm build.log
    done
    set -e
}

if [ -z "$BUILD_PARITY" ]; then
    mod=1
    rem=0
elif [ "$BUILD_PARITY" = "even" ]; then
    mod=2
    rem=0
elif [ "$BUILD_PARITY" = "odd" ]; then
    mod=2
    rem=1
fi

install_platformio
build_sketches_with_platformio "$TRAVIS_BUILD_DIR"/libraries "--board nodemcuv2 --verbose" "$mod" "$rem"

rm -rf "$cache_dir"

