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

class ClientContext {
    public:
        ClientContext(tcp_pcb* pcb, discard_cb_t discard_cb, void* discard_cb_arg) :
                _pcb(pcb), _rx_buf(0), _rx_buf_offset(0), _discard_cb(discard_cb), _discard_cb_arg(discard_cb_arg), _refcnt(0), _next(0), _send_waiting(false) {
            tcp_setprio(pcb, TCP_PRIO_MIN);
            tcp_arg(pcb, this);
            tcp_recv(pcb, &_s_recv);
            tcp_sent(pcb, &_s_sent);
            tcp_err(pcb, &_s_error);
        }

        ~ClientContext() {
        }

        ClientContext* next() const {
            return _next;
        }

        ClientContext* next(ClientContext* new_next) {
            _next = new_next;
            return _next;
        }

        void ref() {
            ++_refcnt;
            DEBUGV(":ref %d\r\n", _refcnt);
        }

        void unref() {
            err_t err;
            DEBUGV(":ur %d\r\n", _refcnt);
            if(--_refcnt == 0) {
                flush();
                if(_pcb) {
                    tcp_arg(_pcb, NULL);
                    tcp_sent(_pcb, NULL);
                    tcp_recv(_pcb, NULL);
                    tcp_err(_pcb, NULL);
                    err = tcp_close(_pcb);
                    if(err != ERR_OK) {
                        DEBUGV(":tc err %d\r\n", err);
                        tcp_abort(_pcb);
                    }
                    _pcb = 0;
                }
                delete this;
            }
        }

        uint32_t getRemoteAddress() {
            if(!_pcb) return 0;

            return _pcb->remote_ip.addr;
        }

        uint16_t getRemotePort() {
            if(!_pcb) return 0;

            return _pcb->remote_port;
        }

        size_t getSize() const {
            if(!_rx_buf) return 0;

            return _rx_buf->tot_len - _rx_buf_offset;
        }

        char read() {
            if(!_rx_buf) return 0;

            char c = reinterpret_cast<char*>(_rx_buf->payload)[_rx_buf_offset];
            _consume(1);
            return c;
        }

        size_t read(char* dst, size_t size) {
            if(!_rx_buf) return 0;

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

        char peek() {
            if(!_rx_buf) return 0;

            return reinterpret_cast<char*>(_rx_buf->payload)[_rx_buf_offset];
        }

        void flush() {
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

        uint8_t state() const {
            if(!_pcb) return CLOSED;

            return _pcb->state;
        }

        size_t write(const char* data, size_t size) {
            if(!_pcb) {
                DEBUGV(":wr !_pcb\r\n");
                return 0;
            }

            if(size == 0) {
                return 0;
            }

            size_t room = tcp_sndbuf(_pcb);
            size_t will_send = (room < size) ? room : size;
            err_t err = tcp_write(_pcb, data, will_send, 0);
            if(err != ERR_OK) {
                DEBUGV(":wr !ERR_OK\r\n");
                return 0;
            }

            _size_sent = will_send;
            DEBUGV(":wr\r\n");
            tcp_output( _pcb );
            _send_waiting = true;
            delay(5000); // max send timeout
            _send_waiting = false;
            DEBUGV(":ww\r\n");
            return will_send - _size_sent;
        }

    private:

        void _consume(size_t size) {
            ptrdiff_t left = _rx_buf->len - _rx_buf_offset - size;
            if(left > 0) {
                _rx_buf_offset += size;
            } else if(!_rx_buf->next) {
                DEBUGV(":c0 %d, %d\r\n", size, _rx_buf->tot_len);
                if(_pcb) tcp_recved(_pcb, _rx_buf->len);
                pbuf_free(_rx_buf);
                _rx_buf = 0;
                _rx_buf_offset = 0;
            } else {
                DEBUGV(":c %d, %d, %d\r\n", size, _rx_buf->len, _rx_buf->tot_len);
                auto head = _rx_buf;
                _rx_buf = _rx_buf->next;
                _rx_buf_offset = 0;
                pbuf_ref(_rx_buf);
                if(_pcb) tcp_recved(_pcb, head->len);
                pbuf_free(head);
            }
        }

        err_t _recv(tcp_pcb* pcb, pbuf* pb, err_t err) {

            if(pb == 0) // connection closed
            {
                DEBUGV(":rcl\r\n");
                tcp_arg(pcb, NULL);
                tcp_sent(pcb, NULL);
                tcp_recv(pcb, NULL);
                tcp_err(pcb, NULL);
                // int error = tcp_close(pcb);
                // if (error != ERR_OK)
                {
                    DEBUGV(":rcla\r\n");
                    tcp_abort(pcb);
                    _pcb = 0;
                    return ERR_ABRT;
                }
                _pcb = 0;
                return ERR_OK;
            }

            if(_rx_buf) {
                // there is some unread data
                // chain the new pbuf to the existing one
                DEBUGV(":rch %d, %d\r\n", _rx_buf->tot_len, pb->tot_len);
                pbuf_cat(_rx_buf, pb);
            } else {
                DEBUGV(":rn %d\r\n", pb->tot_len);
                _rx_buf = pb;
                _rx_buf_offset = 0;
            }
            // tcp_recved(pcb, received);
            // pbuf_free(pb);
            return ERR_OK;
        }

        void _error(err_t err) {
            DEBUGV(":er %d\r\n", err);

            if(_pcb) {
                tcp_arg(_pcb, NULL);
                tcp_sent(_pcb, NULL);
                tcp_recv(_pcb, NULL);
                tcp_err(_pcb, NULL);
                err = tcp_close(_pcb);
                if(err != ERR_OK) {
                    DEBUGV(":tc err %d\r\n", err);
                    tcp_abort(_pcb);
                }
            }
            _pcb = 0;

            if(_size_sent && _send_waiting) {
                esp_schedule();
            }
        }

        err_t _poll(tcp_pcb* pcb) {
            return ERR_OK;
        }

        err_t _sent(tcp_pcb* pcb, uint16_t len) {
            DEBUGV(":sent %d\r\n", len);
            _size_sent -= len;
            if(_size_sent == 0 && _send_waiting) esp_schedule();
            return ERR_OK;
        }

        static err_t _s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err) {
            return reinterpret_cast<ClientContext*>(arg)->_recv(tpcb, pb, err);
        }

        static void _s_error(void *arg, err_t err) {
            reinterpret_cast<ClientContext*>(arg)->_error(err);
        }

        static err_t _s_poll(void *arg, struct tcp_pcb *tpcb) {
            return reinterpret_cast<ClientContext*>(arg)->_poll(tpcb);
        }

        static err_t _s_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len) {
            return reinterpret_cast<ClientContext*>(arg)->_sent(tpcb, len);
        }

    private:
        ClientContext* _next;
        int _refcnt;

        tcp_pcb* _pcb;

        pbuf* _rx_buf;
        size_t _rx_buf_offset;

        discard_cb_t _discard_cb;
        void* _discard_cb_arg;

        size_t _size_sent;
        bool _send_waiting;
};

#endif//CLIENTCONTEXT_H
