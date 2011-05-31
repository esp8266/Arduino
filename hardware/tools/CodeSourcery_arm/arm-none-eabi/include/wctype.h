#ifndef _WCTYPE_H_
#define _WCTYPE_H_

#include <_ansi.h>
#include <sys/_types.h>

#define __need_wint_t
#include <stddef.h>

#ifndef WEOF
# define WEOF ((wint_t)-1)
#endif

_BEGIN_STD_C

/* We can honor AEABI portability if this condition is true, since we
   can pretend that the integer values returned by and passed to
   functions are really pointers.  The necessary preprocessor macros are
   only defined in GCC versions >= 4.3, though.  */
#if defined __GNUC__ && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3 && __SIZEOF_INT__ == __SIZEOF_POINTER__
# if defined _AEABI_PORTABILITY_LEVEL && _AEABI_PORTABILITY_LEVEL != 0 && !defined _AEABI_PORTABLE
#  define _AEABI_PORTABLE
# endif
#endif

#ifndef _WCTYPE_T
#define _WCTYPE_T
# ifdef _AEABI_PORTABLE
typedef void *wctype_t;
# else
typedef int wctype_t;
# endif
#endif /* !_WCTYPE_T */

#ifndef _WCTRANS_T
#define _WCTRANS_T
# ifdef _AEABI_PORTABLE
typedef void *wctrans_t;
# else
typedef int wctrans_t;
# endif
#endif /* !_WCTRANS_T */

int	_EXFUN(iswalpha, (wint_t));
int	_EXFUN(iswalnum, (wint_t));
int	_EXFUN(iswblank, (wint_t));
int	_EXFUN(iswcntrl, (wint_t));
int	_EXFUN(iswctype, (wint_t, wctype_t));
int	_EXFUN(iswdigit, (wint_t));
int	_EXFUN(iswgraph, (wint_t));
int	_EXFUN(iswlower, (wint_t));
int	_EXFUN(iswprint, (wint_t));
int	_EXFUN(iswpunct, (wint_t));
int	_EXFUN(iswspace, (wint_t));
int	_EXFUN(iswupper, (wint_t));
int	_EXFUN(iswxdigit, (wint_t));
wint_t	_EXFUN(towctrans, (wint_t, wctrans_t));
wint_t	_EXFUN(towupper, (wint_t));
wint_t	_EXFUN(towlower, (wint_t));
wctrans_t _EXFUN(wctrans, (const char *));
wctype_t _EXFUN(wctype, (const char *));

_END_STD_C

#endif /* _WCTYPE_H_ */
