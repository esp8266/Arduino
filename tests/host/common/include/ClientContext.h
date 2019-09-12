/*
 ClientContext.h - emulation of TCP connection handling on top of lwIP

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
#ifndef CLIENTCONTEXT_H
#define CLIENTCONTEXT_H

class ClientContext;
class WiFiClient;

extern "C" void esp_yield();
extern "C" void esp_schedule();

#include <include/DataSource.h>

bool getDefaultPrivateGlobalSyncValue ();

typedef void (*discard_cb_t)(void*, ClientContext*);

class ClientContext
{
public:
    ClientContext(tcp_pcb* pcb, discard_cb_t discard_cb, void* discard_cb_arg) :
        _discard_cb(discard_cb), _discard_cb_arg(discard_cb_arg), _refcnt(0), _next(0),
        _sync(::getDefaultPrivateGlobalSyncValue()), _sock(-1)
    {
        (void)pcb;
    }
    
    ClientContext (int sock) :
        _discard_cb(nullptr), _discard_cb_arg(nullptr), _refcnt(0), _next(nullptr),
        _sync(::getDefaultPrivateGlobalSyncValue()), _sock(sock)
    {
    }
    
    err_t abort()
    {
        if (_sock >= 0)
        {
            ::close(_sock);
	    mockverbose("socket %d closed\n", _sock);
        }
        _sock = -1;
        return ERR_ABRT;
    }

    err_t close()
    {
        abort();
        return ERR_OK;
    }

    ~ClientContext()
    {
        abort();
    }

    ClientContext* next() const
    {
        return _next;
    }

    ClientContext* next(ClientContext* new_next)
    {
        _next = new_next;
        return _next;
    }

    void ref()
    {
        ++_refcnt;
        DEBUGV(":ref %d\r\n", _refcnt);
    }

    void unref()
    {
        DEBUGV(":ur %d\r\n", _refcnt);
        if(--_refcnt == 0) {
            discard_received();
            close();
            if (_discard_cb)
                 _discard_cb(_discard_cb_arg, this);
            DEBUGV(":del\r\n");
            delete this;
        }
    }

    int connect(const ip_addr_t* addr, uint16_t port)
    {
        return mockConnect(addr->addr, _sock, port);
    }

    size_t availableForWrite()
    {
        // XXXFIXME be smarter
        return 512;
    }

    void setNoDelay(bool nodelay)
    {
        mockverbose("TODO setNoDelay(%d)\n", (int)nodelay);
    }

    bool getNoDelay() const
    {
        mockverbose("TODO getNoDelay()\n");
        return false;
    }

    void setTimeout(int timeout_ms)
    {
        _timeout_ms = timeout_ms;
    }

    int getTimeout() const
    {
        return _timeout_ms;
    }

    uint32_t getRemoteAddress() const
    {
        mockverbose("TODO getRemoteAddress()\n");
        return 0;
    }

    uint16_t getRemotePort() const
    {
        mockverbose("TODO getRemotePort()\n");
        return 0;
    }

    uint32_t getLocalAddress() const
    {
        mockverbose("TODO getLocalAddress()\n");
        return 0;
    }

    uint16_t getLocalPort() const
    {
        mockverbose("TODO getLocalPort()\n");
        return 0;
    }

    size_t getSize()
    {
        if (_sock < 0)
            return 0;
        if (_inbufsize)
            return _inbufsize;
        ssize_t ret = mockFillInBuf(_sock, _inbuf, _inbufsize);
        if (ret < 0)
        {
            abort();
            return 0;
        }
        return ret;
    }

    int read()
    {
        char c;
        return read(&c, 1)? (unsigned char)c: -1;
    }

    size_t read (char* dst, size_t size)
    {
        ssize_t ret = mockRead(_sock, dst, size, 0, _inbuf, _inbufsize);
        if (ret < 0)
        {
            abort();
            return 0;
        }
        return ret;
    }

    int peek()
    {
        char c;
        return peekBytes(&c, 1)? c: -1;
    }

    size_t peekBytes(char *dst, size_t size)
    {
        ssize_t ret = mockPeekBytes(_sock, dst, size, _timeout_ms, _inbuf, _inbufsize);
        if (ret < 0)
        {
            abort();
            return 0;
        }
        return ret;
    }

    void discard_received()
    {
        mockverbose("TODO: ClientContext::discard_received()\n");
    }

    bool wait_until_sent(int max_wait_ms = WIFICLIENT_MAX_FLUSH_WAIT_MS)
    {
        (void)max_wait_ms;
        return true;
    }

    uint8_t state()
    {
	(void)getSize(); // read on socket to force detect closed peer
        return _sock >= 0? ESTABLISHED: CLOSED;
    }

    size_t write(const uint8_t* data, size_t size)
    {
	ssize_t ret = mockWrite(_sock, data, size, _timeout_ms);
	if (ret < 0)
	{
	    abort();
	    return 0;
	}
	return ret;
    }

    size_t write(Stream& stream)
    {
        size_t avail = stream.available();
        uint8_t buf [avail];
        avail = stream.readBytes(buf, avail);
        size_t totwrote = 0;
        uint8_t* w = buf;
        while (avail && _sock >= 0)
        {
            size_t wrote = write(w, avail);
            w += wrote;
            avail -= wrote;
            totwrote += wrote;
    	}
        return totwrote;
    }

    size_t write_P(PGM_P buf, size_t size)
    {
        return write((const uint8_t*)buf, size);
    }

    void keepAlive (uint16_t idle_sec = TCP_DEFAULT_KEEPALIVE_IDLE_SEC, uint16_t intv_sec = TCP_DEFAULT_KEEPALIVE_INTERVAL_SEC, uint8_t count = TCP_DEFAULT_KEEPALIVE_COUNT)
    {
        (void) idle_sec;
        (void) intv_sec;
        (void) count;
        mockverbose("TODO ClientContext::keepAlive()\n");
    }

    bool isKeepAliveEnabled () const
    {
        mockverbose("TODO ClientContext::isKeepAliveEnabled()\n");
        return false;
    }

    uint16_t getKeepAliveIdle () const
    {
        mockverbose("TODO ClientContext::getKeepAliveIdle()\n");
        return 0;
    }

    uint16_t getKeepAliveInterval () const
    {
        mockverbose("TODO ClientContext::getKeepAliveInternal()\n");
        return 0;
    }

    uint8_t getKeepAliveCount () const
    {
        mockverbose("TODO ClientContext::getKeepAliveCount()\n");
        return 0;
    }

    bool getSync () const
    {
        mockverbose("TODO ClientContext::getSync()\n");
        return _sync;
    }

    void setSync (bool sync)
    {
        mockverbose("TODO ClientContext::setSync()\n");
        _sync = sync;
    }

    // return a pointer to available data buffer (size = availableForPeek())
    // semantic forbids any kind of read() before calling peekConsume()
    const char* peekBuffer ()
    {
        return _inbuf;
    }

    // return number of byte accessible by peekBuffer()
    size_t availableForPeek ()
    {
        ssize_t ret = mockPeekBytes(_sock, nullptr, sizeof _inbuf, 0, _inbuf, _inbufsize);
        if (ret < 0)
        {
            abort();
            return 0;
        }
        return _inbufsize;
    }

    // consume bytes after use (see peekBuffer)
    void peekConsume (size_t consume)
    {
        assert(consume <= _inbufsize);
        size_t move = consume;
        if (consume + move > sizeof _inbuf)
            move = sizeof _inbuf - consume;
        memmove(_inbuf, _inbuf + consume, move);
        _inbufsize -= consume;
    }

private:

    discard_cb_t _discard_cb = nullptr;
    void* _discard_cb_arg = nullptr;

    int8_t _refcnt;
    ClientContext* _next;
    
    bool _sync;
    
    // MOCK
    
    int _sock = -1;
    int _timeout_ms = 5000;

    char _inbuf [CCBUFSIZE];
    size_t _inbufsize = 0;
};

#endif //CLIENTCONTEXT_H
