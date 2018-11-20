
Host Tests for Continuous Integration
-------------------------------------

	make FORCE32=0 OPTZ=-O0 CI

	(FORCE32=0: https://bugs.launchpad.net/ubuntu/+source/valgrind/+bug/948004)

Sketch emulation on host
------------------------

This environment let compile esp8266/Arduino sketches into native
environment.  Network (tcp, udp, including ssl and multicast) is linked to
local host interfaces.  WiFi is trivialy emulated and reported as "just"
already connected and usable.

Currently network emulation is a complete rewrite of
WiFiClient+WiFiServer/ClientContext and WifiUdp/UdpContext using socket
posix API.  Further work will optionally propose native lwIP library
instead.

How to compile and run a sketch
-------------------------------

All results are stored in ./bin/ .

Show the core example list:
	make list


Build one example
	make D=1 ../../libraries/esp8266/examples/Blink/Blink
run it:
	./bin/Blink/Blink -h


Optional 'V=1' enables makefile verbosity
Optional 'D=1' enables core debug (same as IDE's tools menu)
Optional 'OPTZ=-O2' will update gcc -O option (default is -Os, D=1 implies -O0)
Optional 'FORCE32=0' will use native/default gcc (default is FORCE32=1 unless gcc-multilib is not detected)


Non exhaustive list of working examples:
	make D=1 ../../libraries/ESP8266WiFi/examples/udp/udp
	make D=1 ../../libraries/ESP8266WiFi/examples/WiFiClient/WiFiClient
	make D=1 ../../libraries/ESP8266WebServer/examples/HelloServer/HelloServer
	make D=1 ../../libraries/ESP8266WebServer/examples/AdvancedWebServer/AdvancedWebServer
	make D=1 ../../libraries/ESP8266mDNS/examples/mDNS_Web_Server/mDNS_Web_Server
	make D=1 ../../libraries/ESP8266WiFi/examples/BearSSL_Validation/BearSSL_Validation

Compile other sketches:
- library paths are specified using ULIBDIRS variable, separated by ':'
- call 'make path-to-the-sketch-file' to build (without its '.ino' extension):
- CAVEAT: functions must be declared *before* being called (arduino builder is not around)

	make D=1  ULIBDIRS=/path/to/your/arduino/libraries/lib1:/path/to/another/place/lib2  /path/to/your/sketchdir/sketch/sketch
  or:
	ULIBDIRS=/path/to/your/arduino/libraries/lib1:/path/to/another/place/lib2  make  D=1  /path/to/your/sketchdir/sketch/sketch

  or (preferred):
	export ULIBDIRS=/path/to/your/arduino/libraries/lib1:/path/to/another/place/lib2
	export D=1
	export OPTZ=-O2
	make clean
	make /path/to/your/sketchdir/sketch/sketch
	./bin/sketch/sketch


Executable location is always in bin/. Once a sketch is compiled, just run it:
	bin/sketch/sketch

Options are available:
	-h
	-i eth0		bind servers to this interface (WIP)
	-l		bind Multicast to the above interface (WIP)
	-f              no throttle (possibly 100%CPU)

TODO
----
A lot.
Make fun, propose PRs.

- replace some "fprintf(stderr" with redirectable log functions
- spiffs in a file (done, need to initialize and check)
- EEPROM in a file (partly done)
- SDCARD on Host filesystem ? or in an image ?
- nice curses interface to display/change gpios ?
- display device emulation (like ssd1306)
- optionaly use arduino-builder ?
- store sketch objects and binaries outside from the source directories (done for sketches)
- compile and use lwIP on host
- easily debug HTTP classes
- https://github.com/esp8266/Arduino/issues/1715
- gpio, currently:
  read as 0(digital) or 512(analog).
  output is printed on console.
