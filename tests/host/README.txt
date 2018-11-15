
Host Tests for Continuous Integration
-------------------------------------

	make CI


Host emulation (WIP)
--------------------

This environment let compile esp8266/Arduino sketches into native
environment.  Network (tcp, udp, including ssl and multicast) is linked to
local host interfaces.  WiFi is trivialy emulated and reported as "just"
already connected and usable.

Currently network emulation is a complete rewrite of
WiFiClient+WiFiServer/ClientContext and WifiUdp/UdpContext using socket
posix API.  Further work will optionally propose native lwIP library
instead.

How to compile a sketch
-----------------------

All results are stored or linked from ./bin/ .

Show the core example list:
	make list


Build one example
	make D=1 ../../libraries/esp8266/examples/Blink/Blink
run it:
	./bin/Blink -h


Optionnal 'D=1' enables core debug.


Non exhaustive list of working examples:
	make D=1 ../../libraries/ESP8266WiFi/examples/udp/udp
	make D=1 ../../libraries/ESP8266WiFi/examples/WiFiClient/WiFiClient
	make D=1 ../../libraries/ESP8266WebServer/examples/HelloServer/HelloServer
	make D=1 ../../libraries/ESP8266WebServer/examples/AdvancedWebServer/AdvancedWebServer
	make D=1 ../../libraries/ESP8266mDNS/examples/mDNS_Web_Server/mDNS_Web_Server
	

Compile other sketches:
- library paths are specified using ULIBDIRS variable, separated by ':'
- call 'make path-to-the-sketch-file' to build (without its '.ino' extension):

	make D=1  ULIBDIRS=/path/to/your/arduino/libraries:/path/to/another/one  /path/to/your/sketchdir/sketch/sketch
  or
	ULIBDIRS=/path/to/your/arduino/libraries:/path/to/another/one  make  D=1  /path/to/your/sketchdir/sketch/sketch
  or
	export ULIBDIRS=/path/to/your/arduino/libraries:/path/to/another/one
	make  D=1  /path/to/your/sketchdir/sketch/sketch


Build all examples with debug enabled (D=1): (not fully working yet)
	make D=1 examples


Running a sketch
----------------

- SPIFFS
  (already done, to check)
  
- SDCARD
  (TODO)

- EEPROM
  (to check) a file named 'eeprom' is created in current directory for reading and writing.

- Serial:
  for now Serial.read() returns -1. Serial.write() will printf().

- GPIOs
  read as 0(digital) or 512(analog).
  output is printed on console.

Once a sketch is compiled, just run it:
	/path/to/your/sketchdir/sketch/sketch

Options are available:
	-h
	-i eth0		bind servers to this interface (WIP)
	-l		bind Multicast to the above interface (WIP)

TODO
----
A lot.
Make fun, propose PRs.
