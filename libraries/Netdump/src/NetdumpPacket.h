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

#ifndef __NETDUMP_PACKET_H
#define __NETDUMP_PACKET_H

#include <lwipopts.h>
#include <IPAddress.h>
#include <StreamString.h>
#include "NetdumpIP.h"

#define ETH_HDR_LEN 14

class NetdumpPacket
{
public:
    NetdumpPacket(int n, const char* d, size_t l, int o, int s)
        : netif_idx(n), data(d), len(l), out(o), success(s)
    {};

    virtual ~NetdumpPacket() {};

    enum class PacketDetail
    {
        NONE,
        FULL,
        CHARS
    };

    int netif_idx;
    const char* data;
    size_t len;
    int out;
    int success;

    uint16_t ntoh16(uint16_t idx)
    {
        return data[idx + 1] | (((uint16_t)data[idx]) << 8);
    };
    uint32_t ntoh32(uint16_t idx)
    {
        return ntoh16(idx + 2) | (((uint32_t)ntoh16(idx)) << 16);
    };
    uint8_t  byteData(uint16_t idx)
    {
        return data[idx];
    }
    const char* byteIdx(uint16_t idx)
    {
        return &data[idx];
    };
    uint16_t ethType()
    {
        return ntoh16(12);
    };
    uint8_t  ipType()
    {
        return isIPv4() ? data[ETH_HDR_LEN + 9] : data[ETH_HDR_LEN + 6];
    };

    uint16_t getIpHdrLen()
    {
        return isIPv4() ? (((unsigned char)data[ETH_HDR_LEN]) & 0x0f) << 2 : 40 ;   // IPv6 is fixed length
    }
    uint16_t getIpTotLen()
    {
        return ntoh16(ETH_HDR_LEN + 2);
    }
    //	uint16_t getIpOptLen()        { return getIpHdrLen() - 20; }
    //	uint16_t getIpUsrLen()        { return getIpTotLen() - getIpHdrLen(); }

    uint32_t getTcpSeq()
    {
        return ntoh32(ETH_HDR_LEN + getIpHdrLen() + 4);
    }
    uint32_t getTcpAck()
    {
        return ntoh32(ETH_HDR_LEN + getIpHdrLen() + 8);
    }
    uint16_t getTcpFlags()
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 12);
    }
    uint16_t getTcpWindow()
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 14);
    }
    uint8_t  getTcpHdrLen()
    {
        return (data[ETH_HDR_LEN + getIpHdrLen() + 12] >> 4) * 4;
    };//Header len is in multiple of 4 bytes
    uint16_t getTcpLen()
    {
        return getIpTotLen() - getIpHdrLen() - getTcpHdrLen() ;
    };

    uint8_t  getIcmpType()
    {
        return data[ETH_HDR_LEN + getIpHdrLen() + 0];
    }
    uint8_t  getIgmpType()
    {
        return data[ETH_HDR_LEN + getIpHdrLen() + 0];
    }

    uint8_t getARPType()
    {
        return data[ETH_HDR_LEN + 7];
    }
    bool    is_ARP_who()
    {
        return getARPType() == 1;
    }
    bool    is_ARP_is()
    {
        return getARPType() == 2;
    }

    uint8_t getUdpHdrLen()
    {
        return 8;
    };
    uint16_t getUdpLen()
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 4);
    };


    bool isARP()
    {
        return (ethType() == 0x0806);
    };
    bool isIPv4()
    {
        return (ethType() == 0x0800);
    };
    bool isIPv6()
    {
        return (ethType() == 0x86dd);
    };
    bool isIP()
    {
        return (isIPv4() || isIPv6());
    };
    bool isICMP()
    {
        return ((ipType() == 1) || ipType() == 58);
    };
    bool isIGMP()
    {
        return ipType() == 2;
    };
    bool isTCP()
    {
        return ipType() == 6;
    };
    bool isUDP()
    {
        return ipType() == 17;
    };
    bool isMDNS()
    {
        return hasPort(5353);
    };
    bool isDNS()
    {
        return hasPort(53);
    };
    bool isSSDP()
    {
        return hasPort(1900);
    };
    bool isDHCP()
    {
        return (hasPort(546) || hasPort(547) || hasPort(67) || hasPort(68));
    };
    bool isWSDD()
    {
        return (hasPort(3702));
    };
    bool isHTTP()
    {
        return (hasPort(80));
    };


    NetdumpIP getIP(uint16_t idx)
    {
        return NetdumpIP(data[idx],
                         data[idx + 1],
                         data[idx + 2],
                         data[idx + 3]);
    };

    NetdumpIP getIP6(uint16_t idx)
    {
        return NetdumpIP((const uint8_t*)&data[idx], false);
    };


    NetdumpIP sourceIP()
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

    NetdumpIP destIP()
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
    uint16_t getSrcPort()
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 0);
    }
    uint16_t getDstPort()
    {
        return ntoh16(ETH_HDR_LEN + getIpHdrLen() + 2);
    }
    bool     hasPort(uint16_t p)
    {
        return ((getSrcPort() == p) || (getDstPort() == p));
    }

    String toString(PacketDetail netdumpDetail = PacketDetail::NONE);
    void printDetail(Print& out, String indent, const char* data, size_t size, PacketDetail pd);

};


#endif /* LIBRARIES_ESPGOODIES_HR_SRC_NETDUMP_PACKET_H_ */
