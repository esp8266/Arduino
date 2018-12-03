/*
 AddrList.h - cycle through lwIP netif's ip addresses like a c++ list
 Copyright (c) 2018 david gauchard.  All right reserved.

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

/*
  This class allows to explore all configured IP addresses
  in lwIP netifs, with that kind of c++ loop:

  for (auto a: addrList)
    out.printf("IF='%s' index=%d legacy=%d IPv4=%d local=%d hostname='%s' addr= %s\n",
               a.iface().c_str(),
               a.number(),
               a.addr().isLegacy(),
               a.addr().isV4(),
               a.addr().isLocal(),
               a.hostname().c_str(),
               a.addr().toString().c_str());

  This loop:

      while (WiFi.status() != WL_CONNECTED()) {
          Serial.print('.');
          delay(500);
      }

  can be replaced by:

      for (bool configured = false; !configured; ) {
          for (auto iface: addrList)
              if ((configured = !iface.addr().isLocal())
                  break;
          Serial.print('.');
          delay(500);
      }

  waiting for an IPv6 global address:

      for (bool configured = false; !configured; ) {
          for (auto iface: addrList)
              if ((configured = (   !iface.addr().isV4()
                                 && !iface.addr().isLocal())))
                  break;
          Serial.print('.');
          delay(500);
      }

  waiting for an IPv6 global address, on a specific interface:

      for (bool configured = false; !configured; ) {
          for (auto iface: addrList)
              if ((configured = (   !iface.addr().isV4()
                                 && !iface.addr().isLocal()
                                 && iface.number() == STATION_IF)))
                  break;
          Serial.print('.');
          delay(500);
      }
*/

#ifndef __ADDRLIST_H
#define __ADDRLIST_H

#include <IPAddress.h>
#include <lwip/netif.h>

#if LWIP_IPV6
#define IF_NUM_ADDRESSES (1 + LWIP_IPV6_NUM_ADDRESSES)
#else
#define IF_NUM_ADDRESSES (1)
#endif


namespace esp8266
{

namespace AddressListImplementation
{


struct netifWrapper
{
    netifWrapper(netif * netif) : _netif(netif), _num(-1) {}
    netifWrapper(const netifWrapper & o) : _netif(o._netif), _num(o._num) {}

    netifWrapper& operator=(const netifWrapper & o) {_netif = o._netif; _num = o._num; return *this;}

    bool equal (const netifWrapper & o) 
    {
        return _netif == o._netif && (!_netif || _num == o._num);
    }


    bool isLegacy() const         { return _num == 0; }
    bool isLocal() const          { return addr().isLocal(); }
    IPAddress addr () const       { return ipFromNetifNum(); }
    IPAddress netmask () const    { return _netif->netmask; }
    IPAddress gw () const         { return _netif->gw; }
    String iface () const         { return String(_netif->name[0]) + _netif->name[1]; }
    const char* hostname () const { return _netif->hostname?: emptyString.c_str(); }
    const char* mac () const      { return (const char*)_netif->hwaddr; }
    int number () const           { return _netif->num; }

    const ip_addr_t* ipFromNetifNum () const 
    {
#if LWIP_IPV6
        return _num ? &_netif->ip6_addr[_num - 1] : &_netif->ip_addr;
#else
        return &_netif->ip_addr;
#endif
    }


    netif * _netif;
    int _num;
};



class AddressListIterator
{
public:
    AddressListIterator(const netifWrapper &o) : netIf(o) {}
    AddressListIterator(netif * netif) : netIf(netif) {}

    const netifWrapper& operator* () const {return netIf;}
    const netifWrapper* operator->() const {return &netIf;}

    bool operator==(AddressListIterator & o) {return netIf.equal(*o);}
    bool operator!=(AddressListIterator & o) {return !netIf.equal(*o);}

    AddressListIterator & operator= (const AddressListIterator& o) {netIf = o.netIf; return *this; }

    AddressListIterator operator++(int) 
    {
        AddressListIterator ret = *this;
        ++(*this);
        return ret;
    }

    AddressListIterator & operator++()
    {
        while (netIf._netif) 
        {
            if (++netIf._num == IF_NUM_ADDRESSES) 
            {
                netIf = netifWrapper(netIf._netif->next); //num is inited to -1
                continue;
            }
            if (!ip_addr_isany(netIf.ipFromNetifNum()))
                break;
        }
        return *this;
    }

    netifWrapper netIf;
};



class AddressList
{
public:
  using const_iterator = const AddressListIterator;

  const_iterator begin() const {return const_iterator(netif_list);}
  const_iterator end()   const {return const_iterator(nullptr);}

};


inline AddressList::const_iterator begin(const AddressList &a) {return a.begin();}
inline AddressList::const_iterator end(const AddressList &a) {return a.end();}

} //AddressListImplementation

} //esp8266

extern esp8266::AddressListImplementation::AddressList addrList;


#endif
