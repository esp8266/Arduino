/* Changes :
 *  
 * Sketch:
 * - #define logic to select FS
 * - switched from SD to SDFS
 * - begin() does not support parameters > removed SS and added optional config
 * - LittleFS.open() second parametsr is mandatory > specified "r" where needed
 * - 'FILE_WRITE' was not declared in this scope > replaced by "w"
 * 
 * Web page:
 * - Tree panel width is now proportional (20%) to see long names on big screens
 * - Added an icon for files, and indented them to the same level as folders
 * - Changed file/folder icon set to use lighter and more neutral ones from https://feathericons.com/ (passed the result through compresspng.com and base64-image.de)
 * - Items are now sorted (folders first, then plain files, each in alphabetic order)
 * - Added file size after each file name
 * - Added FS status information at the top right
 * - Replaced that FS status by operation status when upload/delete is in progress
 * - Filled filename box in header with the name of the currently selected file
 *
 * TODO:
 * - Fix Editor (add Save/Discard buttons ?)
 * - Dim screen (and http://www.ajaxload.info/ ) when Upload/Delete in progress ?
 * - Test on SPIFFS
 */

/*
  WebFileManager - A web-based File Manager for ESP8266 filesystems
  (unified from former FSBrowser, FSWebServer and SDWebServer examples)

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
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



  This example implements a file manager using http requests and a html/javascript frontend.
  The filesystem itself can be SDFS, SPIFFS, LittleFS and has to be selected using one of the "#define USE_xxx" directives below.
  See https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html for more information on FileSystems.

  This example requires that a copy of the 'data' folder be first copied to the filesystem. To do so:
  - for SDFS, copy that contents to the root of a FAT/FAT32-formated SD card connected to the SPI port of the ESP8266
  - for SPIFFS or LittleFS, please follow the instructions at https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#uploading-files-to-file-system

  Once the data and sketch have been uploaded, access the editor by going to http://espfilemanager.local/edit

  Notes:
  - For SDFS, if your card's CS pin is not connected the default pin (4), enable the line just before "FILESYSTEM.begin()", 
  specifying the GPIO the CS pin is connected to
  - index.htm is the default index (works on subfolders as well)
  - Filesystem limitations apply. For example, SDFS is limited to 8.3 filenames - https://en.wikipedia.org/wiki/8.3_filename .
  SPIFFS and LittleFS also have limitations. Plese see the https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#spiffs-file-system-limitations
  - Directories are supported on SDFS and LittleFS. On SPIFFS, all files are at the root, although their names may contain the "/" character

*/

//////////////////

// Select the FileSystem by uncommenting one of the lines below

#define USE_SDFS
//#define USE_SPIFFS
//#define USE_LITTLEFS

//////////////////

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>

#if defined USE_SDFS
  #define FILESYSTEM SDFS
  #include <SDFS.h>
#elif defined USE_SPIFFS
  #define FILESYSTEM SPIFFS
  #include <FS.h>
#elif defined USE_LITTLEFS
  #define FILESYSTEM LittleFS
  #include <LittleFS.h>
#else 
  #error Please select a filesystem first by uncommenting one of the "#define USE_xxx" lines at the beginning of the sketch.
#endif

#define DBG_OUTPUT_PORT Serial

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "webfilemanager";

ESP8266WebServer server(80);

static bool hasSD = false;
File uploadFile;


void returnOK() {
  server.send(200, "text/plain", "");
}

void returnFail(String msg) {
  server.send(500, "text/plain", msg + "\r\n");
}

bool loadFromSdCard(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) {
    path += "index.htm";
  }

  if (path.endsWith(".src")) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if (path.endsWith(".htm")) {
    dataType = "text/html";
  } else if (path.endsWith(".css")) {
    dataType = "text/css";
  } else if (path.endsWith(".js")) {
    dataType = "application/javascript";
  } else if (path.endsWith(".png")) {
    dataType = "image/png";
  } else if (path.endsWith(".gif")) {
    dataType = "image/gif";
  } else if (path.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if (path.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if (path.endsWith(".xml")) {
    dataType = "text/xml";
  } else if (path.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if (path.endsWith(".zip")) {
    dataType = "application/zip";
  }

  File dataFile = FILESYSTEM.open(path.c_str(), "r");
  if (dataFile.isDirectory()) {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = FILESYSTEM.open(path.c_str(), "r");
  }

  if (!dataFile) {
    return false;
  }

  if (server.hasArg("download")) {
    dataType = "application/octet-stream";
  }

  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    DBG_OUTPUT_PORT.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

void handleFileUpload() {
  if (server.uri() != "/edit") {
    return;
  }
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    if (FILESYSTEM.exists((char *)upload.filename.c_str())) {
      FILESYSTEM.remove((char *)upload.filename.c_str());
    }
    uploadFile = FILESYSTEM.open(upload.filename.c_str(), "w");
    DBG_OUTPUT_PORT.print("Upload: START, filename: "); DBG_OUTPUT_PORT.println(upload.filename);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
    DBG_OUTPUT_PORT.print("Upload: WRITE, Bytes: "); DBG_OUTPUT_PORT.println(upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
    DBG_OUTPUT_PORT.print("Upload: END, Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void deleteRecursive(String path) {
  File file = FILESYSTEM.open((char *)path.c_str(), "r");
  if (!file.isDirectory()) {
    file.close();
    FILESYSTEM.remove((char *)path.c_str());
    return;
  }

  file.rewindDirectory();
  while (true) {
    File entry = file.openNextFile();
    if (!entry) {
      break;
    }
    String entryPath = path + "/" + entry.name();
    if (entry.isDirectory()) {
      entry.close();
      deleteRecursive(entryPath);
    } else {
      entry.close();
      FILESYSTEM.remove((char *)entryPath.c_str());
    }
    yield();
  }

  FILESYSTEM.rmdir((char *)path.c_str());
  file.close();
}

void handleDelete() {
  if (server.args() == 0) {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  if (path == "/" || !FILESYSTEM.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }
  deleteRecursive(path);
  returnOK();
}

void handleCreate() {
  if (server.args() == 0) {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  if (path == "/" || FILESYSTEM.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }

  if (path.indexOf('.') > 0) {
    File file = FILESYSTEM.open((char *)path.c_str(), "w");
    if (file) {
      file.write((const char *)0);
      file.close();
    }
  } else {
    FILESYSTEM.mkdir((char *)path.c_str());
  }
  returnOK();
}

void handleList() {
  if (!server.hasArg("dir")) {
    return returnFail("BAD ARGS");
  }
  String path = server.arg("dir");
  if (path != "/" && !FILESYSTEM.exists((char *)path.c_str())) {
    return returnFail("BAD PATH");
  }
  File dir = FILESYSTEM.open((char *)path.c_str(), "r");
  path = String();
  if (!dir.isDirectory()) {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/json", "");
  WiFiClient client = server.client();

  server.sendContent("[");
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }

    String output;
    if (cnt > 0) {
      output = ',';
    }

    output += "{\"type\":\"";
    if (entry.isDirectory()) {
      output += "dir";
    }
    else {
      output += "file";
      output += "\",\"size\":\"";
      output += entry.size();      
    }
    output += "\",\"name\":\"";
    output += entry.name();
    output += "\"";
    output += "}";
    server.sendContent(output);
    entry.close();
  }
  server.sendContent("]");
  server.sendContent(""); // Terminate the HTTP chunked transmission with a 0-length chunk
  dir.close();
}

void handleStatus() {
  FSInfo fs_info;
  FILESYSTEM.info(fs_info);

  String json = String("{\"totalBytes\":\"") + fs_info.totalBytes
  + "\", \"usedBytes\":\"" + fs_info.usedBytes
  + "\"}";

  server.send(200, "application/json", json);
}

void handleNotFound() {
  if (hasSD && loadFromSdCard(server.uri())) {
    return;
  }
  String message = "SDCARD Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  DBG_OUTPUT_PORT.print(message);
}

void setup(void) {
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.setDebugOutput(true);
  DBG_OUTPUT_PORT.print("\n");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  DBG_OUTPUT_PORT.print("Connecting to ");
  DBG_OUTPUT_PORT.println(ssid);

  // Wait for connection
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) {//wait 10 seconds
    delay(500);
  }
  if (i == 21) {
    DBG_OUTPUT_PORT.print("Could not connect to");
    DBG_OUTPUT_PORT.println(ssid);
    while (1) {
      delay(500);
    }
  }
  DBG_OUTPUT_PORT.print("Connected! IP address: ");
  DBG_OUTPUT_PORT.println(WiFi.localIP());

  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    DBG_OUTPUT_PORT.println("MDNS responder started");
    DBG_OUTPUT_PORT.print("You can now connect to http://");
    DBG_OUTPUT_PORT.print(host);
    DBG_OUTPUT_PORT.println(".local");
  }


  server.on("/status",HTTP_GET, handleStatus);
  server.on("/list",  HTTP_GET, handleList);
  server.on("/edit",  HTTP_DELETE, handleDelete);
  server.on("/edit",  HTTP_PUT, handleCreate);
  server.on("/edit",  HTTP_POST, []() {
    returnOK();
  }, handleFileUpload);
  server.onNotFound(handleNotFound);

  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");

  // SDFS.setConfig(SDFSConfig().setCSPin(chipSelectPin));
  if (FILESYSTEM.begin()) {
    DBG_OUTPUT_PORT.println("SD Card initialized.");
    hasSD = true;
  }
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
