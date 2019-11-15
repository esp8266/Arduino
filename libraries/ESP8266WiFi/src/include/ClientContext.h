/*
 ClientContext.h - TCP connection handling on top of lwIP

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

typedef void (*discard_cb_t)(void*, ClientContext*);

extern "C" void esp_yield();
extern "C" void esp_schedule();

#include "DataSource.h"

bool getDefaultPrivateGlobalSyncValue ();

class ClientContext
{
public:
    ClientContext(tcp_pcb* pcb, discard_cb_t discard_cb, void* discard_cb_arg) :
        _pcb(pcb), _rx_buf(0), _rx_buf_offset(0), _discard_cb(discard_cb), _discard_cb_arg(discard_cb_arg), _refcnt(0), _next(0),
        _sync(::getDefaultPrivateGlobalSyncValue())
    {
        tcp_setprio(pcb, TCP_PRIO_MIN);
        tcp_arg(pcb, this);
        tcp_recv(pcb, &_s_recv);
        tcp_sent(pcb, &_s_acked);
        tcp_err(pcb, &_s_error);
        tcp_poll(pcb, &_s_poll, 1);

        // keep-alive not enabled by default
        //keepAlive();
    }

    err_t abort()
    {
        if(_pcb) {
            DEBUGV(":abort\r\n");
            tcp_arg(_pcb, NULL);
            tcp_sent(_pcb, NULL);
            tcp_recv(_pcb, NULL);
            tcp_err(_pcb, NULL);
            tcp_poll(_pcb, NULL, 0);
            tcp_abort(_pcb);
            _pcb = nullptr;
        }
        return ERR_ABRT;
    }

    err_t close()
    {
        err_t err = ERR_OK;
        if(_pcb) {
            DEBUGV(":close\r\n");
            tcp_arg(_pcb, NULL);
            tcp_sent(_pcb, NULL);
            tcp_recv(_pcb, NULL);
            tcp_err(_pcb, NULL);
            tcp_poll(_pcb, NULL, 0);
            err = tcp_close(_pcb);
            if(err != ERR_OK) {
                DEBUGV(":tc err %d\r\n", (int) err);
                tcp_abort(_pcb);
                err = ERR_ABRT;
            }
            _pcb = nullptr;
        }
        return err;
    }

    ~ClientContext()
    {
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
            if(_discard_cb) {
                _discard_cb(_discard_cb_arg, this);
            }
            DEBUGV(":del\r\n");
            delete this;
        }
    }

    int connect(CONST ip_addr_t* addr, uint16_t port)
    {
        err_t err = tcp_connect(_pcb, addr, port, &ClientContext::_s_connected);
        if (err != ERR_OK) {
            return 0;
        }
        _connect_pending = true;
        _op_start_time = millis();
        for (decltype(_timeout_ms) i = 0; _connect_pending && i < _timeout_ms; i++) {
               // Give scheduled functions a chance to run (e.g. Ethernet uses recurrent)
               delay(1);
               // will resume on timeout or when _connected or _notify_error fires
        }
        _connect_pending = false;
        if (!_pcb) {
            DEBUGV(":cabrt\r\n");
            return 0;
        }
        if (state() != ESTABLISHED) {
            DEBUGV(":ctmo\r\n");
            abort();
            return 0;
        }
        return 1;
    }

    size_t availableForWrite() const
    {
        return _pcb? tcp_sndbuf(_pcb): 0;
    }

    void setNoDelay(bool nodelay)
    {
        if(!_pcb) {
            return;
        }
        if(nodelay) {
            tcp_nagle_disable(_pcb);
        } else {
            tcp_nagle_enable(_pcb);
        }
    }

    bool getNoDelay() const
    {
        if(!_pcb) {
            return false;
        }
        return tcp_nagle_disabled(_pcb);
    }

    void setTimeout(int timeout_ms)
    {
        _timeout_ms = timeout_ms;
    }

    int getTimeout() const
    {
        return _timeout_ms;
    }

    const ip_addr_t* getRemoteAddress() const
    {
        if(!_pcb) {
            return 0;
        }

        return &_pcb->remote_ip;
    }

    uint16_t getRemotePort() const
    {
        if(!_pcb) {
            return 0;
        }

        return _pcb->remote_port;
    }

    const ip_addr_t* getLocalAddress() const
    {
        if(!_pcb) {
            return 0;
        }

        return &_pcb->local_ip;
    }

    uint16_t getLocalPort() const
    {
        if(!_pcb) {
            return 0;
        }

        return _pcb->local_port;
    }

    size_t getSize() const
    {
        if(!_rx_buf) {
            return 0;
        }

        return _rx_buf->tot_len - _rx_buf_offset;
    }

    char read()
    {
        if(!_rx_buf) {
            return 0;
        }

        char c = reinterpret_cast<char*>(_rx_buf->payload)[_rx_buf_offset];
        _consume(1);
        return c;
    }

    size_t read(char* dst, size_t size)
    {
        if(!_rx_buf) {
            return 0;
        }

        size_t max_size = _rx_buf->tot_len - _rx_buf_offset;
        size = (size < max_size) ? size : max_size;

        DEBUGV(":rd %d, %d, %d\r\n", size, _rx_buf->tot_len, _rx_buf_offset);
        size_t size_read = 0;
        while(size) {
            size_t buf_size = _rx_buf->len - _rx_buf_offset;
            size_t copy_size = (size < buf_size) ? size : buf_size;
            DEBUGV(":rdi %d, %d\r\n", buf_size, copy_size);
            os_memcpy(dst, reinterpret_cast<char*>(_rx_buf->payload) + _rx_buf_offset, copy_size);
            dst += copy_size;
            _consume(copy_size);
            size -= copy_size;
            size_read += copy_size;
        }
        return size_read;
    }

    char peek() const
    {
        if(!_rx_buf) {
            return 0;
        }

        return reinterpret_cast<char*>(_rx_buf->payload)[_rx_buf_offset];
    }

    size_t peekBytes(char *dst, size_t size) const
    {
        if(!_rx_buf) {
            return 0;
        }

        size_t max_size = _rx_buf->tot_len - _rx_buf_offset;
        size = (size < max_size) ? size : max_size;

        DEBUGV(":pd %d, %d, %d\r\n", size, _rx_buf->tot_len, _rx_buf_offset);
        size_t buf_size = _rx_buf->len - _rx_buf_offset;
        size_t copy_size = (size < buf_size) ? size : buf_size;
        DEBUGV(":rpi %d, %d\r\n", buf_size, copy_size);
        os_memcpy(dst, reinterpret_cast<char*>(_rx_buf->payload) + _rx_buf_offset, copy_size);
        return copy_size;
    }

    void discard_received()
    {
        if(!_rx_buf) {
            return;
        }
        if(_pcb) {
            tcp_recved(_pcb, (size_t) _rx_buf->tot_len);
        }
        pbuf_free(_rx_buf);
        _rx_buf = 0;
        _rx_buf_offset = 0;
    }

    bool wait_until_sent(int max_wait_ms = WIFICLIENT_MAX_FLUSH_WAIT_MS)
    {
        // https://github.com/esp8266/Arduino/pull/3967#pullrequestreview-83451496
        // option 1 done
        // option 2 / _write_some() not necessary since _datasource is always nullptr here

        if (!_pcb)
            return true;

        int prevsndbuf = -1;

        // wait for peer's acks to flush lwIP's output buffer
        uint32_t last_sent = millis();
        while (1) {
            if (millis() - last_sent > (uint32_t) max_wait_ms) {
#ifdef DEBUGV
                // wait until sent: timeout
                DEBUGV(":wustmo\n");
#endif
                // All data was not flushed, timeout hit
                return false;
            }

            // force lwIP to send what can be sent
            tcp_output(_pcb);

            int sndbuf = tcp_sndbuf(_pcb);
            if (sndbuf != prevsndbuf) {
                // send buffer has changed (or first iteration)
                prevsndbuf = sndbuf;
                // We just sent a bit, move timeout forward
                last_sent = millis();
            }

            delay(0); // from sys or os context

            if ((state() != ESTABLISHED) || (sndbuf == TCP_SND_BUF)) {
                break;
            }
        }

        // All data flushed
        return true;
    }

    uint8_t state() const
    {
        if(!_pcb) {
            return CLOSED;
        }

        return _pcb->state;
    }

    size_t write(const uint8_t* data, size_t size)
    {
        if (!_pcb) {
            return 0;
        }
        return _write_from_source(new BufferDataSource(data, size));
    }

    size_t write(Stream& stream)
    {
        if (!_pcb) {
            return 0;
        }
        return _write_from_source(new BufferedStreamDataSource<Stream>(stream, stream.available()));
    }

    size_t write_P(PGM_P buf, size_t size)
    {
        if (!_pcb) {
            return 0;
        }
        ProgmemStream stream(buf, size);
        return _write_from_source(new BufferedStreamDataSource<ProgmemStream>(stream, size));
    }

    void keepAlive (uint16_t idle_sec = TCP_DEFAULT_KEEPALIVE_IDLE_SEC, uint16_t intv_sec = TCP_DEFAULT_KEEPALIVE_INTERVAL_SEC, uint8_t count = TCP_DEFAULT_KEEPALIVE_COUNT)
    {
        if (idle_sec && intv_sec && count) {
            _pcb->so_options |= SOF_KEEPALIVE;
            _pcb->keep_idle = (uint32_t)1000 * idle_sec;
            _pcb->keep_intvl = (uint32_t)1000 * intv_sec;
            _pcb->keep_cnt = count;
        }
        else
            _pcb->so_options &= ~SOF_KEEPALIVE;
    }

    bool isKeepAliveEnabled () const
    {
        return !!(_pcb->so_options & SOF_KEEPALIVE);
    }

    uint16_t getKeepAliveIdle () const
    {
        return isKeepAliveEnabled()? (_pcb->keep_idle + 500) / 1000: 0;
    }

    uint16_t getKeepAliveInterval () const
    {
        return isKeepAliveEnabled()? (_pcb->keep_intvl + 500) / 1000: 0;
    }

    uint8_t getKeepAliveCount () const
    {
        return isKeepAliveEnabled()? _pcb->keep_cnt: 0;
    }

    bool getSync () const
    {
        return _sync;
    }

    void setSync (bool sync)
    {
        _sync = sync;
    }

protected:

    bool _is_timeout()
    {
        return millis() - _op_start_time > _timeout_ms;
    }

    void _notify_error()
    {
        if (_connect_pending || _send_waiting) {
            _send_waiting = false;
            _connect_pending = false;
            esp_schedule(); // break delay in connect or _write_from_source
        }
    }

    size_t _write_from_source(DataSource* ds)
    {
        assert(_datasource == nullptr);
        assert(!_send_waiting);
        _datasource = ds;
        _written = 0;
        _op_start_time = millis();
        do {
            if (_write_some()) {
                _op_start_time = millis();
            }

            if (!_datasource->available() || _is_timeout() || state() == CLOSED) {
                if (_is_timeout()) {
                    DEBUGV(":wtmo\r\n");
                }
                delete _datasource;
                _datasource = nullptr;
                break;
            }

            _send_waiting = true;
            for (decltype(_timeout_ms) i = 0; _send_waiting && i < _timeout_ms; i++) {
               // Give scheduled functions a chance to run (e.g. Ethernet uses recurrent)
               delay(1);
               // will resume on timeout or when _write_some_from_cb or _notify_error fires

            }
            _send_waiting = false;
        } while(true);

        if (_sync)
            wait_until_sent();

        return _written;
    }

    bool _write_some()
    {
        if (!_datasource || !_pcb) {
            return false;
        }

        DEBUGV(":wr %d %d\r\n", _datasource->available(), _written);

        bool has_written = false;

        while (_datasource) {
            if (state() == CLOSED)
                return false;
            size_t next_chunk_size = std::min((size_t)tcp_sndbuf(_pcb), _datasource->available());
            if (!next_chunk_size)
                break;
            const uint8_t* buf = _datasource->get_buffer(next_chunk_size);

            uint8_t flags = 0;
            if (next_chunk_size < _datasource->available())
                //   PUSH is meant for peer, telling to give data to user app as soon as received
                //   PUSH "may be set" when sender has finished sending a "meaningful" data block
                //   PUSH does not break Nagle
                //   #5173: windows needs this flag
                //   more info: https://lists.gnu.org/archive/html/lwip-users/2009-11/msg00018.html
                flags |= TCP_WRITE_FLAG_MORE; // do not tcp-PuSH (yet)
            if (!_sync)
                // user data must be copied when data are sent but not yet acknowledged
                // (with sync, we wait for acknowledgment before returning to user)
                flags |= TCP_WRITE_FLAG_COPY;

            err_t err = tcp_write(_pcb, buf, next_chunk_size, flags);

            DEBUGV(":wrc %d %d %d\r\n", next_chunk_size, _datasource->available(), (int)err);

            if (err == ERR_OK) {
                _datasource->release_buffer(buf, next_chunk_size);
                _written += next_chunk_size;
                has_written = true;
            } else {
		// ERR_MEM(-1) is a valid error meaning
		// "come back later". It leaves state() opened
                break;
            }
        }

        if (has_written)
        {
            // lwIP's tcp_output doc: "Find out what we can send and send it"
            // *with respect to Nagle*
            // more info: https://lists.gnu.org/archive/html/lwip-users/2017-11/msg00134.html
            tcp_output(_pcb);
        }

        return has_written;
    }

    void _write_some_from_cb()
    {
        if (_send_waiting) {
            _send_waiting = false;
            esp_schedule(); // break delay in _write_from_source
        }
    }

    err_t _acked(tcp_pcb* pcb, uint16_t len)
    {
        (void) pcb;
        (void) len;
        DEBUGV(":ack %d\r\n", len);
        _write_some_from_cb();
        return ERR_OK;
    }

    void _consume(size_t size)
    {
        if(_pcb)
            tcp_recved(_pcb, size);
        ptrdiff_t left = _rx_buf->len - _rx_buf_offset - size;
        if(left > 0) {
            _rx_buf_offset += size;
        } else if(!_rx_buf->next) {
            DEBUGV(":c0 %d, %d\r\n", size, _rx_buf->tot_len);
            pbuf_free(_rx_buf);
            _rx_buf = 0;
            _rx_buf_offset = 0;
        } else {
            DEBUGV(":c %d, %d, %d\r\n", size, _rx_buf->len, _rx_buf->tot_len);
            auto head = _rx_buf;
            _rx_buf = _rx_buf->next;
            _rx_buf_offset = 0;
            pbuf_ref(_rx_buf);
            pbuf_free(head);
        }
    }

    err_t _recv(tcp_pcb* pcb, pbuf* pb, err_t err)
    {
        (void) pcb;
        (void) err;
        if(pb == 0) { // connection closed
            DEBUGV(":rcl\r\n");
            _notify_error();
            abort();
            return ERR_ABRT;
        }

        if(_rx_buf) {
            DEBUGV(":rch %d, %d\r\n", _rx_buf->tot_len, pb->tot_len);
            pbuf_cat(_rx_buf, pb);
        } else {
            DEBUGV(":rn %d\r\n", pb->tot_len);
            _rx_buf = pb;
            _rx_buf_offset = 0;
        }
        return ERR_OK;
    }

    void _error(err_t err)
    {
        (void) err;
        DEBUGV(":er %d 0x%08x\r\n", (int) err, (uint32_t) _datasource);
        tcp_arg(_pcb, NULL);
        tcp_sent(_pcb, NULL);
        tcp_recv(_pcb, NULL);
        tcp_err(_pcb, NULL);
        _pcb = nullptr;
        _notify_error();
    }

    err_t _connected(struct tcp_pcb *pcb, err_t err)
    {
        (void) err;
        (void) pcb;
        assert(pcb == _pcb);
        if (_connect_pending) {
            _connect_pending = false;
            esp_schedule(); // break delay in connect
        }
        return ERR_OK;
    }

    err_t _poll(tcp_pcb*)
    {
        _write_some_from_cb();
        return ERR_OK;
    }

    static err_t _s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err)
    {
        return reinterpret_cast<ClientContext*>(arg)->_recv(tpcb, pb, err);
    }

    static void _s_error(void *arg, err_t err)
    {
        reinterpret_cast<ClientContext*>(arg)->_error(err);
    }

    static err_t _s_poll(void *arg, struct tcp_pcb *tpcb)
    {
        return reinterpret_cast<ClientContext*>(arg)->_poll(tpcb);
    }

    static err_t _s_acked(void *arg, struct tcp_pcb *tpcb, uint16_t len)
    {
        return reinterpret_cast<ClientContext*>(arg)->_acked(tpcb, len);
    }

    static err_t _s_connected(void* arg, struct tcp_pcb *pcb, err_t err)
    {
        return reinterpret_cast<ClientContext*>(arg)->_connected(pcb, err);
    }

private:
    tcp_pcb* _pcb;

    pbuf* _rx_buf;
    size_t _rx_buf_offset;

    discard_cb_t _discard_cb;
    void* _discard_cb_arg;

    DataSource* _datasource = nullptr;
    size_t _written = 0;
    uint32_t _timeout_ms = 5000;
    uint32_t _op_start_time = 0;
    bool _send_waiting = false;
    bool _connect_pending = false;

    int8_t _refcnt;
    ClientContext* _next;

    bool _sync;
};

#endif//CLIENTCONTEXT_H
