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
# esp01                 esp8266        80Mhz     512Kb   32Kb   Espressif ESP8266 ESP-01 board
# esp01_1m              esp8266        80Mhz     1024Kb  32Kb   Espressif ESP8266 ESP-01-1MB board
# esp12e                esp8266        80Mhz     4096Kb  32Kb   Espressif ESP8266 ESP-12E board (NodeMCU)
# ...

#
# Initialise base project
#
platformio init --board %TYPE%(see above)
# for example, initialise project for ESP8266 ESP-12E board (NodeMCU)
platformio init --board esp12e

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

## OTA firmware uploading

There are 2 options:

- Directly specify `--upoad-port` in command line
```bash
platformio run --target upload --upload-port IP_ADDRESS_HERE
```
- Specify [upload_port](http://docs.platformio.org/en/latest/projectconf.html#upload-port) option in `platformio.ini`
```ini
[env:***]
...
upload_port = IP_ADDRESS_HERE
```

### Authentication and upload options

You can pass additional options/flags to OTA uploader using [upload_flags](http://docs.platformio.org/en/latest/projectconf.html#upload-flags)  option in `platformio.ini`
```ini
[env:***]
upload_flags = --port=8266
```

Availalbe flags
- `--port=ESP_PORT` ESP8266 ota Port. Default 8266
- `--auth=AUTH` Set authentication password
- `--spiffs` Use this option to transmit a SPIFFS image and do not flash the module

For the full list with availalbe options please run this command `~/.platformio/packages/framework-arduinoespressif/tools/espota.py -h`.


## IDE Integration
In addition, PlatformIO [can be integrated into the popular IDEs](http://docs.platformio.org/en/latest/ide.html). For example, initialise project for Espressif ESP8266 ESP-01 board and Eclipse IDE
```
platformio init --board esp01 --ide eclipse
```
Then [import project](http://docs.platformio.org/en/latest/ide/eclipse.html) using `Eclipse Menu: File > Import... > General > Existing Projects into Workspace`.

## Demo of OTA firmware uploading
[![PlatformIO and OTA firmware uploading to Espressif ESP8266 ESP-01](http://img.youtube.com/vi/W8wWjvQ8ZQs/0.jpg)](http://www.youtube.com/watch?v=W8wWjvQ8ZQs "PlatformIO and OTA firmware uploading to Espressif ESP8266 ESP-01")
