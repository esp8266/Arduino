#!/usr/bin/env bash

trap 'echo " ${BASH_SOURCE[1]}:$LINENO $BASH_COMMAND"' ERR

root=$(git rev-parse --show-toplevel)
source "$root/tests/env.sh"

unset -f step_summary
function step_summary()
{
    echo ""
}

export ARDUINO_BUILD_CACHE_PATH="$cache_dir"

esp8266_dir="${ESP8266_ARDUINO_BUILD_DIR}"
commonfileh="${esp8266_dir}/cores/esp8266/CommonHFile.h"

tests_dir="$root/tests/test_mkbuildoptglobals"
tests=$(ls -1 "$tests_dir" | sed 's/.sh$//g')

cores_dir="${cache_dir}/cores/"
sketches_dir="${cache_dir}/sketches/"

function name_size_mtime()
{
    stat --printf '%n:%s:%Y' "$1"
}

function most_recent_file()
{
    local name="$1"
    local location="$2"

    local file
    file=$(ls -t1 "${location}" | grep "$name" | head -1)

    readlink -f "${location}/${file}"
}

function most_recent_dir()
{
    local location="$1"

    local file
    file=$(ls -t1 "$location" | head -1)

    readlink -f "${location}/${file}"
}

function assert_build()
{
    local name="$1"
    local build_dir="$2"
    local size_check="${3:-0}"

    local build_opt="$build_dir"/sketch/build.opt
    test -e "$build_opt"

    local globals_h="$build_dir"/${name}.ino.globals.h
    test -e "$globals_h"

    if [ "$size_check" = "1" ] ; then
        test -s "$build_opt"
        test -s "$globals_h"
    fi
}

function assert_core()
{
    local size_check="$1"

    if [ "$size_check" = "1" ] ; then
        test -s "$commonfileh"
    else
        test ! -s "$commonfileh"
    fi

}

function build_esp8266_example()
{
    local name="$1"

    ESP8266_ARDUINO_SKETCHES="$root/libraries/esp8266/examples/${name}/${name}.ino"
    build_sketches_with_arduino "$ESP8266_ARDUINO_LWIP" "$mod" "$rem" "$cnt"
}

function make_commonh_stat()
{
    local stat
    stat=$(name_size_mtime "$commonfileh")

    test -n "$stat"
    echo "$stat"
}

function make_core_stat()
{
    local recent_core
    recent_core=$(most_recent_dir "$cores_dir")

    local recent_file
    recent_file=$(most_recent_file "core.a" "$recent_core")

    local stat
    stat=$(name_size_mtime "$recent_file")

    test -n "$stat"
    echo "$stat"
}

case "${1:-}" in
"list")
    printf "%s\n" $tests
    ;;

"run")
    for test in $tests ; do
        printf "Checking \"%s\"\n" "$test"
        /usr/bin/env bash "$root"/tests/test_mkbuildoptglobals.sh "$test"
    done
    ;;
*)
    source "$tests_dir/${1}.sh"
    ;;
esac
