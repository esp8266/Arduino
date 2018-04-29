/*
 NetDump library - tcpdump-like packet logger facility

 Copyright (c) 2018 David Gauchard. All rights reserved.
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
*/

#include <NetDump.h>

#if 0
#include <lwip/netif.h>
#include <lwip/etharp.h>

// WIP - attempt to show correct mac addresses for output packets
void netDumpMacsForOutput (Print& out, const char* ethdata, char netif_idx)
{
    uint32_t ip = 0;
    if (netDump_is_IPv4(ethdata))
        memcpy(&ip, ethdata + ETH_HDR_LEN + 16, 4);
    else if (netDump_is_ARP(ethdata))
    {
        if (netDump_is_ARP_who(ethdata))
        {
            netDumpMac(out, ethdata + 6);
            out.print(F(">ff:ff:ff:ff:ff:ff"));
            return;
        }
        else if (netDump_is_ARP_is(ethdata))
            memcpy(&ip, ethdata + ETH_HDR_LEN + 14, 4);
        else
        {
            out.print(F("arpmac?"));
            return;
        }
    }
    else
        out.print(F("mac/proto?"));
    
    ip4_addr_t ip4dst;
    struct eth_addr* eth_ret;
    struct netif* netif_ret;
    ip4_addr_t* ip_ret;

    ip4_addr_set_u32(&ip4dst, ip);

    for (int i = 0; etharp_get_entry(i, &ip_ret, &netif_ret, &eth_ret); i++)
        if (ip_ret->addr == ip4dst.addr)
        {
            netDumpMac(out, (const char*)netif_ret->hwaddr);
            out.print('>');
            netDumpMac(out, (const char*)eth_ret->addr);
            return;
        }

    out.print(F("mac?"));
}
#endif

void netDumpMacs (Print& out, const char* ethdata)
{
    netDumpMac(out, ethdata + 6);
    out.print('>');
    netDumpMac(out, ethdata);
}
