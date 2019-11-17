/*
    NetDump library - tcpdump-like packet logger facility

    Copyright (c) 2019 Herman Reintke. All rights reserved.
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

#ifndef __NETDUMP_PACKET_H
#define __NETDUMP_PACKET_H

#include <lwipopts.h>
#include <IPAddress.h>
#include <StreamString.h>
#include "NetdumpIP.h"

namespace NetCapture
{

int constexpr ETH_HDR_LEN = 14;

class Packet
{
public:
    Packet(unsigned long msec, int n, const char* d, size_t l, int o, int s)
        : packetTime(msec), netif_idx(n), data(d), len(l), out(o), success(s)
    {};

    ~Packet() {};

    enum class PacketDetail
    {
        NONE,
        FULL,
        CHARS
    };

    enum class PacketType
	{
    	ARP,
		IP,
		UDP,
		MDNS,
		DNS,
		SSDP,
		DHCP,
		WSDD,
		NETBIOS,
		SMB,
		OTA,
		TCP,
		HTTP,
		ICMP,
		IGMP,
		UKNW,
	};

    static constexpr char* packetTypeArray[] = {"ARP ","IP  ","UDP ","MDNS","DNS ",
                                                "SSDP","DHCP","WSDD","NBIO", "SMB ","OTA ","TCP ",
                                                "HTTP","ICMP","IGMP","UKNW"};

    const char* packetTypeString (PacketType pt) const
    {
    	return packetTypeArray[static_cast<int>(pt)];
    }
    PacketType packetType() const;

    const char* rawData() const
    {
    	return data;
    }
    int getInOut() const
    {
    	return out;
    }
    unsigned long getTime() const
    {
    	return packetTime;
    }
    uint32_t getPacketSize() const
    {
    	return len;
    }
    uint16_t ntoh16(uint16_t idx) const
    {
        return data[idx + 1] | (((uint16_t)data[idx]) << 8);
    };
    uint32_t ntoh32(uint16_t idx) const
    {
        return ntoh16(idx + 2) | (((uint32_t)ntoh16(idx)) << 16);
    };
    uint8_t  byteData(uint16_t idx) const
    {
        return data[idx];
    }
    const char* byteIdx(uint16_t idx) const
    {
        return &data[idx];
    };
    uint16_t ethType() const
    {
        return ntoh16(12);
    };
    uint8_t  ipType() const
    {
        return isIPv4() ? data[ETH_HDR_LEN + 9] : data[ETH_HDR_LEN + 6];
    };

    uint16_t getIpHdrLen() const
    {
        return isIPv4() ? (((unsigned char)data[ETH_HDR_LEN]) & 0x0f) << 2 : 40 ;   // IPv6 is fixed length
    }
    uint16_t getIpTotalLen() const
    {
        return ntoh16(ETH_HDR_LEN + 2);
    }
    //	uint16_t getIpOptLen()        { return getIpHdrLen() - 20; }
    //	uint16_t getIpUsrLen()        { return getIpTotLen() - getIpHdrLen(); }

    uint32_t getTcpSeq() const
    {
        return ntoh32(ETH_HDR_LEN + getIpHdrLen() + 4);
    }
    uint32_t getTcpAck() const
    {
        return ntoh32(ETH_HDR_LEN + getIpHdrLen() + 8);
    }
    uint16_t getTcpFlags() const
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 12);
    }
    uint16_t getTcpWindow() const
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 14);
    }
    uint8_t  getTcpHdrLen() const
    {
        return (data[ETH_HDR_LEN + getIpHdrLen() + 12] >> 4) * 4;
    };//Header len is in multiple of 4 bytes
    uint16_t getTcpLen() const
    {
        return getIpTotalLen() - getIpHdrLen() - getTcpHdrLen() ;
    };

    uint8_t  getIcmpType() const
    {
        return data[ETH_HDR_LEN + getIpHdrLen() + 0];
    }
    uint8_t  getIgmpType() const
    {
        return data[ETH_HDR_LEN + getIpHdrLen() + 0];
    }

    uint8_t getARPType() const
    {
        return data[ETH_HDR_LEN + 7];
    }
    bool    is_ARP_who() const
    {
        return getARPType() == 1;
    }
    bool    is_ARP_is() const
    {
        return getARPType() == 2;
    }

    uint8_t getUdpHdrLen() const
    {
        return 8;
    };
    uint16_t getUdpLen() const
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 4);
    };
    bool isARP() const
    {
        return (ethType() == 0x0806);
    };
    bool isIPv4() const
    {
        return (ethType() == 0x0800);
    };
    bool isIPv6() const
    {
        return (ethType() == 0x86dd);
    };
    bool isIP() const
    {
        return (isIPv4() || isIPv6());
    };
    bool isICMP() const
    {
        return ((ipType() == 1) || ipType() == 58);
    };
    bool isIGMP() const
    {
        return ipType() == 2;
    };
    bool isTCP() const
    {
        return ipType() == 6;
    };
    bool isUDP() const
    {
        return ipType() == 17;
    };
    bool isMDNS() const
    {
        return hasPort(5353);
    };
    bool isDNS() const
    {
        return hasPort(53);
    };
    bool isSSDP() const
    {
        return hasPort(1900);
    };
    bool isDHCP() const
    {
        return (hasPort(546) || hasPort(547) || hasPort(67) || hasPort(68));
    };
    bool isWSDD() const
    {
        return (hasPort(3702));
    };
    bool isHTTP() const
    {
        return (hasPort(80));
    };
    bool isOTA() const
    {
    	return (hasPort(8266));
    }
    bool isNETBIOS() const
    {
    	return (hasPort(137) || hasPort(138) || hasPort(139));
    }
    bool isSMB() const
    {
    	return (hasPort(445));
    }
    NetdumpIP getIP(uint16_t idx) const
    {
        return NetdumpIP(data[idx],
                         data[idx + 1],
                         data[idx + 2],
                         data[idx + 3]);
    };

    NetdumpIP getIP6(uint16_t idx) const
    {
        return NetdumpIP((const uint8_t*)&data[idx], false);
    };


    NetdumpIP sourceIP() const
    {
        NetdumpIP ip;
        if (isIPv4())
        {
            ip = getIP(ETH_HDR_LEN + 12);
        }
        else if (isIPv6())
        {
            ip = getIP6(ETH_HDR_LEN + 8);
        }
        return ip;
    };

    bool      hasIP(NetdumpIP ip) const
    {
        return ((ip == sourceIP()) || (ip == destIP()));
    }

    NetdumpIP destIP() const
    {
        NetdumpIP ip;
        if (isIPv4())
        {
            ip = getIP(ETH_HDR_LEN + 16);
        }
        else if (isIPv6())
        {
            ip = getIP6(ETH_HDR_LEN + 24);
        }
        return ip;
    };
    uint16_t getSrcPort() const
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 0);
    }
    uint16_t getDstPort() const
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 2);
    }
    bool     hasPort(uint16_t p) const
    {
        return ((getSrcPort() == p) || (getDstPort() == p));
    }

    String toString(PacketDetail netdumpDetail = PacketDetail::NONE) const;
    void printDetail(Print& out, const String& indent, const char* data, size_t size, PacketDetail pd) const;

private:
    unsigned long packetTime;
    int netif_idx;
    const char* data;
    size_t len;
    int out;
    int success;
};

} // namespace NetCapture

#endif /* __NETDUMP_PACKET_H */
