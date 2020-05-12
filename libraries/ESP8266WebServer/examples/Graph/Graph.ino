/*
  Graph - A web-based Graph display of ESP8266 data

  This file is part of the ESP8266WebServer library for Arduino environment.

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

  See readme.md for more information.
*/

////////////////////////////////

// Select the FileSystem by uncommenting one of the lines below

//#define USE_SPIFFS
#define USE_LITTLEFS
//#define USE_SDFS

////////////////////////////////

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>

#if defined USE_SPIFFS
#include <FS.h>
FS* fileSystem = &SPIFFS;
SPIFFSConfig fileSystemConfig = SPIFFSConfig();
#elif defined USE_LITTLEFS
#include <LittleFS.h>
FS* fileSystem = &LittleFS;
LittleFSConfig fileSystemConfig = LittleFSConfig();
#elif defined USE_SDFS
#include <SDFS.h>
FS* fileSystem = &SDFS;
SDFSConfig fileSystemConfig = SDFSConfig();
// fileSystemConfig.setCSPin(chipSelectPin);
#else
#error Please select a filesystem first by uncommenting one of the "#define USE_xxx" lines at the beginning of the sketch.
#endif


#define DBG_OUTPUT_PORT Serial

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

// Indicate which digital I/Os should be displayed on the chart.
// From GPIO16 to GPIO0, a '1' means the corresponding GPIO will be shown
// Note: SD GPIOs are hidden by default:
#define DEFAULT_GPIO_MASK 0b11111000000111111

unsigned int gpioMask = DEFAULT_GPIO_MASK;

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "graph";

ESP8266WebServer server(80);

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char FS_INIT_ERROR[] PROGMEM = "FS INIT ERROR";
static const char FILE_NOT_FOUND[] PROGMEM = "FileNotFound";

////////////////////////////////
// Utils to return HTTP codes

void replyOK() {
  server.send(200, FPSTR(TEXT_PLAIN), "");
}

void replyOKWithMsg(String msg) {
  server.send(200, FPSTR(TEXT_PLAIN), msg);
}

void replyNotFound(String msg) {
  server.send(404, FPSTR(TEXT_PLAIN), msg);
}

void replyBadRequest(String msg) {
  DBG_OUTPUT_PORT.println(msg);
  server.send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void replyServerError(String msg) {
  DBG_OUTPUT_PORT.println(msg);
  server.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

////////////////////////////////
// Request handlers

/*
   Read the given file from the filesystem and stream it back to the client
*/
bool handleFileRead(String path) {
  DBG_OUTPUT_PORT.println(String("handleFileRead: ") + path);

  if (path.endsWith("/")) {
    path += "index.htm";
  }

  String contentType = mime::getContentType(path);

  if (!fileSystem->exists(path)) {
    // File not found, try gzip version
    path = path + ".gz";
  }
  if (fileSystem->exists(path)) {
    File file = fileSystem->open(path, "r");
    if (server.streamFile(file, contentType) != file.size()) {
      DBG_OUTPUT_PORT.println("Sent less data than expected!");
    }
    file.close();
    return true;
  }

  return false;
}


/*
   The "Not Found" handler catches all URI not explicitely declared in code
   First try to find and return the requested file from the filesystem,
   and if it fails, return a 404 page with debug information
*/
void handleNotFound() {
  String uri = ESP8266WebServer::urlDecode(server.uri()); // required to read paths with blanks

  if (handleFileRead(uri)) {
    return;
  }

  // Dump debug data
  String message;
  message.reserve(100);
  message = F("Error: File not found\n\nURI: ");
  message += uri;
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += '\n';
  for (uint8_t i = 0; i < server.args(); i++) {
    message += F(" NAME:");
    message += server.argName(i);
    message += F("\n VALUE:");
    message += server.arg(i);
    message += '\n';
  }
  message += "path=";
  message += server.arg("path");
  message += '\n';
  DBG_OUTPUT_PORT.print(message);

  return replyNotFound(message);
}

void setup(void) {
  ////////////////////////////////
  // SERIAL INIT
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.setDebugOutput(true);
  DBG_OUTPUT_PORT.print('\n');

  ////////////////////////////////
  // FILESYSTEM INIT

  fileSystemConfig.setAutoFormat(false);
  fileSystem->setConfig(fileSystemConfig);
  boolean fsOK = fileSystem->begin();
  DBG_OUTPUT_PORT.println(fsOK ? F("Filesystem initialized.") : F("Filesystem init failed!"));

  ////////////////////////////////
  // PIN INIT
  pinMode(4, INPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(15, OUTPUT);

  ////////////////////////////////
  // WI-FI INIT
  DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DBG_OUTPUT_PORT.print(".");
  }
  DBG_OUTPUT_PORT.println("");
  DBG_OUTPUT_PORT.print(F("Connected! IP address: "));
  DBG_OUTPUT_PORT.println(WiFi.localIP());

  ////////////////////////////////
  // MDNS INIT
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    DBG_OUTPUT_PORT.print(F("Open http://"));
    DBG_OUTPUT_PORT.print(host);
    DBG_OUTPUT_PORT.println(F(".local to open the graph page"));
  }

  ////////////////////////////////
  // WEB SERVER INIT

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/espData", HTTP_GET, []() {
    String json;
    json.reserve(88);
    json = "{\"time\":";
    json += millis();
    json += ", \"heap\":";
    json += ESP.getFreeHeap();
    json += ", \"analog\":";
    json += analogRead(A0);
    json += ", \"gpioMask\":";
    json += gpioMask;
    json += ", \"gpioData\":";
    json += (uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16));
    json += "}";
    server.send(200, "text/json", json);
  });

  // Default handler for all URIs not defined above
  // Use it to read files from filesystem
  server.onNotFound(handleNotFound);


  // Start server
  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");

  DBG_OUTPUT_PORT.println("Please pull GPIO4 low (e.g. press button) to switch output mode:");
  DBG_OUTPUT_PORT.println(" 0 (OFF):    outputs are off and hidden from chart");
  DBG_OUTPUT_PORT.println(" 1 (AUTO):   outputs are rotated automatically every second");
  DBG_OUTPUT_PORT.println(" 2 (MANUAL): outputs can be toggled from the web page");

}

int rgbMode = 1; // 0=off - 1=auto - 2=manual
int rgbValue = 0;
long lastChangeTime = 0;
boolean modeChangeRequested = false;

void loop(void) {
  server.handleClient();
  MDNS.update();

  if (digitalRead(4) == 0) {
    // button pressed
    modeChangeRequested = true;
  }

  // see if one second has passed since last change
  long now = millis();
  if (now - lastChangeTime > 1000) {
    // see if a mode change was requested
    if (modeChangeRequested) {
      // increment mode (reset after 2)
      rgbMode++;
      if (rgbMode > 2) {
        rgbMode = 0;
      }

      modeChangeRequested = false;
    }

    // act according to mode
    switch (rgbMode) {
      case 0: // off
        gpioMask = 0b10100000000111111; // GPIOs 12-13-15 are hidden
        // output 0
        digitalWrite(12, 0);
        digitalWrite(13, 0);
        digitalWrite(15, 0);
        break;

      case 1: // auto
        gpioMask = DEFAULT_GPIO_MASK;
        // increment value (reset after 7)
        rgbValue++;
        if (rgbValue > 7) {
          rgbValue = 0;
        }

        // output values
        digitalWrite(12, rgbValue & 0b001);
        digitalWrite(13, rgbValue & 0b010);
        digitalWrite(15, rgbValue & 0b100);
        break;

      case 2: // manual
        gpioMask = DEFAULT_GPIO_MASK;
        // don't change values
        break;
    }

    // update last change time
    lastChangeTime = now;
  }
}
