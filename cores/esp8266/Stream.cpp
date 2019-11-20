/*
 Stream.cpp - adds parsing methods to Stream class
 Copyright (c) 2008 David A. Mellis.  All right reserved.

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

 Created July 2011
 parsing functions based on TextFinder library by Michael Margolis
 */

#include <Arduino.h>
#include <Stream.h>
#include <assert.h>

#define PARSE_TIMEOUT 1000  // default number of milli-seconds to wait
#define NO_SKIP_CHAR  1  // a magic char not found in a valid ASCII numeric field

// private method to read stream with timeout
int Stream::timedRead() {
    int c;
    _startMillis = millis();
    do {
        c = read();
        if(c >= 0)
            return c;
        if(_timeout == 0)
            return -1;
        yield();
    } while(millis() - _startMillis < _timeout);
    return -1;     // -1 indicates timeout
}

// private method to peek stream with timeout
int Stream::timedPeek() {
    int c;
    _startMillis = millis();
    do {
        c = peek();
        if(c >= 0)
            return c;
        if(_timeout == 0)
            return -1;
        yield();
    } while(millis() - _startMillis < _timeout);
    return -1;     // -1 indicates timeout
}

// returns peek of the next digit in the stream or -1 if timeout
// discards non-numeric characters
int Stream::peekNextDigit() {
    int c;
    while(1) {
        c = timedPeek();
        if(c < 0)
            return c;  // timeout
        if(c == '-')
            return c;
        if(c >= '0' && c <= '9')
            return c;
        read();  // discard non-numeric
    }
}

// Public Methods
//////////////////////////////////////////////////////////////

void Stream::setTimeout(unsigned long timeout)  // sets the maximum number of milliseconds to wait
{
    _timeout = timeout;
}

// find returns true if the target string is found
bool Stream::find(const char *target) {
    return findUntil(target, (char*) "");
}

// reads data from the stream until the target string of given length is found
// returns true if target string is found, false if timed out
bool Stream::find(const char *target, size_t length) {
    return findUntil(target, length, NULL, 0);
}

// as find but search ends if the terminator string is found
bool Stream::findUntil(const char *target, const char *terminator) {
    return findUntil(target, strlen(target), terminator, strlen(terminator));
}

// reads data from the stream until the target string of the given length is found
// search terminated if the terminator string is found
// returns true if target string is found, false if terminated or timed out
bool Stream::findUntil(const char *target, size_t targetLen, const char *terminator, size_t termLen) {
    size_t index = 0;  // maximum target string length is 64k bytes!
    size_t termIndex = 0;
    int c;

    if(*target == 0)
        return true;   // return true if target is a null string
    while((c = timedRead()) > 0) {

        if(c != target[index])
            index = 0; // reset index if any char does not match

        if(c == target[index]) {
            //////Serial.print("found "); Serial.write(c); Serial.print("index now"); Serial.println(index+1);
            if(++index >= targetLen) { // return true if all chars in the target match
                return true;
            }
        }

        if(termLen > 0 && c == terminator[termIndex]) {
            if(++termIndex >= termLen)
                return false;       // return false if terminate string found before target string
        } else
            termIndex = 0;
    }
    return false;
}

// returns the first valid (long) integer value from the current position.
// initial characters that are not digits (or the minus sign) are skipped
// function is terminated by the first character that is not a digit.
long Stream::parseInt() {
    return parseInt(NO_SKIP_CHAR); // terminate on first non-digit character (or timeout)
}

// as above but a given skipChar is ignored
// this allows format characters (typically commas) in values to be ignored
long Stream::parseInt(char skipChar) {
    boolean isNegative = false;
    long value = 0;
    int c;

    c = peekNextDigit();
    // ignore non numeric leading characters
    if(c < 0)
        return 0; // zero returned if timeout

    do {
        if(c == skipChar)
            ; // ignore this charactor
        else if(c == '-')
            isNegative = true;
        else if(c >= '0' && c <= '9')        // is c a digit?
            value = value * 10 + c - '0';
        read();  // consume the character we got with peek
        c = timedPeek();
    } while((c >= '0' && c <= '9') || c == skipChar);

    if(isNegative)
        value = -value;
    return value;
}

// as parseInt but returns a floating point value
float Stream::parseFloat() {
    return parseFloat(NO_SKIP_CHAR);
}

// as above but the given skipChar is ignored
// this allows format characters (typically commas) in values to be ignored
float Stream::parseFloat(char skipChar) {
    boolean isNegative = false;
    boolean isFraction = false;
    long value = 0;
    int c;
    float fraction = 1.0;

    c = peekNextDigit();
    // ignore non numeric leading characters
    if(c < 0)
        return 0; // zero returned if timeout

    do {
        if(c == skipChar)
            ; // ignore
        else if(c == '-')
            isNegative = true;
        else if(c == '.')
            isFraction = true;
        else if(c >= '0' && c <= '9') {      // is c a digit?
            value = value * 10 + c - '0';
            if(isFraction)
                fraction *= 0.1;
        }
        read();  // consume the character we got with peek
        c = timedPeek();
    } while((c >= '0' && c <= '9') || c == '.' || c == skipChar);

    if(isNegative)
        value = -value;
    if(isFraction)
        return value * fraction;
    else
        return value;
}

// read characters from stream into buffer
// terminates if length characters have been read, or timeout (see setTimeout)
// returns the number of characters placed in the buffer
// the buffer is NOT null terminated.
//
size_t Stream::readBytes(char *buffer, size_t length) {
    size_t count = 0;
    while(count < length) {
        int c = timedRead();
        if(c < 0)
            break;
        *buffer++ = (char) c;
        count++;
    }
    return count;
}

// as readBytes with terminator character
// terminates if length characters have been read, timeout, or if the terminator character  detected
// returns the number of characters placed in the buffer (0 means no valid data found)

size_t Stream::readBytesUntil(char terminator, char *buffer, size_t length) {
    if(length < 1)
        return 0;
    size_t index = 0;
    while(index < length) {
        int c = timedRead();
        if(c < 0 || c == terminator)
            break;
        *buffer++ = (char) c;
        index++;
    }
    return index; // return number of characters, not including null terminator
}

String Stream::readString() {
    String ret;
    int c = timedRead();
    while(c >= 0) {
        ret += (char) c;
        c = timedRead();
    }
    return ret;
}

String Stream::readStringUntil(char terminator) {
    String ret;
    int c = timedRead();
    while(c >= 0 && c != terminator) {
        ret += (char) c;
        c = timedRead();
    }
    return ret;
}

#if 0
size_t Stream::read (char* buffer, size_t maxLen)
{
    IAMSLOW();

    size_t nbread = 0;
    while (nbread < maxLen && available())
        buffer[nbread++] = read();
    return nbread;
}
#endif

size_t Stream::to (Print& to,
                   esp8266::polledTimeout::oneShotFastMs::timeType timeout,
                   size_t maxLen,
                   int readUntilChar)
{
    esp8266::polledTimeout::periodicFastMs yieldNow(100);
    esp8266::polledTimeout::oneShotFastMs timedOut(timeout);
    size_t written = 0;
    size_t w;
    
    if (timeout == esp8266::polledTimeout::oneShotFastMs::neverExpires)
        timeout = getTimeout();

    if (peekBufferAPI())

        // peek-buffer API

        while ((!maxLen || written < maxLen) && (w = to.availableForWrite()))
        {
            if (maxLen)
                w = std::min(w, maxLen - written);
            w = std::min(w, availableForPeek());
            if (w)
            {
                const char* directbuf = peekBuffer();
                bool ignore = false;
                if (readUntilChar >= 0)
                {
                    const char* last = (const char*)memchr(directbuf, readUntilChar, w);
                    if (last)
                    {
                        w = std::min((size_t)(last - directbuf + 1), w);
                        ignore = true;
                    }
                }
                if (w && ((w = to.write(directbuf, w))))
                {
                    peekConsume(w + ignore);
                    written += w;
                    timedOut.reset();
                }
            }
            else if (timedOut)
                break;
            if (yieldNow)
                yield();
        }

    else if (readUntilChar >= 0)

        // regular Stream API
        // no other choice than reading byte by byte

        while ((!maxLen || written < maxLen) && (w = to.availableForWrite()))
        {
            int c = read();
            if (c != -1)
            {
                if (c == readUntilChar)
                    break;
                w = to.write(c);
                assert(w);
                written += 1;
                timedOut.reset();
            }
            else if (timedOut)
                break;
            if (yieldNow)
                yield();
        }

    else
    
        // regular Stream API
        // use an intermediary buffer

        while ((!maxLen || written < maxLen) && (w = to.availableForWrite()))
        {
            w = std::min(w, (size_t)available());
            w = std::min(w, 64U);
            char temp[w];
            size_t r = read(temp, w);
            w = to.write(temp, r);
            assert(r == w);
            written += w;
            if (w)
                timedOut.reset();
            else if (timedOut)
                break;
            if (yieldNow)
                yield();
        }

    return written;
}
