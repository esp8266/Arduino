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

enum HTTPMethod { HTTP_ANY, HTTP_GET, HTTP_POST, HTTP_PUT, HTTP_PATCH, HTTP_DELETE };
enum HTTPUploadStatus { UPLOAD_FILE_START, UPLOAD_FILE_WRITE, UPLOAD_FILE_END };

#define HTTP_DOWNLOAD_UNIT_SIZE 1460
#define HTTP_UPLOAD_BUFLEN 2048
#define HTTP_MAX_DATA_WAIT 1000 //ms to wait for the client to send the request
#define HTTP_MAX_CLOSE_WAIT 2000 //ms to wait for the client to close the connection

#define CONTENT_LENGTH_UNKNOWN ((size_t) -1)
#define CONTENT_LENGTH_NOT_SET ((size_t) -2)

typedef struct {
  HTTPUploadStatus status;
  String  filename;
  String  name;
  String  type;
  size_t  totalSize;    // file size
  size_t  currentSize;  // size of data currently in buf
  uint8_t buf[HTTP_UPLOAD_BUFLEN];
} HTTPUpload;

class ESP8266WebServer
{
public:
  ESP8266WebServer(int port = 80);
  ~ESP8266WebServer();

  void begin();
  void handleClient();

  typedef std::function<void(void)> THandlerFunction;
  void on(const char* uri, THandlerFunction handler);
  void on(const char* uri, HTTPMethod method, THandlerFunction fn);
  void onNotFound(THandlerFunction fn);  //called when handler is not assigned
  void onFileUpload(THandlerFunction fn); //handle file uploads

  String uri() { return _currentUri; }
  HTTPMethod method() { return _currentMethod; }
  WiFiClient client() { return _currentClient; }
  HTTPUpload& upload() { return _currentUpload; }
  
  String arg(const char* name);   // get request argument value by name
  String arg(int i);              // get request argument value by number
  String argName(int i);          // get request argument name by number
  int args();                     // get arguments count
  bool hasArg(const char* name);  // check if argument exists
  
  // send response to the client
  // code - HTTP response code, can be 200 or 404
  // content_type - HTTP content type, like "text/plain" or "image/png"
  // content - actual content body
  void send(int code, const char* content_type = NULL, String content = String(""));
  void send(int code, char* content_type, String content);
  void send(int code, String content_type, String content);

  void setContentLength(size_t contentLength) { _contentLength = contentLength; }
  void sendHeader(String name, String value, bool first = false);
  void sendContent(String content);

template<typename T> size_t streamFile(T &file, String contentType){
  setContentLength(file.size());
  if (String(file.name()).endsWith(".gz") && 
      contentType != "application/x-gzip" &&
      contentType != "application/octet-stream"){
    sendHeader("Content-Encoding", "gzip");
  }
  send(200, contentType, "");
  return _currentClient.write(file, HTTP_DOWNLOAD_UNIT_SIZE);
}
  
protected:
  void _handleRequest();
  bool _parseRequest(WiFiClient& client);
  void _parseArguments(String data);
  static const char* _responseCodeToString(int code);
  void _parseForm(WiFiClient& client, String boundary, uint32_t len);
  void _uploadWriteByte(uint8_t b);
  uint8_t _uploadReadByte(WiFiClient& client);
  
  struct RequestHandler;
  struct RequestArgument {
    String key;
    String value;
  };

  WiFiServer  _server;

  WiFiClient  _currentClient;
  HTTPMethod  _currentMethod;
  String      _currentUri;

  size_t           _currentArgCount;
  RequestArgument* _currentArgs;
  HTTPUpload       _currentUpload;

  size_t           _contentLength;
  String           _responseHeaders;

  RequestHandler*  _firstHandler;
  RequestHandler*  _lastHandler;
  THandlerFunction _notFoundHandler;
  THandlerFunction _fileUploadHandler;

};


#endif //ESP8266WEBSERVER_H
