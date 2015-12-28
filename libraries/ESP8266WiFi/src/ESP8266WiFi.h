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

// Note:
// this enums need to be in sync with the SDK!

enum WiFiMode {
    WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
};

typedef enum {
    WIFI_PHY_MODE_11B = 1, WIFI_PHY_MODE_11G = 2, WIFI_PHY_MODE_11N = 3
} WiFiPhyMode_t;

typedef enum {
    WIFI_NONE_SLEEP = 0, WIFI_LIGHT_SLEEP = 2, WIFI_MODEM_SLEEP = 3
} WiFiSleepType_t;

class ESP8266WiFiClass {

        // ----------------------------------------------------------------------------------------------
        // -------------------------------------- ESP8266WiFiClass --------------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        ESP8266WiFiClass();

    protected:

        static void _eventCallback(void *event);

        // ----------------------------------------------------------------------------------------------
        // ---------------------------------------- STA function ----------------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        int begin(const char* ssid, const char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL);
        int begin(char* ssid, char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL);
        int begin();

        void config(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
        void config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns);

        int disconnect(bool wifioff = false);

        uint8_t waitForConnectResult();

        // STA network info
        IPAddress localIP();

        uint8_t* macAddress(uint8_t* mac);
        String macAddress(void);

        IPAddress subnetMask();
        IPAddress gatewayIP();
        IPAddress dnsIP(uint8_t dns_no = 0);

        String hostname(void);
        bool hostname(char* aHostname);
        bool hostname(const char* aHostname);
        bool hostname(String aHostname);

        // STA WiFi info
        wl_status_t status();
        String SSID() const;
        String psk() const;

        uint8_t *BSSID(void);
        String BSSIDstr(void);

        int32_t RSSI();

    protected:

        bool _useStaticIp;

        // ----------------------------------------------------------------------------------------------
        // ----------------------------------------- AP function ----------------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        void softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0);
        void softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
        int softAPdisconnect(bool wifioff = false);

        IPAddress softAPIP();

        uint8_t* softAPmacAddress(uint8_t* mac);
        String softAPmacAddress(void);

    protected:

        // ----------------------------------------------------------------------------------------------
        // ----------------------------------------- scan function --------------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        int8_t scanNetworks(bool async = false, bool show_hidden = false);

        int8_t scanComplete();
        void scanDelete();

        // scan result
        bool getNetworkInfo(uint8_t networkItem, String &ssid, uint8_t &encryptionType, int32_t &RSSI, uint8_t* &BSSID, int32_t &channel, bool &isHidden);

        String SSID(uint8_t networkItem);
        uint8_t encryptionType(uint8_t networkItem);
        int32_t RSSI(uint8_t networkItem);
        uint8_t * BSSID(uint8_t networkItem);
        String BSSIDstr(uint8_t networkItem);
        int32_t channel(uint8_t networkItem);
        bool isHidden(uint8_t networkItem);

    protected:

        static bool _scanAsync;
        static bool _scanStarted;
        static bool _scanComplete;

        static size_t _scanCount;
        static void* _scanResult;

        static void _scanDone(void* result, int status);
        void * _getScanInfoByIndex(int i);

        // ----------------------------------------------------------------------------------------------
        // -------------------------------------- Generic WiFi function ---------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        int32_t channel(void);

        bool setSleepMode(WiFiSleepType_t type);
        WiFiSleepType_t getSleepMode();

        bool setPhyMode(WiFiPhyMode_t mode);
        WiFiPhyMode_t getPhyMode();

        void persistent(bool persistent);
        void mode(WiFiMode);
        WiFiMode getMode();

    protected:
        bool _useApMode;
        bool _useClientMode;
        bool _persistent;
        void _mode(WiFiMode);

        // ----------------------------------------------------------------------------------------------
        // ------------------------------------ Generic Network function --------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        int hostByName(const char* aHostname, IPAddress& aResult);

    protected:

        // ----------------------------------------------------------------------------------------------
        // ------------------------------------ STA remote configure  -----------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        bool beginWPSConfig(void);

        void beginSmartConfig();
        bool smartConfigDone();
        void stopSmartConfig();

    protected:

        bool _smartConfigStarted;
        bool _smartConfigDone;
        static void _smartConfigCallback(uint32_t status, void* result);

        // ----------------------------------------------------------------------------------------------
        // ------------------------------------------- Debug --------------------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        void printDiag(Print& dest);

        friend class WiFiClient;
        friend class WiFiServer;

};

extern ESP8266WiFiClass WiFi;

#endif
