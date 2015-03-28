/* Internal functions.
   Copyright (C) 2011-2013 Free Software Foundation, Inc.

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

#ifndef GCC_INTERNAL_FN_H
#define GCC_INTERNAL_FN_H

enum internal_fn {
#define DEF_INTERNAL_FN(CODE, FLAGS) IFN_##CODE,
#include "internal-fn.def"
#undef DEF_INTERNAL_FN
  IFN_LAST
};

/* Return the name of internal function FN.  The name is only meaningful
   for dumps; it has no linkage.  */

extern const char *const internal_fn_name_array[];

static inline const char *
internal_fn_name (enum internal_fn fn)
{
  return internal_fn_name_array[(int) fn];
}

/* Return the ECF_* flags for function FN.  */

extern const int internal_fn_flags_array[];

static inline int
internal_fn_flags (enum internal_fn fn)
{
  return internal_fn_flags_array[(int) fn];
}

extern void expand_internal_call (gimple);

#endif
