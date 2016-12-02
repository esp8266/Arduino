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
#ifndef __LWIP_NETBUF_H__
#define __LWIP_NETBUF_H__

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** This netbuf has dest-addr/port set */
#define NETBUF_FLAG_DESTADDR    0x01
/** This netbuf includes a checksum */
#define NETBUF_FLAG_CHKSUM      0x02

struct netbuf {
  struct pbuf *p, *ptr;
  ip_addr_t addr;
  u16_t port;
#if LWIP_NETBUF_RECVINFO || LWIP_CHECKSUM_ON_COPY
#if LWIP_CHECKSUM_ON_COPY
  u8_t flags;
#endif /* LWIP_CHECKSUM_ON_COPY */
  u16_t toport_chksum;
#if LWIP_NETBUF_RECVINFO
  ip_addr_t toaddr;
#endif /* LWIP_NETBUF_RECVINFO */
#endif /* LWIP_NETBUF_RECVINFO || LWIP_CHECKSUM_ON_COPY */
};

/* Network buffer functions: */
struct netbuf *   netbuf_new      (void)ICACHE_FLASH_ATTR;
void              netbuf_delete   (struct netbuf *buf)ICACHE_FLASH_ATTR;
void *            netbuf_alloc    (struct netbuf *buf, u16_t size)ICACHE_FLASH_ATTR;
void              netbuf_free     (struct netbuf *buf)ICACHE_FLASH_ATTR;
err_t             netbuf_ref      (struct netbuf *buf,
                                   const void *dataptr, u16_t size)ICACHE_FLASH_ATTR;
void              netbuf_chain    (struct netbuf *head,
           struct netbuf *tail)ICACHE_FLASH_ATTR;

err_t             netbuf_data     (struct netbuf *buf,
                                   void **dataptr, u16_t *len)ICACHE_FLASH_ATTR;
s8_t              netbuf_next     (struct netbuf *buf)ICACHE_FLASH_ATTR;
void              netbuf_first    (struct netbuf *buf)ICACHE_FLASH_ATTR;


#define netbuf_copy_partial(buf, dataptr, len, offset) \
  pbuf_copy_partial((buf)->p, (dataptr), (len), (offset))
#define netbuf_copy(buf,dataptr,len) netbuf_copy_partial(buf, dataptr, len, 0)
#define netbuf_take(buf, dataptr, len) pbuf_take((buf)->p, dataptr, len)
#define netbuf_len(buf)              ((buf)->p->tot_len)
#define netbuf_fromaddr(buf)         (&((buf)->addr))
#define netbuf_set_fromaddr(buf, fromaddr) ip_addr_set((&(buf)->addr), fromaddr)
#define netbuf_fromport(buf)         ((buf)->port)
#if LWIP_NETBUF_RECVINFO
#define netbuf_destaddr(buf)         (&((buf)->toaddr))
#define netbuf_set_destaddr(buf, destaddr) ip_addr_set((&(buf)->addr), destaddr)
#define netbuf_destport(buf)         (((buf)->flags & NETBUF_FLAG_DESTADDR) ? (buf)->toport_chksum : 0)
#endif /* LWIP_NETBUF_RECVINFO */
#if LWIP_CHECKSUM_ON_COPY
#define netbuf_set_chksum(buf, chksum) do { (buf)->flags = NETBUF_FLAG_CHKSUM; \
                                            (buf)->toport_chksum = chksum; } while(0)
#endif /* LWIP_CHECKSUM_ON_COPY */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_NETBUF_H__ */
