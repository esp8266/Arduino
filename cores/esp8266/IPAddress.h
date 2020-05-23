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
// compatibility macros to make lwIP-v1 compiling lwIP-v2 API
#define LWIP_IPV6_NUM_ADDRESSES 0
#define ip_2_ip4(x) (x)
#define ipv4_addr ip_addr
#define ipv4_addr_t ip_addr_t
#define IP_IS_V4_VAL(x) (1)
#define IP_SET_TYPE_VAL(x,y) do { (void)0; } while (0)
#define IP_ANY_TYPE (&ip_addr_any)
#define IP4_ADDR_ANY IPADDR_ANY
#define IP4_ADDR_ANY4 IP_ADDR_ANY
#define IPADDR4_INIT(x) { x }
#define CONST /* nothing: lwIP-v1 does not use const */
#define ip4_addr_netcmp ip_addr_netcmp
#define netif_dhcp_data(netif) ((netif)->dhcp)
#define netif_get_index(netif) ((u8_t)((netif)->num + 1))
#else // lwIP-v2+
#define CONST const
#if !LWIP_IPV6
struct ip_addr: ipv4_addr { };
#endif // !LWIP_IPV6
#endif // lwIP-v2+

// A class to make it easier to handle and pass around IP addresses
// IPv6 update:
// IPAddress is now a decorator class for lwIP's ip_addr_t
// fully backward compatible with legacy IPv4-only Arduino's
// with unchanged footprint when IPv6 is disabled

class IPAddress: public Printable {
    private:

        ip_addr_t _ip;

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

        void ctor32 (uint32_t);

    public:
        // Constructors
        IPAddress();
        IPAddress(const IPAddress& from);
        IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
        IPAddress(uint32_t address) { ctor32(address); }
        IPAddress(u32_t address) { ctor32(address); }
        IPAddress(int address) { ctor32(address); }
        IPAddress(const uint8_t *address);

        bool fromString(const char *address);
        bool fromString(const String &address) { return fromString(address.c_str()); }

        // Overloaded cast operator to allow IPAddress objects to be used where a pointer
        // to a four-byte uint8_t array is expected
        operator uint32_t() const { return isV4()? v4(): (uint32_t)0; }
        operator uint32_t()       { return isV4()? v4(): (uint32_t)0; }
        operator u32_t()    const { return isV4()? v4():    (u32_t)0; }
        operator u32_t()          { return isV4()? v4():    (u32_t)0; }

        bool isSet () const;
        operator bool () const { return isSet(); } // <-
        operator bool ()       { return isSet(); } // <- both are needed

        // generic IPv4 wrapper to uint32-view like arduino loves to see it
        const u32_t& v4() const { return ip_2_ip4(&_ip)->addr; } // for raw_address(const)
              u32_t& v4()       { return ip_2_ip4(&_ip)->addr; }

        bool operator==(const IPAddress& addr) const {
            return ip_addr_cmp(&_ip, &addr._ip);
        }
        bool operator!=(const IPAddress& addr) const {
            return !ip_addr_cmp(&_ip, &addr._ip);
        }
        bool operator==(uint32_t addr) const {
            return isV4() && v4() == addr;
        }
        bool operator==(u32_t addr) const {
            return isV4() && v4() == addr;
        }
        bool operator!=(uint32_t addr) const {
            return !(isV4() && v4() == addr);
        }
        bool operator!=(u32_t addr) const {
            return !(isV4() && v4() == addr);
        }
        bool operator==(const uint8_t* addr) const;

        int operator>>(int n) const {
            return isV4()? v4() >> n: 0;
        }

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
        IPAddress& operator=(const IPAddress&) = default;

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
        IPAddress(const ipv4_addr& fw_addr)   { setV4(); v4() = fw_addr.addr; }
        IPAddress(const ipv4_addr* fw_addr)   { setV4(); v4() = fw_addr->addr; }

        IPAddress& operator=(const ipv4_addr& fw_addr)   { setV4(); v4() = fw_addr.addr;  return *this; }
        IPAddress& operator=(const ipv4_addr* fw_addr)   { setV4(); v4() = fw_addr->addr; return *this; }

        operator       ip_addr_t () const { return  _ip; }
        operator const ip_addr_t*() const { return &_ip; }
        operator       ip_addr_t*()       { return &_ip; }

        bool isV4() const { return IP_IS_V4_VAL(_ip); }
        void setV4() { IP_SET_TYPE_VAL(_ip, IPADDR_TYPE_V4); }

        bool isLocal () const { return ip_addr_islinklocal(&_ip); }

#if LWIP_IPV6

        IPAddress(const ip_addr_t& lwip_addr) { ip_addr_copy(_ip, lwip_addr); }
        IPAddress(const ip_addr_t* lwip_addr) { ip_addr_copy(_ip, *lwip_addr); }

        IPAddress& operator=(const ip_addr_t& lwip_addr) { ip_addr_copy(_ip, lwip_addr); return *this; }
        IPAddress& operator=(const ip_addr_t* lwip_addr) { ip_addr_copy(_ip, *lwip_addr); return *this; }

        uint16_t* raw6()
        {
            setV6();
            return reinterpret_cast<uint16_t*>(ip_2_ip6(&_ip));
        }

        const uint16_t* raw6() const
        {
            return isV6()? reinterpret_cast<const uint16_t*>(ip_2_ip6(&_ip)): nullptr;
        }

        // when not IPv6, ip_addr_t == ip4_addr_t so this one would be ambiguous
        // required otherwise
        operator const ip4_addr_t*() const { return isV4()? ip_2_ip4(&_ip): nullptr; }

        bool isV6() const { return IP_IS_V6_VAL(_ip); }
        void setV6() { IP_SET_TYPE_VAL(_ip, IPADDR_TYPE_V6); }

    protected:
        bool fromString6(const char *address);

#else

        // allow portable code when IPv6 is not enabled

        uint16_t* raw6() { return nullptr; }
        const uint16_t* raw6() const { return nullptr; }
        bool isV6() const { return false; }
        void setV6() { }

#endif

    protected:
        bool fromString4(const char *address);

};

extern CONST IPAddress INADDR_ANY;
extern const IPAddress INADDR_NONE;

#endif
