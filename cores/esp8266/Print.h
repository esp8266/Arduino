/*
 Print.h - Base class that provides print() and println()
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
 */

#ifndef Print_h
#define Print_h

#include <stdint.h>
#include <stddef.h>

#include "WString.h"
#include "Printable.h"

#include "stdlib_noniso.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Print {
    private:
        int write_error = 0;
        template<typename T> size_t printNumber(T n, uint8_t base);
        template<typename T, typename... P> inline size_t _println(T v, P... args);
    protected:
        void setWriteError(int err = 1) {
            write_error = err;
        }
    public:
        Print() {}

        int getWriteError() {
            return write_error;
        }
        void clearWriteError() {
            setWriteError(0);
        }

        virtual size_t write(uint8_t) = 0;
        size_t write(const char *str) {
            if(str == NULL)
                return 0;
            return write((const uint8_t *) str, strlen_P(str));
        }
        virtual size_t write(const uint8_t *buffer, size_t size);
        size_t write(const char *buffer, size_t size) {
            return write((const uint8_t *) buffer, size);
        }
        // These handle ambiguity for write(0) case, because (0) can be a pointer or an integer
        inline size_t write(short t) { return write((uint8_t)t); }
        inline size_t write(unsigned short t) { return write((uint8_t)t); }
        inline size_t write(int t) { return write((uint8_t)t); }
        inline size_t write(unsigned int t) { return write((uint8_t)t); }
        inline size_t write(long t) { return write((uint8_t)t); }
        inline size_t write(unsigned long t) { return write((uint8_t)t); }
        inline size_t write(long long t) { return write((uint8_t)t); }
        inline size_t write(unsigned long long t) { return write((uint8_t)t); }
        // Enable write(char) to fall through to write(uint8_t)
        inline size_t write(char c) { return write((uint8_t) c); }
        inline size_t write(int8_t c) { return write((uint8_t) c); }

        // default to zero, meaning "a single write may block"
        // should be overridden by subclasses with buffering
        virtual int availableForWrite() { return 0; }

        size_t printf(const char * format, ...)  __attribute__ ((format (printf, 2, 3)));
        size_t printf_P(PGM_P format, ...) __attribute__((format(printf, 2, 3)));
        size_t print(const __FlashStringHelper *);
        size_t print(const String &);
        size_t print(const char[]);
        size_t print(char);
        size_t print(unsigned char, int = DEC);
        size_t print(int, int = DEC);
        size_t print(unsigned int, int = DEC);
        size_t print(long, int = DEC);
        size_t print(unsigned long, int = DEC);
        size_t print(long long, int = DEC);
        size_t print(unsigned long long, int = DEC);
        size_t print(double, int = 2);
        size_t print(const Printable&);

        size_t println(const __FlashStringHelper *);
        size_t println(const String &s);
        size_t println(const char[]);
        size_t println(char);
        size_t println(unsigned char, int = DEC);
        size_t println(int, int = DEC);
        size_t println(unsigned int, int = DEC);
        size_t println(long, int = DEC);
        size_t println(unsigned long, int = DEC);
        size_t println(long long, int = DEC);
        size_t println(unsigned long long, int = DEC);
        size_t println(double, int = 2);
        size_t println(const Printable&);
        size_t println(void);

        // flush():
        // Empty implementation by default in Print::
        // should wait for all outgoing characters to be sent, output buffer is empty after this call
        virtual void flush() { }

        // by default write timeout is possible (outgoing data from network,serial..)
        // (children can override to false (like String))
        virtual bool outputCanTimeout () { return true; }
};

template<> size_t Print::printNumber(double number, uint8_t digits);

#endif
