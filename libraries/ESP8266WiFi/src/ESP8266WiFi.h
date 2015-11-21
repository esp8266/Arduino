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
#include "WiFiClientSecure.h"

#define WIFI_SCAN_RUNNING   (-1)
#define WIFI_SCAN_FAILED     (-2)

enum WiFiMode { WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3 };

class ESP8266WiFiClass
{
public:

    ESP8266WiFiClass();

    void persistent(bool persistent);

    void mode(WiFiMode);
    WiFiMode getMode();

    /**
     * Start Wifi connection
     * if passphrase is set the most secure supported mode will be automatically selected
     * @param ssid const char*          Pointer to the SSID string.
     * @param passphrase const char *   Optional. Passphrase. Valid characters in a passphrase must be between ASCII 32-126 (decimal).
     * @param bssid uint8_t[6]          Optional. BSSID / MAC of AP
     * @param channel                   Optional. Channel of AP
     * @return
     */
    int begin(const char* ssid, const char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL);
    int begin(char* ssid, char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL);

    // Use sdk config to connect.
    int begin();


   /* Wait for Wifi connection to reach a result
    * returns the status reached or disconnect if STA is off
    */
    uint8_t waitForConnectResult();

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
     * param ssid_hidden: Network cloaking? 0 = broadcast SSID, 1 = hide SSID
     */
    void softAP(const char* ssid, const char* passphrase, int channel = 1, int ssid_hidden = 0);

    /* Change Ip configuration settings disabling the dhcp client
        *
        * param local_ip: 	Static ip configuration
        * param gateway: 	Static gateway configuration
        * param subnet:		Static Subnet mask
        */
    void config(IPAddress local_ip, IPAddress gateway, IPAddress subnet);

	/* Change Ip configuration settings disabling the dhcp client
        *
        * param local_ip: 	Static ip configuration
        * param gateway: 	Static gateway configuration
        * param subnet:		Static Subnet mask
		* param dns: 		Defined DNS
        */
    void config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns);

    /* Configure access point
     *
     * param local_ip: access point IP
     * param gateway: gateway IP
     * param subnet: subnet mask
     */
    void softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);

    /*
      * Disconnect from the network (close AP)
      *
      * return: one value of wl_status_t enum
      */
    int softAPdisconnect(bool wifioff = false);

    /*
     * Disconnect from the network
     *
     * return: one value of wl_status_t enum
     */
    int disconnect(bool wifioff = false);

    /*
     * Get the station interface MAC address.
     *
     * return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
     * return: String
     */
    uint8_t* macAddress(uint8_t* mac);
    String macAddress(void);

    /*
     * Get the softAP interface MAC address.
     *
     * return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
     * return: String
     */
    uint8_t* softAPmacAddress(uint8_t* mac);
    String softAPmacAddress(void);

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
     * Get the DNS ip address.
     *
     * return: DNS ip address value
     */
   IPAddress dnsIP(int dns_no = 0);

    /*
     * Return the current SSID associated with the network
     *
     * return: ssid string
     */
    String SSID() const;

    /*
     * Return the current pre shared key associated with the network
     *
     * return: psk string
     */
    String psk() const;

    /*
     * Return the current bssid / mac associated with the network if configured
     *
     * return: bssid uint8_t *
     */
    uint8_t *BSSID(void);

    /*
     * Return the current bssid / mac associated with the network if configured
     *
     * return: bssid string
     */
    String BSSIDstr(void);

    /*
     * Return the current channel associated with the network
     *
     * return: channel
     */
    int32_t channel(void);

    /*
     * Return the current network RSSI.
     *
     * return: RSSI value
     */

    int32_t RSSI();


    /*
     * called to get the scan state in Async mode
     *
     * return -1 if scan not fin
     * return -2 if scan not triggered
     */
    int8_t scanComplete();

    /*
     * delete last scan result from RAM
     */
    void scanDelete();

    /*
     * Start scan WiFi networks available
     *
     * return: Number of discovered networks
     */
    int8_t scanNetworks(bool async = false);

    /*
     * Return the SSID discovered during the network scan.
     *
     * param networkItem: specify from which network item want to get the information
	 *
     * return: ssid string of the specified item on the networks scanned list
     */
    String SSID(uint8_t networkItem);

    /*
     * Return the encryption type of the networks discovered during the scanNetworks
     *
     * param networkItem: specify from which network item want to get the information
	 *
     * return: encryption type (enum wl_enc_type) of the specified item on the networks scanned list
     */
    uint8_t	encryptionType(uint8_t networkItem);

    /*
     * Return the RSSI of the networks discovered during the scanNetworks
     *
     * param networkItem: specify from which network item want to get the information
	 *
     * return: signed value of RSSI of the specified item on the networks scanned list
     */
    int32_t RSSI(uint8_t networkItem);


    /**
     * return MAC / BSSID of scanned wifi
     * @param networkItem specify from which network item want to get the information
     * @return uint8_t * MAC / BSSID of scanned wifi
     */
    uint8_t * BSSID(uint8_t networkItem);

    /**
     * return MAC / BSSID of scanned wifi
     * @param networkItem specify from which network item want to get the information
     * @return String MAC / BSSID of scanned wifi
     */
    String BSSIDstr(uint8_t networkItem);

    /**
     * return channel of scanned wifi
     * @param networkItem specify from which network item want to get the information
     * @return uint32_t channel of scanned wifi
     */
    int32_t channel(uint8_t networkItem);

    /**
     * return if the scanned wifi is Hidden (no SSID)
     * @param networkItem specify from which network item want to get the information
     * @return bool (true == hidden)
     */
    bool isHidden(uint8_t networkItem);

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
    bool getNetworkInfo(uint8_t networkItem, String &ssid, uint8_t &encryptionType, int32_t &RSSI, uint8_t* &BSSID, int32_t &channel, bool &isHidden);


    /*
     * Return Connection status.
     *
     * return: one of the value defined in wl_status_t
     */
    wl_status_t status();

    /*
     * Resolve the given hostname to an IP address.
     * param aHostname: Name to be resolved
     * param aResult: IPAddress structure to store the returned IP address
     * result: 1 if aIPAddrString was successfully converted to an IP address,
     *          else error code
     */
    int hostByName(const char* aHostname, IPAddress& aResult);

    /*
     * Get ESP8266 station DHCP hostname
     */
    String hostname(void);

    /*
     * Set ESP8266 station DHCP hostname
     * hostname, max length:32
     */
    bool hostname(char* aHostname);
    bool hostname(const char* aHostname);
    bool hostname(String aHostname);


    /**
     * WPS config
     * so far only WPS_TYPE_PBC is supported (SDK 1.2.0)
     */
    bool beginWPSConfig(void);

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
    void _mode(WiFiMode);
    static void _scanDone(void* result, int status);
    void * _getScanInfoByIndex(int i);
    static void _smartConfigCallback(uint32_t status, void* result);
    static void _eventCallback(void *event);
    bool _smartConfigStarted;
    bool _smartConfigDone;

    bool _useApMode;
    bool _useClientMode;
	bool _useStaticIp;
    bool _persistent;

	static bool _scanAsync;
	static bool _scanStarted;
	static bool _scanComplete;

    static size_t _scanCount;
    static void* _scanResult;

};

extern ESP8266WiFiClass WiFi;

#endif
