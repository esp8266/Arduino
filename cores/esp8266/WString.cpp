/*
 WString.cpp - String library for Wiring & Arduino
 ...mostly rewritten by Paul Stoffregen...
 Copyright (c) 2009-10 Hernando Barragan.  All rights reserved.
 Copyright 2011, Paul Stoffregen, paul@pjrc.com
 Modified by Ivan Grokhotkov, 2014 - esp8266 support
 Modified by Michael C. Miller, 2015 - esp8266 progmem support

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

#include "Arduino.h"
#include "WString.h"
#include "stdlib_noniso.h"

#include <limits>

/*********************************************/
/*  OOM Debugging                            */
/*********************************************/

#define OOM_STRING_BORDER_DISPLAY           10
#define OOM_STRING_THRESHOLD_REALLOC_WARN  128

#define __STRHELPER(x) #x
#define STR(x) __STRHELPER(x) // stringifier

/*********************************************/
/*  Conversion helpers                       */
/*********************************************/

static String toString(unsigned char value, unsigned char base) {
    String out;

    char buf[1 + std::numeric_limits<unsigned char>::digits];
    out = utoa(value, buf, base);

    return out;
}

static String toString(int value, unsigned char base) {
    String out;

    char buf[2 + std::numeric_limits<int>::digits];
    out = itoa(value, buf, base);

    return out;
}

static String toString(unsigned int value, unsigned char base) {
    String out;

    char buf[1 + std::numeric_limits<unsigned int>::digits];
    out = utoa(value, buf, base);

    return out;
}

static String toString(long value, unsigned char base) {
    String out;

    char buf[2 + std::numeric_limits<long>::digits];
    out = ltoa(value, buf, base);

    return out;
}

static String toString(unsigned long value, unsigned char base) {
    String out;

    char buf[1 + std::numeric_limits<unsigned long>::digits];
    out = ultoa(value, buf, base);

    return out;
}

// TODO: {u,}lltoa don't guarantee that the buffer is usable directly, one should always use the returned pointer

static String toString(long long value, unsigned char base) {
    String out;

    char buf[2 + std::numeric_limits<long long>::digits];
    out = lltoa(value, buf, sizeof(buf), base);

    return out;
}

static String toString(unsigned long long value, unsigned char base) {
    String out;

    char buf[1 + std::numeric_limits<unsigned long long>::digits];
    out = ulltoa(value, buf, sizeof(buf), base);

    return out;
}

static String toString(double value, unsigned char decimalPlaces) {
    String out;

    char buf[33];
    out = dtostrf(value, (decimalPlaces + 2), decimalPlaces, buf);

    return out;
}

static String toString(float value, unsigned char decimalPlaces) {
    return toString(static_cast<double>(value), decimalPlaces);
}

/*********************************************/
/*  Constructors                             */
/*********************************************/

String::String(const char *cstr) {
    init();
    if (cstr)
        copy(cstr, strlen_P(cstr));
}

String::String(const String &value) {
    init();
    *this = value;
}

String::String(String &&rval) noexcept {
    init();
    move(rval);
}

String::String(unsigned char value, unsigned char base) :
    String(toString(value, base))
{}

String::String(int value, unsigned char base) :
    String(toString(value, base))
{}

String::String(unsigned int value, unsigned char base) :
    String(toString(value, base))
{}

String::String(long value, unsigned char base) :
    String(toString(value, base))
{}

String::String(unsigned long value, unsigned char base) :
    String(toString(value, base))
{}

String::String(long long value, unsigned char base) :
    String(toString(value, base))
{}

String::String(unsigned long long value, unsigned char base) :
    String(toString(value, base))
{}

String::String(float value, unsigned char decimalPlaces) :
    String(toString(value, decimalPlaces))
{}

String::String(double value, unsigned char decimalPlaces) :
    String(toString(value, decimalPlaces))
{}

/*********************************************/
/*  Memory Management                        */
/*********************************************/

void String::invalidate(void) {
    if (!isSSO() && wbuffer())
        free(wbuffer());
    init();
}

bool String::reserve(unsigned int size) {
    if (buffer() && capacity() >= size)
        return true;
    if (changeBuffer(size)) {
        if (len() == 0)
            wbuffer()[0] = 0;
        return true;
    }
    return false;
}

#ifdef DEBUG_ESP_PORT
static void identifyString (const String& badOne)
{
    DEBUG_ESP_PORT.printf("[String] '%." STR(OOM_STRING_BORDER_DISPLAY) "s ... %." STR(OOM_STRING_BORDER_DISPLAY) "s': ",
        badOne.c_str(),
        badOne.length() > OOM_STRING_BORDER_DISPLAY? badOne.c_str() + std::max((int)badOne.length() - OOM_STRING_BORDER_DISPLAY, OOM_STRING_BORDER_DISPLAY): "");
}
#endif

bool String::changeBuffer(unsigned int maxStrLen) {
    // Can we use SSO here to avoid allocation?
    if (maxStrLen < sizeof(sso.buff) - 1) {
        if (isSSO() || !buffer()) {
            // Already using SSO, nothing to do
            uint16_t oldLen = len();
            setSSO(true);
            setLen(oldLen);
        } else { // if bufptr && !isSSO()
            // Using bufptr, need to shrink into sso.buff
            const char *temp = buffer();
            uint16_t oldLen = len();
            setSSO(true);
            setLen(oldLen);
            memcpy(wbuffer(), temp, maxStrLen);
            free((void *)temp);
        }
        return true;
    }
    // Fallthrough to normal allocator
    size_t newSize = (maxStrLen + 16) & (~0xf);
#ifdef DEBUG_ESP_PORT
    if (!isSSO() && capacity() >= OOM_STRING_THRESHOLD_REALLOC_WARN && maxStrLen > capacity()) {
        // warn when badly re-allocating
        identifyString(*this);
        DEBUG_ESP_PORT.printf("Reallocating large String(%d -> %d bytes)\n", len(), maxStrLen);
    }
#endif
    // Make sure we can fit newsize in the buffer
    if (newSize > CAPACITY_MAX) {
#ifdef DEBUG_ESP_PORT
        identifyString(*this);
        DEBUG_ESP_PORT.printf("Maximum capacity reached (" STR(CAPACITY_MAX) ")\n");
#endif
        return false;
    }
    uint16_t oldLen = len();
    char *newbuffer = (char *)realloc(isSSO() ? nullptr : wbuffer(), newSize);
    if (newbuffer) {
        size_t oldSize = capacity() + 1; // include NULL.
        if (isSSO()) {
            // Copy the SSO buffer into allocated space
            memmove_P(newbuffer, sso.buff, sizeof(sso.buff));
        }
        if (newSize > oldSize) {
            memset(newbuffer + oldSize, 0, newSize - oldSize);
        }
        setSSO(false);
        setCapacity(newSize - 1);
        setLen(oldLen); // Needed in case of SSO where len() never existed
        setBuffer(newbuffer);
        return true;
    }
#ifdef DEBUG_ESP_PORT
    identifyString(*this);
    DEBUG_ESP_PORT.printf("OOM: %d -> %zu bytes\n", isSSO() ? 0: capacity(), newSize);
#endif
    return false;
}

/*********************************************/
/*  Copy and Move                            */
/*********************************************/

String &String::copy(const char *cstr, unsigned int length) {
    if (!reserve(length)) {
        invalidate();
        return *this;
    }
    setLen(length);
    memmove_P(wbuffer(), cstr, length);
    wbuffer()[length] = 0;
    return *this;
}

void String::move(String &rhs) noexcept {
    invalidate();
    sso = rhs.sso;
    rhs.init();
}

String &String::operator =(const String &rhs) {
    if (this == &rhs)
        return *this;
    if (rhs.buffer())
        copy(rhs.buffer(), rhs.len());
    else
        invalidate();
    return *this;
}

String &String::operator =(String &&rval) noexcept {
    if (this != &rval)
        move(rval);
    return *this;
}

String &String::operator =(const char *cstr) {
    if (cstr)
        copy(cstr, strlen_P(cstr));
    else
        invalidate();
    return *this;
}

String &String::operator =(char c) {
    char buffer[2] { c, '\0' };
    *this = buffer;
    return *this;
}

/*********************************************/
/*  concat                                   */
/*********************************************/

bool String::concat(const String &s) {
    // Special case if we're concatting ourself (s += s;) since we may end up
    // realloc'ing the buffer and moving s.buffer in the method called
    if (this == &s) {
        unsigned int newlen = 2 * len();
        if (!s.buffer())
            return false;
        if (s.len() == 0)
            return true;
        if (!reserve(newlen))
            return false;
        memmove_P(wbuffer() + len(), buffer(), len());
        setLen(newlen);
        wbuffer()[newlen] = 0;
        return true;
    } else {
        return concat(s.buffer(), s.len());
    }
}

bool String::concat(const char *cstr, unsigned int length) {
    unsigned int newlen = len() + length;
    if (!cstr)
        return false;
    if (length == 0)
        return true;
    if (!reserve(newlen))
        return false;
    memmove_P(wbuffer() + len(), cstr, length);
    setLen(newlen);
    wbuffer()[newlen] = 0;
    return true;
}

bool String::concat(const char *cstr) {
    if (!cstr)
        return false;
    return concat(cstr, strlen_P(cstr));
}

bool String::concat(char c) {
    return concat(&c, 1);
}

bool String::concat(unsigned char num) {
    return concat(String(num));
}

bool String::concat(int num) {
    return concat(String(num));
}

bool String::concat(unsigned int num) {
    return concat(String(num));
}

bool String::concat(long num) {
    return concat(String(num));
}

bool String::concat(unsigned long num) {
    return concat(String(num));
}

bool String::concat(long long num) {
    return concat(String(num));
}

bool String::concat(unsigned long long num) {
    return concat(String(num));
}

bool String::concat(float num) {
    return concat(String(num));
}

bool String::concat(double num) {
    return concat(String(num));
}

/*********************************************/
/*  Insert                                   */
/*********************************************/

String &String::insert(size_t position, const char *other, size_t other_length) {
    if (position > length())
        return *this;

    auto len = length();
    auto total = len + other_length;
    if (!reserve(total))
        return *this;

    auto left = len - position;
    setLen(total);

    auto *start = wbuffer() + position;
    memmove(start + other_length, start, left);
    memmove_P(start, other, other_length);
    wbuffer()[total] = '\0';

    return *this;
}

String &String::insert(size_t position, char other) {
    char tmp[2] { other, '\0' };
    return insert(position, tmp, 1);
}

String &String::insert(size_t position, const char *other) {
    return insert(position, other, strlen(other));
}

String &String::insert(size_t position, const String &other) {
    return insert(position, other.c_str(), other.length());
}

String operator +(const String &lhs, String &&rhs) {
    String res;
    auto total = lhs.length() + rhs.length();
    if (rhs.capacity() > total) {
        rhs.insert(0, lhs);
        res = std::move(rhs);
    } else {
        res.reserve(total);
        res += lhs;
        res += rhs;
        rhs.invalidate();
    }

    return res;
}

String operator +(String &&lhs, String &&rhs) {
    String res;
    auto total = lhs.length() + rhs.length();
    if ((total > lhs.capacity()) && (total < rhs.capacity())) {
        rhs.insert(0, lhs);
        res = std::move(rhs);
    } else {
        lhs += rhs;
        rhs.invalidate();
        res = std::move(lhs);
    }

    return res;
}

String operator +(char lhs, const String &rhs) {
    String res;
    res.reserve(rhs.length() + 1);
    res += lhs;
    res += rhs;
    return res;
}

String operator +(const char *lhs, const String &rhs) {
    String res;

    const auto lhs_len = strlen_P(lhs);
    res.reserve(lhs_len + rhs.length());
    res.concat(lhs, lhs_len);
    res.concat(rhs);

    return res;
}

/*********************************************/
/*  Comparison                               */
/*********************************************/

int String::compareTo(const char *cstr, size_t length) const {
    const auto min_len = std::min(len(), length);
    int res = strncmp_P(buffer(), cstr, min_len);
    if (!res)
        return res;

    if (len() < length)
        return -1;
    if (len() > length)
        return 1;

    return 0;
}

int String::compareTo(const String &s) const {
    return compareTo(s.buffer(), s.len());
}

int String::compareTo(const char *cstr) const {
    return compareTo(cstr, strlen_P(cstr));
}

bool String::equals(const String &s) const {
    return equals(s.buffer(), s.len());
}

bool String::equals(const char *cstr) const {
    return equals(cstr, strlen_P(cstr));
}

bool String::equals(const char *cstr, size_t length) const {
    if (!cstr)
        return false;

    const auto same_length = len() == length;
    if (same_length && length == 0)
        return true;

    return same_length
        && strncmp_P(buffer(), cstr, length) == 0;
}

bool String::operator<(const String &rhs) const {
    return compareTo(rhs) < 0;
}
bool String::operator<(const char *rhs) const {
    return compareTo(rhs) < 0;
}

bool String::operator>(const String &rhs) const {
    return compareTo(rhs) > 0;
}
bool String::operator>(const char *rhs) const {
    return compareTo(rhs) > 0;
}

bool String::operator<=(const String &rhs) const {
    return compareTo(rhs) <= 0;
}
bool String::operator<=(const char *rhs) const {
    return compareTo(rhs) <= 0;
}

bool String::operator>=(const String &rhs) const {
    return compareTo(rhs) >= 0;
}
bool String::operator>=(const char *rhs) const {
    return compareTo(rhs) >= 0;
}

bool String::equalsIgnoreCase(const char *str, size_t length) const {
    if (len() != length)
        return false;
    if (!cstr || !length)
        return true;
    return strncasecmp_P(buffer(), cstr, length) == 0;
}

bool String::equalsIgnoreCase(const String &s) const {
    if (this == &s)
        return true;
    return equalsIgnoreCase(s.buffer(), s.len());
}

bool String::equalsIgnoreCase(const char *s) const {
    return equalsIgnoreCase(s, strlen_P(s));
}

unsigned char String::equalsConstantTime(const char *str, size_t length) const {
    // To avoid possible time-based attacks present function
    // compares given strings in a constant time.
    if (len() != length)
        return 0;
    //at this point lengths are the same
    if (length == 0)
        return 1;
    //at this point lengths are the same and non-zero
    const char *p1 = buffer();
    const char *p2 = str;
    unsigned int equalchars = 0;
    unsigned int diffchars = 0;
    while (*p1) {
        if (*p1 == pgm_read_byte(p2))
            ++equalchars;
        else
            ++diffchars;
        ++p1;
        ++p2;
    }
    //the following should force a constant time eval of the condition without a compiler "logical shortcut"
    unsigned char equalcond = (equalchars == len());
    unsigned char diffcond = (diffchars == 0);
    return (equalcond & diffcond); //bitwise AND
}

unsigned char String::equalsConstantTime(const String& s) const {
    return equalsConstantTime(s.buffer(), s.len());
}

bool String::startsWith(const char *str, size_t length, unsigned int offset) const {
    if (!buffer() || !str)
        return false;
    if (len() < length)
        return false;
    if (offset > (unsigned)(len() - length))
        return false;
    return strncmp_P(&buffer()[offset], str, length) == 0;
}

bool String::startsWith(const String &prefix) const {
    return startsWith(prefix, 0);
}

bool String::startsWith(const char *prefix) const {
    return startsWith(prefix, 0);
}

bool String::startsWith(const String &prefix, unsigned int offset) const {
    return startsWith(prefix.buffer(), prefix.len(), offset);
}

bool String::startsWith(const char *prefix, unsigned int offset) const {
    return startsWith(prefix, strlen_P(prefix), offset);
}

bool String::endsWith(const char *str, size_t length) const {
    if (!buffer() || !str)
        return false;
    if (len() < length)
        return false;
    return strncmp_P(&buffer()[len() - length], str, length) == 0;
}

bool String::endsWith(const String &suffix) const {
    return endsWith(suffix.buffer(), suffix.len());
}

bool String::endsWith(const char *suffix) const {
    return endsWith(suffix, strlen_P(suffix));
}

/*********************************************/
/*  Character Access                         */
/*********************************************/

void String::setCharAt(unsigned int loc, char c) {
    if (loc < len())
        wbuffer()[loc] = c;
}

char &String::operator[](unsigned int index) {
    static char dummy_writable_char;
    if (index >= len() || !buffer()) {
        dummy_writable_char = 0;
        return dummy_writable_char;
    }
    return wbuffer()[index];
}

char String::operator[](unsigned int index) const {
    if (index >= len() || !buffer())
        return '\0';
    return buffer()[index];
}

void String::toCharArray(char *buf, unsigned int bufsize, unsigned int index) const {
    if (!bufsize || !buf)
        return;
    if (index >= len()) {
        buf[0] = 0;
        return;
    }
    unsigned int n = bufsize - 1;
    if (n > len() - index)
        n = len() - index;
    strncpy(buf, buffer() + index, n);
    buf[n] = 0;
}

/*********************************************/
/*  Search                                   */
/*********************************************/

int String::indexOf(char ch, unsigned int fromIndex) const {
    if (fromIndex >= len())
        return -1;
    const char *temp = strchr(buffer() + fromIndex, ch);
    if (temp == NULL)
        return -1;
    return temp - buffer();
}

int String::indexOf(const char *s2, unsigned int fromIndex) const {
    if (fromIndex >= len())
        return -1;
    const char *found = strstr_P(buffer() + fromIndex, s2);
    if (found == NULL)
        return -1;
    return found - buffer();
}

int String::indexOf(const String &s2, unsigned int fromIndex) const {
    return indexOf(s2.buffer(), fromIndex);
}

int String::lastIndexOf(char ch) const {
    return lastIndexOf(ch, len() - 1);
}

int String::lastIndexOf(char ch, unsigned int fromIndex) const {
    if (fromIndex >= len())
        return -1;
    int index = fromIndex + 1;
    while (index-- > 0 && buffer()[index] != ch);
    return index;
}

int String::lastIndexOf(const char *str, size_t length, unsigned int fromIndex) const {
    if (!len() || !length || length > len())
        return -1;
    if (fromIndex >= len())
        fromIndex = len() - 1;
    int found = -1;
    for (const char *p = buffer(); p <= buffer() + fromIndex; p++) {
        p = strstr_P(p, str);
        if (!p)
            break;
        if ((unsigned int)(p - buffer()) <= fromIndex)
            found = p - buffer();
    }
    return found;
}

int String::lastIndexOf(const String &str) const {
    return lastIndexOf(str, len() - str.len());
}

int String::lastIndexOf(const String &str, unsigned int fromIndex) const {
    return lastIndexOf(str.buffer(), str.len(), fromIndex);
}

int String::lastIndexOf(const char *str) const {
    const auto length = strlen_P(str);
    return lastIndexOf(str, length, len() - length);
}

int String::lastIndexOf(const char *str, unsigned int fromIndex) const {
    return lastIndexOf(str, strlen_P(str), fromIndex);
}

String String::substring(unsigned int left, unsigned int right) const {
    if (left > right) {
        unsigned int temp = right;
        right = left;
        left = temp;
    }
    String out;
    if (left >= len())
        return out;
    if (right > len())
        right = len();
    out.concat(buffer() + left, right - left);
    return out;
}

/*********************************************/
/*  Modification                             */
/*********************************************/

void String::replace(char find, char replace) {
    if (!buffer())
        return;
    for (char *p = wbuffer(); *p; p++) {
        if (*p == find)
            *p = replace;
    }
}

void String::replace(const char *find, size_t find_len, const char *replace, size_t replace_len) {
    if (len() == 0 || find_len == 0)
        return;
    int diff = replace_len - find_len;
    const char *readFrom = buffer();
    const char *foundAt;
    if (diff == 0) {
        while ((foundAt = strstr_P(readFrom, find)) != NULL) {
            memmove_P(const_cast<char *>(foundAt), replace, replace_len);
            readFrom = foundAt + replace_len;
        }
    } else if (diff < 0) {
        char *writeTo = wbuffer();
        while ((foundAt = strstr_P(readFrom, find)) != NULL) {
            unsigned int n = foundAt - readFrom;
            memmove_P(writeTo, readFrom, n);
            writeTo += n;
            memmove_P(writeTo, replace, replace_len);
            writeTo += replace_len;
            readFrom = foundAt + find_len;
            setLen(len() + diff);
        }
        memmove_P(writeTo, readFrom, strlen(readFrom) + 1);
    } else {
        unsigned int size = len(); // compute size needed for result
        while ((foundAt = strstr_P(readFrom, find)) != NULL) {
            readFrom = foundAt + find_len;
            size += diff;
        }
        if (size == len())
            return;
        if (size > capacity() && !changeBuffer(size))
            return;
        int index = len() - 1;
        while (index >= 0 && (index = lastIndexOf(find, index)) >= 0) {
            readFrom = wbuffer() + index + find_len;
            memmove_P(const_cast<char *>(readFrom) + diff, readFrom, len() - (readFrom - buffer()));
            int newLen = len() + diff;
            memmove_P(wbuffer() + index, replace, replace_len);
            setLen(newLen);
            wbuffer()[newLen] = 0;
            index--;
        }
    }
}

void String::replace(const String &find, const String &replace) {
    this->replace(find.buffer(), find.len(), replace.buffer(), replace.len());
}
void String::replace(const String& find, const char *replace) {
    this->replace(find.buffer(), find.len(), replace, strlen_P(replace));
}
void String::replace(const char *find, const String &replace) {
    this->replace(find, strlen_P(find), replace.buffer(), replace.len());
}
void String::replace(const char *find, const char *replace) {
    this->replace(find, strlen_P(find), replace, strlen_P(replace));
}

void String::remove(unsigned int index, unsigned int count) {
    if (index >= len()) {
        return;
    }
    if (count <= 0) {
        return;
    }
    if (count > len() - index) {
        count = len() - index;
    }
    char *writeTo = wbuffer() + index;
    unsigned int newlen = len() - count;
    setLen(newlen);
    memmove_P(writeTo, wbuffer() + index + count, newlen - index);
    wbuffer()[newlen] = 0;
}

void String::toLowerCase(void) {
    if (!buffer())
        return;
    for (char *p = wbuffer(); *p; p++) {
        *p = tolower(*p);
    }
}

void String::toUpperCase(void) {
    if (!buffer())
        return;
    for (char *p = wbuffer(); *p; p++) {
        *p = toupper(*p);
    }
}

void String::trim(void) {
    if (!buffer() || len() == 0)
        return;
    char *begin = wbuffer();
    while (isspace(*begin))
        begin++;
    char *end = wbuffer() + len() - 1;
    while (isspace(*end) && end >= begin)
        end--;
    unsigned int newlen = end + 1 - begin;
    setLen(newlen);
    if (begin > buffer())
        memmove_P(wbuffer(), begin, newlen);
    wbuffer()[newlen] = 0;
}

/*********************************************/
/*  Parsing / Conversion                     */
/*********************************************/

long String::toInt(void) const {
    if (buffer())
        return atol(buffer());
    return 0;
}

float String::toFloat(void) const {
    if (buffer())
        return atof(buffer());
    return 0.0F;
}

double String::toDouble(void) const {
    if (buffer())
        return atof(buffer());
    return 0.0;
}

// global empty string to allow returning const String& with nothing

const String emptyString;
