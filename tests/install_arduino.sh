#!/usr/bin/env bash

root=$(git rev-parse --show-toplevel)
source "$root/tests/env.sh"

install_arduino "$ESP8266_ARDUINO_DEBUG"
