#!/usr/bin/env python3

# boards.txt python builder for esp8266/Arduino
# Copyright (C) 2017 community
# Permission is hereby granted, free of charge, to any person who buy it,
# use it, break it, fix it, trash it, change it, mail - upgrade it, charge
# it, point it, zoom it, press it, snap it, work it, quick - erase it, write
# it, cut it, paste it, save it, load it, check it, quick - rewrite it, plug
# it, play it, burn it, rip it, drag and drop it, zip - unzip it, lock it,
# fill it, call it, find it, view it, code it, jam - unlock it, surf it,
# scroll it, pause it, click it, cross it, crack it, switch - update it,
# name it, rate it, tune it, print it, scan it, send it, fax - rename it,
# touch it, bring it, pay it, watch it, turn it, leave it, start - format
# it.

# diff ldscripts after regeneration:
# (cd tools/sdk/ld/backup/; for i in *; do diff -u $i ../$i|less; done)

# board descriptor:
#    name     display name
#    opts:    specific entries dicts (overrides same entry in macros)
#    macro:   common entries
#        unmodifiable parameters:
#            resetmethod_ck/_nodemcu/_none/_dtrset: fixed reset method
#            flashmode_qio/_dio/_qout/_dout:        fixed flash mode
#            flashfreq_40/_80:                      fixed flash frequency
#        selection menu:
#            resetmethod_menu            menus for reset method
#            resetmethod_menu_extra      menus for additional reset methods
#            crystalfreq/flashfreq_menu: menus for crystal/flash frequency selection
#            flashmode_menu:             menus for flashmode selection (dio/dout/qio/qout)
#            512K/1M/2M/4M/8M/16M:       menus for flash & FS size
#            lwip                        menus for available lwip versions

import argparse
import doctest
import os
import sys
import collections
import operator
import itertools
import json
import contextlib

################################################################
# serial upload speed order in menu, in kibibauds
# default is 115 for every board
# (unless, "serial" field from BOARDS below overrides it)

SERIAL_SPEEDS = collections.OrderedDict([
    (  '57',  ( 's57',  's115', 's230', 's256', 's460', 's512', 's921', 's3000' )),
    ( '115',  ( 's115', 's57',  's230', 's256', 's460', 's512', 's921', 's3000' )),
    ( '230',  ( 's230', 's57',  's115', 's256', 's460', 's512', 's921', 's3000' )),
    ( '256',  ( 's256', 's57',  's115', 's230', 's460', 's512', 's921', 's3000' )),
    ( '460',  ( 's460', 's57',  's115', 's230', 's256', 's512', 's921', 's3000' )),
    ( '512',  ( 's512', 's57',  's115', 's230', 's256', 's460', 's921', 's3000' )),
    ( '921',  ( 's921', 's57',  's115', 's230', 's256', 's460', 's512', 's3000' )),
    ( '3000', ( 's3000','s57',  's115', 's230', 's256', 's460', 's512', 's921'  )),
])

DEFAULT_SERIAL_SPEED = '115'

################################################################
# boards list

BOARDS = collections.OrderedDict([
    ( 'generic', {
        'name': 'Generic ESP8266 Module',
        'opts': {
            '.build.board': 'ESP8266_GENERIC',
            },
        'macro': [
            'resetmethod_menu',
            'resetmethod_menu_extra',
            'crystalfreq_menu',
            'flashfreq_menu',
            'flashmode_menu',
            '1M', '2M', '4M', '8M', '16M', '512K',
            'led',
            'sdk',
            ],
        'desc': [ 'These modules come in different form factors and pinouts. See the page at ESP8266 community wiki for more info: `ESP8266 Module Family <http://www.esp8266.com/wiki/doku.php?id=esp8266-module-family>`__.',
                  '',
                  'Usually these modules have no bootstrapping resistors on board, insufficient decoupling capacitors, no voltage regulator, no reset circuit, and no USB-serial adapter. This makes using them somewhat tricky, compared to development boards which add these features.',
                  '',
                  'In order to use these modules, make sure to observe the following:',
                  '',
                  '-  **Provide sufficient power to the module.** For stable use of the ESP8266 a power supply with 3.3V and >= 250mA is required. Using the power available from USB to Serial adapter is not recommended, these adapters typically do not supply enough current to run ESP8266 reliably in every situation. An external supply or regulator alongwith filtering capacitors is preferred.',
                  '',
                  '-  **Connect bootstrapping resistors** to GPIO0, GPIO2, GPIO15 according to the schematics below.',
                  '',
                  '-  **Put ESP8266 into bootloader mode** before uploading code.',
                  '',
                  'Serial Adapter',
                  '--------------',
                  '',
                  'There are many different USB to Serial adapters / boards. To be able to put ESP8266 into bootloader mode using serial handshaking lines, you need the adapter which breaks out RTS and DTR outputs. CTS and DSR are not useful for upload (they are inputs). Make sure the adapter can work with 3.3V IO voltage: it should have a jumper or a switch to select between 5V and 3.3V, or be marked as 3.3V only.',
                  '',
                  'Adapters based around the following ICs should work:',
                  '',
                  '-  FT232RL',
                  '-  CP2102',
                  '-  CH340G',
                  '',
                  'PL2303-based adapters are known not to work on Mac OS X. See https://github.com/igrr/esptool-ck/issues/9 for more info.',
                  '',
                  'Minimal Hardware Setup for Bootloading and Usage',
                  '------------------------------------------------',
                  '',
                  '+-----------------+------------+------------------+',
                  '| PIN             | Resistor   | Serial Adapter   |',
                  '+=================+============+==================+',
                  '| VCC             |            | VCC (3.3V)       |',
                  '+-----------------+------------+------------------+',
                  '| GND             |            | GND              |',
                  '+-----------------+------------+------------------+',
                  '| TX or GPIO2\*   |            | RX               |',
                  '+-----------------+------------+------------------+',
                  '| RX              |            | TX               |',
                  '+-----------------+------------+------------------+',
                  '| GPIO0           | PullUp     | DTR              |',
                  '+-----------------+------------+------------------+',
                  '| Reset\*         | PullUp     | RTS              |',
                  '+-----------------+------------+------------------+',
                  '| GPIO15\*        | PullDown   |                  |',
                  '+-----------------+------------+------------------+',
                  '| CH\_PD          | PullUp     |                  |',
                  '+-----------------+------------+------------------+',
                  '',
                  '-  Note',
                  '-  GPIO15 is also named MTDO',
                  '-  Reset is also named RSBT or REST (adding PullUp improves the',
                  '   stability of the module)',
                  '-  GPIO2 is alternative TX for the boot loader mode',
                  '-  **Directly connecting a pin to VCC or GND is not a substitute for a',
                  '   PullUp or PullDown resistor, doing this can break upload management',
                  '   and the serial console, instability has also been noted in some',
                  '   cases.**',
                  '',
                  'ESP to Serial',
                  '-------------',
                  '',
                  '.. figure:: ESP_to_serial.png',
                  '   :alt: ESP to Serial',
                  '',
                  '   ESP to Serial',
                  '',
                  'Minimal Hardware Setup for Bootloading only',
                  '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~',
                  '',
                  'ESPxx Hardware',
                  '',
                  '+---------------+------------+------------------+',
                  '| PIN           | Resistor   | Serial Adapter   |',
                  '+===============+============+==================+',
                  '| VCC           |            | VCC (3.3V)       |',
                  '+---------------+------------+------------------+',
                  '| GND           |            | GND              |',
                  '+---------------+------------+------------------+',
                  '| TX or GPIO2   |            | RX               |',
                  '+---------------+------------+------------------+',
                  '| RX            |            | TX               |',
                  '+---------------+------------+------------------+',
                  '| GPIO0         |            | GND              |',
                  '+---------------+------------+------------------+',
                  '| Reset         |            | RTS\*            |',
                  '+---------------+------------+------------------+',
                  '| GPIO15        | PullDown   |                  |',
                  '+---------------+------------+------------------+',
                  '| CH\_PD        | PullUp     |                  |',
                  '+---------------+------------+------------------+',
                  '',
                  '-  Note',
                  '-  if no RTS is used a manual power toggle is needed',
                  '',
                  'Minimal Hardware Setup for Running only',
                  '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~',
                  '',
                  'ESPxx Hardware',
                  '',
                  '+----------+------------+----------------+',
                  '| PIN      | Resistor   | Power supply   |',
                  '+==========+============+================+',
                  '| VCC      |            | VCC (3.3V)     |',
                  '+----------+------------+----------------+',
                  '| GND      |            | GND            |',
                  '+----------+------------+----------------+',
                  '| GPIO0    | PullUp     |                |',
                  '+----------+------------+----------------+',
                  '| GPIO15   | PullDown   |                |',
                  '+----------+------------+----------------+',
                  '| CH\_PD   | PullUp     |                |',
                  '+----------+------------+----------------+',
                  '',
                  'Minimal',
                  '-------',
                  '',
                  '.. figure:: ESP_min.png',
                  '   :alt: ESP min',
                  '',
                  '   ESP min',
                  '',
                  'Improved Stability',
                  '------------------',
                  '',
                  '.. figure:: ESP_improved_stability.png',
                  '   :alt: ESP improved stability',
                  '',
                  '   ESP improved stability',
                  '',
                  'Boot Messages and Modes',
                  '-----------------------',
                  '',
                  'The ESP module checks at every boot the Pins 0, 2 and 15. based on them its boots in different modes:',
                  '',
                  '+----------+---------+---------+------------------------------------+',
                  '| GPIO15   | GPIO0   | GPIO2   | Mode                               |',
                  '+==========+=========+=========+====================================+',
                  '| 0V       | 0V      | 3.3V    | Uart Bootloader                    |',
                  '+----------+---------+---------+------------------------------------+',
                  '| 0V       | 3.3V    | 3.3V    | Boot sketch (SPI flash)            |',
                  '+----------+---------+---------+------------------------------------+',
                  '| 3.3V     | x       | x       | SDIO mode (not used for Arduino)   |',
                  '+----------+---------+---------+------------------------------------+',
                  '',
                  'at startup the ESP prints out the current boot mode example:',
                  '',
                  '::',
                  '',
                  '    rst cause:2, boot mode:(3,6)',
                  '',
                  'note: - GPIO2 is used as TX output and the internal Pullup is enabled on boot.',
                  '',
                  'rst cause',
                  '~~~~~~~~~',
                  '',
                  '+----------+------------------+',
                  '| Number   | Description      |',
                  '+==========+==================+',
                  '| 0        | unknown          |',
                  '+----------+------------------+',
                  '| 1        | normal boot      |',
                  '+----------+------------------+',
                  '| 2        | reset pin        |',
                  '+----------+------------------+',
                  '| 3        | software reset   |',
                  '+----------+------------------+',
                  '| 4        | watchdog reset   |',
                  '+----------+------------------+',
                  '',
                  'boot mode',
                  '~~~~~~~~~',
                  '',
                  'the first value respects the pin setup of the Pins 0, 2 and 15.',
                  '',
                  '+----------+----------+---------+---------+-------------+',
                  '| Number   | GPIO15   | GPIO0   | GPIO2   | Mode        |',
                  '+==========+==========+=========+=========+=============+',
                  '| 0        | 0V       | 0V      | 0V      | Not valid   |',
                  '+----------+----------+---------+---------+-------------+',
                  '| 1        | 0V       | 0V      | 3.3V    | Uart        |',
                  '+----------+----------+---------+---------+-------------+',
                  '| 2        | 0V       | 3.3V    | 0V      | Not valid   |',
                  '+----------+----------+---------+---------+-------------+',
                  '| 3        | 0V       | 3.3V    | 3.3V    | Flash       |',
                  '+----------+----------+---------+---------+-------------+',
                  '| 4        | 3.3V     | 0V      | 0V      | SDIO        |',
                  '+----------+----------+---------+---------+-------------+',
                  '| 5        | 3.3V     | 0V      | 3.3V    | SDIO        |',
                  '+----------+----------+---------+---------+-------------+',
                  '| 6        | 3.3V     | 3.3V    | 0V      | SDIO        |',
                  '+----------+----------+---------+---------+-------------+',
                  '| 7        | 3.3V     | 3.3V    | 3.3V    | SDIO        |',
                  '+----------+----------+---------+---------+-------------+',
                  '',
                  'note: - number = ((GPIO15 << 2) \| (GPIO0 << 1) \| GPIO2);',
                  ],
    }),
    ( 'esp8285', {
        'name': 'Generic ESP8285 Module',
        'opts': {
            '.build.board': 'ESP8266_ESP01',
            '.build.variant': 'esp8285'
            },
        'macro': [
            'resetmethod_menu',
            'resetmethod_menu_extra',
            'crystalfreq_menu',
            'flashmode_dout',
            'flashfreq_40',
            '1M', '2M',
            'led',
            'sdk',
            ],
        'desc': [ 'ESP8285 (`datasheet <http://www.espressif.com/sites/default/files/0a-esp8285_datasheet_en_v1.0_20160422.pdf>`__) is a multi-chip package which contains ESP8266 and 1MB flash. All points related to bootstrapping resistors and recommended circuits listed above apply to ESP8285 as well.',
                  '',
                  'Note that since ESP8285 has SPI flash memory internally connected in DOUT mode, pins 9 and 10 may be used as GPIO / I2C / PWM pins.',
                  ],
    }),
    ( 'agruminolemon', {
        'name': 'Lifely Agrumino Lemon v4',
        'opts': collections.OrderedDict([
            ( '.build.board', 'ESP8266_AGRUMINO_LEMON_V4' ),
            ( '.build.variant', 'agruminolemonv4' ),
            ]),
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '2M',
            ],
        'desc': [ 'Procuct page https://www.lifely.cc',
                  '',
                  'This Board "Lifely Agrumino Lemon" is based with WT8266-S1 core with WiFi 2,4Ghz and 2MB of Flash.',
                  'Power',
                  'Micro usb power cable, Lir2450 rechargeable battery (or not rechargeable)or with JST connector in the back board Max 6 Vin',
                  'Libraries and examples',
                  'Download libraries from: Official Arduino Ide, our website https://www.lifely.cc or https://github.com/lifely-cc/',
                  'Full pinout and PDF for setup here https://www.lifely.cc our libraries is OpenSource',
                  ],
    }),
    ( 'espduino', {
        'name': 'ESPDuino (ESP-13 Module)',
        'opts': collections.OrderedDict([
            ( '.build.board', 'ESP8266_ESP13' ),
            ( '.build.variant', 'ESPDuino' ),
            ( '.menu.ResetMethod.v2', 'ESPduino-V2' ),
            ( '.menu.ResetMethod.v2.upload.resetmethod', '--before default_reset --after hard_reset' ),
            ( '.menu.ResetMethod.v1', 'ESPduino-V1' ),
            ( '.menu.ResetMethod.v1.upload.resetmethod', '--before no_reset --after soft_reset' ),
            ( '.menu.UploadTool.esptool', 'Serial' ),
            ( '.menu.UploadTool.esptool.upload.tool', 'esptool' ),
            ( '.menu.UploadTool.esptool.upload.verbose', '--trace' ),
            ( '.menu.UploadTool.espota', 'OTA' ),
            ( '.menu.UploadTool.espota.upload.tool', 'espota' ),
            ]),
        'macro': [
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ '*TODO*' ],
    }),
    ( 'huzzah', {
        'name': 'Adafruit Feather HUZZAH ESP8266',
        'opts': {
            '.build.board': 'ESP8266_ADAFRUIT_HUZZAH',
            '.build.variant': 'adafruit',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_qio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'The Adafruit Feather HUZZAH ESP8266 is an Arduino-compatible Wi-Fi development board powered by Ai-Thinker\'s ESP-12S, clocked at 80 MHz at 3.3V logic. A high-quality SiLabs CP2104 USB-Serial chip is included so that you can upload code at a blistering 921600 baud for fast development time. It also has auto-reset so no noodling with pins and reset button pressings. A 3.7V Lithium polymer battery connector is included, making it ideal for portable projects. The Adafruit Feather HUZZAH ESP8266 will automatically recharge a connected battery when USB power is available.',
                  '',
                  'Product page: https://www.adafruit.com/product/2821'
                  ],
    }),
    ( 'wifi_kit_8', {
        'name': 'WiFi Kit 8',
        'opts': {
            '.build.board': 'wifi_kit_8',
            '.build.variant': 'wifi_kit_8',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'The Heltec WiFi Kit 8 is an Arduino-compatible Wi-Fi development board powered by Ai-Thinker\'s ESP-12S, clocked at 80 MHz at 3.3V logic. A high-quality SiLabs CP2104 USB-Serial chip is included so that you can upload code at a blistering 921600 baud for fast development time. It also has auto-reset so no noodling with pins and reset button pressings. A 3.7V Lithium polymer battery connector is included, making it ideal for portable projects. The Heltec WiFi Kit 8 will automatically recharge a connected battery when USB power is available.',
                  '',
                  'Product page: https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series'
                  ],
    }),
    ( 'inventone', {
        'name': 'Invent One',
        'opts': {
            '.build.board': 'ESP8266_INVENT_ONE',
            '.build.variant': 'inventone',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'The Invent One is an Arduino-compatible Wi-Fi development board powered by Ai-Thinker\'s ESP-12F, clocked at 80 MHz at 3.3V logic. It has an onboard ADC (PCF8591) so that you can have multiple analog inputs to work with. More information can be found here: https://blog.inventone.ng',
                  '',
                  'Product page: https://inventone.ng'
                  ],
    }),
    ( 'cw01', {
        'name': 'XinaBox CW01',
        'opts': {
            '.build.board': 'ESP8266_XINABOX_CW01',
            '.build.variant': 'xinabox',
            },
        'macro': [
            'resetmethod_nodemcu',
            'crystalfreq_menu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'The XinaBox CW01(ESP8266) is an Arduino-compatible Wi-Fi development board powered by an ESP-12F, clocked at 80 MHz at 3.3V logic. The CW01 has an onboard RGB LED and 3 xBUS connection ports.',
                  '',
                  'Product page: https://xinabox.cc/products/CW01'
                  ],
    }),
    ( 'espresso_lite_v1', {
        'name': 'ESPresso Lite 1.0',
        'opts': {
            '.build.board': 'ESP8266_ESPRESSO_LITE_V1',
            '.build.variant': 'espresso_lite_v1',
            },
        'macro': [
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            'resetmethod_menu',
            ],
        'desc': [ 'ESPresso Lite 1.0 (beta version) is an Arduino-compatible Wi-Fi development board powered by Espressif System\'s own ESP8266 WROOM-02 module. It has breadboard-friendly breakout pins with in-built LED, two reset/flash buttons and a user programmable button . The operating voltage is 3.3VDC, regulated with 800mA maximum current. Special distinctive features include on-board I2C pads that allow direct connection to OLED LCD and sensor boards.', ]
    }),
    ( 'espresso_lite_v2', {
        'name': 'ESPresso Lite 2.0',
        'opts': {
            '.build.board': 'ESP8266_ESPRESSO_LITE_V2',
            '.build.variant': 'espresso_lite_v2',
            },
        'macro': [
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            'resetmethod_menu',
            ],
        'desc': [ 'ESPresso Lite 2.0 is an Arduino-compatible Wi-Fi development board based on an earlier V1 (beta version). Re-designed together with Cytron Technologies, the newly-revised ESPresso Lite V2.0 features the auto-load/auto-program function, eliminating the previous need to reset the board manually before flashing a new program. It also feature two user programmable side buttons and a reset button. The special distinctive features of on-board pads for I2C sensor and actuator is retained.', ]
    }),
    ( 'phoenix_v1', {
        'name': 'Phoenix 1.0',
        'opts': {
            '.build.board': 'ESP8266_PHOENIX_V1',
            '.build.variant': 'phoenix_v1',
            },
        'macro': [
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            'resetmethod_menu',
            ],
        'desc': [ 'Product page: http://www.espert.co', ],
    }),
    ( 'phoenix_v2', {
        'name': 'Phoenix 2.0',
        'opts': {
            '.build.board': 'ESP8266_PHOENIX_V2',
            '.build.variant': 'phoenix_v2',
            },
        'macro': [
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            'resetmethod_menu',
            ],
        'desc': [ 'Product page: http://www.espert.co', ],
    }),
    ( 'nodemcu', {
        'name': 'NodeMCU 0.9 (ESP-12 Module)',
        'opts': {
            '.build.board': 'ESP8266_NODEMCU_ESP12',
            '.build.variant': 'nodemcu',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_qio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'Pin mapping',
                  '~~~~~~~~~~~',
                  '',
                  'Pin numbers written on the board itself do not correspond to ESP8266 GPIO pin numbers. Constants are defined to make using this board easier:',
                  '',
                  '.. code:: c++',
                  '',
                  '    static const uint8_t D0   = 16;',
                  '    static const uint8_t D1   = 5;',
                  '    static const uint8_t D2   = 4;',
                  '    static const uint8_t D3   = 0;',
                  '    static const uint8_t D4   = 2;',
                  '    static const uint8_t D5   = 14;',
                  '    static const uint8_t D6   = 12;',
                  '    static const uint8_t D7   = 13;',
                  '    static const uint8_t D8   = 15;',
                  '    static const uint8_t D9   = 3;',
                  '    static const uint8_t D10  = 1;',
                  '',
                  'If you want to use NodeMCU pin 5, use D5 for pin number, and it will be translated to \'real\' GPIO pin 14.',
                  ],
    }),
    ( 'nodemcuv2', {
        'name': 'NodeMCU 1.0 (ESP-12E Module)',
        'opts': {
            '.build.board': 'ESP8266_NODEMCU_ESP12E',
            '.build.variant': 'nodemcu',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            'led216',
            ],
        'desc': [ 'This module is sold under many names for around $6.50 on AliExpress and it\'s one of the cheapest, fully integrated ESP8266 solutions.',
                  '',
                  'It\'s an open hardware design with an ESP-12E core and 4 MB of SPI flash.',
                  '',
                  'According to the manufacturer, "with a micro USB cable, you can connect NodeMCU devkit to your laptop and flash it without any trouble". This is more or less true: the board comes with a CP2102 onboard USB to serial adapter which just works, well, the majority of the time. Sometimes flashing fails and you have to reset the board by holding down FLASH +',
                  'RST, then releasing FLASH, then releasing RST. This forces the CP2102 device to power cycle and to be re-numbered by Linux.',
                  '',
                  'The board also features a NCP1117 voltage regulator, a blue LED on GPIO16 and a 220k/100k Ohm voltage divider on the ADC input pin.',
                  'The ESP-12E usually has a led connected on GPIO2.',
                  '',
                  'Full pinout and PDF schematics can be found `here <https://github.com/nodemcu/nodemcu-devkit-v1.0>`__',
                  ],
    }),
    ( 'modwifi', {
        'name': 'Olimex MOD-WIFI-ESP8266(-DEV)',
        'opts': {
            '.build.board': 'MOD_WIFI_ESP8266',
            '.build.variant': 'modwifi',
            },
        'macro': [
            'resetmethod_menu',
            'resetmethod_menu_extra',
            'flashmode_menu',
            'flashfreq_40',
            '2M',
            ],
        'desc': [ 'This board comes with 2 MB of SPI flash and optional accessories (e.g. evaluation board ESP8266-EVB or BAT-BOX for batteries).',
                  '',
                  'The basic module has three solder jumpers that allow you to switch the operating mode between SDIO, UART and FLASH.',
                  '',
                  'The board is shipped for FLASH operation mode, with jumpers TD0JP=0, IO0JP=1, IO2JP=1.',
                  '',
                  'Since jumper IO0JP is tied to GPIO0, which is PIN 21, you\'ll have to ground it before programming with a USB to serial adapter and reset the board by power cycling it.',
                  '',
                  'UART pins for programming and serial I/O are GPIO1 (TXD, pin 3) and GPIO3 (RXD, pin 4).',
                  '',
                  'You can find the board schematics `here <https://github.com/OLIMEX/ESP8266/blob/master/HARDWARE/MOD-WIFI-ESP8266-DEV/MOD-WIFI-ESP8266-DEV_schematic.pdf>`__',
                  ],
    }),
    ( 'thing', {
        'name': 'SparkFun ESP8266 Thing',
        'opts': {
            '.build.board': 'ESP8266_THING',
            '.build.variant': 'thing',
            },
        'macro': [
            'resetmethod_ck',
            'flashmode_qio',
            'flashfreq_40',
            '512K',
            ],
        'desc': [ 'Product page: https://www.sparkfun.com/products/13231' ],
    }),
    ( 'thingdev', {
        'name': 'SparkFun ESP8266 Thing Dev',
        'opts': {
            '.build.board': 'ESP8266_THING_DEV',
            '.build.variant': 'thing',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '512K',
            ],
        'desc': [ 'Product page: https://www.sparkfun.com/products/13711' ],
    }),
    ( 'blynk', {
        'name': 'SparkFun Blynk Board',
        'opts': {
            '.build.board': 'ESP8266_THING',
            '.build.variant': 'thing',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_qio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'Product page: https://www.sparkfun.com/products/13794' ],
    }),
    ( 'esp210', {
        'name': 'SweetPea ESP-210',
        'opts': {
            '.build.board': 'ESP8266_ESP210',
            },
        'macro': [
            'resetmethod_ck',
            'flashmode_qio',
            'flashfreq_40',
            '4M',
            ],
        'serial': '57',
        'desc': [ '*TODO*' ],
    }),
    ( 'd1_mini', {
        'name': 'LOLIN(WEMOS) D1 R2 & mini',
        'opts': {
            '.build.board': 'ESP8266_WEMOS_D1MINI',
            '.build.variant': 'd1_mini',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'serial': '921',
        'desc': [ 'Product page: https://www.wemos.cc/' ],
    }),
    ( 'd1_mini_clone', {
        'name': 'LOLIN(WEMOS) D1 mini (clone)',
        'opts': {
            '.build.board': 'ESP8266_WEMOS_D1MINI',
            '.build.variant': 'd1_mini',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_menu',
            'flashfreq_menu',
            '4M',
            ],
        'serial': '921',
        'desc': [ 'Clone variant of the LOLIN(WEMOS) D1 mini board,',
                  'with enabled flash-mode menu, DOUT selected by default.',
                  '',
                  'Product page of the preferred official board: https://www.wemos.cc/',
                  ],
    }),
    ( 'd1_mini_pro', {
        'name': 'LOLIN(WEMOS) D1 mini Pro',
        'opts': {
            '.build.board': 'ESP8266_WEMOS_D1MINIPRO',
            '.build.variant': 'd1_mini',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '16M',
            ],
        'serial': '921',
        'desc': [ 'Product page: https://www.wemos.cc/' ],
    }),
    ( 'd1_mini_lite', {
        'name': 'LOLIN(WEMOS) D1 mini Lite',
        'opts': {
            '.build.board': 'ESP8266_WEMOS_D1MINILITE',
            '.build.variant': 'd1_mini',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dout',
            'flashfreq_40',
            '1M',
            ],
        'serial': '921',
        'desc': [
            'Parameters in Arduino IDE:',
            '~~~~~~~~~~~~~~~~~~~~~~~~~~',
            '',
            '- Card: "WEMOS D1 Mini Lite"',
            '- Flash Size: "1M (512K FS)"',
            '- CPU Frequency: "80 Mhz"',
          # '- Upload Speed: "230400"',
            '',
            'Power:',
            '~~~~~~',
            '',
            '- 5V pin : 4.7V 500mA output when the board is powered by USB ; 3.5V-6V input',
            '- 3V3 pin : 3.3V 500mA regulated output',
            '- Digital pins : 3.3V 30mA.',
            '',
            'links:',
            '~~~~~~',
            '',
            '- Product page: https://www.wemos.cc/',
            '- Board schematic: https://wiki.wemos.cc/_media/products:d1:sch_d1_mini_lite_v1.0.0.pdf',
            '- ESP8285 datasheet: https://www.espressif.com/sites/default/files/0a-esp8285_datasheet_en_v1.0_20160422.pdf',
            '- Voltage regulator datasheet: http://pdf-datasheet.datasheet.netdna-cdn.com/pdf-down/M/E/6/ME6211-Microne.pdf',
        ],
    }),
    ( 'd1', {
        'name': 'LOLIN(WeMos) D1 R1',
        'opts': {
            '.build.board': 'ESP8266_WEMOS_D1R1',
            '.build.variant': 'd1',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'serial': '921',
        'desc': [ 'Product page: https://www.wemos.cc/' ],
    }),
    ( 'espino', {
        'name': 'ESPino (ESP-12 Module)',
        'opts': {
            '.build.board': 'ESP8266_ESPINO_ESP12',
            '.build.variant': 'espino',
            },
        'macro': [
            'resetmethod_menu',
            'flashmode_qio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'ESPino integrates the ESP-12 module with a 3.3v regulator, CP2104 USB-Serial bridge and a micro USB connector for easy programming. It is designed for fitting in a breadboard and has an RGB Led and two buttons for easy prototyping.',
                  '',
                  'For more information about the hardware, pinout diagram and programming procedures, please see the `datasheet <https://github.com/makerlabmx/ESPino-tools/raw/master/Docs/ESPino-Datasheet-EN.pdf>`__.',
                  '',
                  'Product page: http://www.espino.io/en',
                  ],
    }),
    ( 'espinotee', {
        'name': 'ThaiEasyElec\'s ESPino',
        'opts': {
            '.build.board': 'ESP8266_ESPINO_ESP13',
            '.build.variant': 'espinotee',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_qio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'ESPino by ThaiEasyElec using WROOM-02 module from Espressif Systems with 4 MB Flash.',
                  '',
                  'We will update an English description soon. - Product page:',
                  'http://thaieasyelec.com/products/wireless-modules/wifi-modules/espino-wifi-development-board-detail.html',
                  '- Schematics:',
                  'www.thaieasyelec.com/downloads/ETEE052/ETEE052\_ESPino\_Schematic.pdf -',
                  'Dimensions:',
                  'http://thaieasyelec.com/downloads/ETEE052/ETEE052\_ESPino\_Dimension.pdf',
                  '- Pinouts:',
                  'http://thaieasyelec.com/downloads/ETEE052/ETEE052\_ESPino\_User\_Manual\_TH\_v1\_0\_20160204.pdf (Please see pg. 8)',
                  ],
    }),
    ( 'wifinfo', {
        'name': 'WifInfo',
        'opts': collections.OrderedDict([
            ( '.build.board', 'WIFINFO' ),
            ( '.build.variant', 'wifinfo' ),
            ( '.menu.ESPModule.ESP07192', 'ESP07 (1M/192K FS)' ),
            ( '.menu.ESPModule.ESP07192.build.board', 'ESP8266_ESP07' ),
            ( '.menu.ESPModule.ESP07192.build.flash_size', '1M' ),
            ( '.menu.ESPModule.ESP07192.build.flash_ld', 'eagle.flash.1m192.ld' ),
            ( '.menu.ESPModule.ESP07192.build.spiffs_start', '0xCB000' ),
            ( '.menu.ESPModule.ESP07192.build.spiffs_end', '0xFB000' ),
            ( '.menu.ESPModule.ESP07192.build.spiffs_blocksize', '4096' ),
            ( '.menu.ESPModule.ESP07192.upload.maximum_size', '827376' ),
            ( '.menu.ESPModule.ESP12', 'ESP12 (4M/1M FS)' ),
            ( '.menu.ESPModule.ESP12.build.board', 'ESP8266_ESP12' ),
            ( '.menu.ESPModule.ESP12.build.flash_size', '4M' ),
            ( '.menu.ESPModule.ESP12.build.flash_ld', 'eagle.flash.4m1m.ld' ),
            ( '.menu.ESPModule.ESP12.build.spiffs_start', '0x300000' ),
            ( '.menu.ESPModule.ESP12.build.spiffs_end', '0x3FB000' ),
            ( '.menu.ESPModule.ESP12.build.spiffs_blocksize', '8192' ),
            ( '.menu.ESPModule.ESP12.build.spiffs_pagesize', '256' ),
            ( '.menu.ESPModule.ESP12.upload.maximum_size', '1044464' ),
        ]),
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_qio',
            'flashfreq_menu',
            '1M',
            ],
        'desc': [ 'WifInfo integrates the ESP-12 or ESP-07+Ext antenna module with a 3.3v regulator and the hardware to be able to measure French telemetry issue from ERDF powering meter serial output. It has a USB connector for powering, an RGB WS2812 Led, 4 pins I2C connector to fit OLED or sensor, and two buttons + FTDI connector and auto reset feature.',
                  '',
                  'For more information, please see WifInfo related `blog <http://hallard.me/category/wifinfo/>`__ entries, `github <https://github.com/hallard/WifInfo>`__ and `community <https://community.hallard.me/category/16/wifinfo>`__ forum.',
                  ],
    }),
    ( 'arduino-esp8266', {
        'name': 'Arduino',
        'opts': collections.OrderedDict([
            ( '.build.board', 'ESP8266_ARDUINO' ),
            ( '.menu.BoardModel.primo', 'Primo' ),
            ( '.menu.BoardModel.primo.build.board', 'ESP8266_ARDUINO_PRIMO' ),
            ( '.menu.BoardModel.primo.build.variant', 'arduino_spi' ),
            ( '.menu.BoardModel.primo.build.extra_flags', '-DF_CRYSTAL=40000000' ),
            ( '.menu.BoardModel.unowifideved', 'Uno WiFi' ),
            ( '.menu.BoardModel.unowifideved.build.board', 'ESP8266_ARDUINO_UNOWIFI' ),
            ( '.menu.BoardModel.unowifideved.build.variant', 'arduino_uart' ),
            ( '.menu.BoardModel.unowifideved.build.extra_flags=-DF_CRYSTAL', '40000000' ),
            ( '.menu.BoardModel.starottodeved', 'Star OTTO' ),
            ( '.menu.BoardModel.starottodeved.build.variant', 'arduino_uart' ),
            ( '.menu.BoardModel.starottodeved.build.board', 'ESP8266_ARDUINO_STAR_OTTO' ),
            ( '.menu.BoardModel.starottodeved.build.extra_flags', '-DF_CRYSTAL=40000000' ),
        ]),
        'macro': [
            'resetmethod_ck',
            'flashmode_qio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ '*TODO*' ],
    }),
    ( 'gen4iod', {
        'name': '4D Systems gen4 IoD Range',
        'opts': {
            '.build.board': 'GEN4_IOD',
            '.build.f_cpu': '160000000L',
            '.build.variant': 'generic',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_menu',
            'flashfreq_80',
            '2M',
            '512K',
            ],
        'desc': [ 'gen4-IoD Range of ESP8266 powered Display Modules by 4D Systems.',
                  '',
                  '2.4", 2.8" and 3.2" TFT LCD with uSD card socket and Resistive Touch. Chip Antenna + uFL Connector.',
                  '',
                  'Datasheet and associated downloads can be found on the 4D Systems product page.',
                  '',
                  'The gen4-IoD range can be programmed using the Arduino IDE and also the 4D Systems Workshop4 IDE, which incorporates many additional graphics benefits. GFX4d library is available, along with a number of demo applications.',
                  '',
                  '- Product page: https://4dsystems.com.au/products/iot-display-modules',
                  ],
    }),
    ( 'oak', {
        'name': 'Digistump Oak',
        'opts': {
            '.build.board': 'ESP8266_OAK',
            '.build.variant': 'oak',
            },
        'macro': [
            'resetmethod_none',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'serial': '921',
        'desc': [ 'The Oak requires an `Serial Adapter`_ for a serial connection or flashing; its micro USB port is only for power.',
                  '',
                  'To make a serial connection, wire the adapter\'s **TX to P3**, **RX to P4**, and **GND** to **GND**.  Supply 3.3v from the serial adapter if not already powered via USB.',
                  '',
                  'To put the board into bootloader mode, configure a serial connection as above, connect **P2 to GND**, then re-apply power.  Once flashing is complete, remove the connection from P2 to GND, then re-apply power to boot into normal mode.',
                  ],
    }),
    ( 'wifiduino', {
        'name': 'WiFiduino',
        'opts': {
            '.build.board': 'WIFIDUINO_ESP8266',
            '.build.variant': 'wifiduino',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'serial': '921',
        'desc': [ 'Product page: https://wifiduino.com/esp8266' ],
    }),
    ( 'wifi_slot', {
        'name': 'Amperka WiFi Slot',
        'opts': {
            '.build.board': 'AMPERKA_WIFI_SLOT',
            '.build.variant': 'wifi_slot',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashfreq_menu',
            'flashmode_menu',
            '1M', '2M',
            ],
        'desc': [ 'Product page: http://wiki.amperka.ru/wifi-slot' ],
    }),
    ( 'wiolink', {
        'name': 'Seeed Wio Link',
        'opts': {
            '.build.board': 'ESP8266_WIO_LINK',
            '.build.variant': 'wiolink',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_qio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'Wio Link is designed to simplify your IoT development. It is an ESP8266 based open-source Wi-Fi development board to create IoT applications by virtualizing plug-n-play modules to RESTful APIs with mobile APPs. Wio Link is also compatible with the Arduino IDE.',
                  '',
                  'Please DO NOTICE that you MUST pull up pin 15 to enable the power for Grove ports, the board is designed like this for the purpose of peripherals power management.',
                  '',
                  'Product page: https://www.seeedstudio.com/Wio-Link-p-2604.html'
                ],
    }),
    ('espectro', {
        'name': 'ESPectro Core',
        'opts': {
            '.build.board': 'ESP8266_ESPECTRO_CORE',
            '.build.variant': 'espectro',
        },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
        ],
        'desc': [
            'ESPectro Core is ESP8266 development board as the culmination of our 3+ year experience in exploring and developing products with ESP8266 MCU.',
            '',
            'Initially designed for kids in mind, everybody should be able to use it. Yet it\'s still hacker-friendly as we break out all ESP8266 ESP-12F pins.',
            '',
            'More details at https://shop.makestro.com/product/espectrocore/',
        ],
    }),

	( 'eduinowifi', {
        'name': 'Schirmilabs Eduino WiFi',
        'opts': {
            '.build.board': 'ESP8266_SCHIRMILABS_EDUINO_WIFI',
            '.build.variant': 'eduinowifi',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'serial': '512',
        'desc': [ 'Eduino WiFi is an Arduino-compatible DIY WiFi development board using an ESP-12 module',
		          '',
				  'Product page: https://schirmilabs.de/?page_id=165',
				  ]

    }),
    ( 'sonoff', {
        'name': 'ITEAD Sonoff',
        'opts': {
            '.build.board': 'ESP8266_SONOFF_SV',
            '.build.variant': 'itead',
            '.build.flash_size': '1M',
            '.menu.BoardModel.sonoffSV': 'ITEAD Sonoff SV',
            '.menu.BoardModel.sonoffSV.build.board': 'ESP8266_SONOFF_SV',
            '.menu.BoardModel.sonoffTH': 'ITEAD Sonoff TH',
            '.menu.BoardModel.sonoffTH.build.board': 'ESP8266_SONOFF_TH',
            '.menu.BoardModel.sonoffBasic': 'ITEAD Sonoff Basic',
            '.menu.BoardModel.sonoffBasic.build.board': 'ESP8266_SONOFF_BASIC',
            '.menu.BoardModel.sonoffS20': 'ITEAD Sonoff S20',
            '.menu.BoardModel.sonoffS20.build.board': 'ESP8266_SONOFF_S20',
             },
        'macro': [
            'resetmethod_none',
            'flashmode_dout',
            'flashfreq_40',
            '1M',
            ],
        'desc': [
            'ESP8266 based devices from ITEAD: Sonoff SV, Sonoff TH, Sonoff Basic, ' +
            'and Sonoff S20',
            '',
            'These are not development boards. The development process is ' +
            'inconvenient with these devices. When flashing firmware you will ' +
            'need a Serial Adapter to connect it to your computer.',
            '',
            ' | Most of these devices, during normal operation, are connected to ' +
            '*wall power (AKA Mains Electricity)*. **NEVER** try to flash these ' +
            'devices when connected to *wall power*. **ALWAYS** have them ' +
            'disconnected from *wall power* when connecting them to your ' +
            'computer. Your life may depend on it!',
            '',
            'When flashing you will need to hold down the push button connected ' +
            'to the GPIO0 pin, while powering up with a safe 3.3 Volt source. Some USB ' +
            'Serial Adapters may supply enough power to handle flashing; ' +
            'however, it many may not supply enough power to handle the ' +
            'activities when the device reboots.',
            '',
            'More product details at the bottom of https://www.itead.cc/wiki/Product/'
        ],
    }),
    ( 'espmxdevkit', {
        'name': 'DOIT ESP-Mx DevKit (ESP8285)',
        'opts': {
            '.build.board': 'ESP8266_ESP01',
            '.build.variant': 'esp8285',
            '.build.led': '-DLED_BUILTIN=16',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dout',
            'flashfreq_40',
            '1M',
            ],
        'desc': [
            'DOIT ESP-Mx DevKit - This is a development board by DOIT, with a DOIT ESP-Mx module ' +
            '(`datasheet <https://github.com/SmartArduino/SZDOITWiKi/wiki/ESP8285---ESP-M2>`__) ' +
            'using a ESP8285 Chip. With the DOIT ESP-Mx module, GPIO pins 9 and 10 are not available. ' +
            'The DOIT ESP-Mx DevKit board has a red power LED and a blue LED connected to GPIO16 ' +
            'and is active low to turn on. It uses a CH340C, USB to Serial converter chip. ',
            '',
            'ESP8285 (`datasheet <http://www.espressif.com/sites/default/files/0a-esp8285_datasheet_en_v1.0_20160422.pdf>`__) ' +
            'is a multi-chip package which contains ESP8266 and 1MB flash. '
        ],

    })
	])

REQUIRED_BOARDS = ( 'generic', 'esp8285' )

################################################################

MACROS = {
    'defaults': collections.OrderedDict([
        ( '.upload.tool', 'esptool' ),
        ( '.upload.maximum_data_size', '81920' ),
        ( '.upload.wait_for_upload_port', 'true' ),
        ( '.upload.erase_cmd', ''),
        ( '.serial.disableDTR', 'true' ),
        ( '.serial.disableRTS', 'true' ),
        ( '.build.mcu', 'esp8266' ),
        ( '.build.core', 'esp8266' ),
        ( '.build.variant', 'generic' ),
        ( '.build.spiffs_pagesize', '256' ),
        ( '.build.debug_port', '' ),
        ( '.build.debug_level', '' ),
        ]),

    #######################

    'cpufreq_menu': collections.OrderedDict([
        ( '.menu.xtal.80', '80 MHz' ),
        ( '.menu.xtal.80.build.f_cpu', '80000000L' ),
        ( '.menu.xtal.160', '160 MHz' ),
        ( '.menu.xtal.160.build.f_cpu', '160000000L' ),
        ]),

    'vtable_menu': collections.OrderedDict([
        ( '.menu.vt.flash', 'Flash'),
        ( '.menu.vt.flash.build.vtable_flags', '-DVTABLES_IN_FLASH'),
        ( '.menu.vt.heap', 'Heap'),
        ( '.menu.vt.heap.build.vtable_flags', '-DVTABLES_IN_DRAM'),
        ( '.menu.vt.iram', 'IRAM'),
        ( '.menu.vt.iram.build.vtable_flags', '-DVTABLES_IN_IRAM'),
        ]),

    'exception_menu': collections.OrderedDict([
        ( '.menu.exception.disabled', 'Disabled (new aborts on oom)' ),
        ( '.menu.exception.disabled.build.exception_flags', '-fno-exceptions' ),
        ( '.menu.exception.disabled.build.stdcpp_lib', '-lstdc++' ),
        ( '.menu.exception.enabled', 'Enabled' ),
        ( '.menu.exception.enabled.build.exception_flags', '-fexceptions' ),
        ( '.menu.exception.enabled.build.stdcpp_lib', '-lstdc++-exc' ),
        ]),

    'stacksmash_menu': collections.OrderedDict([
        ( '.menu.stacksmash.disabled', 'Disabled' ),
        ( '.menu.stacksmash.disabled.build.stacksmash_flags', '' ),
        ( '.menu.stacksmash.enabled', 'Enabled' ),
        ( '.menu.stacksmash.enabled.build.stacksmash_flags', '-fstack-protector' ),
        ]),

    'crystalfreq_menu': collections.OrderedDict([
        ( '.menu.CrystalFreq.26', '26 MHz' ),
        ( '.menu.CrystalFreq.40', '40 MHz' ),
        ( '.menu.CrystalFreq.40.build.extra_flags', '-DF_CRYSTAL=40000000' ),
        ]),

    'flashfreq_menu': collections.OrderedDict([
        ( '.menu.FlashFreq.40', '40MHz' ),
        ( '.menu.FlashFreq.40.build.flash_freq', '40' ),
        ( '.menu.FlashFreq.80', '80MHz' ),
        ( '.menu.FlashFreq.80.build.flash_freq', '80' ),
        ( '.menu.FlashFreq.20', '20MHz' ),
        ( '.menu.FlashFreq.20.build.flash_freq', '20' ),
        ( '.menu.FlashFreq.26', '26MHz' ),
        ( '.menu.FlashFreq.26.build.flash_freq', '26' ),
        ]),

    'flashfreq_40': collections.OrderedDict([
        ( '.build.flash_freq', '40' ),
        ]),

    'flashfreq_80': collections.OrderedDict([
        ( '.build.flash_freq', '80' ),
        ]),

    ####################### menu.resetmethod

    'resetmethod_menu': collections.OrderedDict([
        ( '.menu.ResetMethod.nodemcu', 'dtr (aka nodemcu)' ),
        ( '.menu.ResetMethod.nodemcu.upload.resetmethod', '--before default_reset --after hard_reset' ),
        ( '.menu.ResetMethod.ck', 'no dtr (aka ck)' ),
        ( '.menu.ResetMethod.ck.upload.resetmethod', '--before no_reset --after soft_reset' ),
        ]),

    'resetmethod_menu_extra': collections.OrderedDict([
        ( '.menu.ResetMethod.nodtr_nosync', 'no dtr, no_sync' ),
        ( '.menu.ResetMethod.nodtr_nosync.upload.resetmethod', '--before no_reset_no_sync --after soft_reset' ),
        ]),

    ####################### upload.resetmethod (new esptool.py options)

    'resetmethod_ck': collections.OrderedDict([
        ( '.upload.resetmethod', '--before no_reset --after soft_reset' ),
        ]),

    'resetmethod_nodemcu': collections.OrderedDict([
        ( '.upload.resetmethod', '--before default_reset --after hard_reset' ),
        ]),

    'resetmethod_none': collections.OrderedDict([
        ( '.upload.resetmethod', '--before no_reset --after soft_reset' ),
        ]),

    'resetmethod_dtrset': collections.OrderedDict([
        ( '.upload.resetmethod', '--before default_reset --after hard_reset' ),
        ]),

    'resetmethod_nodtr_nosync': collections.OrderedDict([
        ( '.upload.resetmethod', '--before no_reset_no_sync --after soft_reset' ),
        ]),

    ####################### menu.FlashMode

    'flashmode_menu': collections.OrderedDict([
        ( '.menu.FlashMode.dout', 'DOUT (compatible)' ),
        ( '.menu.FlashMode.dout.build.flash_mode', 'dout' ),
        ( '.menu.FlashMode.dout.build.flash_flags', '-DFLASHMODE_DOUT' ),
        ( '.menu.FlashMode.dio', 'DIO' ),
        ( '.menu.FlashMode.dio.build.flash_mode', 'dio' ),
        ( '.menu.FlashMode.dio.build.flash_flags', '-DFLASHMODE_DIO' ),
        ( '.menu.FlashMode.qout', 'QOUT' ),
        ( '.menu.FlashMode.qout.build.flash_mode', 'qout' ),
        ( '.menu.FlashMode.qout.build.flash_flags', '-DFLASHMODE_QOUT' ),
        ( '.menu.FlashMode.qio', 'QIO (fast)' ),
        ( '.menu.FlashMode.qio.build.flash_mode', 'qio' ),
        ( '.menu.FlashMode.qio.build.flash_flags', '-DFLASHMODE_QIO' ),
        ]),

    ####################### default flash_mode

    'flashmode_dio': collections.OrderedDict([
        ( '.build.flash_mode', 'dio' ),
        ( '.build.flash_flags', '-DFLASHMODE_DIO' ),
        ]),

    'flashmode_qio': collections.OrderedDict([
        ( '.build.flash_mode', 'qio' ),
        ( '.build.flash_flags', '-DFLASHMODE_QIO' ),
        ]),

    'flashmode_dout': collections.OrderedDict([
        ( '.build.flash_mode', 'dout' ),
        ( '.build.flash_flags', '-DFLASHMODE_DOUT' ),
        ]),

    'flashmode_qout': collections.OrderedDict([
        ( '.build.flash_mode', 'qout' ),
        ( '.build.flash_flags', '-DFLASHMODE_QOUT' ),
        ]),

    ####################### lwip

    'lwip': collections.OrderedDict([
        ( '.menu.ip.lm2f', 'v2 Lower Memory' ),
        ( '.menu.ip.lm2f.build.lwip_include', 'lwip2/include' ),
        ( '.menu.ip.lm2f.build.lwip_lib', '-llwip2-536-feat' ),
        ( '.menu.ip.lm2f.build.lwip_flags', '-DLWIP_OPEN_SRC -DTCP_MSS=536 -DLWIP_FEATURES=1 -DLWIP_IPV6=0' ),
        ( '.menu.ip.hb2f', 'v2 Higher Bandwidth' ),
        ( '.menu.ip.hb2f.build.lwip_include', 'lwip2/include' ),
        ( '.menu.ip.hb2f.build.lwip_lib', '-llwip2-1460-feat' ),
        ( '.menu.ip.hb2f.build.lwip_flags', '-DLWIP_OPEN_SRC -DTCP_MSS=1460 -DLWIP_FEATURES=1 -DLWIP_IPV6=0' ),
        ( '.menu.ip.lm2n', 'v2 Lower Memory (no features)' ),
        ( '.menu.ip.lm2n.build.lwip_include', 'lwip2/include' ),
        ( '.menu.ip.lm2n.build.lwip_lib', '-llwip2-536' ),
        ( '.menu.ip.lm2n.build.lwip_flags', '-DLWIP_OPEN_SRC -DTCP_MSS=536 -DLWIP_FEATURES=0 -DLWIP_IPV6=0' ),
        ( '.menu.ip.hb2n', 'v2 Higher Bandwidth (no features)' ),
        ( '.menu.ip.hb2n.build.lwip_include', 'lwip2/include' ),
        ( '.menu.ip.hb2n.build.lwip_lib', '-llwip2-1460' ),
        ( '.menu.ip.hb2n.build.lwip_flags', '-DLWIP_OPEN_SRC -DTCP_MSS=1460 -DLWIP_FEATURES=0 -DLWIP_IPV6=0' ),
        ( '.menu.ip.lm6f', 'v2 IPv6 Lower Memory' ),
        ( '.menu.ip.lm6f.build.lwip_include', 'lwip2/include' ),
        ( '.menu.ip.lm6f.build.lwip_lib', '-llwip6-536-feat' ),
        ( '.menu.ip.lm6f.build.lwip_flags', '-DLWIP_OPEN_SRC -DTCP_MSS=536 -DLWIP_FEATURES=1 -DLWIP_IPV6=1' ),
        ( '.menu.ip.hb6f', 'v2 IPv6 Higher Bandwidth' ),
        ( '.menu.ip.hb6f.build.lwip_include', 'lwip2/include' ),
        ( '.menu.ip.hb6f.build.lwip_lib', '-llwip6-1460-feat' ),
        ( '.menu.ip.hb6f.build.lwip_flags', '-DLWIP_OPEN_SRC -DTCP_MSS=1460 -DLWIP_FEATURES=1 -DLWIP_IPV6=1' ),
        ]),

    ####################### serial

    's9': collections.OrderedDict([
        ( '.menu.baud.9600', '9600' ),
        ( '.menu.baud.9600.upload.speed', '9600' ),
        ]),
    's57': collections.OrderedDict([
        ( '.menu.baud.57600', '57600' ),
        ( '.menu.baud.57600.upload.speed', '57600' ),
        ]),
    's115': collections.OrderedDict([
        ( '.menu.baud.115200', '115200' ),
        ( '.menu.baud.115200.upload.speed', '115200' ),
        ]),
    's256': collections.OrderedDict([
        ( '.menu.baud.256000.windows', '256000' ),
        ( '.menu.baud.256000.upload.speed', '256000' ),
        ]),
    's230': collections.OrderedDict([
        ( '.menu.baud.230400.linux', '230400' ),
        ( '.menu.baud.230400.macosx', '230400' ),
        ( '.menu.baud.230400.upload.speed', '230400' ),
        ]),
    's460': collections.OrderedDict([
        ( '.menu.baud.460800.linux', '460800' ),
        ( '.menu.baud.460800.macosx', '460800' ),
        ( '.menu.baud.460800.upload.speed', '460800' ),
        ]),
    's512': collections.OrderedDict([
        ( '.menu.baud.512000.windows', '512000' ),
        ( '.menu.baud.512000.upload.speed', '512000' ),
        ]),
    's921': collections.OrderedDict([
        ( '.menu.baud.921600', '921600' ),
        ( '.menu.baud.921600.upload.speed', '921600' ),
        ]),
    's3000': collections.OrderedDict([
        ( '.menu.baud.3000000', '3000000' ),
        ( '.menu.baud.3000000.upload.speed', '3000000' ),
        ]),

    ####################### flash erase

    'flash_erase_menu': collections.OrderedDict([
        ( '.menu.wipe.none', 'Only Sketch' ),
        ( '.menu.wipe.none.upload.erase_cmd', '' ),
        ( '.menu.wipe.sdk', 'Sketch + WiFi Settings' ),
        ( '.menu.wipe.sdk.upload.erase_cmd', 'erase_region "{build.rfcal_addr}" 0x4000' ),
        ( '.menu.wipe.all', 'All Flash Contents' ),
        ( '.menu.wipe.all.upload.erase_cmd', 'erase_flash' ),
        ]),

    ######################## SSL supported protocols

    'ssl_cipher_menu': collections.OrderedDict([
        ( '.menu.ssl.all', 'All SSL ciphers (most compatible)' ),
        ( '.menu.ssl.all.build.sslflags', ''),
        ( '.menu.ssl.basic', 'Basic SSL ciphers (lower ROM use)' ),
        ( '.menu.ssl.basic.build.sslflags', '-DBEARSSL_SSL_BASIC'),
        ]),

    ####################### mmu

    'mmu_menu': collections.OrderedDict([
        ( '.menu.mmu.3232', '32KB cache + 32KB IRAM (balanced)' ),
        ( '.menu.mmu.3232.build.mmuflags', '-DMMU_IRAM_SIZE=0x8000 -DMMU_ICACHE_SIZE=0x8000'),
        ( '.menu.mmu.4816', '16KB cache + 48KB IRAM (IRAM)' ),
        ( '.menu.mmu.4816.build.mmuflags', '-DMMU_IRAM_SIZE=0xC000 -DMMU_ICACHE_SIZE=0x4000' ),
        ( '.menu.mmu.4816H', '16KB cache + 48KB IRAM and 2nd Heap (shared)' ),
        ( '.menu.mmu.4816H.build.mmuflags', '-DMMU_IRAM_SIZE=0xC000 -DMMU_ICACHE_SIZE=0x4000 -DMMU_IRAM_HEAP' ),
        ( '.menu.mmu.3216', '16KB cache + 32KB IRAM + 16KB 2nd Heap (not shared)' ),
        ( '.menu.mmu.3216.build.mmuflags', '-DMMU_IRAM_SIZE=0x8000 -DMMU_ICACHE_SIZE=0x4000 -DMMU_SEC_HEAP=0x40108000 -DMMU_SEC_HEAP_SIZE=0x4000' ),
        ( '.menu.mmu.ext128k', '128K Heap External 23LC1024' ),
        ( '.menu.mmu.ext128k.build.mmuflags', '-DMMU_EXTERNAL_HEAP=128 -DMMU_IRAM_SIZE=0x8000 -DMMU_ICACHE_SIZE=0x8000' ),
        ( '.menu.mmu.ext8192k', '8M w/256K Heap External 64 MBit PSRAM' ),
        ( '.menu.mmu.ext8192k.build.mmuflags', '-DMMU_EXTERNAL_HEAP=256 -DMMU_IRAM_SIZE=0x8000 -DMMU_ICACHE_SIZE=0x8000' ),
        ]),

    ######################## Non 32-bit load/store exception handler

    'non32xfer_menu': collections.OrderedDict([
        ('.menu.non32xfer.fast', 'Use pgm_read macros for IRAM/PROGMEM' ),
        ('.menu.non32xfer.fast.build.non32xferflags', ''),
        ('.menu.non32xfer.safe', 'Byte/Word access to IRAM/PROGMEM (very slow)' ),
        ('.menu.non32xfer.safe.build.non32xferflags', '-DNON32XFER_HANDLER'),
        ])
    }

################################################################
# file writer helpers, we need these for generator functions


class BaseOpenWithBackup:
    """Generic context object. Replace stdout with the specified path.
    If backup is set, save on entry and restore it on error."""

    def __init__(self, path):
        self.path = os.path.normpath(path)
        self.backup = None

    def __enter__(self):
        self._stdout = sys.stdout
        sys.stdout = open(self.path, "w", encoding="utf-8", newline="\n")
        return self

    def __exit__(self, *exc):
        sys.stdout.close()
        sys.stdout = self._stdout
        if any(exc):
            if self.backup and os.path.isfile(self.backup):
                os.rename(self.backup, self.path)
                print(f"error: {exc}\nrestored {self.path} from {self.backup}")
            return
        print(f"wrote: {self.path}")
        if self.backup:
            print(f"backup: {self.backup}")


class Open(BaseOpenWithBackup):
    """Open and replace stdout with the specified path."""

    pass


class OpenWithBackupFile(BaseOpenWithBackup):
    """Replace stdout with the specified path.
    Creates .orig file in the same location before entry."""

    def __init__(self, path):
        super().__init__(path)
        self.backup = f"{self.path}.orig"

    def __enter__(self):
        if os.path.isfile(self.backup):
            os.remove(self.backup)

        if os.path.isfile(self.path):
            os.rename(self.path, self.backup)

        return super().__enter__()


################################################################
# debug options


def subslices(values):
    """Generate a combinations list for each possible length of the provided list.
    Useful for build defines combinations, where we would like to generate
    multiple menu entries for both specific flags and a combinations of them.

    >>> list(subslices(["A", "B"]))
    [('A',), ('B',), ('A', 'B')]
    >>> list(subslices(["A", "B", "C"]))
    [('A',), ('B',), ('C',), ('A', 'B'), ('A', 'C'), ('B', 'C'), ('A', 'B', 'C')]
    """
    for n, _ in enumerate(values, start=1):
        for combination in itertools.combinations(values, n):
            yield combination


def debug_macros():
    debugmenu = collections.OrderedDict(
        [
            (".menu.dbg.Disabled", "Disabled"),
            (".menu.dbg.Disabled.build.debug_port", ""),
            (".menu.dbg.Serial", "Serial"),
            (".menu.dbg.Serial.build.debug_port", "-DDEBUG_ESP_PORT=Serial"),
            (".menu.dbg.Serial1", "Serial1"),
            (".menu.dbg.Serial1.build.debug_port", "-DDEBUG_ESP_PORT=Serial1"),
            (".menu.lvl.None____", "None"),
            (".menu.lvl.None____.build.debug_level", ""),
        ]
    )

    options = []

    # since IDE does not allow to flag multiple options, these are split
    # into two sort-of related groups. first ones end up as 'subslice' combinations
    a = ("SSL", "TLS_MEM", "HTTP_CLIENT", "HTTP_SERVER")

    options.extend(subslices(a))

    # these end up as standalone options and a combined one
    b = ("CORE", "WIFI", "HTTP_UPDATE", "UPDATER", "OTA", "OOM", "MDNS")
    for flag in b:
        options.append((flag,))
    options.append(b)

    # and these could only happen on their own or with grouped options
    for flag in ("HWDT", "HWDT_NOEXTRA4K"):
        options.append((flag,))
        options.append(b + (flag,))
        options.append(a + b + (flag,))

    def add_menu_entry(name, menuname, flags):
        debugmenu.update(
            (
                (
                    (f".menu.lvl.{name}", menuname),
                    (
                        f".menu.lvl.{name}.build.debug_level",
                        " {}".format(" ".join(flags)),
                    ),
                )
            )
        )

    add_menu_entry("NoAssert-NDEBUG", "NoAssert-NDEBUG", ["-DNDEBUG"])

    # TODO make sure to prepend with space
    for optlist in options:
        add_menu_entry(
            "".join(optlist),
            "+".join(optlist),
            [f"-DDEBUG_ESP_{opt}" for opt in optlist],
        )

    return {"debug_menu": debugmenu}


################################################################
# flash size

# TODO well, actually... it's kibi and mibi. update menu and .ld comment to use KiB and MiB?
# menu selectors remain as 1 char suffixes, still

BYTES = 1
KILOBYTES = 1024
MEGABYTES = 1024 * 1024


def Bytes(size):
    """Just bytes
    >>> Bytes(1)
    1
    >>> Bytes(1024)
    1024
    """
    return size


def Kilobytes(size):
    """Represent N kilobytes as bytes
    >>> Kilobytes(1)
    1024
    >>> Kilobytes(1024)
    1048576
    """
    return size * KILOBYTES


def Megabytes(size):
    """Represent N megabytes as bytes
    >>> Megabytes(1)
    1048576
    >>> Megabytes(1024)
    1073741824
    """
    return size * MEGABYTES


def humanize(size, *, decimal=False, convert=None):
    """Print something intelligible instead of just the value as-is.
    To use with .ld and menu, also support custom suffixes for ratios
    as [RATIO, SUFFIX] pairs.

    >>> humanize(Bytes(8))
    '8B'
    >>> humanize(Megabytes(1))
    '1MB'
    >>> humanize(Megabytes(1) - Bytes(10))
    '1023KB'
    >>> humanize(Megabytes(1) + Bytes(10))
    '1MB'
    >>> humanize(Megabytes(1) + Kilobytes(512))
    '1536KB'

    """

    if not convert:
        convert = [
            [Bytes(1), "B"],
            [Kilobytes(1), "KB"],
            [Megabytes(1), "MB"],
        ]

    for ratio, suffix in reversed(convert):
        if size >= ratio:
            if size % ratio > (size / 4):
                continue

            size = f"{size / ratio:.02f}"
            if not decimal:
                size = size[:-3]

            size = size.replace(".00", "")

            return f"{size}{suffix}"

    return ""


class Region:
    """Represent certain start and end addresses (in bytes).

    >>> a = Region("", 0, 0)
    >>> a.size
    0
    >>> b = Region("", 0, 1024)
    >>> b.start
    0
    >>> b.end
    1024
    >>> b.size
    1024
    >>> Region("", 1024, 0)
    Traceback (most recent call last):
        ...
    ValueError: start=1024 cannot be larger than end=0
    """

    def __init__(self, name, start, end):
        if start > end:
            raise ValueError(f"{start=} cannot be larger than {end=}")
        self.name = name
        self.start = start
        self.end = end
        self.size = Bytes(end - start)

    def distance(self, other):
        if self.end <= other.start:
            return other.start - self.end
        elif self.start >= other.end:
            return other.end - self.start

        return None

    def __repr__(self):
        return f"<Region {self.name=} {self.size=} at [0x{self.start:08x}:0x{self.end:08x})>"

    def copy(self):
        return Region(self.name, self.start, self.end)

    @staticmethod
    def after(region, size):
        """
        >>> a = Region("", 0, 1024)
        >>> b = Region.after(a, 1024)
        >>> b.start
        1024
        >>> b.end
        2048
        >>> b.size
        1024
        """
        return Region("", region.end, region.end + size)

    @staticmethod
    def fromEnd(region, size):
        """
        >>> a = Region("", 0, 1024)
        >>> b = Region.fromEnd(a, 256)
        >>> b.start
        768
        >>> b.end
        1024
        >>> b.size
        256
        """
        return Region("", region.end - size, region.end)

    @staticmethod
    def fromStart(region, size):
        """
        >>> a = Region("", 0, 1024)
        >>> b = Region.fromStart(a, 512)
        >>> b.start
        0
        >>> b.end
        512
        >>> b.size
        512
        """
        return Region("", region.start, region.start + size)


class Layout:
    def __init__(self, region):
        self.region = region
        self.free = self.region.size
        self.subregions = []

    def __repr__(self):
        return f"<Flash at {self.region} with {len(self.subregions)} subregions>"

    def __getitem__(self, name):
        for region in self.subregions:
            if name == region.name:
                return region

        return None

    @property
    def name(self):
        return self.region.name

    @property
    def size(self):
        return self.region.size

    @property
    def used(self):
        size = Bytes(0)
        for region in self.subregions:
            size += region.size

        return size

    @property
    def start(self):
        return self.region.start

    @property
    def end(self):
        return self.region.end

    @property
    def edge(self):
        return (
            self.subregions[-1] if self.subregions else Region("", self.end, self.end)
        )

    def push(self, region):
        """
        >>> x = Layout(Region("", 0, 1024))
        >>> x.push(Region("", 1024, 2048))
        Traceback (most recent call last):
            ...
        ValueError: Out of bounds of <Region self.name='' self.size=1024 at [0x00000000:0x00000400)>
        >>> x.push(Region("", -512, 0))
        Traceback (most recent call last):
            ...
        ValueError: Out of bounds of <Region self.name='' self.size=1024 at [0x00000000:0x00000400)>
        >>> x.add("", 512)
        <Region self.name='' self.size=512 at [0x00000200:0x00000400)>
        >>> x.push(Region("", 512, 768))
        Traceback (most recent call last):
            ...
        ValueError: Region is located before self.edge=<Region self.name='' self.size=512 at [0x00000200:0x00000400)>
        >>> x.add("", 1024)
        Traceback (most recent call last):
            ...
        ValueError: No space left
        """
        if self.free < region.size:
            raise ValueError(f"No space left")
        elif self.start > region.start or self.end < region.end:
            raise ValueError(f"Out of bounds of {self.region}")
        elif self.subregions and region.end > self.edge.start:
            raise ValueError(f"Region is located before {self.edge=}")

        self.free = self.free - region.size
        self.subregions.append(region)

        return region

    def add(self, name, size):
        """
        >>> x = Layout(Region("", 0, 1024))
        >>> x.add("zero", 0)
        <Region self.name='zero' self.size=0 at [0x00000400:0x00000400)>
        >>> x.add("one", 512)
        <Region self.name='one' self.size=512 at [0x00000200:0x00000400)>
        >>> x.add("two", 512)
        <Region self.name='two' self.size=512 at [0x00000000:0x00000200)>
        >>> x.add("three", 512)
        Traceback (most recent call last):
            ...
        ValueError: No space left
        """
        if self.subregions:
            region = Region(name, self.edge.start - size, self.edge.start)
        else:
            region = Region(name, self.end - size, self.end)

        return self.push(region)

    def add_aligned(self, name, size, alignment):
        """
        >>> x = Layout(Region("", 0, Megabytes(1)))
        >>> x.add("", Kilobytes(20))
        <Region self.name='' self.size=20480 at [0x000fb000:0x00100000)>
        >>> r = x.add_aligned("", Kilobytes(512), Kilobytes(8))
        >>> r
        <Region self.name='' self.size=524288 at [0x0007b000:0x000fb000)>
        >>> r.size % Kilobytes(8)
        0
        >>> x.add("", Kilobytes(1))
        <Region self.name='' self.size=1024 at [0x0007ac00:0x0007b000)>
        >>> x.add_aligned("", Kilobytes(20), Kilobytes(8))
        <Region self.name='' self.size=16384 at [0x00075c00:0x00079c00)>
        """
        end = self.edge.copy()

        remainder = size % alignment
        if remainder:
            self.add("", remainder)
            size -= remainder

        return self.add(name, size)


class Filesystem:
    """Simple wrapper around Region to supply block and page sizes."""

    def __init__(self, region, block_size, page_size):
        self.region = region
        self.block_size = block_size
        self.page_size = page_size

    def __bool__(self):
        return self.region.size > 0

    @property
    def size(self):
        return self.region.size

    @property
    def start(self):
        return self.region.start

    @property
    def end(self):
        return self.region.end

    def __repr__(self):
        return f"<Filesystem at {self.region} block {self.block_size} page {self.page_size}>"


# - menu properties *will* be used in scripts, so these suffixes *must* remain the same
#   flash size uses short uppercase suffix (without the B)
#   fs size only adds suffix when >= 1M, no string when no fs
#   plus, if must be >0, otherwise things won't work :)
# - .ld script *names* could be used in scripts, so suffixes *must* remain the same
#   flash size uses short lowercase suffix (without the b)
#   fs size only adds suffix when >= 1M, no string when no fs


def humanize_fs(size):
    """Filesystem size in properties and file names.

    Only suffixed when in greater than 1 megabyte.
    Bytes() mostly stay unused.

    >>> humanize_fs(Bytes(1))
    '1'
    >>> humanize_fs(Kilobytes(64))
    '64'
    >>> humanize_fs(Megabytes(2))
    '2M'
    """
    convert = [
        [Bytes(1), ""],
        [Kilobytes(1), ""],
        [Megabytes(1), "M"],
    ]

    return humanize(size, convert=convert)


def humanize_flash(size):
    """Flash size in properties and file names.

    >>> humanize_flash(Bytes(64))
    '64'
    >>> humanize_flash(Kilobytes(512))
    '512K'
    >>> humanize_flash(Megabytes(4))
    '4M'
    """
    convert = [
        [Bytes(1), ""],
        [Kilobytes(1), "K"],
        [Megabytes(1), "M"],
    ]

    return humanize(size, convert=convert)


def humanize_flash_menu(size):
    """Menu entry in the IDE.

    >>> humanize_flash_menu(Bytes(128))
    '128'
    >>> humanize_flash_menu(Kilobytes(512))
    '512KB'
    >>> humanize_flash_menu(Megabytes(16))
    '16MB'
    """
    convert = [
        [Bytes(1), ""],
        [Kilobytes(1), "KB"],
        [Megabytes(1), "MB"],
    ]

    return humanize(size, convert=convert)


# used as property value and for .ld output generator
def ldscript_name(*, flash_size, expected_fs_size, **kwargs):
    return f"eagle.flash.{humanize_flash(flash_size)}{humanize_fs(expected_fs_size)}.ld".lower()


SPI_START = 0x40200000
SPI_SECTOR = Kilobytes(4)


# ref. https://github.com/esp8266/esp8266-wiki/wiki/Memory-Map
# bootloader | crc | sketch | empty | fs | eeprom | rf-cal | sdk-wifi-settings
def common_layout(name, flash_size):
    # notice that we start from 0 instead of SPI start, menu needs relative addresses
    # (which we then simply add where it is populated)
    layout = Layout(Region(name, 0, flash_size))

    # these are *always* at the end of the flash, so layout is populated backwards
    sdkwifi = layout.add("SDK + WiFi", (SPI_SECTOR * 3))
    rfcal = layout.add("RFCAL", SPI_SECTOR)

    # eeprom sector is always there, since we don't know whether it is used or not
    eeprom = layout.add("EEPROM", SPI_SECTOR)

    return layout


def flash_map(flash_size, fs_size=Bytes(0), name=""):
    """Generate template variables for the specified flash and filesystem sizes.
    Name is optional and is simply passed through.

    >>> x = flash_map(Megabytes(2), Kilobytes(64), 'Test')
    >>> x['flash_map_name']
    'Test'
    >>> x['layout']['Filesystem'].size
    45056
    >>> import pprint
    >>> pprint.pprint(x['layout'].subregions)
    [<Region self.name='SDK + WiFi' self.size=12288 at [0x001fd000:0x00200000)>,
     <Region self.name='RFCAL' self.size=4096 at [0x001fc000:0x001fd000)>,
     <Region self.name='EEPROM' self.size=4096 at [0x001fb000:0x001fc000)>,
     <Region self.name='Filesystem' self.size=45056 at [0x001f0000:0x001fb000)>,
     <Region self.name='Empty' self.size=983040 at [0x00100000:0x001f0000)>,
     <Region self.name='Sketch' self.size=1044464 at [0x00001010:0x00100000)>,
     <Region self.name='' self.size=8 at [0x00001008:0x00001010)>,
     <Region self.name='CRC' self.size=8 at [0x00001000:0x00001008)>,
     <Region self.name='Bootloader' self.size=4088 at [0x00000008:0x00001000)>,
     <Region self.name='' self.size=8 at [0x00000000:0x00000008)>]
    """

    layout = common_layout(name, flash_size)

    if fs_size:
        if fs_size < Kilobytes(512):
            fs_block_size = Kilobytes(4)
        else:
            fs_block_size = Kilobytes(8)
        fs_page_size = Bytes(256)
    else:
        fs_page_size = Bytes(0)
        fs_block_size = Bytes(0)

    # maintaining backwards compatibility, address is based
    # on the *end of flash*, not using available space at the edge
    # (e.g. with 2M64, we end up with only 44K of space)
    expected_fs_size = fs_size
    if fs_size and flash_size > Megabytes(1):
        fs_size = fs_size - layout.used

    if fs_size:
        fs = Filesystem(
            layout.add_aligned("Filesystem", fs_size, fs_block_size),
            fs_block_size,
            fs_page_size,
        )
    else:
        fs = None

    # at this point, we either left with empty + app or just the app space
    # we could create an app up-most to 1mb, so the rest of the space is empty
    nearest = layout.edge
    if layout.free > Megabytes(1):
        empty = layout.push(Region("Empty", layout.start + Megabytes(1), nearest.start))
        nearest = empty
    else:
        empty = None

    # ref. elf2bin.py, 1st flash sector is
    # - 8 bytes of header
    # - bootloader until the end of the sector
    # 2nd flash sector starts with
    # - 8 bytes of header
    # - 4 bytes of crc len
    # - 4 bytes of crc value
    reserved = SPI_SECTOR + Bytes(8) + Bytes(4) + Bytes(4)
    sketch = layout.add("Sketch", nearest.start - (layout.start + reserved))

    layout.add("", Bytes(8))
    layout.add("CRC", Bytes(4) + Bytes(4))
    layout.add("Bootloader", SPI_SECTOR - Bytes(8))
    layout.add("", Bytes(8))

    assert layout.free == 0

    max_upload_size = min(Megabytes(1), flash_size) - reserved
    if empty:
        max_ota_size = empty.size
    else:
        max_ota_size = min(max_upload_size, int(sketch.size / 2))

    return {
        "layout": layout,
        "flash_size": layout.region.size,
        "flash_map_name": layout.region.name,
        "sdkwifi": layout["SDK + WiFi"],
        "rfcal": layout["RFCAL"],
        "eeprom": layout["EEPROM"],
        "empty": empty,
        "fs": fs,
        "expected_fs_size": expected_fs_size,
        "sketch": sketch,
        "max_upload_size": max_upload_size,
        "max_ota_size": max_ota_size,
    }


def menu_generate(
    *, max_ota_size, max_upload_size, rfcal, flash_size, fs, expected_fs_size, **kwargs
):
    menu = f".menu.eesz.{humanize_flash(flash_size)}{humanize_fs(expected_fs_size)}"

    out = [
        (
            menu,
            f'{humanize_flash_menu(flash_size)} (FS:{humanize(fs.size) if fs else "none"} OTA:~{humanize(max_ota_size)})',
        ),
        (f"{menu}.build.flash_size", humanize_flash(flash_size)),
        (
            f"{menu}.build.flash_ld",
            ldscript_name(flash_size=flash_size, expected_fs_size=expected_fs_size),
        ),
        (f"{menu}.build.rfcal_addr", f"0x{rfcal.start:05X}"),
    ]

    out.append((f"{menu}.upload.maximum_size", f"{max_upload_size}"))

    if fs:
        out.extend(
            (
                (f"{menu}.build.spiffs_start", f"0x{fs.start:05X}"),
                (f"{menu}.build.spiffs_end", f"0x{fs.end:05X}"),
                (f"{menu}.build.spiffs_blocksize", fs.block_size),
                (f"{menu}.build.spiffs_pagesize", fs.page_size),
            )
        )

    return collections.OrderedDict(out)


def menu_macros(flash_maps):
    output = {
        "autoflash": collections.OrderedDict(
            [
                (".menu.eesz.autoflash", "Mapping defined by Hardware and Sketch"),
                (".menu.eesz.autoflash.build.flash_size", "16M"),
                (".menu.eesz.autoflash.build.flash_ld", "eagle.flash.auto.ld"),
                (".menu.eesz.autoflash.build.extra_flags", "-DFLASH_MAP_SUPPORT=1"),
                (".menu.eesz.autoflash.upload.maximum_size", "1044464"),
            ]
        )
    }

    for flash_map in flash_maps:
        size = humanize_flash(flash_map["layout"].size)
        menu = menu_generate(**flash_map)

        if size in output:
            output[size].update(menu)
        else:
            output[size] = collections.OrderedDict(menu)

    return output


def ldscript_generate(
    output,
    *,
    layout,
    max_upload_size,
    sdkwifi,
    rfcal,
    eeprom,
    fs,
    empty,
    sketch,
    **kwargs,
):
    def address(value):
        return f"0x{SPI_START + value.start:08X}"

    def symbol(value):
        return f"0x{SPI_START + value:08X}"

    def size(value):
        return f"{humanize(value.size)}"

    if not fs:
        fs = Filesystem(Region("", eeprom.start, eeprom.start), 0, 0)

    with output:
        print(f"/* Flash Split for {size(layout)} chips */")
        print(f"/* sketch @{address(sketch)} (~{size(sketch)}) ({sketch.size}B) */")
        if empty:
            print(f"/* empty  @{address(empty)} (~{size(empty)}) ({empty.size}B) */")
        if fs:
            print(f"/* fs     @{address(fs)} (~{size(fs)}) ({fs.size}B) */")
        print(f"/* eeprom @{address(eeprom)} ({eeprom.size}B) */")
        print(f"/* rfcal  @{address(rfcal)} ({rfcal.size}B) */")
        print(f"/* wifi   @{address(sdkwifi)} ({sdkwifi.size}B) */")
        print()
        print("MEMORY")
        print("{")
        print("  dport0_0_seg :                        org = 0x3FF00000, len = 0x10")
        print("  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000")
        print(
            f"  irom0_0_seg :                         org = {address(sketch)}, len = 0x{sketch.size:x}"
        )
        print("}")
        print()
        print(f"PROVIDE ( _FS_start = {symbol(fs.start)} );")
        print(f"PROVIDE ( _FS_end = {symbol(fs.end)} );")
        print(f"PROVIDE ( _FS_page = 0x{fs.page_size:X} );")
        print(f"PROVIDE ( _FS_block = 0x{fs.block_size:X} );")
        print(f"PROVIDE ( _EEPROM_start = {symbol(eeprom.start)} );")
        # Re-add deprecated symbols pointing to the same address as the new standard ones
        print(
            "/* The following symbols are DEPRECATED and will be REMOVED in a future release */"
        )
        print(f"PROVIDE ( _SPIFFS_start = {symbol(fs.start)} );")
        print(f"PROVIDE ( _SPIFFS_end = {symbol(fs.end)} );")
        print(f"PROVIDE ( _SPIFFS_page = 0x{fs.page_size:X} );")
        print(f"PROVIDE ( _SPIFFS_block = 0x{fs.block_size:X} );")
        print()
        print('INCLUDE "local.eagle.app.v6.common.ld"')


def flashmap_generate(output, flash_maps):
    """
    >>> flashmap_generate(contextlib.nullcontext(), (flash_map(Megabytes(1), Kilobytes(512), 'TEST'), ))
    // - DO NOT EDIT - autogenerated by boards.txt.py
    <BLANKLINE>
    #pragma once
    <BLANKLINE>
    #include <stdint.h>
    #include <stddef.h>
    <BLANKLINE>
    typedef struct
    {
        uint32_t eeprom_start;
        uint32_t fs_start;
        uint32_t fs_end;
        uint32_t fs_block_size;
        uint32_t fs_page_size;
        uint32_t flash_size_kb;
    } flash_map_s;
    <BLANKLINE>
    /*
      Following definitions map the above structure, one per line.
      FLASH_MAP_* is a user choice in sketch:
          `FLASH_MAP_SETUP_CONFIG(FLASH_MAP_OTA_FS)`
      Configuration is made at boot with detected flash chip size (last argument 512..16384)
      Other values are defined from `tools/boards.txt.py`.
    */
    <BLANKLINE>
    #define FLASH_MAP_TEST \\
        { \\
            { .eeprom_start = 0x402fb000, .fs_start = 0x4027b000, .fs_end = 0x402fb000, .fs_block_size = 0x2000, .fs_page_size = 0x100, .flash_size_kb = 1024 }, \\
        }
    <BLANKLINE>
    """

    def as_address(value):
        return f"0x{SPI_START + value:08x}"

    def as_hex(value):
        return f"0x{value:x}"

    def as_dec(value):
        return f"{value:d}"

    # note that the current header version *only* has the values
    # previously PROVIDEd by the .ld script
    # forward those names to the correct object values
    def field_values(*, layout, eeprom, fs, **kwards):
        values = {
            "name": layout.name,
            "eeprom_start": eeprom.start,
            "flash_size_kb": int(layout.size / KILOBYTES),
        }

        if fs:
            values.update(
                {
                    "fs_start": fs.start,
                    "fs_end": fs.end,
                    "fs_block_size": fs.block_size,
                    "fs_page_size": fs.page_size,
                }
            )
        else:
            values.update(
                {
                    "fs_start": eeprom.start,
                    "fs_end": eeprom.start,
                    "fs_block_size": 0,
                    "fs_page_size": 0,
                }
            )

        return values

    fields = [
        ["eeprom_start", as_address],
        ["fs_start", as_address],
        ["fs_end", as_address],
        ["fs_block_size", as_hex],
        ["fs_page_size", as_hex],
        ["flash_size_kb", as_dec],
    ]

    with output:
        print("// - DO NOT EDIT - autogenerated by boards.txt.py")
        print()
        print("#pragma once")
        print()
        print("#include <stdint.h>")
        print("#include <stddef.h>")
        print()
        print("typedef struct")
        print("{")
        for field, _ in fields:
            print(f"    uint32_t {field};")
        print("} flash_map_s;")
        print()
        print("/*")
        print("  Following definitions map the above structure, one per line.")
        print("  FLASH_MAP_* is a user choice in sketch:")
        print("      `FLASH_MAP_SETUP_CONFIG(FLASH_MAP_OTA_FS)`")
        print(
            "  Configuration is made at boot with detected flash chip size (last argument 512..16384)"
        )
        print("  Other values are defined from `tools/boards.txt.py`.")
        print("*/")
        print()

        ordered_maps = collections.OrderedDict([])
        for flash_map in flash_maps:
            values = field_values(**flash_map)
            if not values["name"]:
                continue

            name = values["name"]
            if not ordered_maps.get(name):
                ordered_maps[name] = []

            line = ", ".join(
                f".{field} = {mod(values[field])}" if mod else values[field]
                for field, mod in fields
            )
            ordered_maps[name].append(f"{{ {line} }}, \\")

        for name, lines in ordered_maps.items():
            print(f"#define FLASH_MAP_{name.upper()} \\")
            print("    { \\")
            for line in lines:
                print(f"        {line}")
            print("    }")
            print()


def all_flash_maps():
    # flash_map(name=...) is optional. when it is set,
    # we will generate a named FlashMap.h entry
    return (
        flash_map(Megabytes(1), Kilobytes(64), "OTA_FS"),
        flash_map(Megabytes(1), Kilobytes(128)),
        flash_map(Megabytes(1), Kilobytes(144)),
        flash_map(Megabytes(1), Kilobytes(160)),
        flash_map(Megabytes(1), Kilobytes(192)),
        flash_map(Megabytes(1), Kilobytes(256)),
        flash_map(Megabytes(1), Kilobytes(512), "MAX_FS"),
        flash_map(Megabytes(1), Kilobytes(0), "NO_FS"),
        flash_map(Megabytes(2), Kilobytes(64)),
        flash_map(Megabytes(2), Kilobytes(128)),
        flash_map(Megabytes(2), Kilobytes(256), "OTA_FS"),
        flash_map(Megabytes(2), Kilobytes(512)),
        flash_map(Megabytes(2), Megabytes(1), "MAX_FS"),
        flash_map(Megabytes(2), Kilobytes(0), "NO_FS"),
        flash_map(Megabytes(4), Megabytes(2), "OTA_FS"),
        flash_map(Megabytes(4), Megabytes(3), "MAX_FS"),
        flash_map(Megabytes(4), Megabytes(1)),
        flash_map(Megabytes(4), Kilobytes(0), "NO_FS"),
        flash_map(Megabytes(8), Megabytes(6), "OTA_FS"),
        flash_map(Megabytes(8), Megabytes(7), "MAX_FS"),
        flash_map(Megabytes(8), Kilobytes(0), "NO_FS"),
        flash_map(Megabytes(16), Megabytes(14), "OTA_FS"),
        flash_map(Megabytes(16), Megabytes(15), "MAX_FS"),
        flash_map(Megabytes(16), Kilobytes(0), "NO_FS"),
        flash_map(Kilobytes(512), Kilobytes(32), "OTA_FS"),
        flash_map(Kilobytes(512), Kilobytes(64)),
        flash_map(Kilobytes(512), Kilobytes(128), "MAX_FS"),
        flash_map(Kilobytes(512), Kilobytes(0), "NO_FS"),
    )


################################################################
# builtin led


def led(name, default, ledList):
    menu = collections.OrderedDict(
        (
            (f".menu.led.{default}", str(default)),
            (f".menu.led.{default}.build.led", f"-DLED_BUILTIN={default}"),
        )
    )
    for (
        led
    ) in (
        ledList
    ):  # Make range incluside of max (16), since there are really 16 GPIOS not 15
        if not led == default:
            menu.update(
                (
                    (f".menu.led.{led}", str(led)),
                    (f".menu.led.{led}.build.led", f"-DLED_BUILTIN={led}"),
                )
            )
    return {name: menu}


################################################################
# sdk selection


def sdk_macros():
    return {
        "sdk": collections.OrderedDict(
            [
                (".menu.sdk.nonosdk_190703", "nonos-sdk 2.2.1+100 (190703)"),
                (".menu.sdk.nonosdk_190703.build.sdk", "NONOSDK22x_190703"),
                (".menu.sdk.nonosdk_191122", "nonos-sdk 2.2.1+119 (191122)"),
                (".menu.sdk.nonosdk_191122.build.sdk", "NONOSDK22x_191122"),
                (".menu.sdk.nonosdk_191105", "nonos-sdk 2.2.1+113 (191105)"),
                (".menu.sdk.nonosdk_191105.build.sdk", "NONOSDK22x_191105"),
                (".menu.sdk.nonosdk_191024", "nonos-sdk 2.2.1+111 (191024)"),
                (".menu.sdk.nonosdk_191024.build.sdk", "NONOSDK22x_191024"),
                (".menu.sdk.nonosdk_190313", "nonos-sdk 2.2.1+61 (190313)"),
                (".menu.sdk.nonosdk_190313.build.sdk", "NONOSDK22x_190313"),
                (".menu.sdk.nonosdk221", "nonos-sdk 2.2.1 (legacy)"),
                (".menu.sdk.nonosdk221.build.sdk", "NONOSDK221"),
                (".menu.sdk.nonosdk3v0", "nonos-sdk pre-3 (180626 known issues)"),
                (".menu.sdk.nonosdk3v0.build.sdk", "NONOSDK3V0"),
            ]
        )
    }


################################################################


def abridged_boards_txt(filtered, boards):
    def handler():
        if filtered:
            print(
                "# The following boards were included: {}".format(
                    " ".join(boards.keys())
                )
            )

    return handler


def custom_speeds_boards_txt(speeds):
    def handler(name):
        for speed in speeds:
            print(f"{name}.menu.baud.{speed}={speed}")
            print(f"{name}.menu.baud.{speed}.upload.speed={speed}")

    return handler


def no_float_boards_txt(disabled):
    def handler(name):
        if disabled:
            print(f"{name}.build.float=")

    return handler


def prepare_macros(defaults, flashmap, builtinled):
    macros = defaults
    macros.update(menu_macros(flashmap))
    macros.update(debug_macros())
    macros.update(led("led", builtinled, range(0, 17)))
    macros.update(led("led216", 2, (16,)))
    macros.update(sdk_macros())

    return macros


def boards_generate(output, boards, macros, extra_header=[], extra_board=[]):
    with output:
        print("# DO NOT EDIT - autogenerated by boards.txt.py")
        print(f'# Command line `{" ".join(sys.argv)}`')
        for func in extra_header:
            func()
        print()
        # With Arduino IDE 1.8.7 the order of the menu items will be honored from the tools pull down list.
        print("menu.BoardModel=Model")
        print("menu.ESPModule=Module")
        print("menu.UploadTool=Upload Tool")
        print("menu.led=Builtin Led")
        print("menu.baud=Upload Speed")
        print("menu.xtal=CPU Frequency")
        print("menu.CrystalFreq=Crystal Frequency")
        print("menu.eesz=Flash Size")
        print("menu.FlashMode=Flash Mode")
        print("menu.FlashFreq=Flash Frequency")
        print("menu.ResetMethod=Reset Method")
        print("menu.dbg=Debug port")
        print("menu.lvl=Debug Level")
        print("menu.ip=lwIP Variant")
        print("menu.vt=VTables")
        print("menu.exception=C++ Exceptions")
        print("menu.stacksmash=Stack Protection")
        print("menu.wipe=Erase Flash")
        print("menu.sdk=Espressif FW")
        print("menu.ssl=SSL Support")
        print("menu.mmu=MMU")
        print("menu.non32xfer=Non-32-Bit Access")
        print()

        for name, board in boards.items():
            print("#" * 62)
            print(f'{name}.name={board["name"]}')

            # standalone options
            if "opts" in board:
                for optname in sorted(board["opts"]):
                    print(f'{name}{optname}={board["opts"][optname]}')

            # macros
            macrolist = [
                "defaults",
                "cpufreq_menu",
                "vtable_menu",
                "exception_menu",
                "stacksmash_menu",
                "ssl_cipher_menu",
                "mmu_menu",
                "non32xfer_menu",
            ]
            if "macro" in board:
                macrolist += board["macro"]
            macrolist += ["lwip", "debug_menu", "flash_erase_menu"]

            macrolist += SERIAL_SPEEDS[board.get("serial", DEFAULT_SERIAL_SPEED)]
            macrolist += ["autoflash"]

            for func in extra_board:
                func(name)

            for block in macrolist:
                for optname in macros[block]:
                    if not ("opts" in board) or not (optname in board["opts"]):
                        print(f"{name}{optname}={macros[block][optname]}")

            print()


def filtered_boards(boards, path, action=operator.sub):
    filters = set()

    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            if line.startswith("#"):
                continue
            board, _, desc = line.partition("#")
            board = board.strip()
            filters.add(board)

    filtered = action(set(boards.keys()), filters)

    print("Filtered boards list:")
    print(" ".join(filtered))
    print()

    missing = [name for name in filtered if name not in boards.keys()]
    if missing:
        print("No board definitions were found for the following boards:")
        print(" ".join(missing))
        print("")

    result = collections.OrderedDict()
    for name, board in boards.items():
        if name in filtered:
            result[name] = board

    return result


def prepare_boards(boards, required_boards):
    boardslist = set(required_boards) | set(boards.keys())
    boardslist = boardslist - set(required_boards)
    boardslist = required_boards + tuple(
        sorted(boardslist, key=lambda x: boards[x]["name"])
    )

    out = collections.OrderedDict()
    for name in boardslist:
        out[name] = boards[name]

    return out


def show_names(boards):
    print("# Available board names. Delete or comment out the boards you do not need:")
    for name, board in boards.items():
        print(f'{name: <20s} # {board["name"]}')


################################################################


def package_generate(output, boards):
    with output:
        print(
            json.dumps(
                [{"name": board["name"]} for board in boards.values()],
                indent=3,
                separators=(",", ": "),
            )
        )


################################################################


def doc_generate(output, boards):
    with output:
        print("Boards")
        print("=" * len("Boards"))
        print()

        for board in boards.values():
            print(board["name"])
            print("-" * len(board["name"]))
            print()
            for line in board.get("desc", []):
                print(line)
            print()


################################################################
# entrypoint

GENERATORS = (
    ("boards", "boards-file", "boards.txt", "boards.txt"),
    ("ld", "ld-dir", "tools/sdk/ld", ".ld scripts"),
    ("flashmap", "flashmap-file", "cores/esp8266/FlashMap.h", "FlashMap header"),
    (
        "package",
        "package-file",
        "package/package_esp8266com_index.boards.json",
        "IDE package index boards list (.json)",
    ),
    ("doc", "doc-file", "doc/boards.rst", "Boards documentation (.rst)"),
)


def parse_cmdline():
    parser = argparse.ArgumentParser(description="File generator for esp8266/Arduino")

    generic = parser.add_argument_group(title="Generic options")

    generic.add_argument(
        "--led",
        type=int,
        default=2,
        help="default PIN for builtin LED, specified for generic boards (default %(default)d)",
    )
    generic.add_argument(
        "--no-float",
        action="store_true",
        help="disable float support in printf and scanf (enabled by default)",
    )
    generic.add_argument(
        "--custom-speed",
        action="append",
        default=[],
        help="additional serial speed option for all boards. can be specified multiple times.",
    )

    filters = parser.add_argument_group(
        "Filtering", "either INCLUDE or EXCLUDE boards based on the filter file"
    ).add_mutually_exclusive_group()
    filters.add_argument("--include", nargs="?")
    filters.add_argument("--exclude", nargs="?")

    subparsers = parser.add_subparsers(dest="command", help="sub-commands")
    subparsers.required = True

    names = subparsers.add_parser(
        "names", help="prints a list of all available board names"
    )
    test = subparsers.add_parser("test", help="run a doctest self-check")

    generate = subparsers.add_parser(
        "generate",
        help="generate file(s). pass --all to enable all or --<name> to enable specific generator.",
    )

    outputs = generate.add_argument_group(
        "Output destination",
        "Either use STDOUT or write to the output file(s). "
        "Optionally, when output file(s) already exist, preserve the original with .orig extension.",
    )
    outputs.add_argument(
        "--output",
        choices=["stdout", "file", "file-with-backup"],
        default="file",
        help='(default "%(default)s")',
    )

    use_all = generate.add_argument_group(title="Use all available generators")
    use_all.add_argument(
        "--all",
        dest="generators",
        action="store_const",
        const=[name for name, _, _, _ in GENERATORS],
    )

    for name, output, default, title in GENERATORS:
        group = generate.add_argument_group(title=title)
        group.add_argument(
            f"--{name}", dest="generators", action="append_const", const=name
        )
        group.add_argument(
            f"--{output}", default=default, help='(default "%(default)s")'
        )

    return parser.parse_args()


def run_generators(boards, filter_path, args):
    generators = set(name for name in args.generators or [])
    if not generators:
        raise ValueError("no generators selected")

    def maybe_output(file):
        if args.output == "file-with-backup":
            return OpenWithBackupFile(file)
        if args.output == "file":
            return Open(file)

        return contextlib.nullcontext()

    if "boards" in generators:
        boards_generate(
            maybe_output(args.boards_file),
            boards,
            macros=prepare_macros(MACROS, all_flash_maps(), args.led),
            extra_header=[
                abridged_boards_txt(filter_path, boards),
            ],
            extra_board=[
                no_float_boards_txt(args.no_float),
                custom_speeds_boards_txt(args.custom_speed),
            ],
        )

    if "ld" in generators:
        for flash_map in all_flash_maps():
            file = os.path.join(args.ld_dir, ldscript_name(**flash_map))
            ldscript_generate(maybe_output(file), **flash_map)

    if "flashmap" in generators:
        flashmap_generate(maybe_output(args.flashmap_file), all_flash_maps())

    if "package" in generators:
        package_generate(maybe_output(args.package_file), boards)

    if "doc" in generators:
        doc_generate(maybe_output(args.doc_file), boards)


def main():
    args = parse_cmdline()

    if args.command == "test":
        doctest.testmod()
        return

    boards = prepare_boards(BOARDS, REQUIRED_BOARDS)

    filter_path = args.include or args.exclude
    if filter_path:
        action = operator.and_ if args.include else operator.sub
        boards = filtered_boards(boards, filter_path, action)

    if args.command == "names":
        show_names(boards)
        return

    if args.command == "generate":
        run_generators(boards, filter_path, args)
        return


if __name__ == "__main__":
    main()
