/* Copyright (C) 2004, 2005 Free Software Foundation, Inc.
   Contributed by Apple, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* As a special exception, if you include this header file into source
   files compiled by GCC, this header file does not by itself cause
   the resulting executable to be covered by the GNU General Public
   License.  This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU General
   Public License.  */

/*
 * ISO C Standard:  7.22  Type-generic math <tgmath.h>
 */

#ifndef _TGMATH_H
#define _TGMATH_H

#include <math.h>

#ifndef __cplusplus
#include <complex.h>

/* Naming convention: generic macros are defining using
   __TGMATH_CPLX*, __TGMATH_REAL*, and __TGMATH_CPLX_ONLY.  _CPLX
   means the generic argument(s) may be real or complex, _REAL means
   real only, _CPLX means complex only.  If there is no suffix, we are
   defining a function of one generic argument.  If the suffix is _n
   it is a function of n generic arguments.  If the suffix is _m_n it
   is a function of n arguments, the first m of which are generic.  We
   only define these macros for values of n and/or m that are needed. */

/* The general rules for generic macros are given in 7.22 paragraphs 1 and 2.
   If any generic parameter is complex, we use a complex version.  Otherwise
   we use a real version.  If the real part of any generic parameter is long
   double, we use the long double version.  Otherwise if the real part of any
   generic parameter is double or of integer type, we use the double version.
   Otherwise we use the float version. */

#define __tg_cplx(expr) \
  __builtin_classify_type(expr) == 9

#define __tg_ldbl(expr) \
  __builtin_types_compatible_p(__typeof__(expr), long double)

#define __tg_dbl(expr)                                       \
  (__builtin_types_compatible_p(__typeof__(expr), double)    \
   || __builtin_classify_type(expr) == 1)

#define __tg_choose(x,f,d,l)                                  \
  __builtin_choose_expr(__tg_ldbl(x), l,                      \
                        __builtin_choose_expr(__tg_dbl(x), d, \
                                              f))

#define __tg_choose_2(x,y,f,d,l)                                             \
  __builtin_choose_expr(__tg_ldbl(x) || __tg_ldbl(y), l,                     \
                        __builtin_choose_expr(__tg_dbl(x) || __tg_dbl(y), d, \
                                              f))

#define __tg_choose_3(x,y,z,f,d,l)                                        \
   __builtin_choose_expr(__tg_ldbl(x) || __tg_ldbl(y) || __tg_ldbl(z), l, \
                        __builtin_choose_expr(__tg_dbl(x) || __tg_dbl(y)  \
                                              || __tg_dbl(z), d,          \
                                              f))

#define __TGMATH_CPLX(z,R,C)                                                  \
  __builtin_choose_expr (__tg_cplx(z),                                        \
                         __tg_choose (__real__(z), C##f(z), (C)(z), C##l(z)), \
                         __tg_choose (z, R##f(z), (R)(z), R##l(z)))

#define __TGMATH_CPLX_2(z1,z2,R,C)                                             \
  __builtin_choose_expr (__tg_cplx(z1) || __tg_cplx(z2),                       \
                         __tg_choose_2 (__real__(z1), __real__(z2),            \
                                        C##f(z1,z2), (C)(z1,z2), C##l(z1,z2)), \
                         __tg_choose_2 (z1, z2,                                \
                                        R##f(z1,z2), (R)(z1,z2), R##l(z1,z2)))

#define __TGMATH_REAL(x,R) \
  __tg_choose (x, R##f(x), (R)(x), R##l(x))
#define __TGMATH_REAL_2(x,y,R) \
  __tg_choose_2 (x, y, R##f(x,y), (R)(x,y), R##l(x,y))
#define __TGMATH_REAL_3(x,y,z,R) \
  __tg_choose_3 (x, y, z, R##f(x,y,z), (R)(x,y,z), R##l(x,y,z))
#define __TGMATH_REAL_1_2(x,y,R) \
  __tg_choose (x, R##f(x,y), (R)(x,y), R##l(x,y))
#define __TGMATH_REAL_2_3(x,y,z,R) \
  __tg_choose_2 (x, y, R##f(x,y,z), (R)(x,y,z), R##l(x,y,z))
#define __TGMATH_CPLX_ONLY(z,C) \
  __tg_choose (__real__(z), C##f(z), (C)(z), C##l(z))

/* Functions defined in both <math.h> and <complex.h> (7.22p4) */
#define acos(z)          __TGMATH_CPLX(z, acos, cacos)
#define asin(z)          __TGMATH_CPLX(z, asin, casin)
#define atan(z)          __TGMATH_CPLX(z, atan, catan)
#define acosh(z)         __TGMATH_CPLX(z, acosh, cacosh)
#define asinh(z)         __TGMATH_CPLX(z, asinh, casinh)
#define atanh(z)         __TGMATH_CPLX(z, atanh, catanh)
#define cos(z)           __TGMATH_CPLX(z, cos, ccos)
#define sin(z)           __TGMATH_CPLX(z, sin, csin)
#define tan(z)           __TGMATH_CPLX(z, tan, ctan)
#define cosh(z)          __TGMATH_CPLX(z, cosh, ccosh)
#define sinh(z)          __TGMATH_CPLX(z, sinh, csinh)
#define tanh(z)          __TGMATH_CPLX(z, tanh, ctanh)
#define exp(z)           __TGMATH_CPLX(z, exp, cexp)
#define log(z)           __TGMATH_CPLX(z, log, clog)
#define pow(z1,z2)       __TGMATH_CPLX_2(z1, z2, pow, cpow)
#define sqrt(z)          __TGMATH_CPLX(z, sqrt, csqrt)
#define fabs(z)          __TGMATH_CPLX(z, fabs, cabs)

/* Functions defined in <math.h> only (7.22p5) */
#define atan2(x,y)       __TGMATH_REAL_2(x, y, atan2)
#define cbrt(x)          __TGMATH_REAL(x, cbrt)
#define ceil(x)          __TGMATH_REAL(x, ceil)
#define copysign(x,y)    __TGMATH_REAL_2(x, y, copysign)
#define erf(x)           __TGMATH_REAL(x, erf)
#define erfc(x)          __TGMATH_REAL(x, erfc)
#define exp2(x)          __TGMATH_REAL(x, exp2)
#define expm1(x)         __TGMATH_REAL(x, expm1)
#define fdim(x,y)        __TGMATH_REAL_2(x, y, fdim)
#define floor(x)         __TGMATH_REAL(x, floor)
#define fma(x,y,z)       __TGMATH_REAL_3(x, y, z, fma)
#define fmax(x,y)        __TGMATH_REAL_2(x, y, fmax)
#define fmin(x,y)        __TGMATH_REAL_2(x, y, fmin)
#define fmod(x,y)        __TGMATH_REAL_2(x, y, fmod)
#define frexp(x,y)       __TGMATH_REAL_1_2(x, y, frexp)
#define hypot(x,y)       __TGMATH_REAL_2(x, y, hypot)
#define ilogb(x)         __TGMATH_REAL(x, ilogb)
#define ldexp(x,y)       __TGMATH_REAL_1_2(x, y, ldexp)
#define lgamma(x)        __TGMATH_REAL(x, lgamma)
#define llrint(x)        __TGMATH_REAL(x, llrint)
#define llround(x)       __TGMATH_REAL(x, llround)
#define log10(x)         __TGMATH_REAL(x, log10)
#define log1p(x)         __TGMATH_REAL(x, log1p)
#define log2(x)          __TGMATH_REAL(x, log2)
#define logb(x)          __TGMATH_REAL(x, logb)
#define lrint(x)         __TGMATH_REAL(x, lrint)
#define lround(x)        __TGMATH_REAL(x, lround)
#define nearbyint(x)     __TGMATH_REAL(x, nearbyint)
#define nextafter(x,y)   __TGMATH_REAL_2(x, y, nextafter)
#define nexttoward(x,y)  __TGMATH_REAL_1_2(x, y, nexttoward)
#define remainder(x,y)   __TGMATH_REAL_2(x, y, remainder)
#define remquo(x,y,z)    __TGMATH_REAL_2_3(x, y, z, remquo)
#define rint(x)          __TGMATH_REAL(x, rint)
#define round(x)         __TGMATH_REAL(x, round)
#define scalbn(x,y)      __TGMATH_REAL_1_2(x, y, scalbn)
#define scalbln(x,y)     __TGMATH_REAL_1_2(x, y, scalbln)
#define tgamma(x)        __TGMATH_REAL(x, tgamma)
#define trunc(x)         __TGMATH_REAL(x, trunc)

/* Functions defined in <complex.h> only (7.22p6) */
#define carg(z)          __TGMATH_CPLX_ONLY(z, carg)
#define cimag(z)         __TGMATH_CPLX_ONLY(z, cimag)
#define conj(z)          __TGMATH_CPLX_ONLY(z, conj)
#define cproj(z)         __TGMATH_CPLX_ONLY(z, cproj)
#define creal(z)         __TGMATH_CPLX_ONLY(z, creal)

#endif /* __cplusplus */
#endif /* _TGMATH_H */
