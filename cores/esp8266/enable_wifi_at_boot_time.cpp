/*
 *  empty wrappers to play with linker and reenable wifi at boot time
 */

#include "coredecls.h"

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
    // Does nothing: WiFi is enabled by default in nonos-sdk
}
