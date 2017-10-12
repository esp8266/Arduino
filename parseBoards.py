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

# myPprint(menu, 0)
print("Menu")
myPprint2(menu)
print("customMenus")
myPprint2(customMenus)
print("genericSettings")
pp.pprint(genericSettings)
pp.pprint(settings)
