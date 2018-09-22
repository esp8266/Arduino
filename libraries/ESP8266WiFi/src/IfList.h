/*
 IfList.h - cycle through lwIP netif's ip addresses like a c++ list
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

  for (const auto& a : ifList)
  {
    out.printf("IF='%s' IPv6=%d routable=%d hostname='%s' addr= ",
               a->iface().c_str(),
               a->addr().isV4(),
               a->addr().isRoutable(),
               a->hostname().c_str());
    a->addr().printTo(out);
    out.println();
  }
*/

#ifndef __IFLIST_H
#define __IFLIST_H

#include <IPAddress.h>
#include <lwip/netif.h>

class IfListClass {

    public:

        class const_iterator {

            public:

                const_iterator (bool begin = true): _if(begin? netif_list: nullptr), _num(-1) { }
                const_iterator (const const_iterator& o): _if(o._if), _num(o._num) { }
                const_iterator& operator= (const const_iterator& o) { _if = o._if; _num = o._num; return *this; }

                bool operator!= (const const_iterator& o) { return !equal(o); }
                bool operator== (const const_iterator& o) { return  equal(o); }

                const const_iterator& operator*  () const { return *this; }
                const const_iterator* operator-> () const { return  this; }

                const_iterator operator++(int) {
                    const_iterator ret = *this;
                    ++(*this);
                    return ret;
                }

		const_iterator& operator++() {
                    while (_if) {
                        if (++_num == LWIP_IPV6_NUM_ADDRESSES + 1) {
                            _num = -1;
                            _if = _if->next;
                            continue;
                        }
                        if (!ip_addr_isany(_ip_from_if_num()))
                            break;
                    }
                    return *this;
                }

                // (*iterator):

                bool isLegacy() const { return _num == 0; }
                IPAddress addr () const { return _ip_from_if_num(); }
                IPAddress netmask () const { return _if->netmask; }
                IPAddress gw () const { return _if->gw; }
                String iface () const { return String(_if->name[0]) + _if->name[1]; }
                String hostname () const { return _if->hostname?: ""; }

            protected:

                bool equal (const const_iterator& o) { return _if == o._if && (!_if || _num == o._num); }
                const ip_addr_t* _ip_from_if_num () const {
#if LWIP_IPV6
                    return _num? &_if->ip6_addr[_num - 1]: &_if->ip_addr;
#else
                    return &_if->ip_addr;
#endif
                }

                netif* _if;
                int _num;
        };

        const const_iterator begin () const { return const_iterator(true);  }
        const const_iterator end   () const { return const_iterator(false); }
};

extern IfListClass ifList;

#endif // __IFLIST_H
