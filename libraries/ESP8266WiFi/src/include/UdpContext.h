/*
  UdpContext.h - UDP connection handling on top of lwIP

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.

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
#ifndef UDPCONTEXT_H
#define UDPCONTEXT_H

class UdpContext;

extern "C" {
void esp_yield();
void esp_schedule();
#include "lwip/init.h" // LWIP_VERSION_
#include <assert.h>
}

#include <AddrList.h>

#define PBUF_ALIGNER_ADJUST 4
#define PBUF_ALIGNER(x) ((void*)((((intptr_t)(x))+3)&~3))
#define PBUF_HELPER_FLAG 0xff // lwIP pbuf flag: u8_t

class UdpContext
{
public:

    typedef std::function<void(void)> rxhandler_t;

    UdpContext()
    : _pcb(0)
    , _rx_buf(0)
    , _first_buf_taken(false)
    , _rx_buf_offset(0)
    , _refcnt(0)
    , _tx_buf_head(0)
    , _tx_buf_cur(0)
    , _tx_buf_offset(0)
    {
        _pcb = udp_new();
#ifdef LWIP_MAYBE_XCC
        _mcast_ttl = 1;
#endif
    }

    ~UdpContext()
    {
        udp_remove(_pcb);
        _pcb = 0;
        if (_tx_buf_head)
        {
            pbuf_free(_tx_buf_head);
            _tx_buf_head = 0;
            _tx_buf_cur = 0;
            _tx_buf_offset = 0;
        }
        if (_rx_buf)
        {
            pbuf_free(_rx_buf);
            _rx_buf = 0;
            _rx_buf_offset = 0;
        }
    }

    void ref()
    {
        ++_refcnt;
    }

    void unref()
    {
        DEBUGV(":ur %d\r\n", _refcnt);
        if(--_refcnt == 0) {
            delete this;
        }
    }

#if LWIP_VERSION_MAJOR == 1

    bool connect(IPAddress addr, uint16_t port)
    {
        _pcb->remote_ip = addr;
        _pcb->remote_port = port;
        return true;
    }

    bool listen(IPAddress addr, uint16_t port)
    {
        udp_recv(_pcb, &_s_recv, (void *) this);
        err_t err = udp_bind(_pcb, addr, port);
        return err == ERR_OK;
    }

#else // lwIP-v2

    bool connect(const IPAddress& addr, uint16_t port)
    {
        _pcb->remote_ip = addr;
        _pcb->remote_port = port;
        return true;
    }

    bool listen(const IPAddress& addr, uint16_t port)
    {
        udp_recv(_pcb, &_s_recv, (void *) this);
        err_t err = udp_bind(_pcb, addr, port);
        return err == ERR_OK;
    }

#endif // lwIP-v2

    void disconnect()
    {
        udp_disconnect(_pcb);
    }

#if LWIP_IPV6

    void setMulticastInterface(IPAddress addr)
    {
        // Per 'udp_set_multicast_netif_addr()' signature and comments
        // in lwIP sources:
        // An IPv4 address designating a specific interface must be used.
        // When an IPv6 address is given, the matching IPv4 in the same
        // interface must be selected.

        if (!addr.isV4())
        {
            for (auto a: addrList)
                if (a.addr() == addr)
                {
                    // found the IPv6 address,
                    // redirect parameter to IPv4 address in this interface
                    addr = a.ipv4();
                    break;
                }
            assert(addr.isV4());
        }
        udp_set_multicast_netif_addr(_pcb, ip_2_ip4((const ip_addr_t*)addr));
    }

#else // !LWIP_IPV6

    void setMulticastInterface(const IPAddress& addr)
    {
#if LWIP_VERSION_MAJOR == 1
        udp_set_multicast_netif_addr(_pcb, (ip_addr_t)addr);
#else
        udp_set_multicast_netif_addr(_pcb, ip_2_ip4((const ip_addr_t*)addr));
#endif
    }

#endif // !LWIP_IPV6

    /*
     * Add a netif (by its index) as the multicast interface
     */
    void setMulticastInterface(netif* p_pNetIf)
    {
#if LWIP_VERSION_MAJOR == 1
        udp_set_multicast_netif_addr(_pcb, (p_pNetIf ? p_pNetIf->ip_addr : ip_addr_any));
#else
        udp_set_multicast_netif_index(_pcb, (p_pNetIf ? netif_get_index(p_pNetIf) : NETIF_NO_INDEX));
#endif
    }

    /*
     * Allow access to pcb to change eg. options
     */
    udp_pcb* pcb(void)
    {
        return _pcb;
    }

    void setMulticastTTL(int ttl)
    {
#ifdef LWIP_MAYBE_XCC
        _mcast_ttl = ttl;
#else
        udp_set_multicast_ttl(_pcb, ttl);
#endif
    }

    // warning: handler is called from tcp stack context
    // esp_yield and non-reentrant functions which depend on it will fail
    void onRx(rxhandler_t handler) {
        _on_rx = handler;
    }

    size_t getSize() const
    {
        if (!_rx_buf)
            return 0;

        return _rx_buf->tot_len - _rx_buf_offset;
    }

    size_t tell() const
    {
        return _rx_buf_offset;
    }

    void seek(const size_t pos)
    {
        assert(isValidOffset(pos));
        _rx_buf_offset = pos;
    }

    bool isValidOffset(const size_t pos) const {
        return (pos <= _rx_buf->tot_len);
    }

    netif* getInputNetif() const
    {
        return _currentAddr.input_netif;
    }

    CONST IPAddress& getRemoteAddress() CONST
    {
        return _currentAddr.srcaddr;
    }

    uint16_t getRemotePort() const
    {
        return _currentAddr.srcport;
    }

    const IPAddress& getDestAddress() const
    {
        return _currentAddr.dstaddr;
    }

    uint16_t getLocalPort() const
    {
        if (!_pcb)
            return 0;
        return _pcb->local_port;
    }

    bool next()
    {
        if (!_rx_buf)
            return false;
        if (!_first_buf_taken)
        {
            _first_buf_taken = true;
            return true;
        }

        auto deleteme = _rx_buf;

        while(_rx_buf->len != _rx_buf->tot_len)
            _rx_buf = _rx_buf->next;

        _rx_buf = _rx_buf->next;

        if (_rx_buf)
        {
            if (_rx_buf->flags == PBUF_HELPER_FLAG)
            {
                // we have interleaved informations on addresses within reception pbuf chain:
                // before: (data-pbuf) -> (data-pbuf) -> (data-pbuf) -> ... in the receiving order
                // now: (address-info-pbuf -> data-pbuf) -> (address-info-pbuf -> data-pbuf) -> ...

                // so the first rx_buf contains an address helper,
                // copy it to "current address"
                auto helper = (AddrHelper*)PBUF_ALIGNER(_rx_buf->payload);
                _currentAddr = *helper;

                // destroy the helper in the about-to-be-released pbuf
                helper->~AddrHelper();

                // forward in rx_buf list, next one is effective data
                // current (not ref'ed) one will be pbuf_free'd with deleteme
                _rx_buf = _rx_buf->next;
            }

            // this rx_buf is not nullptr by construction,
            // ref'ing it to prevent release from the below pbuf_free(deleteme)
            pbuf_ref(_rx_buf);
        }
        pbuf_free(deleteme);

        _rx_buf_offset = 0;
        return _rx_buf != nullptr;
    }

    int read()
    {
        if (!_rx_buf || _rx_buf_offset >= _rx_buf->tot_len)
            return -1;

        char c = pbuf_get_at(_rx_buf, _rx_buf_offset);
        _consume(1);
        return c;
    }

    size_t read(char* dst, size_t size)
    {
        if (!_rx_buf)
            return 0;

        size_t max_size = _rx_buf->tot_len - _rx_buf_offset;
        size = (size < max_size) ? size : max_size;
        DEBUGV(":urd %d, %d, %d\r\n", size, _rx_buf->tot_len, _rx_buf_offset);

        void* buf = pbuf_get_contiguous(_rx_buf, dst, size, size, _rx_buf_offset);
        if(!buf)
            return 0;

        if(buf != dst)
            memcpy(dst, buf, size);

        _consume(size);

        return size;
    }

    int peek() const
    {
        if (!_rx_buf || _rx_buf_offset == _rx_buf->tot_len)
            return -1;

        return pbuf_get_at(_rx_buf, _rx_buf_offset);
    }

    void flush()
    {
        //XXX this does not follow Arduino's flush definition
        if (!_rx_buf)
            return;

        _consume(_rx_buf->tot_len - _rx_buf_offset);
    }

    size_t append(const char* data, size_t size)
    {
        if (!_tx_buf_head || _tx_buf_head->tot_len < _tx_buf_offset + size)
        {
            _reserve(_tx_buf_offset + size);
        }
        if (!_tx_buf_head || _tx_buf_head->tot_len < _tx_buf_offset + size)
        {
            DEBUGV("failed _reserve");
            return 0;
        }

        size_t left_to_copy = size;
        while(left_to_copy)
        {
            // size already used in current pbuf
            size_t used_cur = _tx_buf_offset - (_tx_buf_head->tot_len - _tx_buf_cur->tot_len);
            size_t free_cur = _tx_buf_cur->len - used_cur;
            if (free_cur == 0)
            {
                _tx_buf_cur = _tx_buf_cur->next;
                continue;
            }
            size_t will_copy = (left_to_copy < free_cur) ? left_to_copy : free_cur;
            memcpy(reinterpret_cast<char*>(_tx_buf_cur->payload) + used_cur, data, will_copy);
            _tx_buf_offset += will_copy;
            left_to_copy -= will_copy;
            data += will_copy;
        }
        return size;
    }

    bool send(CONST ip_addr_t* addr = 0, uint16_t port = 0)
    {
        size_t data_size = _tx_buf_offset;
        pbuf* tx_copy = pbuf_alloc(PBUF_TRANSPORT, data_size, PBUF_RAM);
        if(!tx_copy){
            DEBUGV("failed pbuf_alloc");
        }
        else{
            uint8_t* dst = reinterpret_cast<uint8_t*>(tx_copy->payload);
            for (pbuf* p = _tx_buf_head; p; p = p->next) {
                size_t will_copy = (data_size < p->len) ? data_size : p->len;
                memcpy(dst, p->payload, will_copy);
                dst += will_copy;
                data_size -= will_copy;
            }
        }
        if (_tx_buf_head)
            pbuf_free(_tx_buf_head);
        _tx_buf_head = 0;
        _tx_buf_cur = 0;
        _tx_buf_offset = 0;
        if(!tx_copy){
            return false;
        }


        if (!addr) {
            addr = &_pcb->remote_ip;
            port = _pcb->remote_port;
        }
#ifdef LWIP_MAYBE_XCC
        uint16_t old_ttl = _pcb->ttl;
        if (ip_addr_ismulticast(addr)) {
            _pcb->ttl = _mcast_ttl;
        }
#endif
        err_t err = udp_sendto(_pcb, tx_copy, addr, port);
        if (err != ERR_OK) {
            DEBUGV(":ust rc=%d\r\n", (int) err);
        }
#ifdef LWIP_MAYBE_XCC
        _pcb->ttl = old_ttl;
#endif
        pbuf_free(tx_copy);
        return err == ERR_OK;
    }

private:

    void _reserve(size_t size)
    {
        const size_t pbuf_unit_size = 128;
        if (!_tx_buf_head)
        {
            _tx_buf_head = pbuf_alloc(PBUF_TRANSPORT, pbuf_unit_size, PBUF_RAM);
            if (!_tx_buf_head)
            {
                return;
            }
            _tx_buf_cur = _tx_buf_head;
            _tx_buf_offset = 0;
        }

        size_t cur_size = _tx_buf_head->tot_len;
        if (size < cur_size)
            return;

        size_t grow_size = size - cur_size;

        while(grow_size)
        {
            pbuf* pb = pbuf_alloc(PBUF_TRANSPORT, pbuf_unit_size, PBUF_RAM);
            if (!pb)
            {
                return;
            }
            pbuf_cat(_tx_buf_head, pb);
            if (grow_size < pbuf_unit_size)
                return;
            grow_size -= pbuf_unit_size;
        }
    }

    void _consume(size_t size)
    {
        _rx_buf_offset += size;
        if (_rx_buf_offset > _rx_buf->tot_len) {
            _rx_buf_offset = _rx_buf->tot_len;
        }
    }

    void _recv(udp_pcb *upcb, pbuf *pb,
            const ip_addr_t *srcaddr, u16_t srcport)
    {
        (void) upcb;
        // check receive pbuf chain depth
        {
            pbuf* p;
            int count = 0;
            for (p = _rx_buf; p && ++count < rxBufMaxDepth*2; p = p->next);
            if (p)
            {
                // pbuf chain too deep, dropping
                pbuf_free(pb);
                DEBUGV(":udr\r\n");
                return;
            }
        }
#if LWIP_VERSION_MAJOR == 1
    #define TEMPDSTADDR (&current_iphdr_dest)
    #define TEMPINPUTNETIF (current_netif)
#else
    #define TEMPDSTADDR (ip_current_dest_addr())
    #define TEMPINPUTNETIF (ip_current_input_netif())
#endif

        // chain this helper pbuf first
        if (_rx_buf)
        {
            // there is some unread data
            // chain pbuf

            // Addresses/ports are stored from this callback because lwIP's
            // macro are valid only now.
            //
            // When peeking data from before payload start (like it was done
            // before IPv6), there's no easy way to safely guess whether
            // packet is from v4 or v6.
            //
            // Now storing data in an intermediate chained pbuf containing
            // AddrHelper

            // allocate new pbuf to store addresses/ports
            pbuf* pb_helper = pbuf_alloc(PBUF_RAW, sizeof(AddrHelper) + PBUF_ALIGNER_ADJUST, PBUF_RAM);
            if (!pb_helper)
            {
                // memory issue - discard received data
                pbuf_free(pb);
                return;
            }
            // construct in place
            new(PBUF_ALIGNER(pb_helper->payload)) AddrHelper(srcaddr, TEMPDSTADDR, srcport, TEMPINPUTNETIF);
            pb_helper->flags = PBUF_HELPER_FLAG; // mark helper pbuf
            // chain it
            pbuf_cat(_rx_buf, pb_helper);

            // now chain the new data pbuf
            DEBUGV(":urch %d, %d\r\n", _rx_buf->tot_len, pb->tot_len);
            pbuf_cat(_rx_buf, pb);
        }
        else
        {
            _currentAddr.srcaddr = srcaddr;
            _currentAddr.dstaddr = TEMPDSTADDR;
            _currentAddr.srcport = srcport;
            _currentAddr.input_netif = TEMPINPUTNETIF;

            DEBUGV(":urn %d\r\n", pb->tot_len);
            _first_buf_taken = false;
            _rx_buf = pb;
            _rx_buf_offset = 0;
        }

        if (_on_rx) {
            _on_rx();
        }

    #undef TEMPDSTADDR
    #undef TEMPINPUTNETIF

    }

    static void _s_recv(void *arg,
            udp_pcb *upcb, pbuf *p,
            CONST ip_addr_t *srcaddr, u16_t srcport)
    {
        reinterpret_cast<UdpContext*>(arg)->_recv(upcb, p, srcaddr, srcport);
    }

#if LWIP_VERSION_MAJOR == 1
    /*
     * Code in this conditional block is copied/backported verbatim from
     * LwIP 2.1.2 to provide pbuf_get_contiguous.
     */

    static const struct pbuf *
    pbuf_skip_const(const struct pbuf *in, u16_t in_offset, u16_t *out_offset)
    {
      u16_t offset_left = in_offset;
      const struct pbuf *pbuf_it = in;

      /* get the correct pbuf */
      while ((pbuf_it != NULL) && (pbuf_it->len <= offset_left)) {
        offset_left = (u16_t)(offset_left - pbuf_it->len);
        pbuf_it = pbuf_it->next;
      }
      if (out_offset != NULL) {
        *out_offset = offset_left;
      }
      return pbuf_it;
    }

    u16_t
    pbuf_copy_partial(const struct pbuf *buf, void *dataptr, u16_t len, u16_t offset)
    {
      const struct pbuf *p;
      u16_t left = 0;
      u16_t buf_copy_len;
      u16_t copied_total = 0;

      LWIP_ERROR("pbuf_copy_partial: invalid buf", (buf != NULL), return 0;);
      LWIP_ERROR("pbuf_copy_partial: invalid dataptr", (dataptr != NULL), return 0;);

      /* Note some systems use byte copy if dataptr or one of the pbuf payload pointers are unaligned. */
      for (p = buf; len != 0 && p != NULL; p = p->next) {
        if ((offset != 0) && (offset >= p->len)) {
          /* don't copy from this buffer -> on to the next */
          offset = (u16_t)(offset - p->len);
        } else {
          /* copy from this buffer. maybe only partially. */
          buf_copy_len = (u16_t)(p->len - offset);
          if (buf_copy_len > len) {
            buf_copy_len = len;
          }
          /* copy the necessary parts of the buffer */
          MEMCPY(&((char *)dataptr)[left], &((char *)p->payload)[offset], buf_copy_len);
          copied_total = (u16_t)(copied_total + buf_copy_len);
          left = (u16_t)(left + buf_copy_len);
          len = (u16_t)(len - buf_copy_len);
          offset = 0;
        }
      }
      return copied_total;
    }

    void *
    pbuf_get_contiguous(const struct pbuf *p, void *buffer, size_t bufsize, u16_t len, u16_t offset)
    {
      const struct pbuf *q;
      u16_t out_offset;

      LWIP_ERROR("pbuf_get_contiguous: invalid buf", (p != NULL), return NULL;);
      LWIP_ERROR("pbuf_get_contiguous: invalid dataptr", (buffer != NULL), return NULL;);
      LWIP_ERROR("pbuf_get_contiguous: invalid dataptr", (bufsize >= len), return NULL;);

      q = pbuf_skip_const(p, offset, &out_offset);
      if (q != NULL) {
        if (q->len >= (out_offset + len)) {
          /* all data in this pbuf, return zero-copy */
          return (u8_t *)q->payload + out_offset;
        }
        /* need to copy */
        if (pbuf_copy_partial(q, buffer, len, out_offset) != len) {
          /* copying failed: pbuf is too short */
          return NULL;
        }
        return buffer;
      }
      /* pbuf is too short (offset does not fit in) */
      return NULL;
    }
#endif

private:
    udp_pcb* _pcb;
    pbuf* _rx_buf;
    bool _first_buf_taken;
    size_t _rx_buf_offset;
    int _refcnt;
    pbuf* _tx_buf_head;
    pbuf* _tx_buf_cur;
    size_t _tx_buf_offset;
    rxhandler_t _on_rx;
#ifdef LWIP_MAYBE_XCC
    uint16_t _mcast_ttl;
#endif
    struct AddrHelper
    {
        IPAddress srcaddr, dstaddr;
        int16_t srcport;
        netif* input_netif;

        AddrHelper() { }
        AddrHelper(const ip_addr_t* src, const ip_addr_t* dst, uint16_t srcport, netif* input_netif):
            srcaddr(src), dstaddr(dst), srcport(srcport), input_netif(input_netif) { }
    };
    AddrHelper _currentAddr;

    // rx pbuf depth barrier (counter of buffered UDP received packets)
    // keep it small
    static constexpr int rxBufMaxDepth = 4;
};



#endif//UDPCONTEXT_H
