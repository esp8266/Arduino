/* 
  ESP8266WebServer.cpp - Dead simple web-server.
  Supports only one simultaneous client, knows how to handle GET and POST.

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 
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
  Modified 8 May 2015 by Hristo Gochkov (proper post and file upload handling)
*/


#include <Arduino.h>
#include "WiFiServer.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"

//#define DEBUG
#define DEBUG_OUTPUT Serial1

struct ESP8266WebServer::RequestHandler {
  RequestHandler(ESP8266WebServer::THandlerFunction fn, const char* uri, HTTPMethod method)
  : fn(fn)
  , uri(uri)
  , method(method)
  , next(NULL)
  {
  }

  ESP8266WebServer::THandlerFunction fn;
  String uri;
  HTTPMethod method;
  RequestHandler* next;

};

ESP8266WebServer::ESP8266WebServer(int port)
: _server(port)
, _firstHandler(0)
, _lastHandler(0)
, _currentArgCount(0)
, _currentArgs(0)
{
}

ESP8266WebServer::~ESP8266WebServer()
{
  if (!_firstHandler)
    return;
  RequestHandler* handler = _firstHandler;
  while (handler) {
    RequestHandler* next = handler->next;
    delete handler;
    handler = next;
  }
}

void ESP8266WebServer::begin() {
  _server.begin();
}


void ESP8266WebServer::on(const char* uri, ESP8266WebServer::THandlerFunction handler)
{
  on(uri, HTTP_ANY, handler);
}

void ESP8266WebServer::on(const char* uri, HTTPMethod method, ESP8266WebServer::THandlerFunction fn)
{
  RequestHandler* handler = new RequestHandler(fn, uri, method);
  if (!_lastHandler) {
    _firstHandler = handler;
    _lastHandler = handler;
  }
  else {
    _lastHandler->next = handler;
    _lastHandler = handler;
  }
}

void ESP8266WebServer::handleClient()
{
  WiFiClient client = _server.available();
  if (!client) {
    return;
  }

#ifdef DEBUG
  DEBUG_OUTPUT.println("New client");
#endif
  // Wait for data from client to become available
  while(client.connected() && !client.available()){
    delay(1);
  }

  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');
  client.readStringUntil('\n');

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
#ifdef DEBUG
    DEBUG_OUTPUT.print("Invalid request: ");
    DEBUG_OUTPUT.println(req);
#endif
    return;
  }
  
  String methodStr = req.substring(0, addr_start);
  String url = req.substring(addr_start + 1, addr_end);
  String searchStr = "";
  int hasSearch = url.indexOf('?');
  if(hasSearch != -1){
    searchStr = url.substring(hasSearch + 1);
    url = url.substring(0, hasSearch);
  }
  _currentUri      = url;
  
  HTTPMethod method = HTTP_GET;
  if (methodStr == "POST") {
    method = HTTP_POST;
  } else if (methodStr == "DELETE") {
    method = HTTP_DELETE;
  } else if (methodStr == "PUT") {
    method = HTTP_PUT;
  } else if (methodStr == "PATCH") {
    method = HTTP_PATCH;
  }
  
#ifdef DEBUG
  DEBUG_OUTPUT.print("method: ");
  DEBUG_OUTPUT.print(methodStr);
  DEBUG_OUTPUT.print(" url: ");
  DEBUG_OUTPUT.print(url);
  DEBUG_OUTPUT.print(" search: ");
  DEBUG_OUTPUT.println(searchStr);
#endif

  String formData;
  //bellow is needed only when POST type request
  if(method == HTTP_POST || method == HTTP_PUT || method == HTTP_PATCH){
    String boundaryStr;
    String headerName;
    String headerValue;
    bool isForm = false;
    uint32_t contentLength = 0;
    //parse headers
    while(1){
      req = client.readStringUntil('\r');
      client.readStringUntil('\n');
      if(req == "") break;//no moar headers
      int headerDiv = req.indexOf(':');
      if(headerDiv == -1){
        break;
      }
      headerName = req.substring(0, headerDiv);
      headerValue = req.substring(headerDiv + 2);
      if(headerName == "Content-Type"){
        if(headerValue.startsWith("text/plain")){
          isForm = false;
        } else if(headerValue.startsWith("multipart/form-data")){
          boundaryStr = headerValue.substring(headerValue.indexOf('=')+1);
          isForm = true;
        }
      } else if(headerName == "Content-Length"){
        contentLength = headerValue.toInt();
      }
    }
  
    if(!isForm){
      if(searchStr != "") searchStr += '&';
      searchStr += client.readStringUntil('\r');
      client.readStringUntil('\n');
    }
    _parseArguments(searchStr);
    if(isForm){
      _parseForm(client, boundaryStr, contentLength);
    }
  } else {
    _parseArguments(searchStr);
  }
  client.flush();

#ifdef DEBUG
  DEBUG_OUTPUT.print("Request: ");
  DEBUG_OUTPUT.println(url);
  DEBUG_OUTPUT.print(" Arguments: ");
  DEBUG_OUTPUT.println(searchStr);
#endif

  _handleRequest(client, url, method);
}

void ESP8266WebServer::send(int code, const char* content_type, String content) {
  String response = "HTTP/1.1 ";
  response += String(code);
  response += " ";
  response += _responseCodeToString(code);
  response += "\r\n";

  if (!content_type)
    content_type = "text/html";
  _appendHeader(response, "Content-Type", content_type);
  
  response += "\r\n";
  response += content;
  _currentClient.print(response);
}

String ESP8266WebServer::arg(const char* name) {
  for (int i = 0; i < _currentArgCount; ++i) {
    if (_currentArgs[i].key == name)
      return _currentArgs[i].value;
  }
  return String();
}

String ESP8266WebServer::arg(int i) {
  if (i < _currentArgCount)
    return _currentArgs[i].value;
  return String();
}

String ESP8266WebServer::argName(int i) {
  if (i < _currentArgCount)
    return _currentArgs[i].key;
  return String();
}

int ESP8266WebServer::args() {
  return _currentArgCount;
}

bool ESP8266WebServer::hasArg(const char* name) {
  for (int i = 0; i < _currentArgCount; ++i) {
    if (_currentArgs[i].key == name)
      return true;
  }
  return false;
}

void ESP8266WebServer::_parseArguments(String data) {
#ifdef DEBUG
  DEBUG_OUTPUT.print("args: ");
  DEBUG_OUTPUT.println(data);
#endif
  if (_currentArgs)
    delete[] _currentArgs;
  _currentArgs = 0;
  if (data.length() == 0) {
    _currentArgCount = 0;
    return;
  }
  _currentArgCount = 1;

  for (int i = 0; i < data.length(); ) {
    i = data.indexOf('&', i);
    if (i == -1)
      break;
    ++i;
    ++_currentArgCount;
  }
#ifdef DEBUG
  DEBUG_OUTPUT.print("args count: ");
  DEBUG_OUTPUT.println(_currentArgCount);
#endif

  _currentArgs = new RequestArgument[_currentArgCount];
  int pos = 0;
  int iarg;
  for (iarg = 0; iarg < _currentArgCount;) {
    int equal_sign_index = data.indexOf('=', pos);
    int next_arg_index = data.indexOf('&', pos);
#ifdef DEBUG
    DEBUG_OUTPUT.print("pos ");
    DEBUG_OUTPUT.print(pos);
    DEBUG_OUTPUT.print("=@ ");
    DEBUG_OUTPUT.print(equal_sign_index);
    DEBUG_OUTPUT.print(" &@ ");
    DEBUG_OUTPUT.println(next_arg_index);
#endif
    if ((equal_sign_index == -1) || ((equal_sign_index > next_arg_index) && (next_arg_index != -1))) {
#ifdef DEBUG
      DEBUG_OUTPUT.print("arg missing value: ");
      DEBUG_OUTPUT.println(iarg);
#endif
      if (next_arg_index == -1)
        break;
      pos = next_arg_index + 1;
      continue;
    }
    RequestArgument& arg = _currentArgs[iarg];
    arg.key = data.substring(pos, equal_sign_index);
    arg.value = data.substring(equal_sign_index + 1, next_arg_index);
#ifdef DEBUG
    DEBUG_OUTPUT.print("arg ");
    DEBUG_OUTPUT.print(iarg);
    DEBUG_OUTPUT.print(" key: ");
    DEBUG_OUTPUT.print(arg.key);
    DEBUG_OUTPUT.print(" value: ");
    DEBUG_OUTPUT.println(arg.value);
#endif
    ++iarg;
    if (next_arg_index == -1)
      break;
    pos = next_arg_index + 1;
  }
  _currentArgCount = iarg;
#ifdef DEBUG
  DEBUG_OUTPUT.print("args count: ");
  DEBUG_OUTPUT.println(_currentArgCount);
#endif

}

void ESP8266WebServer::_parseForm(WiFiClient& client, String boundary, uint32_t len){
  
#ifdef DEBUG
  DEBUG_OUTPUT.print("Parse Form: Boundary: ");
  DEBUG_OUTPUT.print(boundary);
  DEBUG_OUTPUT.print("Length: ");
  DEBUG_OUTPUT.println(len);
#endif
  String line;
  line = client.readStringUntil('\r');
  client.readStringUntil('\n');
  //start reading the form
  if(line == ("--"+boundary)){
    RequestArgument* postArgs = new RequestArgument[32];
    int postArgsLen = 0;
    while(1){
      String argName;
      String argValue;
      String argType;
      String argFilename;
      bool argIsFile = false;
      
      line = client.readStringUntil('\r');
      client.readStringUntil('\n');
      if(line.startsWith("Content-Disposition")){
        int nameStart = line.indexOf('=');
        if(nameStart != -1){
          argName = line.substring(nameStart+2);
          nameStart = argName.indexOf('=');
          if(nameStart == -1){
            argName = argName.substring(0, argName.length() - 1);
          } else {
            argFilename = argName.substring(nameStart+2, argName.length() - 1);
            argName = argName.substring(0, argName.indexOf('"'));
            argIsFile = true;
  #ifdef DEBUG
            DEBUG_OUTPUT.print("PostArg FileName: ");
            DEBUG_OUTPUT.println(argFilename);
  #endif
            //use GET to set the filename if uploading using blob
            if(argFilename == "blob" && hasArg("filename")) argFilename = arg("filename");
          }
  #ifdef DEBUG
          DEBUG_OUTPUT.print("PostArg Name: ");
          DEBUG_OUTPUT.println(argName);
  #endif
          argType = "text/plain";
          line = client.readStringUntil('\r');
          client.readStringUntil('\n');
          if(line.startsWith("Content-Type")){
            argType = line.substring(line.indexOf(':')+2);
            //skip next line
            client.readStringUntil('\r');
            client.readStringUntil('\n');
          }
  #ifdef DEBUG
          DEBUG_OUTPUT.print("PostArg Type: ");
          DEBUG_OUTPUT.println(argType);
  #endif
          if(!argIsFile){
            while(1){
              line = client.readStringUntil('\r');
              client.readStringUntil('\n');
              if(line.startsWith("--"+boundary)) break;
              argValue += line+"\n";
            }
  #ifdef DEBUG
            DEBUG_OUTPUT.print("PostArg Value: ");
            DEBUG_OUTPUT.println(argValue);
            DEBUG_OUTPUT.println();
  #endif
            
            RequestArgument& arg = postArgs[postArgsLen++];
            arg.key = argName;
            arg.value = argValue;
            
            if(line == ("--"+boundary+"--")){
  #ifdef DEBUG
              DEBUG_OUTPUT.println("Done Parsing POST");
  #endif
              break;
            }
          } else {
            _currentUpload.status = UPLOAD_FILE_START;
            _currentUpload.name = argName;
            _currentUpload.filename = argFilename;
            _currentUpload.type = argType;
            _currentUpload.size = 0;
            _currentUpload.buflen = 0;
#ifdef DEBUG
            DEBUG_OUTPUT.print("Start File: ");
            DEBUG_OUTPUT.print(_currentUpload.filename);
            DEBUG_OUTPUT.print(" Type: ");
            DEBUG_OUTPUT.println(_currentUpload.type);
#endif
            if(_fileUploadHandler) _fileUploadHandler();
            _currentUpload.status = UPLOAD_FILE_WRITE;
            uint8_t argByte = client.read();
readfile:
            while(argByte != 0x0D){
              _currentUpload.buf[_currentUpload.buflen++] = argByte;
              if(_currentUpload.buflen == 1460){
  #ifdef DEBUG
                DEBUG_OUTPUT.println("Write File: 1460");
  #endif
                if(_fileUploadHandler) _fileUploadHandler();
                _currentUpload.size += _currentUpload.buflen;
                _currentUpload.buflen = 0;
              }
              argByte = client.read();
            }
            
            argByte = client.read();
            if(argByte == 0x0A){
              line = client.readStringUntil(0x0D);
              client.readStringUntil(0x0A);
#ifdef DEBUG
              DEBUG_OUTPUT.print("Write File: ");
              DEBUG_OUTPUT.println(_currentUpload.buflen);
#endif
              if(_fileUploadHandler) _fileUploadHandler();
              _currentUpload.size += _currentUpload.buflen;
              _currentUpload.buflen = 0;
              if(line.startsWith("--"+boundary)){
                _currentUpload.status = UPLOAD_FILE_END;
#ifdef DEBUG
                DEBUG_OUTPUT.print("End File: ");
                DEBUG_OUTPUT.print(_currentUpload.filename);
                DEBUG_OUTPUT.print(" Type: ");
                DEBUG_OUTPUT.print(_currentUpload.type);
                DEBUG_OUTPUT.print(" Size: ");
                DEBUG_OUTPUT.println(_currentUpload.size);
#endif
                if(_fileUploadHandler) _fileUploadHandler();
                if(line == ("--"+boundary+"--")){
#ifdef DEBUG
                  DEBUG_OUTPUT.println("Done Parsing POST");
#endif
                  break;
                }
                continue;
              } else {
                _currentUpload.buf[_currentUpload.buflen++] = 0x0D;
                _currentUpload.buf[_currentUpload.buflen++] = 0x0A;
                const char * lineChars = line.c_str();
                uint32_t i = 0;
                while(i < os_strlen(lineChars)){
                  _currentUpload.buf[_currentUpload.buflen++] = lineChars[i++];
                  if(_currentUpload.buflen == 1460){
#ifdef DEBUG
                    DEBUG_OUTPUT.println("Write File: 1460");
#endif
                    if(_fileUploadHandler) _fileUploadHandler();
                    _currentUpload.size += _currentUpload.buflen;
                    _currentUpload.buflen = 0;
                  }
                }
                argByte = client.read();
                goto readfile;
              }
            } else {
              _currentUpload.buf[_currentUpload.buflen++] = 0x0D;
              if(_currentUpload.buflen == 1460){
  #ifdef DEBUG
                DEBUG_OUTPUT.println("Write File: 1460");
  #endif
                if(_fileUploadHandler) _fileUploadHandler();
                _currentUpload.size += _currentUpload.buflen;
                _currentUpload.buflen = 0;
              }
              goto readfile;
            }
            break;
          }
        }
      }
    }
    
    int iarg;
    int totalArgs = ((32 - postArgsLen) < _currentArgCount)?(32 - postArgsLen):_currentArgCount;
    for (iarg = 0; iarg < totalArgs; iarg++){
      RequestArgument& arg = postArgs[postArgsLen++];
      arg.key = _currentArgs[iarg].key;
      arg.value = _currentArgs[iarg].value;
    }
    if (_currentArgs) delete[] _currentArgs;
    _currentArgs = new RequestArgument[postArgsLen];
    for (iarg = 0; iarg < postArgsLen; iarg++){
      RequestArgument& arg = _currentArgs[iarg];
      arg.key = postArgs[iarg].key;
      arg.value = postArgs[iarg].value;
    }
    _currentArgCount = iarg;
    if (postArgs) delete[] postArgs;
  }
}

void ESP8266WebServer::onFileUpload(THandlerFunction fn) {
  _fileUploadHandler = fn;
}

void ESP8266WebServer::onNotFound(THandlerFunction fn) {
  _notFoundHandler = fn;
}

void ESP8266WebServer::_handleRequest(WiFiClient& client, String uri, HTTPMethod method) {
  _currentClient   = client;
  _currentUri      = uri;
  _currentMethod   = method;

  RequestHandler* handler;
  for (handler = _firstHandler; handler; handler = handler->next)
  {
    if (handler->method != HTTP_ANY && handler->method != method)
      continue;

    if (handler->uri != uri)
      continue;

    handler->fn();
    break;
  }

  if (!handler){
#ifdef DEBUG
    DEBUG_OUTPUT.println("request handler not found");
#endif

    if(_notFoundHandler) {
      _notFoundHandler();
    }
    else {
      send(404, "text/plain", String("Not found: ") + uri);
    }
  }

  _currentClient   = WiFiClient();
  _currentUri      = String();
  
}

const char* ESP8266WebServer::_responseCodeToString(int code) {
  switch (code) {
    case 200: return "OK";
    case 404: return "Not found";
    default:  return "";
  }
}

void ESP8266WebServer::_appendHeader(String& response, const char* name, const char* value) {
  response += name;
  response += ": ";
  response += value;
  response += "\r\n";
}
