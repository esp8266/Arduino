/* Inline functions for tree-flow.h
   Copyright (C) 2001, 2003, 2005, 2006, 2007, 2008, 2010
   Free Software Foundation, Inc.
   Contributed by Diego Novillo <dnovillo@redhat.com>

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

#ifndef _TREE_FLOW_INLINE_H
#define _TREE_FLOW_INLINE_H 1

/* Inline functions for manipulating various data structures defined in
   tree-flow.h.  See tree-flow.h for documentation.  */

/* Return true when gimple SSA form was built.
   gimple_in_ssa_p is queried by gimplifier in various early stages before SSA
   infrastructure is initialized.  Check for presence of the datastructures
   at first place.  */
static inline bool
gimple_in_ssa_p (const struct function *fun)
{
  return fun && fun->gimple_df && fun->gimple_df->in_ssa_p;
}

/* Array of all variables referenced in the function.  */
static inline htab_t
gimple_referenced_vars (const struct function *fun)
{
  if (!fun->gimple_df)
    return NULL;
  return fun->gimple_df->referenced_vars;
}

/* Artificial variable used for the virtual operand FUD chain.  */
static inline tree
gimple_vop (const struct function *fun)
{
  gcc_assert (fun && fun->gimple_df);
  return fun->gimple_df->vop;
}

/* Initialize the hashtable iterator HTI to point to hashtable TABLE */

static inline void *
first_htab_element (htab_iterator *hti, htab_t table)
{
  hti->htab = table;
  hti->slot = table->entries;
  hti->limit = hti->slot + htab_size (table);
  do
    {
      PTR x = *(hti->slot);
      if (x != HTAB_EMPTY_ENTRY && x != HTAB_DELETED_ENTRY)
	break;
    } while (++(hti->slot) < hti->limit);

  if (hti->slot < hti->limit)
    return *(hti->slot);
  return NULL;
}

/* Return current non-empty/deleted slot of the hashtable pointed to by HTI,
   or NULL if we have  reached the end.  */

static inline bool
end_htab_p (const htab_iterator *hti)
{
  if (hti->slot >= hti->limit)
    return true;
  return false;
}

/* Advance the hashtable iterator pointed to by HTI to the next element of the
   hashtable.  */

static inline void *
next_htab_element (htab_iterator *hti)
{
  while (++(hti->slot) < hti->limit)
    {
      PTR x = *(hti->slot);
      if (x != HTAB_EMPTY_ENTRY && x != HTAB_DELETED_ENTRY)
	return x;
    };
  return NULL;
}

/* Initialize ITER to point to the first referenced variable in the
   referenced_vars hashtable, and return that variable.  */

static inline tree
first_referenced_var (referenced_var_iterator *iter)
{
  return (tree) first_htab_element (&iter->hti,
				    gimple_referenced_vars (cfun));
}

/* Return true if we have hit the end of the referenced variables ITER is
   iterating through.  */

static inline bool
end_referenced_vars_p (const referenced_var_iterator *iter)
{
  return end_htab_p (&iter->hti);
}

/* Make ITER point to the next referenced_var in the referenced_var hashtable,
   and return that variable.  */

static inline tree
next_referenced_var (referenced_var_iterator *iter)
{
  return (tree) next_htab_element (&iter->hti);
}

/* Return the variable annotation for T, which must be a _DECL node.
   Return NULL if the variable annotation doesn't already exist.  */
static inline var_ann_t
var_ann (const_tree t)
{
  const var_ann_t *p = DECL_VAR_ANN_PTR (t);
  return p ? *p : NULL;
}

/* Return the variable annotation for T, which must be a _DECL node.
   Create the variable annotation if it doesn't exist.  */
static inline var_ann_t
get_var_ann (tree var)
{
  var_ann_t *p = DECL_VAR_ANN_PTR (var);
  gcc_assert (p);
  return *p ? *p : create_var_ann (var);
}

/* Get the number of the next statement uid to be allocated.  */
static inline unsigned int
gimple_stmt_max_uid (struct function *fn)
{
  return fn->last_stmt_uid;
}

/* Set the number of the next statement uid to be allocated.  */
static inline void
set_gimple_stmt_max_uid (struct function *fn, unsigned int maxid)
{
  fn->last_stmt_uid = maxid;
}

/* Set the number of the next statement uid to be allocated.  */
static inline unsigned int
inc_gimple_stmt_max_uid (struct function *fn)
{
  return fn->last_stmt_uid++;
}

/* Return the line number for EXPR, or return -1 if we have no line
   number information for it.  */
static inline int
get_lineno (const_gimple stmt)
{
  location_t loc;

  if (!stmt)
    return -1;

  loc = gimple_location (stmt);
  if (loc == UNKNOWN_LOCATION)
    return -1;

  return LOCATION_LINE (loc);
}

/* Delink an immediate_uses node from its chain.  */
static inline void
delink_imm_use (ssa_use_operand_t *linknode)
{
  /* Return if this node is not in a list.  */
  if (linknode->prev == NULL)
    return;

  linknode->prev->next = linknode->next;
  linknode->next->prev = linknode->prev;
  linknode->prev = NULL;
  linknode->next = NULL;
}

/* Link ssa_imm_use node LINKNODE into the chain for LIST.  */
static inline void
link_imm_use_to_list (ssa_use_operand_t *linknode, ssa_use_operand_t *list)
{
  /* Link the new node at the head of the list.  If we are in the process of
     traversing the list, we won't visit any new nodes added to it.  */
  linknode->prev = list;
  linknode->next = list->next;
  list->next->prev = linknode;
  list->next = linknode;
}

/* Link ssa_imm_use node LINKNODE into the chain for DEF.  */
static inline void
link_imm_use (ssa_use_operand_t *linknode, tree def)
{
  ssa_use_operand_t *root;

  if (!def || TREE_CODE (def) != SSA_NAME)
    linknode->prev = NULL;
  else
    {
      root = &(SSA_NAME_IMM_USE_NODE (def));
#ifdef ENABLE_CHECKING
      if (linknode->use)
        gcc_assert (*(linknode->use) == def);
#endif
      link_imm_use_to_list (linknode, root);
    }
}

/* Set the value of a use pointed to by USE to VAL.  */
static inline void
set_ssa_use_from_ptr (use_operand_p use, tree val)
{
  delink_imm_use (use);
  *(use->use) = val;
  link_imm_use (use, val);
}

/* Link ssa_imm_use node LINKNODE into the chain for DEF, with use occurring
   in STMT.  */
static inline void
link_imm_use_stmt (ssa_use_operand_t *linknode, tree def, gimple stmt)
{
  if (stmt)
    link_imm_use (linknode, def);
  else
    link_imm_use (linknode, NULL);
  linknode->loc.stmt = stmt;
}

/* Relink a new node in place of an old node in the list.  */
static inline void
relink_imm_use (ssa_use_operand_t *node, ssa_use_operand_t *old)
{
  /* The node one had better be in the same list.  */
  gcc_assert (*(old->use) == *(node->use));
  node->prev = old->prev;
  node->next = old->next;
  if (old->prev)
    {
      old->prev->next = node;
      old->next->prev = node;
      /* Remove the old node from the list.  */
      old->prev = NULL;
    }
}

/* Relink ssa_imm_use node LINKNODE into the chain for OLD, with use occurring
   in STMT.  */
static inline void
relink_imm_use_stmt (ssa_use_operand_t *linknode, ssa_use_operand_t *old,
		     gimple stmt)
{
  if (stmt)
    relink_imm_use (linknode, old);
  else
    link_imm_use (linknode, NULL);
  linknode->loc.stmt = stmt;
}


/* Return true is IMM has reached the end of the immediate use list.  */
static inline bool
end_readonly_imm_use_p (const imm_use_iterator *imm)
{
  return (imm->imm_use == imm->end_p);
}

/* Initialize iterator IMM to process the list for VAR.  */
static inline use_operand_p
first_readonly_imm_use (imm_use_iterator *imm, tree var)
{
  imm->end_p = &(SSA_NAME_IMM_USE_NODE (var));
  imm->imm_use = imm->end_p->next;
#ifdef ENABLE_CHECKING
  imm->iter_node.next = imm->imm_use->next;
#endif
  if (end_readonly_imm_use_p (imm))
    return NULL_USE_OPERAND_P;
  return imm->imm_use;
}

/* Bump IMM to the next use in the list.  */
static inline use_operand_p
next_readonly_imm_use (imm_use_iterator *imm)
{
  use_operand_p old = imm->imm_use;

#ifdef ENABLE_CHECKING
  /* If this assertion fails, it indicates the 'next' pointer has changed
     since the last bump.  This indicates that the list is being modified
     via stmt changes, or SET_USE, or somesuch thing, and you need to be
     using the SAFE version of the iterator.  */
  gcc_assert (imm->iter_node.next == old->next);
  imm->iter_node.next = old->next->next;
#endif

  imm->imm_use = old->next;
  if (end_readonly_imm_use_p (imm))
    return NULL_USE_OPERAND_P;
  return imm->imm_use;
}

/* tree-cfg.c */
extern bool has_zero_uses_1 (const ssa_use_operand_t *head);
extern bool single_imm_use_1 (const ssa_use_operand_t *head,
			      use_operand_p *use_p, gimple *stmt);

/* Return true if VAR has no nondebug uses.  */
static inline bool
has_zero_uses (const_tree var)
{
  const ssa_use_operand_t *const ptr = &(SSA_NAME_IMM_USE_NODE (var));

  /* A single use_operand means there is no items in the list.  */
  if (ptr == ptr->next)
    return true;

  /* If there are debug stmts, we have to look at each use and see
     whether there are any nondebug uses.  */
  if (!MAY_HAVE_DEBUG_STMTS)
    return false;

  return has_zero_uses_1 (ptr);
}

/* Return true if VAR has a single nondebug use.  */
static inline bool
has_single_use (const_tree var)
{
  const ssa_use_operand_t *const ptr = &(SSA_NAME_IMM_USE_NODE (var));

  /* If there aren't any uses whatsoever, we're done.  */
  if (ptr == ptr->next)
    return false;

  /* If there's a single use, check that it's not a debug stmt.  */
  if (ptr == ptr->next->next)
    return !is_gimple_debug (USE_STMT (ptr->next));

  /* If there are debug stmts, we have to look at each of them.  */
  if (!MAY_HAVE_DEBUG_STMTS)
    return false;

  return single_imm_use_1 (ptr, NULL, NULL);
}


/* If VAR has only a single immediate nondebug use, return true, and
   set USE_P and STMT to the use pointer and stmt of occurrence.  */
static inline bool
single_imm_use (const_tree var, use_operand_p *use_p, gimple *stmt)
{
  const ssa_use_operand_t *const ptr = &(SSA_NAME_IMM_USE_NODE (var));

  /* If there aren't any uses whatsoever, we're done.  */
  if (ptr == ptr->next)
    {
    return_false:
      *use_p = NULL_USE_OPERAND_P;
      *stmt = NULL;
      return false;
    }

  /* If there's a single use, check that it's not a debug stmt.  */
  if (ptr == ptr->next->next)
    {
      if (!is_gimple_debug (USE_STMT (ptr->next)))
	{
	  *use_p = ptr->next;
	  *stmt = ptr->next->loc.stmt;
	  return true;
	}
      else
	goto return_false;
    }

  /* If there are debug stmts, we have to look at each of them.  */
  if (!MAY_HAVE_DEBUG_STMTS)
    goto return_false;

  return single_imm_use_1 (ptr, use_p, stmt);
}

/* Return the number of nondebug immediate uses of VAR.  */
static inline unsigned int
num_imm_uses (const_tree var)
{
  const ssa_use_operand_t *const start = &(SSA_NAME_IMM_USE_NODE (var));
  const ssa_use_operand_t *ptr;
  unsigned int num = 0;

  if (!MAY_HAVE_DEBUG_STMTS)
    for (ptr = start->next; ptr != start; ptr = ptr->next)
      num++;
  else
    for (ptr = start->next; ptr != start; ptr = ptr->next)
      if (!is_gimple_debug (USE_STMT (ptr)))
	num++;

  return num;
}

/* Return the tree pointed-to by USE.  */
static inline tree
get_use_from_ptr (use_operand_p use)
{
  return *(use->use);
}

/* Return the tree pointed-to by DEF.  */
static inline tree
get_def_from_ptr (def_operand_p def)
{
  return *def;
}

/* Return a use_operand_p pointer for argument I of PHI node GS.  */

static inline use_operand_p
gimple_phi_arg_imm_use_ptr (gimple gs, int i)
{
  return &gimple_phi_arg (gs, i)->imm_use;
}

/* Return the tree operand for argument I of PHI node GS.  */

static inline tree
gimple_phi_arg_def (gimple gs, size_t index)
{
  struct phi_arg_d *pd = gimple_phi_arg (gs, index);
  return get_use_from_ptr (&pd->imm_use);
}

/* Return a pointer to the tree operand for argument I of PHI node GS.  */

static inline tree *
gimple_phi_arg_def_ptr (gimple gs, size_t index)
{
  return &gimple_phi_arg (gs, index)->def;
}

/* Return the edge associated with argument I of phi node GS.  */

static inline edge
gimple_phi_arg_edge (gimple gs, size_t i)
{
  return EDGE_PRED (gimple_bb (gs), i);
}

/* Return the source location of gimple argument I of phi node GS.  */

static inline source_location
gimple_phi_arg_location (gimple gs, size_t i)
{
  return gimple_phi_arg (gs, i)->locus;
}

/* Return the source location of the argument on edge E of phi node GS.  */

static inline source_location
gimple_phi_arg_location_from_edge (gimple gs, edge e)
{
  return gimple_phi_arg (gs, e->dest_idx)->locus;
}

/* Set the source location of gimple argument I of phi node GS to LOC.  */

static inline void
gimple_phi_arg_set_location (gimple gs, size_t i, source_location loc)
{
  gimple_phi_arg (gs, i)->locus = loc;
}

/* Return TRUE if argument I of phi node GS has a location record.  */

static inline bool
gimple_phi_arg_has_location (gimple gs, size_t i)
{
  return gimple_phi_arg_location (gs, i) != UNKNOWN_LOCATION;
}


/* Return the PHI nodes for basic block BB, or NULL if there are no
   PHI nodes.  */
static inline gimple_seq
phi_nodes (const_basic_block bb)
{
  gcc_assert (!(bb->flags & BB_RTL));
  if (!bb->il.gimple)
    return NULL;
  return bb->il.gimple->phi_nodes;
}

/* Set PHI nodes of a basic block BB to SEQ.  */

static inline void
set_phi_nodes (basic_block bb, gimple_seq seq)
{
  gimple_stmt_iterator i;

  gcc_assert (!(bb->flags & BB_RTL));
  bb->il.gimple->phi_nodes = seq;
  if (seq)
    for (i = gsi_start (seq); !gsi_end_p (i); gsi_next (&i))
      gimple_set_bb (gsi_stmt (i), bb);
}

/* Return the phi argument which contains the specified use.  */

static inline int
phi_arg_index_from_use (use_operand_p use)
{
  struct phi_arg_d *element, *root;
  size_t index;
  gimple phi;

  /* Since the use is the first thing in a PHI argument element, we can
     calculate its index based on casting it to an argument, and performing
     pointer arithmetic.  */

  phi = USE_STMT (use);
  gcc_assert (gimple_code (phi) == GIMPLE_PHI);

  element = (struct phi_arg_d *)use;
  root = gimple_phi_arg (phi, 0);
  index = element - root;

#ifdef ENABLE_CHECKING
  /* Make sure the calculation doesn't have any leftover bytes.  If it does,
     then imm_use is likely not the first element in phi_arg_d.  */
  gcc_assert ((((char *)element - (char *)root)
	       % sizeof (struct phi_arg_d)) == 0
	      && index < gimple_phi_capacity (phi));
#endif

 return index;
}

/* Mark VAR as used, so that it'll be preserved during rtl expansion.  */

static inline void
set_is_used (tree var)
{
  var_ann_t ann = get_var_ann (var);
  ann->used = 1;
}


/* Return true if T (assumed to be a DECL) is a global variable.
   A variable is considered global if its storage is not automatic.  */

static inline bool
is_global_var (const_tree t)
{
  return (TREE_STATIC (t) || DECL_EXTERNAL (t));
}


/* Return true if VAR may be aliased.  A variable is considered as
   maybe aliased if it has its address taken by the local TU
   or possibly by another TU and might be modified through a pointer.  */

static inline bool
may_be_aliased (const_tree var)
{
  return (TREE_CODE (var) != CONST_DECL
	  && !((TREE_STATIC (var) || TREE_PUBLIC (var) || DECL_EXTERNAL (var))
	       && TREE_READONLY (var)
	       && !TYPE_NEEDS_CONSTRUCTING (TREE_TYPE (var)))
	  && (TREE_PUBLIC (var)
	      || DECL_EXTERNAL (var)
	      || TREE_ADDRESSABLE (var)));
}


/* PHI nodes should contain only ssa_names and invariants.  A test
   for ssa_name is definitely simpler; don't let invalid contents
   slip in in the meantime.  */

static inline bool
phi_ssa_name_p (const_tree t)
{
  if (TREE_CODE (t) == SSA_NAME)
    return true;
#ifdef ENABLE_CHECKING
  gcc_assert (is_gimple_min_invariant (t));
#endif
  return false;
}


/* Returns the loop of the statement STMT.  */

static inline struct loop *
loop_containing_stmt (gimple stmt)
{
  basic_block bb = gimple_bb (stmt);
  if (!bb)
    return NULL;

  return bb->loop_father;
}


/* Return true if VAR is clobbered by function calls.  */
static inline bool
is_call_clobbered (const_tree var)
{
  return (is_global_var (var)
	  || (may_be_aliased (var)
	      && pt_solution_includes (&cfun->gimple_df->escaped, var)));
}

/* Return true if VAR is used by function calls.  */
static inline bool
is_call_used (const_tree var)
{
  return (is_call_clobbered (var)
	  || (may_be_aliased (var)
	      && pt_solution_includes (&cfun->gimple_df->callused, var)));
}

/*  -----------------------------------------------------------------------  */

/* The following set of routines are used to iterator over various type of
   SSA operands.  */

/* Return true if PTR is finished iterating.  */
static inline bool
op_iter_done (const ssa_op_iter *ptr)
{
  return ptr->done;
}

/* Get the next iterator use value for PTR.  */
static inline use_operand_p
op_iter_next_use (ssa_op_iter *ptr)
{
  use_operand_p use_p;
#ifdef ENABLE_CHECKING
  gcc_assert (ptr->iter_type == ssa_op_iter_use);
#endif
  if (ptr->uses)
    {
      use_p = USE_OP_PTR (ptr->uses);
      ptr->uses = ptr->uses->next;
      return use_p;
    }
  if (ptr->phi_i < ptr->num_phi)
    {
      return PHI_ARG_DEF_PTR (ptr->phi_stmt, (ptr->phi_i)++);
    }
  ptr->done = true;
  return NULL_USE_OPERAND_P;
}

/* Get the next iterator def value for PTR.  */
static inline def_operand_p
op_iter_next_def (ssa_op_iter *ptr)
{
  def_operand_p def_p;
#ifdef ENABLE_CHECKING
  gcc_assert (ptr->iter_type == ssa_op_iter_def);
#endif
  if (ptr->defs)
    {
      def_p = DEF_OP_PTR (ptr->defs);
      ptr->defs = ptr->defs->next;
      return def_p;
    }
  ptr->done = true;
  return NULL_DEF_OPERAND_P;
}

/* Get the next iterator tree value for PTR.  */
static inline tree
op_iter_next_tree (ssa_op_iter *ptr)
{
  tree val;
#ifdef ENABLE_CHECKING
  gcc_assert (ptr->iter_type == ssa_op_iter_tree);
#endif
  if (ptr->uses)
    {
      val = USE_OP (ptr->uses);
      ptr->uses = ptr->uses->next;
      return val;
    }
  if (ptr->defs)
    {
      val = DEF_OP (ptr->defs);
      ptr->defs = ptr->defs->next;
      return val;
    }

  ptr->done = true;
  return NULL_TREE;

}


/* This functions clears the iterator PTR, and marks it done.  This is normally
   used to prevent warnings in the compile about might be uninitialized
   components.  */

static inline void
clear_and_done_ssa_iter (ssa_op_iter *ptr)
{
  ptr->defs = NULL;
  ptr->uses = NULL;
  ptr->iter_type = ssa_op_iter_none;
  ptr->phi_i = 0;
  ptr->num_phi = 0;
  ptr->phi_stmt = NULL;
  ptr->done = true;
}

/* Initialize the iterator PTR to the virtual defs in STMT.  */
static inline void
op_iter_init (ssa_op_iter *ptr, gimple stmt, int flags)
{
  /* We do not support iterating over virtual defs or uses without
     iterating over defs or uses at the same time.  */
  gcc_assert ((!(flags & SSA_OP_VDEF) || (flags & SSA_OP_DEF))
	      && (!(flags & SSA_OP_VUSE) || (flags & SSA_OP_USE)));
  ptr->defs = (flags & (SSA_OP_DEF|SSA_OP_VDEF)) ? gimple_def_ops (stmt) : NULL;
  if (!(flags & SSA_OP_VDEF)
      && ptr->defs
      && gimple_vdef (stmt) != NULL_TREE)
    ptr->defs = ptr->defs->next;
  ptr->uses = (flags & (SSA_OP_USE|SSA_OP_VUSE)) ? gimple_use_ops (stmt) : NULL;
  if (!(flags & SSA_OP_VUSE)
      && ptr->uses
      && gimple_vuse (stmt) != NULL_TREE)
    ptr->uses = ptr->uses->next;
  ptr->done = false;

  ptr->phi_i = 0;
  ptr->num_phi = 0;
  ptr->phi_stmt = NULL;
}

/* Initialize iterator PTR to the use operands in STMT based on FLAGS. Return
   the first use.  */
static inline use_operand_p
op_iter_init_use (ssa_op_iter *ptr, gimple stmt, int flags)
{
  gcc_assert ((flags & SSA_OP_ALL_DEFS) == 0
	      && (flags & SSA_OP_USE));
  op_iter_init (ptr, stmt, flags);
  ptr->iter_type = ssa_op_iter_use;
  return op_iter_next_use (ptr);
}

/* Initialize iterator PTR to the def operands in STMT based on FLAGS. Return
   the first def.  */
static inline def_operand_p
op_iter_init_def (ssa_op_iter *ptr, gimple stmt, int flags)
{
  gcc_assert ((flags & SSA_OP_ALL_USES) == 0
	      && (flags & SSA_OP_DEF));
  op_iter_init (ptr, stmt, flags);
  ptr->iter_type = ssa_op_iter_def;
  return op_iter_next_def (ptr);
}

/* Initialize iterator PTR to the operands in STMT based on FLAGS. Return
   the first operand as a tree.  */
static inline tree
op_iter_init_tree (ssa_op_iter *ptr, gimple stmt, int flags)
{
  op_iter_init (ptr, stmt, flags);
  ptr->iter_type = ssa_op_iter_tree;
  return op_iter_next_tree (ptr);
}


/* If there is a single operand in STMT matching FLAGS, return it.  Otherwise
   return NULL.  */
static inline tree
single_ssa_tree_operand (gimple stmt, int flags)
{
  tree var;
  ssa_op_iter iter;

  var = op_iter_init_tree (&iter, stmt, flags);
  if (op_iter_done (&iter))
    return NULL_TREE;
  op_iter_next_tree (&iter);
  if (op_iter_done (&iter))
    return var;
  return NULL_TREE;
}


/* If there is a single operand in STMT matching FLAGS, return it.  Otherwise
   return NULL.  */
static inline use_operand_p
single_ssa_use_operand (gimple stmt, int flags)
{
  use_operand_p var;
  ssa_op_iter iter;

  var = op_iter_init_use (&iter, stmt, flags);
  if (op_iter_done (&iter))
    return NULL_USE_OPERAND_P;
  op_iter_next_use (&iter);
  if (op_iter_done (&iter))
    return var;
  return NULL_USE_OPERAND_P;
}



/* If there is a single operand in STMT matching FLAGS, return it.  Otherwise
   return NULL.  */
static inline def_operand_p
single_ssa_def_operand (gimple stmt, int flags)
{
  def_operand_p var;
  ssa_op_iter iter;

  var = op_iter_init_def (&iter, stmt, flags);
  if (op_iter_done (&iter))
    return NULL_DEF_OPERAND_P;
  op_iter_next_def (&iter);
  if (op_iter_done (&iter))
    return var;
  return NULL_DEF_OPERAND_P;
}


/* Return true if there are zero operands in STMT matching the type
   given in FLAGS.  */
static inline bool
zero_ssa_operands (gimple stmt, int flags)
{
  ssa_op_iter iter;

  op_iter_init_tree (&iter, stmt, flags);
  return op_iter_done (&iter);
}


/* Return the number of operands matching FLAGS in STMT.  */
static inline int
num_ssa_operands (gimple stmt, int flags)
{
  ssa_op_iter iter;
  tree t;
  int num = 0;

  FOR_EACH_SSA_TREE_OPERAND (t, stmt, iter, flags)
    num++;
  return num;
}


/* Delink all immediate_use information for STMT.  */
static inline void
delink_stmt_imm_use (gimple stmt)
{
   ssa_op_iter iter;
   use_operand_p use_p;

   if (ssa_operands_active ())
     FOR_EACH_SSA_USE_OPERAND (use_p, stmt, iter, SSA_OP_ALL_USES)
       delink_imm_use (use_p);
}


/* If there is a single DEF in the PHI node which matches FLAG, return it.
   Otherwise return NULL_DEF_OPERAND_P.  */
static inline tree
single_phi_def (gimple stmt, int flags)
{
  tree def = PHI_RESULT (stmt);
  if ((flags & SSA_OP_DEF) && is_gimple_reg (def))
    return def;
  if ((flags & SSA_OP_VIRTUAL_DEFS) && !is_gimple_reg (def))
    return def;
  return NULL_TREE;
}

/* Initialize the iterator PTR for uses matching FLAGS in PHI.  FLAGS should
   be either SSA_OP_USES or SSA_OP_VIRTUAL_USES.  */
static inline use_operand_p
op_iter_init_phiuse (ssa_op_iter *ptr, gimple phi, int flags)
{
  tree phi_def = gimple_phi_result (phi);
  int comp;

  clear_and_done_ssa_iter (ptr);
  ptr->done = false;

  gcc_assert ((flags & (SSA_OP_USE | SSA_OP_VIRTUAL_USES)) != 0);

  comp = (is_gimple_reg (phi_def) ? SSA_OP_USE : SSA_OP_VIRTUAL_USES);

  /* If the PHI node doesn't the operand type we care about, we're done.  */
  if ((flags & comp) == 0)
    {
      ptr->done = true;
      return NULL_USE_OPERAND_P;
    }

  ptr->phi_stmt = phi;
  ptr->num_phi = gimple_phi_num_args (phi);
  ptr->iter_type = ssa_op_iter_use;
  return op_iter_next_use (ptr);
}


/* Start an iterator for a PHI definition.  */

static inline def_operand_p
op_iter_init_phidef (ssa_op_iter *ptr, gimple phi, int flags)
{
  tree phi_def = PHI_RESULT (phi);
  int comp;

  clear_and_done_ssa_iter (ptr);
  ptr->done = false;

  gcc_assert ((flags & (SSA_OP_DEF | SSA_OP_VIRTUAL_DEFS)) != 0);

  comp = (is_gimple_reg (phi_def) ? SSA_OP_DEF : SSA_OP_VIRTUAL_DEFS);

  /* If the PHI node doesn't have the operand type we care about,
     we're done.  */
  if ((flags & comp) == 0)
    {
      ptr->done = true;
      return NULL_DEF_OPERAND_P;
    }

  ptr->iter_type = ssa_op_iter_def;
  /* The first call to op_iter_next_def will terminate the iterator since
     all the fields are NULL.  Simply return the result here as the first and
     therefore only result.  */
  return PHI_RESULT_PTR (phi);
}

/* Return true is IMM has reached the end of the immediate use stmt list.  */

static inline bool
end_imm_use_stmt_p (const imm_use_iterator *imm)
{
  return (imm->imm_use == imm->end_p);
}

/* Finished the traverse of an immediate use stmt list IMM by removing the
   placeholder node from the list.  */

static inline void
end_imm_use_stmt_traverse (imm_use_iterator *imm)
{
  delink_imm_use (&(imm->iter_node));
}

/* Immediate use traversal of uses within a stmt require that all the
   uses on a stmt be sequentially listed.  This routine is used to build up
   this sequential list by adding USE_P to the end of the current list
   currently delimited by HEAD and LAST_P.  The new LAST_P value is
   returned.  */

static inline use_operand_p
move_use_after_head (use_operand_p use_p, use_operand_p head,
		      use_operand_p last_p)
{
#ifdef ENABLE_CHECKING
  gcc_assert (USE_FROM_PTR (use_p) == USE_FROM_PTR (head));
#endif
  /* Skip head when we find it.  */
  if (use_p != head)
    {
      /* If use_p is already linked in after last_p, continue.  */
      if (last_p->next == use_p)
	last_p = use_p;
      else
	{
	  /* Delink from current location, and link in at last_p.  */
	  delink_imm_use (use_p);
	  link_imm_use_to_list (use_p, last_p);
	  last_p = use_p;
	}
    }
  return last_p;
}


/* This routine will relink all uses with the same stmt as HEAD into the list
   immediately following HEAD for iterator IMM.  */

static inline void
link_use_stmts_after (use_operand_p head, imm_use_iterator *imm)
{
  use_operand_p use_p;
  use_operand_p last_p = head;
  gimple head_stmt = USE_STMT (head);
  tree use = USE_FROM_PTR (head);
  ssa_op_iter op_iter;
  int flag;

  /* Only look at virtual or real uses, depending on the type of HEAD.  */
  flag = (is_gimple_reg (use) ? SSA_OP_USE : SSA_OP_VIRTUAL_USES);

  if (gimple_code (head_stmt) == GIMPLE_PHI)
    {
      FOR_EACH_PHI_ARG (use_p, head_stmt, op_iter, flag)
	if (USE_FROM_PTR (use_p) == use)
	  last_p = move_use_after_head (use_p, head, last_p);
    }
  else
    {
      if (flag == SSA_OP_USE)
	{
	  FOR_EACH_SSA_USE_OPERAND (use_p, head_stmt, op_iter, flag)
	    if (USE_FROM_PTR (use_p) == use)
	      last_p = move_use_after_head (use_p, head, last_p);
	}
      else if ((use_p = gimple_vuse_op (head_stmt)) != NULL_USE_OPERAND_P)
	{
	  if (USE_FROM_PTR (use_p) == use)
	    last_p = move_use_after_head (use_p, head, last_p);
	}
    }
  /* Link iter node in after last_p.  */
  if (imm->iter_node.prev != NULL)
    delink_imm_use (&imm->iter_node);
  link_imm_use_to_list (&(imm->iter_node), last_p);
}

/* Initialize IMM to traverse over uses of VAR.  Return the first statement.  */
static inline gimple
first_imm_use_stmt (imm_use_iterator *imm, tree var)
{
  imm->end_p = &(SSA_NAME_IMM_USE_NODE (var));
  imm->imm_use = imm->end_p->next;
  imm->next_imm_name = NULL_USE_OPERAND_P;

  /* iter_node is used as a marker within the immediate use list to indicate
     where the end of the current stmt's uses are.  Initialize it to NULL
     stmt and use, which indicates a marker node.  */
  imm->iter_node.prev = NULL_USE_OPERAND_P;
  imm->iter_node.next = NULL_USE_OPERAND_P;
  imm->iter_node.loc.stmt = NULL;
  imm->iter_node.use = NULL;

  if (end_imm_use_stmt_p (imm))
    return NULL;

  link_use_stmts_after (imm->imm_use, imm);

  return USE_STMT (imm->imm_use);
}

/* Bump IMM to the next stmt which has a use of var.  */

static inline gimple
next_imm_use_stmt (imm_use_iterator *imm)
{
  imm->imm_use = imm->iter_node.next;
  if (end_imm_use_stmt_p (imm))
    {
      if (imm->iter_node.prev != NULL)
	delink_imm_use (&imm->iter_node);
      return NULL;
    }

  link_use_stmts_after (imm->imm_use, imm);
  return USE_STMT (imm->imm_use);
}

/* This routine will return the first use on the stmt IMM currently refers
   to.  */

static inline use_operand_p
first_imm_use_on_stmt (imm_use_iterator *imm)
{
  imm->next_imm_name = imm->imm_use->next;
  return imm->imm_use;
}

/*  Return TRUE if the last use on the stmt IMM refers to has been visited.  */

static inline bool
end_imm_use_on_stmt_p (const imm_use_iterator *imm)
{
  return (imm->imm_use == &(imm->iter_node));
}

/* Bump to the next use on the stmt IMM refers to, return NULL if done.  */

static inline use_operand_p
next_imm_use_on_stmt (imm_use_iterator *imm)
{
  imm->imm_use = imm->next_imm_name;
  if (end_imm_use_on_stmt_p (imm))
    return NULL_USE_OPERAND_P;
  else
    {
      imm->next_imm_name = imm->imm_use->next;
      return imm->imm_use;
    }
}

/* Return true if VAR cannot be modified by the program.  */

static inline bool
unmodifiable_var_p (const_tree var)
{
  if (TREE_CODE (var) == SSA_NAME)
    var = SSA_NAME_VAR (var);

  return TREE_READONLY (var) && (TREE_STATIC (var) || DECL_EXTERNAL (var));
}

/* Return true if REF, an ARRAY_REF, has an INDIRECT_REF somewhere in it.  */

static inline bool
array_ref_contains_indirect_ref (const_tree ref)
{
  gcc_assert (TREE_CODE (ref) == ARRAY_REF);

  do {
    ref = TREE_OPERAND (ref, 0);
  } while (handled_component_p (ref));

  return TREE_CODE (ref) == INDIRECT_REF;
}

/* Return true if REF, a handled component reference, has an ARRAY_REF
   somewhere in it.  */

static inline bool
ref_contains_array_ref (const_tree ref)
{
  gcc_assert (handled_component_p (ref));

  do {
    if (TREE_CODE (ref) == ARRAY_REF)
      return true;
    ref = TREE_OPERAND (ref, 0);
  } while (handled_component_p (ref));

  return false;
}

/* Return true if REF has an VIEW_CONVERT_EXPR somewhere in it.  */

static inline bool
contains_view_convert_expr_p (const_tree ref)
{
  while (handled_component_p (ref))
    {
      if (TREE_CODE (ref) == VIEW_CONVERT_EXPR)
	return true;
      ref = TREE_OPERAND (ref, 0);
    }

  return false;
}

/* Return true, if the two ranges [POS1, SIZE1] and [POS2, SIZE2]
   overlap.  SIZE1 and/or SIZE2 can be (unsigned)-1 in which case the
   range is open-ended.  Otherwise return false.  */

static inline bool
ranges_overlap_p (unsigned HOST_WIDE_INT pos1,
		  unsigned HOST_WIDE_INT size1,
		  unsigned HOST_WIDE_INT pos2,
		  unsigned HOST_WIDE_INT size2)
{
  if (pos1 >= pos2
      && (size2 == (unsigned HOST_WIDE_INT)-1
	  || pos1 < (pos2 + size2)))
    return true;
  if (pos2 >= pos1
      && (size1 == (unsigned HOST_WIDE_INT)-1
	  || pos2 < (pos1 + size1)))
    return true;

  return false;
}

/* Accessor to tree-ssa-operands.c caches.  */
static inline struct ssa_operands *
gimple_ssa_operands (const struct function *fun)
{
  return &fun->gimple_df->ssa_operands;
}

/* Given an edge_var_map V, return the PHI arg definition.  */

static inline tree
redirect_edge_var_map_def (edge_var_map *v)
{
  return v->def;
}

/* Given an edge_var_map V, return the PHI result.  */

static inline tree
redirect_edge_var_map_result (edge_var_map *v)
{
  return v->result;
}

/* Given an edge_var_map V, return the PHI arg location.  */

static inline source_location
redirect_edge_var_map_location (edge_var_map *v)
{
  return v->locus;
}


/* Return an SSA_NAME node for variable VAR defined in statement STMT
   in function cfun.  */

static inline tree
make_ssa_name (tree var, gimple stmt)
{
  return make_ssa_name_fn (cfun, var, stmt);
}

#endif /* _TREE_FLOW_INLINE_H  */
