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

#define GET_IP_HDR(pb) reinterpret_cast<ip_hdr*>(((uint8_t*)((pb)->payload)) - UDP_HLEN - IP_HLEN);
#define GET_UDP_HDR(pb) reinterpret_cast<udp_hdr*>(((uint8_t*)((pb)->payload)) - UDP_HLEN);

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

    bool connect(ip_addr_t addr, uint16_t port)
    {
        _dst = addr;
        _dstport = port;
        return true;
    }

    bool listen(ip_addr_t addr, uint16_t port)
    {
        return mockUDPListen(_sock, addr.addr, port);
    }

    void disconnect()
    {
        if (_sock >= 0)
            close(_sock);
        _sock = -1;
    }

    void setMulticastInterface(const ip_addr_t& addr)
    {
        fprintf(stderr, MOCK "TODO: UdpContext::setMulticastInterface\n");
    }

    void setMulticastTTL(int ttl)
    {
        fprintf(stderr, MOCK "TODO: UdpContext::setMulticastTTL\n");
    }

    // warning: handler is called from tcp stack context
    // esp_yield and non-reentrant functions which depend on it will fail
    void onRx(rxhandler_t handler) {
        _on_rx = handler;
    }

    size_t getSize()
    {
    	return _inbufsize?: mockFillInBuf(_sock, _inbuf, _inbufsize);
    }

    size_t tell() const
    {
        return 0;
    }

    void seek(const size_t pos)
    {
	fprintf(stderr, MOCK "TODO: implement UDP offset\n");
        if (!isValidOffset(pos))
        {
            fprintf(stderr, MOCK "UDPContext::seek too far (%d >= %d)\n", (int)pos, (int)_inbufsize);
            exit(EXIT_FAILURE);
        }
    }

    bool isValidOffset(const size_t pos) const {
        return pos <= _inbufsize;
    }

    uint32_t getRemoteAddress()
    {
	fprintf(stderr, MOCK "TODO: implement getRemoteAddress\n");
        return 0; //ip_hdr* iphdr = GET_IP_HDR(_rx_buf);
    }

    uint16_t getRemotePort()
    {
	fprintf(stderr, MOCK "TODO: implement UDP getRemotePort\n");
        return 0; //udp_hdr* udphdr = GET_UDP_HDR(_rx_buf);
    }

    uint32_t getDestAddress()
    {
	fprintf(stderr, MOCK "TODO: implement UDP getDestAddress\n");
        return 0; //ip_hdr* iphdr = GET_IP_HDR(_rx_buf);
    }

    uint16_t getLocalPort()
    {
	fprintf(stderr, MOCK "TODO: implement UDP getLocalPort\n");
    	return 0; //
    }

    bool next()
    {
	uint32_t a;
        _inbufsize = 0;
    	mockUDPFillInBuf(_sock, _inbuf, _inbufsize, &a);
    	return _inbufsize > 0;
    }

    int read()
    {
        char c;
        return read(&c, 1)? c: -1;
    }

    size_t read(char* dst, size_t size)
    {
	uint32_t a;
        size_t ret = mockUDPRead(_sock, dst, size, _timeout_ms, _inbuf, _inbufsize, (void*)&a);
        return ret;
    }

    int peek()
    {
        char c;
	uint32_t a;
        return mockUDPPeekBytes(_sock, &c, 1, _timeout_ms, _inbuf, _inbufsize, &a)? c: -1;
    }

    void flush()
    {
	fprintf(stderr, MOCK "UdpContext::flush() does not follow arduino's flush concept\n");
	exit(EXIT_FAILURE);
	// would be:
        _inbufsize = 0;
    }

    size_t append (const char* data, size_t size)
    {
    	if (size + _outbufsize > sizeof _outbuf)
    	{
    	    fprintf(stderr, MOCK "UdpContext::append: increase CCBUFSIZE (%d -> %d)\n", CCBUFSIZE, (int)(size + _outbufsize));
    	    exit(EXIT_FAILURE);
    	}
    	
    	memcpy(_outbuf + _outbufsize, data, size);
    	_outbufsize += size;
    	return size;
    }

    bool send (ip_addr_t* addr = 0, uint16_t port = 0)
    {
        uint32_t a = lwip_htonl(addr? addr->addr: 0);
    	size_t ret = mockUDPWrite(_sock, (const uint8_t*)_outbuf, _outbufsize, _timeout_ms, &a);
    	_outbufsize = 0;
    	return ret > 0;
    }

private:
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
};

#endif//UDPCONTEXT_H
