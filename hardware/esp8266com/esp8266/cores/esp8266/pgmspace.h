#ifndef __PGMSPACE_H_
#define __PGMSPACE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#ifdef __cplusplus
}
#endif

#define PROGMEM
#define PGM_P  		const char *
#define PSTR(str) 	(str)

#define vsnprintf_P(...) 	ets_vsnprintf( __VA_ARGS__ )
#define snprintf_P(...) 	snprintf( __VA_ARGS__ )
#define printf_P(...)       os_printf(__VA_ARGS__)

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

#define memcpy_P(dest, src, num) 	memcpy((dest), (src), (num))
#define strcpy_P(dest, src) 		strcpy((dest), (src))
#define strcat_P(dest, src) 		strcat((dest), (src))
#define strcmp_P(a, b) 				strcmp((a), (b))
#define strstr_P(a, b) 				strstr((a), (b))
#define strlen_P(s) 				strlen((const char *)(s))
#define strcasecmp_P(a, b)          strcasecmp((a), (b))
#define strncpy_P(dest, src, size) 		strncpy((dest), (src), (size))
#define strncat_P(dest, src, size) 		strncat((dest), (src), (size))
#define strncmp_P(a, b, size) 			strncmp((a), (b), (size))
#define strnlen_P(s, size) 				strnlen((const char *)(s), (size))
#define strncasecmp_P(a, b, size)       strncasecmp((a), (b), (size))

#define pgm_read_byte(addr) 		(*(const unsigned char *)(addr))
#define pgm_read_word(addr) 		(*(const unsigned short *)(addr))
#define pgm_read_dword(addr) 		(*(const unsigned long *)(addr))
#define pgm_read_float(addr) 		(*(const float *)(addr))

#define pgm_read_byte_near(addr) 	pgm_read_byte(addr)
#define pgm_read_word_near(addr) 	pgm_read_word(addr)
#define pgm_read_dword_near(addr) 	pgm_read_dword(addr)
#define pgm_read_float_near(addr)	pgm_read_float(addr)
#define pgm_read_byte_far(addr) 	pgm_read_byte(addr)
#define pgm_read_word_far(addr) 	pgm_read_word(addr)
#define pgm_read_dword_far(addr) 	pgm_read_dword(addr)
#define pgm_read_float_far(addr) 	pgm_read_float(addr)

#endif //__PGMSPACE_H_
