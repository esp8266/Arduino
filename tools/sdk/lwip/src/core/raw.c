/**
 * @file
 * Implementation of raw protocol PCBs for low-level handling of
 * different types of protocols besides (or overriding) those
 * already available in lwIP.
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"

#if LWIP_RAW /* don't build if not configured for use in lwipopts.h */

#include "lwip/def.h"
#include "lwip/memp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/raw.h"
#include "lwip/stats.h"
#include "arch/perf.h"

#include <string.h>

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

/** The list of RAW PCBs */
static struct raw_pcb *raw_pcbs;

/**
 * Determine if in incoming IP packet is covered by a RAW PCB
 * and if so, pass it to a user-provided receive callback function.
 *
 * Given an incoming IP datagram (as a chain of pbufs) this function
 * finds a corresponding RAW PCB and calls the corresponding receive
 * callback function.
 *
 * @param p pbuf to be demultiplexed to a RAW PCB.
 * @param inp network interface on which the datagram was received.
 * @return - 1 if the packet has been eaten by a RAW PCB receive
 *           callback function. The caller MAY NOT not reference the
 *           packet any longer, and MAY NOT call pbuf_free().
 * @return - 0 if packet is not eaten (pbuf is still referenced by the
 *           caller).
 *
 */
u8_t ICACHE_FLASH_ATTR
raw_input(struct pbuf *p, struct netif *inp)
{
  struct raw_pcb *pcb, *prev;
  struct ip_hdr *iphdr;
  s16_t proto;
  u8_t eaten = 0;

  LWIP_UNUSED_ARG(inp);

  iphdr = (struct ip_hdr *)p->payload;
  proto = IPH_PROTO(iphdr);

  prev = NULL;
  pcb = raw_pcbs;
  /* loop through all raw pcbs until the packet is eaten by one */
  /* this allows multiple pcbs to match against the packet by design */
  while ((eaten == 0) && (pcb != NULL)) {
    if ((pcb->protocol == proto) &&
        (ip_addr_isany(&pcb->local_ip) ||
         ip_addr_cmp(&(pcb->local_ip), &current_iphdr_dest))) {
#if IP_SOF_BROADCAST_RECV
      /* broadcast filter? */
      if ((pcb->so_options & SOF_BROADCAST) || !ip_addr_isbroadcast(&current_iphdr_dest, inp))
#endif /* IP_SOF_BROADCAST_RECV */
      {
        /* receive callback function available? */
        if (pcb->recv != NULL) {
          /* the receive callback function did not eat the packet? */
          if (pcb->recv(pcb->recv_arg, pcb, p, ip_current_src_addr()) != 0) {
            /* receive function ate the packet */
            p = NULL;
            eaten = 1;
            if (prev != NULL) {
            /* move the pcb to the front of raw_pcbs so that is
               found faster next time */
              prev->next = pcb->next;
              pcb->next = raw_pcbs;
              raw_pcbs = pcb;
            }
          }
        }
        /* no receive callback function was set for this raw PCB */
      }
      /* drop the packet */
    }
    prev = pcb;
    pcb = pcb->next;
  }
  return eaten;
}

/**
 * Bind a RAW PCB.
 *
 * @param pcb RAW PCB to be bound with a local address ipaddr.
 * @param ipaddr local IP address to bind with. Use IP_ADDR_ANY to
 * bind to all local interfaces.
 *
 * @return lwIP error code.
 * - ERR_OK. Successful. No error occured.
 * - ERR_USE. The specified IP address is already bound to by
 * another RAW PCB.
 *
 * @see raw_disconnect()
 */
err_t ICACHE_FLASH_ATTR
raw_bind(struct raw_pcb *pcb, ip_addr_t *ipaddr)
{
  ip_addr_set(&pcb->local_ip, ipaddr);
  return ERR_OK;
}

/**
 * Connect an RAW PCB. This function is required by upper layers
 * of lwip. Using the raw api you could use raw_sendto() instead
 *
 * This will associate the RAW PCB with the remote address.
 *
 * @param pcb RAW PCB to be connected with remote address ipaddr and port.
 * @param ipaddr remote IP address to connect with.
 *
 * @return lwIP error code
 *
 * @see raw_disconnect() and raw_sendto()
 */
err_t ICACHE_FLASH_ATTR
raw_connect(struct raw_pcb *pcb, ip_addr_t *ipaddr)
{
  ip_addr_set(&pcb->remote_ip, ipaddr);
  return ERR_OK;
}


/**
 * Set the callback function for received packets that match the
 * raw PCB's protocol and binding. 
 * 
 * The callback function MUST either
 * - eat the packet by calling pbuf_free() and returning non-zero. The
 *   packet will not be passed to other raw PCBs or other protocol layers.
 * - not free the packet, and return zero. The packet will be matched
 *   against further PCBs and/or forwarded to another protocol layers.
 * 
 * @return non-zero if the packet was free()d, zero if the packet remains
 * available for others.
 */
void ICACHE_FLASH_ATTR
raw_recv(struct raw_pcb *pcb, raw_recv_fn recv, void *recv_arg)
{
  /* remember recv() callback and user data */
  pcb->recv = recv;
  pcb->recv_arg = recv_arg;
}

/**
 * Send the raw IP packet to the given address. Note that actually you cannot
 * modify the IP headers (this is inconsistent with the receive callback where
 * you actually get the IP headers), you can only specify the IP payload here.
 * It requires some more changes in lwIP. (there will be a raw_send() function
 * then.)
 *
 * @param pcb the raw pcb which to send
 * @param p the IP payload to send
 * @param ipaddr the destination address of the IP packet
 *
 */
err_t ICACHE_FLASH_ATTR
raw_sendto(struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *ipaddr)
{
  err_t err;
  struct netif *netif;
  ip_addr_t *src_ip;
  struct pbuf *q; /* q will be sent down the stack */
  
  LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("raw_sendto\n"));
  
  /* not enough space to add an IP header to first pbuf in given p chain? */
  if (pbuf_header(p, IP_HLEN)) {
    /* allocate header in new pbuf */
    q = pbuf_alloc(PBUF_IP, 0, PBUF_RAM);
    /* new header pbuf could not be allocated? */
    if (q == NULL) {
      LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("raw_sendto: could not allocate header\n"));
      return ERR_MEM;
    }
    if (p->tot_len != 0) {
      /* chain header q in front of given pbuf p */
      pbuf_chain(q, p);
    }
    /* { first pbuf q points to header pbuf } */
    LWIP_DEBUGF(RAW_DEBUG, ("raw_sendto: added header pbuf %p before given pbuf %p\n", (void *)q, (void *)p));
  }  else {
    /* first pbuf q equals given pbuf */
    q = p;
    if(pbuf_header(q, -IP_HLEN)) {
      LWIP_ASSERT("Can't restore header we just removed!", 0);
      return ERR_MEM;
    }
  }

  if ((netif = ip_route(ipaddr)) == NULL) {
    LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_LEVEL_WARNING, ("raw_sendto: No route to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
      ip4_addr1_16(ipaddr), ip4_addr2_16(ipaddr), ip4_addr3_16(ipaddr), ip4_addr4_16(ipaddr)));
    /* free any temporary header pbuf allocated by pbuf_header() */
    if (q != p) {
      pbuf_free(q);
    }
    return ERR_RTE;
  }

#if IP_SOF_BROADCAST
  /* broadcast filter? */
  if (((pcb->so_options & SOF_BROADCAST) == 0) && ip_addr_isbroadcast(ipaddr, netif)) {
    LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_LEVEL_WARNING, ("raw_sendto: SOF_BROADCAST not enabled on pcb %p\n", (void *)pcb));
    /* free any temporary header pbuf allocated by pbuf_header() */
    if (q != p) {
      pbuf_free(q);
    }
    return ERR_VAL;
  }
#endif /* IP_SOF_BROADCAST */

  if (ip_addr_isany(&pcb->local_ip)) {
    /* use outgoing network interface IP address as source address */
    src_ip = &(netif->ip_addr);
  } else {
    /* use RAW PCB local IP address as source address */
    src_ip = &(pcb->local_ip);
  }

#if LWIP_NETIF_HWADDRHINT
  netif->addr_hint = &(pcb->addr_hint);
#endif /* LWIP_NETIF_HWADDRHINT*/
  err = ip_output_if (q, src_ip, ipaddr, pcb->ttl, pcb->tos, pcb->protocol, netif);
#if LWIP_NETIF_HWADDRHINT
  netif->addr_hint = NULL;
#endif /* LWIP_NETIF_HWADDRHINT*/

  /* did we chain a header earlier? */
  if (q != p) {
    /* free the header */
    pbuf_free(q);
  }
  return err;
}

/**
 * Send the raw IP packet to the address given by raw_connect()
 *
 * @param pcb the raw pcb which to send
 * @param p the IP payload to send
 *
 */
err_t ICACHE_FLASH_ATTR
raw_send(struct raw_pcb *pcb, struct pbuf *p)
{
  return raw_sendto(pcb, p, &pcb->remote_ip);
}

/**
 * Remove an RAW PCB.
 *
 * @param pcb RAW PCB to be removed. The PCB is removed from the list of
 * RAW PCB's and the data structure is freed from memory.
 *
 * @see raw_new()
 */
void ICACHE_FLASH_ATTR
raw_remove(struct raw_pcb *pcb)
{
  struct raw_pcb *pcb2;
  /* pcb to be removed is first in list? */
  if (raw_pcbs == pcb) {
    /* make list start at 2nd pcb */
    raw_pcbs = raw_pcbs->next;
    /* pcb not 1st in list */
  } else {
    for(pcb2 = raw_pcbs; pcb2 != NULL; pcb2 = pcb2->next) {
      /* find pcb in raw_pcbs list */
      if (pcb2->next != NULL && pcb2->next == pcb) {
        /* remove pcb from list */
        pcb2->next = pcb->next;
      }
    }
  }
  memp_free(MEMP_RAW_PCB, pcb);
}

/**
 * Create a RAW PCB.
 *
 * @return The RAW PCB which was created. NULL if the PCB data structure
 * could not be allocated.
 *
 * @param proto the protocol number of the IPs payload (e.g. IP_PROTO_ICMP)
 *
 * @see raw_remove()
 */
struct raw_pcb * ICACHE_FLASH_ATTR
raw_new(u8_t proto)
{
  struct raw_pcb *pcb;

  LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("raw_new\n"));

  pcb = (struct raw_pcb *)memp_malloc(MEMP_RAW_PCB);
  /* could allocate RAW PCB? */
  if (pcb != NULL) {
    /* initialize PCB to all zeroes */
    os_memset(pcb, 0, sizeof(struct raw_pcb));
    pcb->protocol = proto;
    pcb->ttl = RAW_TTL;
    pcb->next = raw_pcbs;
    raw_pcbs = pcb;
  }
  return pcb;
}

#endif /* LWIP_RAW */
