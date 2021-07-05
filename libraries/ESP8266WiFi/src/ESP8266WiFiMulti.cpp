/**
 *
 * @file ESP8266WiFiMulti.cpp
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

#include "PolledTimeout.h"
#include "ESP8266WiFiMulti.h"
#include <limits.h>
#include <string.h>
#include <coredecls.h>

/**
 * @brief Print WiFi status
 * @details
 *      Macro DEBUG_ESP_WIFI and DEBUG_ESP_PORT must be configured
 * @param status
 *      WiFi status
 */
static void printWiFiStatus(wl_status_t status)
{
#ifdef DEBUG_ESP_WIFI
    IPAddress ip;
    uint8_t *mac;

    switch (status) {
        case WL_CONNECTED:
            ip = WiFi.localIP();
            mac = WiFi.BSSID();

            DEBUG_WIFI_MULTI("[WIFIM] Connected:\n");
            DEBUG_WIFI_MULTI("[WIFIM]   SSID: %s\n", WiFi.SSID().c_str());
            DEBUG_WIFI_MULTI("[WIFIM]   IP:   %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
            DEBUG_WIFI_MULTI("[WIFIM]   MAC:  %02X:%02X:%02X:%02X:%02X:%02X\n",
                             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            DEBUG_WIFI_MULTI("[WIFIM]   CH:   %d\n", WiFi.channel());
            DEBUG_WIFI_MULTI("[WIFIM]   RSSI: %d\n", WiFi.RSSI());
            break;
        case WL_NO_SSID_AVAIL:
            DEBUG_WIFI_MULTI("[WIFIM] Connecting failed AP not found.\n");
            break;
        case WL_CONNECT_FAILED:
            DEBUG_WIFI_MULTI("[WIFIM] Connecting failed.\n");
            break;
        case WL_WRONG_PASSWORD:
            DEBUG_WIFI_MULTI("[WIFIM] Wrong password.\n");
            break;
        default:
            DEBUG_WIFI_MULTI("[WIFIM] Connecting failed (%d).\n", status);
            break;
    }
#else
    // Suppress warning unused variable
    (void)(status);
#endif
}

/**
 * @brief Wait for WiFi connect status change, protected with timeout
 * @param connectTimeoutMs
 *      WiFi connection timeout in ms
 * @return
 *      WiFi connection status
 */
static wl_status_t waitWiFiConnect(uint32_t connectTimeoutMs)
{
    wl_status_t status;
    // The final argument, intvl_ms, to esp_delay influences how frequently
    // the scheduled recurrent functions (Schedule.h) are probed.
    esp_delay(connectTimeoutMs,
        [&status]() { status = WiFi.status(); return status != WL_CONNECTED && status != WL_CONNECT_FAILED; }, 0);

    // Check status
    if (status == WL_CONNECTED) {
        // Connected, print WiFi status
        printWiFiStatus(status);

        // Return WiFi status
        return status;
    } else if (status == WL_CONNECT_FAILED) {
        DEBUG_WIFI_MULTI("[WIFIM] Connect failed\n");
    } else {
        DEBUG_WIFI_MULTI("[WIFIM] Connect timeout\n");
    }

    // Return WiFi connect failed
    return WL_CONNECT_FAILED;
}

/**
 * @brief Constructor
 */
ESP8266WiFiMulti::ESP8266WiFiMulti() : _firstRun(true)
{
}

/**
 * @brief Destructor
 */
ESP8266WiFiMulti::~ESP8266WiFiMulti()
{
    // Cleanup memory
    APlistClean();
}

/**
 * @brief Add Access Point
 * @param ssid
 *      WiFi SSID char array, max 32 characters + NULL character
 * @param passphrase
 *      WiFi password char array, max 63 characters + NULL character
 * @retval true
 *      Success
 * @retval false
 *      Failure
 */
bool ESP8266WiFiMulti::addAP(const char *ssid, const char *passphrase)
{
    return APlistAdd(ssid, passphrase);
}

/**
 * @brief Remove all Access Points from list
 */
void ESP8266WiFiMulti::cleanAPlist(void)
{
    APlistClean();
}

/**
 * @brief Check if Access Point exists in list
 * @param ssid
 *      WiFi SSID
 * @param passphrase
 *      WiFi Password
 * @retval true
 *      Success
 * @retval false
 *      Failure
 */
bool ESP8266WiFiMulti::existsAP(const char *ssid, const char *passphrase)
{
    return APlistExists(ssid, passphrase);
}

/**
 * @brief Keep WiFi connected to Access Point with strongest WiFi signal (RSSI)
 * @param connectTimeoutMs
 *      Timeout in ms per WiFi connection (excluding fixed 5 seconds scan timeout)
 * @return
 *      WiFi status
 */
wl_status_t ESP8266WiFiMulti::run(uint32_t connectTimeoutMs)
{
    int8_t scanResult;
    wl_status_t status;

    // Fast connect to previous WiFi on startup
    if (_firstRun) {
        _firstRun = false;

        // Check if previous WiFi connection saved
        if (strlen(WiFi.SSID().c_str())) {
            DEBUG_WIFI_MULTI("[WIFIM] Connecting saved WiFi\n");

            // Connect to previous saved WiFi
            WiFi.begin();

            // Wait for status change
            status = waitWiFiConnect(connectTimeoutMs);
        }
    }

    // Check connection state
    status = WiFi.status();
    if (status == WL_CONNECTED) {
        // Already connected
        return status;
    }

    // Start WiFi scan
    scanResult = startScan();
    if (scanResult < 0) {
        // No WiFi scan results
        return WL_NO_SSID_AVAIL;
    }

    // Try to connect to multiple WiFi's with strongest signal (RSSI)
    return connectWiFiMulti(connectTimeoutMs);
}

/**
 * @brief Start WiFi scan
 * @retval >0
 *      Number of detected WiFi SSID's
 * @retval 0
 *      No WiFi connections found
 * @retval -2
 *      WiFi scan failed
 */
int8_t ESP8266WiFiMulti::startScan()
{
    int8_t scanResult;

    DEBUG_WIFI_MULTI("[WIFIM] Start scan\n");

    // Clean previous scan
    WiFi.scanDelete();

    // Remove previous WiFi SSID/password
    WiFi.disconnect();

    // Start wifi scan in async mode
    WiFi.scanNetworks(true);

    // Wait for WiFi scan change or timeout
    // The final argument, intvl_ms, to esp_delay influences how frequently
    // the scheduled recurrent functions (Schedule.h) are probed.
    esp_delay(WIFI_SCAN_TIMEOUT_MS,
        [&scanResult]() { scanResult = WiFi.scanComplete(); return scanResult < 0; }, 0);
    // Check for scan timeout which may occur when scan does not report completion
    if (scanResult < 0) {
        DEBUG_WIFI_MULTI("[WIFIM] Scan timeout\n");
        return WIFI_SCAN_FAILED;
    }

    // Print WiFi scan result
    printWiFiScan();

    // Return (positive) number of detected WiFi networks
    return scanResult;
}

/**
 * @brief Connect to multiple WiFi's
 * @param connectTimeoutMs
 *      WiFi connect timeout in ms
 * @return
 *      WiFi connection status
 */
wl_status_t ESP8266WiFiMulti::connectWiFiMulti(uint32_t connectTimeoutMs)
{
    int8_t scanResult;
    String ssid;
    int32_t rssi;
    uint8_t encType;
    uint8_t *bssid;
    int32_t channel;
    bool hidden;

    // Get scan results
    scanResult = WiFi.scanComplete();

    // Find known WiFi networks
    uint8_t known[_APlist.size()];
    uint8_t numNetworks = 0;
    for (int8_t i = 0; i < scanResult; i++) {
        // Get network information
        WiFi.getNetworkInfo(i, ssid, encType, rssi, bssid, channel, hidden);

        // Check if the WiFi network contains an entry in AP list
        for (auto entry : _APlist) {
            // Check SSID
            if (ssid == entry.ssid) {
                // Known network
                known[numNetworks++] = i;
            }
        }
    }

    // Sort WiFi networks by RSSI
    for (int i = 0; i < numNetworks; i++) {
        for (int j = i + 1; j < numNetworks; j++) {
            if (WiFi.RSSI(known[j]) > WiFi.RSSI(known[i])) {
                int8_t tmp;

                // Swap indices
                tmp = known[i];
                known[i] = known[j];
                known[j] = tmp;
            }
        }
    }

    // Print sorted indices
    DEBUG_WIFI_MULTI("[WIFIM] Sorted indices: ");
    for (int8_t i = 0; i < numNetworks; i++) {
        DEBUG_WIFI_MULTI("%d ", known[i]);
    }
    DEBUG_WIFI_MULTI("\n");

    // Create indices for AP connection failures
    uint8_t connectSkipIndex[_APlist.size()];
    memset(connectSkipIndex, 0, sizeof(connectSkipIndex));

    // Connect to known WiFi AP's sorted by RSSI
    for (int8_t i = 0; i < numNetworks; i++) {
        // Get network information
        WiFi.getNetworkInfo(known[i], ssid, encType, rssi, bssid, channel, hidden);

        for (uint8_t j = 0; j < _APlist.size(); j++) {
            auto &entry = _APlist[j];

            // Check SSID
            if (ssid == entry.ssid) {
                DEBUG_WIFI_MULTI("[WIFIM] Connecting %s\n", ssid);

                // Connect to WiFi
                WiFi.begin(ssid, entry.passphrase, channel, bssid);

                // Wait for status change
                if (waitWiFiConnect(connectTimeoutMs) == WL_CONNECTED) {
                    return WL_CONNECTED;
                }

                // Failed to connect, skip for hidden SSID connects
                connectSkipIndex[j] = true;
            }
        }
    }

    // Try to connect to hidden AP's which are not reported by WiFi scan
    for (uint8_t i = 0; i < _APlist.size(); i++) {
        auto &entry = _APlist[i];

        if (!connectSkipIndex[i]) {
            DEBUG_WIFI_MULTI("[WIFIM] Try hidden connect %s\n", entry.ssid);

            // Connect to WiFi
            WiFi.begin(entry.ssid, entry.passphrase);

            // Wait for status change
            if (waitWiFiConnect(connectTimeoutMs) == WL_CONNECTED) {
                return WL_CONNECTED;
            }
        }
    }

    DEBUG_WIFI_MULTI("[WIFIM] Could not connect\n", ssid);

    // Could not connect to any WiFi network
    return WL_CONNECT_FAILED;
}

// ##################################################################################

/**
 * @brief Add WiFi connection to internal AP list
 * @param ssid
 *      WiFi SSID
 * @param passphrase
 *      WiFi Password
 * @retval true
 *      Success
 * @retval false
 *      Failure
 */
bool ESP8266WiFiMulti::APlistAdd(const char *ssid, const char *passphrase)
{
    WifiAPEntry newAP;

    if (!ssid || (*ssid == 0x00) || (strlen(ssid) > 32)) {
        // Fail SSID too long or missing!
        DEBUG_WIFI_MULTI("[WIFIM][APlistAdd] No ssid or ssid too long\n");
        return false;
    }

    // For passphrase, max is 63 ascii + null. For psk, 64hex + null.
    if (passphrase && (strlen(passphrase) > 64)) {
        // fail passphrase too long!
        DEBUG_WIFI_MULTI("[WIFIM][APlistAdd] Passphrase too long\n");
        return false;
    }

    if (APlistExists(ssid, passphrase)) {
        DEBUG_WIFI_MULTI("[WIFIM][APlistAdd] SSID: %s already exists\n", ssid);
        return true;
    }

    newAP.ssid = strdup(ssid);

    if (!newAP.ssid) {
        DEBUG_WIFI_MULTI("[WIFIM][APlistAdd] Fail newAP.ssid == 0\n");
        return false;
    }

    if (passphrase) {
        newAP.passphrase = strdup(passphrase);
    } else {
        newAP.passphrase = strdup("");
    }

    if (!newAP.passphrase) {
        DEBUG_WIFI_MULTI("[WIFIM][APlistAdd] Fail newAP.passphrase == 0\n");
        free(newAP.ssid);
        return false;
    }

    _APlist.push_back(newAP);
    DEBUG_WIFI_MULTI("[WIFIM][APlistAdd] Add SSID: %s\n", newAP.ssid);
    return true;
}

/**
 * @brief Check if AP exists in list
 * @param ssid
 *      WiFi SSID
 * @param passphrase
 *      WiFi Password
 * @return
 */
bool ESP8266WiFiMulti::APlistExists(const char *ssid, const char *passphrase)
{
    if (!ssid || (*ssid == 0x00) || (strlen(ssid) > 32)) {
        // Fail SSID too long or missing
        DEBUG_WIFI_MULTI("[WIFIM][APlistExists] No ssid or ssid too long\n");
        return false;
    }

    for (auto entry : _APlist) {
        if (!strcmp(entry.ssid, ssid)) {
            if (!passphrase) {
                if (!strcmp(entry.passphrase, "")) {
                    return true;
                }
            } else {
                if (!strcmp(entry.passphrase, passphrase)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief Remove all AP's from list
 */
void ESP8266WiFiMulti::APlistClean(void)
{
    // Remove all entries from APlist
    for (auto entry : _APlist) {
        if (entry.ssid) {
            free(entry.ssid);
        }
        if (entry.passphrase) {
            free(entry.passphrase);
        }
    }

    _APlist.clear();
}

/**
 * @brief Print WiFi scan results
 * @details
 *      Macro DEBUG_ESP_WIFI and DEBUG_ESP_PORT must be configured
 */
void ESP8266WiFiMulti::printWiFiScan()
{
#ifdef DEBUG_ESP_WIFI
    String ssid;
    int32_t rssi;
    uint8_t encryptionType;
    uint8_t* bssid;
    int32_t channel;
    bool hidden;
    int8_t scanResult;

    scanResult = WiFi.scanComplete();

    DEBUG_WIFI_MULTI("[WIFIM] %d networks found:\n", scanResult);

    // Print unsorted scan results
    for (int8_t i = 0; i < scanResult; i++) {
        bool known = false;

        WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);

        for(auto entry : _APlist) {
            if(ssid == entry.ssid) {
                // SSID match
                known = true;
            }
        }

        if (known) {
            DEBUG_WIFI_MULTI(" --->");
        } else {
            DEBUG_WIFI_MULTI("     ");
        }

        DEBUG_WIFI_MULTI(" %d: [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm %c %s\n",
                         i,
                         channel,
                         bssid[0], bssid[1], bssid[2],
                         bssid[3], bssid[4], bssid[5],
                         rssi,
                         (encryptionType == ENC_TYPE_NONE) ? ' ' : '*',
                         ssid.c_str());
        delay(0);
    }
#endif
}
