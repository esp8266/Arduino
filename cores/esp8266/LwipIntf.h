/*
  LwipIntf.h

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

#ifndef _LWIPINTF_H
#define _LWIPINTF_H

#include <lwip/netif.h>
#include <IPAddress.h>

#include <functional>

class LwipIntf
{
public:
    using CBType = std::function<void(netif*)>;

    static bool stateUpCB(LwipIntf::CBType&& cb);

    // reorder WiFi.config() parameters for a esp8266/official Arduino dual-compatibility API
    // args     | esp order  arduino order
    // ----     + ---------  -------------
    // local_ip | local_ip   local_ip
    // arg1     | gateway    dns1
    // arg2     | netmask    [Agateway
    // arg3     | dns1       netmask
    //
    // result stored into gateway/netmask/dns1
    static bool ipAddressReorder(const IPAddress& local_ip, const IPAddress& arg1,
                                 const IPAddress& arg2, const IPAddress& arg3, IPAddress& gateway,
                                 IPAddress& netmask, IPAddress& dns1);

    String hostname();

    bool   hostname(const String& aHostname)
    {
        return hostname(aHostname.c_str());
    }

    bool hostname(const char* aHostname);

    // ESP32 API compatibility
    bool setHostname(const char* aHostName)
    {
        return hostname(aHostName);
    }

    // ESP32 API compatibility
    const char* getHostname();

protected:
    static bool stateChangeSysCB(LwipIntf::CBType&& cb);
};

#endif  // _LWIPINTF_H
