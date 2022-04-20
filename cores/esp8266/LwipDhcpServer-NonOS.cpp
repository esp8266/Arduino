/*
    lwIPDhcpServer-NonOS - DHCP server wrapper

    Copyright (c) 2020 esp8266 arduino. All rights reserved.
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
*/

// STARTS/STOPS DHCP SERVER ON WIFI AP INTERFACE
// these functions must exists as-is with "C" interface,
// nonos-sdk calls them at boot time and later

#include "LwipDhcpServer-NonOS.h"

#include <lwip/netif.h>

// Global static DHCP instance for softAP interface
// (since the netif object never goes away, even when AP is disabled)
// Initial version fully emulates nonos-sdk api in DhcpServer class,
// before trying to further change it and possibly break legacy behaviour
DhcpServer& dhcpSoftAP()
{
    extern netif      netif_git[2];
    static DhcpServer server(&netif_git[SOFTAP_IF]);
    return server;
}

extern "C"
{
    // `ip_info` is useless, since we get the information from the netif directly
    // `netif` would be netif_git[SOFTAP_IF], which we get from the lwip2 glue
    void dhcps_start(ip_info*, netif*)
    {
        auto& server = dhcpSoftAP();
        if (!server.isRunning())
        {
            server.begin();
        }
    }

    void dhcps_stop()
    {
        auto& server = dhcpSoftAP();
        if (server.isRunning())
        {
            server.end();
        }
    }

    // providing the rest of the nonos-sdk API, which was originally removed in 3.0.0

    bool wifi_softap_set_dhcps_lease(dhcps_lease* please)
    {
        auto& server = dhcpSoftAP();
        return server.set_dhcps_lease(please);
    }

    bool wifi_softap_get_dhcps_lease(dhcps_lease* please)
    {
        auto& server = dhcpSoftAP();
        return server.get_dhcps_lease(please);
    }

    uint32 wifi_softap_get_dhcps_lease_time()
    {
        auto& server = dhcpSoftAP();
        return server.get_dhcps_lease_time();
    }

    bool wifi_softap_set_dhcps_lease_time(uint32 minutes)
    {
        auto& server = dhcpSoftAP();
        return server.set_dhcps_lease_time(minutes);
    }

    bool wifi_softap_reset_dhcps_lease_time()
    {
        auto& server = dhcpSoftAP();
        return server.reset_dhcps_lease_time();
    }

    bool wifi_softap_add_dhcps_lease(uint8* macaddr)
    {
        auto& server = dhcpSoftAP();
        return server.add_dhcps_lease(macaddr);
    }

}  // extern "C"
