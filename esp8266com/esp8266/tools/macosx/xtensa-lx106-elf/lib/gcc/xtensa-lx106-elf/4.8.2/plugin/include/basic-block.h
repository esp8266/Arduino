/* Define control flow data structures for the CFG.
   Copyright (C) 1987-2013 Free Software Foundation, Inc.

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

#ifndef GCC_BASIC_BLOCK_H
#define GCC_BASIC_BLOCK_H

#include "predict.h"
#include "vec.h"
#include "function.h"

/* Type we use to hold basic block counters.  Should be at least
   64bit.  Although a counter cannot be negative, we use a signed
   type, because erroneous negative counts can be generated when the
   flow graph is manipulated by various optimizations.  A signed type
   makes those easy to detect.  */
typedef HOST_WIDEST_INT gcov_type;
typedef unsigned HOST_WIDEST_INT gcov_type_unsigned;

/* Control flow edge information.  */
struct GTY((user)) edge_def {
  /* The two blocks at the ends of the edge.  */
  basic_block src;
  basic_block dest;

  /* Instructions queued on the edge.  */
  union edge_def_insns {
    gimple_seq g;
    rtx r;
  } insns;

  /* Auxiliary info specific to a pass.  */
  PTR aux;

  /* Location of any goto implicit in the edge.  */
  location_t goto_locus;

  /* The index number corresponding to this edge in the edge vector
     dest->preds.  */
  unsigned int dest_idx;

  int flags;			/* see cfg-flags.def */
  int probability;		/* biased by REG_BR_PROB_BASE */
  gcov_type count;		/* Expected number of executions calculated
				   in profile.c  */
};


/* Garbage collection and PCH support for edge_def.  */
extern void gt_ggc_mx (edge_def *e);
extern void gt_pch_nx (edge_def *e);
extern void gt_pch_nx (edge_def *e, gt_pointer_operator, void *);

/* Masks for edge.flags.  */
#define DEF_EDGE_FLAG(NAME,IDX) EDGE_##NAME = 1 << IDX ,
enum cfg_edge_flags {
#include "cfg-flags.def"
  LAST_CFG_EDGE_FLAG		/* this is only used for EDGE_ALL_FLAGS */
};
#undef DEF_EDGE_FLAG

/* Bit mask for all edge flags.  */
#define EDGE_ALL_FLAGS		((LAST_CFG_EDGE_FLAG - 1) * 2 - 1)

/* The following four flags all indicate something special about an edge.
   Test the edge flags on EDGE_COMPLEX to detect all forms of "strange"
   control flow transfers.  */
#define EDGE_COMPLEX \
  (EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_EH | EDGE_PRESERVE)

/* Counter summary from the last set of coverage counts read by
   profile.c.  */
extern const struct gcov_ctr_summary *profile_info;

/* Working set size statistics for a given percentage of the entire
   profile (sum_all from the counter summary).  */
typedef struct gcov_working_set_info
{
  /* Number of hot counters included in this working set.  */
  unsigned num_counters;
  /* Smallest counter included in this working set.  */
  gcov_type min_counter;
} gcov_working_set_t;

/* Structure to gather statistic about profile consistency, per pass.
   An array of this structure, indexed by pass static number, is allocated
   in passes.c.  The structure is defined here so that different CFG modes
   can do their book-keeping via CFG hooks.

   For every field[2], field[0] is the count before the pass runs, and
   field[1] is the post-pass count.  This allows us to monitor the effect
   of each individual pass on the profile consistency.
   
   This structure is not supposed to be used by anything other than passes.c
   and one CFG hook per CFG mode.  */
struct profile_record
{
  /* The number of basic blocks where sum(freq) of the block's predecessors
     doesn't match reasonably well with the incoming frequency.  */
  int num_mismatched_freq_in[2];
  /* Likewise for a basic block's successors.  */
  int num_mismatched_freq_out[2];
  /* The number of basic blocks where sum(count) of the block's predecessors
     doesn't match reasonably well with the incoming frequency.  */
  int num_mismatched_count_in[2];
  /* Likewise for a basic block's successors.  */
  int num_mismatched_count_out[2];
  /* A weighted cost of the run-time of the function body.  */
  gcov_type time[2];
  /* A weighted cost of the size of the function body.  */
  int size[2];
  /* True iff this pass actually was run.  */
  bool run;
};

/* Declared in cfgloop.h.  */
struct loop;

struct GTY(()) rtl_bb_info {
  /* The first insn of the block is embedded into bb->il.x.  */
  /* The last insn of the block.  */
  rtx end_;

  /* In CFGlayout mode points to insn notes/jumptables to be placed just before
     and after the block.   */
  rtx header_;
  rtx footer_;
};

struct GTY(()) gimple_bb_info {
  /* Sequence of statements in this block.  */
  gimple_seq seq;

  /* PHI nodes for this block.  */
  gimple_seq phi_nodes;
};

/* A basic block is a sequence of instructions with only one entry and
   only one exit.  If any one of the instructions are executed, they
   will all be executed, and in sequence from first to last.

   There may be COND_EXEC instructions in the basic block.  The
   COND_EXEC *instructions* will be executed -- but if the condition
   is false the conditionally executed *expressions* will of course
   not be executed.  We don't consider the conditionally executed
   expression (which might have side-effects) to be in a separate
   basic block because the program counter will always be at the same
   location after the COND_EXEC instruction, regardless of whether the
   condition is true or not.

   Basic blocks need not start with a label nor end with a jump insn.
   For example, a previous basic block may just "conditionally fall"
   into the succeeding basic block, and the last basic block need not
   end with a jump insn.  Block 0 is a descendant of the entry block.

   A basic block beginning with two labels cannot have notes between
   the labels.

   Data for jump tables are stored in jump_insns that occur in no
   basic block even though these insns can follow or precede insns in
   basic blocks.  */

/* Basic block information indexed by block number.  */
struct GTY((chain_next ("%h.next_bb"), chain_prev ("%h.prev_bb"))) basic_block_def {
  /* The edges into and out of the block.  */
  vec<edge, va_gc> *preds;
  vec<edge, va_gc> *succs;

  /* Auxiliary info specific to a pass.  */
  PTR GTY ((skip (""))) aux;

  /* Innermost loop containing the block.  */
  struct loop *loop_father;

  /* The dominance and postdominance information node.  */
  struct et_node * GTY ((skip (""))) dom[2];

  /* Previous and next blocks in the chain.  */
  basic_block prev_bb;
  basic_block next_bb;

  union basic_block_il_dependent {
      struct gimple_bb_info GTY ((tag ("0"))) gimple;
      struct {
        rtx head_;
        struct rtl_bb_info * rtl;
      } GTY ((tag ("1"))) x;
    } GTY ((desc ("((%1.flags & BB_RTL) != 0)"))) il;

  /* Various flags.  See cfg-flags.def.  */
  int flags;

  /* The index of this block.  */
  int index;

  /* Expected number of executions: calculated in profile.c.  */
  gcov_type count;

  /* Expected frequency.  Normalized to be in range 0 to BB_FREQ_MAX.  */
  int frequency;

  /* The discriminator for this block.  The discriminator distinguishes
     among several basic blocks that share a common locus, allowing for
     more accurate sample-based profiling.  */
  int discriminator;
};

/* This ensures that struct gimple_bb_info is smaller than
   struct rtl_bb_info, so that inlining the former into basic_block_def
   is the better choice.  */
typedef int __assert_gimple_bb_smaller_rtl_bb
              [(int)sizeof(struct rtl_bb_info)
               - (int)sizeof (struct gimple_bb_info)];


#define BB_FREQ_MAX 10000

/* Masks for basic_block.flags.  */
#define DEF_BASIC_BLOCK_FLAG(NAME,IDX) BB_##NAME = 1 << IDX ,
enum cfg_bb_flags
{
#include "cfg-flags.def"
  LAST_CFG_BB_FLAG		/* this is only used for BB_ALL_FLAGS */
};
#undef DEF_BASIC_BLOCK_FLAG

/* Bit mask for all basic block flags.  */
#define BB_ALL_FLAGS		((LAST_CFG_BB_FLAG - 1) * 2 - 1)

/* Bit mask for all basic block flags that must be preserved.  These are
   the bit masks that are *not* cleared by clear_bb_flags.  */
#define BB_FLAGS_TO_PRESERVE					\
  (BB_DISABLE_SCHEDULE | BB_RTL | BB_NON_LOCAL_GOTO_TARGET	\
   | BB_HOT_PARTITION | BB_COLD_PARTITION)

/* Dummy bitmask for convenience in the hot/cold partitioning code.  */
#define BB_UNPARTITIONED	0

/* Partitions, to be used when partitioning hot and cold basic blocks into
   separate sections.  */
#define BB_PARTITION(bb) ((bb)->flags & (BB_HOT_PARTITION|BB_COLD_PARTITION))
#define BB_SET_PARTITION(bb, part) do {					\
  basic_block bb_ = (bb);						\
  bb_->flags = ((bb_->flags & ~(BB_HOT_PARTITION|BB_COLD_PARTITION))	\
		| (part));						\
} while (0)

#define BB_COPY_PARTITION(dstbb, srcbb) \
  BB_SET_PARTITION (dstbb, BB_PARTITION (srcbb))

/* State of dominance information.  */

enum dom_state
{
  DOM_NONE,		/* Not computed at all.  */
  DOM_NO_FAST_QUERY,	/* The data is OK, but the fast query data are not usable.  */
  DOM_OK		/* Everything is ok.  */
};

/* What sort of profiling information we have.  */
enum profile_status_d
{
  PROFILE_ABSENT,
  PROFILE_GUESSED,
  PROFILE_READ,
  PROFILE_LAST	/* Last value, used by profile streaming.  */
};

/* A structure to group all the per-function control flow graph data.
   The x_* prefixing is necessary because otherwise references to the
   fields of this struct are interpreted as the defines for backward
   source compatibility following the definition of this struct.  */
struct GTY(()) control_flow_graph {
  /* Block pointers for the exit and entry of a function.
     These are always the head and tail of the basic block list.  */
  basic_block x_entry_block_ptr;
  basic_block x_exit_block_ptr;

  /* Index by basic block number, get basic block struct info.  */
  vec<basic_block, va_gc> *x_basic_block_info;

  /* Number of basic blocks in this flow graph.  */
  int x_n_basic_blocks;

  /* Number of edges in this flow graph.  */
  int x_n_edges;

  /* The first free basic block number.  */
  int x_last_basic_block;

  /* UIDs for LABEL_DECLs.  */
  int last_label_uid;

  /* Mapping of labels to their associated blocks.  At present
     only used for the gimple CFG.  */
  vec<basic_block, va_gc> *x_label_to_block_map;

  enum profile_status_d x_profile_status;

  /* Whether the dominators and the postdominators are available.  */
  enum dom_state x_dom_computed[2];

  /* Number of basic blocks in the dominance tree.  */
  unsigned x_n_bbs_in_dom_tree[2];

  /* Maximal number of entities in the single jumptable.  Used to estimate
     final flowgraph size.  */
  int max_jumptable_ents;
};

/* Defines for accessing the fields of the CFG structure for function FN.  */
#define ENTRY_BLOCK_PTR_FOR_FUNCTION(FN)     ((FN)->cfg->x_entry_block_ptr)
#define EXIT_BLOCK_PTR_FOR_FUNCTION(FN)	     ((FN)->cfg->x_exit_block_ptr)
#define basic_block_info_for_function(FN)    ((FN)->cfg->x_basic_block_info)
#define n_basic_blocks_for_function(FN)	     ((FN)->cfg->x_n_basic_blocks)
#define n_edges_for_function(FN)	     ((FN)->cfg->x_n_edges)
#define last_basic_block_for_function(FN)    ((FN)->cfg->x_last_basic_block)
#define label_to_block_map_for_function(FN)  ((FN)->cfg->x_label_to_block_map)
#define profile_status_for_function(FN)	     ((FN)->cfg->x_profile_status)

#define BASIC_BLOCK_FOR_FUNCTION(FN,N) \
  ((*basic_block_info_for_function(FN))[(N)])
#define SET_BASIC_BLOCK_FOR_FUNCTION(FN,N,BB) \
  ((*basic_block_info_for_function(FN))[(N)] = (BB))

/* Defines for textual backward source compatibility.  */
#define ENTRY_BLOCK_PTR		(cfun->cfg->x_entry_block_ptr)
#define EXIT_BLOCK_PTR		(cfun->cfg->x_exit_block_ptr)
#define basic_block_info	(cfun->cfg->x_basic_block_info)
#define n_basic_blocks		(cfun->cfg->x_n_basic_blocks)
#define n_edges			(cfun->cfg->x_n_edges)
#define last_basic_block	(cfun->cfg->x_last_basic_block)
#define label_to_block_map	(cfun->cfg->x_label_to_block_map)
#define profile_status		(cfun->cfg->x_profile_status)

#define BASIC_BLOCK(N)		((*basic_block_info)[(N)])
#define SET_BASIC_BLOCK(N,BB)	((*basic_block_info)[(N)] = (BB))

/* For iterating over basic blocks.  */
#define FOR_BB_BETWEEN(BB, FROM, TO, DIR) \
  for (BB = FROM; BB != TO; BB = BB->DIR)

#define FOR_EACH_BB_FN(BB, FN) \
  FOR_BB_BETWEEN (BB, (FN)->cfg->x_entry_block_ptr->next_bb, (FN)->cfg->x_exit_block_ptr, next_bb)

#define FOR_EACH_BB(BB) FOR_EACH_BB_FN (BB, cfun)

#define FOR_EACH_BB_REVERSE_FN(BB, FN) \
  FOR_BB_BETWEEN (BB, (FN)->cfg->x_exit_block_ptr->prev_bb, (FN)->cfg->x_entry_block_ptr, prev_bb)

#define FOR_EACH_BB_REVERSE(BB) FOR_EACH_BB_REVERSE_FN(BB, cfun)

/* For iterating over insns in basic block.  */
#define FOR_BB_INSNS(BB, INSN)			\
  for ((INSN) = BB_HEAD (BB);			\
       (INSN) && (INSN) != NEXT_INSN (BB_END (BB));	\
       (INSN) = NEXT_INSN (INSN))

/* For iterating over insns in basic block when we might remove the
   current insn.  */
#define FOR_BB_INSNS_SAFE(BB, INSN, CURR)			\
  for ((INSN) = BB_HEAD (BB), (CURR) = (INSN) ? NEXT_INSN ((INSN)): NULL;	\
       (INSN) && (INSN) != NEXT_INSN (BB_END (BB));	\
       (INSN) = (CURR), (CURR) = (INSN) ? NEXT_INSN ((INSN)) : NULL)

#define FOR_BB_INSNS_REVERSE(BB, INSN)		\
  for ((INSN) = BB_END (BB);			\
       (INSN) && (INSN) != PREV_INSN (BB_HEAD (BB));	\
       (INSN) = PREV_INSN (INSN))

#define FOR_BB_INSNS_REVERSE_SAFE(BB, INSN, CURR)	\
  for ((INSN) = BB_END (BB),(CURR) = (INSN) ? PREV_INSN ((INSN)) : NULL;	\
       (INSN) && (INSN) != PREV_INSN (BB_HEAD (BB));	\
       (INSN) = (CURR), (CURR) = (INSN) ? PREV_INSN ((INSN)) : NULL)

/* Cycles through _all_ basic blocks, even the fake ones (entry and
   exit block).  */

#define FOR_ALL_BB(BB) \
  for (BB = ENTRY_BLOCK_PTR; BB; BB = BB->next_bb)

#define FOR_ALL_BB_FN(BB, FN) \
  for (BB = ENTRY_BLOCK_PTR_FOR_FUNCTION (FN); BB; BB = BB->next_bb)


/* Stuff for recording basic block info.  */

#define BB_HEAD(B)      (B)->il.x.head_
#define BB_END(B)       (B)->il.x.rtl->end_
#define BB_HEADER(B)    (B)->il.x.rtl->header_
#define BB_FOOTER(B)    (B)->il.x.rtl->footer_

/* Special block numbers [markers] for entry and exit.
   Neither of them is supposed to hold actual statements.  */
#define ENTRY_BLOCK (0)
#define EXIT_BLOCK (1)

/* The two blocks that are always in the cfg.  */
#define NUM_FIXED_BLOCKS (2)

#define set_block_for_insn(INSN, BB)  (BLOCK_FOR_INSN (INSN) = BB)

extern void compute_bb_for_insn (void);
extern unsigned int free_bb_for_insn (void);
extern void update_bb_for_insn (basic_block);

extern void insert_insn_on_edge (rtx, edge);
basic_block split_edge_and_insert (edge, rtx);

extern void commit_one_edge_insertion (edge e);
extern void commit_edge_insertions (void);

extern edge unchecked_make_edge (basic_block, basic_block, int);
extern edge cached_make_edge (sbitmap, basic_block, basic_block, int);
extern edge make_edge (basic_block, basic_block, int);
extern edge make_single_succ_edge (basic_block, basic_block, int);
extern void remove_edge_raw (edge);
extern void redirect_edge_succ (edge, basic_block);
extern edge redirect_edge_succ_nodup (edge, basic_block);
extern void redirect_edge_pred (edge, basic_block);
extern basic_block create_basic_block_structure (rtx, rtx, rtx, basic_block);
extern void clear_bb_flags (void);
extern void dump_bb_info (FILE *, basic_block, int, int, bool, bool);
extern void dump_edge_info (FILE *, edge, int, int);
extern void brief_dump_cfg (FILE *, int);
extern void clear_edges (void);
extern void scale_bbs_frequencies_int (basic_block *, int, int, int);
extern void scale_bbs_frequencies_gcov_type (basic_block *, int, gcov_type,
					     gcov_type);

/* Structure to group all of the information to process IF-THEN and
   IF-THEN-ELSE blocks for the conditional execution support.  This
   needs to be in a public file in case the IFCVT macros call
   functions passing the ce_if_block data structure.  */

typedef struct ce_if_block
{
  basic_block test_bb;			/* First test block.  */
  basic_block then_bb;			/* THEN block.  */
  basic_block else_bb;			/* ELSE block or NULL.  */
  basic_block join_bb;			/* Join THEN/ELSE blocks.  */
  basic_block last_test_bb;		/* Last bb to hold && or || tests.  */
  int num_multiple_test_blocks;		/* # of && and || basic blocks.  */
  int num_and_and_blocks;		/* # of && blocks.  */
  int num_or_or_blocks;			/* # of || blocks.  */
  int num_multiple_test_insns;		/* # of insns in && and || blocks.  */
  int and_and_p;			/* Complex test is &&.  */
  int num_then_insns;			/* # of insns in THEN block.  */
  int num_else_insns;			/* # of insns in ELSE block.  */
  int pass;				/* Pass number.  */
} ce_if_block_t;

/* This structure maintains an edge list vector.  */
/* FIXME: Make this a vec<edge>.  */
struct edge_list
{
  int num_edges;
  edge *index_to_edge;
};

/* The base value for branch probability notes and edge probabilities.  */
#define REG_BR_PROB_BASE  10000

/* This is the value which indicates no edge is present.  */
#define EDGE_INDEX_NO_EDGE	-1

/* EDGE_INDEX returns an integer index for an edge, or EDGE_INDEX_NO_EDGE
   if there is no edge between the 2 basic blocks.  */
#define EDGE_INDEX(el, pred, succ) (find_edge_index ((el), (pred), (succ)))

/* INDEX_EDGE_PRED_BB and INDEX_EDGE_SUCC_BB return a pointer to the basic
   block which is either the pred or succ end of the indexed edge.  */
#define INDEX_EDGE_PRED_BB(el, index)	((el)->index_to_edge[(index)]->src)
#define INDEX_EDGE_SUCC_BB(el, index)	((el)->index_to_edge[(index)]->dest)

/* INDEX_EDGE returns a pointer to the edge.  */
#define INDEX_EDGE(el, index)           ((el)->index_to_edge[(index)])

/* Number of edges in the compressed edge list.  */
#define NUM_EDGES(el)			((el)->num_edges)

/* BB is assumed to contain conditional jump.  Return the fallthru edge.  */
#define FALLTHRU_EDGE(bb)		(EDGE_SUCC ((bb), 0)->flags & EDGE_FALLTHRU \
					 ? EDGE_SUCC ((bb), 0) : EDGE_SUCC ((bb), 1))

/* BB is assumed to contain conditional jump.  Return the branch edge.  */
#define BRANCH_EDGE(bb)			(EDGE_SUCC ((bb), 0)->flags & EDGE_FALLTHRU \
					 ? EDGE_SUCC ((bb), 1) : EDGE_SUCC ((bb), 0))

#define RDIV(X,Y) (((X) + (Y) / 2) / (Y))
/* Return expected execution frequency of the edge E.  */
#define EDGE_FREQUENCY(e)		RDIV ((e)->src->frequency * (e)->probability, \
					      REG_BR_PROB_BASE)

/* Return nonzero if edge is critical.  */
#define EDGE_CRITICAL_P(e)		(EDGE_COUNT ((e)->src->succs) >= 2 \
					 && EDGE_COUNT ((e)->dest->preds) >= 2)

#define EDGE_COUNT(ev)			vec_safe_length (ev)
#define EDGE_I(ev,i)			(*ev)[(i)]
#define EDGE_PRED(bb,i)			(*(bb)->preds)[(i)]
#define EDGE_SUCC(bb,i)			(*(bb)->succs)[(i)]

/* Returns true if BB has precisely one successor.  */

static inline bool
single_succ_p (const_basic_block bb)
{
  return EDGE_COUNT (bb->succs) == 1;
}

/* Returns true if BB has precisely one predecessor.  */

static inline bool
single_pred_p (const_basic_block bb)
{
  return EDGE_COUNT (bb->preds) == 1;
}

/* Returns the single successor edge of basic block BB.  Aborts if
   BB does not have exactly one successor.  */

static inline edge
single_succ_edge (const_basic_block bb)
{
  gcc_checking_assert (single_succ_p (bb));
  return EDGE_SUCC (bb, 0);
}

/* Returns the single predecessor edge of basic block BB.  Aborts
   if BB does not have exactly one predecessor.  */

static inline edge
single_pred_edge (const_basic_block bb)
{
  gcc_checking_assert (single_pred_p (bb));
  return EDGE_PRED (bb, 0);
}

/* Returns the single successor block of basic block BB.  Aborts
   if BB does not have exactly one successor.  */

static inline basic_block
single_succ (const_basic_block bb)
{
  return single_succ_edge (bb)->dest;
}

/* Returns the single predecessor block of basic block BB.  Aborts
   if BB does not have exactly one predecessor.*/

static inline basic_block
single_pred (const_basic_block bb)
{
  return single_pred_edge (bb)->src;
}

/* Iterator object for edges.  */

typedef struct {
  unsigned index;
  vec<edge, va_gc> **container;
} edge_iterator;

static inline vec<edge, va_gc> *
ei_container (edge_iterator i)
{
  gcc_checking_assert (i.container);
  return *i.container;
}

#define ei_start(iter) ei_start_1 (&(iter))
#define ei_last(iter) ei_last_1 (&(iter))

/* Return an iterator pointing to the start of an edge vector.  */
static inline edge_iterator
ei_start_1 (vec<edge, va_gc> **ev)
{
  edge_iterator i;

  i.index = 0;
  i.container = ev;

  return i;
}

/* Return an iterator pointing to the last element of an edge
   vector.  */
static inline edge_iterator
ei_last_1 (vec<edge, va_gc> **ev)
{
  edge_iterator i;

  i.index = EDGE_COUNT (*ev) - 1;
  i.container = ev;

  return i;
}

/* Is the iterator `i' at the end of the sequence?  */
static inline bool
ei_end_p (edge_iterator i)
{
  return (i.index == EDGE_COUNT (ei_container (i)));
}

/* Is the iterator `i' at one position before the end of the
   sequence?  */
static inline bool
ei_one_before_end_p (edge_iterator i)
{
  return (i.index + 1 == EDGE_COUNT (ei_container (i)));
}

/* Advance the iterator to the next element.  */
static inline void
ei_next (edge_iterator *i)
{
  gcc_checking_assert (i->index < EDGE_COUNT (ei_container (*i)));
  i->index++;
}

/* Move the iterator to the previous element.  */
static inline void
ei_prev (edge_iterator *i)
{
  gcc_checking_assert (i->index > 0);
  i->index--;
}

/* Return the edge pointed to by the iterator `i'.  */
static inline edge
ei_edge (edge_iterator i)
{
  return EDGE_I (ei_container (i), i.index);
}

/* Return an edge pointed to by the iterator.  Do it safely so that
   NULL is returned when the iterator is pointing at the end of the
   sequence.  */
static inline edge
ei_safe_edge (edge_iterator i)
{
  return !ei_end_p (i) ? ei_edge (i) : NULL;
}

/* Return 1 if we should continue to iterate.  Return 0 otherwise.
   *Edge P is set to the next edge if we are to continue to iterate
   and NULL otherwise.  */

static inline bool
ei_cond (edge_iterator ei, edge *p)
{
  if (!ei_end_p (ei))
    {
      *p = ei_edge (ei);
      return 1;
    }
  else
    {
      *p = NULL;
      return 0;
    }
}

/* This macro serves as a convenient way to iterate each edge in a
   vector of predecessor or successor edges.  It must not be used when
   an element might be removed during the traversal, otherwise
   elements will be missed.  Instead, use a for-loop like that shown
   in the following pseudo-code:

   FOR (ei = ei_start (bb->succs); (e = ei_safe_edge (ei)); )
     {
	IF (e != taken_edge)
	  remove_edge (e);
	ELSE
	  ei_next (&ei);
     }
*/

#define FOR_EACH_EDGE(EDGE,ITER,EDGE_VEC)	\
  for ((ITER) = ei_start ((EDGE_VEC));		\
       ei_cond ((ITER), &(EDGE));		\
       ei_next (&(ITER)))

#define CLEANUP_EXPENSIVE	1	/* Do relatively expensive optimizations
					   except for edge forwarding */
#define CLEANUP_CROSSJUMP	2	/* Do crossjumping.  */
#define CLEANUP_POST_REGSTACK	4	/* We run after reg-stack and need
					   to care REG_DEAD notes.  */
#define CLEANUP_THREADING	8	/* Do jump threading.  */
#define CLEANUP_NO_INSN_DEL	16	/* Do not try to delete trivially dead
					   insns.  */
#define CLEANUP_CFGLAYOUT	32	/* Do cleanup in cfglayout mode.  */
#define CLEANUP_CFG_CHANGED	64      /* The caller changed the CFG.  */

/* In cfganal.c */
extern void bitmap_intersection_of_succs (sbitmap, sbitmap *, basic_block);
extern void bitmap_intersection_of_preds (sbitmap, sbitmap *, basic_block);
extern void bitmap_union_of_succs (sbitmap, sbitmap *, basic_block);
extern void bitmap_union_of_preds (sbitmap, sbitmap *, basic_block);

/* In lcm.c */
extern struct edge_list *pre_edge_lcm (int, sbitmap *, sbitmap *,
				       sbitmap *, sbitmap *, sbitmap **,
				       sbitmap **);
extern struct edge_list *pre_edge_rev_lcm (int, sbitmap *,
					   sbitmap *, sbitmap *,
					   sbitmap *, sbitmap **,
					   sbitmap **);
extern void compute_available (sbitmap *, sbitmap *, sbitmap *, sbitmap *);

/* In predict.c */
extern bool maybe_hot_bb_p (struct function *, const_basic_block);
extern bool maybe_hot_edge_p (edge);
extern bool probably_never_executed_bb_p (struct function *, const_basic_block);
extern bool optimize_bb_for_size_p (const_basic_block);
extern bool optimize_bb_for_speed_p (const_basic_block);
extern bool optimize_edge_for_size_p (edge);
extern bool optimize_edge_for_speed_p (edge);
extern bool optimize_loop_for_size_p (struct loop *);
extern bool optimize_loop_for_speed_p (struct loop *);
extern bool optimize_loop_nest_for_size_p (struct loop *);
extern bool optimize_loop_nest_for_speed_p (struct loop *);
extern bool gimple_predicted_by_p (const_basic_block, enum br_predictor);
extern bool rtl_predicted_by_p (const_basic_block, enum br_predictor);
extern void gimple_predict_edge (edge, enum br_predictor, int);
extern void rtl_predict_edge (edge, enum br_predictor, int);
extern void predict_edge_def (edge, enum br_predictor, enum prediction);
extern void guess_outgoing_edge_probabilities (basic_block);
extern void remove_predictions_associated_with_edge (edge);
extern bool edge_probability_reliable_p (const_edge);
extern bool br_prob_note_reliable_p (const_rtx);
extern bool predictable_edge_p (edge);

/* In cfg.c  */
extern void init_flow (struct function *);
extern void debug_bb (basic_block);
extern basic_block debug_bb_n (int);
extern void dump_flow_info (FILE *, int);
extern void expunge_block (basic_block);
extern void link_block (basic_block, basic_block);
extern void unlink_block (basic_block);
extern void compact_blocks (void);
extern basic_block alloc_block (void);
extern void alloc_aux_for_blocks (int);
extern void clear_aux_for_blocks (void);
extern void free_aux_for_blocks (void);
extern void alloc_aux_for_edge (edge, int);
extern void alloc_aux_for_edges (int);
extern void clear_aux_for_edges (void);
extern void free_aux_for_edges (void);

/* In cfganal.c  */
extern void find_unreachable_blocks (void);
extern bool mark_dfs_back_edges (void);
struct edge_list * create_edge_list (void);
void free_edge_list (struct edge_list *);
void print_edge_list (FILE *, struct edge_list *);
void verify_edge_list (FILE *, struct edge_list *);
int find_edge_index (struct edge_list *, basic_block, basic_block);
edge find_edge (basic_block, basic_block);
extern void remove_fake_edges (void);
extern void remove_fake_exit_edges (void);
extern void add_noreturn_fake_exit_edges (void);
extern void connect_infinite_loops_to_exit (void);
extern int post_order_compute (int *, bool, bool);
extern basic_block dfs_find_deadend (basic_block);
extern int inverted_post_order_compute (int *);
extern int pre_and_rev_post_order_compute (int *, int *, bool);
extern int dfs_enumerate_from (basic_block, int,
			       bool (*)(const_basic_block, const void *),
			       basic_block *, int, const void *);
extern void compute_dominance_frontiers (struct bitmap_head_def *);
extern bitmap compute_idf (bitmap, struct bitmap_head_def *);

/* In cfgrtl.c  */
extern rtx block_label (basic_block);
extern rtx bb_note (basic_block);
extern bool purge_all_dead_edges (void);
extern bool purge_dead_edges (basic_block);
extern bool fixup_abnormal_edges (void);
extern basic_block force_nonfallthru_and_redirect (edge, basic_block, rtx);
extern bool contains_no_active_insn_p (const_basic_block);
extern bool forwarder_block_p (const_basic_block);
extern bool can_fallthru (basic_block, basic_block);

/* In cfgbuild.c.  */
extern void find_many_sub_basic_blocks (sbitmap);
extern void rtl_make_eh_edge (sbitmap, basic_block, rtx);

enum replace_direction { dir_none, dir_forward, dir_backward, dir_both };

/* In cfgcleanup.c.  */
extern bool cleanup_cfg (int);
extern int flow_find_cross_jump (basic_block, basic_block, rtx *, rtx *,
                                 enum replace_direction*);
extern int flow_find_head_matching_sequence (basic_block, basic_block,
					     rtx *, rtx *, int);

extern bool delete_unreachable_blocks (void);

extern void update_br_prob_note (basic_block);
extern bool inside_basic_block_p (const_rtx);
extern bool control_flow_insn_p (const_rtx);
extern rtx get_last_bb_insn (basic_block);

/* In dominance.c */

enum cdi_direction
{
  CDI_DOMINATORS = 1,
  CDI_POST_DOMINATORS = 2
};

extern enum dom_state dom_info_state (enum cdi_direction);
extern void set_dom_info_availability (enum cdi_direction, enum dom_state);
extern bool dom_info_available_p (enum cdi_direction);
extern void calculate_dominance_info (enum cdi_direction);
extern void free_dominance_info (enum cdi_direction);
extern basic_block nearest_common_dominator (enum cdi_direction,
					     basic_block, basic_block);
extern basic_block nearest_common_dominator_for_set (enum cdi_direction,
						     bitmap);
extern void set_immediate_dominator (enum cdi_direction, basic_block,
				     basic_block);
extern basic_block get_immediate_dominator (enum cdi_direction, basic_block);
extern bool dominated_by_p (enum cdi_direction, const_basic_block, const_basic_block);
extern vec<basic_block> get_dominated_by (enum cdi_direction, basic_block);
extern vec<basic_block> get_dominated_by_region (enum cdi_direction,
							 basic_block *,
							 unsigned);
extern vec<basic_block> get_dominated_to_depth (enum cdi_direction,
							basic_block, int);
extern vec<basic_block> get_all_dominated_blocks (enum cdi_direction,
							  basic_block);
extern void add_to_dominance_info (enum cdi_direction, basic_block);
extern void delete_from_dominance_info (enum cdi_direction, basic_block);
basic_block recompute_dominator (enum cdi_direction, basic_block);
extern void redirect_immediate_dominators (enum cdi_direction, basic_block,
					   basic_block);
extern void iterate_fix_dominators (enum cdi_direction,
				    vec<basic_block> , bool);
extern void verify_dominators (enum cdi_direction);
extern basic_block first_dom_son (enum cdi_direction, basic_block);
extern basic_block next_dom_son (enum cdi_direction, basic_block);
unsigned bb_dom_dfs_in (enum cdi_direction, basic_block);
unsigned bb_dom_dfs_out (enum cdi_direction, basic_block);

extern edge try_redirect_by_replacing_jump (edge, basic_block, bool);
extern void break_superblocks (void);
extern void relink_block_chain (bool);
extern void update_bb_profile_for_threading (basic_block, int, gcov_type, edge);
extern void init_rtl_bb_info (basic_block);

extern void initialize_original_copy_tables (void);
extern void free_original_copy_tables (void);
extern void set_bb_original (basic_block, basic_block);
extern basic_block get_bb_original (basic_block);
extern void set_bb_copy (basic_block, basic_block);
extern basic_block get_bb_copy (basic_block);
void set_loop_copy (struct loop *, struct loop *);
struct loop *get_loop_copy (struct loop *);

#include "cfghooks.h"

/* Return true when one of the predecessor edges of BB is marked with EDGE_EH.  */
static inline bool
bb_has_eh_pred (basic_block bb)
{
  edge e;
  edge_iterator ei;

  FOR_EACH_EDGE (e, ei, bb->preds)
    {
      if (e->flags & EDGE_EH)
	return true;
    }
  return false;
}

/* Return true when one of the predecessor edges of BB is marked with EDGE_ABNORMAL.  */
static inline bool
bb_has_abnormal_pred (basic_block bb)
{
  edge e;
  edge_iterator ei;

  FOR_EACH_EDGE (e, ei, bb->preds)
    {
      if (e->flags & EDGE_ABNORMAL)
	return true;
    }
  return false;
}

/* Return the fallthru edge in EDGES if it exists, NULL otherwise.  */
static inline edge
find_fallthru_edge (vec<edge, va_gc> *edges)
{
  edge e;
  edge_iterator ei;

  FOR_EACH_EDGE (e, ei, edges)
    if (e->flags & EDGE_FALLTHRU)
      break;

  return e;
}

/* In cfgloopmanip.c.  */
extern edge mfb_kj_edge;
extern bool mfb_keep_just (edge);

/* In cfgexpand.c.  */
extern void rtl_profile_for_bb (basic_block);
extern void rtl_profile_for_edge (edge);
extern void default_rtl_profile (void);

/* In profile.c.  */
extern gcov_working_set_t *find_working_set(unsigned pct_times_10);

/* Check tha probability is sane.  */

static inline void
check_probability (int prob)
{
  gcc_checking_assert (prob >= 0 && prob <= REG_BR_PROB_BASE);
}

/* Given PROB1 and PROB2, return PROB1*PROB2/REG_BR_PROB_BASE. 
   Used to combine BB probabilities.  */

static inline int
combine_probabilities (int prob1, int prob2)
{
  check_probability (prob1);
  check_probability (prob2);
  return RDIV (prob1 * prob2, REG_BR_PROB_BASE);
}

/* Apply probability PROB on frequency or count FREQ.  */

static inline gcov_type
apply_probability (gcov_type freq, int prob)
{
  check_probability (prob);
  return RDIV (freq * prob, REG_BR_PROB_BASE);
}

/* Return inverse probability for PROB.  */

static inline int
inverse_probability (int prob1)
{
  check_probability (prob1);
  return REG_BR_PROB_BASE - prob1;
}
#endif /* GCC_BASIC_BLOCK_H */
