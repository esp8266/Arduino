#!/usr/bin/env bash

# expect to have git available
root=$(git rev-parse --show-toplevel)

# general configuration related to the builder itself
ESP8266_ARDUINO_BUILD_DIR=${ESP8266_ARDUINO_BUILD_DIR:-$root}
ESP8266_ARDUINO_BUILDER=${ESP8266_ARDUINO_BUILDER:-arduino}
ESP8266_ARDUINO_PRESERVE_CACHE=${ESP8266_ARDUINO_PRESERVE_CACHE:-}

# sketch build options
ESP8266_ARDUINO_DEBUG=${ESP8266_ARDUINO_DEBUG:-nodebug}
ESP8266_ARDUINO_LWIP=${ESP8266_ARDUINO_LWIP:-default}
ESP8266_ARDUINO_SKETCHES=${ESP8266_ARDUINO_SKETCHES:-}

ESP8266_ARDUINO_CLI=${ESP8266_ARDUINO_CLI:-$HOME/.local/bin/arduino-cli}

# ref. https://arduino.github.io/arduino-cli/1.2/configuration/#default-directories
case "${RUNNER_OS:-Linux}" in
"Linux")
    ESP8266_ARDUINO_HARDWARE=${ESP8266_ARDUINO_HARDWARE:-$HOME/Arduino/hardware}
    ESP8266_ARDUINO_LIBRARIES=${ESP8266_ARDUINO_LIBRARIES:-$HOME/Arduino/libraries}
    ;;
"macOS")
    ESP8266_ARDUINO_HARDWARE=${ESP8266_ARDUINO_HARDWARE:-$HOME/Documents/Arduino/hardware}
    ESP8266_ARDUINO_LIBRARIES=${ESP8266_ARDUINO_LIBRARIES:-$HOME/Documents/Arduino/libraries}
    ;;
"Windows")
    ESP8266_ARDUINO_HARDWARE=${ESP8266_ARDUINO_HARDWARE:-$HOME/Documents/Arduino/hardware}
    ESP8266_ARDUINO_LIBRARIES=${ESP8266_ARDUINO_LIBRARIES:-$HOME/Documents/Arduino/libraries}
    ;;
*)
    echo 'Unknown ${RUNNER_OS} = "' ${RUNNER_OS} '"'
    exit 2
esac

source "$root/tests/lib-skip-ino.sh"
source "$root/tests/common.sh"

cmd=${0##*/}
usage="
ENVIRONMENT:
  ESP8266_ARDUINO_SKETCHES - list of .ino files; defaults to **all available examples**
  ESP8266_ARDUINO_BUILDER - arduino or platformio

  For Arduino CLI:
    ESP8266_ARDUINO_HARDWARE - path to the hardware directory (usually, containing our repo)
    ESP8266_ARDUINO_LIBRATIES - path to the libraries directory (external dependencies)
    ESP8266_ARDUINO_DEBUG - debug or nodebug
    ESP8266_ARDUINO_LWIP - v4 or v6

USAGE:
  $cmd <[even | odd]> - build every Nth, when '<N> % 2' is either even or odd
  $cmd <mod> <rem> <[cnt]> - build every Nth, when '<N> % <mod>' is equal to 'rem'
                             optionally, set <cnt> to start with the Nth sketch
  $cmd - build every .ino file from ESP8266_ARDUINO_SKETCHES
"

mod=1
rem=0
cnt=0

if [ "$#" -eq 1 ] ; then
    case "$1" in
    "-h")
        echo "$usage"
        exit 0
        ;;
    "even")
        mod=2
        rem=0
        ;;
    "odd")
        mod=2
        rem=1
        ;;
    *)
        echo 'Can either be even or odd'
        exit 1
        ;;
    esac
elif [ "$#" -eq 2 ] ; then
    mod=$1
    rem=$2
elif [ "$#" -eq 3 ] ; then
    mod=$1
    rem=$2
    cnt=$3
elif [ "$#" -gt 2 ] ; then
    echo "$usage"
    exit 1
fi

if [ -z "$ESP8266_ARDUINO_SKETCHES" ] ; then
    ESP8266_ARDUINO_SKETCHES=$(find $root/libraries -name *.ino | sort)
fi

case "$ESP8266_ARDUINO_BUILDER" in
"arduino")
    install_arduino "$ESP8266_ARDUINO_DEBUG"
    build_sketches_with_arduino "$ESP8266_ARDUINO_LWIP" "$mod" "$rem" "$cnt"
    ;;
"platformio")
    install_platformio nodemcuv2
    build_sketches_with_platformio "$mod" "$rem" "$cnt"
    ;;
"print")
    print_sketch_info "$mod" "$rem"
    ;;
*)
    echo "Unknown builder! Must be one of - arduino, platformio or print"
    exit 1
    ;;
esac
