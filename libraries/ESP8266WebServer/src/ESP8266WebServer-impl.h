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
#include <libb64/cencode.h>
#include "WiFiServer.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"
#include "FS.h"
#include "detail/RequestHandlersImpl.h"
#include <StreamDev.h>

//#define DEBUG_ESP_HTTP_SERVER
#ifdef DEBUG_ESP_PORT
#define DEBUG_OUTPUT DEBUG_ESP_PORT
#else
#define DEBUG_OUTPUT Serial
#endif

static const char AUTHORIZATION_HEADER[] PROGMEM = "Authorization";
static const char qop_auth[] PROGMEM = "qop=auth";
static const char qop_auth_quoted[] PROGMEM = "qop=\"auth\"";
static const char WWW_Authenticate[] PROGMEM = "WWW-Authenticate";
static const char Content_Length[] PROGMEM = "Content-Length";


template <typename ServerType>
ESP8266WebServerTemplate<ServerType>::ESP8266WebServerTemplate(IPAddress addr, int port)
: _server(addr, port)
, _currentMethod(HTTP_ANY)
, _currentVersion(0)
, _currentStatus(HC_NONE)
, _statusChange(0)
, _currentHandler(nullptr)
, _firstHandler(nullptr)
, _lastHandler(nullptr)
, _currentArgCount(0)
, _currentArgs(nullptr)
, _postArgsLen(0)
, _postArgs(nullptr)
, _headerKeysCount(0)
, _currentHeaders(nullptr)
, _contentLength(0)
, _chunked(false)
{
}

template <typename ServerType>
ESP8266WebServerTemplate<ServerType>::ESP8266WebServerTemplate(int port)
: _server(port)
, _currentMethod(HTTP_ANY)
, _currentVersion(0)
, _currentStatus(HC_NONE)
, _statusChange(0)
, _currentHandler(nullptr)
, _firstHandler(nullptr)
, _lastHandler(nullptr)
, _currentArgCount(0)
, _currentArgs(nullptr)
, _postArgsLen(0)
, _postArgs(nullptr)
, _headerKeysCount(0)
, _currentHeaders(nullptr)
, _contentLength(0)
, _chunked(false)
{
}

template <typename ServerType>
ESP8266WebServerTemplate<ServerType>::~ESP8266WebServerTemplate() {
  _server.close();
  if (_currentHeaders)
    delete[]_currentHeaders;
  RequestHandlerType* handler = _firstHandler;
  while (handler) {
    RequestHandlerType* next = handler->next();
    delete handler;
    handler = next;
  }
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::begin() {
  close();
  _server.begin();
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::begin(uint16_t port) {
  close();
  _server.begin(port);
}

template <typename ServerType>
String ESP8266WebServerTemplate<ServerType>::_extractParam(String& authReq,const String& param,const char delimit) const {
  int _begin = authReq.indexOf(param);
  if (_begin == -1)
    return emptyString;
  return authReq.substring(_begin+param.length(),authReq.indexOf(delimit,_begin+param.length()));
}

template <typename ServerType>
bool ESP8266WebServerTemplate<ServerType>::authenticate(const char * username, const char * password){
  if(hasHeader(FPSTR(AUTHORIZATION_HEADER))) {
    String authReq = header(FPSTR(AUTHORIZATION_HEADER));
    if(authReq.startsWith(F("Basic"))){
      authReq = authReq.substring(6);
      authReq.trim();
      char toencodeLen = strlen(username)+strlen(password)+1;
      char *toencode = new char[toencodeLen + 1];
      if(toencode == NULL){
        authReq = "";
        return false;
      }
      char *encoded = new char[base64_encode_expected_len(toencodeLen)+1];
      if(encoded == NULL){
        authReq = "";
        delete[] toencode;
        return false;
      }
      sprintf(toencode, "%s:%s", username, password);
      if(base64_encode_chars(toencode, toencodeLen, encoded) > 0 && authReq.equalsConstantTime(encoded)) {
        authReq = "";
        delete[] toencode;
        delete[] encoded;
        return true;
      }
      delete[] toencode;
      delete[] encoded;
    } else if(authReq.startsWith(F("Digest"))) {
      String _realm    = _extractParam(authReq, F("realm=\""));
      String _H1 = credentialHash((String)username,_realm,(String)password);
      return authenticateDigest((String)username,_H1);
    }
    authReq = "";
  }
  return false;
}

template <typename ServerType>
bool ESP8266WebServerTemplate<ServerType>::authenticateDigest(const String& username, const String& H1)
{
  if(hasHeader(FPSTR(AUTHORIZATION_HEADER))) {
    String authReq = header(FPSTR(AUTHORIZATION_HEADER));
    if(authReq.startsWith(F("Digest"))) {
      authReq = authReq.substring(7);
      #ifdef DEBUG_ESP_HTTP_SERVER
      DEBUG_OUTPUT.println(authReq);
      #endif
      String _username = _extractParam(authReq,F("username=\""));
      if(!_username.length() || _username != String(username)) {
        authReq = "";
        return false;
      }
      // extracting required parameters for RFC 2069 simpler Digest
      String _realm    = _extractParam(authReq, F("realm=\""));
      String _nonce    = _extractParam(authReq, F("nonce=\""));
      String _uri      = _extractParam(authReq, F("uri=\""));
      String _response = _extractParam(authReq, F("response=\""));
      String _opaque   = _extractParam(authReq, F("opaque=\""));

      if((!_realm.length()) || (!_nonce.length()) || (!_uri.length()) || (!_response.length()) || (!_opaque.length())) {
        authReq = "";
        return false;
      }
      if((_opaque != _sopaque) || (_nonce != _snonce) || (_realm != _srealm)) {
        authReq = "";
        return false;
      }
      // parameters for the RFC 2617 newer Digest
      String _nc,_cnonce;
      if(authReq.indexOf(FPSTR(qop_auth)) != -1 || authReq.indexOf(FPSTR(qop_auth_quoted)) != -1) {
        _nc = _extractParam(authReq, F("nc="), ',');
        _cnonce = _extractParam(authReq, F("cnonce=\""));
      }
      #ifdef DEBUG_ESP_HTTP_SERVER
      DEBUG_OUTPUT.println("Hash of user:realm:pass=" + H1);
      #endif
      MD5Builder md5;
      md5.begin();
      if(_currentMethod == HTTP_GET){
        md5.add(String(F("GET:")) + _uri);
      }else if(_currentMethod == HTTP_POST){
        md5.add(String(F("POST:")) + _uri);
      }else if(_currentMethod == HTTP_PUT){
        md5.add(String(F("PUT:")) + _uri);
      }else if(_currentMethod == HTTP_DELETE){
        md5.add(String(F("DELETE:")) + _uri);
      }else{
        md5.add(String(F("GET:")) + _uri);
      }
      md5.calculate();
      String _H2 = md5.toString();
      #ifdef DEBUG_ESP_HTTP_SERVER
      DEBUG_OUTPUT.println("Hash of GET:uri=" + _H2);
      #endif
      md5.begin();
      if(authReq.indexOf(FPSTR(qop_auth)) != -1 || authReq.indexOf(FPSTR(qop_auth_quoted)) != -1) {
        md5.add(H1 + ':' + _nonce + ':' + _nc + ':' + _cnonce + F(":auth:") + _H2);
      } else {
        md5.add(H1 + ':' + _nonce + ':' + _H2);
      }
      md5.calculate();
      String _responsecheck = md5.toString();
      #ifdef DEBUG_ESP_HTTP_SERVER
      DEBUG_OUTPUT.println("The Proper response=" +_responsecheck);
      #endif
      if(_response == _responsecheck){
        authReq = "";
        return true;
      }
    }
    authReq = "";
  }
  return false;
}

template <typename ServerType>
String ESP8266WebServerTemplate<ServerType>::_getRandomHexString() {
  char buffer[33];  // buffer to hold 32 Hex Digit + /0
  int i;
  for(i = 0; i < 4; i++) {
    sprintf (buffer + (i*8), "%08x", RANDOM_REG32);
  }
  return String(buffer);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::requestAuthentication(HTTPAuthMethod mode, const char* realm, const String& authFailMsg) {
  if(realm == NULL) {
    _srealm = String(F("Login Required"));
  } else {
    _srealm = String(realm);
  }
  if(mode == BASIC_AUTH) {
    sendHeader(String(FPSTR(WWW_Authenticate)), String(F("Basic realm=\"")) + _srealm + String('\"'));
  } else {
    _snonce=_getRandomHexString();
    _sopaque=_getRandomHexString();
    sendHeader(String(FPSTR(WWW_Authenticate)), String(F("Digest realm=\"")) +_srealm + String(F("\", qop=\"auth\", nonce=\"")) + _snonce + String(F("\", opaque=\"")) + _sopaque + String('\"'));
  }
  using namespace mime;
  send(401, String(FPSTR(mimeTable[html].mimeType)), authFailMsg);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::on(const Uri &uri, ESP8266WebServerTemplate<ServerType>::THandlerFunction handler) {
  on(uri, HTTP_ANY, handler);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::on(const Uri &uri, HTTPMethod method, ESP8266WebServerTemplate<ServerType>::THandlerFunction fn) {
  on(uri, method, fn, _fileUploadHandler);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::on(const Uri &uri, HTTPMethod method, ESP8266WebServerTemplate<ServerType>::THandlerFunction fn, ESP8266WebServerTemplate<ServerType>::THandlerFunction ufn) {
  _addRequestHandler(new FunctionRequestHandler<ServerType>(fn, ufn, uri, method));
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::addHandler(RequestHandlerType* handler) {
    _addRequestHandler(handler);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::_addRequestHandler(RequestHandlerType* handler) {
    if (!_lastHandler) {
      _firstHandler = handler;
      _lastHandler = handler;
    }
    else {
      _lastHandler->next(handler);
      _lastHandler = handler;
    }
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::serveStatic(const char* uri, FS& fs, const char* path, const char* cache_header) {
    _addRequestHandler(new StaticRequestHandler<ServerType>(fs, path, uri, cache_header));
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::handleClient() {
  if (_currentStatus == HC_NONE) {
    ClientType client = _server.available();
    if (!client) {
      return;
    }

#ifdef DEBUG_ESP_HTTP_SERVER
    DEBUG_OUTPUT.println("New client");
#endif

    _currentClient = client;
    _currentStatus = HC_WAIT_READ;
    _statusChange = millis();
  }

  bool keepCurrentClient = false;
  bool callYield = false;

  if (_currentClient.connected() || _currentClient.available()) {
    switch (_currentStatus) {
    case HC_NONE:
      // No-op to avoid C++ compiler warning
      break;
    case HC_WAIT_READ:
      // Wait for data from client to become available
      if (_currentClient.available()) {
        if (_parseRequest(_currentClient)) {
          _currentClient.setTimeout(HTTP_MAX_SEND_WAIT);
          _contentLength = CONTENT_LENGTH_NOT_SET;
          _handleRequest();

          if (_currentClient.connected()) {
            _currentStatus = HC_WAIT_CLOSE;
            _statusChange = millis();
            keepCurrentClient = true;
          }
        }
      } else { // !_currentClient.available()
        if (millis() - _statusChange <= HTTP_MAX_DATA_WAIT) {
          keepCurrentClient = true;
        }
        callYield = true;
      }
      break;
    case HC_WAIT_CLOSE:
      // Wait for client to close the connection
      if (millis() - _statusChange <= HTTP_MAX_CLOSE_WAIT) {
        keepCurrentClient = true;
        callYield = true;
      }
    }
  }

  if (!keepCurrentClient) {
    _currentClient = ClientType();
    _currentStatus = HC_NONE;
    _currentUpload.reset();
  }

  if (callYield) {
    yield();
  }
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::close() {
  _server.close();
  _currentStatus = HC_NONE;
  if(!_headerKeysCount)
    collectHeaders(0, 0);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::stop() {
  close();
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::sendHeader(const String& name, const String& value, bool first) {
  String headerLine = name;
  headerLine += F(": ");
  headerLine += value;
  headerLine += "\r\n";

  if (first) {
    _responseHeaders = headerLine + _responseHeaders;
  }
  else {
    _responseHeaders += headerLine;
  }
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::setContentLength(const size_t contentLength) {
    _contentLength = contentLength;
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::_prepareHeader(String& response, int code, const char* content_type, size_t contentLength) {
    response = String(F("HTTP/1.")) + String(_currentVersion) + ' ';
    response += String(code);
    response += ' ';
    response += responseCodeToString(code);
    response += "\r\n";

    using namespace mime;
    if (!content_type)
        content_type = mimeTable[html].mimeType;

    sendHeader(String(F("Content-Type")), String(FPSTR(content_type)), true);
    if (_contentLength == CONTENT_LENGTH_NOT_SET) {
        sendHeader(String(FPSTR(Content_Length)), String(contentLength));
    } else if (_contentLength != CONTENT_LENGTH_UNKNOWN) {
        sendHeader(String(FPSTR(Content_Length)), String(_contentLength));
    } else if(_contentLength == CONTENT_LENGTH_UNKNOWN && _currentVersion){ //HTTP/1.1 or above client
      //let's do chunked
      _chunked = true;
      sendHeader(String(F("Accept-Ranges")),String(F("none")));
      sendHeader(String(F("Transfer-Encoding")),String(F("chunked")));
    }
    sendHeader(String(F("Connection")), String(F("close")));

    response += _responseHeaders;
    response += "\r\n";
    _responseHeaders = "";
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send(int code, const char* content_type, const String& content) {
    String header;
    // Can we asume the following?
    //if(code == 200 && content.length() == 0 && _contentLength == CONTENT_LENGTH_NOT_SET)
    //  _contentLength = CONTENT_LENGTH_UNKNOWN;
    _prepareHeader(header, code, content_type, content.length());
    size_t sent = StreamPtr(header.c_str(), header.length()).to(&_currentClient); // transfer all of it, with timeout
    (void)sent;
#ifdef DEBUG_ESP_HTTP_SERVER
    if (sent != header.length())
        DEBUG_OUTPUT.printf("HTTPServer: error: sent %zd on %zd bytes\n", sent, header.length());
#endif

    if(content.length())
      sendContent(content);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send_P(int code, PGM_P content_type, PGM_P content) {
    size_t contentLength = 0;

    if (content != NULL) {
        contentLength = strlen_P(content);
    }

    String header;
    char type[64];
    memccpy_P((void*)type, (PGM_VOID_P)content_type, 0, sizeof(type));
    _prepareHeader(header, code, (const char* )type, contentLength);
    _currentClient.write((const uint8_t *)header.c_str(), header.length());
    if (contentLength) {
        sendContent_P(content);
    }
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send_P(int code, PGM_P content_type, PGM_P content, size_t contentLength) {
    String header;
    char type[64];
    memccpy_P((void*)type, (PGM_VOID_P)content_type, 0, sizeof(type));
    _prepareHeader(header, code, (const char* )type, contentLength);
    _currentClient.write((const uint8_t *)header.c_str(), header.length());
    if (contentLength) {
      sendContent_P(content, contentLength);
    }
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send(int code, char* content_type, const String& content) {
  send(code, (const char*)content_type, content);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send(int code, const String& content_type, const String& content) {
  send(code, (const char*)content_type.c_str(), content);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::sendContent(Stream& content) {
    sendContent(&content);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::sendContent(Stream* content) {
  if (_currentMethod == HTTP_HEAD) return;
  const char * footer = "\r\n";
  size_t len = content->streamSize();
  if(_chunked) {
    char chunkSize[11];
    sprintf(chunkSize, "%zx\r\n", len);
    _currentClient.write((const uint8_t *)chunkSize, strlen(chunkSize));
  }
  size_t sent = content->to(_currentClient);
  (void)sent; /// if (sent != len) print-error-on-console-and-return-false
  if(_chunked){
    _currentClient.write((const uint8_t *)footer, 2);
    if (len == 0) {
      _chunked = false;
    }
  }
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::sendContent(const String& content) {
  if (_currentMethod == HTTP_HEAD) return;
  const char * footer = "\r\n";
  size_t len = content.length();
  if(_chunked) {
    char chunkSize[11];
    sprintf(chunkSize, "%zx\r\n", len);
    _currentClient.write((const uint8_t *)chunkSize, strlen(chunkSize));
  }
  _currentClient.write((const uint8_t *)content.c_str(), len);
  if(_chunked){
    _currentClient.write((const uint8_t *)footer, 2);
    if (len == 0) {
      _chunked = false;
    }
  }
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::sendContent_P(PGM_P content) {
  sendContent_P(content, strlen_P(content));
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::sendContent_P(PGM_P content, size_t size) {
  const char * footer = "\r\n";
  if(_chunked) {
    char chunkSize[11];
    sprintf(chunkSize, "%zx\r\n", size);
    _currentClient.write((const uint8_t *)chunkSize, strlen(chunkSize));
  }
  _currentClient.write_P(content, size);
  if(_chunked){
    _currentClient.write((const uint8_t *)footer, 2);
    if (size == 0) {
      _chunked = false;
    }
  }
}

template <typename ServerType>
String ESP8266WebServerTemplate<ServerType>::credentialHash(const String& username, const String& realm, const String& password)
{
  MD5Builder md5;
  md5.begin();
  md5.add(username + ':' + realm + ':' + password);  // md5 of the user:realm:password
  md5.calculate();
  return md5.toString();
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::_streamFileCore(const size_t fileSize, const String &fileName, const String &contentType)
{
  using namespace mime;
  setContentLength(fileSize);
  if (fileName.endsWith(String(FPSTR(mimeTable[gz].endsWith))) &&
      contentType != String(FPSTR(mimeTable[gz].mimeType)) &&
      contentType != String(FPSTR(mimeTable[none].mimeType))) {
    sendHeader(F("Content-Encoding"), F("gzip"));
  }
  send(200, contentType, emptyString);
}

template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::pathArg(unsigned int i) const { 
  if (_currentHandler != nullptr)
    return _currentHandler->pathArg(i);
  return emptyString;
}

template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::arg(const String& name) const {
  for (int j = 0; j < _postArgsLen; ++j) {
    if ( _postArgs[j].key == name )
      return _postArgs[j].value;
  }
  for (int i = 0; i < _currentArgCount; ++i) {
    if ( _currentArgs[i].key == name )
      return _currentArgs[i].value;
  }
  return emptyString;
}

template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::arg(int i) const {
  if (i >= 0 && i < _currentArgCount)
    return _currentArgs[i].value;
  return emptyString;
}

template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::argName(int i) const {
  if (i >= 0 && i < _currentArgCount)
    return _currentArgs[i].key;
  return emptyString;
}

template <typename ServerType>
int ESP8266WebServerTemplate<ServerType>::args() const {
  return _currentArgCount;
}

template <typename ServerType>
bool ESP8266WebServerTemplate<ServerType>::hasArg(const String& name) const {
  for (int j = 0; j < _postArgsLen; ++j) {
    if (_postArgs[j].key == name)
      return true;
  }
  for (int i = 0; i < _currentArgCount; ++i) {
    if (_currentArgs[i].key == name)
      return true;
  }
  return false;
}


template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::header(const String& name) const {
  for (int i = 0; i < _headerKeysCount; ++i) {
    if (_currentHeaders[i].key.equalsIgnoreCase(name))
      return _currentHeaders[i].value;
  }
  return emptyString;
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::collectHeaders(const char* headerKeys[], const size_t headerKeysCount) {
  _headerKeysCount = headerKeysCount + 1;
  if (_currentHeaders)
     delete[]_currentHeaders;
  _currentHeaders = new RequestArgument[_headerKeysCount];
  _currentHeaders[0].key = FPSTR(AUTHORIZATION_HEADER);
  for (int i = 1; i < _headerKeysCount; i++){
    _currentHeaders[i].key = headerKeys[i-1];
  }
}

template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::header(int i) const {
  if (i < _headerKeysCount)
    return _currentHeaders[i].value;
  return emptyString;
}

template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::headerName(int i) const {
  if (i < _headerKeysCount)
    return _currentHeaders[i].key;
  return emptyString;
}

template <typename ServerType>
int ESP8266WebServerTemplate<ServerType>::headers() const {
  return _headerKeysCount;
}

template <typename ServerType>
bool ESP8266WebServerTemplate<ServerType>::hasHeader(const String& name) const {
  for (int i = 0; i < _headerKeysCount; ++i) {
    if ((_currentHeaders[i].key.equalsIgnoreCase(name)) &&  (_currentHeaders[i].value.length() > 0))
      return true;
  }
  return false;
}

template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::hostHeader() const {
  return _hostHeader;
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::onFileUpload(THandlerFunction fn) {
  _fileUploadHandler = fn;
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::onNotFound(THandlerFunction fn) {
  _notFoundHandler = fn;
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::_handleRequest() {
  bool handled = false;
  if (!_currentHandler){
#ifdef DEBUG_ESP_HTTP_SERVER
    DEBUG_OUTPUT.println("request handler not found");
#endif
  }
  else {
    handled = _currentHandler->handle(*this, _currentMethod, _currentUri);
#ifdef DEBUG_ESP_HTTP_SERVER
    if (!handled) {
      DEBUG_OUTPUT.println("request handler failed to handle request");
    }
#endif
  }
  if (!handled && _notFoundHandler) {
    _notFoundHandler();
    handled = true;
  }
  if (!handled) {
    using namespace mime;
    send(404, String(FPSTR(mimeTable[html].mimeType)), String(F("Not found: ")) + _currentUri);
    handled = true;
  }
  if (handled) {
    _finalizeResponse();
  }
  _currentUri = "";
}


template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::_finalizeResponse() {
  if (_chunked) {
    sendContent(emptyString);
  }
}

template <typename ServerType>
String ESP8266WebServerTemplate<ServerType>::responseCodeToString(const int code) {
    // By first determining the pointer to the flash stored string in the switch
    // statement and then doing String(FlashStringHelper) return reduces the total code
    // size of this function by over 50%.
    const __FlashStringHelper *r;
    switch (code)
    {
    case 100:
        r = F("Continue");
        break;
    case 101:
        r = F("Switching Protocols");
        break;
    case 200:
        r = F("OK");
        break;
    case 201:
        r = F("Created");
        break;
    case 202:
        r = F("Accepted");
        break;
    case 203:
        r = F("Non-Authoritative Information");
        break;
    case 204:
        r = F("No Content");
        break;
    case 205:
        r = F("Reset Content");
        break;
    case 206:
        r = F("Partial Content");
        break;
    case 300:
        r = F("Multiple Choices");
        break;
    case 301:
        r = F("Moved Permanently");
        break;
    case 302:
        r = F("Found");
        break;
    case 303:
        r = F("See Other");
        break;
    case 304:
        r = F("Not Modified");
        break;
    case 305:
        r = F("Use Proxy");
        break;
    case 307:
        r = F("Temporary Redirect");
        break;
    case 400:
        r = F("Bad Request");
        break;
    case 401:
        r = F("Unauthorized");
        break;
    case 402:
        r = F("Payment Required");
        break;
    case 403:
        r = F("Forbidden");
        break;
    case 404:
        r = F("Not Found");
        break;
    case 405:
        r = F("Method Not Allowed");
        break;
    case 406:
        r = F("Not Acceptable");
        break;
    case 407:
        r = F("Proxy Authentication Required");
        break;
    case 408:
        r = F("Request Timeout");
        break;
    case 409:
        r = F("Conflict");
        break;
    case 410:
        r = F("Gone");
        break;
    case 411:
        r = F("Length Required");
        break;
    case 412:
        r = F("Precondition Failed");
        break;
    case 413:
        r = F("Request Entity Too Large");
        break;
    case 414:
        r = F("URI Too Long");
        break;
    case 415:
        r = F("Unsupported Media Type");
        break;
    case 416:
        r = F("Range not satisfiable");
        break;
    case 417:
        r = F("Expectation Failed");
        break;
    case 500:
        r = F("Internal Server Error");
        break;
    case 501:
        r = F("Not Implemented");
        break;
    case 502:
        r = F("Bad Gateway");
        break;
    case 503:
        r = F("Service Unavailable");
        break;
    case 504:
        r = F("Gateway Timeout");
        break;
    case 505:
        r = F("HTTP Version not supported");
        break;
    default:
        r = F("");
        break;
    }
    return String(r);
}
