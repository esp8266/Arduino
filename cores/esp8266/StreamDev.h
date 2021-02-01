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
*/

#ifndef __STREAMDEV_H
#define __STREAMDEV_H

#include <limits>
#include <esp_priv.h>
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
        return std::numeric_limits<int16_t>::max();
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

    virtual bool outputCanTimeout() override
    {
        return false;
    }

    virtual bool inputCanTimeout() override
    {
        return false;
    }

    virtual ssize_t streamRemaining() override
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

    char _zero;

public:

    StreamZero(char zero = 0): _zero(zero) { }

    // Stream
    virtual int available() override
    {
        return std::numeric_limits<int16_t>::max();
    }

    virtual int read() override
    {
        return _zero;
    }

    virtual int peek() override
    {
        return _zero;
    }

    virtual size_t readBytes(char* buffer, size_t len) override
    {
        memset(buffer, _zero, len);
        return len;
    }

    virtual int read(uint8_t* buffer, size_t len) override
    {
        memset((char*)buffer, _zero, len);
        return len;
    }

    virtual ssize_t streamRemaining() override
    {
        return std::numeric_limits<int16_t>::max();
    }
};

///////////////////////////////////////////////
// static buffer (in flash or ram)
// - black hole as output, swallow everything, availableForWrite = infinite
// - Stream buffer out as input, resettable

class StreamConstPtr: public StreamNull
{
protected:
    const char* _buffer;
    size_t _size;
    bool _byteAddressable;
    size_t _peekPointer = 0;

public:
    StreamConstPtr(const String& string): _buffer(string.c_str()), _size(string.length()), _byteAddressable(true) { }
    StreamConstPtr(const char* buffer, size_t size): _buffer(buffer), _size(size), _byteAddressable(__byteAddressable(buffer)) { }
    StreamConstPtr(const uint8_t* buffer, size_t size): _buffer((const char*)buffer), _size(size), _byteAddressable(__byteAddressable(buffer)) { }
    StreamConstPtr(const __FlashStringHelper* buffer, size_t size): _buffer(reinterpret_cast<const char*>(buffer)), _size(size), _byteAddressable(false) { }
    StreamConstPtr(const __FlashStringHelper* text): _buffer(reinterpret_cast<const char*>(text)), _size(strlen_P((PGM_P)text)), _byteAddressable(false) { }

    void resetPointer(int pointer = 0)
    {
        _peekPointer = pointer;
    }

    // Stream
    virtual int available() override
    {
        return peekAvailable();
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
        memcpy_P(buffer, _buffer + _peekPointer, cpylen); // whether byte adressible is true
        _peekPointer += cpylen;
        return cpylen;
    }

    virtual int read(uint8_t* buffer, size_t len) override
    {
        return readBytes((char*)buffer, len);
    }

    virtual ssize_t streamRemaining() override
    {
        return _size;
    }

    // peekBuffer
    virtual bool hasPeekBufferAPI() const override
    {
        return _byteAddressable;
    }

    virtual size_t peekAvailable() override
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

///////////////////////////////////////////////

Stream& operator << (Stream& out, String& string);
Stream& operator << (Stream& out, Stream& stream);
Stream& operator << (Stream& out, StreamString& stream);
Stream& operator << (Stream& out, const char* text);
Stream& operator << (Stream& out, const __FlashStringHelper* text);

///////////////////////////////////////////////

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_STREAMDEV)
extern StreamNull devnull;
#endif

#endif // __STREAMDEV_H
