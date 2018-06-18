#!/usr/bin/env python

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
#            512K/1M/2M/4M/8M/16M:       menus for flash & SPIFFS size
#            lwip/lwip2                  menus for available lwip versions

import os
import sys
import collections
import getopt
import re

# serial upload speed order in menu
# default is 115 for every board unless specified with 'serial' in board
# or by user command line

speeds = collections.OrderedDict([
    (   '9', [ 's9', 's57', 's115', 's230', 's256', 's460', 's512', 's921' ]),
    (  '57', [ 's57', 's9', 's115', 's230', 's256', 's460', 's512', 's921' ]),
    ( '115', [ 's115', 's9', 's57', 's230', 's256', 's460', 's512', 's921' ]),
    ( '230', [ 's230', 's9', 's57', 's115', 's256', 's460', 's512', 's921' ]),
    ( '256', [ 's256', 's9', 's57', 's115', 's230', 's460', 's512', 's921' ]),
    ( '460', [ 's460', 's9', 's57', 's115', 's230', 's256', 's512', 's921' ]),
    ( '512', [ 's512', 's9', 's57', 's115', 's230', 's256', 's460', 's921' ]),
    ( '921', [ 's921', 's9', 's57', 's115', 's230', 's256', 's460', 's512' ]),
    ])

# boards list

boards = collections.OrderedDict([
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
            '512K', '1M', '2M', '4M', '8M', '16M',
            'led',
            ],
        'desc': [ 'These modules come in different form factors and pinouts. See the page at ESP8266 community wiki for more info: `ESP8266 Module Family <http://www.esp8266.com/wiki/doku.php?id=esp8266-module-family>`__.',
                  '',
                  'Usually these modules have no bootstapping resistors on board, insufficient decoupling capacitors, no voltage regulator, no reset circuit, and no USB-serial adapter. This makes using them somewhat tricky, compared to development boards which add these features.',
                  '',
                  'In order to use these modules, make sure to observe the following:',
                  '',
                  '-  **Provide sufficient power to the module.** For stable use of the ESP8266 a power supply with 3.3V and >= 250mA is required. Using the power available from USB to Serial adapter is not recommended, these adapters typically do not supply enough current to run ESP8266 reliably in every situation. An external supply or regulator alongwith filtering capacitors is preferred.',
                  '',
                  '-  **Connect bootstapping resistors** to GPIO0, GPIO2, GPIO15 according to the schematics below.',
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
            },
        'macro': [
            'resetmethod_menu',
            'resetmethod_menu_extra',
            'crystalfreq_menu',
            'flashmode_dout',
            'flashfreq_40',
            '1M',
            'led',
            ],
        'desc': [ 'ESP8285 (`datasheet <http://www.espressif.com/sites/default/files/0a-esp8285_datasheet_en_v1.0_20160422.pdf>`__) is a multi-chip package which contains ESP8266 and 1MB flash. All points related to bootstrapping resistors and recommended circuits listed above apply to ESP8285 as well.',
                  '',
                  'Note that since ESP8285 has SPI flash memory internally connected in DOUT mode, pins 9 and 10 may be used as GPIO / I2C / PWM pins.',
                  ],
    }),
    ( 'espduino', {
        'name': 'ESPDuino (ESP-13 Module)',
        'opts': collections.OrderedDict([
            ( '.build.board', 'ESP8266_ESP13' ),
            ( '.build.variant', 'ESPDuino' ),
            ( '.menu.ResetMethod.v2', 'ESPduino-V2' ),
            ( '.menu.ResetMethod.v2.upload.resetmethod', 'nodemcu' ),
            ( '.menu.ResetMethod.v1', 'ESPduino-V1' ),
            ( '.menu.ResetMethod.v1.upload.resetmethod', 'ck' ),
            ( '.menu.UploadTool.esptool', 'Serial' ),
            ( '.menu.UploadTool.esptool.upload.tool', 'esptool' ),
            ( '.menu.UploadTool.esptool.upload.verbose', '-vv' ),
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
            '.build.board': 'ESP8266_ESP12',
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
            '.build.board': 'ESP8266_NODEMCU',
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
            '.build.board': 'ESP8266_NODEMCU',
            '.build.variant': 'nodemcu',
            },
        'macro': [
            'resetmethod_nodemcu',
            'flashmode_dio',
            'flashfreq_40',
            '4M',
            ],
        'desc': [ 'This module is sold under many names for around $6.50 on AliExpress and it\'s one of the cheapest, fully integrated ESP8266 solutions.',
                  '',
                  'It\'s an open hardware design with an ESP-12E core and 4 MB of SPI flash.',
                  '',
                  'According to the manufacturer, "with a micro USB cable, you can connect NodeMCU devkit to your laptop and flash it without any trouble". This is more or less true: the board comes with a CP2102 onboard USB to serial adapter which just works, well, the majority of the time. Sometimes flashing fails and you have to reset the board by holding down FLASH +',
                  'RST, then releasing FLASH, then releasing RST. This forces the CP2102 device to power cycle and to be re-numbered by Linux.',
                  '',
                  'The board also features a NCP1117 voltage regulator, a blue LED on GPIO16 and a 220k/100k Ohm voltage divider on the ADC input pin.',
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
            'resetmethod_ck',
            'flashmode_qio',
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
        'name': 'WeMos D1 R2 & mini',
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
    ( 'd1_mini_pro', {
        'name': 'WeMos D1 mini Pro',
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
        'name': 'WeMos D1 mini Lite',
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
			'~~~~~~~~~~~~~~~~~~~~~~~~~',
			'',
			'- Card: "WEMOS D1 Mini Lite"',
			'- Flash Size: "1M (512K SPIFFS)"',
			'- CPU Frequency: "80 Mhz"',
			'- Upload Speed: "230400"',
			'',
			'Power:',
			'~~~~~',
			'',
			'- 5V pin : 4.7V 500mA output when the board is powered by USB ; 3.5V-6V input',
			'- 3V3 pin : 3.3V 500mA regulated output',
			'- Digital pins : 3.3V 30mA.',
			'',
			'links:',
			'~~~~~',
			'',
			'- Product page: https://www.wemos.cc/',
			'- Board schematic: https://wiki.wemos.cc/_media/products:d1:sch_d1_mini_lite_v1.0.0.pdf',
			'- ESP8285 datasheet: https://www.espressif.com/sites/default/files/0a-esp8285_datasheet_en_v1.0_20160422.pdf',
			'- Voltage regulator datasheet: http://pdf-datasheet.datasheet.netdna-cdn.com/pdf-down/M/E/6/ME6211-Microne.pdf',
        ],
    }),
    ( 'd1', {
        'name': 'WeMos D1 R1',
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
            '.build.board': 'ESP8266_ESP12',
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
            '.build.board': 'ESP8266_ESP13',
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
        'opts': {
            '.build.board': 'WIFINFO',
            '.build.variant': 'wifinfo',
            '.menu.ESPModule.ESP07192': 'ESP07 (1M/192K SPIFFS)',
            '.menu.ESPModule.ESP07192.build.board': 'ESP8266_ESP07',
            '.menu.ESPModule.ESP07192.build.flash_size': '1M',
            '.menu.ESPModule.ESP07192.build.flash_ld': 'eagle.flash.1m192.ld',
            '.menu.ESPModule.ESP07192.build.spiffs_start': '0xCB000',
            '.menu.ESPModule.ESP07192.build.spiffs_end': '0xFB000',
            '.menu.ESPModule.ESP07192.build.spiffs_blocksize': '4096',
            '.menu.ESPModule.ESP07192.upload.maximum_size': '827376',
            '.menu.ESPModule.ESP12': 'ESP12 (4M/1M SPIFFS)',
            '.menu.ESPModule.ESP12.build.board': 'ESP8266_ESP12',
            '.menu.ESPModule.ESP12.build.flash_size': '4M',
            '.menu.ESPModule.ESP12.build.flash_ld': 'eagle.flash.4m1m.ld',
            '.menu.ESPModule.ESP12.build.spiffs_start': '0x300000',
            '.menu.ESPModule.ESP12.build.spiffs_end': '0x3FB000',
            '.menu.ESPModule.ESP12.build.spiffs_blocksize': '8192',
            '.menu.ESPModule.ESP12.build.spiffs_pagesize': '256',
            '.menu.ESPModule.ESP12.upload.maximum_size': '1044464',
            },
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
        'opts': {
            '.build.board': 'ESP8266_ARDUINO',
            '.menu.BoardModel.primo': 'Primo',
            '.menu.BoardModel.primo.build.board': 'ESP8266_ARDUINO_PRIMO',
            '.menu.BoardModel.primo.build.variant': 'arduino_spi',
            '.menu.BoardModel.primo.build.extra_flags': '-DF_CRYSTAL=40000000 -DESP8266',
            '.menu.BoardModel.unowifideved': 'Uno WiFi',
            '.menu.BoardModel.unowifideved.build.board': 'ESP8266_ARDUINO_UNOWIFI',
            '.menu.BoardModel.unowifideved.build.variant': 'arduino_uart',
            '.menu.BoardModel.unowifideved.build.extra_flags=-DF_CRYSTAL': '40000000 -DESP8266',
            '.menu.BoardModel.starottodeved': 'Star OTTO',
            '.menu.BoardModel.starottodeved.build.variant': 'arduino_uart',
            '.menu.BoardModel.starottodeved.build.board': 'ESP8266_ARDUINO_STAR_OTTO',
            '.menu.BoardModel.starottodeved.build.extra_flags': '-DF_CRYSTAL=40000000 -DESP8266',
            },
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
            'flashmode_dio',
            'flashfreq_80',
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
                  '- Product page: http://www.4dsystems.com.au/product/gen4-IoD',
                  ],
    }),
    ( 'oak', {
        'name': 'Digistump Oak',
        'opts': {
            '.build.board': 'ESP8266_OAK',
            '.build.variant': 'oak',
            '.upload.maximum_size': '1040368',
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
    ])

################################################################

macros = {
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
        ( '.menu.CpuFrequency.80', '80 MHz' ),
        ( '.menu.CpuFrequency.80.build.f_cpu', '80000000L' ),
        ( '.menu.CpuFrequency.160', '160 MHz' ),
        ( '.menu.CpuFrequency.160.build.f_cpu', '160000000L' ),
        ]),

    'vtable_menu': collections.OrderedDict([
        ( '.menu.VTable.flash', 'Flash'),
        ( '.menu.VTable.flash.build.vtable_flags', '-DVTABLES_IN_FLASH'),
        ( '.menu.VTable.heap', 'Heap'),
        ( '.menu.VTable.heap.build.vtable_flags', '-DVTABLES_IN_DRAM'),
        ( '.menu.VTable.iram', 'IRAM'),
        ( '.menu.VTable.iram.build.vtable_flags', '-DVTABLES_IN_IRAM'),
        ]),

    'crystalfreq_menu': collections.OrderedDict([
        ( '.menu.CrystalFreq.26', '26 MHz' ),
        ( '.menu.CrystalFreq.40', '40 MHz' ),
        ( '.menu.CrystalFreq.40.build.extra_flags', '-DF_CRYSTAL=40000000 -DESP8266' ),
        ]),

    'flashfreq_menu': collections.OrderedDict([
        ( '.menu.FlashFreq.40', '40MHz' ),
        ( '.menu.FlashFreq.40.build.flash_freq', '40' ),
        ( '.menu.FlashFreq.80', '80MHz' ),
        ( '.menu.FlashFreq.80.build.flash_freq', '80' ),
        ]),

    'flashfreq_40': collections.OrderedDict([
        ( '.build.flash_freq', '40' ),
        ]),

    'flashfreq_80': collections.OrderedDict([
        ( '.build.flash_freq', '80' ),
        ]),

    ####################### menu.resetmethod

    'resetmethod_menu': collections.OrderedDict([
        ( '.menu.ResetMethod.ck', 'ck' ),
        ( '.menu.ResetMethod.ck.upload.resetmethod', 'ck' ),
        ( '.menu.ResetMethod.nodemcu', 'nodemcu' ),
        ( '.menu.ResetMethod.nodemcu.upload.resetmethod', 'nodemcu' ),
        ]),

    'resetmethod_menu_extra': collections.OrderedDict([
        ( '.menu.ResetMethod.none', 'none' ),
        ( '.menu.ResetMethod.none.upload.resetmethod', 'none' ),
        ( '.menu.ResetMethod.dtrset', 'dtrset' ),
        ( '.menu.ResetMethod.dtrset.upload.resetmethod', 'dtrset' ),
        ]),

    ####################### upload.resetmethod

    'resetmethod_ck': collections.OrderedDict([
        ( '.upload.resetmethod', 'ck' ),
        ]),

    'resetmethod_nodemcu': collections.OrderedDict([
        ( '.upload.resetmethod', 'nodemcu' ),
        ]),
    
    'resetmethod_none': collections.OrderedDict([
        ( '.upload.resetmethod', 'none' ),
        ]),

    'resetmethod_dtrset': collections.OrderedDict([
        ( '.upload.resetmethod', 'dtrset' ),
        ]),
    
    ####################### menu.FlashMode

    'flashmode_menu': collections.OrderedDict([
        ( '.menu.FlashMode.qio', 'QIO' ),
        ( '.menu.FlashMode.qio.build.flash_mode', 'qio' ),
        ( '.menu.FlashMode.qout', 'QOUT' ),
        ( '.menu.FlashMode.qout.build.flash_mode', 'qout' ),
        ( '.menu.FlashMode.dio', 'DIO' ),
        ( '.menu.FlashMode.dio.build.flash_mode', 'dio' ),
        ( '.menu.FlashMode.dout', 'DOUT' ),
        ( '.menu.FlashMode.dout.build.flash_mode', 'dout' ),
        ]),

    ####################### default flash_mode

    'flashmode_dio': collections.OrderedDict([
        ( '.build.flash_mode', 'dio' ),
        ]),

    'flashmode_qio': collections.OrderedDict([
        ( '.build.flash_mode', 'qio' ),
        ]),

    'flashmode_dout': collections.OrderedDict([
        ( '.build.flash_mode', 'dout' ),
        ]),

    'flashmode_qout': collections.OrderedDict([
        ( '.build.flash_mode', 'qout' ),
        ]),

    ####################### lwip

    'lwip2': collections.OrderedDict([
        ( '.menu.LwIPVariant.v2mss536', 'v2 Lower Memory' ),
        ( '.menu.LwIPVariant.v2mss536.build.lwip_include', 'lwip2/include' ),
        ( '.menu.LwIPVariant.v2mss536.build.lwip_lib', '-llwip2' ),
        ( '.menu.LwIPVariant.v2mss536.build.lwip_flags', '-DLWIP_OPEN_SRC -DTCP_MSS=536' ),
        ( '.menu.LwIPVariant.v2mss1460', 'v2 Higher Bandwidth' ),
        ( '.menu.LwIPVariant.v2mss1460.build.lwip_include', 'lwip2/include' ),
        ( '.menu.LwIPVariant.v2mss1460.build.lwip_lib', '-llwip2_1460' ),
        ( '.menu.LwIPVariant.v2mss1460.build.lwip_flags', '-DLWIP_OPEN_SRC -DTCP_MSS=1460' ),
        ]),

    'lwip': collections.OrderedDict([
        ( '.menu.LwIPVariant.Prebuilt', 'v1.4 Higher Bandwidth' ),
        ( '.menu.LwIPVariant.Prebuilt.build.lwip_lib', '-llwip_gcc' ),
        ( '.menu.LwIPVariant.Prebuilt.build.lwip_flags', '-DLWIP_OPEN_SRC' ),
        #( '.menu.LwIPVariant.Espressif', 'v1.4 Espressif (xcc)' ),
        #( '.menu.LwIPVariant.Espressif.build.lwip_lib', '-llwip' ),
        #( '.menu.LwIPVariant.Espressif.build.lwip_flags', '-DLWIP_MAYBE_XCC' ),
        ( '.menu.LwIPVariant.OpenSource', 'v1.4 Compile from source' ),
        ( '.menu.LwIPVariant.OpenSource.build.lwip_lib', '-llwip_src' ),
        ( '.menu.LwIPVariant.OpenSource.build.lwip_flags', '-DLWIP_OPEN_SRC' ),
        ( '.menu.LwIPVariant.OpenSource.recipe.hooks.sketch.prebuild.1.pattern', 'make -C "{runtime.platform.path}/tools/sdk/lwip/src" install TOOLS_PATH="{runtime.tools.xtensa-lx106-elf-gcc.path}/bin/xtensa-lx106-elf-"' ),
        ]),

    ####################### serial

    's9': collections.OrderedDict([
        ( '.menu.UploadSpeed.9600', '9600' ),
        ( '.menu.UploadSpeed.9600.upload.speed', '9600' ),
        ]),
    's57': collections.OrderedDict([
        ( '.menu.UploadSpeed.57600', '57600' ),
        ( '.menu.UploadSpeed.57600.upload.speed', '57600' ),
        ]),
    's115': collections.OrderedDict([
        ( '.menu.UploadSpeed.115200', '115200' ),
        ( '.menu.UploadSpeed.115200.upload.speed', '115200' ),
        ]),
    's256': collections.OrderedDict([
        ( '.menu.UploadSpeed.256000.windows', '256000' ),
        ( '.menu.UploadSpeed.256000.upload.speed', '256000' ),
        ]),
    's230': collections.OrderedDict([
        ( '.menu.UploadSpeed.230400.linux', '230400' ),
        ( '.menu.UploadSpeed.230400.macosx', '230400' ),
        ( '.menu.UploadSpeed.230400.upload.speed', '230400' ),
        ]),
    's460': collections.OrderedDict([
        ( '.menu.UploadSpeed.460800.linux', '460800' ),
        ( '.menu.UploadSpeed.460800.macosx', '460800' ),
        ( '.menu.UploadSpeed.460800.upload.speed', '460800' ),
        ]),
    's512': collections.OrderedDict([
        ( '.menu.UploadSpeed.512000.windows', '512000' ),
        ( '.menu.UploadSpeed.512000.upload.speed', '512000' ),
        ]),
    's921': collections.OrderedDict([
        ( '.menu.UploadSpeed.921600', '921600' ),
        ( '.menu.UploadSpeed.921600.upload.speed', '921600' ),
        ]),

    ####################### flash erase

    'flash_erase_menu': collections.OrderedDict([
        ( '.menu.FlashErase.none', 'Only Sketch' ),
        ( '.menu.FlashErase.none.upload.erase_cmd', '' ),
        ( '.menu.FlashErase.sdk', 'Sketch + WiFi Settings' ),
        ( '.menu.FlashErase.sdk.upload.erase_cmd', '-ca "{build.rfcal_addr}" -cz 0x4000' ),
        ( '.menu.FlashErase.all', 'All Flash Contents' ),
        ( '.menu.FlashErase.all.upload.erase_cmd', '-ca 0x0 -cz "{build.flash_size_bytes}"' ),
        ]),

    }

################################################################
# defs

def checkdir ():
    if not os.path.isfile("boards.txt"):
        print "please run me from boards.txt directory (like: ./tools/boards.txt.py -...)"
        sys.exit(1)

################################################################
# debug options

# https://rosettacode.org/wiki/Combinations#Python
def comb (m, lst):
    if m == 0: return [[]]
    return [[x] + suffix for i, x in enumerate(lst) for suffix in comb(m - 1, lst[i + 1:])]

def combn (lst):
    all = []
    for i in range(0, len(lst)):
        all += comb(i + 1, lst)
    return all

def comb1 (lst):
    all = []
    for i in range(0, len(lst)):
        all += [ [ lst[i] ] ]
    all += [ lst ]
    return all

def all_debug ():
    listcomb = [ 'SSL', 'TLS_MEM', 'HTTP_CLIENT', 'HTTP_SERVER' ]
    listnocomb = [ 'CORE', 'WIFI', 'HTTP_UPDATE', 'UPDATER', 'OTA', 'OOM' ]
    listsingle = [ 'NoAssert-NDEBUG' ]
    options = combn(listcomb)
    options += comb1(listnocomb)
    options += [ listcomb + listnocomb ]
    options += [ listsingle ]
    debugmenu = collections.OrderedDict([
            ( '.menu.Debug.Disabled', 'Disabled' ),
            ( '.menu.Debug.Disabled.build.debug_port', '' ),
            ( '.menu.Debug.Serial', 'Serial' ),
            ( '.menu.Debug.Serial.build.debug_port', '-DDEBUG_ESP_PORT=Serial' ),
            ( '.menu.Debug.Serial1', 'Serial1' ),
            ( '.menu.Debug.Serial1.build.debug_port', '-DDEBUG_ESP_PORT=Serial1' ),
            ( '.menu.DebugLevel.None____', 'None' ),
            ( '.menu.DebugLevel.None____.build.debug_level', '' ),
        ])

    for optlist in options:
        debugname = ''
        debugmenuname = ''
        debugdefs = ''
        for opt in optlist:
            space = opt.find(" ")
            if space > 0:
                # remove subsequent associated gcc cmdline option
                simpleopt = opt[0:space]
            else:
                simpleopt = opt
            debugname += simpleopt
            if debugmenuname != '':
                debugmenuname += '+'
            debugmenuname += simpleopt
            if opt == 'NoAssert-NDEBUG':
                debugdefs += ' -DNDEBUG'
            else:
                debugdefs += ' -DDEBUG_ESP_' + opt
        debugmenu.update(collections.OrderedDict([
            ( '.menu.DebugLevel.' + debugname, debugmenuname ),
            ( '.menu.DebugLevel.' + debugname + '.build.debug_level', debugdefs )
            ]))
    return { 'debug_menu': debugmenu }

################################################################
# flash size

def flash_size (size_bytes, display, optname, ld, desc, max_upload_size, spiffs_start = 0, spiffs_size = 0, spiffs_blocksize = 0):
    menu = '.menu.FlashSize.' + optname
    menub = menu + '.build.'
    d = collections.OrderedDict([
        ( menu, display + ' (' + desc + ')' ),
        ( menub + 'flash_size', display ),
        ( menub + 'flash_size_bytes', "0x%X" % size_bytes ),
        ( menub + 'flash_ld', ld ),
        ( menub + 'spiffs_pagesize', '256' ),
        ( menu + '.upload.maximum_size', "%i" % max_upload_size ),
        ( menub + 'rfcal_addr', "0x%X" % (size_bytes - 0x4000))
        ])
    if spiffs_size > 0:
        d.update(collections.OrderedDict([
            ( menub + 'spiffs_start', "0x%05X" % spiffs_start ),
            ( menub + 'spiffs_end', "0x%05X" % (spiffs_start + spiffs_size) ),
            ( menub + 'spiffs_blocksize', "%i" % spiffs_blocksize ),
            ]))

    if ldshow:
        if ldgen:

            checkdir()

            lddir = "tools/sdk/ld/"
            ldbackupdir = lddir + "backup/"
            if not os.path.isdir(ldbackupdir):
                os.mkdir(ldbackupdir)
            if os.path.isfile(lddir + ld) and not os.path.isfile(ldbackupdir + ld):
                os.rename(lddir + ld, ldbackupdir + ld)
            realstdout = sys.stdout
            sys.stdout = open(lddir + ld, 'w')

        if spiffs_size == 0:
            page = 0
            block = 0
        elif spiffs_size < 0x80000:
            page = 0x100
            block = 0x1000
        else:
            page = 0x100
            block = 0x2000

        print "/* Flash Split for %s chips */" % display
        print "/* sketch %dKB */" % (max_upload_size / 1024)
        if spiffs_size > 0:
            empty_size = spiffs_start - max_upload_size - 4096
            if empty_size > 1024:
                print "/* empty  %dKB */" % (empty_size / 1024)
            print "/* spiffs %dKB */" % (spiffs_size / 1024)
        print "/* eeprom 20KB */"
        print ""
        print "MEMORY"
        print "{"
        print "  dport0_0_seg :                        org = 0x3FF00000, len = 0x10"
        print "  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000"
        print "  iram1_0_seg :                         org = 0x40100000, len = 0x8000"
        print "  irom0_0_seg :                         org = 0x40201010, len = 0x%x" % max_upload_size
        print "}"
        print ""
        print "PROVIDE ( _SPIFFS_start = 0x%08X );" % (0x40200000 + spiffs_start)
        print "PROVIDE ( _SPIFFS_end = 0x%08X );" % (0x40200000 + spiffs_start + spiffs_size)
        print "PROVIDE ( _SPIFFS_page = 0x%X );" % page
        print "PROVIDE ( _SPIFFS_block = 0x%X );" % block
        print ""
        print 'INCLUDE "../ld/eagle.app.v6.common.ld"'

        if ldgen:
            sys.stdout.close()
            sys.stdout = realstdout

    return d

def all_flash_size ():
    f512 =      flash_size(0x80000,  '512K', '512K0',   'eagle.flash.512k0.ld',     'no SPIFFS', 499696,   0x7B000)
    f512.update(flash_size(0x80000,  '512K', '512K64',  'eagle.flash.512k64.ld',   '64K SPIFFS', 434160,   0x6B000,   0x10000, 4096))
    f512.update(flash_size(0x80000,  '512K', '512K128', 'eagle.flash.512k128.ld', '128K SPIFFS', 368624,   0x5B000,   0x20000, 4096))
    f1m =       flash_size(0x100000,   '1M', '1M0',     'eagle.flash.1m0.ld',       'no SPIFFS', 1023984,  0xFB000)
    f1m.update( flash_size(0x100000,   '1M', '1M64',    'eagle.flash.1m64.ld',     '64K SPIFFS', 958448,   0xEB000,   0x10000, 4096))
    f1m.update( flash_size(0x100000,   '1M', '1M128',   'eagle.flash.1m128.ld',   '128K SPIFFS', 892912,   0xDB000,   0x20000, 4096))
    f1m.update( flash_size(0x100000,   '1M', '1M144',   'eagle.flash.1m144.ld',   '144K SPIFFS', 876528,   0xD7000,   0x24000, 4096))
    f1m.update( flash_size(0x100000,   '1M', '1M160',   'eagle.flash.1m160.ld',   '160K SPIFFS', 860144,   0xD3000,   0x28000, 4096))
    f1m.update( flash_size(0x100000,   '1M', '1M192',   'eagle.flash.1m192.ld',   '192K SPIFFS', 827376,   0xCB000,   0x30000, 4096))
    f1m.update( flash_size(0x100000,   '1M', '1M256',   'eagle.flash.1m256.ld',   '256K SPIFFS', 761840,   0xBB000,   0x40000, 4096))
    f1m.update( flash_size(0x100000,   '1M', '1M512',   'eagle.flash.1m512.ld',   '512K SPIFFS', 499696,   0x7B000,   0x80000, 8192))
    f2m =       flash_size(0x200000,   '2M', '2M',      'eagle.flash.2m.ld',        '1M SPIFFS', 1044464, 0x100000,   0xFB000, 8192)
    f4m =       flash_size(0x400000,   '4M', '4M1M',    'eagle.flash.4m1m.ld',      '1M SPIFFS', 1044464, 0x300000,   0xFB000, 8192)
    f4m.update( flash_size(0x400000,   '4M', '4M2M',    'eagle.flash.4m2m.ld',      '2M SPIFFS', 1044464, 0x200000,  0x1FB000, 8192))
    f4m.update( flash_size(0x400000,   '4M', '4M3M',    'eagle.flash.4m.ld',        '3M SPIFFS', 1044464, 0x100000,  0x2FB000, 8192))
    f8m =       flash_size(0x800000,   '8M', '8M7M',    'eagle.flash.8m.ld',        '7M SPIFFS', 1044464, 0x100000,  0x6FB000, 8192)
    f16m =      flash_size(0x1000000, '16M', '16M15M',  'eagle.flash.16m.ld',      '15M SPIFFS', 1044464, 0x100000,  0xEFB000, 8192)
    return {
        '512K': f512,
          '1M':  f1m,
          '2M':  f2m,
          '4M':  f4m,
          '8M':  f8m,
         '16M': f16m
        }

################################################################
# builtin led

def led (default,max):
    led = collections.OrderedDict([
                ('.menu.led.' + str(default), str(default)),
                ('.menu.led.' + str(default) + '.build.led', '-DLED_BUILTIN=' + str(default)),
          ]);
    for i in range(0,max):
        if not i == default:
            led.update(
                collections.OrderedDict([
                    ('.menu.led.' + str(i), str(i)),
                    ('.menu.led.' + str(i) + '.build.led', '-DLED_BUILTIN=' + str(i)),
                ]))
    return { 'led': led }

################################################################

def all_boards ():

    if boardsgen:

        checkdir()

        # check if backup already exists
        if not os.path.isfile("boards.txt.orig"):
            os.rename("boards.txt", "boards.txt.orig")

        realstdout = sys.stdout
        sys.stdout = open("boards.txt", 'w')

    macros.update(all_flash_size())
    macros.update(all_debug())
    macros.update(led(led_default, led_max))

    print '#'
    print '# this file is script-generated and is likely to be overwritten by ' + os.path.basename(sys.argv[0])
    print '#'
    print ''
    print 'menu.BoardModel=Model'
    print 'menu.UploadSpeed=Upload Speed'
    print 'menu.CpuFrequency=CPU Frequency'
    print 'menu.CrystalFreq=Crystal Frequency'
    print 'menu.FlashSize=Flash Size'
    print 'menu.FlashMode=Flash Mode'
    print 'menu.FlashFreq=Flash Frequency'
    print 'menu.ResetMethod=Reset Method'
    print 'menu.ESPModule=Module'
    print 'menu.Debug=Debug port'
    print 'menu.DebugLevel=Debug Level'
    print 'menu.LwIPVariant=lwIP Variant'
    print 'menu.VTable=VTables'
    print 'menu.led=Builtin Led'
    print 'menu.FlashErase=Erase Flash'
    print ''

    for id in boards:
        print '##############################################################'
        board = boards[id]
        print id + '.name=' + board['name']

        # standalone options
        if 'opts' in board:
            for optname in board['opts']:
                print id + optname + '=' + board['opts'][optname]

        # macros
        macrolist = [ 'defaults', 'cpufreq_menu', 'vtable_menu' ]
        if 'macro' in board:
            macrolist += board['macro']
        if lwip == 2:
            macrolist += [ 'lwip2', 'lwip' ]
        else:
            macrolist += [ 'lwip', 'lwip2' ]
        macrolist += [ 'debug_menu', 'flash_erase_menu' ]

        for cs in customspeeds:
            print id + cs

        if 'serial' in board:
            macrolist += speeds[board['serial']]
        else:
            macrolist += speeds[default_speed]

        for block in macrolist:
            for optname in macros[block]:
                if not ('opts' in board) or not (optname in board['opts']):
                    print id + optname + '=' + macros[block][optname]

        if nofloat:
            print id + '.build.float='

        print ''

    if boardsgen:
        sys.stdout.close()
        sys.stdout = realstdout

################################################################

def package ():

    pkgfname = "package/package_esp8266com_index.template.json"
    pkgfname_read = pkgfname

    checkdir()

    if packagegen:
        pkgfname_read = pkgfname + '.orig'
        # check if backup already exists
        if os.path.isfile(pkgfname_read):
            print "package file is in the way, please move it"
            print "    %s" % pkgfname_read
            sys.exit(1)
        os.rename(pkgfname, pkgfname_read)

    # read package file
    with open (pkgfname_read, "r") as package_file:
        filestr = package_file.read()

    substitution = '"boards": [\n'
    board_items = ['            {\n              "name": "%s"\n            }' % boards[id]['name']
                    for id in boards]
    substitution += ',\n'.join(board_items)        
    substitution += '\n          ],'

    newfilestr = re.sub(r'"boards":[^\]]*\],', substitution, filestr, re.MULTILINE)

    if packagegen:
        with open(pkgfname, 'w') as package_file:
            package_file.write(newfilestr)
    else:
        sys.stdout.write(newfilestr)

################################################################

def doc ():

    if docgen:

        checkdir()

        # check if backup already exists
        if not os.path.isfile("doc/boards.rst.orig"):
            os.rename("doc/boards.rst", "doc/boards.rst.orig")

        realstdout = sys.stdout
        sys.stdout = open("doc/boards.rst", 'w')

    print 'Boards'
    print '======'
    print ''

    for id in boards:
        board = boards[id]
        print board['name']
        dash = ""
        for i in range(len(board['name'])):
            dash += '-'
        print dash

        print ''
        if 'desc' in board:
            for line in board['desc']:
                print line
        else:
            print 'No description'
        print ''

    if docgen:
        sys.stdout.close()
        sys.stdout = realstdout

################################################################
# help / usage

def usage (name,ret):
    print ""
    print "boards.txt generator for esp8266/Arduino"
    print ""
    print "usage: %s [options]" % name
    print ""
    print " -h, --help"
    print " --lwip          - preferred default lwIP version (default %d)" % lwip
    print " --led           - preferred default builtin led for generic boards (default %d)" % led_default
    print " --board b       - board to modify:"
    print " --speed s       - change default serial speed"
    print " --customspeed s - new serial speed for all boards"
    print " --nofloat       - disable float support in printf/scanf"
    print ""
    print " mandatory option (at least one):"
    print ""
    print " --boards        - show boards.txt"
    print " --boardsgen     - replace boards.txt"
    print " --ld            - show ldscripts"
    print " --ldgen         - replace ldscripts"
    print " --package       - show package"
    print " --packagegen    - replace board:[] in package"
    print " --doc           - shows doc/boards.rst"
    print " --docgen        - replace doc/boards.rst"
    print " --allgen        - generate and replace everything"
    print "                   (useful for pushing on github)"
    print ""

    out = ""
    for s in speeds:
        out += s + ' '
    print "available serial speed options (kbps):", out

    out = ""
    for b in boards:
        out += b + '('
        if 'serial' in boards[b]:
            out += boards[b]['serial']
        else:
            out += default_speed
        out += 'k) '
    print "available board names:", out

    print ""

    sys.exit(ret)

################################################################
################################################################
# entry point

lwip = 2
default_speed = '115'
led_default = 2
led_max = 16
nofloat = False
ldgen = False
ldshow = False
boardsgen = False
boardsshow = False
packageshow = False
packagegen = False
docshow = False
docgen = False
customspeeds = []

#### vvvv cmdline parsing starts

try:
    opts, args = getopt.getopt(sys.argv[1:], "h",
        [ "help", "lwip=", "led=", "speed=", "board=", "customspeed=", "nofloat",
          "ld", "ldgen", "boards", "boardsgen", "package", "packagegen", "doc", "docgen",
          "allgen"] )
except getopt.GetoptError as err:
    print str(err)  # will print something like "option -a not recognized"
    usage(sys.argv[0], 1)

no = '(not set)'
board = no

for o, a in opts:

    if o in ("-h", "--help"):
        usage(sys.argv[0], 0)

    elif o in ("--lwip"):
        lwip = a

    elif o in ("--led"):
        led_default = int(a)

    elif o in ("--customspeed"):
        customspeeds += [
            '.menu.UploadSpeed.' + a + '=' + a,
            '.menu.UploadSpeed.' + a + '.upload.speed' + '=' + a ]

    elif o in ("--board"):
        if not a in boards:
            print "board %s not available" % a
            usage(sys.argv[0], 1)
        board = a

    elif o in ("--speed"):
        if board == no:
            print "board not set"
            usage(sys.argv[0], 1)
        if not a in speeds:
            print "speed %s not available" % a
            usage(sys.argv[0], 1)
        boards[board]['serial'] = a

    elif o in ("--nofloat"):
        nofloat=True

    elif o in ("--ldshow"):
        ldshow = True

    elif o in ("--ldgen"):
        ldshow = True
        ldgen = True

    elif o in ("--boardsshow"):
        boardsshow = True

    elif o in ("--boardsgen"):
        boardsshow = True
        boardsgen = True

    elif o in ("--package"):
        packageshow = True

    elif o in ("--packagegen"):
        packageshow = True
        packagegen = True

    elif o in ("--doc"):
        docshow = True

    elif o in ("--docgen"):
        docshow = True
        docgen = True

    elif o in ("--allgen"):
        ldshow = True
        ldgen = True
        boardsshow = True
        boardsgen = True
        packageshow = True
        packagegen = True
        docshow = True
        docgen = True

    else:
        assert False, "unhandled option"

#### ^^^^ cmdline parsing ends

did = False

if ldshow:
    all_flash_size()
    did = True

if boardsshow:
    ldshow = False
    ldgen = False
    all_boards()
    did = True

if packageshow:
    package()
    did = True

if docshow:
    doc()
    did = True

if not did:
    usage(sys.argv[0], 0)
