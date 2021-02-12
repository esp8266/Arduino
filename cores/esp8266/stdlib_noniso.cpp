/*
  stdlib_noniso.h - nonstandard (but usefull) conversion functions

  Copyright (c) 2021 David Gauchard. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.

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

#include "stdlib_noniso.h"

// ulltoa() is slower than std::to_char() (1.6 times)
// but is smaller by ~800B/flash and ~250B/rodata

// ulltoa fills str backwards and can return a pointer different from str
char* ulltoa(unsigned long long val, char* str, int slen, unsigned int radix)
{
    str += --slen;
    *str = 0;
    do
    {
        auto mod = val % radix;
        val /= radix;
        *--str = mod + ((mod > 9) ? ('a' - 10) : '0');
    } while (--slen && val);
    return val? nullptr: str;
}

// lltoa fills str backwards and can return a pointer different from str
char* lltoa (long long val, char* str, int slen, unsigned int radix)
{
    bool neg;
    if (val < 0)
    {
        val = -val;
        neg = true;
    }
    else
    {
        neg = false;
    }
    char* ret = ulltoa(val, str, slen, radix);
    if (neg)
    {
        if (ret == str)
            return nullptr;
        *--ret = '-';
    }
    return ret;
}
