/*
 ESP8266WiFiAP.h - esp8266 Wifi support.
 Based on WiFi.h from Arduino WiFi shield library.
 Copyright (c) 2011-2014 Arduino.  All right reserved.
 Modified by Ivan Grokhotkov, December 2014
 Reworked by Markus Sattler, December 2015

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

#ifndef ESP8266WIFIAP_H_
#define ESP8266WIFIAP_H_


#include "ESP8266WiFiType.h"
#include "ESP8266WiFiGeneric.h"


class ESP8266WiFiAPClass {

        // ----------------------------------------------------------------------------------------------
        // ----------------------------------------- AP function ----------------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        bool softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4);
        bool softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
        bool softAPdisconnect(bool wifioff = false);

        uint8_t softAPgetStationNum();

        IPAddress softAPIP();

        uint8_t* softAPmacAddress(uint8_t* mac);
        String softAPmacAddress(void);

    protected:

};

#endif /* ESP8266WIFIAP_H_*/
