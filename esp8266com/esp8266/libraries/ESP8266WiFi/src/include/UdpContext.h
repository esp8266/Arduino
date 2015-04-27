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
        if (--_refcnt == 0)
        {
            delete this;
        }
    }

    bool connect(ip_addr_t addr, uint16_t port)
    {
        err_t err = udp_connect(_pcb, &addr, port);
        return err == ERR_OK;
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
        _pcb->ttl = ttl;
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

    char read()
    {
        if (!_rx_buf || _rx_buf->len == _rx_buf_offset)
            return 0;

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
        DEBUGV(":rd %d, %d, %d\r\n", size, _rx_buf->len, _rx_buf_offset);
        
        os_memcpy(dst, reinterpret_cast<char*>(_rx_buf->payload) + _rx_buf_offset, size);        
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
            os_memcpy(reinterpret_cast<char*>(_tx_buf_cur->payload) + used_cur, data, will_copy);
            _tx_buf_offset += will_copy;
            left_to_copy -= will_copy;
            data += will_copy;
        }
        return size;
    }

    void send(ip_addr_t* addr = 0, uint16_t port = 0)
    {
        size_t orig_size = _tx_buf_head->tot_len;

        size_t data_size = _tx_buf_offset;
        size_t size_adjustment = orig_size - data_size;
        for (pbuf* p = _tx_buf_head; p; p = p->next)
        {
            p->tot_len -= size_adjustment;
            if (!p->next)
            {
                p->len = p->tot_len;
            }
        }

        if (addr)
            udp_sendto(_pcb, _tx_buf_head, addr, port);
        else
            udp_send(_pcb, _tx_buf_head);

        for (pbuf* p = _tx_buf_head; p; p = p->next)
        {
            p->tot_len += size_adjustment;
            if (!p->next)
            {
                p->len = p->tot_len;
            }
        }

        pbuf_free(_tx_buf_head);
        _tx_buf_head = 0;
        _tx_buf_cur = 0;
        _tx_buf_offset = 0;
    }

private:

    void _reserve(size_t size)
    {
        const size_t pbuf_unit_size = 1024;
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
            DEBUGV(":rch %d, %d\r\n", _rx_buf->tot_len, pb->tot_len);
            pbuf_cat(_rx_buf, pb);
        }
        else
        {
            DEBUGV(":rn %d\r\n", pb->tot_len);
            _first_buf_taken = false;
            _rx_buf = pb;
            _rx_buf_offset = 0;
        }
    }


    static void _s_recv(void *arg, 
            udp_pcb *upcb, pbuf *p,
            ip_addr_t *addr, u16_t port)
    {
        reinterpret_cast<UdpContext*>(arg)->_recv(upcb, p, addr, port);
    }

private:
    int _refcnt;
    udp_pcb* _pcb;

    bool _first_buf_taken;
    pbuf* _rx_buf;
    size_t _rx_buf_offset;

    pbuf* _tx_buf_head;
    pbuf* _tx_buf_cur;
    size_t _tx_buf_offset;
};



#endif//CLIENTCONTEXT_H
