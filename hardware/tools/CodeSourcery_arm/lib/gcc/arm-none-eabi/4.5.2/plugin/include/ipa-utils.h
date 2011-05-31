/* Utilities for ipa analysis.
   Copyright (C) 2004, 2005, 2007 Free Software Foundation, Inc.
   Contributed by Kenneth Zadeck <zadeck@naturalbridge.com>

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

#ifndef GCC_IPA_UTILS_H
#define GCC_IPA_UTILS_H
#include "tree.h"
#include "cgraph.h"

struct ipa_dfs_info {
  int dfn_number;
  int low_link;
  bool new_node;
  bool on_stack;
  struct cgraph_node* next_cycle;
  PTR aux;
};



/* In ipa-utils.c  */
void ipa_utils_print_order (FILE*, const char *, struct cgraph_node**, int);
int ipa_utils_reduced_inorder (struct cgraph_node **, bool, bool,
			       bool (*ignore_edge) (struct cgraph_edge *));
tree get_base_var (tree);


#endif  /* GCC_IPA_UTILS_H  */


