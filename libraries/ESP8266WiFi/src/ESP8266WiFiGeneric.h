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
#include <functional>
#include <memory>

#ifdef DEBUG_ESP_WIFI
#ifdef DEBUG_ESP_PORT
#define DEBUG_WIFI_GENERIC(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_WIFI_GENERIC
#define DEBUG_WIFI_GENERIC(...)
#endif

struct WiFiEventHandlerOpaque;
typedef std::shared_ptr<WiFiEventHandlerOpaque> WiFiEventHandler;

typedef void (*WiFiEventCb)(WiFiEvent_t);

class ESP8266WiFiGenericClass {
        // ----------------------------------------------------------------------------------------------
        // -------------------------------------- Generic WiFi function ---------------------------------
        // ----------------------------------------------------------------------------------------------

    public:
        ESP8266WiFiGenericClass();

        // Note: this function is deprecated. Use one of the functions below instead.
        void onEvent(WiFiEventCb cb, WiFiEvent_t event = WIFI_EVENT_ANY) __attribute__((deprecated));

        // Subscribe to specific event and get event information as an argument to the callback
        WiFiEventHandler onStationModeConnected(std::function<void(const WiFiEventStationModeConnected&)>);
        WiFiEventHandler onStationModeDisconnected(std::function<void(const WiFiEventStationModeDisconnected&)>);
        WiFiEventHandler onStationModeAuthModeChanged(std::function<void(const WiFiEventStationModeAuthModeChanged&)>);
        WiFiEventHandler onStationModeGotIP(std::function<void(const WiFiEventStationModeGotIP&)>);
        WiFiEventHandler onStationModeDHCPTimeout(std::function<void(void)>);
        WiFiEventHandler onSoftAPModeStationConnected(std::function<void(const WiFiEventSoftAPModeStationConnected&)>);
        WiFiEventHandler onSoftAPModeStationDisconnected(std::function<void(const WiFiEventSoftAPModeStationDisconnected&)>);
        // WiFiEventHandler onWiFiModeChange(std::function<void(const WiFiEventModeChange&)>);

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
