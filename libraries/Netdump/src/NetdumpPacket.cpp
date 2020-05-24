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

void Packet::printDetail(Print& out, const String& indent, const uint8_t* data, size_t size, PacketDetail pd) const
{
    if (pd == PacketDetail::NONE)
    {
        return;
    }

    uint16_t charCount = (pd == PacketDetail::CHAR) ? 80 : 24;

    size_t start = 0;
    while (start < size)
    {
        size_t end = start + charCount;
        if (end > size)
        {
            end = size;
        }
        out.printf("%s", indent.c_str());
        if (pd != PacketDetail::CHAR)
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

void Packet::setPacketType(PacketType pt)
{
    thisPacketType = pt;
    thisAllPacketTypes.emplace_back(pt);
}

void Packet::setPacketTypes()
{
    if (isARP())
    {
        setPacketType(PacketType::ARP);
    }
    else if (isIP())
    {
        setPacketType(PacketType::IP);
        setPacketType(isIPv4() ? PacketType::IPv4 : PacketType::IPv6);
        if (isUDP())
        {
            setPacketType(PacketType::UDP);
            if (isMDNS())
            {
                setPacketType(PacketType::MDNS);
            }
            if (isDNS())
            {
                setPacketType(PacketType::DNS);
            }
            if (isSSDP())
            {
                setPacketType(PacketType::SSDP);
            }
            if (isDHCP())
            {
                setPacketType(PacketType::DHCP);
            }
            if (isWSDD())
            {
                setPacketType(PacketType::WSDD);
            }
            if (isNETBIOS())
            {
                setPacketType(PacketType::NETBIOS);
            }
            if (isSMB())
            {
                setPacketType(PacketType::SMB);
            }
            if (isOTA())
            {
                setPacketType(PacketType::OTA);
            }
        }
        if (isTCP())
        {
            setPacketType(PacketType::TCP);
            if (isHTTP())
            {
                setPacketType(PacketType::HTTP);
            }
        }
        if (isICMP())
        {
            setPacketType(PacketType::ICMP);
        }
        if (isIGMP())
        {
            setPacketType(PacketType::IGMP);
        }
    }
    else
    {
        setPacketType(PacketType::UKNW);
    }
}

const PacketType Packet::packetType() const
{
    return thisPacketType;
}

const std::vector<PacketType> Packet::allPacketTypes() const
{
    return thisAllPacketTypes;
}

void Packet::MACtoString(const uint8_t* mac, StreamString& sstr) const
{
    for (int i = 0; i < 6; i++)
    {
        sstr.printf_P(PSTR("%02x"), mac[i]);
        if (i < 5)
        {
            sstr.print(':');
        }
    }
}

void Packet::ARPtoString(PacketDetail netdumpDetail, StreamString& sstr) const
{
	if (!arpPacket)
	{
		sstr.printf_P(PSTR("ARPtoString access error\r\n"));
		return;
	}
    switch (arpPacket->opcode())
    {
    case 1 : sstr.printf_P(PSTR("who has %s tell %s"), arpPacket->targetIP().toString().c_str(), arpPacket->senderIP().toString().c_str());
        break;
    case 2 : sstr.printf_P(PSTR("%s is at "), arpPacket->senderIP().toString().c_str());
        MACtoString(arpPacket->hdr->senderHardwareAddress, sstr);
        break;
    }
    sstr.printf("\r\n");
    printDetail(sstr, PSTR("           D "), arpPacket->raw, packetLength - ETH_HDR_LEN, netdumpDetail);
}

void Packet::DNStoString(PacketDetail netdumpDetail, StreamString& sstr) const
{
	if (!dnsPacket || !udpPacket || !ipPacket)
	{
		sstr.printf_P(PSTR("DNStoString access error\r\n"));
		return;
	}
	sstr.printf_P(PSTR("%s>%s "),  ipPacket->sourceIP().toString().c_str(), ipPacket->destinationIP().toString().c_str());
    sstr.printf_P(PSTR("ID=0x%04x "), dnsPacket->id());
    sstr.printf_P(PSTR("F=0x%04x "), dnsPacket->flags());

    if (uint16_t t = dnsPacket->qdcount())
    {
        sstr.printf_P(PSTR("Q=%d "), t);
    }
    if (uint16_t t = dnsPacket->ancount())
    {
        sstr.printf_P(PSTR("R=%d "), t);
    }
    if (uint16_t t = dnsPacket->nscount())
    {
        sstr.printf_P(PSTR("TR=%d "), t);
    }
    if (uint16_t t = dnsPacket->arcount())
    {
        sstr.printf_P(PSTR("DR=%d "), t);
    }

    sstr.printf_P(PSTR("\r\n"));
    printDetail(sstr, PSTR("           H "), udpPacket->raw, udpPacket->hdrLength(), netdumpDetail);
    printDetail(sstr, PSTR("           D "), udpPacket->hdr->payload, udpPacket->length(), netdumpDetail);
}

void Packet::UDPtoString(PacketDetail netdumpDetail, StreamString& sstr) const
{
    sstr.printf_P(PSTR("%s>%s "), sourceIP().toString().c_str(), destIP().toString().c_str());
    sstr.printf_P(PSTR("%d:%d"), getSrcPort(), getDstPort());
    sstr.printf_P(PSTR("\r\n"));
    printDetail(sstr, PSTR("           H "), udpPacket->raw, getUdpHdrLen(), netdumpDetail);
    printDetail(sstr, PSTR("           D "), udpPacket->hdr->payload, getUdpLen(), netdumpDetail);
}

void Packet::TCPtoString(PacketDetail netdumpDetail, StreamString& sstr) const
{
	if (!tcpPacket)
	{
		sstr.printf_P(PSTR("TCPtoString access error\r\n"));
		return;
	}
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
    printDetail(sstr, PSTR("           H "), tcpPacket->raw, getTcpHdrLen(), netdumpDetail);
    printDetail(sstr, PSTR("           D "), tcpPacket->hdr->payload, getTcpLen(), netdumpDetail);
}

void Packet::ICMPtoString(PacketDetail netdumpDetail, StreamString& sstr) const
{
	if (!icmpPacket || !ipPacket)
	{
		sstr.printf_P(PSTR("ICMPtoString access error\r\n"));
		return;
	}
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
}

void Packet::IGMPtoString(PacketDetail netdumpDetail, StreamString& sstr) const
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
}

void Packet::IPtoString(PacketDetail netdumpDetail, StreamString& sstr) const
{
    sstr.printf_P(PSTR("%s>%s "), sourceIP().toString().c_str(), destIP().toString().c_str());
    sstr.printf_P(PSTR("Unknown IP type : %d\r\n"), ipType());
    printDetail(sstr, PSTR("           H "), ipPacket->raw(), getIpHdrLen(), netdumpDetail);
    printDetail(sstr, PSTR("           D "), ipPacket->raw(), getIpTotalLen() - getIpHdrLen(), netdumpDetail);
}

void Packet::UKNWtoString(PacketDetail netdumpDetail, StreamString& sstr) const
{
    sstr.printf_P(PSTR("Unknown EtherType 0x%04x Src : "), ethType());
    MACtoString(rawData(), sstr);
    sstr.printf_P(PSTR(" Dst : "));
    MACtoString(rawData()+6, sstr);
    sstr.printf_P(PSTR("\r\n"));
}

const String Packet::toString() const
{
    return toString(PacketDetail::NONE);
}


const String Packet::toString(PacketDetail netdumpDetail) const
{
    StreamString sstr;
    sstr.reserve(128);

    sstr.printf_P(PSTR("%d %3s %-4s "), netif_idx, out ? "out" : "in ", packetType().toString().c_str());

    if (netdumpDetail == PacketDetail::RAW)
    {
        sstr.printf_P(PSTR(" : "));
        for (auto at : thisAllPacketTypes)
        {
            sstr.printf_P(PSTR("%s "), at.toString().c_str());
        }
        sstr.printf_P(PSTR("\r\n"));
        printDetail(sstr, PSTR("           D "), rawData(), packetLength, netdumpDetail);
        return sstr;
    }

    switch (thisPacketType)
    {
    case PacketType::ARP :
    {
        ARPtoString(netdumpDetail, sstr);
        break;
    }
    case PacketType::MDNS :
    case PacketType::DNS :
    {
        DNStoString(netdumpDetail, sstr);
        break;
    }
    case PacketType::SSDP :
    case PacketType::DHCP :
    case PacketType::WSDD :
    case PacketType::NETBIOS :
    case PacketType::SMB :
    case PacketType::OTA :
    case PacketType::UDP :
    {
        UDPtoString(netdumpDetail, sstr);
        break;
    }
    case PacketType::TCP :
    case PacketType::HTTP :
    {
        TCPtoString(netdumpDetail, sstr);
        break;
    }
    case PacketType::ICMP :
    {
        ICMPtoString(netdumpDetail, sstr);
        break;
    }
    case PacketType::IGMP :
    {
        IGMPtoString(netdumpDetail, sstr);
        break;
    }
    case PacketType::IPv4 :
    case PacketType::IPv6 :
    {
        IPtoString(netdumpDetail, sstr);
        break;
    }
    case PacketType::UKNW :
    {
        UKNWtoString(netdumpDetail, sstr);
        break;
    }
    default :
    {
        sstr.printf_P(PSTR("Non identified packet\r\n"));
        break;
    }
    }
    return sstr;
}

} // namespace NetCapture
