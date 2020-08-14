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
using esp8266::polledTimeout::periodicFastMs;

size_t Stream::toFull(Print* to,
                      const ssize_t len,
                      const int readUntilChar,
                      const oneShotFastMs::timeType timeoutMs)
{
    setWriteError(STREAMTO_SUCCESS);

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

    // yield about every 5ms (XXX SHOULD BE A SYSTEM-WIDE CONSTANT?)
    periodicFastMs yieldNow(5);

    size_t written = 0;

    // len==-1 => maxLen=0 <=> until starvation
    size_t maxLen = std::max((ssize_t)0, len);

    if (peekBufferAPI())

        // peek-buffer API

        while (!maxLen || written < maxLen)
        {
            size_t avpk = availableForPeek();
            if (avpk == 0 && !inputTimeoutPossible())
                // no more data to read, ever
            {
                break;
            }

            size_t w = to->availableForWrite();
            if (w == 0 && !outputTimeoutPossible())
                // no more data can be written, ever
            {
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
                // nothing has been transferred and no specific condition is requested
            {
                break;
            }

            if (timedOut)
                // either (maxLen>0) nothing has been transferred for too long
                // or readUntilChar >= 0 but char is not encountered for too long
                // or (maxLen=0) too much time has been spent here
            {
                break;
            }

            if (yieldNow)
            {
                yield();
            }
        }

    else if (readUntilChar >= 0)

        // regular Stream API
        // no other choice than reading byte by byte

        while (!maxLen || written < maxLen)
        {
            size_t avpk = availableForPeek();
            if (avpk == 0 && !inputTimeoutPossible())
                // no more data to read, ever
            {
                break;
            }

            size_t w = to->availableForWrite();
            if (w == 0 && !outputTimeoutPossible())
                // no more data can be written, ever
            {
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
                    setWriteError(STREAMTO_WRITE_ERROR);
                    break;
                }
                written += 1;
                if (maxLen)
                {
                    timedOut.reset();
                }
            }

            if (!w && !maxLen && readUntilChar < 0)
                // nothing has been transferred and no specific condition is requested
            {
                break;
            }

            if (timedOut)
                // either (maxLen>0) nothing has been transferred for too long
                // or readUntilChar >= 0 but char is not encountered for too long
                // or (maxLen=0) too much time has been spent here
            {
                break;
            }

            if (yieldNow)
            {
                yield();
            }
        }

    else

        // regular Stream API
        // use an intermediary buffer

        while (!maxLen || written < maxLen)
        {
            size_t avr = available();
            if (avr == 0 && !inputTimeoutPossible())
                // no more data to read, ever
            {
                break;
            }

            size_t w = to->availableForWrite();
            if (w == 0 && !to->outputTimeoutPossible())
                // no more data can be written, ever
            {
                break;
            }

            w = std::min(w, avr);
            w = std::min(w, (decltype(w))64); //XXX FIXME 64 is a constant
            if (w)
            {
                char temp[w];
                ssize_t r = read(temp, w);
                if (r < 0)
                {
                    setWriteError(STREAMTO_READ_ERROR);
                    break;
                }
                w = to->write(temp, r);
                written += w;
                if ((size_t)r != w)
                {
                    setWriteError(STREAMTO_WRITE_ERROR);
                    break;
                }
                if (maxLen && w)
                {
                    timedOut.reset();
                }
            }

            if (!w && !maxLen && readUntilChar < 0)
                // nothing has been transferred and no specific condition is requested
            {
                break;
            }

            if (timedOut)
                // either (maxLen>0) nothing has been transferred for too long
                // or readUntilChar >= 0 but char is not encountered for too long
                // or (maxLen=0) too much time has been spent here
            {
                break;
            }

            if (yieldNow)
            {
                yield();
            }
        }

    if (getWriteError() == STREAMTO_SUCCESS && maxLen > 0)
    {
        if (timeoutMs && timedOut)
        {
            setWriteError(STREAMTO_TIMED_OUT);
        }
        else if ((ssize_t)written != len)
            // This is happening when source cannot timeout (ex: a String)
            // but has not enough data, or a dest has closed or cannot
            // timeout but is too small (String, buffer...)
            //
            // Mark it as an error because user usually wants to get what is
            // asked for.
        {
            setWriteError(STREAMTO_SHORT);
        }
    }
    return written;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_STREAMDEV)
StreamNull devnull;
#endif
