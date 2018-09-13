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

IPAddress::IPAddress() {
    setV4();
    v4() = 0;
}

IPAddress::IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
    setV4();
    (*this)[0] = first_octet;
    (*this)[1] = second_octet;
    (*this)[2] = third_octet;
    (*this)[3] = fourth_octet;
}

IPAddress::IPAddress(uint32_t address) {
    setV4();
    v4() = address;
}

IPAddress::IPAddress(const uint8_t *address) {
    setV4();
    v4() = *reinterpret_cast<const uint32_t*>(address);
}

bool IPAddress::fromString(const char *address) {
    // TODO: (IPv4) add support for "a", "a.b", "a.b.c" formats

#if LWIP_IPV6
    const char *org = address;
#endif

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
#if LWIP_IPV6
            return fromString6(org);
#else
            return false;
#endif
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

#if LWIP_IPV6
    if (isV6()) {
        const uint16_t* raw = raw6();
        for(int i = 0; i < 8; i++) {
            n += p.printf("%x", PP_NTOHS(raw[i]));
            if (i != 7)
                n += p.print(':');
        }
        return n;
    }
#endif

    for(int i = 0; i < 3; i++) {
        n += p.print((*this)[i], DEC);
        n += p.print('.');
    }
    n += p.print((*this)[3], DEC);
    return n;
}

String IPAddress::toString() const
{
    char szRet[16];
    sprintf(szRet,"%u.%u.%u.%u", (*this)[0], (*this)[1], (*this)[2], (*this)[3]);
    return String(szRet);
}

bool IPAddress::isValid(const String& arg) {
	return IPAddress().fromString(arg);
}

bool IPAddress::isValid(const char* arg) {
	return IPAddress().fromString(arg);
}

const IPAddress INADDR_NONE(0, 0, 0, 0);

/**************************************/

/* lwip */

IPAddress::IPAddress(ipv4_addr fw_addr) {
    setV4();
    v4() = fw_addr.addr;
}

IPAddress::IPAddress(const ipv4_addr* fw_addr) {
    setV4();
    v4() = fw_addr->addr;
}

/* ipv6 */

#if LWIP_IPV6

bool IPAddress::fromString6(const char *address) {
    // TODO: test test test
    // TODO: "::"

    uint32_t acc = 0; // Accumulator
    uint8_t dots = 0;

    while (*address)
    {
        char c = tolower(*address++);
        if (isalnum(c))
        {
            if (c >= 'a')
                c -= 'a' - '0' - 10;
            acc = acc * 16 + (c - '0');
            if (acc > 0xffff)
                // Value out of range
                return false;
        }
        else if (c == ':')
        {
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

    if (dots != 7)
        // Too few separators
        return false;
    raw6()[7] = PP_HTONS(acc);

//os_printf("(%x:%x:%x:%x:%x:%x:%x:%x)", (*this)[0]

    setV6();
    return true;
}

#endif
