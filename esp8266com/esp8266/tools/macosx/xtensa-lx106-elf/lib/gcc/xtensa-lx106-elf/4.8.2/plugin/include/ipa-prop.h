/* Interprocedural analyses.
   Copyright (C) 2005-2013 Free Software Foundation, Inc.

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

#ifndef IPA_PROP_H
#define IPA_PROP_H

#include "tree.h"
#include "vec.h"
#include "cgraph.h"
#include "gimple.h"
#include "alloc-pool.h"

/* The following definitions and interfaces are used by
   interprocedural analyses or parameters.  */

/* ipa-prop.c stuff (ipa-cp, indirect inlining):  */

/* A jump function for a callsite represents the values passed as actual
   arguments of the callsite.  They were originally proposed in a paper called
   "Interprocedural Constant Propagation", by David Callahan, Keith D Cooper,
   Ken Kennedy, Linda Torczon in Comp86, pg 152-161.  There are three main
   types of values :

   Pass-through - the caller's formal parameter is passed as an actual
                  argument, possibly one simple operation performed on it.
   Constant     - a constant (is_gimple_ip_invariant)is passed as an actual
                  argument.
   Unknown      - neither of the above.

   IPA_JF_ANCESTOR is a special pass-through jump function, which means that
   the result is an address of a part of the object pointed to by the formal
   parameter to which the function refers.  It is mainly intended to represent
   getting addresses of of ancestor fields in C++
   (e.g. &this_1(D)->D.1766.D.1756).  Note that if the original pointer is
   NULL, ancestor jump function must behave like a simple pass-through.

   Other pass-through functions can either simply pass on an unchanged formal
   parameter or can apply one simple binary operation to it (such jump
   functions are called polynomial).

   IPA_JF_KNOWN_TYPE is a special type of an "unknown" function that applies
   only to pointer parameters.  It means that even though we cannot prove that
   the passed value is an interprocedural constant, we still know the exact
   type of the containing object which may be valuable for devirtualization.

   Jump functions are computed in ipa-prop.c by function
   update_call_notes_after_inlining.  Some information can be lost and jump
   functions degraded accordingly when inlining, see
   update_call_notes_after_inlining in the same file.  */

enum jump_func_type
{
  IPA_JF_UNKNOWN = 0,  /* newly allocated and zeroed jump functions default */
  IPA_JF_KNOWN_TYPE,        /* represented by field known_type */
  IPA_JF_CONST,             /* represented by field costant */
  IPA_JF_PASS_THROUGH,	    /* represented by field pass_through */
  IPA_JF_ANCESTOR	    /* represented by field ancestor */
};

/* Structure holding data required to describe a known type jump function.  */
struct GTY(()) ipa_known_type_data
{
  /* Offset of the component of the base_type being described.  */
  HOST_WIDE_INT offset;
  /* Type of the whole object.  */
  tree base_type;
  /* Type of the component of the object that is being described.  */
  tree component_type;
};

/* Structure holding data required to describe a pass-through jump function.  */

struct GTY(()) ipa_pass_through_data
{
  /* If an operation is to be performed on the original parameter, this is the
     second (constant) operand.  */
  tree operand;
  /* Number of the caller's formal parameter being passed.  */
  int formal_id;
  /* Operation that is performed on the argument before it is passed on.
     NOP_EXPR means no operation.  Otherwise oper must be a simple binary
     arithmetic operation where the caller's parameter is the first operand and
     operand field from this structure is the second one.  */
  enum tree_code operation;
  /* When the passed value is a pointer, it is set to true only when we are
     certain that no write to the object it points to has occurred since the
     caller functions started execution, except for changes noted in the
     aggregate part of the jump function (see description of
     ipa_agg_jump_function).  The flag is used only when the operation is
     NOP_EXPR.  */
  bool agg_preserved;
};

/* Structure holding data required to describe an ancestor pass-through
   jump function.  */

struct GTY(()) ipa_ancestor_jf_data
{
  /* Offset of the field representing the ancestor.  */
  HOST_WIDE_INT offset;
  /* Type of the result.  */
  tree type;
  /* Number of the caller's formal parameter being passed.  */
  int formal_id;
  /* Flag with the same meaning like agg_preserve in ipa_pass_through_data.  */
  bool agg_preserved;
};

/* An element in an aggegate part of a jump function describing a known value
   at a given offset.  When it is part of a pass-through jump function with
   agg_preserved set or an ancestor jump function with agg_preserved set, all
   unlisted positions are assumed to be preserved but the value can be a type
   node, which means that the particular piece (starting at offset and having
   the size of the type) is clobbered with an unknown value.  When
   agg_preserved is false or the type of the containing jump function is
   different, all unlisted parts are assumed to be unknown and all values must
   fullfill is_gimple_ip_invariant.  */

typedef struct GTY(()) ipa_agg_jf_item
{
  /* The offset at which the known value is located within the aggregate.  */
  HOST_WIDE_INT offset;

  /* The known constant or type if this is a clobber.  */
  tree value;
} ipa_agg_jf_item_t;


/* Aggregate jump function - i.e. description of contents of aggregates passed
   either by reference or value.  */

struct GTY(()) ipa_agg_jump_function
{
  /* Description of the individual items.  */
  vec<ipa_agg_jf_item_t, va_gc> *items;
  /* True if the data was passed by reference (as opposed to by value). */
  bool by_ref;
};

typedef struct ipa_agg_jump_function *ipa_agg_jump_function_p;
typedef struct ipa_agg_jump_function ipa_agg_jump_function_t;

/* A jump function for a callsite represents the values passed as actual
   arguments of the callsite. See enum jump_func_type for the various
   types of jump functions supported.  */
typedef struct GTY (()) ipa_jump_func
{
  /* Aggregate contants description.  See struct ipa_agg_jump_function and its
     description.  */
  struct ipa_agg_jump_function agg;

  enum jump_func_type type;
  /* Represents a value of a jump function.  pass_through is used only in jump
     function context.  constant represents the actual constant in constant jump
     functions and member_cst holds constant c++ member functions.  */
  union jump_func_value
  {
    struct ipa_known_type_data GTY ((tag ("IPA_JF_KNOWN_TYPE"))) known_type;
    tree GTY ((tag ("IPA_JF_CONST"))) constant;
    struct ipa_pass_through_data GTY ((tag ("IPA_JF_PASS_THROUGH"))) pass_through;
    struct ipa_ancestor_jf_data GTY ((tag ("IPA_JF_ANCESTOR"))) ancestor;
  } GTY ((desc ("%1.type"))) value;
} ipa_jump_func_t;


/* Return the offset of the component that is decribed by a known type jump
   function JFUNC.  */

static inline HOST_WIDE_INT
ipa_get_jf_known_type_offset (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_KNOWN_TYPE);
  return jfunc->value.known_type.offset;
}

/* Return the base type of a known type jump function JFUNC.  */

static inline tree
ipa_get_jf_known_type_base_type (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_KNOWN_TYPE);
  return jfunc->value.known_type.base_type;
}

/* Return the component type of a known type jump function JFUNC.  */

static inline tree
ipa_get_jf_known_type_component_type (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_KNOWN_TYPE);
  return jfunc->value.known_type.component_type;
}

/* Return the constant stored in a constant jump functin JFUNC.  */

static inline tree
ipa_get_jf_constant (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_CONST);
  return jfunc->value.constant;
}

/* Return the operand of a pass through jmp function JFUNC.  */

static inline tree
ipa_get_jf_pass_through_operand (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_PASS_THROUGH);
  return jfunc->value.pass_through.operand;
}

/* Return the number of the caller's formal parameter that a pass through jump
   function JFUNC refers to.  */

static inline int
ipa_get_jf_pass_through_formal_id (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_PASS_THROUGH);
  return jfunc->value.pass_through.formal_id;
}

/* Return operation of a pass through jump function JFUNC.  */

static inline enum tree_code
ipa_get_jf_pass_through_operation (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_PASS_THROUGH);
  return jfunc->value.pass_through.operation;
}

/* Return the agg_preserved flag of a pass through jump functin JFUNC.  */

static inline bool
ipa_get_jf_pass_through_agg_preserved (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_PASS_THROUGH);
  return jfunc->value.pass_through.agg_preserved;
}

/* Return the offset of an ancestor jump function JFUNC.  */

static inline HOST_WIDE_INT
ipa_get_jf_ancestor_offset (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_ANCESTOR);
  return jfunc->value.ancestor.offset;
}

/* Return the result type of an ancestor jump function JFUNC.  */

static inline tree
ipa_get_jf_ancestor_type (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_ANCESTOR);
  return jfunc->value.ancestor.type;
}

/* Return the number of the caller's formal parameter that an ancestor jump
   function JFUNC refers to.  */

static inline int
ipa_get_jf_ancestor_formal_id (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_ANCESTOR);
  return jfunc->value.ancestor.formal_id;
}

/* Return the agg_preserved flag of an ancestor jump functin JFUNC.  */

static inline bool
ipa_get_jf_ancestor_agg_preserved (struct ipa_jump_func *jfunc)
{
  gcc_checking_assert (jfunc->type == IPA_JF_ANCESTOR);
  return jfunc->value.ancestor.agg_preserved;
}

/* Summary describing a single formal parameter.  */

struct ipa_param_descriptor
{
  /* PARAM_DECL of this parameter.  */
  tree decl;
  /* The parameter is used.  */
  unsigned used : 1;
};

typedef struct ipa_param_descriptor ipa_param_descriptor_t;
struct ipcp_lattice;

/* ipa_node_params stores information related to formal parameters of functions
   and some other information for interprocedural passes that operate on
   parameters (such as ipa-cp).  */

struct ipa_node_params
{
  /* Information about individual formal parameters that are gathered when
     summaries are generated. */
  vec<ipa_param_descriptor_t> descriptors;
  /* Pointer to an array of structures describing individual formal
     parameters.  */
  struct ipcp_param_lattices *lattices;
  /* Only for versioned nodes this field would not be NULL,
     it points to the node that IPA cp cloned from.  */
  struct cgraph_node *ipcp_orig_node;
  /* If this node is an ipa-cp clone, these are the known values that describe
     what it has been specialized for.  */
  vec<tree> known_vals;
  /* Whether the param uses analysis has already been performed.  */
  unsigned uses_analysis_done : 1;
  /* Whether the function is enqueued in ipa-cp propagation stack.  */
  unsigned node_enqueued : 1;
  /* Whether we should create a specialized version based on values that are
     known to be constant in all contexts.  */
  unsigned do_clone_for_all_contexts : 1;
  /* Set if this is an IPA-CP clone for all contexts.  */
  unsigned is_all_contexts_clone : 1;
  /* Node has been completely replaced by clones and will be removed after
     ipa-cp is finished.  */
  unsigned node_dead : 1;
};

/* ipa_node_params access functions.  Please use these to access fields that
   are or will be shared among various passes.  */

/* Return the number of formal parameters. */

static inline int
ipa_get_param_count (struct ipa_node_params *info)
{
  return info->descriptors.length ();
}

/* Return the declaration of Ith formal parameter of the function corresponding
   to INFO.  Note there is no setter function as this array is built just once
   using ipa_initialize_node_params. */

static inline tree
ipa_get_param (struct ipa_node_params *info, int i)
{
  return info->descriptors[i].decl;
}

/* Set the used flag corresponding to the Ith formal parameter of the function
   associated with INFO to VAL.  */

static inline void
ipa_set_param_used (struct ipa_node_params *info, int i, bool val)
{
  info->descriptors[i].used = val;
}

/* Return the used flag corresponding to the Ith formal parameter of the
   function associated with INFO.  */

static inline bool
ipa_is_param_used (struct ipa_node_params *info, int i)
{
  return info->descriptors[i].used;
}

/* Information about replacements done in aggregates for a given node (each
   node has its linked list).  */
struct GTY(()) ipa_agg_replacement_value
{
  /* Next item in the linked list.  */
  struct ipa_agg_replacement_value *next;
  /* Offset within the aggregate.  */
  HOST_WIDE_INT offset;
  /* The constant value.  */
  tree value;
  /* The paramter index.  */
  int index;
  /* Whether the value was passed by reference.  */
  bool by_ref;
};

typedef struct ipa_agg_replacement_value *ipa_agg_replacement_value_p;

void ipa_set_node_agg_value_chain (struct cgraph_node *node,
				   struct ipa_agg_replacement_value *aggvals);

/* ipa_edge_args stores information related to a callsite and particularly its
   arguments.  It can be accessed by the IPA_EDGE_REF macro.  */
typedef struct GTY(()) ipa_edge_args
{
  /* Vector of the callsite's jump function of each parameter.  */
  vec<ipa_jump_func_t, va_gc> *jump_functions;
} ipa_edge_args_t;

/* ipa_edge_args access functions.  Please use these to access fields that
   are or will be shared among various passes.  */

/* Return the number of actual arguments. */

static inline int
ipa_get_cs_argument_count (struct ipa_edge_args *args)
{
  return vec_safe_length (args->jump_functions);
}

/* Returns a pointer to the jump function for the ith argument.  Please note
   there is no setter function as jump functions are all set up in
   ipa_compute_jump_functions. */

static inline struct ipa_jump_func *
ipa_get_ith_jump_func (struct ipa_edge_args *args, int i)
{
  return &(*args->jump_functions)[i];
}

/* Vectors need to have typedefs of structures.  */
typedef struct ipa_node_params ipa_node_params_t;

/* Types of vectors holding the infos.  */

/* Vector where the parameter infos are actually stored. */
extern vec<ipa_node_params_t> ipa_node_params_vector;
/* Vector of known aggregate values in cloned nodes.  */
extern GTY(()) vec<ipa_agg_replacement_value_p, va_gc> *ipa_node_agg_replacements;
/* Vector where the parameter infos are actually stored. */
extern GTY(()) vec<ipa_edge_args_t, va_gc> *ipa_edge_args_vector;

/* Return the associated parameter/argument info corresponding to the given
   node/edge.  */
#define IPA_NODE_REF(NODE) (&ipa_node_params_vector[(NODE)->uid])
#define IPA_EDGE_REF(EDGE) (&(*ipa_edge_args_vector)[(EDGE)->uid])
/* This macro checks validity of index returned by
   ipa_get_param_decl_index function.  */
#define IS_VALID_JUMP_FUNC_INDEX(I) ((I) != -1)

/* Creating and freeing ipa_node_params and ipa_edge_args.  */
void ipa_create_all_node_params (void);
void ipa_create_all_edge_args (void);
void ipa_free_edge_args_substructures (struct ipa_edge_args *);
void ipa_free_node_params_substructures (struct ipa_node_params *);
void ipa_free_all_node_params (void);
void ipa_free_all_edge_args (void);
void ipa_free_all_structures_after_ipa_cp (void);
void ipa_free_all_structures_after_iinln (void);
void ipa_register_cgraph_hooks (void);

/* This function ensures the array of node param infos is big enough to
   accommodate a structure for all nodes and reallocates it if not.  */

static inline void
ipa_check_create_node_params (void)
{
  if (!ipa_node_params_vector.exists ())
    ipa_node_params_vector.create (cgraph_max_uid);

  if (ipa_node_params_vector.length () <= (unsigned) cgraph_max_uid)
    ipa_node_params_vector.safe_grow_cleared (cgraph_max_uid + 1);
}

/* This function ensures the array of edge arguments infos is big enough to
   accommodate a structure for all edges and reallocates it if not.  */

static inline void
ipa_check_create_edge_args (void)
{
  if (vec_safe_length (ipa_edge_args_vector) <= (unsigned) cgraph_edge_max_uid)
    vec_safe_grow_cleared (ipa_edge_args_vector, cgraph_edge_max_uid + 1);
}

/* Returns true if the array of edge infos is large enough to accommodate an
   info for EDGE.  The main purpose of this function is that debug dumping
   function can check info availability without causing reallocations.  */

static inline bool
ipa_edge_args_info_available_for_edge_p (struct cgraph_edge *edge)
{
  return ((unsigned) edge->uid < vec_safe_length (ipa_edge_args_vector));
}

/* Return the aggregate replacements for NODE, if there are any.  */

static inline struct ipa_agg_replacement_value *
ipa_get_agg_replacements_for_node (struct cgraph_node *node)
{
  if ((unsigned) node->uid >= vec_safe_length (ipa_node_agg_replacements))
    return NULL;
  return (*ipa_node_agg_replacements)[node->uid];
}

/* Function formal parameters related computations.  */
void ipa_initialize_node_params (struct cgraph_node *node);
bool ipa_propagate_indirect_call_infos (struct cgraph_edge *cs,
					vec<cgraph_edge_p> *new_edges);

/* Indirect edge and binfo processing.  */
tree ipa_get_indirect_edge_target (struct cgraph_edge *ie,
				   vec<tree> ,
				   vec<tree> ,
				   vec<ipa_agg_jump_function_p> );
struct cgraph_edge *ipa_make_edge_direct_to_target (struct cgraph_edge *, tree);

/* Functions related to both.  */
void ipa_analyze_node (struct cgraph_node *);

/* Aggregate jump function related functions.  */
tree ipa_find_agg_cst_for_param (struct ipa_agg_jump_function *, HOST_WIDE_INT,
				 bool);
bool ipa_load_from_parm_agg (struct ipa_node_params *, gimple, tree, int *,
			     HOST_WIDE_INT *, bool *);

/* Debugging interface.  */
void ipa_print_node_params (FILE *, struct cgraph_node *node);
void ipa_print_all_params (FILE *);
void ipa_print_node_jump_functions (FILE *f, struct cgraph_node *node);
void ipa_print_all_jump_functions (FILE * f);
void ipcp_verify_propagated_values (void);

extern alloc_pool ipcp_values_pool;
extern alloc_pool ipcp_sources_pool;
extern alloc_pool ipcp_agg_lattice_pool;

/* Structure to describe transformations of formal parameters and actual
   arguments.  Each instance describes one new parameter and they are meant to
   be stored in a vector.  Additionally, most users will probably want to store
   adjustments about parameters that are being removed altogether so that SSA
   names belonging to them can be replaced by SSA names of an artificial
   variable.  */
struct ipa_parm_adjustment
{
  /* The original PARM_DECL itself, helpful for processing of the body of the
     function itself.  Intended for traversing function bodies.
     ipa_modify_formal_parameters, ipa_modify_call_arguments and
     ipa_combine_adjustments ignore this and use base_index.
     ipa_modify_formal_parameters actually sets this.  */
  tree base;

  /* Type of the new parameter.  However, if by_ref is true, the real type will
     be a pointer to this type.  */
  tree type;

  /* Alias refrerence type to be used in MEM_REFs when adjusting caller
     arguments.  */
  tree alias_ptr_type;

  /* The new declaration when creating/replacing a parameter.  Created by
     ipa_modify_formal_parameters, useful for functions modifying the body
     accordingly. */
  tree reduction;

  /* New declaration of a substitute variable that we may use to replace all
     non-default-def ssa names when a parm decl is going away.  */
  tree new_ssa_base;

  /* If non-NULL and the original parameter is to be removed (copy_param below
     is NULL), this is going to be its nonlocalized vars value.  */
  tree nonlocal_value;

  /* Offset into the original parameter (for the cases when the new parameter
     is a component of an original one).  */
  HOST_WIDE_INT offset;

  /* Zero based index of the original parameter this one is based on.  (ATM
     there is no way to insert a new parameter out of the blue because there is
     no need but if it arises the code can be easily exteded to do so.)  */
  int base_index;

  /* This new parameter is an unmodified parameter at index base_index. */
  unsigned copy_param : 1;

  /* This adjustment describes a parameter that is about to be removed
     completely.  Most users will probably need to book keep those so that they
     don't leave behinfd any non default def ssa names belonging to them.  */
  unsigned remove_param : 1;

  /* The parameter is to be passed by reference.  */
  unsigned by_ref : 1;
};

typedef struct ipa_parm_adjustment ipa_parm_adjustment_t;

typedef vec<ipa_parm_adjustment_t> ipa_parm_adjustment_vec;

vec<tree> ipa_get_vector_of_formal_parms (tree fndecl);
void ipa_modify_formal_parameters (tree fndecl, ipa_parm_adjustment_vec,
				   const char *);
void ipa_modify_call_arguments (struct cgraph_edge *, gimple,
				ipa_parm_adjustment_vec);
ipa_parm_adjustment_vec ipa_combine_adjustments (ipa_parm_adjustment_vec,
						 ipa_parm_adjustment_vec);
void ipa_dump_param_adjustments (FILE *, ipa_parm_adjustment_vec, tree);
void ipa_dump_agg_replacement_values (FILE *f,
				      struct ipa_agg_replacement_value *av);
void ipa_prop_write_jump_functions (void);
void ipa_prop_read_jump_functions (void);
void ipa_prop_write_all_agg_replacement (void);
void ipa_prop_read_all_agg_replacement (void);
void ipa_update_after_lto_read (void);
int ipa_get_param_decl_index (struct ipa_node_params *, tree);
tree ipa_value_from_jfunc (struct ipa_node_params *info,
			   struct ipa_jump_func *jfunc);
unsigned int ipcp_transform_function (struct cgraph_node *node);


/* From tree-sra.c:  */
tree build_ref_for_offset (location_t, tree, HOST_WIDE_INT, tree,
			   gimple_stmt_iterator *, bool);

#endif /* IPA_PROP_H */
