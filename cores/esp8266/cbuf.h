/* 
 cbuf.h - Circular buffer implementation
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

#ifndef __cbuf_h
#define __cbuf_h

#include <stddef.h>
#include <stdint.h>
#include <string.h>

class cbuf {
    public:
        cbuf(size_t size) :
                _size(size), _buf(new char[size]), _bufend(_buf + size), _begin(_buf), _end(_begin) {
        }

        ~cbuf() {
            delete[] _buf;
        }

        size_t getSize() const;

        size_t room() const {
            if(_end >= _begin) {
                return _size - (_end - _begin) - 1;
            }
            return _begin - _end - 1;
        }

        inline bool empty() const {
            return _begin == _end;
        }

        inline bool full() const {
            return wrap_if_bufend(_end + 1) == _begin;
        }

        int peek() {
            if(empty()) return -1;

            return static_cast<int>(*_begin);
        }

        int read();

        size_t read(char* dst, size_t size) {
            size_t bytes_available = getSize();
            size_t size_to_read = (size < bytes_available) ? size : bytes_available;
            size_t size_read = size_to_read;
            if(_end < _begin && size_to_read > (size_t)(_bufend - _begin)) {
                size_t top_size = _bufend - _begin;
                memcpy(dst, _begin, top_size);
                _begin = _buf;
                size_to_read -= top_size;
                dst += top_size;
            }
            memcpy(dst, _begin, size_to_read);
            _begin = wrap_if_bufend(_begin + size_to_read);
            return size_read;
        }

        size_t write(char c);

        size_t write(const char* src, size_t size) {
            size_t bytes_available = room();
            size_t size_to_write = (size < bytes_available) ? size : bytes_available;
            size_t size_written = size_to_write;
            if(_end >= _begin && size_to_write > (size_t)(_bufend - _end)) {
                size_t top_size = _bufend - _end;
                memcpy(_end, src, top_size);
                _end = _buf;
                size_to_write -= top_size;
                src += top_size;
            }
            memcpy(_end, src, size_to_write);
            _end = wrap_if_bufend(_end + size_to_write);
            return size_written;
        }

        void flush() {
            _begin = _buf;
            _end = _buf;
        }

    private:
        inline char* wrap_if_bufend(char* ptr) const {
            return (ptr == _bufend) ? _buf : ptr;
        }

        const size_t _size;
        char* _buf;
        const char* const _bufend;
        char* _begin;
        char* _end;
};

#endif//__cbuf_h
