Changelog
=========

2.3.0
-----

June 23, 2016

Package link:
``http://arduino.esp8266.com/versions/2.3.0/package_esp8266com_index.json``.

Core
~~~~

-  Fix NMI interrupt handler alignment
-  Update SDK to 1.5.3
-  umm\_malloc: print block start address before heap corruption
   callback is triggered
-  If GDBStub library is used, break into gdb on assert and panic
-  Add option to keep FS classes in namespace (#2030)
-  Add SPIFFS::end (#1657)
-  Add ArduinoOTA::getHostname() interface
-  Add \_\_throw\_out\_of\_range
-  Add support for RTC user memory in ESP-specific APIs. (#1836)
-  Expose RTC\_USER\_MEM in esp8266\_peri.h
-  Remove DISABLED macro (#2072)
-  Execute global constructors in correct order (#2074)
-  Real board name available in Sketch/MDNS/OTA (#2054)
-  Add DOUT/QOUT flash modes
-  Add ESP8285 entry in boards menu
-  Move timer detachInterrupt functions into IRAM (#2083)
-  Make Updater be able to run inside async callbacks (#2096)
-  Add new boards Phoenix 1.0 & Phoenix 2.0 (#2088)
-  Store git version of the core in the compiled binary (#2099)
-  Rebuild libstdc++ with mlongcalls and link against it (#1983)
-  Add mechanism for posting functions to the main loop (#2082)
-  MD5Builder::addStream: fixed falsy calculated hash for len >
   filelength (#2126)
-  Fix SPIFFS.openDir("") (#2143)
-  Bring back old semantics to random and randomSeed, add secureRandom
   (#1710) (#2142)
-  Add missing pgm\_read\_ptr{\_near/\_far} macros (#2160)
-  Add macro for maximum open SPIFFS files, settings it to 1 saves about
   1k heap. (#2167)
-  Fix UART pins setting (#2098)
-  Fix ESP.getSketchSize, add ESP.getSketchMD5 (#2158)
-  Add Serial.baudRate() to get current baud rate (#2079)

Libraries
~~~~~~~~~

-  SNI support in WiFiClientSecure (#1285)
-  Update axTLS to 139914f
-  HTTPClient: return error when HTTPClient::begin is called with HTTPS
   URL without certificate fingerprint (#1941)
-  HTTPClient: fix default port not being set
-  HTTPClient: fix handling of chunked transfer encoding (#1975)
-  ESP8266SSDP: switch SSDP send arguments around
-  ESP8266WiFi: fix UdpContext::peek to return int (#1946)
-  ESP8266WiFi: fix WiFiSleepType\_t values to match SDK ones
-  LwIP: use gcc-built LwIP by default (#1926)
-  LwIP: fix crash in igmp\_start\_timer (#1826)
-  HTTPClient: include non-standard ports in Host: header
-  ESP8266WiFi: Prevent WiFi config corruption (#1997 #1856 #1699 #1675)
-  GDBStub: fix section attribute for core gdbstub functions
-  Wire: I2C bus reset with info to user
-  ESP8266HTTPClient: allow HTTP header value without LWS
-  ESP8266mDNS: Fix mDNS doesn't accept queryService responses from
   avahi-daemon (#2015)
-  Add MFRC522 to supported libraries (#2044)
-  Update axTLS to ab516f7 (1.5.3+)
-  Mention ESP8266Ping library
-  ESP8266HTTPClient: fix duplicate Content-Length headers (#1902)
-  ESP8266HTTPUpdateServer: make HTTP Update Server more secure (#2104)
-  ESP8266WiFi: add virtual destructor to WiFiServer class (#2116)
-  ESP8266WiFi: fix error when calling ``WiFiServer::close`` more than
   once
-  ESP8266WiFi: WiFi event handling refactoring (#2119)
-  ESP8266mDNS: restart listening when WiFi STA is
   connected/disconnected (#1828)
-  ESP8266WiFi: allow DHCP client to be re-enabled using WiFi.config(0U,
   0U, 0U) (#1896)
-  ESP8266WiFi: enable SO\_REUSE in LwIP and WiFiServer (#1431)
-  ESP8266WebServer: make ESP8266WebServer::urlDecode public (#1419)
-  LwIP: sntp\_localtime: return -1 in tm\_isdst field (#2010)
-  ESP8266WiFi: fix for crash in WiFiClientSecure when WiFi is
   disconnected (#2139)
-  SD: Prevent WDT resets in SD library (#1815)
-  ESP8266WiFi: Fix issue when WiFi.begin(ssid, pass) is called right
   after WiFi.mode(WIFI\_OFF)

Tools
~~~~~

-  Python 3 compatibility for get.py
-  Device side test library and test runner
-  Fix ARM toolchain files permissions (#2004)
-  Update esptool to 0.4.9

2.2.0
-----

April 18, 2016

Package link:
``http://arduino.esp8266.com/versions/2.2.0/package_esp8266com_index.json``.

Core
~~~~

-  Leverage realloc() in String::changeBuffer()
-  Clean up core files
-  Add host side tests
-  Fix possible null pointer in umm\_malloc
-  Remove "Upload Using" option from Tools menu
-  Move attachInterrupt and detachInterrupt into IRAM (#1734)
-  Implement strstr\_P
-  Allow indefinite duration for tone()
-  Fix crashes when using tone()
-  Fix RF\_MODE and ADC\_MODE
-  Move micros, delayMicroseconds, millis to IRAM (#1326)
-  Fix pulseIn (#1072, #1149)
-  Accept both named constant and ADC channel number in analogRead
   (#1766)
-  Enable heap poisoning only when debug options are enabled (#1800)
-  Bootloader: don't touch RTC memory if it doesn't contain a valid
   command (#619)
-  Update SDK to 1.5.2 (#1653)
-  Clean up variants, fix digitalPinHasPWM definition (#1831)
-  Don't set RF mode on boot unless it was overridden
-  Change build.board property for boards which renumber pins like
   NodeMCU (#1878)
-  Fix Exception 2 when using printf or vprintf

Libraries
~~~~~~~~~

-  Update axTLS to 5b4be7d
-  WiFiClientSecure: implement connection timeout, fix connected method
   behavior
-  WiFiClient: fix write behavior when connection is closed by remote
   side
-  ESP8266HTTPServer: add font MIME types, fix #1601
-  ESP8266mDNS: add client support
-  Update SPIFFS to 82aeac6
-  Servo: move some functions into IRAM (#1742)
-  Update SoftwareSerial to version 3.1.0
-  ESP8266SSDP: change templates to include deviceType
-  ESP8266WebServer: handle more file types
-  SPI: add CPOL setting
-  ESP8266WebServer: Fix buffer overflow in
   ESP8266WebServer::authenticate (#1790)
-  ESP8266WiFi: fix undefined behavior in WiFiServer::setNoDelay (#1695)
-  Servo: use peripheral clock frequency when calculating FRC1 tick
   count (#1789)
-  ESP8266WiFi: avoid multiple instances of INADDR\_NONE
-  Add LwIP binary built with gcc
-  ESP8266WiFi: Allow PSK instead of passphrase in WiFiSTA::begin
-  SPI: Fix SPI.transfer16() using wrong endianness
-  HTTPClient: decouple transport layer handling + save some RAM
-  ESP8266httpUpdate: decouple HTTPS overloads + save some RAM
-  Update and move lwIP headers, add options to use different lwIP build
-  ESP8266WebServer: wait for data to arrive
-  ESP8266WebServer: save RAM by moving response strings to flash
   (#1732)
-  SPI: Speed up SPI.writePattern()

Tools
~~~~~

-  Add ARM tools (#269)

2.0.0
-----

November 30, 2015

Package link:
``http://arduino.esp8266.com/versions/2.0.0/package_esp8266com_index.json``.

Core
~~~~

-  Add file system APIs and documentation
-  Add ConfigFile example
-  Allow user to run code in user\_rf\_pre\_init
-  Add strtoul and strtol, fix strtod
-  Update documentation for NodeMCU and Olimex boards
-  Disable interrupts inside ESP.getVcc (#567)
-  Erase RTC RAM only if RF mode looks invalid (#619)
-  Get pin levels at time of interrupt, rather than the time of calling
   the handler.
-  Move interrupt handlers to ram.
-  Improve debug output on critical errors
-  Add ArduinoOTA library and docs
-  Add WeMos D1 & D1 mini boards
-  Add documentation about boot messages and mode meaning
-  Disable sleep mode before doing OTA (#1005)
-  Add the ability to be called back when the device is about to reset
-  Add "Reset Method" menu
-  Add MD5 to core
-  I2C: generate STOP in case of NACK (fix #698, #254)
-  Add libc time functions
-  Fix linker script for 512k(no SPIFFS) variant (#966)
-  I2S optimizations
-  Support Sketch > Export compiled binary
-  Update SPIFFS wrapper for 0.3.3
-  Fix placement of code into RAM, enable gc-sections
-  Make soft wdt reset more obvious
-  Force disable IOSWAP for UART0 in HardwareSerial initialization
   (#744)
-  Add IPAddress::toString()

Libraries
~~~~~~~~~

-  ESP8266WebServer: support for sending of PROGMEM strings
-  ESP8266WebServer: support for serving files from file system
-  ESP8266WiFi: fix mode selection (#529)
-  ESP8266mDNS: allow to work on SoftAP interface
-  EEPROM: round requested size to 4 bytes (#659)
-  Add ESP8266AVRISP library
-  Add ESP8266HTTPUpdate library
-  Add HTTPClient library
-  Add WiFiClientSecure
-  ESP8266WiFi library: add persistent option, fix #1054
-  Make RequestHandler handle uploads
-  Add Digest Authentication to OTA and espota.py
-  Don't close UDP pcbs when WiFi connection drops (#969)
-  Add espsoftwareserial library
-  Add HTTP Updater library
-  Add Ethernet library for W5100
-  Add SPIFFS WebServer Example
-  add dnsIP() to ESP8266WiFi class
-  OTA support encapsulated to ArduinoOTA class
-  Add gdb stub library
-  Extracted the WebUpdate example into a library.
-  Fix to Servo allowing write() to be called before attach()
-  ESP8266WiFi: add function ``begin`` without any parameters and add
   ``psk`` function to return current PSK form sdk config
-  Fix a crash due to abort() called from TCP error callback (#428)
-  Adding support for OPTIONS requests to ESP8266WebServer
-  Add HTTPS request sample (#43)
-  Fix \_useClientMode & \_useApMode in SDK auto connect mode (#754)
-  Add ESP8266WebServer::sendContent\_P with 'size\_t size' argument for
   binary content
-  Fix bug in WiFiClient::write\_P when content was binary
-  Add WiFiClient::write\_P to be used with PROGMEM

Tools
~~~~~

-  Update SDK to 1.3.0\_15\_08\_10\_p1
-  Update esptool to 0.4.6
-  Bump toolchain version to force libm update on Windows
-  ESP8266FS tool update

1.6.4-673-g8cd3697
------------------

May 22, 2015

Package link:
``http://arduino.esp8266.com/versions/1.6.4-673-g8cd3697/package_esp8266com_index.json``.

Tools
~~~~~

-  Add 32-bit Linux toolchain.
-  Rebuild toolchain and esptool with support for OS X down to 10.6.

Libraries
~~~~~~~~~

-  Better connection handling in ESP8266WebServer. The server now sends
   Content-Length and Connection: close headers, then waits for the
   client to disconnect. By not closing the connection actively, server
   avoids TIME\_WAIT TCP state, and TCP stack is able to release the
   memory immediately, without waiting for 2xMSL period. If the client
   doesn't disconnect in 2000ms, the server closes the connection
   actively.
-  Add Hash library, which has a function to calculate SHA1 hash.
-  SD, Adafruit\_ILI9341, and OneWire libraries are now bundled.
-  Fix incorrect sector calculation in EEPROM library.

--------------

1.6.4-628-g545ffde
------------------

May 19, 2015

-  Initial release of Boards Manager package for ESP8266 platform.
