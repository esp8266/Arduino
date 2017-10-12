#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# generateBoards.py — generate boards.txt and various make files for ESP Arduino
#
# Copyright © 2017 Julian Davison
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
#

from __future__ import print_function
import sys
import os
import argparse
import subprocess
import tempfile
import shutil
import pprint

menu = {}

nonDevicesList = {'menu': 'Menu items',
                 }

platformSuffixes = ['linux', 'windows', 'macosx']

# Pull out the 'menu' subtree (at the top level) to define the custom menus
# 'boardIds' == remaining top-level keys
# iterate over boardIds to pull out board specific config

# platform suffixes stripped in IDE on load
        # key = processPlatformSuffix(key, ".linux", OSUtils.isLinux());
        # key = processPlatformSuffix(key, ".windows", OSUtils.isWindows());
        # key = processPlatformSuffix(key, ".macosx", OSUtils.isMacOS());

# FInd top-level custom menus, and then hiff the 'menu' branch of boards.txt
      # // Create custom menus for this platform
      # PreferencesMap menus = boardsPreferences.get("menu");
      # if (menus != null)
      #   customMenus = menus.topLevelMap();
      # boardsPreferences.remove("menu");



devicesList = {
    'generic': {'name': 'Generic ESP8266 Module',
                },
    'esp8285': {'name': 'Generic ESP8285 Module',
                },
    'espduino': {'name': 'ESPDuino (ESP-13 Module)',
                 },
    'huzzah': {'name': 'Adafruit HUZZAH ESP8266',
               },
    'espresso_lite_v1': {'name': 'ESPresso Lite 1.0',
                         },
    'espresso_lite_v2': {'name': 'ESPresso Lite 2.0',
                         },
    'phoenix_v1': {'name': 'Phoenix 1.0',
                   },
    'phoenix_v2': {'name': 'Phoenix 2.0',
                   },
    'nodemcu': {'name': 'NodeMCU 0.9 (ESP-12 Module)',
                },
    'nodemcuv2': {'name': 'NodeMCU 1.0 (ESP-12E Module)',
                  },
    'modwifi': {'name': 'Olimex MOD-WIFI-ESP8266(-DEV)',
                },
    'thing': {'name': 'SparkFun ESP8266 Thing',
              },
    'thingdev': {'name': 'SparkFun ESP8266 Thing Dev',
                 },
    'esp210': {'name': 'SweetPea ESP-210',
               },
    'd1_mini': {'name': 'WeMos D1 R2 & mini',
                },
    'd1': {'name': 'WeMos D1(Retired)',
           },
    'espino': {'name': 'ESPino (ESP-12 Module)',
               },
    'espinotee': {'name': 'ThaiEasyElec''s ESPino',
                  },
    'wifinfo': {'name': 'WifInfo',
                },
    'coredev': {'name': 'Core Development Module',
                },
}

with open('boards.txt') as f:
    for line in f:
        line = line.rstrip("\n")
        # SKip blank lines
        if (line == ''):
            continue
        # Skip comment lines
        if (line[0] == '#'):
            continue

        (key, value) = line.split('=', 1)
        components = key.split('.')
        # print(components)
        # print(value)

        device = '' #None
        item = menu
        for k in components:
            if (k in devicesList.keys()):
                device = k
                continue
            if (k not in item.keys()):
                item[k] = {}
            item = item[k]

        if (value is None):
            value = '%None%'
        if (value in item.keys()):
            item[value].append(device)
        else:
            item[value] = [device]


menu = {}
devceMenu = {}
customMenus = {}

settings = {}
genericSettings = {}

with open('boards.txt') as f:
    for line in f:
        line = line.rstrip("\n")
        # SKip blank lines
        if (line == ''):
            continue
        # Skip comment lines
        if (line[0] == '#'):
            continue

        (key, value) = line.split('=', 1)
        (device, config_line) = key.split('.', 1)
        # print(components)
        # print(value)
        # print('Line: ' + line)
        # print('Key: ' + key)
        # print('Value: ' + value)
        # print('Device: ' + device)
        # print('Config: ' + config_line)

        item = menu
        # device = '' #None
        if (device in nonDevicesList.keys()):
            # config_line = device + '.' + config_line
            # device = 'None'
            item = customMenus
        else:
            device = "'{device}'".format(device = device)
        # print('Device: ' + device)
        # print('Config: ' + config_line)



        if (config_line not in item.keys()):
            item[config_line] = {value: [device]}
        if (value not in item[config_line]):
            item[config_line][value] = [device]
        if (device not in item[config_line][value]):
            item[config_line][value].append(device)
        if (device == '\'generic\''):
            if (config_line not in genericSettings.keys()):
                genericSettings[config_line] = value
        else:
            if ((config_line not in genericSettings.keys()) or 
                (genericSettings[config_line] != value)):
                if (device not in settings.keys()):
                    settings[device] = {}
                settings[device][config_line] = value


pp = pprint.PrettyPrinter(indent=1)
# pp.pprint(menu)

# print(menu)

def build(data, prefix):
    if (type(data) == type([])):
        return data
    for k in data.keys():
        if (prefix != ''):
            devices = build(data[k], prefix + '.' + k)
        else:
            devices = build(data[k], k)
        if (devices is not None):
            for device in devices:
                print("{device}.{prefix}={k}".format(device = device, prefix = prefix, k = k))
    return None

# build(menu, '')

def myPprint(data, prefix_len):
    if (type(data) == type([])):
        return data
    firstIteration = True
    for k in sorted(data):
        if (firstIteration):
            firstIteration = False
        else:
            print(" " * prefix_len, end='')
        # print(prefix_len, end=';')
        start = "{{'{k}': ".format(k = k)
        print(start, end='')
        # print(" " * (len(start) + prefix_len), end='')
        # print(len(start), end=':')
        returnVal = myPprint(data[k], prefix_len + len(start))
        if (returnVal is not None):
            # print(returnVal)
            print("['", end='')
            print("', '".join(returnVal), end='')
            print("']")
            # print(prefix_len, end='')
    return None

def myPprint2(data):
    indent = 1
    print("{", end='')
    firstIteration = True
    for config_line in sorted(data.iterkeys()):
        if (firstIteration):
            firstIteration = False
        else:
            print(" ", end='')
        outputStr = "'{k}': {{".format(k = config_line)
        print(outputStr, end='')
        indent = len(outputStr)
        entries = 0
        for value in data[config_line]:
            entries += 1
            if (entries > 1):
                print()
                print(" " * indent, end='')
            print("'{v}': [".format(v = value.replace("'", "\\'")), end='')
            print(", ".join(data[config_line][value]), end='')
            # for device in data[config_line][value]:
            #     print("'{d}', ".format(d = device), end='')
            print("],", end='')
        if (entries > 1):
            print(" " * indent, end='')
        print("},")
    print("}")

def myPprint3(data):
    indent = 1
    print("{", end='')
    firstIteration = True
    for config_line in sorted(data.iterkeys()):
        if (firstIteration):
            firstIteration = False
        else:
            print(" ", end='')
        outputStr = "'{k}': {{".format(k = config_line)
        print(outputStr, end='')
        indent = len(outputStr)
        entries = 0
        for value in data[config_line]:
            entries += 1
            if (entries > 1):
                print()
                print(" " * indent, end='')
            print("'{v}': [".format(v = value.replace("'", "\\'")), end='')
            print(", ".join(data[config_line][value]), end='')
            # for device in data[config_line][value]:
            #     print("'{d}', ".format(d = device), end='')
            print("],", end='')
        if (entries > 1):
            print(" " * indent, end='')
        print("},")
    print("}")

# myPprint(menu, 0)
print("Menu")
myPprint2(menu)
print("customMenus")
myPprint2(customMenus)
print("genericSettings")
pp.pprint(genericSettings)
pp.pprint(settings)

{'build': {'board': {'ESP8266_ESP01': ['generic', 'esp8285', 'coredev'],
                     'ESP8266_ESP12': ['huzzah', 'espino'],
                     'ESP8266_ESP13': ['espduino', 'espinotee'],
                     'ESP8266_ESP210': ['esp210'],
                     'ESP8266_ESPRESSO_LITE_V1': ['espresso_lite_v1'],
                     'ESP8266_ESPRESSO_LITE_V2': ['espresso_lite_v2'],
                     'ESP8266_NODEMCU': ['nodemcu', 'nodemcuv2'],
                     'ESP8266_PHOENIX_V1': ['phoenix_v1'],
                     'ESP8266_PHOENIX_V2': ['phoenix_v2'],
                     'ESP8266_THING': ['thing'],
                     'ESP8266_THING_DEV': ['thingdev'],
                     'ESP8266_WEMOS_D1MINI': ['d1_mini', 'd1'],
                     'MOD_WIFI_ESP8266': ['modwifi'],
                     'WIFINFO': ['wifinfo']},
           'core': {'esp8266': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']},
           'debug_level': {'': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev'],
                           'Wifinfo': ['wifinfo']},
           'debug_port': {'': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev'],
                          'Serial1': ['wifinfo']},
           'f_cpu': {'80000000L': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']},
           'flash_freq': {'40': ['esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee']},
           'flash_ld': {'eagle.flash.2m.ld': ['modwifi'],
                        'eagle.flash.512k64.ld': ['thing', 'thingdev']},
           'flash_mode': {'dio': ['espduino', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'thingdev', 'd1_mini', 'd1'], 'dout': ['esp8285'], 'qio': ['generic', 'huzzah', 'nodemcu', 'modwifi', 'thing', 'esp210', 'espino', 'espinotee', 'coredev']},
           'flash_size': {'2M': ['modwifi'], '4M': ['espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee'], '512K': ['thing', 'thingdev']},
           'lwip_flags': {'': ['coredev']},
           'lwip_lib': {'-llwip': ['coredev']},
           'mcu': {'esp8266': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']},
           'spiffs_blocksize': {'4096': ['thing'], '8192': ['modwifi']},
           'spiffs_end': {'0x1FB000': ['modwifi'], '0x7B000': ['thing']},
           'spiffs_pagesize': {'256': ['generic', 'esp8285', 'modwifi', 'thing', 'espino', 'wifinfo', 'coredev']},
           'spiffs_start': {'0x100000': ['modwifi'], '0x6B000': ['thing']},
           'variant': {'ESPDuino': ['espduino'], 'adafruit': ['huzzah'], 'd1': ['d1'], 'd1_mini': ['d1_mini'], 'espino': ['espino'], 'espinotee': ['espinotee'], 'espresso_lite_v1': ['espresso_lite_v1'], 'espresso_lite_v2': ['espresso_lite_v2'], 'generic': ['generic', 'esp8285', 'modwifi', 'esp210', 'coredev'], 'nodemcu': ['nodemcu', 'nodemcuv2'], 'phoenix_v1': ['phoenix_v1'], 'phoenix_v2': ['phoenix_v2'], 'thing': ['thing', 'thingdev'], 'wifinfo': ['wifinfo']}},
 'menu': {'CpuFrequency': {'160': {'160 MHz': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev'], 'build': {'f_cpu': {'160000000L': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}}, '80': {'80 MHz': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev'], 'build': {'f_cpu': {'80000000L': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}}, 'CPU Frequency': [None]},
          'Debug': {'Debug port': [None], 'Disabled': {'Disabled': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'wifinfo', 'coredev'], 'build': {'debug_port': {'': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'wifinfo', 'coredev']}}}, 'Serial': {'Serial': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'wifinfo', 'coredev'], 'build': {'debug_port': {'-DDEBUG_ESP_PORT=Serial': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'wifinfo', 'coredev']}}}, 'Serial1': {'Serial1': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'wifinfo', 'coredev'], 'build': {'debug_port': {'-DDEBUG_ESP_PORT=Serial1': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'wifinfo', 'coredev']}}}},
          'DebugLevel': {'Core____': {'Core': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_CORE': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'Debug Level': [None],
                         'HTTPClient': {'HTTPClient': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_HTTP_CLIENT': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'HTTPClient2': {'HTTPClient + SSL': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_HTTP_CLIENT -DDEBUG_ESP_SSL': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'HTTPServer': {'HTTPServer': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_HTTP_SERVER': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'HTTPUpdate': {'HTTPUpdate': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_HTTP_UPDATE': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'HTTPUpdate2': {'HTTPClient + HTTPUpdate': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_HTTP_UPDATE -DDEBUG_ESP_HTTP_UPDATE': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'HTTPUpdate3': {'HTTPClient + HTTPUpdate + Updater': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_HTTP_UPDATE -DDEBUG_ESP_HTTP_UPDATE -DDEBUG_ESP_UPDATER': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'None': {'None': ['wifinfo'], 'build': {'debug_level': {'': ['wifinfo']}}},
                         'None____': {'None': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'OTA2____': {'OTA + Updater': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_OTA -DDEBUG_ESP_UPDATER': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'OTA_____': {'OTA': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_OTA': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'SSL_MEM_': {'Core + SSL + TLS Mem': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_CORE -DDEBUG_ESP_SSL -DDEBUG_TLS_MEM': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'SSL_____': {'Core + SSL': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_CORE -DDEBUG_ESP_SSL': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'UPDATER': {'Updater': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_UPDATER': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'WiFi____': {'WiFi': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_WIFI': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'WiFic___': {'Core + WiFi': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_CORE -DDEBUG_ESP_WIFI': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}},
                         'Wifinfo': {'Wifinfo': ['wifinfo'], 'build': {'debug_level': {'-DDEBUG_ESP_WIFINFO': ['wifinfo']}}},
                         'all_____': {'All': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev'], 'build': {'debug_level': {'-DDEBUG_ESP_CORE -DDEBUG_ESP_SSL -DDEBUG_ESP_WIFI -DDEBUG_ESP_HTTP_CLIENT -DDEBUG_ESP_HTTP_UPDATE -DDEBUG_ESP_HTTP_SERVER -DDEBUG_ESP_UPDATER -DDEBUG_ESP_OTA -DDEBUG_TLS_MEM': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcuv2', 'coredev']}}}},
          'ESPModule': {'ESP07192': {'ESP07 (1M/192K SPIFFS)': ['wifinfo'], 'build': {'board': {'ESP8266_ESP07': ['wifinfo']}, 'flash_ld': {'eagle.flash.1m192.ld': ['wifinfo']}, 'flash_size': {'1M': ['wifinfo']}, 'spiffs_blocksize': {'4096': ['wifinfo']}, 'spiffs_end': {'0xFB000': ['wifinfo']}, 'spiffs_start': {'0xCB000': ['wifinfo']}}, 'upload': {'maximum_size': {'827376': ['wifinfo']}}}, 'ESP12': {'ESP12 (4M/1M SPIFFS)': ['wifinfo'], 'build': {'board': {'ESP8266_ESP12': ['wifinfo']}, 'flash_ld': {'eagle.flash.4m1m.ld': ['wifinfo']}, 'flash_size': {'4M': ['wifinfo']}, 'spiffs_blocksize': {'8192': ['wifinfo']}, 'spiffs_end': {'0x3FB000': ['wifinfo']}, 'spiffs_pagesize': {'256': ['wifinfo']}, 'spiffs_start': {'0x300000': ['wifinfo']}}, 'upload': {'maximum_size': {'1044464': ['wifinfo']}}}, 'Module': [None]},
          'FlashFreq': {'40': {'40MHz': ['generic', 'wifinfo', 'coredev'], 'build': {'flash_freq': {'40': ['generic', 'wifinfo', 'coredev']}}}, '80': {'80MHz': ['generic', 'wifinfo', 'coredev'], 'build': {'flash_freq': {'80': ['generic', 'wifinfo', 'coredev']}}}, 'Flash Frequency': [None]},
          'FlashMode': {'Flash Mode': [None], 'dio': {'DIO': ['generic', 'espino', 'wifinfo', 'coredev'], 'build': {'flash_mode': {'dio': ['generic', 'espino', 'wifinfo', 'coredev']}}}, 'dout': {'DOUT': ['generic', 'coredev'], 'build': {'flash_mode': {'dout': ['generic', 'coredev']}}}, 'qio': {'QIO': ['generic', 'espino', 'wifinfo', 'coredev'], 'build': {'flash_mode': {'qio': ['generic', 'espino', 'wifinfo', 'coredev']}}}, 'qout': {'QOUT': ['generic', 'coredev'], 'build': {'flash_mode': {'qout': ['generic', 'coredev']}}}},
          'FlashSize': {'16M15M': {'16M (15M SPIFFS)': ['coredev'], 'build': {'flash_ld': {'eagle.flash.16m.ld': ['coredev']}, 'flash_size': {'16M': ['coredev']}, 'spiffs_blocksize': {'8192': ['coredev']}, 'spiffs_end': {'0x17FB000': ['coredev']}, 'spiffs_pagesize': {'256': ['coredev']}, 'spiffs_start': {'0x100000': ['coredev']}}, 'upload': {'maximum_size': {'1044464': ['coredev']}}},
                        '1M128': {'1M (128K SPIFFS)': ['generic', 'esp8285', 'coredev'], 'build': {'flash_ld': {'eagle.flash.1m128.ld': ['generic', 'esp8285', 'coredev']}, 'flash_size': {'1M': ['generic', 'esp8285', 'coredev']}, 'spiffs_blocksize': {'4096': ['generic', 'esp8285', 'coredev']}, 'spiffs_end': {'0xFB000': ['generic', 'esp8285', 'coredev']}, 'spiffs_start': {'0xDB000': ['generic', 'esp8285', 'coredev']}}, 'upload': {'maximum_size': {'892912': ['generic', 'esp8285', 'coredev']}}},
                        '1M144': {'1M (144K SPIFFS)': ['generic', 'esp8285', 'coredev'], 'build': {'flash_ld': {'eagle.flash.1m144.ld': ['generic', 'esp8285', 'coredev']}, 'flash_size': {'1M': ['generic', 'esp8285', 'coredev']}, 'spiffs_blocksize': {'4096': ['generic', 'esp8285', 'coredev']}, 'spiffs_end': {'0xFB000': ['generic', 'esp8285', 'coredev']}, 'spiffs_start': {'0xD7000': ['generic', 'esp8285', 'coredev']}}, 'upload': {'maximum_size': {'876528': ['generic', 'esp8285', 'coredev']}}},
                        '1M160': {'1M (160K SPIFFS)': ['generic', 'esp8285', 'coredev'], 'build': {'flash_ld': {'eagle.flash.1m160.ld': ['generic', 'esp8285', 'coredev']}, 'flash_size': {'1M': ['generic', 'esp8285', 'coredev']}, 'spiffs_blocksize': {'4096': ['generic', 'esp8285', 'coredev']}, 'spiffs_end': {'0xFB000': ['generic', 'esp8285', 'coredev']}, 'spiffs_start': {'0xD3000': ['generic', 'esp8285', 'coredev']}}, 'upload': {'maximum_size': {'860144': ['generic', 'esp8285', 'coredev']}}},
                        '1M192': {'1M (192K SPIFFS)': ['generic', 'esp8285', 'coredev'], 'build': {'flash_ld': {'eagle.flash.1m192.ld': ['generic', 'esp8285', 'coredev']}, 'flash_size': {'1M': ['generic', 'esp8285', 'coredev']}, 'spiffs_blocksize': {'4096': ['generic', 'esp8285', 'coredev']}, 'spiffs_end': {'0xFB000': ['generic', 'esp8285', 'coredev']}, 'spiffs_start': {'0xCB000': ['generic', 'esp8285', 'coredev']}}, 'upload': {'maximum_size': {'827376': ['generic', 'esp8285', 'coredev']}}},
                        '1M256': {'1M (256K SPIFFS)': ['generic', 'esp8285', 'coredev'], 'build': {'flash_ld': {'eagle.flash.1m256.ld': ['generic', 'esp8285', 'coredev']}, 'flash_size': {'1M': ['generic', 'esp8285', 'coredev']}, 'spiffs_blocksize': {'4096': ['generic', 'esp8285', 'coredev']}, 'spiffs_end': {'0xFB000': ['generic', 'esp8285', 'coredev']}, 'spiffs_start': {'0xBB000': ['generic', 'esp8285', 'coredev']}}, 'upload': {'maximum_size': {'761840': ['generic', 'esp8285', 'coredev']}}},
                        '1M512': {'1M (512K SPIFFS)': ['generic', 'esp8285', 'coredev'], 'build': {'flash_ld': {'eagle.flash.1m512.ld': ['generic', 'esp8285', 'coredev']}, 'flash_size': {'1M': ['generic', 'esp8285', 'coredev']}, 'spiffs_blocksize': {'8192': ['generic', 'esp8285', 'coredev']}, 'spiffs_end': {'0xFB000': ['generic', 'esp8285', 'coredev']}, 'spiffs_start': {'0x7B000': ['generic', 'esp8285', 'coredev']}}, 'upload': {'maximum_size': {'499696': ['generic', 'esp8285', 'coredev']}}},
                        '1M64': {'1M (64K SPIFFS)': ['generic', 'esp8285', 'coredev'], 'build': {'flash_ld': {'eagle.flash.1m64.ld': ['generic', 'esp8285', 'coredev']}, 'flash_size': {'1M': ['generic', 'esp8285', 'coredev']}, 'spiffs_blocksize': {'4096': ['generic', 'esp8285', 'coredev']}, 'spiffs_end': {'0xFB000': ['generic', 'esp8285', 'coredev']}, 'spiffs_start': {'0xEB000': ['generic', 'esp8285', 'coredev']}}, 'upload': {'maximum_size': {'958448': ['generic', 'esp8285', 'coredev']}}},
                        '2M': {'2M (1M SPIFFS)': ['generic', 'coredev'], 'build': {'flash_ld': {'eagle.flash.2m.ld': ['generic', 'coredev']}, 'flash_size': {'2M': ['generic', 'coredev']}, 'spiffs_blocksize': {'8192': ['generic', 'coredev']}, 'spiffs_end': {'0x1FB000': ['generic', 'coredev']}, 'spiffs_start': {'0x100000': ['generic', 'coredev']}}, 'upload': {'maximum_size': {'1044464': ['generic', 'coredev']}}},
                        '4M1M': {'4M (1M SPIFFS)': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev'], 'build': {'flash_ld': {'eagle.flash.4m1m.ld': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}, 'flash_size': {'4M': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}, 'spiffs_blocksize': {'8192': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}, 'spiffs_end': {'0x3FB000': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}, 'spiffs_pagesize': {'256': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}, 'spiffs_start': {'0x300000': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}}, 'upload': {'maximum_size': {'1044464': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'espino', 'coredev']}}},
                        '4M3M': {'4M (3M SPIFFS)': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev'], 'build': {'flash_ld': {'eagle.flash.4m.ld': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}, 'flash_size': {'4M': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}, 'spiffs_blocksize': {'8192': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}, 'spiffs_end': {'0x3FB000': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}, 'spiffs_pagesize': {'256': ['espduino', 'huzzah', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espinotee', 'coredev']}, 'spiffs_start': {'0x100000': ['generic', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'coredev']}}, 'upload': {'maximum_size': {'1044464': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'espino', 'coredev']}}},
                        '512K0': {'512K (no SPIFFS)': ['generic', 'coredev'], 'build': {'flash_ld': {'eagle.flash.512k0.ld': ['generic', 'coredev']}, 'flash_size': {'512K': ['generic', 'coredev']}}, 'upload': {'maximum_size': {'499696': ['generic', 'coredev']}}},
                        '512K128': {'512K (128K SPIFFS)': ['generic', 'coredev'], 'build': {'flash_ld': {'eagle.flash.512k128.ld': ['generic', 'coredev']}, 'flash_size': {'512K': ['generic', 'coredev']}, 'spiffs_blocksize': {'4096': ['generic', 'coredev']}, 'spiffs_end': {'0x7B000': ['generic', 'coredev']}, 'spiffs_start': {'0x5B000': ['generic', 'coredev']}}, 'upload': {'maximum_size': {'368624': ['generic', 'coredev']}}},
                        '512K64': {'512K (64K SPIFFS)': ['generic', 'coredev'], 'build': {'flash_ld': {'eagle.flash.512k64.ld': ['generic', 'coredev']}, 'flash_size': {'512K': ['generic', 'coredev']}, 'spiffs_blocksize': {'4096': ['generic', 'coredev']}, 'spiffs_end': {'0x7B000': ['generic', 'coredev']}, 'spiffs_start': {'0x6B000': ['generic', 'coredev']}}, 'upload': {'maximum_size': {'434160': ['generic', 'coredev']}}},
                        '8M7M': {'8M (7M SPIFFS)': ['coredev'], 'build': {'flash_ld': {'eagle.flash.8m.ld': ['coredev']}, 'flash_size': {'8M': ['coredev']}, 'spiffs_blocksize': {'8192': ['coredev']}, 'spiffs_end': {'0x7FB000': ['coredev']}, 'spiffs_pagesize': {'256': ['coredev']}, 'spiffs_start': {'0x100000': ['coredev']}}, 'upload': {'maximum_size': {'1044464': ['coredev']}}},
                        'Flash Size': [None]},
          'LwIPVariant': {'Espressif': {'Espressif (xcc)': ['coredev'], 'build': {'lwip_flags': {'-DLWIP_MAYBE_XCC': ['coredev']}, 'lwip_lib': {'-llwip': ['coredev']}}}, 'OpenSource': {'Open Source (gcc)': ['coredev'], 'build': {'lwip_flags': {'-DLWIP_OPEN_SRC': ['coredev']}, 'lwip_lib': {'-llwip_src': ['coredev']}}, 'recipe': {'hooks': {'sketch': {'prebuild': {'1': {'pattern': {'make -C "{runtime.platform.path}/tools/sdk/lwip/src" install TOOLS_PATH="{runtime.tools.xtensa-lx106-elf-gcc.path}/bin/xtensa-lx106-elf-"': ['coredev']}}}}}}}, 'Prebuilt': {'Prebuilt Source (gcc)': ['coredev'], 'build': {'lwip_flags': {'-DLWIP_OPEN_SRC': ['coredev']}, 'lwip_lib': {'-llwip_gcc': ['coredev']}}}, 'lwIP Variant': [None]},
          'ResetMethod': {'Reset Method': [None], 'ck': {'ck': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'espino', 'coredev'], 'upload': {'resetmethod': {'ck': ['generic', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'espino', 'coredev']}}}, 'nodemcu': ['nodemcu', 'nodemcu', 'nodemcu', 'nodemcu', 'nodemcu', 'nodemcu', 'nodemcu'], 'upload': {'resetmethod': {'nodemcu': ['nodemcu', 'nodemcu', 'nodemcu', 'nodemcu', 'nodemcu', 'nodemcu', 'nodemcu']}}},
          'UploadSpeed': {'115200': {'115200': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev'], 'upload': {'speed': {'115200': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}},
                          '230400': {'linux': {'230400': ['generic', 'esp8285', 'espduino', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}, 'macosx': {'230400': ['generic', 'esp8285', 'espduino', 'espduino', 'espresso_lite_v1', 'espresso_lite_v1', 'espresso_lite_v2', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v1', 'phoenix_v2', 'phoenix_v2', 'nodemcu', 'nodemcu', 'nodemcuv2', 'nodemcuv2', 'modwifi', 'modwifi', 'thing', 'thingdev', 'esp210', 'esp210', 'd1_mini', 'd1_mini', 'd1', 'd1', 'espino', 'espinotee', 'espinotee', 'wifinfo', 'coredev']}, 'upload': {'speed': {'230400': ['generic', 'esp8285', 'espduino', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}},
                          '256000': {'256000': ['huzzah'], 'upload': {'speed': {'256000': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}, 'windows': {'256000': ['generic', 'esp8285', 'espduino', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}},
                          '460800': {'linux': {'460800': ['generic', 'esp8285', 'espduino', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}, 'macosx': {'460800': ['generic', 'esp8285', 'espduino', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}, 'upload': {'speed': {'460800': ['generic', 'esp8285', 'espduino', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}},
                          '512000': {'upload': {'speed': {'512000': ['generic', 'esp8285', 'espduino', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}, 'windows': {'512000': ['generic', 'esp8285', 'espduino', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}},
                          '57600': {'57600': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev'], 'upload': {'speed': {'57600': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}},
                          '921600': {'921600': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev'], 'upload': {'speed': {'921600': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}},
                          '9600': {'9600': ['generic', 'esp8285', 'espduino', 'huzzah', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev'], 'upload': {'speed': {'9600': ['generic', 'esp8285', 'espduino', 'huzzah', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}},
                          'Upload Speed': [None]}},
 'name': {'Adafruit HUZZAH ESP8266': ['huzzah'], 'Core Development Module': ['coredev'], 'ESPDuino (ESP-13 Module)': ['espduino'], 'ESPino (ESP-12 Module)': ['espino'], 'ESPresso Lite 1.0': ['espresso_lite_v1'], 'ESPresso Lite 2.0': ['espresso_lite_v2'], 'Generic ESP8266 Module': ['generic'], 'Generic ESP8285 Module': ['esp8285'], 'NodeMCU 0.9 (ESP-12 Module)': ['nodemcu'], 'NodeMCU 1.0 (ESP-12E Module)': ['nodemcuv2'], 'Olimex MOD-WIFI-ESP8266(-DEV)': ['modwifi'], 'Phoenix 1.0': ['phoenix_v1'], 'Phoenix 2.0': ['phoenix_v2'], 'SparkFun ESP8266 Thing': ['thing'], 'SparkFun ESP8266 Thing Dev': ['thingdev'], 'SweetPea ESP-210': ['esp210'], "ThaiEasyElec's ESPino": ['espinotee'], 'WeMos D1 R2 & mini': ['d1_mini'], 'WeMos D1(Retired)': ['d1'], 'WifInfo': ['wifinfo']},
 'serial': {'disableDTR': {'true': ['generic', 'esp8285', 'espduino', 'huzzah', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}, 'disableRTS': {'true': ['generic', 'esp8285', 'espduino', 'huzzah', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}},
 'upload': {'maximum_data_size': {'81920': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}, 'maximum_size': {'1044464': ['espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee'], '434160': ['generic', 'esp8285', 'thing', 'thingdev', 'wifinfo', 'coredev']}, 'resetmethod': {'ck': ['generic', 'esp8285', 'espduino', 'modwifi', 'thing', 'esp210', 'espino', 'coredev'], 'nodemcu': ['huzzah', 'nodemcu', 'nodemcuv2', 'thingdev', 'd1_mini', 'd1', 'espinotee', 'wifinfo']}, 'speed': {'115200': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'esp210', 'espino', 'espinotee', 'wifinfo', 'coredev'], '460800': ['d1_mini', 'd1'], '921600': ['thing', 'thingdev']}, 'tool': {'esptool': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}, 'wait_for_upload_port': {'true': ['generic', 'esp8285', 'espduino', 'huzzah', 'espresso_lite_v1', 'espresso_lite_v2', 'phoenix_v1', 'phoenix_v2', 'nodemcu', 'nodemcuv2', 'modwifi', 'thing', 'thingdev', 'esp210', 'd1_mini', 'd1', 'espino', 'espinotee', 'wifinfo', 'coredev']}}}
