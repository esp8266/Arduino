/* 
 libc_replacements.c - replaces libc functions with functions
 from Espressif SDK

 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
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

 Modified 03 April 2015 by Markus Sattler

 */

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "debug.h"

void* malloc(size_t size) {
    size = ((size + 3) & ~((size_t)0x3));
    return os_malloc(size);
}

void free(void* ptr) {
    os_free(ptr);
}

void* realloc(void* ptr, size_t size) {
    size = ((size + 3) & ~((size_t)0x3));
    return os_realloc(ptr, size);
}

int puts(const char * str) {
    return os_printf("%s", str);
}

int printf(const char* format, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, format);
    ret = ets_vprintf(format, arglist);
    va_end(arglist);
    return ret;
}

int sprintf(char* buffer, const char* format, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, format);
    ret = ets_vsprintf(buffer, format, arglist);
    va_end(arglist);
    return ret;
}

int snprintf(char* buffer, size_t size, const char* format, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, format);
    ret = ets_vsnprintf(buffer, size, format, arglist);
    va_end(arglist);
    return ret;
}

int vsnprintf(char * buffer, size_t size, const char * format, va_list arg) {
    return ets_vsnprintf(buffer, size, format, arg);
}

size_t ICACHE_FLASH_ATTR strnlen(const char *s, size_t len) {
    // there is no ets_strnlen
    const char *cp;
    for (cp = s; len != 0 && *cp != '\0'; cp++, len--);
    return (size_t)(cp - s);
}

char* ICACHE_FLASH_ATTR strchr(const char * str, int character) {
    while(1) {
        if(*str == 0x00) {
            return NULL;
        }
        if(*str == (char) character) {
            return (char *) str;
        }
        str++;
    }
}

char * ICACHE_FLASH_ATTR strrchr(const char * str, int character) {
    char * ret = NULL;
    while(1) {
        if(*str == 0x00) {
            return ret;
        }
        if(*str == (char) character) {
            ret = (char *) str;
        }
        str++;
    }
}

char* ICACHE_FLASH_ATTR strcat(char * dest, const char * src) {
    return strncat(dest, src, strlen(src));
}

char* ICACHE_FLASH_ATTR strncat(char * dest, const char * src, size_t n) {
    size_t i;
    size_t offset = strlen(dest);
    for(i = 0; i < n && src[i]; i++) {
        dest[i + offset] = src[i];
    }
    dest[i + offset] = 0;
    return dest;
}

char* ICACHE_FLASH_ATTR strtok_r(char* s, const char* delim, char** last) {
    const char* spanp;
    char* tok;
    char c;
    char sc;

    if (s == NULL && (s = *last) == NULL) {
        return (NULL);
    }


    // Skip (span) leading delimiters 
    //
cont:
    c = *s++;
    for (spanp = delim; (sc = *spanp++) != 0;) {
        if (c == sc) {
            goto cont;
        }
    }

    // check for no delimiters left
    //
    if (c == '\0') {
        *last = NULL;
        return (NULL);
    }

    tok = s - 1;


    // Scan token 
    // Note that delim must have one NUL; we stop if we see that, too.
    //
    for (;;) {
        c = *s++;
        spanp = (char *)delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0) {
                    s = NULL;
                }
                else {
                    s[-1] = '\0';
                }
                *last = s;
                return (tok);
            }

        } while (sc != 0);
    }

    // NOTREACHED EVER
}

char* ICACHE_FLASH_ATTR strtok(char* s, const char* delim) {
    static char* last;

    return (strtok_r(s, delim, &last));
}

int ICACHE_FLASH_ATTR strcasecmp(const char * str1, const char * str2) {
    int d = 0;
    while(1) {
        int c1 = tolower(*str1++);
        int c2 = tolower(*str2++);
        if(((d = c1 - c2) != 0) || (c2 == '\0')) {
            break;
        }
    }
    return d;
}

char* ICACHE_FLASH_ATTR strdup(const char *str) {
    size_t len = strlen(str) + 1;
    char *cstr = malloc(len);
    if(cstr) {
        memcpy(cstr, str, len);
    }
    return cstr;
}

long int ICACHE_FLASH_ATTR strtol(const char* str, char** endptr, int base) {
    long int result = 0;
    int sign = 1;

    while(isspace(*str)) {
        str++;
    }

    if(*str == 0x00) {
        // only space in str?
        *endptr = (char*) str;
        return result;
    }

    switch(base) {
        case 10:

            if(*str == '-') {
                sign = -1;
                str++;
            } else if(*str == '+') {
                str++;
            }

            for(uint8_t i = 0; *str; i++, str++) {
                int x = *str - '0';
                if(x < 0 || x > 9) {
                    break;
                }
                result = result * 10 + x;
            }
            break;
        case 2:
            for(uint8_t i = 0; *str; i++, str++) {
                int x = *str - '0';
                if(x < 0 || x > 1) {
                    break;
                }
                result = result * 2 + x;
            }
            break;
        case 16:
        default:
            os_printf("fnk: strtol() only supports base 10 and 2 ATM!\n");
            break;

    }
    *endptr = (char*) str;
    return sign * result;
}

// based on Source:
// https://github.com/anakod/Sming/blob/master/Sming/system/stringconversion.cpp#L93
double ICACHE_FLASH_ATTR strtod(const char* str, char** endptr) {
    double result = 0.0;
    double factor = 1.0;
    bool decimals = false;
    char c;

    while(isspace(*str)) {
        str++;
    }

    if(*str == 0x00) {
        // only space in str?
        *endptr = (char*) str;
        return result;
    }

    if(*str == '-') {
        factor = -1;
        str++;
    } else if(*str == '+') {
        str++;
    }

    while((c = *str)) {
        if(c == '.') {
            decimals = true;
            str++;
            continue;
        }

        int d = c - '0';
        if(d < 0 || d > 9) {
            break;
        }

        result = 10.0 * result + d;
        if(decimals) {
            factor *= 0.1;
        }

        str++;
    }
    *endptr = (char*) str;
    return result * factor;
}

// ##########################################################################
//                             ctype functions
// ##########################################################################

int isalnum(int c) {
    if(isalpha(c) || isdigit(c)) {
        return 1;
    }
    return 0;
}

int isalpha(int c) {
    if(islower(c) || isupper(c)) {
        return 1;
    }
    return 0;
}

int iscntrl(int c) {
    if(c <= 0x1F || c == 0x7F) {
        return 1;
    }
    return 0;
}

int isdigit(int c) {
    if(c >= '0' && c <= '9') {
        return 1;
    }
    return 0;
}

int isgraph(int c) {
    if(isprint(c) && c != ' ') {
        return 1;
    }
    return 0;
}

int islower(int c) {
    if(c >= 'a' && c <= 'z') {
        return 1;
    }
    return 0;
}

int isprint(int c) {
    if(!iscntrl(c)) {
        return 1;
    }
    return 0;
}

int ispunct(int c) {
    if(isgraph(c) && !isalnum(c)) {
        return 1;
    }
    return 0;
}

int isspace(int c) {
    switch(c) {
        case 0x20: // ' '
        case 0x09: // '\t'
        case 0x0a: // '\n'
        case 0x0b: // '\v'
        case 0x0c: // '\f'
        case 0x0d: // '\r'
            return 1;
    }
    return 0;
}

int isupper(int c) {
    if(c >= 'A' && c <= 'Z') {
        return 1;
    }
    return 0;
}

int isxdigit(int c) {
    if(c >= 'A' && c <= 'F') {
        return 1;
    }
    if(c >= 'a' && c <= 'f') {
        return 1;
    }
    if(isdigit(c)) {
        return 1;
    }
    return 0;
}

int tolower(int c) {
    if(isupper(c)) {
        c += 0x20;
    }
    return c;
}

int toupper(int c) {
    if(islower(c)) {
        c -= 0x20;
    }
    return c;
}

int isblank(int c) {
    switch(c) {
        case 0x20: // ' '
        case 0x09: // '\t'
            return 1;
    }
    return 0;
}

// ##########################################################################

static int errno_var = 0;

int* ICACHE_FLASH_ATTR __errno(void) {
    DEBUGV("__errno is called last error: %d (not current)\n", errno_var);
    return &errno_var;
}


/*
 * begin newlib/string/strlcpy.c
 * 
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

size_t ICACHE_FLASH_ATTR strlcpy(char* dst, const char* src, size_t size) {
    const char *s = src;
    size_t n = size;

    if (n != 0 && --n != 0) {
        do {
            if ((*dst++ = *s++) == 0)
                break;
        } while (--n != 0);
    }
    
    if (n == 0) {
        if (size != 0)
            *dst = 0;
        while (*s++);
    }
    
    return(s - src - 1);
}
/*
 * end of newlib/string/strlcpy.c
 */

