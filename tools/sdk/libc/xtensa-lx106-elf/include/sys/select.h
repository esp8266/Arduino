#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

/* We don't define fd_set and friends if we are compiling POSIX
   source, or if we have included (or may include as indicated
   by __USE_W32_SOCKETS) the W32api winsock[2].h header which
   defines Windows versions of them.   Note that a program which
   includes the W32api winsock[2].h header must know what it is doing;
   it must not call the Cygwin select function.
*/
# if !(defined (_WINSOCK_H) || defined (_WINSOCKAPI_) || defined (__USE_W32_SOCKETS))

#include <sys/cdefs.h>
#include <sys/_sigset.h>
#include <sys/_timeval.h>
#include <sys/timespec.h>

#if !defined(_SIGSET_T_DECLARED)
#define	_SIGSET_T_DECLARED
typedef	__sigset_t	sigset_t;
#endif

#  define _SYS_TYPES_FD_SET
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	64
#endif

typedef unsigned long	__fd_mask;
#if __BSD_VISIBLE
typedef __fd_mask	fd_mask;
#endif

#define _NFDBITS	((int)sizeof(__fd_mask) * 8) /* bits per mask */
#if __BSD_VISIBLE
#define NFDBITS		_NFDBITS
#endif

#ifndef	_howmany
#define	_howmany(x,y)	(((x) + ((y) - 1)) / (y))
#endif

typedef	struct fd_set {
	__fd_mask	__fds_bits[_howmany(FD_SETSIZE, _NFDBITS)];
} fd_set;
#if __BSD_VISIBLE
#define fds_bits	__fds_bits
#endif

#define __fdset_mask(n)	((__fd_mask)1 << ((n) % _NFDBITS))
#define FD_CLR(n, p)	((p)->__fds_bits[(n)/_NFDBITS] &= ~__fdset_mask(n))
#if __BSD_VISIBLE
#define FD_COPY(f, t)	(void)(*(t) = *(f))
#endif
#define FD_ISSET(n, p)	(((p)->__fds_bits[(n)/_NFDBITS] & __fdset_mask(n)) != 0)
#define FD_SET(n, p)	((p)->__fds_bits[(n)/_NFDBITS] |= __fdset_mask(n))
#define FD_ZERO(p) do {					\
        fd_set *_p;					\
        __size_t _n;					\
							\
        _p = (p);					\
        _n = _howmany(FD_SETSIZE, _NFDBITS);		\
        while (_n > 0)					\
                _p->__fds_bits[--_n] = 0;		\
} while (0)

#if !defined (__INSIDE_CYGWIN_NET__)

__BEGIN_DECLS

int select __P ((int __n, fd_set *__readfds, fd_set *__writefds,
		 fd_set *__exceptfds, struct timeval *__timeout));
#if __POSIX_VISIBLE >= 200112
int pselect __P ((int __n, fd_set *__readfds, fd_set *__writefds,
		  fd_set *__exceptfds, const struct timespec *__timeout,
		  const sigset_t *__set));
#endif

__END_DECLS

#endif /* !__INSIDE_CYGWIN_NET__ */

#endif /* !(_WINSOCK_H || _WINSOCKAPI_ || __USE_W32_SOCKETS) */

#endif /* sys/select.h */
