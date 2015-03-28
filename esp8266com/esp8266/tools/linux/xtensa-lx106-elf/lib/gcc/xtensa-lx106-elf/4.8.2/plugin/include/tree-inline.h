/* Tree inlining hooks and declarations.
   Copyright (C) 2001-2013 Free Software Foundation, Inc.
   Contributed by Alexandre Oliva  <aoliva@redhat.com>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_TREE_INLINE_H
#define GCC_TREE_INLINE_H

struct cgraph_edge;

/* Indicate the desired behavior wrt call graph edges.  We can either
   duplicate the edge (inlining, cloning), move the edge (versioning,
   parallelization), or move the edges of the clones (saving).  */

enum copy_body_cge_which
{
  CB_CGE_DUPLICATE,
  CB_CGE_MOVE,
  CB_CGE_MOVE_CLONES
};

/* Data required for function body duplication.  */

typedef struct copy_body_data
{
  /* FUNCTION_DECL for function being inlined, or in general the
     source function providing the original trees.  */
  tree src_fn;

  /* FUNCTION_DECL for function being inlined into, or in general
     the destination function receiving the new trees.  */
  tree dst_fn;

  /* Callgraph node of the source function.  */
  struct cgraph_node *src_node;

  /* Callgraph node of the destination function.  */
  struct cgraph_node *dst_node;

  /* struct function for function being inlined.  Usually this is the same
     as DECL_STRUCT_FUNCTION (src_fn), but can be different if saved_cfg
     and saved_eh are in use.  */
  struct function *src_cfun;

  /* The VAR_DECL for the return value.  */
  tree retvar;

  /* The map from local declarations in the inlined function to
     equivalents in the function into which it is being inlined.  */
  struct pointer_map_t *decl_map;

  /* Create a new decl to replace DECL in the destination function.  */
  tree (*copy_decl) (tree, struct copy_body_data *);

  /* Current BLOCK.  */
  tree block;

  /* GIMPLE_CALL if va arg parameter packs should be expanded or NULL
     is not.  */
  gimple gimple_call;

  /* Exception landing pad the inlined call lies in.  */
  int eh_lp_nr;

  /* Maps region and landing pad structures from the function being copied
     to duplicates created within the function we inline into.  */
  struct pointer_map_t *eh_map;

  /* We use the same mechanism do all sorts of different things.  Rather
     than enumerating the different cases, we categorize the behavior
     in the various situations.  */

  /* What to do with call graph edges.  */
  enum copy_body_cge_which transform_call_graph_edges;

  /* True if a new CFG should be created.  False for inlining, true for
     everything else.  */
  bool transform_new_cfg;

  /* True if RETURN_EXPRs should be transformed to just the contained
     MODIFY_EXPR.  The branch semantics of the return will be handled
     by manipulating the CFG rather than a statement.  */
  bool transform_return_to_modify;

  /* True if this statement will need to be regimplified.  */
  bool regimplify;

  /* True if trees should not be unshared.  */
  bool do_not_unshare;

  /* > 0 if we are remapping a type currently.  */
  int remapping_type_depth;

  /* A function to be called when duplicating BLOCK nodes.  */
  void (*transform_lang_insert_block) (tree);

  /* Statements that might be possibly folded.  */
  struct pointer_set_t *statements_to_fold;

  /* Entry basic block to currently copied body.  */
  basic_block entry_bb;

  /* Debug statements that need processing.  */
  vec<gimple> debug_stmts;

  /* A map from local declarations in the inlined function to
     equivalents in the function into which it is being inlined, where
     the originals have been mapped to a value rather than to a
     variable.  */
  struct pointer_map_t *debug_map;
} copy_body_data;

/* Weights of constructions for estimate_num_insns.  */

typedef struct eni_weights_d
{
  /* Cost per call.  */
  unsigned call_cost;

  /* Cost per indirect call.  */
  unsigned indirect_call_cost;

  /* Cost per call to a target specific builtin */
  unsigned target_builtin_call_cost;

  /* Cost of "expensive" div and mod operations.  */
  unsigned div_mod_cost;

  /* Cost for omp construct.  */
  unsigned omp_cost;

  /* Cost for tm transaction.  */
  unsigned tm_cost;

  /* Cost of return.  */
  unsigned return_cost;

  /* True when time of statemnt should be estimated.  Thus i.e
     cost of switch statement is logarithmic rather than linear in number
     of cases.  */
  bool time_based;
} eni_weights;

/* Weights that estimate_num_insns uses for heuristics in inlining.  */

extern eni_weights eni_inlining_weights;

/* Weights that estimate_num_insns uses to estimate the size of the
   produced code.  */

extern eni_weights eni_size_weights;

/* Weights that estimate_num_insns uses to estimate the time necessary
   to execute the produced code.  */

extern eni_weights eni_time_weights;

/* Function prototypes.  */

extern tree copy_tree_body_r (tree *, int *, void *);
extern void insert_decl_map (copy_body_data *, tree, tree);

unsigned int optimize_inline_calls (tree);
tree maybe_inline_call_in_expr (tree);
bool tree_inlinable_function_p (tree);
tree copy_tree_r (tree *, int *, void *);
tree copy_decl_no_change (tree decl, copy_body_data *id);
int estimate_move_cost (tree type);
int estimate_num_insns (gimple, eni_weights *);
int estimate_num_insns_fn (tree, eni_weights *);
int count_insns_seq (gimple_seq, eni_weights *);
bool tree_versionable_function_p (tree);

extern tree remap_decl (tree decl, copy_body_data *id);
extern tree remap_type (tree type, copy_body_data *id);
extern gimple_seq copy_gimple_seq_and_replace_locals (gimple_seq seq);

extern HOST_WIDE_INT estimated_stack_frame_size (struct cgraph_node *);

#endif /* GCC_TREE_INLINE_H */
