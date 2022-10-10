#!/usr/bin/env bash

set -e -E -o pipefail

cache_dir=$(mktemp -d)
trap 'trap_exit' EXIT

function trap_exit()
{
    if [ -z "$ESP8266_ARDUINO_PRESERVE_CACHE" ]; then
        rm -rf "$cache_dir"
    fi
}

# ref. https://docs.github.com/en/actions/using-workflows/workflow-commands-for-github-actions#adding-a-job-summary
function github_step_summary()
{
    local header=$1
    local contents=$2

    if [ -n "$CI" ]; then
        { echo "# $header"; echo '```console'; cat "$contents"; echo '```'; } \
            >> $GITHUB_STEP_SUMMARY
    else
        echo "# $header"
        cat "$contents"
    fi
}

# return 0 if this sketch should not be built in CI (for other archs, not needed, etc.)
function skip_ino()
{
    case $1 in
    *"/#attic/"* | \
    *"/AvrAdcLogger/"* | \
    *"/examplesV1/"* | \
    *"/RtcTimestampTest/"* | \
    *"/SoftwareSpi/"* | \
    *"/TeensyDmaAdcLogger/"* | \
    *"/TeensyRtcTimestamp/"* | \
    *"/TeensySdioDemo/"* | \
    *"/TeensySdioLogger/"* | \
    *"/UserChipSelectFunction/"* | \
    *"/UserSPIDriver/"*)
        return 0
        ;;
    *"Teensy"*)
        return 0
        ;;
    *)
        ;;
    esac

    return 1
}

# return reason if this sketch is not the main one or it is explicitly disabled with .test.skip in its directory
function skip_sketch()
{
    local sketch=$1
    local sketchname=$2
    local sketchdir=$3
    local sketchdirname=$4

    if [[ "${sketchdirname}.ino" != "$sketchname" ]]; then
        echo "Skipping $sketch (not the main sketch file)"
    fi
    if skip_ino "$sketch" || [[ -f "$sketchdir/.test.skip" ]]; then
        echo "Skipping $sketch"
    fi
}

function print_size_info()
{
    local elf_file=$1
    local awk_script='
/^\.data/ || /^\.rodata/ || /^\.bss/ || /^\.text/ || /^\.irom0\.text/{
    size[$1] = $2
}

END {
    total_ram = size[".data"] + size[".rodata"] + size[".bss"]
    total_flash = size[".data"] + size[".rodata"] + size[".text"] + size[".irom0.text"]

    printf "%-28s %-8d %-8d %-8d %-8d %-10d %-8d %-8d\n",
            sketch_name,
            size[".data"], size[".rodata"], size[".bss"], size[".text"], size[".irom0.text"],
            total_ram, total_flash
}
'

    if [ -z "$elf_file" ]; then
        printf "%-28s %-8s %-8s %-8s %-8s %-10s %-8s %-8s\n" sketch data rodata bss text irom0.text dram flash
        return 0
    fi

    local elf_name
    elf_name=$(basename $elf_file)
    xtensa-lx106-elf-size --format=sysv "$elf_file" | \
        awk -v sketch_name="${elf_name%.*}" "$awk_script" -
}

function build_sketches()
{
    local ide_path=$1
    local hardware_path=$2
    local library_path=$3
    local build_mod=$4
    local build_rem=$5
    local lwip=$6

    local build_dir="$cache_dir"/build
    mkdir -p "$build_dir"

    local build_cache="$cache_dir"/cache
    mkdir -p "$build_cache"

    local build_cmd
    build_cmd="python3 tools/build.py"\
" --build_cache $build_cache"\
" --build_path $build_dir"\
" --hardware_path $hardware_path"\
" --ide_path $ide_path"\
" --library_path $library_path"\
" --lwIP $lwip"\
" --board_name generic --verbose --warnings all"\
" --flash_size 4M1M --keep"\
" $build_arg"

    print_size_info >"$cache_dir"/size.log

    local mk_clean_core=1
    local testcnt=0

    for sketch in $ESP8266_ARDUINO_SKETCHES; do
        testcnt=$(( ($testcnt + 1) % $build_mod ))
        if [ $testcnt -ne "$build_rem" ]; then
            continue  # Not ours to do
        fi

        # mkbuildoptglobals.py is optimized around the Arduino IDE 1.x
        # behaviour. One way the CI differs from the Arduino IDE is in the
        # handling of core and caching core. With the Arduino IDE, each sketch
        # has a private copy of core and contributes to a core cache. With the
        # CI, there is one shared copy of core for all sketches. When global
        # options are used, the shared copy of core and cache are removed before
        # and after the build.
        #
        # Do we need a clean core build? $build_dir/core/* cannot be shared
        # between sketches when global options are present.
        if [ -s ${sketch}.globals.h ]; then
            mk_clean_core=1
        fi
        if [ $mk_clean_core -ne 0 ]; then
            rm -rf "$build_dir"/core/*
        else
            # Remove sketch specific files from ./core/ between builds.
            rm -rf "$build_dir/core/build.opt" "$build_dir"/core/*.ino.globals.h
        fi

        if [ -e $cache_dir/core/*.a ]; then
            # We need to preserve the build.options.json file and replace the last .ino
            # with this sketch's ino file, or builder will throw everything away.
            jq '."sketchLocation" = "'$sketch'"' $build_dir/build.options.json \
                > "$build_dir"/build.options.json.tmp
            mv "$build_dir"/build.options.json.tmp "$build_dir"/build.options.json
            if [ $mk_clean_core -ne 0 ]; then
                # Hack workaround for CI not handling core rebuild for global options
                rm $cache_dir/core/*.a
            fi
        fi

        if [ -s ${sketch}.globals.h ]; then
            # Set to cleanup core at the start of the next build.
            mk_clean_core=1
        else
            mk_clean_core=0
        fi

        # Clear out the last built sketch, map, elf, bin files, but leave the compiled
        # objects in the core and libraries available for use so we don't need to rebuild
        # them each sketch.
        rm -rf "$build_dir"/sketch \
            "$build_dir"/*.bin \
            "$build_dir"/*.map \
            "$build_dir"/*.elf

        local sketchdir
        sketchdir=$(dirname $sketch)

        local sketchdirname
        sketchdirname=$(basename $sketchdir)

        local sketchname
        sketchname=$(basename $sketch)

        local skip
        skip=$(skip_sketch "$sketch" "$sketchname" "$sketchdir" "$sketchdirname")
        if [ -n "$skip" ]; then
            echo "$skip"
            continue
        fi

        echo ::group::Building $sketch
        echo "$build_cmd $sketch"

        local result
        time $build_cmd $sketch >"$cache_dir"/build.log \
            && result=0 || result=1

        if [ $result -ne 0 ]; then
            echo ::error::Build failed for $sketch
            cat "$cache_dir/build.log"
            echo ::endgroup::
            return $result
        else
            grep -s -c warning: "$cache_dir"/build.log \
                && github_step_summary "$sketch warnings" "$cache_dir/build.log"
        fi

        print_size_info $build_dir/*.elf >>$cache_dir/size.log

        echo ::endgroup::
    done
}

function fetch_and_unpack() {
    local archive=$1
    local url=$2

    test -r "$archive" || curl --output "$archive" --location "$url"
    case $archive in
    *".zip")
        unzip -q "$archive"
        ;;
    *)
        tar xf "$archive"
        ;;
    esac
}

function install_libraries()
{
    mkdir -p "$ESP8266_ARDUINO_LIBRARIES"
    pushd "$ESP8266_ARDUINO_LIBRARIES"

    fetch_and_unpack "ArduinoJson-v6.11.0.zip" \
        "https://github.com/bblanchon/ArduinoJson/releases/download/v6.11.0/ArduinoJson-v6.11.0.zip"

    popd
}

function install_ide()
{
    # notice that this is 1.8.x, not 2.x.x
    local idever='nightly'
    local ideurl="https://downloads.arduino.cc/arduino-$idever"

    echo "Arduino IDE ${idever}"

    local ide_path=$1
    local hardware_path=$2
    local core_path=$3
    local debug=$4

    mkdir -p ${core_path}/tools/dist
    pushd ${core_path}/tools/dist

    if [ "$RUNNER_OS" = "Windows" ]; then
        fetch_and_unpack "arduino-windows.zip" "${ideurl}-windows.zip"
        mv arduino-$idever arduino-distrib
    elif [ "$RUNNER_OS" = "macOS" ]; then
        fetch_and_unpack "arduino-macos.zip" "${ideurl}-macosx.zip"
        # Hack to place arduino-builder in the same spot as sane OSes
        mv Arduino.app arduino-distrib
        mv arduino-distrib/Contents/Java/* arduino-distrib/.
    else
        fetch_and_unpack "arduino-linux.tar.xz" "${ideurl}-linux64.tar.xz"
        mv arduino-$idever arduino-distrib
    fi

    mv arduino-distrib $ide_path
    popd

    pushd "${core_path}"

    local debug_flags=""
    if [ "$debug" = "debug" ]; then
        debug_flags="-DDEBUG_ESP_PORT=Serial -DDEBUG_ESP_SSL -DDEBUG_ESP_TLS_MEM"\
" -DDEBUG_ESP_HTTP_CLIENT -DDEBUG_ESP_HTTP_SERVER -DDEBUG_ESP_CORE -DDEBUG_ESP_WIFI"\
" -DDEBUG_ESP_HTTP_UPDATE -DDEBUG_ESP_UPDATER -DDEBUG_ESP_OTA -DDEBUG_ESP_OOM"
    fi

    # Set our custom warnings for all builds
    { echo "compiler.c.extra_flags=-Wall -Wextra -Werror $debug_flags";
      echo "compiler.cpp.extra_flags=-Wall -Wextra -Werror $debug_flags";
      echo "mkbuildoptglobals.extra_flags=--ci --cache_core"; } \
          > platform.local.txt
    echo -e "\n----platform.local.txt----"
    cat platform.local.txt
    echo -e "\n----\n"

    pushd tools
    python3 get.py -q
    if [ "$RUNNER_OS" = "Windows" ]; then
        # Because the symlinks don't work well under Win32, we need to add the path to this copy, not the original...
        relbin=$(realpath "$PWD"/xtensa-lx106-elf/bin)
        export PATH="$ide_path:$relbin:$PATH"
    else
        export PATH="$ide_path:$core_path/tools/xtensa-lx106-elf/bin:$PATH"
    fi

    popd
    popd

    mkdir -p "$hardware_path"/esp8266com
    pushd "$hardware_path"/esp8266com
    if [ "$RUNNER_OS" = "Windows" ]; then
        cp -a "$core_path" esp8266
    else
        ln -s "$core_path" esp8266
    fi
    popd
}

function install_arduino()
{
    local debug=$1
    echo ::group::Install arduino
    pushd $ESP8266_ARDUINO_BUILD_DIR
    install_ide "$ESP8266_ARDUINO_IDE" "$ESP8266_ARDUINO_HARDWARE" "$ESP8266_ARDUINO_BUILD_DIR" "$debug"
    install_libraries
    popd
    echo ::endgroup::
}

function arduino_lwip_menu_option() {
    case $1 in
    "default")
        echo "lm2f"
        ;;
    "IPv6")
        echo "lm6f"
        ;;
    esac
}

function build_sketches_with_arduino()
{
    local build_mod=$1
    local build_rem=$2

    local lwip
    lwip=$(arduino_lwip_menu_option $3)

    build_sketches "$ESP8266_ARDUINO_IDE" "$ESP8266_ARDUINO_HARDWARE" "$ESP8266_ARDUINO_LIBRARIES" "$build_mod" "$build_rem" "$lwip"
    github_step_summary "Size report" "$cache_dir/size.log"
}

function install_platformio()
{
    echo ::group::Install PlatformIO

    local board=$1

    pushd $ESP8266_ARDUINO_BUILD_DIR/tools
    python3 get.py -q
    popd

    # we should reference our up-to-date build tools
    # ref. https://docs.platformio.org/en/latest/core/userguide/pkg/cmd_install.html
    pio pkg install --global --skip-dependencies --platform "https://github.com/platformio/platform-espressif8266.git"

    local framework_symlink="framework-arduinoespressif8266 @ symlink://${ESP8266_ARDUINO_BUILD_DIR}"
    local toolchain_symlink="toolchain-xtensa @ symlink://${ESP8266_ARDUINO_BUILD_DIR}/tools/xtensa-lx106-elf/"

    # pre-generate config; pio-ci with multiple '-O' replace each other instead of appending to the same named list
    # (and, it is much nicer to write this instead of a multi-line cmdline with several large strings)
    cat <<EOF > $cache_dir/platformio.ini
[env:$board]
platform = espressif8266
board = $board
framework = arduino
platform_packages =
    ${framework_symlink}
    ${toolchain_symlink}
EOF

    # Install dependencies:
    # - esp8266/examples/ConfigFile
    pio pkg install --global --library "ArduinoJson@^6.11.0"

    echo ::endgroup::
}

function build_sketches_with_platformio()
{
    local build_mod=$1
    local build_rem=$2
    local testcnt=0

    for sketch in $ESP8266_ARDUINO_SKETCHES; do
        testcnt=$(( ($testcnt + 1) % $build_mod ))
        if [ $testcnt -ne $build_rem ]; then
            continue  # Not ours to do
        fi

        local sketchdir
        sketchdir=$(dirname $sketch)

        local sketchdirname
        sketchdirname=$(basename $sketchdir)

        local sketchname
        sketchname=$(basename $sketch)

        local skip
        skip=$(skip_sketch "$sketch" "$sketchname" "$sketchdir" "$sketchdirname")
        if [ -n "$skip" ]; then
            echo "$skip"
            continue
        fi

        echo ::group::Building $sketch

        local result
        time pio ci \
            --verbose \
            --project-conf $cache_dir/platformio.ini \
            $sketchdir >$cache_dir/build.log 2>&1 \
            && result=0 || result=1

        if [ $result -ne 0 ]; then
            echo ::error::Build failed for $sketch
            cat "$cache_dir/build.log"
            echo ::endgroup::
            return $result
        fi

        echo ::endgroup::
    done
}

if [ -z "$ESP8266_ARDUINO_BUILD_DIR" ]; then
    ESP8266_ARDUINO_BUILD_DIR=$(git rev-parse --show-toplevel)
    echo "Using ESP8266_ARDUINO_BUILD_DIR=$ESP8266_ARDUINO_BUILD_DIR"
fi
