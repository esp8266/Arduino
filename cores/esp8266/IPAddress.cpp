/*
 IPAddress.cpp - Base class that provides IPAddress
 Copyright (c) 2011 Adrian McEwen.  All right reserved.

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

#include <Arduino.h>
#include <IPAddress.h>
#include <Print.h>
#include <StreamString.h>

IPAddress::IPAddress(const IPAddress& from)
{
    ip_addr_copy(_ip, from._ip);
}

IPAddress::IPAddress(IPAddress&& from)
{
    ip_addr_copy(_ip, from._ip);
}

IPAddress::IPAddress() {
    _ip = *IP_ANY_TYPE; // lwIP's v4-or-v6 generic address
}

bool IPAddress::isSet () const {
    return !ip_addr_isany(&_ip) && ((*this) != IPADDR_NONE);
}

IPAddress::IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
    setV4();
    (*this)[0] = first_octet;
    (*this)[1] = second_octet;
    (*this)[2] = third_octet;
    (*this)[3] = fourth_octet;
}

void IPAddress::ctor32(uint32_t address) {
    setV4();
    v4() = address;
}

IPAddress::IPAddress(const uint8_t *address) {
    setV4();
    (*this)[0] = address[0];
    (*this)[1] = address[1];
    (*this)[2] = address[2];
    (*this)[3] = address[3];
}

bool IPAddress::fromString(const char *address) {
    if (!fromString4(address)) {
#if LWIP_IPV6
        return fromString6(address);
#else
        return false;
#endif
    }
    return true;
}

bool IPAddress::fromString4(const char *address) {
    // TODO: (IPv4) add support for "a", "a.b", "a.b.c" formats

    uint16_t acc = 0; // Accumulator
    uint8_t dots = 0;

    while (*address)
    {
        char c = *address++;
        if (c >= '0' && c <= '9')
        {
            acc = acc * 10 + (c - '0');
            if (acc > 255) {
                // Value out of [0..255] range
                return false;
            }
        }
        else if (c == '.')
        {
            if (dots == 3) {
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

    if (dots != 3) {
        // Too few dots (there must be 3 dots)
        return false;
    }
    (*this)[3] = acc;

    setV4();
    return true;
}

IPAddress& IPAddress::operator=(const uint8_t *address) {
    setV4();
    v4() = *reinterpret_cast<const uint32_t*>(address);
    return *this;
}

IPAddress& IPAddress::operator=(uint32_t address) {
    setV4();
    v4() = address;
    return *this;
}

bool IPAddress::operator==(const uint8_t* addr) const {
    return isV4() && v4() == *reinterpret_cast<const uint32_t*>(addr);
}

size_t IPAddress::printTo(Print& p) const {
    size_t n = 0;

    if (!isSet())
        return p.print(F("(IP unset)"));

#if LWIP_IPV6
    if (isV6()) {
        int count0 = 0;
        for (int i = 0; i < 8; i++) {
            uint16_t bit = PP_NTOHS(raw6()[i]);
            if (bit || count0 < 0) {
                n += p.printf("%x", bit);
                if (count0 > 0)
                    // no more hiding 0
                    count0 = -8;
            } else
                count0++;
            if ((i != 7 && count0 < 2) || count0 == 7)
                n += p.print(':');
        }
        return n;
    }
#endif

    for(int i = 0; i < 4; i++) {
        n += p.print((*this)[i], DEC);
        if (i != 3)
            n += p.print('.');
    }
    return n;
}

String IPAddress::toString() const
{
    StreamString sstr;
#if LWIP_IPV6
    if (isV6())
        sstr.reserve(40); // 8 shorts x 4 chars each + 7 colons + nullterm
    else
#endif
        sstr.reserve(16); // 4 bytes with 3 chars max + 3 dots + nullterm, or '(IP unset)'
    printTo(sstr);
    return sstr;
}

bool IPAddress::isValid(const String& arg) {
	return IPAddress().fromString(arg);
}

bool IPAddress::isValid(const char* arg) {
	return IPAddress().fromString(arg);
}

const IPAddress INADDR_ANY; // generic "0.0.0.0" for IPv4 & IPv6
const IPAddress INADDR_NONE(255,255,255,255);

void IPAddress::clear() {
    (*this) = INADDR_ANY;
}

/**************************************/

#if LWIP_IPV6

bool IPAddress::fromString6(const char *address) {
    // TODO: test test test

    uint32_t acc = 0; // Accumulator
    int dots = 0, doubledots = -1;

    while (*address)
    {
        char c = tolower(*address++);
        if (isalnum(c)) {
            if (c >= 'a')
                c -= 'a' - '0' - 10;
            acc = acc * 16 + (c - '0');
            if (acc > 0xffff)
                // Value out of range
                return false;
        }
        else if (c == ':') {
            if (*address == ':') {
                if (doubledots >= 0)
                    // :: allowed once
                    return false;
                // remember location
                doubledots = dots + !!acc;
                address++;
            }
            if (dots == 7)
                // too many separators
                return false;
            raw6()[dots++] = PP_HTONS(acc);
            acc = 0;
        }
        else
            // Invalid char
            return false;
    }

    if (doubledots == -1 && dots != 7)
        // Too few separators
        return false;
    raw6()[dots++] = PP_HTONS(acc);

    if (doubledots != -1) {
        for (int i = dots - doubledots - 1; i >= 0; i--)
            raw6()[8 - dots + doubledots + i] = raw6()[doubledots + i];
        for (int i = doubledots; i < 8 - dots + doubledots; i++)
            raw6()[i] = 0;
    }

    setV6();
    return true;
}

#endif
