/*
 *  empty wrappers to play with linker and re-enable wifi at boot time
 */

#include "coredecls.h"

#include <ESP8266WiFi.h>

extern "C" void enableWiFiAtBootTime()
{
    /*
     * Called by user from anywhere, does nothing and allows overriding
     * the core_esp8266_main.cpp's default disableWiFiAtBootTime() by the
     * one below, at link time.
     */
}

extern "C" void __disableWiFiAtBootTime()
{
    // overrides the default __disableWiFiAtBootTime:
    // Does (almost) nothing: WiFi is enabled by default in nonos-sdk

    // ... but restores legacy WiFi credentials persistence to true at boot time
    // (can be still overrisden by user before setting up WiFi, like before)

    // (note: c++ ctors not called yet at this point)
    ESP8266WiFiClass::persistent(true);
}

extern "C" int __get_rf_disable_mode(void)
{
    // overrides the default __get_rf_disable_mode
    return -1;  // mode not set
}

extern "C" int __get_rf_powerup_disable_mode(void)
{
    // overrides the default __get_rf__powerup_disable_mode
    return -1;  // mode not set
}
