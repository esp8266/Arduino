---
title: Change log
---

*Current release*

### Core

- I2C library updated to better handle repeated start for certain devices,
  improved waveforms, higher frequencies for 160MHz core clock, fix case where
  using different pins would not work with libs calling begin internally.
- Add Adafruit HUZZAH board
- Add SparkFun Thing board
- Add SweetPea ESP-210 board
- Add eboot bootloader
- Timer0 support
- Add PWM range and frequency control
- Add ESP.eraseConfig method
- Fix pin change interrupt handling (#322)
- Add SLC and I2S register definitions
- Fix math functions calling themselves recursively (#233, #354)
- Print stack on exception and soft WDT reset
- Add Updater class
- Remove implementations of WDT-related functions
- Provide selection between A0 and VCC (#443, #338)

### Libraries

- ESP8266WebServer: add gzip streaming, fix sendContent behaviour,
  add setContentSize method.
- ESP8266WiFi: add BSSID, channel, isHidden methods, fix AP/STA mode
  selection (#28).
- Better handling of WiFi disconnect (#231)
- Add API to set the beginning of local ports range for WiFiClient.
- Add RSSI function
- Add function to get the MAC / BSSID as String
- Servo library support
- Add ESP8266WiFiMesh library
- Add ESP8266SSDP library
- Add DNS-SD support to ESP8266mDNS library

### Tools

- Update SDK to v1.2.0_15_07_03
- Better sketch size reporting (#314)
- Update esptool to 0.4.5

---

## 1.6.4-673-g8cd3697
May 22, 2015

### Tools

- Add 32-bit Linux toolchain.
- Rebuild toolchain and esptool with support for OS X down to 10.6.

### Libraries

- Better connection handling in ESP8266WebServer.
  The server now sends Content-Length and Connection: close headers,
  then waits for the client to disconnect. By not closing the connection
  actively, server avoids TIME_WAIT TCP state, and TCP stack is able to
  release the memory immediately, without waiting for 2xMSL period.
  If the client doesn't disconnect in 2000ms, the server closes the connection
  actively.
- Add Hash library, which has a function to calculate SHA1 hash.
- SD, Adafruit_ILI9341, and OneWire libraries are now bundled.
- Fix incorrect sector calculation in EEPROM library.

---

## 1.6.4-628-g545ffde
May 19, 2015

- Initial release of Boards Manager package for ESP8266 platform.
