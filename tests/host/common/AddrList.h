
// TODO
// mock AddrList with POSIX mock API
// later: real AddrList will work with lwIP API

// mock is IPv4 only

#ifndef __ADDRLISTX_H
#define __ADDRLISTX_H

#include <ESP8266WiFi.h>

namespace esp8266
{

namespace AddressListImplementation
{


struct netifWrapper
{
    netifWrapper (bool netif) : _netif(netif) {}
    netifWrapper (const netifWrapper& o) : _netif(o._netif) {}

    netifWrapper& operator= (const netifWrapper& o)
    {
        _netif = o._netif;
        return *this;
    }

    bool equal(const netifWrapper& o)
    {
        return _netif == o._netif;
    }

    // address properties
    IPAddress addr () const         { return WiFi.localIP(); }
    bool isLegacy () const          { return true; }
    bool isLocal () const           { return false; }
    bool isV4 () const              { return addr().isV4(); }
    bool isV6 () const              { return !addr().isV4(); }
    String toString() const         { return addr().toString(); }

    // related to legacy address (_num=0, ipv4)
    IPAddress ipv4 () const         { ip_info info; wifi_get_ip_info(0, &info); return info.ip; }
    IPAddress netmask () const      { ip_info info; wifi_get_ip_info(0, &info); return info.netmask; }
    IPAddress gw () const           { ip_info info; wifi_get_ip_info(0, &info); return info.gw; }

    // common to all addresses of this interface
    String ifname () const          { return "st"; }
    const char* ifhostname () const { return wifi_station_get_hostname(); }
    String ifmac () const           { uint8_t mac[20]; WiFi.macAddress(mac); return String((char*)mac); }
    int ifnumber () const           { return 0; }
    bool ifUp () const              { return true; }

    bool _netif;
};


class AddressListIterator
{
public:
    AddressListIterator (const netifWrapper& o) : netIf(o) {}
    AddressListIterator (bool netif) : netIf(netif)
    {
        // This constructor is called with lwIP's global netif_list, or
        // nullptr.  operator++() is designed to loop through _configured_
        // addresses.  That's why netIf's _num is initialized to -1 to allow
        // returning the first usable address to AddressList::begin().
        (void)operator++();
    }

    const netifWrapper& operator*  () const { return netIf; }
    const netifWrapper* operator-> () const { return &netIf; }

    bool operator== (AddressListIterator& o) { return netIf.equal(*o); }
    bool operator!= (AddressListIterator& o) { return !netIf.equal(*o); }

    AddressListIterator& operator= (const AddressListIterator& o) { netIf = o.netIf; return *this; }

    AddressListIterator operator++ (int)
    {
        AddressListIterator ret = *this;
        (void)operator++();
        return ret;
    }

    AddressListIterator& operator++ ()
    {
        netIf._netif = !netIf._netif;
        return *this;
    }

    netifWrapper netIf;
};


class AddressList
{
public:
  using const_iterator = const AddressListIterator;

  const_iterator begin () const { return const_iterator(true); }
  const_iterator   end () const { return const_iterator(false); }

};

inline AddressList::const_iterator begin (const AddressList& a) { return a.begin(); }
inline AddressList::const_iterator   end (const AddressList& a) { return a.end(); }


} // AddressListImplementation

} // esp8266

extern esp8266::AddressListImplementation::AddressList addrList;

#endif
