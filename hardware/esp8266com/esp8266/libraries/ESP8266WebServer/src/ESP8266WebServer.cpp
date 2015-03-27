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
*/


#include <Arduino.h>
#include "WiFiServer.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"

// #define DEBUG

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
	Serial.println("New client");
#endif
	// Wait for data from client to become available
	while(client.connected() && !client.available()){
		delay(1);
	}

	// Read the first line of HTTP request
	String req = client.readStringUntil('\r');
	client.readStringUntil('\n');

	HTTPMethod method = HTTP_GET;
	if (req.startsWith("POST")) {
		method = HTTP_POST;
	}


	// First line of HTTP request looks like "GET /path HTTP/1.1"
	// Retrieve the "/path" part by finding the spaces
	int addr_start = req.indexOf(' ');
	int addr_end = req.indexOf(' ', addr_start + 1);
	if (addr_start == -1 || addr_end == -1) {
#ifdef DEBUG
		Serial.print("Invalid request: ");
		Serial.println(req);
#endif
		return;
	}

	req = req.substring(addr_start + 1, addr_end);

	String formData;
	if (method == HTTP_POST)
	{
		int contentLength = -1;
		int headerCount = 0;
		while(headerCount < 1024) {	// there shouldn't be that much really
			String line = client.readStringUntil('\r');
			client.readStringUntil('\n');

			if (line.length() > 0) {	// this is a header
				++headerCount;
				if (contentLength < 0 && line.startsWith("Content-Length")) {
					// get content length from the header
					int valuePos = line.indexOf(' ', 14);
					if (valuePos > 0) {
						String valueStr = line.substring(valuePos+1);
						contentLength = valueStr.toInt();
#ifdef DEBUG
						Serial.print("Content-Length: ");
						Serial.println(contentLength);
#endif
					}
				}
			}
			else {
				break;
			}
		}
#ifdef DEBUG
		Serial.print("headerCount=");
		Serial.println(headerCount);
#endif
		if (contentLength >= 0) {
			formData = "";
			int n = 0;	 // timeout counter
			while (formData.length() < contentLength && ++n < 3)
				formData += client.readString();
		}
		else {
			formData = client.readStringUntil('\r'); // will return after timing out once
		}
		// read form data
		// formData = 
	}
	else if (method == HTTP_GET)
	{
		int args_start = req.indexOf('?');
		if (args_start != -1)
		{
			formData = req.substring(args_start + 1);
			req = req.substring(0, args_start);
		}
	}

	client.flush();

#ifdef DEBUG
	Serial.print("Request: ");
	Serial.println(req);
	Serial.print("Args: ");
	Serial.println(formData);
#endif

	_parseArguments(formData);
	_handleRequest(client, req, method);

}

void ESP8266WebServer::send(int code, const char* content_type, String content) {
	String response = "HTTP/1.1 ";
	response += String(code);
	response += " ";
	if (code == 200)
		response += "OK";
	else if (code == 404)
		response += "Not found";
	response += "\r\n";
	if (!content_type)
		content_type = "text/html";
	response += "Content-Type: ";
	response += content_type;
	response += "\r\n\r\n";
	response += content;
	_currentClient.print(response);
}

String ESP8266WebServer::arg(const char* name)
{
	for (int i = 0; i < _currentArgCount; ++i) {
		if (_currentArgs[i].key == name)
			return _currentArgs[i].value;
	}
	return String();
}


void ESP8266WebServer::_parseArguments(String data)
{
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
	Serial.print("args count: ");
	Serial.println(_currentArgCount);
#endif

	_currentArgs = new RequestArgument[_currentArgCount];
	int pos = 0;
	int iarg;
	for (iarg = 0; iarg < _currentArgCount;) {
		int equal_sign_index = data.indexOf('=', pos);
		int next_arg_index = data.indexOf('&', pos);
#ifdef DEBUG
		Serial.print("pos ");
		Serial.print(pos);
		Serial.print("=@ ");
		Serial.print(equal_sign_index);
		Serial.print(" &@ ");
		Serial.println(next_arg_index);
#endif
		if (equal_sign_index == -1 || equal_sign_index > next_arg_index && next_arg_index != -1) {
#ifdef DEBUG
			Serial.print("arg missing value: ");
			Serial.println(iarg);
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
		Serial.print("arg ");
		Serial.print(iarg);
		Serial.print(" key: ");
		Serial.print(arg.key);
		Serial.print(" value: ");
		Serial.println(arg.value);
#endif
		++iarg;
		if (next_arg_index == -1)
			break;
		pos = next_arg_index + 1;
	}
	_currentArgCount = iarg;
#ifdef DEBUG
	Serial.print("args count: ");
	Serial.println(_currentArgCount);
#endif

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

	if (!handler)
	{
#ifdef DEBUG
		Serial.println("request handler not found");
#endif
		send(404, "text/plain", String("Not found: ") + uri);
	}

	_currentClient   = WiFiClient();
	_currentUri      = String();
	
}
