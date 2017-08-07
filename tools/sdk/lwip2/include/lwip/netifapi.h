/**
 * @file
 * netif API (to be used from non-TCPIP threads)
 */

/*
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
 */
#ifndef LWIP_HDR_NETIFAPI_H
#define LWIP_HDR_NETIFAPI_H

#include "lwip/opt.h"

#if LWIP_NETIF_API /* don't build if not configured for use in lwipopts.h */

#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/priv/tcpip_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_MPU_COMPATIBLE
#define NETIFAPI_IPADDR_DEF(type, m)  type m
#else /* LWIP_MPU_COMPATIBLE */
#define NETIFAPI_IPADDR_DEF(type, m)  const type * m
#endif /* LWIP_MPU_COMPATIBLE */

typedef void (*netifapi_void_fn)(struct netif *netif);
typedef err_t (*netifapi_errt_fn)(struct netif *netif);

struct netifapi_msg {
  struct tcpip_api_call_data call;
  struct netif *netif;
  union {
    struct {
#if LWIP_IPV4
      NETIFAPI_IPADDR_DEF(ip4_addr_t, ipaddr);
      NETIFAPI_IPADDR_DEF(ip4_addr_t, netmask);
      NETIFAPI_IPADDR_DEF(ip4_addr_t, gw);
#endif /* LWIP_IPV4 */
      void *state;
      netif_init_fn init;
      netif_input_fn input;
    } add;
    struct {
      netifapi_void_fn voidfunc;
      netifapi_errt_fn errtfunc;
    } common;
  } msg;
};


/* API for application */
err_t netifapi_netif_add(struct netif *netif,
#if LWIP_IPV4
                         const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw,
#endif /* LWIP_IPV4 */
                         void *state, netif_init_fn init, netif_input_fn input);

#if LWIP_IPV4
err_t netifapi_netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr,
                              const ip4_addr_t *netmask, const ip4_addr_t *gw);
#endif /* LWIP_IPV4*/

err_t netifapi_netif_common(struct netif *netif, netifapi_void_fn voidfunc,
                            netifapi_errt_fn errtfunc);

/** @ingroup netifapi_netif */
#define netifapi_netif_remove(n)        netifapi_netif_common(n, netif_remove, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_up(n)        netifapi_netif_common(n, netif_set_up, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_down(n)      netifapi_netif_common(n, netif_set_down, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_default(n)   netifapi_netif_common(n, netif_set_default, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_link_up(n)   netifapi_netif_common(n, netif_set_link_up, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_link_down(n) netifapi_netif_common(n, netif_set_link_down, NULL)

/**
 * @defgroup netifapi_dhcp4 DHCPv4
 * @ingroup netifapi
 * To be called from non-TCPIP threads
 */
/** @ingroup netifapi_dhcp4 */
#define netifapi_dhcp_start(n)        netifapi_netif_common(n, NULL, dhcp_start)
/** @ingroup netifapi_dhcp4 */
#define netifapi_dhcp_stop(n)         netifapi_netif_common(n, dhcp_stop, NULL)
/** @ingroup netifapi_dhcp4 */
#define netifapi_dhcp_inform(n)       netifapi_netif_common(n, dhcp_inform, NULL)
/** @ingroup netifapi_dhcp4 */
#define netifapi_dhcp_renew(n)        netifapi_netif_common(n, NULL, dhcp_renew)
/** @ingroup netifapi_dhcp4 */
#define netifapi_dhcp_release(n)      netifapi_netif_common(n, NULL, dhcp_release)

/**
 * @defgroup netifapi_autoip AUTOIP
 * @ingroup netifapi
 * To be called from non-TCPIP threads
 */
/** @ingroup netifapi_autoip */
#define netifapi_autoip_start(n)      netifapi_netif_common(n, NULL, autoip_start)
/** @ingroup netifapi_autoip */
#define netifapi_autoip_stop(n)       netifapi_netif_common(n, NULL, autoip_stop)

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETIF_API */

#endif /* LWIP_HDR_NETIFAPI_H */
