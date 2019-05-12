/* sys/stdio.h - #defines for legacy PROGMEM _P functions (no longer needed) */

#ifndef _SYS_STDIO_H_
#define _SYS_STDIO_H_

#include <sys/pgmspace.h>
#include <stdarg.h>

#ifndef _NEWLIB_STDIO_H
#define _NEWLIB_STDIO_H

#include <sys/lock.h>
#include <sys/reent.h>

/* Internal locking macros, used to protect stdio functions.  In the
   general case, expand to nothing. Use __SSTR flag in FILE _flags to
   detect if FILE is private to sprintf/sscanf class of functions; if
   set then do nothing as lock is not initialised. */
#if !defined(_flockfile)
#ifndef __SINGLE_THREAD__
#  define _flockfile(fp) (((fp)->_flags & __SSTR) ? 0 : __lock_acquire_recursive((fp)->_lock))
#else
#  define _flockfile(fp)	(_CAST_VOID 0)
#endif
#endif

#if !defined(_funlockfile)
#ifndef __SINGLE_THREAD__
#  define _funlockfile(fp) (((fp)->_flags & __SSTR) ? 0 : __lock_release_recursive((fp)->_lock))
#else
#  define _funlockfile(fp)	(_CAST_VOID 0)
#endif
#endif

#endif /* _NEWLIB_STDIO_H */

#ifdef __cplusplus
extern "C" {
#endif

int     printf_P(PGM_P formatP, ...) __attribute__((format(printf, 1, 2)));
int     sprintf_P(char *str, PGM_P formatP, ...) __attribute__((format(printf, 2, 3)));
int     snprintf_P(char *str, size_t strSize, PGM_P formatP, ...) __attribute__((format(printf, 3, 4)));
int     vsnprintf_P(char *str, size_t strSize, PGM_P formatP, va_list ap) __attribute__((format(printf, 3, 0)));

#ifdef __cplusplus
}
#endif

#endif
