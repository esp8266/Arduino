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

class httpClient {
    public:
        httpClient();
        ~httpClient();

        void begin(const char *host, uint16_t port, const char * url  = "/", bool https = false, const char * httpsFingerprint = "");
        void begin(String host, uint16_t port, String url = "/", bool https = false, String httpsFingerprint = "");

        bool connected(void);

        bool GET();
        bool POST(uint8_t * payload, size_t size);
        bool POST(String payload);

        void addHeader(const String& name, const String& value, bool first = false);



        WiFiClient & getStream(void);

    protected:
        WiFiClient * _tcp;
        WiFiClientSecure * _tcps;

        String _host;
        uint16_t _port;

        String _url;
        bool _https;
        String _httpsFingerprint;

        String  _Headers;

        bool connect(void);

        bool sendHeader(const char * type);


};



#endif /* ESP8266HTTPCLIENT_H_ */
