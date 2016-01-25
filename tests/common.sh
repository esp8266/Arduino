#!/bin/bash

function build_sketches()
{
    local arduino=$1
    local srcpath=$2
    local sketches=$(find $srcpath -name *.ino)
    for sketch in $sketches; do
        local sketchdir=$(dirname $sketch)
        if [[ -f "$sketchdir/.test.skip" ]]; then
            echo -e "\n\n ------------ Skipping $sketch ------------ \n\n";
            continue
        fi
        echo -e "\n\n ------------ Building $sketch ------------ \n\n";
        $arduino --verify $sketch;
        local result=$?
        if [ $result -ne 0 ]; then
            echo "Build failed ($1)"
            return $result
        fi
    done
}

function install_libraries()
{
    mkdir -p $HOME/Arduino/libraries
    pushd $HOME/Arduino/libraries
    
    # install ArduinoJson library
    wget https://github.com/bblanchon/ArduinoJson/releases/download/v4.6.1/ArduinoJson-v4.6.1.zip && unzip ArduinoJson-v4.6.1.zip

    popd
}
