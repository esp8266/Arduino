/**
 *
 * @file ESP8266WiFiMulti.h
 * @date 30.09.2020
 * @author Markus Sattler, Erriez
 *
 * Copyright (c) 2015-2020 Markus Sattler. All rights reserved.
 * This file is part of the esp8266 core for Arduino environment.
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


#ifndef WIFI_CLIENT_MULTI_H_
#define WIFI_CLIENT_MULTI_H_

#include "ESP8266WiFi.h"
#include <vector>

#ifdef DEBUG_ESP_WIFI
#ifdef DEBUG_ESP_PORT
#define DEBUG_WIFI_MULTI(fmt, ...) DEBUG_ESP_PORT.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_WIFI_MULTI
#define DEBUG_WIFI_MULTI(...) do { (void)0; } while (0)
#endif

//! Default WiFi connection timeout in ms
#ifndef WIFI_CONNECT_TIMEOUT_MS
#define WIFI_CONNECT_TIMEOUT_MS     5000
#endif

//! Default WiFi scan timeout in ms
#ifndef WIFI_SCAN_TIMEOUT_MS
#define WIFI_SCAN_TIMEOUT_MS        5000
#endif

class ESP8266WiFiMulti
{
public:
    using SSIDSelectedCallback = void (*)(const char *ssid);

    ESP8266WiFiMulti();
    ~ESP8266WiFiMulti();

    bool addAP(const char *ssid, const char *passphrase = NULL);
    bool existsAP(const char *ssid, const char *passphrase = NULL);
    void onSSIDSelected(SSIDSelectedCallback callback) { _onSSIDSelected = callback; }

    wl_status_t run(uint32_t connectTimeoutMs=WIFI_CONNECT_TIMEOUT_MS);

    void cleanAPlist();
    int count() { return _APlist.size(); }
private:
    struct WifiAPEntry {
        char *ssid;
        char *passphrase;
    };

    std::vector<WifiAPEntry> _APlist;
    bool _firstRun;
    SSIDSelectedCallback _onSSIDSelected = NULL;

    bool APlistAdd(const char *ssid, const char *passphrase = NULL);
    bool APlistExists(const char *ssid, const char *passphrase = NULL);
    void APlistClean();

    wl_status_t connectWiFiMulti(uint32_t connectTimeoutMs);
    int8_t startScan();
    void printWiFiScan();
};

#endif // WIFI_CLIENT_MULTI_H_
