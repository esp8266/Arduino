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
#include <stdio.h>      // FILE
#include <stdlib.h>     // abort()
#include <sys/stat.h>   // struct stat
#include <unistd.h>     // ..._FILENO
#include <sys/reent.h>  // struct _reent

#include "ets_sys.h"  // ets_...
#include "c_types.h"  // IRAM_ATTR

extern "C"
{

int IRAM_ATTR _open_r(struct _reent* unused, const char* ptr, int flags, int mode)
{
    (void)unused;
    (void)ptr;
    (void)flags;
    (void)mode;
    return 0;
}

int IRAM_ATTR _close_r(struct _reent* unused, int file)
{
    (void)unused;
    (void)file;
    return 0;
}

int IRAM_ATTR _fstat_r(struct _reent* unused, int file, struct stat* st)
{
    (void)unused;
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

_off_t IRAM_ATTR _lseek_r(struct _reent* unused, int file, _off_t ptr, int dir)
{
    (void)unused;
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

_ssize_t IRAM_ATTR _read_r(struct _reent* unused, int file, void* ptr, size_t len)
{
    (void)unused;
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

_ssize_t IRAM_ATTR _write_r(struct _reent* unused, int file, const void* vptr, size_t len)
{
    (void)unused;
    int         pos = len;
    const char* ptr = reinterpret_cast<const char*>(vptr);
    if (file == STDOUT_FILENO)
    {
        while (pos--)
        {
            ets_putc(*ptr);
            ++ptr;
        }
    }
    return len;
}

int           _putc_r(struct _reent* unused, int c, FILE* file) __attribute__((weak));
int IRAM_ATTR _putc_r(struct _reent* unused, int c, FILE* file)
{
    (void)unused;
    if (file->_file == STDOUT_FILENO)
    {
        ets_putc(c);
        return c;
    }
    return EOF;
}

int IRAM_ATTR puts(const char* str)
{
    char c;
    while ((c = *str) != 0)
    {
        ets_putc(c);
        ++str;
    }
    ets_putc('\n');
    return true;
}

// macro in stdio.h, usually redirected to _putc_r
// in case its not, make sure to have a stub
#undef putchar

int IRAM_ATTR putchar(int c)
{
    ets_putc(c);
    return c;
}

void _exit(int status)
{
    (void)status;
    abort();
}

int atexit(void (*func)()) __attribute__((weak));
int atexit(void (*func)())
{
    (void)func;
    return 0;
}

}  // extern "C"
