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

#include <inttypes.h>
#include <stdio.h> // for size_t

typedef int ssize_t;

#include "WString.h"
#include "Printable.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Print
{
  private:
    ssize_t printNumber(unsigned long, uint8_t);
    ssize_t printFloat(double, uint8_t);
  public:
    virtual ssize_t write(uint8_t) = 0;
    virtual ssize_t write(const char *str);
    virtual ssize_t write(const uint8_t *buffer, size_t size);
    
    ssize_t print(const __FlashStringHelper *);
    ssize_t print(const String &);
    ssize_t print(const char[]);
    ssize_t print(char);
    ssize_t print(unsigned char, int = DEC);
    ssize_t print(int, int = DEC);
    ssize_t print(unsigned int, int = DEC);
    ssize_t print(long, int = DEC);
    ssize_t print(unsigned long, int = DEC);
    ssize_t print(double, int = 2);
    ssize_t print(const Printable&);

    ssize_t println(const __FlashStringHelper *);
    ssize_t println(const String &s);
    ssize_t println(const char[]);
    ssize_t println(char);
    ssize_t println(unsigned char, int = DEC);
    ssize_t println(int, int = DEC);
    ssize_t println(unsigned int, int = DEC);
    ssize_t println(long, int = DEC);
    ssize_t println(unsigned long, int = DEC);
    ssize_t println(double, int = 2);
    ssize_t println(const Printable&);
    ssize_t println(void);
};

#endif
