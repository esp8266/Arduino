/*
 * stdio_ext.h
 *
 * Definitions for I/O internal operations, originally from Solaris.
 */

#ifndef _STDIO_EXT_H_
#define _STDIO_EXT_H_

#ifdef __rtems__
#error "<stdio_ext.h> not supported"
#endif

#include <stdio.h>

_BEGIN_STD_C

void	 _EXFUN(__fpurge,(FILE *));

_END_STD_C

#endif /* _STDIO_EXT_H_ */
