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

static void snap (Print& out)
{
    out.println(F("(snap)"));
}

void netDumpMac (Print& out, const char* mac)
{
    for (int i = 0; i < 6; i++)
    {
        out.printf("%02x", (unsigned char)mac[i]);
        if (i < 5)
            out.print(':');
    }
}

void netDumpIPv4 (Print& out, const char* ethdata)
{
    for (int i = 0; i < 4; i++)
    {
        out.printf("%u", (unsigned char)ethdata[i]);
        if (i < 3)
            out.print('.');
    }
}

static void netDumpARP (Print& out, const char* ethdata, size_t size)
{
    out.print(F(" ARP "));
    if (size < ETH_HDR_LEN + 28)
        return;
    char type = netDump_getARPType(ethdata);
    if (type == 1)
    {
        out.print(F("who has "));
        netDumpIPv4(out, ethdata + ETH_HDR_LEN + 24);
        out.print(F(" tell "));
        netDumpIPv4(out, ethdata + ETH_HDR_LEN + 14);
    }
    else if (type == 2)
    {
        netDumpIPv4(out, ethdata + ETH_HDR_LEN + 14);
        out.print(F(" is at "));
        netDumpMac(out, ethdata + ETH_HDR_LEN + 8);
    }
    else
        out.printf("(type=%d)", type);
    out.println();
}

static void netDumpICMP (Print& out, const char* ethdata, size_t size)
{
    out.print(F(" ICMP "));
    if (size < 1)
        return snap(out);
        
    switch (ethdata[ETH_HDR_LEN + 20 + 0])
    {
    case 0: out.println(F("ping reply")); break;
    case 8: out.println(F("ping request")); break;
    default: out.printf("type(0x%02x)\r\n", ethdata[ETH_HDR_LEN + 20 + 0]);
    }
}

static void netDumpIGMP (Print& out, const char* ethdata, size_t size)
{
    out.println(F(" IGMP"));
    if (size < 1)
        return snap(out);
    (void)ethdata;
}

static void netDumpPort (Print& out, const char* ethdata)
{
    out.printf("%d>%d", netDump_getSrcPort(ethdata), netDump_getDstPort (ethdata));
}

void netDumpTCPFlags (Print& out, const char* ethdata)
{
    uint16_t flags = netDump_getTcpFlags(ethdata);
    out.print('[');
    const char chars [] = "FSRP.UECN";
    for (uint8_t i = 0; i < sizeof chars; i++)
        if (flags & (1 << i))
            out.print(chars[i]);
    out.print(']');
}

static void netDumpTCP (Print& out, const char* ethdata, size_t size)
{
    out.print(F(" TCP "));
    if (size < ETH_HDR_LEN + 20 + 16)
        return snap(out);
    netDumpPort(out, ethdata);
    netDumpTCPFlags(out, ethdata);

    uint16_t tcplen = netDump_getIpUsrLen(ethdata) - netDump_getTcpHdrLen(ethdata);
    uint32_t seq = netDump_getTcpSeq(ethdata);
    out.printf(" seq:%u", seq);
    if (tcplen)
        out.printf("..%u", seq + tcplen);
    out.printf(" ack:%u win:%d",
        (unsigned)netDump_getTcpAck(ethdata),
        netDump_getTcpWindow(ethdata));
    if (tcplen)
        out.printf(" len=%u", tcplen);
    
    size_t options = ETH_HDR_LEN + netDump_getIpHdrLen(ethdata) + 20;
    size_t options_len = netDump_getTcpHdrLen(ethdata) - 20;
    for (size_t i = 0; i < options_len; )
    {
        uint8_t opt = ethdata[options + i];
        uint8_t sz = opt >= 2? ethdata[options + i + 1]: 1;
        switch (opt)
        {
        case 0:
        case 1: break;
        case 2: out.printf(" mss=%u", ntoh16(ethdata + options + i + 2)); break;
        default: out.printf(" opt%u(%u)", opt, sz);
        }
        if (!opt)
            // end of option
            break;
        i += sz;
    }

    out.println();
}

static void netDumpUDP (Print& out, const char* ethdata, size_t size)
{
    out.print(F(" UDP "));
    if (size < ETH_HDR_LEN + 20 + 8)
        return snap(out);

    netDumpPort(out, ethdata);
    uint16_t udplen = netDump_getUdpUsrLen(ethdata);
    uint16_t iplen = netDump_getIpUsrLen(ethdata) - 8/*udp hdr size*/;
    if (udplen != iplen)
        out.printf(" len=%d?", iplen);
    out.printf(" len=%d\r\n", udplen);
}

static void netDumpIPv4 (Print& out, const char* ethdata, size_t size)
{
    if (size < ETH_HDR_LEN + 20)
        return snap(out);
        
    out.print(F(" IPv4 "));
    
    netDumpIPv4(out, ethdata + ETH_HDR_LEN + 12);
    out.print('>');
    netDumpIPv4(out, ethdata + ETH_HDR_LEN + 16);
    //out.printf(" (iphdrlen=%d)", netDump_getIpHdrLen(ethdata));

    if      (netDump_is_ICMP(ethdata)) netDumpICMP(out, ethdata, size);
    else if (netDump_is_IGMP(ethdata)) netDumpIGMP(out, ethdata, size);
    else if (netDump_is_TCP(ethdata))  netDumpTCP (out, ethdata, size);
    else if (netDump_is_UDP(ethdata))  netDumpUDP (out, ethdata, size);
    else out.printf(" ip proto 0x%02x\r\n", netDump_getIpType(ethdata));
}

void netDump (Print& out, const char* ethdata, size_t size)
{
    if (size < ETH_HDR_LEN)
        return snap(out);

    if      (netDump_is_ARP(ethdata))  netDumpARP (out, ethdata, size);
    else if (netDump_is_IPv4(ethdata)) netDumpIPv4(out, ethdata, size);
    else if (netDump_is_IPv6(ethdata)) out.println(F(" IPV6"));
    else out.printf(" eth proto 0x%04x\r\n", netDump_ethtype(ethdata));
}
