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

#include "WString.h"

///////////////////////////////////////////////////////////////
#if STRING_IS_STREAM

// StreamString has been integrated into String
using StreamString = String;

#else // !STRING_IS_STREAM

///////////////////////////////////////////////////////////////
// using sstream helper as Stream and String pointer

class sstream: public Stream
{
public:

    sstream(String& string): string(&string)
    {
    }

    sstream(String* string): string(string)
    {
    }

    virtual int available() override
    {
        return string->length();
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
            // return pointed and move pointer
        {
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

    virtual size_t write(const uint8_t *buffer, size_t size) override
    {
        return string->concat((const char*)buffer, size) ? size : 0;
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

    virtual bool inputTimeoutPossible() override
    {
        return false;
    }
    virtual bool outputTimeoutPossible() override
    {
        return false;
    }

    //// Stream's peekBufferAPI

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
            // string is really consumed
        {
            string->remove(0, consume);
        }
        else
            // only the pointer is moved
        {
            peekPointer = std::min((size_t)string->length(), peekPointer + consume);
        }
    }

    // calling peekPointerSetConsume() will consume bytes as they are stream-read
    void peekPointerSetConsume()
    {
        peekPointer = -1;
    }
    void peekPointerReset(int pointer = 0)
    {
        peekPointer = pointer;
    }

protected:

    String* string;

    // peekPointer is used with peekBufferAPI,
    // on peekConsume(), chars can either:
    // - be really consumed = disappeared
    //   (case when peekPointer==-1)
    // - marked as read
    //   (peekPointer >=0 is increased)
    int peekPointer = 0;
};

// StreamString is a String and a sstream pointing to itself-as-String

class StreamString: public String, public sstream
{
public:
    StreamString(String&& string): String(string), sstream(this) { }
    StreamString(const String& string): String(string), sstream(this) { }
    StreamString(StreamString&& bro): String(bro), sstream(this) { }
    StreamString(const StreamString& bro): String(bro), sstream(this) { }
    StreamString(): String(), sstream(this) { }
};

#endif // !STRING_IS_STREAM

#endif // __STREAMSTRING_H
