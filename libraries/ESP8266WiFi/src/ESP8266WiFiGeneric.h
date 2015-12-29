/*
 ESP8266WiFiGeneric.h - esp8266 Wifi support.
 Based on WiFi.h from Ardiono WiFi shield library.
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

#ifndef ESP8266WIFIGENERIC_H_
#define ESP8266WIFIGENERIC_H_

#include "ESP8266WiFiType.h"

typedef void (*WiFiEventCb)(WiFiEvent_t event);

typedef struct {
        WiFiEventCb cb;
        WiFiEvent_t event;
} WiFiEventCbList_t;

class ESP8266WiFiGenericClass {
        // ----------------------------------------------------------------------------------------------
        // -------------------------------------- Generic WiFi function ---------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        ESP8266WiFiGenericClass();

        void onEvent(WiFiEventCb cbEvent, WiFiEvent_t event = WIFI_EVENT_MAX);
        void removeEvent(WiFiEventCb cbEvent, WiFiEvent_t event = WIFI_EVENT_MAX);

        int32_t channel(void);

        bool setSleepMode(WiFiSleepType_t type);
        WiFiSleepType_t getSleepMode();

        bool setPhyMode(WiFiPhyMode_t mode);
        WiFiPhyMode_t getPhyMode();

        void setOutputPower(float dBm);

        void persistent(bool persistent);

        bool mode(WiFiMode_t);
        WiFiMode_t getMode();

        bool enableSTA(bool enable);
        bool enableAP(bool enable);

        bool forceSleepBegin(uint32 sleepUs = 0);
        bool forceSleepWake();

    protected:
        static bool _persistent;
        static WiFiMode_t _forceSleepLastMode;

        static void _eventCallback(void *event);

        // ----------------------------------------------------------------------------------------------
        // ------------------------------------ Generic Network function --------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        int hostByName(const char* aHostname, IPAddress& aResult);

    protected:

        friend class ESP8266WiFiSTAClass;
        friend class ESP8266WiFiScanClass;
        friend class ESP8266WiFiAPClass;
};

#endif /* ESP8266WIFIGENERIC_H_ */
