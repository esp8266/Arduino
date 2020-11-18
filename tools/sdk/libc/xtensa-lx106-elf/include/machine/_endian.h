#ifndef __MACHINE_ENDIAN_H__
#error "must be included via <machine/endian.h>"
#endif /* !__MACHINE_ENDIAN_H__ */

#include <sys/config.h>

#ifdef __PPC__
/* Get rid of GCC builtin defines on PowerPC */
#ifdef _BIG_ENDIAN
#undef _BIG_ENDIAN
#endif
#ifdef _LITTLE_ENDIAN
#undef _LITTLE_ENDIAN
#endif
#endif /* __PPC__ */

#ifndef _LITTLE_ENDIAN
#define	_LITTLE_ENDIAN	1234
#endif

#ifndef _BIG_ENDIAN
#define	_BIG_ENDIAN	4321
#endif

#ifndef _PDP_ENDIAN
#define	_PDP_ENDIAN	3412
#endif

#ifndef _BYTE_ORDER
#if defined(__IEEE_LITTLE_ENDIAN) || defined(__IEEE_BYTES_LITTLE_ENDIAN)
#define	_BYTE_ORDER	_LITTLE_ENDIAN
#else
#define	_BYTE_ORDER	_BIG_ENDIAN
#endif
#endif
