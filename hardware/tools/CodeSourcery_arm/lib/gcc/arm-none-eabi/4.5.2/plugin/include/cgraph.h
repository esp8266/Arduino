/* Callgraph handling code.
   Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010
   Free Software Foundation, Inc.
   Contributed by Jan Hubicka

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

#ifndef GCC_CGRAPH_H
#define GCC_CGRAPH_H
#include "tree.h"
#include "basic-block.h"

enum availability
{
  /* Not yet set by cgraph_function_body_availability.  */
  AVAIL_UNSET,
  /* Function body/variable initializer is unknown.  */
  AVAIL_NOT_AVAILABLE,
  /* Function body/variable initializer is known but might be replaced
     by a different one from other compilation unit and thus needs to
     be dealt with a care.  Like AVAIL_NOT_AVAILABLE it can have
     arbitrary side effects on escaping variables and functions, while
     like AVAILABLE it might access static variables.  */
  AVAIL_OVERWRITABLE,
  /* Function body/variable initializer is known and will be used in final
     program.  */
  AVAIL_AVAILABLE,
  /* Function body/variable initializer is known and all it's uses are explicitly
     visible within current unit (ie it's address is never taken and it is not
     exported to other units).
     Currently used only for functions.  */
  AVAIL_LOCAL
};

/* This is the information that is put into the cgraph local structure
   to recover a function.  */
struct lto_file_decl_data;

extern const char * const cgraph_availability_names[];

/* Function inlining information.  */

struct GTY(()) inline_summary
{
  /* Estimated stack frame consumption by the function.  */
  HOST_WIDE_INT estimated_self_stack_size;

  /* Size of the function body.  */
  int self_size;
  /* How many instructions are likely going to disappear after inlining.  */
  int size_inlining_benefit;
  /* Estimated time spent executing the function body.  */
  int self_time;
  /* How much time is going to be saved by inlining.  */
  int time_inlining_benefit;
};

/* Information about thunk, used only for same body aliases.  */

struct GTY(()) cgraph_thunk_info {
  /* Information about the thunk.  */
  HOST_WIDE_INT fixed_offset;
  HOST_WIDE_INT virtual_value;
  tree alias;
  bool this_adjusting;
  bool virtual_offset_p;
  /* Set to true when alias node is thunk.  */
  bool thunk_p;
};

/* Information about the function collected locally.
   Available after function is analyzed.  */

struct GTY(()) cgraph_local_info {
  /* File stream where this node is being written to.  */
  struct lto_file_decl_data * GTY ((skip)) lto_file_data;

  struct inline_summary inline_summary;

  /* Set when function function is visible in current compilation unit only
     and its address is never taken.  */
  unsigned local : 1;

  /* Set when function is visible by other units.  */
  unsigned externally_visible : 1;

  /* Set once it has been finalized so we consider it to be output.  */
  unsigned finalized : 1;

  /* False when there something makes inlining impossible (such as va_arg).  */
  unsigned inlinable : 1;

  /* True when function should be inlined independently on its size.  */
  unsigned disregard_inline_limits : 1;

  /* True when the function has been originally extern inline, but it is
     redefined now.  */
  unsigned redefined_extern_inline : 1;

  /* True if statics_read_for_function and
     statics_written_for_function contain valid data.  */
  unsigned for_functions_valid : 1;

  /* True if the function is going to be emitted in some other translation
     unit, referenced from vtable.  */
  unsigned vtable_method : 1;
};

/* Information about the function that needs to be computed globally
   once compilation is finished.  Available only with -funit-at-a-time.  */

struct GTY(()) cgraph_global_info {
  /* Estimated stack frame consumption by the function.  */
  HOST_WIDE_INT estimated_stack_size;
  /* Expected offset of the stack frame of inlined function.  */
  HOST_WIDE_INT stack_frame_offset;

  /* For inline clones this points to the function they will be
     inlined into.  */
  struct cgraph_node *inlined_to;

  /* Estimated size of the function after inlining.  */
  int time;
  int size;

  /* Estimated growth after inlining.  INT_MIN if not computed.  */
  int estimated_growth;

  /* Set iff the function has been inlined at least once.  */
  bool inlined;
};

/* Information about the function that is propagated by the RTL backend.
   Available only for functions that has been already assembled.  */

struct GTY(()) cgraph_rtl_info {
   unsigned int preferred_incoming_stack_boundary;
};

/* Represent which DECL tree (or reference to such tree)
   will be replaced by another tree while versioning.  */
struct GTY(()) ipa_replace_map
{
  /* The tree that will be replaced.  */
  tree old_tree;
  /* The new (replacing) tree.  */
  tree new_tree;
  /* True when a substitution should be done, false otherwise.  */
  bool replace_p;
  /* True when we replace a reference to old_tree.  */
  bool ref_p;
};
typedef struct ipa_replace_map *ipa_replace_map_p;
DEF_VEC_P(ipa_replace_map_p);
DEF_VEC_ALLOC_P(ipa_replace_map_p,gc);

struct GTY(()) cgraph_clone_info
{
  VEC(ipa_replace_map_p,gc)* tree_map;
  bitmap args_to_skip;
  bitmap combined_args_to_skip;
};

/* The cgraph data structure.
   Each function decl has assigned cgraph_node listing callees and callers.  */

struct GTY((chain_next ("%h.next"), chain_prev ("%h.previous"))) cgraph_node {
  tree decl;
  struct cgraph_edge *callees;
  struct cgraph_edge *callers;
  struct cgraph_node *next;
  struct cgraph_node *previous;
  /* For nested functions points to function the node is nested in.  */
  struct cgraph_node *origin;
  /* Points to first nested function, if any.  */
  struct cgraph_node *nested;
  /* Pointer to the next function with same origin, if any.  */
  struct cgraph_node *next_nested;
  /* Pointer to the next function in cgraph_nodes_queue.  */
  struct cgraph_node *next_needed;
  /* Pointer to the next clone.  */
  struct cgraph_node *next_sibling_clone;
  struct cgraph_node *prev_sibling_clone;
  struct cgraph_node *clones;
  struct cgraph_node *clone_of;
  /* For normal nodes pointer to the list of alias and thunk nodes,
     in alias/thunk nodes pointer to the normal node.  */
  struct cgraph_node *same_body;
  /* Circular list of nodes in the same comdat group if non-NULL.  */
  struct cgraph_node *same_comdat_group;
  /* For functions with many calls sites it holds map from call expression
     to the edge to speed up cgraph_edge function.  */
  htab_t GTY((param_is (struct cgraph_edge))) call_site_hash;

  PTR GTY ((skip)) aux;

  /* Interprocedural passes scheduled to have their transform functions
     applied next time we execute local pass on them.  We maintain it
     per-function in order to allow IPA passes to introduce new functions.  */
  VEC(ipa_opt_pass,heap) * GTY((skip)) ipa_transforms_to_apply;

  struct cgraph_local_info local;
  struct cgraph_global_info global;
  struct cgraph_rtl_info rtl;
  struct cgraph_clone_info clone;
  struct cgraph_thunk_info thunk;

  /* Expected number of executions: calculated in profile.c.  */
  gcov_type count;
  /* Unique id of the node.  */
  int uid;
  /* Ordering of all cgraph nodes.  */
  int order;

  /* unique id for profiling. pid is not suitable because of different
     number of cfg nodes with -fprofile-generate and -fprofile-use */
  int pid;

  /* Set when function must be output for some reason.  The primary
     use of this flag is to mark functions needed to be output for
     non-standard reason.  Functions that are externally visible
     or reachable from functions needed to be output are marked
     by specialized flags.  */
  unsigned needed : 1;
  /* Set when function has address taken.
     In current implementation it imply needed flag. */
  unsigned address_taken : 1;
  /* Set when decl is an abstract function pointed to by the
     ABSTRACT_DECL_ORIGIN of a reachable function.  */
  unsigned abstract_and_needed : 1;
  /* Set when function is reachable by call from other function
     that is either reachable or needed.
     This flag is computed at original cgraph construction and then
     updated in cgraph_remove_unreachable_nodes.  Note that after
     cgraph_remove_unreachable_nodes cgraph still can contain unreachable
     nodes when they are needed for virtual clone instantiation.  */
  unsigned reachable : 1;
  /* Set once the function is lowered (i.e. its CFG is built).  */
  unsigned lowered : 1;
  /* Set once the function has been instantiated and its callee
     lists created.  */
  unsigned analyzed : 1;
  /* Set when function is scheduled to be processed by local passes.  */
  unsigned process : 1;
  /* Set for aliases once they got through assemble_alias.  */
  unsigned alias : 1;
  /* Set if the function is a nested function or has nested functions.  */
  unsigned ever_was_nested : 1;
  /* Set for nodes that was constructed and finalized by frontend.  */
  unsigned finalized_by_frontend : 1;
  /* Set for alias and thunk nodes, same_body points to the node they are alias
     of and they are linked through the next/previous pointers.  */
  unsigned same_body_alias : 1;
};

typedef struct cgraph_node *cgraph_node_ptr;

DEF_VEC_P(cgraph_node_ptr);
DEF_VEC_ALLOC_P(cgraph_node_ptr,heap);
DEF_VEC_ALLOC_P(cgraph_node_ptr,gc);

/* A cgraph node set is a collection of cgraph nodes.  A cgraph node
   can appear in multiple sets.  */
struct GTY(()) cgraph_node_set_def
{
  htab_t GTY((param_is (struct cgraph_node_set_element_def))) hashtab;
  VEC(cgraph_node_ptr, gc) *nodes;
  PTR GTY ((skip)) aux;
};

typedef struct cgraph_node_set_def *cgraph_node_set;

DEF_VEC_P(cgraph_node_set);
DEF_VEC_ALLOC_P(cgraph_node_set,gc);
DEF_VEC_ALLOC_P(cgraph_node_set,heap);

/* A cgraph node set element contains an index in the vector of nodes in
   the set.  */
struct GTY(()) cgraph_node_set_element_def
{
  struct cgraph_node *node;
  HOST_WIDE_INT index;
};

typedef struct cgraph_node_set_element_def *cgraph_node_set_element;
typedef const struct cgraph_node_set_element_def *const_cgraph_node_set_element;

/* Iterator structure for cgraph node sets.  */
typedef struct
{
  cgraph_node_set set;
  unsigned index;
} cgraph_node_set_iterator;

#define DEFCIFCODE(code, string)	CIF_ ## code,
/* Reasons for inlining failures.  */
typedef enum {
#include "cif-code.def"
  CIF_N_REASONS
} cgraph_inline_failed_t;

struct GTY((chain_next ("%h.next_caller"), chain_prev ("%h.prev_caller"))) cgraph_edge {
  /* Expected number of executions: calculated in profile.c.  */
  gcov_type count;
  struct cgraph_node *caller;
  struct cgraph_node *callee;
  struct cgraph_edge *prev_caller;
  struct cgraph_edge *next_caller;
  struct cgraph_edge *prev_callee;
  struct cgraph_edge *next_callee;
  gimple call_stmt;
  PTR GTY ((skip (""))) aux;
  /* When equal to CIF_OK, inline this call.  Otherwise, points to the
     explanation why function was not inlined.  */
  cgraph_inline_failed_t inline_failed;
  /* The stmt_uid of call_stmt.  This is used by LTO to recover the call_stmt
     when the function is serialized in.  */
  unsigned int lto_stmt_uid;
  /* Expected frequency of executions within the function.
     When set to CGRAPH_FREQ_BASE, the edge is expected to be called once
     per function call.  The range is 0 to CGRAPH_FREQ_MAX.  */
  int frequency;
  /* Unique id of the edge.  */
  int uid;
  /* Depth of loop nest, 1 means no loop nest.  */
  unsigned short int loop_nest;
  /* Whether this edge describes a call that was originally indirect.  */
  unsigned int indirect_call : 1;
  /* True if the corresponding CALL stmt cannot be inlined.  */
  unsigned int call_stmt_cannot_inline_p : 1;
  /* Can this call throw externally?  */
  unsigned int can_throw_external : 1;
};

#define CGRAPH_FREQ_BASE 1000
#define CGRAPH_FREQ_MAX 100000

typedef struct cgraph_edge *cgraph_edge_p;

DEF_VEC_P(cgraph_edge_p);
DEF_VEC_ALLOC_P(cgraph_edge_p,heap);

/* The varpool data structure.
   Each static variable decl has assigned varpool_node.  */

struct GTY((chain_next ("%h.next"))) varpool_node {
  tree decl;
  /* Pointer to the next function in varpool_nodes.  */
  struct varpool_node *next;
  /* Pointer to the next function in varpool_nodes_queue.  */
  struct varpool_node *next_needed;
  /* For normal nodes a pointer to the first extra name alias.  For alias
     nodes a pointer to the normal node.  */
  struct varpool_node *extra_name;
  /* Ordering of all cgraph nodes.  */
  int order;

  /* Set when function must be output - it is externally visible
     or its address is taken.  */
  unsigned needed : 1;
  /* Needed variables might become dead by optimization.  This flag
     forces the variable to be output even if it appears dead otherwise.  */
  unsigned force_output : 1;
  /* Set once the variable has been instantiated and its callee
     lists created.  */
  unsigned analyzed : 1;
  /* Set once it has been finalized so we consider it to be output.  */
  unsigned finalized : 1;
  /* Set when variable is scheduled to be assembled.  */
  unsigned output : 1;
  /* Set when function is visible by other units.  */
  unsigned externally_visible : 1;
  /* Set for aliases once they got through assemble_alias.  Also set for
     extra name aliases in varpool_extra_name_alias.  */
  unsigned alias : 1;
};

/* Every top level asm statement is put into a cgraph_asm_node.  */

struct GTY(()) cgraph_asm_node {
  /* Next asm node.  */
  struct cgraph_asm_node *next;
  /* String for this asm node.  */
  tree asm_str;
  /* Ordering of all cgraph nodes.  */
  int order;
};

extern GTY(()) struct cgraph_node *cgraph_nodes;
extern GTY(()) int cgraph_n_nodes;
extern GTY(()) int cgraph_max_uid;
extern GTY(()) int cgraph_edge_max_uid;
extern GTY(()) int cgraph_max_pid;
extern bool cgraph_global_info_ready;
enum cgraph_state
{
  /* Callgraph is being constructed.  It is safe to add new functions.  */
  CGRAPH_STATE_CONSTRUCTION,
  /* Callgraph is built and IPA passes are being run.  */
  CGRAPH_STATE_IPA,
  /* Callgraph is built and all functions are transformed to SSA form.  */
  CGRAPH_STATE_IPA_SSA,
  /* Functions are now ordered and being passed to RTL expanders.  */
  CGRAPH_STATE_EXPANSION,
  /* All cgraph expansion is done.  */
  CGRAPH_STATE_FINISHED
};
extern enum cgraph_state cgraph_state;
extern bool cgraph_function_flags_ready;
extern GTY(()) struct cgraph_node *cgraph_nodes_queue;
extern GTY(()) struct cgraph_node *cgraph_new_nodes;

extern GTY(()) struct cgraph_asm_node *cgraph_asm_nodes;
extern GTY(()) int cgraph_order;

/* In cgraph.c  */
void dump_cgraph (FILE *);
void debug_cgraph (void);
void dump_cgraph_node (FILE *, struct cgraph_node *);
void debug_cgraph_node (struct cgraph_node *);
void cgraph_insert_node_to_hashtable (struct cgraph_node *node);
void cgraph_remove_edge (struct cgraph_edge *);
void cgraph_remove_node (struct cgraph_node *);
void cgraph_remove_node_and_inline_clones (struct cgraph_node *);
void cgraph_release_function_body (struct cgraph_node *);
void cgraph_node_remove_callees (struct cgraph_node *node);
struct cgraph_edge *cgraph_create_edge (struct cgraph_node *,
					struct cgraph_node *,
					gimple, gcov_type, int, int);

struct cgraph_node * cgraph_get_node (tree);
struct cgraph_node *cgraph_node (tree);
bool cgraph_same_body_alias (tree, tree);
void cgraph_add_thunk (tree, tree, bool, HOST_WIDE_INT, HOST_WIDE_INT, tree, tree);
void cgraph_remove_same_body_alias (struct cgraph_node *);
struct cgraph_node *cgraph_node_for_asm (tree);
struct cgraph_edge *cgraph_edge (struct cgraph_node *, gimple);
void cgraph_set_call_stmt (struct cgraph_edge *, gimple);
void cgraph_set_call_stmt_including_clones (struct cgraph_node *, gimple, gimple);
void cgraph_create_edge_including_clones (struct cgraph_node *,
					  struct cgraph_node *,
					  gimple, gimple, gcov_type, int, int,
					  cgraph_inline_failed_t);
void cgraph_update_edges_for_call_stmt (gimple, tree, gimple);
struct cgraph_local_info *cgraph_local_info (tree);
struct cgraph_global_info *cgraph_global_info (tree);
struct cgraph_rtl_info *cgraph_rtl_info (tree);
const char * cgraph_node_name (struct cgraph_node *);
struct cgraph_edge * cgraph_clone_edge (struct cgraph_edge *,
					struct cgraph_node *, gimple,
					unsigned, gcov_type, int, int, bool);
struct cgraph_node * cgraph_clone_node (struct cgraph_node *, gcov_type, int,
					int, bool, VEC(cgraph_edge_p,heap) *);

void cgraph_redirect_edge_callee (struct cgraph_edge *, struct cgraph_node *);

struct cgraph_asm_node *cgraph_add_asm_node (tree);

bool cgraph_function_possibly_inlined_p (tree);
void cgraph_unnest_node (struct cgraph_node *);

enum availability cgraph_function_body_availability (struct cgraph_node *);
void cgraph_add_new_function (tree, bool);
const char* cgraph_inline_failed_string (cgraph_inline_failed_t);
struct cgraph_node * cgraph_create_virtual_clone (struct cgraph_node *old_node,
			                          VEC(cgraph_edge_p,heap)*,
			                          VEC(ipa_replace_map_p,gc)* tree_map,
			                          bitmap args_to_skip);

void cgraph_set_nothrow_flag (struct cgraph_node *, bool);
void cgraph_set_readonly_flag (struct cgraph_node *, bool);
void cgraph_set_pure_flag (struct cgraph_node *, bool);
void cgraph_set_looping_const_or_pure_flag (struct cgraph_node *, bool);
tree clone_function_name (tree);

/* In cgraphunit.c  */
void cgraph_finalize_function (tree, bool);
void cgraph_mark_if_needed (tree);
void cgraph_finalize_compilation_unit (void);
void cgraph_optimize (void);
void cgraph_mark_needed_node (struct cgraph_node *);
void cgraph_mark_address_taken_node (struct cgraph_node *);
void cgraph_mark_reachable_node (struct cgraph_node *);
bool cgraph_inline_p (struct cgraph_edge *, cgraph_inline_failed_t *reason);
bool cgraph_preserve_function_body_p (tree);
void verify_cgraph (void);
void verify_cgraph_node (struct cgraph_node *);
void cgraph_build_static_cdtor (char which, tree body, int priority);
void cgraph_reset_static_var_maps (void);
void init_cgraph (void);
struct cgraph_node *cgraph_function_versioning (struct cgraph_node *,
						VEC(cgraph_edge_p,heap)*,
						VEC(ipa_replace_map_p,gc)*,
						bitmap);
void tree_function_versioning (tree, tree, VEC (ipa_replace_map_p,gc)*, bool, bitmap);
struct cgraph_node *save_inline_function_body (struct cgraph_node *);
void record_references_in_initializer (tree, bool);
bool cgraph_process_new_functions (void);

bool cgraph_decide_is_function_needed (struct cgraph_node *, tree);

typedef void (*cgraph_edge_hook)(struct cgraph_edge *, void *);
typedef void (*cgraph_node_hook)(struct cgraph_node *, void *);
typedef void (*cgraph_2edge_hook)(struct cgraph_edge *, struct cgraph_edge *,
				  void *);
typedef void (*cgraph_2node_hook)(struct cgraph_node *, struct cgraph_node *,
				  void *);
struct cgraph_edge_hook_list;
struct cgraph_node_hook_list;
struct cgraph_2edge_hook_list;
struct cgraph_2node_hook_list;
struct cgraph_edge_hook_list *cgraph_add_edge_removal_hook (cgraph_edge_hook, void *);
void cgraph_remove_edge_removal_hook (struct cgraph_edge_hook_list *);
struct cgraph_node_hook_list *cgraph_add_node_removal_hook (cgraph_node_hook,
							    void *);
void cgraph_remove_node_removal_hook (struct cgraph_node_hook_list *);
struct cgraph_node_hook_list *cgraph_add_function_insertion_hook (cgraph_node_hook,
							          void *);
void cgraph_remove_function_insertion_hook (struct cgraph_node_hook_list *);
void cgraph_call_function_insertion_hooks (struct cgraph_node *node);
struct cgraph_2edge_hook_list *cgraph_add_edge_duplication_hook (cgraph_2edge_hook, void *);
void cgraph_remove_edge_duplication_hook (struct cgraph_2edge_hook_list *);
struct cgraph_2node_hook_list *cgraph_add_node_duplication_hook (cgraph_2node_hook, void *);
void cgraph_remove_node_duplication_hook (struct cgraph_2node_hook_list *);
void cgraph_materialize_all_clones (void);
gimple cgraph_redirect_edge_call_stmt_to_callee (struct cgraph_edge *);
/* In cgraphbuild.c  */
unsigned int rebuild_cgraph_edges (void);
void reset_inline_failed (struct cgraph_node *);
int compute_call_stmt_bb_frequency (tree, basic_block bb);

/* In ipa.c  */
bool cgraph_remove_unreachable_nodes (bool, FILE *);
int cgraph_postorder (struct cgraph_node **);
cgraph_node_set cgraph_node_set_new (void);
cgraph_node_set_iterator cgraph_node_set_find (cgraph_node_set,
					       struct cgraph_node *);
void cgraph_node_set_add (cgraph_node_set, struct cgraph_node *);
void cgraph_node_set_remove (cgraph_node_set, struct cgraph_node *);
void dump_cgraph_node_set (FILE *, cgraph_node_set);
void debug_cgraph_node_set (cgraph_node_set);


/* In predict.c  */
bool cgraph_maybe_hot_edge_p (struct cgraph_edge *e);

/* In varpool.c  */
extern GTY(()) struct varpool_node *varpool_nodes_queue;
extern GTY(()) struct varpool_node *varpool_nodes;

struct varpool_node *varpool_node (tree);
struct varpool_node *varpool_node_for_asm (tree asmname);
void varpool_mark_needed_node (struct varpool_node *);
void debug_varpool (void);
void dump_varpool (FILE *);
void dump_varpool_node (FILE *, struct varpool_node *);

void varpool_finalize_decl (tree);
bool decide_is_variable_needed (struct varpool_node *, tree);
enum availability cgraph_variable_initializer_availability (struct varpool_node *);
void cgraph_make_decl_local (tree);
void cgraph_make_node_local (struct cgraph_node *);
bool cgraph_node_can_be_local_p (struct cgraph_node *);

bool varpool_assemble_pending_decls (void);
bool varpool_assemble_decl (struct varpool_node *node);
bool varpool_analyze_pending_decls (void);
void varpool_remove_unreferenced_decls (void);
void varpool_empty_needed_queue (void);
bool varpool_extra_name_alias (tree, tree);
const char * varpool_node_name (struct varpool_node *node);

/* Walk all reachable static variables.  */
#define FOR_EACH_STATIC_VARIABLE(node) \
   for ((node) = varpool_nodes_queue; (node); (node) = (node)->next_needed)

/* Return first reachable static variable with initializer.  */
static inline struct varpool_node *
varpool_first_static_initializer (void)
{
  struct varpool_node *node;
  for (node = varpool_nodes_queue; node; node = node->next_needed)
    {
      gcc_assert (TREE_CODE (node->decl) == VAR_DECL);
      if (DECL_INITIAL (node->decl))
	return node;
    }
  return NULL;
}

/* Return next reachable static variable with initializer after NODE.  */
static inline struct varpool_node *
varpool_next_static_initializer (struct varpool_node *node)
{
  for (node = node->next_needed; node; node = node->next_needed)
    {
      gcc_assert (TREE_CODE (node->decl) == VAR_DECL);
      if (DECL_INITIAL (node->decl))
	return node;
    }
  return NULL;
}

/* Walk all static variables with initializer set.  */
#define FOR_EACH_STATIC_INITIALIZER(node) \
   for ((node) = varpool_first_static_initializer (); (node); \
        (node) = varpool_next_static_initializer (node))

/* In ipa-inline.c  */
void cgraph_clone_inlined_nodes (struct cgraph_edge *, bool, bool);
unsigned int compute_inline_parameters (struct cgraph_node *);


/* Create a new static variable of type TYPE.  */
tree add_new_static_var (tree type);

/* lto-cgraph.c */

enum LTO_cgraph_tags
{
  /* Must leave 0 for the stopper.  */
  LTO_cgraph_avail_node = 1,
  LTO_cgraph_overwritable_node,
  LTO_cgraph_unavail_node,
  LTO_cgraph_edge,
  LTO_cgraph_last_tag
};

extern const char * LTO_cgraph_tag_names[LTO_cgraph_last_tag];

#define LCC_NOT_FOUND	(-1)


/* Return true if iterator CSI points to nothing.  */
static inline bool
csi_end_p (cgraph_node_set_iterator csi)
{
  return csi.index >= VEC_length (cgraph_node_ptr, csi.set->nodes);
}

/* Advance iterator CSI.  */
static inline void
csi_next (cgraph_node_set_iterator *csi)
{
  csi->index++;
}

/* Return the node pointed to by CSI.  */
static inline struct cgraph_node *
csi_node (cgraph_node_set_iterator csi)
{
  return VEC_index (cgraph_node_ptr, csi.set->nodes, csi.index);
}

/* Return an iterator to the first node in SET.  */
static inline cgraph_node_set_iterator
csi_start (cgraph_node_set set)
{
  cgraph_node_set_iterator csi;

  csi.set = set;
  csi.index = 0;
  return csi;
}

/* Return true if SET contains NODE.  */
static inline bool
cgraph_node_in_set_p (struct cgraph_node *node, cgraph_node_set set)
{
  cgraph_node_set_iterator csi;
  csi = cgraph_node_set_find (set, node);
  return !csi_end_p (csi);
}

/* Return number of nodes in SET.  */
static inline size_t
cgraph_node_set_size (cgraph_node_set set)
{
  return htab_elements (set->hashtab);
}

/* Uniquize all constants that appear in memory.
   Each constant in memory thus far output is recorded
   in `const_desc_table'.  */

struct GTY(()) constant_descriptor_tree {
  /* A MEM for the constant.  */
  rtx rtl;

  /* The value of the constant.  */
  tree value;

  /* Hash of value.  Computing the hash from value each time
     hashfn is called can't work properly, as that means recursive
     use of the hash table during hash table expansion.  */
  hashval_t hash;
};

/* Return true when function NODE is only called directly.
   i.e. it is not externally visible, address was not taken and
   it is not used in any other non-standard way.  */

static inline bool
cgraph_only_called_directly_p (struct cgraph_node *node)
{
  return !node->needed && !node->local.externally_visible;
}

/* Return true when function NODE can be removed from callgraph
   if all direct calls are eliminated.  */

static inline bool
cgraph_can_remove_if_no_direct_calls_p (struct cgraph_node *node)
{
  return (!node->needed
  	  && (DECL_COMDAT (node->decl) || !node->local.externally_visible));
}

/* Constant pool accessor function.  */
htab_t constant_pool_htab (void);

#endif  /* GCC_CGRAPH_H  */
