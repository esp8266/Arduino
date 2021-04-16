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

#include <cstring>
#include <list>
#include <memory>
#include <type_traits>

#include <coredecls.h>
#include <PolledTimeout.h>
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
#include "lwip/dhcp.h"
#include "lwip/apps/sntp.h"
}

#include "WiFiClient.h"
#include "WiFiUdp.h"
#include "debug.h"
#include "include/WiFiState.h"

// see comments on wifi_station_hostname in LwipIntf.cpp
extern "C" char* wifi_station_hostname; // sdk's hostname location

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------- Generic WiFi function -----------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

struct WiFiEventHandlerOpaque
{
    WiFiEventHandlerOpaque(WiFiEvent_t event, std::function<void(System_Event_t*)> handler)
    : mEvent(event), mHandler(handler)
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

bool ESP8266WiFiGenericClass::_persistent = false;
WiFiMode_t ESP8266WiFiGenericClass::_forceSleepLastMode = WIFI_OFF;

ESP8266WiFiGenericClass::ESP8266WiFiGenericClass()
{
    wifi_set_event_handler_cb((wifi_event_handler_cb_t) &ESP8266WiFiGenericClass::_eventCallback);
}

void ESP8266WiFiGenericClass::onEvent(WiFiEventCb f, WiFiEvent_t event)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(event, [f](System_Event_t* e) {
        (*f)(static_cast<WiFiEvent>(e->event));
    });
    handler->mCanExpire = false;
    sCbEventList.push_back(handler);
}

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeConnected(std::function<void(const WiFiEventStationModeConnected&)> f)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_CONNECTED, [f](System_Event_t* e) {
        auto& src = e->event_info.connected;
        WiFiEventStationModeConnected dst;
        dst.ssid.concat(reinterpret_cast<char*>(src.ssid), src.ssid_len);
        memcpy(dst.bssid, src.bssid, 6);
        dst.channel = src.channel;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeDisconnected(std::function<void(const WiFiEventStationModeDisconnected&)> f)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_DISCONNECTED, [f](System_Event_t* e){
        auto& src = e->event_info.disconnected;
        WiFiEventStationModeDisconnected dst;
        dst.ssid.concat(reinterpret_cast<char*>(src.ssid), src.ssid_len);
        memcpy(dst.bssid, src.bssid, 6);
        dst.reason = static_cast<WiFiDisconnectReason>(src.reason);
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeAuthModeChanged(std::function<void(const WiFiEventStationModeAuthModeChanged&)> f)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_AUTHMODE_CHANGE, [f](System_Event_t* e){
        auto& src = e->event_info.auth_change;
        WiFiEventStationModeAuthModeChanged dst;
        dst.oldMode = src.old_mode;
        dst.newMode = src.new_mode;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeGotIP(std::function<void(const WiFiEventStationModeGotIP&)> f)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_GOT_IP, [f](System_Event_t* e){
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

WiFiEventHandler ESP8266WiFiGenericClass::onStationModeDHCPTimeout(std::function<void(void)> f)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_STAMODE_DHCP_TIMEOUT, [f](System_Event_t* e){
        (void) e;
        f();
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onSoftAPModeStationConnected(std::function<void(const WiFiEventSoftAPModeStationConnected&)> f)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_SOFTAPMODE_STACONNECTED, [f](System_Event_t* e){
        auto& src = e->event_info.sta_connected;
        WiFiEventSoftAPModeStationConnected dst;
        memcpy(dst.mac, src.mac, 6);
        dst.aid = src.aid;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onSoftAPModeStationDisconnected(std::function<void(const WiFiEventSoftAPModeStationDisconnected&)> f)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_SOFTAPMODE_STADISCONNECTED, [f](System_Event_t* e){
        auto& src = e->event_info.sta_disconnected;
        WiFiEventSoftAPModeStationDisconnected dst;
        memcpy(dst.mac, src.mac, 6);
        dst.aid = src.aid;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onSoftAPModeProbeRequestReceived(std::function<void(const WiFiEventSoftAPModeProbeRequestReceived&)> f)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED, [f](System_Event_t* e){
        auto& src = e->event_info.ap_probereqrecved;
        WiFiEventSoftAPModeProbeRequestReceived dst;
        memcpy(dst.mac, src.mac, 6);
        dst.rssi = src.rssi;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

WiFiEventHandler ESP8266WiFiGenericClass::onWiFiModeChange(std::function<void(const WiFiEventModeChange&)> f)
{
    WiFiEventHandler handler = std::make_shared<WiFiEventHandlerOpaque>(WIFI_EVENT_MODE_CHANGE, [f](System_Event_t* e){
        auto& src = e->event_info.opmode_changed;
        WiFiEventModeChange dst;
        dst.oldMode = (WiFiMode_t)src.old_opmode;
        dst.newMode = (WiFiMode_t)src.new_opmode;
        f(dst);
    });
    sCbEventList.push_back(handler);
    return handler;
}

/**
 * callback for WiFi events
 * @param arg
 */
void ESP8266WiFiGenericClass::_eventCallback(void* arg)
{
    System_Event_t* event = reinterpret_cast<System_Event_t*>(arg);
    DEBUG_WIFI("wifi evt: %d\n", event->event);

    if (event->event == EVENT_STAMODE_DISCONNECTED) {
        DEBUG_WIFI("STA disconnect: %d\n", event->event_info.disconnected.reason);
        // workaround for https://github.com/esp8266/Arduino/issues/7432
        // still delivers the event, just handle this specific case
        if ((wifi_station_get_connect_status() == STATION_GOT_IP) && !wifi_station_get_reconnect_policy()) {
            DEBUG_WIFI("forcibly stopping the station connection manager\n");
            wifi_station_disconnect();
        }
    }

    if (event->event == EVENT_STAMODE_AUTHMODE_CHANGE) {
        auto& src = event->event_info.auth_change;
        if ((src.old_mode != AUTH_OPEN) && (src.new_mode == AUTH_OPEN)) {
            // CVE-2020-12638 workaround.  When we get a change to AUTH_OPEN from any other mode, drop the WiFi link because it's a downgrade attack
            // TODO - When upgrading to 3.x.x with fix, remove this code
            DEBUG_WIFI("WIFI_EVENT_STAMODE_AUTHMODE_CHANGE from encrypted(%d) to AUTH_OPEN, potential downgrade attack. Reconnecting WiFi. See CVE-2020-12638 for more details\n", src.old_mode);
            WiFi.reconnect();  // Disconnects from STA and then reconnects
        }
    }

    for(auto it = std::begin(sCbEventList); it != std::end(sCbEventList); ) {
        WiFiEventHandler &handler = *it;
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
uint8_t ESP8266WiFiGenericClass::channel(void) {
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

#if (NONOSDK >= (0x30000))

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
#else  // (NONOSDK >= (0x30000))
    (void)listenInterval;
#endif // (NONOSDK >= (0x30000))

    bool ret = wifi_set_sleep_type((sleep_type_t) type);
    if (!ret) {
        DEBUG_WIFI_GENERIC("wifi_set_sleep_type(%d): error\n", (int)type);
    }
    return ret;
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
 * set the output power of WiFi
 * @param dBm max: +20.5dBm  min: 0dBm
 */
void ESP8266WiFiGenericClass::setOutputPower(float dBm) {

    int i_dBm = int(dBm * 4.0f);

    // i_dBm 82 == 20.5 dBm
    if(i_dBm > 82) {
        i_dBm = 82;
    } else if(i_dBm < 0) {
        i_dBm = 0;
    }

    system_phy_set_max_tpw((uint8_t) i_dBm);
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
bool ESP8266WiFiGenericClass::getPersistent(){
    return _persistent;
}

/**
 * set new mode
 * @param m WiFiMode_t
 */
bool ESP8266WiFiGenericClass::mode(WiFiMode_t m) {
    if (m & ~(WIFI_STA | WIFI_AP)) {
        // any other bits than legacy disallowed
        return false;
    }

    if(_persistent){
        if(wifi_get_opmode() == (uint8) m && wifi_get_opmode_default() == (uint8) m){
            return true;
        }
    } else if(wifi_get_opmode() == (uint8) m){
        return true;
    }

    char backup_hostname [33] { 0 }; // hostname is 32 chars long (RFC)

    if (m != WIFI_OFF) {
        memcpy(backup_hostname, wifi_station_hostname, sizeof(backup_hostname));
        ESP.forcedModemSleepOff();
    }

    bool ret = false;
    ETS_UART_INTR_DISABLE();
    if(_persistent) {
        ret = wifi_set_opmode(m);
    } else {
        ret = wifi_set_opmode_current(m);
    }
    ETS_UART_INTR_ENABLE();

    if(!ret)
      return false; //calling wifi_set_opmode failed

    //Wait for mode change, which is asynchronous.
    //Only wait if in CONT context. If this were called from SYS, it's up to the user to serialize
    //tasks to wait correctly.
    constexpr unsigned int timeoutValue = 1000; //1 second
    if(can_yield()) {
        // check opmode every 100ms or give up after timeout
        esp_delay(timeoutValue, [m]() { return wifi_get_opmode() != m; }, 100);

        //if at this point mode still hasn't been reached, give up
        if(wifi_get_opmode() != (uint8) m) {
            return false; //timeout
        }
    }

    if (backup_hostname[0])
        memcpy(wifi_station_hostname, backup_hostname, sizeof(backup_hostname));

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

    if (isEnabled == enable)
        return true;

    if (enable)
        return mode((WiFiMode_t)(currentMode | WIFI_STA));

    return mode((WiFiMode_t)(currentMode & (~WIFI_STA)));
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
    if(!mode(WIFI_OFF)) {
        DEBUG_WIFI("core: error with mode(WIFI_OFF)\n");
        return false;
    }
    return ESP.forcedModemSleep(sleepUs);
}

/**
 * wake up WiFi Modem
 * @return ok
 */
bool ESP8266WiFiGenericClass::forceSleepWake() {
    // restore last mode
    ESP.forcedModemSleepOff();
    if(mode(_forceSleepLastMode)) {
        if((_forceSleepLastMode & WIFI_STA) != 0){
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
#if (NONOSDK >= (0x30000))
    return wifi_get_listen_interval();
#else
    return 0;
#endif
}

/**
 * Get sleep level of modem sleep and light sleep
 * @return true if max level
 */
bool ESP8266WiFiGenericClass::isSleepLevelMax () {
#if (NONOSDK >= (0x30000))
    return wifi_get_sleep_level() == MAX_SLEEP_T;
#else
    return false;
#endif
}


// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ Generic Network function ---------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

namespace {

struct _dns_found_result {
    IPAddress addr;
    bool done;
};

}

static void _dns_found_callback(const char *, const ip_addr_t *, void *);

static int hostByNameImpl(const char* aHostname, IPAddress& aResult, uint32_t timeout_ms, DNSResolveType resolveType) {
    if (aResult.fromString(aHostname)) {
        DEBUG_WIFI_GENERIC("[hostByName] Host: %s is IP!\n", aHostname);
        return 1;
    }

    static_assert(std::is_same_v<uint8_t, std::underlying_type_t<decltype(resolveType)>>, "");
    DEBUG_WIFI_GENERIC("[hostByName] request IP for: %s\n", aHostname);

    ip_addr_t addr;
    auto pending = std::make_unique<_dns_found_result>(
        _dns_found_result{
            .addr = IPADDR_NONE,
            .done = false,
        });

    err_t err = dns_gethostbyname_addrtype(aHostname,
        &addr, &_dns_found_callback, pending.get(),
        static_cast<uint8_t>(resolveType));

    switch (err) {
    // Address already known
    case ERR_OK:
        aResult = addr;
        break;

    // We are no longer able to issue requests
    case ERR_MEM:
        break;

    // We need to wait for c/b to fire *or* we exit on our own timeout
    // (which also requires us to notify the c/b that it is supposed to delete the pending obj)
    case ERR_INPROGRESS:
        // sleep until dns_found_callback is called or timeout is reached
        esp_delay(timeout_ms, [&]() { return !pending->done; });

        if (pending->done) {
            if ((pending->addr).isSet()) {
                aResult = pending->addr;
                err = ERR_OK;
            }
        } else {
            pending->done = true;
            pending.release();
            err = ERR_TIMEOUT;
        }

        break;
    }

    if (err == ERR_OK) {
        DEBUG_WIFI_GENERIC("[hostByName] Host: %s IP: %s\n", aHostname, aResult.toString().c_str());
        return 1;
    }

    DEBUG_WIFI_GENERIC("[hostByName] Host: %s lookup error: %s (%d)!\n",
            aHostname,
            (err == ERR_TIMEOUT) ? "Timeout" :
            (err == ERR_INPROGRESS) ? "No response" :
                "Unknown", static_cast<int>(err));

    return 0;
}

/**
 * Resolve the given hostname to an IP address.
 * @param aHostname     Name to be resolved
 * @param aResult       IPAddress structure to store the returned IP address
 * @return 1 if aIPAddrString was successfully converted to an IP address,
 *          else 0
 */
int ESP8266WiFiGenericClass::hostByName(const char* aHostname, IPAddress& aResult)
{
    return hostByNameImpl(aHostname, aResult, DNSDefaultTimeoutMs, DNSResolveTypeDefault);
}

int ESP8266WiFiGenericClass::hostByName(const char* aHostname, IPAddress& aResult, uint32_t timeout_ms)
{
    return hostByNameImpl(aHostname, aResult, timeout_ms, DNSResolveTypeDefault);
}

#if LWIP_IPV4 && LWIP_IPV6
int ESP8266WiFiGenericClass::hostByName(const char* aHostname, IPAddress& aResult, uint32_t timeout_ms, DNSResolveType resolveType)
{
    return hostByNameImpl(aHostname, aResult, timeout_ms, resolveType);
}
#endif

/**
 * DNS callback
 * @param name
 * @param ipaddr
 * @param callback_arg
 */
static void _dns_found_callback(const char*, const ip_addr_t* ipaddr, void* arg)
{
    auto result = reinterpret_cast<_dns_found_result*>(arg);
    if (result->done) {
        delete result;
        return;
    }

    if (ipaddr) {
        result->addr = IPAddress(ipaddr);
    }

    result->done = true;
    esp_schedule();
}

uint32_t ESP8266WiFiGenericClass::shutdownCRC (const WiFiState& state)
{
    return crc32(&state.state, sizeof(state.state));
}

bool ESP8266WiFiGenericClass::shutdownValidCRC (const WiFiState& state)
{
    return crc32(&state.state, sizeof(state.state)) == state.crc;
}

bool ESP8266WiFiGenericClass::shutdown (WiFiState& state, uint32 sleepUs)
{
    bool persistent = _persistent;
    WiFiMode_t before_off_mode = getMode();

    if (before_off_mode & WIFI_STA)
    {
        bool ret = wifi_get_ip_info(STATION_IF, &state.state.ip);
        if (!ret)
        {
            DEBUG_WIFI("core: error with wifi_get_ip_info(STATION_IF)\n");
            return false;
        }
        memset(state.state.fwconfig.bssid, 0xff, 6);
        ret = wifi_station_get_config(&state.state.fwconfig);
        if (!ret)
        {
            DEBUG_WIFI("core: error with wifi_station_get_config\n");
            return false;
        }
        state.state.channel = wifi_get_channel();
    }

    // disable persistence in FW so in case of power failure
    // it doesn't wake up in off mode.
    // persistence state will be restored on WiFi resume.
    WiFi.persistent(false);
    if (!WiFi.forceSleepBegin(sleepUs))
    {
        // WIFI_OFF mode set by forceSleepBegin()
        DEBUG_WIFI("core: error with forceSleepBegin()\n");
        WiFi.mode(before_off_mode);
        WiFi.persistent(persistent);
        return false;
    }

    // WiFi is now in force-sleep mode
    // finish filling state and process crc

    state.state.persistent = persistent;
    state.state.mode = before_off_mode;

    uint8_t i = 0;
    for (auto& ntp: state.state.ntp)
    {
        ntp = *sntp_getserver(i++);
    }
    i = 0;

    for (auto& dns: state.state.dns)
    {
        dns = WiFi.dnsIP(i++);
    }

    state.crc = shutdownCRC(state);
    DEBUG_WIFI("core: state is saved\n");

    return true;
}

bool ESP8266WiFiGenericClass::shutdown (WiFiState& state) {
    return shutdown(state, 0);
}

bool ESP8266WiFiGenericClass::resumeFromShutdown (WiFiState& state)
{
    ESP.forcedModemSleepOff();

    if (shutdownCRC(state) != state.crc)
    {
        DEBUG_WIFI("core: resume: bad crc\n");
        return false;
    }

    persistent(state.state.persistent);

    if (!mode(state.state.mode))
    {
        DEBUG_WIFI("core: resume: can't set wifi mode to %d\n", state.state.mode);
        return false;
    }

    if (state.state.mode & WIFI_STA)
    {
        IPAddress local(state.state.ip.ip);
        if (local)
        {
            DEBUG_WIFI("core: resume: static address '%s'\n", local.toString().c_str());
            WiFi.config(state.state.ip.ip, state.state.ip.gw, state.state.ip.netmask, state.state.dns[0], state.state.dns[1]);
            uint8_t i = 0;
            for (const auto& ntp: state.state.ntp)
            {
                IPAddress ip(ntp);
                if (ip.isSet())
                {
                    DEBUG_WIFI("core: resume: start SNTP, server='%s'\n", ip.toString().c_str());
                    sntp_setserver(i++, &ntp);
                }
            }
        }

        String ssid;
        {
            const char* ptr = reinterpret_cast<const char*>(state.state.fwconfig.ssid);
            ssid.concat(ptr, strnlen(ptr, sizeof(station_config::ssid)));
        }

        String pass;
        {
            const char* ptr = reinterpret_cast<const char*>(state.state.fwconfig.password);
            pass.concat(ptr, strnlen(ptr, sizeof(station_config::password)));
        }

        auto beginResult = WiFi.begin(ssid.c_str(),
                       pass.c_str(),
                       state.state.channel,
                       state.state.fwconfig.bssid,
                       true);
        if (beginResult == WL_CONNECT_FAILED)
        {
            DEBUG_WIFI("core: resume: WiFi.begin failed\n");
            return false;
        }
        if (beginResult == WL_WRONG_PASSWORD)
        {
            DEBUG_WIFI("core: resume: WiFi.begin wrong password\n");
            return false;
        }
    }

    if (state.state.mode & WIFI_AP)
    {
        DEBUG_WIFI("core: resume AP mode TODO\n");
        return false;
    }

    // success, invalidate saved state
    state.crc++;

    return true;
}

void ESP8266WiFiGenericClass::preinitWiFiOff () {
    // It was meant to be called from user-defined ::preinit()
    // It is now deprecated by enableWiFiAtBootTime() and __disableWiFiAtBootTime()
}
