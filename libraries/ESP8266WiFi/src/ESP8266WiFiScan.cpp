/*
 ESP8266WiFiScan.cpp - WiFi library for esp8266

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
#include "ESP8266WiFiScan.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
}

#include "debug.h"

extern "C" void esp_schedule();
extern "C" void esp_yield();

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- Private functions ------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------




// -----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------- scan function ---------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

bool ESP8266WiFiScanClass::_scanAsync = false;
bool ESP8266WiFiScanClass::_scanStarted = false;
bool ESP8266WiFiScanClass::_scanComplete = false;

size_t ESP8266WiFiScanClass::_scanCount = 0;
void* ESP8266WiFiScanClass::_scanResult = 0;

std::function<void(int)> ESP8266WiFiScanClass::_onComplete;

/**
 * Start scan WiFi networks available
 * @param async         run in async mode
 * @param show_hidden   show hidden networks
 * @param channel       scan only this channel (0 for all channels)
 * @param ssid*         scan for only this ssid (NULL for all ssid's)
 * @return Number of discovered networks
 */
int8_t ESP8266WiFiScanClass::scanNetworks(bool async, bool show_hidden, uint8 channel, uint8* ssid) {
    if(ESP8266WiFiScanClass::_scanStarted) {
        return WIFI_SCAN_RUNNING;
    }

    ESP8266WiFiScanClass::_scanAsync = async;

    WiFi.enableSTA(true);

    int status = wifi_station_get_connect_status();
    if(status != STATION_GOT_IP && status != STATION_IDLE) {
        wifi_station_disconnect();
    }

    scanDelete();

    struct scan_config config;
    memset(&config, 0, sizeof(config));
    config.ssid = ssid;
    config.channel = channel;
    config.show_hidden = show_hidden;
    if(wifi_station_scan(&config, reinterpret_cast<scan_done_cb_t>(&ESP8266WiFiScanClass::_scanDone))) {
        ESP8266WiFiScanClass::_scanComplete = false;
        ESP8266WiFiScanClass::_scanStarted = true;

        if(ESP8266WiFiScanClass::_scanAsync) {
            delay(0); // time for the OS to trigger the scan
            return WIFI_SCAN_RUNNING;
        }

        esp_yield();
        return ESP8266WiFiScanClass::_scanCount;
    } else {
        return WIFI_SCAN_FAILED;
    }

}

/**
 * Starts scanning WiFi networks available in async mode
 * @param onComplete    the event handler executed when the scan is done
 * @param show_hidden   show hidden networks
  */
void ESP8266WiFiScanClass::scanNetworksAsync(std::function<void(int)> onComplete, bool show_hidden) {
    _onComplete = onComplete;
    scanNetworks(true, show_hidden);
}

/**
 * called to get the scan state in Async mode
 * @return scan result or status
 *          -1 if scan not fin
 *          -2 if scan not triggered
 */
int8_t ESP8266WiFiScanClass::scanComplete() {

    if(_scanStarted) {
        return WIFI_SCAN_RUNNING;
    }

    if(_scanComplete) {
        return ESP8266WiFiScanClass::_scanCount;
    }

    return WIFI_SCAN_FAILED;
}

/**
 * delete last scan result from RAM
 */
void ESP8266WiFiScanClass::scanDelete() {
    if(ESP8266WiFiScanClass::_scanResult) {
        delete[] reinterpret_cast<bss_info*>(ESP8266WiFiScanClass::_scanResult);
        ESP8266WiFiScanClass::_scanResult = 0;
        ESP8266WiFiScanClass::_scanCount = 0;
    }
    _scanComplete = false;
}


/**
 * loads all infos from a scanned wifi in to the ptr parameters
 * @param networkItem uint8_t
 * @param ssid  const char**
 * @param encryptionType uint8_t *
 * @param RSSI int32_t *
 * @param BSSID uint8_t **
 * @param channel int32_t *
 * @param isHidden bool *
 * @return (true if ok)
 */
bool ESP8266WiFiScanClass::getNetworkInfo(uint8_t i, String &ssid, uint8_t &encType, int32_t &rssi, uint8_t* &bssid, int32_t &channel, bool &isHidden) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return false;
    }

    char ssid_copy[33]; // Ensure space for maximum len SSID (32) plus trailing 0
    memcpy(ssid_copy, it->ssid, sizeof(it->ssid));
    ssid_copy[32] = 0; // Potentially add 0-termination if none present earlier
    ssid = (const char*) ssid_copy;
    encType = encryptionType(i);
    rssi = it->rssi;
    bssid = it->bssid; // move ptr
    channel = it->channel;
    isHidden = (it->is_hidden != 0);

    return true;
}


/**
 * Return the SSID discovered during the network scan.
 * @param i     specify from which network item want to get the information
 * @return       ssid string of the specified item on the networks scanned list
 */
String ESP8266WiFiScanClass::SSID(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return "";
    }
    char tmp[33]; //ssid can be up to 32chars, => plus null term
    memcpy(tmp, it->ssid, sizeof(it->ssid));
    tmp[32] = 0; //nullterm in case of 32 char ssid

    return String(reinterpret_cast<const char*>(tmp));
}


/**
 * Return the encryption type of the networks discovered during the scanNetworks
 * @param i specify from which network item want to get the information
 * @return  encryption type (enum wl_enc_type) of the specified item on the networks scanned list
 */
uint8_t ESP8266WiFiScanClass::encryptionType(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return -1;
    }

    switch(it->authmode) {
        case AUTH_OPEN:
            return ENC_TYPE_NONE;
        case AUTH_WEP:
            return ENC_TYPE_WEP;
        case AUTH_WPA_PSK:
            return ENC_TYPE_TKIP;
        case AUTH_WPA2_PSK:
            return ENC_TYPE_CCMP;
        case AUTH_WPA_WPA2_PSK:
            return ENC_TYPE_AUTO;
        default:
            return -1;
    }
}

/**
 * Return the RSSI of the networks discovered during the scanNetworks
 * @param i specify from which network item want to get the information
 * @return  signed value of RSSI of the specified item on the networks scanned list
 */
int32_t ESP8266WiFiScanClass::RSSI(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return 0;
    }
    return it->rssi;
}


/**
 * return MAC / BSSID of scanned wifi
 * @param i specify from which network item want to get the information
 * @return uint8_t * MAC / BSSID of scanned wifi
 */
uint8_t * ESP8266WiFiScanClass::BSSID(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return 0;
    }
    return it->bssid;
}

/**
 * return MAC / BSSID of scanned wifi
 * @param i specify from which network item want to get the information
 * @return String MAC / BSSID of scanned wifi
 */
String ESP8266WiFiScanClass::BSSIDstr(uint8_t i) {
    char mac[18] = { 0 };
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return String("");
    }
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", it->bssid[0], it->bssid[1], it->bssid[2], it->bssid[3], it->bssid[4], it->bssid[5]);
    return String(mac);
}

int32_t ESP8266WiFiScanClass::channel(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return 0;
    }
    return it->channel;
}

/**
 * return if the scanned wifi is Hidden (no SSID)
 * @param networkItem specify from which network item want to get the information
 * @return bool (true == hidden)
 */
bool ESP8266WiFiScanClass::isHidden(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return false;
    }
    return (it->is_hidden != 0);
}

/**
 * private
 * scan callback
 * @param result  void *arg
 * @param status STATUS
 */
void ESP8266WiFiScanClass::_scanDone(void* result, int status) {
    if(status != OK) {
        ESP8266WiFiScanClass::_scanCount = 0;
        ESP8266WiFiScanClass::_scanResult = 0;
    } else {

        int i = 0;
        bss_info* head = reinterpret_cast<bss_info*>(result);

        for(bss_info* it = head; it; it = STAILQ_NEXT(it, next), ++i)
            ;
        ESP8266WiFiScanClass::_scanCount = i;
        if(i == 0) {
            ESP8266WiFiScanClass::_scanResult = 0;
        } else {
            bss_info* copied_info = new bss_info[i];
            i = 0;
            for(bss_info* it = head; it; it = STAILQ_NEXT(it, next), ++i) {
                memcpy(copied_info + i, it, sizeof(bss_info));
            }

            ESP8266WiFiScanClass::_scanResult = copied_info;
        }

    }

    ESP8266WiFiScanClass::_scanStarted = false;
    ESP8266WiFiScanClass::_scanComplete = true;

    if(!ESP8266WiFiScanClass::_scanAsync) {
        esp_schedule();
    } else if (ESP8266WiFiScanClass::_onComplete) {
        ESP8266WiFiScanClass::_onComplete(ESP8266WiFiScanClass::_scanCount);
        ESP8266WiFiScanClass::_onComplete = nullptr;
    }
}

/**
 *
 * @param i specify from which network item want to get the information
 * @return bss_info *
 */
void * ESP8266WiFiScanClass::_getScanInfoByIndex(int i) {
    if(!ESP8266WiFiScanClass::_scanResult || (size_t) i > ESP8266WiFiScanClass::_scanCount) {
        return 0;
    }
    return reinterpret_cast<bss_info*>(ESP8266WiFiScanClass::_scanResult) + i;
}
