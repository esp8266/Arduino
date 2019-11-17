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

#include "Netdump.h"
#include <lwip/init.h>

namespace NetCapture
{

constexpr char* Packet::packetTypeArray[];

void Packet::printDetail(Print& out, const String& indent, const char* data, size_t size, PacketDetail pd) const
{
    if (pd == PacketDetail::NONE)
    {
        return;
    }

    uint16_t charCount = pd == PacketDetail::FULL ? 24 : 80;

    size_t start = 0;
    while (start < size)
    {
        size_t end = start + charCount;
        if (end > size)
        {
            end = size;
        }
        out.printf("%s", indent.c_str());
        if (pd == PacketDetail::FULL)
        {
            for (size_t i = start; i < end; i++)
            {
                out.printf("%02x ", (unsigned char)data[i]);
            }
            for (size_t i = end; i < start + charCount; i++)
            {
                out.print("   ");
            }
        }
        for (size_t i = start; i < end; i++)
        {
            out.printf("%c", data[i] >= 32 && data[i] < 128 ? data[i] : '.');
        }
        out.println();

        start += charCount;
    }
}

Packet::PacketType Packet::packetType() const
{
	if (isARP()) return PacketType::ARP;
	if (isIP())
	{
		if (isUDP())
		{
			if (isMDNS()) return PacketType::MDNS;
			if (isDNS())  return PacketType::DNS;
			if (isSSDP()) return PacketType::SSDP;
			if (isDHCP()) return PacketType::DHCP;
			if (isWSDD()) return PacketType::WSDD;
			if (isNETBIOS()) return PacketType::NETBIOS;
			if (isSMB())  return PacketType::SMB;
			if (isOTA())  return PacketType::OTA;
			return PacketType::UDP;
		}
		if (isTCP())
		{
			if (isHTTP()) return PacketType::HTTP;
			return PacketType::TCP;
		}
		if (isICMP()) return PacketType::ICMP;
		if (isIGMP()) return PacketType::IGMP;
		return PacketType::IP;
	}
	return PacketType::UKNW;
}

String Packet::toString(PacketDetail netdumpDetail) const
{
    StreamString sstr;
    sstr.reserve(128);

    sstr.printf_P(PSTR("%d %3s %s "), netif_idx, out ? "out" : "in ", packetTypeString(packetType()));

    switch (packetType())
    {
    case PacketType::ARP :
    {
        switch (getARPType())
        {
        case 1 : sstr.printf_P(PSTR("who has %s tell %s"), getIP(ETH_HDR_LEN + 24).toString().c_str(), getIP(ETH_HDR_LEN + 14).toString().c_str());
            break;
        case 2 : sstr.printf_P(PSTR("%s is at "), getIP(ETH_HDR_LEN + 14).toString().c_str());
            for (int i = 0; i < 6; i++)
            {
                sstr.printf_P(PSTR("%02x"), (unsigned char)data[ETH_HDR_LEN + 8 + i]);
                if (i < 5)
                {
                    sstr.print(':');
                }
            }
            break;
        }
        sstr.printf("\r\n");
        return sstr;
    }
    case PacketType::MDNS :
    case PacketType::DNS :
    {
        sstr.printf_P(PSTR("%s>%s "), sourceIP().toString().c_str(), destIP().toString().c_str());
        sstr.printf_P(PSTR("ID=0x%04x "), ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8));
        sstr.printf_P(PSTR("F=0x%04x "), ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 2));
        if (uint16_t t = ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 4))
        {
            sstr.printf_P(PSTR("Q=%d "), t);
        }
        if (uint16_t t = ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 6))
        {
            sstr.printf_P(PSTR("R=%d "), t);
        }
        if (uint16_t t = ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 8))
        {
            sstr.printf_P(PSTR("TR=%d "), t);
        }
        if (uint16_t t = ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 10))
        {
            sstr.printf_P(PSTR("DR=%d "), t);
        }
        sstr.printf_P(PSTR("\r\n"));
        printDetail(sstr, PSTR("           H "), &data[ETH_HDR_LEN + getIpHdrLen()], getUdpHdrLen(), netdumpDetail);
        printDetail(sstr, PSTR("           D "), &data[ETH_HDR_LEN + getIpHdrLen() + getUdpHdrLen()], getUdpLen(), netdumpDetail);
        return sstr;
    }
    case PacketType::SSDP :
    case PacketType::DHCP :
    case PacketType::WSDD :
    case PacketType::NETBIOS :
    case PacketType::SMB :
    case PacketType::OTA :
    case PacketType::UDP :
    {
        sstr.printf_P(PSTR("%s>%s "), sourceIP().toString().c_str(), destIP().toString().c_str());
        sstr.printf_P(PSTR("%d:%d"), getSrcPort(), getDstPort());
        sstr.printf_P(PSTR("\r\n"));
        printDetail(sstr, PSTR("           H "), &data[ETH_HDR_LEN + getIpHdrLen()], getUdpHdrLen(), netdumpDetail);
        printDetail(sstr, PSTR("           D "), &data[ETH_HDR_LEN + getIpHdrLen() + getUdpHdrLen()], getUdpLen(), netdumpDetail);
        return sstr;
    }
    case PacketType::TCP :
    case PacketType::HTTP :
    {
        sstr.printf_P(PSTR("%s>%s "), sourceIP().toString().c_str(), destIP().toString().c_str());
        sstr.printf_P(PSTR("%d:%d "), getSrcPort(), getDstPort());
        uint16_t flags = getTcpFlags();
        sstr.print('[');
        const char chars [] = "FSRPAUECN";
        for (uint8_t i = 0; i < sizeof chars; i++)
            if (flags & (1 << i))
            {
                sstr.print(chars[i]);
            }
        sstr.print(']');
        sstr.printf_P(PSTR(" len: %u seq: %u, ack: %u, wnd: %u "), getTcpLen(), getTcpSeq(), getTcpAck(), getTcpWindow());
        sstr.printf_P(PSTR("\r\n"));
        printDetail(sstr, PSTR("           H "), &data[ETH_HDR_LEN + getIpHdrLen()], getTcpHdrLen(), netdumpDetail);
        printDetail(sstr, PSTR("           D "), &data[ETH_HDR_LEN + getIpHdrLen() + getTcpHdrLen()], getTcpLen(), netdumpDetail);
        return sstr;
    }
    case PacketType::ICMP :
    {
        sstr.printf_P(PSTR("%s>%s "), sourceIP().toString().c_str(), destIP().toString().c_str());
        if (isIPv4())
        {
            switch (getIcmpType())
            {
            case 0 : sstr.printf_P(PSTR("ping reply")); break;
            case 8 : sstr.printf_P(PSTR("ping request")); break;
            default: sstr.printf_P(PSTR("type(0x%02x)"), getIcmpType()); break;
            }
        }
        if (isIPv6())
        {
            switch (getIcmpType())
            {
            case 129 : sstr.printf_P(PSTR("ping reply")); break;
            case 128 : sstr.printf_P(PSTR("ping request")); break;
            case 135 : sstr.printf_P(PSTR("Neighbour solicitation")); break;
            case 136 : sstr.printf_P(PSTR("Neighbour advertisement")); break;
            default: sstr.printf_P(PSTR("type(0x%02x)"), getIcmpType()); break;
            }
        }
        sstr.printf("\r\n");
        return sstr;
    }
    case PacketType::IGMP :
    {
    	switch (getIgmpType())
        {
        case 1 : sstr.printf_P(PSTR("Create Group Request")); break;
        case 2 : sstr.printf_P(PSTR("Create Group Reply")); break;
        case 3 : sstr.printf_P(PSTR("Join Group Request")); break;
        case 4 : sstr.printf_P(PSTR("Join Group Reply")); break;
        case 5 : sstr.printf_P(PSTR("Leave Group Request")); break;
        case 6 : sstr.printf_P(PSTR("Leave Group Reply")); break;
        case 7 : sstr.printf_P(PSTR("Confirm Group Request")); break;
        case 8 : sstr.printf_P(PSTR("Confirm Group Reply")); break;
        case 0x11 : sstr.printf_P(PSTR("Group Membership Query")); break;
        case 0x12 : sstr.printf_P(PSTR("IGMPv1 Membership Report")); break;
        case 0x22 : sstr.printf_P(PSTR("IGMPv3 Membership Report")); break;
        default: sstr.printf_P(PSTR("type(0x%02x)"), getIgmpType()); break;
        }
        sstr.printf_P(PSTR("\r\n"));
        return sstr;
    }
    case PacketType::IP :
    {
        sstr.printf_P(PSTR("Unknown IP type : %d\r\n"), ipType());
        return sstr;
    }
    case PacketType::UKNW :
    {
        sstr.printf_P(("Unknown packet, type = 0x%04x\r\n"), ethType());
        return sstr;
    }
    default :
    {
    	sstr.printf_P(PSTR("Non identified packet\r\n"));
    	return sstr;
    }
    }

}

} // namespace NetCapture
