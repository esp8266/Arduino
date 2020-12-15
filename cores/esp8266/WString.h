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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pgmspace.h>
#if defined(MMU_IRAM_HEAP)
// Host test will fail with this include and build path.
// It reports an error at link time for a missing pvPortMalloc.
#include "umm_malloc/umm_malloc.h"
#endif

// An inherited class for holding the result of a concatenation.  These
// result objects are assumed to be writable by subsequent concatenations.
class StringSumHelper;

// an abstract class used as a means to proide a unique pointer type
// but really has no body
class __FlashStringHelper;
#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#define F(string_literal) (FPSTR(PSTR(string_literal)))



// The string class
class String {
        // use a function pointer to allow for "if (s)" without the
        // complications of an operator bool(). for more information, see:
        // http://www.artima.com/cppsource/safebool.html
        typedef void (String::*StringIfHelperType)() const;
        void StringIfHelper() const {
        }

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
        String(StringSumHelper &&rval) noexcept;
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
        explicit String(float, unsigned char decimalPlaces = 2);
        explicit String(double, unsigned char decimalPlaces = 2);
        ~String() {
            invalidate();
        }

        // memory management
        // return true on success, false on failure (in which case, the string
        // is left unchanged).  reserve(0), if successful, will validate an
        // invalid string (i.e., "if (s)" will be true afterwards)
        unsigned char reserve(unsigned int size);
        unsigned int length(void) const {
            return buffer() ? len() : 0;
        }
        void clear(void) {
            setLen(0);
        }
        bool isEmpty(void) const {
            return length() == 0;
        }
#if defined(MMU_IRAM_HEAP)
        /*
          String Class - Heap Selection Control

          Select one of these options to be used across all String class buffer
          allocations.  The options are defined in `enum Heap`.
            * Allow OOM recovery (OOM rollover) by trying a different Heap on fail.
              * `IramDram` - Try to allocate from IRAM 1ST, then roll over to DRAM on fail.
              * `DramIram` - Try to allocate from DRAM 1ST, then roll over to IRAM on fail.
            * No OOM recovery. Use only the Heap specified.
              * `IramOnly` - Allocate only from the IRAM Heap.
              * `DramOnly` - Allocate only from the DRAM Heap.
            * Use the current umm_malloc Heap selection for new allocations.
              * `CurrentOnly` - Allocate only from the current Heap selected.
            * Use the current umm_malloc Heap selection for new allocations. On
              alloc failure, try the other Heap.
              * `Current` - Try to allocate from the current Heap selected,
                            then roll over to the other Heap.

          Initialize the String class static member _preferredHeap with the
          desired selection for Heap control in your main sketch file somewhere
          before `setup();`.

          Example of legacy behavior using option `CurrentOnly`:
          const String::Heap String::_preferredHeap = String::Heap::CurrentOnly;

          Example of maximizing available DRAM, by using IRAM 1st then DRAM:
          const String::Heap String::_preferredHeap = String::Heap::IramDram;

          When a Heap rollover occurs, the OOM counter for the failing Heap will
          increment. If both Heaps fail, both OOM counters are incremented.
        */
        enum Heap {
            IramDram        =  (1 + UMM_HEAP_IRAM),
            DramIram        =  (1 + UMM_HEAP_DRAM),
            Current         =  (1 + UMM_NUM_HEAPS),
            IramOnly        = -(1 + UMM_HEAP_IRAM),
            DramOnly        = -(1 + UMM_HEAP_DRAM),
            CurrentOnly     = 0
        };
        inline Heap getHeap() {
            return _preferredHeap;
        }
#else
        enum Heap {
            IramDram        = 0,
            DramIram        = 0,
            Current         = 0,
            IramOnly        = 0,
            DramOnly        = 0,
            CurrentOnly     = 0
        };
        inline Heap getHeap() { return Heap::CurrentOnly; }
#endif
        // creates a copy of the assigned value.  if the value is null or
        // invalid, or if the memory allocation fails, the string will be
        // marked as invalid ("if (s)" will be false).
        String &operator =(const String &rhs);
        String &operator =(const char *cstr);
        String &operator =(const __FlashStringHelper *str);
        String &operator =(String &&rval) noexcept;
        String &operator =(StringSumHelper &&rval) noexcept {
            return operator =((String &&)rval);
        }

        // concatenate (works w/ built-in types)

        // returns true on success, false on failure (in which case, the string
        // is left unchanged).  if the argument is null or invalid, the
        // concatenation is considered unsuccessful.
        unsigned char concat(const String &str);
        unsigned char concat(const char *cstr);
        unsigned char concat(char c);
        unsigned char concat(unsigned char c);
        unsigned char concat(int num);
        unsigned char concat(unsigned int num);
        unsigned char concat(long num);
        unsigned char concat(unsigned long num);
        unsigned char concat(float num);
        unsigned char concat(double num);
        unsigned char concat(const __FlashStringHelper *str);
        unsigned char concat(const char *cstr, unsigned int length);

        // if there's not enough memory for the concatenated value, the string
        // will be left unchanged (but this isn't signalled in any way)
        String &operator +=(const String &rhs) {
            concat(rhs);
            return *this;
        }
        String &operator +=(const char *cstr) {
            concat(cstr);
            return *this;
        }
        String &operator +=(char c) {
            concat(c);
            return *this;
        }
        String &operator +=(unsigned char num) {
            concat(num);
            return *this;
        }
        String &operator +=(int num) {
            concat(num);
            return *this;
        }
        String &operator +=(unsigned int num) {
            concat(num);
            return *this;
        }
        String &operator +=(long num) {
            concat(num);
            return *this;
        }
        String &operator +=(unsigned long num) {
            concat(num);
            return *this;
        }
        String &operator +=(float num) {
            concat(num);
            return *this;
        }
        String &operator +=(double num) {
            concat(num);
            return *this;
        }
        String &operator +=(const __FlashStringHelper *str) {
            concat(str);
            return *this;
        }

        friend StringSumHelper &operator +(const StringSumHelper &lhs, const String &rhs);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, const char *cstr);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, char c);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, unsigned char num);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, int num);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, unsigned int num);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, long num);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, unsigned long num);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, float num);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, double num);
        friend StringSumHelper &operator +(const StringSumHelper &lhs, const __FlashStringHelper *rhs);

        // comparison (only works w/ Strings and "strings")
        operator StringIfHelperType() const {
            return buffer() ? &String::StringIfHelper : 0;
        }
        int compareTo(const String &s) const;
        unsigned char equals(const String &s) const;
        unsigned char equals(const char *cstr) const;
        unsigned char operator ==(const String &rhs) const {
            return equals(rhs);
        }
        unsigned char operator ==(const char *cstr) const {
            return equals(cstr);
        }
        unsigned char operator !=(const String &rhs) const {
            return !equals(rhs);
        }
        unsigned char operator !=(const char *cstr) const {
            return !equals(cstr);
        }
        unsigned char operator <(const String &rhs) const;
        unsigned char operator >(const String &rhs) const;
        unsigned char operator <=(const String &rhs) const;
        unsigned char operator >=(const String &rhs) const;
        unsigned char equalsIgnoreCase(const String &s) const;
        unsigned char equalsConstantTime(const String &s) const;
        unsigned char startsWith(const String &prefix) const;
        unsigned char startsWith(const char *prefix) const {
            return this->startsWith(String(prefix));
        }
        unsigned char startsWith(const __FlashStringHelper *prefix) const {
            return this->startsWith(String(prefix));
        }
        unsigned char startsWith(const String &prefix, unsigned int offset) const;
        unsigned char endsWith(const String &suffix) const;
        unsigned char endsWith(const char *suffix) const {
            return this->endsWith(String(suffix));
        }
        unsigned char endsWith(const __FlashStringHelper *suffix) const {
            return this->endsWith(String(suffix));
        }

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
        int indexOf(char ch, unsigned int fromIndex = 0) const;
        int indexOf(const char *str, unsigned int fromIndex = 0) const;
        int indexOf(const __FlashStringHelper *str, unsigned int fromIndex = 0) const {
            return indexOf((const char*)str, fromIndex);
        }
        int indexOf(const String &str, unsigned int fromIndex = 0) const;
        int lastIndexOf(char ch) const;
        int lastIndexOf(char ch, unsigned int fromIndex) const;
        int lastIndexOf(const String &str) const;
        int lastIndexOf(const String &str, unsigned int fromIndex) const;
        String substring(unsigned int beginIndex) const {
            return substring(beginIndex, len());
        }
        String substring(unsigned int beginIndex, unsigned int endIndex) const;

        // modification
        void replace(char find, char replace);
        void replace(const String &find, const String &replace);
        void replace(const char *find, const String &replace) {
            this->replace(String(find), replace);
        }
        void replace(const __FlashStringHelper *find, const String &replace) {
            this->replace(String(find), replace);
        }
        void replace(const char *find, const char *replace) {
            this->replace(String(find), String(replace));
        }
        void replace(const __FlashStringHelper *find, const char *replace) {
            this->replace(String(find), String(replace));
        }
        void replace(const __FlashStringHelper *find, const __FlashStringHelper *replace) {
            this->replace(String(find), String(replace));
        }
        // Pass the biggest integer if the count is not specified.
        // The remove method below will take care of truncating it at the end of the string.
        void remove(unsigned int index, unsigned int count = (unsigned int)-1);
        void toLowerCase(void);
        void toUpperCase(void);
        void trim(void);

        // parsing/conversion
        long toInt(void) const;
        float toFloat(void) const;
        double toDouble(void) const;

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
        const char *buffer() const { return wbuffer(); }
        char *wbuffer() const { return isSSO() ? const_cast<char *>(sso.buff) : ptr.buff; } // Writable version of buffer

    private:
#if defined(MMU_IRAM_HEAP)
        static const Heap _preferredHeap;
#endif

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
        unsigned char changeBuffer(unsigned int maxStrLen);

        // copy and move
        String &copy(const char *cstr, unsigned int length);
        String &copy(const __FlashStringHelper *pstr, unsigned int length);
        void move(String &rhs) noexcept;
};

class StringSumHelper: public String {
    public:
        StringSumHelper(const String &s) :
                String(s) {
        }
        StringSumHelper(const char *p) :
                String(p) {
        }
        StringSumHelper(char c) :
                String(c) {
        }
        StringSumHelper(unsigned char num) :
                String(num) {
        }
        StringSumHelper(int num) :
                String(num) {
        }
        StringSumHelper(unsigned int num) :
                String(num) {
        }
        StringSumHelper(long num) :
                String(num) {
        }
        StringSumHelper(unsigned long num) :
                String(num) {
        }
        StringSumHelper(float num) :
                String(num) {
        }
        StringSumHelper(double num) :
                String(num) {
        }
        StringSumHelper(const __FlashStringHelper *s) :
                String(s) {
        }
};

extern const String emptyString;

#endif  // __cplusplus
#endif  // String_class_h
