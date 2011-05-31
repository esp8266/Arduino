/* GCC core type declarations.
   Copyright (C) 2002, 2004, 2007, 2008, 2009 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/* Provide forward declarations of core types which are referred to by
   most of the compiler.  This allows header files to use these types
   (e.g. in function prototypes) without concern for whether the full
   definitions are visible.  Some other declarations that need to be
   universally visible are here, too.

   In the context of tconfig.h, most of these have special definitions
   which prevent them from being used except in further type
   declarations.  This is a kludge; the right thing is to avoid
   including the "tm.h" header set in the context of tconfig.h, but
   we're not there yet.  */

#ifndef GCC_CORETYPES_H
#define GCC_CORETYPES_H

#ifndef GTY
#define GTY(x)  /* nothing - marker for gengtype */
#endif

#ifndef USED_FOR_TARGET

struct bitmap_head_def;
typedef struct bitmap_head_def *bitmap;
typedef const struct bitmap_head_def *const_bitmap;
struct rtx_def;
typedef struct rtx_def *rtx;
typedef const struct rtx_def *const_rtx;
struct rtvec_def;
typedef struct rtvec_def *rtvec;
typedef const struct rtvec_def *const_rtvec;
union tree_node;
typedef union tree_node *tree;
union gimple_statement_d;
typedef union gimple_statement_d *gimple;
typedef const union tree_node *const_tree;
typedef const union gimple_statement_d *const_gimple;
union section;
typedef union section section;
struct cl_target_option;
struct cl_optimization;
struct gimple_seq_d;
typedef struct gimple_seq_d *gimple_seq;
typedef const struct gimple_seq_d *const_gimple_seq;
struct gimple_seq_node_d;
typedef struct gimple_seq_node_d *gimple_seq_node;
typedef const struct gimple_seq_node_d *const_gimple_seq_node;

/* Address space number for named address space support.  */
typedef unsigned char addr_space_t;

/* The value of addr_space_t that represents the generic address space.  */
#define ADDR_SPACE_GENERIC 0
#define ADDR_SPACE_GENERIC_P(AS) ((AS) == ADDR_SPACE_GENERIC)

/* The major intermediate representations of GCC.  */
enum ir_type {
  IR_GIMPLE,
  IR_RTL_CFGRTL,
  IR_RTL_CFGLAYOUT
};

/* Provide forward struct declaration so that we don't have to include
   all of cpplib.h whenever a random prototype includes a pointer.
   Note that the cpp_reader and cpp_token typedefs remain part of
   cpplib.h.  */

struct cpp_reader;
struct cpp_token;

/* The thread-local storage model associated with a given VAR_DECL
   or SYMBOL_REF.  This isn't used much, but both trees and RTL refer
   to it, so it's here.  */
enum tls_model {
  TLS_MODEL_NONE,
  TLS_MODEL_EMULATED,
  TLS_MODEL_REAL,
  TLS_MODEL_GLOBAL_DYNAMIC = TLS_MODEL_REAL,
  TLS_MODEL_LOCAL_DYNAMIC,
  TLS_MODEL_INITIAL_EXEC,
  TLS_MODEL_LOCAL_EXEC
};

struct edge_def;
typedef struct edge_def *edge;
typedef const struct edge_def *const_edge;
struct basic_block_def;
typedef struct basic_block_def *basic_block;
typedef const struct basic_block_def *const_basic_block;
#else

struct _dont_use_rtx_here_;
struct _dont_use_rtvec_here_;
union _dont_use_tree_here_;
#define rtx struct _dont_use_rtx_here_ *
#define const_rtx struct _dont_use_rtx_here_ *
#define rtvec struct _dont_use_rtvec_here *
#define const_rtvec struct _dont_use_rtvec_here *
#define tree union _dont_use_tree_here_ *
#define const_tree union _dont_use_tree_here_ *

#endif

#endif /* coretypes.h */

