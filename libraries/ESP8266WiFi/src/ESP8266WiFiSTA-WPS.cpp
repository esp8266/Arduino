/*
 ESP8266WiFiSTA-WPS.cpp - WiFi library for esp8266

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 Reworked on 28 Dec 2015 by Markus Sattler

 */


#include "ESP8266WiFi.h"
#include "ESP8266WiFiGeneric.h"
#include "ESP8266WiFiSTA.h"
#include "coredecls.h" // disable_extra4k_at_link_time()

static void wifi_wps_status_cb(wps_cb_status status);

static bool _wps_config_pending = false;

/**
 * WPS config
 * so far only WPS_TYPE_PBC is supported (SDK 1.2.0)
 * @return ok
 */
bool ESP8266WiFiSTAClass::beginWPSConfig(void) {

    // SYS ram is used by WPS, let's configure user stack inside user's HEAP
    disable_extra4k_at_link_time();

    if(!WiFi.enableSTA(true)) {
        // enable STA failed
        return false;
    }

    disconnect();

    DEBUGV("wps begin\n");

    if(!wifi_wps_disable()) {
        DEBUGV("wps disable failed\n");
        return false;
    }

    // so far only WPS_TYPE_PBC is supported (SDK 1.2.0)
    if(!wifi_wps_enable(WPS_TYPE_PBC)) {
        DEBUGV("wps enable failed\n");
        return false;
    }

    if(!wifi_set_wps_cb((wps_st_cb_t) &wifi_wps_status_cb)) {
        DEBUGV("wps cb failed\n");
        return false;
    }

    if(!wifi_wps_start()) {
        DEBUGV("wps start failed\n");
        return false;
    }

    _wps_config_pending = true;
    // will continue when wifi_wps_status_cb fires
    while (_wps_config_pending) {
        esp_yield();
    }
    _wps_config_pending = false;

    return true;
}

/**
 * WPS callback
 * @param status wps_cb_status
 */
void wifi_wps_status_cb(wps_cb_status status) {
    DEBUGV("wps cb status: %d\r\n", status);
    switch(status) {
        case WPS_CB_ST_SUCCESS:
            if(!wifi_wps_disable()) {
                DEBUGV("wps disable failed\n");
            }
            wifi_station_connect();
            break;
        case WPS_CB_ST_FAILED:
            DEBUGV("wps FAILED\n");
            break;
        case WPS_CB_ST_TIMEOUT:
            DEBUGV("wps TIMEOUT\n");
            break;
        case WPS_CB_ST_WEP:
            DEBUGV("wps WEP\n");
            break;
        case WPS_CB_ST_UNK:
            DEBUGV("wps UNKNOWN\n");
            if(!wifi_wps_disable()) {
                DEBUGV("wps disable failed\n");
            }
            break;
    }
    // TODO user function to get status

    _wps_config_pending = false; // resume beginWPSConfig
    esp_schedule();
}
