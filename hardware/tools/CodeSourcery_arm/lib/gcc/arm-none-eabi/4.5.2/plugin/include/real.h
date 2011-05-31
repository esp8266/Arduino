/* Definitions of floating-point access for GNU compiler.
   Copyright (C) 1989, 1991, 1994, 1996, 1997, 1998, 1999,
   2000, 2002, 2003, 2004, 2005, 2007, 2008, 2009, 2010
   Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3, or (at your option) any later
   version.

   GCC is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef GCC_REAL_H
#define GCC_REAL_H

#ifndef GENERATOR_FILE
#include <gmp.h>
#include <mpfr.h>
#include <mpc.h>
extern tree do_mpc_arg2 (tree, tree, tree, int, int (*)(mpc_ptr, mpc_srcptr, mpc_srcptr, mpc_rnd_t));
#endif
#include "machmode.h"

/* An expanded form of the represented number.  */

/* Enumerate the special cases of numbers that we encounter.  */
enum real_value_class {
  rvc_zero,
  rvc_normal,
  rvc_inf,
  rvc_nan
};

#define SIGNIFICAND_BITS	(128 + HOST_BITS_PER_LONG)
#define EXP_BITS		(32 - 6)
#define MAX_EXP			((1 << (EXP_BITS - 1)) - 1)
#define SIGSZ			(SIGNIFICAND_BITS / HOST_BITS_PER_LONG)
#define SIG_MSB			((unsigned long)1 << (HOST_BITS_PER_LONG - 1))

struct GTY(()) real_value {
  /* Use the same underlying type for all bit-fields, so as to make
     sure they're packed together, otherwise REAL_VALUE_TYPE_SIZE will
     be miscomputed.  */
  unsigned int /* ENUM_BITFIELD (real_value_class) */ cl : 2;
  unsigned int decimal : 1;
  unsigned int sign : 1;
  unsigned int signalling : 1;
  unsigned int canonical : 1;
  unsigned int uexp : EXP_BITS;
  unsigned long sig[SIGSZ];
};

#define REAL_EXP(REAL) \
  ((int)((REAL)->uexp ^ (unsigned int)(1 << (EXP_BITS - 1))) \
   - (1 << (EXP_BITS - 1)))
#define SET_REAL_EXP(REAL, EXP) \
  ((REAL)->uexp = ((unsigned int)(EXP) & (unsigned int)((1 << EXP_BITS) - 1)))

/* Various headers condition prototypes on #ifdef REAL_VALUE_TYPE, so it
   needs to be a macro.  We do need to continue to have a structure tag
   so that other headers can forward declare it.  */
#define REAL_VALUE_TYPE struct real_value

/* We store a REAL_VALUE_TYPE into an rtx, and we do this by putting it in
   consecutive "w" slots.  Moreover, we've got to compute the number of "w"
   slots at preprocessor time, which means we can't use sizeof.  Guess.  */

#define REAL_VALUE_TYPE_SIZE (SIGNIFICAND_BITS + 32)
#define REAL_WIDTH \
  (REAL_VALUE_TYPE_SIZE/HOST_BITS_PER_WIDE_INT \
   + (REAL_VALUE_TYPE_SIZE%HOST_BITS_PER_WIDE_INT ? 1 : 0)) /* round up */

/* Verify the guess.  */
extern char test_real_width
  [sizeof(REAL_VALUE_TYPE) <= REAL_WIDTH*sizeof(HOST_WIDE_INT) ? 1 : -1];

/* Calculate the format for CONST_DOUBLE.  We need as many slots as
   are necessary to overlay a REAL_VALUE_TYPE on them.  This could be
   as many as four (32-bit HOST_WIDE_INT, 128-bit REAL_VALUE_TYPE).

   A number of places assume that there are always at least two 'w'
   slots in a CONST_DOUBLE, so we provide them even if one would suffice.  */

#if REAL_WIDTH == 1
# define CONST_DOUBLE_FORMAT	 "ww"
#else
# if REAL_WIDTH == 2
#  define CONST_DOUBLE_FORMAT	 "ww"
# else
#  if REAL_WIDTH == 3
#   define CONST_DOUBLE_FORMAT	 "www"
#  else
#   if REAL_WIDTH == 4
#    define CONST_DOUBLE_FORMAT	 "wwww"
#   else
#    if REAL_WIDTH == 5
#     define CONST_DOUBLE_FORMAT "wwwww"
#    else
#     if REAL_WIDTH == 6
#      define CONST_DOUBLE_FORMAT "wwwwww"
#     else
       #error "REAL_WIDTH > 6 not supported"
#     endif
#    endif
#   endif
#  endif
# endif
#endif


/* Describes the properties of the specific target format in use.  */
struct real_format
{
  /* Move to and from the target bytes.  */
  void (*encode) (const struct real_format *, long *,
		  const REAL_VALUE_TYPE *);
  void (*decode) (const struct real_format *, REAL_VALUE_TYPE *,
		  const long *);

  /* The radix of the exponent and digits of the significand.  */
  int b;

  /* Size of the significand in digits of radix B.  */
  int p;

  /* Size of the significant of a NaN, in digits of radix B.  */
  int pnan;

  /* The minimum negative integer, x, such that b**(x-1) is normalized.  */
  int emin;

  /* The maximum integer, x, such that b**(x-1) is representable.  */
  int emax;

  /* The bit position of the sign bit, for determining whether a value
     is positive/negative, or -1 for a complex encoding.  */
  int signbit_ro;

  /* The bit position of the sign bit, for changing the sign of a number,
     or -1 for a complex encoding.  */
  int signbit_rw;

  /* Default rounding mode for operations on this format.  */
  bool round_towards_zero;
  bool has_sign_dependent_rounding;

  /* Properties of the format.  */
  bool has_nans;
  bool has_inf;
  bool has_denorm;
  bool has_signed_zero;
  bool qnan_msb_set;
  bool canonical_nan_lsbs_set;
};


/* The target format used for each floating point mode.
   Float modes are followed by decimal float modes, with entries for
   float modes indexed by (MODE - first float mode), and entries for
   decimal float modes indexed by (MODE - first decimal float mode) +
   the number of float modes.  */
extern const struct real_format *
  real_format_for_mode[MAX_MODE_FLOAT - MIN_MODE_FLOAT + 1
		       + MAX_MODE_DECIMAL_FLOAT - MIN_MODE_DECIMAL_FLOAT + 1];

#define REAL_MODE_FORMAT(MODE)						\
  (real_format_for_mode[DECIMAL_FLOAT_MODE_P (MODE)			\
			? (((MODE) - MIN_MODE_DECIMAL_FLOAT)		\
			   + (MAX_MODE_FLOAT - MIN_MODE_FLOAT + 1))	\
			: ((MODE) - MIN_MODE_FLOAT)])

#define FLOAT_MODE_FORMAT(MODE) \
  (REAL_MODE_FORMAT (SCALAR_FLOAT_MODE_P (MODE)? (MODE) \
					       : GET_MODE_INNER (MODE)))

/* The following macro determines whether the floating point format is
   composite, i.e. may contain non-consecutive mantissa bits, in which
   case compile-time FP overflow may not model run-time overflow.  */
#define MODE_COMPOSITE_P(MODE) \
  (FLOAT_MODE_P (MODE) \
   && FLOAT_MODE_FORMAT (MODE)->pnan < FLOAT_MODE_FORMAT (MODE)->p)

/* Accessor macros for format properties.  */
#define MODE_HAS_NANS(MODE) \
  (FLOAT_MODE_P (MODE) && FLOAT_MODE_FORMAT (MODE)->has_nans)
#define MODE_HAS_INFINITIES(MODE) \
  (FLOAT_MODE_P (MODE) && FLOAT_MODE_FORMAT (MODE)->has_inf)
#define MODE_HAS_SIGNED_ZEROS(MODE) \
  (FLOAT_MODE_P (MODE) && FLOAT_MODE_FORMAT (MODE)->has_signed_zero)
#define MODE_HAS_SIGN_DEPENDENT_ROUNDING(MODE) \
  (FLOAT_MODE_P (MODE) \
   && FLOAT_MODE_FORMAT (MODE)->has_sign_dependent_rounding)

/* True if the given mode has a NaN representation and the treatment of
   NaN operands is important.  Certain optimizations, such as folding
   x * 0 into 0, are not correct for NaN operands, and are normally
   disabled for modes with NaNs.  The user can ask for them to be
   done anyway using the -funsafe-math-optimizations switch.  */
#define HONOR_NANS(MODE) \
  (MODE_HAS_NANS (MODE) && !flag_finite_math_only)

/* Like HONOR_NANs, but true if we honor signaling NaNs (or sNaNs).  */
#define HONOR_SNANS(MODE) (flag_signaling_nans && HONOR_NANS (MODE))

/* As for HONOR_NANS, but true if the mode can represent infinity and
   the treatment of infinite values is important.  */
#define HONOR_INFINITIES(MODE) \
  (MODE_HAS_INFINITIES (MODE) && !flag_finite_math_only)

/* Like HONOR_NANS, but true if the given mode distinguishes between
   positive and negative zero, and the sign of zero is important.  */
#define HONOR_SIGNED_ZEROS(MODE) \
  (MODE_HAS_SIGNED_ZEROS (MODE) && flag_signed_zeros)

/* Like HONOR_NANS, but true if given mode supports sign-dependent rounding,
   and the rounding mode is important.  */
#define HONOR_SIGN_DEPENDENT_ROUNDING(MODE) \
  (MODE_HAS_SIGN_DEPENDENT_ROUNDING (MODE) && flag_rounding_math)

/* Declare functions in real.c.  */

/* Binary or unary arithmetic on tree_code.  */
extern bool real_arithmetic (REAL_VALUE_TYPE *, int, const REAL_VALUE_TYPE *,
			     const REAL_VALUE_TYPE *);

/* Compare reals by tree_code.  */
extern bool real_compare (int, const REAL_VALUE_TYPE *, const REAL_VALUE_TYPE *);

/* Determine whether a floating-point value X is infinite.  */
extern bool real_isinf (const REAL_VALUE_TYPE *);

/* Determine whether a floating-point value X is a NaN.  */
extern bool real_isnan (const REAL_VALUE_TYPE *);

/* Determine whether a floating-point value X is finite.  */
extern bool real_isfinite (const REAL_VALUE_TYPE *);

/* Determine whether a floating-point value X is negative.  */
extern bool real_isneg (const REAL_VALUE_TYPE *);

/* Determine whether a floating-point value X is minus zero.  */
extern bool real_isnegzero (const REAL_VALUE_TYPE *);

/* Compare two floating-point objects for bitwise identity.  */
extern bool real_identical (const REAL_VALUE_TYPE *, const REAL_VALUE_TYPE *);

/* Extend or truncate to a new mode.  */
extern void real_convert (REAL_VALUE_TYPE *, enum machine_mode,
			  const REAL_VALUE_TYPE *);

/* Return true if truncating to NEW is exact.  */
extern bool exact_real_truncate (enum machine_mode, const REAL_VALUE_TYPE *);

/* Render R as a decimal floating point constant.  */
extern void real_to_decimal (char *, const REAL_VALUE_TYPE *, size_t,
			     size_t, int);

/* Render R as a decimal floating point constant, rounded so as to be
   parsed back to the same value when interpreted in mode MODE.  */
extern void real_to_decimal_for_mode (char *, const REAL_VALUE_TYPE *, size_t,
				      size_t, int, enum machine_mode);

/* Render R as a hexadecimal floating point constant.  */
extern void real_to_hexadecimal (char *, const REAL_VALUE_TYPE *,
				 size_t, size_t, int);

/* Render R as an integer.  */
extern HOST_WIDE_INT real_to_integer (const REAL_VALUE_TYPE *);
extern void real_to_integer2 (HOST_WIDE_INT *, HOST_WIDE_INT *,
			      const REAL_VALUE_TYPE *);

/* Initialize R from a decimal or hexadecimal string.  Return -1 if
   the value underflows, +1 if overflows, and 0 otherwise.  */
extern int real_from_string (REAL_VALUE_TYPE *, const char *);
/* Wrapper to allow different internal representation for decimal floats. */
extern void real_from_string3 (REAL_VALUE_TYPE *, const char *, enum machine_mode);

/* Initialize R from an integer pair HIGH/LOW.  */
extern void real_from_integer (REAL_VALUE_TYPE *, enum machine_mode,
			       unsigned HOST_WIDE_INT, HOST_WIDE_INT, int);

extern long real_to_target_fmt (long *, const REAL_VALUE_TYPE *,
				const struct real_format *);
extern long real_to_target (long *, const REAL_VALUE_TYPE *, enum machine_mode);

extern void real_from_target_fmt (REAL_VALUE_TYPE *, const long *,
				  const struct real_format *);
extern void real_from_target (REAL_VALUE_TYPE *, const long *,
			      enum machine_mode);

extern void real_inf (REAL_VALUE_TYPE *);

extern bool real_nan (REAL_VALUE_TYPE *, const char *, int, enum machine_mode);

extern void real_maxval (REAL_VALUE_TYPE *, int, enum machine_mode);

extern void real_2expN (REAL_VALUE_TYPE *, int, enum machine_mode);

extern unsigned int real_hash (const REAL_VALUE_TYPE *);


/* Target formats defined in real.c.  */
extern const struct real_format ieee_single_format;
extern const struct real_format mips_single_format;
extern const struct real_format motorola_single_format;
extern const struct real_format spu_single_format;
extern const struct real_format ieee_double_format;
extern const struct real_format mips_double_format;
extern const struct real_format motorola_double_format;
extern const struct real_format ieee_extended_motorola_format;
extern const struct real_format ieee_extended_intel_96_format;
extern const struct real_format ieee_extended_intel_96_round_53_format;
extern const struct real_format ieee_extended_intel_128_format;
extern const struct real_format ibm_extended_format;
extern const struct real_format mips_extended_format;
extern const struct real_format ieee_quad_format;
extern const struct real_format mips_quad_format;
extern const struct real_format vax_f_format;
extern const struct real_format vax_d_format;
extern const struct real_format vax_g_format;
extern const struct real_format real_internal_format;
extern const struct real_format decimal_single_format;
extern const struct real_format decimal_double_format;
extern const struct real_format decimal_quad_format;
extern const struct real_format ieee_half_format;
extern const struct real_format arm_half_format;


/* ====================================================================== */
/* Crap.  */

#define REAL_ARITHMETIC(value, code, d1, d2) \
  real_arithmetic (&(value), code, &(d1), &(d2))

#define REAL_VALUES_IDENTICAL(x, y)	real_identical (&(x), &(y))
#define REAL_VALUES_EQUAL(x, y)		real_compare (EQ_EXPR, &(x), &(y))
#define REAL_VALUES_LESS(x, y)		real_compare (LT_EXPR, &(x), &(y))

/* Determine whether a floating-point value X is infinite.  */
#define REAL_VALUE_ISINF(x)		real_isinf (&(x))

/* Determine whether a floating-point value X is a NaN.  */
#define REAL_VALUE_ISNAN(x)		real_isnan (&(x))

/* Determine whether a floating-point value X is negative.  */
#define REAL_VALUE_NEGATIVE(x)		real_isneg (&(x))

/* Determine whether a floating-point value X is minus zero.  */
#define REAL_VALUE_MINUS_ZERO(x)	real_isnegzero (&(x))

/* IN is a REAL_VALUE_TYPE.  OUT is an array of longs.  */
#define REAL_VALUE_TO_TARGET_LONG_DOUBLE(IN, OUT)			\
  real_to_target (OUT, &(IN),						\
		  mode_for_size (LONG_DOUBLE_TYPE_SIZE, MODE_FLOAT, 0))

#define REAL_VALUE_TO_TARGET_DOUBLE(IN, OUT) \
  real_to_target (OUT, &(IN), mode_for_size (64, MODE_FLOAT, 0))

/* IN is a REAL_VALUE_TYPE.  OUT is a long.  */
#define REAL_VALUE_TO_TARGET_SINGLE(IN, OUT) \
  ((OUT) = real_to_target (NULL, &(IN), mode_for_size (32, MODE_FLOAT, 0)))

#define REAL_VALUE_FROM_INT(r, lo, hi, mode) \
  real_from_integer (&(r), mode, lo, hi, 0)

#define REAL_VALUE_FROM_UNSIGNED_INT(r, lo, hi, mode) \
  real_from_integer (&(r), mode, lo, hi, 1)

/* Real values to IEEE 754 decimal floats.  */

/* IN is a REAL_VALUE_TYPE.  OUT is an array of longs.  */
#define REAL_VALUE_TO_TARGET_DECIMAL128(IN, OUT) \
  real_to_target (OUT, &(IN), mode_for_size (128, MODE_DECIMAL_FLOAT, 0))

#define REAL_VALUE_TO_TARGET_DECIMAL64(IN, OUT) \
  real_to_target (OUT, &(IN), mode_for_size (64, MODE_DECIMAL_FLOAT, 0))

/* IN is a REAL_VALUE_TYPE.  OUT is a long.  */
#define REAL_VALUE_TO_TARGET_DECIMAL32(IN, OUT) \
  ((OUT) = real_to_target (NULL, &(IN), mode_for_size (32, MODE_DECIMAL_FLOAT, 0)))

extern REAL_VALUE_TYPE real_value_truncate (enum machine_mode,
					    REAL_VALUE_TYPE);

#define REAL_VALUE_TO_INT(plow, phigh, r) \
  real_to_integer2 (plow, phigh, &(r))

extern REAL_VALUE_TYPE real_arithmetic2 (int, const REAL_VALUE_TYPE *,
					 const REAL_VALUE_TYPE *);

#define REAL_VALUE_NEGATE(X) \
  real_arithmetic2 (NEGATE_EXPR, &(X), NULL)

#define REAL_VALUE_ABS(X) \
  real_arithmetic2 (ABS_EXPR, &(X), NULL)

extern int significand_size (enum machine_mode);

extern REAL_VALUE_TYPE real_from_string2 (const char *, enum machine_mode);

#define REAL_VALUE_ATOF(s, m) \
  real_from_string2 (s, m)

#define CONST_DOUBLE_ATOF(s, m) \
  CONST_DOUBLE_FROM_REAL_VALUE (real_from_string2 (s, m), m)

#define REAL_VALUE_FIX(r) \
  real_to_integer (&(r))

/* ??? Not quite right.  */
#define REAL_VALUE_UNSIGNED_FIX(r) \
  real_to_integer (&(r))

/* ??? These were added for Paranoia support.  */

/* Return floor log2(R).  */
extern int real_exponent (const REAL_VALUE_TYPE *);

/* R = A * 2**EXP.  */
extern void real_ldexp (REAL_VALUE_TYPE *, const REAL_VALUE_TYPE *, int);

/* **** End of software floating point emulator interface macros **** */

/* Constant real values 0, 1, 2, -1 and 0.5.  */

extern REAL_VALUE_TYPE dconst0;
extern REAL_VALUE_TYPE dconst1;
extern REAL_VALUE_TYPE dconst2;
extern REAL_VALUE_TYPE dconstm1;
extern REAL_VALUE_TYPE dconsthalf;

#define dconst_e()  (*dconst_e_ptr ())
#define dconst_third()  (*dconst_third_ptr ())
#define dconst_sqrt2()  (*dconst_sqrt2_ptr ())

/* Function to return the real value special constant 'e'.  */
extern const REAL_VALUE_TYPE * dconst_e_ptr (void);

/* Returns the special REAL_VALUE_TYPE corresponding to 1/3.  */
extern const REAL_VALUE_TYPE * dconst_third_ptr (void);

/* Returns the special REAL_VALUE_TYPE corresponding to sqrt(2).  */
extern const REAL_VALUE_TYPE * dconst_sqrt2_ptr (void);

/* Function to return a real value (not a tree node)
   from a given integer constant.  */
REAL_VALUE_TYPE real_value_from_int_cst (const_tree, const_tree);

/* Given a CONST_DOUBLE in FROM, store into TO the value it represents.  */
#define REAL_VALUE_FROM_CONST_DOUBLE(to, from) \
  ((to) = *CONST_DOUBLE_REAL_VALUE (from))

/* Return a CONST_DOUBLE with value R and mode M.  */
#define CONST_DOUBLE_FROM_REAL_VALUE(r, m) \
  const_double_from_real_value (r, m)
extern rtx const_double_from_real_value (REAL_VALUE_TYPE, enum machine_mode);

/* Replace R by 1/R in the given machine mode, if the result is exact.  */
extern bool exact_real_inverse (enum machine_mode, REAL_VALUE_TYPE *);

/* Return true if arithmetic on values in IMODE that were promoted
   from values in TMODE is equivalent to direct arithmetic on values
   in TMODE.  */
bool real_can_shorten_arithmetic (enum machine_mode, enum machine_mode);

/* In tree.c: wrap up a REAL_VALUE_TYPE in a tree node.  */
extern tree build_real (tree, REAL_VALUE_TYPE);

/* Calculate R as the square root of X in the given machine mode.  */
extern bool real_sqrt (REAL_VALUE_TYPE *, enum machine_mode,
		       const REAL_VALUE_TYPE *);

/* Calculate R as X raised to the integer exponent N in mode MODE.  */
extern bool real_powi (REAL_VALUE_TYPE *, enum machine_mode,
		       const REAL_VALUE_TYPE *, HOST_WIDE_INT);

/* Standard round to integer value functions.  */
extern void real_trunc (REAL_VALUE_TYPE *, enum machine_mode,
			const REAL_VALUE_TYPE *);
extern void real_floor (REAL_VALUE_TYPE *, enum machine_mode,
			const REAL_VALUE_TYPE *);
extern void real_ceil (REAL_VALUE_TYPE *, enum machine_mode,
		       const REAL_VALUE_TYPE *);
extern void real_round (REAL_VALUE_TYPE *, enum machine_mode,
			const REAL_VALUE_TYPE *);

/* Set the sign of R to the sign of X.  */
extern void real_copysign (REAL_VALUE_TYPE *, const REAL_VALUE_TYPE *);

#ifndef GENERATOR_FILE
/* Convert between MPFR and REAL_VALUE_TYPE.  The caller is
   responsible for initializing and clearing the MPFR parameter.  */

extern void real_from_mpfr (REAL_VALUE_TYPE *, mpfr_srcptr, tree, mp_rnd_t);
extern void mpfr_from_real (mpfr_ptr, const REAL_VALUE_TYPE *, mp_rnd_t);
#endif

/* Check whether the real constant value given is an integer.  */
extern bool real_isinteger (const REAL_VALUE_TYPE *c, enum machine_mode mode);

/* Write into BUF the maximum representable finite floating-point
   number, (1 - b**-p) * b**emax for a given FP format FMT as a hex
   float string.  BUF must be large enough to contain the result.  */
extern void get_max_float (const struct real_format *, char *, size_t);
#endif /* ! GCC_REAL_H */
