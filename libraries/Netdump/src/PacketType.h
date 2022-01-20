/*
    PacketType.h

    Created on: 19 nov. 2019
        Author: Herman
*/

#ifndef LIBRARIES_NETDUMP_SRC_PACKETTYPE_H_
#define LIBRARIES_NETDUMP_SRC_PACKETTYPE_H_
#include "Arduino.h"

namespace NetCapture
{

class PacketType
{
public:

    enum PType : int
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
        IPv4,
        IPv6,
        UKNW,
    };

    PacketType();
    PacketType(PType pt) : ptype(pt) {};

    operator PType() const
    {
        return ptype;
    };
    bool operator==(const PacketType& p)
    {
        return ptype == p.ptype;
    };

    String toString() const;

private:
    PType ptype;
};

} /* namespace NetCapture */

#endif /* LIBRARIES_NETDUMP_SRC_PACKETTYPE_H_ */
