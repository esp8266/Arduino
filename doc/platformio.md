## Using PlatformIO

- Install [PlatformIO](http://platformio.org)
- Initialise new project

```bash
#
# Create empty directory
#
mkdir myproject
cd myproject

#
# Find type of supported boards
#
platformio boards espressif

# Platform: espressif
# --------------------------------------------------------------------------------------------------------
# Type                  MCU            Frequency  Flash   RAM    Name
# --------------------------------------------------------------------------------------------------------
# huzzah                esp8266        80Mhz     4096Kb  80Kb   Adafruit HUZZAH ESP8266
# espino                esp8266        80Mhz     4096Kb  80Kb   ESPino
# esp12e                esp8266        80Mhz     4096Kb  80Kb   Espressif ESP8266 ESP-12E
# esp01_1m              esp8266        80Mhz     1024Kb  80Kb   Espressif Generic ESP8266 ESP-01 1M
# esp01                 esp8266        80Mhz     512Kb   80Kb   Espressif Generic ESP8266 ESP-01 512k
# nodemcu               esp8266        80Mhz     4096Kb  80Kb   NodeMCU 0.9 & 1.0
# modwifi               esp8266        80Mhz     2048Kb  80Kb   Olimex MOD-WIFI-ESP8266(-DEV)
# thing                 esp8266        80Mhz     512Kb   80Kb   SparkFun ESP8266 Thing
# esp210                esp8266        80Mhz     4096Kb  80Kb   SweetPea ESP-210
# d1                    esp8266        80Mhz     4096Kb  80Kb   WeMos D1
# d1_mini               esp8266        80Mhz     4096Kb  80Kb   WeMos D1 mini
# ...
#
# The rest of Board/MCU variations with the different Flash/SPIFFS can be customized using extra LD Script.
# PlatformIO contains the popular LD scripts.
# See http://docs.platformio.org/en/latest/platforms/espressif.html#custom-flash-size
#

#
# Initialise base project
#
platformio init --board %TYPE%(see above)
# for example, initialise project for Espressif Generic ESP8266 ESP-01
platformio init --board esp01

# The next files/directories will be created in myproject
# platformio.ini - Project Configuration File. |-> PLEASE EDIT ME <-|
# src - Put your source files here
# lib - Put here project specific (private) libraries
# Do you want to continue? [y/N]: Y
```

- Place your source code to `src` directory
- Build/Upload project

```bash
# process/build project
platformio run

# build+upload firmware
platformio run --target upload
```

## Advanced documentation

- [OTA update](http://docs.platformio.org/en/latest/platforms/espressif.html#ota-update)
  * [Authentication and upload options](http://docs.platformio.org/en/latest/platforms/espressif.html#authentication-and-upload-options)
- [Custom CPU Frequency and Upload Speed](http://docs.platformio.org/en/latest/platforms/espressif.html#custom-cpu-frequency-and-upload-speed)
- [Custom Flash Size](http://docs.platformio.org/en/latest/platforms/espressif.html#custom-flash-size)
- [IDE Integration](http://docs.platformio.org/en/latest/ide.html) (Atom, CLion, Eclipse, Qt Creator, Sublime Text, VIM, Visual Studio)
- [Project Examples](http://docs.platformio.org/en/latest/platforms/espressif.html#examples)

## Demo of Over-the-Air (OTA) ESP8266 programming using PlatformIO

http://www.penninkhof.com/2015/12/1610-over-the-air-esp8266-programming-using-platformio/

[![Over-the-Air ESP8266 programming using PlatformIO](http://img.youtube.com/vi/lXchL3hpDO4/0.jpg)](http://www.youtube.com/watch?v=lXchL3hpDO4 "Over-the-Air ESP8266 programming using PlatformIO")
