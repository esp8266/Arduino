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

#ifndef __DHCPS_H__
#define __DHCPS_H__

#include <lwip/init.h>

#include <cstdint>
#include <cstddef>
#include <cstring>

#include <array>
#include <initializer_list>

class DhcpServer
{
public:
    struct OptionsBuffer
    {
        OptionsBuffer(uint8_t* begin, uint8_t* end) : _it(begin), _begin(begin), _end(end) { }

        OptionsBuffer& add(uint8_t code, const uint8_t* data, size_t size);

        OptionsBuffer& add(uint8_t code, const char* data, size_t size)
        {
            return add(code, reinterpret_cast<const uint8_t*>(data), size);
        }

        template<size_t Size>
        OptionsBuffer& add(uint8_t code, const char (&data)[Size])
        {
            return add(code, &data[0], Size - 1);
        }

        template<size_t Size>
        OptionsBuffer& add(uint8_t code, const uint8_t (&data)[Size])
        {
            return add(code, &data[0], Size);
        }

        OptionsBuffer& add(uint8_t code, std::initializer_list<uint8_t> data)
        {
            return add(code, data.begin(), data.size());
        }

        OptionsBuffer& add(uint8_t code, const ip4_addr_t* addr)
        {
            return add(code,
                       { ip4_addr1(addr), ip4_addr2(addr), ip4_addr3(addr), ip4_addr4(addr) });
        }

        OptionsBuffer& add(uint8_t code, uint8_t value)
        {
            return add(code, { value });
        }

        OptionsBuffer& add(uint8_t code, uint16_t value)
        {
            return add(code, { static_cast<uint8_t>((value >> 8) & 0xff),
                               static_cast<uint8_t>(value & 0xff) });
        }

        OptionsBuffer& add(uint8_t code, uint32_t value)
        {
            return add(code, { static_cast<uint8_t>((value >> 24) & 0xff),
                               static_cast<uint8_t>((value >> 16) & 0xff),
                               static_cast<uint8_t>((value >> 8) & 0xff),
                               static_cast<uint8_t>((value & 0xff)) });
        }

        OptionsBuffer& add(uint8_t code)
        {
            if (_it != _end)
            {
                *_it++ = code;
            }
            return *this;
        }

    private:
        uint8_t* _it;
        uint8_t* _begin;
        uint8_t* _end;
    };

    using OptionsBufferHandler = void (*)(const DhcpServer&, OptionsBuffer&);

    DhcpServer(netif* netif);
    ~DhcpServer();

    netif* getNetif() const
    {
        return _netif;
    }

    void setRouter(bool value)
    {
        offer_router = value;
    }

    bool getRouter() const
    {
        return offer_router;
    }

    void setDns(ip_addr_t addr)
    {
        dns_address = addr;
    }

    ip_addr_t getDns() const
    {
        return dns_address;
    }

    void resetLeaseTime();

    void setLeaseTime(uint32_t minutes)
    {
        lease_time = minutes;
    }

    uint32_t getLeaseTime() const
    {
        return lease_time;
    }

    // Will use provided callback for ACK and OFFER replies
    // `options.add(...)` to append to the options list
    // (does not check for duplicates!)
    void onSendOptions(OptionsBufferHandler handler) {
        custom_offer_options = handler;
    }

    bool begin(ip_info* info);
    void end();
    bool isRunning();

    // this is the C interface encapsulated in a class
    // (originally dhcpserver.c in lwIP-v1.4 in NonOS-SDK)
    // (not changing everything at once)
    // the API below is subject to change

    // legacy public C structure and API to eventually turn into C++

    void init_dhcps_lease(uint32 ip);
    bool set_dhcps_lease(struct dhcps_lease* please);
    bool get_dhcps_lease(struct dhcps_lease* please);
    bool add_dhcps_lease(uint8* macaddr);

    void offers();

protected:
    void add_offer_options(OptionsBuffer&);

    // legacy C structure and API to eventually turn into C++

    typedef struct _list_node
    {
        void*              pnode;
        struct _list_node* pnext;
    } list_node;

    void node_insert_to_list(list_node** phead, list_node* pinsert);
    void node_remove_from_list(list_node** phead, list_node* pdelete);

    OptionsBuffer create_msg(struct dhcps_msg* m);

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

    netif* _netif;

    struct udp_pcb* pcb_dhcps = nullptr;
    ip_addr_t       broadcast_dhcps {};
    ip4_addr_t      server_address {};
    ip4_addr_t      client_address {};

    static constexpr uint32_t DefaultLeaseTime { 720 };
    uint32_t                  lease_time = DefaultLeaseTime;

    bool      offer_router = true;
    ip_addr_t dns_address {};

    dhcps_lease lease {};

    list_node* plist = nullptr;
    bool       renew = false;

    OptionsBufferHandler custom_offer_options = nullptr;

    static const uint32 magic_cookie;
};

// SoftAP DHCP server always exists and is started on boot
extern DhcpServer dhcpSoftAP;
extern "C" int    fw_has_started_softap_dhcps;

#endif  // __DHCPS_H__
