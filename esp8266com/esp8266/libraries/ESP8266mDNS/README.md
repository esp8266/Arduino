ESP8266 Multicast DNS
====================

A port of CC3000 Multicast DNS library (version 1.1)

This is a simple implementation of multicast DNS query support for an Arduino
running on ESP8266 chip. Only support for resolving address queries is currently
implemented.

Requirements
------------
- ESP8266WiFi library
- MDNS support in your operating system/client machines:
  - For Mac OSX support is built in through Bonjour already.
  - For Linux, install [Avahi](http://avahi.org/).
  - For Windows, install [Bonjour](http://www.apple.com/support/bonjour/).

Usage
-----
1. Download this repository as a zip (button on the right) and follow [these instructions to install into Arduino](http://arduino.cc/en/Guide/Libraries).
2. Include the ESP8266mDNS library in the sketch.
3. Create an instance of the MDNSResponder class.
4. Call the begin method in the sketch's setup and provide a domain name (without
   the '.local' suffix, i.e. just provide 'foo' to resolve 'foo.local'), and the 
   IP address to advertise.  Optionally provide a time to live (in seconds) 
   for the DNS record--the default is 1 hour.
5. Call the update method in each iteration of the sketch's loop function.

See the included MDNS + HTTP server sketch for a full example.

License
-------
Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.