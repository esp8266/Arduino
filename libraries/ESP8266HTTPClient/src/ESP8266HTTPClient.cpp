/**
 * ESP8266HTTPClient.cpp
 *
 * Created on: 02.11.2015
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the ESP8266HTTPClient for Arduino.
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
#include <StreamString.h>

#include "ESP8266HTTPClient.h"

/**
 * constractor
 */
HTTPClient::HTTPClient() {
    _tcp = NULL;
    _tcps = NULL;

    _port = 0;

    _reuse = false;
    _https = false;

    _userAgent = "ESP8266HTTPClient";

    _headerKeysCount = 0;
    _currentHeaders = NULL;

    _returnCode = 0;
    _size = -1;
    _canReuse = false;

}

/**
 * deconstractor
 */
HTTPClient::~HTTPClient() {

    if(_tcps) {
        _tcps->stop();
        delete _tcps;
        _tcps = NULL;
        _tcp = NULL;
    } else if(_tcp) {
        _tcp->stop();
        delete _tcp;
        _tcp = NULL;
    }

    if(_currentHeaders) {
        delete[] _currentHeaders;
    }
}

/**
 * phasing the url for all needed informations
 * @param url const char *
 * @param httpsFingerprint const char *
 */
void HTTPClient::begin(const char *url, const char * httpsFingerprint) {
    begin(String(url), String(httpsFingerprint));
}

/**
 * phasing the url for all needed informations
 * @param url String
 * @param httpsFingerprint String
 */
void HTTPClient::begin(String url, String httpsFingerprint) {

    DEBUG_HTTPCLIENT("[HTTP-Client][begin] url: %s\n", url.c_str());

    _httpsFingerprint = httpsFingerprint;
    _returnCode = 0;
    _size = -1;

    _Headers = "";

    String protocol;
    // check for : (http: or https:
    int index = url.indexOf(':');
    int index2;
    bool hasPort = false;
    if(index) {
        protocol = url.substring(0, index);
        url.remove(0, (index + 3)); // remove http:// or https://

        index = url.indexOf(':');
        index2 = url.indexOf('/');

        if(index >= 0 && ((index2 >= 0 && index < index2) || index2 == 0)) { // do we have a port?
            _host = url.substring(0, index); // hostname
            url.remove(0, (index + 1)); // remove hostname + :

            index = url.indexOf('/');
            _port = url.substring(0, index).toInt(); // get port
            url.remove(0, index); // remove port
            hasPort = true;
        } else {
            index = index2;
            _host = url.substring(0, index);
            url.remove(0, index); // remove hostname
        }

        _url = url;

        if(protocol.equalsIgnoreCase("http")) {
            _https = false;
            if(!hasPort) {
                _port = 80;
            }
        } else if(protocol.equalsIgnoreCase("https")) {
            _https = true;
            if(!hasPort) {
                _port = 443;
            }
        } else {
            DEBUG_HTTPCLIENT("[HTTP-Client][begin] protocol: %s unknown?!\n", protocol.c_str());
            return;
        }
    }

    DEBUG_HTTPCLIENT("[HTTP-Client][begin] host: %s port: %d url: %s https: %d httpsFingerprint: %s\n", _host.c_str(), _port, _url.c_str(), _https, _httpsFingerprint.c_str());

}

/**
 * begin
 * @param host const char *
 * @param port uint16_t
 * @param url  const char *
 * @param https bool
 * @param httpsFingerprint const char *
 */
void HTTPClient::begin(const char *host, uint16_t port, const char * url, bool https, const char * httpsFingerprint) {

    DEBUG_HTTPCLIENT("[HTTP-Client][begin] host: %s port:%d url: %s https: %d httpsFingerprint: %s\n", host, port, url, https, httpsFingerprint);

    _host = host;
    _port = port;
    _url = url;
    _https = https;
    _httpsFingerprint = httpsFingerprint;

    _returnCode = 0;
    _size = -1;

    _Headers = "";

}

void HTTPClient::begin(String host, uint16_t port, String url, bool https, String httpsFingerprint) {
    begin(host.c_str(), port, url.c_str(), https, httpsFingerprint.c_str());
}

/**
 * end
 * called after the payload is handled
 */
void HTTPClient::end(void) {
    if(connected()) {
        if(_reuse && _canReuse) {
            DEBUG_HTTPCLIENT("[HTTP-Client][end] tcp keep open for reuse\n");
        } else {
            DEBUG_HTTPCLIENT("[HTTP-Client][end] tcp stop\n");
            _tcp->stop();
        }
    } else {
        DEBUG_HTTPCLIENT("[HTTP-Client][end] tcp is closed\n");
    }
}

/**
 * connected
 * @return connected status
 */
bool HTTPClient::connected() {
    if(_tcp) {
        return (_tcp->connected() || (_tcp->available() > 0));
    }
    return false;
}

/**
 * try to reuse the connection to the server
 * keep-alive
 * @param reuse bool
 */
void HTTPClient::setReuse(bool reuse) {
    _reuse = reuse;
}

/**
 * set User Agent
 * @param userAgent const char *
 */
void HTTPClient::setUserAgent(const char * userAgent) {
    _userAgent = userAgent;
}

/**
 * send a GET request
 * @return http code
 */
int HTTPClient::GET() {
    return sendRequest("GET");
}

/**
 * sends a post request to the server
 * @param payload uint8_t *
 * @param size size_t
 * @return http code
 */
int HTTPClient::POST(uint8_t * payload, size_t size) {
    return sendRequest("POST", payload, size);
}

int HTTPClient::POST(String payload) {
    return POST((uint8_t *) payload.c_str(), payload.length());
}

/**
 * sendRequest
 * @param type const char *     "GET", "POST", ....
 * @param payload uint8_t *     data for the message body if null not send
 * @param size size_t           size for the message body if 0 not send
 * @return -1 if no info or > 0 when Content-Length is set by server
 */
int HTTPClient::sendRequest(const char * type, uint8_t * payload, size_t size) {
    // connect to server
    if(!connect()) {
        return HTTPC_ERROR_CONNECTION_REFUSED;
    }

    if(payload && size > 0) {
        addHeader("Content-Length", String(size));
    }

    // send Header
    if(!sendHeader(type)) {
        return HTTPC_ERROR_SEND_HEADER_FAILED;
    }

    // send Payload if needed
    if(payload && size > 0) {
        if(_tcp->write(&payload[0], size) != size) {
            return HTTPC_ERROR_SEND_PAYLOAD_FAILED;
        }
    }

    // handle Server Response (Header)
    return handleHeaderResponse();
}

/**
 * sendRequest
 * @param type const char *     "GET", "POST", ....
 * @param stream Stream *       data stream for the message body
 * @param size size_t           size for the message body if 0 not Content-Length is send
 * @return -1 if no info or > 0 when Content-Length is set by server
 */
int HTTPClient::sendRequest(const char * type, Stream * stream, size_t size) {

    if(!stream) {
        return HTTPC_ERROR_NO_STREAM;
    }

    // connect to server
    if(!connect()) {
        return HTTPC_ERROR_CONNECTION_REFUSED;
    }

    if(size > 0) {
        addHeader("Content-Length", String(size));
    }

    // send Header
    if(!sendHeader(type)) {
        return HTTPC_ERROR_SEND_HEADER_FAILED;
    }

    // create buffer for read
    uint8_t buff[1460] = { 0 };

    int len = size;
    int bytesWritten = 0;

    if(len == 0) {
        len = -1;
    }

    // read all data from stream and send it to server
    while(connected() && stream->available() && (len > 0 || len == -1)) {

        // get available data size
        size_t s = stream->available();

        if(s) {
            int c = stream->readBytes(buff, ((s > sizeof(buff)) ? sizeof(buff) : s));

            // write it to Stream
            bytesWritten += _tcp->write((const uint8_t *)buff, c);

            if(len > 0) {
                len -= c;
            }

            delay(0);
        } else {
            delay(1);
        }
    }

    if(size && (int)size != bytesWritten) {
        DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] Stream payload bytesWritten %d and size %d mismatch!.\n", bytesWritten, _size);
        DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] ERROR SEND PAYLOAD FAILED!");
        return HTTPC_ERROR_SEND_PAYLOAD_FAILED;
    } else {
        DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] Stream payload written: %d\n", bytesWritten);
    }

    // handle Server Response (Header)
    return handleHeaderResponse();
}

/**
 * size of message body / payload
 * @return -1 if no info or > 0 when Content-Length is set by server
 */
int HTTPClient::getSize(void) {
    return _size;
}

/**
 * deprecated Note: this is not working with https!
 * returns the stream of the tcp connection
 * @return WiFiClient
 */
WiFiClient & HTTPClient::getStream(void) {
    if(connected()) {
        return *_tcp;
    }

    DEBUG_HTTPCLIENT("[HTTP-Client] no stream to return!?\n");

    // todo return error?
}

/**
 * returns the stream of the tcp connection
 * @return WiFiClient *
 */
WiFiClient * HTTPClient::getStreamPtr(void) {
    if(connected()) {
        return _tcp;
    }

    DEBUG_HTTPCLIENT("[HTTP-Client] no stream to return!?\n");
    return NULL;
}

/**
 * write all  message body / payload to Stream
 * @param stream Stream *
 * @return bytes written ( negative values are error codes )
 */
int HTTPClient::writeToStream(Stream * stream) {

    if(!stream) {
        return HTTPC_ERROR_NO_STREAM;
    }

    if(!connected()) {
        return HTTPC_ERROR_NOT_CONNECTED;
    }

    // get length of document (is -1 when Server sends no Content-Length header)
    int len = _size;
    int bytesWritten = 0;

    // create buffer for read
    uint8_t buff[1460] = { 0 };

    // read all data from server
    while(connected() && (len > 0 || len == -1)) {

        // get available data size
        size_t size = _tcp->available();

        if(size) {
            int c = _tcp->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

            // write it to Stream
            bytesWritten += stream->write(buff, c);

            if(len > 0) {
                len -= c;
            }

            delay(0);
        } else {
            delay(1);
        }
    }

    DEBUG_HTTPCLIENT("[HTTP-Client][writeToStream] connection closed or file end (written: %d).\n", bytesWritten);

    if(_size && _size != bytesWritten) {
        DEBUG_HTTPCLIENT("[HTTP-Client][writeToStream] bytesWritten %d and size %d mismatch!.\n", bytesWritten, _size);
    }

    end();
    return bytesWritten;
}

/**
 * return all payload as String (may need lot of ram or trigger out of memory!)
 * @return String
 */
String HTTPClient::getString(void) {
    StreamString sstring;

    if(_size) {
        // try to reserve needed memmory
        if(!sstring.reserve((_size + 1))) {
            DEBUG_HTTPCLIENT("[HTTP-Client][getString] too less memory to reserve as string! need: %d\n", (_size + 1));
            return String("--too less memory--");
        }
    }

    writeToStream(&sstring);
    return sstring;
}

/**
 * adds Header to the request
 * @param name
 * @param value
 * @param first
 */
void HTTPClient::addHeader(const String& name, const String& value, bool first) {

    // not allow set of Header handled by code
    if(!name.equalsIgnoreCase("Connection") && !name.equalsIgnoreCase("User-Agent") && !name.equalsIgnoreCase("Host")) {
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

}

void HTTPClient::collectHeaders(const char* headerKeys[], const size_t headerKeysCount) {
    _headerKeysCount = headerKeysCount;
    if(_currentHeaders)
        delete[] _currentHeaders;
    _currentHeaders = new RequestArgument[_headerKeysCount];
    for(size_t i = 0; i < _headerKeysCount; i++) {
        _currentHeaders[i].key = headerKeys[i];
    }
}

String HTTPClient::header(const char* name) {
    for(size_t i = 0; i < _headerKeysCount; ++i) {
        if(_currentHeaders[i].key == name)
            return _currentHeaders[i].value;
    }
    return String();
}

String HTTPClient::header(size_t i) {
    if(i < _headerKeysCount)
        return _currentHeaders[i].value;
    return String();
}

String HTTPClient::headerName(size_t i) {
    if(i < _headerKeysCount)
        return _currentHeaders[i].key;
    return String();
}

int HTTPClient::headers() {
    return _headerKeysCount;
}

bool HTTPClient::hasHeader(const char* name) {
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
bool HTTPClient::connect(void) {

    if(connected()) {
        DEBUG_HTTPCLIENT("[HTTP-Client] connect. already connected, try reuse!\n");
        return true;
    }

    if(_https) {
        DEBUG_HTTPCLIENT("[HTTP-Client] connect https...\n");
        if(_tcps) {
            delete _tcps;
            _tcps = NULL;
            _tcp = NULL;
        }
        _tcps = new WiFiClientSecure();
        _tcp = _tcps;
    } else {
        DEBUG_HTTPCLIENT("[HTTP-Client] connect http...\n");
        if(_tcp) {
            delete _tcp;
            _tcp = NULL;
        }
        _tcp = new WiFiClient();
    }

    if(!_tcp->connect(_host.c_str(), _port)) {
        DEBUG_HTTPCLIENT("[HTTP-Client] failed connect to %s:%u\n", _host.c_str(), _port);
        return false;
    }

    DEBUG_HTTPCLIENT("[HTTP-Client] connected to %s:%u\n", _host.c_str(), _port);

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
bool HTTPClient::sendHeader(const char * type) {
    if(!connected()) {
        return false;
    }

    String header = String(type) + " " + _url + " HTTP/1.1\r\n"
            "Host: " + _host + "\r\n"
            "User-Agent: " + _userAgent + "\r\n"
            "Connection: ";

    if(_reuse) {
        header += "keep-alive";
    } else {
        header += "close";
    }
    header += "\r\n" + _Headers + "\r\n";

    return (_tcp->write(header.c_str(), header.length()) == header.length());
}

/**
 * reads the response from the server
 * @return int http code
 */
int HTTPClient::handleHeaderResponse() {

    if(!connected()) {
        return HTTPC_ERROR_NOT_CONNECTED;
    }

    _returnCode = -1;
    _size = -1;

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

                if(headerName.equalsIgnoreCase("Connection")) {
                    _canReuse = headerValue.equalsIgnoreCase("keep-alive");
                }

                for(size_t i = 0; i < _headerKeysCount; i++) {
                    if(_currentHeaders[i].key.equalsIgnoreCase(headerName)) {
                        _currentHeaders[i].value = headerValue;
                        break;
                    }
                }
            }

            if(headerLine == "") {
                DEBUG_HTTPCLIENT("[HTTP-Client][handleHeaderResponse] code: %d\n", _returnCode);
                if(_size) {
                    DEBUG_HTTPCLIENT("[HTTP-Client][handleHeaderResponse] size: %d\n", _size);
                }
                if(_returnCode) {
                    return _returnCode;
                } else {
                    DEBUG_HTTPCLIENT("[HTTP-Client][handleHeaderResponse] Remote host is not an HTTP Server!");
                    return HTTPC_ERROR_NO_HTTP_SERVER;
                }
            }

        } else {
            delay(0);
        }
    }

    return HTTPC_ERROR_CONNECTION_LOST;
}
