/* Data and Control Flow Analysis for Trees.
   Copyright (C) 2001, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010
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

#ifndef _TREE_FLOW_H
#define _TREE_FLOW_H 1

#include "bitmap.h"
#include "hard-reg-set.h"
#include "basic-block.h"
#include "hashtab.h"
#include "gimple.h"
#include "tree-ssa-operands.h"
#include "cgraph.h"
#include "ipa-reference.h"
#include "tree-ssa-alias.h"


/* Gimple dataflow datastructure. All publicly available fields shall have
   gimple_ accessor defined in tree-flow-inline.h, all publicly modifiable
   fields should have gimple_set accessor.  */
struct GTY(()) gimple_df {
  /* Array of all variables referenced in the function.  */
  htab_t GTY((param_is (union tree_node))) referenced_vars;

  /* A vector of all the noreturn calls passed to modify_stmt.
     cleanup_control_flow uses it to detect cases where a mid-block
     indirect call has been turned into a noreturn call.  When this
     happens, all the instructions after the call are no longer
     reachable and must be deleted as dead.  */
  VEC(gimple,gc) *modified_noreturn_calls;

  /* Array of all SSA_NAMEs used in the function.  */
  VEC(tree,gc) *ssa_names;

  /* Artificial variable used for the virtual operand FUD chain.  */
  tree vop;

  /* The PTA solution for the ESCAPED artificial variable.  */
  struct pt_solution escaped;

  /* The PTA solution for the CALLUSED artificial variable.  */
  struct pt_solution callused;

  /* A map of decls to artificial ssa-names that point to the partition
     of the decl.  */
  struct pointer_map_t * GTY((skip(""))) decls_to_pointers;

  /* Free list of SSA_NAMEs.  */
  tree free_ssanames;

  /* Hashtable holding definition for symbol.  If this field is not NULL, it
     means that the first reference to this variable in the function is a
     USE or a VUSE.  In those cases, the SSA renamer creates an SSA name
     for this variable with an empty defining statement.  */
  htab_t GTY((param_is (union tree_node))) default_defs;

  /* Symbols whose SSA form needs to be updated or created for the first
     time.  */
  bitmap syms_to_rename;

  /* True if the code is in ssa form.  */
  unsigned int in_ssa_p : 1;

  struct ssa_operands ssa_operands;
};

/* Accessors for internal use only.  Generic code should use abstraction
   provided by tree-flow-inline.h or specific modules.  */
#define FREE_SSANAMES(fun) (fun)->gimple_df->free_ssanames
#define SSANAMES(fun) (fun)->gimple_df->ssa_names
#define MODIFIED_NORETURN_CALLS(fun) (fun)->gimple_df->modified_noreturn_calls
#define DEFAULT_DEFS(fun) (fun)->gimple_df->default_defs
#define SYMS_TO_RENAME(fun) (fun)->gimple_df->syms_to_rename

typedef struct
{
  htab_t htab;
  PTR *slot;
  PTR *limit;
} htab_iterator;

/* Iterate through the elements of hashtable HTAB, using htab_iterator ITER,
   storing each element in RESULT, which is of type TYPE.  */
#define FOR_EACH_HTAB_ELEMENT(HTAB, RESULT, TYPE, ITER) \
  for (RESULT = (TYPE) first_htab_element (&(ITER), (HTAB)); \
	!end_htab_p (&(ITER)); \
	RESULT = (TYPE) next_htab_element (&(ITER)))

/*---------------------------------------------------------------------------
		      Attributes for SSA_NAMEs.

  NOTE: These structures are stored in struct tree_ssa_name
  but are only used by the tree optimizers, so it makes better sense
  to declare them here to avoid recompiling unrelated files when
  making changes.
---------------------------------------------------------------------------*/

/* Aliasing information for SSA_NAMEs representing pointer variables.  */
struct GTY(()) ptr_info_def
{
  /* The points-to solution, TBAA-pruned if the pointer is dereferenced.  */
  struct pt_solution pt;
};


/* It is advantageous to avoid things like life analysis for variables which
   do not need PHI nodes.  This enum describes whether or not a particular
   variable may need a PHI node.  */

enum need_phi_state {
  /* This is the default.  If we are still in this state after finding
     all the definition and use sites, then we will assume the variable
     needs PHI nodes.  This is probably an overly conservative assumption.  */
  NEED_PHI_STATE_UNKNOWN,

  /* This state indicates that we have seen one or more sets of the
     variable in a single basic block and that the sets dominate all
     uses seen so far.  If after finding all definition and use sites
     we are still in this state, then the variable does not need any
     PHI nodes.  */
  NEED_PHI_STATE_NO,

  /* This state indicates that we have either seen multiple definitions of
     the variable in multiple blocks, or that we encountered a use in a
     block that was not dominated by the block containing the set(s) of
     this variable.  This variable is assumed to need PHI nodes.  */
  NEED_PHI_STATE_MAYBE
};


/* The "no alias" attribute allows alias analysis to make more
   aggressive assumptions when assigning alias sets, computing
   points-to information and memory partitions.  These attributes
   are the result of user annotations or flags (e.g.,
   -fargument-noalias).  */
enum noalias_state {
    /* Default state.  No special assumptions can be made about this
       symbol.  */
    MAY_ALIAS = 0,

    /* The symbol does not alias with other symbols that have a
       NO_ALIAS* attribute.  */
    NO_ALIAS,

    /* The symbol does not alias with other symbols that have a
       NO_ALIAS*, and it may not alias with global symbols.  */
    NO_ALIAS_GLOBAL,

    /* The symbol does not alias with any other symbols.  */
    NO_ALIAS_ANYTHING
};


struct GTY(()) var_ann_d {
  /* Used when building base variable structures in a var_map.  */
  unsigned base_var_processed : 1;

  /* Nonzero if this variable was used after SSA optimizations were
     applied.  We set this when translating out of SSA form.  */
  unsigned used : 1;

  /* This field indicates whether or not the variable may need PHI nodes.
     See the enum's definition for more detailed information about the
     states.  */
  ENUM_BITFIELD (need_phi_state) need_phi_state : 2;

  /* True for HEAP artificial variables.  These variables represent
     the memory area allocated by a call to malloc.  */
  unsigned is_heapvar : 1;

  /* This field describes several "no alias" attributes that some
     symbols are known to have.  See the enum's definition for more
     information on each attribute.  */
  ENUM_BITFIELD (noalias_state) noalias_state : 2;

  /* Used by var_map for the base index of ssa base variables.  */
  unsigned base_index;

  /* During into-ssa and the dominator optimizer, this field holds the
     current version of this variable (an SSA_NAME).  */
  tree current_def;
};


/* Immediate use lists are used to directly access all uses for an SSA
   name and get pointers to the statement for each use.

   The structure ssa_use_operand_d consists of PREV and NEXT pointers
   to maintain the list.  A USE pointer, which points to address where
   the use is located and a LOC pointer which can point to the
   statement where the use is located, or, in the case of the root
   node, it points to the SSA name itself.

   The list is anchored by an occurrence of ssa_operand_d *in* the
   ssa_name node itself (named 'imm_uses').  This node is uniquely
   identified by having a NULL USE pointer. and the LOC pointer
   pointing back to the ssa_name node itself.  This node forms the
   base for a circular list, and initially this is the only node in
   the list.

   Fast iteration allows each use to be examined, but does not allow
   any modifications to the uses or stmts.

   Normal iteration allows insertion, deletion, and modification. the
   iterator manages this by inserting a marker node into the list
   immediately before the node currently being examined in the list.
   this marker node is uniquely identified by having null stmt *and* a
   null use pointer.

   When iterating to the next use, the iteration routines check to see
   if the node after the marker has changed. if it has, then the node
   following the marker is now the next one to be visited. if not, the
   marker node is moved past that node in the list (visualize it as
   bumping the marker node through the list).  this continues until
   the marker node is moved to the original anchor position. the
   marker node is then removed from the list.

   If iteration is halted early, the marker node must be removed from
   the list before continuing.  */
typedef struct immediate_use_iterator_d
{
  /* This is the current use the iterator is processing.  */
  ssa_use_operand_t *imm_use;
  /* This marks the last use in the list (use node from SSA_NAME)  */
  ssa_use_operand_t *end_p;
  /* This node is inserted and used to mark the end of the uses for a stmt.  */
  ssa_use_operand_t iter_node;
  /* This is the next ssa_name to visit.  IMM_USE may get removed before
     the next one is traversed to, so it must be cached early.  */
  ssa_use_operand_t *next_imm_name;
} imm_use_iterator;


/* Use this iterator when simply looking at stmts.  Adding, deleting or
   modifying stmts will cause this iterator to malfunction.  */

#define FOR_EACH_IMM_USE_FAST(DEST, ITER, SSAVAR)			\
  for ((DEST) = first_readonly_imm_use (&(ITER), (SSAVAR));	\
       !end_readonly_imm_use_p (&(ITER));			\
       (DEST) = next_readonly_imm_use (&(ITER)))

/* Use this iterator to visit each stmt which has a use of SSAVAR.  */

#define FOR_EACH_IMM_USE_STMT(STMT, ITER, SSAVAR)		\
  for ((STMT) = first_imm_use_stmt (&(ITER), (SSAVAR));		\
       !end_imm_use_stmt_p (&(ITER));				\
       (STMT) = next_imm_use_stmt (&(ITER)))

/* Use this to terminate the FOR_EACH_IMM_USE_STMT loop early.  Failure to
   do so will result in leaving a iterator marker node in the immediate
   use list, and nothing good will come from that.   */
#define BREAK_FROM_IMM_USE_STMT(ITER)				\
   {								\
     end_imm_use_stmt_traverse (&(ITER));			\
     break;							\
   }


/* Use this iterator in combination with FOR_EACH_IMM_USE_STMT to
   get access to each occurrence of ssavar on the stmt returned by
   that iterator..  for instance:

     FOR_EACH_IMM_USE_STMT (stmt, iter, var)
       {
         FOR_EACH_IMM_USE_ON_STMT (use_p, iter)
	   {
	     SET_USE (use_p, blah);
	   }
	 update_stmt (stmt);
       }							 */

#define FOR_EACH_IMM_USE_ON_STMT(DEST, ITER)			\
  for ((DEST) = first_imm_use_on_stmt (&(ITER));		\
       !end_imm_use_on_stmt_p (&(ITER));			\
       (DEST) = next_imm_use_on_stmt (&(ITER)))



typedef struct var_ann_d *var_ann_t;

static inline var_ann_t var_ann (const_tree);
static inline var_ann_t get_var_ann (tree);
static inline void update_stmt (gimple);
static inline int get_lineno (const_gimple);

/*---------------------------------------------------------------------------
                  Structure representing predictions in tree level.
---------------------------------------------------------------------------*/
struct GTY((chain_next ("%h.ep_next"))) edge_prediction {
  struct edge_prediction *ep_next;
  edge ep_edge;
  enum br_predictor ep_predictor;
  int ep_probability;
};

/* Accessors for basic block annotations.  */
static inline gimple_seq phi_nodes (const_basic_block);
static inline void set_phi_nodes (basic_block, gimple_seq);

/*---------------------------------------------------------------------------
			      Global declarations
---------------------------------------------------------------------------*/
struct GTY(()) int_tree_map {

  unsigned int uid;
  tree to;
};

extern unsigned int int_tree_map_hash (const void *);
extern int int_tree_map_eq (const void *, const void *);

extern unsigned int uid_decl_map_hash (const void *);
extern int uid_decl_map_eq (const void *, const void *);

typedef struct
{
  htab_iterator hti;
} referenced_var_iterator;

/* This macro loops over all the referenced vars, one at a time, putting the
   current var in VAR.  Note:  You are not allowed to add referenced variables
   to the hashtable while using this macro.  Doing so may cause it to behave
   erratically.  */

#define FOR_EACH_REFERENCED_VAR(VAR, ITER) \
  for ((VAR) = first_referenced_var (&(ITER)); \
       !end_referenced_vars_p (&(ITER)); \
       (VAR) = next_referenced_var (&(ITER)))

extern tree referenced_var_lookup (unsigned int);
extern bool referenced_var_check_and_insert (tree);
#define num_referenced_vars htab_elements (gimple_referenced_vars (cfun))
#define referenced_var(i) referenced_var_lookup (i)

#define num_ssa_names (VEC_length (tree, cfun->gimple_df->ssa_names))
#define ssa_name(i) (VEC_index (tree, cfun->gimple_df->ssa_names, (i)))

/* Macros for showing usage statistics.  */
#define SCALE(x) ((unsigned long) ((x) < 1024*10	\
		  ? (x)					\
		  : ((x) < 1024*1024*10			\
		     ? (x) / 1024			\
		     : (x) / (1024*1024))))

#define LABEL(x) ((x) < 1024*10 ? 'b' : ((x) < 1024*1024*10 ? 'k' : 'M'))

#define PERCENT(x,y) ((float)(x) * 100.0 / (float)(y))

/*---------------------------------------------------------------------------
			      OpenMP Region Tree
---------------------------------------------------------------------------*/

/* Parallel region information.  Every parallel and workshare
   directive is enclosed between two markers, the OMP_* directive
   and a corresponding OMP_RETURN statement.  */

struct omp_region
{
  /* The enclosing region.  */
  struct omp_region *outer;

  /* First child region.  */
  struct omp_region *inner;

  /* Next peer region.  */
  struct omp_region *next;

  /* Block containing the omp directive as its last stmt.  */
  basic_block entry;

  /* Block containing the OMP_RETURN as its last stmt.  */
  basic_block exit;

  /* Block containing the OMP_CONTINUE as its last stmt.  */
  basic_block cont;

  /* If this is a combined parallel+workshare region, this is a list
     of additional arguments needed by the combined parallel+workshare
     library call.  */
  tree ws_args;

  /* The code for the omp directive of this region.  */
  enum gimple_code type;

  /* Schedule kind, only used for OMP_FOR type regions.  */
  enum omp_clause_schedule_kind sched_kind;

  /* True if this is a combined parallel+workshare region.  */
  bool is_combined_parallel;
};

extern struct omp_region *root_omp_region;
extern struct omp_region *new_omp_region (basic_block, enum gimple_code,
					  struct omp_region *);
extern void free_omp_regions (void);
void omp_expand_local (basic_block);
extern tree find_omp_clause (tree, enum omp_clause_code);
tree copy_var_decl (tree, tree, tree);

/*---------------------------------------------------------------------------
			      Function prototypes
---------------------------------------------------------------------------*/
/* In tree-cfg.c  */

/* Location to track pending stmt for edge insertion.  */
#define PENDING_STMT(e)	((e)->insns.g)

extern void delete_tree_cfg_annotations (void);
extern bool stmt_ends_bb_p (gimple);
extern bool is_ctrl_stmt (gimple);
extern bool is_ctrl_altering_stmt (gimple);
extern bool simple_goto_p (gimple);
extern bool stmt_can_make_abnormal_goto (gimple);
extern basic_block single_noncomplex_succ (basic_block bb);
extern void gimple_dump_bb (basic_block, FILE *, int, int);
extern void gimple_debug_bb (basic_block);
extern basic_block gimple_debug_bb_n (int);
extern void gimple_dump_cfg (FILE *, int);
extern void gimple_debug_cfg (int);
extern void dump_cfg_stats (FILE *);
extern void dot_cfg (void);
extern void debug_cfg_stats (void);
extern void debug_loops (int);
extern void debug_loop (struct loop *, int);
extern void debug_loop_num (unsigned, int);
extern void print_loops (FILE *, int);
extern void print_loops_bb (FILE *, basic_block, int, int);
extern void cleanup_dead_labels (void);
extern void group_case_labels (void);
extern gimple first_stmt (basic_block);
extern gimple last_stmt (basic_block);
extern gimple last_and_only_stmt (basic_block);
extern edge find_taken_edge (basic_block, tree);
extern basic_block label_to_block_fn (struct function *, tree);
#define label_to_block(t) (label_to_block_fn (cfun, t))
extern void notice_special_calls (gimple);
extern void clear_special_calls (void);
extern void verify_stmts (void);
extern void verify_gimple (void);
extern void verify_types_in_gimple_seq (gimple_seq);
extern tree gimple_block_label (basic_block);
extern void extract_true_false_edges_from_block (basic_block, edge *, edge *);
extern bool gimple_duplicate_sese_region (edge, edge, basic_block *, unsigned,
					basic_block *);
extern bool gimple_duplicate_sese_tail (edge, edge, basic_block *, unsigned,
				      basic_block *);
extern void gather_blocks_in_sese_region (basic_block entry, basic_block exit,
					  VEC(basic_block,heap) **bbs_p);
extern void add_phi_args_after_copy_bb (basic_block);
extern void add_phi_args_after_copy (basic_block *, unsigned, edge);
extern bool gimple_purge_dead_abnormal_call_edges (basic_block);
extern bool gimple_purge_dead_eh_edges (basic_block);
extern bool gimple_purge_all_dead_eh_edges (const_bitmap);
extern tree gimplify_build1 (gimple_stmt_iterator *, enum tree_code,
			     tree, tree);
extern tree gimplify_build2 (gimple_stmt_iterator *, enum tree_code,
			     tree, tree, tree);
extern tree gimplify_build3 (gimple_stmt_iterator *, enum tree_code,
			     tree, tree, tree, tree);
extern void init_empty_tree_cfg (void);
extern void init_empty_tree_cfg_for_function (struct function *);
extern void fold_cond_expr_cond (void);
extern void make_abnormal_goto_edges (basic_block, bool);
extern void replace_uses_by (tree, tree);
extern void start_recording_case_labels (void);
extern void end_recording_case_labels (void);
extern basic_block move_sese_region_to_fn (struct function *, basic_block,
				           basic_block, tree);
void remove_edge_and_dominated_blocks (edge);
bool tree_node_can_be_shared (tree);

/* In tree-cfgcleanup.c  */
extern bitmap cfgcleanup_altered_bbs;
extern bool cleanup_tree_cfg (void);

/* In tree-pretty-print.c.  */
extern void dump_generic_bb (FILE *, basic_block, int, int);
extern int op_code_prio (enum tree_code);
extern int op_prio (const_tree);
extern const char *op_symbol_code (enum tree_code);

/* In tree-dfa.c  */
extern var_ann_t create_var_ann (tree);
extern void renumber_gimple_stmt_uids (void);
extern void renumber_gimple_stmt_uids_in_blocks (basic_block *, int);
extern void dump_dfa_stats (FILE *);
extern void debug_dfa_stats (void);
extern void debug_referenced_vars (void);
extern void dump_referenced_vars (FILE *);
extern void dump_variable (FILE *, tree);
extern void debug_variable (tree);
extern tree get_virtual_var (tree);
extern bool add_referenced_var (tree);
extern void remove_referenced_var (tree);
extern void mark_symbols_for_renaming (gimple);
extern void find_new_referenced_vars (gimple);
extern tree make_rename_temp (tree, const char *);
extern void set_default_def (tree, tree);
extern tree gimple_default_def (struct function *, tree);
extern bool stmt_references_abnormal_ssa_name (gimple);
extern tree get_ref_base_and_extent (tree, HOST_WIDE_INT *,
				     HOST_WIDE_INT *, HOST_WIDE_INT *);
extern void find_referenced_vars_in (gimple);

/* In tree-phinodes.c  */
extern void reserve_phi_args_for_new_edge (basic_block);
extern void add_phi_node_to_bb (gimple phi, basic_block bb);
extern gimple make_phi_node (tree var, int len);
extern gimple create_phi_node (tree, basic_block);
extern void add_phi_arg (gimple, tree, edge, source_location);
extern void remove_phi_args (edge);
extern void remove_phi_node (gimple_stmt_iterator *, bool);
extern void remove_phi_nodes (basic_block);
extern void init_phinodes (void);
extern void fini_phinodes (void);
extern void release_phi_node (gimple);
#ifdef GATHER_STATISTICS
extern void phinodes_print_statistics (void);
#endif

/* In gimple-low.c  */
extern void record_vars_into (tree, tree);
extern void record_vars (tree);
extern bool block_may_fallthru (const_tree);
extern bool gimple_seq_may_fallthru (gimple_seq);
extern bool gimple_stmt_may_fallthru (gimple);
extern bool gimple_check_call_args (gimple);


/* In tree-ssa.c  */

/* Mapping for redirected edges.  */
struct GTY(()) _edge_var_map {
  tree result;			/* PHI result.  */
  tree def;			/* PHI arg definition.  */
  source_location locus;        /* PHI arg location.  */
};
typedef struct _edge_var_map edge_var_map;

DEF_VEC_O(edge_var_map);
DEF_VEC_ALLOC_O(edge_var_map, heap);

/* A vector of var maps.  */
typedef VEC(edge_var_map, heap) *edge_var_map_vector;

extern void init_tree_ssa (struct function *);
extern void redirect_edge_var_map_add (edge, tree, tree, source_location);
extern void redirect_edge_var_map_clear (edge);
extern void redirect_edge_var_map_dup (edge, edge);
extern edge_var_map_vector redirect_edge_var_map_vector (edge);
extern void redirect_edge_var_map_destroy (void);

extern edge ssa_redirect_edge (edge, basic_block);
extern void flush_pending_stmts (edge);
extern void verify_ssa (bool);
extern void delete_tree_ssa (void);
extern bool ssa_undefined_value_p (tree);
extern void execute_update_addresses_taken (bool);

/* Call-back function for walk_use_def_chains().  At each reaching
   definition, a function with this prototype is called.  */
typedef bool (*walk_use_def_chains_fn) (tree, gimple, void *);

extern void walk_use_def_chains (tree, walk_use_def_chains_fn, void *, bool);

void insert_debug_temps_for_defs (gimple_stmt_iterator *);
void insert_debug_temp_for_var_def (gimple_stmt_iterator *, tree);
void release_defs_bitset (bitmap toremove);

/* In tree-into-ssa.c  */
void update_ssa (unsigned);
void delete_update_ssa (void);
void register_new_name_mapping (tree, tree);
tree create_new_def_for (tree, gimple, def_operand_p);
bool need_ssa_update_p (struct function *);
bool name_mappings_registered_p (void);
bool name_registered_for_update_p (tree);
bitmap ssa_names_to_replace (void);
void release_ssa_name_after_update_ssa (tree);
void compute_global_livein (bitmap, bitmap);
void mark_sym_for_renaming (tree);
void mark_set_for_renaming (bitmap);
tree get_current_def (tree);
void set_current_def (tree, tree);

/* In tree-ssanames.c  */
extern void init_ssanames (struct function *, int);
extern void fini_ssanames (void);
extern tree make_ssa_name_fn (struct function *, tree, gimple);
extern tree duplicate_ssa_name (tree, gimple);
extern void duplicate_ssa_name_ptr_info (tree, struct ptr_info_def *);
extern void release_ssa_name (tree);
extern void release_defs (gimple);
extern void replace_ssa_name_symbol (tree, tree);

#ifdef GATHER_STATISTICS
extern void ssanames_print_statistics (void);
#endif

/* In tree-ssa-ccp.c  */
bool fold_stmt (gimple_stmt_iterator *);
bool fold_stmt_inplace (gimple);
tree get_symbol_constant_value (tree);
tree fold_const_aggregate_ref (tree);
bool may_propagate_address_into_dereference (tree, tree);


/* In tree-ssa-dom.c  */
extern void dump_dominator_optimization_stats (FILE *);
extern void debug_dominator_optimization_stats (void);
int loop_depth_of_name (tree);
tree degenerate_phi_result (gimple);

/* In tree-ssa-copy.c  */
extern void propagate_value (use_operand_p, tree);
extern void propagate_tree_value (tree *, tree);
extern void propagate_tree_value_into_stmt (gimple_stmt_iterator *, tree);
extern void replace_exp (use_operand_p, tree);
extern bool may_propagate_copy (tree, tree);
extern bool may_propagate_copy_into_stmt (gimple, tree);
extern bool may_propagate_copy_into_asm (tree);

/* Affine iv.  */

typedef struct
{
  /* Iv = BASE + STEP * i.  */
  tree base, step;

  /* True if this iv does not overflow.  */
  bool no_overflow;
} affine_iv;

/* Description of number of iterations of a loop.  All the expressions inside
   the structure can be evaluated at the end of the loop's preheader
   (and due to ssa form, also anywhere inside the body of the loop).  */

struct tree_niter_desc
{
  tree assumptions;	/* The boolean expression.  If this expression evaluates
			   to false, then the other fields in this structure
			   should not be used; there is no guarantee that they
			   will be correct.  */
  tree may_be_zero;	/* The boolean expression.  If it evaluates to true,
			   the loop will exit in the first iteration (i.e.
			   its latch will not be executed), even if the niter
			   field says otherwise.  */
  tree niter;		/* The expression giving the number of iterations of
			   a loop (provided that assumptions == true and
			   may_be_zero == false), more precisely the number
			   of executions of the latch of the loop.  */
  double_int max;	/* The upper bound on the number of iterations of
			   the loop.  */

  /* The simplified shape of the exit condition.  The loop exits if
     CONTROL CMP BOUND is false, where CMP is one of NE_EXPR,
     LT_EXPR, or GT_EXPR, and step of CONTROL is positive if CMP is
     LE_EXPR and negative if CMP is GE_EXPR.  This information is used
     by loop unrolling.  */
  affine_iv control;
  tree bound;
  enum tree_code cmp;
};

/* In tree-ssa-phiopt.c */
bool empty_block_p (basic_block);
basic_block *blocks_in_phiopt_order (void);

/* In tree-ssa-loop*.c  */

void tree_ssa_lim (void);
unsigned int tree_ssa_unswitch_loops (void);
unsigned int canonicalize_induction_variables (void);
unsigned int tree_unroll_loops_completely (bool, bool);
unsigned int tree_ssa_prefetch_arrays (void);
void tree_ssa_iv_optimize (void);
unsigned tree_predictive_commoning (void);
tree canonicalize_loop_ivs (struct loop *, tree *, bool);
bool parallelize_loops (void);

bool loop_only_exit_p (const struct loop *, const_edge);
bool number_of_iterations_exit (struct loop *, edge,
				struct tree_niter_desc *niter, bool);
tree find_loop_niter (struct loop *, edge *);
tree loop_niter_by_eval (struct loop *, edge);
tree find_loop_niter_by_eval (struct loop *, edge *);
void estimate_numbers_of_iterations (void);
bool array_at_struct_end_p (tree);
bool scev_probably_wraps_p (tree, tree, gimple, struct loop *, bool);
bool convert_affine_scev (struct loop *, tree, tree *, tree *, gimple, bool);

bool nowrap_type_p (tree);
enum ev_direction {EV_DIR_GROWS, EV_DIR_DECREASES, EV_DIR_UNKNOWN};
enum ev_direction scev_direction (const_tree);

void free_numbers_of_iterations_estimates (void);
void free_numbers_of_iterations_estimates_loop (struct loop *);
void rewrite_into_loop_closed_ssa (bitmap, unsigned);
void verify_loop_closed_ssa (void);
bool for_each_index (tree *, bool (*) (tree, tree *, void *), void *);
void create_iv (tree, tree, tree, struct loop *, gimple_stmt_iterator *, bool,
		tree *, tree *);
basic_block split_loop_exit_edge (edge);
void standard_iv_increment_position (struct loop *, gimple_stmt_iterator *,
				     bool *);
basic_block ip_end_pos (struct loop *);
basic_block ip_normal_pos (struct loop *);
bool gimple_duplicate_loop_to_header_edge (struct loop *, edge,
					 unsigned int, sbitmap,
					 edge, VEC (edge, heap) **,
					 int);
struct loop *slpeel_tree_duplicate_loop_to_edge_cfg (struct loop *, edge);
void rename_variables_in_loop (struct loop *);
void rename_variables_in_bb (basic_block bb);
struct loop *tree_ssa_loop_version (struct loop *, tree,
				    basic_block *);
tree expand_simple_operations (tree);
void substitute_in_loop_info (struct loop *, tree, tree);
edge single_dom_exit (struct loop *);
bool can_unroll_loop_p (struct loop *loop, unsigned factor,
			struct tree_niter_desc *niter);
void tree_unroll_loop (struct loop *, unsigned,
		       edge, struct tree_niter_desc *);
typedef void (*transform_callback)(struct loop *, void *);
void tree_transform_and_unroll_loop (struct loop *, unsigned,
				     edge, struct tree_niter_desc *,
				     transform_callback, void *);
bool contains_abnormal_ssa_name_p (tree);
bool stmt_dominates_stmt_p (gimple, gimple);
void mark_virtual_ops_for_renaming (gimple);

/* In tree-ssa-dce.c */
void mark_virtual_phi_result_for_renaming (gimple);

/* In tree-ssa-threadedge.c */
extern void threadedge_initialize_values (void);
extern void threadedge_finalize_values (void);
extern VEC(tree,heap) *ssa_name_values;
#define SSA_NAME_VALUE(x) \
    (SSA_NAME_VERSION(x) < VEC_length(tree, ssa_name_values) \
     ? VEC_index(tree, ssa_name_values, SSA_NAME_VERSION(x)) \
     : NULL_TREE)
extern void set_ssa_name_value (tree, tree);
extern bool potentially_threadable_block (basic_block);
extern void thread_across_edge (gimple, edge, bool,
				VEC(tree, heap) **, tree (*) (gimple, gimple));

/* In tree-ssa-loop-im.c  */
/* The possibilities of statement movement.  */

enum move_pos
  {
    MOVE_IMPOSSIBLE,		/* No movement -- side effect expression.  */
    MOVE_PRESERVE_EXECUTION,	/* Must not cause the non-executed statement
				   become executed -- memory accesses, ... */
    MOVE_POSSIBLE		/* Unlimited movement.  */
  };
extern enum move_pos movement_possibility (gimple);
char *get_lsm_tmp_name (tree, unsigned);

/* In tree-flow-inline.h  */
static inline bool is_call_clobbered (const_tree);
static inline void set_is_used (tree);
static inline bool unmodifiable_var_p (const_tree);
static inline bool ref_contains_array_ref (const_tree);
static inline bool array_ref_contains_indirect_ref (const_tree);

/* In tree-eh.c  */
extern void make_eh_edges (gimple);
extern bool make_eh_dispatch_edges (gimple);
extern edge redirect_eh_edge (edge, basic_block);
extern void redirect_eh_dispatch_edge (gimple, edge, basic_block);
extern bool tree_could_trap_p (tree);
extern bool operation_could_trap_helper_p (enum tree_code, bool, bool, bool,
					   bool, tree, bool *);
extern bool operation_could_trap_p (enum tree_code, bool, bool, tree);
extern bool stmt_could_throw_p (gimple);
extern bool tree_could_throw_p (tree);
extern bool stmt_can_throw_internal (gimple);
extern bool stmt_can_throw_external (gimple);
extern void add_stmt_to_eh_lp_fn (struct function *, gimple, int);
extern void add_stmt_to_eh_lp (gimple, int);
extern bool remove_stmt_from_eh_lp (gimple);
extern bool remove_stmt_from_eh_lp_fn (struct function *, gimple);
extern int lookup_stmt_eh_lp_fn (struct function *, gimple);
extern int lookup_stmt_eh_lp (gimple);
extern bool maybe_clean_eh_stmt_fn (struct function *, gimple);
extern bool maybe_clean_eh_stmt (gimple);
extern bool maybe_clean_or_replace_eh_stmt (gimple, gimple);
extern bool maybe_duplicate_eh_stmt_fn (struct function *, gimple,
					struct function *, gimple,
					struct pointer_map_t *, int);
extern bool maybe_duplicate_eh_stmt (gimple, gimple);
extern bool verify_eh_edges (gimple);
extern bool verify_eh_dispatch_edge (gimple);

/* In tree-ssa-pre.c  */
struct pre_expr_d;
void add_to_value (unsigned int, struct pre_expr_d *);
void debug_value_expressions (unsigned int);
void print_value_expressions (FILE *, unsigned int);

/* In tree-ssa-sink.c  */
bool is_hidden_global_store (gimple);

/* In tree-loop-linear.c  */
extern void linear_transform_loops (void);
extern unsigned perfect_loop_nest_depth (struct loop *);

/* In graphite.c  */
extern void graphite_transform_loops (void);

/* In tree-data-ref.c  */
extern void tree_check_data_deps (void);

/* In tree-ssa-loop-ivopts.c  */
bool expr_invariant_in_loop_p (struct loop *, tree);
bool stmt_invariant_in_loop_p (struct loop *, gimple);
bool multiplier_allowed_in_address_p (HOST_WIDE_INT, enum machine_mode,
				      addr_space_t);
unsigned multiply_by_cost (HOST_WIDE_INT, enum machine_mode, bool);
bool may_be_nonaddressable_p (tree expr);

/* In tree-ssa-threadupdate.c.  */
extern bool thread_through_all_blocks (bool);
extern void register_jump_thread (edge, edge);

/* In gimplify.c  */
tree force_gimple_operand (tree, gimple_seq *, bool, tree);
tree force_gimple_operand_gsi (gimple_stmt_iterator *, tree, bool, tree,
			       bool, enum gsi_iterator_update);
tree gimple_fold_indirect_ref (tree);

/* In tree-ssa-live.c */
extern void remove_unused_locals (void);
extern void dump_scope_blocks (FILE *, int);
extern void debug_scope_blocks (int);
extern void debug_scope_block (tree, int);

/* In tree-ssa-address.c  */

/* Description of a memory address.  */

struct mem_address
{
  tree symbol, base, index, step, offset;
};

struct affine_tree_combination;
tree create_mem_ref (gimple_stmt_iterator *, tree,
		     struct affine_tree_combination *, tree, bool);
rtx addr_for_mem_ref (struct mem_address *, addr_space_t, bool);
void get_address_description (tree, struct mem_address *);
tree maybe_fold_tmr (tree);

unsigned int execute_free_datastructures (void);
unsigned int execute_fixup_cfg (void);

#include "tree-flow-inline.h"

void swap_tree_operands (gimple, tree *, tree *);

int least_common_multiple (int, int);

#endif /* _TREE_FLOW_H  */
