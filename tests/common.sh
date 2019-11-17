#!/usr/bin/env bash

# return 1 if this test should not be built in CI (for other archs, not needed, etc.)
function skip_ino()
{
    local ino=$1
    local skiplist=""
    # Add items to the following list with "\n" netween them to skip running.  No spaces, tabs, etc. allowed
    read -d '' skiplist << EOL || true
/#attic/
/AnalogBinLogger/
/LowLatencyLogger/
/LowLatencyLoggerADXL345/
/LowLatencyLoggerMPU6050/
/PrintBenchmark/
/TeensySdioDemo/
/SoftwareSpi/
/STM32Test/
/extras/
EOL
    echo $ino | grep -q -F "$skiplist"
    echo $(( 1 - $? ))
}

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

    done < <(xtensa-lx106-elf-size --format=sysv $elf_file | sed 's/\r//g' )

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
    local build_cmd="python3 tools/build.py -b generic -v -w all -s 4M1M -v -k --build_cache $cache_dir -p $PWD/$build_dir -n $lwip $build_arg "
    if [ "$WINDOWS" = "1" ]; then
        # Paths to the arduino builder need to be / referenced, not our native ones
        build_cmd=$(echo $build_cmd --ide_path $arduino | sed 's/ \/c\// \//g' ) # replace '/c/' with '/'
    fi
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
        if [[ $(skip_ino $sketch) = 1 ]]; then
            echo -e "\n ------------ Skipping $sketch ------------ \n";
            continue
        fi
        echo -e "\n ------------ Building $sketch ------------ \n";
        # $arduino --verify $sketch;
    	if [ "$WINDOWS" == "1" ]; then
                sketch=$(echo $sketch | sed 's/^\/c//')
                # MINGW will try to be helpful and silently convert args that look like paths to point to a spot inside the MinGW dir.  This breaks everything.
                # http://www.mingw.org/wiki/Posix_path_conversion
                # https://stackoverflow.com/questions/7250130/how-to-stop-mingw-and-msys-from-mangling-path-names-given-at-the-command-line#34386471
                export MSYS2_ARG_CONV_EXC="*"
                export MSYS_NO_PATHCONV=1
        fi
        echo "$build_cmd $sketch"
        time ($build_cmd $sketch >build.log)
        time ($build_cmd $sketch )
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
    { test -r ArduinoJson-v6.11.0.zip || wget -nv https://github.com/bblanchon/ArduinoJson/releases/download/v6.11.0/ArduinoJson-v6.11.0.zip; } && unzip -q ArduinoJson-v6.11.0.zip

    popd
}

function install_ide()
{
    #local idever='nightly'
    #local ideurl='https://www.arduino.cc/download.php?f=/arduino-nightly'

    local idever='1.8.10'
    local ideurl="https://downloads.arduino.cc/arduino-$idever"

    echo "using Arduino IDE distribution ${idever}"

    local ide_path=$1
    local core_path=$2
    local debug=$3
    if [ "$WINDOWS" = "1" ]; then
        # Acquire needed packages from Windows package manager
        choco install --no-progress python3 >& pylog.txt
	# Parse the python instrall dir from the output log.  Sorry, can't set it via choco on the free version
	PYDIR=$(cat pylog.txt | grep "^Installed to:"  | cut -f2 -d"'" | sed 's/C:\\/\/c\//')
	echo "Detected python3 install dir: $PYDIR"
        export PATH="$PYDIR:$PATH"  # Ensure it's live from now on...
        cp "$PYDIR/python.exe" "$PYDIR/python3.exe"
        choco install --no-progress unzip
        choco install --no-progress sed
        #choco install --no-progress golang
        test -r arduino-windows.zip || wget -nv -O arduino-windows.zip "${ideurl}-windows.zip"
        unzip -q arduino-windows.zip
        mv arduino-${idever} arduino-distrib
    elif [ "$MACOSX" = "1" ]; then
        # MACOS only has next-to-obsolete Python2 installed.  Install Python 3 from python.org
        wget https://www.python.org/ftp/python/3.7.4/python-3.7.4-macosx10.9.pkg
        sudo installer -pkg python-3.7.4-macosx10.9.pkg -target /
        # Install the Python3 certificates, because SSL connections fail w/o them and of course they aren't installed by default.
        ( cd "/Applications/Python 3.7/" && sudo "./Install Certificates.command" )
        # Hack to place arduino-builder in the same spot as sane OSes
        test -r arduino-macos.zip || wget -O arduino-macos.zip "${ideurl}-macosx.zip"
        unzip -q arduino-macos.zip
        mv Arduino.app arduino-distrib
        mv arduino-distrib/Contents/Java/* arduino-distrib/.
    else
        #test -r arduino.tar.xz || wget -O arduino.tar.xz https://www.arduino.cc/download.php?f=/arduino-nightly-linux64.tar.xz
        test -r arduino-linux.tar.xz || wget -O arduino-linux.tar.xz "${ideurl}-linux64.tar.xz"
        tar xf arduino-linux.tar.xz
        mv arduino-${idever} arduino-distrib
    fi
    mv arduino-distrib $ide_path
    cd $ide_path/hardware
    mkdir esp8266com
    cd esp8266com
    if [ "$WINDOWS" = "1" ]; then
        cp -a $core_path esp8266
    else
        ln -s $core_path esp8266
    fi
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
    python3 get.py -q
    if [ "$WINDOWS" = "1" ]; then
        # Because the symlinks don't work well under Win32, we need to add the path to this copy, not the original...
        relbin=$(realpath $PWD/xtensa-lx106-elf/bin)
        export PATH="$ide_path:$relbin:$PATH"
    else
        export PATH="$ide_path:$core_path/tools/xtensa-lx106-elf/bin:$PATH"
    fi
}

function install_arduino()
{
    local debug=$1
    # Install Arduino IDE and required libraries
    echo -e "travis_fold:start:sketch_test_env_prepare"
    cd $TRAVIS_BUILD_DIR
    install_ide $HOME/arduino_ide $TRAVIS_BUILD_DIR $debug
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

