#!/usr/bin/env bash

function print_size_info()
{
    elf_file=$1

    if [ -z "$elf_file" ]; then
        printf "sketch                       data     rodata   bss      text     irom0.text   dram     flash\n"
        return 0
    fi

    elf_name=$(basename $elf_file)
    sketch_name="${elf_name%.*}"
    # echo $sketch_name
    declare -A segments
    while read -a tokens; do
        seg=${tokens[0]}
        seg=${seg//./}
        size=${tokens[1]}
        addr=${tokens[2]}
        if [ "$addr" -eq "$addr" -a "$addr" -ne "0" ] 2>/dev/null; then
            segments[$seg]=$size
        fi


    done < <(xtensa-lx106-elf-size --format=sysv $elf_file)

    total_ram=$((${segments[data]} + ${segments[rodata]} + ${segments[bss]}))
    total_flash=$((${segments[data]} + ${segments[rodata]} + ${segments[text]} + ${segments[irom0text]}))

    printf "%-28s %-8d %-8d %-8d %-8d %-8d     %-8d %-8d\n" $sketch_name ${segments[data]} ${segments[rodata]} ${segments[bss]} ${segments[text]} ${segments[irom0text]} $total_ram $total_flash
    return 0
}

function build_sketches()
{
    set +e
    local arduino=$1
    local srcpath=$2
    local build_arg=$3
    local build_dir=build.tmp
    local build_mod=$4
    local build_rem=$5
    local lwip=$6
    mkdir -p $build_dir
    local build_cmd="python tools/build.py -b generic -v -w all -s 4M1M -v -k --build_cache $cache_dir -p $PWD/$build_dir -n $lwip $build_arg "
    local sketches=$(find $srcpath -name *.ino | sort)
    print_size_info >size.log
    export ARDUINO_IDE_PATH=$arduino
    local testcnt=0
    for sketch in $sketches; do
        testcnt=$(( ($testcnt + 1) % $build_mod ))
        if [ $testcnt -ne $build_rem ]; then
            continue  # Not ours to do
        fi

        if [ -e $cache_dir/core/*.a ]; then
            # We need to preserve the build.options.json file and replace the last .ino
            # with this sketch's ino file, or builder will throw everything away.
	    sed -i "s,^.*sketchLocation.*$, \"sketchLocation\": \"$sketch\"\,,g" $build_dir/build.options.json
            # Set the time of the cached core.a file to the future so the GIT header
            # we regen won't cause the builder to throw it out and rebuild from scratch.
            touch -d 'now + 1 day' $cache_dir/core/*.a
        fi

        # Clear out the last built sketch, map, elf, bin files, but leave the compiled
        # objects in the core and libraries available for use so we don't need to rebuild
        # them each sketch.
        rm -rf $build_dir/sketch $build_dir/*.bin $build_dir/*.map $build_dir/*.elf

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
        echo -e "\n ------------ Building $sketch ------------ \n";
        # $arduino --verify $sketch;
        echo "$build_cmd $sketch"
        time ($build_cmd $sketch >build.log)
        local result=$?
        if [ $result -ne 0 ]; then
            echo "Build failed ($1)"
            echo "Build log:"
            cat build.log
            set -e
            return $result
        else
            local warns=$( grep -c warning: build.log )
            if [ $warns -ne 0 ]; then
                echo "Warnings detected, log follows:"
                cat build.log
            fi
        fi
        rm build.log
        print_size_info $build_dir/*.elf >>size.log
    done
    set -e
}

function install_libraries()
{
    mkdir -p $HOME/Arduino/libraries
    pushd $HOME/Arduino/libraries

    # install ArduinoJson library
    { test -r ArduinoJson-v4.6.1.zip || wget https://github.com/bblanchon/ArduinoJson/releases/download/v4.6.1/ArduinoJson-v4.6.1.zip; } && unzip ArduinoJson-v4.6.1.zip

    popd
}

function install_ide()
{
    local ide_path=$1
    local core_path=$2
    local debug=$3
    test -r arduino.tar.xz || wget -O arduino.tar.xz https://www.arduino.cc/download.php?f=/arduino-nightly-linux64.tar.xz
    tar xf arduino.tar.xz
    mv arduino-nightly $ide_path
    cd $ide_path/hardware
    mkdir esp8266com
    cd esp8266com
    ln -s $core_path esp8266
    local debug_flags=""
    if [ "$debug" = "debug" ]; then
        debug_flags="-DDEBUG_ESP_PORT=Serial -DDEBUG_ESP_SSL -DDEBUG_ESP_TLS_MEM -DDEBUG_ESP_HTTP_CLIENT -DDEBUG_ESP_HTTP_SERVER -DDEBUG_ESP_CORE -DDEBUG_ESP_WIFI -DDEBUG_ESP_HTTP_UPDATE -DDEBUG_ESP_UPDATER -DDEBUG_ESP_OTA -DDEBUG_ESP_OOM"
    fi
    # Set custom warnings for all builds (i.e. could add -Wextra at some point)
    echo "compiler.c.extra_flags=-Wall -Wextra -Werror $debug_flags" > esp8266/platform.local.txt
    echo "compiler.cpp.extra_flags=-Wall -Wextra -Werror $debug_flags" >> esp8266/platform.local.txt
    echo -e "\n----platform.local.txt----"
    cat esp8266/platform.local.txt
    echo -e "\n----\n"
    cd esp8266/tools
    python get.py
    export PATH="$ide_path:$core_path/tools/xtensa-lx106-elf/bin:$PATH"
}

function install_platformio()
{
    pip install --user -U https://github.com/platformio/platformio/archive/develop.zip
    platformio platform install "https://github.com/platformio/platform-espressif8266.git#feature/stage"
    sed -i 's/https:\/\/github\.com\/esp8266\/Arduino\.git/*/' ~/.platformio/platforms/espressif8266/platform.json
    ln -s $TRAVIS_BUILD_DIR ~/.platformio/packages/framework-arduinoespressif8266
    # Install dependencies:
    # - esp8266/examples/ConfigFile
    pio lib install ArduinoJson
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
            echo "Skipping $sketch, beacause it is not the main sketch file";
            continue
        fi;
        if [[ -f "$sketchdir/.test.skip" ]]; then
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

function install_arduino()
{
    local debug=$1
    # Install Arduino IDE and required libraries
    echo -e "travis_fold:start:sketch_test_env_prepare"
    cd $TRAVIS_BUILD_DIR
    install_ide $HOME/arduino_ide $TRAVIS_BUILD_DIR $debug
    which arduino
    cd $TRAVIS_BUILD_DIR
    install_libraries
    echo -e "travis_fold:end:sketch_test_env_prepare"
}

function build_sketches_with_arduino()
{
    local build_mod=$1
    local build_rem=$2
    local lwip=$3

    # Compile sketches
    echo -e "travis_fold:start:sketch_test"
    build_sketches $HOME/arduino_ide $TRAVIS_BUILD_DIR/libraries "-l $HOME/Arduino/libraries" $build_mod $build_rem $lwip
    echo -e "travis_fold:end:sketch_test"

    # Generate size report
    echo -e "travis_fold:start:size_report"
    cat size.log
    echo -e "travis_fold:end:size_report"
}


set -e

if [ -z "$TRAVIS_BUILD_DIR" ]; then
    echo "TRAVIS_BUILD_DIR is not set, trying to guess:"
    pushd $(dirname $0)/../ > /dev/null
    TRAVIS_BUILD_DIR=$PWD
    popd > /dev/null
    echo "TRAVIS_BUILD_DIR=$TRAVIS_BUILD_DIR"
fi

cache_dir=$(mktemp -d)

if [ "$BUILD_TYPE" = "build" ]; then
    install_arduino nodebug
    build_sketches_with_arduino 1 0 lm2f
elif [ "$BUILD_TYPE" = "build6" ]; then
    install_arduino nodebug
    build_sketches_with_arduino 1 0 lm6f
elif [ "$BUILD_TYPE" = "build_even" ]; then
    install_arduino nodebug
    build_sketches_with_arduino 2 0 lm2f
elif [ "$BUILD_TYPE" = "build_odd" ]; then
    install_arduino nodebug
    build_sketches_with_arduino 2 1 lm2f
elif [ "$BUILD_TYPE" = "debug_even" ]; then
    install_arduino debug
    build_sketches_with_arduino 2 0 lm2f
elif [ "$BUILD_TYPE" = "debug_odd" ]; then
    install_arduino debug
    build_sketches_with_arduino 2 1 lm2f
elif [ "$BUILD_TYPE" = "build6_even" ]; then
    install_arduino nodebug
    build_sketches_with_arduino 2 0 lm6f
elif [ "$BUILD_TYPE" = "build6_odd" ]; then
    install_arduino nodebug
    build_sketches_with_arduino 2 1 lm6f
elif [ "$BUILD_TYPE" = "platformio" ]; then
    # PlatformIO
    install_platformio
    build_sketches_with_platformio $TRAVIS_BUILD_DIR/libraries "--board nodemcuv2 --verbose" 1 0
elif [ "$BUILD_TYPE" = "platformio_even" ]; then
    # PlatformIO
    install_platformio
    build_sketches_with_platformio $TRAVIS_BUILD_DIR/libraries "--board nodemcuv2 --verbose" 2 0
elif [ "$BUILD_TYPE" = "platformio_odd" ]; then
    # PlatformIO
    install_platformio
    build_sketches_with_platformio $TRAVIS_BUILD_DIR/libraries "--board nodemcuv2 --verbose" 2 1
else
    echo "BUILD_TYPE not set or invalid"
    rm -rf $cache_dir
    exit 1
fi

rm -rf $cache_dir
