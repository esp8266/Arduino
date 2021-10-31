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
#include <lwip/init.h> // LWIP_VERSION_*
}

#include "debug.h"
#include "LwipDhcpServer.h"

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
    if(lhs.ssid_len != rhs.ssid_len) {
        return false;
    }
    if(memcmp(lhs.ssid, rhs.ssid, lhs.ssid_len) != 0) {
        return false;
    }
    if(strncmp(reinterpret_cast<const char*>(lhs.password), reinterpret_cast<const char*>(rhs.password), sizeof(softap_config::password)) != 0) {
        return false;
    }
    if(lhs.channel != rhs.channel) {
        return false;
    }
    if(lhs.ssid_hidden != rhs.ssid_hidden) {
        return false;
    }
    if(lhs.max_connection != rhs.max_connection) {
        return false;
    }
    if(lhs.beacon_interval != rhs.beacon_interval) {
        return false;
    }
    if(lhs.authmode != rhs.authmode) {
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------- AP function -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------


/**
 * Set up an access point
 * @param ssid              Pointer to the SSID (max 32 char).
 * @param psk               For WPA2 min 8 char max 64 char, for open use "" or NULL.
 * @param channel           WiFi channel number, 1 - 13.
 * @param ssid_hidden       Network cloaking (0 = broadcast SSID, 1 = hide SSID)
 * @param max_connection    Max simultaneous connected clients, 0 - 8. https://bbs.espressif.com/viewtopic.php?f=46&t=481&p=1832&hilit=max_connection#p1832
 */
bool ESP8266WiFiAPClass::softAP(const char* ssid, const char* psk, int channel, int ssid_hidden, int max_connection) {

    if(!WiFi.enableAP(true)) {
        // enable AP failed
        DEBUG_WIFI("[AP] enableAP failed!\n");
        return false;
    }

    size_t ssid_len = ssid ? strlen(ssid) : 0;
    if(ssid_len == 0 || ssid_len > 32) {
        DEBUG_WIFI("[AP] SSID length %d, too long or missing!\n", (int)ssid_len);
        return false;
    }

    size_t psk_len = psk ? strlen(psk) : 0;
    if(psk_len > 0 && (psk_len > 64 || psk_len < 8)) {
        DEBUG_WIFI("[AP] fail psk length %d, too long or short!\n", (int)psk_len);
        return false;
    }

    bool ret = true;

    struct softap_config conf;
    memcpy(reinterpret_cast<char*>(conf.ssid), ssid, ssid_len);
    if (ssid_len < sizeof(conf.ssid)) {
        conf.ssid[ssid_len] = 0;
    }
    conf.ssid_len = ssid_len;

    if(psk_len) {
        conf.authmode = AUTH_WPA2_PSK;
        memcpy(reinterpret_cast<char*>(conf.password), psk, psk_len);
        if (psk_len < sizeof(conf.password)) {
            conf.password[psk_len] = 0;
        }
    } else {
        conf.authmode = AUTH_OPEN;
        conf.password[0] = 0;
    }

    conf.channel = channel;
    conf.ssid_hidden = ssid_hidden;
    conf.max_connection = max_connection;
    conf.beacon_interval = 100;

    struct softap_config conf_compare;
    if(WiFi._persistent){
        wifi_softap_get_config_default(&conf_compare);
    }
    else {
        wifi_softap_get_config(&conf_compare);
    }

    if(!softap_config_equal(conf, conf_compare)) {

        ETS_UART_INTR_DISABLE();
        if(WiFi._persistent) {
            ret = wifi_softap_set_config(&conf);
        } else {
            ret = wifi_softap_set_config_current(&conf);
        }
        ETS_UART_INTR_ENABLE();

        if(!ret) {
            DEBUG_WIFI("[AP] set_config failed!\n");
            return false;
        }

    } else {
        DEBUG_WIFI("[AP] softap config unchanged\n");
    }

    dhcpSoftAP.end();

    // check IP config
    struct ip_info ip;
    if(wifi_get_ip_info(SOFTAP_IF, &ip)) {
        if(ip.ip.addr == 0x00000000) {
            // Invalid config
            DEBUG_WIFI("[AP] IP config Invalid resetting...\n");
            //192.168.4.1 , 192.168.4.1 , 255.255.255.0
            ret = softAPConfig(0x0104A8C0, 0x0104A8C0, 0x00FFFFFF);
            if(!ret) {
                DEBUG_WIFI("[AP] softAPConfig failed!\n");
                ret = false;
            }
        }
    } else {
        DEBUG_WIFI("[AP] wifi_get_ip_info failed!\n");
        ret = false;
    }

    dhcpSoftAP.begin(&ip);

    return ret;
}

bool ESP8266WiFiAPClass::softAP(const String& ssid, const String& psk, int channel, int ssid_hidden, int max_connection) {
    return softAP(ssid.c_str(), psk.c_str(), channel, ssid_hidden, max_connection);
}

/**
 * Configure access point
 * @param local_ip      access point IP
 * @param gateway       gateway IP (0.0.0.0 to disable)
 * @param subnet        subnet mask
 */
bool ESP8266WiFiAPClass::softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet) {
    DEBUG_WIFI("[APConfig] local_ip: %s gateway: %s subnet: %s\n", local_ip.toString().c_str(), gateway.toString().c_str(), subnet.toString().c_str());
    if(!WiFi.enableAP(true)) {
        // enable AP failed
        DEBUG_WIFI("[APConfig] enableAP failed!\n");
        return false;
    }
    bool ret = true;

    if (   !local_ip.isV4()
        || !subnet.isV4()
#if LWIP_IPV6
        // uninitialized gateway is valid
        || gateway.isV6()
#endif
       ) {
        return false;
    }
    struct ip_info info;
    info.ip.addr = local_ip.v4();
    info.gw.addr = gateway.v4();
    info.netmask.addr = subnet.v4();

    if(!wifi_softap_dhcps_stop()) {
        DEBUG_WIFI("[APConfig] wifi_softap_dhcps_stop failed!\n");
    }

    if(!wifi_set_ip_info(SOFTAP_IF, &info)) {
        DEBUG_WIFI("[APConfig] wifi_set_ip_info failed!\n");
        ret = false;
    }

    struct dhcps_lease dhcp_lease;
    dhcp_lease.enable = true;
    IPAddress ip = local_ip;
    ip[3] += 99;
    dhcp_lease.start_ip.addr = ip.v4();
    DEBUG_WIFI("[APConfig] DHCP IP start: %s\n", ip.toString().c_str());

    ip[3] += 100;
    dhcp_lease.end_ip.addr = ip.v4();
    DEBUG_WIFI("[APConfig] DHCP IP end: %s\n", ip.toString().c_str());

    if(!dhcpSoftAP.set_dhcps_lease(&dhcp_lease))
    {
        DEBUG_WIFI("[APConfig] wifi_set_ip_info failed!\n");
        ret = false;
    }

    // set lease time to 720min --> 12h
    if(!dhcpSoftAP.set_dhcps_lease_time(720))
    {
        DEBUG_WIFI("[APConfig] wifi_softap_set_dhcps_lease_time failed!\n");
        ret = false;
    }

    uint8 mode = info.gw.addr ? 1 : 0;
    if(!dhcpSoftAP.set_dhcps_offer_option(OFFER_ROUTER, &mode))
    {
        DEBUG_WIFI("[APConfig] wifi_softap_set_dhcps_offer_option failed!\n");
        ret = false;
    }

    if(!wifi_softap_dhcps_start()) {
        DEBUG_WIFI("[APConfig] wifi_softap_dhcps_start failed!\n");
        ret = false;
    }

    // check config
    if(wifi_get_ip_info(SOFTAP_IF, &info)) {
        if(info.ip.addr == 0x00000000) {
            DEBUG_WIFI("[APConfig] IP config Invalid?!\n");
            ret = false;
        } else if(local_ip.v4() != info.ip.addr) {
            DEBUG_WIFI("[APConfig] IP config not set correct?! new IP: %s\n", IPAddress(info.ip.addr).toString().c_str());
            ret = false;
        }
    } else {
        DEBUG_WIFI("[APConfig] wifi_get_ip_info failed!\n");
        ret = false;
    }

    return ret;
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
    conf.authmode = AUTH_OPEN;
    ETS_UART_INTR_DISABLE();
    if(WiFi._persistent) {
        ret = wifi_softap_set_config(&conf);
    } else {
        ret = wifi_softap_set_config_current(&conf);
    }
    ETS_UART_INTR_ENABLE();

    if(!ret) {
        DEBUG_WIFI("[APdisconnect] set_config failed!\n");
    }

    if(ret && wifioff) {
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

/**
 * Get the configured(Not-In-Flash) softAP SSID name.
 * @return String SSID.
 */
String ESP8266WiFiAPClass::softAPSSID() const {
    struct softap_config config;
    wifi_softap_get_config(&config);

    String ssid;
    ssid.concat(reinterpret_cast<const char*>(config.ssid), config.ssid_len);

    return ssid;
}

/**
 * Get the configured(Not-In-Flash) softAP PSK.
 * @return String psk.
 */
String ESP8266WiFiAPClass::softAPPSK() const {
    struct softap_config config;
    wifi_softap_get_config(&config);

    char* ptr = reinterpret_cast<char*>(config.password);
    String psk;
    psk.concat(ptr, strnlen(ptr, sizeof(config.password)));

    return psk;
}
