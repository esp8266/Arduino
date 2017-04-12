#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include "ESP8266HTTPUpdateServer.h"


const char* ESP8266HTTPUpdateServer::_serverIndex =

R"(<html><body><form method='POST' action='?cmd=0' enctype='multipart/form-data'>
		  <input type='hidden' name='cmd' value='0'>
                   <input type='file' name='update'>
                  <input type='submit' value='Update Flash'>
               </form>
	       <form method='POST' action='?cmd=100' enctype='multipart/form-data'> 
		  <input type='hidden' name='cmd' value='100'>
                  <input type='file' name='update'>
                  <input type='submit' value='Update Spiffs'>
                </form>
          </body></html>)";
	
const char* ESP8266HTTPUpdateServer::_failedResponse = R"(Update Failed!)";
const char* ESP8266HTTPUpdateServer::_successResponse = "<META http-equiv=\"refresh\" content=\"15;URL=\">Update Success! Rebooting...";
int _command;
Ticker restartTicker;

ESP8266HTTPUpdateServer::ESP8266HTTPUpdateServer(bool serial_debug)
{
  _serial_output = serial_debug;
  _server = NULL;
  _username = NULL;
  _password = NULL;
  _hash = NULL;
  _authenticated = false;
}

void ESP8266HTTPUpdateServer::setup(ESP8266WebServer *server, const char * path)
{
    _server = server;
    
	if (_hash == NULL && _username != NULL && _password != NULL) {
		_hash = _server->getUserPasswordHash(_username, _password);
	}
	
    // handler for the /update form page
    _server->on(path, HTTP_GET, [&](){
      if(_hash != NULL && !_server->authenticate(_hash))
        return _server->requestAuthentication();
      _server->send(200, "text/html", _serverIndex);
    });

    // handler for the /update form POST (once file upload finishes)
    _server->on(path, HTTP_POST, [&](){
      if(!_authenticated)
        return _server->requestAuthentication();
			
      _server->send(200, "text/html", Update.hasError() ? _failedResponse : _successResponse);
      restartTicker.once_ms(5000, [&](){ESP.restart()});
    },[&](){
      // handler for the file upload, get's the sketch bytes, and writes
      // them through the Update object
      HTTPUpload& upload = _server->upload();
      if(upload.status == UPLOAD_FILE_START){
        if (_serial_output)
          Serial.setDebugOutput(true);

        _authenticated = (_hash == NULL || _server->authenticate(_hash));
        if(!_authenticated){
          if (_serial_output)
            Serial.printf("Unauthenticated Update\n");
          return;
        }

        WiFiUDP::stopAll();
        if (_serial_output)
          Serial.printf("Update: %s\n", upload.filename.c_str());
        
		uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
		
		_command = _server->arg("cmd").toInt();
		
		if(!Update.begin(maxSketchSpace, _command)){//start with max available size
          if (_serial_output) Update.printError(Serial);
        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_WRITE){
        if (_serial_output) Serial.printf(".");
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          if (_serial_output) Update.printError(Serial);

        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          if (_serial_output) Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          if (_serial_output) Update.printError(Serial);
        }
        if (_serial_output) Serial.setDebugOutput(false);
      } else if(_authenticated && upload.status == UPLOAD_FILE_ABORTED){
        Update.end();
        if (_serial_output) Serial.println("Update was aborted");
      }
      delay(0);
    });
}

bool ESP8266HTTPUpdateServer::setLoginPassword(const char * username, const char * password) {
	
	if(_hash == NULL && _username == NULL && _password == NULL && password && username) {
		_username = username;
		_password = password;
		return true;
	}
	
	return false;
}
	
bool ESP8266HTTPUpdateServer::setLoginPasswordHASH(const char * hashUNP) {
	
	if(_hash == NULL && _username == NULL && _password == NULL && hashUNP) {
		_hash = hashUNP;
		return true;
	}
	
	return false;
}
