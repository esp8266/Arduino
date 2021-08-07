/*
 WString.h - String library for Wiring & Arduino
 ...mostly rewritten by Paul Stoffregen...
 Copyright (c) 2009-10 Hernando Barragan.  All right reserved.
 Copyright 2011, Paul Stoffregen, paul@pjrc.com

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

#ifndef String_class_h
#define String_class_h
#ifdef __cplusplus

#include <pgmspace.h>

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cctype>

#include <utility>
#include <type_traits>

#ifdef _MSC_VER
#include <misc_wstring.h>
#else
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pgmspace.h>
#include "KFCBaseLibrary/include/misc_wstring.h"
#endif

 // an abstract class used as a means to proide a unique pointer type
 // but really has no body
class __FlashStringHelper;
#ifndef FPSTR
#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#endif
#ifndef F
#define F(string_literal) (FPSTR(PSTR(string_literal)))
#endif

// support libraries that expect this name to be available
// replace with `using StringSumHelper = String;` in case something wants this constructible
class StringSumHelper;

// The string class
class String {
    public:
        // constructors
        // creates a copy of the initial value.
        // if the initial value is null or invalid, or if memory allocation
        // fails, the string will be marked as invalid (i.e. "if (s)" will
        // be false).
        String() __attribute__((always_inline)) { // See init()
            init();
        }
        String(const char *cstr);
        String(const String &str);
        String(const __FlashStringHelper *str);
        String(String &&rval) noexcept;
        explicit String(char c) {
            sso.buff[0] = c;
            sso.buff[1] = 0;
            sso.len     = 1;
            sso.isHeap  = 0;
        }
        explicit String(unsigned char, unsigned char base = 10);
        explicit String(int, unsigned char base = 10);
        explicit String(unsigned int, unsigned char base = 10);
        explicit String(long, unsigned char base = 10);
        explicit String(unsigned long, unsigned char base = 10);
        explicit String(long long /* base 10 */);
        explicit String(long long, unsigned char base);
        explicit String(unsigned long long /* base 10 */);
        explicit String(unsigned long long, unsigned char base);
        explicit String(float, unsigned char decimalPlaces = 2);
        explicit String(double, unsigned char decimalPlaces = 2);
        ~String() {
            invalidate();
        }

        // memory management
        // return true on success, false on failure (in which case, the string
        // is left unchanged).  reserve(0), if successful, will validate an
        // invalid string (i.e., "if (s)" will be true afterwards)
        bool reserve(unsigned int size);
        unsigned int length(void) const {
            return buffer() ? len() : 0;
        }
        void clear(void) {
            setLen(0);
        }
        bool isEmpty(void) const {
            return length() == 0;
        }

        // creates a copy of the assigned value.  if the value is null or
        // invalid, or if the memory allocation fails, the string will be
        // marked as invalid ("if (s)" will be false).
        String &operator =(const String &rhs);
        String &operator =(const char *cstr);
        String &operator =(const __FlashStringHelper *str);
        String &operator =(String &&rval) noexcept;
        String &operator =(char c) {
            char buffer[2] { c, '\0' };
            *this = buffer;
            return *this;
        }

        // concatenate (works w/ built-in types)

        // returns true on success, false on failure (in which case, the string
        // is left unchanged).  if the argument is null or invalid, the
        // concatenation is considered unsuccessful.
        bool concat(const String &str);
        bool concat(const char *cstr);
        bool concat(char c);
        bool concat(unsigned char c);
        bool concat(int num);
        bool concat(unsigned int num);
        bool concat(long num);
        bool concat(unsigned long num);
        bool concat(long long num);
        bool concat(unsigned long long num);
        bool concat(float num);
        bool concat(double num);
        bool concat(const __FlashStringHelper *str);
        bool concat(const char *cstr, unsigned int length);

        // if there's not enough memory for the concatenated value, the string
        // will be left unchanged (but this isn't signalled in any way)
        template <typename T>
        String &operator +=(const T &rhs) {
            concat(rhs);
            return *this;
        }

        explicit operator bool() const {
            return buffer() != nullptr;
        }

        int compareTo(const String &s) const;
        bool operator ==(const String &rhs) const {
            return equals(rhs);
        }
        bool operator ==(const char *cstr) const {
            return equals(cstr);
        }
        bool operator !=(const String &rhs) const {
            return !equals(rhs);
        }
        bool operator !=(const char *cstr) const {
            return !equals(cstr);
        }
        bool operator <(const String &rhs) const;
        bool operator >(const String &rhs) const;
        bool operator <=(const String &rhs) const;
        bool operator >=(const String &rhs) const;
        unsigned char equalsConstantTime(const String &s) const;

    // additional operators for char and const __FashStringHelper *
    public:
        bool operator ==(char ch) const {
            return equals(ch);
        }
        bool operator ==(const __FlashStringHelper *fstr) const {
            return strcmp_P(buffer(), reinterpret_cast<PGM_P>(fstr)) == 0;
        }
        bool operator !=(char ch) const {
            return !equals(ch);
        }
        bool operator !=(const __FlashStringHelper *fstr) const {
            return strcmp_P(buffer(), reinterpret_cast<PGM_P>(fstr)) != 0;
        }

    public:

        // character access
        char charAt(unsigned int index) const {
            return operator [](index);
        }
        void setCharAt(unsigned int index, char c);
        char operator [](unsigned int index) const;
        char &operator [](unsigned int index);
        void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index = 0) const;
        void toCharArray(char *buf, unsigned int bufsize, unsigned int index = 0) const {
            getBytes((unsigned char *) buf, bufsize, index);
        }
        const char *c_str() const { return buffer(); }
        char *begin() { return wbuffer(); }
        char *end() { return wbuffer() + length(); }
        const char *begin() const { return c_str(); }
        const char *end() const { return c_str() + length(); }

        // search
        String substring(unsigned int beginIndex) const {
            return substring(beginIndex, len());
        }
        String substring(unsigned int beginIndex, unsigned int endIndex) const;

        // Pass the biggest integer if the count is not specified.
        // The remove method below will take care of truncating it at the end of the string.
        void remove(unsigned int index, unsigned int count = (unsigned int)-1);

        // added return type
        String &toLowerCase(void);
        String &toUpperCase(void);

        // parsing/conversion
        long toInt(void) const;
        float toFloat(void) const;
        double toDouble(void) const;

    // additional equals methods with ignorecase and support for char, const char *, const __FlashStringHelper * and String
    public:
        //
        // equals()
        //
        // changed behaviour:
        // any nullptr argument will return false -> String.equals(nullptr) does not return true if String is empty

        // the terminating NUL byte is not considered part of the string
        // String.equals('\0') always returns false, even if String is empty
        // use length() == 0 to determine if a string is empty
        bool equals(char ch) const {
            if (length() != 1 || !ch) {
                return false;
            }
            return buffer()[0] == ch;
        }

        bool equals(const String &str) const {
            if (this == &str) {
                return true;
            }
            if (length() != str.length()) {
                return false;
            }
            return strcmp(buffer(), str.buffer()) == 0;
        }

        bool equals(const char *cStr) const {
            if (!cStr) {
                return false;
            }
            if (length() == 0) {
                return (*cStr == 0);
            }
            return strcmp(buffer(), cStr) == 0;
        }

        bool equals(const __FlashStringHelper *fStr) const {
            if (!fStr) {
                return false;
            }
            return strcmp_P(buffer(), reinterpret_cast<PGM_P>(fStr)) == 0;
        }

        // return true if String matches the end of str1
        // basically a.endsWith(b) = b.endEquals(a)
        //
        // String("123").endEquals("test123") == true
        // String("").endEquals(nullptr) == false
        bool endEquals(const char *str1) const {
            size_t len1;
            size_t len2;
            if (!str1 || (len2 = length()) > (len1 = strlen(str1))) {
                return false;
            }
            if (len2 == len1) {
                return strcmp(str1, buffer()) == 0;
            }
            return strcmp(str1 + len1 - len2, buffer()) == 0;
        }

        bool endEquals(const __FlashStringHelper *fStr) const {
            PGM_P str1 = reinterpret_cast<PGM_P>(fStr);
            size_t len1;
            size_t len2;
            if (!str1 || (len2 = length()) > (len1 = strlen_P(str1))) {
                return false;
            }
            if (len2 == len1) {
                return strcmp_P_P(str1, buffer()) == 0;
            }
            return strcmp_P_P(str1 + len1 - len2, buffer()) == 0;
        }

        // we can use endsWith in this case
        inline bool endEquals(const String &str) const {
            return str.endsWith(*this);
        }

        // return true if String matches the end of str1
        // basically a.endsWith(b) = b.endEquals(a)
        //
        // String("abc").endEqualsIgnoreCase("TESTABC") == true
        // String("").endEqualsIgnoreCase(nullptr) == false
        bool endEqualsIgnoreCase(const char *str1) const {
            size_t len1;
            size_t len2;
            if (!str1 || (len2 = length()) > (len1 = strlen(str1))) {
                return false;
            }
            if (len2 == len1) {
                return strcasecmp(str1, buffer()) == 0;
            }
            return strcasecmp(str1 + len1 - len2, buffer()) == 0;
        }

        bool endEqualsIgnoreCase(const __FlashStringHelper *fStr) const {
            PGM_P str1 = reinterpret_cast<PGM_P>(fStr);
            size_t len1;
            size_t len2;
            if (!str1 || (len2 = length()) > (len1 = strlen_P(str1))) {
                return false;
            }
            if (len2 == len1) {
                return strcasecmp_P_P(str1, buffer()) == 0;
            }
            return strcasecmp_P_P(str1 + len1 - len2, buffer()) == 0;
        }

        // we can just use endsWithIgnoreCase in this case
        inline bool endEqualsIgnoreCase(const String &str) const {
            return str.endsWithIgnoreCase(*this);
        }

        //
        // equals() with offset
        //

        unsigned char equals(const String &str, size_t offset) const {
            size_t len1;
            size_t len2;
            if (((len1 = length()) == 0) || (offset >= len1) || ((len2 = str.length()) == 0) || ((len1 + offset) != len2)) {
                return false;
            }
            return strcmp(buffer() + offset, str.c_str()) == 0;
        }

        unsigned char equals(const char *cStr, size_t offset) const {
            if (!cStr || !*cStr || offset >= length()) {
                return false;
            }
            return strcmp(buffer() + offset, cStr) == 0;
        }

        unsigned char equals(const __FlashStringHelper *fStr, size_t offset) const {
            if (!fStr || offset >= length()) {
                return false;
            }
            return strcmp_P(buffer() + offset, reinterpret_cast<PGM_P>(fStr)) == 0;
        }

        //
        // equalsIgnoreCase() with offset
        //

        unsigned char equalsIgnoreCase(const String &str, size_t offset = 0) const {
            if (offset == 0 && this == &str) {
                return true;
            }
            size_t len1;
            size_t len2;
            if (((len1 = length()) == 0) || (offset >= len1) || ((len2 = str.length()) == 0) || ((len1 + offset) != len2)) {
                return false;
            }
            return strcasecmp(buffer() + offset, str.c_str()) == 0;
        }

        unsigned char equalsIgnoreCase(const char *cStr, size_t offset = 0) const {
            if (!cStr || !*cStr || offset >= length()) {
                return false;
            }
            return strcasecmp(buffer() + offset, cStr) == 0;
        }

        unsigned char equalsIgnoreCase(const __FlashStringHelper *fStr, size_t offset = 0) const {
            if (!fStr || offset >= length()) {
                return false;
            }
            return strcasecmp_P(buffer() + offset, reinterpret_cast<PGM_P>(fStr)) == 0;
        }

    // additional startWith methods with ignorecase and support for char, const char *, const __FlashStringHelper * and String
    public:

        //
        // _startsWith
        //

        inline  unsigned char _startsWith(const char *prefix, size_t prefixLen, size_t offset) const {
            return prefixLen && (length() >= prefixLen + offset) && !strncmp(c_str() + offset, prefix, prefixLen);
        }
        inline unsigned char _startsWith_P(PGM_P prefix, size_t prefixLen, size_t offset) const {
            return prefixLen && (length() >= prefixLen + offset) && !strncmp_P(c_str() + offset, prefix, prefixLen);
        }
        inline unsigned char _startsWithIgnoreCase(const char *prefix, size_t prefixLen, size_t offset) const {
            return prefixLen && (length() >= prefixLen + offset) && !strncasecmp(c_str() + offset, prefix, prefixLen);
        }
        inline unsigned char _startsWithIgnoreCase_P(PGM_P prefix, size_t prefixLen, size_t offset) const {
            return prefixLen && (length() >= prefixLen + offset) && !strncasecmp_P(c_str() + offset, prefix, prefixLen);
        }

        //
        // startsWith(<char|const char *|const String &|const __FlashStringHelper *>[,offset]])
        //

        unsigned char startsWith(char ch) const {
            return (length() != 0) && (buffer()[0] == ch);
        }
        unsigned char startsWith(char ch, size_t offset) const {
            return (length() > offset) && (buffer()[offset] == ch);
        }
        unsigned char startsWith(const char *prefix, size_t offset = 0) const {
            return prefix && _startsWith(prefix, strlen(prefix), offset);
        }
        unsigned char startsWith(const String &prefix, size_t offset = 0) const {
            size_t len;
            return ((len = prefix.length())) != 0 && _startsWith(prefix.buffer(), len, offset);
        }
        unsigned char startsWith(const __FlashStringHelper *prefix, size_t offset = 0) const {
            return prefix && _startsWith_P(reinterpret_cast<PGM_P>(prefix), strlen_P(reinterpret_cast<PGM_P>(prefix)), offset);
        }

        //
        // startsWithIgnoreCase(<char|const char *|const String &|const __FlashStringHelper *>[,offset]])
        //

        unsigned char startsWithIgnoreCase(char ch, size_t offset = 0) const {
            return (length() > offset) && (tolower(buffer()[offset]) == tolower(ch));
        }
        unsigned char startsWithIgnoreCase(const char *prefix, size_t offset = 0) const {
            return prefix && _startsWithIgnoreCase(prefix, strlen(prefix), offset);
        }
        unsigned char startsWithIgnoreCase(const String &prefix, size_t offset = 0) const {
            size_t len;
            return ((len = prefix.length())) != 0 && _startsWithIgnoreCase(prefix.buffer(), len, offset);
        }
        unsigned char startsWithIgnoreCase(const __FlashStringHelper *prefix, size_t offset = 0) const {
            return prefix && _startsWithIgnoreCase_P(reinterpret_cast<PGM_P>(prefix), strlen_P(reinterpret_cast<PGM_P>(prefix)), offset);
        }

    protected:
        //
        // _endsWith
        //

        inline __attribute__((__always_inline__))
            unsigned char _endsWith(const char *suffix, size_t suffixLen) const {
            size_t len;
            return suffixLen && ((len = length()) >= suffixLen) && !strcmp(buffer() + len - suffixLen, suffix);
        }
        inline __attribute__((__always_inline__))
            unsigned char _endsWith_P(PGM_P suffix, size_t suffixLen) const {
            size_t len;
            return suffixLen && ((len = length()) >= suffixLen) && !strcmp_P(buffer() + len - suffixLen, suffix);
        }
        inline __attribute__((__always_inline__))
            unsigned char _endsWithIgnoreCase(const char *suffix, size_t suffixLen) const {
            size_t len;
            return suffixLen && ((len = length()) >= suffixLen) && !strcasecmp(buffer() + len - suffixLen, suffix);
        }
        inline __attribute__((__always_inline__))
            unsigned char _endsWithIgnoreCase_P(PGM_P suffix, size_t suffixLen) const {
            size_t len;
            return suffixLen && ((len = length()) >= suffixLen) && !strcasecmp_P(buffer() + len - suffixLen, suffix);
        }


    // additional endsWith methods with ignorecase and support for char, const char *, const __FlashStringHelper * and String
    public:
        //
        // endsWith
        //

        unsigned char endsWith(char ch) const {
            auto len = length();
            return (len != 0) && (buffer()[len - 1] == ch);
        }
        unsigned char endsWith(const String &suffix) const {
            return _endsWith(suffix.buffer(), suffix.length());
        }
        unsigned char endsWith(const char *suffix) const {
            return suffix && _endsWith(suffix, strlen(suffix));
        }
        unsigned char endsWith(const __FlashStringHelper *suffix) const {
            return suffix && _endsWith_P(reinterpret_cast<PGM_P>(suffix), strlen_P(reinterpret_cast<PGM_P>(suffix)));
        }

        //
        // endsWithIgnoreCase
        //

        unsigned char endsWithIgnoreCase(char ch) const {
            auto len = length();
            return (len != 0) && (tolower(buffer()[len - 1]) == tolower(ch));
        }
        unsigned char endsWithIgnoreCase(const String &suffix) const {
            return _endsWithIgnoreCase(suffix.buffer(), suffix.length());
        }
        unsigned char endsWithIgnoreCase(const char *suffix) const {
            return suffix && _endsWithIgnoreCase(suffix, strlen(suffix));
        }
        unsigned char endsWithIgnoreCase(const __FlashStringHelper *suffix) const {
            return suffix && _endsWithIgnoreCase_P(reinterpret_cast<PGM_P>(suffix), strlen_P(reinterpret_cast<PGM_P>(suffix)));
        }


        // // search
        // int indexOf(char ch, unsigned int fromIndex = 0) const;
        // int indexOf(const char *str, unsigned int fromIndex = 0) const;
        // int indexOf(const __FlashStringHelper *str, unsigned int fromIndex = 0) const {
        //     return indexOf((const char*)str, fromIndex);
        // }
        // int indexOf(const String &str, unsigned int fromIndex = 0) const;
        // int lastIndexOf(char ch) const;
        // int lastIndexOf(char ch, unsigned int fromIndex) const;
        // int lastIndexOf(const String &str) const;
        // int lastIndexOf(const String &str, unsigned int fromIndex) const;

    // internal search functions
    protected:
        // findLength is optional, provide only if available
        int _indexOf(const char *find, size_t fromIndex, size_t findLength = ~0) const
        {
            size_t len;
            if (!find || !findLength || ((len = length()) == 0) || (findLength != ~0U && (fromIndex + findLength >= len))) {
                return -1;
            }
            auto ptr = buffer();
            auto idxPtr = strstr(ptr + fromIndex, find);
            if (!idxPtr) {
                return -1;
            }
            return idxPtr - ptr;
        }

        int _indexOfIgnoreCase(const char *find, size_t fromIndex, size_t findLength = ~0) const
        {
            size_t len;
            if (!find || !findLength || ((len = length()) == 0) || (findLength != ~0U && (fromIndex + findLength >= len))) {
                return -1;
            }
            auto ptr = buffer();
            auto idxPtr = stristr(ptr + fromIndex, find);
            if (!idxPtr) {
                return -1;
            }
            return idxPtr - ptr;
        }

        int _indexOf_P(PGM_P find, size_t fromIndex, size_t findLength = ~0) const
        {
            size_t len;
            if (!find || !findLength || ((len = length()) == 0) || (findLength != ~0U && (fromIndex + findLength >= len))) {
                return -1;
            }
            auto ptr = buffer();
            auto idxPtr = strstr_P(ptr + fromIndex, find);
            if (!idxPtr) {
                return -1;
            }
            return idxPtr - ptr;
        }

        int _indexOfIgnoreCase_P(PGM_P find, size_t fromIndex, size_t findLength) const
        {
            size_t len;
            if (!find || !findLength || ((len = length()) == 0) || (fromIndex + findLength >= len)) {
                return -1;
            }
            auto ptr = buffer();
            auto idxPtr = stristr_P(const_cast<char *>(ptr) + fromIndex, find, findLength);
            if (!idxPtr) {
                return -1;
            }
            return idxPtr - ptr;
        }

        int _lastIndexOf(char find) const
        {
            auto ptr = strrchr(buffer(), find);
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf_P(char find) const
        {
            auto ptr = strrchr_P(buffer(), find);
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf(char find, size_t fromIndex) const
        {
            if (!find) {
                return -1;
            }
            auto len = length();
            if (fromIndex == ~0U) {
                fromIndex = len;
            }
            else if (fromIndex > len || fromIndex < 1) {
                return -1;
            }
            auto ptr = reinterpret_cast<const char *>(memrchr(buffer(), find, fromIndex));
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf_P(char find, size_t fromIndex) const
        {
            if (!find) {
                return -1;
            }
            auto len = length();
            if (fromIndex == ~0U) {
                fromIndex = len;
            }
            else if (fromIndex > len || fromIndex < 1) {
                return -1;
            }
            auto ptr = reinterpret_cast<const char *>(memrchr(buffer(), find, fromIndex));
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf_P(PGM_P find, size_t fromIndex, size_t findLen) const
        {
            size_t len;
            if (!find || !(len = length())) {
                return -1;
            }
            if (fromIndex == ~0U) {
                fromIndex = len;
            }
            else if (fromIndex < findLen || fromIndex > len) {
                return -1;
            }
            auto ptr = __strrstr_P(const_cast<char *>(buffer()), fromIndex + findLen, find, findLen);
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf(const char *find, size_t fromIndex, size_t findLen) const
        {
            size_t len;
            if (!find || !(len = length())) {
                return -1;
            }
            if (fromIndex == ~0U) {
                fromIndex = len;
            }
            else if (fromIndex < findLen || fromIndex > len) {
                return -1;
            }
            auto ptr = __strrstr(const_cast<char *>(buffer()), fromIndex + findLen, find, findLen);
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }


    // additional indexOf methods with ignore case and support for char, const char *, const __FlashStringHelper * and String
    public:
        // search
        int indexOf(char ch, unsigned int fromIndex = 0) const;
        int indexOf(const String &str) const;
        int indexOf(const String &str, unsigned int fromIndex) const;

        int indexOf(const char *str, unsigned int fromIndex = 0) const {
            return _indexOf(str, fromIndex, ~0U);
        }
        int indexOf(const __FlashStringHelper *fstr, unsigned int fromIndex = 0) const {
            return _indexOf_P(reinterpret_cast<PGM_P>(fstr), fromIndex, ~0U);
        }


        int indexOfIgnoreCase(char ch, unsigned int fromIndex = 0) const {
            if (fromIndex >= len())
                return -1;
            const char *temp = strichr(buffer() + fromIndex, ch);
            if (temp == NULL)
                return -1;
            return temp - buffer();
        }


        int indexOfIgnoreCase(const char *str, unsigned int fromIndex = 0) const {
            return _indexOfIgnoreCase(str, fromIndex, ~0U);
        }
        int indexOfIgnoreCase(const String &str, unsigned int fromIndex = 0) const {
            return _indexOfIgnoreCase(str.c_str(), fromIndex, str.length());
        }
        int indexOfIgnoreCase(const __FlashStringHelper *fstr, unsigned int fromIndex = 0) const {
            return _indexOfIgnoreCase_P(reinterpret_cast<PGM_P>(fstr), fromIndex, ~0U);
        }


        int lastIndexOf(char ch) const {
            return _lastIndexOf(ch);
        }
        int lastIndexOf(char ch, unsigned int fromIndex) const {
            return _lastIndexOf(ch, fromIndex);
        }
        int lastIndexOf(const String &str) const {
            auto strLen = str.length();
            return _lastIndexOf(str.buffer(), length() - strLen, strLen);
        }
        int lastIndexOf(const String &str, unsigned int fromIndex) const {
            auto findLength = str.length();
            return _lastIndexOf(str.buffer(), fromIndex - findLength, findLength);
        }
        int lastIndexOf(const __FlashStringHelper *str, unsigned int fromIndex) const {
            auto findLength = strlen_P(reinterpret_cast<PGM_P>(str));
            return _lastIndexOf_P(reinterpret_cast<PGM_P>(str), fromIndex - findLength, findLength);
        }


    // new replace functions that support char, char *, const __FlashStringHelper * and String
    public:
        bool replace(char find, char replace);

    // internal replace function with PROGMEM support
    protected:
        bool _replace(PGM_P find, size_t findLen, PGM_P replace, size_t replaceLen);

    // public replace function with return type if bool for success
    public:
        inline bool replace(const char *find, const char *replace) {
            return _replace(find, strlen(find), replace, strlen(replace));
        }
        inline bool replace(const char *find, const __FlashStringHelper *replace) {
            return _replace(find, strlen(find), reinterpret_cast<PGM_P>(replace), strlen_P(reinterpret_cast<PGM_P>(replace)));
        }
        inline bool replace(const char *find, const String &replace) {
            return _replace(find, strlen(find), replace.buffer(), replace.length());
        }
        inline bool replace(const String &find, const String &replace) {
            return _replace(find.buffer(), find.length(), replace.buffer(), replace.length());
        }
        inline bool replace(const String &find, const char *replace) {
            return _replace(find.buffer(), find.length(), replace, strlen(replace));
        }
        inline bool replace(const String &find, const __FlashStringHelper *replace) {
            return _replace(find.buffer(), find.length(), reinterpret_cast<PGM_P>(replace), strlen_P(reinterpret_cast<PGM_P>(replace)));
        }
        inline bool replace(const __FlashStringHelper *find, const String &replace) {
            return _replace(reinterpret_cast<PGM_P>(find), strlen_P(reinterpret_cast<PGM_P>(find)), replace.buffer(), replace.length());
        }
        inline  bool replace(const __FlashStringHelper *find, const char *replace) {
            return _replace(reinterpret_cast<PGM_P>(find), strlen_P(reinterpret_cast<PGM_P>(find)), replace, strlen(replace));
        }
        inline  bool replace(const __FlashStringHelper *find, const __FlashStringHelper *replace) {
            return _replace(reinterpret_cast<PGM_P>(find), strlen_P(reinterpret_cast<PGM_P>(find)), reinterpret_cast<PGM_P>(replace), strlen_P(reinterpret_cast<PGM_P>(replace)));
        }

    // new trim function internal methods
    // trim, rtrim, ltrim
    // direct support for char, char *, const FlashStringHelper * and String as subset to trim
    protected:
        enum class TrimType : uint8_t {
            LEFT = 0x01,
            RIGHT = 0x02,
            BOTH = LEFT | RIGHT
        };

        String &_trim(TrimType type);

        inline String &_trim(TrimType type, char character) {
            char buf[2] = { character, 0 };
            return _trim(type, buf, 1);
        }

        String &_trim(TrimType type, PGM_P characters, size_t charLen);

        inline String &_trim(TrimType type, const char *characters) {
            return _trim(type, characters, strlen(characters));
        }

        inline String &_trim(TrimType type, const __FlashStringHelper *characters) {
            return _trim(type, reinterpret_cast<PGM_P>(characters), strlen_P(reinterpret_cast<PGM_P>(characters)));
        }

        inline String &_trim(TrimType type, const String &characters) {
            return _trim(type, characters.c_str(), characters.length());
        }

    // public trim functions with return type
    public:
        inline String &trim() {
            return _trim(TrimType::BOTH);
        }
        inline String &rtrim() {
            return _trim(TrimType::RIGHT);
        }
        inline String &ltrim() {
            return _trim(TrimType::LEFT);
        }

        inline String &trim(char character) {
            return _trim(TrimType::BOTH, character);
        }
        inline String &rtrim(char character) {
            return _trim(TrimType::RIGHT, character);
        }
        inline String &ltrim(char character) {
            return _trim(TrimType::LEFT, character);
        }

        inline String &trim(const char *characters) {
            return _trim(TrimType::BOTH, characters);
        }
        inline String &trim(const String &characters) {
            return _trim(TrimType::BOTH, characters);
        }
        inline String &trim(const __FlashStringHelper *characters) {
            return _trim(TrimType::BOTH, characters);
        }

        inline String &rtrim(const char *characters) {
            return _trim(TrimType::RIGHT, characters);
        }
        inline String &rtrim(const String &characters) {
            return _trim(TrimType::RIGHT, characters);
        }
        inline String &rtrim(const __FlashStringHelper *characters) {
            return _trim(TrimType::RIGHT, characters);
        }

        inline  String &ltrim(const char *characters) {
            return _trim(TrimType::LEFT, characters);
        }
        inline String &ltrim(const String &characters) {
            return _trim(TrimType::LEFT, characters);
        }
        inline String &ltrim(const __FlashStringHelper *characters) {
            return _trim(TrimType::LEFT, characters);
        }

    public:
        // access to SSO and internals

        // returns allocated memory
        inline size_t __getAllocSize() const {
#if defined(ESP8266)
            if (isSSO()) {
                return 0;
            }
            return capacity() + 1;
#else
            return (length() + 8) & ~7;
#endif
        }

        // returns memory usage
        inline size_t __getMemorySize() const {
            return sizeof(String) + __getAllocSize();
        }

        // get pointer to allocated memory and release ownership
        // capacity is the size of the allocated block
        // if no memory is allocated, it returns nullptr and capacity is set to 0
        // the string is empty afterwards
        inline char *__release(size_t &capacity) {
            if (isSSO()) {
                capacity = 0;
                return nullptr;
            }
            capacity = this->capacity() + 1;
            auto rPtr = ptr.buff;
            ptr = {};
            invalidate();
            return rPtr;
        }

        // see __release(size_t &capacity)
        inline char *__release() {
            if (isSSO()) {
                return nullptr;
            }
            auto rPtr = ptr.buff;
            ptr = {};
            invalidate();
            return rPtr;
        }

    protected:
        // Contains the string info when we're not in SSO mode
        struct _ptr {
            char *   buff;
            uint16_t cap;
            uint16_t len;
        };
        // This allows strings up up to 11 (10 + \0 termination) without any extra space.
        enum { SSOSIZE = sizeof(struct _ptr) + 4 - 1 }; // Characters to allocate space for SSO, must be 12 or more
        struct _sso {
            char     buff[SSOSIZE];
            unsigned char len    : 7; // Ensure only one byte is allocated by GCC for the bitfields
            unsigned char isHeap : 1;
        } __attribute__((packed)); // Ensure that GCC doesn't expand the flag byte to a 32-bit word for alignment issues
        enum { CAPACITY_MAX = 65535 }; // If typeof(cap) changed from uint16_t, be sure to update this enum to the max value storable in the type
        union {
            struct _ptr ptr;
            struct _sso sso;
        };
        // Accessor functions
        bool isSSO() const { return !sso.isHeap; }
        unsigned int len() const { return isSSO() ? sso.len : ptr.len; }
        unsigned int capacity() const { return isSSO() ? (unsigned int)SSOSIZE - 1 : ptr.cap; } // Size of max string not including terminal NUL
        void setSSO(bool set) { sso.isHeap = !set; }
        void setLen(int len) {
            if (isSSO()) {
                setSSO(true); // Avoid emitting of bitwise EXTRACT-AND-OR ops (store-merging optimization)
                sso.len = len;
            } else
                ptr.len = len;
        }
        void setCapacity(int cap) { if (!isSSO()) ptr.cap = cap; }
        void setBuffer(char *buff) { if (!isSSO()) ptr.buff = buff; }
        // Buffer accessor functions
#ifdef _MSC_VER
        const char *buffer() const { return isSSO() ? sso.buff : ptr.buff; } // Writable version of buffer
        const char *wbuffer() const {
            return buffer();
        }
        char *wbuffer() { return isSSO() ? sso.buff : ptr.buff; } // Writable version of buffer
#else
        const char *buffer() const { return wbuffer(); }
        char *wbuffer() const { return isSSO() ? const_cast<char *>(sso.buff) : ptr.buff; } // Writable version of buffer
#endif

        // concatenation is done via non-member functions
        // make sure we still have access to internal methods, since we optimize based on capacity of both sides and want to manipulate internal buffers directly
        friend String operator +(const String &lhs, String &&rhs);
        friend String operator +(String &&lhs, String &&rhs);
        friend String operator +(char lhs, String &&rhs);
        friend String operator +(const char *lhs, String &&rhs);
        friend String operator +(const __FlashStringHelper *lhs, String &&rhs);

    protected:
        void init(void) __attribute__((always_inline)) {
            sso.buff[0] = 0;
            sso.len     = 0;
            sso.isHeap  = 0;
            // Without the 6 statements shown below, GCC simply emits such as: "MOVI.N aX,0", "S8I aX,a2,0" and "S8I aX,a2,11" (8 bytes in total)
            sso.buff[1]  = 0;
            sso.buff[2]  = 0;
            sso.buff[3]  = 0;
            sso.buff[8]  = 0;
            sso.buff[9]  = 0;
            sso.buff[10] = 0;
            // With the above, thanks to store-merging, GCC can use the narrow form of 32-bit store insn ("S32I.N") and emits:
            //   "MOVI.N aX,0", "S32I.N aX,a2,0" and "S32I.N aX,a2,8" (6 bytes in total)
            // (Literature: Xtensa(R) Instruction Set Reference Manual, "S8I - Store 8-bit" [p.504] and "S32I.N - Narrow Store 32-bit" [p.512])
            // Unfortunately, GCC seems not to re-evaluate the cost of inlining after the store-merging optimizer stage,
            // `always_inline` attribute is necessary in order to keep inlining.
        }
        void invalidate(void);
        bool changeBuffer(unsigned int maxStrLen);

        // copy or insert at a specific position
        String &copy(const char *cstr, unsigned int length);
        String &copy(const __FlashStringHelper *pstr, unsigned int length);

        String &insert(size_t position, char);
        String &insert(size_t position, const char *);
        String &insert(size_t position, const __FlashStringHelper *);
        String &insert(size_t position, const char *, size_t length);
        String &insert(size_t position, const String &);

        // rvalue helper
        void move(String &rhs) noexcept;
};

// concatenation (note that it's done using non-method operators to handle both possible type refs)

inline String operator +(const String &lhs, const String &rhs) {
    String res;
    res.reserve(lhs.length() + rhs.length());
    res += lhs;
    res += rhs;
    return res;
}

inline String operator +(String &&lhs, const String &rhs) {
    lhs += rhs;
    return std::move(lhs);
}

String operator +(const String &lhs, String &&rhs);
String operator +(String &&lhs, String &&rhs);

template <typename T,
    typename = std::enable_if_t<!std::is_same_v<String, std::decay_t<T>>>>
inline String operator +(const String &lhs, const T &value) {
    String res(lhs);
    res += value;
    return res;
}

template <typename T,
    typename = std::enable_if_t<!std::is_same_v<String, std::decay_t<T>>>>
inline String operator +(String &&lhs, const T &value) {
    lhs += value;
    return std::move(lhs);
}

// `String(char)` is explicit, but we used to have StringSumHelper silently allowing the following:
// `String x; x = 'a' + String('b') + 'c';`
// For comparison, `std::string(char)` does not exist. However, we are allowed to chain `char` as both lhs and rhs

String operator +(char lhs, const String &rhs);

inline String operator +(char lhs, String &&rhs) {
    return std::move(rhs.insert(0, lhs));
}

// both `char*` and `__FlashStringHelper*` are implicitly converted into `String()`, calling the `operator+(const String& ...);`
// however, here we:
// - do an automatic `reserve(total length)` for the resulting string
// - possibly do rhs.insert(0, ...), when &&rhs capacity could fit both

String operator +(const char *lhs, const String &rhs);

inline String operator +(const char *lhs, String &&rhs) {
    return std::move(rhs.insert(0, lhs));
}

inline String operator +(const __FlashStringHelper *lhs, const String &rhs) {
    return reinterpret_cast<const char*>(lhs) + rhs;
}

inline String operator +(const __FlashStringHelper *lhs, String &&rhs) {
    return std::move(rhs.insert(0, lhs));
}

extern const String emptyString;

#endif  // __cplusplus
#endif  // String_class_h
