/*
 ESP8266WiFiGeneric.cpp - WiFi library for esp8266

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

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/dns.h"
}

#include "WiFiClient.h"
#include "WiFiUdp.h"

#include "debug.h"

extern "C" void esp_schedule();
extern "C" void esp_yield();

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------- Generic WiFi function -----------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

bool ESP8266WiFiGenericClass::_persistent = true;
WiFiEventCb ESP8266WiFiGenericClass::_cbEvent = NULL;

ESP8266WiFiGenericClass::ESP8266WiFiGenericClass()  {
    wifi_set_event_handler_cb((wifi_event_handler_cb_t) &ESP8266WiFiGenericClass::_eventCallback);
}

/**
 * set callback function
 * @param cbEvent WiFiEventCb
 */
void ESP8266WiFiGenericClass::onEvent(WiFiEventCb cbEvent) {
    _cbEvent = cbEvent;
}

/**
 * callback for WiFi events
 * @param arg
 */
void ESP8266WiFiGenericClass::_eventCallback(void* arg) {
    System_Event_t* event = reinterpret_cast<System_Event_t*>(arg);
    DEBUGV("wifi evt: %d\n", event->event);

    if(event->event == EVENT_STAMODE_DISCONNECTED) {
        WiFiClient::stopAll();
    }

    if(_cbEvent) {
        _cbEvent((WiFiEvent_t) event->event);
    }
}

/**
 * Return the current channel associated with the network
 * @return channel (1-13)
 */
int32_t ESP8266WiFiGenericClass::channel(void) {
    return wifi_get_channel();
}

/**
 * set Sleep mode
 * @param type sleep_type_t
 * @return bool
 */
bool ESP8266WiFiGenericClass::setSleepMode(WiFiSleepType_t type) {
    return wifi_set_sleep_type((sleep_type_t) type);
}

/**
 * get Sleep mode
 * @return sleep_type_t
 */
WiFiSleepType_t ESP8266WiFiGenericClass::getSleepMode() {
    return (WiFiSleepType_t) wifi_get_sleep_type();
}

/**
 * set phy Mode
 * @param mode phy_mode_t
 * @return bool
 */
bool ESP8266WiFiGenericClass::setPhyMode(WiFiPhyMode_t mode) {
    return wifi_set_phy_mode((phy_mode_t) mode);
}

/**
 * get phy Mode
 * @return phy_mode_t
 */
WiFiPhyMode_t ESP8266WiFiGenericClass::getPhyMode() {
    return (WiFiPhyMode_t) wifi_get_phy_mode();
}

/**
 * store WiFi config in SDK flash area
 * @param persistent
 */
void ESP8266WiFiGenericClass::persistent(bool persistent) {
    _persistent = persistent;
}


/**
 * set new mode
 * @param m WiFiMode_t
 */
bool ESP8266WiFiGenericClass::mode(WiFiMode_t m) {
    if(wifi_get_opmode() == (uint8) m) {
        return true;
    }

    bool ret = false;

    ETS_UART_INTR_DISABLE();
    if(_persistent) {
        ret = wifi_set_opmode(m);
    } else {
        ret = wifi_set_opmode_current(m);
    }
    ETS_UART_INTR_ENABLE();

    return ret;
}

/**
 * get WiFi mode
 * @return WiFiMode
 */
WiFiMode_t ESP8266WiFiGenericClass::getMode() {
    return (WiFiMode_t) wifi_get_opmode();
}

/**
 * control STA mode
 * @param enable bool
 * @return ok
 */
bool ESP8266WiFiGenericClass::enableSTA(bool enable) {

    WiFiMode_t currentMode = getMode();
    bool isEnabled = ((currentMode & WIFI_STA) != 0);

    if(isEnabled != enable) {
        if(enable) {
            return mode((WiFiMode_t)(currentMode & WIFI_STA));
        } else {
            return mode((WiFiMode_t)(currentMode & (~WIFI_STA)));
        }
    } else {
        return true;
    }
}

/**
 * control AP mode
 * @param enable bool
 * @return ok
 */
bool ESP8266WiFiGenericClass::enableAP(bool enable){

    WiFiMode_t currentMode = getMode();
    bool isEnabled = ((currentMode & WIFI_AP) != 0);

    if(isEnabled != enable) {
        if(enable) {
            return mode((WiFiMode_t)(currentMode & WIFI_AP));
        } else {
            return mode((WiFiMode_t)(currentMode & (~WIFI_AP)));
        }
    } else {
        return true;
    }
}





// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ Generic Network function ---------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void wifi_dns_found_callback(const char *name, ip_addr_t *ipaddr, void *callback_arg);

/**
 * Resolve the given hostname to an IP address.
 * @param aHostname     Name to be resolved
 * @param aResult       IPAddress structure to store the returned IP address
 * @return 1 if aIPAddrString was successfully converted to an IP address,
 *          else error code
 */
int ESP8266WiFiGenericClass::hostByName(const char* aHostname, IPAddress& aResult) {
    ip_addr_t addr;
    aResult = static_cast<uint32_t>(0);
    err_t err = dns_gethostbyname(aHostname, &addr, &wifi_dns_found_callback, &aResult);
    if(err == ERR_OK) {
        aResult = addr.addr;
    } else if(err == ERR_INPROGRESS) {
        esp_yield();
        // will return here when dns_found_callback fires
    }

    return (aResult != 0) ? 1 : 0;
}

/**
 * DNS callback
 * @param name
 * @param ipaddr
 * @param callback_arg
 */
void wifi_dns_found_callback(const char *name, ip_addr_t *ipaddr, void *callback_arg) {
    if(ipaddr) {
        (*reinterpret_cast<IPAddress*>(callback_arg)) = ipaddr->addr;
    }
    esp_schedule(); // resume the hostByName function
}


