/*
 ESP8266WiFiGratuitous.cpp - esp8266 Wifi support
 copyright esp8266/arduino

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
#include "lwip/init.h" // LWIP_VERSION_*
#if LWIP_VERSION_MAJOR == 1
#include "netif/wlan_lwip_if.h" // eagle_lwip_getif()
#include "netif/etharp.h" // gratuitous arp
#include "user_interface.h"
#else
#include "lwip/etharp.h" // gratuitous arp
#endif
} // extern "C"

#include <Schedule.h>

#include "ESP8266WiFiGratuitous.h"

namespace experimental
{

ETSTimer* ESP8266WiFiGratuitous::_timer = nullptr;

void ESP8266WiFiGratuitous::stationKeepAliveNow ()
{
    for (netif* interface = netif_list; interface != nullptr; interface = interface->next)
        if (
               (interface->flags & NETIF_FLAG_LINK_UP)
            && (interface->flags & NETIF_FLAG_UP)
#if LWIP_VERSION_MAJOR == 1
            && interface == eagle_lwip_getif(STATION_IF) /* lwip1 does not set if->num properly */
            && (!ip_addr_isany(&interface->ip_addr))
#else
            && interface->num == STATION_IF
            && (!ip4_addr_isany_val(*netif_ip4_addr(interface)))
#endif
        )
        {
            etharp_gratuitous(interface);
            break;
        }
}

void ESP8266WiFiGratuitous::scheduleItForNextYieldOnce (void*)
{
    schedule_recurrent_function_us([]()
    {
        ESP8266WiFiGratuitous::stationKeepAliveNow();
        return false;
    }, 0);
}

bool ESP8266WiFiGratuitous::stationKeepAliveSetIntervalMs (uint32_t ms)
{
    if (_timer)
    {
        os_timer_disarm(_timer);
        _timer = nullptr;
    }

    if (ms)
    {
        // send one now
        stationKeepAliveNow();

        _timer = (ETSTimer*)malloc(sizeof(ETSTimer));
        if (_timer == nullptr)
            return false;

        os_timer_setfn(_timer, ESP8266WiFiGratuitous::scheduleItForNextYieldOnce, nullptr);
        os_timer_arm(_timer, ms, true);
    }

    return true;
}

}; // experimental::
