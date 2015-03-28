/* Various declarations for language-independent pretty-print
   subroutines that are only for use in the compilers proper and not
   the driver or other programs.
   Copyright (C) 2002-2013 Free Software Foundation, Inc.

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

#ifndef GCC_TREE_PRETTY_PRINT_H
#define GCC_TREE_PRETTY_PRINT_H

#include "pretty-print.h"

#define pp_tree_identifier(PP, T)                      \
  pp_base_tree_identifier (pp_base (PP), T)

#define pp_unsupported_tree(PP, T)                         \
  pp_verbatim (pp_base (PP), "#%qs not supported by %s#", \
               tree_code_name[(int) TREE_CODE (T)], __FUNCTION__)

#define pp_ti_abstract_origin(TI) ((tree *) (TI)->x_data)

extern void pp_base_tree_identifier (pretty_printer *, tree);

/* In tree-pretty-print.c  */
extern void print_declaration (pretty_printer *, tree, int, int);
extern int dump_generic_node (pretty_printer *, tree, int, int, bool);
extern void print_generic_stmt (FILE *, tree, int);
extern void print_generic_stmt_indented (FILE *, tree, int, int);
extern void print_generic_expr (FILE *, tree, int);
extern void print_generic_decl (FILE *, tree, int);
extern void dump_omp_clauses (pretty_printer *, tree, int, int);
extern void print_call_name (pretty_printer *, tree, int);
extern void debug_generic_expr (tree);
extern void debug_generic_stmt (tree);
extern void debug_tree_chain (tree);
extern void percent_K_format (text_info *);
extern void dump_function_header (FILE *, tree, int);
/* In c-pretty-print.c  */
extern void debug_c_tree (tree);

#endif /* ! GCC_TREE_PRETTY_PRINT_H */
