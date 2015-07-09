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

#include <stdint.h>
class cbuf {
    public:
        cbuf(size_t size) :
                _size(size), _buf(new char[size]), _bufend(_buf + size), _begin(_buf), _end(_begin) {
        }

        ~cbuf() {
            delete[] _buf;
        }

        size_t getSize() const {
            if(_end >= _begin) return _end - _begin;

            return _size - (_begin - _end);
        }

        size_t room() const {
            if(_end >= _begin) return _size - (_end - _begin) - 1;

            return _begin - _end - 1;
        }

        bool empty() const {
            return _begin == _end;
        }

        int peek() {
            if(_end == _begin) return -1;

            return static_cast<int>(*_begin);
        }

        int read() {
            if(getSize() == 0) return -1;

            char result = *_begin;
            if(++_begin == _bufend) _begin = _buf;
            return static_cast<int>(result);
        }

        size_t read(char* dst, size_t size) {
            size_t bytes_available = getSize();
            size_t size_to_read = (size < bytes_available) ? size : bytes_available;
            size_t size_read = size_to_read;
            if(_end < _begin && size_to_read > _bufend - _begin) {
                size_t top_size = _bufend - _begin;
                memcpy(dst, _begin, top_size);
                _begin = _buf;
                size_to_read -= top_size;
                dst += top_size;
            }
            memcpy(dst, _begin, size_to_read);
            _begin += size_to_read;
            if(_begin == _bufend) _begin = _buf;
            return size_read;
        }

        size_t write(char c) {
            if(room() == 0) return 0;

            *_end = c;
            if(++_end == _bufend) _end = _buf;
            return 1;
        }

        size_t write(const char* src, size_t size) {
            size_t bytes_available = room();
            size_t size_to_write = (size < bytes_available) ? size : bytes_available;
            size_t size_written = size_to_write;
            if(_end > _begin && size_to_write > _bufend - _end) {
                size_t top_size = _bufend - _end;
                memcpy(_end, src, top_size);
                _end = _buf;
                size_to_write -= top_size;
                src += top_size;
            }
            memcpy(_end, src, size_to_write);
            _end += size_to_write;
            if(_end == _bufend) _end = _buf;
            return size_written;
        }

        void flush() {
            _begin = _buf;
            _end = _buf;
        }

    private:
        size_t _size;
        char* _buf;
        char* _bufend;
        char* _begin;
        char* _end;
};

#endif//__cbuf_h
