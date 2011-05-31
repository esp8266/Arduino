/* SSA operand management for trees.
   Copyright (C) 2003, 2005, 2006, 2007, 2008 Free Software Foundation, Inc.

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

#ifndef GCC_TREE_SSA_OPERANDS_H
#define GCC_TREE_SSA_OPERANDS_H

/* Interface to SSA operands.  */


/* This represents a pointer to a DEF operand.  */
typedef tree *def_operand_p;

/* This represents a pointer to a USE operand.  */
typedef ssa_use_operand_t *use_operand_p;

/* NULL operand types.  */
#define NULL_USE_OPERAND_P 		((use_operand_p)NULL)
#define NULL_DEF_OPERAND_P 		((def_operand_p)NULL)

/* This represents the DEF operands of a stmt.  */
struct def_optype_d
{
  struct def_optype_d *next;
  tree *def_ptr;
};
typedef struct def_optype_d *def_optype_p;

/* This represents the USE operands of a stmt.  */
struct use_optype_d
{
  struct use_optype_d *next;
  struct ssa_use_operand_d use_ptr;
};
typedef struct use_optype_d *use_optype_p;

/* This structure represents a variable sized buffer which is allocated by the
   operand memory manager.  Operands are suballocated out of this block.  The
   MEM array varies in size.  */

struct GTY((chain_next("%h.next"))) ssa_operand_memory_d {
  struct ssa_operand_memory_d *next;
  char mem[1];
};

/* Per-function operand caches.  */
struct GTY(()) ssa_operands {
   struct ssa_operand_memory_d *operand_memory;
   unsigned operand_memory_index;
   /* Current size of the operand memory buffer.  */
   unsigned int ssa_operand_mem_size;

   bool ops_active;

   struct def_optype_d * GTY ((skip (""))) free_defs;
   struct use_optype_d * GTY ((skip (""))) free_uses;
};

#define USE_FROM_PTR(PTR)	get_use_from_ptr (PTR)
#define DEF_FROM_PTR(PTR)	get_def_from_ptr (PTR)
#define SET_USE(USE, V)		set_ssa_use_from_ptr (USE, V)
#define SET_DEF(DEF, V)		((*(DEF)) = (V))

#define USE_STMT(USE)		(USE)->loc.stmt

#define USE_OP_PTR(OP)		(&((OP)->use_ptr))
#define USE_OP(OP)		(USE_FROM_PTR (USE_OP_PTR (OP)))

#define DEF_OP_PTR(OP)		((OP)->def_ptr)
#define DEF_OP(OP)		(DEF_FROM_PTR (DEF_OP_PTR (OP)))

#define PHI_RESULT_PTR(PHI)	gimple_phi_result_ptr (PHI)
#define PHI_RESULT(PHI)		DEF_FROM_PTR (PHI_RESULT_PTR (PHI))
#define SET_PHI_RESULT(PHI, V)	SET_DEF (PHI_RESULT_PTR (PHI), (V))

#define PHI_ARG_DEF_PTR(PHI, I)	gimple_phi_arg_imm_use_ptr ((PHI), (I))
#define PHI_ARG_DEF(PHI, I)	USE_FROM_PTR (PHI_ARG_DEF_PTR ((PHI), (I)))
#define SET_PHI_ARG_DEF(PHI, I, V)					\
				SET_USE (PHI_ARG_DEF_PTR ((PHI), (I)), (V))
#define PHI_ARG_DEF_FROM_EDGE(PHI, E)					\
				PHI_ARG_DEF ((PHI), (E)->dest_idx)
#define PHI_ARG_DEF_PTR_FROM_EDGE(PHI, E)				\
				PHI_ARG_DEF_PTR ((PHI), (E)->dest_idx)
#define PHI_ARG_INDEX_FROM_USE(USE)   phi_arg_index_from_use (USE)


extern void init_ssa_operands (void);
extern void fini_ssa_operands (void);
extern void update_stmt_operands (gimple);
extern void free_stmt_operands (gimple);
extern bool verify_imm_links (FILE *f, tree var);

extern void dump_immediate_uses (FILE *file);
extern void dump_immediate_uses_for (FILE *file, tree var);
extern void debug_immediate_uses (void);
extern void debug_immediate_uses_for (tree var);
extern void dump_decl_set (FILE *, bitmap);
extern void debug_decl_set (bitmap);

extern bool ssa_operands_active (void);

extern void unlink_stmt_vdef (gimple);

enum ssa_op_iter_type {
  ssa_op_iter_none = 0,
  ssa_op_iter_tree,
  ssa_op_iter_use,
  ssa_op_iter_def
};

/* This structure is used in the operand iterator loops.  It contains the
   items required to determine which operand is retrieved next.  During
   optimization, this structure is scalarized, and any unused fields are
   optimized away, resulting in little overhead.  */

typedef struct ssa_operand_iterator_d
{
  bool done;
  enum ssa_op_iter_type iter_type;
  def_optype_p defs;
  use_optype_p uses;
  int phi_i;
  int num_phi;
  gimple phi_stmt;
} ssa_op_iter;

/* These flags are used to determine which operands are returned during
   execution of the loop.  */
#define SSA_OP_USE		0x01	/* Real USE operands.  */
#define SSA_OP_DEF		0x02	/* Real DEF operands.  */
#define SSA_OP_VUSE		0x04	/* VUSE operands.  */
#define SSA_OP_VDEF		0x08	/* VDEF operands.  */

/* These are commonly grouped operand flags.  */
#define SSA_OP_VIRTUAL_USES	(SSA_OP_VUSE)
#define SSA_OP_VIRTUAL_DEFS	(SSA_OP_VDEF)
#define SSA_OP_ALL_VIRTUALS     (SSA_OP_VIRTUAL_USES | SSA_OP_VIRTUAL_DEFS)
#define SSA_OP_ALL_USES		(SSA_OP_VIRTUAL_USES | SSA_OP_USE)
#define SSA_OP_ALL_DEFS		(SSA_OP_VIRTUAL_DEFS | SSA_OP_DEF)
#define SSA_OP_ALL_OPERANDS	(SSA_OP_ALL_USES | SSA_OP_ALL_DEFS)

/* This macro executes a loop over the operands of STMT specified in FLAG,
   returning each operand as a 'tree' in the variable TREEVAR.  ITER is an
   ssa_op_iter structure used to control the loop.  */
#define FOR_EACH_SSA_TREE_OPERAND(TREEVAR, STMT, ITER, FLAGS)	\
  for (TREEVAR = op_iter_init_tree (&(ITER), STMT, FLAGS);	\
       !op_iter_done (&(ITER));					\
       TREEVAR = op_iter_next_tree (&(ITER)))

/* This macro executes a loop over the operands of STMT specified in FLAG,
   returning each operand as a 'use_operand_p' in the variable USEVAR.
   ITER is an ssa_op_iter structure used to control the loop.  */
#define FOR_EACH_SSA_USE_OPERAND(USEVAR, STMT, ITER, FLAGS)	\
  for (USEVAR = op_iter_init_use (&(ITER), STMT, FLAGS);	\
       !op_iter_done (&(ITER));					\
       USEVAR = op_iter_next_use (&(ITER)))

/* This macro executes a loop over the operands of STMT specified in FLAG,
   returning each operand as a 'def_operand_p' in the variable DEFVAR.
   ITER is an ssa_op_iter structure used to control the loop.  */
#define FOR_EACH_SSA_DEF_OPERAND(DEFVAR, STMT, ITER, FLAGS)	\
  for (DEFVAR = op_iter_init_def (&(ITER), STMT, FLAGS);	\
       !op_iter_done (&(ITER));					\
       DEFVAR = op_iter_next_def (&(ITER)))

/* This macro will execute a loop over all the arguments of a PHI which
   match FLAGS.   A use_operand_p is always returned via USEVAR.  FLAGS
   can be either SSA_OP_USE or SSA_OP_VIRTUAL_USES or SSA_OP_ALL_USES.  */
#define FOR_EACH_PHI_ARG(USEVAR, STMT, ITER, FLAGS)		\
  for ((USEVAR) = op_iter_init_phiuse (&(ITER), STMT, FLAGS);	\
       !op_iter_done (&(ITER));					\
       (USEVAR) = op_iter_next_use (&(ITER)))


/* This macro will execute a loop over a stmt, regardless of whether it is
   a real stmt or a PHI node, looking at the USE nodes matching FLAGS.  */
#define FOR_EACH_PHI_OR_STMT_USE(USEVAR, STMT, ITER, FLAGS)	\
  for ((USEVAR) = (gimple_code (STMT) == GIMPLE_PHI 		\
		   ? op_iter_init_phiuse (&(ITER), STMT, FLAGS)	\
		   : op_iter_init_use (&(ITER), STMT, FLAGS));	\
       !op_iter_done (&(ITER));					\
       (USEVAR) = op_iter_next_use (&(ITER)))

/* This macro will execute a loop over a stmt, regardless of whether it is
   a real stmt or a PHI node, looking at the DEF nodes matching FLAGS.  */
#define FOR_EACH_PHI_OR_STMT_DEF(DEFVAR, STMT, ITER, FLAGS)	\
  for ((DEFVAR) = (gimple_code (STMT) == GIMPLE_PHI 		\
		   ? op_iter_init_phidef (&(ITER), STMT, FLAGS)	\
		   : op_iter_init_def (&(ITER), STMT, FLAGS));	\
       !op_iter_done (&(ITER));					\
       (DEFVAR) = op_iter_next_def (&(ITER)))

/* This macro returns an operand in STMT as a tree if it is the ONLY
   operand matching FLAGS.  If there are 0 or more than 1 operand matching
   FLAGS, then NULL_TREE is returned.  */
#define SINGLE_SSA_TREE_OPERAND(STMT, FLAGS)			\
  single_ssa_tree_operand (STMT, FLAGS)

/* This macro returns an operand in STMT as a use_operand_p if it is the ONLY
   operand matching FLAGS.  If there are 0 or more than 1 operand matching
   FLAGS, then NULL_USE_OPERAND_P is returned.  */
#define SINGLE_SSA_USE_OPERAND(STMT, FLAGS)			\
  single_ssa_use_operand (STMT, FLAGS)

/* This macro returns an operand in STMT as a def_operand_p if it is the ONLY
   operand matching FLAGS.  If there are 0 or more than 1 operand matching
   FLAGS, then NULL_DEF_OPERAND_P is returned.  */
#define SINGLE_SSA_DEF_OPERAND(STMT, FLAGS)			\
  single_ssa_def_operand (STMT, FLAGS)

/* This macro returns TRUE if there are no operands matching FLAGS in STMT.  */
#define ZERO_SSA_OPERANDS(STMT, FLAGS) 	zero_ssa_operands (STMT, FLAGS)

/* This macro counts the number of operands in STMT matching FLAGS.  */
#define NUM_SSA_OPERANDS(STMT, FLAGS)	num_ssa_operands (STMT, FLAGS)

#endif  /* GCC_TREE_SSA_OPERANDS_H  */
