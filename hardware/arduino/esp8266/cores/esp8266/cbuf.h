/* Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 * This file is part of the atproto AT protocol library
 *
 * Redistribution and use is permitted according to the conditions of the
 * 3-clause BSD license to be found in the LICENSE file.
 */
#ifndef __cbuf_h
#define __cbuf_h

#include <stdint.h>
class cbuf
{
public:
    cbuf(size_t size)
    : _size(size)
    , _buf(new char[size])
    , _bufend(_buf + size)
    , _begin(_buf)
    , _end(_begin)
    {
    }
    
    ~cbuf()
    {
        delete[] _buf;
    }
    
    size_t getSize() const
    {
        if (_end >= _begin)
            return _end - _begin;
        
        return _size - (_begin - _end);
    }
    
    size_t room() const
    {
        if (_end >= _begin)
            return _size - (_end - _begin) - 1;
        
        return _begin - _end - 1;
    }
    
    char peek()
    {
        if (_end == _begin)
            return -1;
        
        return *_begin;
    }
    
    char read()
    {
        if (getSize() == 0)
            return -1;
        
        char result = *_begin;
        if (++_begin == _bufend)
            _begin = _buf;
        return result;
    }
    
    size_t read(char* dst, size_t size)
    {
        size_t bytes_available = getSize();
        size_t size_to_read = (size < bytes_available) ? size : bytes_available;
        size_t size_read = size_to_read;
        if (_end < _begin && size_to_read > _bufend - _begin)
        {
            size_t top_size = _bufend - _begin;
            memcpy(dst, _begin, top_size);
            _begin = _buf;
            size_to_read -= top_size;
            dst += top_size;
        }
        memcpy(dst, _begin, size_to_read);
        _begin += size_to_read;
        if (_begin == _bufend)
            _begin = _buf;
        return size_read;
    }
    
    size_t write(char c)
    {
        if (room() == 0)
            return 0;
        
        *_end = c;
        if (++_end == _bufend)
            _end = _buf;
        return 1;
    }
    
    size_t write(const char* src, size_t size)
    {
        size_t bytes_available = room();
        size_t size_to_write = (size < bytes_available) ? size : bytes_available;
        size_t size_written = size_to_write;
        if (_end > _begin && size_to_write > _bufend - _end)
        {
            size_t top_size = _bufend - _end;
            memcpy(_end, src, top_size);
            _end = _buf;
            size_to_write -= top_size;
            src += top_size;
        }
        memcpy(_end, src, size_to_write);
        _end += size_to_write;
        if (_end == _bufend)
            _end = _buf;
        return size_written;
    }

    void flush()
    {
        _begin = _buf;
        _end = _buf;
    }
    
private:
    size_t _size;
    char*  _buf;
    char*  _bufend;
    char*  _begin;
    char*  _end;
};


#endif//__cbuf_h
