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

#include <inttypes.h>
#include "Print.h"

// compatability macros for testing
/*
 #define   getInt()            parseInt()
 #define   getInt(skipChar)    parseInt(skipchar)
 #define   getFloat()          parseFloat()
 #define   getFloat(skipChar)  parseFloat(skipChar)
 #define   getString( pre_string, post_string, buffer, length)
 readBytesBetween( pre_string, terminator, buffer, length)
 */

#include <PolledTimeout.h>
using OneShotFastMs  = esp8266::polledTimeout::oneShotFastMs;
using PeriodicFastMs = esp8266::polledTimeout::periodicFastMs;
template <typename Tfrom, typename Tto>
size_t streamMove (Tfrom& from, Tto& to, size_t maxLen = 0, OneShotFastMs timeout = OneShotFastMs::alwaysExpired);

class Stream: public Print {
    protected:
        unsigned long _timeout;      // number of milliseconds to wait for the next char before aborting timed read
        unsigned long _startMillis;  // used for timeout measurement
        int timedRead();    // private method to read stream with timeout
        int timedPeek();    // private method to peek stream with timeout
        int peekNextDigit(); // returns the next numeric digit in the stream or -1 if timeout

    public:
        virtual int available() = 0;
        virtual int read() = 0;
        virtual int peek() = 0;

        Stream() {
            _timeout = 1000;
        }

// parsing methods

        void setTimeout(unsigned long timeout);  // sets maximum milliseconds to wait for stream data, default is 1 second

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

        // addition: directly access to read buffer
        // (currently implemented in HardwareSerial:: and WiFiClient::)

        // return a pointer to available data buffer (size = availableForPeek())
        // api availability when return value is not nullptr
        // semantic forbids any kind of read() before calling peekConsume()
        virtual const char* peekBuffer () { return nullptr; }

        // should be reimplemented as constexpr where relevant
        // for streamMove<> / STREAM_MOVE optimization
        bool peekBufferAvailableAPI () { return !!peekBuffer(); }

        // return number of byte accessible by peekBuffer()
        virtual size_t availableForPeek () { return 0; }

        // consume bytes after use (see peekBuffer)
        virtual void peekConsume (size_t consume) { (void)consume; }

        // additions: streamTo

        // transfer at most maxlen bytes (maxlen==0 means transfer until starvation)
        // immediate return number of transfered bytes (no timeout)
        // generic implementation using arduino virtual API
        // (also available: virtual-less template streamMove(from,to))
        virtual size_t streamTo (Print& to, size_t maxLen = 0);

    protected:
        long parseInt(char skipChar); // as above but the given skipChar is ignored
        // as above but the given skipChar is ignored
        // this allows format characters (typically commas) in values to be ignored

        float parseFloat(char skipChar);  // as above but the given skipChar is ignored
};

#include <assert.h>

#define STREAM_MOVE(from,to,...) (streamMove<decltype(from),decltype(to)>(from, to, ## __VA_ARGS__))

template <typename Tfrom, typename Tto>
size_t streamMove (Tfrom& from, Tto& to, size_t maxLen, OneShotFastMs timeout)
{
    static constexpr auto yield_ms = 100;
    static constexpr auto maxStreamToOnHeap = 128;
    PeriodicFastMs yieldNow(yield_ms);
    size_t written = 0;
    size_t w;
//////////////XXX use timeout
    if (from.peekBufferAvailableAPI())
        // direct buffer read API available, avoid one copy
        while ((!maxLen || written < maxLen) && (w = to.availableForWrite()))
        {
            const char* pb = from.peekBuffer();
            size_t r = from.availableForPeek();
            if (w > r)
                w = r;
            if (!w)
                return written;
            w = to.write(pb, w);
            from.peekConsume(w);

            written += w;

            if (yieldNow)
                yield();
        }
    else
        // use Stream blck-read/write API
        while ((!maxLen || written < maxLen) && (w = to.availableForWrite()))
        {
            size_t r = from.available();
            if (w > r)
                w = r;
            if (!w)
                return written;
            if (w > maxStreamToOnHeap)
                w = maxStreamToOnHeap;
            char temp[w];
            r = from.read(temp, w);
            w = to.write(temp, r);
            assert(w == r);

            written += w;

            if (yieldNow)
                yield();
        }

    return written;
}

#endif
