/* Tree SCC value numbering
   Copyright (C) 2007, 2008, 2009 Free Software Foundation, Inc.
   Contributed by Daniel Berlin <dberlin@dberlin.org>

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef TREE_SSA_SCCVN_H
#define TREE_SSA_SCCVN_H

/* In tree-ssa-sccvn.c  */
bool expressions_equal_p (tree, tree);


/* TOP of the VN lattice.  */
extern tree VN_TOP;

/* N-ary operations in the hashtable consist of length operands, an
   opcode, and a type.  Result is the value number of the operation,
   and hashcode is stored to avoid having to calculate it
   repeatedly.  */

typedef struct vn_nary_op_s
{
  /* Unique identify that all expressions with the same value have. */
  unsigned int value_id;
  ENUM_BITFIELD(tree_code) opcode : 16;
  unsigned length : 16;
  hashval_t hashcode;
  tree result;
  tree type;
  tree op[4];
} *vn_nary_op_t;
typedef const struct vn_nary_op_s *const_vn_nary_op_t;

/* Phi nodes in the hashtable consist of their non-VN_TOP phi
   arguments, and the basic block the phi is in. Result is the value
   number of the operation, and hashcode is stored to avoid having to
   calculate it repeatedly.  Phi nodes not in the same block are never
   considered equivalent.  */

typedef struct vn_phi_s
{
  /* Unique identifier that all expressions with the same value have. */
  unsigned int value_id;
  hashval_t hashcode;
  VEC (tree, heap) *phiargs;
  basic_block block;
  tree result;
} *vn_phi_t;
typedef const struct vn_phi_s *const_vn_phi_t;

/* Reference operands only exist in reference operations structures.
   They consist of an opcode, type, and some number of operands.  For
   a given opcode, some, all, or none of the operands may be used.
   The operands are there to store the information that makes up the
   portion of the addressing calculation that opcode performs.  */

typedef struct vn_reference_op_struct
{
  enum tree_code opcode;
  tree type;
  tree op0;
  tree op1;
  tree op2;
} vn_reference_op_s;
typedef vn_reference_op_s *vn_reference_op_t;
typedef const vn_reference_op_s *const_vn_reference_op_t;

DEF_VEC_O(vn_reference_op_s);
DEF_VEC_ALLOC_O(vn_reference_op_s, heap);

/* A reference operation in the hashtable is representation as
   the vuse, representing the memory state at the time of
   the operation, and a collection of operands that make up the
   addressing calculation.  If two vn_reference_t's have the same set
   of operands, they access the same memory location. We also store
   the resulting value number, and the hashcode.  */

typedef struct vn_reference_s
{
  /* Unique identifier that all expressions with the same value have. */
  unsigned int value_id;
  hashval_t hashcode;
  tree vuse;
  alias_set_type set;
  tree type;
  VEC (vn_reference_op_s, heap) *operands;
  tree result;
} *vn_reference_t;
typedef const struct vn_reference_s *const_vn_reference_t;

typedef struct vn_constant_s
{
  unsigned int value_id;
  hashval_t hashcode;
  tree constant;
} *vn_constant_t;

/* Hash the constant CONSTANT with distinguishing type incompatible
   constants in the types_compatible_p sense.  */

static inline hashval_t
vn_hash_constant_with_type (tree constant)
{
  tree type = TREE_TYPE (constant);
  return (iterative_hash_expr (constant, 0)
	  + INTEGRAL_TYPE_P (type)
	  + (INTEGRAL_TYPE_P (type)
	     ? TYPE_PRECISION (type) + TYPE_UNSIGNED (type) : 0));
}

/* Compare the constants C1 and C2 with distinguishing type incompatible
   constants in the types_compatible_p sense.  */

static inline bool
vn_constant_eq_with_type (tree c1, tree c2)
{
  return (expressions_equal_p (c1, c2)
	  && types_compatible_p (TREE_TYPE (c1), TREE_TYPE (c2)));
}

typedef struct vn_ssa_aux
{
  /* Value number. This may be an SSA name or a constant.  */
  tree valnum;
  /* Representative expression, if not a direct constant. */
  tree expr;

  /* Unique identifier that all expressions with the same value have. */
  unsigned int value_id;

  /* SCC information.  */
  unsigned int dfsnum;
  unsigned int low;
  unsigned visited : 1;
  unsigned on_sccstack : 1;

  /* Whether the representative expression contains constants.  */
  unsigned has_constants : 1;
  /* Whether the SSA_NAME has been value numbered already.  This is
     only saying whether visit_use has been called on it at least
     once.  It cannot be used to avoid visitation for SSA_NAME's
     involved in non-singleton SCC's.  */
  unsigned use_processed : 1;

  /* Whether the SSA_NAME has no defining statement and thus an
     insertion of such with EXPR as definition is required before
     a use can be created of it.  */
  unsigned needs_insertion : 1;
} *vn_ssa_aux_t;

/* Return the value numbering info for an SSA_NAME.  */
extern vn_ssa_aux_t VN_INFO (tree);
extern vn_ssa_aux_t VN_INFO_GET (tree);
tree vn_get_expr_for (tree);
bool run_scc_vn (bool);
void free_scc_vn (void);
tree vn_nary_op_lookup (tree, vn_nary_op_t *);
tree vn_nary_op_lookup_stmt (gimple, vn_nary_op_t *);
tree vn_nary_op_lookup_pieces (unsigned int, enum tree_code,
			       tree, tree, tree, tree, tree,
			       vn_nary_op_t *);
vn_nary_op_t vn_nary_op_insert (tree, tree);
vn_nary_op_t vn_nary_op_insert_stmt (gimple, tree);
vn_nary_op_t vn_nary_op_insert_pieces (unsigned int, enum tree_code,
				       tree, tree, tree, tree,
				       tree, tree, unsigned int);
void vn_reference_fold_indirect (VEC (vn_reference_op_s, heap) **,
				 unsigned int *);
void copy_reference_ops_from_ref (tree, VEC(vn_reference_op_s, heap) **);
void copy_reference_ops_from_call (gimple, VEC(vn_reference_op_s, heap) **);
bool ao_ref_init_from_vn_reference (ao_ref *, alias_set_type, tree,
				    VEC (vn_reference_op_s, heap) *);
tree vn_reference_lookup_pieces (tree, alias_set_type, tree,
				 VEC (vn_reference_op_s, heap) *,
				 vn_reference_t *, bool);
tree vn_reference_lookup (tree, tree, bool, vn_reference_t *);
vn_reference_t vn_reference_insert (tree, tree, tree);
vn_reference_t vn_reference_insert_pieces (tree, alias_set_type, tree,
					   VEC (vn_reference_op_s, heap) *,
					   tree, unsigned int);

hashval_t vn_nary_op_compute_hash (const vn_nary_op_t);
int vn_nary_op_eq (const void *, const void *);
bool vn_nary_may_trap (vn_nary_op_t);
hashval_t vn_reference_compute_hash (const vn_reference_t);
int vn_reference_eq (const void *, const void *);
unsigned int get_max_value_id (void);
unsigned int get_next_value_id (void);
unsigned int get_constant_value_id (tree);
unsigned int get_or_alloc_constant_value_id (tree);
bool value_id_constant_p (unsigned int);
#endif /* TREE_SSA_SCCVN_H  */
