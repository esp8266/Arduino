/*
  ESP8266WiFi.h - esp8266 Wifi support.
  Based on WiFi.h from Ardiono WiFi shield library.
  Copyright (c) 2011-2014 Arduino.  All right reserved.
  Modified by Ivan Grokhotkov, December 2014

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

#ifndef WiFi_h
#define WiFi_h

#include <stdint.h>

extern "C" {
  #include "include/wl_definitions.h"
}

#include "IPAddress.h"
#include "WiFiClient.h"
#include "WiFiServer.h"

enum WiFiMode { WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3 };

class ESP8266WiFiClass
{
public:

    ESP8266WiFiClass();

    void mode(WiFiMode);


    /* Start Wifi connection for OPEN networks
     *
     * param ssid: Pointer to the SSID string.
     */
    int begin(const char* ssid);

    /* Start Wifi connection with passphrase
     * the most secure supported mode will be automatically selected
     *
     * param ssid: Pointer to the SSID string.
     * param passphrase: Passphrase. Valid characters in a passphrase
     *        must be between ASCII 32-126 (decimal).
     */
    int begin(const char* ssid, const char *passphrase);


    /* Set up an open access point
     *
     * param ssid: Pointer to the SSID string.
     */
    void softAP(const char* ssid);


    /* Set up a WPA2-secured access point
     *
     * param ssid: Pointer to the SSID string.
     * param passphrase: Pointer to passphrase, 8 characters min.
     * param channel: WiFi channel number, 1 - 13.
     */
    void softAP(const char* ssid, const char* passphrase, int channel = 1);


    /* Change Ip configuration settings disabling the dhcp client
        *
        * param local_ip:   Static ip configuration
        * param gateway:    Static gateway configuration
        * param subnet:     Static Subnet mask
        */
    void config(IPAddress local_ip, IPAddress gateway, IPAddress subnet);

    /* Configure access point
     *
     * param local_ip: access point IP
     * param gateway: gateway IP
     * param subnet: subnet mask
     */
    void softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);

    /*
     * Disconnect from the network
     *
     * return: one value of wl_status_t enum
     */
    int disconnect(void);

    /*
     * Get the station interface MAC address.
     *
     * return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
     */
    uint8_t* macAddress(uint8_t* mac);

    /*
     * Get the softAP interface MAC address.
     *
     * return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
     */
    uint8_t* softAPmacAddress(uint8_t* mac);

    /*
     * Get the station interface IP address.
     *
     * return: Ip address value
     */
    IPAddress localIP();

    /*
     * Get the softAP interface IP address.
     *
     * return: Ip address value
     */
    IPAddress softAPIP();

    /*
     * Get the interface subnet mask address.
     *
     * return: subnet mask address value
     */
    IPAddress subnetMask();

    /*
     * Get the gateway ip address.
     *
     * return: gateway ip address value
     */
   IPAddress gatewayIP();

    /*
     * Return the current SSID associated with the network
     *
     * return: ssid string
     */
    char* SSID();

    /*
     * Return the current network RSSI. Note: this is just a stub, there is no way to
     *  get the RSSI in the Espressif SDK yet.
     *
     * return: RSSI value (currently 0)
     */

    int32_t RSSI() { return 0; }

    /*
     * Start scan WiFi networks available
     *
     * return: Number of discovered networks
     */
    int8_t scanNetworks();

    /*
     * Return the SSID discovered during the network scan.
     *
     * param networkItem: specify from which network item want to get the information
     *
     * return: ssid string of the specified item on the networks scanned list
     */
    const char* SSID(uint8_t networkItem);

    /*
     * Return the encryption type of the networks discovered during the scanNetworks
     *
     * param networkItem: specify from which network item want to get the information
     *
     * return: encryption type (enum wl_enc_type) of the specified item on the networks scanned list
     */
    uint8_t encryptionType(uint8_t networkItem);

    /*
     * Return the RSSI of the networks discovered during the scanNetworks
     *
     * param networkItem: specify from which network item want to get the information
     *
     * return: signed value of RSSI of the specified item on the networks scanned list
     */
    int32_t RSSI(uint8_t networkItem);

    /*
     * Return Connection status.
     *
     * return: one of the value defined in wl_status_t
     */
    uint8_t status();

    /*
     * Resolve the given hostname to an IP address.
     * param aHostname: Name to be resolved
     * param aResult: IPAddress structure to store the returned IP address
     * result: 1 if aIPAddrString was successfully converted to an IP address,
     *          else error code
     */
    int hostByName(const char* aHostname, IPAddress& aResult);

    /*
     * Output WiFi settings to an object derived from Print interface (like Serial).
     *
     */
    void printDiag(Print& dest);

    /*
     * Start SmartConfig
     *
     */
    void beginSmartConfig();

    /*
     * Query SmartConfig status, to decide when stop config
     *
     */
    bool smartConfigDone();

    /*
     * Stop SmartConfig
     *
     */
    void stopSmartConfig();

    friend class WiFiClient;
    friend class WiFiServer;

protected:
    static void _scanDone(void* result, int status);
    void * _getScanInfoByIndex(int i);
    static void _smartConfigDone(void* result);
    bool _smartConfigStarted = false;

    static size_t _scanCount;
    static void* _scanResult;

};

extern ESP8266WiFiClass WiFi;

#endif