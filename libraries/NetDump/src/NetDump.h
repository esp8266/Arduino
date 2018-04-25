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

#ifndef __NETDUMP_H
#define __NETDUMP_H

#include <Print.h>

#define ETH_HDR_LEN 14

// helpers that can be used for filtering

inline uint16_t ntoh16               (const char* data)    { return data[1] | (((uint16_t)data[0]) << 8); }
inline uint32_t ntoh32               (const char* data)    { return ntoh16(data + 2) | (((uint32_t)ntoh16(data)) << 16); }

inline uint16_t netDump_ethtype      (const char* ethdata) { return ntoh16(ethdata + 12); }
inline     bool netDump_is_ARP       (const char* ethdata) { return netDump_ethtype(ethdata) == 0x0806; }
inline     bool netDump_is_IPv4      (const char* ethdata) { return netDump_ethtype(ethdata) == 0x0800; }
inline     bool netDump_is_IPv6      (const char* ethdata) { return netDump_ethtype(ethdata) == 0x86dd; }
inline  uint8_t netDump_getIpType    (const char* ethdata) { return ethdata[ETH_HDR_LEN + 9]; }
inline     bool netDump_is_ICMP      (const char* ethdata) { return netDump_getIpType(ethdata) == 1; }
inline     bool netDump_is_IGMP      (const char* ethdata) { return netDump_getIpType(ethdata) == 2; }
inline     bool netDump_is_TCP       (const char* ethdata) { return netDump_getIpType(ethdata) == 6; }
inline     bool netDump_is_UDP       (const char* ethdata) { return netDump_getIpType(ethdata) == 17; }

inline  uint8_t netDump_getARPType   (const char* ethdata) { return ethdata[ETH_HDR_LEN + 7]; }
inline     bool netDump_is_ARP_who   (const char* ethdata) { return netDump_getARPType(ethdata) == 1; }
inline     bool netDump_is_ARP_is    (const char* ethdata) { return netDump_getARPType(ethdata) == 2; }

inline uint16_t netDump_getIpHdrLen  (const char* ethdata) { return (((unsigned char)ethdata[ETH_HDR_LEN]) & 0x0f) << 2; }
inline uint16_t netDump_getIpTotLen  (const char* ethdata) { return ntoh16(ethdata + ETH_HDR_LEN + 2); }
inline uint16_t netDump_getIpOptLen  (const char* ethdata) { return netDump_getIpHdrLen(ethdata) - 20; }
inline uint16_t netDump_getIpUsrLen  (const char* ethdata) { return netDump_getIpTotLen(ethdata) - netDump_getIpHdrLen(ethdata); }

inline uint16_t netDump_getSrcPort   (const char* ethdata) { return ntoh16(ethdata + ETH_HDR_LEN + netDump_getIpHdrLen(ethdata) + 0); }
inline uint16_t netDump_getDstPort   (const char* ethdata) { return ntoh16(ethdata + ETH_HDR_LEN + netDump_getIpHdrLen(ethdata) + 2); } 

inline uint16_t netDump_getUdpUsrLen (const char* ethdata) { return ntoh16(ethdata + ETH_HDR_LEN + netDump_getIpHdrLen(ethdata) + 4) - 8; }

inline uint32_t netDump_getTcpSeq    (const char* ethdata) { return ntoh32(ethdata + ETH_HDR_LEN + netDump_getIpHdrLen(ethdata) + 4); }
inline uint32_t netDump_getTcpAck    (const char* ethdata) { return ntoh32(ethdata + ETH_HDR_LEN + netDump_getIpHdrLen(ethdata) + 8); }
inline uint16_t netDump_getTcpFlags  (const char* ethdata) { return ntoh16(ethdata + ETH_HDR_LEN + netDump_getIpHdrLen(ethdata) + 12); } 
inline uint16_t netDump_getTcpHdrLen (const char* ethdata) { return (ntoh16(ethdata + ETH_HDR_LEN + netDump_getIpHdrLen(ethdata) + 12) >> 12) << 2; }
inline uint16_t netDump_getTcpWindow (const char* ethdata) { return ntoh16(ethdata + ETH_HDR_LEN + netDump_getIpHdrLen(ethdata) + 14); }
inline uint16_t netDump_getTcpUsrLen (const char* ethdata) { return netDump_getIpUsrLen(ethdata) - netDump_getTcpHdrLen(ethdata); }

void netDumpIPv4 (Print& out, const char* ethdata);
void netDumpMac  (Print& out, const char* mac);
void netDumpMacs (Print& out, const char* mac);

// main dump functions

void netDump    (Print& out, const char* ethdata, size_t size);
void netDumpHex (Print& out, const char* data, size_t size, bool show_hex = true, bool show_ascii = true, size_t per_line = 16);

// tcpdump server:
// call tcpdump_setup() in your setup()
// call tcpdump_loop() in your loop()
// open a terminal, run:
//     nc esp-ip-address 2 | tcpdump -r - [<options>] [<pcap-filter>]

bool tcpdump_setup (uint16_t port = 2, size_t snap = 96, bool fast = true);
void tcpdump_loop ();
extern size_t tcpdump_err;

#endif // __NETDUMP_H
