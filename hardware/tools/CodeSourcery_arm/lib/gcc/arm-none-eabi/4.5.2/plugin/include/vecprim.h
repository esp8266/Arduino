/* VEC types for primitive types
   Copyright (C) 2006, 2007, 2008 Free Software Foundation, Inc.

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

#ifndef GCC_VECPRIM_H
#define GCC_VECPRIM_H

DEF_VEC_I(char);
DEF_VEC_ALLOC_I(char,heap);

typedef unsigned char uchar;
DEF_VEC_I(uchar);
DEF_VEC_ALLOC_I(uchar,heap);
DEF_VEC_ALLOC_I(uchar,gc);

DEF_VEC_I(int);
DEF_VEC_ALLOC_I(int,heap);

DEF_VEC_I(unsigned);
DEF_VEC_ALLOC_I(unsigned,heap);

#endif /* GCC_VECPRIM_H */
