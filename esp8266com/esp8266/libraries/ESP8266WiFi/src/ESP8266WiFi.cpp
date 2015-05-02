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


extern "C" void esp_schedule();
extern "C" void esp_yield();

ESP8266WiFiClass::ESP8266WiFiClass()
{

}

void ESP8266WiFiClass::mode(WiFiMode m)
{
    ETS_UART_INTR_DISABLE();
    wifi_set_opmode(m);
    ETS_UART_INTR_ENABLE();
}


int ESP8266WiFiClass::begin(const char* ssid)
{
    return begin(ssid, 0);
}


int ESP8266WiFiClass::begin(const char* ssid, const char *passphrase)
{
    if ((wifi_get_opmode() & 1) == 0)//1 and 3 have STA enabled
    {
        // turn on AP+STA mode
        mode(WIFI_AP_STA);
    }

    struct station_config conf;
    strcpy(reinterpret_cast<char*>(conf.ssid), ssid);
    if (passphrase)
        strcpy(reinterpret_cast<char*>(conf.password), passphrase);
    else
        *conf.password = 0;

    conf.bssid_set = 0;

    ETS_UART_INTR_DISABLE();
    wifi_station_set_config(&conf);
    wifi_station_connect();
    ETS_UART_INTR_ENABLE();
    wifi_station_dhcpc_start();
    return status();
}

void ESP8266WiFiClass::config(IPAddress local_ip, IPAddress gateway, IPAddress subnet)
{
    struct ip_info info;
    info.ip.addr = static_cast<uint32_t>(local_ip);
    info.gw.addr = static_cast<uint32_t>(gateway);
    info.netmask.addr = static_cast<uint32_t>(subnet);

    wifi_station_dhcpc_stop();
    wifi_set_ip_info(STATION_IF, &info);
}

int ESP8266WiFiClass::disconnect()
{
    struct station_config conf;
    *conf.ssid = 0;
    *conf.password = 0;
    ETS_UART_INTR_DISABLE();
    wifi_station_set_config(&conf);
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();
    return 0;
}

void ESP8266WiFiClass::softAP(const char* ssid)
{
    softAP(ssid, 0);
}


void ESP8266WiFiClass::softAP(const char* ssid, const char* passphrase, int channel)
{
    if (wifi_get_opmode() < WIFI_AP)//will be OFF or STA
    {
        // turn on AP+STA mode
        mode(WIFI_AP_STA);
    }

    struct softap_config conf;
    wifi_softap_get_config(&conf);
    strcpy(reinterpret_cast<char*>(conf.ssid), ssid);
    conf.channel = channel;
    conf.ssid_len = strlen(ssid);
    conf.ssid_hidden = 0;
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

    ETS_UART_INTR_DISABLE();
    wifi_softap_set_config(&conf);
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

uint8_t* ESP8266WiFiClass::softAPmacAddress(uint8_t* mac)
{
    wifi_get_macaddr(SOFTAP_IF, mac);
    return mac;
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

char* ESP8266WiFiClass::SSID()
{
    static struct station_config conf;
    wifi_station_get_config(&conf);
    return reinterpret_cast<char*>(conf.ssid);
}

// uint8_t* ESP8266WiFiClass::BSSID(uint8_t* bssid)
// {
//  uint8_t* _bssid = WiFiDrv::getCurrentBSSID();
//  memcpy(bssid, _bssid, WL_MAC_ADDR_LENGTH);
//     return bssid;
// }

// int32_t ESP8266WiFiClass::RSSI()
// {
//     return WiFiDrv::getCurrentRSSI();
// }

// uint8_t ESP8266WiFiClass::encryptionType()
// {
//     return WiFiDrv::getCurrentEncryptionType();
// }

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
    esp_schedule();
}


int8_t ESP8266WiFiClass::scanNetworks()
{
    if ((wifi_get_opmode() & 1) == 0)//1 and 3 have STA enabled
    {
        mode(WIFI_AP_STA);
    }
    int status = wifi_station_get_connect_status();
    if (status != STATION_GOT_IP && status != STATION_IDLE)
    {
        disconnect();
    }

    if (ESP8266WiFiClass::_scanResult)
    {
        delete[] reinterpret_cast<bss_info*>(ESP8266WiFiClass::_scanResult);
        ESP8266WiFiClass::_scanResult = 0;
        ESP8266WiFiClass::_scanCount = 0;
    }

    struct scan_config config;
    config.ssid = 0;
    config.bssid = 0;
    config.channel = 0;
    config.show_hidden = 0;
    wifi_station_scan(&config, reinterpret_cast<scan_done_cb_t>(&ESP8266WiFiClass::_scanDone));
    esp_yield();
    return ESP8266WiFiClass::_scanCount;
}

void * ESP8266WiFiClass::_getScanInfoByIndex(int i)
{
    if (!ESP8266WiFiClass::_scanResult || i > ESP8266WiFiClass::_scanCount)
    {
        return 0;
    }

    return reinterpret_cast<bss_info*>(ESP8266WiFiClass::_scanResult) + i;
}

const char* ESP8266WiFiClass::SSID(uint8_t i)
{
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if (!it)
        return 0;

    return reinterpret_cast<const char*>(it->ssid);
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

uint8_t ESP8266WiFiClass::status()
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

void ESP8266WiFiClass::beginSmartConfig()
{
    if (_smartConfigStarted)
        return;

    if ((wifi_get_opmode() & 1) == 0)//1 and 3 have STA enabled
    {
        mode(WIFI_AP_STA);
    }

    _smartConfigStarted = true;

    //SC_TYPE_ESPTOUCH use ESPTOUCH for smartconfig, or use SC_TYPE_AIRKISS for AIRKISS
    smartconfig_start(SC_TYPE_ESPTOUCH, &ESP8266WiFiClass::_smartConfigDone);
}

void ESP8266WiFiClass::stopSmartConfig()
{
    if (!_smartConfigStarted)
        return;

    smartconfig_stop();
    _smartConfigStarted = false;
}

bool ESP8266WiFiClass::smartConfigDone(){
    if (!_smartConfigStarted)
        return false;

    return smartconfig_get_status() == SC_STATUS_LINK_OVER;
}

void ESP8266WiFiClass::_smartConfigDone(void* result)
{
    station_config* sta_conf = reinterpret_cast<station_config*>(result);

    wifi_station_set_config(sta_conf);
    wifi_station_disconnect();
    wifi_station_connect();
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

size_t ESP8266WiFiClass::_scanCount = 0;
void* ESP8266WiFiClass::_scanResult = 0;


ESP8266WiFiClass WiFi;