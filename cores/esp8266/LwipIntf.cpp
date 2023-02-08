/*
  LwipIntf.cpp

  Arduino interface for lwIP generic callbacks and functions

  Original Copyright (c) 2020 esp8266 Arduino All rights reserved.
  This file is part of the esp8266 Arduino core environment.

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

extern "C"
{
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"  // LWIP_VERSION_
#if LWIP_IPV6
#include "lwip/netif.h"  // struct netif
#endif

#include <user_interface.h>
}

#include "debug.h"
#include "LwipIntf.h"

// wifi_station_hostname is SDK's station(=global) hostname location
// - It is never nullptr but wifi_station_get_hostname()
//   can return nullptr when STA is down
// - Because WiFi is started in off mode at boot time,
//   wifi_station_set/get_hostname() is now no more used
//   because setting hostname firt does not work anymore
// - wifi_station_hostname is overwritten by SDK when wifi is
//   woken up in WiFi::mode()
//
extern "C" char* wifi_station_hostname;

// args      | esp order    arduino order
// ----      + ---------    -------------
// local_ip  | local_ip     local_ip
// arg1      | gateway      dns1
// arg2      | netmask      gateway
// arg3      | dns1         netmask
//
// result stored into gateway/netmask/dns1

bool LwipIntf::ipAddressReorder(const IPAddress& local_ip, const IPAddress& arg1,
                                const IPAddress& arg2, const IPAddress& arg3, IPAddress& gateway,
                                IPAddress& netmask, IPAddress& dns1)
{
    // To allow compatibility, check first octet of 3rd arg. If 255, interpret as ESP order,
    // otherwise Arduino order.
    gateway = arg1;
    netmask = arg2;
    dns1    = arg3;

    if (netmask[0] != 255)
    {
        // octet is not 255 => interpret as Arduino order
        gateway = arg2;
        netmask = arg3[0] == 0 ? IPAddress(255, 255, 255, 0)
                               : arg3;  // arg order is arduino and 4th arg not given => assign it
                                        // arduino default
        dns1 = arg1;
    }

    // check whether all is IPv4 (or gateway not set)
    if (!(local_ip.isV4() && netmask.isV4() && (!gateway.isSet() || gateway.isV4())))
    {
        return false;
    }

    // ip and gateway must be in the same netmask
    if (gateway.isSet() && (local_ip.v4() & netmask.v4()) != (gateway.v4() & netmask.v4()))
    {
        return false;
    }

    return true;
}

/**
    Get ESP8266 station DHCP hostname
    @return hostname
*/
String LwipIntf::hostname(void)
{
    return wifi_station_hostname;
}

/**
    Get ESP8266 station DHCP hostname
    @return hostname
*/
const char* LwipIntf::getHostname(void)
{
    return wifi_station_hostname;
}

/**
    Set ESP8266 station DHCP hostname
    @param aHostname max length:24
    @return ok
*/
bool LwipIntf::hostname(const char* aHostname)
{
    /*
        vvvv RFC952 vvvv
        ASSUMPTIONS
        1. A "name" (Net, Host, Gateway, or Domain name) is a text string up
        to 24 characters drawn from the alphabet (A-Z), digits (0-9), minus
        sign (-), and period (.).  Note that periods are only allowed when
        they serve to delimit components of "domain style names". (See
        RFC-921, "Domain Name System Implementation Schedule", for
        background).  No blank or space characters are permitted as part of a
        name. No distinction is made between upper and lower case.  The first
        character must be an alpha character.  The last character must not be
        a minus sign or period.  A host which serves as a GATEWAY should have
        "-GATEWAY" or "-GW" as part of its name.  Hosts which do not serve as
        Internet gateways should not use "-GATEWAY" and "-GW" as part of
        their names. A host which is a TAC should have "-TAC" as the last
        part of its host name, if it is a DoD host.  Single character names
        or nicknames are not allowed.
        ^^^^ RFC952 ^^^^

        - 24 chars max
        - only a..z A..Z 0..9 '-'
        - no '-' as last char
    */

    size_t len = strlen(aHostname);

    if (len == 0 || len > 32)
    {
        // nonos-sdk limit is 32
        // (dhcp hostname option minimum size is ~60)
        DEBUGV("WiFi.(set)hostname(): empty or large(>32) name\n");
        return false;
    }

    // check RFC compliance
    bool compliant = (len <= 24);
    for (size_t i = 0; compliant && i < len; i++)
        if (!isalnum(aHostname[i]) && aHostname[i] != '-')
        {
            compliant = false;
        }
    if (aHostname[len - 1] == '-')
    {
        compliant = false;
    }

    if (!compliant)
    {
        DEBUGV("hostname '%s' is not compliant with RFC952\n", aHostname);
    }

    bool ret = true;

    strcpy(wifi_station_hostname, aHostname);

    // now we should inform dhcp server for this change, using lwip_renew()
    // looping through all existing interface
    // harmless for AP, also compatible with ethernet adapters (to come)
    for (netif* intf = netif_list; intf; intf = intf->next)
    {
        // unconditionally update all known interfaces
        intf->hostname = wifi_station_hostname;

        if (netif_dhcp_data(intf) != nullptr)
        {
            // renew already started DHCP leases
            err_t lwipret = dhcp_renew(intf);
            if (lwipret != ERR_OK)
            {
                DEBUGV("WiFi.hostname(%s): lwIP error %d on interface %c%c (index %d)\n",
                       intf->hostname, (int)lwipret, intf->name[0], intf->name[1], intf->num);
                ret = false;
            }
        }
    }

    return ret && compliant;
}
