/**
 *
 * @file ESP8266WiFiMulti.cpp
 * @date 16.05.2015
 * @author Markus Sattler
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
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

#include "ESP8266WiFi.h"
#include "ESP8266WiFiMulti.h"
#include <limits.h>

ESP8266WiFiMulti::ESP8266WiFiMulti() {
}

ESP8266WiFiMulti::~ESP8266WiFiMulti() {
    APlistClean();
}

bool ESP8266WiFiMulti::addAP(const char* ssid, const char *passphrase) {
    return APlistAdd(ssid, passphrase);
}

wl_status_t ESP8266WiFiMulti::run(void) {

    wl_status_t status = WiFi.status();
    if(status == WL_DISCONNECTED || status == WL_NO_SSID_AVAIL || status == WL_IDLE_STATUS || status == WL_CONNECT_FAILED) {

        WifiAPlist_t bestNetwork { NULL, NULL };
        int bestNetworkDb = INT_MIN;

        // WiFi.scanNetworks will return the number of networks found
        int8_t n = WiFi.scanNetworks();

        DEBUG_WIFI_MULTI("[WIFI] scan done\n");
        delay(0);

        if(n <= 0) {
            DEBUG_WIFI_MULTI("[WIFI] no networks found\n");
        } else {
            DEBUG_WIFI_MULTI("[WIFI] %d networks found\n", n);
            for(int8_t i = 0; i < n; ++i) {
                const char * ssid_scan = WiFi.SSID(i);
                int32_t rssi_scan = WiFi.RSSI(i);
                uint8_t sec_scan = WiFi.encryptionType(i);

                bool known = false;
                for(uint32_t x = 0; x < APlist.size(); x++) {
                    WifiAPlist_t entry = APlist[x];

                    if(strcmp(entry.ssid, ssid_scan) == 0) { // SSID match
                        known = true;
                        if(rssi_scan > bestNetworkDb) { // best network
                            if(sec_scan == ENC_TYPE_NONE || entry.passphrase) { // check for passphrase if not open wlan
                                bestNetworkDb = rssi_scan;
                                memcpy((void*) &bestNetwork, (void*) &entry, sizeof(bestNetwork));
                            }
                        }
                        break;
                    }
                }
                if(known) {
                    DEBUG_WIFI_MULTI(" ---> ");
                } else {
                    DEBUG_WIFI_MULTI("      ");
                }

                DEBUG_WIFI_MULTI(" %d: %s (%d) %c\n", i, ssid_scan, rssi_scan, (sec_scan == ENC_TYPE_NONE) ? ' ' : '*');
                delay(0);
            }
        }

        DEBUG_WIFI_MULTI("\n\n");
        delay(0);

        if(bestNetwork.ssid) {
            DEBUG_WIFI_MULTI("[WIFI] Connecting SSID: %s (%d)\n", bestNetwork.ssid, bestNetworkDb);

            WiFi.begin(bestNetwork.ssid, bestNetwork.passphrase);
            status = WiFi.status();

            // wait for connection or fail
            while(status != WL_CONNECTED && status != WL_NO_SSID_AVAIL && status != WL_CONNECT_FAILED) {
                delay(10);
                status = WiFi.status();
            }

            switch(status) {
                case WL_CONNECTED:
                    DEBUG_WIFI_MULTI("[WIFI] Connecting Done.\n");
                    break;
                case WL_NO_SSID_AVAIL:
                    DEBUG_WIFI_MULTI("[WIFI] Connecting Faild AP not found.\n");
                    break;
                case WL_CONNECT_FAILED:
                    DEBUG_WIFI_MULTI("[WIFI] Connecting Faild.\n");
                    break;
                default:
                    DEBUG_WIFI_MULTI("[WIFI] Connecting Faild (%d).\n", status);
                    break;
            }
        } else {
            DEBUG_WIFI_MULTI("[WIFI] no matching wifi found!\n");
        }
    }
    return status;
}

// ##################################################################################

bool ESP8266WiFiMulti::APlistAdd(const char* ssid, const char *passphrase) {

    WifiAPlist_t newAP;

    newAP.ssid = (char*) malloc(strlen(ssid));

    if(!newAP.ssid) {
        return false;
    }

    strcpy(newAP.ssid, ssid);

    if(passphrase && *passphrase != 0x00) {
        newAP.passphrase = (char*) malloc(strlen(passphrase));
    }

    if(!newAP.passphrase) {
        free(newAP.ssid);
        return false;
    }

    strcpy(newAP.passphrase, passphrase);

    APlist.push_back(newAP);
    return true;
}

void ESP8266WiFiMulti::APlistClean(void) {
    for(uint32_t i = 0; i < APlist.size(); i++) {
        WifiAPlist_t entry = APlist[i];
        if(entry.ssid) {
            free(entry.ssid);
        }
        if(entry.passphrase) {
            free(entry.passphrase);
        }
    }
    APlist.clear();
}

