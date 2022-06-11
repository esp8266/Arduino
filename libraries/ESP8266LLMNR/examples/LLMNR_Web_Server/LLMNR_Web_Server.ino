/*
   ESP8266 LLMNR responder sample
   Copyright (C) 2017 Stephen Warren <swarren@wwwdotorg.org>

   Based on:
   ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
   Version 1.1
   Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
   ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
   MDNS-SD Support 2015 Hristo Gochkov
   Extended MDNS-SD support 2016 Lars Englund (lars.englund@gmail.com)

   License (MIT license):

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

*/
/*
   This is an example of an HTTP server that is accessible via http://esp8266/
   (or perhaps http://esp8266.local/) thanks to the LLMNR responder.

   Instructions:
   - Update WiFi SSID and password as necessary.
   - Flash the sketch to the ESP8266 board.
   - Windows:
     - No additional software is necessary.
     - Point your browser to http://esp8266/, you should see a response. In most
       cases, it is important that you manually type the "http://" to force the
       browser to search for a hostname to connect to, rather than perform a web
       search.
     - Alternatively, run the following command from the command prompt:
       ping esp8266
   - Linux:
     - To validate LLMNR, install the systemd-resolve utility.
     - Execute the following command:
       systemd-resolve -4 -p llmnr esp8266
     - It may be possible to configure your system to use LLMNR for all name
       lookups. However, that is beyond the scope of this description.

*/

#include <ESP8266WiFi.h>
#include <ESP8266LLMNR.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer web_server(80);

void handle_http_not_found() {
  web_server.send(404, "text/plain", "Not Found");
}

void handle_http_root() {
  web_server.send(200, "text/plain", "It works!");
}

void setup(void) {
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start LLMNR responder
  LLMNR.begin("esp8266");
  Serial.println("LLMNR responder started");
  // Start HTTP server
  web_server.onNotFound(handle_http_not_found);
  web_server.on("/", handle_http_root);
  web_server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  web_server.handleClient();
}
