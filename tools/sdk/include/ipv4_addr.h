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

#ifndef __IPV4_ADDR_H__
#define __IPV4_ADDR_H__

#include <stdint.h>
#include <lwip/init.h>

// ipv4_addr is necessary for lwIP-v2 because
// - espressif binary firmware is IPv4 only, under the name of ip_addr/_t
// - ip_addr/_t is different when IPv6 is enabled with lwIP-v2
// hence ipv4_addr/t is IPv4 version/copy of IPv4 ip_addr/_t
// when IPv6 is enabled so we can deal with IPv4 use from firmware API.

#define ipv4_addr ip4_addr
#define ipv4_addr_t ip4_addr_t

// official lwIP's definitions
#include "lwip/ip_addr.h"
#if LWIP_VERSION_MAJOR == 1
struct ip4_addr { uint32_t addr; };
typedef struct ip4_addr ip4_addr_t;
#else
#include <lwip/ip4_addr.h>

// defined in lwip-v1.4 sources only, used in fw
struct ip_info {
    struct ipv4_addr ip;
    struct ipv4_addr netmask;
    struct ipv4_addr gw;
};

#endif
#endif // __IPV4_ADDR_H__
