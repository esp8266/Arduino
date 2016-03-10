#!/bin/bash

function build_sketches()
{
    set +e
    local arduino=$1
    local srcpath=$2
    local build_cmd=$3
    echo $build_cmd
    local sketches=$(find $srcpath -name *.ino)
    export ARDUINO_IDE_PATH=$arduino
    for sketch in $sketches; do
        local sketchdir=$(dirname $sketch)
        local sketchdirname=$(basename $sketchdir)
        local sketchname=$(basename $sketch)
        if [[ "${sketchdirname}.ino" != "$sketchname" ]]; then
            echo "Skipping $sketch, beacause it is not the main sketch file";
            continue
        fi;
        if [[ -f "$sketchdir/.test.skip" ]]; then
            echo -e "\n ------------ Skipping $sketch ------------ \n";
            continue
        fi
        echo -e "\n ------------ Building $sketch ------------ \n";
        # $arduino --verify $sketch;
        echo "$build_cmd $sketch"
        time ($build_cmd $sketch >build.log)
        local result=$?
        if [ $result -ne 0 ]; then
            echo "Build failed ($1)"
            echo "Build log:"
            cat build.log
            return $result
        fi
        rm build.log
    done
    set -e
}

function install_libraries()
{
    mkdir -p $HOME/Arduino/libraries
    pushd $HOME/Arduino/libraries

    # install ArduinoJson library
    wget https://github.com/bblanchon/ArduinoJson/releases/download/v4.6.1/ArduinoJson-v4.6.1.zip && unzip ArduinoJson-v4.6.1.zip

    popd
}
