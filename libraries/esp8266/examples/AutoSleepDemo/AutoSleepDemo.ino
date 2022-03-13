/*
  ESP8266 auto sleep mode with webserver example

  Copyright (c) 2021 Dirk O. Kaar. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


  This example starts in the default sleep mode, which would be auto modem sleep.
  Use a web browser to open http://.../sleep to let it switch into auto light sleep,
  open http://.../nosleep to revert to the previous auto modem sleep.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PolledTimeout.h>
#include <Schedule.h>

#ifndef D5
#define D5 (14)
#endif

// enter your WiFi configuration below
const char* AP_SSID = "SSID";  // your router's SSID here
const char* AP_PASS = "PSK";   // your router's password here

uint32_t timeout = 30E3;                                 // 30 second timeout on the WiFi connection
esp8266::polledTimeout::oneShotMs wifiTimeout(timeout);  // 30 second timeout on WiFi connection

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!\r\n");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(74880);
  while (!Serial)
    ;
  delay(100);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  Serial.print(F("connecting to WiFi "));
  Serial.println(WiFi.SSID());

  wifiTimeout.reset(timeout);
  while (((!WiFi.localIP()) || (WiFi.status() != WL_CONNECTED)) && (!wifiTimeout)) {
    yield();
  }
  if ((WiFi.status() != WL_CONNECTED) || !WiFi.localIP()) {
    Serial.println(F("WiFi timed out and didn't connect"));
  } else {
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
  }
  WiFi.setAutoReconnect(true);

  // if (MDNS.begin("esp8266")) {
  //     Serial.println("MDNS responder started");
  // }

  server.on("/", handleRoot);

  server.on("/on", []() {
    tone(D5, 440);
    server.send(200, "text/plain", "tone on");
  });

  server.on("/off", []() {
    noTone(D5);
    server.send(200, "text/plain", "tone off");
  });

  server.on("/sleep", []() {
    ESP.autoLightSleep();
    server.send(200, "text/plain", "auto light sleep on");
  });

  server.on("/nosleep", []() {
    ESP.autoSleepOff();
    server.send(200, "text/plain", "sleep off");
  });

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("HTTP server started");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  server.handleClient();
  digitalWrite(LED_BUILTIN, HIGH);
  // in order to see relevant durations when the CPU is auto light sleeping
  // in this example, regardless of the added latency for web requests:
  delay(300);
}
