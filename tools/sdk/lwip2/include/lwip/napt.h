/**
 * @file
 * Network Address and Port Translation
 */

/*
 * Copyright (c) 2016-2019 NeoCat <neocat@neocat.jp>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: NeoCat <neocat@neocat.jp>
 *
 */

#ifndef __LWIP_NAPT_H__
#define __LWIP_NAPT_H__

#include "lwip/opt.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IP_FORWARD
#if IP_NAPT

/* Default size of the tables used for NAPT */
#define IP_NAPT_MAX 512
#define IP_PORTMAP_MAX 32

/* Timeouts in sec for the various protocol types */
#define IP_NAPT_TIMEOUT_MS_TCP (30*60*1000)
#define IP_NAPT_TIMEOUT_MS_TCP_DISCON (20*1000)
#define IP_NAPT_TIMEOUT_MS_UDP (2*1000)
#define IP_NAPT_TIMEOUT_MS_ICMP (2*1000)

#define IP_NAPT_PORT_RANGE_START 49152
#define IP_NAPT_PORT_RANGE_END   61439

struct napt_table {
  u32_t last;
  u32_t src;
  u32_t dest;
  u16_t sport;
  u16_t dport;
  u16_t mport;
  u8_t proto;
  u8_t fin1 : 1;
  u8_t fin2 : 1;
  u8_t finack1 : 1;
  u8_t finack2 : 1;
  u8_t synack : 1;
  u8_t rst : 1;
  u16_t next, prev;
};

struct portmap_table {
  u32_t maddr;
  u32_t daddr;
  u16_t mport;
  u16_t dport;
  u8_t proto;
  u8_t valid;
};

extern struct portmap_table *ip_portmap_table;

/**
 * Allocates and initializes the NAPT tables.
 *
 * @param max_nat max number of enties in the NAPT table (use IP_NAPT_MAX if in doubt)
 * @param max_portmap max number of enties in the NAPT table (use IP_PORTMAP_MAX if in doubt)
 */
err_t
ip_napt_init(u16_t max_nat, u8_t max_portmap);


/**
 * Enable/Disable NAPT for a specified interface.
 *
 * @param addr ip address of the interface
 * @param enable non-zero to enable NAPT, or 0 to disable.
 */
err_t
ip_napt_enable(u32_t addr, int enable);


/**
 * Enable/Disable NAPT for a specified interface.
 *
 * @param netif number of the interface
 * @param enable non-zero to enable NAPT, or 0 to disable.
 */
err_t
ip_napt_enable_no(u8_t number, int enable);


/**
 * Register port mapping on the external interface to internal interface.
 * When the same port mapping is registered again, the old mapping is overwritten.
 * In this implementation, only 1 unique port mapping can be defined for each target address/port.
 *
 * @param proto target protocol
 * @param maddr ip address of the external interface
 * @param mport mapped port on the external interface, in host byte order.
 * @param daddr destination ip address
 * @param dport destination port, in host byte order.
 */
u8_t 
ip_portmap_add(u8_t proto, u32_t maddr, u16_t mport, u32_t daddr, u16_t dport);


/**
 * Unregister port mapping on the external interface to internal interface.
 *
 * @param proto target protocol
 * @param maddr ip address of the external interface
 */
u8_t 
ip_portmap_remove(u8_t proto, u16_t mport);

#endif /* IP_NAPT */
#endif /* IP_FORWARD */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_NAPT_H__ */
