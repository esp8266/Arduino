/*
	Parsing.cpp - HTTP request parsing.

	Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.

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


baseline:
Sketch uses 411200 bytes (39%) of program storage space. Maximum is 1044464 bytes.
Global variables use 38732 bytes (47%) of dynamic memory, leaving 43188 bytes for local variables. Maximum is 81920 bytes.

early stage (no posting, should have fast buffer reading)
Sketch uses 406924 bytes (38%) of program storage space. Maximum is 1044464 bytes.
Global variables use 38824 bytes (47%) of dynamic memory, leaving 43096 bytes for local variables. Maximum is 81920 bytes.

middle stage (everything is beautiful)
Sketch uses 407264 bytes (38%) of program storage space. Maximum is 1044464 bytes.
Global variables use 38812 bytes (47%) of dynamic memory, leaving 43108 bytes for local variables. Maximum is 81920 bytes.

early code cleanup stage (some things are still missing)
Sketch uses 407632 bytes (39%) of program storage space. Maximum is 1044464 bytes.
Global variables use 38732 bytes (47%) of dynamic memory, leaving 43188 bytes for local variables. Maximum is 81920 bytes.
*/


#include <Arduino.h>
#include "WiFiServer.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"
#include "detail/mimetable.h"

//#define DEBUG_ESP_HTTP_SERVER
#ifdef DEBUG_ESP_PORT
#define DEBUG_OUTPUT DEBUG_ESP_PORT
#else
#define DEBUG_OUTPUT Serial
#endif

namespace WebServerDarkain {


////////////////////////////////////////////////////////////////////////////////
// ATTEMPT TO REALLOCATE MEMORY - OR RETURN NULLPTR ON FAILURE
// UNLIKE NORMAL REALLOC, THIS WILL FREE() THE PREVIOUS BUFFER ON FAILURE
////////////////////////////////////////////////////////////////////////////////
template <class T>
static inline T *vealloc(T *ptr, size_t size) {
	T *newptr = (T*) realloc(ptr, size);
	if (!newptr) free(ptr);
	return newptr;
}




////////////////////////////////////////////////////////////////////////////////
// RESET ALL REQUEST VARIABLES TO DEFAULT FOR NEW INCOMING CONNECTION
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::_resetRequest() {
	// CLEAR ALL HEADERS
	_headers.reset();

	//CLEAR ALL GET/POST PARAMETERS
	_params.reset();

	//CLEAR THE REQUEST BUFFER
	free(_request);

	//RESET ALL OTHER STATUS VARIABLES
	_readTimeout		= millis();
	_readBytes			= 0;
	_request			= nullptr;
	_requestMethod		= nullptr;
	_requestPath		= nullptr;
	_requestParams		= nullptr;
	_requestVersion		= nullptr;
	_requestPayload		= nullptr;
	_method				= HTTP_ANY;
	_statusCode			= 0;
}




////////////////////////////////////////////////////////////////////////////////
// READ BYTES FROM THE STREAM
////////////////////////////////////////////////////////////////////////////////
HTTPStatus ESP8266WebServer::_readStream(WiFiClient &client) {
	if (millis() - _readTimeout > HTTP_MAX_DATA_WAIT) {
		return HTTP_TIMEOUT;
	}

	//CHECK FOR AVAILABLE BYTES
	auto available	= client.available();
	if (!available) {
		yield();
		return HTTP_OK;
	}

	//RESET READ TIMEOUT BECAUSE WE GOT NEW BYTES
	_readTimeout = millis();

	//INCREASE BUFFER SIZE
	_request = vealloc(_request, _readBytes + available + 1);

	//NO ROOM TO EXPAND BUFFER? OUTPUT ERROR!
	if (!_request) return HTTP_PAYLOAD_LARGE;

	//READ AVAILABLE BYTES INTO OUR BUFFER
	client.read(((unsigned char*)_request) + _readBytes, available);

	//INCREASE VALUE LETTING US KNOW HOW MUCH WE READ
	_readBytes += available;

	//NULL TERMINATE THE BUFFER STRING
	_request[_readBytes] = '\0';

	return HTTP_OK;
}




////////////////////////////////////////////////////////////////////////////////
// PARSE THE FIRST LINE OF THE HEADER
// THIS POPULATES _requestPath, _requestParams, AND _requestVersion
////////////////////////////////////////////////////////////////////////////////
HTTPStatus ESP8266WebServer::_parseVersion(char *buffer) {
	if (!buffer  ||  !*buffer) return HTTP_TEAPOT;

	//SET THE REQUEST METHOD (EG: "GET", "POST", "PUT", ETC)
	_requestMethod = buffer;

	do {
		if (*buffer == ' ') {
			*buffer++			= '\0';

			//SET THE REQUEST PATH (URL)
			if (!_requestPath) {
				_requestPath	= buffer;

			//SET THE REQUEST VERSION (EG: "HTTP/1.1")
			} else if (!_requestVersion) {
				_requestVersion	= buffer;

			} else {
				return HTTP_BAD_REQUEST;
			}

		} else if (*buffer == '?') {
			//SET THE GET REQUEST URL PARAMETERS
			if (_requestPath  &&  !_requestParams  &&  !_requestVersion) {
				*buffer++		= '\0';
				_requestParams	= buffer;
			}

		} else {
			buffer++;
		}
	} while (*buffer);

#	ifdef DEBUG_ESP_HTTP_SERVER
		DEBUG_OUTPUT.print(F("Method: ("));
		DEBUG_OUTPUT.print(_headers.key(0));
		DEBUG_OUTPUT.println(F(")"));

		DEBUG_OUTPUT.print(F("Path: ("));
		DEBUG_OUTPUT.print(_requestPath);
		DEBUG_OUTPUT.println(F(")"));

		DEBUG_OUTPUT.print(F("Parameters: ("));
		DEBUG_OUTPUT.print(_requestParams);
		DEBUG_OUTPUT.println(F(")"));

		DEBUG_OUTPUT.print(F("Version: ("));
		DEBUG_OUTPUT.print(_requestVersion);
		DEBUG_OUTPUT.println(F(")"));
#	endif

	return _requestVersion ? HTTP_OK : HTTP_BAD_REQUEST;
}




////////////////////////////////////////////////////////////////////////////////
// PARSE THE REQUEST METHOD (EG: "GET", "POST", "PUT", ETC)
////////////////////////////////////////////////////////////////////////////////
HTTPMethod ESP8266WebServer::_parseMethod(const char *method) {
	if (!strcasecmp_P(method, PSTR("GET")))		return HTTP_GET;
	if (!strcasecmp_P(method, PSTR("HEAD")))	return HTTP_HEAD;
	if (!strcasecmp_P(method, PSTR("POST")))	return HTTP_POST;
	if (!strcasecmp_P(method, PSTR("PUT")))		return HTTP_PUT;
	if (!strcasecmp_P(method, PSTR("DELETE")))	return HTTP_DELETE;
	if (!strcasecmp_P(method, PSTR("CONNECT")))	return HTTP_CONNECT;
	if (!strcasecmp_P(method, PSTR("OPTIONS")))	return HTTP_OPTIONS;
	if (!strcasecmp_P(method, PSTR("TRACE")))	return HTTP_TRACE;
	if (!strcasecmp_P(method, PSTR("PATCH")))	return HTTP_PATCH;
	return HTTP_ANY;
}




////////////////////////////////////////////////////////////////////////////////
// PARSE THE REQUEST FROM THE CLIENT BROWSER
////////////////////////////////////////////////////////////////////////////////
HTTPStatus ESP8266WebServer::_parseRequest(WiFiClient &client) {

	//RESET ALL REQUEST VARIABLES
	_resetRequest();


	//READ FROM STREAM UNTIL WE HAVE ALL HEADERS
	do {
		auto code = _readStream(client);
		if (code != HTTP_OK) return code;
	} while (!strstr(_request, "\r\n\r\n"));



#	ifdef DEBUG_ESP_HTTP_SERVER
		//DISPLAY RAW REQUEST TO DEBUG OUTPUT (SERIAL)
		DEBUG_OUTPUT.println(F("REQUEST:"));
		DEBUG_OUTPUT.println(_request);
#	endif



	//PROCESS HEADERS
	char	*offset	= _request;
	char	*buffer	= _headers.process(_request);
	int		hlength	= buffer - _request;
	int		clength	= _headers.integer("Content-Length");


	//READ PAYLOAD UNTIL CONTENT LENGTH IS SATISFIED
	while (_readBytes - hlength < clength) {
		auto code = _readStream(client);
		if (code != HTTP_OK) return code;
	}

	//ADJUST THE POINTER OFFSETS IN CASE REALLOC GAVE US A NEW POINT IN RAM
	_headers.__offset(_request - offset);


	//SET THE PAYLOAD BUFFER
	_requestPayload = _request + hlength;


	//PARSE THE FIRST LINE OF THE HEADER
	{
		char *header	= (char*) _headers.key(0);
		auto code		= _parseVersion(header);
		if (code != HTTP_OK) return code;
	}



	//PARSE REQUEST METHOD
	_method = _parseMethod(_requestMethod);



#	ifdef DEBUG_ESP_HTTP_SERVER
		DEBUG_OUTPUT.print(F("Method detected: "));
		DEBUG_OUTPUT.println(_method);
#	endif



	//PARSE URL PARAMETERS
	if (_requestParams) {
		_params.process(_requestParams, true);
	}


	switch (_method) {
		case HTTP_POST:
		case HTTP_PUT:
		case HTTP_PATCH:
		case HTTP_DELETE:
			if (_requestPayload) {
				//TODO: THIS IS BASED ON ENCODING METHOD, CHECK THAT FIRST
				_params.process(_requestPayload, false);
			}
		default:
			break;
	}


	//ATTACH HANDLER
	for (_handler=_firstHandler; _handler; _handler=_handler->next()) {
		if (_handler->canHandle(_method, _requestPath)) {
			break;
		}
	}


	//FLUSH THE CLIENT BUFFER
	client.flush();


	//RETURN HTTP STATUS 200 "OK"
	return HTTP_OK;
}

}; // namespace WebServerDarkain
