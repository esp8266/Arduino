/* DataStrategyImpl.h - a few strategies for writing data to ClientContext,
                        plus some helper classes
 * Copyright (c) 2016 Ivan Grokhotkov. All rights reserved.
 * This file is distributed under MIT license.
 */

#ifndef DATASTRATEGYIMPL_H
#define DATASTRATEGYIMPL_H

#include "DataStrategy.h"
#include <assert.h>

class BufferStrategy : public DataStrategy
{
public:
    BufferStrategy(const uint8_t* buf, size_t size, uint32_t timeout)
        : _buf(buf),
          _size(size),
          _timeout(timeout + millis())
    {
    }

    size_t write(ClientContext& ctx) override
    {
        write_some(ctx);
        while (!_done) {
            esp_yield();
        }
        return _written;
    }

    void write_some(ClientContext& ctx)
    {
        size_t can_write = ctx.getSendBufferSize();
        size_t will_write = (can_write < _size) ? can_write : _size;
        if (!ctx.write(_buf, will_write)) {
            end();
            return;
        }
        _buf += will_write;
        _size -= will_write;
        _queued += will_write;
    }

    void on_sent(ClientContext& ctx, size_t size) override
    {
        if (_size > 0) {
            write_some(ctx);
        }
        _queued -= size;
        _written += size;
        if (_queued == 0) {
            end();
        }
    }

    void on_error(ClientContext& ctx) override
    {
        if (_queued > 0) {
            end();
        }
    }

    void on_poll(ClientContext& ctx) override
    {
        if (millis() > _timeout && _queued > 0) {
            end();
        }
    }

    void end()
    {
        if (!_done) {
            _done = true;
            esp_schedule();
        }
    }

protected:
    const uint8_t * _buf;
    size_t _size;
    size_t _written = 0;
    size_t _queued = 0;
    uint32_t _timeout = 0;
    bool _done = false;
};

class BufferLink
{
public:
    BufferLink(size_t size, BufferLink* prev) :
        _size(size),
        _data(new uint8_t[size])
    {
        if (prev) {
            prev->_next = this;
        }
    }

    ~BufferLink()
    {
        delete[] _data;
    }

    uint8_t* data() const
    {
        return _data;
    }

    size_t size() const
    {
        return _size;
    }

    BufferLink* next() const
    {
        return _next;
    }

protected:
    size_t _size;
    uint8_t* _data;
    BufferLink* _next = nullptr;
};

class ProgmemSource
{
public:
    ProgmemSource(PGM_P buf, size_t size) :
        _buf(buf),
        _left(size)
    {
    }

    size_t readBytes(char* dst, size_t size)
    {
        size_t will_read = (_left < size) ? _left : size;
        memcpy_P((void*)dst, (PGM_VOID_P)_buf, will_read);
        _left -= will_read;
        _buf  += will_read;
        return will_read;
    }

protected:
    PGM_P _buf;
    size_t _left;
};

template<typename TSource>
class ChunkedStrategy : public DataStrategy
{
public:
    ChunkedStrategy(TSource& source, size_t size, unsigned timeout) :
        _source(source),
        _size(size),
        _timeout(timeout)
    {
    }

    ~ChunkedStrategy() override
    {
        while (_buffers_head) {
            auto tmp = _buffers_head;
            _buffers_head = _buffers_head->next();
            delete tmp;
        }
    }

    size_t write(ClientContext& ctx) override
    {
        write_some(ctx);
        while (!_done) {
            esp_yield();
        }
        return _written;
    }

    void write_some(ClientContext& ctx)
    {
        size_t can_write = ctx.getSendBufferSize();
        size_t will_write = (can_write < _size) ? can_write : _size;

        BufferLink* new_buf = new BufferLink(will_write, _buffers_tail);
        if (!_buffers_head) {
            _buffers_head = new_buf;
        }
        _buffers_tail = new_buf;
        size_t cb = _source.readBytes((char*) new_buf->data(), will_write);
        if (cb < will_write) {
            end();
            return;
        }
        if (!ctx.write(new_buf->data(), will_write)) {
            end();
            return;
        }
        _size -= will_write;
        _last_write_time = millis();
    }

    void on_sent(ClientContext& ctx, size_t size) override
    {
        if (_size > 0) {
            write_some(ctx);
        }
        auto size_to_remove = size;
        while (size_to_remove) {
            assert(_buffers_head != nullptr);
            size_t buf_size = _buffers_head->size() - _offset;
            if (buf_size > size_to_remove) {
                _offset += size_to_remove;
                break;
            }
            auto tmp = _buffers_head;
            _buffers_head = _buffers_head->next();
            delete tmp;
            _offset = 0;
            size_to_remove -= buf_size;
        }
        _written += size;
        if (_buffers_head == nullptr) {
            end();
        }
    }

    void on_error(ClientContext& ctx) override
    {
        if (_buffers_head != nullptr) {
            end();
        }
    }

    void on_poll(ClientContext& ctx) override
    {
        if (_last_write_time != 0 &&
                millis() - _last_write_time > _timeout &&
                _buffers_head != nullptr) {
            end();
        }
    }

    void end()
    {
        if (!_done) {
            _done = true;
            esp_schedule();
        }
    }

protected:
    TSource& _source;
    BufferLink* _buffers_head = nullptr;
    BufferLink* _buffers_tail = nullptr;
    size_t _size;
    size_t _written = 0;
    uint32_t _timeout = 0;
    size_t _offset = 0;
    bool _done = false;
    uint32_t _last_write_time = 0;
};


#endif//DATASTRATEGYIMPL_H
