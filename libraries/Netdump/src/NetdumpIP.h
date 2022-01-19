/*
    NetdumpIP.h

    Created on: 18 mei 2019
        Author: Herman
*/

#ifndef LIBRARIES_ESPGOODIES_HR_SRC_NETDUMP_NETDUMPIP_H_
#define LIBRARIES_ESPGOODIES_HR_SRC_NETDUMP_NETDUMPIP_H_

#include <IPAddress.h>
#include <StreamString.h>
#include <lwip/init.h>
#include <stdint.h>

namespace NetCapture
{
class NetdumpIP
{
   public:
    NetdumpIP();

    NetdumpIP(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    NetdumpIP(const uint8_t* address, bool V4 = true);
    NetdumpIP(const IPAddress& ip);
    NetdumpIP(const String& ip);

    uint8_t& operator[](int index)
    {
        return rawip[index];
    }

    bool fromString(const char* address);

    String toString();

   private:
    enum class IPversion
    {
        UNSET,
        IPV4,
        IPV6
    };
    IPversion ipv = IPversion::UNSET;

    uint8_t rawip[16] = {0};

    void setV4()
    {
        ipv = IPversion::IPV4;
    };
    void setV6()
    {
        ipv = IPversion::IPV6;
    };
    void setUnset()
    {
        ipv = IPversion::UNSET;
    };
    bool isV4() const
    {
        return (ipv == IPversion::IPV4);
    };
    bool isV6() const
    {
        return (ipv == IPversion::IPV6);
    };
    bool isUnset() const
    {
        return (ipv == IPversion::UNSET);
    };
    bool isSet() const
    {
        return (ipv != IPversion::UNSET);
    };

    bool compareRaw(IPversion v, const uint8_t* a, const uint8_t* b) const;
    bool compareIP(const IPAddress& ip) const;
    bool compareIP(const NetdumpIP& nip) const;

    bool fromString4(const char* address);
    bool fromString6(const char* address);

    size_t printTo(Print& p);

   public:
    bool operator==(const IPAddress& addr) const
    {
        return compareIP(addr);
    };
    bool operator!=(const IPAddress& addr)
    {
        return compareIP(addr);
    };
    bool operator==(const NetdumpIP& addr)
    {
        return compareIP(addr);
    };
    bool operator!=(const NetdumpIP& addr)
    {
        return !compareIP(addr);
    };
};

}  // namespace NetCapture

#endif /* LIBRARIES_ESPGOODIES_HR_SRC_NETDUMP_NETDUMPIP_H_ */
