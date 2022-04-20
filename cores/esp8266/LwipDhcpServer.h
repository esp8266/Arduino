/*
    lwIPDhcpServer.h - DHCP server

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

#pragma once

#include <lwip/init.h>  // LWIP_VERSION

class DhcpServer
{
public:
    static constexpr int     DefaultLeaseTime = 120 /* seconds */;
    static constexpr ip_info DefaultIpConfig  = {
         .ip { 0x0104a8c0 },       // 192.168.4.1
         .netmask { 0x00ffffff },  // 255.255.255.0
         .gw { 0 }                 // 0.0.0.0
    };

    static const uint32 magic_cookie;

    DhcpServer(netif*);
    ~DhcpServer();

    void setDns(int num, const ipv4_addr_t* dns);

    bool begin();
    void end();
    bool isRunning() const;

    // this is the C interface encapsulated in a class
    // (originally dhcpserver.c in lwIP-v1.4 in NonOS-SDK)
    // (not changing everything at once)
    // the API below is subject to change

    // legacy public C structure and API to eventually turn into C++

    void   init_dhcps_lease(uint32 ip);
    bool   set_dhcps_lease(struct dhcps_lease* please);
    bool   get_dhcps_lease(struct dhcps_lease* please);
    bool   set_dhcps_offer_option(uint8 level, void* optarg);
    bool   set_dhcps_lease_time(uint32 minute);
    bool   reset_dhcps_lease_time(void);
    uint32 get_dhcps_lease_time(void);
    bool   add_dhcps_lease(uint8* macaddr);

    void dhcps_set_dns(int num, const ipv4_addr_t* dns);

protected:
    // legacy C structure and API to eventually turn into C++

    typedef struct _list_node
    {
        void*              pnode;
        struct _list_node* pnext;
    } list_node;

    void        node_insert_to_list(list_node** phead, list_node* pinsert);
    void        node_remove_from_list(list_node** phead, list_node* pdelete);
    uint8_t*    add_msg_type(uint8_t* optptr, uint8_t type);
    uint8_t*    add_offer_options(uint8_t* optptr);
    uint8_t*    add_end(uint8_t* optptr);
    void        create_msg(struct dhcps_msg* m);
    void        send_offer(struct dhcps_msg* m);
    void        send_nak(struct dhcps_msg* m);
    void        send_ack(struct dhcps_msg* m);
    uint8_t     parse_options(uint8_t* optptr, sint16_t len);
    sint16_t    parse_msg(struct dhcps_msg* m, u16_t len);
    static void S_handle_dhcp(void* arg, struct udp_pcb* pcb, struct pbuf* p, const ip_addr_t* addr,
                              uint16_t port);
    void   handle_dhcp(struct udp_pcb* pcb, struct pbuf* p, const ip_addr_t* addr, uint16_t port);
    void   kill_oldest_dhcps_pool(void);
    void   dhcps_coarse_tmr(void);  // CURRENTLY NOT CALLED
    void   dhcps_client_leave(u8* bssid, struct ipv4_addr* ip, bool force);
    uint32 dhcps_client_update(u8* bssid, struct ipv4_addr* ip);

    netif* _netif = nullptr;

    udp_pcb*  pcb_dhcps = nullptr;
    ip_addr_t broadcast_dhcps {};
    ipv4_addr server_address {};
    ipv4_addr client_address {};
    ipv4_addr dns_address {};
    uint32    dhcps_lease_time = DefaultLeaseTime;

    struct dhcps_lease dhcps_lease;
    list_node*         plist;
    uint8              offer;
    bool               renew;
};
