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
*/


#ifndef ESP8266WEBSERVER_H
#define ESP8266WEBSERVER_H

#include <functional>

enum HTTPMethod { HTTP_ANY, HTTP_GET, HTTP_POST };


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

	String uri() { return _currentUri; }
	HTTPMethod method() { return _currentMethod; }
	WiFiClient client() { return _currentClient; }

	// send response to the client
	// code - HTTP response code, can be 200 or 404
	// content_type - HTTP content type, like "text/plain" or "image/png"
	// content - actual content body
	void send(int code, const char* content_type = NULL, String content = String(""));

	// get request argument value
	String arg(const char* name);

protected:
	void _handleRequest(WiFiClient& client, String uri, HTTPMethod method);
	void _parseArguments(String data);

	struct RequestHandler;
	struct RequestArgument {
		String key;
		String value;
	};

	WiFiServer 	_server;

	WiFiClient 	_currentClient;
	HTTPMethod 	_currentMethod;
	String 		_currentUri;

	size_t      _currentArgCount;
	RequestArgument* _currentArgs;

	RequestHandler* _firstHandler;
	RequestHandler* _lastHandler;

};


#endif //ESP8266WEBSERVER_H
