/* Changes :
 *  
 * Fixes to work on LittleFS based on SDFS
 * - #define logic to select FS
 * - switched from SD to SDFS
 * - begin() does not support parameters > removed SS and added optional config
 * - LittleFS.open() second parametsr is mandatory > specified "r" where needed
 * - 'FILE_WRITE' was not declared in this scope > replaced by "w"
 * 
 * UI improvements:
 * - Tree panel width is now proportional (20%) to see long names on big screens 
 * - Added an icon for files, and indented them to the same level as folders
 * - Changed file/folder icon set to use lighter and more neutral ones from https://feathericons.com/ (passed the result through compresspng.com and base64-image.de)
 * - Items are now sorted (folders first, then plain files, each in alphabetic order)
 * - Added file size after each file name
 * - Added FS status information at the top right
 * - Replaced that FS status by operation status when async operations are in progress
 * - Filled filename box in header with the name of the last clicked file
 * - Selecting a file for upload proposes to put it in the same folder as the last clicked file
 * - Removed limitation to 8.3 lowercase filenames
 * - Removed limitation "files must have an extension, folders may not"
 * - Improved refresh of parts of the tree (e.g. upon file delete, refresh subfolder, not root)
 *
 * TODO:
 * - test creation of edit.txt/EDIT.TXT on LittleFS (should succeed). (Correclty supported on SPIFFS but not on SDFS)
 * - Support Filenames without extension, Dirnames with extension ?
 * - Cleanup (look for TODO below)
 * - Reload tree (size) and status on Editor Save
 * - Add Editor Save/Discard buttons ?
 * - Can we query the fatType of the SDFS ?
 * 
 * TEST: (*) = failed vXXX = OK
 * - On SPIFFS:
 *  - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload at root
 *  - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload in subfolder
 * - with Long Filenames:
 *  - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload at root
 *  - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload in subfolder
 * 
 * - On LittleFS:
 *  - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload / vMkdir at root
 *  - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload / vMkdir in subfolder
 * - with Long Filenames:
 *  - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload / vMkdir at root
 *  - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload / vMkdir in subfolder
 * 
 * - On SDFS:
 * - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload / vMkdir at root
 * - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload / vMkdir in subfolder
 * - with Long Filenames:
 * - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload / vMkdir at root
 * - vList / vEdit / vView image / vMkFile / vDownload / vDelete / vUpload / vMkdir in subfolder
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

  Once the data and sketch have been uploaded, access the editor by going to http://webfilemanager.local/edit

  Notes:
  - For SDFS, if your card's CS pin is not connected the default pin (4), enable the line "fileSystemConfig.setCSPin(chipSelectPin);"
  specifying the GPIO the CS pin is connected to
  - index.htm is the default index (works on subfolders as well)
  - Filesystem limitations apply. For example, FAT16 is limited to 8.3 filenames - https://en.wikipedia.org/wiki/8.3_filename .
  SPIFFS and LittleFS also have limitations. Plese see the https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#spiffs-file-system-limitations
  - Directories are supported on SDFS and LittleFS. On SPIFFS, all files are at the root, although their names may contain the "/" character

*/

////////////////////////////////

// Select the FileSystem by uncommenting one of the lines below

#define USE_SPIFFS
//#define USE_LITTLEFS
//#define USE_SDFS

////////////////////////////////

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>

#if defined USE_SPIFFS
  #include <FS.h>
  const char* fsName = "SPIFFS";
  FS* fileSystem = &SPIFFS;
  SPIFFSConfig fileSystemConfig = SPIFFSConfig();
#elif defined USE_LITTLEFS
  #include <LittleFS.h>
  const char* fsName = "LittleFS";
  FS* fileSystem = &LittleFS;
  LitteFSConfig fileSystemConfig = LittleFSConfig();
#elif defined USE_SDFS
  #include <SDFS.h>
  const char* fsName = "SDFS";
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

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "webfilemanager";

ESP8266WebServer server(80);

static bool fsOK;
File uploadFile;


////////////////////////////////
// Utils to return HTTP codes and determine content-type

void returnOK() {
  server.send(200, "text/plain", "");
}

void returnNotFound(String msg) {
  server.send(404, "text/plain", msg);
}

void returnFail(String msg) {
  DBG_OUTPUT_PORT.println(msg);
  server.send(500, "text/plain", msg + "\r\n");
}

String getContentType(String filename) {
  if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".jpeg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}


////////////////////////////////
// Request handlers

/* 
 * Return the FS type, status and size info 
 */
void handleStatus() {
  FSInfo fs_info;
  String json = String("{\"type\":\"") + fsName + "\", \"isOk\":";

  if (fsOK) {
    fileSystem->info(fs_info);
    json += String("\"true\", \"totalBytes\":\"") + fs_info.totalBytes + "\", \"usedBytes\":\""  + fs_info.usedBytes + "\"";
  }
  else {
    json += "\"false\"";
  }
  json += "}";
  server.send(200, "application/json", json);
}


/* 
 * Return the list of files in the directory specified by the "dir" query string parameter 
 */
void handleFileList() {
  if (!fsOK) {
    return returnFail("FS INIT ERROR");
  }

  if (!server.hasArg("dir")) {
    return returnFail("BAD ARGS");
  }

  String path = server.arg("dir");
  if (path != "/" && !fileSystem->exists(path)) {
    return returnFail("BAD PATH");
  }

  /* TODO should we still perform a test equivalent to this one 
  File dir = fileSystem->open(path, "r");
  path = String();
  if (!dir.isDirectory()) {
    dir.close();
    return returnFail("NOT DIR");
  }
   */
   
  DBG_OUTPUT_PORT.println("handleFileList: " + path);
  Dir dir = fileSystem->openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") {
      output += ',';
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
    
    if (entry.name()[0] == '/') {
      output += &(entry.name()[1]);
    } 
    else {
      output += entry.name();
    }
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}


/*
 * Read the given file from the filesystem and stream it back to the client 
 */
bool handleFileRead(String path) {
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.htm";
  }
  
  String contentType;
  if (server.hasArg("download")) {
    contentType = "application/octet-stream";
  } else {
    contentType = getContentType(path);
  }
  
  String pathWithGz = path + ".gz";
  if (fileSystem->exists(pathWithGz) || fileSystem->exists(path)) {
    if (fileSystem->exists(pathWithGz)) {
      path += ".gz";
    }
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
 * Handle the creation of a new file 
 */
void handleFileCreate() {
  if (!fsOK) {
    return returnFail("FS INIT ERROR");
  }
  if (server.args() == 0) {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
  if (path == "/") {
    return returnFail("BAD PATH");
  }
  if (fileSystem->exists(path)) {
    return returnFail("FILE EXISTS");
  }

  if (path.endsWith("/")) {
    // Create a folder
    path.remove(path.length() - 1);
    fileSystem->mkdir(path);
  } 
  else {
    // Create a file
    File file = fileSystem->open(path, "w");
    if (file) {
      file.write((const char *)0);
      file.close();
    }
  }
  return returnOK();
}


/*
 * Delete the file or folder designed by the given path.
 * If it's a file, delete it.
 * If it's a folder, delete all nested contents first then the folder itself
 */
void deleteRecursive(String path) {
  File file = fileSystem->open(path, "r");
  bool isDir = file.isDirectory();
  file.close();
  
  // If it's a plain file, delete it
  if (!isDir) {
    fileSystem->remove(path);
    return;
  }
  
  // Otherwise delete its contents first
  Dir dir = fileSystem->openDir(path);

  while (dir.next()) {
    String entryPath = path + "/" + dir.fileName();
    deleteRecursive(entryPath);
  }

  // Then delete the folder itself
  fileSystem->rmdir(path);
}


/*
 * Handle a file deletion request
 */
void handleFileDelete() {
  if (!fsOK) {
    return returnFail("FS INIT ERROR");
  }
  if (server.args() == 0) {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
  if (path == "/" || !fileSystem->exists(path)) {
    return returnFail("BAD PATH");
  }
  deleteRecursive(path);
  returnOK();
}

/*
 * Handle a file upload request
 */
void handleFileUpload() {
  if (!fsOK) {
    return returnFail("FS INIT ERROR");
  }
  if (server.uri() != "/edit") {
    return;
  }
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    DBG_OUTPUT_PORT.println("handleFileUpload Name: " + filename);
    uploadFile = fileSystem->open(filename, "w");
    DBG_OUTPUT_PORT.print("Upload: START, filename: " + filename);
  } 
  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
    DBG_OUTPUT_PORT.print("Upload: WRITE, Bytes: " + upload.currentSize);
  } 
  else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
    DBG_OUTPUT_PORT.print("Upload: END, Size: " + upload.totalSize);
  }
}


/* 
 * The "Not Found" handler catches all URI not explicitely declared in code
 * First try to find and return the requested file from the filesystem,
 * and if it fails, return a 404 page with details
 */
void handleNotFound() {
  if (!fsOK) {
    return returnFail("FS INIT ERROR");
  }
  if (!handleFileRead(server.uri())) {
    String message = "Error: File not found\n\n";
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
    DBG_OUTPUT_PORT.print(message);
    return returnNotFound(message);
  }
}


void setup(void) {
  ////////////////////////////////
  // SERIAL INIT
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.setDebugOutput(true);
  DBG_OUTPUT_PORT.print("\n");

  ////////////////////////////////
  // FILESYSTEM INIT

  fileSystemConfig.setAutoFormat(false);
  fileSystem->setConfig(fileSystemConfig);
  fsOK = fileSystem->begin();
  DBG_OUTPUT_PORT.println(fsOK ? "Filesystem initialized." : "Filesystem init failed!");

  {
    // Debug: dump contents of root folder on console
    Dir dir = fileSystem->openDir("/");
    while (dir.next()) DBG_OUTPUT_PORT.println("FS File: " + dir.fileName() + ", size: " + dir.fileSize());
    DBG_OUTPUT_PORT.printf("\n");
  }

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
  DBG_OUTPUT_PORT.print("Connected! IP address: ");
  DBG_OUTPUT_PORT.println(WiFi.localIP());

  ////////////////////////////////
  // MDNS INIT
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    DBG_OUTPUT_PORT.print("Open http://");
    DBG_OUTPUT_PORT.print(host);
    DBG_OUTPUT_PORT.println(".local/edit to open the file manager");
  }

  ////////////////////////////////
  // WEB SERVER INIT

  // Filesystem status
  server.on("/status",HTTP_GET, handleStatus);
  
  // List directory
  server.on("/list",  HTTP_GET, handleFileList);
  
  // Load editor
  server.on("/edit",  HTTP_GET, []() {
    if (!handleFileRead("/edit/index.htm")) returnNotFound("FileNotFound");
  });
  
  // Create file
  server.on("/edit",  HTTP_PUT, handleFileCreate);
  
  // Delete file
  server.on("/edit",  HTTP_DELETE, handleFileDelete);
  
  // Upload file
  // - first callback is called after the request has ended with all parsed arguments
  // - second callback handles file upload at that location
  server.on("/edit",  HTTP_POST, /*[]() {
    returnOK();
  }*/returnOK, handleFileUpload);

  // Default handler for all URIs not defined above
  // Use it to read files from filesystem
  server.onNotFound(handleNotFound);

  // Start server
  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");
}


void loop(void) {
  server.handleClient();
  MDNS.update();
}
