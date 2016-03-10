#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "ESP8266HTTPUpdateServer.h"


const char* ESP8266HTTPUpdateServer::_serverIndex =
R"(<html><body><form method='POST' action='/update' enctype='multipart/form-data'>
                  <input type='file' name='update'>
                  <input type='submit' value='Update'>
               </form>
         </body></html>)";

ESP8266HTTPUpdateServer::ESP8266HTTPUpdateServer(bool serial_debug)
{
  _serial_output = serial_debug;
  _server = NULL;
}

void ESP8266HTTPUpdateServer::setup(ESP8266WebServer *server)
{
    _server = server;

    // handler for the /update form page
    _server->on("/update", HTTP_GET, [&](){
      _server->sendHeader("Connection", "close");
      _server->sendHeader("Access-Control-Allow-Origin", "*");
      _server->send(200, "text/html", _serverIndex);
    });

    // handler for the /update form POST (once file upload finishes)
    _server->on("/update", HTTP_POST, [&](){
      _server->sendHeader("Connection", "close");
      _server->sendHeader("Access-Control-Allow-Origin", "*");
      _server->send(200, "text/html", (Update.hasError())?"FAIL":"<META http-equiv=\"refresh\" content=\"15;URL=/update\">OK");
      ESP.restart();
    },[&](){
      // handler for the file upload, get's the sketch bytes, and writes
      // them through the Update object
      HTTPUpload& upload = _server->upload();
      if(upload.status == UPLOAD_FILE_START){
        if (_serial_output)
          Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        if (_serial_output)
          Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          if (_serial_output) Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if (_serial_output) Serial.printf(".");
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          if (_serial_output) Update.printError(Serial);

        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          if (_serial_output) Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          if (_serial_output) Update.printError(Serial);
        }
        if (_serial_output) Serial.setDebugOutput(false);
      } else if(upload.status == UPLOAD_FILE_ABORTED){
        Update.end();
        if (_serial_output) Serial.println("Update was aborted");
      }
      delay(0);
    });
}
