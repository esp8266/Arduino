/*
    StreamDev.h - Stream helpers
    Copyright (c) 2019 David Gauchard.  All right reserved.

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

    parsing functions based on TextFinder library by Michael Margolis
*/

#ifndef __STREAMDEV_H
#define __STREAMDEV_H

#include <StreamString.h>

///////////////////////////////////////////////
// /dev/null
// - black hole as output, swallow everything, availableForWrite = infinite
// - black hole as input, nothing to read, available = 0

class StreamNull: public Stream
{
public:

    // Print
    virtual size_t write(uint8_t) override
    {
        return 1;
    }
    virtual size_t write(const uint8_t* buffer, size_t size) override
    {
        (void)buffer;
        (void)size;
        return size;
    }
    virtual int availableForWrite() override
    {
        return 32767;
    }

    // Stream
    virtual int available() override
    {
        return 0;
    }
    virtual int read() override
    {
        return -1;
    }
    virtual int peek() override
    {
        return -1;
    }
    virtual size_t readBytes(char* buffer, size_t len) override
    {
        (void)buffer;
        (void)len;
        return 0;
    }
    virtual int read(uint8_t* buffer, size_t len) override
    {
        (void)buffer;
        (void)len;
        return 0;
    }
    virtual bool outputTimeoutPossible() override
    {
        return false;
    }
    virtual bool inputTimeoutPossible() override
    {
        return false;
    }

    virtual ssize_t streamSize() override
    {
        return 0;
    }
};

///////////////////////////////////////////////
// /dev/zero
// - black hole as output, swallow everything, availableForWrite = infinite
// - big bang as input, gives infinity to read, available = infinite

class StreamZero: public StreamNull
{
protected:
    char _x;
public:
    StreamZero(char x = 0): _x(x) { }

    // Stream
    virtual int available() override
    {
        return 32767;
    }
    virtual int read() override
    {
        return _x;
    }
    virtual int peek() override
    {
        return _x;
    }
    virtual size_t readBytes(char* buffer, size_t len) override
    {
        memset(buffer, _x, len);
        return len;
    }
    virtual int read(uint8_t* buffer, size_t len) override
    {
        memset((char*)buffer, _x, len);
        return len;
    }

    virtual ssize_t streamSize() override
    {
        return 32767;
    }
};

///////////////////////////////////////////////
// static buffer (in flash or ram)
// - black hole as output, swallow everything, availableForWrite = infinite
// - Stream buffer out as input, resettable

class StreamPtr: public StreamNull
{
protected:
    const char* _buffer;
    size_t _size;
    bool _in_flash;
    size_t _peekPointer = 0;

public:
    StreamPtr(const String& string): _buffer(string.c_str()), _size(string.length()), _in_flash(false) { }
    StreamPtr(const char* buffer, size_t size, bool in_flash = false): _buffer(buffer), _size(size), _in_flash(in_flash) { }
    StreamPtr(const uint8_t* buffer, size_t size, bool in_flash = false): _buffer((const char*)buffer), _size(size), _in_flash(in_flash) { }
    StreamPtr(const __FlashStringHelper* buffer, size_t size): _buffer(reinterpret_cast<const char*>(buffer)), _size(size), _in_flash(true) { }
//    StreamPtr(const __FlashStringHelper* text): _buffer(reinterpret_cast<const char*>(text)), _size(strlen_P(text)), _in_flash(true) { }

    void peekPointerReset(int pointer = 0)
    {
        _peekPointer = pointer;
    }

    // Stream
    virtual int available() override
    {
        return availableForPeek();
    }

    virtual int read() override
    {
        return _peekPointer < _size ? _buffer[_peekPointer++] : -1;
    }

    virtual int peek() override
    {
        return _peekPointer < _size ? _buffer[_peekPointer] : -1;
    }

    virtual size_t readBytes(char* buffer, size_t len) override
    {
        if (_peekPointer >= _size)
        {
            return 0;
        }
        size_t cpylen = std::min(_size - _peekPointer, len);
        if (_in_flash)
        {
            memcpy_P(buffer, _buffer + _peekPointer, cpylen);
        }
        else
        {
            memcpy(buffer, _buffer + _peekPointer, cpylen);
        }
        _peekPointer += cpylen;
        return cpylen;
    }

    virtual int read(uint8_t* buffer, size_t len) override
    {
        return readBytes((char*)buffer, len);
    }

    virtual ssize_t streamSize() override
    {
        return _size;
    }

    // peekBuffer
    virtual bool peekBufferAPI() const override
    {
        return !_in_flash;
    }

    virtual size_t availableForPeek() override
    {
        return _peekPointer < _size ? _size - _peekPointer : 0;
    }

    virtual const char* peekBuffer() override
    {
        return _peekPointer < _size ? _buffer + _peekPointer : nullptr;
    }

    virtual void peekConsume(size_t consume) override
    {
        _peekPointer += consume;
    }
};

Stream& operator << (Stream& out, String& string);
Stream& operator << (Stream& out, Stream& stream);
Stream& operator << (Stream& out, StreamString& stream);
Stream& operator << (Stream& out, const char* text);
Stream& operator << (Stream& out, const __FlashStringHelper* text);

///////////////////////////////////////////////
// serialization:
// combine multiple input Stream into one
// useful when sending HTML content (strings, files. ...)

template <int MaxSegments = 10>
class SerialStreamArray: public Stream
{
protected:

    Stream* m_segments [MaxSegments];
    int m_size = 0;
    int m_current = 0;

public:

    // not writable
    virtual size_t write(uint8_t) override
    {
        return 0;
    }
    virtual size_t write(const uint8_t* buffer, size_t size) override
    {
        (void)buffer;
        (void)size;
        return 0;
    }
    virtual int availableForWrite() override
    {
        return 0;
    }
    virtual bool outputTimeoutPossible() override
    {
        return false;
    }

    // not offering peekBuffer because one streamed element can be not compatible
    // (Stream:: is by default not peekBuffer-enabled)
    // input timeout may be possible:
    virtual bool inputTimeoutPossible() override
    {
        return true;
    }

    SerialStreamArray() {}

    bool addref(Stream& s)
    {
        return addref(&s);
    }

    bool addref(Stream* s)
    {
        if (m_size >= MaxSegments)
        {
            //XXXDEBUG: print exhausted message
            return false;
        }

        m_segments[m_size++] = s;
        return true;
    }

    // Stream
    virtual int available() override
    {
        while (true)
        {
            if (m_current >= m_size)
                // end of all
            {
                return 0;
            }

            int ret = m_segments[m_current]->available();
            if (ret > 0)
            {
                return ret;
            }

            m_current++;
        }
    }

    virtual int read() override
    {
        while (true)
        {
            if (m_current >= m_size)
                // end of all
            {
                return 0;
            }

            int ret = m_segments[m_current]->read();
            if (ret > 0)
            {
                return ret;
            }

            m_current++;
        }
    }

    virtual int peek() override
    {
        while (true)
        {
            if (m_current >= m_size)
                // end of all
            {
                return 0;
            }

            int ret = m_segments[m_current]->peek();
            if (ret > 0)
            {
                return ret;
            }

            m_current++;
        }
    }

    virtual size_t readBytes(char* buffer, size_t len) override
    {
        while (true)
        {
            if (m_current >= m_size)
                // end of all
            {
                return 0;
            }

            size_t ret = m_segments[m_current]->readBytes(buffer, len);
            if (ret > 0)
            {
                return ret;
            }

            m_current++;
        }
    }

    virtual int read(uint8_t* buffer, size_t len) override
    {
        while (true)
        {
            if (m_current >= m_size)
                // end of all
            {
                return 0;
            }

            int ret = m_segments[m_current]->read(buffer, len);
            if (ret > 0)
            {
                return ret;
            }

            m_current++;
        }
    }

    virtual ssize_t streamSize() override
    {
        ssize_t ret = 0;
        for (int i = 0; i < m_size; i++)
        {
            ssize_t s = m_segments[i]->size();
            if (s == -1)
            {
                return -1;
            }
            ret += s;
        }
        return ret;
    }
};

class SerialStream: public SerialStreamArray<>
{
};

///////////////////////////////////////////////

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_STREAMDEV)
extern StreamNull devnull;
#endif

#endif // __STREAMDEV_H
