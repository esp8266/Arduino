/*
 CircularBuffer.h - yet another circular buffer,
               compatible with Arduino Stream transfer API
 Copyright (c) 2019 david gauchard.  All right reserved.

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

#include <Stream.h>

// CONSTst (CONST-for-stream) should be 'const',
// Stream&Print should be updated accordingly (or Ardui-not).
#define CONSTst //const

template <int _len, typename idx_t = uint16_t>
class CircularBuffer: public Stream
{
protected:

    idx_t _posr, _posw;
    uint8_t _buffer[_len];
    bool _full;
    
    void inc_posr (idx_t contig = 1)
    {
        if ((_posr += contig) == _len)
            _posr = 0;
        _full = false;
    }

    void inc_posw (idx_t contig = 1)
    {
        if ((_posw += contig) == _len)
            _posw = 0;
        if (_posw == _posr)
            _full = true;
    }

    idx_t availableForWriteContig () const
    {
        if (full())
            return 0;
        if (_posr > _posw)
            return _posr - _posw;
        return _len - _posw;
    }

    idx_t availableForReadContig () const
    {
        if (full())
            return _len;
        if (_posr > _posw)
            return _len - _posr;
        return _posw - _posr;
    }

public:

    CircularBuffer (): _posr(0), _posw(0), _full(false) { }

    size_t length () const
    {
    	return _len;
    }

    bool full () const
    {
        return _full;
    }

    bool empty () const
    {
        return _posw == _posr && !_full;
    }

    int availableForWrite () CONSTst override
    {
        if (empty())
            return _len;
        if (_posw > _posr)
            return (_len - _posw) + (_posr - 0);
        return _posr - _posw;
    }
    
    int available () CONSTst override
    {
        if (full())
            return _len;
        if (_posr > _posw)
            return (_len - _posr) + (_posw - 0);
        return _posw - _posr;
    }

    // char/byte oriented

    int peek () CONSTst override
    {
        return empty()? -1 : _buffer[_posr];
    }

    int read () override
    {
        if (empty())
            return -1;
        int c = _buffer[_posr];
        inc_posr();
        return c;
    }
    
    size_t write (uint8_t c) override
    {
        if (full())
            return 0;
        _buffer[_posw] = c;
        inc_posw();
        return 1;
    }

    // buffer oriented
    
    size_t peek (uint8_t* buffer, size_t len) const /*override*/
    {
        if (empty())
            return 0;
        idx_t chunk1 = availableForReadContig();
        if (chunk1 > len)
            chunk1 = len;
        memcpy(buffer, &_buffer[_posr], chunk1);
        return chunk1;
    }
    
    size_t peekMaxSize () const
    {
        return availableForReadContig();
    }
    
    const uint8_t* peekPtr () const
    {
        return &_buffer[_posr];
    }

    size_t read (char* buffer, size_t maxLen) override
    {
        idx_t copied = 0;
        while (copied < maxLen)
        {
            idx_t contig = availableForReadContig();
            if (!contig)
                break;
            if (contig > maxLen - copied)
                contig = maxLen - copied;
            memcpy(buffer + copied, &_buffer[_posr], contig);
            copied += contig;
            inc_posr(contig);
        }
        return copied;
    }

    size_t write (const uint8_t* buffer, size_t maxLen) override
    {
        idx_t copied = 0;
        while (copied < maxLen)
        {
            idx_t contig = availableForWriteContig();
            if (!contig)
                break;
            if (contig > maxLen - copied)
                contig = maxLen - copied;
            memcpy(&_buffer[_posw], buffer + copied, contig);
            copied += contig;
            inc_posw(contig);
        }
        return copied;
    }
};
