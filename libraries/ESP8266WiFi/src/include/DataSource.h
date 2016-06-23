/* DataSource.h - a read-only object similar to Stream, but with less methods
 * Copyright (c) 2016 Ivan Grokhotkov. All rights reserved.
 * This file is distributed under MIT license.
 */
#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <assert.h>

class DataSource {
public:
    virtual ~DataSource() {}
    virtual size_t available() = 0;
    virtual const uint8_t* get_buffer(size_t size) = 0;
    virtual void release_buffer(const uint8_t* buffer, size_t size) = 0;

};

class BufferDataSource : public DataSource {
public:
    BufferDataSource(const uint8_t* data, size_t size) :
        _data(data),
        _size(size)
    {
    }

    size_t available() override
    {
        return _size - _pos;
    }

    const uint8_t* get_buffer(size_t size) override
    {
        assert(_pos + size <= _size);
        return _data + _pos;
    }

    void release_buffer(const uint8_t* buffer, size_t size) override
    {
        assert(buffer == _data + _pos);
        _pos += size;
    }

protected:
    const uint8_t* _data;
    const size_t _size;
    size_t _pos = 0;
};

template<typename TStream>
class BufferedStreamDataSource : public DataSource {
public:
    BufferedStreamDataSource(TStream& stream, size_t size) :
        _stream(stream),
        _size(size)
    {
    }

    size_t available() override
    {
        return _size - _pos;
    }

    const uint8_t* get_buffer(size_t size) override
    {
        assert(_pos + size <= _size);
        if (_bufferSize < size) {
            _buffer.reset(new uint8_t[size]);
            _bufferSize = size;
        }
        size_t cb = _stream.readBytes(reinterpret_cast<char*>(_buffer.get()), size);
        assert(cb == size);
        return _buffer.get();
    }

    void release_buffer(const uint8_t* buffer, size_t size) override
    {
        _pos += size;
    }

protected:
    TStream& _stream;
    std::unique_ptr<uint8_t[]> _buffer;
    size_t _size;
    size_t _pos = 0;
    size_t _bufferSize = 0;
};

class ProgmemStream
{
public:
    ProgmemStream(PGM_P buf, size_t size) :
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


#endif //DATASOURCE_H
