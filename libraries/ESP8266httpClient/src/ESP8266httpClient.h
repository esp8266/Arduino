/**
 * ESP8266httpClient.h
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

#ifndef ESP8266HTTPCLIENT_H_
#define ESP8266HTTPCLIENT_H_

#define DEBUG_HTTPCLIENT(...) Serial1.printf( __VA_ARGS__ )

#ifndef DEBUG_HTTPCLIENT
#define DEBUG_HTTPCLIENT(...)
#endif

#define HTTPCLIENT_TCP_TIMEOUT (1000)

/// HTTP client errors
#define HTTPC_ERROR_CONNECTION_REFUSED  (-1)
#define HTTPC_ERROR_SEND_HEADER_FAILD   (-2)
#define HTTPC_ERROR_SEND_PAYLOAD_FAILD  (-3)
#define HTTPC_ERROR_NOT_CONNECTED       (-4)
#define HTTPC_ERROR_CONNECTION_LOST     (-5)


class httpClient {
    public:
        httpClient();
        ~httpClient();

        void begin(const char *host, uint16_t port, const char * url  = "/", bool https = false, const char * httpsFingerprint = "");
        void begin(String host, uint16_t port, String url = "/", bool https = false, String httpsFingerprint = "");

        bool connected(void);

        /// request handling
        int GET();
        int POST(uint8_t * payload, size_t size);
        int POST(String payload);
        int sendRequest(const char * type, uint8_t * payload = NULL, size_t size = 0);

        void addHeader(const String& name, const String& value, bool first = false);

        /// Response handling
        void collectHeaders(const char* headerKeys[], const size_t headerKeysCount);
        String header(const char* name);   // get request header value by name
        String header(size_t i);              // get request header value by number
        String headerName(size_t i);          // get request header name by number
        int headers();                     // get header count
        bool hasHeader(const char* name);  // check if header exists


        int getSize(void);

        WiFiClient & getStream(void);

    protected:

        struct RequestArgument {
          String key;
          String value;
        };


        WiFiClient * _tcp;
        WiFiClientSecure * _tcps;

        /// request handling
        String _host;
        uint16_t _port;

        String _url;
        bool _https;
        String _httpsFingerprint;

        String  _Headers;

        /// Response handling
        RequestArgument* _currentHeaders;
        size_t           _headerKeysCount;

        int _returnCode;
        int _size;


        bool connect(void);
        bool sendHeader(const char * type);
        int handleHeaderResponse();

};



#endif /* ESP8266HTTPCLIENT_H_ */
