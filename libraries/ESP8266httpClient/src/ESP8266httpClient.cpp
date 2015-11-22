/**
 * ESP8266httpClient.cpp
 *
 * Created on: 02.11.2015
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the ESP8266httpClient for Arduino.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include "ESP8266httpClient.h"

httpClient::httpClient() {
    _tcp = NULL;
    _tcps = NULL;

    _headerKeysCount = 0;
    _currentHeaders = NULL;

    _returnCode = 0;
    _size = -1;
}

httpClient::~httpClient() {
    if(connected()) {
        _tcp->stop();
    }

    if(_currentHeaders) {
        delete[] _currentHeaders;
    }
    _headerKeysCount = 0;
}

void httpClient::begin(const char *host, uint16_t port, const char * url, bool https, const char * httpsFingerprint) {

    _host = host;
    _port = port;
    _url = url;
    _https = https;
    _httpsFingerprint = httpsFingerprint;

    _returnCode = 0;
    _size = -1;

    _Headers = "";

}

void httpClient::begin(String host, uint16_t port, String url, bool https, String httpsFingerprint) {
    begin(host.c_str(), port, url.c_str(), https, httpsFingerprint.c_str());
}

/**
 * connected
 * @return connected status
 */
bool httpClient::connected() {
    if(_tcp) {
        return _tcp->connected();
    }
    return false;
}

/**
 * send a GET request
 * @return http code
 */
int httpClient::GET() {
    return sendRequest("GET");
}

/**
 * sends a post request to the server
 * @param payload uint8_t *
 * @param size size_t
 * @return http code
 */
int httpClient::POST(uint8_t * payload, size_t size) {
    return sendRequest("POST", payload, size);
}

int httpClient::POST(String payload) {
    return POST((uint8_t *) payload.c_str(), payload.length());
}

/**
 * sendRequest
 * @param type const char *     "GET", "POST", ....
 * @param payload uint8_t *     data for the message body if null not send
 * @param size size_t           size for the message body if 0 not send
 * @return -1 if no info or > 0 when Content-Length is set by server
 */
int httpClient::sendRequest(const char * type, uint8_t * payload, size_t size) {
    // connect ro server
    if(!connect()) {
        return HTTPC_ERROR_CONNECTION_REFUSED;
    }

    if(payload && size > 0) {
        addHeader("Content-Length", String(size));
    }

    // send Header
    if(!sendHeader(type)) {
        return HTTPC_ERROR_SEND_HEADER_FAILD;
    }

    // send Payload if needed
    if(payload && size > 0) {
        if(_tcp->write(&payload[0], size) != size) {
            return HTTPC_ERROR_SEND_PAYLOAD_FAILD;
        }
    }

    // handle Server Response (Header)
    return handleHeaderResponse();
}

/**
 * size of message body / payload
 * @return -1 if no info or > 0 when Content-Length is set by server
 */
int httpClient::getSize(void) {
    return _size;
}

/**
 * returns the stram of the tcp connection
 * @return WiFiClient
 */
WiFiClient & httpClient::getStream(void) {
    if(connected()) {
        return *_tcp;
    }

    DEBUG_HTTPCLIENT("[HTTP-Client] no stream to return!?\n");

    // todo return error?
}

/**
 * adds Headder to the request
 * @param name
 * @param value
 * @param first
 */
void httpClient::addHeader(const String& name, const String& value, bool first) {

    String headerLine = name;
    headerLine += ": ";
    headerLine += value;
    headerLine += "\r\n";

    if(first) {
        _Headers = headerLine + _Headers;
    } else {
        _Headers += headerLine;
    }
}

void httpClient::collectHeaders(const char* headerKeys[], const size_t headerKeysCount) {
    _headerKeysCount = headerKeysCount;
    if(_currentHeaders)
        delete[] _currentHeaders;
    _currentHeaders = new RequestArgument[_headerKeysCount];
    for(size_t i = 0; i < _headerKeysCount; i++) {
        _currentHeaders[i].key = headerKeys[i];
    }
}

String httpClient::header(const char* name) {
    for(size_t i = 0; i < _headerKeysCount; ++i) {
        if(_currentHeaders[i].key == name)
            return _currentHeaders[i].value;
    }
    return String();
}

String httpClient::header(size_t i) {
    if(i < _headerKeysCount)
        return _currentHeaders[i].value;
    return String();
}

String httpClient::headerName(size_t i) {
    if(i < _headerKeysCount)
        return _currentHeaders[i].key;
    return String();
}

int httpClient::headers() {
    return _headerKeysCount;
}

bool httpClient::hasHeader(const char* name) {
    for(size_t i = 0; i < _headerKeysCount; ++i) {
        if((_currentHeaders[i].key == name) && (_currentHeaders[i].value.length() > 0))
            return true;
    }
    return false;
}

/**
 * init TCP connection and handle ssl verify if needed
 * @return true if connection is ok
 */
bool httpClient::connect(void) {

    if(connected()) {
        DEBUG_HTTPCLIENT("[HTTP-Client] connect. already connected, reuse!\n");
        return true;
    }

    if(_https) {
        DEBUG_HTTPCLIENT("[HTTP-Client] connect https...\n");
        _tcps = new WiFiClientSecure();
        _tcp = _tcps;
    } else {
        DEBUG_HTTPCLIENT("[HTTP-Client] connect...\n");
        _tcp = new WiFiClient();
    }

    if(!_tcp->connect(_host.c_str(), _port)) {
        DEBUG_HTTPCLIENT("[HTTP-Client] failed connect to %s:%u.\n", _host.c_str(), _port);
        return false;
    }

    DEBUG_HTTPCLIENT("[HTTP-Client] connected to %s:%u.\n", _host.c_str(), _port);

    if(_https && _httpsFingerprint.length() > 0) {
        if(_tcps->verify(_httpsFingerprint.c_str(), _host.c_str())) {
            DEBUG_HTTPCLIENT("[HTTP-Client] https certificate matches\n");
        } else {
            DEBUG_HTTPCLIENT("[HTTP-Client] https certificate doesn't match!\n");
            _tcp->stop();
            return false;
        }
    }

    // set Timeout for readBytesUntil and readStringUntil
    _tcp->setTimeout(HTTPCLIENT_TCP_TIMEOUT);

#ifdef ESP8266
    _tcp->setNoDelay(true);
#endif
    return connected();
}

/**
 * sends HTTP request header
 * @param type (GET, POST, ...)
 * @return status
 */
bool httpClient::sendHeader(const char * type) {
    if(!connected()) {
        return false;
    }
    String header = String(type) + " " + _url + " HTTP/1.1\r\n"
            "Host: " + _host + "\r\n"
            "User-Agent: ESP8266httpClient\r\n"
            "Connection: close\r\n" + _Headers + "\r\n";

    return _tcp->write(header.c_str(), header.length());
}

/**
 * reads the respone from the server
 * @return int http code
 */
int httpClient::handleHeaderResponse() {

    if(!connected()) {
        return HTTPC_ERROR_NOT_CONNECTED;
    }

    while(connected()) {
        size_t len = _tcp->available();
        if(len > 0) {
            String headerLine = _tcp->readStringUntil('\n');
            headerLine.trim(); // remove \r

            DEBUG_HTTPCLIENT("[HTTP-Client][handleHeaderResponse] RX: '%s'\n", headerLine.c_str());

            if(headerLine.startsWith("HTTP/1.")) {
                _returnCode = headerLine.substring(9, headerLine.indexOf(' ', 9)).toInt();
            } else if(headerLine.indexOf(':')) {
                String headerName = headerLine.substring(0, headerLine.indexOf(':'));
                String headerValue = headerLine.substring(headerLine.indexOf(':') + 2);

                if(headerName.equalsIgnoreCase("Content-Length")) {
                    _size = headerValue.toInt();
                }

                for(size_t i = 0; i < _headerKeysCount; i++) {
                    if(_currentHeaders[i].key == headerName) {
                        _currentHeaders[i].value = headerValue;
                        break;
                    }
                }

            }

            if(headerLine == "") {
                DEBUG_HTTPCLIENT("[HTTP-Client][handleHeaderResponse] code: '%s'\n", String(_returnCode).c_str());
                if(_size) {
                    DEBUG_HTTPCLIENT("[HTTP-Client][handleHeaderResponse] size: '%s'\n", String(_size).c_str());
                }
                return _returnCode;
            }

        } else {
            delay(0);
        }
    }

    return HTTPC_ERROR_CONNECTION_LOST;
}
