Arduino-compatible IDE with ESP8266 support
===========================================

[![Linux build status](http://img.shields.io/travis/igrr/Arduino.svg)](https://travis-ci.org/igrr/Arduino)
[![Donate](http://img.shields.io/paypal/donate.png?color=yellow)](https://www.paypal.com/webscr?cmd=_s-xclick&hosted_button_id=4M56YCWV6PX66)

This project brings support for ESP8266 chip to the Arduino environment. ESP8266WiFi library bundled with this project has the same interface as the WiFi Shield library, making it easy to re-use existing code and libraries.

### Installing with Boards Manager ###

Starting with 1.6.4, Arduino allows installation of third-party platform packages using Boards Manager. We have packages available for Windows, Mac OS, and Linux (32 and 64 bit).

- Install Arduino 1.6.4 from the [Arduino website](http://www.arduino.cc/en/main/software).
- Start Arduino and open Perferences window.
- Enter ```http://arduino.esp8266.com/package_esp8266com_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *esp8266* platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).

### Building latest version from source ###
```
$ git clone https://github.com/esp8266/Arduino.git
$ cd Arduino/build
$ ant dist
```

### Documentation ###
Latest development version: [Reference](hardware/esp8266com/esp8266/doc/README.md)

### Supported boards ###
- Generic esp8266 modules (without auto-reset support)
- NodeMCU 0.9
- NodeMCU 1.0
- Olimex MOD-WIFI-ESP8266
- Adafruit HUZZAH ESP8266 (ESP-12)
- [ST7735](https://github.com/nzmichaelh/Adafruit-ST7735-Library) - Adafruit's ST7735 library modified to be compatible with esp8266.  Just make sure to modify the pins in the examples as they are still AVR specific.

### Issues and support ###

Forum: http://www.esp8266.com/arduino

Submit issues on Github: https://github.com/esp8266/Arduino/issues

### License and credits ###

Arduino IDE is based on Wiring and Processing. It is developed and maintained by the Arduino team. The IDE is licensed under GPL.

ESP8266 core includes an xtensa gcc toolchain, which is also under GPL.

Esptool written by Christian Klippel is licensed under GPLv2, currently maintained by Ivan Grokhotkov: https://github.com/igrr/esptool-ck.

Espressif SDK included in this build is under Espressif MIT License.

ESP8266 core files are licensed under LGPL.

[SPI Flash File System (SPIFFS)](https://github.com/pellepl/spiffs) written by Peter Andersson is used in this project. It is distributed under MIT license.
