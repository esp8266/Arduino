/*
 ESP8266WiFiGratuitous.h - esp8266 Wifi support
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

#ifndef ESP8266WIFIGRATUITOUS_H_
#define ESP8266WIFIGRATUITOUS_H_

#include <stdint.h>  // uint32_t
#include <ets_sys.h> // ETSTimer

namespace experimental
{

class ESP8266WiFiGratuitous
{
public:

    // disable(0) or enable/update automatic sending of Gratuitous ARP packets.
    // A gratuitous ARP packet is immediately sent when calling this function, then
    // based on a time interval in milliseconds, default = 1s
    // return value: true when started, false otherwise
    static bool stationKeepAliveSetIntervalMs (uint32_t ms = 1000);

    // request for stopping arp gratuitous packets
    static void stationKeepAliveStop () { (void)stationKeepAliveSetIntervalMs(0); }

    // immediately send one gratuitous ARP from STA
    static void stationKeepAliveNow ();

protected:

    static void scheduleItForNextYieldOnce (void*);

    static ETSTimer* _timer;
};

}; // experimental::

#endif // ESP8266WIFIGRATUITOUS_H_
