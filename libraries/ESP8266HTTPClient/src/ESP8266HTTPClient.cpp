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
#include <base64.h>

#include "ESP8266HTTPClient.h"

/**
 * constractor
 */
HTTPClient::HTTPClient() {
    _tcp = NULL;
    _tcps = NULL;

    _port = 0;

    _reuse = false;
    _tcpTimeout = HTTPCLIENT_DEFAULT_TCP_TIMEOUT;
    _useHTTP10 = false;

    _https = false;

    _userAgent = "ESP8266HTTPClient";

    _headerKeysCount = 0;
    _currentHeaders = NULL;

    _returnCode = 0;
    _size = -1;
    _canReuse = false;
    _transferEncoding = HTTPC_TE_IDENTITY;

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
    //int index2;
    bool hasPort = false;
    if(index >= 0) {
        protocol = url.substring(0, index);
        url.remove(0, (index + 3)); // remove http:// or https://

        index = url.indexOf('/');
        String host = url.substring(0, index);
        url.remove(0, index); // remove host part

        // get Authorization
        index = host.indexOf('@');
        if(index >= 0) {
            // auth info
            String auth = host.substring(0, index);
            host.remove(0, index + 1); // remove auth part including @
            _base64Authorization = base64::encode(auth);
        }

        // get port
        index = host.indexOf(':');
        if(index >= 0) {
            _host = host.substring(0, index); // hostname
            host.remove(0, (index + 1)); // remove hostname + :
            _port = host.toInt(); // get port
            hasPort = true;
        } else {
            _host = host;
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
        if(_tcp->available() > 0) {
            DEBUG_HTTPCLIENT("[HTTP-Client][end] still data in buffer (%d), clean up.\n", _tcp->available());
            while(_tcp->available() > 0) {
                _tcp->read();
            }
        }
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
 * set the Authorizatio for the http request
 * @param user const char *
 * @param password const char *
 */
void HTTPClient::setAuthorization(const char * user, const char * password) {
    if(user && password) {
        String auth = user;
        auth += ":";
        auth += password;
        _base64Authorization = base64::encode(auth);
    }
}

/**
 * set the Authorizatio for the http request
 * @param auth const char * base64
 */
void HTTPClient::setAuthorization(const char * auth) {
    if(auth) {
        _base64Authorization = auth;
    }
}

/**
 * set the timeout for the TCP connection
 * @param timeout unsigned int
 */
void HTTPClient::setTimeout(uint16_t timeout) {
    _tcpTimeout = timeout;
    if(connected()) {
        _tcp->setTimeout(timeout);
    }
}

/**
 * use HTTP1.0
 * @param timeout
 */
void HTTPClient::useHTTP10(bool useHTTP10) {
    _useHTTP10 = useHTTP10;
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
 * @param payload String        data for the message body
 * @return
 */
int HTTPClient::sendRequest(const char * type, String payload) {
    return sendRequest(type, (uint8_t *) payload.c_str(), payload.length());
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
        return returnError(HTTPC_ERROR_CONNECTION_REFUSED);
    }

    if(payload && size > 0) {
        addHeader("Content-Length", String(size));
    }

    // send Header
    if(!sendHeader(type)) {
        return returnError(HTTPC_ERROR_SEND_HEADER_FAILED);
    }

    // send Payload if needed
    if(payload && size > 0) {
        if(_tcp->write(&payload[0], size) != size) {
            return returnError(HTTPC_ERROR_SEND_PAYLOAD_FAILED);
        }
    }

    // handle Server Response (Header)
    return returnError(handleHeaderResponse());
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
        return returnError(HTTPC_ERROR_NO_STREAM);
    }

    // connect to server
    if(!connect()) {
        return returnError(HTTPC_ERROR_CONNECTION_REFUSED);
    }

    if(size > 0) {
        addHeader("Content-Length", String(size));
    }

    // send Header
    if(!sendHeader(type)) {
        return returnError(HTTPC_ERROR_SEND_HEADER_FAILED);
    }

    int buff_size = HTTP_TCP_BUFFER_SIZE;

    int len = size;
    int bytesWritten = 0;

    if(len == 0) {
        len = -1;
    }

    // if possible create smaller buffer then HTTP_TCP_BUFFER_SIZE
    if((len > 0) && (len < HTTP_TCP_BUFFER_SIZE)) {
        buff_size = len;
    }

    // create buffer for read
    uint8_t * buff = (uint8_t *) malloc(buff_size);

    if(buff) {
        // read all data from stream and send it to server
        while(connected() && (stream->available() > -1) && (len > 0 || len == -1)) {

            // get available data size
            int sizeAvailable = stream->available();

            if(sizeAvailable) {

                int readBytes = sizeAvailable;

                // read only the asked bytes
                if(len > 0 && readBytes > len) {
                    readBytes = len;
                }

                // not read more the buffer can handle
                if(readBytes > buff_size) {
                    readBytes = buff_size;
                }

                // read data
                int bytesRead = stream->readBytes(buff, readBytes);

                // write it to Stream
                int bytesWrite = _tcp->write((const uint8_t *) buff, bytesRead);
                bytesWritten += bytesWrite;

                // are all Bytes a writen to stream ?
                if(bytesWrite != bytesRead) {
                    DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] short write, asked for %d but got %d retry...\n", bytesRead, bytesWrite);

                    // check for write error
                    if(_tcp->getWriteError()) {
                        DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] stream write error %d\n", _tcp->getWriteError());

                        //reset write error for retry
                        _tcp->clearWriteError();
                    }

                    // some time for the stream
                    delay(1);

                    int leftBytes = (readBytes - bytesWrite);

                    // retry to send the missed bytes
                    bytesWrite = _tcp->write((const uint8_t *) (buff + bytesWrite), leftBytes);
                    bytesWritten += bytesWrite;

                    if(bytesWrite != leftBytes) {
                        // failed again
                        DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] short write, asked for %d but got %d failed.\n", leftBytes, bytesWrite);
                        free(buff);
                        return returnError(HTTPC_ERROR_SEND_PAYLOAD_FAILED);
                    }
                }

                // check for write error
                if(_tcp->getWriteError()) {
                    DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] stream write error %d\n", _tcp->getWriteError());
                    free(buff);
                    return returnError(HTTPC_ERROR_SEND_PAYLOAD_FAILED);
                }

                // count bytes to read left
                if(len > 0) {
                    len -= readBytes;
                }

                delay(0);
            } else {
                delay(1);
            }
        }

        free(buff);

        if(size && (int) size != bytesWritten) {
            DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] Stream payload bytesWritten %d and size %d mismatch!.\n", bytesWritten, size); DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] ERROR SEND PAYLOAD FAILED!");
            return returnError(HTTPC_ERROR_SEND_PAYLOAD_FAILED);
        } else {
            DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] Stream payload written: %d\n", bytesWritten);
        }

    } else {
        DEBUG_HTTPCLIENT("[HTTP-Client][sendRequest] too less ram! need %d\n", HTTP_TCP_BUFFER_SIZE);
        return returnError(HTTPC_ERROR_TOO_LESS_RAM);
    }

    // handle Server Response (Header)
    return returnError(handleHeaderResponse());
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
        return returnError(HTTPC_ERROR_NO_STREAM);
    }

    if(!connected()) {
        return returnError(HTTPC_ERROR_NOT_CONNECTED);
    }

    // get length of document (is -1 when Server sends no Content-Length header)
    int len = _size;
    int ret = 0;

    if(_transferEncoding == HTTPC_TE_IDENTITY) {
        ret = writeToStreamDataBlock(stream, len);

        // have we an error?
        if(ret < 0) {
            return returnError(ret);
        }
    } else if(_transferEncoding == HTTPC_TE_CHUNKED) {
        int size = 0;
        while(1) {
            if(!connected()) {
                return returnError(HTTPC_ERROR_CONNECTION_LOST);
            }
            String chunkHeader = _tcp->readStringUntil('\n');

            if(chunkHeader.length() <= 0) {
                return returnError(HTTPC_ERROR_READ_TIMEOUT);
            }

            chunkHeader.trim(); // remove \r

            // read size of chunk
            len = (uint32_t) strtol((const char *) chunkHeader.c_str(), NULL, 16);
            size += len;
            DEBUG_HTTPCLIENT("[HTTP-Client] read chunk len: %d\n", len);

            // data left?
            if(len > 0) {
                int r = writeToStreamDataBlock(stream, len);
                if(r < 0) {
                    // error in writeToStreamDataBlock
                    return returnError(r);
                }
                ret += r;
            } else {

                // if no length Header use global chunk size
                if(_size <= 0) {
                    _size = size;
                }

                // check if we have write all data out
                if(ret != _size) {
                    return returnError(HTTPC_ERROR_STREAM_WRITE);
                }
                break;
            }

            delay(0);
        }
    } else {
        return returnError(HTTPC_ERROR_ENCODING);
    }

    end();
    return ret;
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
 * converts error code to String
 * @param error int
 * @return String
 */
String HTTPClient::errorToString(int error) {
    switch(error) {
        case HTTPC_ERROR_CONNECTION_REFUSED:
            return String("connection refused");
        case HTTPC_ERROR_SEND_HEADER_FAILED:
            return String("send header failed");
        case HTTPC_ERROR_SEND_PAYLOAD_FAILED:
            return String("send payload failed");
        case HTTPC_ERROR_NOT_CONNECTED:
            return String("not connected");
        case HTTPC_ERROR_CONNECTION_LOST:
            return String("connection lost");
        case HTTPC_ERROR_NO_STREAM:
            return String("no stream");
        case HTTPC_ERROR_NO_HTTP_SERVER:
            return String("no HTTP server");
        case HTTPC_ERROR_TOO_LESS_RAM:
            return String("too less ram");
        case HTTPC_ERROR_ENCODING:
            return String("Transfer-Encoding not supported");
        case HTTPC_ERROR_STREAM_WRITE:
            return String("Stream write error");
        case HTTPC_ERROR_READ_TIMEOUT:
            return String("read Timeout");
        default:
            return String();
    }
}

/**
 * adds Header to the request
 * @param name
 * @param value
 * @param first
 */
void HTTPClient::addHeader(const String& name, const String& value, bool first) {

    // not allow set of Header handled by code
    if(!name.equalsIgnoreCase("Connection") && !name.equalsIgnoreCase("User-Agent") && !name.equalsIgnoreCase("Host") && !(_base64Authorization.length() && name.equalsIgnoreCase("Authorization"))) {
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
        while(_tcp->available() > 0) {
            _tcp->read();
        }
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
    _tcp->setTimeout(_tcpTimeout);

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

    String header = String(type) + " " + _url + " HTTP/1.";

    if(_useHTTP10) {
        header += "0";
    } else {
        header += "1";
    }

    header += "\r\n"
            "Host: " + _host + "\r\n"
            "User-Agent: " + _userAgent + "\r\n"
            "Connection: ";

    if(_reuse) {
        header += "keep-alive";
    } else {
        header += "close";
    }
    header += "\r\n";

    if(!_useHTTP10) {
        header += "Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0\r\n";
    }

    if(_base64Authorization.length()) {
        header += "Authorization: Basic " + _base64Authorization + "\r\n";
    }

    header += _Headers + "\r\n";

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

    String transferEncoding;
    _returnCode = -1;
    _size = -1;
    _transferEncoding = HTTPC_TE_IDENTITY;
    unsigned long lastDataTime = millis();

    while(connected()) {
        size_t len = _tcp->available();
        if(len > 0) {
            String headerLine = _tcp->readStringUntil('\n');
            headerLine.trim(); // remove \r

            lastDataTime = millis();

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

                if(headerName.equalsIgnoreCase("Transfer-Encoding")) {
                    transferEncoding = headerValue;
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

                if(_size > 0) {
                    DEBUG_HTTPCLIENT("[HTTP-Client][handleHeaderResponse] size: %d\n", _size);
                }

                if(transferEncoding.length() > 0) {
                    DEBUG_HTTPCLIENT("[HTTP-Client][handleHeaderResponse] Transfer-Encoding: %s\n", transferEncoding.c_str());
                    if(transferEncoding.equalsIgnoreCase("chunked")) {
                        _transferEncoding = HTTPC_TE_CHUNKED;
                    } else {
                        return HTTPC_ERROR_ENCODING;
                    }
                } else {
                    _transferEncoding = HTTPC_TE_IDENTITY;
                }

                if(_returnCode) {
                    return _returnCode;
                } else {
                    DEBUG_HTTPCLIENT("[HTTP-Client][handleHeaderResponse] Remote host is not an HTTP Server!");
                    return HTTPC_ERROR_NO_HTTP_SERVER;
                }
            }

        } else {
            if((millis() - lastDataTime) > _tcpTimeout) {
                return HTTPC_ERROR_READ_TIMEOUT;
            }
            delay(0);
        }
    }

    return HTTPC_ERROR_CONNECTION_LOST;
}

/**
 * write one Data Block to Stream
 * @param stream Stream *
 * @param size int
 * @return < 0 = error >= 0 = size written
 */
int HTTPClient::writeToStreamDataBlock(Stream * stream, int size) {
    int buff_size = HTTP_TCP_BUFFER_SIZE;
    int len = size;
    int bytesWritten = 0;

    // if possible create smaller buffer then HTTP_TCP_BUFFER_SIZE
    if((len > 0) && (len < HTTP_TCP_BUFFER_SIZE)) {
        buff_size = len;
    }

    // create buffer for read
    uint8_t * buff = (uint8_t *) malloc(buff_size);

    if(buff) {
        // read all data from server
        while(connected() && (len > 0 || len == -1)) {

            // get available data size
            size_t sizeAvailable = _tcp->available();

            if(sizeAvailable) {

                int readBytes = sizeAvailable;

                // read only the asked bytes
                if(len > 0 && readBytes > len) {
                    readBytes = len;
                }

                // not read more the buffer can handle
                if(readBytes > buff_size) {
                    readBytes = buff_size;
                }

                // read data
                int bytesRead = _tcp->readBytes(buff, readBytes);

                // write it to Stream
                int bytesWrite = stream->write(buff, bytesRead);
                bytesWritten += bytesWrite;

                // are all Bytes a writen to stream ?
                if(bytesWrite != bytesRead) {
                    DEBUG_HTTPCLIENT("[HTTP-Client][writeToStream] short write asked for %d but got %d retry...\n", bytesRead, bytesWrite);

                    // check for write error
                    if(stream->getWriteError()) {
                        DEBUG_HTTPCLIENT("[HTTP-Client][writeToStreamDataBlock] stream write error %d\n", stream->getWriteError());

                        //reset write error for retry
                        stream->clearWriteError();
                    }

                    // some time for the stream
                    delay(1);

                    int leftBytes = (readBytes - bytesWrite);

                    // retry to send the missed bytes
                    bytesWrite = stream->write((buff + bytesWrite), leftBytes);
                    bytesWritten += bytesWrite;

                    if(bytesWrite != leftBytes) {
                        // failed again
                        DEBUG_HTTPCLIENT("[HTTP-Client][writeToStream] short write asked for %d but got %d failed.\n", leftBytes, bytesWrite);
                        free(buff);
                        return HTTPC_ERROR_STREAM_WRITE;
                    }
                }

                // check for write error
                if(stream->getWriteError()) {
                    DEBUG_HTTPCLIENT("[HTTP-Client][writeToStreamDataBlock] stream write error %d\n", stream->getWriteError());
                    free(buff);
                    return HTTPC_ERROR_STREAM_WRITE;
                }

                // count bytes to read left
                if(len > 0) {
                    len -= readBytes;
                }

                delay(0);
            } else {
                delay(1);
            }
        }

        free(buff);

        DEBUG_HTTPCLIENT("[HTTP-Client][writeToStreamDataBlock] connection closed or file end (written: %d).\n", bytesWritten);

        if((size > 0) && (size != bytesWritten)) {
            DEBUG_HTTPCLIENT("[HTTP-Client][writeToStreamDataBlock] bytesWritten %d and size %d mismatch!.\n", bytesWritten, size);
            return HTTPC_ERROR_STREAM_WRITE;
        }

    } else {
        DEBUG_HTTPCLIENT("[HTTP-Client][writeToStreamDataBlock] too less ram! need %d\n", HTTP_TCP_BUFFER_SIZE);
        return HTTPC_ERROR_TOO_LESS_RAM;
    }

    return bytesWritten;
}

/**
 * called to handle error return, may disconnect the connection if still exists
 * @param error
 * @return error
 */
int HTTPClient::returnError(int error) {
    if(error < 0) {
        DEBUG_HTTPCLIENT("[HTTP-Client][returnError] error(%d): %s\n", error, errorToString(error).c_str());
        if(connected()) {
            DEBUG_HTTPCLIENT("[HTTP-Client][returnError] tcp stop\n");
            _tcp->stop();
        }
    }
    return error;
}
