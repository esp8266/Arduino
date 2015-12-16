/*
  ESP8266WiFi.cpp - WiFi library for esp8266

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
*/

#include "ESP8266WiFi.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/dns.h"
}

#include "WiFiClient.h"
#include "WiFiUdp.h"
#include "debug.h"

extern "C" void esp_schedule();
extern "C" void esp_yield();

ESP8266WiFiClass::ESP8266WiFiClass()
: _smartConfigStarted(false)
, _smartConfigDone(false)
, _useStaticIp(false)
, _persistent(true)
{
    uint8 m = wifi_get_opmode();
    _useClientMode = (m & WIFI_STA);
    _useApMode = (m & WIFI_AP);
    wifi_set_event_handler_cb((wifi_event_handler_cb_t)&ESP8266WiFiClass::_eventCallback);
}

void ESP8266WiFiClass::persistent(bool persistent)
{
    _persistent = persistent;
}


void ESP8266WiFiClass::mode(WiFiMode m)
{
    if(wifi_get_opmode() == (uint8)m) {
        return;
    }

    if((m & WIFI_AP)) {
        _useApMode = true;
    } else {
        _useApMode = false;
    }

    if((m & WIFI_STA)) {
        _useClientMode = true;
    } else {
        _useClientMode = false;
    }

    _mode(m);
}

WiFiMode ESP8266WiFiClass::getMode()
{
    return (WiFiMode)wifi_get_opmode();
}

void ESP8266WiFiClass::_mode(WiFiMode m)
{
    if(wifi_get_opmode() == (uint8)m) {
        return;
    }

    ETS_UART_INTR_DISABLE();
    if (_persistent)
        wifi_set_opmode(m);
    else
        wifi_set_opmode_current(m);
    ETS_UART_INTR_ENABLE();

}

static bool sta_config_equal(const station_config& lhs, const station_config& rhs)
{
    if (strcmp(reinterpret_cast<const char*>(lhs.ssid), reinterpret_cast<const char*>(rhs.ssid)) != 0)
        return false;

    if (strcmp(reinterpret_cast<const char*>(lhs.password), reinterpret_cast<const char*>(rhs.password)) != 0)
        return false;

    if (lhs.bssid_set) {
        if (!rhs.bssid_set)
            return false;

        if (memcmp(lhs.bssid, rhs.bssid, 6) != 0)
            return false;
    }
    else {
        if (rhs.bssid_set)
            return false;
    }

    return true;
}

int ESP8266WiFiClass::begin(char* ssid, char *passphrase, int32_t channel, const uint8_t* bssid)
{
    return begin((const char*) ssid, (const char*) passphrase, channel, bssid);
}

int ESP8266WiFiClass::begin(const char* ssid, const char *passphrase, int32_t channel, const uint8_t* bssid)
{
    _useClientMode = true;

    if(_useApMode) {
        // turn on AP+STA mode
        _mode(WIFI_AP_STA);
    } else {
        // turn on STA mode
        _mode(WIFI_STA);
    }

    if(!ssid || *ssid == 0x00 || strlen(ssid) > 31) {
        // fail SSID too long or missing!
        return WL_CONNECT_FAILED;
    }

    if(passphrase && strlen(passphrase) > 63) {
        // fail passphrase too long!
        return WL_CONNECT_FAILED;
    }

    struct station_config conf;
    strcpy(reinterpret_cast<char*>(conf.ssid), ssid);

    if (passphrase) {
        strcpy(reinterpret_cast<char*>(conf.password), passphrase);
    } else {
        *conf.password = 0;
    }

    if (bssid) {
        conf.bssid_set = 1;
        memcpy((void *) &conf.bssid[0], (void *) bssid, 6);
    } else {
        conf.bssid_set = 0;
    }

    struct station_config current_conf;
    wifi_station_get_config(&current_conf);
    if (sta_config_equal(current_conf, conf)) {
        DEBUGV("sta config unchanged");
        return status();
    }

    ETS_UART_INTR_DISABLE();
    if (_persistent)
        wifi_station_set_config(&conf);
    else
        wifi_station_set_config_current(&conf);
    wifi_station_connect();
    ETS_UART_INTR_ENABLE();

    if(channel > 0 && channel <= 13) {
        wifi_set_channel(channel);
    }

    if(!_useStaticIp)
        wifi_station_dhcpc_start();
    return status();
}

int ESP8266WiFiClass::begin()
{
    ETS_UART_INTR_DISABLE();
    wifi_station_connect();
    ETS_UART_INTR_ENABLE();

    if(!_useStaticIp)
        wifi_station_dhcpc_start();
    return status();
}

uint8_t ESP8266WiFiClass::waitForConnectResult(){
  if ((wifi_get_opmode() & 1) == 0)//1 and 3 have STA enabled
      return WL_DISCONNECTED;
  while (status() == WL_DISCONNECTED)
    delay(100);
  return status();
}


// You will have to set the DNS-Server manually later since this will not enable DHCP2
void ESP8266WiFiClass::config(IPAddress local_ip, IPAddress gateway, IPAddress subnet)
{
    struct ip_info info;
    info.ip.addr = static_cast<uint32_t>(local_ip);
    info.gw.addr = static_cast<uint32_t>(gateway);
    info.netmask.addr = static_cast<uint32_t>(subnet);

    wifi_station_dhcpc_stop();
    wifi_set_ip_info(STATION_IF, &info);

    _useStaticIp = true;
}

void ESP8266WiFiClass::config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns)
{
    struct ip_info info;
    info.ip.addr = static_cast<uint32_t>(local_ip);
    info.gw.addr = static_cast<uint32_t>(gateway);
    info.netmask.addr = static_cast<uint32_t>(subnet);

    wifi_station_dhcpc_stop();
    wifi_set_ip_info(STATION_IF, &info);

    // Set DNS-Server
    ip_addr_t d;
    d.addr = static_cast<uint32_t>(dns);
    dns_setserver(0,&d);

    _useStaticIp = true;
}

int ESP8266WiFiClass::softAPdisconnect(bool wifioff)
{
    struct softap_config conf;
    *conf.ssid = 0;
    *conf.password = 0;
    ETS_UART_INTR_DISABLE();
    if (_persistent)
        wifi_softap_set_config(&conf);
    else
        wifi_softap_set_config_current(&conf);
    ETS_UART_INTR_ENABLE();

    if(wifioff) {
        _useApMode = false;

        if( _useClientMode) {
            // turn on STA
            _mode(WIFI_STA);
        } else {
            // turn wifi off
            _mode(WIFI_OFF);
        }
    }

    return 0;
}

int ESP8266WiFiClass::disconnect(bool wifioff)
{
    struct station_config conf;
    *conf.ssid = 0;
    *conf.password = 0;
    ETS_UART_INTR_DISABLE();
    if (_persistent)
        wifi_station_set_config(&conf);
    else
        wifi_station_set_config_current(&conf);
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();

    if(wifioff) {
        _useClientMode = false;

        if(_useApMode) {
            // turn on AP
            _mode(WIFI_AP);
        } else {
            // turn wifi off
            _mode(WIFI_OFF);
        }
    }

    return 0;
}

static bool softap_config_equal(const softap_config& lhs, const softap_config& rhs)
{
    if (strcmp(reinterpret_cast<const char*>(lhs.ssid), reinterpret_cast<const char*>(rhs.ssid)) != 0)
        return false;
    if (strcmp(reinterpret_cast<const char*>(lhs.password), reinterpret_cast<const char*>(rhs.password)) != 0)
        return false;
    if (lhs.channel != rhs.channel)
        return false;
    if (lhs.ssid_hidden != rhs.ssid_hidden)
        return false;
    return true;
}


void ESP8266WiFiClass::softAP(const char* ssid)
{
    softAP(ssid, 0);
}


void ESP8266WiFiClass::softAP(const char* ssid, const char* passphrase, int channel, int ssid_hidden)
{
    _useApMode = true;
    if(_useClientMode) {
        // turn on AP+STA mode
        _mode(WIFI_AP_STA);
    } else {
        // turn on STA mode
        _mode(WIFI_AP);
    }

    if(!ssid || *ssid == 0 || strlen(ssid) > 31) {
        // fail SSID too long or missing!
        return;
    }

    if(passphrase && strlen(passphrase) > 63) {
        // fail passphrase to long!
        return;
    }

    struct softap_config conf;
    wifi_softap_get_config(&conf);
    strcpy(reinterpret_cast<char*>(conf.ssid), ssid);
    conf.channel = channel;
    conf.ssid_len = strlen(ssid);
    conf.ssid_hidden = ssid_hidden;
    conf.max_connection = 4;
    conf.beacon_interval = 100;

    if (!passphrase || strlen(passphrase) == 0)
    {
        conf.authmode = AUTH_OPEN;
        *conf.password = 0;
    }
    else
    {
        conf.authmode = AUTH_WPA2_PSK;
        strcpy(reinterpret_cast<char*>(conf.password), passphrase);
    }

    struct softap_config conf_current;
    wifi_softap_get_config(&conf_current);
    if (softap_config_equal(conf, conf_current))
    {
        DEBUGV("softap config unchanged");
        return;
    }

    ETS_UART_INTR_DISABLE();
    if (_persistent)
        wifi_softap_set_config(&conf);
    else
        wifi_softap_set_config_current(&conf);
    ETS_UART_INTR_ENABLE();
}

void ESP8266WiFiClass::softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet)
{
    struct ip_info info;
    info.ip.addr = static_cast<uint32_t>(local_ip);
    info.gw.addr = static_cast<uint32_t>(gateway);
    info.netmask.addr = static_cast<uint32_t>(subnet);
    wifi_softap_dhcps_stop();
    wifi_set_ip_info(SOFTAP_IF, &info);
    wifi_softap_dhcps_start();
}

uint8_t* ESP8266WiFiClass::macAddress(uint8_t* mac)
{
    wifi_get_macaddr(STATION_IF, mac);
    return mac;
}

String ESP8266WiFiClass::macAddress(void)
{
    uint8_t mac[6];
    char macStr[18] = {0};
    wifi_get_macaddr(STATION_IF, mac);

    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

uint8_t* ESP8266WiFiClass::softAPmacAddress(uint8_t* mac)
{
    wifi_get_macaddr(SOFTAP_IF, mac);
    return mac;
}

String ESP8266WiFiClass::softAPmacAddress(void)
{
    uint8_t mac[6];
    char macStr[18] = {0};
    wifi_get_macaddr(SOFTAP_IF, mac);

    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

IPAddress ESP8266WiFiClass::localIP()
{
    struct ip_info ip;
    wifi_get_ip_info(STATION_IF, &ip);
    return IPAddress(ip.ip.addr);
}

IPAddress ESP8266WiFiClass::softAPIP()
{
    struct ip_info ip;
    wifi_get_ip_info(SOFTAP_IF, &ip);
    return IPAddress(ip.ip.addr);
}

IPAddress ESP8266WiFiClass::subnetMask()
{
    struct ip_info ip;
    wifi_get_ip_info(STATION_IF, &ip);
    return IPAddress(ip.netmask.addr);
}

IPAddress ESP8266WiFiClass::gatewayIP()
{
    struct ip_info ip;
    wifi_get_ip_info(STATION_IF, &ip);
    return IPAddress(ip.gw.addr);
}

IPAddress ESP8266WiFiClass::dnsIP(int dns_no)
{
    ip_addr_t dns_ip = dns_getserver(dns_no);
    return IPAddress(dns_ip.addr);
}

String ESP8266WiFiClass::SSID() const
{
    static struct station_config conf;
    wifi_station_get_config(&conf);
    return String(reinterpret_cast<char*>(conf.ssid));
}

String ESP8266WiFiClass::psk() const
{
    static struct station_config conf;
    wifi_station_get_config(&conf);
		return String(reinterpret_cast<char*>(conf.password));
}

uint8_t* ESP8266WiFiClass::BSSID(void)
{
    static struct station_config conf;
    wifi_station_get_config(&conf);
    return reinterpret_cast<uint8_t*>(conf.bssid);
}

String ESP8266WiFiClass::BSSIDstr(void)
{
    static struct station_config conf;
    char mac[18] = {0};
    wifi_station_get_config(&conf);
    sprintf(mac,"%02X:%02X:%02X:%02X:%02X:%02X", conf.bssid[0],  conf.bssid[1],  conf.bssid[2], conf.bssid[3], conf.bssid[4], conf.bssid[5]);
    return String(mac);
}


int32_t ESP8266WiFiClass::channel(void) {
    return wifi_get_channel();
}


int32_t ESP8266WiFiClass::RSSI(void) {
    return wifi_station_get_rssi();
}

extern "C"
{
    typedef STAILQ_HEAD(, bss_info) bss_info_head_t;
}

void ESP8266WiFiClass::_scanDone(void* result, int status)
{
    if (status != OK)
    {
        ESP8266WiFiClass::_scanCount = 0;
        ESP8266WiFiClass::_scanResult = 0;
    }
    else
    {

        int i = 0;
        bss_info_head_t* head = reinterpret_cast<bss_info_head_t*>(result);

        for (bss_info* it = STAILQ_FIRST(head); it; it = STAILQ_NEXT(it, next), ++i);
        ESP8266WiFiClass::_scanCount = i;
        if (i == 0)
        {
            ESP8266WiFiClass::_scanResult = 0;
        }
        else
        {
            bss_info* copied_info = new bss_info[i];
            i = 0;
            for (bss_info* it = STAILQ_FIRST(head); it; it = STAILQ_NEXT(it, next), ++i)
            {
                memcpy(copied_info + i, it, sizeof(bss_info));
            }

            ESP8266WiFiClass::_scanResult = copied_info;
        }

    }

    ESP8266WiFiClass::_scanStarted = false;
    ESP8266WiFiClass::_scanComplete = true;

    if(!ESP8266WiFiClass::_scanAsync) {
        esp_schedule();
    }
}

int8_t ESP8266WiFiClass::scanComplete() {

    if(_scanStarted) {
        return WIFI_SCAN_RUNNING;
    }

    if(_scanComplete) {
        return ESP8266WiFiClass::_scanCount;
    }

    return WIFI_SCAN_FAILED;
}

void ESP8266WiFiClass::scanDelete()
{
    if (ESP8266WiFiClass::_scanResult)
    {
        delete[] reinterpret_cast<bss_info*>(ESP8266WiFiClass::_scanResult);
        ESP8266WiFiClass::_scanResult = 0;
        ESP8266WiFiClass::_scanCount = 0;
    }
    _scanComplete = false;
}

int8_t ESP8266WiFiClass::scanNetworks(bool async, bool show_hidden)
{
    if(ESP8266WiFiClass::_scanStarted) {
        return WIFI_SCAN_RUNNING;
    }

    ESP8266WiFiClass::_scanAsync = async;

    if(_useApMode) {
        // turn on AP+STA mode
        _mode(WIFI_AP_STA);
    } else {
        // turn on STA mode
        _mode(WIFI_STA);
    }

    int status = wifi_station_get_connect_status();
    if (status != STATION_GOT_IP && status != STATION_IDLE)
    {
        disconnect();
    }

    scanDelete();

    struct scan_config config;
    config.ssid = 0;
    config.bssid = 0;
    config.channel = 0;
    config.show_hidden = show_hidden;
    if(wifi_station_scan(&config, reinterpret_cast<scan_done_cb_t>(&ESP8266WiFiClass::_scanDone))) {
        ESP8266WiFiClass::_scanComplete = false;
        ESP8266WiFiClass::_scanStarted = true;

        if(ESP8266WiFiClass::_scanAsync) {
            delay(0); // time for the OS to trigger the scan
            return WIFI_SCAN_RUNNING;
        }

        esp_yield();
        return ESP8266WiFiClass::_scanCount;
    } else {
        return WIFI_SCAN_FAILED;
    }

}

void * ESP8266WiFiClass::_getScanInfoByIndex(int i)
{
    if (!ESP8266WiFiClass::_scanResult || (size_t)i > ESP8266WiFiClass::_scanCount)
    {
        return 0;
    }

    return reinterpret_cast<bss_info*>(ESP8266WiFiClass::_scanResult) + i;
}

String ESP8266WiFiClass::SSID(uint8_t i)
{
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if (!it)
        return "";

    return String(reinterpret_cast<const char*>(it->ssid));
}

uint8_t * ESP8266WiFiClass::BSSID(uint8_t i)
{
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if (!it)
        return 0;

    return it->bssid;
}

String ESP8266WiFiClass::BSSIDstr(uint8_t i)
{
    char mac[18] = {0};
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if (!it)
        return String("");

    sprintf(mac,"%02X:%02X:%02X:%02X:%02X:%02X", it->bssid[0],  it->bssid[1],  it->bssid[2], it->bssid[3], it->bssid[4], it->bssid[5]);
    return String(mac);
}

int32_t ESP8266WiFiClass::channel(uint8_t i)
{
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if (!it)
        return 0;

    return it->channel;
}

bool ESP8266WiFiClass::isHidden(uint8_t i)
{
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if (!it)
        return false;

    return (it->is_hidden != 0);
}

bool ESP8266WiFiClass::getNetworkInfo(uint8_t i, String &ssid, uint8_t &encType, int32_t &rssi, uint8_t* &bssid, int32_t &channel, bool &isHidden)
{
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
     if (!it)
         return false;

     ssid = (const char*)it->ssid;
     encType = encryptionType(i);
     rssi = it->rssi;
     bssid = it->bssid; // move ptr
     channel = it->channel;
     isHidden = (it->is_hidden != 0);

     return true;
}

int32_t ESP8266WiFiClass::RSSI(uint8_t i)
{
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if (!it)
        return 0;

    return it->rssi;
}

uint8_t ESP8266WiFiClass::encryptionType(uint8_t i)
{
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if (!it)
        return -1;

    int authmode = it->authmode;
    if (authmode == AUTH_OPEN)
        return ENC_TYPE_NONE;
    if (authmode == AUTH_WEP)
        return ENC_TYPE_WEP;
    if (authmode == AUTH_WPA_PSK)
        return ENC_TYPE_TKIP;
    if (authmode == AUTH_WPA2_PSK)
        return ENC_TYPE_CCMP;
    if (authmode == AUTH_WPA_WPA2_PSK)
        return ENC_TYPE_AUTO;
    return -1;
}

wl_status_t ESP8266WiFiClass::status()
{
    int status = wifi_station_get_connect_status();

    if (status == STATION_GOT_IP)
      return WL_CONNECTED;
    else if (status == STATION_NO_AP_FOUND)
      return WL_NO_SSID_AVAIL;
    else if (status == STATION_CONNECT_FAIL || status == STATION_WRONG_PASSWORD)
      return WL_CONNECT_FAILED;
    else if (status == STATION_IDLE)
      return WL_IDLE_STATUS;
    else
      return WL_DISCONNECTED;
}

void wifi_dns_found_callback(const char *name, ip_addr_t *ipaddr, void *callback_arg)
{
    if (ipaddr)
        (*reinterpret_cast<IPAddress*>(callback_arg)) = ipaddr->addr;
    esp_schedule(); // resume the hostByName function
}

int ESP8266WiFiClass::hostByName(const char* aHostname, IPAddress& aResult)
{
    ip_addr_t addr;
    aResult = static_cast<uint32_t>(0);
    err_t err = dns_gethostbyname(aHostname, &addr, &wifi_dns_found_callback, &aResult);
    if (err == ERR_OK)
    {
        aResult = addr.addr;
    }
    else if (err == ERR_INPROGRESS)
    {
        esp_yield();
        // will return here when dns_found_callback fires
    }

    return (aResult != 0) ? 1 : 0;
}

String ESP8266WiFiClass::hostname(void) {
    return String(wifi_station_get_hostname());
}

bool ESP8266WiFiClass::hostname(char* aHostname) {
    if(strlen(aHostname) > 32) {
        return false;
    }
    return wifi_station_set_hostname(aHostname);
}

bool ESP8266WiFiClass::hostname(const char* aHostname) {
    return hostname((char*) aHostname);
}

bool ESP8266WiFiClass::hostname(String aHostname) {
    return hostname((char*) aHostname.c_str());
}

//--------------------------------------------------------------

void wifi_wps_status_cb(wps_cb_status status)
{
    DEBUGV("wps cb status: %d\r\n", status);
    switch (status) {
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
    }
    // todo user function to get status

    esp_schedule(); // resume the beginWPSConfig function
}

bool ESP8266WiFiClass::beginWPSConfig(void) {

    _useClientMode = true;

     if(_useApMode) {
         // turn on AP+STA mode
         _mode(WIFI_AP_STA);
     } else {
         // turn on STA mode
         _mode(WIFI_STA);
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

    esp_yield();
    // will return here when wifi_wps_status_cb fires

    return true;
}

//--------------------------------------------------------------



void ESP8266WiFiClass::beginSmartConfig()
{
    if (_smartConfigStarted)
        return;

    if(_useApMode) {
        // turn on AP+STA mode
        _mode(WIFI_AP_STA);
    } else {
        // turn on STA mode
        _mode(WIFI_STA);
    }

    _smartConfigStarted = true;
    _smartConfigDone = false;

    //SC_TYPE_ESPTOUCH use ESPTOUCH for smartconfig, or use SC_TYPE_AIRKISS for AIRKISS
    smartconfig_start(reinterpret_cast<sc_callback_t>(&ESP8266WiFiClass::_smartConfigCallback), 1);
}

void ESP8266WiFiClass::stopSmartConfig()
{
    if (!_smartConfigStarted)
        return;

    smartconfig_stop();
    _smartConfigStarted = false;
}

bool ESP8266WiFiClass::smartConfigDone()
{
    if (!_smartConfigStarted)
        return false;

    return _smartConfigDone;
}

void ESP8266WiFiClass::_smartConfigCallback(uint32_t st, void* result)
{
    sc_status status = (sc_status) st;
    if (status == SC_STATUS_LINK) {
        station_config* sta_conf = reinterpret_cast<station_config*>(result);

        wifi_station_set_config(sta_conf);
        wifi_station_disconnect();
        wifi_station_connect();

        WiFi._smartConfigDone = true;
    }
    else if (status == SC_STATUS_LINK_OVER) {
        WiFi.stopSmartConfig();
    }
}

//--------------------------------------------------------------

void ESP8266WiFiClass::_eventCallback(void* arg)
{
    System_Event_t* event = reinterpret_cast<System_Event_t*>(arg);
    DEBUGV("wifi evt: %d\r\n", event->event);

    if (event->event == EVENT_STAMODE_DISCONNECTED) {
        WiFiClient::stopAll();
    }
}

void ESP8266WiFiClass::printDiag(Print& p)
{
    const char* modes[] = {"NULL", "STA", "AP", "STA+AP"};
    p.print("Mode: ");
    p.println(modes[wifi_get_opmode()]);

    const char* phymodes[] = {"", "B", "G", "N"};
    p.print("PHY mode: ");
    p.println(phymodes[(int) wifi_get_phy_mode()]);

    p.print("Channel: ");
    p.println(wifi_get_channel());

    p.print("AP id: ");
    p.println(wifi_station_get_current_ap_id());

    p.print("Status: ");
    p.println(wifi_station_get_connect_status());

    p.print("Auto connect: ");
    p.println(wifi_station_get_auto_connect());

    static struct station_config conf;
    wifi_station_get_config(&conf);

    const char* ssid = reinterpret_cast<const char*>(conf.ssid);
    p.print("SSID (");
    p.print(strlen(ssid));
    p.print("): ");
    p.println(ssid);

    const char* passphrase = reinterpret_cast<const char*>(conf.password);
    p.print("Passphrase (");
    p.print(strlen(passphrase));
    p.print("): ");
    p.println(passphrase);

    p.print("BSSID set: ");
    p.println(conf.bssid_set);

}

bool ESP8266WiFiClass::_scanAsync = false;
bool ESP8266WiFiClass::_scanStarted = false;
bool ESP8266WiFiClass::_scanComplete = false;

size_t ESP8266WiFiClass::_scanCount = 0;
void* ESP8266WiFiClass::_scanResult = 0;


ESP8266WiFiClass WiFi;
