/*
 pgmspace.cpp - string functions that support PROGMEM
 Copyright (c) 2015 Michael C. Miller.  All right reserved.

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

#include <ctype.h>
#include "pgmspace.h"

size_t strnlen_P(const char* s, size_t size) {
    const char* cp;
    for (cp = s; size != 0 && pgm_read_byte(cp) != '\0'; cp++, size--);
    return (size_t) (cp - s);
}

void* memcpy_P(void* dest, const void* src, size_t count) {
    const uint8_t* read = reinterpret_cast<const uint8_t*>(src);
    uint8_t* write = reinterpret_cast<uint8_t*>(dest);

    while (count)
    {
        *write++ = pgm_read_byte(read++);
        count--;
    }

    return dest;
}

char* strncpy_P(char* dest, const char* src, size_t size) {
    const char* read = src;
    char* write = dest;
    char ch = '.';
    while (size > 0 && ch != '\0')
    {
        ch = pgm_read_byte(read++);
        *write++ = ch;
        size--;
    }

    return dest;
}

char* strncat_P(char* dest, const char* src, size_t size) {
    char* write = dest;

    while (*write != '\0')
    {
        write++;
    }

    const char* read = src;
    char ch = '.';

    while (size > 0 && ch != '\0')
    {
        ch = pgm_read_byte(read++);
        *write++ = ch;

        size--;
    }

    if (ch != '\0')
    {
        *write = '\0';
    }

    return dest;
}

int strncmp_P(const char* str1, const char* str2P, size_t size) {
    int result = 0;

    while (size > 0)
    {
        char ch1 = *str1++;
        char ch2 = pgm_read_byte(str2P++);
        result = ch1 - ch2;
        if (result != 0 || ch2 == '\0')
        {
            break;
        }

        size--;
    }

    return result;
}

int strncasecmp_P(const char* str1, const char* str2P, size_t size) {
    int result = 0;

    while (size > 0)
    {
        char ch1 = tolower(*str1++);
        char ch2 = tolower(pgm_read_byte(str2P++));
        result = ch1 - ch2;
        if (result != 0 || ch2 == '\0')
        {
            break;
        }

        size--;
    }

    return result;
}

int printf_P(const char* formatP, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, formatP);

    size_t fmtLen = strlen_P(formatP);
    char* format = new char[fmtLen + 1];
    strcpy_P(format, formatP);

    ret = os_printf(format, arglist);

    delete[] format;

    va_end(arglist);
    return ret;
}

int snprintf_P(char* str, size_t strSize, const char* formatP, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, formatP);

    ret = vsnprintf_P(str, strSize, formatP, arglist);

    va_end(arglist);
    return ret;
}

int vsnprintf_P(char* str, size_t strSize, const char* formatP, va_list ap) {
    int ret;

    size_t fmtLen = strlen_P(formatP);
    char* format = new char[fmtLen + 1];
    strcpy_P(format, formatP);

    ret = ets_vsnprintf(str, strSize, format, ap);

    delete[] format;

    return ret;
}
