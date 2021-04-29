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
#include <functional>
#include <ESP8266WiFi.h>
#include <FS.h>
#include "detail/mimetable.h"
#include "Uri.h"

//#define DEBUG_ESP_HTTP_SERVER

#ifdef DEBUG_ESP_HTTP_SERVER
#ifdef DEBUG_ESP_PORT
#define DBGWS(f,...) do { DEBUG_ESP_PORT.printf(PSTR(f), ##__VA_ARGS__); } while (0)
#else
#define DBGWS(f,...) do { Serial.printf(PSTR(f), ##__VA_ARGS__); } while (0)
#endif
#else
#define DBGWS(x...) do { (void)0; } while (0)
#endif

enum HTTPMethod { HTTP_ANY, HTTP_GET, HTTP_HEAD, HTTP_POST, HTTP_PUT, HTTP_PATCH, HTTP_DELETE, HTTP_OPTIONS };
enum HTTPUploadStatus { UPLOAD_FILE_START, UPLOAD_FILE_WRITE, UPLOAD_FILE_END,
                        UPLOAD_FILE_ABORTED };
enum HTTPClientStatus { HC_NONE, HC_WAIT_READ, HC_WAIT_CLOSE };
enum HTTPAuthMethod { BASIC_AUTH, DIGEST_AUTH };

#define WEBSERVER_HAS_HOOK 1

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

}

#include "detail/RequestHandler.h"

namespace esp8266webserver {

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
  enum ClientFuture { CLIENT_REQUEST_CAN_CONTINUE, CLIENT_REQUEST_IS_HANDLED, CLIENT_MUST_STOP, CLIENT_IS_GIVEN };
  typedef String (*ContentTypeFunction) (const String&);
  using HookFunction = std::function<ClientFuture(const String& method, const String& url, WiFiClient* client, ContentTypeFunction contentType)>;

  void begin();
  void begin(uint16_t port);
  void handleClient();
  void close();
  void stop();

  bool authenticate(const char * username, const char * password);
  bool authenticateDigest(const String& username, const String& H1);
  void requestAuthentication(HTTPAuthMethod mode = BASIC_AUTH, const char* realm = NULL, const String& authFailMsg = String("") );

  typedef std::function<void(void)> THandlerFunction;
  void on(const Uri &uri, THandlerFunction handler);
  void on(const Uri &uri, HTTPMethod method, THandlerFunction fn);
  void on(const Uri &uri, HTTPMethod method, THandlerFunction fn, THandlerFunction ufn);
  void addHandler(RequestHandlerType* handler);
  void serveStatic(const char* uri, fs::FS& fs, const char* path, const char* cache_header = NULL );
  void onNotFound(THandlerFunction fn);  //called when handler is not assigned
  void onFileUpload(THandlerFunction fn); //handle file uploads
  void enableCORS(bool enable);

  const String& uri() const { return _currentUri; }
  HTTPMethod method() const { return _currentMethod; }
  ClientType& client() { return _currentClient; }
  HTTPUpload& upload() { return *_currentUpload; }

  // Allows setting server options (i.e. SSL keys) by the instantiator
  ServerType &getServer() { return _server; }

  const String& pathArg(unsigned int i) const; // get request path argument by number
  const String& arg(const String& name) const;    // get request argument value by name
  const String& arg(int i) const;          // get request argument value by number
  const String& argName(int i) const;      // get request argument name by number
  int args() const;                        // get arguments count
  bool hasArg(const String& name) const;   // check if argument exists
  void collectHeaders(const char* headerKeys[], const size_t headerKeysCount); // set the request headers to collect
  template<typename... Args>
  void collectHeaders(const Args&... args); // set the request headers to collect (variadic template version)
  const String& header(const String& name) const; // get request header value by name
  const String& header(int i) const;       // get request header value by number
  const String& headerName(int i) const;   // get request header name by number
  int headers() const;                     // get header count
  bool hasHeader(const String& name) const;       // check if header exists
  const String& hostHeader() const;        // get request host header if available or empty String if not

  // send response to the client
  // code - HTTP response code, can be 200 or 404
  // content_type - HTTP content type, like "text/plain" or "image/png"
  // content - actual content body
  void send(int code, const char* content_type = NULL, const String& content = emptyString);
  void send(int code, char* content_type, const String& content);
  void send(int code, const String& content_type, const String& content);
  void send(int code, const char *content_type, const char *content) {
    send_P(code, content_type, content);
  }
  void send(int code, const char *content_type, const char *content, size_t content_length) {
    send_P(code, content_type, content, content_length);
  }
  void send(int code, const char *content_type, const uint8_t *content, size_t content_length) {
    send_P(code, content_type, (const char *)content, content_length);
  }
  void send_P(int code, PGM_P content_type, PGM_P content);
  void send_P(int code, PGM_P content_type, PGM_P content, size_t contentLength);

  void send(int code, const char* content_type, Stream* stream, size_t content_length = 0);
  void send(int code, const char* content_type, Stream& stream, size_t content_length = 0);

  void setContentLength(const size_t contentLength);
  void sendHeader(const String& name, const String& value, bool first = false);
  void sendContent(const String& content);
  void sendContent(String& content) {
    sendContent((const String&)content);
  }
  void sendContent_P(PGM_P content);
  void sendContent_P(PGM_P content, size_t size);
  void sendContent(const char *content) { sendContent_P(content); }
  void sendContent(const char *content, size_t size) { sendContent_P(content, size); }

  void sendContent(Stream* content, ssize_t content_length = 0);
  void sendContent(Stream& content, ssize_t content_length = 0) { sendContent(&content, content_length); }

  bool chunkedResponseModeStart_P (int code, PGM_P content_type) {
    if (_currentVersion == 0)
        // no chunk mode in HTTP/1.0
        return false;
    setContentLength(CONTENT_LENGTH_UNKNOWN);
    send_P(code, content_type, "");
    return true;
  }
  bool chunkedResponseModeStart (int code, const char* content_type) {
    return chunkedResponseModeStart_P(code, content_type);
  }
  bool chunkedResponseModeStart (int code, const String& content_type) {
    return chunkedResponseModeStart_P(code, content_type.c_str());
  }
  void chunkedResponseFinalize () {
    sendContent(emptyString);
  }

  // Whether other requests should be accepted from the client on the
  // same socket after a response is sent.
  // This will automatically configure the "Connection" header of the response.
  // Defaults to true when the client's HTTP version is 1.1 or above, otherwise it defaults to false.
  // If the client sends the "Connection" header, the value given by the header is used.
  void keepAlive(bool keepAlive) { _keepAlive = keepAlive; }
  bool keepAlive() { return _keepAlive; }

  static String credentialHash(const String& username, const String& realm, const String& password);

  static String urlDecode(const String& text);

  // Handle a GET request by sending a response header and stream file content to response body
  template<typename T>
  size_t streamFile(T &file, const String& contentType) {
    return streamFile(file, contentType, HTTP_GET);
  }

  // Implement GET and HEAD requests for files.
  // Stream body on HTTP_GET but not on HTTP_HEAD requests.
  template<typename T>
  size_t streamFile(T &file, const String& contentType, HTTPMethod requestMethod) {
    size_t contentLength = 0;
    _streamFileCore(file.size(), file.name(), contentType);
    if (requestMethod == HTTP_GET) {
      contentLength = file.sendAll(_currentClient);
    }
    return contentLength;
  }

  // Implement GET and HEAD requests for stream
  // Stream body on HTTP_GET but not on HTTP_HEAD requests.
  template<typename T>
  size_t stream(T &aStream, const String& contentType, HTTPMethod requestMethod, ssize_t size) {
    setContentLength(size);
    send(200, contentType, emptyString);
    if (requestMethod == HTTP_GET)
        size = aStream.sendSize(_currentClient, size);
    return size;
  }

  // Implement GET and HEAD requests for stream
  // Stream body on HTTP_GET but not on HTTP_HEAD requests.
  template<typename T>
  size_t stream(T& aStream, const String& contentType, HTTPMethod requestMethod = HTTP_GET) {
    ssize_t size = aStream.size();
    if (size < 0)
    {
        send(500, F("text/html"), F("input stream: undetermined size"));
        return 0;
    }
    return stream(aStream, contentType, requestMethod, size);
  }

  static String responseCodeToString(const int code);

  void addHook (HookFunction hook) {
    if (_hook) {
      auto previousHook = _hook;
      _hook = [previousHook, hook](const String& method, const String& url, WiFiClient* client, ContentTypeFunction contentType) {
          auto whatNow = previousHook(method, url, client, contentType);
          if (whatNow == CLIENT_REQUEST_CAN_CONTINUE)
            return hook(method, url, client, contentType);
          return whatNow;
        };
    } else {
      _hook = hook;
    }
  }

protected:
  void _addRequestHandler(RequestHandlerType* handler);
  void _handleRequest();
  void _finalizeResponse();
  ClientFuture _parseRequest(ClientType& client);
  void _parseArguments(const String& data);
  int _parseArgumentsPrivate(const String& data, std::function<void(String&,String&,const String&,int,int,int,int)> handler);
  bool _parseForm(ClientType& client, const String& boundary, uint32_t len);
  bool _parseFormUploadAborted();
  void _uploadWriteByte(uint8_t b);
  int _uploadReadByte(ClientType& client);
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
  HTTPMethod  _currentMethod = HTTP_ANY;
  String      _currentUri;
  uint8_t     _currentVersion = 0;
  HTTPClientStatus _currentStatus = HC_NONE;
  unsigned long _statusChange = 0;

  RequestHandlerType*  _currentHandler = nullptr;
  RequestHandlerType*  _firstHandler = nullptr;
  RequestHandlerType*  _lastHandler = nullptr;
  THandlerFunction _notFoundHandler;
  THandlerFunction _fileUploadHandler;

  int              _currentArgCount = 0;
  RequestArgument* _currentArgs = nullptr;
  int              _currentArgsHavePlain = 0;
  std::unique_ptr<HTTPUpload> _currentUpload;
  int              _postArgsLen = 0;
  RequestArgument* _postArgs = nullptr;

  int              _headerKeysCount = 0;
  RequestArgument* _currentHeaders = nullptr;

  size_t           _contentLength = 0;
  String           _responseHeaders;

  String           _hostHeader;
  bool             _chunked = false;
  bool             _corsEnabled = false;
  bool             _keepAlive = false;

  String           _snonce;  // Store noance and opaque for future comparison
  String           _sopaque;
  String           _srealm;  // Store the Auth realm between Calls

  HookFunction     _hook;
};

} // namespace

#include "ESP8266WebServer-impl.h"
#include "Parsing-impl.h"

using ESP8266WebServer = esp8266webserver::ESP8266WebServerTemplate<WiFiServer>;
using RequestHandler = esp8266webserver::RequestHandler<WiFiServer>;

#endif //ESP8266WEBSERVER_H