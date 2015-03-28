/* Various declarations for pretty formatting of GIMPLE statements and
   expressions.
   Copyright (C) 2000-2013 Free Software Foundation, Inc.

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

#ifndef GCC_GIMPLE_PRETTY_PRINT_H
#define GCC_GIMPLE_PRETTY_PRINT_H

#include "pretty-print.h"
#include "tree-pretty-print.h"

/* In gimple-pretty-print.c  */
extern void debug_gimple_stmt (gimple);
extern void debug_gimple_seq (gimple_seq);
extern void print_gimple_seq (FILE *, gimple_seq, int, int);
extern void print_gimple_stmt (FILE *, gimple, int, int);
extern void print_gimple_expr (FILE *, gimple, int, int);
extern void pp_gimple_stmt_1 (pretty_printer *, gimple, int, int);
extern void gimple_dump_bb_for_graph (pretty_printer *, basic_block);

#endif /* ! GCC_GIMPLE_PRETTY_PRINT_H */
