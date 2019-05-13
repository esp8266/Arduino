/*
    LEAmDNS_Priv.h

    License (MIT license):
      Permission is hereby granted, free of charge, to any person obtaining a copy
      of this software and associated documentation files (the "Software"), to deal
      in the Software without restriction, including without limitation the rights
      to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
      copies of the Software, and to permit persons to whom the Software is
      furnished to do so, subject to the following conditions:

      The above copyright notice and this permission notice shall be included in
      all copies or substantial portions of the Software.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
      IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
      FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
      AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
      LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
      THE SOFTWARE.

*/

#ifndef MDNS_PRIV_H
#define MDNS_PRIV_H

namespace esp8266
{

/*
    LEAmDNS
*/

namespace MDNSImplementation
{

// Enable class debug functions
#define ESP_8266_MDNS_INCLUDE
//#define DEBUG_ESP_MDNS_RESPONDER


#ifndef LWIP_OPEN_SRC
#define LWIP_OPEN_SRC
#endif

//
// If ENABLE_ESP_MDNS_RESPONDER_PASSIV_MODE is defined, the mDNS responder ignores a successful probing
// This allows to drive the responder in a environment, where 'update()' isn't called in the loop
//#define ENABLE_ESP_MDNS_RESPONDER_PASSIV_MODE

// Enable/disable debug trace macros
#ifdef DEBUG_ESP_MDNS_RESPONDER
#define DEBUG_ESP_MDNS_INFO
#define DEBUG_ESP_MDNS_ERR
#define DEBUG_ESP_MDNS_TX
#define DEBUG_ESP_MDNS_RX
#endif

#ifdef DEBUG_ESP_MDNS_RESPONDER
#ifdef DEBUG_ESP_MDNS_INFO
#define DEBUG_EX_INFO(A)    A
#else
#define DEBUG_EX_INFO(A)
#endif
#ifdef DEBUG_ESP_MDNS_ERR
#define DEBUG_EX_ERR(A) A
#else
#define DEBUG_EX_ERR(A)
#endif
#ifdef DEBUG_ESP_MDNS_TX
#define DEBUG_EX_TX(A)  A
#else
#define DEBUG_EX_TX(A)
#endif
#ifdef DEBUG_ESP_MDNS_RX
#define DEBUG_EX_RX(A)  A
#else
#define DEBUG_EX_RX(A)
#endif

#ifdef DEBUG_ESP_PORT
#define DEBUG_OUTPUT DEBUG_ESP_PORT
#else
#define DEBUG_OUTPUT Serial
#endif
#else
#define DEBUG_EX_INFO(A)
#define DEBUG_EX_ERR(A)
#define DEBUG_EX_TX(A)
#define DEBUG_EX_RX(A)
#endif


/*  Replaced by 'lwip/prot/dns.h' definitions
    #ifdef MDNS_IP4_SUPPORT
    #define MDNS_MULTICAST_ADDR_IP4     (IPAddress(224, 0, 0, 251)) // ip_addr_t v4group = DNS_MQUERY_IPV4_GROUP_INIT
    #endif
    #ifdef MDNS_IP6_SUPPORT
    #define MDNS_MULTICAST_ADDR_IP6     (IPAddress("FF02::FB"))     // ip_addr_t v6group = DNS_MQUERY_IPV6_GROUP_INIT
    #endif*/
//#define MDNS_MULTICAST_PORT               5353

/*
    This is NOT the TTL (Time-To-Live) for MDNS records, but the
    subnet level distance MDNS records should travel.
    1 sets the subnet distance to 'local', which is default for MDNS.
    (Btw.: 255 would set it to 'as far as possible' -> internet)

    However, RFC 3171 seems to force 255 instead
*/
#define MDNS_MULTICAST_TTL              255/*1*/

/*
    This is the MDNS record TTL
    Host level records are set to 2min (120s)
    service level records are set to 75min (4500s)
*/
#define MDNS_HOST_TTL                   120
#define MDNS_SERVICE_TTL                4500

/*
    Compressed labels are flaged by the two topmost bits of the length byte being set
*/
#define MDNS_DOMAIN_COMPRESS_MARK       0xC0
/*
    Avoid endless recursion because of malformed compressed labels
*/
#define MDNS_DOMAIN_MAX_REDIRCTION      6

/*
    Default service priority and weight in SRV answers
*/
#define MDNS_SRV_PRIORITY               0
#define MDNS_SRV_WEIGHT                 0

/*
    Delay between and number of probes for host and service domains
    Delay between and number of announces for host and service domains
    Delay between and number of service queries; the delay is multiplied by the resent number in '_checkServiceQueryCache'
*/
#define MDNS_PROBE_DELAY                250
#define MDNS_PROBE_COUNT                3
#define MDNS_ANNOUNCE_DELAY             1000
#define MDNS_ANNOUNCE_COUNT             8
#define MDNS_DYNAMIC_QUERY_RESEND_COUNT 5
#define MDNS_DYNAMIC_QUERY_RESEND_DELAY 5000


/*
    Force host domain to use only lowercase letters
*/
//#define MDNS_FORCE_LOWERCASE_HOSTNAME

/*
    Enable/disable the usage of the F() macro in debug trace printf calls.
    There needs to be an PGM comptible printf function to use this.

    USE_PGM_PRINTF and F
*/
#define USE_PGM_PRINTF

#ifdef USE_PGM_PRINTF
#else
#ifdef F
#undef F
#endif
#define F(A)    A
#endif

}   // namespace MDNSImplementation

} // namespace esp8266

// Include the main header, so the submodlues only need to include this header
#include "LEAmDNS.h"


#endif  // MDNS_PRIV_H
