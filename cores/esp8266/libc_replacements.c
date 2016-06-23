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
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/reent.h>

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "debug.h"


int ICACHE_RAM_ATTR _open_r (struct _reent* unused, const char *ptr, int mode) {
    (void)unused;
    (void)ptr;
    (void)mode;
    return 0;
}

int ICACHE_RAM_ATTR _close_r(struct _reent* unused, int file) {
    (void)unused;
    (void)file;
    return 0;
}

int ICACHE_RAM_ATTR _fstat_r(struct _reent* unused, int file, struct stat *st) {
    (void)unused;
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int ICACHE_RAM_ATTR _lseek_r(struct _reent* unused, int file, int ptr, int dir) {
    (void)unused;
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int ICACHE_RAM_ATTR _read_r(struct _reent* unused, int file, char *ptr, int len) {
    (void)unused;
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

int ICACHE_RAM_ATTR _write_r(struct _reent* r, int file, char *ptr, int len) {
    if (file == STDOUT_FILENO) {
        while(len--) {
            ets_putc(*ptr);
            ++ptr;
        }
    }
    return len;
}

int ICACHE_RAM_ATTR _putc_r(struct _reent* r, int c, FILE* file) {
    if (file->_file == STDOUT_FILENO) {
        return ets_putc(c);
    }
    return EOF;
}

int ICACHE_RAM_ATTR puts(const char * str) {
    char c;
    while((c = *str) != 0) {
        ets_putc(c);
        ++str;
    }
    ets_putc('\n');
    return true;
}

#undef putchar
int ICACHE_RAM_ATTR putchar(int c) {
    ets_putc(c);
    return c;
}

#if 0

int ICACHE_RAM_ATTR printf(const char* format, ...) {
    va_list arglist;
    va_start(arglist, format);
    int ret = ets_vprintf(ets_putc, format, arglist);
    va_end(arglist);
    return ret;
}

int ICACHE_RAM_ATTR sprintf(char* buffer, const char* format, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, format);
    ret = ets_vsprintf(buffer, format, arglist);
    va_end(arglist);
    return ret;
}

int ICACHE_RAM_ATTR snprintf(char* buffer, size_t size, const char* format, ...) {
    int ret;
    va_list arglist;
    va_start(arglist, format);
    ret = ets_vsnprintf(buffer, size, format, arglist);
    va_end(arglist);
    return ret;
}

int ICACHE_RAM_ATTR vprintf(const char * format, va_list arg) {
    return ets_vprintf(ets_putc, format, arg);
}

int ICACHE_RAM_ATTR vsnprintf(char * buffer, size_t size, const char * format, va_list arg) {
    return ets_vsnprintf(buffer, size, format, arg);
}
#endif
