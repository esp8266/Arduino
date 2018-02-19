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
    _address.dword = 0;
}

IPAddress::IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
    _address.bytes[0] = first_octet;
    _address.bytes[1] = second_octet;
    _address.bytes[2] = third_octet;
    _address.bytes[3] = fourth_octet;
}

IPAddress::IPAddress(uint32_t address) {
    _address.dword = address;
}

IPAddress::IPAddress(const uint8_t *address) {
    memcpy(_address.bytes, address, sizeof(_address.bytes));
}

bool IPAddress::fromString(const char *address) {
    // TODO: add support for "a", "a.b", "a.b.c" formats

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
            _address.bytes[dots++] = acc;
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
    _address.bytes[3] = acc;
    return true;
}

IPAddress& IPAddress::operator=(const uint8_t *address) {
    memcpy(_address.bytes, address, sizeof(_address.bytes));
    return *this;
}

IPAddress& IPAddress::operator=(uint32_t address) {
    _address.dword = address;
    return *this;
}

bool IPAddress::operator==(const uint8_t* addr) const {
    return memcmp(addr, _address.bytes, sizeof(_address.bytes)) == 0;
}

size_t IPAddress::printTo(Print& p) const {
    size_t n = 0;
    for(int i = 0; i < 3; i++) {
        n += p.print(_address.bytes[i], DEC);
        n += p.print('.');
    }
    n += p.print(_address.bytes[3], DEC);
    return n;
}

String IPAddress::toString() const
{
    char szRet[16];
    sprintf(szRet,"%u.%u.%u.%u", _address.bytes[0], _address.bytes[1], _address.bytes[2], _address.bytes[3]);
    return String(szRet);
}

/* ref: https://code.woboq.org/userspace/glibc/resolv/inet_pton.c.html#inet_pton4 */
static bool inet_pton4(const char *start, const char *end) {
	bool saw_digit = false;
	char ch;
	uint8_t octets = 0;
	unsigned char tmp[4], *tp;
	*(tp = tmp) = 0;

	while (start < end) {
		ch = *start++;
		if (isdigit(ch)) {
			unsigned int _new = *tp * 10 + (ch - '0');
			if (saw_digit && *tp == 0)
				return false;
			if (_new > 255)
				return false;
			*tp = _new;
			if (!saw_digit) {
				if (++octets > 4)
					return false;
				saw_digit = true;
			}
		} else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return false;
			*++tp = 0;
			saw_digit = false;
		} else {
			return false;
		}
	}

	if (octets < 4)
		return false;

	return true;
}

bool IPAddress::isValid(const String& arg) {
	auto c_str = arg.c_str();
	return inet_pton4(c_str, c_str + arg.length());
}

bool IPAddress::isValid(const char* arg, size_t len) {
	return inet_pton4(arg, arg + len);
}

const IPAddress INADDR_NONE(0, 0, 0, 0);
