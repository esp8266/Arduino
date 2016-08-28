---
title: Libraries
---

## Table of Contents
  * [WiFi(ESP8266WiFi library)](#wifiesp8266wifi-library)
  * [Ticker](#ticker)
  * [EEPROM](#eeprom)
  * [I2C (Wire library)](#i2c-wire-library)
  * [SPI](#spi)
  * [SoftwareSerial](#softwareserial)
  * [ESP\-specific APIs](#esp-specific-apis)
  * [mDNS and DNS\-SD responder (ESP8266mDNS library)](#mdns-and-dns-sd-responder-esp8266mdns-library)
  * [SSDP responder (ESP8266SSDP)](#ssdp-responder-esp8266ssdp)
  * [DNS server (DNSServer library)](#dns-server-dnsserver-library)
  * [Servo](#servo)
  * [Other libraries (not included with the IDE)](#other-libraries-not-included-with-the-ide)

## WiFi(ESP8266WiFi library)

This is mostly similar to WiFi shield library. Differences include:

- `WiFi.mode(m)`: set mode to `WIFI_AP`, `WIFI_STA`, `WIFI_AP_STA` or `WIFI_OFF`.
- call `WiFi.softAP(ssid)` to set up an open network
- call `WiFi.softAP(ssid, password)` to set up a WPA2-PSK network (password should be at least 8 characters)
- `WiFi.macAddress(mac)` is for STA, `WiFi.softAPmacAddress(mac)` is for AP.
- `WiFi.localIP()` is for STA, `WiFi.softAPIP()` is for AP.
- `WiFi.printDiag(Serial)` will print out some diagnostic info
- `WiFiUDP` class supports sending and receiving multicast packets on STA interface.
When sending a multicast packet, replace `udp.beginPacket(addr, port)` with
`udp.beginPacketMulticast(addr, port, WiFi.localIP())`.
When listening to multicast packets, replace `udp.begin(port)` with
`udp.beginMulticast(WiFi.localIP(), multicast_ip_addr, port)`.
You can use `udp.destinationIP()` to tell whether the packet received was
sent to the multicast or unicast address.

`WiFiServer`, `WiFiClient`, and `WiFiUDP` behave mostly the same way as with WiFi shield library.
Four samples are provided for this library.
You can see more commands here: [http://www.arduino.cc/en/Reference/WiFi](http://www.arduino.cc/en/Reference/WiFi)

## Ticker

Library for calling functions repeatedly with a certain period. Two examples included.

It is currently not recommended to do blocking IO operations (network, serial, file) from Ticker
callback functions. Instead, set a flag inside the ticker callback and check for that flag inside the loop function.

Here is library to simplificate `Ticker` usage and avoid WDT reset: [TickerScheduler](https://github.com/Toshik/TickerScheduler)

## EEPROM

This is a bit different from standard EEPROM class. You need to call `EEPROM.begin(size)`
before you start reading or writing, size being the number of bytes you want to use.
Size can be anywhere between 4 and 4096 bytes.

`EEPROM.write` does not write to flash immediately, instead you must call `EEPROM.commit()`
whenever you wish to save changes to flash. `EEPROM.end()` will also commit, and will
release the RAM copy of EEPROM contents.

EEPROM library uses one sector of flash located just after the SPIFFS.

Three examples included.

## I2C (Wire library)

Wire library currently supports master mode up to approximately 450KHz.
Before using I2C, pins for SDA and SCL need to be set by calling
`Wire.begin(int sda, int scl)`, i.e. `Wire.begin(0, 2)` on ESP-01,
else they default to pins 4(SDA) and 5(SCL).

## SPI

SPI library supports the entire Arduino SPI API including transactions, including setting phase (CPHA).
Setting the Clock polarity (CPOL) is not supported, yet (SPI_MODE2 and SPI_MODE3 not working).

## SoftwareSerial

An ESP8266 port of SoftwareSerial library done by Peter Lerup (@plerup) supports baud rate up to 115200 and multiples SoftwareSerial instances. See https://github.com/plerup/espsoftwareserial if you want to suggest an improvement or open an issue related to SoftwareSerial.

## ESP-specific APIs

APIs related to deep sleep and watchdog timer are available in the `ESP` object, only available in Alpha version.

`ESP.deepSleep(microseconds, mode)` will put the chip into deep sleep. `mode` is one of `WAKE_RF_DEFAULT`, `WAKE_RFCAL`, `WAKE_NO_RFCAL`, `WAKE_RF_DISABLED`. (GPIO16 needs to be tied to RST to wake from deepSleep.)

`ESP.rtcUserMemoryWrite(offset, &data, sizeof(data))` and `ESP.rtcUserMemoryRead(offset, &data, sizeof(data))` allow data to be stored in and retrieved from the RTC user memory of the chip respectively. Total size of RTC user memory is 512 bytes, so offset + sizeof(data) shouldn't exceed 512. Data should be 4-byte aligned. The stored data can be retained between deep sleep cycles. However, the data might be lost after power cycling the chip.

`ESP.restart()` restarts the CPU.

`ESP.getResetReason()` returns String containing the last reset resaon in human readable format.

`ESP.getFreeHeap()` returns the free heap size.

`ESP.getChipId()` returns the ESP8266 chip ID as a 32-bit integer.

Several APIs may be used to get flash chip info:

`ESP.getFlashChipId()` returns the flash chip ID as a 32-bit integer.

`ESP.getFlashChipSize()` returns the flash chip size, in bytes, as seen by the SDK (may be less than actual size).

`ESP.getFlashChipSpeed(void)` returns the flash chip frequency, in Hz.

`ESP.getCycleCount()` returns the cpu instruction cycle count since start as an unsigned 32-bit.  This is useful for accurate timing of very short actions like bit banging.

`ESP.getVcc()` may be used to measure supply voltage. ESP needs to reconfigure the ADC
at startup in order for this feature to be available. Add the following line to the top
of your sketch to use `getVcc`:

```c++
ADC_MODE(ADC_VCC);
```

TOUT pin has to be disconnected in this mode.

Note that by default ADC is configured to read from TOUT pin using `analogRead(A0)`, and
`ESP.getVCC()` is not available.

## mDNS and DNS-SD responder (ESP8266mDNS library)

Allows the sketch to respond to multicast DNS queries for domain names like "foo.local", and DNS-SD (service dicovery) queries.
See attached example for details.

## SSDP responder (ESP8266SSDP)

SSDP is another service discovery protocol, supported on Windows out of the box. See attached example for reference.

## DNS server (DNSServer library)

Implements a simple DNS server that can be used in both STA and AP modes. The DNS server currently supports only one domain (for all other domains it will reply with NXDOMAIN or custom status code). With it clients can open a web server running on ESP8266 using a domain name, not an IP address.
See attached example for details.

## Servo

This library exposes the ability to control RC (hobby) servo motors. It will support upto 24 servos on any available output pin. By defualt the first 12 servos will use Timer0 and currently this will not interfere with any other support.  Servo counts above 12 will use Timer1 and features that use it will be effected.
While many RC servo motors will accept the 3.3V IO data pin from a ESP8266, most will not be able to run off 3.3v and will require another power source that matches their specifications.  Make sure to connect the grounds between the ESP8266 and the servo motor power supply.

## Other libraries (not included with the IDE)

Libraries that don't rely on low-level access to AVR registers should work well. Here are a few libraries that were verified to work:

- [Adafruit_ILI9341](https://github.com/Links2004/Adafruit_ILI9341) - Port of the Adafruit ILI9341 for the ESP8266 
- [arduinoVNC](https://github.com/Links2004/arduinoVNC) - VNC Client for Arduino
- [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets) - WebSocket Server and Client compatible with ESP8266 (RFC6455)
- [aREST](https://github.com/marcoschwartz/aREST) - REST API handler library.
- [Blynk](https://github.com/blynkkk/blynk-library) - easy IoT framework for Makers (check out the [Kickstarter page](http://tiny.cc/blynk-kick)).
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library.git)
- [DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library) - Arduino library for the DHT11/DHT22 temperature and humidity sensors. Download latest v1.1.1 library and no changes are necessary.  Older versions should initialize DHT as follows: `DHT dht(DHTPIN, DHTTYPE, 15)`
- [DimSwitch](https://github.com/krzychb/DimSwitch) - Control electronic dimmable ballasts for fluorescent light tubes remotely as if using a wall switch.
- [Encoder](https://github.com/PaulStoffregen/Encoder) - Arduino library for rotary encoders. Version 1.4 supports ESP8266.
- [esp8266_mdns](https://github.com/mrdunk/esp8266_mdns) - mDNS queries and responses on esp8266. Or to describe it another way: An mDNS Client or Bonjour Client library for the esp8266.
- [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) - Asynchronous TCP Library for ESP8266 and ESP32/31B
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - Asynchronous Web Server Library for ESP8266 and ESP32/31B
- [Homie for ESP8266](https://github.com/marvinroger/homie-esp8266) - Arduino framework for ESP8266 implementing Homie, an MQTT convention for the IoT.
- [NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) - Adafruit's NeoPixel library, now with support for the ESP8266 (use version 1.0.2 or higher from Arduino's library manager).
- [NeoPixelBus](https://github.com/Makuna/NeoPixelBus) - Arduino NeoPixel library compatible with ESP8266. Use the "DmaDriven" or "UartDriven" branches for ESP8266. Includes HSL color support and more.
- [PubSubClient](https://github.com/Imroy/pubsubclient) - MQTT library by @Imroy.
- [RTC](https://github.com/Makuna/Rtc) - Arduino Library for Ds1307 & Ds3231 compatible with ESP8266.
- [Souliss, Smart Home](https://github.com/souliss/souliss) - Framework for Smart Home based on Arduino, Android and openHAB.
- [ST7735](https://github.com/nzmichaelh/Adafruit-ST7735-Library) - Adafruit's ST7735 library modified to be compatible with ESP8266.  Just make sure to modify the pins in the examples as they are still AVR specific.
- [Task](https://github.com/Makuna/Task) - Arduino Nonpreemptive multitasking library.  While similiar to the included Ticker library in the functionality provided, this library was meant for cross Arduino compatibility.
- [TickerScheduler](https://github.com/Toshik/TickerScheduler) - Library provides simple scheduler for `Ticker` to avoid WDT reset
- [Teleinfo](https://github.com/hallard/LibTeleinfo) - Generic French Power Meter library to read Teleinfo energy monitoring data such as consuption, contract, power, period, ... This library is cross platform, ESP8266, Arduino, Particle, and simple C++. French dedicated [post](https://hallard.me/libteleinfo/) on author's blog and all related information about [Teleinfo](https://hallard.me/category/tinfo/) also available.
- [UTFT-ESP8266](https://github.com/gnulabis/UTFT-ESP8266) - UTFT display library with support for ESP8266. Only serial interface (SPI) displays are supported for now (no 8-bit parallel mode, etc). Also includes support for the hardware SPI controller of the ESP8266.
- [WiFiManager](https://github.com/tzapu/WiFiManager) - WiFi Connection manager with web captive portal. If it can't connect, it starts AP mode and a configuration portal so you can choose and enter WiFi credentials.
- [OneWire](https://github.com/PaulStoffregen/OneWire) - Library for Dallas/Maxim 1-Wire Chips.
- [Adafruit-PCD8544-Nokia-5110-LCD-Library](https://github.com/WereCatf/Adafruit-PCD8544-Nokia-5110-LCD-library) - Port of the Adafruit PCD8544 - library for the ESP8266.
- [PCF8574_ESP](https://github.com/WereCatf/PCF8574_ESP) - A very simplistic library for using the PCF8574/PCF8574A I2C 8-pin GPIO-expander.
- [Dot Matrix Display Library 2](https://github.com/freetronics/DMD2) - Freetronics DMD & Generic 16 x 32 P10 style Dot Matrix Display Library
- [SdFat-beta](https://github.com/greiman/SdFat-beta) - SD-card library with support for long filenames, software- and hardware-based SPI and lots more.
- [FastLED](https://github.com/FastLED/FastLED) - a library for easily & efficiently controlling a wide variety of LED chipsets, like the Neopixel (WS2812B), DotStar, LPD8806 and many more. Includes fading, gradient, color conversion functions.
- [OLED](https://github.com/klarsys/esp8266-OLED) - a library for controlling I2C connected OLED displays. Tested with 0.96 inch OLED graphics display.
- [MFRC522](https://github.com/miguelbalboa/rfid) - A library for using the Mifare RC522 RFID-tag reader/writer.
- [Ping](https://github.com/dancol90/ESP8266Ping) - lets the ESP8266 ping a remote machine.
