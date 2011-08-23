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

#include "WString.h"
#include "Printable.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Print
{
  private:
    long printNumber(unsigned long, uint8_t);
    long printFloat(double, uint8_t);
  public:
    virtual long write(uint8_t) = 0;
    virtual long write(const char *str);
    virtual long write(const uint8_t *buffer, size_t size);
    
    long print(const __FlashStringHelper *);
    long print(const String &);
    long print(const char[]);
    long print(char);
    long print(unsigned char, int = DEC);
    long print(int, int = DEC);
    long print(unsigned int, int = DEC);
    long print(long, int = DEC);
    long print(unsigned long, int = DEC);
    long print(double, int = 2);
    long print(const Printable&);

    long println(const __FlashStringHelper *);
    long println(const String &s);
    long println(const char[]);
    long println(char);
    long println(unsigned char, int = DEC);
    long println(int, int = DEC);
    long println(unsigned int, int = DEC);
    long println(long, int = DEC);
    long println(unsigned long, int = DEC);
    long println(double, int = 2);
    long println(const Printable&);
    long println(void);
};

#endif
