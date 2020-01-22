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

        //////////////////// extension: readNow (is ::read() with unified signature)
        // (supposed to be internally used, and ephemeral)
        //
        // about ::read(buf, len): conflicting returned type:
        // - `int` in arduino's Client::
        // - `size_t` in esp8266 API (HardwareSerial::, FS::)
        // - not existent in arduino's Stream::
        // changing every read()/write() `size_t` return type to `int` will be a breaking change
        // => adding int ::readNow(buf, len) for now (following official `int Client::read(buf, len))`
        //
        // int ::readNow(buf, len)
        // read at most len bytes, returns effectively transferred bytes (can be less than 'len')
        // with no timeout: immediate return when no more data are available
        virtual int readNow (char* buffer, size_t len);
        virtual int readNow (uint8_t* buffer, size_t len) final { return readNow((char*)buffer, len); }

        //////////////////// extension: direct access to input buffer
        // for providing, when possible, a pointer to available data for read

        // informs user and ::to() on effective buffered peek API implementation
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
        // (outputTimeoutPossible() is defined in Print::)
        virtual bool inputTimeoutPossible () { return true; }

        //////////////////// extensions: Stream streams
        // Stream::to()
        //
        // Stream::to() uses 1-copy transfers when peekBuffer API is
        // available, or makes a regular transfer through a local temporary
        // stack 2-copies buffer.
        //
        // By default "source->to(&dest)" transfers everything until
        // available (read or write) gets to 0, then immediately returns.
        //
        // "source->to(&dest, maxLen)" is like above but also returns when
        // maxLen bytes are transferred, using the default Stream timeout.
        //
        // "source->to(&string, -1, '\n')" transfers source to string until
        // and including a specific character, with the default Stream
        // timeout.
        //
        // More generally ::to() will transfer as much as possible with the
        // following constraints:
        //      - at most maxLen bytes (-1 by default is no length
        //        constraint)
        //      - readUntilChar as last transferred byte (-1 by default is
        //        no last char contraint)
        //      - timeoutMs as maximum wait time (only if maxLen>=0 or
        //        readUntilChar>=0, immediate otherwise)
        //
        // timeoutMs value is by default "oneShotMs::neverExpires" which is
        // internally converted to this->getTimeout() but it can be set to
        // "oneShotMs::alwaysExpired" (=0) or any value within oneShotMs
        // allowed range.
        //
        // Return value:
        //      >0: the number of transferred bytes
        //      0:  nothing has been transferred
        // When result is 0 or less than requested maxLen, this->getLastTo()
        // may contain an error reason.
        //
        // Notes:
        // - readUntilChar is copied and counted
        // - for efficiency, Stream classes should implement peekAPI when
        //   possible
        // - for an efficient timeout management, Print/Stream classes
        //   should implement {output,input}TimeoutPossible()

        using oneShotMs = esp8266::polledTimeout::oneShotFastMs;

        size_t to (Print* to,
                   const ssize_t maxLen = -1,
                   int readUntilChar = -1,
                   oneShotMs::timeType timeoutMs = oneShotMs::neverExpires /* =>getTimeout() */);

        size_t to (Print& to,
                   const ssize_t maxLen = -1,
                   int readUntilChar = -1,
                   oneShotMs::timeType timeoutMs = oneShotMs::neverExpires /* =>getTimeout() */)
        {
            return this->to(&to, maxLen, readUntilChar, timeoutMs);
        }

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
        // size of input
        // -1 by default is unknown
        // may be used by http streamer (using a SerialStream as a file)

        virtual ssize_t size () { return -1; }

        //////////////////// end of extensions

    protected:
        long parseInt(char skipChar); // as above but the given skipChar is ignored
        // as above but the given skipChar is ignored
        // this allows format characters (typically commas) in values to be ignored

        float parseFloat(char skipChar);  // as above but the given skipChar is ignored
};

#endif
