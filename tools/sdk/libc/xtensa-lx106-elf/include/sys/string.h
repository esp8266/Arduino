/*
 * sys/string.h
 *
 * Xtensa custom PROGMEM string function definitions
 */

#ifndef _SYS_STRING_H_
#define	_SYS_STRING_H_

#include "_ansi.h"
#include <sys/reent.h>
#include <sys/cdefs.h>
#include <sys/features.h>

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#define SIZE_IRRELEVANT 0x7fffffff

#ifdef __cplusplus
extern "C" {
#endif

int 	 _EXFUN(memcmp_P,(const _PTR, const _PTR, size_t));
_PTR	 _EXFUN(memmem_P, (const _PTR, size_t, const _PTR, size_t));
_PTR 	 _EXFUN(memcpy_P,(_PTR __restrict, const _PTR __restrict, size_t));
_PTR	 _EXFUN(memccpy_P,(_PTR __restrict, const _PTR __restrict, int, size_t));
_PTR     _EXFUN(memchr_P,(const _PTR, int, size_t));

char 	*_EXFUN(strncpy_P,(char *__restrict, const char *__restrict, size_t));
#define strcpy_P(dest, src)          strncpy_P((dest), (src), SIZE_IRRELEVANT)

char 	*_EXFUN(strncat_P,(char *__restrict, const char *__restrict, size_t));
#define strcat_P(dest, src)          strncat_P((dest), (src), SIZE_IRRELEVANT)

int	 _EXFUN(strncmp_P,(const char *, const char *, size_t));
#define strcmp_P(str1, str2P)          strncmp_P((str1), (str2P), SIZE_IRRELEVANT)

int	_EXFUN(strncasecmp_P,(const char *, const char *, size_t));
#define strcasecmp_P(str1, str2P)          strncasecmp_P((str1), (str2P), SIZE_IRRELEVANT)

size_t	 _EXFUN(strnlen_P,(const char *, size_t));
#define strlen_P(strP)          strnlen_P((strP), SIZE_IRRELEVANT)

char 	*_EXFUN(strstr_P,(const char *, const char *));

#ifdef __cplusplus
}
#endif


#endif /* _SYS_STRING_H_ */
