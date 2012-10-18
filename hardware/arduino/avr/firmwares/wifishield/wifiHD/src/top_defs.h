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

#include <stdlib.h>
#include <stdint.h>

#define ARRAY_SIZE(a) sizeof(a) / sizeof((a)[0])

#ifndef UNREF
#define UNREF(x) x = x
#endif

#if __GNUC__
#ifdef __KERNEL__
#define WEAK_DECL
#else
#define WEAK_DECL __attribute__ ((__weak__))
#endif
#define PACKED __attribute__ ((__packed__))
#define USED __attribute__ ((__used__))
#else
 #error "Unsupported compiler"
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
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



#if defined(__linux__) || defined(__APPLE__)
 #include <stdint.h>
 #include <assert.h>
 #define sniprintf snprintf
 #define asiprintf asprintf
 #define printk printf
 #define siscanf sscanf

 #define WL_ASSERT(x) assert(x)
 #define WL_DEBUG(args...) printf(args)

 #ifdef NO_LWIP
  /* IP address representation from lwIP */
  struct ip_addr {
         uint32_t addr;
  } PACKED;
 #endif

 #define FEAT_SOCKETS 

#else
 #define WL_ASSERT(cond) do {                                    \
                if (!(cond)) {                                  \
                        printk("%s:%d\n", __FILE__, __LINE__);  \
                        for(;;);                                \
                }                                               \
        } while(0)
 #define WL_DEBUG(args...) printk(args)


#endif
