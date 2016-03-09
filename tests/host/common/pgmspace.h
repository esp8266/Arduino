/*
 pgmspace.cpp - PROGMEM stubs for host-side tests
 Copyright © 2016 Ivan Grokhotkov
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 */
#ifndef __PGMSPACE_H_
#define __PGMSPACE_H_

#include <stdint.h>
#include <stdio.h>

#define PROGMEM
#define PGM_P  		const char *
#define PGM_VOID_P  const void *
#define PSTR(s)     (s)


#define _SFR_BYTE(n) (n)

typedef void prog_void;
typedef char prog_char;
typedef unsigned char prog_uchar;
typedef int8_t prog_int8_t;
typedef uint8_t prog_uint8_t;
typedef int16_t prog_int16_t;
typedef uint16_t prog_uint16_t;
typedef int32_t prog_int32_t;
typedef uint32_t prog_uint32_t;

#define memcmp_P        memcmp
#define memccpy_P       memccpy
#define memmem_P        memmem
#define memcpy_P        memcpy

#define strncpy_P       strncpy
#define strcpy_P        strcpy

#define strncat_P       strncat
#define strcat_P        strcat

#define strncmp_P       strncmp
#define strcmp_P        strcmp

#define strncasecmp_P   strncasecmp
#define strcasecmp_P    strcasecmp

#define strnlen_P       strnlen
#define strlen_P        strlen

#define printf_P        printf
#define sprintf_P       sprintf
#define snprintf_P      snprintf
#define	vsnprintf_P     vsnprintf

#define pgm_read_byte(addr)     (*reinterpret_cast<const uint8_t*>(addr))
#define pgm_read_word(addr)     (*reinterpret_cast<const uint16_t*>(addr))
#define pgm_read_float(addr)    (*reinterpret_cast<const float*>(addr))
#define pgm_read_dword(addr)    (*reinterpret_cast<const uint32_t*>(addr))

#define pgm_read_byte_near(addr) 	pgm_read_byte(addr)
#define pgm_read_word_near(addr) 	pgm_read_word(addr)
#define pgm_read_dword_near(addr) 	pgm_read_dword(addr)
#define pgm_read_float_near(addr)	pgm_read_float(addr)
#define pgm_read_byte_far(addr) 	pgm_read_byte(addr)
#define pgm_read_word_far(addr) 	pgm_read_word(addr)
#define pgm_read_dword_far(addr) 	pgm_read_dword(addr)
#define pgm_read_float_far(addr) 	pgm_read_float(addr)

#endif //__PGMSPACE_H_
