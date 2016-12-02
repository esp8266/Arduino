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

#ifdef LWIP_OPEN_SRC
typedef err_t recv_ret_t;
#else
typedef int32_t recv_ret_t;
#endif

#include "DataSource.h"

class ClientContext
{
public:
    ClientContext(tcp_pcb* pcb, discard_cb_t discard_cb, void* discard_cb_arg) :
        _pcb(pcb), _rx_buf(0), _rx_buf_offset(0), _discard_cb(discard_cb), _discard_cb_arg(discard_cb_arg), _refcnt(0), _next(0)
    {
        tcp_setprio(pcb, TCP_PRIO_MIN);
        tcp_arg(pcb, this);
        tcp_recv(pcb, (tcp_recv_fn) &_s_recv);
        tcp_sent(pcb, &_s_sent);
        tcp_err(pcb, &_s_error);
        tcp_poll(pcb, &_s_poll, 1);
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
            _pcb = 0;
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
                DEBUGV(":tc err %d\r\n", err);
                tcp_abort(_pcb);
                err = ERR_ABRT;
            }
            _pcb = 0;
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
        if(this != 0) {
            DEBUGV(":ur %d\r\n", _refcnt);
            if(--_refcnt == 0) {
                flush();
                close();
                if(_discard_cb) {
                    _discard_cb(_discard_cb_arg, this);
                }
                DEBUGV(":del\r\n");
                delete this;
            }
        }
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

    bool getNoDelay()
    {
        if(!_pcb) {
            return false;
        }
        return tcp_nagle_disabled(_pcb);
    }

    void setNonBlocking(bool nonblocking)
    {
        _noblock = nonblocking;
    }

    bool getNonBlocking()
    {
        return _noblock;
    }

    uint32_t getRemoteAddress()
    {
        if(!_pcb) {
            return 0;
        }

        return _pcb->remote_ip.addr;
    }

    uint16_t getRemotePort()
    {
        if(!_pcb) {
            return 0;
        }

        return _pcb->remote_port;
    }

    uint32_t getLocalAddress()
    {
        if(!_pcb) {
            return 0;
        }

        return _pcb->local_ip.addr;
    }

    uint16_t getLocalPort()
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

    char peek()
    {
        if(!_rx_buf) {
            return 0;
        }

        return reinterpret_cast<char*>(_rx_buf->payload)[_rx_buf_offset];
    }

    size_t peekBytes(char *dst, size_t size)
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

    void flush()
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

protected:

    void _cancel_write()
    {
        if (_datasource) {
            delete _datasource;
            _datasource = nullptr;
            esp_schedule();
        }
    }

    size_t _write_from_source(DataSource* ds)
    {
        assert(_datasource == nullptr);
        _datasource = ds;
        _written = 0;
        _write_some();
        while (_datasource && !_noblock) {
            _send_waiting = true;
            esp_yield();
        }
        _send_waiting = false;
        return _written;
    }


    void _write_some()
    {
        if (!_datasource || !_pcb) {
            return;
        }

        size_t left = _datasource->available();
        size_t can_send = tcp_sndbuf(_pcb);
        if (_pcb->snd_queuelen >= TCP_SND_QUEUELEN) {
            can_send = 0;
        }
        size_t will_send = (can_send < left) ? can_send : left;
        if (will_send) {
            const uint8_t* buf = _datasource->get_buffer(will_send);
            err_t err = tcp_write(_pcb, buf, will_send, TCP_WRITE_FLAG_COPY);
            _datasource->release_buffer(buf, will_send);
            if (err == ERR_OK) {
                _written += will_send;
                tcp_output(_pcb);
            }
        }

        if (!_datasource->available() || _noblock) {
            delete _datasource;
            _datasource = nullptr;
        }
    }

    void _write_some_from_cb()
    {
        _write_some();
        if (!_datasource && _send_waiting) {
            esp_schedule();
        }
    }

    err_t _sent(tcp_pcb* pcb, uint16_t len)
    {
        DEBUGV(":sent %d\r\n", len);
        _write_some_from_cb();
        return ERR_OK;
    }

    void _consume(size_t size)
    {
        ptrdiff_t left = _rx_buf->len - _rx_buf_offset - size;
        if(left > 0) {
            _rx_buf_offset += size;
        } else if(!_rx_buf->next) {
            DEBUGV(":c0 %d, %d\r\n", size, _rx_buf->tot_len);
            if(_pcb) {
                tcp_recved(_pcb, _rx_buf->len);
            }
            pbuf_free(_rx_buf);
            _rx_buf = 0;
            _rx_buf_offset = 0;
        } else {
            DEBUGV(":c %d, %d, %d\r\n", size, _rx_buf->len, _rx_buf->tot_len);
            auto head = _rx_buf;
            _rx_buf = _rx_buf->next;
            _rx_buf_offset = 0;
            pbuf_ref(_rx_buf);
            if(_pcb) {
                tcp_recved(_pcb, head->len);
            }
            pbuf_free(head);
        }
    }

    recv_ret_t _recv(tcp_pcb* pcb, pbuf* pb, err_t err)
    {
        if(pb == 0) { // connection closed
            DEBUGV(":rcl\r\n");
            _cancel_write();
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
        DEBUGV(":er %d %08x\r\n", err, (uint32_t) _datasource);
        tcp_arg(_pcb, NULL);
        tcp_sent(_pcb, NULL);
        tcp_recv(_pcb, NULL);
        tcp_err(_pcb, NULL);
        _pcb = NULL;
        _cancel_write();
    }

    err_t _poll(tcp_pcb*)
    {
        _write_some_from_cb();
        return ERR_OK;
    }

    static recv_ret_t _s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err)
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

    static err_t _s_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len)
    {
        return reinterpret_cast<ClientContext*>(arg)->_sent(tpcb, len);
    }

private:
    tcp_pcb* _pcb;

    pbuf* _rx_buf;
    size_t _rx_buf_offset;

    discard_cb_t _discard_cb;
    void* _discard_cb_arg;

    int _refcnt;
    ClientContext* _next;

    DataSource* _datasource = nullptr;
    size_t _written = 0;
    bool _noblock = false;
    bool _send_waiting = false;
};

#endif//CLIENTCONTEXT_H
