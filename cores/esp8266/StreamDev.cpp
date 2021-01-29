/*
    StreamDev.cpp - 1-copy transfer function
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


#include <Arduino.h>
#include <StreamDev.h>

using esp8266::polledTimeout::oneShotFastMs;

size_t Stream::sendGeneric(Print* to,
                           const ssize_t len,
                           const int readUntilChar,
                           const oneShotFastMs::timeType timeoutMs)
{
    _sendReport = STREAMSEND_SUCCESS;

    if (len == 0)
    {
        return 0;    // conveniently avoids timeout for no requested data
    }

    // There are two timeouts:
    // - read (network, serial, ...)
    // - write (network, serial, ...)
    // However
    // - getTimeout() is for reading only
    // - there is no getOutputTimeout() api
    // So we use getTimeout() for both,
    // (also when inputTimeoutPossible() is false)

    // "neverExpires (default, impossible)" is translated to default timeout
    oneShotFastMs timedOut(timeoutMs >= oneShotFastMs::neverExpires ? getTimeout() : timeoutMs);

    size_t written = 0;

    // len==-1 => maxLen=0 <=> until starvation
    size_t maxLen = std::max((ssize_t)0, len);

    if (peekBufferAPI())

        // peek-buffer API

        while (!maxLen || written < maxLen)
        {
            size_t avpk = peekAvailable();
            if (avpk == 0 && !inputTimeoutPossible())
            {
                // no more data to read, ever
                break;
            }

            size_t w = to->availableForWrite();
            if (w == 0 && !outputTimeoutPossible())
            {
                // no more data can be written, ever
                break;
            }

            w = std::min(w, avpk);
            if (maxLen)
            {
                w = std::min(w, maxLen - written);
            }
            if (w)
            {
                const char* directbuf = peekBuffer();
                bool foundChar = false;
                if (readUntilChar >= 0)
                {
                    const char* last = (const char*)memchr(directbuf, readUntilChar, w);
                    if (last)
                    {
                        w = std::min((size_t)(last - directbuf), w);
                        foundChar = true;
                    }
                }
                if (w && ((w = to->write(directbuf, w))))
                {
                    peekConsume(w);
                    written += w;
                    if (maxLen)
                    {
                        timedOut.reset();
                    }
                }
                if (foundChar)
                {
                    peekConsume(1);
                    break;
                }
            }

            if (!w && !maxLen && readUntilChar < 0)
            {
                // nothing has been transferred and no specific condition is requested
                break;
            }

            if (timedOut)
            {
                // either (maxLen>0) nothing has been transferred for too long
                // or readUntilChar >= 0 but char is not encountered for too long
                // or (maxLen=0) too much time has been spent here
                break;
            }

            optimistic_yield(1000);
        }

    else if (readUntilChar >= 0)

        // regular Stream API
        // no other choice than reading byte by byte

        while (!maxLen || written < maxLen)
        {
            size_t avpk = peekAvailable();
            if (avpk == 0 && !inputTimeoutPossible())
            {
                // no more data to read, ever
                break;
            }

            size_t w = to->availableForWrite();
            if (w == 0 && !outputTimeoutPossible())
            {
                // no more data can be written, ever
                break;
            }

            int c = read();
            if (c != -1)
            {
                if (c == readUntilChar)
                {
                    break;
                }
                w = to->write(c);
                if (w != 1)
                {
                    _sendReport = STREAMSEND_WRITE_ERROR;
                    break;
                }
                written += 1;
                if (maxLen)
                {
                    timedOut.reset();
                }
            }

            if (!w && !maxLen && readUntilChar < 0)
            {
                // nothing has been transferred and no specific condition is requested
                break;
            }

            if (timedOut)
            {
                // either (maxLen>0) nothing has been transferred for too long
                // or readUntilChar >= 0 but char is not encountered for too long
                // or (maxLen=0) too much time has been spent here
                break;
            }

            optimistic_yield(1000);
        }

    else

        // regular Stream API
        // use an intermediary buffer

        while (!maxLen || written < maxLen)
        {
            size_t avr = available();
            if (avr == 0 && !inputTimeoutPossible())
            {
                // no more data to read, ever
                break;
            }

            size_t w = to->availableForWrite();
            if (w == 0 && !to->outputTimeoutPossible())
                // no more data can be written, ever
            {
                break;
            }

            w = std::min(w, avr);
            if (maxLen)
            {
                w = std::min(w, maxLen - written);
            }
            w = std::min(w, (decltype(w))64); //XXX FIXME 64 is a constant
            if (w)
            {
                char temp[w];
                ssize_t r = read(temp, w);
                if (r < 0)
                {
                    _sendReport = STREAMSEND_READ_ERROR;
                    break;
                }
                w = to->write(temp, r);
                written += w;
                if ((size_t)r != w)
                {
                    _sendReport = STREAMSEND_WRITE_ERROR;
                    break;
                }
                if (maxLen && w)
                {
                    timedOut.reset();
                }
            }

            if (!w && !maxLen && readUntilChar < 0)
            {
                // nothing has been transferred and no specific condition is requested
                break;
            }

            if (timedOut)
            {
                // either (maxLen>0) nothing has been transferred for too long
                // or readUntilChar >= 0 but char is not encountered for too long
                // or (maxLen=0) too much time has been spent here
                break;
            }

            optimistic_yield(1000);
        }

    if (getWriteError() == STREAMSEND_SUCCESS && maxLen > 0)
    {
        if (timeoutMs && timedOut)
        {
            _sendReport = STREAMSEND_TIMED_OUT;
        }
        else if ((ssize_t)written != len)
        {
            // This is happening when source cannot timeout (ex: a String)
            // but has not enough data, or a dest has closed or cannot
            // timeout but is too small (String, buffer...)
            //
            // Mark it as an error because user usually wants to get what is
            // asked for.
            _sendReport = STREAMSEND_SHORT;
        }
    }
    return written;
}

Stream& operator << (Stream& out, String& string)
{
    StreamPtr(string).sendAll(out);
    return out;
}

Stream& operator << (Stream& out, StreamString& stream)
{
    stream.sendAll(out);
    return out;
}

Stream& operator << (Stream& out, Stream& stream)
{
    if (stream.streamSize() < 0)
    {
        if (stream.inputTimeoutPossible())
        {
            // restrict with only what's buffered on input
            stream.sendNow(out);
        }
        else
        {
            // take all what is in input
            stream.sendAll(out);
        }
    }
    else
    {
        stream.sendSize(out, stream.streamSize());
    }
    return out;
}

Stream& operator << (Stream& out, const char* text)
{
    StreamPtr(text).sendAll(out);
    return out;
}

Stream& operator << (Stream& out, const __FlashStringHelper* text)
{
    StreamPtr(text).sendAll(out);
    return out;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_STREAMDEV)
StreamNull devnull;
#endif
