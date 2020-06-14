/*
  SSLContext.h - Used by WiFiClientAxTLS
  Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
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

#ifndef SSLCONTEXT_H
#define SSLCONTEXT_H

#define LWIP_INTERNAL

extern "C"
{
#include "osapi.h"
#include "ets_sys.h"
}
#include <list>
#include <errno.h>
#include "lwip/opt.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include <include/ClientContext.h>
#include <WiFiClientSecureAxTLS.h>
#include <debug.h>
#include "c_types.h"

namespace axTLS {

typedef struct BufferItem
{
    BufferItem(const uint8_t* data_, size_t size_)
    : size(size_), data(new uint8_t[size])
    {
        if (data.get() != nullptr) {
            memcpy(data.get(), data_, size);
        } else {
            DEBUGV(":wcs alloc %d failed\r\n", size_);
            size = 0;
        }
    }

    size_t size;
    std::unique_ptr<uint8_t[]> data;
} BufferItem;

typedef std::list<BufferItem> BufferList;

class SSLContext
{
public:
    SSLContext(bool isServer = false)
    {
        _isServer = isServer;
        if (!_isServer) {
            if (_ssl_client_ctx_refcnt == 0) {
                _ssl_client_ctx = ssl_ctx_new(SSL_SERVER_VERIFY_LATER | SSL_DEBUG_OPTS | SSL_CONNECT_IN_PARTS | SSL_READ_BLOCKING | SSL_NO_DEFAULT_KEY, 0);
            }
            ++_ssl_client_ctx_refcnt;
        } else {
            if (_ssl_svr_ctx_refcnt == 0) {
                _ssl_svr_ctx = ssl_ctx_new(SSL_SERVER_VERIFY_LATER | SSL_DEBUG_OPTS | SSL_CONNECT_IN_PARTS | SSL_READ_BLOCKING | SSL_NO_DEFAULT_KEY, 0);
            }
            ++_ssl_svr_ctx_refcnt;
        }
    }

    ~SSLContext()
    {
        if (io_ctx) {
            io_ctx->unref();
            io_ctx = nullptr;
        }
        _ssl = nullptr;
        if (!_isServer) {
            --_ssl_client_ctx_refcnt;
            if (_ssl_client_ctx_refcnt == 0) {
                ssl_ctx_free(_ssl_client_ctx);
                _ssl_client_ctx = nullptr;
            }
        } else {
            --_ssl_svr_ctx_refcnt;
            if (_ssl_svr_ctx_refcnt == 0) {
                ssl_ctx_free(_ssl_svr_ctx);
                _ssl_svr_ctx = nullptr;
            }
        }
    }

    static void _delete_shared_SSL(SSL *_to_del)
    {
        ssl_free(_to_del);
    }

    void connect(ClientContext* ctx, const char* hostName, uint32_t timeout_ms)
    {
        SSL_EXTENSIONS* ext = ssl_ext_new();
        ssl_ext_set_host_name(ext, hostName);
        if (_ssl) {
            /* Creating a new TLS session on top of a new TCP connection.
               ssl_free will want to send a close notify alert, but the old TCP connection
               is already gone at this point, so reset io_ctx. */
            io_ctx = nullptr;
            _ssl = nullptr;
            _available = 0;
            _read_ptr = nullptr;
        }
        io_ctx = ctx;
        ctx->ref();

        // Wrap the new SSL with a smart pointer, custom deleter to call ssl_free
        SSL *_new_ssl = ssl_client_new(_ssl_client_ctx, reinterpret_cast<int>(this), nullptr, 0, ext);
        std::shared_ptr<SSL> _new_ssl_shared(_new_ssl, _delete_shared_SSL);
        _ssl = _new_ssl_shared;

        uint32_t t = millis();

        while (millis() - t < timeout_ms && ssl_handshake_status(_ssl.get()) != SSL_OK) {
            uint8_t* data;
            int rc = ssl_read(_ssl.get(), &data);
            if (rc < SSL_OK) {
                ssl_display_error(rc);
                break;
            }
        }
    }

    void connectServer(ClientContext *ctx, uint32_t timeout_ms)
    {
        io_ctx = ctx;
        ctx->ref();

        // Wrap the new SSL with a smart pointer, custom deleter to call ssl_free
	SSL *_new_ssl = ssl_server_new(_ssl_svr_ctx, reinterpret_cast<int>(this));
        std::shared_ptr<SSL> _new_ssl_shared(_new_ssl, _delete_shared_SSL);
        _ssl = _new_ssl_shared;

        uint32_t t = millis();

        while (millis() - t < timeout_ms && ssl_handshake_status(_ssl.get()) != SSL_OK) {
            uint8_t* data;
            int rc = ssl_read(_ssl.get(), &data);
            if (rc < SSL_OK) {
                ssl_display_error(rc);
                break;
            }
        }
    }

    void stop()
    {
        if (io_ctx) {
            io_ctx->unref();
        }
        io_ctx = nullptr;
    }

    bool connected()
    {
        if (_isServer) {
            return _ssl != nullptr;
        } else {
            return _ssl != nullptr && ssl_handshake_status(_ssl.get()) == SSL_OK;
        }
    }

    int read(uint8_t* dst, size_t size)
    {
        if (!_available) {
            if (!_readAll()) {
                return 0;
            }
        }
        size_t will_copy = (_available < size) ? _available : size;
        memcpy(dst, _read_ptr, will_copy);
        _read_ptr += will_copy;
        _available -= will_copy;
        if (_available == 0) {
            _read_ptr = nullptr;
            /* Send pending outgoing data, if any */
            if (_hasWriteBuffers()) {
                _writeBuffersSend();
            }
        }
        return will_copy;
    }

    int read()
    {
        if (!_available) {
            if (!_readAll()) {
                return -1;
            }
        }
        int result = _read_ptr[0];
        ++_read_ptr;
        --_available;
        if (_available == 0) {
            _read_ptr = nullptr;
            /* Send pending outgoing data, if any */
            if (_hasWriteBuffers()) {
                _writeBuffersSend();
            }
        }
        return result;
    }

    int write(const uint8_t* src, size_t size)
    {
        if (_isServer) {
            return _write(src, size);
        } else if (!_available) {
            if (_hasWriteBuffers()) {
                int rc = _writeBuffersSend();
                if (rc < 0) {
                    return rc;
                }
            }
            return _write(src, size);
        }
        /* Some received data is still present in the axtls fragment buffer.
           We can't call ssl_write now, as that will overwrite the contents of
           the fragment buffer, corrupting the received data.
           Save a copy of the outgoing data, and call ssl_write when all
           recevied data has been consumed by the application.
        */
        return _writeBufferAdd(src, size);
    }

    int peek()
    {
        if (!_available) {
            if (!_readAll()) {
                return -1;
            }
        }
        return _read_ptr[0];
    }

    size_t peekBytes(char *dst, size_t size)
    {
        if (!_available) {
            if (!_readAll()) {
                return -1;
            }
        }

        size_t will_copy = (_available < size) ? _available : size;
        memcpy(dst, _read_ptr, will_copy);
        return will_copy;
    }

    int available()
    {
        auto cb = _available;
        if (cb == 0) {
            cb = _readAll();
        } else {
            optimistic_yield(100);
        }
        return cb;
    }

    // similar to available, but doesn't return exact size
    bool hasData()
    {
        return _available > 0 || (io_ctx && io_ctx->getSize() > 0);
    }

    bool loadObject(int type, Stream& stream, size_t size)
    {
        std::unique_ptr<uint8_t[]> buf(new uint8_t[size]);
        if (!buf.get()) {
            DEBUGV("loadObject: failed to allocate memory\n");
            return false;
        }

        size_t cb = stream.readBytes(buf.get(), size);
        if (cb != size) {
            DEBUGV("loadObject: reading %u bytes, got %u\n", size, cb);
            return false;
        }

        return loadObject(type, buf.get(), size);
    }

    bool loadObject_P(int type, PGM_VOID_P data, size_t size)
    {
        std::unique_ptr<uint8_t[]> buf(new uint8_t[size]);
        memcpy_P(buf.get(),data, size);
        return loadObject(type, buf.get(), size);
    }

    bool loadObject(int type, const uint8_t* data, size_t size)
    {
        int rc = ssl_obj_memory_load(_isServer?_ssl_svr_ctx:_ssl_client_ctx, type, data, static_cast<int>(size), nullptr);
        if (rc != SSL_OK) {
            DEBUGV("loadObject: ssl_obj_memory_load returned %d\n", rc);
            return false;
        }
        return true;
    }

    bool verifyCert()
    {
        int rc = ssl_verify_cert(_ssl.get());
        if (_allowSelfSignedCerts && rc == SSL_X509_ERROR(X509_VFY_ERROR_SELF_SIGNED)) {
            DEBUGV("Allowing self-signed certificate\n");
            return true;
        } else if (rc != SSL_OK) {
            DEBUGV("ssl_verify_cert returned %d\n", rc);
            ssl_display_error(rc);
            return false;
        }
        return true;
    }

    void allowSelfSignedCerts()
    {
        _allowSelfSignedCerts = true;
    }

    operator SSL*()
    {
        return _ssl.get();
    }

    static ClientContext* getIOContext(int fd)
    {
        if (fd) {
            SSLContext *thisSSL = reinterpret_cast<SSLContext*>(fd);
            return thisSSL->io_ctx;
        }
        return nullptr;
    }

protected:
    int _readAll()
    {
        if (!_ssl) {
            return 0;
        }

        optimistic_yield(100);

        uint8_t* data;
        int rc = ssl_read(_ssl.get(), &data);
        if (rc <= 0) {
            if (rc < SSL_OK && rc != SSL_CLOSE_NOTIFY && rc != SSL_ERROR_CONN_LOST) {
                _ssl = nullptr;
            }
            return 0;
        }
        DEBUGV(":wcs ra %d\r\n", rc);
        _read_ptr = data;
        _available = rc;
        return _available;
    }

    int _write(const uint8_t* src, size_t size)
    {
        if (!_ssl) {
            return 0;
        }

        int rc = ssl_write(_ssl.get(), src, size);
        if (rc >= 0) {
            return rc;
        }
        DEBUGV(":wcs write rc=%d\r\n", rc);
        return rc;
    }

    int _writeBufferAdd(const uint8_t* data, size_t size)
    {
        if (!_ssl) {
            return 0;
        }

        _writeBuffers.emplace_back(data, size);
        if (_writeBuffers.back().data.get() == nullptr) {
            _writeBuffers.pop_back();
            return 0;
        }
        return size;
    }

    int _writeBuffersSend()
    {
        while (!_writeBuffers.empty()) {
            auto& first = _writeBuffers.front();
            int rc = _write(first.data.get(), first.size);
            _writeBuffers.pop_front();
            if (rc < 0) {
                if (_hasWriteBuffers()) {
                    DEBUGV(":wcs _writeBuffersSend dropping unsent data\r\n");
                    _writeBuffers.clear();
                }
                return rc;
            }
        }
        return 0;
    }

    bool _hasWriteBuffers()
    {
        return !_writeBuffers.empty();
    }

    bool _isServer = false;
    static SSL_CTX* _ssl_client_ctx;
    static int _ssl_client_ctx_refcnt;
    static SSL_CTX* _ssl_svr_ctx;
    static int _ssl_svr_ctx_refcnt;
    std::shared_ptr<SSL> _ssl = nullptr;
    const uint8_t* _read_ptr = nullptr;
    size_t _available = 0;
    BufferList _writeBuffers;
    bool _allowSelfSignedCerts = false;
    ClientContext* io_ctx = nullptr;
};

}; // End namespace axTLS

#endif
