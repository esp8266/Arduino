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
#include "FS.h"
#include "detail/RequestHandler.h"
// #define DEBUG
#define DEBUG_OUTPUT Serial


ESP8266WebServer::ESP8266WebServer(int port)
: _server(port)
, _firstHandler(0)
, _lastHandler(0)
, _currentArgCount(0)
, _currentArgs(0)
{
}

ESP8266WebServer::~ESP8266WebServer() {
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

void ESP8266WebServer::on(const char* uri, ESP8266WebServer::THandlerFunction handler) {
  on(uri, HTTP_ANY, handler);
}

void ESP8266WebServer::on(const char* uri, HTTPMethod method, ESP8266WebServer::THandlerFunction fn) {
  _addRequestHandler(new FunctionRequestHandler(fn, uri, method));
}

void ESP8266WebServer::_addRequestHandler(RequestHandler* handler) {
    if (!_lastHandler) {
      _firstHandler = handler;
      _lastHandler = handler;
    }
    else {
      _lastHandler->next = handler;
      _lastHandler = handler;
    }
}

void ESP8266WebServer::serveStatic(const char* uri, FS& fs, const char* path) {
    _addRequestHandler(new StaticRequestHandler(fs, path, uri));
}

void ESP8266WebServer::handleClient() {
  WiFiClient client = _server.available();
  if (!client) {
    return;
  }

#ifdef DEBUG
  DEBUG_OUTPUT.println("New client");
#endif

  // Wait for data from client to become available
  uint16_t maxWait = HTTP_MAX_DATA_WAIT;
  while(client.connected() && !client.available() && maxWait--){
    delay(1);
  }

  if (!_parseRequest(client)) {
    return;
  }

  _currentClient = client;
  _contentLength = CONTENT_LENGTH_NOT_SET;
  _handleRequest();
}

void ESP8266WebServer::sendHeader(const String& name, const String& value, bool first) {
  String headerLine = name;
  headerLine += ": ";
  headerLine += value;
  headerLine += "\r\n";

  if (first) {
    _responseHeaders = headerLine + _responseHeaders;
  }
  else {
    _responseHeaders += headerLine;
  }
}


void ESP8266WebServer::_prepareHeader(String& response, int code, const char* content_type, size_t contentLength) {
    response = "HTTP/1.1 ";
    response += String(code);
    response += " ";
    response += _responseCodeToString(code);
    response += "\r\n";

    if (!content_type)
        content_type = "text/html";

    sendHeader("Content-Type", content_type, true);
    if (_contentLength != CONTENT_LENGTH_UNKNOWN && _contentLength != CONTENT_LENGTH_NOT_SET) {
        sendHeader("Content-Length", String(_contentLength).c_str());
    }
    else if (contentLength > 0){
        sendHeader("Content-Length", String(contentLength).c_str());
    }
    sendHeader("Connection", "close");
    sendHeader("Access-Control-Allow-Origin", "*");

    response += _responseHeaders;
    response += "\r\n";
    _responseHeaders = String();
}

void ESP8266WebServer::send(int code, const char* content_type, const String& content) {
    String header;
    _prepareHeader(header, code, content_type, content.length());
    sendContent(header);

    sendContent(content);
}

void ESP8266WebServer::send_P(int code, PGM_P content_type, PGM_P content) {
    size_t contentLength = 0;

    if (content != NULL) {
        contentLength = strlen_P(content);
    }

    String header;
    char type[64];
    memccpy_P((void*)type, (PGM_VOID_P)content_type, 0, sizeof(type));
    _prepareHeader(header, code, (const char* )type, contentLength);
    sendContent(header);
    sendContent_P(content);
}

void ESP8266WebServer::send_P(int code, PGM_P content_type, PGM_P content, size_t contentLength) {
    String header;
    char type[64];
    memccpy_P((void*)type, (PGM_VOID_P)content_type, 0, sizeof(type));
    _prepareHeader(header, code, (const char* )type, contentLength);
    sendContent(header);
    sendContent_P(content, contentLength);
}

void ESP8266WebServer::send(int code, char* content_type, const String& content) {
  send(code, (const char*)content_type, content);
}

void ESP8266WebServer::send(int code, const String& content_type, const String& content) {
  send(code, (const char*)content_type.c_str(), content);
}

void ESP8266WebServer::sendContent(const String& content) {
  const size_t unit_size = HTTP_DOWNLOAD_UNIT_SIZE;
  size_t size_to_send = content.length();
  const char* send_start = content.c_str();

  while (size_to_send) {
    size_t will_send = (size_to_send < unit_size) ? size_to_send : unit_size;
    size_t sent = _currentClient.write(send_start, will_send);
    if (sent == 0) {
      break;
    }
    size_to_send -= sent;
    send_start += sent;
  }
}

void ESP8266WebServer::sendContent_P(PGM_P content) {
    char contentUnit[HTTP_DOWNLOAD_UNIT_SIZE + 1];

    contentUnit[HTTP_DOWNLOAD_UNIT_SIZE] = '\0';

    while (content != NULL) {
        size_t contentUnitLen;
        PGM_P contentNext;

        // due to the memccpy signature, lots of casts are needed
        contentNext = (PGM_P)memccpy_P((void*)contentUnit, (PGM_VOID_P)content, 0, HTTP_DOWNLOAD_UNIT_SIZE);

        if (contentNext == NULL) {
            // no terminator, more data available
            content += HTTP_DOWNLOAD_UNIT_SIZE;
            contentUnitLen = HTTP_DOWNLOAD_UNIT_SIZE;
        }
        else {
            // reached terminator
            contentUnitLen = contentNext - content;
            content = NULL;
        }

        // write is so overloaded, had to use the cast to get it pick the right one
        _currentClient.write((const char*)contentUnit, contentUnitLen);
    }
}

void ESP8266WebServer::sendContent_P(PGM_P content, size_t size) {
    char contentUnit[HTTP_DOWNLOAD_UNIT_SIZE + 1];
    contentUnit[HTTP_DOWNLOAD_UNIT_SIZE] = '\0';
    size_t remaining_size = size;

    while (content != NULL && remaining_size > 0) {
        size_t contentUnitLen = HTTP_DOWNLOAD_UNIT_SIZE;

        if (remaining_size < HTTP_DOWNLOAD_UNIT_SIZE) contentUnitLen = remaining_size;
        // due to the memcpy signature, lots of casts are needed
        memcpy_P((void*)contentUnit, (PGM_VOID_P)content, contentUnitLen);

        content += contentUnitLen;
        remaining_size -= contentUnitLen;

        // write is so overloaded, had to use the cast to get it pick the right one
        _currentClient.write((const char*)contentUnit, contentUnitLen);
    }
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

String ESP8266WebServer::hostHeader() {
  return _hostHeader;
}

void ESP8266WebServer::onFileUpload(THandlerFunction fn) {
  _fileUploadHandler = fn;
}

void ESP8266WebServer::onNotFound(THandlerFunction fn) {
  _notFoundHandler = fn;
}

void ESP8266WebServer::_handleRequest() {
  RequestHandler* handler;
  for (handler = _firstHandler; handler; handler = handler->next) {
    if (handler->handle(*this, _currentMethod, _currentUri))
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
      send(404, "text/plain", String("Not found: ") + _currentUri);
    }
  }

  uint16_t maxWait = HTTP_MAX_CLOSE_WAIT;
  while(_currentClient.connected() && maxWait--) {
    delay(1);
  }
  _currentClient   = WiFiClient();
  _currentUri      = String();
}

const char* ESP8266WebServer::_responseCodeToString(int code) {
  switch (code) {
    case 101: return "Switching Protocols";
    case 200: return "OK";
    case 403: return "Forbidden";
    case 404: return "Not found";
    case 500: return "Fail";
    default:  return "";
  }
}
