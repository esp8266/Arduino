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

#include "ESP8266WiFiMulti.h"
#include <limits.h>
#include <string.h>

ESP8266WiFiMulti::ESP8266WiFiMulti() {
}

ESP8266WiFiMulti::~ESP8266WiFiMulti() {
    APlistClean();
}

bool ESP8266WiFiMulti::addAP(const char* ssid, const char *passphrase) {
    return APlistAdd(ssid, passphrase);
}

void ESP8266WiFiMulti::cleanAPlist(void) {
    APlistClean();
}

bool ESP8266WiFiMulti::existsAP(const char* ssid, const char *passphrase) {
    return APlistExists(ssid, passphrase);
}

wl_status_t ESP8266WiFiMulti::run(uint32_t connectTimeoutMs) {

    wl_status_t status = WiFi.status();
    if(status == WL_DISCONNECTED || status == WL_NO_SSID_AVAIL || status == WL_IDLE_STATUS || status == WL_CONNECT_FAILED) {

        int8_t scanResult = WiFi.scanComplete();

        if(scanResult == WIFI_SCAN_RUNNING) {
            // scan is running, do nothing yet
            status = WL_NO_SSID_AVAIL;
            return status;
        } 

        if(scanResult == 0) {
            // scan done, no ssids found. Start another scan.
            DEBUG_WIFI_MULTI("[WIFI] scan done\n");
            DEBUG_WIFI_MULTI("[WIFI] no networks found\n");
            WiFi.scanDelete();
            DEBUG_WIFI_MULTI("\n\n");
            delay(0);
            WiFi.disconnect();
            DEBUG_WIFI_MULTI("[WIFI] start scan\n");
            // scan wifi async mode
            WiFi.scanNetworks(true);
            return status;
        } 

        if(scanResult > 0) {
            // scan done, analyze
            WifiAPEntry bestNetwork { NULL, NULL };
            int bestNetworkDb = INT_MIN;
            uint8 bestBSSID[6];
            int32_t bestChannel;

            DEBUG_WIFI_MULTI("[WIFI] scan done\n");
            delay(0);

            DEBUG_WIFI_MULTI("[WIFI] %d networks found\n", scanResult);
            for(int8_t i = 0; i < scanResult; ++i) {

                String ssid_scan;
                int32_t rssi_scan;
                uint8_t sec_scan;
                uint8_t* BSSID_scan;
                int32_t chan_scan;
                bool hidden_scan;

                WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);

                bool known = false;
                for(auto entry : APlist) {
                    if(ssid_scan == entry.ssid) { // SSID match
                        known = true;
                        if(rssi_scan > bestNetworkDb) { // best network
                            if(sec_scan == ENC_TYPE_NONE || entry.passphrase) { // check for passphrase if not open wlan
                                bestNetworkDb = rssi_scan;
                                bestChannel = chan_scan;
                                bestNetwork = entry;
                                memcpy((void*) &bestBSSID, (void*) BSSID_scan, sizeof(bestBSSID));
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

                DEBUG_WIFI_MULTI(" %d: [%d][%02X:%02X:%02X:%02X:%02X:%02X] %s (%d) %c\n", i, chan_scan, BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], ssid_scan.c_str(), rssi_scan, (sec_scan == ENC_TYPE_NONE) ? ' ' : '*');
                delay(0);
            }

            // clean up ram
            WiFi.scanDelete();

            DEBUG_WIFI_MULTI("\n\n");
            delay(0);

            if(bestNetwork.ssid) {
                DEBUG_WIFI_MULTI("[WIFI] Connecting BSSID: %02X:%02X:%02X:%02X:%02X:%02X SSID: %s Channel: %d (%d)\n", bestBSSID[0], bestBSSID[1], bestBSSID[2], bestBSSID[3], bestBSSID[4], bestBSSID[5], bestNetwork.ssid, bestChannel, bestNetworkDb);

                WiFi.begin(bestNetwork.ssid, bestNetwork.passphrase, bestChannel, bestBSSID);
                status = WiFi.status();
                
                auto startTime = millis();
                // wait for connection, fail, or timeout
                while(status != WL_CONNECTED && status != WL_NO_SSID_AVAIL && status != WL_CONNECT_FAILED && (millis() - startTime) <= connectTimeoutMs) {
                    delay(10);
                    status = WiFi.status();
                }
                
#ifdef DEBUG_ESP_WIFI
                IPAddress ip;
                uint8_t * mac;
                switch(status) {
                    case WL_CONNECTED:
                        ip = WiFi.localIP();
                        mac = WiFi.BSSID();
                        DEBUG_WIFI_MULTI("[WIFI] Connecting done.\n");
                        DEBUG_WIFI_MULTI("[WIFI] SSID: %s\n", WiFi.SSID().c_str());
                        DEBUG_WIFI_MULTI("[WIFI] IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
                        DEBUG_WIFI_MULTI("[WIFI] MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                        DEBUG_WIFI_MULTI("[WIFI] Channel: %d\n", WiFi.channel());
                        break;
                    case WL_NO_SSID_AVAIL:
                        DEBUG_WIFI_MULTI("[WIFI] Connecting Failed AP not found.\n");
                        break;
                    case WL_CONNECT_FAILED:
                        DEBUG_WIFI_MULTI("[WIFI] Connecting Failed.\n");
                        break;
                    default:
                        DEBUG_WIFI_MULTI("[WIFI] Connecting Failed (%d).\n", status);
                        break;
                }
#endif
            } else {
                DEBUG_WIFI_MULTI("[WIFI] no matching wifi found!\n");
            }

            return status;
        }
       
       
        // scan failed, or some other condition not handled above. Start another scan.
        DEBUG_WIFI_MULTI("[WIFI] delete old wifi config...\n");
        WiFi.disconnect();

        DEBUG_WIFI_MULTI("[WIFI] start scan\n");
        // scan wifi async mode
        WiFi.scanNetworks(true);
    }
    return status;
}

// ##################################################################################

bool ESP8266WiFiMulti::APlistAdd(const char* ssid, const char *passphrase) {

    WifiAPEntry newAP;

    if(!ssid || *ssid == 0x00 || strlen(ssid) > 32) {
        // fail SSID too long or missing!
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] no ssid or ssid too long\n");
        return false;
    }

    //for passphrase, max is 63 ascii + null. For psk, 64hex + null.
    if(passphrase && strlen(passphrase) > 64) {
        // fail passphrase too long!
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] passphrase too long\n");
        return false;
    }

    if(APlistExists(ssid, passphrase)) {
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] SSID: %s already exists\n", ssid);
        return true;
    }

    newAP.ssid = strdup(ssid);

    if(!newAP.ssid) {
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] fail newAP.ssid == 0\n");
        return false;
    }

    if(passphrase) {
        newAP.passphrase = strdup(passphrase);
    } else {
        newAP.passphrase = strdup("");
    }

    if(!newAP.passphrase) {
        DEBUG_WIFI_MULTI("[WIFI][APlistAdd] fail newAP.passphrase == 0\n");
        free(newAP.ssid);
        return false;
    }

    APlist.push_back(newAP);
    DEBUG_WIFI_MULTI("[WIFI][APlistAdd] add SSID: %s\n", newAP.ssid);
    return true;
}

bool ESP8266WiFiMulti::APlistExists(const char* ssid, const char *passphrase) {
    if(!ssid || *ssid == 0x00 || strlen(ssid) > 32) {
        // fail SSID too long or missing!
        DEBUG_WIFI_MULTI("[WIFI][APlistExists] no ssid or ssid too long\n");
        return false;
    }
    for(auto entry : APlist) {
        if(!strcmp(entry.ssid, ssid)) {
            if(!passphrase) {
                if(!strcmp(entry.passphrase, "")) {
                    return true;
                }
            } else {
                if(!strcmp(entry.passphrase, passphrase)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void ESP8266WiFiMulti::APlistClean(void) {
    for(auto entry : APlist) {
        if(entry.ssid) {
            free(entry.ssid);
        }
        if(entry.passphrase) {
            free(entry.passphrase);
        }
    }
    APlist.clear();
}

