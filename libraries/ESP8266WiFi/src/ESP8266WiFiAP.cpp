/*
 ESP8266WiFiSTA.cpp - WiFi library for esp8266

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
#include "ESP8266WiFiAP.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
}

#include "debug.h"



// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- Private functions ------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

static bool softap_config_equal(const softap_config& lhs, const softap_config& rhs);



/**
 * compare two AP configurations
 * @param lhs softap_config
 * @param rhs softap_config
 * @return equal
 */
static bool softap_config_equal(const softap_config& lhs, const softap_config& rhs) {
    if(strcmp(reinterpret_cast<const char*>(lhs.ssid), reinterpret_cast<const char*>(rhs.ssid)) != 0) {
        return false;
    }
    if(strcmp(reinterpret_cast<const char*>(lhs.password), reinterpret_cast<const char*>(rhs.password)) != 0) {
        return false;
    }
    if(lhs.channel != rhs.channel) {
        return false;
    }
    if(lhs.ssid_hidden != rhs.ssid_hidden) {
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------- AP function -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------


/**
 * Set up an access point
 * @param ssid          Pointer to the SSID (max 63 char).
 * @param passphrase    (for WPA2 min 8 char, for open use NULL)
 * @param channel       WiFi channel number, 1 - 13.
 * @param ssid_hidden   Network cloaking (0 = broadcast SSID, 1 = hide SSID)
 */
bool ESP8266WiFiAPClass::softAP(const char* ssid, const char* passphrase, int channel, int ssid_hidden) {

    if(!WiFi.enableAP(true)) {
        // enable AP failed
        return false;
    }

    if(!ssid || *ssid == 0 || strlen(ssid) > 31) {
        // fail SSID too long or missing!
        return false;
    }

    if(passphrase && (strlen(passphrase) > 63 || strlen(passphrase) < 8)) {
        // fail passphrase to long or short!
        return false;
    }

    struct softap_config conf;
    strcpy(reinterpret_cast<char*>(conf.ssid), ssid);
    conf.channel = channel;
    conf.ssid_len = strlen(ssid);
    conf.ssid_hidden = ssid_hidden;
    conf.max_connection = 4;
    conf.beacon_interval = 100;

    if(!passphrase || strlen(passphrase) == 0) {
        conf.authmode = AUTH_OPEN;
        *conf.password = 0;
    } else {
        conf.authmode = AUTH_WPA2_PSK;
        strcpy(reinterpret_cast<char*>(conf.password), passphrase);
    }

    struct softap_config conf_current;
    wifi_softap_get_config(&conf_current);
    if(softap_config_equal(conf, conf_current)) {
        DEBUGV("softap config unchanged");
        return true;
    }

    bool ret;

    ETS_UART_INTR_DISABLE();
    if(WiFi._persistent) {
        ret = wifi_softap_set_config(&conf);
    } else {
        ret = wifi_softap_set_config_current(&conf);
    }
    ETS_UART_INTR_ENABLE();

    return ret;
}


/**
 * Configure access point
 * @param local_ip      access point IP
 * @param gateway       gateway IP
 * @param subnet        subnet mask
 */
bool ESP8266WiFiAPClass::softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet) {

    if(!WiFi.enableAP(true)) {
        // enable AP failed
        return false;
    }

    struct ip_info info;
    info.ip.addr = static_cast<uint32_t>(local_ip);
    info.gw.addr = static_cast<uint32_t>(gateway);
    info.netmask.addr = static_cast<uint32_t>(subnet);
    wifi_softap_dhcps_stop();
    if(wifi_set_ip_info(SOFTAP_IF, &info)) {
        return wifi_softap_dhcps_start();
    }
    return false;
}



/**
 * Disconnect from the network (close AP)
 * @param wifioff disable mode?
 * @return one value of wl_status_t enum
 */
bool ESP8266WiFiAPClass::softAPdisconnect(bool wifioff) {
    bool ret;
    struct softap_config conf;
    *conf.ssid = 0;
    *conf.password = 0;
    ETS_UART_INTR_DISABLE();
    if(WiFi._persistent) {
        ret = wifi_softap_set_config(&conf);
    } else {
        ret = wifi_softap_set_config_current(&conf);
    }
    ETS_UART_INTR_ENABLE();

    if(wifioff) {
        ret = WiFi.enableAP(false);
    }

    return ret;
}


/**
 * Get the count of the Station / client that are connected to the softAP interface
 * @return Stations count
 */
uint8_t ESP8266WiFiAPClass::softAPgetStationNum() {
    return wifi_softap_get_station_num();
}

/**
 * Get the softAP interface IP address.
 * @return IPAddress softAP IP
 */
IPAddress ESP8266WiFiAPClass::softAPIP() {
    struct ip_info ip;
    wifi_get_ip_info(SOFTAP_IF, &ip);
    return IPAddress(ip.ip.addr);
}


/**
 * Get the softAP interface MAC address.
 * @param mac   pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
 * @return      pointer to uint8_t*
 */
uint8_t* ESP8266WiFiAPClass::softAPmacAddress(uint8_t* mac) {
    wifi_get_macaddr(SOFTAP_IF, mac);
    return mac;
}

/**
 * Get the softAP interface MAC address.
 * @return String mac
 */
String ESP8266WiFiAPClass::softAPmacAddress(void) {
    uint8_t mac[6];
    char macStr[18] = { 0 };
    wifi_get_macaddr(SOFTAP_IF, mac);

    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}
