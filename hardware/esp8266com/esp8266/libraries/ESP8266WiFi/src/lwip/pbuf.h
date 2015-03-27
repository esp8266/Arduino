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

#ifndef __LWIP_PBUF_H__
#define __LWIP_PBUF_H__

#include "lwip/opt.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Currently, the pbuf_custom code is only needed for one specific configuration
 * of IP_FRAG */
#define LWIP_SUPPORT_CUSTOM_PBUF (IP_FRAG && !IP_FRAG_USES_STATIC_BUF && !LWIP_NETIF_TX_SINGLE_PBUF)

#define PBUF_TRANSPORT_HLEN 20
#define PBUF_IP_HLEN        20

typedef enum {
  PBUF_TRANSPORT,
  PBUF_IP,
  PBUF_LINK,
  PBUF_RAW
} pbuf_layer;

typedef enum {
  PBUF_RAM, /* pbuf data is stored in RAM */
  PBUF_ROM, /* pbuf data is stored in ROM */
  PBUF_REF, /* pbuf comes from the pbuf pool */
  PBUF_POOL, /* pbuf payload refers to RAM */
#ifdef EBUF_LWIP
  PBUF_ESF_RX /* pbuf payload is from WLAN */
#endif /* ESF_LWIP */
} pbuf_type;


/** indicates this packet's data should be immediately passed to the application */
#define PBUF_FLAG_PUSH      0x01U
/** indicates this is a custom pbuf: pbuf_free and pbuf_header handle such a
    a pbuf differently */
#define PBUF_FLAG_IS_CUSTOM 0x02U
/** indicates this pbuf is UDP multicast to be looped back */
#define PBUF_FLAG_MCASTLOOP 0x04U

struct pbuf {
  /** next pbuf in singly linked pbuf chain */
  struct pbuf *next;

  /** pointer to the actual data in the buffer */
  void *payload;

  /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
  u16_t tot_len;

  /** length of this buffer */
  u16_t len;

  /** pbuf_type as u8_t instead of enum to save space */
  u8_t /*pbuf_type*/ type;

  /** misc flags */
  u8_t flags;

  /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
  u16_t ref;
  
  /* add a pointer for esf_buf */
  void * eb; 
};

#if LWIP_SUPPORT_CUSTOM_PBUF
/** Prototype for a function to free a custom pbuf */
typedef void (*pbuf_free_custom_fn)(struct pbuf *p);
  
/** A custom pbuf: like a pbuf, but following a function pointer to free it. */
struct pbuf_custom {
  /** The actual pbuf */
  struct pbuf pbuf;
  /** This function is called when pbuf_free deallocates this pbuf(_custom) */
  pbuf_free_custom_fn custom_free_function;
};
#endif /* LWIP_SUPPORT_CUSTOM_PBUF */

/* Initializes the pbuf module. This call is empty for now, but may not be in future. */
#define pbuf_init()

struct pbuf *pbuf_alloc(pbuf_layer l, u16_t length, pbuf_type type)ICACHE_FLASH_ATTR;
#if LWIP_SUPPORT_CUSTOM_PBUF
struct pbuf *pbuf_alloced_custom(pbuf_layer l, u16_t length, pbuf_type type,
                                 struct pbuf_custom *p, void *payload_mem,
                                 u16_t payload_mem_len)ICACHE_FLASH_ATTR;
#endif /* LWIP_SUPPORT_CUSTOM_PBUF */
void pbuf_realloc(struct pbuf *p, u16_t size)ICACHE_FLASH_ATTR;
u8_t pbuf_header(struct pbuf *p, s16_t header_size)ICACHE_FLASH_ATTR;
void pbuf_ref(struct pbuf *p)ICACHE_FLASH_ATTR;
u8_t pbuf_free(struct pbuf *p)ICACHE_FLASH_ATTR;
u8_t pbuf_clen(struct pbuf *p)ICACHE_FLASH_ATTR;
void pbuf_cat(struct pbuf *head, struct pbuf *tail)ICACHE_FLASH_ATTR;
void pbuf_chain(struct pbuf *head, struct pbuf *tail)ICACHE_FLASH_ATTR;
struct pbuf *pbuf_dechain(struct pbuf *p)ICACHE_FLASH_ATTR;
err_t pbuf_copy(struct pbuf *p_to, struct pbuf *p_from)ICACHE_FLASH_ATTR;
u16_t pbuf_copy_partial(struct pbuf *p, void *dataptr, u16_t len, u16_t offset)ICACHE_FLASH_ATTR;
err_t pbuf_take(struct pbuf *buf, const void *dataptr, u16_t len)ICACHE_FLASH_ATTR;
struct pbuf *pbuf_coalesce(struct pbuf *p, pbuf_layer layer)ICACHE_FLASH_ATTR;
#if LWIP_CHECKSUM_ON_COPY
err_t pbuf_fill_chksum(struct pbuf *p, u16_t start_offset, const void *dataptr,
                       u16_t len, u16_t *chksum)ICACHE_FLASH_ATTR;
#endif /* LWIP_CHECKSUM_ON_COPY */

u8_t pbuf_get_at(struct pbuf* p, u16_t offset)ICACHE_FLASH_ATTR;
u16_t pbuf_memcmp(struct pbuf* p, u16_t offset, const void* s2, u16_t n)ICACHE_FLASH_ATTR;
u16_t pbuf_memfind(struct pbuf* p, const void* mem, u16_t mem_len, u16_t start_offset)ICACHE_FLASH_ATTR;
u16_t pbuf_strstr(struct pbuf* p, const char* substr)ICACHE_FLASH_ATTR;

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_PBUF_H__ */
