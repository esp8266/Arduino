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

if [ "${CI-}" = "true" ] ; then
    ci_group="::group::"
    ci_end_group="::endgroup::"
    ci_error="::error::"
else
    ci_group="==> "
    ci_end_group=""
    ci_error=">>> "
fi

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

function print_sketch_info()
{
    local build_mod=$1
    local build_rem=$2

    local testcnt=0
    local cnt=0

    for sketch in $ESP8266_ARDUINO_SKETCHES; do
        testcnt=$(( ($testcnt + 1) % $build_mod ))
        if [ $testcnt -ne $build_rem ]; then
            continue  # Not ours to do
        fi

        local skip
        skip=$(skip_sketch "$sketch")
        if [ -n "$skip" ]; then
            continue # Should be skipped / cannot be built
        fi

        cnt=$(( $cnt + 1 ))
        printf '%2d\t%s\n' "$cnt" "$sketch"
    done
}

function format_fqbn()
{
    local board_name=$1
    local flash_size=$2
    local lwip=$3

    echo "esp8266com:esp8266:${board_name}:"\
"eesz=${flash_size},"\
"ip=${lwip}"
}

function build_sketches()
{
    local core_path=$1
    local cli_path=$2
    local library_path=$3
    local lwip=$4
    local build_mod=$5
    local build_rem=$6
    local build_cnt=$7

    local build_dir="$cache_dir"/build
    mkdir -p "$build_dir"

    local build_out="$cache_dir"/out
    mkdir -p "$build_out"

    local fqbn=$(format_fqbn "generic" "4M1M" "$lwip")
    echo "FQBN: $fqbn"

    local build_cmd
    build_cmd+=${cli_path}
    build_cmd+=" compile"\
" --build-path $build_dir"\
" --fqbn $fqbn"\
" --libraries $library_path"\
" --output-dir $build_out"

    print_size_info_header >"$cache_dir"/size.log

    local clean_core=1
    local testcnt=0
    local cnt=0

    for sketch in $ESP8266_ARDUINO_SKETCHES; do
        testcnt=$(( ($testcnt + 1) % $build_mod ))
        if [ $testcnt -ne "$build_rem" ]; then
            continue  # Not ours to do
        fi

        local skip
        skip=$(skip_sketch "$sketch")
        if [ -n "$skip" ]; then
            echo "$skip"
            continue # Should be skipped / cannot be built
        fi

        cnt=$(( $cnt + 1 ))
        if [ $build_cnt != 0 ] ; then
            if [ $build_cnt != $cnt ] ; then
                continue # Haven't reached the $cnt yet
            fi
            build_cnt=0
        fi

        # Do we need a clean core build? $build_dir/core/* cannot be shared
        # between sketches when global options are present.
        clean_core=$(arduino_mkbuildoptglobals_cleanup "$clean_core" "$build_dir" "$sketch")

        # Clear out the last built sketch, map, elf, bin files, but leave the compiled
        # objects in the core and libraries available for use so we don't need to rebuild
        # them each sketch.
        rm -rf "$build_dir"/sketch \
            "$build_dir"/*.bin \
            "$build_dir"/*.map \
            "$build_dir"/*.elf

        echo ${ci_group}Building $cnt $sketch
        echo "$build_cmd $sketch"

        local result
        time $build_cmd $sketch >"$cache_dir"/build.log \
            && result=0 || result=1

        if [ $result -ne 0 ]; then
            echo ${ci_error}Build failed for $cnt $sketch
            cat "$cache_dir/build.log"
            echo $ci_end_group
            return $result
        else
            grep -s -c warning: "$cache_dir"/build.log \
                && step_summary "$sketch warnings" "$cache_dir/build.log"
        fi

        print_size_info "$core_path"/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-size \
            $build_dir/*.elf >>$cache_dir/size.log

        echo $ci_end_group
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
    local extract=$3
    local archive=$4
    local hash=$5
    local url=$6

    fetch_and_unpack "$archive" "$hash" "$url"
    mkdir -p "$lib_path"
    rm -rf "$lib_path/$name"
    mv "$extract" "$lib_path/$name"
}

function install_libraries()
{
    local core_path=$1
    local lib_path=$2

    mkdir -p "$core_path"/tools/dist
    pushd "$core_path"/tools/dist

    source "$root/tests/dep-libraries.sh"

    popd
}

function install_arduino_cli()
{
    local path=$1
    local core_path=$2

    local ver='1.2.2'
    local urlbase="https://github.com/arduino/arduino-cli/releases/download/v${ver}/arduino-cli_${ver}_"

    echo "Arduino CLI ${ver}"

    mkdir -p ${core_path}/dist
    pushd ${core_path}/dist

    source "$root/tests/dep-arduino-cli.sh"

    mkdir -p $(dirname $path)
    cp -v arduino-cli $path
    chmod +x $path

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
    echo ${ci_group}Install arduino
    local debug=$1

    local hardware_core_path="$ESP8266_ARDUINO_HARDWARE/esp8266com/esp8266"
    test -d "$hardware_core_path" \
        || install_core "$ESP8266_ARDUINO_BUILD_DIR" "$hardware_core_path" "$debug"

    command -v "${ESP8266_ARDUINO_CLI}" \
        || install_arduino_cli "${ESP8266_ARDUINO_CLI}" "$hardware_core_path"

    install_libraries "$ESP8266_ARDUINO_BUILD_DIR" "$ESP8266_ARDUINO_LIBRARIES"

    echo $ci_end_group
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

# mkbuildoptglobals.py is optimized around the Arduino IDE 1.x
# behaviour. One way the CI differs from the Arduino IDE is in the
# handling of core and caching core. With the Arduino IDE, each sketch
# has a private copy of core and contributes to a core cache. With the
# CI, there is one shared copy of core for all sketches. When global
# options are used, the shared copy of core and cache are removed before
# and after the build.
function arduino_mkbuildoptglobals_cleanup()
{
    local clean_core=$1
    local build_dir=$2
    local sketch=$3

    if [ -s ${sketch}.globals.h ]; then
        clean_core=1
    fi

    # Remove sketch specific files from ./core/ between builds.
    if [ $clean_core -ne 0 ]; then
        rm -rf "$build_dir"/core/*
    else
        rm -rf "$build_dir/core/build.opt" "$build_dir"/core/*.ino.globals.h
    fi

    if [ -e ${build_dir}/core/*.a ]; then
        # We need to preserve the build.options.json file and replace the last .ino
        # with this sketch's ino file, or builder will throw everything away.
        jq '."sketchLocation" = "'$sketch'"' $build_dir/build.options.json \
            > "$build_dir"/build.options.json.tmp
        mv "$build_dir"/build.options.json.tmp "$build_dir"/build.options.json
        if [ $clean_core -ne 0 ]; then
            # Hack workaround for CI not handling core rebuild for global options
            rm ${build_dir}/core/*.a
        fi
    fi

    if [ -s ${sketch}.globals.h ]; then
        # Set to cleanup core at the start of the next build.
        clean_core=1
    else
        clean_core=0
    fi

    echo $clean_core
}

function build_sketches_with_arduino()
{
    local lwip
    lwip=$(arduino_lwip_menu_option $1)

    local build_mod=$2
    local build_rem=$3
    local build_cnt=$4

    build_sketches "$ESP8266_ARDUINO_BUILD_DIR" \
        "$ESP8266_ARDUINO_CLI" \
        "$ESP8266_ARDUINO_LIBRARIES" \
        "$lwip" "$build_mod" "$build_rem" "$build_cnt"
    step_summary "Size report" "$cache_dir/size.log"
}

function install_platformio()
{
    echo ${ci_group}Install PlatformIO

    local board=$1

    pushd $ESP8266_ARDUINO_BUILD_DIR/tools
    python3 get.py -q
    popd

    install_libraries "$ESP8266_ARDUINO_BUILD_DIR" "$ESP8266_ARDUINO_LIBRARIES"

    # we should reference our up-to-date build tools
    # ref. https://docs.platformio.org/en/latest/core/userguide/pkg/cmd_install.html
    pio pkg install --global --skip-dependencies --platform "https://github.com/platformio/platform-espressif8266.git"

    local framework_symlink="framework-arduinoespressif8266 @ symlink://${ESP8266_ARDUINO_BUILD_DIR}"
    local toolchain_symlink="toolchain-xtensa @ symlink://${ESP8266_ARDUINO_BUILD_DIR}/tools/xtensa-lx106-elf/"

    # pre-generate config; pio-ci with multiple '-O' options replace each other instead of appending to the same named list
    cat <<EOF > $cache_dir/platformio.ini
[platformio]
lib_dir =
    ${ESP8266_ARDUINO_LIBRARIES}
[env:$board]
platform = espressif8266
board = $board
framework = arduino
platform_packages =
    ${framework_symlink}
    ${toolchain_symlink}
EOF

    echo $ci_end_group
}

function build_sketches_with_platformio()
{
    local build_mod=$1
    local build_rem=$2
    local build_cnt=$3
    local testcnt=0

    for sketch in $ESP8266_ARDUINO_SKETCHES; do
        testcnt=$(( ($testcnt + 1) % $build_mod ))
        if [ $testcnt -ne $build_rem ]; then
            continue  # Not ours to do
        fi

        local skip
        skip=$(skip_sketch "$sketch")
        if [ -n "$skip" ]; then
            echo "$skip"
            continue # Should be skipped / cannot be built
        fi

        cnt=$(( $cnt + 1 ))
        if [ $build_cnt != 0 ] ; then
            if [ $build_cnt != $cnt ] ; then
                continue # Haven't reached the $cnt yet
            fi
            build_cnt=0
        fi

        echo ${ci_group}Building $sketch

        local sketchdir
        sketchdir=$(dirname $sketch)

        local result
        time pio ci \
            --verbose \
            --project-conf $cache_dir/platformio.ini \
            $sketchdir >$cache_dir/build.log 2>&1 \
            && result=0 || result=1

        if [ $result -ne 0 ]; then
            echo ${ci_error}Build failed for $sketch
            cat "$cache_dir/build.log"
            echo $ci_end_group
            return $result
        fi

        echo $ci_end_group
    done
}

if [ -z "${ESP8266_ARDUINO_BUILD_DIR-}" ]; then
    ESP8266_ARDUINO_BUILD_DIR=$(git rev-parse --show-toplevel)
    echo "Using ESP8266_ARDUINO_BUILD_DIR=$ESP8266_ARDUINO_BUILD_DIR"
fi
