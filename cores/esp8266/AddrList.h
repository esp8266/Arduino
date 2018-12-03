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

  for (auto a: ifList)
    out.printf("IF='%s' index=%d legacy=%d IPv4=%d local=%d hostname='%s' addr= %s\n",
               a->iface().c_str(),
               a->ifnumber(),
               a->addr().isLegacy(),
               a->addr().isV4(),
               a->addr().isLocal(),
               a->hostname().c_str(),
               a->addr().toString().c_str());

  This loop:

      while (WiFi.status() != WL_CONNECTED()) {
          Serial.print('.');
          delay(500);
      }

  can be replaced by:

      for (bool configured = false; !configured; ) {
          for (auto iface: ifList)
              if ((configured = !iface->addr().isLocal())
                  break;
          Serial.print('.');
          delay(500);
      }

  waiting for an IPv6 global address:

      for (bool configured = false; !configured; ) {
          for (auto iface: ifList)
              if ((configured = (   !iface->addr()->isV4()
                                 && !iface->addr().isLocal())))
                  break;
          Serial.print('.');
          delay(500);
      }

  waiting for an IPv6 global address, on a specific interface:

      for (bool configured = false; !configured; ) {
          for (auto iface: ifList)
              if ((configured = (   !iface->addr()->isV4()
                                 && !iface->addr().isLocal()
                                 && iface->ifnumber() == STATION_IF)))
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


class AddrListClass {

    // no member in this class
    // lwIP's global 'struct netif* netif_list' is used
    // designed to be used with 'for (auto x: ifList)'

    public:

        class const_iterator {

            public:

                // iterator operations:

                const_iterator (bool begin = true): _netif(begin? netif_list: nullptr), _num(-1) { ++*this; }
                const_iterator (const const_iterator& o): _netif(o._netif), _num(o._num) { }
                const_iterator& operator= (const const_iterator& o) { _netif = o._netif; _num = o._num; return *this; }

                bool operator!= (const const_iterator& o) { return !equal(o); }
                bool operator== (const const_iterator& o) { return  equal(o); }

                const_iterator operator++(int) {
                    const_iterator ret = *this;
                    ++(*this);
                    return ret;
                }

                const_iterator& operator++() {
                    while (_netif) {
                        if (++_num == IF_NUM_ADDRESSES) {
                            _num = -1;
                            _netif = _netif->next;
                            continue;
                        }
                        if (!ip_addr_isany(_ip_from_netif_num()))
                            break;
                    }
                    return *this;
                }

                // (*iterator) emulation:
                const const_iterator& operator*  () const { return *this; }
                const const_iterator* operator-> () const { return  this; }

                // iterated address
                bool isLegacy() const { return _num == 0; }
                bool isLocal() const { return addr().isLocal(); }
                IPAddress addr () const { return _ip_from_netif_num(); }
                
                // properties of legacy/IPv4 address (one per interface)
                IPAddress netmask () const { return _netif->netmask; }
                IPAddress gw () const { return _netif->gw; }

                // interface properties (common to all addresses in the interface)
                String ifname () const { return String(_netif->name[0]) + _netif->name[1]; }
                const char* ifhostname () const { return _netif->hostname?: emptyString.c_str(); }
                const char* ifmac () const { return (const char*)_netif->hwaddr; }
                int ifnumber () const { return _netif->num; }

            protected:

                bool equal (const const_iterator& o) {
                    return    _netif == o._netif
                           && (!_netif || _num == o._num);
                }

                const ip_addr_t* _ip_from_netif_num () const {
#if LWIP_IPV6
                    return _num? &_netif->ip6_addr[_num - 1]: &_netif->ip_addr;
#else
                    return &_netif->ip_addr;
#endif
                }

                netif* _netif;
                int _num; // address index (0 is legacy, _num-1 is ip6_addr[]'s index)
        };

        const const_iterator begin () const { return const_iterator(true);  }
        const const_iterator end   () const { return const_iterator(false); }
};

extern AddrListClass addrList;

#endif // __ADDRLIST_H
