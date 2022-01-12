/*
    ESP8266mDNS.h - mDNSResponder for ESP8266 family
    This file is part of the esp8266 core for Arduino environment.

    mDNS implementation, that supports many mDNS features like:
    - Presenting a DNS-SD service to interested observers, eg. a http server by presenting _http._tcp service
    - Support for multi-level compressed names in input; in output only a very simple one-leven full-name compression is implemented
    - Probing host and service domains for uniqueness in the local network
    - Tiebreaking while probing is supported in a very minimalistic way (the 'higher' IP address wins the tiebreak)
    - Announcing available services after successful probing
    - Using fixed service TXT items or
    - Using dynamic service TXT items for presented services (via callback)
    - Remove services (and un-announcing them to the observers by sending goodbye-messages)
    - Static queries for DNS-SD services (creating a fixed answer set after a certain timeout period)
    - Dynamic queries for DNS-SD services with cached and updated answers and user notifications
    - Support for multi-homed client host domains

    See 'src/LEAmDNS.h' for implementation details, configuration and usage information.
    See 'examples/LEAmDNS/' for examples of the new features.

    LEAmDNS is expected to be compatible with the original ESP8266mDNS implementation, and it can be
    used as a drop-in replacement in existing projects.


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

#ifndef __ESP8266MDNS_H
#define __ESP8266MDNS_H

#include "LEAmDNS.h"            // LEA

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
// Maps the implementation to use to the global namespace type
using MDNSResponder = esp8266::MDNSImplementation::MDNSResponder;                // LEA

extern MDNSResponder MDNS;
#endif

#endif // __ESP8266MDNS_H
