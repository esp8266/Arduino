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

*/
#include <Arduino.h>
#include <NetdumpIP.h>

namespace NetCapture
{

NetdumpIP::NetdumpIP()
{
}

NetdumpIP::NetdumpIP(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet)
{
    setV4();
    (*this)[0] = first_octet;
    (*this)[1] = second_octet;
    (*this)[2] = third_octet;
    (*this)[3] = fourth_octet;
}

NetdumpIP::NetdumpIP(const uint8_t *address, bool v4)
{
    uint8_t cnt;
    if (v4)
    {
        cnt = 4;
        setV4();
    }
    else
    {
        cnt = 16;
        setV6();
    }
    for (int i = 0; i < cnt; i++)
    {
        (*this)[i] = address[i];
    }
}

NetdumpIP::NetdumpIP(const IPAddress& ip)
{
    if (!ip.isSet())
    {
        setUnset();
    }
    else if (ip.isV4())
    {
        setV4();
        for (int i = 0; i < 4; i++)
        {
            rawip[i] = ip[i];
        }
    }
    else
    {
        setV6();
        for (int i = 0; i < 16; i++)
        {
            rawip[i] = ip[i];
        }
    }
}

NetdumpIP::NetdumpIP(const String& ip)
{
    if (!fromString(ip.c_str()))
    {
        setUnset();
    }
}

bool NetdumpIP::fromString(const char *address)
{
    if (!fromString4(address))
    {
        return fromString6(address);
    }
    return true;
}

bool NetdumpIP::fromString4(const char *address)
{
    // TODO: (IPv4) add support for "a", "a.b", "a.b.c" formats

    uint16_t acc = 0; // Accumulator
    uint8_t dots = 0;

    while (*address)
    {
        char c = *address++;
        if (c >= '0' && c <= '9')
        {
            acc = acc * 10 + (c - '0');
            if (acc > 255)
            {
                // Value out of [0..255] range
                return false;
            }
        }
        else if (c == '.')
        {
            if (dots == 3)
            {
                // Too much dots (there must be 3 dots)
                return false;
            }
            (*this)[dots++] = acc;
            acc = 0;
        }
        else
        {
            // Invalid char
            return false;
        }
    }

    if (dots != 3)
    {
        // Too few dots (there must be 3 dots)
        return false;
    }
    (*this)[3] = acc;

    setV4();
    return true;
}

bool NetdumpIP::fromString6(const char *address)
{
    // TODO: test test test

    uint32_t acc = 0; // Accumulator
    int dots = 0, doubledots = -1;

    while (*address)
    {
        char c = tolower(*address++);
        if (isalnum(c))
        {
            if (c >= 'a')
            {
                c -= 'a' - '0' - 10;
            }
            acc = acc * 16 + (c - '0');
            if (acc > 0xffff)
                // Value out of range
            {
                return false;
            }
        }
        else if (c == ':')
        {
            if (*address == ':')
            {
                if (doubledots >= 0)
                    // :: allowed once
                {
                    return false;
                }
                // remember location
                doubledots = dots + !!acc;
                address++;
            }
            if (dots == 7)
                // too many separators
            {
                return false;
            }
            reinterpret_cast<uint16_t*>(rawip)[dots++] = PP_HTONS(acc);
            acc = 0;
        }
        else
            // Invalid char
        {
            return false;
        }
    }

    if (doubledots == -1 && dots != 7)
        // Too few separators
    {
        return false;
    }
    reinterpret_cast<uint16_t*>(rawip)[dots++] = PP_HTONS(acc);

    if (doubledots != -1)
    {
        for (int i = dots - doubledots - 1; i >= 0; i--)
        {
            reinterpret_cast<uint16_t*>(rawip)[8 - dots + doubledots + i] = reinterpret_cast<uint16_t*>(rawip)[doubledots + i];
        }
        for (int i = doubledots; i < 8 - dots + doubledots; i++)
        {
            reinterpret_cast<uint16_t*>(rawip)[i] = 0;
        }
    }

    setV6();
    return true;
}

String NetdumpIP::toString()
{
    StreamString sstr;
    if (isV6())
    {
        sstr.reserve(40); // 8 shorts x 4 chars each + 7 colons + nullterm

    }
    else
    {
        sstr.reserve(16); // 4 bytes with 3 chars max + 3 dots + nullterm, or '(IP unset)'
    }
    printTo(sstr);
    return sstr;
}

size_t NetdumpIP::printTo(Print& p)
{
    size_t n = 0;

    if (!isSet())
    {
        return p.print(F("(IP unset)"));
    }

    if (isV6())
    {
        int count0 = 0;
        for (int i = 0; i < 8; i++)
        {
            uint16_t bit = PP_NTOHS(reinterpret_cast<const uint16_t*>(rawip)[i]);
            if (bit || count0 < 0)
            {
                n += p.printf_P(PSTR("%x"), bit);
                if (count0 > 0)
                    // no more hiding 0
                {
                    count0 = -8;
                }
            }
            else
            {
                count0++;
            }
            if ((i != 7 && count0 < 2) || count0 == 7)
            {
                n += p.print(':');
            }
        }
        return n;
    }
    for (int i = 0; i < 4; i++)
    {
        n += p.print((*this)[i], DEC);
        if (i != 3)
        {
            n += p.print('.');
        }
    }
    return n;
}

bool NetdumpIP::compareRaw(IPversion v, const uint8_t* a,  const uint8_t* b) const
{
    for (int i = 0; i < (v == IPversion::IPV4 ? 4 : 16); i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

bool NetdumpIP::compareIP(const IPAddress& ip) const
{
    switch (ipv)
    {
    case IPversion::UNSET :
        if (ip.isSet())
        {
            return false;
        }
        else
        {
            return true;
        }
        break;
    case IPversion::IPV4 :
        if (ip.isV6() || !ip.isSet())
        {
            return false;
        }
        else
        {
            return compareRaw(IPversion::IPV4, rawip, reinterpret_cast<const uint8_t*>(&ip.v4()));
        }
        break;
    case IPversion::IPV6 :
        if (ip.isV4() || !ip.isSet())
        {
            return false;
        }
        else
        {
            return compareRaw(IPversion::IPV6, rawip, reinterpret_cast<const uint8_t*>(ip.raw6()));
        }
        break;
    default :
        return false;
        break;
    }
}

bool NetdumpIP::compareIP(const NetdumpIP& nip) const
{
    switch (ipv)
    {
    case IPversion::UNSET :
        if (nip.isSet())
        {
            return false;
        }
        else
        {
            return true;
        }
        break;
    case IPversion::IPV4 :
        if (nip.isV6() || !nip.isSet())
        {
            return false;
        }
        else
        {
            return compareRaw(IPversion::IPV4, rawip, nip.rawip);
        }
        break;
    case IPversion::IPV6 :
        if (nip.isV4() || !nip.isSet())
        {
            return false;
        }
        else
        {
            return compareRaw(IPversion::IPV6, rawip, nip.rawip);
        }
        break;
    default :
        return false;
        break;
    }
}

} // namespace NetCapture
