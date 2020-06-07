/*
  UdpContext.h - emulation of UDP connection handling on top of lwIP

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

#include <functional>

class UdpContext;

#define GET_IP_HDR(pb) reinterpret_cast<ip_hdr*>(((uint8_t*)((pb)->payload)) - UDP_HLEN - IP_HLEN);
#define GET_UDP_HDR(pb) reinterpret_cast<udp_hdr*>(((uint8_t*)((pb)->payload)) - UDP_HLEN);

extern netif netif0;

class UdpContext
{
public:

    typedef std::function<void(void)> rxhandler_t;

    UdpContext(): _on_rx(nullptr), _refcnt(0)
    {
        _sock = mockUDPSocket();
    }

    ~UdpContext()
    {
    }

    void ref()
    {
        ++_refcnt;
    }

    void unref()
    {
        if(--_refcnt == 0) {
            delete this;
        }
    }

    bool connect (const ip_addr_t* addr, uint16_t port)
    {
        _dst = *addr;
        _dstport = port;
        return true;
    }

    bool listen(const ip_addr_t* addr, uint16_t port)
    {
        bool ret = mockUDPListen(_sock, addr->addr, port, staticMCastAddr);
        register_udp(_sock, this);
        return ret;
    }

    void disconnect()
    {
        if (_sock >= 0)
        {
            close(_sock);
            register_udp(_sock, nullptr);
        }
        _sock = -1;
    }

    void setMulticastInterface(const ip_addr_t& addr)
    {
        (void)addr;
        // user multicast, and this is how it works with posix: send to multicast address:
        _dst.addr = staticMCastAddr;
    }

    void setMulticastInterface(netif* p_pNetIf)
    {
        (void)p_pNetIf;
        // user multicast, and this is how it works with posix: send to multicast address:
        _dst.addr = staticMCastAddr;
    }

    void setMulticastTTL(int ttl)
    {
        (void)ttl;
        //mockverbose("TODO: UdpContext::setMulticastTTL\n");
    }

    netif* getInputNetif() const
    {
        return &netif0;
    }

    // warning: handler is called from tcp stack context
    // esp_yield and non-reentrant functions which depend on it will fail
    void onRx(rxhandler_t handler) {
        _on_rx = handler;
    }

    size_t getSize()
    {
        return _inbufsize;
    }

    size_t tell() const
    {
        return 0;
    }

    void seek(const size_t pos)
    {
        if (!isValidOffset(pos))
        {
            mockverbose("UDPContext::seek too far (%zd >= %zd)\n", pos, _inbufsize);
            exit(EXIT_FAILURE);
        }
        mockUDPSwallow(pos, _inbuf, _inbufsize);
    }

    bool isValidOffset(const size_t pos) const {
        return pos <= _inbufsize;
    }

    uint32_t getRemoteAddress()
    {
        return _dst.addr;
    }

    uint16_t getRemotePort()
    {
        return _dstport;
    }

    uint32_t getDestAddress()
    {
        mockverbose("TODO: implement UDP getDestAddress\n");
        return 0; //ip_hdr* iphdr = GET_IP_HDR(_rx_buf);
    }

    uint16_t getLocalPort()
    {
        mockverbose("TODO: implement UDP getLocalPort\n");
        return 0; //
    }

    bool next()
    {
        _inbufsize = 0;
        mockUDPFillInBuf(_sock, _inbuf, _inbufsize, addrsize, addr, _dstport);
        if (_inbufsize > 0)
        {
            translate_addr();
            return true;
        }
        return false;
    }

    int read()
    {
        char c;
        return read(&c, 1)? c: -1;
    }

    size_t read(char* dst, size_t size)
    {
        return mockUDPRead(_sock, dst, size, _timeout_ms, _inbuf, _inbufsize);
    }

    int peek()
    {
        char c;
        return mockUDPPeekBytes(_sock, &c, 1, _timeout_ms, _inbuf, _inbufsize)?: -1;
    }

    void flush()
    {
        //mockverbose("UdpContext::flush() does not follow arduino's flush concept\n");
        //exit(EXIT_FAILURE);
        // would be:
        _inbufsize = 0;
    }

    size_t append (const char* data, size_t size)
    {
        if (size + _outbufsize > sizeof _outbuf)
        {
            mockverbose("UdpContext::append: increase CCBUFSIZE (%d -> %zd)\n", CCBUFSIZE, (size + _outbufsize));
            exit(EXIT_FAILURE);
        }

        memcpy(_outbuf + _outbufsize, data, size);
        _outbufsize += size;
        return size;
    }

    bool send (ip_addr_t* addr = 0, uint16_t port = 0)
    {
    uint32_t dst = addr? addr->addr: _dst.addr;
    uint16_t dstport = port?: _dstport;
        size_t ret = mockUDPWrite(_sock, (const uint8_t*)_outbuf, _outbufsize, _timeout_ms, dst, dstport);
        _outbufsize = 0;
        return ret > 0;
    }

    void mock_cb (void)
    {
        if (_on_rx) _on_rx();
    }

public:

    static uint32_t staticMCastAddr;

private:

    void translate_addr ()
    {
        if (addrsize == 4)
        {
            uint32_t ipv4;
            memcpy(&ipv4, addr, 4);
            ip4_addr_set_u32(&ip_2_ip4(_dst), ipv4);
            // ^ this is a workaround for "type-punned pointer" with "*(uint32*)addr"
            //ip4_addr_set_u32(&ip_2_ip4(_dst), *(uint32_t*)addr);
        }
        else
            mockverbose("TODO unhandled udp address of size %d\n", (int)addrsize);
    }

    int _sock = -1;
    rxhandler_t _on_rx;
    int _refcnt = 0;

    ip_addr_t _dst;
    uint16_t _dstport;

    char _inbuf [CCBUFSIZE];
    size_t _inbufsize = 0;
    char _outbuf [CCBUFSIZE];
    size_t _outbufsize = 0;

    int _timeout_ms = 0;

    uint8_t addrsize;
    uint8_t addr[16];
};

extern "C" inline err_t igmp_joingroup (const ip4_addr_t *ifaddr, const ip4_addr_t *groupaddr)
{
    (void)ifaddr;
    UdpContext::staticMCastAddr = groupaddr->addr;
    return ERR_OK;
}

#endif//UDPCONTEXT_H
