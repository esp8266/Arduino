/*
    ESP8266mDNS.h - mDNSResponder for ESP8266 family
    This file is part of the esp8266 core for Arduino environment.

    Legacy_ESP8266mDNS:
    The well known, thouroughly tested (yet no flawless) default mDNS library for the ESP8266 family

    LEA_ESP8266mDNS:
    An (currently) experimental mDNS implementation, that supports a lot more of mDNS features than Legacy_ESP8266mDNS, like:
    - Presenting a DNS-SD service to interested observers, eg. a http server by presenting _http._tcp service
    - Support for multi-level compressed names in input; in output only a very simple one-leven full-name compression is implemented
    - Probing host and service domains for uniqueness in the local network
    - Tiebreaking while probing is supportet in a very minimalistic way (the 'higher' IP address wins the tiebreak)
    - Announcing available services after successful probing
    - Using fixed service TXT items or
    - Using dynamic service TXT items for presented services (via callback)
    - Remove services (and un-announcing them to the observers by sending goodbye-messages)
    - Static queries for DNS-SD services (creating a fixed answer set after a certain timeout period)
    - Dynamic queries for DNS-SD services with cached and updated answers and user notifications
    - Support for multi-homed client host domains

    See 'LEA_ESP8266mDNS/EPS8266mDNS.h' for more implementation details and usage informations.
    See 'examples/mDNS_Clock' and 'examples/mDNS_ServiceMonitor' for implementation examples of the advanced features.

    LEA_ESP8266mDNS is (mostly) client source code compatible to 'Legacy_ESP8266mDNS', so it could be
    use as a 'drop-in' replacement in existing projects.


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

enum class MDNSApiVersion { Legacy, LEA, LEAv2Compat, LEAv2 };

#include "ESP8266mDNS_Legacy.h" // Legacy
#include "LEAmDNS.h"            // LEA
#include "LEAmDNS2_Legacy.h"    // LEAv2Compat - replacement for LEA using v2
#include "LEAmDNS2Host.h"       // LEAv2       - API updated

// clsMDNSHost replaces MDNSResponder in LEAv2
using clsMDNSHost = esp8266::experimental::clsLEAMDNSHost;

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
// Maps the implementation to use to the global namespace type
//using MDNSResponder = Legacy_MDNSResponder::MDNSResponder;                // Legacy
using MDNSResponder = esp8266::MDNSImplementation::MDNSResponder;           // LEA
//using MDNSResponder = experimental::MDNSImplementation::clsLEAMDNSHost_Legacy; // LEAv2Compat
//using MDNSResponder = clsMDNSHost;                                        // LEAv2

extern MDNSResponder MDNS;
#endif
