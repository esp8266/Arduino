#!/usr/bin/env bash

set -u -e -E -o pipefail

cache_dir=$(mktemp -d)
trap 'trap_exit' EXIT

function trap_exit()
{
    # workaround for macOS shipping with broken bash
    local exit_code=$?
    if [ -z "${ESP8266_ARDUINO_PRESERVE_CACHE-}" ]; then
        rm -rf "$cache_dir"
    fi

    exit $exit_code
}

function step_summary()
{
    local header=$1
    local contents=$2

    # ref. https://docs.github.com/en/actions/using-workflows/workflow-commands-for-github-actions#adding-a-job-summary
    if [ -n "${GITHUB_STEP_SUMMARY-}" ]; then
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
    *"/UserSPIDriver/"* | \
    *"/debug/"*)
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

function print_size_info_header()
{
    printf "%-28s %-8s %-8s %-8s %-8s %-10s %-8s %-8s\n" sketch data rodata bss text irom0.text dram flash
}

function print_size_info()
{
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
    local size=$1
    local elf_file=$2

    local elf_name
    elf_name=$(basename $elf_file)
    $size --format=sysv "$elf_file" | \
        awk -v sketch_name="${elf_name%.*}" "$awk_script" -
}

function build_sketches()
{
    local core_path=$1
    local ide_path=$2
    local hardware_path=$3
    local library_path=$4
    local build_mod=$5
    local build_rem=$6
    local lwip=$7

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
" --flash_size 4M1M --keep"

    print_size_info_header >"$cache_dir"/size.log

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
                && step_summary "$sketch warnings" "$cache_dir/build.log"
        fi

        print_size_info "$core_path"/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-size \
            $build_dir/*.elf >>$cache_dir/size.log

        echo ::endgroup::
    done
}

function check_hash()
{
    local file=$1
    local hash=$2

    local shasum
    case ${RUNNER_OS-} in
    "macOS")
        shasum="shasum -a 512"
        ;;
    *)
        shasum="sha512sum"
        ;;
    esac

    echo "$hash  $file" | $shasum -c -
}

function fetch_and_unpack()
{
    local archive=$1
    local hash=$2
    local url=$3

    test -r "$archive" \
        && check_hash "$archive" "$hash" \
        || { pushd "$cache_dir"
             curl --output "$archive" --location "$url";
             check_hash "$archive" "$hash";
             popd;
             mv "$cache_dir/$archive" ./"$archive"; }

    case $archive in
    *".zip")
        unzip -q "$archive"
        ;;
    *)
        tar xf "$archive"
        ;;
    esac
}

function install_library()
{
    local lib_path=$1
    local name=$2
    local archive=$3
    local hash=$4
    local url=$5

    fetch_and_unpack "$archive" "$hash" "$url"
    mkdir -p "$lib_path"
    rm -rf "$lib_path/$name"
    mv "$name" "$lib_path/$name"
}

function install_libraries()
{
    local core_path=$1
    local lib_path=$2

    mkdir -p "$core_path"/tools/dist
    pushd "$core_path"/tools/dist

    install_library "$lib_path" \
        "ArduinoJson" \
        "ArduinoJson-v6.11.5.zip" \
        "8b836c862e69e60c4357a5ed7cbcf1310a3bb1c6bd284fe028faaa3d9d7eed319d10febc8a6a3e06040d1c73aaba5ca487aeffe87ae9388dc4ae1677a64d602c" \
        "https://github.com/bblanchon/ArduinoJson/releases/download/v6.11.5/ArduinoJson-v6.11.5.zip"

    popd
}

function install_ide()
{
    # TODO replace ide distribution + arduino-builder with arduino-cli
    local idever='1.8.19'
    local ideurl="https://downloads.arduino.cc/arduino-$idever"

    echo "Arduino IDE ${idever}"

    local core_path=$1
    local ide_path=$2

    mkdir -p ${core_path}/tools/dist
    pushd ${core_path}/tools/dist

    if [ "${RUNNER_OS-}" = "Windows" ]; then
        fetch_and_unpack "arduino-windows.zip" \
            "c4072d808aea3848bceff5772f9d1e56a0fde02366b5aa523d10975c54eee2ca8def25ee466abbc88995aa323d475065ad8eb30bf35a2aaf07f9473f9168e2da" \
            "${ideurl}-windows.zip"
        mv arduino-$idever arduino-distrib
    elif [ "${RUNNER_OS-}" = "macOS" ]; then
        fetch_and_unpack "arduino-macos.zip" \
            "053b0c1e70da9176680264e40fcb9502f45ca5a879aeb8b6f71282b38bfdb87c63ebc6b88e35ea70a73720ad439d828cc8cb110e4c6ab07357126a36ee396325" \
            "${ideurl}-macosx.zip"
        # Hack to place arduino-builder in the same spot as sane OSes
        mv Arduino.app arduino-distrib
        mv arduino-distrib/Contents/Java/* arduino-distrib/.
    else
        fetch_and_unpack "arduino-linux.tar.xz" \
            "9328abf8778200019ed40d4fc0e6afb03a4cee8baaffbcea7dd3626477e14243f779eaa946c809fb153a542bf2ed60cf11a5f135c91ecccb1243c1387be95328" \
            "${ideurl}-linux64.tar.xz"
        mv arduino-$idever arduino-distrib
    fi

    mv arduino-distrib "$ide_path"
    popd
}

function install_core()
{
    local core_path=$1
    local hardware_core_path=$2
    local debug=$3

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

    popd
    popd

    local core_dir
    core_dir=$(dirname "$hardware_core_path")
    mkdir -p "$core_dir"

    if [ "${RUNNER_OS-}" = "Windows" ]; then
        cp -a "$core_path" "${core_dir}/esp8266"
    else
        ln -s "$core_path" "$hardware_core_path"
    fi
}

function install_arduino()
{
    echo ::group::Install arduino
    local debug=$1

    test -d "$ESP8266_ARDUINO_IDE" \
        || install_ide "$ESP8266_ARDUINO_BUILD_DIR" "$ESP8266_ARDUINO_IDE"

    local hardware_core_path="$ESP8266_ARDUINO_HARDWARE/esp8266com/esp8266"
    test -d "$hardware_core_path" \
        || install_core "$ESP8266_ARDUINO_BUILD_DIR" "$hardware_core_path" "$debug"

    install_libraries "$ESP8266_ARDUINO_BUILD_DIR" "$ESP8266_ARDUINO_LIBRARIES"

    echo ::endgroup::
}

function arduino_lwip_menu_option()
{
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

    build_sketches "$ESP8266_ARDUINO_BUILD_DIR" \
        "$ESP8266_ARDUINO_IDE" \
        "$ESP8266_ARDUINO_HARDWARE" \
        "$ESP8266_ARDUINO_LIBRARIES" \
        "$build_mod" "$build_rem" "$lwip"
    step_summary "Size report" "$cache_dir/size.log"
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

if [ -z "${ESP8266_ARDUINO_BUILD_DIR-}" ]; then
    ESP8266_ARDUINO_BUILD_DIR=$(git rev-parse --show-toplevel)
    echo "Using ESP8266_ARDUINO_BUILD_DIR=$ESP8266_ARDUINO_BUILD_DIR"
fi
