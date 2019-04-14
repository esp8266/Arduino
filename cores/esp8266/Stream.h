/*
 Stream.h - base class for character-based streams.
 Copyright (c) 2010 David A. Mellis.  All right reserved.

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

#ifndef Stream_h
#define Stream_h

#include <debug.h>
#include <inttypes.h>
#include "Print.h"

// compatibility macros for testing
/*
 #define   getInt()            parseInt()
 #define   getInt(skipChar)    parseInt(skipchar)
 #define   getFloat()          parseFloat()
 #define   getFloat(skipChar)  parseFloat(skipChar)
 #define   getString( pre_string, post_string, buffer, length)
 readBytesBetween( pre_string, terminator, buffer, length)
 */

class Stream: public Print {
    protected:
        unsigned long _timeout;      // number of milliseconds to wait for the next char before aborting timed read
        unsigned long _startMillis;  // used for timeout measurement
        int timedRead();    // private method to read stream with timeout
        int timedPeek();    // private method to peek stream with timeout
        int peekNextDigit(); // returns the next numeric digit in the stream or -1 if timeout
        char oneChar;        // used by direct access extension

    public:
        virtual int available() = 0;
        virtual int read() = 0;
        virtual int peek() = 0;

        Stream() {
            _timeout = 1000;
        }

// parsing methods

        void setTimeout(unsigned long timeout);  // sets maximum milliseconds to wait for stream data, default is 1 second
        unsigned long getTimeout () { return _timeout; }

        bool find(const char *target);   // reads data from the stream until the target string is found
        bool find(uint8_t *target) {
            return find((char *) target);
        }
        // returns true if target string is found, false if timed out (see setTimeout)

        bool find(const char *target, size_t length);   // reads data from the stream until the target string of given length is found
        bool find(const uint8_t *target, size_t length) {
            return find((char *) target, length);
        }
        // returns true if target string is found, false if timed out

        bool find(char target) { return find (&target, 1); }

        bool findUntil(const char *target, const char *terminator);   // as find but search ends if the terminator string is found
        bool findUntil(const uint8_t *target, const char *terminator) {
            return findUntil((char *) target, terminator);
        }

        bool findUntil(const char *target, size_t targetLen, const char *terminate, size_t termLen);   // as above but search ends if the terminate string is found
        bool findUntil(const uint8_t *target, size_t targetLen, const char *terminate, size_t termLen) {
            return findUntil((char *) target, targetLen, terminate, termLen);
        }

        long parseInt(); // returns the first valid (long) integer value from the current position.
        // initial characters that are not digits (or the minus sign) are skipped
        // integer is terminated by the first character that is not a digit.

        float parseFloat();               // float version of parseInt

        // both arduino's "size_t Stream::readBytes(char*/uint8_t*, size_t):
        virtual size_t readBytes(char *buffer, size_t length); // read chars from stream into buffer
        virtual size_t readBytes(uint8_t *buffer, size_t length) {
            return readBytes((char *) buffer, length);
        }
        // terminates if length characters have been read or timeout (see setTimeout)
        // returns the number of characters placed in the buffer (0 means no valid data found)

        size_t readBytesUntil(char terminator, char *buffer, size_t length); // as readBytes with terminator character
        size_t readBytesUntil(char terminator, uint8_t *buffer, size_t length) {
            return readBytesUntil(terminator, (char *) buffer, length);
        }
        // terminates if length characters have been read, timeout, or if the terminator character  detected
        // returns the number of characters placed in the buffer (0 means no valid data found)

        // Arduino String functions to be added here
        virtual String readString();
        String readStringUntil(char terminator);

        // additions: Client:: read() definitions moved here, because HardwareSerial have them too

        // read at most maxLen bytes:
        // returns effectively transfered bytes (can be less than maxLen)
        // immediate return when no more data are available (no timeout)
        virtual int read(char* buffer, size_t maxLen);
        // arduino's "int Client::read(uint8_t*,size_t)":
        virtual int read(uint8_t* buffer, size_t maxLen) { return read((char*)buffer, maxLen); }

        //////////////////// extensions:

        // addition: directly access to read buffer
        // (currently implemented in HardwareSerial:: and WiFiClient::)

        // return number of byte accessible by peekBuffer()
        virtual size_t availableForPeek () { IAMSLOW(); return (oneChar = peek()) == -1? 0: 1; }

        // return a pointer to available data buffer (size = availableForPeek())
        // semantic forbids any kind of read() before calling peekConsume()
        virtual const char* peekBuffer () { return &oneChar; }

        // consume bytes after peekBuffer use
        virtual void peekConsume (size_t consume) { (void)read(); }

        // additions: streamTo

        // transfer at most maxlen bytes
        // return number of transfered bytes
        // generic implementation using arduino virtual API
        // (also available: virtual-less template streamMove(from,to))
        // - maxLen==0 will transfer until read starvation or destination temporarily full
        // - readUntilChar: using anything in 0..255 will stop transfer when this char is read
        // - timeout_ms==0: means immediate exit if nothing more can be read even if maxLen is not reached
        
        virtual size_t streamTo (Print& to, size_t maxLen = 0, int readUntilChar = -1, unsigned long timeout_ms = 0);

        //////////////////// end of extensions

    protected:
        long parseInt(char skipChar); // as above but the given skipChar is ignored
        // as above but the given skipChar is ignored
        // this allows format characters (typically commas) in values to be ignored

        float parseFloat(char skipChar);  // as above but the given skipChar is ignored
};

#define STREAM_MOVE(from,to,...) (streamMove<decltype(from),decltype(to)>(from, to, ## __VA_ARGS__))

#include <PolledTimeout.h>

template <typename Tfrom, typename Tto,
          typename PeriodicMs = esp8266::polledTimeout::periodicFastMs,
          typename TimeoutMs  = esp8266::polledTimeout::oneShotMs>
size_t streamMove (Tfrom& from, Tto& to, size_t maxLen = 0, int readUntilChar = -1, TimeoutMs::timeType timeout = TimeoutMs::alwayExpired)
{
    PeriodicMs yieldNow(100);
    size_t written = 0;
    size_t w;

    if (timeout == TimeoutMs::neverExpires)
        // neverExpires forbidden value => take Stream (source) value
        // TODO: check for to::getTimeout() existence (w/ SFINAE or equiv.)
        // and use std::min(from.getTimeout(), to.getTimeout())
        timeout.reset(from.getTimeout());
    else
        timeout.reset();

    while ((!maxLen || written < maxLen) && ((w = to.availableForWrite()) || !timeout)
    {
        size_t r = from.availableForPeek();
        if (w > r)
            w = r;
        if (w)
        {
            const char* directbuf = from.peekBuffer();
            if (readUntilChar >= 0)
            {
                const char* last = memchr(directbuf, readUntilChar, w);
                if (last)
                    w = last - directbuf + 1;
            }
            if (w && ((w = to.write(directbuf, w)))
            {
                from.peekConsume(w);
                written += w;
                timeout.reset();
            }
        }
        if (yieldNow)
            yield();
    }

    return written;
}

#endif
