ESP8266 LLMNR (Link-Local Multicast Name Resolution)
====================================================

This is a simple implementation of an LLMNR responder the ESP8266 Arduino
package. Only support for advertizing a single hostname is currently
implemented.

LLMNR is a very similar protocol to MDNS. The primary practical difference is
that Windows systems (at least Windows 7 and later; perhaps earlier) support
the protocol out-of-the-box, whereas additional software is required to support
MDNS. However, Linux support is currently more complex, and MacOS X support
appears non-existent.

Requirements
------------
- ESP8266WiFi library
- LLMNR support in your operating system/client machines:
  - For Windows, support is already built in (in Windows 7 at least).
  - For Linux, the systemd-resolve application supports LLMNR.
  - For Mac OSX: Unknown; likely not supported.

Usage
-----
1. Install this repository using the instructions in the top-levle README.md
   file.
2. Include the ESP8266LLMNR library in the sketch.
3. Call the LLMNR.begin() method in the sketch's setup() function, and provide
   the hostname to advertize. This should not include any ".local" prefix.
4. If ESP8266 AP mode is enabled, disabled, or the WiFi or AP configuration is
   changed, call LLMNR.notify_ap_change() after the change is made.

See the included LLMNR + HTTP server sketch for a full example.

References
----------
1. https://tools.ietf.org/html/rfc4795 (LLMNR)
2. https://tools.ietf.org/html/rfc1035 (DNS)

Caveats
-------
1. LLMNR implementations MUST support EDNS0 [RFC2671] and extended RCODE
   values. It is likely that this implementation does not; I have not read
   that RFC.
2. LLMNR responders MUST support listening for TCP queries. This implementation
   does not.
3. On receiving an LLMNR query, the responder MUST check whether it was sent to
   an LLMNR multicast addresses defined in Section 2. If it was sent to another
   multicast address, then the query MUST be silently discarded. This
   implementation makes no such check; it is hoped that the ESP8266 network
   stack filters out such packets since the code only joins the relevant
   multicast group and does not listen for unicast packets. This assumption may
   be invalid.
4. Prior to sending an LLMNR response with the 'T' bit clear, a responder
   configured with a UNIQUE name MUST verify that there is no other host within
   the scope of LLMNR query propagation that is authoritative for the same name
   on that interface. This implementation performs no such verification.
5. Prior to verifying that its name is UNIQUE, a responder MUST set the 'T' bit
   in responses. This implementation does not; it assumes that name is unique
   and responds with the 'T' bit clear in all cases.
6. To verify uniqueness, a responder MUST send an LLMNR query with the 'C' bit
   clear, over all protocols on which it responds to LLMNR queries (IPv4 and/or
   IPv6). This implementation does not.

License
-------
Copyright (C) 2017 Stephen Warren <swarren@wwwdotorg.org>

Based on:
ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
Version 1.1
Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
MDNS-SD Suport 2015 Hristo Gochkov
Extended MDNS-SD support 2016 Lars Englund (lars.englund@gmail.com)

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
