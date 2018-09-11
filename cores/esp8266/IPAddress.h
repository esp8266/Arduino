/*
 IPAddress.h - Base class that provides IPAddress
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

#ifndef IPAddress_h
#define IPAddress_h

#include <stdint.h>
#include <WString.h>
#include <Printable.h>

#include <lwip/init.h>
#include <lwip/ip_addr.h>

#if LWIP_VERSION_MAJOR == 1
#define ip_2_ip4(x) (x)
#endif

// A class to make it easier to handle and pass around IP addresses
// based on lwip's ip address (ipv4, limited compatibility with ipv6)

class IPAddress: public Printable {
    private:

        ip_addr_t _ip;

        // generic IPv4 wrapper to uint32-view like arduino loves to see it
        const u32_t& v4() const { return ip_2_ip4(&_ip)->addr; } // for raw_address(const)
        //    u32_t  v4() const { return ip_2_ip4(&_ip)->addr; } // cannot overload the above
              u32_t& v4()       { return ip_2_ip4(&_ip)->addr; }

        // Access the raw byte array containing the address.  Because this returns a pointer
        // to the internal structure rather than a copy of the address this function should only
        // be used when you know that the usage of the returned uint8_t* will be transient and not
        // stored.
        uint8_t* raw_address() {
            return reinterpret_cast<uint8_t*>(&v4());
        }
        const uint8_t* raw_address() const {
            return reinterpret_cast<const uint8_t*>(&v4());
        }

    public:
        // Constructors
        IPAddress();
        IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
        IPAddress(uint32_t address);
        IPAddress(const uint8_t *address);

        bool fromString(const char *address);
        bool fromString(const String &address) { return fromString(address.c_str()); }

        // Overloaded cast operator to allow IPAddress objects to be used where a pointer
        // to a four-byte uint8_t array is expected
        operator uint32_t() const {
            return isV4()? v4(): 0;
        }
        bool operator==(const IPAddress& addr) const {
            return ip_addr_cmp(&_ip, &addr._ip);
        }
        bool operator==(uint32_t addr) const {
            return isV4() && v4() == addr;
        }
        bool operator==(const uint8_t* addr) const;

        // Overloaded index operator to allow getting and setting individual octets of the address
        uint8_t operator[](int index) const {
            return isV4()? *(raw_address() + index): 0;
        }
        uint8_t& operator[](int index) {
            setV4();
            return *(raw_address() + index);
        }

        // Overloaded copy operators to allow initialisation of IPAddress objects from other types
        IPAddress& operator=(const uint8_t *address);
        IPAddress& operator=(uint32_t address);

        virtual size_t printTo(Print& p) const;
        String toString() const;

        /* 
                check if input string(arg) is a valid IPV4 address or not.
                return true on valid.
                return false on invalid.
        */
        static bool isValid(const String& arg);
        static bool isValid(const char* arg);

        friend class EthernetClass;
        friend class UDP;
        friend class Client;
        friend class Server;
        friend class DhcpClass;
        friend class DNSClient;

        /*
               lwIP address compatibility
        */
        IPAddress(ipv4_addr fw_addr);
        IPAddress(const ipv4_addr* fw_addr);
        IPAddress(const ip_addr_t& lwip_addr) { _ip = lwip_addr; }
        IPAddress(const ip_addr_t* lwip_addr) { _ip = *lwip_addr; }
        
        operator       ip_addr_t () const { return  _ip; }
        operator const ip_addr_t*() const { return &_ip; }
        operator       ip_addr_t*()       { return &_ip; }

        bool isV4() const { return IP_IS_V4_VAL(_ip); }
        void setV4() { IP_SET_TYPE_VAL(_ip, IPADDR_TYPE_V4); }
 
#if LWIP_IPV6

        bool fromString6(const char *address);

        uint16_t* raw6()
        {
            setV6();
            return isV6()? reinterpret_cast<uint16_t*>(ip_2_ip6(&_ip)): 0;
        }

        const uint16_t* raw6() const
        {
            return reinterpret_cast<const uint16_t*>(ip_2_ip6(&_ip));
        }

        // when not IPv6, ip_addr_t == ip4_addr_t so this one would be ambiguous
        // required otherwise
        operator const ip4_addr_t*() const { return isV4()? ip_2_ip4(&_ip): nullptr; }
        
        bool isV6() const { return IP_IS_V6_VAL(_ip); }
        void setV6() { IP_SET_TYPE_VAL(_ip, IPADDR_TYPE_V6); }

#endif
};

extern const IPAddress INADDR_NONE;

#endif
