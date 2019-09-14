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

void NetdumpPacket::printDetail(Print& out, String indent, const char* data, size_t size, PacketDetail pd)
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


String NetdumpPacket::toString(PacketDetail netdumpDetail)
{
    StreamString sstr;
    sstr.reserve(128);

    sstr.printf("%d %3s ", netif_idx, out ? "out" : "in ");

    if (isARP())
    {
        sstr.printf("ARP  ");
        switch (getARPType())
        {
        case 1 : sstr.printf("who has %s tell %s", getIP(ETH_HDR_LEN + 24).toString().c_str(), getIP(ETH_HDR_LEN + 14).toString().c_str());
            break;
        case 2 : sstr.printf("%s is at ", getIP(ETH_HDR_LEN + 14).toString().c_str());
            for (int i = 0; i < 6; i++)
            {
                sstr.printf("%02x", (unsigned char)data[ETH_HDR_LEN + 8 + i]);
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

    if (isIP())
    {
        if (isUDP())
        {
            if (isMDNS() || isDNS())
            {
                sstr.printf("%s%s>%s ", isMDNS() ? "MDNS " : "DNS  ", sourceIP().toString().c_str(), destIP().toString().c_str());
                sstr.printf("ID=0x%04x ", ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8));
                sstr.printf("F=0x%04x ", ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 2));
                if (uint16_t t = ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 4))
                {
                    sstr.printf("Q=%d ", t);
                }
                if (uint16_t t = ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 6))
                {
                    sstr.printf("R=%d ", t);
                }
                if (uint16_t t = ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 8))
                {
                    sstr.printf("TR=%d ", t);
                }
                if (uint16_t t = ntoh16(ETH_HDR_LEN + getIpHdrLen() + 8 + 10))
                {
                    sstr.printf("DR=%d ", t);
                }
                sstr.printf("\r\n");
            }
            else
            {
                String udpType;

                if (isSSDP())
                {
                    udpType = "SSDP";
                }
                else if (isDHCP())
                {
                    udpType = "DHCP";
                }
                else if (isWSDD())
                {
                    udpType = "WSDD";
                }
                else
                {
                    udpType = "UDP ";
                }
                sstr.printf("%s %s>%s ", udpType.c_str(), sourceIP().toString().c_str(), destIP().toString().c_str());
                sstr.printf("%d:%d", getSrcPort(), getDstPort());
                sstr.printf("\r\n");
            }
            printDetail(sstr, "           H ", &data[ETH_HDR_LEN + getIpHdrLen()], getUdpHdrLen(), netdumpDetail);
            printDetail(sstr, "           D ", &data[ETH_HDR_LEN + getIpHdrLen() + getUdpHdrLen()], getUdpLen(), netdumpDetail);
            return sstr;
        }
        else if (isTCP())
        {
            sstr.printf(isHTTP() ? "HTTP " : "TCP  ");
            sstr.printf("%s>%s ", sourceIP().toString().c_str(), destIP().toString().c_str());
            sstr.printf("%d:%d ", getSrcPort(), getDstPort());
            uint16_t flags = getTcpFlags();
            sstr.print('[');
            const char chars [] = "FSRPAUECN";
            for (uint8_t i = 0; i < sizeof chars; i++)
                if (flags & (1 << i))
                {
                    sstr.print(chars[i]);
                }
            sstr.print(']');
            sstr.printf(" len: %u seq: %u, ack: %u, wnd: %u ", getTcpLen(), getTcpSeq(), getTcpAck(), getTcpWindow());
            sstr.printf("\r\n");
            printDetail(sstr, "           H ", &data[ETH_HDR_LEN + getIpHdrLen()], getTcpHdrLen(), netdumpDetail);
            printDetail(sstr, "           D ", &data[ETH_HDR_LEN + getIpHdrLen() + getTcpHdrLen()], getTcpLen(), netdumpDetail);
            return sstr;
        }
        else if (isICMP())
        {
            sstr.printf("ICMP ");
            sstr.printf("%s>%s ", sourceIP().toString().c_str(), destIP().toString().c_str());
            if (isIPv4())
            {
                switch (getIcmpType())
                {
                case 0 : sstr.printf("ping reply"); break;
                case 8 : sstr.printf("ping request"); break;
                default: sstr.printf("type(0x%02x)", getIcmpType()); break;
                }
            }
            if (isIPv6())
            {
                switch (getIcmpType())
                {
                case 129 : sstr.printf("ping reply"); break;
                case 128 : sstr.printf("ping request"); break;
                case 135 : sstr.printf("Neighbour solicitation"); break;
                case 136 : sstr.printf("Neighbour advertisement"); break;
                default: sstr.printf("type(0x%02x)", getIcmpType()); break;
                }
            }
            sstr.printf("\r\n");
            return sstr;
        }
        else if (isIGMP())
        {
            sstr.printf("IGMP ");
            switch (getIgmpType())
            {
            case 1 : sstr.printf("Create Group Request"); break;
            case 2 : sstr.printf("Create Group Reply"); break;
            case 3 : sstr.printf("Join Group Request"); break;
            case 4 : sstr.printf("Join Group Reply"); break;
            case 5 : sstr.printf("Leave Group Request"); break;
            case 6 : sstr.printf("Leave Group Reply"); break;
            case 7 : sstr.printf("Confirm Group Request"); break;
            case 8 : sstr.printf("Confirm Group Reply"); break;
            case 0x11 : sstr.printf("Group Membership Query"); break;
            case 0x12 : sstr.printf("IGMPv1 Membership Report"); break;
            case 0x22 : sstr.printf("IGMPv3 Membership Report"); break;
            default: sstr.printf("type(0x%02x)", getIgmpType()); break;
            }
            sstr.printf("\r\n");
            return sstr;
        }
        else
        {
            sstr.printf("UKWN type = %d\r\n", ipType());
            return sstr;
        }
    }

    sstr.printf("Unknown packet, type = 0x%04x\r\n", ethType());
    return sstr;

}

