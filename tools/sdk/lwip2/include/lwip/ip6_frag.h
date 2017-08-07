/**
 * @file
 *
 * IPv6 fragmentation and reassembly.
 */

/*
 * Copyright (c) 2010 Inico Technologies Ltd.
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
 * Author: Ivan Delamer <delamer@inicotech.com>
 *
 *
 * Please coordinate changes and requests with Ivan Delamer
 * <delamer@inicotech.com>
 */
#ifndef LWIP_HDR_IP6_FRAG_H
#define LWIP_HDR_IP6_FRAG_H

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ip6_addr.h"
#include "lwip/ip6.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif


#if LWIP_IPV6 && LWIP_IPV6_REASS  /* don't build if not configured for use in lwipopts.h */

/** IP6_FRAG_COPYHEADER==1: for platforms where sizeof(void*) > 4, this needs to
 * be enabled (to not overwrite part of the data). When enabled, the IPv6 header
 * is copied instead of referencing it, which gives more room for struct ip6_reass_helper */
#ifndef IPV6_FRAG_COPYHEADER
#define IPV6_FRAG_COPYHEADER   0
#endif

/** The IPv6 reassembly timer interval in milliseconds. */
#define IP6_REASS_TMR_INTERVAL 1000

/* Copy the complete header of the first fragment to struct ip6_reassdata
   or just point to its original location in the first pbuf? */
#if IPV6_FRAG_COPYHEADER
#define IPV6_FRAG_HDRPTR
#define IPV6_FRAG_HDRREF(hdr) (&(hdr))
#else /* IPV6_FRAG_COPYHEADER */
#define IPV6_FRAG_HDRPTR *
#define IPV6_FRAG_HDRREF(hdr) (hdr)
#endif /* IPV6_FRAG_COPYHEADER */

/** IPv6 reassembly helper struct.
 * This is exported because memp needs to know the size.
 */
struct ip6_reassdata {
  struct ip6_reassdata *next;
  struct pbuf *p;
  struct ip6_hdr IPV6_FRAG_HDRPTR iphdr;
  u32_t identification;
  u16_t datagram_len;
  u8_t nexth;
  u8_t timer;
};

#define ip6_reass_init() /* Compatibility define */
void ip6_reass_tmr(void);
struct pbuf *ip6_reass(struct pbuf *p);

#endif /* LWIP_IPV6 && LWIP_IPV6_REASS */

#if LWIP_IPV6 && LWIP_IPV6_FRAG  /* don't build if not configured for use in lwipopts.h */

#ifndef LWIP_PBUF_CUSTOM_REF_DEFINED
#define LWIP_PBUF_CUSTOM_REF_DEFINED
/** A custom pbuf that holds a reference to another pbuf, which is freed
 * when this custom pbuf is freed. This is used to create a custom PBUF_REF
 * that points into the original pbuf. */
struct pbuf_custom_ref {
  /** 'base class' */
  struct pbuf_custom pc;
  /** pointer to the original pbuf that is referenced */
  struct pbuf *original;
};
#endif /* LWIP_PBUF_CUSTOM_REF_DEFINED */

err_t ip6_frag(struct pbuf *p, struct netif *netif, const ip6_addr_t *dest);

#endif /* LWIP_IPV6 && LWIP_IPV6_FRAG */


#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_IP6_FRAG_H */
