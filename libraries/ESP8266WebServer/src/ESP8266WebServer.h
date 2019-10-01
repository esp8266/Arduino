/*
  ESP8266WebServer.h - Dead simple web-server.
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


#ifndef ESP8266WEBSERVER_H
#define ESP8266WEBSERVER_H

#include <functional>
#include <memory>
#include <ESP8266WiFi.h>
#include <FS.h>
#include "detail/mimetable.h"

enum HTTPMethod { HTTP_ANY, HTTP_GET, HTTP_HEAD, HTTP_POST, HTTP_PUT, HTTP_PATCH, HTTP_DELETE, HTTP_OPTIONS };
enum HTTPUploadStatus { UPLOAD_FILE_START, UPLOAD_FILE_WRITE, UPLOAD_FILE_END,
                        UPLOAD_FILE_ABORTED };
enum HTTPClientStatus { HC_NONE, HC_WAIT_READ, HC_WAIT_CLOSE };
enum HTTPAuthMethod { BASIC_AUTH, DIGEST_AUTH };

#define HTTP_DOWNLOAD_UNIT_SIZE 1460

#ifndef HTTP_UPLOAD_BUFLEN
#define HTTP_UPLOAD_BUFLEN 2048
#endif

#define HTTP_MAX_DATA_WAIT 5000 //ms to wait for the client to send the request
#define HTTP_MAX_POST_WAIT 5000 //ms to wait for POST data to arrive
#define HTTP_MAX_SEND_WAIT 5000 //ms to wait for data chunk to be ACKed
#define HTTP_MAX_CLOSE_WAIT 2000 //ms to wait for the client to close the connection

#define CONTENT_LENGTH_UNKNOWN ((size_t) -1)
#define CONTENT_LENGTH_NOT_SET ((size_t) -2)

typedef struct {
  HTTPUploadStatus status;
  String  filename;
  String  name;
  String  type;
  size_t  totalSize;    // total size of uploaded file so far
  size_t  currentSize;  // size of data currently in buf
  size_t  contentLength; // size of entire post request, file size + headers and other request data.
  uint8_t buf[HTTP_UPLOAD_BUFLEN];
} HTTPUpload;

namespace esp8266webserver {

template<typename ServerType>
class ESP8266WebServerTemplate;

#include "detail/RequestHandler.h"

template<typename ServerType>
class ESP8266WebServerTemplate
{
public:
  ESP8266WebServerTemplate(IPAddress addr, int port = 80);
  ESP8266WebServerTemplate(int port = 80);
  ~ESP8266WebServerTemplate();

  using ClientType = typename ServerType::ClientType;
  using RequestHandlerType = RequestHandler<ServerType>;
  using WebServerType = ESP8266WebServerTemplate<ServerType>;

  void begin();
  void begin(uint16_t port);
  void handleClient();
  void close();
  void stop();

  bool authenticate(const char * username, const char * password);
  bool authenticateDigest(const String& username, const String& H1);
  void requestAuthentication(HTTPAuthMethod mode = BASIC_AUTH, const char* realm = NULL, const String& authFailMsg = String("") );

  typedef std::function<void(void)> THandlerFunction;
  void on(const String &uri, THandlerFunction handler);
  void on(const String &uri, HTTPMethod method, THandlerFunction fn);
  void on(const String &uri, HTTPMethod method, THandlerFunction fn, THandlerFunction ufn);
  void addHandler(RequestHandlerType* handler);
  void serveStatic(const char* uri, fs::FS& fs, const char* path, const char* cache_header = NULL );
  void onNotFound(THandlerFunction fn);  //called when handler is not assigned
  void onFileUpload(THandlerFunction fn); //handle file uploads

  const String& uri() const { return _currentUri; }
  HTTPMethod method() const { return _currentMethod; }
  ClientType client() { return _currentClient; }
  HTTPUpload& upload() { return *_currentUpload; }

  // Allows setting server options (i.e. SSL keys) by the instantiator
  ServerType &getServer() { return _server; }

  const String& arg(String name) const;    // get request argument value by name
  const String& arg(int i) const;          // get request argument value by number
  const String& argName(int i) const;      // get request argument name by number
  int args() const;                        // get arguments count
  bool hasArg(const String& name) const;   // check if argument exists
  void collectHeaders(const char* headerKeys[], const size_t headerKeysCount); // set the request headers to collect
  const String& header(String name) const; // get request header value by name
  const String& header(int i) const;       // get request header value by number
  const String& headerName(int i) const;   // get request header name by number
  int headers() const;                     // get header count
  bool hasHeader(String name) const;       // check if header exists
  const String& hostHeader() const;        // get request host header if available or empty String if not

  // send response to the client
  // code - HTTP response code, can be 200 or 404
  // content_type - HTTP content type, like "text/plain" or "image/png"
  // content - actual content body
  void send(int code, const char* content_type = NULL, const String& content = String(""));
  void send(int code, char* content_type, const String& content);
  void send(int code, const String& content_type, const String& content);
  void send_P(int code, PGM_P content_type, PGM_P content);
  void send_P(int code, PGM_P content_type, PGM_P content, size_t contentLength);

  void setContentLength(const size_t contentLength);
  void sendHeader(const String& name, const String& value, bool first = false);
  void sendContent(const String& content);
  void sendContent_P(PGM_P content);
  void sendContent_P(PGM_P content, size_t size);
  void sendContent(const char *content) { sendContent_P(content); }
  void sendContent(const char *content, size_t size) { sendContent_P(content, size); }

  static String credentialHash(const String& username, const String& realm, const String& password);

  static String urlDecode(const String& text);

  template<typename T>
  size_t streamFile(T &file, const String& contentType) {
    _streamFileCore(file.size(), file.name(), contentType);
    return _currentClient.write(file);
  }

  static const String responseCodeToString(const int code);

protected:
  void _addRequestHandler(RequestHandlerType* handler);
  void _handleRequest();
  void _finalizeResponse();
  bool _parseRequest(ClientType& client);
  void _parseArguments(const String& data);
  int _parseArgumentsPrivate(const String& data, std::function<void(String&,String&,const String&,int,int,int,int)> handler);
  bool _parseForm(ClientType& client, const String& boundary, uint32_t len);
  bool _parseFormUploadAborted();
  void _uploadWriteByte(uint8_t b);
  uint8_t _uploadReadByte(ClientType& client);
  void _prepareHeader(String& response, int code, const char* content_type, size_t contentLength);
  bool _collectHeader(const char* headerName, const char* headerValue);

  void _streamFileCore(const size_t fileSize, const String & fileName, const String & contentType);

  static String _getRandomHexString();
  // for extracting Auth parameters
  String _extractParam(String& authReq,const String& param,const char delimit = '"') const;

  struct RequestArgument {
    String key;
    String value;
  };

  ServerType  _server;
  ClientType  _currentClient;
  HTTPMethod  _currentMethod;
  String      _currentUri;
  uint8_t     _currentVersion;
  HTTPClientStatus _currentStatus;
  unsigned long _statusChange;

  RequestHandlerType*  _currentHandler;
  RequestHandlerType*  _firstHandler;
  RequestHandlerType*  _lastHandler;
  THandlerFunction _notFoundHandler;
  THandlerFunction _fileUploadHandler;

  int              _currentArgCount;
  RequestArgument* _currentArgs;
  std::unique_ptr<HTTPUpload> _currentUpload;
  int              _postArgsLen;
  RequestArgument* _postArgs;

  int              _headerKeysCount;
  RequestArgument* _currentHeaders;

  size_t           _contentLength;
  String           _responseHeaders;

  String           _hostHeader;
  bool             _chunked;

  String           _snonce;  // Store noance and opaque for future comparison
  String           _sopaque;
  String           _srealm;  // Store the Auth realm between Calls

};


#include "ESP8266WebServer-impl.h"
#include "Parsing-impl.h"

};


using ESP8266WebServer = esp8266webserver::ESP8266WebServerTemplate<WiFiServer>;
using RequestHandler = esp8266webserver::RequestHandler<WiFiServer>;


#endif //ESP8266WEBSERVER_H
