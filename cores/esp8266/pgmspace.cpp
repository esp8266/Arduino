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
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include "pgmspace.h"

extern "C" {

size_t strnlen_P(PGM_P s, size_t size) {
    const char* cp;
    for (cp = s; size != 0 && pgm_read_byte(cp) != '\0'; cp++, size--);
    return (size_t) (cp - s);
}

char* strstr_P(const char* haystack, PGM_P needle)
{
    const char* pn = reinterpret_cast<const char*>(needle);
    if (haystack[0] == 0) {
        if (pgm_read_byte(pn)) {
	        return NULL;
        }
        return (char*) haystack;
    }

    while (*haystack) {
        size_t i = 0;
        while (true) {
            char n = pgm_read_byte(pn + i);
            if (n == 0) {
                return (char *) haystack;
            }
            if (n != haystack[i]) {
                break;
            }
            ++i;
        }
        ++haystack;
    }
    return NULL;
}

void* memcpy_P(void* dest, PGM_VOID_P src, size_t count) {
    const uint8_t* read = reinterpret_cast<const uint8_t*>(src);
    uint8_t* write = reinterpret_cast<uint8_t*>(dest);

    while (count)
    {
        *write++ = pgm_read_byte(read++);
        count--;
    }

    return dest;
}

int memcmp_P(const void* buf1, PGM_VOID_P buf2P, size_t size) {
    int result = 0;
    const uint8_t* read1 = (const uint8_t*)buf1;
    const uint8_t* read2 = (const uint8_t*)buf2P;

    while (size > 0) {
        uint8_t ch2 = pgm_read_byte(read2);
        uint8_t ch1 = *read1;
        if (ch1 != ch2) {
            result = (int)(ch1)-(int)(ch2);
            break;
        }

        read1++;
        read2++;
        size--;
    }

    return result;
}

void* memccpy_P(void* dest, PGM_VOID_P src, int c, size_t count) {
    uint8_t* read = (uint8_t*)src;
    uint8_t* write = (uint8_t*)dest;
    void* result = NULL;

    while (count > 0) {
        uint8_t ch = pgm_read_byte(read++);
        *write++ = ch;
        count--;
        if (c == ch) {
            return write; // the value after the found c
        }
    }

    return result;
}

void* memmem_P(const void* buf, size_t bufSize, PGM_VOID_P findP, size_t findPSize) {
    const uint8_t* read = (const uint8_t*)buf;
    const uint8_t* find = (uint8_t*)findP;
    uint8_t first = pgm_read_byte(find++);

    findPSize--;

    while (bufSize > 0) {
        if (*read == first) {
            size_t findSize = findPSize;
            const uint8_t* tag = read + 1;
            size_t tagBufSize = bufSize - 1;
            const uint8_t* findTag = find;

            while (tagBufSize > 0 && findSize > 0) {
                uint8_t ch = pgm_read_byte(findTag++);
                if (ch != *tag) {
                    bufSize--;
                    read++;
                    break;
                }
                findSize--;
                tagBufSize--;
                tag++;
            }
            if (findSize == 0) {
                return (void*)read;
            }
        }
        else {
            bufSize--;
            read++;
        }
    }
    return NULL;
}


char* strncpy_P(char* dest, PGM_P src, size_t size) {
    bool size_known = (size != SIZE_IRRELEVANT);
    const char* read = src;
    char* write = dest;
    char ch = '.';
    while (size > 0 && ch != '\0')
    {
        ch = pgm_read_byte(read++);
        *write++ = ch;
        size--;
    }
    if (size_known)
    {
        while (size > 0)
        {
            *write++ = 0;
            size--;
        }
    }

    return dest;
}

char* strncat_P(char* dest, PGM_P src, size_t size) {
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

int strncmp_P(const char* str1, PGM_P str2P, size_t size) {
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

int strncasecmp_P(const char* str1, PGM_P str2P, size_t size) {
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

int printf_P(PGM_P formatP, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, formatP);

    size_t fmtLen = strlen_P(formatP);
    char* format = new char[fmtLen + 1];
    strcpy_P(format, formatP);

    ret = vprintf(format, arglist);

    delete[] format;

    va_end(arglist);
    return ret;
}

int sprintf_P(char* str, PGM_P formatP, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, formatP);

    ret = vsnprintf_P(str, SIZE_IRRELEVANT, formatP, arglist);

    va_end(arglist);
    return ret;
}

int snprintf_P(char* str, size_t strSize, PGM_P formatP, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, formatP);

    ret = vsnprintf_P(str, strSize, formatP, arglist);

    va_end(arglist);
    return ret;
}

int vsnprintf_P(char* str, size_t strSize, PGM_P formatP, va_list ap) {
    int ret;

    size_t fmtLen = strlen_P(formatP);
    char* format = new char[fmtLen + 1];
    strcpy_P(format, formatP);

    ret = vsnprintf(str, strSize, format, ap);

    delete[] format;

    return ret;
}

} // extern "C"
