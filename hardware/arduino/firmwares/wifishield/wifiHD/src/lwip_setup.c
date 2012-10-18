/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \file lwIP setup code
 *
 * \brief Collects the lwIP setup code that an application has to
 *        execute in a standalone environment.
 *
 * \author               H&D Wireless AB \n
 *
 */

#include <lwip/init.h>
#include <lwip/dhcp.h>
#include <lwip/tcp.h>
#include <ipv4/lwip/ip_frag.h>
#include <netif/etharp.h>
#include <netif/wlif.h>
#include <timer.h>

#include "lwip_setup.h"
#include "lwip/dns.h"


/**
 *
 */
static void
tcp_tmr_cb(void *ctx)
{
        tcp_tmr();
}

/**
 *
 */
static void
ip_tmr_cb(void *ctx)
{
        ip_reass_tmr();
}

/**
 *
 */
static void
dns_tmr_cb(void *ctx)
{
	dns_tmr();
}

/**
 *
 */
static void
etharp_tmr_cb(void *ctx)
{
        etharp_tmr();
}


/**
 *
 */
static void
dhcp_fine_tmr_cb(void *ctx)
{
    dhcp_fine_tmr();
}

/**
 *
 */
static void
dhcp_coarse_tmr_cb(void *ctx)
{
        dhcp_coarse_tmr();
}

int start_ip_stack(struct net_cfg *cfg, 
                   struct ip_addr ipaddr, 
                   struct ip_addr netmask,
                   struct ip_addr gw) {

        if (cfg->dhcp_enabled) {
                IP4_ADDR(&gw, 0,0,0,0);
                IP4_ADDR(&ipaddr, 0,0,0,0);
                IP4_ADDR(&netmask, 0,0,0,0);
        }
        
        /* add wl to lwip interface list and set as default */
        cfg->netif = netif_add(cfg->netif, 
                               &ipaddr, 
                               &netmask, 
                               &gw, 
                               NULL,
                               wlif_init, /* init */
                               ethernet_input /* handles ARP and IP packets */);

        if (cfg->netif == NULL)
            return -1;
        netif_set_default(cfg->netif);

        /* register lwip timer callbacks for tcp, arp and dhcp protocols */
        timer_sched_timeout_cb(5000, TIMEOUT_PERIODIC, 
                               etharp_tmr_cb, NULL);
        timer_sched_timeout_cb(TCP_TMR_INTERVAL, TIMEOUT_PERIODIC, 
                               tcp_tmr_cb, NULL);
        timer_sched_timeout_cb(DHCP_FINE_TIMER_MSECS, TIMEOUT_PERIODIC, 
                               dhcp_fine_tmr_cb, NULL);
        timer_sched_timeout_cb(DHCP_COARSE_TIMER_MSECS, TIMEOUT_PERIODIC,
                               dhcp_coarse_tmr_cb, NULL);
        timer_sched_timeout_cb(IP_TMR_INTERVAL, TIMEOUT_PERIODIC,
                               ip_tmr_cb, NULL);
        timer_sched_timeout_cb(DNS_TMR_INTERVAL, TIMEOUT_PERIODIC,
                                      dns_tmr_cb, NULL);

        return 1;
}
