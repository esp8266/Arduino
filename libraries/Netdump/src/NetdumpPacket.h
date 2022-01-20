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
#include "PacketType.h"
#include <vector>

namespace NetCapture
{
int constexpr ETH_HDR_LEN = 14;

class Packet
{
public:
    Packet(unsigned long msec, int n, const char* d, size_t l, int o, int s) :
        packetTime(msec), netif_idx(n), data(d), packetLength(l), out(o), success(s)
    {
        setPacketTypes();
    };

    enum class PacketDetail
    {
        NONE,
        FULL,
        CHAR,
        RAW
    };

    const char* rawData() const { return data; }
    int         getInOut() const { return out; }
    time_t      getTime() const { return packetTime; }
    uint32_t    getPacketSize() const { return packetLength; }
    uint16_t    ntoh16(uint16_t idx) const { return data[idx + 1] | (((uint16_t)data[idx]) << 8); };
    uint32_t    ntoh32(uint16_t idx) const
    {
        return ntoh16(idx + 2) | (((uint32_t)ntoh16(idx)) << 16);
    };
    uint8_t     byteData(uint16_t idx) const { return data[idx]; }
    const char* byteIdx(uint16_t idx) const { return &data[idx]; };
    uint16_t    ethType() const { return ntoh16(12); };
    uint8_t     ipType() const
    {
        return isIP() ? isIPv4() ? data[ETH_HDR_LEN + 9] : data[ETH_HDR_LEN + 6] : 0;
    };
    uint16_t getIpHdrLen() const
    {
        return isIPv4() ? (((unsigned char)data[ETH_HDR_LEN]) & 0x0f) << 2 :
                          40;  // IPv6 is fixed length
    }
    uint16_t getIpTotalLen() const
    {
        return isIP() ? isIPv4() ? ntoh16(ETH_HDR_LEN + 2) : (packetLength - ETH_HDR_LEN) : 0;
    }
    uint32_t getTcpSeq() const { return isTCP() ? ntoh32(ETH_HDR_LEN + getIpHdrLen() + 4) : 0; }
    uint32_t getTcpAck() const { return isTCP() ? ntoh32(ETH_HDR_LEN + getIpHdrLen() + 8) : 0; }
    uint16_t getTcpFlags() const { return isTCP() ? ntoh16(ETH_HDR_LEN + getIpHdrLen() + 12) : 0; }
    uint16_t getTcpWindow() const { return isTCP() ? ntoh16(ETH_HDR_LEN + getIpHdrLen() + 14) : 0; }
    uint8_t  getTcpHdrLen() const
    {
        return isTCP() ? (data[ETH_HDR_LEN + getIpHdrLen() + 12] >> 4) * 4 : 0;
    };  // Header len is in multiple of 4 bytes
    uint16_t getTcpLen() const
    {
        return isTCP() ? getIpTotalLen() - getIpHdrLen() - getTcpHdrLen() : 0;
    };

    uint8_t getIcmpType() const { return isICMP() ? data[ETH_HDR_LEN + getIpHdrLen() + 0] : 0; }
    uint8_t getIgmpType() const { return isIGMP() ? data[ETH_HDR_LEN + getIpHdrLen() + 0] : 0; }
    uint8_t getARPType() const { return isARP() ? data[ETH_HDR_LEN + 7] : 0; }
    bool    is_ARP_who() const { return (getARPType() == 1); }
    bool    is_ARP_is() const { return (getARPType() == 2); }

    uint8_t  getUdpHdrLen() const { return isUDP() ? 8 : 0; };
    uint16_t getUdpLen() const { return isUDP() ? ntoh16(ETH_HDR_LEN + getIpHdrLen() + 4) : 0; };
    bool     isARP() const { return (ethType() == 0x0806); };
    bool     isIPv4() const { return (ethType() == 0x0800); };
    bool     isIPv6() const { return (ethType() == 0x86dd); };
    bool     isIP() const { return (isIPv4() || isIPv6()); };
    bool     isICMP() const { return (isIP() && ((ipType() == 1) || (ipType() == 58))); };
    bool     isIGMP() const { return (isIP() && (ipType() == 2)); };
    bool     isTCP() const { return (isIP() && (ipType() == 6)); };
    bool     isUDP() const { return (isIP() && ipType() == 17); };
    bool     isMDNS() const { return (isUDP() && hasPort(5353)); };
    bool     isDNS() const { return (isUDP() && hasPort(53)); };
    bool     isSSDP() const { return (isUDP() && hasPort(1900)); };
    bool     isDHCP() const
    {
        return (isUDP() && ((hasPort(546) || hasPort(547) || hasPort(67) || hasPort(68))));
    };
    bool isWSDD() const { return (isUDP() && hasPort(3702)); };
    bool isHTTP() const { return (isTCP() && hasPort(80)); };
    bool isOTA() const { return (isUDP() && hasPort(8266)); }
    bool isNETBIOS() const { return (isUDP() && (hasPort(137) || hasPort(138) || hasPort(139))); }
    bool isSMB() const { return (isUDP() && hasPort(445)); }
    NetdumpIP getIP(uint16_t idx) const
    {
        return NetdumpIP(data[idx], data[idx + 1], data[idx + 2], data[idx + 3]);
    };

    NetdumpIP getIP6(uint16_t idx) const { return NetdumpIP((const uint8_t*)&data[idx], false); };
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

    bool hasIP(NetdumpIP ip) const { return (isIP() && ((ip == sourceIP()) || (ip == destIP()))); }

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
    uint16_t getSrcPort() const { return isIP() ? ntoh16(ETH_HDR_LEN + getIpHdrLen() + 0) : 0; }
    uint16_t getDstPort() const { return isIP() ? ntoh16(ETH_HDR_LEN + getIpHdrLen() + 2) : 0; }
    bool     hasPort(uint16_t p) const
    {
        return (isIP() && ((getSrcPort() == p) || (getDstPort() == p)));
    }

    const String toString() const;
    const String toString(PacketDetail netdumpDetail) const;
    void         printDetail(Print& out, const String& indent, const char* data, size_t size,
                             PacketDetail pd) const;

    const PacketType               packetType() const;
    const std::vector<PacketType>& allPacketTypes() const;

private:
    void setPacketType(PacketType);
    void setPacketTypes();

    void MACtoString(int dataIdx, StreamString& sstr) const;
    void ARPtoString(PacketDetail netdumpDetail, StreamString& sstr) const;
    void DNStoString(PacketDetail netdumpDetail, StreamString& sstr) const;
    void UDPtoString(PacketDetail netdumpDetail, StreamString& sstr) const;
    void TCPtoString(PacketDetail netdumpDetail, StreamString& sstr) const;
    void ICMPtoString(PacketDetail netdumpDetail, StreamString& sstr) const;
    void IGMPtoString(PacketDetail netdumpDetail, StreamString& sstr) const;
    void IPtoString(PacketDetail netdumpDetail, StreamString& sstr) const;
    void UKNWtoString(PacketDetail netdumpDetail, StreamString& sstr) const;

    time_t                  packetTime;
    int                     netif_idx;
    const char*             data;
    size_t                  packetLength;
    int                     out;
    int                     success;
    PacketType              thisPacketType;
    std::vector<PacketType> thisAllPacketTypes;
};

}  // namespace NetCapture

#endif /* __NETDUMP_PACKET_H */
