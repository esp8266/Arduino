---
title: Installation
---

## Boards Manager ##

This is the suggested installation method for end users.

### Prerequisites
- Arduino 1.6.8, get it from [Arduino website](https://www.arduino.cc/en/Main/OldSoftwareReleases#previous).
- Internet connection

### Instructions
- Start Arduino and open Preferences window.
- Enter ```http://arduino.esp8266.com/stable/package_esp8266com_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and find *esp8266* platform.
- Select the version you need from a drop-down box.
- Click *install* button.
- Don't forget to select your ESP8266 board from Tools > Board menu after installation.

You may optionally use *staging* boards manager package link:
`http://arduino.esp8266.com/staging/package_esp8266com_index.json`. This may contain some new features, but at the same time, some things might be broken.

## Using git version

This is the suggested installation method for contributors and library developers.

### Prerequisites

- Arduino 1.6.8 (or newer, if you know what you are doing)
- git
- python 2.7
- terminal, console, or command prompt (depending on you OS)
- Internet connection

### Instructions

- Open the console and go to Arduino directory. This can be either your *sketchbook* directory (usually `<Documents>` or `<Documents>/Arduino`), or the directory of Arduino application itself, the choice is up to you. For Windows 10 the directory will typically be:  `C:\Users\{username}\Documents\hardware\`

- Note that although the Arduino installer will use (for Windows)  
`C:\Users\{username}\AppData\Local\Arduino15\packages\esp8266\hardware` this is not the recommended directory for the git version. (and it is probably best not to install both)

- Clone this repository into hardware/esp8266com/esp8266 directory. Alternatively, clone it elsewhere and create a symlink, if your OS supports them.

    ```bash
    cd hardware
    mkdir esp8266com
    cd esp8266com
    git clone https://github.com/esp8266/Arduino.git esp8266
    ```
    You should end up with the following directory structure:

    ```bash
    Arduino
    |
    --- hardware
        |
        --- esp8266com
            |
            --- esp8266
                |
                --- bootloaders
                --- cores
                --- doc
                --- libraries
                --- package
                --- tests
                --- tools
                --- variants
                --- platform.txt
                --- programmers.txt
                --- README.md
                --- boards.txt
                --- LICENSE
    ```

- Download binary tools

    ```bash
    cd esp8266/tools
    python get.py
    ```

- Restart Arduino
