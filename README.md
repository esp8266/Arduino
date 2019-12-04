Arduino core for ESP8266 WiFi chip
===========================================

# Quick links

- [Latest release documentation](https://arduino-esp8266.readthedocs.io/en/2.6.2/)
- [Current "git version" documentation](https://arduino-esp8266.readthedocs.io/en/latest/)
- [Install git version](https://arduino-esp8266.readthedocs.io/en/latest/installing.html#using-git-version) ([sources](doc/installing.rst#using-git-version))

# Arduino on ESP8266

This project brings support for the ESP8266 chip to the Arduino environment. It lets you write sketches, using familiar Arduino functions and libraries, and run them directly on ESP8266, with no external microcontroller required.

ESP8266 Arduino core comes with libraries to communicate over WiFi using TCP and UDP, set up HTTP, mDNS, SSDP, and DNS servers, do OTA updates, use a file system in flash memory, and work with SD cards, servos, SPI and I2C peripherals.

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

### Installing with Boards Manager

Starting with 1.6.4, Arduino allows installation of third-party platform packages using Boards Manager. We have packages available for Windows, Mac OS, and Linux (32 and 64 bit).

- Install the current upstream Arduino IDE at the 1.8.7 level or later. The current version is on the [Arduino website](https://www.arduino.cc/en/main/software).
- Start Arduino and open the Preferences window.
- Enter ```https://arduino.esp8266.com/stable/package_esp8266com_index.json``` into the *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *esp8266* platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).

#### Latest release [![Latest release](https://img.shields.io/github/release/esp8266/Arduino.svg)](https://github.com/esp8266/Arduino/releases/latest/)
Boards manager link: `https://arduino.esp8266.com/stable/package_esp8266com_index.json`

Documentation: [https://arduino-esp8266.readthedocs.io/en/2.6.2/](https://arduino-esp8266.readthedocs.io/en/2.6.2/)

### Using git version
[![Linux build status](https://travis-ci.org/esp8266/Arduino.svg)](https://travis-ci.org/esp8266/Arduino)

Also known as latest git or master branch.

- Install the current upstream Arduino IDE at the 1.8 level or later. The current version is on the [Arduino website](https://www.arduino.cc/en/main/software).
- Follow the [instructions in the documentation](https://arduino-esp8266.readthedocs.io/en/latest/installing.html#using-git-version).

### Using PlatformIO

[PlatformIO](https://platformio.org?utm_source=github&utm_medium=arduino-esp8266) is an open source ecosystem for IoT
development with a cross-platform build system, a library manager, and full support
for Espressif (ESP8266) development. It works on the following popular host operating systems: macOS, Windows,
Linux 32/64, and Linux ARM (like Raspberry Pi, BeagleBone, CubieBoard).

- [What is PlatformIO?](https://docs.platformio.org/en/latest/what-is-platformio.html?utm_source=github&utm_medium=arduino-esp8266)
- [PlatformIO IDE](https://platformio.org/platformio-ide?utm_source=github&utm_medium=arduino-esp8266)
- [PlatformIO Core](https://docs.platformio.org/en/latest/core.html?utm_source=github&utm_medium=arduino-esp8266) (command line tool)
- [Advanced usage](https://docs.platformio.org/en/latest/platforms/espressif8266.html?utm_source=github&utm_medium=arduino-esp8266) -
  custom settings, uploading to SPIFFS, Over-the-Air (OTA), staging version
- [Integration with Cloud and Standalone IDEs](https://docs.platformio.org/en/latest/ide.html?utm_source=github&utm_medium=arduino-esp8266) -
  Cloud9, Codeanywhere, Eclipse Che (Codenvy), Atom, CLion, Eclipse, Emacs, NetBeans, Qt Creator, Sublime Text, VIM, Visual Studio, and VSCode
- [Project Examples](https://docs.platformio.org/en/latest/platforms/espressif8266.html?utm_source=github&utm_medium=arduino-esp8266#examples)

### Building with make

[makeEspArduino](https://github.com/plerup/makeEspArduino) is a generic makefile for any ESP8266 Arduino project.
Using make instead of the Arduino IDE makes it easier to do automated and production builds.

### Documentation

Documentation for latest development version: https://arduino-esp8266.readthedocs.io/en/latest/

### Issues and support ###

[ESP8266 Community Forum](https://www.esp8266.com/u/arduinoanswers) is a well-established community for questions and answers about Arduino for ESP8266. If you need help, have a "How do I..." type question, have a problem with a 3rd party library not hosted in this repo, or just want to discuss how to approach a problem, please ask there.

If you find the forum useful, please consider supporting it with a donation. <br />
[![Donate](https://img.shields.io/badge/paypal-donate-yellow.svg)](https://www.paypal.com/webscr?cmd=_s-xclick&hosted_button_id=4M56YCWV6PX66)

If you encounter an issue which you think is a bug in the ESP8266 Arduino Core or the associated libraries, or if you want to propose an enhancement, you are welcome to submit it here on Github: https://github.com/esp8266/Arduino/issues.

Please provide as much context as possible, as well as the information requested in the issue template:

- ESP8266 Arduino core version which you are using (you can check it in Boards Manager)
- your sketch code; please wrap it into a code block, see [Github markdown manual](https://help.github.com/articles/basic-writing-and-formatting-syntax/#quoting-code)
- when encountering an issue that happens at run time, attach the serial output. Wrap it into a code block, just like the code.
- for issues that happen at compile time, enable verbose compiler output in the IDE preferences, and attach that output (also inside a code block)
- ESP8266 development board model
- IDE settings (board choice, flash size)
- etc

### Contributing

For minor fixes of code and documentation, please go ahead and submit a pull request.  A gentle introduction to the process can be found [here](https://www.freecodecamp.org/news/a-simple-git-guide-and-cheat-sheet-for-open-source-contributors/).

Check out the list of issues that are easy to fix — [easy issues pending](https://github.com/esp8266/Arduino/issues?q=is%3Aopen+is%3Aissue+label%3A%22level%3A+easy%22). Working on them is a great way to move the project forward.

Larger changes (rewriting parts of existing code from scratch, adding new functions to the core, adding new libraries) should generally be discussed by opening an issue first.

Feature branches with lots of small commits (especially titled "oops", "fix typo", "forgot to add file", etc.) should be squashed before opening a pull request. At the same time, please refrain from putting multiple unrelated changes into a single pull request.

### License and credits ###

Arduino IDE is developed and maintained by the Arduino team. The IDE is licensed under GPL.

ESP8266 core includes an xtensa gcc toolchain, which is also under GPL.

Esptool written by Christian Klippel is licensed under GPLv2, currently maintained by Ivan Grokhotkov: https://github.com/igrr/esptool-ck.

Espressif SDK included in this build is under Espressif MIT License.

ESP8266 core files are licensed under LGPL.

[SPI Flash File System (SPIFFS)](https://github.com/pellepl/spiffs) written by Peter Andersson is used in this project. It is distributed under the MIT license.

[umm_malloc](https://github.com/rhempel/umm_malloc) memory management library written by Ralph Hempel is used in this project. It is distributed under the MIT license.

[SoftwareSerial](https://github.com/plerup/espsoftwareserial) library and examples written by Peter Lerup. Distributed under LGPL 2.1.

[axTLS](http://axtls.sourceforge.net/) library written by Cameron Rich, built from https://github.com/igrr/axtls-8266, is used in this project. It is distributed under [BSD license](https://github.com/igrr/axtls-8266/blob/master/LICENSE).

[BearSSL](https://bearssl.org) library written by Thomas Pornin, built from https://github.com/earlephilhower/bearssl-esp8266, is used in this project.  It is distributed under the [MIT License](https://bearssl.org/#legal-details).

[LittleFS](https://github.com/ARMmbed/littlefs) library written by ARM Limited and released under the [BSD 3-clause license](https://github.com/ARMmbed/littlefs/blob/master/LICENSE.md).

### Other useful links ###

[Toolchain repo](https://github.com/earlephilhower/esp-quick-toolchain)

[Lwip link layer repo](https://github.com/d-a-v/esp82xx-nonos-linklayer)

[SoftwareSerial repo](https://github.com/plerup/espsoftwareserial)

[Serial Monitor Arduino IDE plugin](https://github.com/mytrain/arduino-esp8266-serial-plugin) Original discussion [here](https://github.com/esp8266/Arduino/issues/1360), quick download [there](http://mytrain.fr/cms//images/mytrain/private/ESP8266SM.v3.zip).
