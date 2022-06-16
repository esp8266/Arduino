/*
    lwIPDhcpServer.c - DHCP server

    Copyright (c) 2016 Espressif. All rights reserved.
    Copyright (c) 2020 esp8266 arduino. All rights reserved.
    This file is part of the esp8266 core for Arduino environment.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    // original sources (no license provided)
    // ESP8266_NONOS_SDK/third_party/lwip/app/dhcpserver.c
    // ESP8266_NONOS_SDK/third_party/include/lwip/app/dhcpserver.h
*/

// lwIPDhcpServer.{cc,h} encapsulate original nonos-sdk dhcp server
// nearly as-is. This is an initial version to guaranty legacy behavior
// with same default values.

// vv this comment is supposed to be removed after the first commit
// Logic and coding style in this file can be wrong but left to the closest
// of the initial version for easier issue tracking.
// (better is enemy of [good = already working])
// ^^ this comment is supposed to be removed after the first commit

#include <lwip/init.h>  // LWIP_VERSION

#define USE_DNS

#include "lwip/inet.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "osapi.h"

#include "LwipDhcpServer.h"

#include "user_interface.h"
#include "mem.h"

#include <cstring>
#include <sys/pgmspace.h>

typedef struct dhcps_state
{
    sint16_t state;
} dhcps_state;

typedef struct dhcps_msg
{
    uint8_t  op, htype, hlen, hops;
    uint8_t  xid[4];
    uint16_t secs, flags;
    uint8_t  ciaddr[4];
    uint8_t  yiaddr[4];
    uint8_t  siaddr[4];
    uint8_t  giaddr[4];
    uint8_t  chaddr[16];
    uint8_t  sname[64];
    uint8_t  file[128];
    uint8_t  options[312];
} dhcps_msg;

#ifndef LWIP_OPEN_SRC
struct dhcps_lease
{
    bool             enable;
    struct ipv4_addr start_ip;
    struct ipv4_addr end_ip;
};

#endif

typedef enum
{
    DHCPS_TYPE_DYNAMIC,
    DHCPS_TYPE_STATIC
} dhcps_type_t;

typedef enum
{
    DHCPS_STATE_ONLINE,
    DHCPS_STATE_OFFLINE
} dhcps_state_t;

struct dhcps_pool
{
    struct ipv4_addr ip;
    uint8            mac[6];
    uint32           lease_timer;
    dhcps_type_t     type;
    dhcps_state_t    state;
};

#define DHCPS_MAX_LEASE 0x64
#define BOOTP_BROADCAST 0x8000

#define DHCP_REQUEST 1
#define DHCP_REPLY 2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET 6
#define DHCP_MSG_LEN 236

#define DHCPS_SERVER_PORT 67
#define DHCPS_CLIENT_PORT 68

static constexpr uint8_t DHCPDISCOVER = 1;
static constexpr uint8_t DHCPOFFER    = 2;
static constexpr uint8_t DHCPREQUEST  = 3;
static constexpr uint8_t DHCPDECLINE  = 4;
static constexpr uint8_t DHCPACK      = 5;
static constexpr uint8_t DHCPNAK      = 6;
static constexpr uint8_t DHCPRELEASE  = 7;

#define DHCP_OPTION_SUBNET_MASK 1
#define DHCP_OPTION_ROUTER 3
#define DHCP_OPTION_DNS_SERVER 6
#define DHCP_OPTION_REQ_IPADDR 50
#define DHCP_OPTION_LEASE_TIME 51
#define DHCP_OPTION_MSG_TYPE 53
#define DHCP_OPTION_SERVER_ID 54
#define DHCP_OPTION_INTERFACE_MTU 26
#define DHCP_OPTION_PERFORM_ROUTER_DISCOVERY 31
#define DHCP_OPTION_BROADCAST_ADDRESS 28
#define DHCP_OPTION_REQ_LIST 55
#define DHCP_OPTION_END 255

//#define USE_CLASS_B_NET 1
#define DHCPS_DEBUG 0
#define MAX_STATION_NUM 8

#define DHCPS_STATE_OFFER 1
#define DHCPS_STATE_DECLINE 2
#define DHCPS_STATE_ACK 3
#define DHCPS_STATE_NAK 4
#define DHCPS_STATE_IDLE 5
#define DHCPS_STATE_RELEASE 6

#ifdef MEMLEAK_DEBUG
const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

#if DHCPS_DEBUG
#define LWIP_IS_OK(what, err)                                                                      \
    ({                                                                                             \
        int ret = 1, errval = (err);                                                               \
        if (errval != ERR_OK)                                                                      \
        {                                                                                          \
            os_printf("DHCPS ERROR: %s (lwip:%s(%d))\n", what, lwip_strerr(errval), errval);       \
            ret = 0;                                                                               \
        }                                                                                          \
        ret;                                                                                       \
    })
#else
#define LWIP_IS_OK(what, err) ((err) == ERR_OK)
#endif

////////////////////////////////////////////////////////////////////////////////////

DhcpServer::OptionsBuffer& DhcpServer::OptionsBuffer::add(uint8_t code, const uint8_t* data,
                                                          size_t size)
{
    if (size >= UINT8_MAX)
    {
        return *this;
    }

    if ((size_t)(_end - _it) < (size + 2))
    {
        return *this;
    }

    *_it++ = code;
    *_it++ = size;

    memcpy_P(_it, data, size);
    _it += size;

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////

DhcpServer::DhcpServer(netif* netif) : _netif(netif) { }

// wifi_softap_set_station_info is missing in user_interface.h:
extern "C" void wifi_softap_set_station_info(uint8_t* mac, struct ipv4_addr*);

/******************************************************************************
    FunctionName : node_insert_to_list
    Description  : insert the node to the list
    Parameters   : arg -- Additional argument to pass to the callback function
    Returns      : none
*******************************************************************************/
void DhcpServer::node_insert_to_list(list_node** phead, list_node* pinsert)
{
    list_node*         plist       = nullptr;
    struct dhcps_pool* pdhcps_pool = nullptr;
    struct dhcps_pool* pdhcps_node = nullptr;
    if (*phead == nullptr)
    {
        *phead = pinsert;
    }
    else
    {
        plist       = *phead;
        pdhcps_node = (struct dhcps_pool*)pinsert->pnode;
        pdhcps_pool = (struct dhcps_pool*)plist->pnode;

        if (pdhcps_node->ip.addr < pdhcps_pool->ip.addr)
        {
            pinsert->pnext = plist;
            *phead         = pinsert;
        }
        else
        {
            while (plist->pnext != nullptr)
            {
                pdhcps_pool = (struct dhcps_pool*)plist->pnext->pnode;
                if (pdhcps_node->ip.addr < pdhcps_pool->ip.addr)
                {
                    pinsert->pnext = plist->pnext;
                    plist->pnext   = pinsert;
                    break;
                }
                plist = plist->pnext;
            }

            if (plist->pnext == nullptr)
            {
                plist->pnext = pinsert;
            }
        }
    }
    //  pinsert->pnext = nullptr;
}

/******************************************************************************
    FunctionName : node_delete_from_list
    Description  : remove the node from list
    Parameters   : arg -- Additional argument to pass to the callback function
    Returns      : none
*******************************************************************************/
void DhcpServer::node_remove_from_list(list_node** phead, list_node* pdelete)
{
    list_node* plist = nullptr;

    plist = *phead;
    if (plist == nullptr)
    {
        *phead = nullptr;
    }
    else
    {
        if (plist == pdelete)
        {
            *phead         = plist->pnext;
            pdelete->pnext = nullptr;
        }
        else
        {
            while (plist != nullptr)
            {
                if (plist->pnext == pdelete)
                {
                    plist->pnext   = pdelete->pnext;
                    pdelete->pnext = nullptr;
                }
                plist = plist->pnext;
            }
        }
    }
}

/******************************************************************************
    FunctionName : add_dhcps_lease
    Description  : add static lease on the list, this will be the next available @
    Parameters   : mac address
    Returns      : true if ok and false if this mac already exist or if all ip are already reserved
*******************************************************************************/
bool DhcpServer::add_dhcps_lease(uint8* macaddr)
{
    struct dhcps_pool* pdhcps_pool = nullptr;
    list_node*         pback_node  = nullptr;

    uint32 start_ip = lease.start_ip.addr;
    uint32 end_ip   = lease.end_ip.addr;

    for (pback_node = plist; pback_node != nullptr; pback_node = pback_node->pnext)
    {
        pdhcps_pool = (dhcps_pool*)pback_node->pnode;
        if (memcmp(pdhcps_pool->mac, macaddr, sizeof(pdhcps_pool->mac)) == 0)
        {
#if DHCPS_DEBUG
            os_printf("this mac already exist");
#endif
            return false;
        }
        else
        {
            start_ip = htonl((ntohl(start_ip) + 1));
        }
    }

    if (start_ip > end_ip)
    {
#if DHCPS_DEBUG
        os_printf("no more ip available");
#endif
        return false;
    }

    pdhcps_pool          = (struct dhcps_pool*)zalloc(sizeof(struct dhcps_pool));
    pdhcps_pool->ip.addr = start_ip;
    memcpy(pdhcps_pool->mac, macaddr, sizeof(pdhcps_pool->mac));
    pdhcps_pool->lease_timer = this->lease_time;
    pdhcps_pool->type        = DHCPS_TYPE_STATIC;
    pdhcps_pool->state       = DHCPS_STATE_ONLINE;
    pback_node               = (list_node*)zalloc(sizeof(list_node));
    pback_node->pnode        = pdhcps_pool;
    pback_node->pnext        = nullptr;
    node_insert_to_list(&plist, pback_node);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////
/*
    Set DHCP msg offer options for the given server

    @param buffer -- DHCP options buffer
    @param server -- DHCP server instance
*/
///////////////////////////////////////////////////////////////////////////////////
void DhcpServer::add_offer_options(OptionsBuffer& options)
{
    options.add(DHCP_OPTION_SUBNET_MASK, ip_2_ip4(&_netif->netmask))
        .add(DHCP_OPTION_SERVER_ID, ip_2_ip4(&_netif->ip_addr));

    // option units are seconds, while server opt is minutes
    const uint32_t lease_time_seconds = lease_time * 60;
    options.add(DHCP_OPTION_LEASE_TIME, lease_time_seconds);

    if (offer_router && !ip4_addr_isany_val(*ip_2_ip4(&_netif->gw)))
    {
        options.add(DHCP_OPTION_ROUTER, ip_2_ip4(&_netif->gw));
    }

#ifdef USE_DNS
    options.add(DHCP_OPTION_DNS_SERVER,
                !ip4_addr_isany_val(dns_address) ? &dns_address : ip_2_ip4(&_netif->ip_addr));
#endif

    {
        const auto* addr      = ip_2_ip4(&_netif->ip_addr);
        const auto* mask      = ip_2_ip4(&_netif->netmask);
        const auto  broadcast = ip4_addr_t { .addr = (addr->addr & mask->addr) | ~mask->addr };

        options.add(DHCP_OPTION_BROADCAST_ADDRESS, &broadcast);
    }

    // TODO: _netif->mtu ?
    static constexpr uint16_t Mtu { 1500 };
    options.add(DHCP_OPTION_INTERFACE_MTU, Mtu);

    static constexpr uint8_t RouterDiscovery { 0 };
    options.add(DHCP_OPTION_PERFORM_ROUTER_DISCOVERY, RouterDiscovery);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
DhcpServer::OptionsBuffer DhcpServer::create_msg(struct dhcps_msg* m)
{
    struct ipv4_addr client;

    client.addr = client_address.addr;

    m->op    = DHCP_REPLY;
    m->htype = DHCP_HTYPE_ETHERNET;
    m->hlen  = 6;
    m->hops  = 0;
    m->secs  = 0;
    m->flags = htons(BOOTP_BROADCAST);

    memcpy((char*)m->yiaddr, (char*)&client.addr, sizeof(m->yiaddr));
    memset((char*)m->ciaddr, 0, sizeof(m->ciaddr));
    memset((char*)m->siaddr, 0, sizeof(m->siaddr));
    memset((char*)m->giaddr, 0, sizeof(m->giaddr));
    memset((char*)m->sname, 0, sizeof(m->sname));
    memset((char*)m->file, 0, sizeof(m->file));
    memset((char*)m->options, 0, sizeof(m->options));
    memcpy((char*)m->options, &MagicCookie, sizeof(MagicCookie));

    return { &m->options[sizeof(magic_cookie)], std::end(m->options) };
}
///////////////////////////////////////////////////////////////////////////////////
/*
    OFFER

    @param -- m DHCP msg
*/
///////////////////////////////////////////////////////////////////////////////////
void DhcpServer::send_offer(struct dhcps_msg* m)
{
    struct pbuf *p, *q;

    auto options = create_msg(m);
    options.add(DHCP_OPTION_MSG_TYPE, DHCPOFFER);

    add_offer_options(options);
    if (custom_offer_options)
    {
        custom_offer_options(*this, options);
    }

    options.add(DHCP_OPTION_END);

    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcps_msg), PBUF_RAM);
#if DHCPS_DEBUG
    os_printf("udhcp: send_offer>>p->ref = %d\n", p->ref);
#endif
    if (p != nullptr)
    {
#if DHCPS_DEBUG
        os_printf("dhcps: send_offer>>pbuf_alloc succeed\n");
        os_printf("dhcps: send_offer>>p->tot_len = %d\n", p->tot_len);
        os_printf("dhcps: send_offer>>p->len = %d\n", p->len);
#endif
        q = p;
        while (q != nullptr)
        {
            std::memcpy((u8_t*)q->payload, reinterpret_cast<u8_t*>(&m), q->len);
            q = q->next;
        }
    }
    else
    {
#if DHCPS_DEBUG
        os_printf("dhcps: send_offer>>pbuf_alloc failed\n");
#endif
        return;
    }
    if (!LWIP_IS_OK("send_offer", udp_sendto(pcb_dhcps, p, &broadcast_dhcps, DHCPS_CLIENT_PORT)))
    {
#if DHCPS_DEBUG
        os_printf("dhcps: send_offer>>udp_sendto\n");
#endif
    }
    if (p->ref != 0)
    {
#if DHCPS_DEBUG
        os_printf("udhcp: send_offer>>free pbuf\n");
#endif
        pbuf_free(p);
    }
}
///////////////////////////////////////////////////////////////////////////////////
/*
    NAK

    @param m DHCP msg
*/
///////////////////////////////////////////////////////////////////////////////////
void DhcpServer::send_nak(struct dhcps_msg* m)
{
    struct pbuf *p, *q;

    auto options = create_msg(m);
    options.add(DHCP_OPTION_MSG_TYPE, DHCPNAK);
    options.add(DHCP_OPTION_END);

    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcps_msg), PBUF_RAM);
#if DHCPS_DEBUG
    os_printf("udhcp: send_nak>>p->ref = %d\n", p->ref);
#endif
    if (p != nullptr)
    {
#if DHCPS_DEBUG
        os_printf("dhcps: send_nak>>pbuf_alloc succeed\n");
        os_printf("dhcps: send_nak>>p->tot_len = %d\n", p->tot_len);
        os_printf("dhcps: send_nak>>p->len = %d\n", p->len);
#endif
        q = p;
        while (q != nullptr)
        {
            std::memcpy((u8_t*)q->payload, (u8_t*)m, q->len);
            q = q->next;
        }
    }
    else
    {
#if DHCPS_DEBUG
        os_printf("dhcps: send_nak>>pbuf_alloc failed\n");
#endif
        return;
    }
    LWIP_IS_OK("dhcps send nak", udp_sendto(pcb_dhcps, p, &broadcast_dhcps, DHCPS_CLIENT_PORT));
    if (p->ref != 0)
    {
#if DHCPS_DEBUG
        os_printf("udhcp: send_nak>>free pbuf\n");
#endif
        pbuf_free(p);
    }
}
///////////////////////////////////////////////////////////////////////////////////
/*
    ACK DHCP

    @param m DHCP msg
*/
///////////////////////////////////////////////////////////////////////////////////
void DhcpServer::send_ack(struct dhcps_msg* m)
{
    struct pbuf *p, *q;

    auto options = create_msg(m);
    options.add(DHCP_OPTION_MSG_TYPE, DHCPACK);

    add_offer_options(options);
    if (custom_offer_options)
    {
        custom_offer_options(*this, options);
    }

    options.add(DHCP_OPTION_END);

    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcps_msg), PBUF_RAM);
#if DHCPS_DEBUG
    os_printf("udhcp: send_ack>>p->ref = %d\n", p->ref);
#endif
    if (p != nullptr)
    {
#if DHCPS_DEBUG
        os_printf("dhcps: send_ack>>pbuf_alloc succeed\n");
        os_printf("dhcps: send_ack>>p->tot_len = %d\n", p->tot_len);
        os_printf("dhcps: send_ack>>p->len = %d\n", p->len);
#endif
        q = p;
        while (q != nullptr)
        {
            std::memcpy((u8_t*)q->payload, (u8_t*)m, q->len);
            q = q->next;
        }
    }
    else
    {
#if DHCPS_DEBUG
        os_printf("dhcps: send_ack>>pbuf_alloc failed\n");
#endif
        return;
    }
    if (!LWIP_IS_OK("dhcps send ack",
                    udp_sendto(pcb_dhcps, p, &broadcast_dhcps, DHCPS_CLIENT_PORT)))
    {
#if DHCPS_DEBUG
        os_printf("dhcps: send_ack>>udp_sendto\n");
#endif
    }

    if (p->ref != 0)
    {
#if DHCPS_DEBUG
        os_printf("udhcp: send_ack>>free pbuf\n");
#endif
        pbuf_free(p);
    }
}
///////////////////////////////////////////////////////////////////////////////////
/*
    DHCP

    @param optptr DHCP msg е
    @param len

    @return uint8_t* DHCP Server
*/
///////////////////////////////////////////////////////////////////////////////////
uint8_t DhcpServer::parse_options(uint8_t* optptr, sint16_t len)
{
    struct ipv4_addr   client;
    bool               is_dhcp_parse_end = false;
    struct dhcps_state s;

    client.addr = client_address.addr;

    u8_t* end  = optptr + len;
    u16_t type = 0;

    s.state = DHCPS_STATE_IDLE;

    while (optptr < end)
    {
#if DHCPS_DEBUG
        os_printf("dhcps: (sint16_t)*optptr = %d\n", (sint16_t)*optptr);
#endif
        switch ((sint16_t)*optptr)
        {
        case DHCP_OPTION_MSG_TYPE:  // 53
            type = *(optptr + 2);
            break;

        case DHCP_OPTION_REQ_IPADDR:  // 50
            // os_printf("dhcps:0x%08x,0x%08x\n",client.addr,*(uint32*)(optptr+2));
            if (memcmp((char*)&client.addr, (char*)optptr + 2, 4) == 0)
            {
#if DHCPS_DEBUG
                os_printf("dhcps: DHCP_OPTION_REQ_IPADDR = 0 ok\n");
#endif
                s.state = DHCPS_STATE_ACK;
            }
            else
            {
#if DHCPS_DEBUG
                os_printf("dhcps: DHCP_OPTION_REQ_IPADDR != 0 err\n");
#endif
                s.state = DHCPS_STATE_NAK;
            }
            break;
        case DHCP_OPTION_END:
        {
            is_dhcp_parse_end = true;
        }
        break;
        }

        if (is_dhcp_parse_end)
        {
            break;
        }

        optptr += optptr[1] + 2;
    }

    switch (type)
    {
    case DHCPDISCOVER:  // 1
        s.state = DHCPS_STATE_OFFER;
#if DHCPS_DEBUG
        os_printf("dhcps: DHCPD_STATE_OFFER\n");
#endif
        break;

    case DHCPREQUEST:  // 3
        if (!(s.state == DHCPS_STATE_ACK || s.state == DHCPS_STATE_NAK))
        {
            if (renew == true)
            {
                s.state = DHCPS_STATE_ACK;
            }
            else
            {
                s.state = DHCPS_STATE_NAK;
            }
#if DHCPS_DEBUG
            os_printf("dhcps: DHCPD_STATE_NAK\n");
#endif
        }
        break;

    case DHCPDECLINE:  // 4
        s.state = DHCPS_STATE_IDLE;
#if DHCPS_DEBUG
        os_printf("dhcps: DHCPD_STATE_IDLE\n");
#endif
        break;

    case DHCPRELEASE:  // 7
        s.state = DHCPS_STATE_RELEASE;
#if DHCPS_DEBUG
        os_printf("dhcps: DHCPD_STATE_IDLE\n");
#endif
        break;
    }
#if DHCPS_DEBUG
    os_printf("dhcps: return s.state = %d\n", s.state);
#endif
    return s.state;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
sint16_t DhcpServer::parse_msg(struct dhcps_msg* m, u16_t len)
{
    if (memcmp((char*)m->options, &MagicCookie, sizeof(MagicCookie)) == 0)
    {
        struct ipv4_addr ip;
        memcpy(&ip.addr, m->ciaddr, sizeof(ip.addr));
        client_address.addr = dhcps_client_update(m->chaddr, &ip);

        sint16_t ret = parse_options(&m->options[4], len);

        if (ret == DHCPS_STATE_RELEASE)
        {
            dhcps_client_leave(m->chaddr, &ip, true);  // force to delete
            client_address.addr = ip.addr;
        }

        return ret;
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////
/*
    DHCP
    udp_recv() callback

    @param arg
    @param pcb
    @param p
    @param addr
    @param port
*/
///////////////////////////////////////////////////////////////////////////////////

void DhcpServer::S_handle_dhcp(void* arg, struct udp_pcb* pcb, struct pbuf* p,
                               const ip_addr_t* addr, uint16_t port)
{
    DhcpServer* instance = reinterpret_cast<DhcpServer*>(arg);
    instance->handle_dhcp(pcb, p, addr, port);
}

void DhcpServer::handle_dhcp(struct udp_pcb* pcb, struct pbuf* p, const ip_addr_t* addr,
                             uint16_t port)
{
    (void)pcb;
    (void)addr;
    (void)port;

    struct dhcps_msg* pmsg_dhcps    = nullptr;
    sint16_t          tlen          = 0;
    u16_t             i             = 0;
    u16_t             dhcps_msg_cnt = 0;
    u8_t*             p_dhcps_msg   = nullptr;
    u8_t*             data          = nullptr;

#if DHCPS_DEBUG
    os_printf("dhcps: handle_dhcp-> receive a packet\n");
#endif
    if (p == nullptr)
    {
        return;
    }

    pmsg_dhcps = (struct dhcps_msg*)zalloc(sizeof(struct dhcps_msg));
    if (nullptr == pmsg_dhcps)
    {
        pbuf_free(p);
        return;
    }
    p_dhcps_msg = (u8_t*)pmsg_dhcps;
    tlen        = p->tot_len;
    data        = (u8_t*)p->payload;

#if DHCPS_DEBUG
    os_printf("dhcps: handle_dhcp-> p->tot_len = %d\n", tlen);
    os_printf("dhcps: handle_dhcp-> p->len = %d\n", p->len);
#endif

    for (i = 0; i < p->len; i++)
    {
        p_dhcps_msg[dhcps_msg_cnt++] = data[i];
    }

    if (p->next != nullptr)
    {
#if DHCPS_DEBUG
        os_printf("dhcps: handle_dhcp-> p->next != nullptr\n");
        os_printf("dhcps: handle_dhcp-> p->next->tot_len = %d\n", p->next->tot_len);
        os_printf("dhcps: handle_dhcp-> p->next->len = %d\n", p->next->len);
#endif

        data = (u8_t*)p->next->payload;
        for (i = 0; i < p->next->len; i++)
        {
            p_dhcps_msg[dhcps_msg_cnt++] = data[i];
        }
    }

#if DHCPS_DEBUG
    os_printf("dhcps: handle_dhcp-> parse_msg(p)\n");
#endif

    switch (parse_msg(pmsg_dhcps, tlen - 240))
    {
    case DHCPS_STATE_OFFER:  // 1
#if DHCPS_DEBUG
        os_printf("dhcps: handle_dhcp-> DHCPD_STATE_OFFER\n");
#endif
        send_offer(pmsg_dhcps);
        break;
    case DHCPS_STATE_ACK:  // 3
#if DHCPS_DEBUG
        os_printf("dhcps: handle_dhcp-> DHCPD_STATE_ACK\n");
#endif
        send_ack(pmsg_dhcps);
        if (_netif->num == SOFTAP_IF)
        {
            wifi_softap_set_station_info(pmsg_dhcps->chaddr, &client_address);
        }
        break;
    case DHCPS_STATE_NAK:  // 4
#if DHCPS_DEBUG
        os_printf("dhcps: handle_dhcp-> DHCPD_STATE_NAK\n");
#endif
        send_nak(pmsg_dhcps);
        break;
    default:
        break;
    }
#if DHCPS_DEBUG
    os_printf("dhcps: handle_dhcp-> pbuf_free(p)\n");
#endif
    pbuf_free(p);
    free(pmsg_dhcps);
    pmsg_dhcps = nullptr;
}
///////////////////////////////////////////////////////////////////////////////////
void DhcpServer::init_dhcps_lease(uint32 ip)
{
    uint32 softap_ip = 0, local_ip = 0;
    uint32 start_ip = 0;
    uint32 end_ip   = 0;
    if (lease.enable == true)
    {
        softap_ip = htonl(ip);
        start_ip  = htonl(lease.start_ip.addr);
        end_ip    = htonl(lease.end_ip.addr);
        /*config ip information can't contain local ip*/
        if ((start_ip <= softap_ip) && (softap_ip <= end_ip))
        {
            lease.enable = false;
        }
        else
        {
            /*config ip information must be in the same segment as the local ip*/
            softap_ip >>= 8;
            if (((start_ip >> 8 != softap_ip) || (end_ip >> 8 != softap_ip))
                || (end_ip - start_ip > DHCPS_MAX_LEASE))
            {
                lease.enable = false;
            }
        }
    }

    if (lease.enable == false)
    {
        local_ip = softap_ip = htonl(ip);
        softap_ip &= 0xFFFFFF00;
        local_ip &= 0xFF;
        if (local_ip >= 0x80)
        {
            local_ip -= DHCPS_MAX_LEASE;
        }
        else
        {
            local_ip++;
        }

        bzero(&lease, sizeof(lease));
        lease.start_ip.addr = softap_ip | local_ip;
        lease.end_ip.addr   = softap_ip | (local_ip + DHCPS_MAX_LEASE - 1);
        lease.start_ip.addr = htonl(lease.start_ip.addr);
        lease.end_ip.addr   = htonl(lease.end_ip.addr);
    }
    //  lease.start_ip.addr = htonl(lease.start_ip.addr);
    //  lease.end_ip.addr= htonl(lease.end_ip.addr);
    //  os_printf("start_ip = 0x%x, end_ip = 0x%x\n",lease.start_ip, lease.end_ip);
}
///////////////////////////////////////////////////////////////////////////////////

bool DhcpServer::begin()
{
    if (pcb_dhcps != nullptr)
    {
        udp_remove(pcb_dhcps);
    }

    pcb_dhcps = udp_new();
    if (pcb_dhcps == nullptr)
    {
#if DHCPS_DEBUG
        os_printf("dhcps_start(): could not obtain pcb\n");
#endif
        return false;
    }

    //  wrong: answer will go to sta  IP4_ADDR(&broadcast_dhcps, 255, 255, 255, 255);
    //  good: going to ap IP4_ADDR(&broadcast_dhcps, 192, 168, 4, 255);
    //  semi proper way:
    broadcast_dhcps = _netif->ip_addr;
    ip_2_ip4(&broadcast_dhcps)->addr &= ip_2_ip4(&_netif->netmask)->addr;
    ip_2_ip4(&broadcast_dhcps)->addr |= ~ip_2_ip4(&_netif->netmask)->addr;
    // XXXFIXMEIPV6 broadcast address?

    server_address = *ip_2_ip4(&_netif->ip_addr);
    init_dhcps_lease(server_address.addr);

    udp_bind(pcb_dhcps, IP_ADDR_ANY, DHCPS_SERVER_PORT);
    udp_recv(pcb_dhcps, S_handle_dhcp, this);
#if DHCPS_DEBUG
    os_printf("dhcps:dhcps_start->udp_recv function Set a receive callback handle_dhcp for UDP_PCB "
              "pcb_dhcps\n");
#endif

    return true;
}

DhcpServer::~DhcpServer()
{
    end();
}

void DhcpServer::end()
{
    if (!pcb_dhcps)
    {
        return;
    }

    udp_disconnect(pcb_dhcps);
    udp_remove(pcb_dhcps);
    pcb_dhcps = nullptr;

    // udp_remove(pcb_dhcps);
    list_node*         pnode      = nullptr;
    list_node*         pback_node = nullptr;
    struct dhcps_pool* dhcp_node  = nullptr;
    struct ipv4_addr   ip_zero;

    memset(&ip_zero, 0x0, sizeof(ip_zero));
    pnode = plist;
    while (pnode != nullptr)
    {
        pback_node = pnode;
        pnode      = pback_node->pnext;
        node_remove_from_list(&plist, pback_node);
        dhcp_node = (struct dhcps_pool*)pback_node->pnode;
        // dhcps_client_leave(dhcp_node->mac,&dhcp_node->ip,true); // force to delete
        if (_netif->num == SOFTAP_IF)
        {
            wifi_softap_set_station_info(dhcp_node->mac, &ip_zero);
        }
        free(pback_node->pnode);
        pback_node->pnode = nullptr;
        free(pback_node);
        pback_node = nullptr;
    }
}

bool DhcpServer::isRunning() const
{
    return pcb_dhcps != nullptr;
}

/******************************************************************************
    FunctionName : set_dhcps_lease
    Description  : set the lease information of DHCP server
    Parameters   : please -- Additional argument to set the lease information,
                            Little-Endian.
    Returns      : true or false
*******************************************************************************/
bool DhcpServer::set_dhcps_lease(struct dhcps_lease* please)
{
    uint32 softap_ip = 0;
    uint32 start_ip  = 0;
    uint32 end_ip    = 0;

    if (_netif->num == SOFTAP_IF || _netif->num == STATION_IF)
    {
        uint8 opmode = wifi_get_opmode();
        if (opmode == STATION_MODE || opmode == NULL_MODE)
        {
            return false;
        }
    }

    if (please == nullptr || isRunning())
    {
        return false;
    }

    if (please->enable)
    {
        // logic below is subject for improvement
        // - is wrong
        // - limited to /24 address plans
        softap_ip = htonl(ip_2_ip4(&_netif->ip_addr)->addr);
        start_ip  = htonl(please->start_ip.addr);
        end_ip    = htonl(please->end_ip.addr);
        /*config ip information can't contain local ip*/
        if ((start_ip <= softap_ip) && (softap_ip <= end_ip))
        {
            return false;
        }

        /*config ip information must be in the same segment as the local ip*/
        softap_ip >>= 8;
        if ((start_ip >> 8 != softap_ip) || (end_ip >> 8 != softap_ip))
        {
            return false;
        }

        if (end_ip - start_ip > DHCPS_MAX_LEASE)
        {
            return false;
        }

        bzero(&lease, sizeof(lease));
        //      lease.start_ip.addr = start_ip;
        //      lease.end_ip.addr = end_ip;
        lease.start_ip.addr = please->start_ip.addr;
        lease.end_ip.addr   = please->end_ip.addr;
    }
    lease.enable = please->enable;
    //  dhcps_lease_flag = false;
    return true;
}

/******************************************************************************
    FunctionName : get_dhcps_lease
    Description  : get the lease information of DHCP server
    Parameters   : please -- Additional argument to get the lease information,
                            Little-Endian.
    Returns      : true or false
*******************************************************************************/
bool DhcpServer::get_dhcps_lease(struct dhcps_lease* please)
{
    if (_netif->num == SOFTAP_IF)
    {
        uint8 opmode = wifi_get_opmode();
        if (opmode == STATION_MODE || opmode == NULL_MODE)
        {
            return false;
        }
    }

    if (nullptr == please)
    {
        return false;
    }

    //  if (dhcps_lease_flag){
    if (lease.enable == false)
    {
        if (isRunning())
        {
            return false;
        }
    }
    else
    {
        //      bzero(please, sizeof(*please));
        //      if (!isRunning()){
        //          please->start_ip.addr = htonl(lease.start_ip.addr);
        //          please->end_ip.addr = htonl(lease.end_ip.addr);
        //      }
    }

    //  if (isRunning()){
    //      bzero(please, sizeof(*please));
    //      please->start_ip.addr = lease.start_ip.addr;
    //      please->end_ip.addr = lease.end_ip.addr;
    //  }
    please->start_ip.addr = lease.start_ip.addr;
    please->end_ip.addr   = lease.end_ip.addr;
    return true;
}

void DhcpServer::kill_oldest_dhcps_pool(void)
{
    list_node *        pre = nullptr, *p = nullptr;
    list_node *        minpre = nullptr, *minp = nullptr;
    struct dhcps_pool *pdhcps_pool = nullptr, *pmin_pool = nullptr;
    pre    = plist;
    p      = pre->pnext;
    minpre = pre;
    minp   = p;
    while (p != nullptr)
    {
        pdhcps_pool = (struct dhcps_pool*)p->pnode;
        pmin_pool   = (struct dhcps_pool*)minp->pnode;
        if (pdhcps_pool->lease_timer < pmin_pool->lease_timer)
        {
            minp   = p;
            minpre = pre;
        }
        pre = p;
        p   = p->pnext;
    }
    minpre->pnext      = minp->pnext;
    pdhcps_pool->state = DHCPS_STATE_OFFLINE;
    free(minp->pnode);
    minp->pnode = nullptr;
    free(minp);
    minp = nullptr;
}

void DhcpServer::dhcps_coarse_tmr(void)
{
    uint8              num_dhcps_pool = 0;
    list_node*         pback_node     = nullptr;
    list_node*         pnode          = nullptr;
    struct dhcps_pool* pdhcps_pool    = nullptr;
    pnode                             = plist;
    while (pnode != nullptr)
    {
        pdhcps_pool = (struct dhcps_pool*)pnode->pnode;
        if (pdhcps_pool->type == DHCPS_TYPE_DYNAMIC)
        {
            pdhcps_pool->lease_timer--;
        }
        if (pdhcps_pool->lease_timer == 0)
        {
            pback_node = pnode;
            pnode      = pback_node->pnext;
            node_remove_from_list(&plist, pback_node);
            free(pback_node->pnode);
            pback_node->pnode = nullptr;
            free(pback_node);
            pback_node = nullptr;
        }
        else
        {
            pnode = pnode->pnext;
            num_dhcps_pool++;
        }
    }

    if (num_dhcps_pool >= MAX_STATION_NUM)
    {
        kill_oldest_dhcps_pool();
    }
}

void DhcpServer::dhcps_client_leave(u8* bssid, struct ipv4_addr* ip, bool force)
{
    struct dhcps_pool* pdhcps_pool = nullptr;
    list_node*         pback_node  = nullptr;

    if ((bssid == nullptr) || (ip == nullptr))
    {
        return;
    }

    for (pback_node = plist; pback_node != nullptr; pback_node = pback_node->pnext)
    {
        pdhcps_pool = (struct dhcps_pool*)pback_node->pnode;
        if (memcmp(pdhcps_pool->mac, bssid, sizeof(pdhcps_pool->mac)) == 0)
        {
            if (memcmp(&pdhcps_pool->ip.addr, &ip->addr, sizeof(pdhcps_pool->ip.addr)) == 0)
            {
                if ((pdhcps_pool->type == DHCPS_TYPE_STATIC) || (force))
                {
                    if (pback_node != nullptr)
                    {
                        node_remove_from_list(&plist, pback_node);
                        free(pback_node);
                        pback_node = nullptr;
                    }

                    if (pdhcps_pool != nullptr)
                    {
                        free(pdhcps_pool);
                        pdhcps_pool = nullptr;
                    }
                }
                else
                {
                    pdhcps_pool->state = DHCPS_STATE_OFFLINE;
                }

                struct ipv4_addr ip_zero;
                memset(&ip_zero, 0x0, sizeof(ip_zero));
                if (_netif->num == SOFTAP_IF)
                {
                    wifi_softap_set_station_info(bssid, &ip_zero);
                }
                break;
            }
        }
    }
}

uint32 DhcpServer::dhcps_client_update(u8* bssid, struct ipv4_addr* ip)
{
    struct dhcps_pool* pdhcps_pool = nullptr;
    list_node*         pback_node  = nullptr;
    list_node*         pmac_node   = nullptr;
    list_node*         pip_node    = nullptr;
    bool               flag        = false;
    uint32             start_ip    = lease.start_ip.addr;
    uint32             end_ip      = lease.end_ip.addr;
    dhcps_type_t       type        = DHCPS_TYPE_DYNAMIC;
    if (bssid == nullptr)
    {
        return IPADDR_ANY;
    }

    if (ip)
    {
        if (IPADDR_BROADCAST == ip->addr)
        {
            return IPADDR_ANY;
        }
        else if (IPADDR_ANY == ip->addr)
        {
            ip = nullptr;
        }
        else
        {
            type = DHCPS_TYPE_STATIC;
        }
    }

    renew = false;
    for (pback_node = plist; pback_node != nullptr; pback_node = pback_node->pnext)
    {
        pdhcps_pool = (struct dhcps_pool*)pback_node->pnode;
        // os_printf("mac:"MACSTR"bssid:"MACSTR"\r\n",MAC2STR(pdhcps_pool->mac),MAC2STR(bssid));
        if (memcmp(pdhcps_pool->mac, bssid, sizeof(pdhcps_pool->mac)) == 0)
        {
            pmac_node = pback_node;
            if (ip == nullptr)
            {
                flag = true;
                break;
            }
        }
        if (ip != nullptr)
        {
            if (memcmp(&pdhcps_pool->ip.addr, &ip->addr, sizeof(pdhcps_pool->ip.addr)) == 0)
            {
                pip_node = pback_node;
            }
        }
        else if (flag == false)
        {
            if (memcmp(&pdhcps_pool->ip.addr, &start_ip, sizeof(pdhcps_pool->ip.addr)) != 0)
            {
                flag = true;
            }
            else
            {
                start_ip = htonl((ntohl(start_ip) + 1));
            }
        }
    }

    if ((ip == nullptr) && (flag == false))
    {
        if (plist == nullptr)
        {
            if (start_ip <= end_ip)
            {
                flag = true;
            }
            else
            {
                return IPADDR_ANY;
            }
        }
        else
        {
            if (start_ip > end_ip)
            {
                return IPADDR_ANY;
            }
            // start_ip = htonl((ntohl(start_ip) + 1));
            flag = true;
        }
    }

    if (pmac_node != nullptr)  // update new ip
    {
        if (pip_node != nullptr)
        {
            pdhcps_pool = (struct dhcps_pool*)pip_node->pnode;

            if (pip_node != pmac_node)
            {
                if (pdhcps_pool->state != DHCPS_STATE_OFFLINE)  // ip is used
                {
                    return IPADDR_ANY;
                }

                // mac exists and ip exists in other node,delete mac
                node_remove_from_list(&plist, pmac_node);
                free(pmac_node->pnode);
                pmac_node->pnode = nullptr;
                free(pmac_node);
                pmac_node = pip_node;
                memcpy(pdhcps_pool->mac, bssid, sizeof(pdhcps_pool->mac));
            }
            else
            {
                renew = true;
                type  = DHCPS_TYPE_DYNAMIC;
            }

            pdhcps_pool->lease_timer = this->lease_time;
            pdhcps_pool->type        = type;
            pdhcps_pool->state       = DHCPS_STATE_ONLINE;
        }
        else
        {
            pdhcps_pool = (struct dhcps_pool*)pmac_node->pnode;
            if (ip != nullptr)
            {
                pdhcps_pool->ip.addr = ip->addr;
            }
            else if (flag == true)
            {
                pdhcps_pool->ip.addr = start_ip;
            }
            else  // no ip to distribute
            {
                return IPADDR_ANY;
            }

            node_remove_from_list(&plist, pmac_node);
            pdhcps_pool->lease_timer = this->lease_time;
            pdhcps_pool->type        = type;
            pdhcps_pool->state       = DHCPS_STATE_ONLINE;
            node_insert_to_list(&plist, pmac_node);
        }
    }
    else  // new station
    {
        if (pip_node != nullptr)  // maybe ip has used
        {
            pdhcps_pool = (struct dhcps_pool*)pip_node->pnode;
            if (pdhcps_pool->state != DHCPS_STATE_OFFLINE)
            {
                return IPADDR_ANY;
            }
            memcpy(pdhcps_pool->mac, bssid, sizeof(pdhcps_pool->mac));
            pdhcps_pool->lease_timer = this->lease_time;
            pdhcps_pool->type        = type;
            pdhcps_pool->state       = DHCPS_STATE_ONLINE;
        }
        else
        {
            pdhcps_pool = (struct dhcps_pool*)zalloc(sizeof(struct dhcps_pool));
            if (ip != nullptr)
            {
                pdhcps_pool->ip.addr = ip->addr;
            }
            else if (flag == true)
            {
                pdhcps_pool->ip.addr = start_ip;
            }
            else  // no ip to distribute
            {
                free(pdhcps_pool);
                return IPADDR_ANY;
            }
            if (pdhcps_pool->ip.addr > end_ip)
            {
                free(pdhcps_pool);
                return IPADDR_ANY;
            }
            memcpy(pdhcps_pool->mac, bssid, sizeof(pdhcps_pool->mac));
            pdhcps_pool->lease_timer = this->lease_time;
            pdhcps_pool->type        = type;
            pdhcps_pool->state       = DHCPS_STATE_ONLINE;
            pback_node               = (list_node*)zalloc(sizeof(list_node));
            pback_node->pnode        = pdhcps_pool;
            pback_node->pnext        = nullptr;
            node_insert_to_list(&plist, pback_node);
        }
    }

    return pdhcps_pool->ip.addr;
}
