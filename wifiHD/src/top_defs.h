/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _TOP_DEFS_H
#define _TOP_DEFS_H

#define ARRAY_SIZE(a) sizeof(a) / sizeof((a)[0])

#if __GNUC__
#define WEAK_DECL __attribute__ ((__weak__))
#else
 #error "Unsupported compiler"
#endif


#if 0
#include <stdio.h>
/*
 * These functions should _NOT_ be used, call iprintf, sniprintf, iscanf, siscanf etc
 * instead. Those functions do not have support for floating point formats.
 * Not using these functions saves 27kB of code.
 */
extern int printf(const char *format, ...) __attribute__ ((deprecated));
extern int sprintf(char *str, const char *format, ...) __attribute__ ((deprecated));
extern int snprintf(char *str, size_t size, const char *format, ...)  __attribute__ ((deprecated));

int vprintf(const char *format, va_list ap) __attribute__ ((deprecated));
int vfprintf(FILE *stream, const char *format, va_list ap) __attribute__ ((deprecated));
int vsprintf(char *str, const char *format, va_list ap) __attribute__ ((deprecated));
int vsnprintf(char *str, size_t size, const char *format, va_list ap) __attribute__ ((deprecated));

int scanf(const char *format, ...) __attribute__ ((deprecated));
int fscanf(FILE *stream, const char *format, ...) __attribute__ ((deprecated));
int sscanf(const char *str, const char *format, ...) __attribute__ ((deprecated));

int vscanf(const char *format, va_list ap) __attribute__ ((deprecated));
int vsscanf(const char *str, const char *format, va_list ap) __attribute__ ((deprecated));
int vfscanf(FILE *stream, const char *format, va_list ap) __attribute__ ((deprecated));
#endif

#endif
