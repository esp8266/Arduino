/*
 lwIPDhcpServer-NonOS.cpp - DHCP server wrapper

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

#include <lwip/init.h> // LWIP_VERSION

#if LWIP_VERSION_MAJOR != 1

#include <lwip/netif.h>
#include "lwIPDhcpServer.h"

extern netif netif_git[2];

// global DHCP instance for softAP interface
DhcpServer dhcpSoftAP(&netif_git[SOFTAP_IF]);

extern "C"
{

void dhcps_start (struct ip_info *info, netif* apnetif)
{
    // apnetif is esp interface, replaced by lwip2's
    // netif_git[SOFTAP_IF] interface in constructor
    (void)apnetif;

#if 0
    // can't use C++ now, global ctors are not initialized yet
    dhcpSoftAP.begin(info);
#else
    (void)info;
    // initial version: emulate nonos-sdk in DhcpServer class before
    //                  trying to change legacy behavor
    // `fw_has_started_softap_dhcps` will be read in DhcpServer::DhcpServer
    // which is called when c++ ctors are initialized, specifically
    // dhcpSoftAP intialized with AP interface number above.
    fw_has_started_softap_dhcps = 1;
#endif
}

void dhcps_stop ()
{
    dhcpSoftAP.end();
}

} // extern "C"

#endif // LWIP_VERSION_MAJOR != 1
