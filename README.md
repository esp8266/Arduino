Arduino core for ESP8266 WiFi chip
===========================================

This project brings support for ESP8266 chip to the Arduino environment. It lets you write sketches using familiar Arduino functions and libraries, and run them directly on ESP8266, no external microcontroller required.

ESP8266 Arduino core comes with libraries to communicate over WiFi using TCP and UDP, set up HTTP, mDNS, SSDP, and DNS servers, do OTA updates, use a file system in flash memory, work with SD cards, servos, SPI and I2C peripherals.

# Contents
- Installing options:
  - [Using Boards Manager](#installing-with-boards-manager)
  - [Using git version](#using-git-version)
  - [Using PlatformIO](#using-platformio)
  - [Building with make](#building-with-make)
- [Documentation](#documentation)
- [Issues and support](#issues-and-support)
- [Contributing](#contributing)  
- [License and credits](#license-and-credits)   

### Installing with Boards Manager ###

Starting with 1.6.4, Arduino allows installation of third-party platform packages using Boards Manager. We have packages available for Windows, Mac OS, and Linux (32 and 64 bit).

- Install Arduino 1.6.8 from the [Arduino website](http://www.arduino.cc/en/main/software).
- Start Arduino and open Preferences window.
- Enter ```http://arduino.esp8266.com/stable/package_esp8266com_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *esp8266* platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).

The best place to ask questions related to this core is ESP8266 community forum: http://www.esp8266.com/arduino.
If you find this forum or the ESP8266 Boards Manager package useful, please consider supporting it with a donation. <br />
[![Donate](https://img.shields.io/badge/paypal-donate-yellow.svg)](https://www.paypal.com/webscr?cmd=_s-xclick&hosted_button_id=4M56YCWV6PX66)

#### Available versions

##### Stable version ![](http://arduino.esp8266.com/stable/badge.svg)
Boards manager link: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`

Documentation: [http://esp8266.github.io/Arduino/versions/2.3.0/](http://esp8266.github.io/Arduino/versions/2.3.0/)

##### Staging version ![](http://arduino.esp8266.com/staging/badge.svg)
Boards manager link: `http://arduino.esp8266.com/staging/package_esp8266com_index.json`

Documentation: [http://esp8266.github.io/Arduino/versions/2.3.0-rc2/](http://esp8266.github.io/Arduino/versions/2.3.0-rc2/)

### Using git version
[![Linux build status](https://travis-ci.org/esp8266/Arduino.svg)](https://travis-ci.org/esp8266/Arduino) [![codecov.io](https://codecov.io/github/esp8266/Arduino/coverage.svg?branch=master)](https://codecov.io/github/esp8266/Arduino?branch=master)

- Install Arduino 1.6.8
- Go to Arduino directory
- Clone this repository into hardware/esp8266com/esp8266 directory (or clone it elsewhere and create a symlink)
```bash
cd hardware
mkdir esp8266com
cd esp8266com
git clone https://github.com/esp8266/Arduino.git esp8266
```
- Download binary tools (you need Python 2.7)
```bash
cd esp8266/tools
python get.py
```
- Restart Arduino

### Using PlatformIO

[PlatformIO](http://platformio.org) is an open source ecosystem for IoT
development with cross platform build system, library manager and full support
for Espressif (ESP8266) development. It works on the popular host OS: Mac OS X, Windows,
Linux 32/64, Linux ARM (like Raspberry Pi, BeagleBone, CubieBoard).

- [What is PlatformIO?](http://docs.platformio.org/en/stable/what-is-platformio.html)
- [PlatformIO IDE](http://platformio.org/platformio-ide)
- Quick Start with [PlatformIO IDE](http://docs.platformio.org/en/stable/ide/atom.html#quick-start) or [PlatformIO CLI](http://docs.platformio.org/en/stable/quickstart.html)
- [Advanced using](http://docs.platformio.org/en/stable/platforms/espressif.html) -
  custom settings, uploading to SPIFFS, Over-the-Air (OTA) or using stage version
- [Integration with other IDE](http://docs.platformio.org/en/stable/ide.html) -
  Atom, CLion, Eclipse, Emacs, NetBeans, Qt Creator, Sublime Text, VIM and Visual Studio
- [Project Examples](http://docs.platformio.org/en/stable/platforms/espressif.html#examples)

### Building with make

[makeEspArduino](https://github.com/plerup/makeEspArduino) is a generic makefile for any ESP8266 Arduino project.
Using make instead of the Arduino IDE makes it easier to do automated and production builds.

### Documentation

Documentation for latest development version:

- [Reference](doc/reference.md)
- [Libraries](doc/libraries.md)
- [File system](doc/filesystem.md)
- [OTA update](doc/ota_updates/readme.md)
- [Supported boards](doc/boards.md)
- [FAQ / Trubleshooting](doc/faq/readme.md)
- [Change log](doc/changes.md)

### Issues and support ###

If you encounter an issue, you are welcome to submit it here on Github: https://github.com/esp8266/Arduino/issues.
Please provide as much context as possible: version which you are using (you can check it in Boards Manager), your sketch code, serial output, board model, IDE settings (board selection, flash size, etc).

If you can not find the answers above, you can also try [ESP8266 Community Forum](http://www.esp8266.com/arduino)

### Contributing

For minor fixes of code and documentation, go ahead and submit a pull request.

Check out the list of issues which are easy to fix — [easy issues for 2.2.0](https://github.com/esp8266/Arduino/issues?q=is%3Aopen+is%3Aissue+milestone%3A2.2.0+label%3A%22level%3A+easy%22). Working on them is a great way to move the project forward.

Larger changes (rewriting parts of existing code from scratch, adding new functions to the core, adding new libraries) should generally be discussed [in the chat](https://gitter.im/esp8266/Arduino) first.

Feature branches with lots of small commits (especially titled "oops", "fix typo", "forgot to add file", etc.) should be squashed before opening a pull request. At the same time, please refrain from putting multiple unrelated changes into a single pull request.

### License and credits ###

Arduino IDE is developed and maintained by the Arduino team. The IDE is licensed under GPL.

ESP8266 core includes an xtensa gcc toolchain, which is also under GPL.

Esptool written by Christian Klippel is licensed under GPLv2, currently maintained by Ivan Grokhotkov: https://github.com/igrr/esptool-ck.

Espressif SDK included in this build is under Espressif MIT License.

ESP8266 core files are licensed under LGPL.

[SPI Flash File System (SPIFFS)](https://github.com/pellepl/spiffs) written by Peter Andersson is used in this project. It is distributed under MIT license.

[umm_malloc](https://github.com/rhempel/umm_malloc) memory management library written by Ralph Hempel is used in this project. It is distributed under MIT license.
