/* Natural loop functions
   Copyright (C) 1987, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004,
   2005, 2006, 2007, 2008, 2009  Free Software Foundation, Inc.

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

#ifndef GCC_CFGLOOP_H
#define GCC_CFGLOOP_H

#include "basic-block.h"
/* For rtx_code.  */
#include "rtl.h"
#include "vecprim.h"
#include "double-int.h"

/* Structure to hold decision about unrolling/peeling.  */
enum lpt_dec
{
  LPT_NONE,
  LPT_PEEL_COMPLETELY,
  LPT_PEEL_SIMPLE,
  LPT_UNROLL_CONSTANT,
  LPT_UNROLL_RUNTIME,
  LPT_UNROLL_STUPID
};

struct GTY (()) lpt_decision {
  enum lpt_dec decision;
  unsigned times;
};

/* The structure describing a bound on number of iterations of a loop.  */

struct GTY ((chain_next ("%h.next"))) nb_iter_bound {
  /* The statement STMT is executed at most ...  */
  gimple stmt;

  /* ... BOUND + 1 times (BOUND must be an unsigned constant).
     The + 1 is added for the following reasons:

     a) 0 would otherwise be unused, while we would need to care more about
        overflows (as MAX + 1 is sometimes produced as the estimate on number
	of executions of STMT).
     b) it is consistent with the result of number_of_iterations_exit.  */
  double_int bound;

  /* True if the statement will cause the loop to be leaved the (at most)
     BOUND + 1-st time it is executed, that is, all the statements after it
     are executed at most BOUND times.  */
  bool is_exit;

  /* The next bound in the list.  */
  struct nb_iter_bound *next;
};

/* Description of the loop exit.  */

struct GTY (()) loop_exit {
  /* The exit edge.  */
  struct edge_def *e;

  /* Previous and next exit in the list of the exits of the loop.  */
  struct loop_exit *prev;
  struct loop_exit *next;

  /* Next element in the list of loops from that E exits.  */
  struct loop_exit *next_e;
};

typedef struct loop *loop_p;
DEF_VEC_P (loop_p);
DEF_VEC_ALLOC_P (loop_p, heap);
DEF_VEC_ALLOC_P (loop_p, gc);

/* An integer estimation of the number of iterations.  Estimate_state
   describes what is the state of the estimation.  */
enum loop_estimation
{
  /* Estimate was not computed yet.  */
  EST_NOT_COMPUTED,
  /* Estimate is ready.  */
  EST_AVAILABLE
};

/* Structure to hold information for each natural loop.  */
struct GTY ((chain_next ("%h.next"))) loop {
  /* Index into loops array.  */
  int num;

  /* Number of loop insns.  */
  unsigned ninsns;

  /* Basic block of loop header.  */
  struct basic_block_def *header;

  /* Basic block of loop latch.  */
  struct basic_block_def *latch;

  /* For loop unrolling/peeling decision.  */
  struct lpt_decision lpt_decision;

  /* Average number of executed insns per iteration.  */
  unsigned av_ninsns;

  /* Number of blocks contained within the loop.  */
  unsigned num_nodes;

  /* Superloops of the loop, starting with the outermost loop.  */
  VEC (loop_p, gc) *superloops;

  /* The first inner (child) loop or NULL if innermost loop.  */
  struct loop *inner;

  /* Link to the next (sibling) loop.  */
  struct loop *next;

  /* Auxiliary info specific to a pass.  */
  PTR GTY ((skip (""))) aux;

  /* The number of times the latch of the loop is executed.
     This is an INTEGER_CST or an expression containing symbolic
     names.  Don't access this field directly:
     number_of_latch_executions computes and caches the computed
     information in this field.  */
  tree nb_iterations;

  /* An integer guaranteed to bound the number of iterations of the loop
     from above.  */
  double_int nb_iterations_upper_bound;

  /* An integer giving the expected number of iterations of the loop.  */
  double_int nb_iterations_estimate;

  bool any_upper_bound;
  bool any_estimate;

  /* An integer estimation of the number of iterations.  Estimate_state
     describes what is the state of the estimation.  */
  enum loop_estimation estimate_state;

  /* Upper bound on number of iterations of a loop.  */
  struct nb_iter_bound *bounds;

  /* Head of the cyclic list of the exits of the loop.  */
  struct loop_exit *exits;

  /* True if the loop can be parallel.  */
  bool can_be_parallel;

  /* The single induction variable of the loop when the loop is in
     normal form.  */
  tree single_iv;
};

/* Flags for state of loop structure.  */
enum
{
  LOOPS_HAVE_PREHEADERS = 1,
  LOOPS_HAVE_SIMPLE_LATCHES = 2,
  LOOPS_HAVE_MARKED_IRREDUCIBLE_REGIONS = 4,
  LOOPS_HAVE_RECORDED_EXITS = 8,
  LOOPS_MAY_HAVE_MULTIPLE_LATCHES = 16,
  LOOP_CLOSED_SSA = 32,
  LOOPS_NEED_FIXUP = 64,
  LOOPS_HAVE_FALLTHRU_PREHEADERS = 128
};

#define LOOPS_NORMAL (LOOPS_HAVE_PREHEADERS | LOOPS_HAVE_SIMPLE_LATCHES \
		      | LOOPS_HAVE_MARKED_IRREDUCIBLE_REGIONS)
#define AVOID_CFG_MODIFICATIONS (LOOPS_MAY_HAVE_MULTIPLE_LATCHES)

/* Structure to hold CFG information about natural loops within a function.  */
struct GTY (()) loops {
  /* State of loops.  */
  int state;

  /* Array of the loops.  */
  VEC (loop_p, gc) *larray;

  /* Maps edges to the list of their descriptions as loop exits.  Edges
     whose sources or destinations have loop_father == NULL (which may
     happen during the cfg manipulations) should not appear in EXITS.  */
  htab_t GTY((param_is (struct loop_exit))) exits;

  /* Pointer to root of loop hierarchy tree.  */
  struct loop *tree_root;
};

/* Loop recognition.  */
extern int flow_loops_find (struct loops *);
extern void disambiguate_loops_with_multiple_latches (void);
extern void flow_loops_free (struct loops *);
extern void flow_loops_dump (FILE *,
			     void (*)(const struct loop *, FILE *, int), int);
extern void flow_loop_dump (const struct loop *, FILE *,
			    void (*)(const struct loop *, FILE *, int), int);
struct loop *alloc_loop (void);
extern void flow_loop_free (struct loop *);
int flow_loop_nodes_find (basic_block, struct loop *);
void fix_loop_structure (bitmap changed_bbs);
bool mark_irreducible_loops (void);
void release_recorded_exits (void);
void record_loop_exits (void);
void rescan_loop_exit (edge, bool, bool);

/* Loop data structure manipulation/querying.  */
extern void flow_loop_tree_node_add (struct loop *, struct loop *);
extern void flow_loop_tree_node_remove (struct loop *);
extern void add_loop (struct loop *, struct loop *);
extern bool flow_loop_nested_p	(const struct loop *, const struct loop *);
extern bool flow_bb_inside_loop_p (const struct loop *, const_basic_block);
extern struct loop * find_common_loop (struct loop *, struct loop *);
struct loop *superloop_at_depth (struct loop *, unsigned);
struct eni_weights_d;
extern unsigned tree_num_loop_insns (struct loop *, struct eni_weights_d *);
extern int num_loop_insns (const struct loop *);
extern int average_num_loop_insns (const struct loop *);
extern unsigned get_loop_level (const struct loop *);
extern bool loop_exit_edge_p (const struct loop *, const_edge);
extern bool is_loop_exit (struct loop *, basic_block);
extern void mark_loop_exit_edges (void);

/* Loops & cfg manipulation.  */
extern basic_block *get_loop_body (const struct loop *);
extern unsigned get_loop_body_with_size (const struct loop *, basic_block *,
					 unsigned);
extern basic_block *get_loop_body_in_dom_order (const struct loop *);
extern basic_block *get_loop_body_in_bfs_order (const struct loop *);
extern basic_block *get_loop_body_in_custom_order (const struct loop *,
			       int (*) (const void *, const void *));

extern VEC (edge, heap) *get_loop_exit_edges (const struct loop *);
edge single_exit (const struct loop *);
extern unsigned num_loop_branches (const struct loop *);

extern edge loop_preheader_edge (const struct loop *);
extern edge loop_latch_edge (const struct loop *);

extern void add_bb_to_loop (basic_block, struct loop *);
extern void remove_bb_from_loops (basic_block);

extern void cancel_loop_tree (struct loop *);
extern void delete_loop (struct loop *);

enum
{
  CP_SIMPLE_PREHEADERS = 1,
  CP_FALLTHRU_PREHEADERS = 2
};

basic_block create_preheader (struct loop *, int);
extern void create_preheaders (int);
extern void force_single_succ_latches (void);

extern void verify_loop_structure (void);

/* Loop analysis.  */
extern bool just_once_each_iteration_p (const struct loop *, const_basic_block);
gcov_type expected_loop_iterations_unbounded (const struct loop *);
extern unsigned expected_loop_iterations (const struct loop *);
extern rtx doloop_condition_get (rtx);

void estimate_numbers_of_iterations_loop (struct loop *);
HOST_WIDE_INT estimated_loop_iterations_int (struct loop *, bool);
bool estimated_loop_iterations (struct loop *, bool, double_int *);

/* Loop manipulation.  */
extern bool can_duplicate_loop_p (const struct loop *loop);

#define DLTHE_FLAG_UPDATE_FREQ	1	/* Update frequencies in
					   duplicate_loop_to_header_edge.  */
#define DLTHE_RECORD_COPY_NUMBER 2	/* Record copy number in the aux
					   field of newly create BB.  */
#define DLTHE_FLAG_COMPLETTE_PEEL 4	/* Update frequencies expecting
					   a complete peeling.  */

extern edge create_empty_if_region_on_edge (edge, tree);
extern struct loop *create_empty_loop_on_edge (edge, tree, tree, tree, tree,
					       tree *, tree *, struct loop *);
extern struct loop * duplicate_loop (struct loop *, struct loop *);
extern void duplicate_subloops (struct loop *, struct loop *);
extern bool duplicate_loop_to_header_edge (struct loop *, edge,
					   unsigned, sbitmap, edge,
 					   VEC (edge, heap) **, int);
extern struct loop *loopify (edge, edge,
			     basic_block, edge, edge, bool,
			     unsigned, unsigned);
struct loop * loop_version (struct loop *, void *,
			    basic_block *, unsigned, unsigned, unsigned, bool);
extern bool remove_path (edge);
void scale_loop_frequencies (struct loop *, int, int);

/* Induction variable analysis.  */

/* The description of induction variable.  The things are a bit complicated
   due to need to handle subregs and extends.  The value of the object described
   by it can be obtained as follows (all computations are done in extend_mode):

   Value in i-th iteration is
     delta + mult * extend_{extend_mode} (subreg_{mode} (base + i * step)).

   If first_special is true, the value in the first iteration is
     delta + mult * base

   If extend = UNKNOWN, first_special must be false, delta 0, mult 1 and value is
     subreg_{mode} (base + i * step)

   The get_iv_value function can be used to obtain these expressions.

   ??? Add a third mode field that would specify the mode in that inner
   computation is done, which would enable it to be different from the
   outer one?  */

struct rtx_iv
{
  /* Its base and step (mode of base and step is supposed to be extend_mode,
     see the description above).  */
  rtx base, step;

  /* The type of extend applied to it (SIGN_EXTEND, ZERO_EXTEND or UNKNOWN).  */
  enum rtx_code extend;

  /* Operations applied in the extended mode.  */
  rtx delta, mult;

  /* The mode it is extended to.  */
  enum machine_mode extend_mode;

  /* The mode the variable iterates in.  */
  enum machine_mode mode;

  /* Whether the first iteration needs to be handled specially.  */
  unsigned first_special : 1;
};

/* The description of an exit from the loop and of the number of iterations
   till we take the exit.  */

struct niter_desc
{
  /* The edge out of the loop.  */
  edge out_edge;

  /* The other edge leading from the condition.  */
  edge in_edge;

  /* True if we are able to say anything about number of iterations of the
     loop.  */
  bool simple_p;

  /* True if the loop iterates the constant number of times.  */
  bool const_iter;

  /* Number of iterations if constant.  */
  unsigned HOST_WIDEST_INT niter;

  /* Upper bound on the number of iterations.  */
  unsigned HOST_WIDEST_INT niter_max;

  /* Assumptions under that the rest of the information is valid.  */
  rtx assumptions;

  /* Assumptions under that the loop ends before reaching the latch,
     even if value of niter_expr says otherwise.  */
  rtx noloop_assumptions;

  /* Condition under that the loop is infinite.  */
  rtx infinite;

  /* Whether the comparison is signed.  */
  bool signed_p;

  /* The mode in that niter_expr should be computed.  */
  enum machine_mode mode;

  /* The number of iterations of the loop.  */
  rtx niter_expr;
};

extern void iv_analysis_loop_init (struct loop *);
extern bool iv_analyze (rtx, rtx, struct rtx_iv *);
extern bool iv_analyze_result (rtx, rtx, struct rtx_iv *);
extern bool iv_analyze_expr (rtx, rtx, enum machine_mode, struct rtx_iv *);
extern rtx get_iv_value (struct rtx_iv *, rtx);
extern bool biv_p (rtx, rtx);
extern void find_simple_exit (struct loop *, struct niter_desc *);
extern void iv_analysis_done (void);

extern struct niter_desc *get_simple_loop_desc (struct loop *loop);
extern void free_simple_loop_desc (struct loop *loop);

static inline struct niter_desc *
simple_loop_desc (struct loop *loop)
{
  return (struct niter_desc *) loop->aux;
}

/* Accessors for the loop structures.  */

/* Returns the loop with index NUM from current_loops.  */

static inline struct loop *
get_loop (unsigned num)
{
  return VEC_index (loop_p, current_loops->larray, num);
}

/* Returns the number of superloops of LOOP.  */

static inline unsigned
loop_depth (const struct loop *loop)
{
  return VEC_length (loop_p, loop->superloops);
}

/* Returns the immediate superloop of LOOP, or NULL if LOOP is the outermost
   loop.  */

static inline struct loop *
loop_outer (const struct loop *loop)
{
  unsigned n = VEC_length (loop_p, loop->superloops);

  if (n == 0)
    return NULL;

  return VEC_index (loop_p, loop->superloops, n - 1);
}

/* Returns the list of loops in current_loops.  */

static inline VEC (loop_p, gc) *
get_loops (void)
{
  if (!current_loops)
    return NULL;

  return current_loops->larray;
}

/* Returns the number of loops in current_loops (including the removed
   ones and the fake loop that forms the root of the loop tree).  */

static inline unsigned
number_of_loops (void)
{
  if (!current_loops)
    return 0;

  return VEC_length (loop_p, current_loops->larray);
}

/* Returns true if state of the loops satisfies all properties
   described by FLAGS.  */

static inline bool
loops_state_satisfies_p (unsigned flags)
{
  return (current_loops->state & flags) == flags;
}

/* Sets FLAGS to the loops state.  */

static inline void
loops_state_set (unsigned flags)
{
  current_loops->state |= flags;
}

/* Clears FLAGS from the loops state.  */

static inline void
loops_state_clear (unsigned flags)
{
  if (!current_loops)
    return;
  current_loops->state &= ~flags;
}

/* Loop iterators.  */

/* Flags for loop iteration.  */

enum li_flags
{
  LI_INCLUDE_ROOT = 1,		/* Include the fake root of the loop tree.  */
  LI_FROM_INNERMOST = 2,	/* Iterate over the loops in the reverse order,
				   starting from innermost ones.  */
  LI_ONLY_INNERMOST = 4		/* Iterate only over innermost loops.  */
};

/* The iterator for loops.  */

typedef struct
{
  /* The list of loops to visit.  */
  VEC(int,heap) *to_visit;

  /* The index of the actual loop.  */
  unsigned idx;
} loop_iterator;

static inline void
fel_next (loop_iterator *li, loop_p *loop)
{
  int anum;

  while (VEC_iterate (int, li->to_visit, li->idx, anum))
    {
      li->idx++;
      *loop = get_loop (anum);
      if (*loop)
	return;
    }

  VEC_free (int, heap, li->to_visit);
  *loop = NULL;
}

static inline void
fel_init (loop_iterator *li, loop_p *loop, unsigned flags)
{
  struct loop *aloop;
  unsigned i;
  int mn;

  li->idx = 0;
  if (!current_loops)
    {
      li->to_visit = NULL;
      *loop = NULL;
      return;
    }

  li->to_visit = VEC_alloc (int, heap, number_of_loops ());
  mn = (flags & LI_INCLUDE_ROOT) ? 0 : 1;

  if (flags & LI_ONLY_INNERMOST)
    {
      for (i = 0; VEC_iterate (loop_p, current_loops->larray, i, aloop); i++)
	if (aloop != NULL
	    && aloop->inner == NULL
	    && aloop->num >= mn)
	  VEC_quick_push (int, li->to_visit, aloop->num);
    }
  else if (flags & LI_FROM_INNERMOST)
    {
      /* Push the loops to LI->TO_VISIT in postorder.  */
      for (aloop = current_loops->tree_root;
	   aloop->inner != NULL;
	   aloop = aloop->inner)
	continue;

      while (1)
	{
	  if (aloop->num >= mn)
	    VEC_quick_push (int, li->to_visit, aloop->num);

	  if (aloop->next)
	    {
	      for (aloop = aloop->next;
		   aloop->inner != NULL;
		   aloop = aloop->inner)
		continue;
	    }
	  else if (!loop_outer (aloop))
	    break;
	  else
	    aloop = loop_outer (aloop);
	}
    }
  else
    {
      /* Push the loops to LI->TO_VISIT in preorder.  */
      aloop = current_loops->tree_root;
      while (1)
	{
	  if (aloop->num >= mn)
	    VEC_quick_push (int, li->to_visit, aloop->num);

	  if (aloop->inner != NULL)
	    aloop = aloop->inner;
	  else
	    {
	      while (aloop != NULL && aloop->next == NULL)
		aloop = loop_outer (aloop);
	      if (aloop == NULL)
		break;
	      aloop = aloop->next;
	    }
	}
    }

  fel_next (li, loop);
}

#define FOR_EACH_LOOP(LI, LOOP, FLAGS) \
  for (fel_init (&(LI), &(LOOP), FLAGS); \
       (LOOP); \
       fel_next (&(LI), &(LOOP)))

#define FOR_EACH_LOOP_BREAK(LI) \
  { \
    VEC_free (int, heap, (LI)->to_visit); \
    break; \
  }

/* The properties of the target.  */

extern unsigned target_avail_regs;
extern unsigned target_clobbered_regs;
extern unsigned target_res_regs;
extern unsigned target_reg_cost [2];
extern unsigned target_spill_cost [2];

/* Register pressure estimation for induction variable optimizations & loop
   invariant motion.  */
extern unsigned estimate_reg_pressure_cost (unsigned, unsigned, bool, bool);
extern void init_set_costs (void);

/* Loop optimizer initialization.  */
extern void loop_optimizer_init (unsigned);
extern void loop_optimizer_finalize (void);

/* Optimization passes.  */
extern void unswitch_loops (void);

enum
{
  UAP_PEEL = 1,		/* Enables loop peeling.  */
  UAP_UNROLL = 2,	/* Enables unrolling of loops if it seems profitable.  */
  UAP_UNROLL_ALL = 4	/* Enables unrolling of all loops.  */
};

extern void unroll_and_peel_loops (int);
extern void doloop_optimize_loops (void);
extern void move_loop_invariants (void);
extern bool finite_loop_p (struct loop *);

#endif /* GCC_CFGLOOP_H */
