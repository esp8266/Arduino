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

from __future__ import print_function
import os
import sys
import collections
import getopt
import re
import json

requiredboards = [ 'generic', 'esp8285' ]

################################################################
# serial upload speed order in menu
# default is 115 for every board unless specified with 'serial' in board
# or by user command line

speeds = collections.OrderedDict([
    (  '57',  [ 's57',  's115', 's230', 's256', 's460', 's512', 's921', 's3000' ]),
    ( '115',  [ 's115', 's57',  's230', 's256', 's460', 's512', 's921', 's3000' ]),
    ( '230',  [ 's230', 's57',  's115', 's256', 's460', 's512', 's921', 's3000' ]),
    ( '256',  [ 's256', 's57',  's115', 's230', 's460', 's512', 's921', 's3000' ]),
    ( '460',  [ 's460', 's57',  's115', 's230', 's256', 's512', 's921', 's3000' ]),
    ( '512',  [ 's512', 's57',  's115', 's230', 's256', 's460', 's921', 's3000' ]),
    ( '921',  [ 's921', 's57',  's115', 's230', 's256', 's460', 's512', 's3000' ]),
    ( '3000', [ 's3000','s57',  's115', 's230', 's256', 's460', 's512', 's921'  ]),
    ])

################################################################
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
            '1M', '2M', '4M', '8M', '16M', '512K',
            'led',
            'sdk',
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
    ( 'inventone', {
        'name': 'Invent One',
        'opts': {
            '.build.board': 'ESP8266_GENERIC',
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
            '.build.board': 'ESP8266_GENERIC',
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
            ( '.menu.BoardModel.primo.build.extra_flags', '-DF_CRYSTAL=40000000 -DESP8266' ),
            ( '.menu.BoardModel.unowifideved', 'Uno WiFi' ),
            ( '.menu.BoardModel.unowifideved.build.board', 'ESP8266_ARDUINO_UNOWIFI' ),
            ( '.menu.BoardModel.unowifideved.build.variant', 'arduino_uart' ),
            ( '.menu.BoardModel.unowifideved.build.extra_flags=-DF_CRYSTAL', '40000000 -DESP8266' ),
            ( '.menu.BoardModel.starottodeved', 'Star OTTO' ),
            ( '.menu.BoardModel.starottodeved.build.variant', 'arduino_uart' ),
            ( '.menu.BoardModel.starottodeved.build.board', 'ESP8266_ARDUINO_STAR_OTTO' ),
            ( '.menu.BoardModel.starottodeved.build.extra_flags', '-DF_CRYSTAL=40000000 -DESP8266' ),
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
            '.build.extra_flags': '-DESP8266',
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
        ( '.menu.exception.legacy', 'Legacy (new can return nullptr)' ),
        ( '.menu.exception.legacy.build.exception_flags', '-fno-exceptions' ),
        ( '.menu.exception.legacy.build.stdcpp_lib', '-lstdc++' ),
        ( '.menu.exception.disabled', 'Disabled (new can abort)' ),
        ( '.menu.exception.disabled.build.exception_flags', '-fno-exceptions -DNEW_OOM_ABORT' ),
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
        ( '.menu.CrystalFreq.40.build.extra_flags', '-DF_CRYSTAL=40000000 -DESP8266' ),
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

    }

################################################################
# defs

def checkdir ():
    if not os.path.isfile("boards.txt"):
        print("please run me from boards.txt directory (like: ./tools/boards.txt.py -...)")
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
    listnocomb = [ 'CORE', 'WIFI', 'HTTP_UPDATE', 'UPDATER', 'OTA', 'OOM', 'MDNS' ]
    listsingle = [ 'NoAssert-NDEBUG' ]
    options = combn(listcomb)
    options += comb1(listnocomb)
    options += [ listcomb + listnocomb ]
    options += [ listsingle ]
    debugmenu = collections.OrderedDict([
            ( '.menu.dbg.Disabled', 'Disabled' ),
            ( '.menu.dbg.Disabled.build.debug_port', '' ),
            ( '.menu.dbg.Serial', 'Serial' ),
            ( '.menu.dbg.Serial.build.debug_port', '-DDEBUG_ESP_PORT=Serial' ),
            ( '.menu.dbg.Serial1', 'Serial1' ),
            ( '.menu.dbg.Serial1.build.debug_port', '-DDEBUG_ESP_PORT=Serial1' ),
            ( '.menu.lvl.None____', 'None' ),
            ( '.menu.lvl.None____.build.debug_level', '' ),
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
            ( '.menu.lvl.' + debugname, debugmenuname ),
            ( '.menu.lvl.' + debugname + '.build.debug_level', debugdefs )
            ]))
    return { 'debug_menu': debugmenu }

################################################################
# flash size

def flash_map (flashsize_kb, fs_kb = 0):

    # mapping:
    # flash | reserved | empty | spiffs | eeprom | rf-cal | sdk-wifi-settings

    spi = 0x40200000 # https://github.com/esp8266/esp8266-wiki/wiki/Memory-Map

    reserved = 4112
    eeprom_size_kb = 4
    rfcal_size_kb = 4
    sdkwifi_size_kb = 12
    fs_end = (flashsize_kb - sdkwifi_size_kb - rfcal_size_kb - eeprom_size_kb) * 1024

    # For legacy reasons (#6531), the EEPROM sector needs to be at the old
    # FS_end calculated without regards to block size
    eeprom_start = fs_end

    rfcal_addr = (flashsize_kb - sdkwifi_size_kb - rfcal_size_kb) * 1024
    if flashsize_kb <= 1024:
        max_upload_size = (flashsize_kb - (fs_kb + eeprom_size_kb + rfcal_size_kb + sdkwifi_size_kb)) * 1024 - reserved
        fs_start = fs_end - fs_kb * 1024
    else:
        max_upload_size = 1024 * 1024 - reserved
        fs_start = (flashsize_kb - fs_kb) * 1024

    if fs_kb < 512:
        fs_blocksize = 4096
    else:
        fs_blocksize = 8192

    # Adjust FS_end to be a multiple of the block size
    fs_end = fs_blocksize * (int)((fs_end - fs_start)/fs_blocksize) + fs_start;

    max_ota_size = min(max_upload_size, fs_start / 2) # =(max_upload_size+empty_size)/2
    strsize = str(int(flashsize_kb / 1024)) + 'M' if (flashsize_kb >= 1024) else str(flashsize_kb) + 'K'
    strfs = str(int(fs_kb / 1024)) + 'M' if (fs_kb >= 1024) else str(fs_kb) + 'K'
    strfs_strip = str(int(fs_kb / 1024)) + 'M' if (fs_kb >= 1024) else str(fs_kb) if (fs_kb > 0) else ''

    ld = 'eagle.flash.' + strsize.lower() + strfs_strip.lower() + '.ld'
    menu = '.menu.eesz.' + strsize + strfs_strip
    menub = menu + '.build.'
    desc = 'none' if (fs_kb == 0) else strfs + 'B'
    d = collections.OrderedDict([
        ( menu, strsize + 'B (FS:' + desc + ' OTA:~%iKB)' % (max_ota_size / 1024)),
        ( menub + 'flash_size', strsize ),
        ( menub + 'flash_size_bytes', "0x%X" % (flashsize_kb * 1024)),
        ( menub + 'flash_ld', ld ),
        ( menub + 'spiffs_pagesize', '256' ),
        ( menu + '.upload.maximum_size', "%i" % max_upload_size ),
        ( menub + 'rfcal_addr', "0x%X" % rfcal_addr)
        ])
    if fs_kb > 0:
        d.update(collections.OrderedDict([
            ( menub + 'spiffs_start', "0x%05X" % fs_start ),
            ( menub + 'spiffs_end', "0x%05X" % fs_end ),
            ( menub + 'spiffs_blocksize', "%i" % fs_blocksize ),
            ]))

    if ldshow:
        if ldgen:

            checkdir()

            ldbackupdir = lddir + "backup/"
            if not os.path.isdir(ldbackupdir):
                os.mkdir(ldbackupdir)
            if os.path.isfile(lddir + ld) and not os.path.isfile(ldbackupdir + ld):
                os.rename(lddir + ld, ldbackupdir + ld)
            realstdout = sys.stdout
            sys.stdout = open(lddir + ld, 'w')

        if fs_kb == 0:
            fs_start = fs_end
            page = 0
            fs_blocksize = 0
        else:
            page = 0x100

        print("/* Flash Split for %s chips */" % strsize)
        print("/* sketch @0x%X (~%dKB) (%dB) */" % (spi, (max_upload_size / 1024), max_upload_size))
        empty_size = fs_start - max_upload_size
        if empty_size > 0:
            print("/* empty  @0x%X (~%dKB) (%dB) */" % (spi + max_upload_size, empty_size / 1024, empty_size))
        print("/* spiffs @0x%X (~%dKB) (%dB) */" % (spi + fs_start, ((fs_end - fs_start) / 1024), fs_end - fs_start))
        print("/* eeprom @0x%X (%dKB) */" % (spi + rfcal_addr - eeprom_size_kb * 1024, eeprom_size_kb))
        print("/* rfcal  @0x%X (%dKB) */" % (spi + rfcal_addr, rfcal_size_kb))
        print("/* wifi   @0x%X (%dKB) */" % (spi + rfcal_addr + rfcal_size_kb * 1024, sdkwifi_size_kb))
        print("")
        print("MEMORY")
        print("{")
        print("  dport0_0_seg :                        org = 0x3FF00000, len = 0x10")
        print("  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000")
        print("  iram1_0_seg :                         org = 0x40100000, len = 0x8000")
        print("  irom0_0_seg :                         org = 0x40201010, len = 0x%x" % max_upload_size)
        print("}")
        print("")
        print("PROVIDE ( _FS_start = 0x%08X );" % (0x40200000 + fs_start))
        print("PROVIDE ( _FS_end = 0x%08X );" % (0x40200000 + fs_end))
        print("PROVIDE ( _FS_page = 0x%X );" % page)
        print("PROVIDE ( _FS_block = 0x%X );" % fs_blocksize)
        print("PROVIDE ( _EEPROM_start = 0x%08x );" % (0x40200000 + eeprom_start))
        # Re-add deprecated symbols pointing to the same address as the new standard ones
        print("/* The following symbols are DEPRECATED and will be REMOVED in a future release */")
        print("PROVIDE ( _SPIFFS_start = 0x%08X );" % (0x40200000 + fs_start))
        print("PROVIDE ( _SPIFFS_end = 0x%08X );" % (0x40200000 + fs_end))
        print("PROVIDE ( _SPIFFS_page = 0x%X );" % page)
        print("PROVIDE ( _SPIFFS_block = 0x%X );" % fs_blocksize)
        print("")
        print('INCLUDE "local.eagle.app.v6.common.ld"')

        if ldgen:
            sys.stdout.close()
            sys.stdout = realstdout

    return d

def all_flash_map ():

    f512 = collections.OrderedDict([])
    f1m  = collections.OrderedDict([])
    f2m  = collections.OrderedDict([])
    f4m  = collections.OrderedDict([])
    f8m  = collections.OrderedDict([])
    f16m = collections.OrderedDict([])

    #                      flash(KB) spiffs(KB)

    f1m.update( flash_map(    1024,      64 ))
    f1m.update( flash_map(    1024,     128 ))
    f1m.update( flash_map(    1024,     144 ))
    f1m.update( flash_map(    1024,     160 ))
    f1m.update( flash_map(    1024,     192 ))
    f1m.update( flash_map(    1024,     256 ))
    f1m.update( flash_map(    1024,     512 ))
    f1m.update( flash_map(    1024))

    f2m.update( flash_map(  2*1024,      64 ))
    f2m.update( flash_map(  2*1024,     128 ))
    f2m.update( flash_map(  2*1024,     256 ))
    f2m.update( flash_map(  2*1024,     512 ))
    f2m.update( flash_map(  2*1024,    1024 ))
    f2m.update( flash_map(  2*1024))

    f4m.update( flash_map(  4*1024,  2*1024 ))
    f4m.update( flash_map(  4*1024,  3*1024 ))
    f4m.update( flash_map(  4*1024,    1024 ))
    f4m.update( flash_map(  4*1024))

    f8m.update( flash_map(  8*1024,  6*1024 ))
    f8m.update( flash_map(  8*1024,  7*1024 ))

    f16m.update(flash_map( 16*1024, 14*1024 ))
    f16m.update(flash_map( 16*1024, 15*1024 ))

    f512.update(flash_map(     512,      32 ))
    f512.update(flash_map(     512,      64 ))
    f512.update(flash_map(     512,     128 ))
    f512.update(flash_map(     512))

    if ldgen:
        print("generated: ldscripts (in %s)" % lddir)

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

def led (name, default, ledList):
    led = collections.OrderedDict([
                ('.menu.led.' + str(default), str(default)),
                ('.menu.led.' + str(default) + '.build.led', '-DLED_BUILTIN=' + str(default)),
          ]);
    for i in ledList: # Make range incluside of max (16), since there are really 16 GPIOS not 15
        if not i == default:
            led.update(
                collections.OrderedDict([
                    ('.menu.led.' + str(i), str(i)),
                    ('.menu.led.' + str(i) + '.build.led', '-DLED_BUILTIN=' + str(i)),
                ]))
    return { name: led }

################################################################
# sdk selection

def sdk ():
    return { 'sdk': collections.OrderedDict([
                        ('.menu.sdk.nonosdk_190703', 'nonos-sdk 2.2.1+100 (190703)'),
                        ('.menu.sdk.nonosdk_190703.build.sdk', 'NONOSDK22x_190703'),
                        ('.menu.sdk.nonosdk_191122', 'nonos-sdk 2.2.1+119 (191122)'),
                        ('.menu.sdk.nonosdk_191122.build.sdk', 'NONOSDK22x_191122'),
                        ('.menu.sdk.nonosdk_191105', 'nonos-sdk 2.2.1+113 (191105)'),
                        ('.menu.sdk.nonosdk_191105.build.sdk', 'NONOSDK22x_191105'),
                        ('.menu.sdk.nonosdk_191024', 'nonos-sdk 2.2.1+111 (191024)'),
                        ('.menu.sdk.nonosdk_191024.build.sdk', 'NONOSDK22x_191024'),
                     #  ('.menu.sdk.nonosdk_190313', 'nonos-sdk 2.2.1+61 (190313 testing)'),
                     #  ('.menu.sdk.nonosdk_190313.build.sdk', 'NONOSDK22x_190313'),
                        ('.menu.sdk.nonosdk221', 'nonos-sdk 2.2.1 (legacy)'),
                        ('.menu.sdk.nonosdk221.build.sdk', 'NONOSDK221'),
                        ('.menu.sdk.nonosdk3v0', 'nonos-sdk pre-3 (180626 known issues)'),
                        ('.menu.sdk.nonosdk3v0.build.sdk', 'NONOSDK3V0'),
                    ])
           }

################################################################

def all_boards ():

    if boardsgen or boardslocalgen:

        checkdir()

        if boardsgen:
            # check if backup already exists
            if not os.path.isfile("boards.txt.orig"):
                os.rename("boards.txt", "boards.txt.orig")

            realstdout = sys.stdout
            sys.stdout = open("boards.txt", 'w')
        else:
            # make backup of boards.local.txt
            if os.path.isfile("boards.local.txt"):
                if not os.path.isfile("boards.local.txt.orig"):
                    os.rename("boards.local.txt", "boards.local.txt.orig")

            realstdout = sys.stdout
            sys.stdout = open("boards.local.txt", 'w')

    macros.update(all_flash_map())
    macros.update(all_debug())
    macros.update(led('led',    led_default, range(0,led_max+1)))
    macros.update(led('led216', 2,           { 16 }))
    macros.update(sdk())

    if boardfilteropt or excludeboards:
        print('#')
        print('# Do not create pull-requests with this abridged file!')
        print('# Do as instructed further down.')
        print('#')

        out = ""
        for a in sys.argv:
            out += " " + a
        print('# Abridged boards.txt or boards.local.txt created by:' + out)
        out = ""
        for a in boardlist:
            out += " " + a
        print('# The following boards were included: ' + out)
        print('#')


    print('#')
    print('# Do not create pull-requests for this file only, CI will not accept them.')
    print('# You *must* edit/modify/run ' + os.path.basename(sys.argv[0]) + ' to regenerate boards.txt.')
    print('# All modified files after running with option "--allgen" must be included in the pull-request.')
    print('#')
    print('')
    # With Arduino IDE 1.8.7 the order of the menu items will be honored from the tools pull down list.
    print('menu.BoardModel=Model')
    print('menu.ESPModule=Module')
    print('menu.led=Builtin Led')
    print('menu.baud=Upload Speed')
    print('menu.xtal=CPU Frequency')
    print('menu.CrystalFreq=Crystal Frequency')
    print('menu.eesz=Flash Size')
    print('menu.FlashMode=Flash Mode')
    print('menu.FlashFreq=Flash Frequency')
    print('menu.ResetMethod=Reset Method')
    print('menu.dbg=Debug port')
    print('menu.lvl=Debug Level')
    print('menu.ip=lwIP Variant')
    print('menu.vt=VTables')
    print('menu.exception=Exceptions')
    print('menu.stacksmash=Stack Protection')
    print('menu.wipe=Erase Flash')
    print('menu.sdk=Espressif FW')
    print('menu.ssl=SSL Support')
    print('')

    missingboards = []
    for id in boardlist:
        if id not in boards:
            missingboards += [ id ];
            continue

        print('##############################################################')
        board = boards[id]
        print(id + '.name=' + board['name'])

        # standalone options
        if 'opts' in board:
            for optname in sorted(board['opts']):
                print(id + optname + '=' + board['opts'][optname])

        # macros
        macrolist = [ 'defaults', 'cpufreq_menu', 'vtable_menu', 'exception_menu', 'stacksmash_menu', 'ssl_cipher_menu' ]
        if 'macro' in board:
            macrolist += board['macro']
        macrolist += [ 'lwip', 'debug_menu', 'flash_erase_menu' ]

        for cs in customspeeds:
            print(id + cs)

        if 'serial' in board:
            macrolist += speeds[board['serial']]
        else:
            macrolist += speeds[default_speed]

        for block in macrolist:
            for optname in macros[block]:
                if not ('opts' in board) or not (optname in board['opts']):
                    print(id + optname + '=' + macros[block][optname])

        if nofloat:
            print(id + '.build.float=')

        print('')

    if boardsgen or boardslocalgen:
        sys.stdout.close()
        sys.stdout = realstdout

    if missingboards:
        print("No board definitions were found for the following boards:")
        print(missingboards)
        print("")

    if boardsgen:
        print("generated: boards.txt")
    else:
        print("generated: boards.local.txt")

################################################################

def package ():

    pkgfname = "package/package_esp8266com_index.template.json"
    pkgfname_read = pkgfname

    checkdir()

    if packagegen:
        pkgfname_read = pkgfname + '.orig'
        if os.path.isfile(pkgfname_read):
            os.remove(pkgfname_read)
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

    # To get consistent indent/formatting read the JSON and write it out programattically
    if packagegen:
        with open(pkgfname, 'w') as package_file:
            filejson = json.loads(newfilestr, object_pairs_hook=collections.OrderedDict)
            package_file.write(json.dumps(filejson, indent=3, separators=(',',': ')))
        print("updated:   %s" % pkgfname)
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

    print('Boards')
    print('======')
    print('')

    for id in boards:
        board = boards[id]
        print(board['name'])
        dash = ""
        for i in range(len(board['name'])):
            dash += '-'
        print(dash)

        print('')
        if 'desc' in board:
            for line in board['desc']:
                print(line)
        else:
            print('No description')
        print('')

    if docgen:
        sys.stdout.close()
        sys.stdout = realstdout
        print("generated: doc/boards.rst")

################################################################

def boardnames ():
    print('# Available board names. Delete or comment out the boards you do not need:')

    for id in boards:
        print('{: <20s} # {}'.format(id, boards[id]['name']))

    sys.exit(0)

################################################################
# help / usage

def usage (name,ret):
    print("")
    print("boards.txt generator for esp8266/Arduino")
    print("")
    print("usage: %s [options]" % name)
    print("")
    print(" -h, --help")
    print(" --led             - preferred default builtin led for generic boards (default %d)" % led_default)
    print(" --board <b>       - board to modify:")
    print(" --filter <file>   - create a short boards.txt based on the boards listed in <file>")
    print(" --xfilter <file>  - create a short boards.txt excluding the boards listed in <file>")
    print("                     (For --filter or --xfilter use only one)")
    print(" --speed <s>       - change default serial speed")
    print(" --customspeed <s> - new serial speed for all boards")
    print(" --nofloat         - disable float support in printf/scanf")
    print("")
    print(" mandatory option (at least one):")
    print("")
    print(" --boards          - show boards.txt")
    print(" --boardsgen       - replace boards.txt")
    print(" --boardslocalgen  - replace boards.local.txt instead of boards.txt")
    print(" --boardnames      - prints a list of board names, one per line")
    print(" --ld              - show ldscripts")
    print(" --ldgen           - replace ldscripts")
    print(" --package         - show package")
    print(" --packagegen      - replace board:[] in package")
    print(" --doc             - shows doc/boards.rst")
    print(" --docgen          - replace doc/boards.rst")
    print(" --allgen          - generate and replace everything")
    print("                     (useful for pushing on github)")
    print("")

    out = ""
    for s in speeds:
        out += s + ' '
    print("available serial speed options (kbps):", out)

    out = ""
    for b in boards:
        out += b + '('
        if 'serial' in boards[b]:
            out += boards[b]['serial']
        else:
            out += default_speed
        out += 'k) '
    print("available board names:", out)

    print("")

    sys.exit(ret)

################################################################
################################################################
# entry point

default_speed = '115'
led_default = 2
led_max = 16
nofloat = False
ldgen = False
ldshow = False
boardsgen = False
boardsshow = False

boardlist = []
boardfilterfile = ""
boardfilteropt = False
excludeboardlist = []
excludeboards = False
boardslocalgen = False

packageshow = False
packagegen = False
docshow = False
docgen = False
customspeeds = []
lddir = "tools/sdk/ld/"

#### vvvv cmdline parsing starts

try:
    opts, args = getopt.getopt(sys.argv[1:], "h",
        [ "help", "led=", "speed=", "board=", "customspeed=", "nofloat",
          "noextra4kheap", "allowWPS",
          "boardslocalgen", "filter=", "xfilter=", "boardnames",
          "ld", "ldgen", "boards", "boardsgen", "package", "packagegen", "doc", "docgen",
          "allgen"] )
except getopt.GetoptError as err:
    print(str(err)) # will print something like "option -a not recognized"
    usage(sys.argv[0], 1)

no = '(not set)'
board = no

for o, a in opts:

    if o in ("-h", "--help"):
        usage(sys.argv[0], 0)

    elif o in ("--boardnames"):
       boardnames()

    elif o in ("--led"):
        led_default = int(a)

    elif o in ("--customspeed"):
        customspeeds += [
            '.menu.baud.' + a + '=' + a,
            '.menu.baud.' + a + '.upload.speed' + '=' + a ]

    elif o in ("--board"):
        if not a in boards:
            print("board %s not available" % a)
            usage(sys.argv[0], 1)
        board = a

    elif o in ("--filter"):
        boardfilteropt = True
        boardfilterfile = a

    elif o in ("--xfilter"):
        excludeboards = True
        boardfilterfile = a

    elif o in ("--speed"):
        if board == no:
            print("board not set")
            usage(sys.argv[0], 1)
        if not a in speeds:
            print("speed %s not available" % a)
            usage(sys.argv[0], 1)
        boards[board]['serial'] = a

    elif o in ("--nofloat"):
        nofloat=True

    elif o in ("--noextra4kheap", "--allowWPS"):
        print('option ' + o + ' is now deprecated, without effect, and will be removed')

    elif o in ("--ld"):
        ldshow = True

    elif o in ("--ldgen"):
        ldshow = True
        ldgen = True

    elif o in ("--boardsshow"):
        boardsshow = True

    elif o in ("--boardsgen"):
        boardsshow = True
        boardsgen = True

    elif o in ("--boardslocalgen"):
        boardsshow = True
        boardslocalgen = True

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

#### vvvv Filter file processing if we have one

if boardfilteropt and excludeboards:
    print('Specify either --filter or --xfilter, not both.')
    usage(sys.argv[0], 1)

if boardfilteropt or excludeboards:
    if not os.path.isfile(boardfilterfile):
        print('Filter file missing: ', boardfilterfile)
        usage(sys.argv[0], 1)

    f = open(boardfilterfile, 'r')
    for line in f:
        a = line.split('#', 1)[0].strip()
        if a != '':
            boardlist += [ a ]
    f.close()

    if not boardslocalgen:
        if boardfilteropt:
            for name in requiredboards:
                if name not in boardlist:
                    boardlist.append(name)
        else:
            # excludeboards:
            for name in requiredboards:
                if name in boardlist:
                    boardlist.remove(name)

    if boardfilteropt:
        print('Applying keep filter list:')
    else:
        print('Applying exclude filter list:')

    print(boardlist)
    print('')

#### ^^^^ Filter file processing finished

did = False

if ldshow:
    all_flash_map()
    did = True

if boardsshow:
    ldshow = False
    ldgen = False
    if not boardfilteropt:
        if excludeboards:
            excludeboardlist = boardlist
        boardlist = []
        for b in boards:
            if b not in excludeboardlist:
                boardlist += [ b ]
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
