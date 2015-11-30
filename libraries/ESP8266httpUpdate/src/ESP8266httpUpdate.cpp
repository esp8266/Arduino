/**
 *
 * @file ESP8266HTTPUpdate.cpp
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


#include "ESP8266httpUpdate.h"


ESP8266HTTPUpdate::ESP8266HTTPUpdate(void) {
}

ESP8266HTTPUpdate::~ESP8266HTTPUpdate(void) {
}

/**
 *
 * @param url const char *
 * @param current_version const char *
 * @param httpsFingerprint const char *
 * @return t_httpUpdate_return
 */
t_httpUpdate_return ESP8266HTTPUpdate::update(const char * url, const char * current_version, const char * httpsFingerprint) {
    HTTPClient http;
    http.begin(url, httpsFingerprint);
    return handleUpdate(&http, current_version);
}

/**
 *
 * @param host const char *
 * @param port uint16_t
 * @param url const char *
 * @param current_version const char *
 * @param httpsFingerprint const char *
 * @return
 */
t_httpUpdate_return ESP8266HTTPUpdate::update(const char * host, uint16_t port, const char * url, const char * current_version, bool https, const char * httpsFingerprint) {
    HTTPClient http;
    http.begin(host, port, url, https, httpsFingerprint);
    return handleUpdate(&http, current_version);
}

t_httpUpdate_return ESP8266HTTPUpdate::update(String host, uint16_t port, String url, String current_version, bool https, String httpsFingerprint) {
    HTTPClient http;
    http.begin(host, port, url, https, httpsFingerprint);
    return handleUpdate(&http, current_version.c_str());
}

/**
 *
 * @param http HTTPClient *
 * @param current_version const char *
 * @return t_httpUpdate_return
 */
t_httpUpdate_return ESP8266HTTPUpdate::handleUpdate(HTTPClient * http, const char * current_version) {

    t_httpUpdate_return ret = HTTP_UPDATE_FAILED;

    http->setUserAgent("ESP8266-http-Update");
    http->addHeader("x-ESP8266-STA-MAC", WiFi.macAddress());
    http->addHeader("x-ESP8266-AP-MAC", WiFi.softAPmacAddress());
    http->addHeader("x-ESP8266-free-space", String(ESP.getFreeSketchSpace()));
    http->addHeader("x-ESP8266-sketch-size", String(ESP.getSketchSize()));
    http->addHeader("x-ESP8266-chip-size", String(ESP.getFlashChipRealSize()));
    http->addHeader("x-ESP8266-sdk-version", ESP.getSdkVersion());

    if(current_version && current_version[0] != 0x00) {
        http->addHeader("x-ESP8266-version", current_version);
    }

    const char * headerkeys[] = { "x-MD5" };
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    // track these headers
    http->collectHeaders(headerkeys, headerkeyssize);


    int code = http->GET();
    int len = http->getSize();

    DEBUG_HTTP_UPDATE("[httpUpdate] Header read fin.\n");
    DEBUG_HTTP_UPDATE("[httpUpdate] Server header:\n");
    DEBUG_HTTP_UPDATE("[httpUpdate]  - code: %d\n", code);
    DEBUG_HTTP_UPDATE("[httpUpdate]  - len: %d\n", len);

    if(http->hasHeader("x-MD5")) {
        DEBUG_HTTP_UPDATE("[httpUpdate]  - MD5: %s\n", http->header("x-MD5").c_str());
    }

    DEBUG_HTTP_UPDATE("[httpUpdate] ESP8266 info:\n");
    DEBUG_HTTP_UPDATE("[httpUpdate]  - free Space: %d\n", ESP.getFreeSketchSpace());
    DEBUG_HTTP_UPDATE("[httpUpdate]  - current Sketch Size: %d\n", ESP.getSketchSize());

    if(current_version && current_version[0] != 0x00) {
        DEBUG_HTTP_UPDATE("[httpUpdate]  - current version: %s\n", current_version);
    }

    switch(code) {
        case 200:  ///< OK (Start Update)
            if(len > 0) {
                if(len > ESP.getFreeSketchSpace()) {
                    ret = HTTP_UPDATE_FAILED;
                    DEBUG_HTTP_UPDATE("[httpUpdate] FreeSketchSpace to low (%d) needed: %d\n", ESP.getFreeSketchSpace(), len);
                } else {

                    WiFiClient * tcp = http->getStreamPtr();

                    WiFiUDP::stopAll();
                    WiFiClient::stopAllExcept(tcp);

                    delay(100);

                    if(runUpdate(*tcp, len, http->header("x-MD5"))) {
                        ret = HTTP_UPDATE_OK;
                        DEBUG_HTTP_UPDATE("[httpUpdate] Update ok\n");
                        http->end();
                        ESP.restart();
                    } else {
                        ret = HTTP_UPDATE_FAILED;
                        DEBUG_HTTP_UPDATE("[httpUpdate] Update failed\n");
                    }
                }
            } else {
                ret = HTTP_UPDATE_FAILED;
                DEBUG_HTTP_UPDATE("[httpUpdate] Content-Length is 0 or not set by Server?!\n");
            }
            break;
        case 304:
            ///< Not Modified (No updates)
            ret = HTTP_UPDATE_NO_UPDATES;
            break;
        case 403:
            ///< Forbidden
            // todo handle login
        default:
            ret = HTTP_UPDATE_FAILED;
            DEBUG_HTTP_UPDATE("[httpUpdate] Code is (%d)\n", code);
            break;
    }

    http->end();

    return ret;
}

/**
 * write Update to flash
 * @param in Stream&
 * @param size uint32_t
 * @param md5 String
 * @return true if Update ok
 */
bool ESP8266HTTPUpdate::runUpdate(Stream& in, uint32_t size, String md5) {

    if(!Update.begin(size)) {
        DEBUG_HTTP_UPDATE("[httpUpdate] Update.begin failed!\n");
        return false;
    }

    if(md5.length()) {
        Update.setMD5(md5.c_str());
    }

    if(Update.writeStream(in) != size) {
        DEBUG_HTTP_UPDATE("[httpUpdate] Update.writeStream failed!\n");
        return false;
    }

    if(!Update.end()) {
        DEBUG_HTTP_UPDATE("[httpUpdate] Update.end failed!\n");
        return false;
    }

    return true;
}



ESP8266HTTPUpdate ESPhttpUpdate;
