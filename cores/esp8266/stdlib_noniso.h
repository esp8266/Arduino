/* 
  stdlib_noniso.h - nonstandard (but usefull) conversion functions

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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

#ifndef STDLIB_NONISO_H
#define STDLIB_NONISO_H

#ifdef __cplusplus
extern "C"{
#endif

int atoi(const char *s);

long atol(const char* s);

double atof(const char* s);

char* itoa (int val, char *s, int radix);

char* ltoa (long val, char *s, int radix);

char* lltoa (long long val, char* str, int slen, unsigned long long radix);

char* utoa (unsigned int val, char *s, int radix);

char* ultoa (unsigned long val, char *s, int radix);

char* ulltoa (unsigned long long val, char* str, int slen, unsigned long long radix);

char* dtostrf (double val, signed char width, unsigned char prec, char *s);

void reverse(char* begin, char* end);

const char* strrstr(const char*__restrict p_pcString,
                    const char*__restrict p_pcPattern);

#ifdef __cplusplus

} // extern "C"

// https://gcc.gnu.org/legacy-ml/gcc/2016-02/msg00001.html
extern "C++" {
template <typename T>
T udivmod(T num, T den, T& mod)
{
    T bit = 1;
    T res = 0;

    while (den < num && bit && !(den & (((T)1) << (sizeof(T)*8 - 1))))
    {
        den <<= 1;
        bit <<= 1;
    }
    while (bit)
    {
        if (num >= den)
        {
            num -= den;
            res |= bit;
        }
        bit >>= 1;
        den >>= 1;
    }
    mod = num;
    return res;
} // udivmod
} // "C++"

#endif // __cplusplus
#endif // STDLIB_NONISO_H
