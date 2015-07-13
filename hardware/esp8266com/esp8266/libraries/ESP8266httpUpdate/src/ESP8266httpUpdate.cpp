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

t_httpUpdate_return ESP8266HTTPUpdate::update(const char * host, uint16_t port, const char * url, const char * current_version) {

    t_httpUpdate_return ret = HTTP_UPDATE_FAILD;
    WiFiClient tcp;
    DEBUG_HTTP_UPDATE("[httpUpdate] connected to %s:%u %s .... ", host, port, url);

    if(!tcp.connect(host, port)) {
        DEBUG_HTTP_UPDATE("faild.\n");
        return ret;
    }
    DEBUG_HTTP_UPDATE("ok.\n");

    // set Timeout for readBytesUntil and readStringUntil
    tcp.setTimeout(2000);
    tcp.setNoDelay(true);

    String req = "GET ";

    req += url;
    req += " HTTP/1.1\r\n"
            "Host: ";
    req += host;
    req += "\r\n"
            "Connection: close\r\n"
            "User-Agent: ESP8266-http-Update\r\n"
            "x-ESP8266-STA-MAC: ";
    req += WiFi.macAddress();
    req += "\r\n"
            "x-ESP8266-AP-MAC: ";
    req += WiFi.softAPmacAddress();
    req += "\r\n"
            "x-ESP8266-free-space: ";
    req += ESP.getFreeSketchSpace();
    req += "\r\n"
            "x-ESP8266-sketch-size: ";
    req += ESP.getSketchSize();
    req += "\r\n"
            "x-ESP8266-chip-size: ";
    req += ESP.getFlashChipRealSize();
    req += "\r\n"
           "x-ESP8266-sdk-version: ";
    req += ESP.getSdkVersion();

    if(current_version[0] != 0x00) {
        req += "\r\n"
               "x-ESP8266-version: ";
        req += current_version;
    }

    req += "\r\n"
           "\r\n";

    tcp.write(req.c_str(), req.length());

    uint32_t code = 0;
    size_t len = 0;

    while(true) {
        String headerLine = tcp.readStringUntil('\n');
        headerLine.trim(); // remove \r

        if(headerLine.length() > 0) {
            DEBUG_HTTP_UPDATE("[httpUpdate][Header] RX: %s\n", headerLine.c_str());
            if(headerLine.startsWith("HTTP/1.")) {
                // 9 = lenght of "HTTP/1.x "
                code = headerLine.substring(9, headerLine.indexOf(' ', 9)).toInt();
            } else if(headerLine.startsWith("Content-Length: ")) {
                // 16 = lenght of "Content-Length: "
                len = headerLine.substring(16).toInt();
            }
        } else {
            break;
        }
    }

    DEBUG_HTTP_UPDATE("[httpUpdate] Header read fin.\n");
    DEBUG_HTTP_UPDATE("[httpUpdate] Server header:\n");
    DEBUG_HTTP_UPDATE("[httpUpdate]  - code: %d\n", code);
    DEBUG_HTTP_UPDATE("[httpUpdate]  - len: %d\n", len);

    DEBUG_HTTP_UPDATE("[httpUpdate] ESP8266 info:\n");
    DEBUG_HTTP_UPDATE("[httpUpdate]  - free Space: %d\n", ESP.getFreeSketchSpace());
    DEBUG_HTTP_UPDATE("[httpUpdate]  - current Sketch Size: %d\n", ESP.getSketchSize());

    if(current_version[0] != 0x00) {
        DEBUG_HTTP_UPDATE("[httpUpdate]  - current version: %s\n", current_version);
    }

    switch(code) {
        case 200:  ///< OK (Start Update)
            if(len > 0) {
                if(len > ESP.getFreeSketchSpace()) {
                    ret = HTTP_UPDATE_FAILD;
                    DEBUG_HTTP_UPDATE("[httpUpdate] FreeSketchSpace to low (%d) needed: %d\n", ESP.getFreeSketchSpace(), len);
                } else {
                    if(ESP.updateSketch(tcp, len)) {
                        // may never reached!
                        ret = HTTP_UPDATE_OK;
                        DEBUG_HTTP_UPDATE("[httpUpdate] Update ok\n");
                    } else {
                        ret = HTTP_UPDATE_FAILD;
                        DEBUG_HTTP_UPDATE("[httpUpdate] Update failed\n");
                    }
                }
            } else {
                ret = HTTP_UPDATE_FAILD;
                DEBUG_HTTP_UPDATE("[httpUpdate]Content-Length is 0?!\n");
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
            ret = HTTP_UPDATE_FAILD;
            DEBUG_HTTP_UPDATE("[httpUpdate] Code is (%d)\n", code);
            break;
    }

    return ret;
}

t_httpUpdate_return ESP8266HTTPUpdate::update(String host, uint16_t port, String url, String current_version) {
    return update(host.c_str(), port, url.c_str(), current_version.c_str());
}

ESP8266HTTPUpdate ESPhttpUpdate;
