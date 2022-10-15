#!/usr/bin/env bash

root=$(git rev-parse --show-toplevel)

ESP8266_ARDUINO_BUILD_DIR=${ESP8266_ARDUINO_BUILD_DIR:-$root}
ESP8266_ARDUINO_BUILDER=${ESP8266_ARDUINO_BUILDER:-arduino}
ESP8266_ARDUINO_PRESERVE_CACHE=${ESP8266_ARDUINO_PRESERVE_CACHE:-}

ESP8266_ARDUINO_IDE=${ESP8266_ARDUINO_IDE:-$HOME/arduino_ide}
ESP8266_ARDUINO_HARDWARE=${ESP8266_ARDUINO_HARDWARE:-$HOME/Arduino/hardware}
ESP8266_ARDUINO_LIBRARIES=${ESP8266_ARDUINO_LIBRARIES:-$HOME/Arduino/libraries}

ESP8266_ARDUINO_DEBUG=${ESP8266_ARDUINO_DEBUG:-nodebug}
ESP8266_ARDUINO_LWIP=${ESP8266_ARDUINO_LWIP:-default}
ESP8266_ARDUINO_SKETCHES=${ESP8266_ARDUINO_SKETCHES:-}

source "$root/tests/common.sh"

cmd=${0##*/}
usage="
ENVIRONMENT:
  ESP8266_ARDUINO_SKETCHES - list of .ino files; defaults to **all available examples**
  ESP8266_ARDUINO_BUILDER - arduino or platformio

  For Arduino IDE:
    ESP8266_ARDUINO_IDE - path to the IDE (portable)
    ESP8266_ARDUINO_HARDWARE - path to the hardware directory (usually, containing our repo)
    ESP8266_ARDUINO_LIBRATIES - path to the libraries directory (external dependencies)
    ESP8266_ARDUINO_DEBUG - debug or nodebug
    ESP8266_ARDUINO_LWIP - v4 or v6

USAGE:
  $cmd <[even | odd]> - build every Nth, when '<N> % 2' is either even or odd
  $cmd <mod> <rem> - build every Nth, when '<N> % <mod>' is equal to 'rem'
  $cmd - build every .ino file from ESP8266_ARDUINO_SKETCHES
"

mod=1
rem=0

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
    build_sketches_with_arduino "$mod" "$rem" "$ESP8266_ARDUINO_LWIP"
    ;;
"platformio")
    install_platformio nodemcuv2
    build_sketches_with_platformio "$mod" "$rem"
    ;;
*)
    echo "Unknown builder! Must be either arduino or platformio"
    exit 1
    ;;
esac
