/*
 ESP8266WiFi.h - esp8266 Wifi support.
 Based on WiFi.h from Arduino WiFi shield library.
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

#include <Arduino.h>

#include <stdint.h>

extern "C" {
#include "include/wl_definitions.h"
}

#include "IPAddress.h"

#include "ESP8266WiFiType.h"
#include "ESP8266WiFiSTA.h"
#include "ESP8266WiFiAP.h"
#include "ESP8266WiFiScan.h"
#include "ESP8266WiFiGeneric.h"

#include "WiFiClient.h"
#include "WiFiServer.h"
#include "WiFiServerSecure.h"
#include "WiFiClientSecure.h"
#include "BearSSLHelpers.h"
#include "CertStoreBearSSL.h"

#ifdef DEBUG_ESP_WIFI
#ifdef DEBUG_ESP_PORT
#define DEBUG_WIFI(fmt, ...) DEBUG_ESP_PORT.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_WIFI
#define DEBUG_WIFI(...) do { (void)0; } while (0)
#endif


class ESP8266WiFiClass : public ESP8266WiFiGenericClass, public ESP8266WiFiSTAClass, public ESP8266WiFiScanClass, public ESP8266WiFiAPClass {
    public:

        // workaround same function name with different signature
        using ESP8266WiFiGenericClass::channel;

        using ESP8266WiFiSTAClass::SSID;
        using ESP8266WiFiSTAClass::RSSI;
        using ESP8266WiFiSTAClass::BSSID;
        using ESP8266WiFiSTAClass::BSSIDstr;

        using ESP8266WiFiScanClass::SSID;
        using ESP8266WiFiScanClass::encryptionType;
        using ESP8266WiFiScanClass::RSSI;
        using ESP8266WiFiScanClass::BSSID;
        using ESP8266WiFiScanClass::BSSIDstr;
        using ESP8266WiFiScanClass::channel;
        using ESP8266WiFiScanClass::isHidden;

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
