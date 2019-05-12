/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __IP_ADDR_H__
#define __IP_ADDR_H__

#include "c_types.h"
#include "ipv4_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Determine if two address are on the same network.
 *
 * @arg addr1 IP address 1
 * @arg addr2 IP address 2
 * @arg mask network identifier mask
 * @return !0 if the network identifiers of both address match
 */
#define ipv4_addr_netcmp(addr1, addr2, mask) (((addr1)->addr & \
        (mask)->addr) == \
        ((addr2)->addr & \
         (mask)->addr))

/** Set an IP address given by the four byte-parts.
    Little-endian version that prevents the use of htonl. */
#define IP4_ADDR(ipaddr, a,b,c,d) \
        (ipaddr)->addr = ((uint32)((d) & 0xff) << 24) | \
                         ((uint32)((c) & 0xff) << 16) | \
                         ((uint32)((b) & 0xff) << 8)  | \
                          (uint32)((a) & 0xff)

#define ipv4_addr1(ipaddr) (((uint8*)(ipaddr))[0])
#define ipv4_addr2(ipaddr) (((uint8*)(ipaddr))[1])
#define ipv4_addr3(ipaddr) (((uint8*)(ipaddr))[2])
#define ipv4_addr4(ipaddr) (((uint8*)(ipaddr))[3])

#define ipv4_addr1_16(ipaddr) ((uint16)ipv4_addr1(ipaddr))
#define ipv4_addr2_16(ipaddr) ((uint16)ipv4_addr2(ipaddr))
#define ipv4_addr3_16(ipaddr) ((uint16)ipv4_addr3(ipaddr))
#define ipv4_addr4_16(ipaddr) ((uint16)ipv4_addr4(ipaddr))


/** 255.255.255.255 */
#define IPADDR_NONE         ((uint32)0xffffffffUL)
/** 0.0.0.0 */
#define IPADDR_ANY          ((uint32)0x00000000UL)
uint32 ipaddr_addr(const char *cp);

#define IP2STR(ipaddr) ipv4_addr1_16(ipaddr), \
    ipv4_addr2_16(ipaddr), \
    ipv4_addr3_16(ipaddr), \
    ipv4_addr4_16(ipaddr)

#define IPSTR "%d.%d.%d.%d"


#ifdef __cplusplus
}
#endif

#endif /* __IP_ADDR_H__ */
