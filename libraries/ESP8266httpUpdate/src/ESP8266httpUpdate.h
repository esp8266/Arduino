/**
 *
 * @file ESP8266HTTPUpdate.h
 * @date 21.06.2015
 * @author Markus Sattler
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the ESP8266 Http Updater.
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

#ifndef ESP8266HTTPUPDATE_H_
#define ESP8266HTTPUPDATE_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>

//#define DEBUG_HTTP_UPDATE(...) Serial1.printf( __VA_ARGS__ )

#ifndef DEBUG_HTTP_UPDATE
#define DEBUG_HTTP_UPDATE(...)
#endif

typedef enum {
    HTTP_UPDATE_FAILD,
    HTTP_UPDATE_NO_UPDATES,
    HTTP_UPDATE_OK
} t_httpUpdate_return;

class ESP8266HTTPUpdate {
    public:
        ESP8266HTTPUpdate(void);
        ~ESP8266HTTPUpdate(void);

        t_httpUpdate_return update(const char * host, uint16_t port, const char * url = "/", const char * current_version = "");
        t_httpUpdate_return update(String host, uint16_t port, String url = "/", String current_version = "");
};

extern ESP8266HTTPUpdate ESPhttpUpdate;

#endif /* ESP8266HTTPUPDATE_H_ */
