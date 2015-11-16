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
}

httpClient::~httpClient() {
    if(connected()) {
        _tcp->stop();
    }
}


void httpClient::begin(const char *host, uint16_t port, const char * url, bool https, const char * httpsFingerprint) {
    _host = host;
    _port = port;
    _url  = url;
    _https = https;
    _httpsFingerprint = httpsFingerprint;
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

bool httpClient::GET() {

    bool status;
    status = connect();
    if(status) {
        status = sendHeader("GET");
    }

    return status;
}

/**
 * sends a post request to the server
 * @param payload uint8_t *
 * @param size size_t
 * @return status
 */
bool httpClient::POST(uint8_t * payload, size_t size) {

    bool status;
    status = connect();
    if(status) {
        addHeader("Content-Length", String(size));
        status = sendHeader("POST");
    }

    if(status) {
        status = _tcp->write(&payload[0], size);
    }

    return status;
}

bool httpClient::POST(String payload) {
    return POST((uint8_t *) payload.c_str(), payload.length());
}

/**
 * returns the stram of the tcp connection
 * @return WiFiClient
 */
WiFiClient & httpClient::getStream(void) {
    if(connected()) {
        return *_tcp;
    }
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

/**
 * init TCP connection and handle ssl verify if needed
 * @return true if connection is ok
 */
bool httpClient::connect(void) {

    if(connected()) {
        DEBUG_HTTPCLIENT("[HTTP-Client] connect. already connected!\n");
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

    if(_https) {
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
    String header = String(type) + " " + _url + " HTTP/1.1\r\n"
            "Host: " + _host + "\r\n"
            "User-Agent: ESP8266httpClient\r\n"
            "Connection: close\r\n" +
            _Headers +
            "\r\n";

    return _tcp->write(header.c_str(), header.length());
}
