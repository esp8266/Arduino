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
#include <cstring>
#include <libb64/cencode.h>
#include "WiFiServer.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"
#include "FS.h"
#include "detail/mimetable.h"

//#define DEBUG_ESP_HTTP_SERVER
#ifdef DEBUG_ESP_PORT
#define DEBUG_OUTPUT DEBUG_ESP_PORT
#else
#define DEBUG_OUTPUT Serial
#endif

namespace WebServerDarkain {

#include "detail/RequestHandlersImpl.h"

static const char AUTHORIZATION_HEADER[] /*PROGMEM*/ = "Authorization";
static const char qop_auth[] PROGMEM = "qop=auth";
static const char WWW_Authenticate[] PROGMEM = "WWW-Authenticate";
static const char Content_Length[] PROGMEM = "Content-Length";




////////////////////////////////////////////////////////////////////////////////
// INITIALIZE ALL THE THINGS
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::_init() {
	_currentVersion		= 0;
	_currentStatus		= HC_NONE;
	_handler			= nullptr;
	_firstHandler		= nullptr;
	_lastHandler		= nullptr;
	_contentLength		= 0;
	_chunked			= false;

	_resetRequest();
}




////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR (VARIANT 1)
////////////////////////////////////////////////////////////////////////////////
ESP8266WebServer::ESP8266WebServer(IPAddress addr, int port) : _server(addr, port) {
	_init();
}




////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR (VARIANT 2)
////////////////////////////////////////////////////////////////////////////////
ESP8266WebServer::ESP8266WebServer(int port) : _server(port) {
	_init();
}




////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////
ESP8266WebServer::~ESP8266WebServer() {
	_server.close();

	_headers.reset();
	_params.reset();

	RequestHandler* handler = _firstHandler;

	while (handler) {
		RequestHandler* next = handler->next();
		delete handler;
		handler = next;
	}
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::begin() {
	close();
	_server.begin();
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::begin(uint16_t port) {
	close();
	_server.begin(port);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
String ESP8266WebServer::_extractParam(const String& authReq,const String& param,const char delimit) {
	int _begin = authReq.indexOf(param);
	if (_begin == -1) {
		return emptyString;
	}
	return authReq.substring(_begin+param.length(),authReq.indexOf(delimit,_begin+param.length()));
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
bool ESP8266WebServer::authenticate(const char * username, const char * password){
	if (hasHeader((AUTHORIZATION_HEADER))) {
		String authReq = header(FPSTR(AUTHORIZATION_HEADER));
		if (authReq.startsWith(F("Basic"))){
			authReq = authReq.substring(6);
			authReq.trim();
			char toencodeLen = strlen(username)+strlen(password)+1;
			char *toencode = new char[toencodeLen + 1];
			if(toencode == NULL){
				//authReq = emptyString;
				return false;
			}
			char *encoded = new char[base64_encode_expected_len(toencodeLen)+1];
			if(encoded == NULL){
				//authReq = emptyString;
				delete[] toencode;
				return false;
			}
			sprintf(toencode, "%s:%s", username, password);
			if(base64_encode_chars(toencode, toencodeLen, encoded) > 0 && authReq.equalsConstantTime(encoded)) {
				//authReq = emptyString;
				delete[] toencode;
				delete[] encoded;
				return true;
			}
			delete[] toencode;
			delete[] encoded;
		} else if (authReq.startsWith(F("Digest"))) {
			authReq = authReq.substring(7);
#			ifdef DEBUG_ESP_HTTP_SERVER
				DEBUG_OUTPUT.println(authReq);
#			endif
			String _username = _extractParam(authReq,F("username=\""));
			if (!_username.length() || _username != String(username)) {
				//authReq = emptyString;
				return false;
			}
			// extracting required parameters for RFC 2069 simpler Digest
			String _realm    = _extractParam(authReq, F("realm=\""));
			String _nonce    = _extractParam(authReq, F("nonce=\""));
			String _uri      = _extractParam(authReq, F("uri=\""));
			String _response = _extractParam(authReq, F("response=\""));
			String _opaque   = _extractParam(authReq, F("opaque=\""));

			if ((!_realm.length()) || (!_nonce.length()) || (!_uri.length()) || (!_response.length()) || (!_opaque.length())) {
				//authReq = emptyString;
				return false;
			}
			if ((_opaque != _sopaque) || (_nonce != _snonce) || (_realm != _srealm)) {
				//authReq = emptyString;
				return false;
			}
			// parameters for the RFC 2617 newer Digest
			String _nc,_cnonce;
			if (authReq.indexOf(FPSTR(qop_auth)) != -1) {
				_nc = _extractParam(authReq, F("nc="), ',');
				_cnonce = _extractParam(authReq, F("cnonce=\""));
			}
			MD5Builder md5;
			md5.begin();
			md5.add(String(username) + ':' + _realm + ':' + String(password));  // md5 of the user:realm:user
			md5.calculate();
			String _H1 = md5.toString();
#			ifdef DEBUG_ESP_HTTP_SERVER
				DEBUG_OUTPUT.println("Hash of user:realm:pass=" + _H1);
#			endif
			md5.begin();
			if(_method == HTTP_GET){
				md5.add(String(F("GET:")) + _uri);
			}else if(_method == HTTP_POST){
				md5.add(String(F("POST:")) + _uri);
			}else if(_method == HTTP_PUT){
				md5.add(String(F("PUT:")) + _uri);
			}else if(_method == HTTP_DELETE){
				md5.add(String(F("DELETE:")) + _uri);
			}else{
				md5.add(String(F("GET:")) + _uri);
			}
			md5.calculate();
			String _H2 = md5.toString();
#			ifdef DEBUG_ESP_HTTP_SERVER
				DEBUG_OUTPUT.println("Hash of GET:uri=" + _H2);
#			endif
			md5.begin();
			if(authReq.indexOf(FPSTR(qop_auth)) != -1) {
				md5.add(_H1 + ':' + _nonce + ':' + _nc + ':' + _cnonce + F(":auth:") + _H2);
			} else {
				md5.add(_H1 + ':' + _nonce + ':' + _H2);
			}
			md5.calculate();
			String _responsecheck = md5.toString();
#			ifdef DEBUG_ESP_HTTP_SERVER
				DEBUG_OUTPUT.println("The Proper response=" +_responsecheck);
#			endif
			if(_response == _responsecheck){
				//authReq = emptyString;
				return true;
			}
		}
		//authReq = emptyString;
	}
	return false;
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
String ESP8266WebServer::_getRandomHexString() {
	char buffer[33];  // buffer to hold 32 Hex Digit + /0
	int i;
	for(i = 0; i < 4; i++) {
		sprintf (buffer + (i*8), "%08x", RANDOM_REG32);
	}
	return String(buffer);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::requestAuthentication(HTTPAuthMethod mode, const char* realm, const String& authFailMsg) {
	if(realm == NULL) {
		_srealm = String(F("Login Required"));
	} else {
		_srealm = String(realm);
	}

	if(mode == BASIC_AUTH) {
		sendHeader(
			String(FPSTR(WWW_Authenticate)),
			String(F("Basic realm=\"")) + _srealm + String(F("\""))
		);

	} else {
		_snonce=_getRandomHexString();
		_sopaque=_getRandomHexString();

		sendHeader(
			String(FPSTR(WWW_Authenticate)),
			String(
				F("Digest realm=\""))
				+ _srealm
				+ String(F("\", qop=\"auth\", nonce=\""))
				+ _snonce
				+ String(F("\", opaque=\""))
				+ _sopaque
				+ String(F("\"")
			)
		);
	}

	using namespace mime;
	send(HTTP_UNAUTHORIZED, String(FPSTR(mimeTable[html].mimeType)), authFailMsg);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::on(const String &uri, ESP8266WebServer::THandlerFunction handler) {
	on(uri, HTTP_ANY, handler);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::on(const String &uri, HTTPMethod method, ESP8266WebServer::THandlerFunction fn) {
	on(uri, method, fn, _fileUploadHandler);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::on(const String &uri, HTTPMethod method, ESP8266WebServer::THandlerFunction fn, ESP8266WebServer::THandlerFunction ufn) {
	_addRequestHandler(new FunctionRequestHandler(fn, ufn, uri, method));
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::addHandler(RequestHandler* handler) {
	_addRequestHandler(handler);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::_addRequestHandler(RequestHandler* handler) {
	if (!_lastHandler) {
		_firstHandler = handler;
		_lastHandler = handler;
	} else {
		_lastHandler->next(handler);
		_lastHandler = handler;
	}
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::serveStatic(const char* uri, FS& fs, const char* path, const char* cache_header) {
	_addRequestHandler(new StaticRequestHandler(fs, path, uri, cache_header));
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::handleClient() {
	if (_currentStatus == HC_NONE) {
		WiFiClient client = _server.available();
		if (!client) return;

#		ifdef DEBUG_ESP_HTTP_SERVER
			DEBUG_OUTPUT.println("New client");
#		endif

		_currentClient = client;
		_currentStatus = HC_WAIT_READ;
		_statusChange = millis();
	}

	bool keepCurrentClient = false;
	bool callYield = false;

	if (_currentClient.connected()) {
		switch (_currentStatus) {
		case HC_NONE:
			// No-op to avoid C++ compiler warning
			break;

		case HC_WAIT_READ:
			// Wait for data from client to become available
			if (_currentClient.available()) {
				auto status = _parseRequest(_currentClient);
				if (status == HTTP_OK) {
					_currentClient.setTimeout(HTTP_MAX_SEND_WAIT);
					_contentLength = CONTENT_LENGTH_NOT_SET;
					_handleRequest();

					if (_currentClient.connected()) {
						_currentStatus = HC_WAIT_CLOSE;
						_statusChange = millis();
						keepCurrentClient = true;
					}
				} else {
					//SEND ERROR RESPONSE TO CLIENT
					_resetRequest();
					send(status);
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
		_currentClient = WiFiClient();
		_currentStatus = HC_NONE;
		_currentUpload.reset();
	}

	if (callYield) {
		yield();
	}
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::close() {
	_server.close();
	_currentStatus = HC_NONE;
	_resetRequest();
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::stop() {
	close();
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::sendHeader(const String& name, const String& value, bool first) {
	//TODO: CHANGE THIS TO A SINGLE FIXED LENGTH BUFFER
	String headerLine = name;
	headerLine += F(": ");
	headerLine += value;
	headerLine += "\r\n";

	if (first) {
		_responseHeaders = headerLine + _responseHeaders;
	} else {
		_responseHeaders += headerLine;
	}
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::setContentLength(const size_t contentLength) {
	_contentLength = contentLength;
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::_prepareHeader(String& response, HTTPStatus code, const char* content_type, size_t contentLength) {
	response = String(F("HTTP/1.")) + String(_currentVersion) + ' ';
	response += String(code);
	response += ' ';
	response += responseCodeToString(code);
	response += "\r\n";

	using namespace mime;
	if (!content_type) {
		content_type = mimeTable[html].mimeType;
	}

	sendHeader(String(F("Content-Type")), String(FPSTR(content_type)), true);
	if (_contentLength == CONTENT_LENGTH_NOT_SET) {
		sendHeader(String(FPSTR(Content_Length)), String(contentLength));

	} else if (_contentLength != CONTENT_LENGTH_UNKNOWN) {
		sendHeader(String(FPSTR(Content_Length)), String(_contentLength));

	} else if(_contentLength == CONTENT_LENGTH_UNKNOWN && _currentVersion) { //HTTP/1.1 or above client
		//let's do chunked
		_chunked = true;
		sendHeader(String(F("Accept-Ranges")),String(F("none")));
		sendHeader(String(F("Transfer-Encoding")),String(F("chunked")));
	}

	sendHeader(String(F("Connection")), String(F("close")));

	response += _responseHeaders;
	response += "\r\n";
	_responseHeaders = emptyString;
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::send(HTTPStatus code, const char* content_type, const String& content) {
	String header;
	// Can we asume the following?
	//if(code == HTTP_OK && content.length() == 0 && _contentLength == CONTENT_LENGTH_NOT_SET)
	//  _contentLength = CONTENT_LENGTH_UNKNOWN;
	_prepareHeader(header, code, content_type, content.length());
	_currentClientWrite(header.c_str(), header.length());
	if(content.length()) {
		sendContent(content);
	}
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::send_P(HTTPStatus code, PGM_P content_type, PGM_P content) {
	size_t contentLength = 0;

	if (content != NULL) {
		contentLength = strlen_P(content);
	}

	String header;
	char type[64];
	memccpy_P((void*)type, (PGM_VOID_P)content_type, 0, sizeof(type));
	_prepareHeader(header, code, (const char* )type, contentLength);
	_currentClientWrite(header.c_str(), header.length());
	sendContent_P(content);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::send_P(HTTPStatus code, PGM_P content_type, PGM_P content, size_t contentLength) {
	String header;
	char type[64];
	memccpy_P((void*)type, (PGM_VOID_P)content_type, 0, sizeof(type));
	_prepareHeader(header, code, (const char* )type, contentLength);
	sendContent(header);
	sendContent_P(content, contentLength);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::send(HTTPStatus code, char *content_type, const String &content) {
	send(code, (const char*)content_type, content);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::send(HTTPStatus code, const String &content_type, const String &content) {
	send(code, (const char*)content_type.c_str(), content);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::sendContent(const String& content) {
	const char *footer = "\r\n";
	size_t len = content.length();
	if (_chunked) {
		char * chunkSize = (char *)malloc(11);
		if (chunkSize) {
			sprintf(chunkSize, "%x%s", len, footer);
			_currentClientWrite(chunkSize, strlen(chunkSize));
			free(chunkSize);
		}
	}
	_currentClientWrite(content.c_str(), len);
	if (_chunked) {
		_currentClient.write(footer, 2);
		if (len == 0) {
			_chunked = false;
		}
	}
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::sendContent_P(PGM_P content) {
	sendContent_P(content, strlen_P(content));
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::sendContent_P(PGM_P content, size_t size) {
	const char *footer = "\r\n";
	if (_chunked) {
		char *chunkSize = (char*) malloc(11);
		if (chunkSize) {
			sprintf(chunkSize, "%x%s", size, footer);
			_currentClientWrite(chunkSize, strlen(chunkSize));
			free(chunkSize);
		}
	}
	_currentClientWrite_P(content, size);
	if (_chunked) {
		_currentClient.write(footer, 2);
		if (size == 0) {
			_chunked = false;
		}
	}
}





////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::_streamFileCore(const size_t fileSize, const String & fileName, const String & contentType) {
	using namespace mime;
	setContentLength(fileSize);
	if (fileName.endsWith(String(FPSTR(mimeTable[gz].endsWith))) &&
			contentType != String(FPSTR(mimeTable[gz].mimeType)) &&
			contentType != String(FPSTR(mimeTable[none].mimeType))) {
		sendHeader(F("Content-Encoding"), F("gzip"));
	}
	send(HTTP_OK, contentType, emptyString);
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::onFileUpload(THandlerFunction fn) {
	_fileUploadHandler = fn;
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::onNotFound(THandlerFunction fn) {
	_notFoundHandler = fn;
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::_handleRequest() {
	bool handled = false;
	if (!_handler){
#		ifdef DEBUG_ESP_HTTP_SERVER
			DEBUG_OUTPUT.println("request handler not found");
#		endif
	}
	else {
		handled = _handler->handle(*this, _method, _requestPath);
#		ifdef DEBUG_ESP_HTTP_SERVER
			if (!handled) {
				DEBUG_OUTPUT.println("request handler failed to handle request");
			}
#		endif
	}
	if (!handled && _notFoundHandler) {
		_notFoundHandler();
		handled = true;
	}
	if (!handled) {
		using namespace mime;
		send(HTTP_NOT_FOUND, String(FPSTR(mimeTable[html].mimeType)), String(F("Not found: ")) + _requestPath);
		handled = true;
	}
	if (handled) {
		_finalizeResponse();
	}
	_requestPath = emptyString.c_str();
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
void ESP8266WebServer::_finalizeResponse() {
	if (_chunked) {
		sendContent(emptyString);
	}
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
String ESP8266WebServer::responseCodeToString(HTTPStatus code) {
	switch (code) {
		case HTTP_CONTINUE:				return F("Continue");
		case HTTP_SWITCH_PROTO:			return F("Switching Protocols");
		case HTTP_OK:					return F("OK");
		case HTTP_CREATED:				return F("Created");
		case HTTP_ACCEPTED:				return F("Accepted");
		case HTTP_NON_AUTH_INFO:		return F("Non-Authoritative Information");
		case HTTP_NO_CONTENT:			return F("No Content");
		case HTTP_RESET_CONTENT:		return F("Reset Content");
		case HTTP_PARTIAL_CONTENT:		return F("Partial Content");
		case HTTP_IM_USED:				return F("IM Used");
		case HTTP_MULTIPLE_CHOICE:		return F("Multiple Choices");
		case HTTP_MOVED_PERM:			return F("Moved Permanently");
		case HTTP_FOUND:				return F("Found");
		case HTTP_SEE_OTHER:			return F("See Other");
		case HTTP_NOT_MODIFIED:			return F("Not Modified");
		case HTTP_USE_PROXY:			return F("Use Proxy");
		case HTTP_SWITCH_PROXY:			return F("Switch Proxy");
		case HTTP_TEMP_REDIRECT:		return F("Temporary Redirect");
		case HTTP_PERM_REDIRECT:		return F("Permanent Redirect");
		case HTTP_BAD_REQUEST:			return F("Bad Request");
		case HTTP_UNAUTHORIZED:			return F("Unauthorized");
		case HTTP_PAYMENT_REQ:			return F("Payment Required");
		case HTTP_FORBIDDEN:			return F("Forbidden");
		case HTTP_NOT_FOUND:			return F("Not Found");
		case HTTP_NOT_ALLOWED:			return F("Method Not Allowed");
		case HTTP_NOT_ACCEPTABLE:		return F("Not Acceptable");
		case HTTP_PROXY_AUTH_REQ:		return F("Proxy Authentication Required");
		case HTTP_TIMEOUT:				return F("Request Timeout");
		case HTTP_CONFLICT:				return F("Conflict");
		case HTTP_GONE:					return F("Gone");
		case HTTP_LENGTH_REQ:			return F("Length Required");
		case HTTP_PRECON_FAIL:			return F("Precondition Failed");
		case HTTP_PAYLOAD_LARGE:		return F("Payload Too Large");
		case HTTP_URI_LARGE:			return F("URI Too Long");
		case HTTP_UNSUPPORT_MEDIA:		return F("Unsupported Media Type");
		case HTTP_RANGE_ERROR:			return F("Range Not Satisfiable");
		case HTTP_EXPECATION_FAIL:		return F("Expectation Failed");
		case HTTP_TEAPOT:				return F("I'm a teapot");
		case HTTP_MISDIRECT_REQUEST:	return F("Misdirected Request");
		case HTTP_TOO_MANY_REQUEST:		return F("Too Many Requests");
		case HTTP_REQUEST_LARGE:		return F("Request Header Fields Too Large");
		case HTTP_SERVER_ERROR:			return F("Internal Server Error");
		case HTTP_NOT_IMPLEMENTED:		return F("Not Implemented");
		case HTTP_BAD_GATEWAY:			return F("Bad Gateway");
		case HTTP_UNAVAILABLE:			return F("Service Unavailable");
		case HTTP_GATEWAY_TIMEOUT:		return F("Gateway Time-out");
		case HTTP_VERSION_UNSUPPORTED:	return F("HTTP Version not supported");
	}

	return emptyString;
}

}; // namespace WebServerDarkain
