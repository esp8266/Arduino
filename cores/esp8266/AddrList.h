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

  for (auto a: addrList) {
    out.printf("IF='%s' n=%d legacy=%d IPv4=%d local=%d hostname='%s' addr= ",
               a->ifname().c_str(),
               a->ifnumber(),
               a->isLegacy(),
               a->isV4(),
               a->isLocal(),
               a->hostname().c_str());
    a->printTo(out);
    out.println();
  }

  This loop:

      while (WiFi.status() != WL_CONNECTED()) {
          Serial.print('.');
          delay(500);
      }

  can be replaced with:

      for (bool configured = false; !configured; ) {
          for (auto addr: addrList)
              if ((configured = !addr->isLocal()))
                  break;
          Serial.print('.');
          delay(500);
      }

  waiting for an IPv6 global address:

      for (bool configured = false; !configured; ) {
          for (auto addr: addrList)
              if ((configured = (   addr->isV6()
                                 && !addr->isLocal())))
                  break;
          Serial.print('.');
          delay(500);
      }

  waiting for an IPv6 global address, on a specific interface:

      for (bool configured = false; !configured; ) {
          for (auto addr: addrList)
              if ((configured = (   addr->isV6()
                                 && !addr->isLocal()
                                 && addr->ifnumber() == STATION_IF)))
                  break;
          Serial.print('.');
          delay(500);
      }
*/

#ifndef __IFLIST_H
#define __IFLIST_H

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
    // designed to be used with 'for (auto x: addrList)'

    public:

        class const_iterator {

            public:

                ////////////////////////////////////////////////
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

                ////////////////////////////////////////////////
                // (*iterator) emulation:

                const const_iterator& operator*  () const { return *this; }
                const const_iterator* operator-> () const { return  this; }

                // legacy members (IPv4 mask/gw)

                bool isLegacy() const { return _num == 0; }
                IPAddress netmask () const { return _netif->netmask; }
                IPAddress gw () const { return _netif->gw; }

                ////////////////////////////////////////////////
                // common to all addresses on current interface

                String ifname () const { return String(_netif->name[0]) + _netif->name[1]; }
                const char* ifhostname () const { return _netif->hostname?: ""; }
                const char* ifmac () const { return (const char*)_netif->hwaddr; }

                // this number is STATION_IF / SOFTAP_IF / ethernet...
                int ifnumber () const { return _netif->num; }

                ////////////////////////////////////////////////
                // access to the iterated address 

                IPAddress addr () const { return _ip_from_netif_num(); }

                ////////////////////////////////////////////////
                // bring IPAddress interface to simplify arduino code

                bool isV4() const { return addr().isV4(); }
                bool isV6() const { return addr().isV6(); }
                bool isLocal() const { return addr().isLocal(); }
                size_t printTo(Print& p) const { return addr().printTo(p); }

                ////////////////////////////////////////////////

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

         ////////////////////////////////////////////////
         // pseudo iterators: begin() and end()
         
        const const_iterator begin () const { return const_iterator(true);  }
        const const_iterator end   () const { return const_iterator(false); }
};

extern AddrListClass addrList;

#endif // __IFLIST_H
