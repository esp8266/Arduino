---
title: Change Log
---

## Current version

### Core

- Allow control of enabling debug and debug level from IDE
- Make HardwareSerial::begin() and end() interrupt safe
- Put HardwareSerial and cbuf methods called from interrupt context in RAM
- Re-enable interrupts before directly enqueuing characters in the UART FIFO
- Add espduino board
- Rework StreamString::write to use String internal buffer directly (#1289)
- Add function to measure stack high water mark
- Update SDK to esp_iot_sdk_v1.5.0_15_12_15_p1
- Fix RAM corruption caused by our hook of register_chipv6_phy(init_data*).
- Optimize PWM interrupt handler for better precision
- Add warning levels configurable through Preferences
- Protect HardwareSerial's cbuf usage with InterruptLock
- SPIFFS: check if path length is valid (#1089)
- Set CPU frequency before running setup
- Add core_esp8266_features.h to be able to detect the features and libraries included in the ESP core
- Added ESPino to supported boards

### Libraries

- ESP8266HTTPClient: add CHUNKED encoding support (#1324)
- Fixed crash bug with mDNS where a string buffer could be used uninitialized
- Add WiFi TX power control
- Add WiFi sleep management
- Allow to hook into WiFi events from sketch
- Allow setting TCP timeout
- Add setSleepMode + getSleepMode and setPhyMode + getPhyMode to WiFi
- Update GDBStub library with the source of esp-gdbstub
- Servo: fix detach and attach
- ESP8266mDNS: refactoring, add TXT support
- Add HTTP Basic Auth to WebServer and libb64 (base64) to core
- Fix link-time dependency of ESP8266WebServer on SPIFFS (#862)
- Allow setting client side TLS key and certificate
- Replace chain of UDP pbufs with a single pbuf before sending (#1009)

### Tools

- espota.py: add support for manually selecting ip and port for host side

---
## 2.0.0
November 30, 2015

Package link: `http://arduino.esp8266.com/versions/2.0.0/package_esp8266com_index.json`.

### Core

- Add file system APIs and documentation
- Add ConfigFile example
- Allow user to run code in user_rf_pre_init
- Add strtoul and strtol, fix strtod
- Update documentation for NodeMCU and Olimex boards
- Disable interrupts inside ESP.getVcc (#567)
- Erase RTC RAM only if RF mode looks invalid (#619)
- Get pin levels at time of interrupt, rather than the time of calling the handler.
- Move interrupt handlers to ram.
- Improve debug output on critical errors
- Add ArduinoOTA library and docs
- Add WeMos D1 & D1 mini boards
- Add documentation about boot messages and mode meaning
- Disable sleep mode before doing OTA (#1005)
- Add the ability to be called back when the device is about to reset
- Add "Reset Method" menu
- Add MD5 to core
- I2C: generate STOP in case of NACK (fix #698, #254)
- Add libc time functions
- Fix linker script for 512k(no SPIFFS) variant (#966)
- I2S optimizations
- Support Sketch > Export compiled binary
- Update SPIFFS wrapper for 0.3.3
- Fix placement of code into RAM, enable gc-sections
- Make soft wdt reset more obvious
- Force disable IOSWAP for UART0 in HardwareSerial initialization (#744)
- Add IPAddress::toString()


### Libraries

- ESP8266WebServer: support for sending of PROGMEM strings
- ESP8266WebServer: support for serving files from file system
- ESP8266WiFi: fix mode selection (#529)
- ESP8266mDNS: allow to work on SoftAP interface
- EEPROM: round requested size to 4 bytes (#659)
- Add ESP8266AVRISP library
- Add ESP8266HTTPUpdate library
- Add HTTPClient library
- Add WiFiClientSecure
- ESP8266WiFi library: add persistent option, fix #1054
- Make RequestHandler handle uploads
- Add Digest Authentication to OTA and espota.py
- Don't close UDP pcbs when WiFi connection drops (#969)
- Add espsoftwareserial library
- Add HTTP Updater library
- Add Ethernet library for W5100
- Add SPIFFS WebServer Example
- add dnsIP() to ESP8266WiFi class
- OTA support encapsulated to ArduinoOTA class
- Add gdb stub library
- Extracted the WebUpdate example into a library.
- Fix to Servo allowing write() to be called before attach()
- ESP8266WiFi: add function `begin` without any parameters and add `psk` function to return current PSK form sdk config
- Fix a crash due to abort() called from TCP error callback (#428)
- Adding support for OPTIONS requests to ESP8266WebServer
- Add HTTPS request sample (#43)
- Fix _useClientMode & _useApMode in SDK auto connect mode (#754)
- Add ESP8266WebServer::sendContent_P with 'size_t size' argument for binary content
- Fix bug in WiFiClient::write_P when content was binary
- Add WiFiClient::write_P to be used with PROGMEM

### Tools

- Update SDK to 1.3.0_15_08_10_p1
- Update esptool to 0.4.6
- Bump toolchain version to force libm update on Windows
- ESP8266FS tool update

---
## 1.6.5-947-g39819f0
July 23, 2015

Package link: `http://arduino.esp8266.com/versions/1.6.5-947-g39819f0/package_esp8266com_index.json`.

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

Package link: `http://arduino.esp8266.com/versions/1.6.4-673-g8cd3697/package_esp8266com_index.json`.

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
