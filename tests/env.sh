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
    RUNNER_OS="Linux"
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

mod=1
rem=0
cnt=0
