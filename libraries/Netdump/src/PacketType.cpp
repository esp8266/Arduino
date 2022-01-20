/*
    PacketType.cpp

    Created on: 19 nov. 2019
        Author: Herman
*/

#include <PacketType.h>

namespace NetCapture
{
PacketType::PacketType() { }

String PacketType::toString() const
{
    switch (ptype)
    {
    case PType::ARP:
        return PSTR("ARP");
    case PType::IP:
        return PSTR("IP");
    case PType::UDP:
        return PSTR("UDP");
    case PType::MDNS:
        return PSTR("MDNS");
    case PType::DNS:
        return PSTR("DNS");
    case PType::SSDP:
        return PSTR("SSDP");
    case PType::DHCP:
        return PSTR("DHCP");
    case PType::WSDD:
        return PSTR("WSDD");
    case PType::NETBIOS:
        return PSTR("NBIO");
    case PType::SMB:
        return PSTR("SMB");
    case PType::OTA:
        return PSTR("OTA");
    case PType::TCP:
        return PSTR("TCP");
    case PType::HTTP:
        return PSTR("HTTP");
    case PType::ICMP:
        return PSTR("ICMP");
    case PType::IGMP:
        return PSTR("IGMP");
    case PType::IPv4:
        return PSTR("IPv4");
    case PType::IPv6:
        return PSTR("IPv6");
    case PType::UKNW:
        return PSTR("UKNW");
    default:
        return PSTR("ERR");
    };
}

} /* namespace NetCapture */
