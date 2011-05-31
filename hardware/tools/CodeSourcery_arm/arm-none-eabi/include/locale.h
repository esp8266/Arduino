/*
	locale.h
	Values appropriate for the formatting of monetary and other
	numberic quantities.
*/

#ifndef _LOCALE_H_
#define _LOCALE_H_

#include "_ansi.h"

  /* Indicate that we honor AEABI portability if requested.  */
#if defined _AEABI_PORTABILITY_LEVEL && _AEABI_PORTABILITY_LEVEL != 0 && !defined _AEABI_PORTABLE
# define _AEABI_PORTABLE
#endif

#ifndef NULL
#define NULL    0
#endif

_BEGIN_STD_C

#ifdef _AEABI_PORTABLE

extern _CONST int __aeabi_LC_COLLATE;
extern _CONST int __aeabi_LC_CTYPE;
extern _CONST int __aeabi_LC_MONETARY;
extern _CONST int __aeabi_LC_NUMERIC;
extern _CONST int __aeabi_LC_TIME;
extern _CONST int __aeabi_LC_ALL;

#define LC_COLLATE (__aeabi_LC_COLLATE)
#define LC_CTYPE (__aeabi_LC_CTYPE)
#define LC_MONETARY (__aeabi_LC_MONETARY)
#define LC_NUMERIC (__aeabi_LC_NUMERIC)
#define LC_TIME (__aeabi_LC_TIME)
#define LC_ALL (__aeabi_LC_ALL)

struct __aeabi_lconv {
  char *decimal_point;
  char *thousands_sep;
  char *grouping;
  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char int_frac_digits;
  char frac_digits;
  char p_cs_precedes;
  char p_sep_by_space;
  char n_cs_precedes;
  char n_sep_by_space;
  char p_sign_posn;
  char n_sign_posn;

  /* The following fields are added by C99.  */
  char int_p_cs_precedes;
  char int_n_cs_precedes;
  char int_p_sep_by_space;
  char int_n_sep_by_space;
  char int_p_sign_posn;
  char int_n_sign_posn;
};
#else

#define LC_ALL	    0
#define LC_COLLATE  1
#define LC_CTYPE    2
#define LC_MONETARY 3
#define LC_NUMERIC  4
#define LC_TIME     5
#define LC_MESSAGES 6

struct lconv
{
  char *decimal_point;
  char *thousands_sep;
  char *grouping;
  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char int_frac_digits;
  char frac_digits;
  char p_cs_precedes;
  char p_sep_by_space;
  char n_cs_precedes;
  char n_sep_by_space;
  char p_sign_posn;
  char n_sign_posn;
  char int_n_cs_precedes;
  char int_n_sep_by_space;
  char int_n_sign_posn;
  char int_p_cs_precedes;
  char int_p_sep_by_space;
  char int_p_sign_posn;
};
#endif /* _AEABI_PORTABLE */

#ifndef _REENT_ONLY
char *_EXFUN(setlocale,(int category, const char *locale));
# ifdef _AEABI_PORTABLE
struct __aeabi_lconv *_EXFUN(__aeabi_localeconv,(void));
# else
struct lconv *_EXFUN(localeconv,(void));
# endif
#endif

struct _reent;
char *_EXFUN(_setlocale_r,(struct _reent *, int category, const char *locale));
struct lconv *_EXFUN(_localeconv_r,(struct _reent *));

_END_STD_C

#endif /* _LOCALE_H_ */
