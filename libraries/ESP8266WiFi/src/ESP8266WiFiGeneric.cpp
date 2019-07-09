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

#include <list>
#include <string.h>
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
#include "lwip/init.h" // LWIP_VERSION_
}

#include "WiFiClient.h"
#include "WiFiUdp.h"
#include "debug.h"

extern "C" void esp_schedule();
extern "C" void esp_yield();

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------- Generic WiFi function -----------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

struct WiFiEventHandlerOpaque {
    WiFiEventHandlerOpaque(WiFiEvent_t event, std::function<void(System_Event_t*)> handler)
        : mEvent(event)
        , mHandler(handler)
    {
    }

    void operator()(System_Event_t* e)
    {
        if (static_cast<WiFiEvent>(e->event) == mEvent || mEvent == WIFI_EVENT_ANY) {
            mHandler(e);
        }
    }

    bool canExpire()
    {
        return mCanExpire;
    }

    WiFiEvent_t mEvent;
    std::function<void(System_Event_t*)> mHandler;
    bool mCanExpire = true; /* stopgap solution to handle deprecated void onEvent(cb, evt) case */
};

static std::list<WiFiEventHandler> sCbEventList;

bool ESP8266WiFiGenericClass::_persistent = true;
WiFiMode_t ESP8266WiFiGenericClass::_forceSleepLastMode = WIFI_OFF;

ESP8266WiFiGenericClass::ESP8266WiFiGenericClass() {
    wifi_set_event_handler_cb((wifi_event_handler_cb_t)&ESP8266WiFiGenericClass::_eventCallback);
}

void ESP8266WiFiGenericClass::onEvent(WiFiEventCb f, WiFiEvent_t event) {
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(event, [f](System_Event_t* e) {
        (*f)(static_cast<WiFiEvent>(e->event));
    });
    handler->mCanExpire = false;
    sCbEventList.push_back(handler);
}

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeConnected(std::function<void(const WiFiEventStationModeConnected&)> f) {
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_CONNECTED, [f](System_Event_t* e) {
        auto& src = e->event_info.connected;
        WiFiEventStationModeConnected dst;
        dst.ssid = String(reinterpret_cast<char*>(src.ssid));
        memcpy(dst.bssid, src.bssid, 6);
        dst.channel = src.channel;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeDisconnected(std::function<void(const WiFiEventStationModeDisconnected&)> f) {
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_DISCONNECTED, [f](System_Event_t* e) {
        auto& src = e->event_info.disconnected;
        WiFiEventStationModeDisconnected dst;
        dst.ssid = String(reinterpret_cast<char*>(src.ssid));
        memcpy(dst.bssid, src.bssid, 6);
        dst.reason = static_cast<WiFiDisconnectReason>(src.reason);
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeAuthModeChanged(std::function<void(const WiFiEventStationModeAuthModeChanged&)> f) {
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_AUTHMODE_CHANGE, [f](System_Event_t* e) {
        auto& src = e->event_info.auth_change;
        WiFiEventStationModeAuthModeChanged dst;
        dst.oldMode = src.old_mode;
        dst.newMode = src.new_mode;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeGotIP(std::function<void(const WiFiEventStationModeGotIP&)> f) {
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_GOT_IP, [f](System_Event_t* e) {
        auto& src = e->event_info.got_ip;
        WiFiEventStationModeGotIP dst;
        dst.ip = src.ip.addr;
        dst.mask = src.mask.addr;
        dst.gw = src.gw.addr;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeDHCPTimeout(std::function<void(void)> f) {
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_DHCP_TIMEOUT, [f](System_Event_t* e) {
        (void)e;
        f();
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onSoftAPModeStationConnected(std::function<void(const WiFiEventSoftAPModeStationConnected&)> f) {
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_SOFTAPMODE_STACONNECTED, [f](System_Event_t* e) {
        auto& src = e->event_info.sta_connected;
        WiFiEventSoftAPModeStationConnected dst;
        memcpy(dst.mac, src.mac, 6);
        dst.aid = src.aid;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onSoftAPModeStationDisconnected(std::function<void(const WiFiEventSoftAPModeStationDisconnected&)> f) {
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_SOFTAPMODE_STADISCONNECTED, [f](System_Event_t* e) {
        auto& src = e->event_info.sta_disconnected;
        WiFiEventSoftAPModeStationDisconnected dst;
        memcpy(dst.mac, src.mac, 6);
        dst.aid = src.aid;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onSoftAPModeProbeRequestReceived(std::function<void(const WiFiEventSoftAPModeProbeRequestReceived&)> f) {
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED, [f](System_Event_t* e) {
        auto& src = e->event_info.ap_probereqrecved;
        WiFiEventSoftAPModeProbeRequestReceived dst;
        memcpy(dst.mac, src.mac, 6);
        dst.rssi = src.rssi;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

// WiFiEventHandler ESP8266WiFiGenericClass::onWiFiModeChange(std::function<void(const WiFiEventModeChange&)> f)
// {
//     WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_MODE_CHANGE, [f](System_Event_t* e){
//         WiFiEventModeChange& dst = *reinterpret_cast<WiFiEventModeChange*>(&e->event_info);
//         f(dst);
//     });
//     sCbEventList.push_back(handler);
//     return handler;
// }

/**
 * callback for WiFi events
 * @param arg
 */
void ESP8266WiFiGenericClass::_eventCallback(void* arg) {
    System_Event_t* event = reinterpret_cast<System_Event_t*>(arg);
    DEBUG_WIFI("wifi evt: %d\n", event->event);

    if (event->event == EVENT_STAMODE_DISCONNECTED) {
        DEBUG_WIFI("STA disconnect: %d\n", event->event_info.disconnected.reason);
        WiFiClient::stopAll();
    }

    for (auto it = std::begin(sCbEventList); it != std::end(sCbEventList);) {
        WiFiEventHandler& handler = *it;
        if (handler->canExpire() && handler.unique()) {
            it = sCbEventList.erase(it);
        }
        else {
            (*handler)(event);
            ++it;
        }
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
bool ESP8266WiFiGenericClass::setSleepMode(WiFiSleepType_t type, uint8_t listenInterval) {

/**
    * datasheet:
    *
   wifi_set_sleep_level():
   Set sleep level of modem sleep and light sleep
   This configuration should be called before calling wifi_set_sleep_type
   Modem-sleep and light sleep mode have minimum and maximum sleep levels.
   - In minimum sleep level, station wakes up at every DTIM to receive
     beacon.  Broadcast data will not be lost because it is transmitted after
     DTIM.  However, it can not save much more power if DTIM period is short,
     as specified in AP.
   - In maximum sleep level, station wakes up at every listen interval to
     receive beacon.  Broadcast data may be lost because station may be in sleep
     state at DTIM time.  If listen interval is longer, more power will be saved, but
     it’s very likely to lose more broadcast data.
   - Default setting is minimum sleep level.
   Further reading: https://routerguide.net/dtim-interval-period-best-setting/

   wifi_set_listen_interval():
   Set listen interval of maximum sleep level for modem sleep and light sleep
   It only works when sleep level is set as MAX_SLEEP_T
   forum: https://github.com/espressif/ESP8266_NONOS_SDK/issues/165#issuecomment-416121920
   default value seems to be 3 (as recommended by https://routerguide.net/dtim-interval-period-best-setting/)

   call order:
     wifi_set_sleep_level(MAX_SLEEP_T) (SDK3)
     wifi_set_listen_interval          (SDK3)
     wifi_set_sleep_type               (all SDKs)

    */

#ifdef NONOSDK3V0

#ifdef DEBUG_ESP_WIFI
    if (listenInterval && type == WIFI_NONE_SLEEP)
        DEBUG_WIFI_GENERIC("listenInterval not usable with WIFI_NONE_SLEEP\n");
#endif

    if (type == WIFI_LIGHT_SLEEP || type == WIFI_MODEM_SLEEP) {
        if (listenInterval) {
            if (!wifi_set_sleep_level(MAX_SLEEP_T)) {
                DEBUG_WIFI_GENERIC("wifi_set_sleep_level(MAX_SLEEP_T): error\n");
                return false;
            }
            if (listenInterval > 10) {
                DEBUG_WIFI_GENERIC("listenInterval must be in [1..10]\n");
#ifndef DEBUG_ESP_WIFI
                // stay within datasheet range when not in debug mode
                listenInterval = 10;
#endif
            }
            if (!wifi_set_listen_interval(listenInterval)) {
                DEBUG_WIFI_GENERIC("wifi_set_listen_interval(%d): error\n", listenInterval);
                return false;
            }
        } else {
            if (!wifi_set_sleep_level(MIN_SLEEP_T)) {
                DEBUG_WIFI_GENERIC("wifi_set_sleep_level(MIN_SLEEP_T): error\n");
                return false;
            }
        }
    }
#else  // !defined(NONOSDK3V0)
    (void)listenInterval;
#endif // !defined(NONOSDK3V0)

    bool ret = wifi_set_sleep_type((sleep_type_t)type);
    if (!ret) {
        DEBUG_WIFI_GENERIC("wifi_set_sleep_type(%d): error\n", (int) type);
    }
    return ret;
}

/**
 * get Sleep mode
 * @return sleep_type_t
 */
WiFiSleepType_t ESP8266WiFiGenericClass::getSleepMode() {
    return (WiFiSleepType_t)wifi_get_sleep_type();
}

/**
 * set phy Mode
 * @param mode phy_mode_t
 * @return bool
 */
bool ESP8266WiFiGenericClass::setPhyMode(WiFiPhyMode_t mode) {
    return wifi_set_phy_mode((phy_mode_t)mode);
}

/**
 * get phy Mode
 * @return phy_mode_t
 */
WiFiPhyMode_t ESP8266WiFiGenericClass::getPhyMode() {
    return (WiFiPhyMode_t)wifi_get_phy_mode();
}

/**
 * set the output power of WiFi
 * @param dBm max: +20.5dBm  min: 0dBm
 */
void ESP8266WiFiGenericClass::setOutputPower(float dBm) {
    if (dBm > 20.5) {
        dBm = 20.5;
    }
    else if (dBm < 0) {
        dBm = 0;
    }

    uint8_t val = (dBm * 4.0f);
    system_phy_set_max_tpw(val);
}

/**
 * store WiFi config in SDK flash area
 * @param persistent
 */
void ESP8266WiFiGenericClass::persistent(bool persistent) {
    _persistent = persistent;
}

/**
 * gets the persistent state
 * @return bool
 */
bool ESP8266WiFiGenericClass::getPersistent() {
    return _persistent;
}

/**
 * set new mode
 * @param m WiFiMode_t
 */
bool ESP8266WiFiGenericClass::mode(WiFiMode_t m) {
    if (_persistent) {
        if (wifi_get_opmode() == (uint8)m && wifi_get_opmode_default() == (uint8)m) {
            return true;
        }
    } else if (wifi_get_opmode() == (uint8)m) {
        return true;
    }

    bool ret = false;

    if (m != WIFI_STA && m != WIFI_AP_STA)
        // calls lwIP's dhcp_stop(),
        // safe to call even if not started
        wifi_station_dhcpc_stop();

    ETS_UART_INTR_DISABLE();
    if (_persistent) {
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
    return (WiFiMode_t)wifi_get_opmode();
}

/**
 * control STA mode
 * @param enable bool
 * @return ok
 */
bool ESP8266WiFiGenericClass::enableSTA(bool enable) {

    WiFiMode_t currentMode = getMode();
    bool isEnabled = ((currentMode & WIFI_STA) != 0);

    if (isEnabled != enable) {
        if (enable) {
            return mode((WiFiMode_t)(currentMode | WIFI_STA));
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
bool ESP8266WiFiGenericClass::enableAP(bool enable) {

    WiFiMode_t currentMode = getMode();
    bool isEnabled = ((currentMode & WIFI_AP) != 0);

    if (isEnabled != enable) {
        if (enable) {
            return mode((WiFiMode_t)(currentMode | WIFI_AP));
        } else {
            return mode((WiFiMode_t)(currentMode & (~WIFI_AP)));
        }
    } else {
        return true;
    }
}

/**
 * Disable WiFi for x us when value is not 0
 * @param sleep_time_in_us
 * @return ok
 */
bool ESP8266WiFiGenericClass::forceSleepBegin(uint32 sleepUs) {
    _forceSleepLastMode = getMode();
    if (!mode(WIFI_OFF)) {
        return false;
    }

    if (sleepUs == 0) {
        sleepUs = 0xFFFFFFF;
    }

    wifi_fpm_set_sleep_type(MODEM_SLEEP_T);
    wifi_fpm_open();
    return (wifi_fpm_do_sleep(sleepUs) == 0);
}

/**
 * wake up WiFi Modem
 * @return ok
 */
bool ESP8266WiFiGenericClass::forceSleepWake() {
    wifi_fpm_do_wakeup();
    wifi_fpm_close();

    // restore last mode
    if (mode(_forceSleepLastMode)) {
        if ((_forceSleepLastMode & WIFI_STA) != 0) {
            wifi_station_connect();
        }
        return true;
    }
    return false;
}

/**
 * Get listen interval of maximum sleep level for modem sleep and light sleep.
 * @return interval
 */
uint8_t ESP8266WiFiGenericClass::getListenInterval () {
#ifndef NONOSDK3V0
    return 0;
#else
    return wifi_get_listen_interval();
#endif
}

/**
 * Get sleep level of modem sleep and light sleep
 * @return true if max level
 */
bool ESP8266WiFiGenericClass::isSleepLevelMax () {
#ifndef NONOSDK3V0
    return false;
#else
    return wifi_get_sleep_level() == MAX_SLEEP_T;
#endif
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ Generic Network function ---------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void wifi_dns_found_callback(const char* name, CONST ip_addr_t* ipaddr, void* callback_arg);

static bool _dns_lookup_pending = false;

/**
 * Resolve the given hostname to an IP address.
 * @param aHostname     Name to be resolved
 * @param aResult       IPAddress structure to store the returned IP address
 * @return 1 if aIPAddrString was successfully converted to an IP address,
 *          else 0
 */
int ESP8266WiFiGenericClass::hostByName(const char* aHostname, IPAddress& aResult) {
    return hostByName(aHostname, aResult, 10000);
}

int ESP8266WiFiGenericClass::hostByName(const char* aHostname, IPAddress& aResult, uint32_t timeout_ms) {
    ip_addr_t addr;
    aResult = static_cast<uint32_t>(0);

    if (aResult.fromString(aHostname)) {
        // Host name is a IP address use it!
        DEBUG_WIFI_GENERIC("[hostByName] Host: %s is a IP!\n", aHostname);
        return 1;
    }

    DEBUG_WIFI_GENERIC("[hostByName] request IP for: %s\n", aHostname);
    err_t err = dns_gethostbyname(aHostname, &addr, &wifi_dns_found_callback, &aResult);
    if (err == ERR_OK) {
        aResult = IPAddress(&addr);
    } else if (err == ERR_INPROGRESS) {
        _dns_lookup_pending = true;
        delay(timeout_ms);
        _dns_lookup_pending = false;
        // will return here when dns_found_callback fires
        if (aResult.isSet()) {
            err = ERR_OK;
        }
    }

    if (err != 0) {
        DEBUG_WIFI_GENERIC("[hostByName] Host: %s lookup error: %d!\n", aHostname, (int)err);
    }
    else {
        DEBUG_WIFI_GENERIC("[hostByName] Host: %s IP: %s\n", aHostname, aResult.toString().c_str());
    }

    return (err == ERR_OK) ? 1 : 0;
}

/**
 * DNS callback
 * @param name
 * @param ipaddr
 * @param callback_arg
 */
void wifi_dns_found_callback(const char* name, CONST ip_addr_t* ipaddr, void* callback_arg) {
    (void)name;
    if (!_dns_lookup_pending) {
        return;
    }
    if (ipaddr) {
        (*reinterpret_cast<IPAddress*>(callback_arg)) = IPAddress(ipaddr);
    }
    esp_schedule(); // resume the hostByName function
}

// Async part

struct host_struct {
    const char* hostname;
    IPAddress addr;
    int status; // 0 failed, 1 ready, 2 in progress (callback not called)
};

static struct host_struct host_var = { NULL, IPADDR_ANY, 2 };

void wifi_dns_found_callback_async(const char* name, CONST ip_addr_t* ipaddr, void* callback_arg);

int ESP8266WiFiGenericClass::hostByNameAsync(const char* aHostname, IPAddress& aResult) {
    if (aHostname == NULL) {
        DEBUG_WIFI_GENERIC("[hostByNameAsync] hostname is NULL!");
        return 0;
    }
    static ip_addr_t addr;
    if (!host_var.hostname) {
        host_var.hostname = aHostname;
        err_t err = dns_gethostbyname(host_var.hostname, &addr,
            &wifi_dns_found_callback_async, &host_var);
        DEBUG_WIFI_GENERIC("[hostByNameAsync] DNS query registred, waiting for response");
        if (err == ERR_OK) {
            aResult = IPAddress(addr);
            DEBUG_WIFI_GENERIC("[hostByNameAsync] IP found!");
            return 1;
        }
        if (err == ERR_INPROGRESS) {
            return 2;
        }
        DEBUG_WIFI_GENERIC("[hostByNameAsync] An error occurred!");
    }
    else if (host_var.hostname == aHostname) { // Better using strcmp ???
        if (host_var.status == 1) {
            aResult = host_var.addr;
            DEBUG_WIFI_GENERIC("[hostByNameAsync] IP found!");
        }
        if (host_var.status == 0) {
            DEBUG_WIFI_GENERIC("[hostByNameAsync] IP NOT found! Please retry");
        }
        if (host_var.status != 2) {
            host_var.addr = (uint32_t) IPADDR_ANY;
            host_var.hostname = NULL;
            host_var.status = 2;
        }
        return host_var.status;
    } else {
        DEBUG_WIFI_GENERIC(
            "[hostByNameAsync] Another DNS search is in progress, please retry later!");
    }
    return 0;
}

void wifi_dns_found_callback_async(const char* name, CONST ip_addr_t* ipaddr, void* callback_arg) {
    if (ipaddr && *ipaddr != (uint32_t) IPADDR_ANY) {
        reinterpret_cast<struct host_struct*>(callback_arg)->addr = IPAddress(ipaddr);
        reinterpret_cast<struct host_struct*>(callback_arg)->status = 1;
    } else {
        reinterpret_cast<struct host_struct*>(callback_arg)->status = 0;
    }
}

//meant to be called from user-defined preinit()
void ESP8266WiFiGenericClass::preinitWiFiOff() {
    // https://github.com/esp8266/Arduino/issues/2111#issuecomment-224251391
    // WiFi.persistent(false);
    // WiFi.mode(WIFI_OFF);
    // WiFi.forceSleepBegin();

    //WiFi.mode(WIFI_OFF) equivalent:
    // datasheet:
    // Set Wi-Fi working mode to Station mode, SoftAP
    // or Station + SoftAP, and do not update flash
    // (not persistent)
    wifi_set_opmode_current(WIFI_OFF);

    //WiFi.forceSleepBegin(/*default*/0) equivalent:
    // sleep forever until wifi_fpm_do_wakeup() is called
    wifi_fpm_set_sleep_type(MODEM_SLEEP_T);
    wifi_fpm_open();
    wifi_fpm_do_sleep(0xFFFFFFF);

    // use WiFi.forceSleepWake() to wake WiFi up
}
