/* 
  FSWebServer - Example WebServer with SPIFFS backend for esp8266

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

  upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
  !!! This will erase any previous SPIFFS data you may have!
  !!! Make Backups First !!!
  if you do not have it, get it at : <<<FILL THIS
  access the sample web page at http://esp8266fs.local
  edit the page by going to http://esp8266fs.local/edit

*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FileSystem.h>

#define DBG_OUTPUT_PORT Serial

const char* ssid = "**********";
const char* password = "**********";
const char* hostname = "esp8266fs";

MDNSResponder mdns;
ESP8266WebServer server(80);
//holds the current upload
FSFile fsUploadFile;

//format bytes
char *formatBytes(size_t bytes){
  if (bytes < 1024){
    return (char *)String(String(bytes)+"B").c_str();
  } else if(bytes < (1024 * 1024)){
    return (char *)String(String(bytes/1024.0)+"KB").c_str();
  } else if(bytes < (1024 * 1024 * 1024)){
    return (char *)String(String(bytes/1024.0/1024.0)+"MB").c_str();
  } else {
    return (char *)String(String(bytes/1024.0/1024.0/1024.0)+"GB").c_str();
  }
}

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  if(FS.exists((char *)(path+".gz").c_str()) || FS.exists((char *)path.c_str())){
    if(FS.exists((char *)(path+".gz").c_str()))
      path += ".gz";
    FSFile file = FS.open((char *)path.c_str());
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpdate(){
  if(server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    DBG_OUTPUT_PORT.print("Upload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = FS.open((char *)filename.c_str(), FSFILE_OVERWRITE);
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //DBG_OUTPUT_PORT.print("Upload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    DBG_OUTPUT_PORT.print("Upload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void handleFileDelete(){
  if(server.args() == 0) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }
  String path = server.arg(0);
  if(path == "/") {
    server.send(500, "text/plain", "BAD PATH");
    return;
  }
  if(!FS.exists((char *)(path.c_str()))) {
    server.send(404, "text/plain", "FileNotFound");
    return;
  }
  FS.remove((char *)path.c_str());
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate(){
  if(server.args() == 0) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }
  String path = server.arg(0);
  if(path == "/") {
    server.send(500, "text/plain", "BAD PATH");
    return;
  }
  if(FS.exists((char *)path.c_str())) {
    server.send(500, "text/plain", "FILE EXISTS");
    return;
  }
  FSFile file = FS.open((char *)path.c_str(), FSFILE_OVERWRITE);
  if(file)
    file.close();
  else {
    server.send(500, "text/plain", "CREATE FAILED");
    return;
  }
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList() {
  if(!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }
  String path = server.arg("dir");
  
  FSFile entry;
  FSFile dir = FS.open((char *)path.c_str());
  path = String();
  if(!dir.isDirectory()){
    dir.close();
    server.send(500, "text/plain", "NOT DIR");
    return;
  }
  dir.rewindDirectory();

  String output = "[";
  while(true){
    entry = dir.openNextFile();
    if (!entry) 
      break;

    if(!FS.exists(entry.name())){
      os_printf("Entry[%s] Not Exists!\n", entry.name());
      entry.remove();
      entry.close();
      continue;
    }
    
    if(output != "[") 
      output += ',';
    output += "{\"type\":\"";
    output += (entry.isDirectory())?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  dir.close();
  
  output += "]";
  server.send(200, "text/json", output);
}

void setup(void){
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.print("\n");
  
  //SPIFFS INIT
  DBG_OUTPUT_PORT.printf("\n==== SPIFFS Info ====\n");
  DBG_OUTPUT_PORT.printf("FS Mount: %d\n", FS.mount());
  DBG_OUTPUT_PORT.printf("FS Size: %s\n", formatBytes(FS.size()));
  DBG_OUTPUT_PORT.printf("FS Bytes: total: %s, used: %s\n", formatBytes(FS.totalBytes()), formatBytes(FS.usedBytes()));
  DBG_OUTPUT_PORT.printf("FS Blocks: total: %d, free: %d, size: %s\n", FS.totalBlocks(), FS.freeBlocks(), formatBytes(FS.blockSize()));
  DBG_OUTPUT_PORT.printf("FS Pages: allocated: %d, deleted: %d, size: %s\n", FS.allocatedPages(), FS.deletedPages(), formatBytes(FS.pageSize()));
  FSFile entry;
  FSFile dir = FS.open((char *)"/");
  while(true){
    entry = dir.openNextFile();
    if (!entry) break;
    DBG_OUTPUT_PORT.printf("FS File: %s, type: %s, size: %s\n", entry.name(), (entry.isDirectory())?"dir":"file", formatBytes(entry.size()));
    entry.close();
  }
  dir.close();
  DBG_OUTPUT_PORT.printf("\n");

  //WIFI INIT
  DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  if(WiFi.waitForConnectResult() != WL_CONNECTED){
    DBG_OUTPUT_PORT.printf("Could not connect to %s\n", ssid);
    while(1) delay(500);
  }
  DBG_OUTPUT_PORT.print("Connected! IP address: ");
  DBG_OUTPUT_PORT.println(WiFi.localIP());
  
  //mDNS INIT
  if (mdns.begin(hostname, WiFi.localIP()))
    DBG_OUTPUT_PORT.printf("mDNS responder started for %s.local\n", hostname);

  //SERVER INIT
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, [](){
    if(!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //called after file upload
  server.on("/edit", HTTP_POST, [](){ server.send(200, "text/plain", ""); });
  //called when a file is received inside POST data
  server.onFileUpload(handleFileUpdate);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, [](){
    String json = "{";
    json += "\"heap\":"+String(ESP.getFreeHeap());
    json += ", \"analog\":"+String(analogRead(A0));
    json += ", \"gpio\":"+String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });
  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");

}
 
void loop(void){
  server.handleClient();
}
