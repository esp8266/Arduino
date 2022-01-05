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
#include "base64.h"
#include "detail/RequestHandlersImpl.h"
#include <StreamDev.h>

static const char AUTHORIZATION_HEADER[] PROGMEM = "Authorization";
static const char qop_auth[] PROGMEM = "qop=auth";
static const char qop_auth_quoted[] PROGMEM = "qop=\"auth\"";
static const char WWW_Authenticate[] PROGMEM = "WWW-Authenticate";
static const char Content_Length[] PROGMEM = "Content-Length";
static const char ETAG_HEADER[] PROGMEM = "If-None-Match";

namespace esp8266webserver {

template <typename ServerType>
ESP8266WebServerTemplate<ServerType>::ESP8266WebServerTemplate(IPAddress addr, int port)
: _server(addr, port)
{
}

template <typename ServerType>
ESP8266WebServerTemplate<ServerType>::ESP8266WebServerTemplate(int port)
: _server(port)
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
void ESP8266WebServerTemplate<ServerType>::enableCORS(bool enable) {
  _corsEnabled = enable;
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::enableETag(bool enable, ETagFunction fn) {
  _eTagEnabled = enable;
  _eTagFunction = fn;
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
      char *toencode = new (std::nothrow) char[toencodeLen + 1];
      if(toencode == NULL){
        authReq = "";
        return false;
      }
      sprintf(toencode, "%s:%s", username, password);
      String encoded = base64::encode((uint8_t *)toencode, toencodeLen, false);
      if(!encoded){
        authReq = "";
        delete[] toencode;
        return false;
      }
      if(authReq.equalsConstantTime(encoded)) {
        authReq = "";
        delete[] toencode;
        return true;
      }
      delete[] toencode;
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
      DBGWS("%s\n", authReq.c_str());
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
      DBGWS("Hash of user:realm:pass=%s\n", H1.c_str());
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
      DBGWS("Hash of GET:uri=%s\n", _H2.c_str());
      md5.begin();
      if(authReq.indexOf(FPSTR(qop_auth)) != -1 || authReq.indexOf(FPSTR(qop_auth_quoted)) != -1) {
        md5.add(H1 + ':' + _nonce + ':' + _nc + ':' + _cnonce + F(":auth:") + _H2);
      } else {
        md5.add(H1 + ':' + _nonce + ':' + _H2);
      }
      md5.calculate();
      String _responsecheck = md5.toString();
      DBGWS("The Proper response=%s\n", _responsecheck.c_str());
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
  bool is_file = false;

  if (fs.exists(path)) {
    File file = fs.open(path, "r");
    is_file = file && file.isFile();
    file.close();
  }

  if(is_file) {
    _addRequestHandler(new StaticFileRequestHandler<ServerType>(fs, path, uri, cache_header));
  } else {
    _addRequestHandler(new StaticDirectoryRequestHandler<ServerType>(fs, path, uri, cache_header));  
  }
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::handleClient() {
  if (_currentStatus == HC_NONE) {
    ClientType client = _server.accept();
    if (!client) {
      return;
    }

    DBGWS("New client\n");

    _currentClient = client;
    _currentStatus = HC_WAIT_READ;
    _statusChange = millis();
  }

  bool keepCurrentClient = false;
  bool callYield = false;

  DBGWS("http-server loop: conn=%d avail=%d status=%s\n",
    _currentClient.connected(), _currentClient.available(),
    _currentStatus==HC_NONE?"none":
    _currentStatus==HC_WAIT_READ?"wait-read":
    _currentStatus==HC_WAIT_CLOSE?"wait-close":
    "??");

  if (_currentClient.connected() || _currentClient.available()) {
    if (_currentClient.available() && _keepAlive) {
      _currentStatus = HC_WAIT_READ;
    }

    switch (_currentStatus) {
    case HC_NONE:
      // No-op to avoid C++ compiler warning
      break;
    case HC_WAIT_READ:
      // Wait for data from client to become available
      if (_currentClient.available()) {
        switch (_parseRequest(_currentClient))
        {
        case CLIENT_REQUEST_CAN_CONTINUE:
          _currentClient.setTimeout(HTTP_MAX_SEND_WAIT);
          _contentLength = CONTENT_LENGTH_NOT_SET;
          _handleRequest();
          /* fallthrough */
        case CLIENT_REQUEST_IS_HANDLED:
          if (_currentClient.connected() || _currentClient.available()) {
            _currentStatus = HC_WAIT_CLOSE;
            _statusChange = millis();
            keepCurrentClient = true;
          }
          else
            DBGWS("webserver: peer has closed after served\n");
          break;
        case CLIENT_MUST_STOP:
          DBGWS("Close client\n");
          _currentClient.stop();
          break;
        case CLIENT_IS_GIVEN:
          // client must not be stopped but must not be handled here anymore
          // (example: tcp connection given to websocket)
          DBGWS("Give client\n");
          break;
        } // switch _parseRequest()
      } else {
        // !_currentClient.available(): waiting for more data
        unsigned long timeSinceChange = millis() - _statusChange;
        // Use faster connection drop timeout if any other client has data
        // or the buffer of pending clients is full
        if ((_server.hasClientData() || _server.hasMaxPendingClients())
          && timeSinceChange > HTTP_MAX_DATA_AVAILABLE_WAIT)
            DBGWS("webserver: closing since there's another connection to read from\n");
        else {
          if (timeSinceChange > HTTP_MAX_DATA_WAIT)
            DBGWS("webserver: closing after read timeout\n");
          else
            keepCurrentClient = true;
        }
        callYield = true;
      }
      break;
    case HC_WAIT_CLOSE:
      // Wait for client to close the connection
      if (!_server.hasClient() && (millis() - _statusChange <= HTTP_MAX_CLOSE_WAIT)) {
        keepCurrentClient = true;
        callYield = true;
        if (_currentClient.available())
            // continue serving current client
            _currentStatus = HC_WAIT_READ;
      }
      break;
    } // switch _currentStatus
  }

  if (!keepCurrentClient) {
    DBGWS("Drop client\n");
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
    collectHeaders();
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
    if (_corsEnabled) {
      sendHeader(String(F("Access-Control-Allow-Origin")), String("*"));
    }

    if (_keepAlive && _server.hasClient()) { // Disable keep alive if another client is waiting.
      _keepAlive = false;
    }
    sendHeader(String(F("Connection")), String(_keepAlive ? F("keep-alive") : F("close")));
    if (_keepAlive) {
      sendHeader(String(F("Keep-Alive")), String(F("timeout=")) + HTTP_MAX_CLOSE_WAIT);
    }


    response += _responseHeaders;
    response += "\r\n";
    _responseHeaders = "";
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send(int code, char* content_type, const String& content) {
  return send(code, (const char*)content_type, content);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send(int code, const char* content_type, const String& content) {
  return send(code, content_type, content.c_str(), content.length());
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send(int code, const String& content_type, const String& content) {
  return send(code, (const char*)content_type.c_str(), content);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::sendContent(const String& content) {
  StreamConstPtr ref(content.c_str(), content.length());
  sendContent(&ref);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send(int code, const char* content_type, Stream* stream, size_t content_length /*= 0*/) {
  String header;
  if (content_length == 0)
      content_length = std::max((ssize_t)0, stream->streamRemaining());
  _prepareHeader(header, code, content_type, content_length);
  size_t sent = StreamConstPtr(header).sendAll(&_currentClient);
  if (sent != header.length())
      DBGWS("HTTPServer: error: sent %zd on %u bytes\n", sent, header.length());
  if (content_length)
    return sendContent(stream, content_length);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send_P(int code, PGM_P content_type, PGM_P content) {
  StreamConstPtr ref(content, strlen_P(content));
  return send(code, String(content_type).c_str(), &ref);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::send_P(int code, PGM_P content_type, PGM_P content, size_t contentLength) {
  StreamConstPtr ref(content, contentLength);
  return send(code, String(content_type).c_str(), &ref);
}

template <typename ServerType>
void ESP8266WebServerTemplate<ServerType>::sendContent(Stream* content, ssize_t content_length /* = 0*/) {
  if (_currentMethod == HTTP_HEAD)
    return;
  if (content_length <= 0)
    content_length = std::max((ssize_t)0, content->streamRemaining());
  if(_chunked) {
    _currentClient.printf("%zx\r\n", content_length);
  }
  ssize_t sent = content->sendSize(&_currentClient, content_length);
  if (sent != content_length)
  {
    DBGWS("HTTPServer: error: short send after timeout (%zu < %zu)\n", sent, content_length);
  }
  if(_chunked) {
    _currentClient.printf_P(PSTR("\r\n"));
    if (content_length == 0) {
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
  StreamConstPtr ptr(content, size);
  return sendContent(&ptr, size);
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
  for (int i = 0; i < _currentArgCount + _currentArgsHavePlain; ++i) {
    if ( _currentArgs[i].key == name )
      return _currentArgs[i].value;
  }
  return emptyString;
}

template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::arg(int i) const {
  if (i >= 0 && i < _currentArgCount + _currentArgsHavePlain)
    return _currentArgs[i].value;
  return emptyString;
}

template <typename ServerType>
const String& ESP8266WebServerTemplate<ServerType>::argName(int i) const {
  if (i >= 0 && i < _currentArgCount + _currentArgsHavePlain)
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
  for (int i = 0; i < _currentArgCount + _currentArgsHavePlain; ++i) {
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

template<typename ServerType>
void ESP8266WebServerTemplate<ServerType>::collectHeaders(const char* headerKeys[], const size_t headerKeysCount) {
  if (_currentHeaders)
    delete[] _currentHeaders;
  _currentHeaders = new RequestArgument[_headerKeysCount = headerKeysCount + 2];
  _currentHeaders[0].key = FPSTR(AUTHORIZATION_HEADER);
  _currentHeaders[1].key = FPSTR(ETAG_HEADER);
  for (int i = 2; i < _headerKeysCount; i++){
      _currentHeaders[i].key = headerKeys[i - 2];
  }
}

template <typename ServerType>
template <typename... Args>
void ESP8266WebServerTemplate<ServerType>::collectHeaders(const Args&... args) {
  if (_currentHeaders)
    delete[] _currentHeaders;
  _currentHeaders = new RequestArgument[_headerKeysCount = sizeof...(args) + 2] {
    { .key = FPSTR(AUTHORIZATION_HEADER), .value = emptyString },
    { .key = FPSTR(ETAG_HEADER), .value = emptyString },
    { .key = args, .value = emptyString } ...
  };
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
    DBGWS("request handler not found\n");
  }
  else {
    handled = _currentHandler->handle(*this, _currentMethod, _currentUri);
    if (!handled) {
      DBGWS("request handler failed to handle request\n");
    }
  }
  if (!handled && _notFoundHandler) {
    _notFoundHandler();
    handled = true;
  }
  if (!handled) {
    using namespace mime;
    send(404, FPSTR(mimeTable[html].mimeType), String(F("Not found: ")) + _currentUri);
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

} // namespace
