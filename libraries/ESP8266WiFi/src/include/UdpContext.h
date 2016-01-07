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

extern "C" void esp_yield();
extern "C" void esp_schedule();

#define GET_IP_HDR(pb) reinterpret_cast<ip_hdr*>(((uint8_t*)((pb)->payload)) - UDP_HLEN - IP_HLEN);
#define GET_UDP_HDR(pb) reinterpret_cast<udp_hdr*>(((uint8_t*)((pb)->payload)) - UDP_HLEN);

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
    , _multicast_ttl(1)
    , _dest_port(0)
    {
        _pcb = udp_new();
        _dest_addr.addr = 0;
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
        if(this != 0) {
            DEBUGV(":ur %d\r\n", _refcnt);
            if(--_refcnt == 0) {
                delete this;
            }
        }
    }

    bool connect(ip_addr_t addr, uint16_t port)
    {
        _dest_addr = addr;
        _dest_port = port;
        return true;
    }

    bool listen(ip_addr_t addr, uint16_t port)
    {
        udp_recv(_pcb, &_s_recv, (void *) this);
        err_t err = udp_bind(_pcb, &addr, port);
        return err == ERR_OK;
    }

    void disconnect()
    {
        udp_disconnect(_pcb);
    }

    void setMulticastInterface(ip_addr_t addr)
    {
        // newer versions of lwip have a macro to set the multicast ip
        // udp_set_multicast_netif_addr(_pcb, addr);
        _pcb->multicast_ip = addr;
    }

    void setMulticastTTL(int ttl)
    {
        // newer versions of lwip have an additional field (mcast_ttl) for this purpose
        // and a macro to set it instead of direct field access
        // udp_set_multicast_ttl(_pcb, ttl);
        _multicast_ttl = ttl;
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

        return _rx_buf->len - _rx_buf_offset;
    }

    uint32_t getRemoteAddress()
    {
        if (!_rx_buf)
            return 0;

        ip_hdr* iphdr = GET_IP_HDR(_rx_buf);
        return iphdr->src.addr;
    }

    uint16_t getRemotePort()
    {
        if (!_rx_buf)
            return 0;

        udp_hdr* udphdr = GET_UDP_HDR(_rx_buf);
        return ntohs(udphdr->src);
    }

    uint32_t getDestAddress()
    {
        ip_hdr* iphdr = GET_IP_HDR(_rx_buf);
        return iphdr->dest.addr;
    }

    uint16_t getLocalPort()
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

        auto head = _rx_buf;
        _rx_buf = _rx_buf->next;
        _rx_buf_offset = 0;

        if (_rx_buf)
        {
            pbuf_ref(_rx_buf);
        }
        pbuf_free(head);
        return _rx_buf != 0;
    }

    int read()
    {
        if (!_rx_buf || _rx_buf->len == _rx_buf_offset)
            return -1;

        char c = reinterpret_cast<char*>(_rx_buf->payload)[_rx_buf_offset];
        _consume(1);
        return c;
    }

    size_t read(char* dst, size_t size)
    {
        if (!_rx_buf)
            return 0;

        size_t max_size = _rx_buf->len - _rx_buf_offset;
        size = (size < max_size) ? size : max_size;
        DEBUGV(":urd %d, %d, %d\r\n", size, _rx_buf->len, _rx_buf_offset);

        memcpy(dst, reinterpret_cast<char*>(_rx_buf->payload) + _rx_buf_offset, size);
        _consume(size);

        return size;
    }

    char peek()
    {
        if (!_rx_buf)
            return 0;

        return reinterpret_cast<char*>(_rx_buf->payload)[_rx_buf_offset];
    }

    void flush()
    {
        if (!_rx_buf)
            return;

        _consume(_rx_buf->len - _rx_buf_offset);
    }


    size_t append(const char* data, size_t size)
    {
        if (!_tx_buf_head || _tx_buf_head->tot_len < _tx_buf_offset + size)
        {
            _reserve(_tx_buf_offset + size);
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

    void send(ip_addr_t* addr = 0, uint16_t port = 0)
    {
        size_t data_size = _tx_buf_offset;
        pbuf* tx_copy = pbuf_alloc(PBUF_TRANSPORT, data_size, PBUF_RAM);
        uint8_t* dst = reinterpret_cast<uint8_t*>(tx_copy->payload);
        for (pbuf* p = _tx_buf_head; p; p = p->next) {
            size_t will_copy = (data_size < p->len) ? data_size : p->len;
            memcpy(dst, p->payload, will_copy);
            dst += will_copy;
            data_size -= will_copy;
        }
        pbuf_free(_tx_buf_head);
        _tx_buf_head = 0;
        _tx_buf_cur = 0;
        _tx_buf_offset = 0;


        if (!addr) {
            addr = &_dest_addr;
            port = _dest_port;
        }

        uint16_t old_ttl = _pcb->ttl;
        if (ip_addr_ismulticast(addr)) {
            _pcb->ttl = _multicast_ttl;
        }

        udp_sendto(_pcb, tx_copy, addr, port);
        _pcb->ttl = old_ttl;
        pbuf_free(tx_copy);
    }

private:

    void _reserve(size_t size)
    {
        const size_t pbuf_unit_size = 128;
        if (!_tx_buf_head)
        {
            _tx_buf_head = pbuf_alloc(PBUF_TRANSPORT, pbuf_unit_size, PBUF_RAM);
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
            pbuf_cat(_tx_buf_head, pb);
            if (grow_size < pbuf_unit_size)
                return;
            grow_size -= pbuf_unit_size;
        }
    }

    void _consume(size_t size)
    {
        _rx_buf_offset += size;
    }

    void _recv(udp_pcb *upcb, pbuf *pb,
            ip_addr_t *addr, u16_t port)
    {
        if (_rx_buf)
        {
            // there is some unread data
            // chain the new pbuf to the existing one
            DEBUGV(":urch %d, %d\r\n", _rx_buf->tot_len, pb->tot_len);
            pbuf_cat(_rx_buf, pb);
        }
        else
        {
            DEBUGV(":urn %d\r\n", pb->tot_len);
            _first_buf_taken = false;
            _rx_buf = pb;
            _rx_buf_offset = 0;
        }
        if (_on_rx) {
            _on_rx();
        }
    }


    static void _s_recv(void *arg,
            udp_pcb *upcb, pbuf *p,
            ip_addr_t *addr, u16_t port)
    {
        reinterpret_cast<UdpContext*>(arg)->_recv(upcb, p, addr, port);
    }

private:
    udp_pcb* _pcb;
    pbuf* _rx_buf;
    bool _first_buf_taken;
    size_t _rx_buf_offset;
    int _refcnt;
    pbuf* _tx_buf_head;
    pbuf* _tx_buf_cur;
    size_t _tx_buf_offset;
    uint16_t _multicast_ttl;
    uint16_t _dest_port;
    ip_addr_t _dest_addr;
    rxhandler_t _on_rx;
};



#endif//CLIENTCONTEXT_H
