/**
    StreamString.h

    Copyright (c) 2020 D. Gauchard. All rights reserved.
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

#ifndef __STREAMSTRING_H
#define __STREAMSTRING_H

#include <limits>
#include <algorithm>
#include "Stream.h"
#include "WString.h"

///////////////////////////////////////////////////////////////
// S2Stream points to a String and makes it a Stream
// (it is also the helper for StreamString)

class S2Stream : public Stream
{
public:
    S2Stream(String& string, int peekPointer = -1)
        : string(&string)
        , peekPointer(peekPointer)
    {
    }

    S2Stream(String* string, int peekPointer = -1)
        : string(string)
        , peekPointer(peekPointer)
    {
    }

    virtual int available() override
    {
        return string->length();
    }

    virtual int availableForWrite() override
    {
        return std::numeric_limits<int16_t>::max();
    }

    virtual int read() override
    {
        if (peekPointer < 0)
        {
            // consume chars
            if (string->length())
            {
                char c = string->charAt(0);
                string->remove(0, 1);
                return c;
            }
        }
        else if (peekPointer < (int)string->length())
        {
            // return pointed and move pointer
            return string->charAt(peekPointer++);
        }

        // everything is read
        return -1;
    }

    virtual size_t write(uint8_t data) override
    {
        return string->concat((char)data);
    }

    virtual int read(uint8_t* buffer, size_t len) override
    {
        if (peekPointer < 0)
        {
            // string will be consumed
            size_t l = std::min(len, (size_t)string->length());
            memcpy(buffer, string->c_str(), l);
            string->remove(0, l);
            return l;
        }

        if (peekPointer >= (int)string->length())
        {
            return 0;
        }

        // only the pointer is moved
        size_t l = std::min(len, (size_t)(string->length() - peekPointer));
        memcpy(buffer, string->c_str() + peekPointer, l);
        peekPointer += l;
        return l;
    }

    virtual size_t write(const uint8_t* buffer, size_t len) override
    {
        return string->concat((const char*)buffer, len) ? len : 0;
    }

    virtual int peek() override
    {
        if (peekPointer < 0)
        {
            if (string->length())
            {
                return string->charAt(0);
            }
        }
        else if (peekPointer < (int)string->length())
        {
            return string->charAt(peekPointer);
        }

        return -1;
    }

    virtual void flush() override
    {
        // nothing to do
    }

    virtual bool inputCanTimeout() override
    {
        return false;
    }

    virtual bool outputCanTimeout() override
    {
        return false;
    }

    //// Stream's peekBufferAPI

    virtual bool hasPeekBufferAPI() const override
    {
        return true;
    }

    virtual size_t peekAvailable()
    {
        if (peekPointer < 0)
        {
            return string->length();
        }
        return string->length() - peekPointer;
    }

    virtual const char* peekBuffer() override
    {
        if (peekPointer < 0)
        {
            return string->c_str();
        }
        if (peekPointer < (int)string->length())
        {
            return string->c_str() + peekPointer;
        }
        return nullptr;
    }

    virtual void peekConsume(size_t consume) override
    {
        if (peekPointer < 0)
        {
            // string is really consumed
            string->remove(0, consume);
        }
        else
        {
            // only the pointer is moved
            peekPointer = std::min((size_t)string->length(), peekPointer + consume);
        }
    }

    virtual ssize_t streamRemaining() override
    {
        return peekPointer < 0 ? string->length() : string->length() - peekPointer;
    }

    // calling setConsume() will consume bytes as the stream is read
    // (enabled by default)
    void setConsume()
    {
        peekPointer = -1;
    }

    // Reading this stream will mark the string as read without consuming
    // (not enabled by default)
    // Calling resetPointer() resets the read state and allows rereading.
    void resetPointer(int pointer = 0)
    {
        peekPointer = pointer;
    }

protected:
    String* string;
    int peekPointer;  // -1:String is consumed / >=0:resettable pointer
};

// StreamString is a S2Stream holding the String

class StreamString : public String, public S2Stream
{
protected:
    void resetpp()
    {
        if (peekPointer > 0)
        {
            peekPointer = 0;
        }
    }

public:
    StreamString(StreamString&& bro)
        : String(bro)
        , S2Stream(this)
    {
    }
    StreamString(const StreamString& bro)
        : String(bro)
        , S2Stream(this)
    {
    }

    // duplicate String constructors and operator=:

    StreamString(const char* text = nullptr)
        : String(text)
        , S2Stream(this)
    {
    }
    StreamString(const String& string)
        : String(string)
        , S2Stream(this)
    {
    }
    StreamString(const __FlashStringHelper* str)
        : String(str)
        , S2Stream(this)
    {
    }
    StreamString(String&& string)
        : String(string)
        , S2Stream(this)
    {
    }

    explicit StreamString(char c)
        : String(c)
        , S2Stream(this)
    {
    }
    explicit StreamString(unsigned char c, unsigned char base = 10)
        : String(c, base)
        , S2Stream(this)
    {
    }
    explicit StreamString(int i, unsigned char base = 10)
        : String(i, base)
        , S2Stream(this)
    {
    }
    explicit StreamString(unsigned int i, unsigned char base = 10)
        : String(i, base)
        , S2Stream(this)
    {
    }
    explicit StreamString(long l, unsigned char base = 10)
        : String(l, base)
        , S2Stream(this)
    {
    }
    explicit StreamString(unsigned long l, unsigned char base = 10)
        : String(l, base)
        , S2Stream(this)
    {
    }
    explicit StreamString(float f, unsigned char decimalPlaces = 2)
        : String(f, decimalPlaces)
        , S2Stream(this)
    {
    }
    explicit StreamString(double d, unsigned char decimalPlaces = 2)
        : String(d, decimalPlaces)
        , S2Stream(this)
    {
    }

    StreamString& operator=(const StreamString& rhs)
    {
        String::operator=(rhs);
        resetpp();
        return *this;
    }

    StreamString& operator=(const String& rhs)
    {
        String::operator=(rhs);
        resetpp();
        return *this;
    }

    StreamString& operator=(const char* cstr)
    {
        String::operator=(cstr);
        resetpp();
        return *this;
    }

    StreamString& operator=(const __FlashStringHelper* str)
    {
        String::operator=(str);
        resetpp();
        return *this;
    }

    StreamString& operator=(String&& rval)
    {
        String::operator=(rval);
        resetpp();
        return *this;
    }
};

#endif  // __STREAMSTRING_H
