#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include "_ansi.h"
#include <sys/signal.h>

_BEGIN_STD_C

  /* Indicate that we honor AEABI portability if requested.  */
#if defined _AEABI_PORTABILITY_LEVEL && _AEABI_PORTABILITY_LEVEL != 0 && !defined _AEABI_PORTABLE
# define _AEABI_PORTABLE
#endif

typedef int	sig_atomic_t;		/* Atomic entity type (ANSI) */
#ifndef _POSIX_SOURCE
typedef _sig_func_ptr sighandler_t;	/* glibc naming */
#endif /* !_POSIX_SOURCE */

#ifdef _AEABI_PORTABLE
void _EXFUN(__aeabi_SIG_DFL, (int));
void _EXFUN(__aeabi_SIG_IGN, (int));
void _EXFUN(__aeabi_SIG_ERR, (int));

#define SIG_DFL (__aeabi_SIG_DFL)
#define SIG_IGN (__aeabi_SIG_IGN)
#define SIG_ERR (__aeabi_SIG_ERR)
#else
#define SIG_DFL ((_sig_func_ptr)0)	/* Default action */
#define SIG_IGN ((_sig_func_ptr)1)	/* Ignore action */
#define SIG_ERR ((_sig_func_ptr)-1)	/* Error return */
#endif

struct _reent;

_sig_func_ptr _EXFUN(_signal_r, (struct _reent *, int, _sig_func_ptr));
int	_EXFUN(_raise_r, (struct _reent *, int));

#ifndef _REENT_ONLY
_sig_func_ptr _EXFUN(signal, (int, _sig_func_ptr));
int	_EXFUN(raise, (int));
#endif

_END_STD_C

#endif /* _SIGNAL_H_ */
