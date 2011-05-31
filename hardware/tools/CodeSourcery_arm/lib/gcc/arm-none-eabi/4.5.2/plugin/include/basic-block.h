/* Define control and data flow tables, and regsets.
   Copyright (C) 1987, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004,
   2005, 2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.

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

#include "bitmap.h"
#include "sbitmap.h"
#include "varray.h"
#include "partition.h"
#include "hard-reg-set.h"
#include "predict.h"
#include "vec.h"
#include "function.h"

/* Head of register set linked list.  */
typedef bitmap_head regset_head;

/* A pointer to a regset_head.  */
typedef bitmap regset;

/* Allocate a register set with oballoc.  */
#define ALLOC_REG_SET(OBSTACK) BITMAP_ALLOC (OBSTACK)

/* Do any cleanup needed on a regset when it is no longer used.  */
#define FREE_REG_SET(REGSET) BITMAP_FREE (REGSET)

/* Initialize a new regset.  */
#define INIT_REG_SET(HEAD) bitmap_initialize (HEAD, &reg_obstack)

/* Clear a register set by freeing up the linked list.  */
#define CLEAR_REG_SET(HEAD) bitmap_clear (HEAD)

/* Copy a register set to another register set.  */
#define COPY_REG_SET(TO, FROM) bitmap_copy (TO, FROM)

/* Compare two register sets.  */
#define REG_SET_EQUAL_P(A, B) bitmap_equal_p (A, B)

/* `and' a register set with a second register set.  */
#define AND_REG_SET(TO, FROM) bitmap_and_into (TO, FROM)

/* `and' the complement of a register set with a register set.  */
#define AND_COMPL_REG_SET(TO, FROM) bitmap_and_compl_into (TO, FROM)

/* Inclusive or a register set with a second register set.  */
#define IOR_REG_SET(TO, FROM) bitmap_ior_into (TO, FROM)

/* Exclusive or a register set with a second register set.  */
#define XOR_REG_SET(TO, FROM) bitmap_xor_into (TO, FROM)

/* Or into TO the register set FROM1 `and'ed with the complement of FROM2.  */
#define IOR_AND_COMPL_REG_SET(TO, FROM1, FROM2) \
  bitmap_ior_and_compl_into (TO, FROM1, FROM2)

/* Clear a single register in a register set.  */
#define CLEAR_REGNO_REG_SET(HEAD, REG) bitmap_clear_bit (HEAD, REG)

/* Set a single register in a register set.  */
#define SET_REGNO_REG_SET(HEAD, REG) bitmap_set_bit (HEAD, REG)

/* Return true if a register is set in a register set.  */
#define REGNO_REG_SET_P(TO, REG) bitmap_bit_p (TO, REG)

/* Copy the hard registers in a register set to the hard register set.  */
extern void reg_set_to_hard_reg_set (HARD_REG_SET *, const_bitmap);
#define REG_SET_TO_HARD_REG_SET(TO, FROM)				\
do {									\
  CLEAR_HARD_REG_SET (TO);						\
  reg_set_to_hard_reg_set (&TO, FROM);					\
} while (0)

typedef bitmap_iterator reg_set_iterator;

/* Loop over all registers in REGSET, starting with MIN, setting REGNUM to the
   register number and executing CODE for all registers that are set.  */
#define EXECUTE_IF_SET_IN_REG_SET(REGSET, MIN, REGNUM, RSI)	\
  EXECUTE_IF_SET_IN_BITMAP (REGSET, MIN, REGNUM, RSI)

/* Loop over all registers in REGSET1 and REGSET2, starting with MIN, setting
   REGNUM to the register number and executing CODE for all registers that are
   set in the first regset and not set in the second.  */
#define EXECUTE_IF_AND_COMPL_IN_REG_SET(REGSET1, REGSET2, MIN, REGNUM, RSI) \
  EXECUTE_IF_AND_COMPL_IN_BITMAP (REGSET1, REGSET2, MIN, REGNUM, RSI)

/* Loop over all registers in REGSET1 and REGSET2, starting with MIN, setting
   REGNUM to the register number and executing CODE for all registers that are
   set in both regsets.  */
#define EXECUTE_IF_AND_IN_REG_SET(REGSET1, REGSET2, MIN, REGNUM, RSI) \
  EXECUTE_IF_AND_IN_BITMAP (REGSET1, REGSET2, MIN, REGNUM, RSI)	\

/* Same information as REGS_INVALIDATED_BY_CALL but in regset form to be used
   in dataflow more conveniently.  */

extern regset regs_invalidated_by_call_regset;

/* Type we use to hold basic block counters.  Should be at least
   64bit.  Although a counter cannot be negative, we use a signed
   type, because erroneous negative counts can be generated when the
   flow graph is manipulated by various optimizations.  A signed type
   makes those easy to detect.  */
typedef HOST_WIDEST_INT gcov_type;

/* Control flow edge information.  */
struct GTY(()) edge_def {
  /* The two blocks at the ends of the edge.  */
  struct basic_block_def *src;
  struct basic_block_def *dest;

  /* Instructions queued on the edge.  */
  union edge_def_insns {
    gimple_seq GTY ((tag ("true"))) g;
    rtx GTY ((tag ("false"))) r;
  } GTY ((desc ("current_ir_type () == IR_GIMPLE"))) insns;

  /* Auxiliary info specific to a pass.  */
  PTR GTY ((skip (""))) aux;

  /* Location of any goto implicit in the edge and associated BLOCK.  */
  tree goto_block;
  location_t goto_locus;

  /* The index number corresponding to this edge in the edge vector
     dest->preds.  */
  unsigned int dest_idx;

  int flags;			/* see EDGE_* below  */
  int probability;		/* biased by REG_BR_PROB_BASE */
  gcov_type count;		/* Expected number of executions calculated
				   in profile.c  */
};

DEF_VEC_P(edge);
DEF_VEC_ALLOC_P(edge,gc);
DEF_VEC_ALLOC_P(edge,heap);

#define EDGE_FALLTHRU		1	/* 'Straight line' flow */
#define EDGE_ABNORMAL		2	/* Strange flow, like computed
					   label, or eh */
#define EDGE_ABNORMAL_CALL	4	/* Call with abnormal exit
					   like an exception, or sibcall */
#define EDGE_EH			8	/* Exception throw */
#define EDGE_FAKE		16	/* Not a real edge (profile.c) */
#define EDGE_DFS_BACK		32	/* A backwards edge */
#define EDGE_CAN_FALLTHRU	64	/* Candidate for straight line
					   flow.  */
#define EDGE_IRREDUCIBLE_LOOP	128	/* Part of irreducible loop.  */
#define EDGE_SIBCALL		256	/* Edge from sibcall to exit.  */
#define EDGE_LOOP_EXIT		512	/* Exit of a loop.  */
#define EDGE_TRUE_VALUE		1024	/* Edge taken when controlling
					   predicate is nonzero.  */
#define EDGE_FALSE_VALUE	2048	/* Edge taken when controlling
					   predicate is zero.  */
#define EDGE_EXECUTABLE		4096	/* Edge is executable.  Only
					   valid during SSA-CCP.  */
#define EDGE_CROSSING		8192    /* Edge crosses between hot
					   and cold sections, when we
					   do partitioning.  */
#define EDGE_ALL_FLAGS	       16383

#define EDGE_COMPLEX	(EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_EH)

/* Counter summary from the last set of coverage counts read by
   profile.c.  */
extern const struct gcov_ctr_summary *profile_info;

/* Declared in cfgloop.h.  */
struct loop;

/* Declared in tree-flow.h.  */
struct edge_prediction;
struct rtl_bb_info;

/* A basic block is a sequence of instructions with only entry and
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
  VEC(edge,gc) *preds;
  VEC(edge,gc) *succs;

  /* Auxiliary info specific to a pass.  */
  PTR GTY ((skip (""))) aux;

  /* Innermost loop containing the block.  */
  struct loop *loop_father;

  /* The dominance and postdominance information node.  */
  struct et_node * GTY ((skip (""))) dom[2];

  /* Previous and next blocks in the chain.  */
  struct basic_block_def *prev_bb;
  struct basic_block_def *next_bb;

  union basic_block_il_dependent {
      struct gimple_bb_info * GTY ((tag ("0"))) gimple;
      struct rtl_bb_info * GTY ((tag ("1"))) rtl;
    } GTY ((desc ("((%1.flags & BB_RTL) != 0)"))) il;

  /* Expected number of executions: calculated in profile.c.  */
  gcov_type count;

  /* The index of this block.  */
  int index;

  /* The loop depth of this block.  */
  int loop_depth;

  /* Expected frequency.  Normalized to be in range 0 to BB_FREQ_MAX.  */
  int frequency;

  /* The discriminator for this block.  */
  int discriminator;

  /* Various flags.  See BB_* below.  */
  int flags;
};

struct GTY(()) rtl_bb_info {
  /* The first and last insns of the block.  */
  rtx head_;
  rtx end_;

  /* In CFGlayout mode points to insn notes/jumptables to be placed just before
     and after the block.   */
  rtx header;
  rtx footer;

  /* This field is used by the bb-reorder and tracer passes.  */
  int visited;
};

struct GTY(()) gimple_bb_info {
  /* Sequence of statements in this block.  */
  gimple_seq seq;

  /* PHI nodes for this block.  */
  gimple_seq phi_nodes;
};

DEF_VEC_P(basic_block);
DEF_VEC_ALLOC_P(basic_block,gc);
DEF_VEC_ALLOC_P(basic_block,heap);

#define BB_FREQ_MAX 10000

/* Masks for basic_block.flags.

   BB_HOT_PARTITION and BB_COLD_PARTITION should be preserved throughout
   the compilation, so they are never cleared.

   All other flags may be cleared by clear_bb_flags().  It is generally
   a bad idea to rely on any flags being up-to-date.  */

enum bb_flags
{
  /* Only set on blocks that have just been created by create_bb.  */
  BB_NEW = 1 << 0,

  /* Set by find_unreachable_blocks.  Do not rely on this being set in any
     pass.  */
  BB_REACHABLE = 1 << 1,

  /* Set for blocks in an irreducible loop by loop analysis.  */
  BB_IRREDUCIBLE_LOOP = 1 << 2,

  /* Set on blocks that may actually not be single-entry single-exit block.  */
  BB_SUPERBLOCK = 1 << 3,

  /* Set on basic blocks that the scheduler should not touch.  This is used
     by SMS to prevent other schedulers from messing with the loop schedule.  */
  BB_DISABLE_SCHEDULE = 1 << 4,

  /* Set on blocks that should be put in a hot section.  */
  BB_HOT_PARTITION = 1 << 5,

  /* Set on blocks that should be put in a cold section.  */
  BB_COLD_PARTITION = 1 << 6,

  /* Set on block that was duplicated.  */
  BB_DUPLICATED = 1 << 7,

  /* Set if the label at the top of this block is the target of a non-local goto.  */
  BB_NON_LOCAL_GOTO_TARGET = 1 << 8,

  /* Set on blocks that are in RTL format.  */
  BB_RTL = 1 << 9 ,

  /* Set on blocks that are forwarder blocks.
     Only used in cfgcleanup.c.  */
  BB_FORWARDER_BLOCK = 1 << 10,

  /* Set on blocks that cannot be threaded through.
     Only used in cfgcleanup.c.  */
  BB_NONTHREADABLE_BLOCK = 1 << 11
};

/* Dummy flag for convenience in the hot/cold partitioning code.  */
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
  PROFILE_READ
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
  VEC(basic_block,gc) *x_basic_block_info;

  /* Number of basic blocks in this flow graph.  */
  int x_n_basic_blocks;

  /* Number of edges in this flow graph.  */
  int x_n_edges;

  /* The first free basic block number.  */
  int x_last_basic_block;

  /* Mapping of labels to their associated blocks.  At present
     only used for the gimple CFG.  */
  VEC(basic_block,gc) *x_label_to_block_map;

  enum profile_status_d x_profile_status;

  /* Whether the dominators and the postdominators are available.  */
  enum dom_state x_dom_computed[2];

  /* Number of basic blocks in the dominance tree.  */
  unsigned x_n_bbs_in_dom_tree[2];

  /* Maximal number of entities in the single jumptable.  Used to estimate
     final flowgraph size.  */
  int max_jumptable_ents;

  /* UIDs for LABEL_DECLs.  */
  int last_label_uid;
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
  (VEC_index (basic_block, basic_block_info_for_function(FN), (N)))
#define SET_BASIC_BLOCK_FOR_FUNCTION(FN,N,BB) \
  (VEC_replace (basic_block, basic_block_info_for_function(FN), (N), (BB)))

/* Defines for textual backward source compatibility.  */
#define ENTRY_BLOCK_PTR		(cfun->cfg->x_entry_block_ptr)
#define EXIT_BLOCK_PTR		(cfun->cfg->x_exit_block_ptr)
#define basic_block_info	(cfun->cfg->x_basic_block_info)
#define n_basic_blocks		(cfun->cfg->x_n_basic_blocks)
#define n_edges			(cfun->cfg->x_n_edges)
#define last_basic_block	(cfun->cfg->x_last_basic_block)
#define label_to_block_map	(cfun->cfg->x_label_to_block_map)
#define profile_status		(cfun->cfg->x_profile_status)

#define BASIC_BLOCK(N)		(VEC_index (basic_block, basic_block_info, (N)))
#define SET_BASIC_BLOCK(N,BB)	(VEC_replace (basic_block, basic_block_info, (N), (BB)))

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

extern bitmap_obstack reg_obstack;


/* Stuff for recording basic block info.  */

#define BB_HEAD(B)      (B)->il.rtl->head_
#define BB_END(B)       (B)->il.rtl->end_

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

extern void remove_fake_edges (void);
extern void remove_fake_exit_edges (void);
extern void add_noreturn_fake_exit_edges (void);
extern void connect_infinite_loops_to_exit (void);
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
extern int post_order_compute (int *, bool, bool);
extern int inverted_post_order_compute (int *);
extern int pre_and_rev_post_order_compute (int *, int *, bool);
extern int dfs_enumerate_from (basic_block, int,
			       bool (*)(const_basic_block, const void *),
			       basic_block *, int, const void *);
extern void compute_dominance_frontiers (bitmap *);
extern bitmap compute_idf (bitmap, bitmap *);
extern void dump_bb_info (basic_block, bool, bool, int, const char *, FILE *);
extern void dump_edge_info (FILE *, edge, int);
extern void brief_dump_cfg (FILE *);
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

#ifdef IFCVT_EXTRA_FIELDS
  IFCVT_EXTRA_FIELDS			/* Any machine dependent fields.  */
#endif

} ce_if_block_t;

/* This structure maintains an edge list vector.  */
struct edge_list
{
  int num_blocks;
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

/* Return expected execution frequency of the edge E.  */
#define EDGE_FREQUENCY(e)		(((e)->src->frequency \
					  * (e)->probability \
					  + REG_BR_PROB_BASE / 2) \
					 / REG_BR_PROB_BASE)

/* Return nonzero if edge is critical.  */
#define EDGE_CRITICAL_P(e)		(EDGE_COUNT ((e)->src->succs) >= 2 \
					 && EDGE_COUNT ((e)->dest->preds) >= 2)

#define EDGE_COUNT(ev)			VEC_length (edge, (ev))
#define EDGE_I(ev,i)			VEC_index  (edge, (ev), (i))
#define EDGE_PRED(bb,i)			VEC_index  (edge, (bb)->preds, (i))
#define EDGE_SUCC(bb,i)			VEC_index  (edge, (bb)->succs, (i))

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
  gcc_assert (single_succ_p (bb));
  return EDGE_SUCC (bb, 0);
}

/* Returns the single predecessor edge of basic block BB.  Aborts
   if BB does not have exactly one predecessor.  */

static inline edge
single_pred_edge (const_basic_block bb)
{
  gcc_assert (single_pred_p (bb));
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
  VEC(edge,gc) **container;
} edge_iterator;

static inline VEC(edge,gc) *
ei_container (edge_iterator i)
{
  gcc_assert (i.container);
  return *i.container;
}

#define ei_start(iter) ei_start_1 (&(iter))
#define ei_last(iter) ei_last_1 (&(iter))

/* Return an iterator pointing to the start of an edge vector.  */
static inline edge_iterator
ei_start_1 (VEC(edge,gc) **ev)
{
  edge_iterator i;

  i.index = 0;
  i.container = ev;

  return i;
}

/* Return an iterator pointing to the last element of an edge
   vector.  */
static inline edge_iterator
ei_last_1 (VEC(edge,gc) **ev)
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
  gcc_assert (i->index < EDGE_COUNT (ei_container (*i)));
  i->index++;
}

/* Move the iterator to the previous element.  */
static inline void
ei_prev (edge_iterator *i)
{
  gcc_assert (i->index > 0);
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

struct edge_list * create_edge_list (void);
void free_edge_list (struct edge_list *);
void print_edge_list (FILE *, struct edge_list *);
void verify_edge_list (FILE *, struct edge_list *);
int find_edge_index (struct edge_list *, basic_block, basic_block);
edge find_edge (basic_block, basic_block);

#define CLEANUP_EXPENSIVE	1	/* Do relatively expensive optimizations
					   except for edge forwarding */
#define CLEANUP_CROSSJUMP	2	/* Do crossjumping.  */
#define CLEANUP_POST_REGSTACK	4	/* We run after reg-stack and need
					   to care REG_DEAD notes.  */
#define CLEANUP_THREADING	8	/* Do jump threading.  */
#define CLEANUP_NO_INSN_DEL	16	/* Do not try to delete trivially dead
					   insns.  */
#define CLEANUP_CFGLAYOUT	32	/* Do cleanup in cfglayout mode.  */

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
extern bool maybe_hot_bb_p (const_basic_block);
extern bool maybe_hot_edge_p (edge);
extern bool probably_never_executed_bb_p (const_basic_block);
extern bool optimize_bb_for_size_p (const_basic_block);
extern bool optimize_bb_for_speed_p (const_basic_block);
extern bool optimize_edge_for_size_p (edge);
extern bool optimize_edge_for_speed_p (edge);
extern bool optimize_function_for_size_p (struct function *);
extern bool optimize_function_for_speed_p (struct function *);
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
extern void dump_regset (regset, FILE *);
extern void debug_regset (regset);
extern void expunge_block (basic_block);
extern void link_block (basic_block, basic_block);
extern void unlink_block (basic_block);
extern void compact_blocks (void);
extern basic_block alloc_block (void);
extern void alloc_aux_for_block (basic_block, int);
extern void alloc_aux_for_blocks (int);
extern void clear_aux_for_blocks (void);
extern void free_aux_for_blocks (void);
extern void alloc_aux_for_edge (edge, int);
extern void alloc_aux_for_edges (int);
extern void clear_aux_for_edges (void);
extern void free_aux_for_edges (void);

/* In cfganal.c  */
extern void find_unreachable_blocks (void);
extern bool forwarder_block_p (const_basic_block);
extern bool can_fallthru (basic_block, basic_block);
extern bool could_fall_through (basic_block, basic_block);
extern void flow_nodes_print (const char *, const_sbitmap, FILE *);
extern void flow_edge_list_print (const char *, const edge *, int, FILE *);

/* In cfgrtl.c  */
extern basic_block force_nonfallthru (edge);
extern basic_block force_nonfallthru_and_redirect (edge, basic_block, rtx);
extern rtx block_label (basic_block);
extern bool purge_all_dead_edges (void);
extern bool purge_dead_edges (basic_block);

/* In cfgbuild.c.  */
extern void find_many_sub_basic_blocks (sbitmap);
extern void rtl_make_eh_edge (sbitmap, basic_block, rtx);

/* In cfgcleanup.c.  */
extern bool cleanup_cfg (int);
extern int flow_find_cross_jump (basic_block, basic_block, rtx *, rtx *);
extern int flow_find_head_matching_sequence (basic_block, basic_block,
					     rtx *, rtx *, int);

extern bool delete_unreachable_blocks (void);

extern bool mark_dfs_back_edges (void);
extern void set_edge_can_fallthru_flag (void);
extern void update_br_prob_note (basic_block);
extern void fixup_abnormal_edges (void);
extern bool inside_basic_block_p (const_rtx);
extern bool control_flow_insn_p (const_rtx);
extern rtx get_last_bb_insn (basic_block);

/* In bb-reorder.c */
extern void reorder_basic_blocks (void);

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
extern VEC (basic_block, heap) *get_dominated_by (enum cdi_direction, basic_block);
extern VEC (basic_block, heap) *get_dominated_by_region (enum cdi_direction,
							 basic_block *,
							 unsigned);
extern VEC (basic_block, heap) *get_dominated_to_depth (enum cdi_direction,
							basic_block, int);
extern VEC (basic_block, heap) *get_all_dominated_blocks (enum cdi_direction,
							  basic_block);
extern void add_to_dominance_info (enum cdi_direction, basic_block);
extern void delete_from_dominance_info (enum cdi_direction, basic_block);
basic_block recompute_dominator (enum cdi_direction, basic_block);
extern void redirect_immediate_dominators (enum cdi_direction, basic_block,
					   basic_block);
extern void iterate_fix_dominators (enum cdi_direction,
				    VEC (basic_block, heap) *, bool);
extern void verify_dominators (enum cdi_direction);
extern basic_block first_dom_son (enum cdi_direction, basic_block);
extern basic_block next_dom_son (enum cdi_direction, basic_block);
unsigned bb_dom_dfs_in (enum cdi_direction, basic_block);
unsigned bb_dom_dfs_out (enum cdi_direction, basic_block);

extern edge try_redirect_by_replacing_jump (edge, basic_block, bool);
extern void break_superblocks (void);
extern void relink_block_chain (bool);
extern void check_bb_profile (basic_block, FILE *);
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


extern rtx insert_insn_end_bb_new (rtx, basic_block);

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
find_fallthru_edge (VEC(edge,gc) *edges)
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

#endif /* GCC_BASIC_BLOCK_H */
