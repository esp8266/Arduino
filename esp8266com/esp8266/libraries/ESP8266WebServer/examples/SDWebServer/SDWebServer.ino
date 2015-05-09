/* 
  SDWebServer - Example WebServer with SD Card backend for esp8266

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

  Have a FAT Formatted SD Card connected to the SPI port of the ESP8266
  The web root is the SD Card root folder
  File extensions with more than 3 charecters are not supported by the SD Library
  File Names longer than 8 charecters will be truncated by the SD library, so keep filenames shorter
  index.htm is the default index (works on subfolders as well)
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <SD.h>

//do not go larger than 1460 bytes as that is the maximum that could fit in a packet
#define WWW_BUF_SIZE 1460

const char* ssid = "**********";
const char* password = "**********";
const char* hostname = "esp8266sd";

MDNSResponder mdns;
ESP8266WebServer server(80);

static bool hasSD = false;
File uploadFile;

void handleFileUpload(){
  if(server.uri() != "/upload") return;
  HTTPUpload upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    Serial.print("Upload: START, filename:");
    Serial.println(upload.filename);
    if(SD.exists((char *)upload.filename.c_str())) SD.remove((char *)upload.filename.c_str());
    uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE);
  } else if(upload.status == UPLOAD_FILE_WRITE){
    Serial.print("Upload: WRITE, Bytes:");
    Serial.println(upload.buflen);
    if(uploadFile) uploadFile.write(upload.buf, upload.buflen);
  } else if(upload.status == UPLOAD_FILE_END){
    Serial.print("Upload: END, Size:");
    Serial.println(upload.size);
    if(uploadFile) uploadFile.close();
  }
}

bool loadFromSdCard(String path){
  String dataType = "text/plain";
  //handle default index
  if(path.endsWith("/")) path += "index.htm";
  
  //set proper Content-Type for the most common extensions
  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  
  //Try to open the file
  File dataFile = SD.open(path.c_str());
  
  //if it's a folder, try to open the default index
  if(dataFile && dataFile.isDirectory()){
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }
  
  //and finally if the file exists, stream the content to the client
  if (dataFile) {
    WiFiClient client = server.client();
    //send the file headers
    String head = "HTTP/1.1 200 OK\r\nContent-Type: ";
    head += dataType;
    head += "\r\nContent-Length: ";
    head += dataFile.size();
    head += "\r\n\r\n";
    client.print(head);
    
    //partition the data packets to fit in a TCP packet (1460 bytes MAX)
    uint8_t obuf[WWW_BUF_SIZE];
    while (dataFile.available() > WWW_BUF_SIZE){
      dataFile.read(obuf, WWW_BUF_SIZE);
      if(client.write(obuf, WWW_BUF_SIZE) != WWW_BUF_SIZE){
        Serial.println("Sent less data than expected!");
        dataFile.close();
        return true;
      }
    }
    //stream the last data left (size is at most WWW_BUF_SIZE bytes)
    uint16_t leftLen = dataFile.available();
    dataFile.read(obuf, leftLen);
    if(client.write(obuf, leftLen) != leftLen){
      Serial.println("Sent less data than expected!");
      dataFile.close();
      return true;
    }
    
    dataFile.close();
    return true;
  }
  return false;
}

void tryLoadFromSdCard(){
  String message = "FileNotFound\n\n";
  if(hasSD){
    //try to load the URL from SD Card
    if(loadFromSdCard(server.uri())) return;
  } else {
    message = "SDCARD Not Detected\n\n";
  }
  server.send(404, "text/plain", message);
}
 
void setup(void){
  uint8_t i = 0;
  Serial.begin(115200);
  
  //setup WiFi
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  //wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  
  //check if we have connected?
  if(i == 21){
    Serial.print("Could not connect to");
    Serial.println(ssid);
    //stop execution and wait forever
    while(1) delay(500);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  
  //start mDNS Server
  if (mdns.begin(hostname, WiFi.localIP())) {
    Serial.println("MDNS responder started");
    Serial.print("You can now connect to http://");
    Serial.print(hostname);
    Serial.println(".local");
  }
  
  //Attach handler
  server.onNotFound(tryLoadFromSdCard);
  
  //Attach Upload handler
  server.onFileUpload(handleFileUpload);
  
  //Attach handler for the Upload location
  server.on("/upload", HTTP_POST, [](){
    WiFiClient client = server.client();
    String message = "HTTP/1.1 200 OK\r\n";
    message += "Content-Type: text/plain\r\n";
    message += "Access-Control-Allow-Origin: *\r\n";
    message += "\r\n";
    client.print(message);
  });
  
  //start server
  server.begin();
  Serial.println("HTTP server started");
  
  //init SD Card
  if (SD.begin(SS)){
     Serial.println("SD Card initialized.");
     hasSD = true;
  }
}
 
void loop(void){
  mdns.update();
  server.handleClient();
} 
