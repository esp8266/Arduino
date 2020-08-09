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
#include <Print.h>
#include <PolledTimeout.h>
#include <sys/types.h> // ssize_t

// compatibility macros for testing
/*
 #define   getInt()            parseInt()
 #define   getInt(skipChar)    parseInt(skipchar)
 #define   getFloat()          parseFloat()
 #define   getFloat(skipChar)  parseFloat(skipChar)
 #define   getString( pre_string, post_string, buffer, length)
 readBytesBetween( pre_string, terminator, buffer, length)
 */

// Arduino `Client: public Stream` class defines `virtual int read(uint8_t *buf, size_t size) = 0;`
// This function is now imported into `Stream::` for `Stream::to*()`.
// Other classes inheriting from `Stream::` and implementing `read(uint8_t *buf, size_t size)`
// must consequently use `int` as return type, namely Hardware/SoftwareSerial, FileSystems...
#define STREAM_READ_RETURNS_INT 1

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
        unsigned long getTimeout () const { return _timeout; }

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

        virtual size_t readBytes(char *buffer, size_t length); // read chars from stream into buffer
        virtual size_t readBytes(uint8_t *buffer, size_t length) {
            return readBytes((char *) buffer, length);
        }
        // terminates if length characters have been read or timeout (see setTimeout)
        // returns the number of characters placed in the buffer (0 means no valid data found)
        // return data type: size_t

        size_t readBytesUntil(char terminator, char *buffer, size_t length); // as readBytes with terminator character
        size_t readBytesUntil(char terminator, uint8_t *buffer, size_t length) {
            return readBytesUntil(terminator, (char *) buffer, length);
        }
        // terminates if length characters have been read, timeout, or if the terminator character  detected
        // returns the number of characters placed in the buffer (0 means no valid data found)

        // Arduino String functions to be added here
        virtual String readString();
        String readStringUntil(char terminator);

        virtual int read (uint8_t* buffer, size_t len);
        int read (char* buffer, size_t len) { return read((uint8_t*)buffer, len); }

        //////////////////// extension: direct access to input buffer
        // to provide when possible a pointer to available data for read

        // informs user and ::to*() on effective buffered peek API implementation
        // by default: not available
        virtual bool peekBufferAPI () const { return false; }

        // returns number of byte accessible by peekBuffer()
        virtual size_t availableForPeek () { return 0; }

        // returns a pointer to available data buffer (size = availableForPeek())
        // semantic forbids any kind of ::read()
        //     - after calling peekBuffer()
        //     - and before calling peekConsume()
        virtual const char* peekBuffer () { return nullptr; }

        // consumes bytes after peekBuffer() use
        // (then ::read() is allowed)
        virtual void peekConsume (size_t consume) { (void)consume; }

        // by default read timeout is possible (incoming data from network,serial..)
        // children can override to false (like String::)
        virtual bool inputTimeoutPossible () { return true; }

        // (outputTimeoutPossible() is defined in Print::)

        //////////////////// extensions: Streaming streams to streams
        // Stream::to*()
        //
        // Stream::to*() uses 1-copy transfers when peekBuffer API is
        // available, or makes a regular transfer through a temporary buffer.
        //
        // - for efficiency, Stream classes should implement peekAPI when
        //   possible
        // - for an efficient timeout management, Print/Stream classes
        //   should implement {output,input}TimeoutPossible()

        using oneShotMs = esp8266::polledTimeout::oneShotFastMs;

        // ::to*() methods:
        // - always stop before timeout when "no-more-input-possible-data"
        //   or "no-more-output-possible-data" condition is met
        // - always return number of transfered bytes
        // When result is 0 or less than requested maxLen, this->getLastTo()
        // contains an error reason.

        // transfers already buffered / immediately available data (no timeout)
        // returns number of transfered bytes
        size_t toNow (Print* to) { return toFull(to, -1, -1, oneShotMs::alwaysExpired); }
        template <typename T>
        size_t toNow (T& to) { return toNow((Print*)&to); }

        // transfers data until timeout
        // returns number of transfered bytes
        size_t toAll (Print* to, const oneShotMs::timeType timeoutMs = oneShotMs::neverExpires) { return toFull(to, -1, -1, timeoutMs); }
        template <typename T>
        size_t toAll (T& to, const oneShotMs::timeType timeoutMs = oneShotMs::neverExpires) { return toAll((Print*)&to, timeoutMs); }

        // transfers data until a char is encountered (the char is also transfered) with timeout
        // returns number of transfered bytes
        size_t toUntil (Print* to, const int readUntilChar, const oneShotMs::timeType timeoutMs = oneShotMs::neverExpires) { return toFull(to, -1, readUntilChar, timeoutMs); }
        template <typename T>
        size_t toUntil (T& to, const int readUntilChar, const oneShotMs::timeType timeoutMs = oneShotMs::neverExpires) { return toUntil((Print*)&to, readUntilChar, timeoutMs); }

        // transfers data until requested size or timeout
        // returns number of transfered bytes
        size_t toSize (Print* to, const ssize_t maxLen, const oneShotMs::timeType timeoutMs = oneShotMs::neverExpires) { return toFull(to, maxLen, -1, timeoutMs); }
        template <typename T>
        size_t toSize (T& to, const ssize_t maxLen, const oneShotMs::timeType timeoutMs = oneShotMs::neverExpires) { return toSize((Print*)&to, maxLen, timeoutMs); }

        // size of input (-1 by default = unknown)
        virtual ssize_t streamSize () { return -1; }

        typedef enum
        {
            STREAMTO_SUCCESS = 0,
            STREAMTO_TIMED_OUT,
            STREAMTO_READ_ERROR,
            STREAMTO_WRITE_ERROR,
            STREAMTO_SHORT,
        } toReport_e;

        toReport_e getLastTo () /*const*/ { return (toReport_e)getWriteError(); }

        ////////////////////

    protected:
        size_t toFull (Print* to,
                       const ssize_t maxLen = -1,
                       const int readUntilChar = -1,
                       oneShotMs::timeType timeoutMs = oneShotMs::neverExpires /* neverExpires=>getTimeout() */);

        //////////////////// end of extensions

    protected:
        long parseInt(char skipChar); // as parseInt() but the given skipChar is ignored
        // this allows format characters (typically commas) in values to be ignored

        float parseFloat(char skipChar);  // as parseFloat() but the given skipChar is ignored
};

#endif
