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
    mkdir -p $build_dir
    local build_cmd="python tools/build.py -b generic -v -k -p $PWD/$build_dir $build_arg "
    local sketches=$(find $srcpath -name *.ino)
    print_size_info >size.log
    export ARDUINO_IDE_PATH=$arduino
    for sketch in $sketches; do
        rm -rf $build_dir/*
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
        print_size_info $build_dir/*.elf >>size.log
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

function install_ide()
{
    local ide_path=$1
    local core_path=$2
    wget -O arduino.tar.xz https://www.arduino.cc/download.php?f=/arduino-nightly-linux64.tar.xz
    tar xf arduino.tar.xz
    mv arduino-nightly $ide_path
    cd $ide_path/hardware
    mkdir esp8266com
    cd esp8266com
    ln -s $core_path esp8266
    cd esp8266/tools
    python get.py
    export PATH="$ide_path:$core_path/tools/xtensa-lx106-elf/bin:$PATH"
}

function run_host_tests()
{
    pushd host
    make
    make clean-objects
    popd
}


function run_travis_ci_build()
{
    export CXX="g++-4.8" CC="gcc-4.8" GCOV="gcov-4.8"
    echo -e "travis_fold:start:host_tests"
    cd $TRAVIS_BUILD_DIR/tests
    run_host_tests
    echo -e "travis_fold:end:host_tests"
    echo -e "travis_fold:start:sketch_test_env_prepare"
    cd $TRAVIS_BUILD_DIR
    install_ide $HOME/arduino_ide $TRAVIS_BUILD_DIR
    which arduino
    cd $TRAVIS_BUILD_DIR
    install_libraries
    echo -e "travis_fold:end:sketch_test_env_prepare"
    echo -e "travis_fold:start:sketch_test"
    build_sketches $HOME/arduino_ide $TRAVIS_BUILD_DIR/libraries "-l $HOME/Arduino/libraries"
    echo -e "travis_fold:end:sketch_test"
    echo -e "travis_fold:start:size_report"
    cat size.log
    echo -e "travis_fold:end:size_report"
    pushd $TRAVIS_BUILD_DIR/tests/host
    bash <(curl -s https://codecov.io/bash) -X gcov
}

function deploy_nightly_package()
{
    cd $TRAVIS_BUILD_DIR/package
    ./deploy_nightly_package.sh
}

set -e

if [ "$BUILD_TYPE" = "deploy_nightly_package" ]; then
    deploy_nightly_package
elif [ "$BUILD_TYPE" = "build" ]; then
    run_travis_ci_build
fi

