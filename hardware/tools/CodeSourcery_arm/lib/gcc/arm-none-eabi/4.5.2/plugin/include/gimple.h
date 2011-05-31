/* Gimple IR definitions.

   Copyright 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
   Contributed by Aldy Hernandez <aldyh@redhat.com>

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

#ifndef GCC_GIMPLE_H
#define GCC_GIMPLE_H

#include "pointer-set.h"
#include "vec.h"
#include "ggc.h"
#include "tm.h"
#include "hard-reg-set.h"
#include "basic-block.h"
#include "tree-ssa-operands.h"

DEF_VEC_P(gimple);
DEF_VEC_ALLOC_P(gimple,heap);
DEF_VEC_ALLOC_P(gimple,gc);

typedef gimple *gimple_p;
DEF_VEC_P(gimple_p);
DEF_VEC_ALLOC_P(gimple_p,heap);

DEF_VEC_P(gimple_seq);
DEF_VEC_ALLOC_P(gimple_seq,gc);
DEF_VEC_ALLOC_P(gimple_seq,heap);

/* For each block, the PHI nodes that need to be rewritten are stored into
   these vectors.  */
typedef VEC(gimple, heap) *gimple_vec;
DEF_VEC_P (gimple_vec);
DEF_VEC_ALLOC_P (gimple_vec, heap);

enum gimple_code {
#define DEFGSCODE(SYM, STRING, STRUCT)	SYM,
#include "gimple.def"
#undef DEFGSCODE
    LAST_AND_UNUSED_GIMPLE_CODE
};

extern const char *const gimple_code_name[];
extern const unsigned char gimple_rhs_class_table[];

/* Error out if a gimple tuple is addressed incorrectly.  */
#if defined ENABLE_GIMPLE_CHECKING
extern void gimple_check_failed (const_gimple, const char *, int,          \
                                 const char *, enum gimple_code,           \
				 enum tree_code) ATTRIBUTE_NORETURN;

#define GIMPLE_CHECK(GS, CODE)						\
  do {									\
    const_gimple __gs = (GS);						\
    if (gimple_code (__gs) != (CODE))					\
      gimple_check_failed (__gs, __FILE__, __LINE__, __FUNCTION__,	\
	  		   (CODE), ERROR_MARK);				\
  } while (0)
#else  /* not ENABLE_GIMPLE_CHECKING  */
#define GIMPLE_CHECK(GS, CODE)			(void)0
#endif

/* Class of GIMPLE expressions suitable for the RHS of assignments.  See
   get_gimple_rhs_class.  */
enum gimple_rhs_class
{
  GIMPLE_INVALID_RHS,	/* The expression cannot be used on the RHS.  */
  GIMPLE_TERNARY_RHS,	/* The expression is a ternary operation.  */
  GIMPLE_BINARY_RHS,	/* The expression is a binary operation.  */
  GIMPLE_UNARY_RHS,	/* The expression is a unary operation.  */
  GIMPLE_SINGLE_RHS	/* The expression is a single object (an SSA
			   name, a _DECL, a _REF, etc.  */
};

/* Specific flags for individual GIMPLE statements.  These flags are
   always stored in gimple_statement_base.subcode and they may only be
   defined for statement codes that do not use sub-codes.

   Values for the masks can overlap as long as the overlapping values
   are never used in the same statement class.

   The maximum mask value that can be defined is 1 << 15 (i.e., each
   statement code can hold up to 16 bitflags).

   Keep this list sorted.  */
enum gf_mask {
    GF_ASM_INPUT		= 1 << 0,
    GF_ASM_VOLATILE		= 1 << 1,
    GF_CALL_CANNOT_INLINE	= 1 << 0,
    GF_CALL_FROM_THUNK		= 1 << 1,
    GF_CALL_RETURN_SLOT_OPT	= 1 << 2,
    GF_CALL_TAILCALL		= 1 << 3,
    GF_CALL_VA_ARG_PACK		= 1 << 4,
    GF_CALL_NOTHROW		= 1 << 5,
    GF_OMP_PARALLEL_COMBINED	= 1 << 0,

    /* True on an GIMPLE_OMP_RETURN statement if the return does not require
       a thread synchronization via some sort of barrier.  The exact barrier
       that would otherwise be emitted is dependent on the OMP statement with
       which this return is associated.  */
    GF_OMP_RETURN_NOWAIT	= 1 << 0,

    GF_OMP_SECTION_LAST		= 1 << 0,
    GF_PREDICT_TAKEN		= 1 << 15
};

/* Currently, there's only one type of gimple debug stmt.  Others are
   envisioned, for example, to enable the generation of is_stmt notes
   in line number information, to mark sequence points, etc.  This
   subcode is to be used to tell them apart.  */
enum gimple_debug_subcode {
  GIMPLE_DEBUG_BIND = 0
};

/* Masks for selecting a pass local flag (PLF) to work on.  These
   masks are used by gimple_set_plf and gimple_plf.  */
enum plf_mask {
    GF_PLF_1	= 1 << 0,
    GF_PLF_2	= 1 << 1
};

/* A node in a gimple_seq_d.  */
struct GTY((chain_next ("%h.next"), chain_prev ("%h.prev"))) gimple_seq_node_d {
  gimple stmt;
  struct gimple_seq_node_d *prev;
  struct gimple_seq_node_d *next;
};

/* A double-linked sequence of gimple statements.  */
struct GTY ((chain_next ("%h.next_free"))) gimple_seq_d {
  /* First and last statements in the sequence.  */
  gimple_seq_node first;
  gimple_seq_node last;

  /* Sequences are created/destroyed frequently.  To minimize
     allocation activity, deallocated sequences are kept in a pool of
     available sequences.  This is the pointer to the next free
     sequence in the pool.  */
  gimple_seq next_free;
};


/* Return the first node in GIMPLE sequence S.  */

static inline gimple_seq_node
gimple_seq_first (const_gimple_seq s)
{
  return s ? s->first : NULL;
}


/* Return the first statement in GIMPLE sequence S.  */

static inline gimple
gimple_seq_first_stmt (const_gimple_seq s)
{
  gimple_seq_node n = gimple_seq_first (s);
  return (n) ? n->stmt : NULL;
}


/* Return the last node in GIMPLE sequence S.  */

static inline gimple_seq_node
gimple_seq_last (const_gimple_seq s)
{
  return s ? s->last : NULL;
}


/* Return the last statement in GIMPLE sequence S.  */

static inline gimple
gimple_seq_last_stmt (const_gimple_seq s)
{
  gimple_seq_node n = gimple_seq_last (s);
  return (n) ? n->stmt : NULL;
}


/* Set the last node in GIMPLE sequence S to LAST.  */

static inline void
gimple_seq_set_last (gimple_seq s, gimple_seq_node last)
{
  s->last = last;
}


/* Set the first node in GIMPLE sequence S to FIRST.  */

static inline void
gimple_seq_set_first (gimple_seq s, gimple_seq_node first)
{
  s->first = first;
}


/* Return true if GIMPLE sequence S is empty.  */

static inline bool
gimple_seq_empty_p (const_gimple_seq s)
{
  return s == NULL || s->first == NULL;
}


void gimple_seq_add_stmt (gimple_seq *, gimple);

/* Link gimple statement GS to the end of the sequence *SEQ_P.  If
   *SEQ_P is NULL, a new sequence is allocated.  This function is
   similar to gimple_seq_add_stmt, but does not scan the operands.
   During gimplification, we need to manipulate statement sequences
   before the def/use vectors have been constructed.  */
void gimplify_seq_add_stmt (gimple_seq *, gimple);

/* Allocate a new sequence and initialize its first element with STMT.  */

static inline gimple_seq
gimple_seq_alloc_with_stmt (gimple stmt)
{
  gimple_seq seq = NULL;
  gimple_seq_add_stmt (&seq, stmt);
  return seq;
}


/* Returns the sequence of statements in BB.  */

static inline gimple_seq
bb_seq (const_basic_block bb)
{
  return (!(bb->flags & BB_RTL) && bb->il.gimple) ? bb->il.gimple->seq : NULL;
}


/* Sets the sequence of statements in BB to SEQ.  */

static inline void
set_bb_seq (basic_block bb, gimple_seq seq)
{
  gcc_assert (!(bb->flags & BB_RTL));
  bb->il.gimple->seq = seq;
}

/* Iterator object for GIMPLE statement sequences.  */

typedef struct
{
  /* Sequence node holding the current statement.  */
  gimple_seq_node ptr;

  /* Sequence and basic block holding the statement.  These fields
     are necessary to handle edge cases such as when statement is
     added to an empty basic block or when the last statement of a
     block/sequence is removed.  */
  gimple_seq seq;
  basic_block bb;
} gimple_stmt_iterator;


/* Data structure definitions for GIMPLE tuples.  NOTE: word markers
   are for 64 bit hosts.  */

struct GTY(()) gimple_statement_base {
  /* [ WORD 1 ]
     Main identifying code for a tuple.  */
  ENUM_BITFIELD(gimple_code) code : 8;

  /* Nonzero if a warning should not be emitted on this tuple.  */
  unsigned int no_warning	: 1;

  /* Nonzero if this tuple has been visited.  Passes are responsible
     for clearing this bit before using it.  */
  unsigned int visited		: 1;

  /* Nonzero if this tuple represents a non-temporal move.  */
  unsigned int nontemporal_move	: 1;

  /* Pass local flags.  These flags are free for any pass to use as
     they see fit.  Passes should not assume that these flags contain
     any useful value when the pass starts.  Any initial state that
     the pass requires should be set on entry to the pass.  See
     gimple_set_plf and gimple_plf for usage.  */
  unsigned int plf		: 2;

  /* Nonzero if this statement has been modified and needs to have its
     operands rescanned.  */
  unsigned modified 		: 1;

  /* Nonzero if this statement contains volatile operands.  */
  unsigned has_volatile_ops 	: 1;

  /* Padding to get subcode to 16 bit alignment.  */
  unsigned pad			: 1;

  /* The SUBCODE field can be used for tuple-specific flags for tuples
     that do not require subcodes.  Note that SUBCODE should be at
     least as wide as tree codes, as several tuples store tree codes
     in there.  */
  unsigned int subcode		: 16;

  /* UID of this statement.  This is used by passes that want to
     assign IDs to statements.  It must be assigned and used by each
     pass.  By default it should be assumed to contain garbage.  */
  unsigned uid;

  /* [ WORD 2 ]
     Locus information for debug info.  */
  location_t location;

  /* Number of operands in this tuple.  */
  unsigned num_ops;

  /* [ WORD 3 ]
     Basic block holding this statement.  */
  struct basic_block_def *bb;

  /* [ WORD 4 ]
     Lexical block holding this statement.  */
  tree block;
};


/* Base structure for tuples with operands.  */

struct GTY(()) gimple_statement_with_ops_base
{
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5-6 ]
     SSA operand vectors.  NOTE: It should be possible to
     amalgamate these vectors with the operand vector OP.  However,
     the SSA operand vectors are organized differently and contain
     more information (like immediate use chaining).  */
  struct def_optype_d GTY((skip (""))) *def_ops;
  struct use_optype_d GTY((skip (""))) *use_ops;
};


/* Statements that take register operands.  */

struct GTY(()) gimple_statement_with_ops
{
  /* [ WORD 1-6 ]  */
  struct gimple_statement_with_ops_base opbase;

  /* [ WORD 7 ]
     Operand vector.  NOTE!  This must always be the last field
     of this structure.  In particular, this means that this
     structure cannot be embedded inside another one.  */
  tree GTY((length ("%h.opbase.gsbase.num_ops"))) op[1];
};


/* Base for statements that take both memory and register operands.  */

struct GTY(()) gimple_statement_with_memory_ops_base
{
  /* [ WORD 1-6 ]  */
  struct gimple_statement_with_ops_base opbase;

  /* [ WORD 7-8 ]
     Virtual operands for this statement.  The GC will pick them
     up via the ssa_names array.  */
  tree GTY((skip (""))) vdef;
  tree GTY((skip (""))) vuse;
};


/* Statements that take both memory and register operands.  */

struct GTY(()) gimple_statement_with_memory_ops
{
  /* [ WORD 1-8 ]  */
  struct gimple_statement_with_memory_ops_base membase;

  /* [ WORD 9 ]
     Operand vector.  NOTE!  This must always be the last field
     of this structure.  In particular, this means that this
     structure cannot be embedded inside another one.  */
  tree GTY((length ("%h.membase.opbase.gsbase.num_ops"))) op[1];
};


/* OpenMP statements (#pragma omp).  */

struct GTY(()) gimple_statement_omp {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ]  */
  gimple_seq body;
};


/* GIMPLE_BIND */

struct GTY(()) gimple_statement_bind {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ]
     Variables declared in this scope.  */
  tree vars;

  /* [ WORD 6 ]
     This is different than the BLOCK field in gimple_statement_base,
     which is analogous to TREE_BLOCK (i.e., the lexical block holding
     this statement).  This field is the equivalent of BIND_EXPR_BLOCK
     in tree land (i.e., the lexical scope defined by this bind).  See
     gimple-low.c.  */
  tree block;

  /* [ WORD 7 ]  */
  gimple_seq body;
};


/* GIMPLE_CATCH */

struct GTY(()) gimple_statement_catch {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ]  */
  tree types;

  /* [ WORD 6 ]  */
  gimple_seq handler;
};


/* GIMPLE_EH_FILTER */

struct GTY(()) gimple_statement_eh_filter {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ]
     Filter types.  */
  tree types;

  /* [ WORD 6 ]
     Failure actions.  */
  gimple_seq failure;
};


/* GIMPLE_EH_MUST_NOT_THROW */

struct GTY(()) gimple_statement_eh_mnt {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ] Abort function decl.  */
  tree fndecl;
};

/* GIMPLE_PHI */

struct GTY(()) gimple_statement_phi {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ]  */
  unsigned capacity;
  unsigned nargs;

  /* [ WORD 6 ]  */
  tree result;

  /* [ WORD 7 ]  */
  struct phi_arg_d GTY ((length ("%h.nargs"))) args[1];
};


/* GIMPLE_RESX, GIMPLE_EH_DISPATCH */

struct GTY(()) gimple_statement_eh_ctrl
{
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ]
     Exception region number.  */
  int region;
};


/* GIMPLE_TRY */

struct GTY(()) gimple_statement_try {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ]
     Expression to evaluate.  */
  gimple_seq eval;

  /* [ WORD 6 ]
     Cleanup expression.  */
  gimple_seq cleanup;
};

/* Kind of GIMPLE_TRY statements.  */
enum gimple_try_flags
{
  /* A try/catch.  */
  GIMPLE_TRY_CATCH = 1 << 0,

  /* A try/finally.  */
  GIMPLE_TRY_FINALLY = 1 << 1,
  GIMPLE_TRY_KIND = GIMPLE_TRY_CATCH | GIMPLE_TRY_FINALLY,

  /* Analogous to TRY_CATCH_IS_CLEANUP.  */
  GIMPLE_TRY_CATCH_IS_CLEANUP = 1 << 2
};

/* GIMPLE_WITH_CLEANUP_EXPR */

struct GTY(()) gimple_statement_wce {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* Subcode: CLEANUP_EH_ONLY.  True if the cleanup should only be
	      executed if an exception is thrown, not on normal exit of its
	      scope.  This flag is analogous to the CLEANUP_EH_ONLY flag
	      in TARGET_EXPRs.  */

  /* [ WORD 5 ]
     Cleanup expression.  */
  gimple_seq cleanup;
};


/* GIMPLE_ASM  */

struct GTY(()) gimple_statement_asm
{
  /* [ WORD 1-8 ]  */
  struct gimple_statement_with_memory_ops_base membase;

  /* [ WORD 9 ]
     __asm__ statement.  */
  const char *string;

  /* [ WORD 10 ]
       Number of inputs, outputs, clobbers, labels.  */
  unsigned char ni;
  unsigned char no;
  unsigned char nc;
  unsigned char nl;

  /* [ WORD 11 ]
     Operand vector.  NOTE!  This must always be the last field
     of this structure.  In particular, this means that this
     structure cannot be embedded inside another one.  */
  tree GTY((length ("%h.membase.opbase.gsbase.num_ops"))) op[1];
};

/* GIMPLE_OMP_CRITICAL */

struct GTY(()) gimple_statement_omp_critical {
  /* [ WORD 1-5 ]  */
  struct gimple_statement_omp omp;

  /* [ WORD 6 ]
     Critical section name.  */
  tree name;
};


struct GTY(()) gimple_omp_for_iter {
  /* Condition code.  */
  enum tree_code cond;

  /* Index variable.  */
  tree index;

  /* Initial value.  */
  tree initial;

  /* Final value.  */
  tree final;

  /* Increment.  */
  tree incr;
};

/* GIMPLE_OMP_FOR */

struct GTY(()) gimple_statement_omp_for {
  /* [ WORD 1-5 ]  */
  struct gimple_statement_omp omp;

  /* [ WORD 6 ]  */
  tree clauses;

  /* [ WORD 7 ]
     Number of elements in iter array.  */
  size_t collapse;

  /* [ WORD 8 ]  */
  struct gimple_omp_for_iter * GTY((length ("%h.collapse"))) iter;

  /* [ WORD 9 ]
     Pre-body evaluated before the loop body begins.  */
  gimple_seq pre_body;
};


/* GIMPLE_OMP_PARALLEL */

struct GTY(()) gimple_statement_omp_parallel {
  /* [ WORD 1-5 ]  */
  struct gimple_statement_omp omp;

  /* [ WORD 6 ]
     Clauses.  */
  tree clauses;

  /* [ WORD 7 ]
     Child function holding the body of the parallel region.  */
  tree child_fn;

  /* [ WORD 8 ]
     Shared data argument.  */
  tree data_arg;
};


/* GIMPLE_OMP_TASK */

struct GTY(()) gimple_statement_omp_task {
  /* [ WORD 1-8 ]  */
  struct gimple_statement_omp_parallel par;

  /* [ WORD 9 ]
     Child function holding firstprivate initialization if needed.  */
  tree copy_fn;

  /* [ WORD 10-11 ]
     Size and alignment in bytes of the argument data block.  */
  tree arg_size;
  tree arg_align;
};


/* GIMPLE_OMP_SECTION */
/* Uses struct gimple_statement_omp.  */


/* GIMPLE_OMP_SECTIONS */

struct GTY(()) gimple_statement_omp_sections {
  /* [ WORD 1-5 ]  */
  struct gimple_statement_omp omp;

  /* [ WORD 6 ]  */
  tree clauses;

  /* [ WORD 7 ]
     The control variable used for deciding which of the sections to
     execute.  */
  tree control;
};

/* GIMPLE_OMP_CONTINUE.

   Note: This does not inherit from gimple_statement_omp, because we
         do not need the body field.  */

struct GTY(()) gimple_statement_omp_continue {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ]  */
  tree control_def;

  /* [ WORD 6 ]  */
  tree control_use;
};

/* GIMPLE_OMP_SINGLE */

struct GTY(()) gimple_statement_omp_single {
  /* [ WORD 1-5 ]  */
  struct gimple_statement_omp omp;

  /* [ WORD 6 ]  */
  tree clauses;
};


/* GIMPLE_OMP_ATOMIC_LOAD.
   Note: This is based on gimple_statement_base, not g_s_omp, because g_s_omp
   contains a sequence, which we don't need here.  */

struct GTY(()) gimple_statement_omp_atomic_load {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5-6 ]  */
  tree rhs, lhs;
};

/* GIMPLE_OMP_ATOMIC_STORE.
   See note on GIMPLE_OMP_ATOMIC_LOAD.  */

struct GTY(()) gimple_statement_omp_atomic_store {
  /* [ WORD 1-4 ]  */
  struct gimple_statement_base gsbase;

  /* [ WORD 5 ]  */
  tree val;
};

#define DEFGSSTRUCT(SYM, STRUCT, HAS_TREE_OP)	SYM,
enum gimple_statement_structure_enum {
#include "gsstruct.def"
    LAST_GSS_ENUM
};
#undef DEFGSSTRUCT


/* Define the overall contents of a gimple tuple.  It may be any of the
   structures declared above for various types of tuples.  */

union GTY ((desc ("gimple_statement_structure (&%h)"))) gimple_statement_d {
  struct gimple_statement_base GTY ((tag ("GSS_BASE"))) gsbase;
  struct gimple_statement_with_ops GTY ((tag ("GSS_WITH_OPS"))) gsops;
  struct gimple_statement_with_memory_ops_base GTY ((tag ("GSS_WITH_MEM_OPS_BASE"))) gsmembase;
  struct gimple_statement_with_memory_ops GTY ((tag ("GSS_WITH_MEM_OPS"))) gsmem;
  struct gimple_statement_omp GTY ((tag ("GSS_OMP"))) omp;
  struct gimple_statement_bind GTY ((tag ("GSS_BIND"))) gimple_bind;
  struct gimple_statement_catch GTY ((tag ("GSS_CATCH"))) gimple_catch;
  struct gimple_statement_eh_filter GTY ((tag ("GSS_EH_FILTER"))) gimple_eh_filter;
  struct gimple_statement_eh_mnt GTY ((tag ("GSS_EH_MNT"))) gimple_eh_mnt;
  struct gimple_statement_phi GTY ((tag ("GSS_PHI"))) gimple_phi;
  struct gimple_statement_eh_ctrl GTY ((tag ("GSS_EH_CTRL"))) gimple_eh_ctrl;
  struct gimple_statement_try GTY ((tag ("GSS_TRY"))) gimple_try;
  struct gimple_statement_wce GTY ((tag ("GSS_WCE"))) gimple_wce;
  struct gimple_statement_asm GTY ((tag ("GSS_ASM"))) gimple_asm;
  struct gimple_statement_omp_critical GTY ((tag ("GSS_OMP_CRITICAL"))) gimple_omp_critical;
  struct gimple_statement_omp_for GTY ((tag ("GSS_OMP_FOR"))) gimple_omp_for;
  struct gimple_statement_omp_parallel GTY ((tag ("GSS_OMP_PARALLEL"))) gimple_omp_parallel;
  struct gimple_statement_omp_task GTY ((tag ("GSS_OMP_TASK"))) gimple_omp_task;
  struct gimple_statement_omp_sections GTY ((tag ("GSS_OMP_SECTIONS"))) gimple_omp_sections;
  struct gimple_statement_omp_single GTY ((tag ("GSS_OMP_SINGLE"))) gimple_omp_single;
  struct gimple_statement_omp_continue GTY ((tag ("GSS_OMP_CONTINUE"))) gimple_omp_continue;
  struct gimple_statement_omp_atomic_load GTY ((tag ("GSS_OMP_ATOMIC_LOAD"))) gimple_omp_atomic_load;
  struct gimple_statement_omp_atomic_store GTY ((tag ("GSS_OMP_ATOMIC_STORE"))) gimple_omp_atomic_store;
};

/* In gimple.c.  */

/* Offset in bytes to the location of the operand vector.
   Zero if there is no operand vector for this tuple structure.  */
extern size_t const gimple_ops_offset_[];

/* Map GIMPLE codes to GSS codes.  */
extern enum gimple_statement_structure_enum const gss_for_code_[];

/* This variable holds the currently expanded gimple statement for purposes
   of comminucating the profile info to the builtin expanders.  */
extern gimple currently_expanding_gimple_stmt;

gimple gimple_build_return (tree);

gimple gimple_build_assign_stat (tree, tree MEM_STAT_DECL);
#define gimple_build_assign(l,r) gimple_build_assign_stat (l, r MEM_STAT_INFO)

void extract_ops_from_tree_1 (tree, enum tree_code *, tree *, tree *, tree *);

gimple gimple_build_assign_with_ops_stat (enum tree_code, tree, tree,
					  tree, tree MEM_STAT_DECL);
#define gimple_build_assign_with_ops(c,o1,o2,o3)			\
  gimple_build_assign_with_ops_stat (c, o1, o2, o3, NULL_TREE MEM_STAT_INFO)
#define gimple_build_assign_with_ops3(c,o1,o2,o3,o4)			\
  gimple_build_assign_with_ops_stat (c, o1, o2, o3, o4 MEM_STAT_INFO)

gimple gimple_build_debug_bind_stat (tree, tree, gimple MEM_STAT_DECL);
#define gimple_build_debug_bind(var,val,stmt)			\
  gimple_build_debug_bind_stat ((var), (val), (stmt) MEM_STAT_INFO)

gimple gimple_build_call_vec (tree, VEC(tree, heap) *);
gimple gimple_build_call (tree, unsigned, ...);
gimple gimple_build_call_from_tree (tree);
gimple gimplify_assign (tree, tree, gimple_seq *);
gimple gimple_build_cond (enum tree_code, tree, tree, tree, tree);
gimple gimple_build_label (tree label);
gimple gimple_build_goto (tree dest);
gimple gimple_build_nop (void);
gimple gimple_build_bind (tree, gimple_seq, tree);
gimple gimple_build_asm_vec (const char *, VEC(tree,gc) *, VEC(tree,gc) *,
                             VEC(tree,gc) *, VEC(tree,gc) *);
gimple gimple_build_catch (tree, gimple_seq);
gimple gimple_build_eh_filter (tree, gimple_seq);
gimple gimple_build_eh_must_not_throw (tree);
gimple gimple_build_try (gimple_seq, gimple_seq, enum gimple_try_flags);
gimple gimple_build_wce (gimple_seq);
gimple gimple_build_resx (int);
gimple gimple_build_eh_dispatch (int);
gimple gimple_build_switch_nlabels (unsigned, tree, tree);
gimple gimple_build_switch (unsigned, tree, tree, ...);
gimple gimple_build_switch_vec (tree, tree, VEC(tree,heap) *);
gimple gimple_build_omp_parallel (gimple_seq, tree, tree, tree);
gimple gimple_build_omp_task (gimple_seq, tree, tree, tree, tree, tree, tree);
gimple gimple_build_omp_for (gimple_seq, tree, size_t, gimple_seq);
gimple gimple_build_omp_critical (gimple_seq, tree);
gimple gimple_build_omp_section (gimple_seq);
gimple gimple_build_omp_continue (tree, tree);
gimple gimple_build_omp_master (gimple_seq);
gimple gimple_build_omp_return (bool);
gimple gimple_build_omp_ordered (gimple_seq);
gimple gimple_build_omp_sections (gimple_seq, tree);
gimple gimple_build_omp_sections_switch (void);
gimple gimple_build_omp_single (gimple_seq, tree);
gimple gimple_build_cdt (tree, tree);
gimple gimple_build_omp_atomic_load (tree, tree);
gimple gimple_build_omp_atomic_store (tree);
gimple gimple_build_predict (enum br_predictor, enum prediction);
enum gimple_statement_structure_enum gss_for_assign (enum tree_code);
void sort_case_labels (VEC(tree,heap) *);
void gimple_set_body (tree, gimple_seq);
gimple_seq gimple_body (tree);
bool gimple_has_body_p (tree);
gimple_seq gimple_seq_alloc (void);
void gimple_seq_free (gimple_seq);
void gimple_seq_add_seq (gimple_seq *, gimple_seq);
gimple_seq gimple_seq_copy (gimple_seq);
int gimple_call_flags (const_gimple);
bool gimple_assign_copy_p (gimple);
bool gimple_assign_ssa_name_copy_p (gimple);
bool gimple_assign_single_p (gimple);
bool gimple_assign_unary_nop_p (gimple);
void gimple_set_bb (gimple, struct basic_block_def *);
void gimple_assign_set_rhs_from_tree (gimple_stmt_iterator *, tree);
void gimple_assign_set_rhs_with_ops_1 (gimple_stmt_iterator *, enum tree_code,
				       tree, tree, tree);
tree gimple_get_lhs (const_gimple);
void gimple_set_lhs (gimple, tree);
void gimple_replace_lhs (gimple, tree);
gimple gimple_copy (gimple);
bool is_gimple_operand (const_tree);
void gimple_set_modified (gimple, bool);
void gimple_cond_get_ops_from_tree (tree, enum tree_code *, tree *, tree *);
gimple gimple_build_cond_from_tree (tree, tree, tree);
void gimple_cond_set_condition_from_tree (gimple, tree);
bool gimple_has_side_effects (const_gimple);
bool gimple_rhs_has_side_effects (const_gimple);
bool gimple_could_trap_p (gimple);
bool gimple_assign_rhs_could_trap_p (gimple);
void gimple_regimplify_operands (gimple, gimple_stmt_iterator *);
bool empty_body_p (gimple_seq);
unsigned get_gimple_rhs_num_ops (enum tree_code);
#define gimple_alloc(c, n) gimple_alloc_stat (c, n MEM_STAT_INFO)
gimple gimple_alloc_stat (enum gimple_code, unsigned MEM_STAT_DECL);
const char *gimple_decl_printable_name (tree, int);
tree gimple_fold_obj_type_ref (tree, tree);

/* Returns true iff T is a valid GIMPLE statement.  */
extern bool is_gimple_stmt (tree);

/* Returns true iff TYPE is a valid type for a scalar register variable.  */
extern bool is_gimple_reg_type (tree);
/* Returns true iff T is a scalar register variable.  */
extern bool is_gimple_reg (tree);
/* Returns true iff T is any sort of variable.  */
extern bool is_gimple_variable (tree);
/* Returns true iff T is any sort of symbol.  */
extern bool is_gimple_id (tree);
/* Returns true iff T is a variable or an INDIRECT_REF (of a variable).  */
extern bool is_gimple_min_lval (tree);
/* Returns true iff T is something whose address can be taken.  */
extern bool is_gimple_addressable (tree);
/* Returns true iff T is any valid GIMPLE lvalue.  */
extern bool is_gimple_lvalue (tree);

/* Returns true iff T is a GIMPLE address.  */
bool is_gimple_address (const_tree);
/* Returns true iff T is a GIMPLE invariant address.  */
bool is_gimple_invariant_address (const_tree);
/* Returns true iff T is a GIMPLE invariant address at interprocedural
   level.  */
bool is_gimple_ip_invariant_address (const_tree);
/* Returns true iff T is a valid GIMPLE constant.  */
bool is_gimple_constant (const_tree);
/* Returns true iff T is a GIMPLE restricted function invariant.  */
extern bool is_gimple_min_invariant (const_tree);
/* Returns true iff T is a GIMPLE restricted interprecodural invariant.  */
extern bool is_gimple_ip_invariant (const_tree);
/* Returns true iff T is a GIMPLE rvalue.  */
extern bool is_gimple_val (tree);
/* Returns true iff T is a GIMPLE asm statement input.  */
extern bool is_gimple_asm_val (tree);
/* Returns true iff T is a valid rhs for a MODIFY_EXPR where the LHS is a
   GIMPLE temporary, a renamed user variable, or something else,
   respectively.  */
extern bool is_gimple_reg_rhs (tree);
extern bool is_gimple_mem_rhs (tree);

/* Returns true iff T is a valid if-statement condition.  */
extern bool is_gimple_condexpr (tree);

/* Returns true iff T is a type conversion.  */
extern bool is_gimple_cast (tree);
/* Returns true iff T is a variable that does not need to live in memory.  */
extern bool is_gimple_non_addressable (tree t);

/* Returns true iff T is a valid call address expression.  */
extern bool is_gimple_call_addr (tree);
/* If T makes a function call, returns the CALL_EXPR operand.  */
extern tree get_call_expr_in (tree t);

extern void recalculate_side_effects (tree);
extern bool compare_field_offset (tree, tree);
extern tree gimple_register_type (tree);
extern void print_gimple_types_stats (void);
extern void free_gimple_type_tables (void);
extern tree gimple_unsigned_type (tree);
extern tree gimple_signed_type (tree);
extern alias_set_type gimple_get_alias_set (tree);
extern void count_uses_and_derefs (tree, gimple, unsigned *, unsigned *,
				   unsigned *);
extern bool walk_stmt_load_store_addr_ops (gimple, void *,
					   bool (*)(gimple, tree, void *),
					   bool (*)(gimple, tree, void *),
					   bool (*)(gimple, tree, void *));
extern bool walk_stmt_load_store_ops (gimple, void *,
				      bool (*)(gimple, tree, void *),
				      bool (*)(gimple, tree, void *));
extern bool gimple_ior_addresses_taken (bitmap, gimple);

/* In gimplify.c  */
extern tree create_tmp_var_raw (tree, const char *);
extern tree create_tmp_var_name (const char *);
extern tree create_tmp_var (tree, const char *);
extern tree get_initialized_tmp_var (tree, gimple_seq *, gimple_seq *);
extern tree get_formal_tmp_var (tree, gimple_seq *);
extern void declare_vars (tree, gimple, bool);
extern void annotate_all_with_location (gimple_seq, location_t);

/* Validation of GIMPLE expressions.  Note that these predicates only check
   the basic form of the expression, they don't recurse to make sure that
   underlying nodes are also of the right form.  */
typedef bool (*gimple_predicate)(tree);


/* FIXME we should deduce this from the predicate.  */
enum fallback {
  fb_none = 0,		/* Do not generate a temporary.  */

  fb_rvalue = 1,	/* Generate an rvalue to hold the result of a
			   gimplified expression.  */

  fb_lvalue = 2,	/* Generate an lvalue to hold the result of a
			   gimplified expression.  */

  fb_mayfail = 4,	/* Gimplification may fail.  Error issued
			   afterwards.  */
  fb_either= fb_rvalue | fb_lvalue
};

typedef int fallback_t;

enum gimplify_status {
  GS_ERROR	= -2,	/* Something Bad Seen.  */
  GS_UNHANDLED	= -1,	/* A langhook result for "I dunno".  */
  GS_OK		= 0,	/* We did something, maybe more to do.  */
  GS_ALL_DONE	= 1	/* The expression is fully gimplified.  */
};

struct gimplify_ctx
{
  struct gimplify_ctx *prev_context;

  VEC(gimple,heap) *bind_expr_stack;
  tree temps;
  gimple_seq conditional_cleanups;
  tree exit_label;
  tree return_temp;

  VEC(tree,heap) *case_labels;
  /* The formal temporary table.  Should this be persistent?  */
  htab_t temp_htab;

  int conditions;
  bool save_stack;
  bool into_ssa;
  bool allow_rhs_cond_expr;
};

extern enum gimplify_status gimplify_expr (tree *, gimple_seq *, gimple_seq *,
					   bool (*) (tree), fallback_t);
extern void gimplify_type_sizes (tree, gimple_seq *);
extern void gimplify_one_sizepos (tree *, gimple_seq *);
extern bool gimplify_stmt (tree *, gimple_seq *);
extern gimple gimplify_body (tree *, tree, bool);
extern void push_gimplify_context (struct gimplify_ctx *);
extern void pop_gimplify_context (gimple);
extern void gimplify_and_add (tree, gimple_seq *);

/* Miscellaneous helpers.  */
extern void gimple_add_tmp_var (tree);
extern gimple gimple_current_bind_expr (void);
extern VEC(gimple, heap) *gimple_bind_expr_stack (void);
extern tree voidify_wrapper_expr (tree, tree);
extern tree build_and_jump (tree *);
extern tree alloc_stmt_list (void);
extern void free_stmt_list (tree);
extern tree force_labels_r (tree *, int *, void *);
extern enum gimplify_status gimplify_va_arg_expr (tree *, gimple_seq *,
						  gimple_seq *);
struct gimplify_omp_ctx;
extern void omp_firstprivatize_variable (struct gimplify_omp_ctx *, tree);
extern tree gimple_boolify (tree);
extern gimple_predicate rhs_predicate_for (tree);
extern tree canonicalize_cond_expr_cond (tree);

/* In omp-low.c.  */
extern tree omp_reduction_init (tree, tree);

/* In tree-nested.c.  */
extern void lower_nested_functions (tree);
extern void insert_field_into_struct (tree, tree);

/* In gimplify.c.  */
extern void gimplify_function_tree (tree);

/* In cfgexpand.c.  */
extern tree gimple_assign_rhs_to_tree (gimple);

/* In builtins.c  */
extern bool validate_gimple_arglist (const_gimple, ...);

/* In tree-ssa.c  */
extern bool tree_ssa_useless_type_conversion (tree);
extern tree tree_ssa_strip_useless_type_conversions (tree);
extern bool useless_type_conversion_p (tree, tree);
extern bool types_compatible_p (tree, tree);

/* Return the code for GIMPLE statement G.  */

static inline enum gimple_code
gimple_code (const_gimple g)
{
  return g->gsbase.code;
}


/* Return the GSS code used by a GIMPLE code.  */

static inline enum gimple_statement_structure_enum
gss_for_code (enum gimple_code code)
{
#ifdef ENABLE_CHECKING
  gcc_assert ((unsigned int)code < LAST_AND_UNUSED_GIMPLE_CODE);
#endif
  return gss_for_code_[code];
}


/* Return which GSS code is used by GS.  */

static inline enum gimple_statement_structure_enum
gimple_statement_structure (gimple gs)
{
  return gss_for_code (gimple_code (gs));
}


/* Return true if statement G has sub-statements.  This is only true for
   High GIMPLE statements.  */

static inline bool
gimple_has_substatements (gimple g)
{
  switch (gimple_code (g))
    {
    case GIMPLE_BIND:
    case GIMPLE_CATCH:
    case GIMPLE_EH_FILTER:
    case GIMPLE_TRY:
    case GIMPLE_OMP_FOR:
    case GIMPLE_OMP_MASTER:
    case GIMPLE_OMP_ORDERED:
    case GIMPLE_OMP_SECTION:
    case GIMPLE_OMP_PARALLEL:
    case GIMPLE_OMP_TASK:
    case GIMPLE_OMP_SECTIONS:
    case GIMPLE_OMP_SINGLE:
    case GIMPLE_OMP_CRITICAL:
    case GIMPLE_WITH_CLEANUP_EXPR:
      return true;

    default:
      return false;
    }
}


/* Return the basic block holding statement G.  */

static inline struct basic_block_def *
gimple_bb (const_gimple g)
{
  return g->gsbase.bb;
}


/* Return the lexical scope block holding statement G.  */

static inline tree
gimple_block (const_gimple g)
{
  return g->gsbase.block;
}


/* Set BLOCK to be the lexical scope block holding statement G.  */

static inline void
gimple_set_block (gimple g, tree block)
{
  g->gsbase.block = block;
}


/* Return location information for statement G.  */

static inline location_t
gimple_location (const_gimple g)
{
  return g->gsbase.location;
}

/* Return pointer to location information for statement G.  */

static inline const location_t *
gimple_location_ptr (const_gimple g)
{
  return &g->gsbase.location;
}


/* Set location information for statement G.  */

static inline void
gimple_set_location (gimple g, location_t location)
{
  g->gsbase.location = location;
}


/* Return true if G contains location information.  */

static inline bool
gimple_has_location (const_gimple g)
{
  return gimple_location (g) != UNKNOWN_LOCATION;
}


/* Return the file name of the location of STMT.  */

static inline const char *
gimple_filename (const_gimple stmt)
{
  return LOCATION_FILE (gimple_location (stmt));
}


/* Return the line number of the location of STMT.  */

static inline int
gimple_lineno (const_gimple stmt)
{
  return LOCATION_LINE (gimple_location (stmt));
}


/* Determine whether SEQ is a singleton. */

static inline bool
gimple_seq_singleton_p (gimple_seq seq)
{
  return ((gimple_seq_first (seq) != NULL)
	  && (gimple_seq_first (seq) == gimple_seq_last (seq)));
}

/* Return true if no warnings should be emitted for statement STMT.  */

static inline bool
gimple_no_warning_p (const_gimple stmt)
{
  return stmt->gsbase.no_warning;
}

/* Set the no_warning flag of STMT to NO_WARNING.  */

static inline void
gimple_set_no_warning (gimple stmt, bool no_warning)
{
  stmt->gsbase.no_warning = (unsigned) no_warning;
}

/* Set the visited status on statement STMT to VISITED_P.  */

static inline void
gimple_set_visited (gimple stmt, bool visited_p)
{
  stmt->gsbase.visited = (unsigned) visited_p;
}


/* Return the visited status for statement STMT.  */

static inline bool
gimple_visited_p (gimple stmt)
{
  return stmt->gsbase.visited;
}


/* Set pass local flag PLF on statement STMT to VAL_P.  */

static inline void
gimple_set_plf (gimple stmt, enum plf_mask plf, bool val_p)
{
  if (val_p)
    stmt->gsbase.plf |= (unsigned int) plf;
  else
    stmt->gsbase.plf &= ~((unsigned int) plf);
}


/* Return the value of pass local flag PLF on statement STMT.  */

static inline unsigned int
gimple_plf (gimple stmt, enum plf_mask plf)
{
  return stmt->gsbase.plf & ((unsigned int) plf);
}


/* Set the UID of statement.  */

static inline void
gimple_set_uid (gimple g, unsigned uid)
{
  g->gsbase.uid = uid;
}


/* Return the UID of statement.  */

static inline unsigned
gimple_uid (const_gimple g)
{
  return g->gsbase.uid;
}


/* Return true if GIMPLE statement G has register or memory operands.  */

static inline bool
gimple_has_ops (const_gimple g)
{
  return gimple_code (g) >= GIMPLE_COND && gimple_code (g) <= GIMPLE_RETURN;
}


/* Return true if GIMPLE statement G has memory operands.  */

static inline bool
gimple_has_mem_ops (const_gimple g)
{
  return gimple_code (g) >= GIMPLE_ASSIGN && gimple_code (g) <= GIMPLE_RETURN;
}


/* Return the set of DEF operands for statement G.  */

static inline struct def_optype_d *
gimple_def_ops (const_gimple g)
{
  if (!gimple_has_ops (g))
    return NULL;
  return g->gsops.opbase.def_ops;
}


/* Set DEF to be the set of DEF operands for statement G.  */

static inline void
gimple_set_def_ops (gimple g, struct def_optype_d *def)
{
  gcc_assert (gimple_has_ops (g));
  g->gsops.opbase.def_ops = def;
}


/* Return the set of USE operands for statement G.  */

static inline struct use_optype_d *
gimple_use_ops (const_gimple g)
{
  if (!gimple_has_ops (g))
    return NULL;
  return g->gsops.opbase.use_ops;
}


/* Set USE to be the set of USE operands for statement G.  */

static inline void
gimple_set_use_ops (gimple g, struct use_optype_d *use)
{
  gcc_assert (gimple_has_ops (g));
  g->gsops.opbase.use_ops = use;
}


/* Return the set of VUSE operand for statement G.  */

static inline use_operand_p
gimple_vuse_op (const_gimple g)
{
  struct use_optype_d *ops;
  if (!gimple_has_mem_ops (g))
    return NULL_USE_OPERAND_P;
  ops = g->gsops.opbase.use_ops;
  if (ops
      && USE_OP_PTR (ops)->use == &g->gsmembase.vuse)
    return USE_OP_PTR (ops);
  return NULL_USE_OPERAND_P;
}

/* Return the set of VDEF operand for statement G.  */

static inline def_operand_p
gimple_vdef_op (const_gimple g)
{
  struct def_optype_d *ops;
  if (!gimple_has_mem_ops (g))
    return NULL_DEF_OPERAND_P;
  ops = g->gsops.opbase.def_ops;
  if (ops
      && DEF_OP_PTR (ops) == &g->gsmembase.vdef)
    return DEF_OP_PTR (ops);
  return NULL_DEF_OPERAND_P;
}


/* Return the single VUSE operand of the statement G.  */

static inline tree
gimple_vuse (const_gimple g)
{
  if (!gimple_has_mem_ops (g))
    return NULL_TREE;
  return g->gsmembase.vuse;
}

/* Return the single VDEF operand of the statement G.  */

static inline tree
gimple_vdef (const_gimple g)
{
  if (!gimple_has_mem_ops (g))
    return NULL_TREE;
  return g->gsmembase.vdef;
}

/* Return the single VUSE operand of the statement G.  */

static inline tree *
gimple_vuse_ptr (gimple g)
{
  if (!gimple_has_mem_ops (g))
    return NULL;
  return &g->gsmembase.vuse;
}

/* Return the single VDEF operand of the statement G.  */

static inline tree *
gimple_vdef_ptr (gimple g)
{
  if (!gimple_has_mem_ops (g))
    return NULL;
  return &g->gsmembase.vdef;
}

/* Set the single VUSE operand of the statement G.  */

static inline void
gimple_set_vuse (gimple g, tree vuse)
{
  gcc_assert (gimple_has_mem_ops (g));
  g->gsmembase.vuse = vuse;
}

/* Set the single VDEF operand of the statement G.  */

static inline void
gimple_set_vdef (gimple g, tree vdef)
{
  gcc_assert (gimple_has_mem_ops (g));
  g->gsmembase.vdef = vdef;
}


/* Return true if statement G has operands and the modified field has
   been set.  */

static inline bool
gimple_modified_p (const_gimple g)
{
  return (gimple_has_ops (g)) ? (bool) g->gsbase.modified : false;
}


/* Return the tree code for the expression computed by STMT.  This is
   only valid for GIMPLE_COND, GIMPLE_CALL and GIMPLE_ASSIGN.  For
   GIMPLE_CALL, return CALL_EXPR as the expression code for
   consistency.  This is useful when the caller needs to deal with the
   three kinds of computation that GIMPLE supports.  */

static inline enum tree_code
gimple_expr_code (const_gimple stmt)
{
  enum gimple_code code = gimple_code (stmt);
  if (code == GIMPLE_ASSIGN || code == GIMPLE_COND)
    return (enum tree_code) stmt->gsbase.subcode;
  else if (code == GIMPLE_CALL)
    return CALL_EXPR;
  else
    gcc_unreachable ();
}


/* Mark statement S as modified, and update it.  */

static inline void
update_stmt (gimple s)
{
  if (gimple_has_ops (s))
    {
      gimple_set_modified (s, true);
      update_stmt_operands (s);
    }
}

/* Update statement S if it has been optimized.  */

static inline void
update_stmt_if_modified (gimple s)
{
  if (gimple_modified_p (s))
    update_stmt_operands (s);
}

/* Return true if statement STMT contains volatile operands.  */

static inline bool
gimple_has_volatile_ops (const_gimple stmt)
{
  if (gimple_has_mem_ops (stmt))
    return stmt->gsbase.has_volatile_ops;
  else
    return false;
}


/* Set the HAS_VOLATILE_OPS flag to VOLATILEP.  */

static inline void
gimple_set_has_volatile_ops (gimple stmt, bool volatilep)
{
  if (gimple_has_mem_ops (stmt))
    stmt->gsbase.has_volatile_ops = (unsigned) volatilep;
}


/* Return true if statement STMT may access memory.  */

static inline bool
gimple_references_memory_p (gimple stmt)
{
  return gimple_has_mem_ops (stmt) && gimple_vuse (stmt);
}


/* Return the subcode for OMP statement S.  */

static inline unsigned
gimple_omp_subcode (const_gimple s)
{
  gcc_assert (gimple_code (s) >= GIMPLE_OMP_ATOMIC_LOAD
	      && gimple_code (s) <= GIMPLE_OMP_SINGLE);
  return s->gsbase.subcode;
}

/* Set the subcode for OMP statement S to SUBCODE.  */

static inline void
gimple_omp_set_subcode (gimple s, unsigned int subcode)
{
  /* We only have 16 bits for the subcode.  Assert that we are not
     overflowing it.  */
  gcc_assert (subcode < (1 << 16));
  s->gsbase.subcode = subcode;
}

/* Set the nowait flag on OMP_RETURN statement S.  */

static inline void
gimple_omp_return_set_nowait (gimple s)
{
  GIMPLE_CHECK (s, GIMPLE_OMP_RETURN);
  s->gsbase.subcode |= GF_OMP_RETURN_NOWAIT;
}


/* Return true if OMP return statement G has the GF_OMP_RETURN_NOWAIT
   flag set.  */

static inline bool
gimple_omp_return_nowait_p (const_gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_RETURN);
  return (gimple_omp_subcode (g) & GF_OMP_RETURN_NOWAIT) != 0;
}


/* Return true if OMP section statement G has the GF_OMP_SECTION_LAST
   flag set.  */

static inline bool
gimple_omp_section_last_p (const_gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_SECTION);
  return (gimple_omp_subcode (g) & GF_OMP_SECTION_LAST) != 0;
}


/* Set the GF_OMP_SECTION_LAST flag on G.  */

static inline void
gimple_omp_section_set_last (gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_SECTION);
  g->gsbase.subcode |= GF_OMP_SECTION_LAST;
}


/* Return true if OMP parallel statement G has the
   GF_OMP_PARALLEL_COMBINED flag set.  */

static inline bool
gimple_omp_parallel_combined_p (const_gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_PARALLEL);
  return (gimple_omp_subcode (g) & GF_OMP_PARALLEL_COMBINED) != 0;
}


/* Set the GF_OMP_PARALLEL_COMBINED field in G depending on the boolean
   value of COMBINED_P.  */

static inline void
gimple_omp_parallel_set_combined_p (gimple g, bool combined_p)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_PARALLEL);
  if (combined_p)
    g->gsbase.subcode |= GF_OMP_PARALLEL_COMBINED;
  else
    g->gsbase.subcode &= ~GF_OMP_PARALLEL_COMBINED;
}


/* Return the number of operands for statement GS.  */

static inline unsigned
gimple_num_ops (const_gimple gs)
{
  return gs->gsbase.num_ops;
}


/* Set the number of operands for statement GS.  */

static inline void
gimple_set_num_ops (gimple gs, unsigned num_ops)
{
  gs->gsbase.num_ops = num_ops;
}


/* Return the array of operands for statement GS.  */

static inline tree *
gimple_ops (gimple gs)
{
  size_t off;

  /* All the tuples have their operand vector at the very bottom
     of the structure.  Note that those structures that do not
     have an operand vector have a zero offset.  */
  off = gimple_ops_offset_[gimple_statement_structure (gs)];
  gcc_assert (off != 0);

  return (tree *) ((char *) gs + off);
}


/* Return operand I for statement GS.  */

static inline tree
gimple_op (const_gimple gs, unsigned i)
{
  if (gimple_has_ops (gs))
    {
#ifdef ENABLE_CHECKING
      gcc_assert (i < gimple_num_ops (gs));
#endif
      return gimple_ops (CONST_CAST_GIMPLE (gs))[i];
    }
  else
    return NULL_TREE;
}

/* Return a pointer to operand I for statement GS.  */

static inline tree *
gimple_op_ptr (const_gimple gs, unsigned i)
{
  if (gimple_has_ops (gs))
    {
#ifdef ENABLE_CHECKING
      gcc_assert (i < gimple_num_ops (gs));
#endif
      return gimple_ops (CONST_CAST_GIMPLE (gs)) + i;
    }
  else
    return NULL;
}

/* Set operand I of statement GS to OP.  */

static inline void
gimple_set_op (gimple gs, unsigned i, tree op)
{
  gcc_assert (gimple_has_ops (gs) && i < gimple_num_ops (gs));

  /* Note.  It may be tempting to assert that OP matches
     is_gimple_operand, but that would be wrong.  Different tuples
     accept slightly different sets of tree operands.  Each caller
     should perform its own validation.  */
  gimple_ops (gs)[i] = op;
}

/* Return true if GS is a GIMPLE_ASSIGN.  */

static inline bool
is_gimple_assign (const_gimple gs)
{
  return gimple_code (gs) == GIMPLE_ASSIGN;
}

/* Determine if expression CODE is one of the valid expressions that can
   be used on the RHS of GIMPLE assignments.  */

static inline enum gimple_rhs_class
get_gimple_rhs_class (enum tree_code code)
{
  return (enum gimple_rhs_class) gimple_rhs_class_table[(int) code];
}

/* Return the LHS of assignment statement GS.  */

static inline tree
gimple_assign_lhs (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);
  return gimple_op (gs, 0);
}


/* Return a pointer to the LHS of assignment statement GS.  */

static inline tree *
gimple_assign_lhs_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);
  return gimple_op_ptr (gs, 0);
}


/* Set LHS to be the LHS operand of assignment statement GS.  */

static inline void
gimple_assign_set_lhs (gimple gs, tree lhs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);
  gimple_set_op (gs, 0, lhs);

  if (lhs && TREE_CODE (lhs) == SSA_NAME)
    SSA_NAME_DEF_STMT (lhs) = gs;
}


/* Return the first operand on the RHS of assignment statement GS.  */

static inline tree
gimple_assign_rhs1 (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);
  return gimple_op (gs, 1);
}


/* Return a pointer to the first operand on the RHS of assignment
   statement GS.  */

static inline tree *
gimple_assign_rhs1_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);
  return gimple_op_ptr (gs, 1);
}

/* Set RHS to be the first operand on the RHS of assignment statement GS.  */

static inline void
gimple_assign_set_rhs1 (gimple gs, tree rhs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);

  gimple_set_op (gs, 1, rhs);
}


/* Return the second operand on the RHS of assignment statement GS.
   If GS does not have two operands, NULL is returned instead.  */

static inline tree
gimple_assign_rhs2 (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);

  if (gimple_num_ops (gs) >= 3)
    return gimple_op (gs, 2);
  else
    return NULL_TREE;
}


/* Return a pointer to the second operand on the RHS of assignment
   statement GS.  */

static inline tree *
gimple_assign_rhs2_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);
  return gimple_op_ptr (gs, 2);
}


/* Set RHS to be the second operand on the RHS of assignment statement GS.  */

static inline void
gimple_assign_set_rhs2 (gimple gs, tree rhs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);

  gimple_set_op (gs, 2, rhs);
}

/* Return the third operand on the RHS of assignment statement GS.
   If GS does not have two operands, NULL is returned instead.  */

static inline tree
gimple_assign_rhs3 (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);

  if (gimple_num_ops (gs) >= 4)
    return gimple_op (gs, 3);
  else
    return NULL_TREE;
}

/* Return a pointer to the third operand on the RHS of assignment
   statement GS.  */

static inline tree *
gimple_assign_rhs3_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);
  return gimple_op_ptr (gs, 3);
}


/* Set RHS to be the third operand on the RHS of assignment statement GS.  */

static inline void
gimple_assign_set_rhs3 (gimple gs, tree rhs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);

  gimple_set_op (gs, 3, rhs);
}

/* A wrapper around gimple_assign_set_rhs_with_ops_1, for callers which expect
   to see only a maximum of two operands.  */

static inline void
gimple_assign_set_rhs_with_ops (gimple_stmt_iterator *gsi, enum tree_code code,
				tree op1, tree op2)
{
  gimple_assign_set_rhs_with_ops_1 (gsi, code, op1, op2, NULL);
}

/* A wrapper around extract_ops_from_tree_1, for callers which expect
   to see only a maximum of two operands.  */

static inline void
extract_ops_from_tree (tree expr, enum tree_code *code, tree *op0,
		       tree *op1)
{
  tree op2;
  extract_ops_from_tree_1 (expr, code, op0, op1, &op2);
  gcc_assert (op2 == NULL_TREE);
}

/* Returns true if GS is a nontemporal move.  */

static inline bool
gimple_assign_nontemporal_move_p (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);
  return gs->gsbase.nontemporal_move;
}

/* Sets nontemporal move flag of GS to NONTEMPORAL.  */

static inline void
gimple_assign_set_nontemporal_move (gimple gs, bool nontemporal)
{
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);
  gs->gsbase.nontemporal_move = nontemporal;
}


/* Return the code of the expression computed on the rhs of assignment
   statement GS.  In case that the RHS is a single object, returns the
   tree code of the object.  */

static inline enum tree_code
gimple_assign_rhs_code (const_gimple gs)
{
  enum tree_code code;
  GIMPLE_CHECK (gs, GIMPLE_ASSIGN);

  code = gimple_expr_code (gs);
  if (get_gimple_rhs_class (code) == GIMPLE_SINGLE_RHS)
    code = TREE_CODE (gimple_assign_rhs1 (gs));

  return code;
}


/* Set CODE to be the code for the expression computed on the RHS of
   assignment S.  */

static inline void
gimple_assign_set_rhs_code (gimple s, enum tree_code code)
{
  GIMPLE_CHECK (s, GIMPLE_ASSIGN);
  s->gsbase.subcode = code;
}


/* Return the gimple rhs class of the code of the expression computed on
   the rhs of assignment statement GS.
   This will never return GIMPLE_INVALID_RHS.  */

static inline enum gimple_rhs_class
gimple_assign_rhs_class (const_gimple gs)
{
  return get_gimple_rhs_class (gimple_assign_rhs_code (gs));
}


/* Return true if S is a type-cast assignment.  */

static inline bool
gimple_assign_cast_p (gimple s)
{
  if (is_gimple_assign (s))
    {
      enum tree_code sc = gimple_assign_rhs_code (s);
      return CONVERT_EXPR_CODE_P (sc)
	     || sc == VIEW_CONVERT_EXPR
	     || sc == FIX_TRUNC_EXPR;
    }

  return false;
}


/* Return true if GS is a GIMPLE_CALL.  */

static inline bool
is_gimple_call (const_gimple gs)
{
  return gimple_code (gs) == GIMPLE_CALL;
}

/* Return the LHS of call statement GS.  */

static inline tree
gimple_call_lhs (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  return gimple_op (gs, 0);
}


/* Return a pointer to the LHS of call statement GS.  */

static inline tree *
gimple_call_lhs_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  return gimple_op_ptr (gs, 0);
}


/* Set LHS to be the LHS operand of call statement GS.  */

static inline void
gimple_call_set_lhs (gimple gs, tree lhs)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  gimple_set_op (gs, 0, lhs);
  if (lhs && TREE_CODE (lhs) == SSA_NAME)
    SSA_NAME_DEF_STMT (lhs) = gs;
}


/* Return the tree node representing the function called by call
   statement GS.  */

static inline tree
gimple_call_fn (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  return gimple_op (gs, 1);
}


/* Return a pointer to the tree node representing the function called by call
   statement GS.  */

static inline tree *
gimple_call_fn_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  return gimple_op_ptr (gs, 1);
}


/* Set FN to be the function called by call statement GS.  */

static inline void
gimple_call_set_fn (gimple gs, tree fn)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  gimple_set_op (gs, 1, fn);
}


/* Set FNDECL to be the function called by call statement GS.  */

static inline void
gimple_call_set_fndecl (gimple gs, tree decl)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  gimple_set_op (gs, 1, build_fold_addr_expr_loc (gimple_location (gs), decl));
}


/* If a given GIMPLE_CALL's callee is a FUNCTION_DECL, return it.
   Otherwise return NULL.  This function is analogous to
   get_callee_fndecl in tree land.  */

static inline tree
gimple_call_fndecl (const_gimple gs)
{
  tree addr = gimple_call_fn (gs);
  if (TREE_CODE (addr) == ADDR_EXPR)
    return TREE_OPERAND (addr, 0);
  return NULL_TREE;
}


/* Return the type returned by call statement GS.  */

static inline tree
gimple_call_return_type (const_gimple gs)
{
  tree fn = gimple_call_fn (gs);
  tree type = TREE_TYPE (fn);

  /* See through the pointer.  */
  type = TREE_TYPE (type);

  /* The type returned by a FUNCTION_DECL is the type of its
     function type.  */
  return TREE_TYPE (type);
}


/* Return the static chain for call statement GS.  */

static inline tree
gimple_call_chain (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  return gimple_op (gs, 2);
}


/* Return a pointer to the static chain for call statement GS.  */

static inline tree *
gimple_call_chain_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  return gimple_op_ptr (gs, 2);
}

/* Set CHAIN to be the static chain for call statement GS.  */

static inline void
gimple_call_set_chain (gimple gs, tree chain)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);

  gimple_set_op (gs, 2, chain);
}


/* Return the number of arguments used by call statement GS.  */

static inline unsigned
gimple_call_num_args (const_gimple gs)
{
  unsigned num_ops;
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  num_ops = gimple_num_ops (gs);
  return num_ops - 3;
}


/* Return the argument at position INDEX for call statement GS.  */

static inline tree
gimple_call_arg (const_gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  return gimple_op (gs, index + 3);
}


/* Return a pointer to the argument at position INDEX for call
   statement GS.  */

static inline tree *
gimple_call_arg_ptr (const_gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  return gimple_op_ptr (gs, index + 3);
}


/* Set ARG to be the argument at position INDEX for call statement GS.  */

static inline void
gimple_call_set_arg (gimple gs, unsigned index, tree arg)
{
  GIMPLE_CHECK (gs, GIMPLE_CALL);
  gimple_set_op (gs, index + 3, arg);
}


/* If TAIL_P is true, mark call statement S as being a tail call
   (i.e., a call just before the exit of a function).  These calls are
   candidate for tail call optimization.  */

static inline void
gimple_call_set_tail (gimple s, bool tail_p)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  if (tail_p)
    s->gsbase.subcode |= GF_CALL_TAILCALL;
  else
    s->gsbase.subcode &= ~GF_CALL_TAILCALL;
}


/* Return true if GIMPLE_CALL S is marked as a tail call.  */

static inline bool
gimple_call_tail_p (gimple s)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  return (s->gsbase.subcode & GF_CALL_TAILCALL) != 0;
}


/* Set the inlinable status of GIMPLE_CALL S to INLINABLE_P.  */

static inline void
gimple_call_set_cannot_inline (gimple s, bool inlinable_p)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  if (inlinable_p)
    s->gsbase.subcode |= GF_CALL_CANNOT_INLINE;
  else
    s->gsbase.subcode &= ~GF_CALL_CANNOT_INLINE;
}


/* Return true if GIMPLE_CALL S cannot be inlined.  */

static inline bool
gimple_call_cannot_inline_p (gimple s)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  return (s->gsbase.subcode & GF_CALL_CANNOT_INLINE) != 0;
}


/* If RETURN_SLOT_OPT_P is true mark GIMPLE_CALL S as valid for return
   slot optimization.  This transformation uses the target of the call
   expansion as the return slot for calls that return in memory.  */

static inline void
gimple_call_set_return_slot_opt (gimple s, bool return_slot_opt_p)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  if (return_slot_opt_p)
    s->gsbase.subcode |= GF_CALL_RETURN_SLOT_OPT;
  else
    s->gsbase.subcode &= ~GF_CALL_RETURN_SLOT_OPT;
}


/* Return true if S is marked for return slot optimization.  */

static inline bool
gimple_call_return_slot_opt_p (gimple s)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  return (s->gsbase.subcode & GF_CALL_RETURN_SLOT_OPT) != 0;
}


/* If FROM_THUNK_P is true, mark GIMPLE_CALL S as being the jump from a
   thunk to the thunked-to function.  */

static inline void
gimple_call_set_from_thunk (gimple s, bool from_thunk_p)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  if (from_thunk_p)
    s->gsbase.subcode |= GF_CALL_FROM_THUNK;
  else
    s->gsbase.subcode &= ~GF_CALL_FROM_THUNK;
}


/* Return true if GIMPLE_CALL S is a jump from a thunk.  */

static inline bool
gimple_call_from_thunk_p (gimple s)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  return (s->gsbase.subcode & GF_CALL_FROM_THUNK) != 0;
}


/* If PASS_ARG_PACK_P is true, GIMPLE_CALL S is a stdarg call that needs the
   argument pack in its argument list.  */

static inline void
gimple_call_set_va_arg_pack (gimple s, bool pass_arg_pack_p)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  if (pass_arg_pack_p)
    s->gsbase.subcode |= GF_CALL_VA_ARG_PACK;
  else
    s->gsbase.subcode &= ~GF_CALL_VA_ARG_PACK;
}


/* Return true if GIMPLE_CALL S is a stdarg call that needs the
   argument pack in its argument list.  */

static inline bool
gimple_call_va_arg_pack_p (gimple s)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  return (s->gsbase.subcode & GF_CALL_VA_ARG_PACK) != 0;
}


/* Return true if S is a noreturn call.  */

static inline bool
gimple_call_noreturn_p (gimple s)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  return (gimple_call_flags (s) & ECF_NORETURN) != 0;
}


/* If NOTHROW_P is true, GIMPLE_CALL S is a call that is known to not throw
   even if the called function can throw in other cases.  */

static inline void
gimple_call_set_nothrow (gimple s, bool nothrow_p)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  if (nothrow_p)
    s->gsbase.subcode |= GF_CALL_NOTHROW;
  else
    s->gsbase.subcode &= ~GF_CALL_NOTHROW;
}

/* Return true if S is a nothrow call.  */

static inline bool
gimple_call_nothrow_p (gimple s)
{
  GIMPLE_CHECK (s, GIMPLE_CALL);
  return (gimple_call_flags (s) & ECF_NOTHROW) != 0;
}


/* Copy all the GF_CALL_* flags from ORIG_CALL to DEST_CALL.  */

static inline void
gimple_call_copy_flags (gimple dest_call, gimple orig_call)
{
  GIMPLE_CHECK (dest_call, GIMPLE_CALL);
  GIMPLE_CHECK (orig_call, GIMPLE_CALL);
  dest_call->gsbase.subcode = orig_call->gsbase.subcode;
}


/* Returns true if this is a GIMPLE_ASSIGN or a GIMPLE_CALL with a
   non-NULL lhs.  */

static inline bool
gimple_has_lhs (gimple stmt)
{
  return (is_gimple_assign (stmt)
	  || (is_gimple_call (stmt)
	      && gimple_call_lhs (stmt) != NULL_TREE));
}


/* Return the code of the predicate computed by conditional statement GS.  */

static inline enum tree_code
gimple_cond_code (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  return (enum tree_code) gs->gsbase.subcode;
}


/* Set CODE to be the predicate code for the conditional statement GS.  */

static inline void
gimple_cond_set_code (gimple gs, enum tree_code code)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  gs->gsbase.subcode = code;
}


/* Return the LHS of the predicate computed by conditional statement GS.  */

static inline tree
gimple_cond_lhs (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  return gimple_op (gs, 0);
}

/* Return the pointer to the LHS of the predicate computed by conditional
   statement GS.  */

static inline tree *
gimple_cond_lhs_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  return gimple_op_ptr (gs, 0);
}

/* Set LHS to be the LHS operand of the predicate computed by
   conditional statement GS.  */

static inline void
gimple_cond_set_lhs (gimple gs, tree lhs)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  gimple_set_op (gs, 0, lhs);
}


/* Return the RHS operand of the predicate computed by conditional GS.  */

static inline tree
gimple_cond_rhs (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  return gimple_op (gs, 1);
}

/* Return the pointer to the RHS operand of the predicate computed by
   conditional GS.  */

static inline tree *
gimple_cond_rhs_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  return gimple_op_ptr (gs, 1);
}


/* Set RHS to be the RHS operand of the predicate computed by
   conditional statement GS.  */

static inline void
gimple_cond_set_rhs (gimple gs, tree rhs)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  gimple_set_op (gs, 1, rhs);
}


/* Return the label used by conditional statement GS when its
   predicate evaluates to true.  */

static inline tree
gimple_cond_true_label (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  return gimple_op (gs, 2);
}


/* Set LABEL to be the label used by conditional statement GS when its
   predicate evaluates to true.  */

static inline void
gimple_cond_set_true_label (gimple gs, tree label)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  gimple_set_op (gs, 2, label);
}


/* Set LABEL to be the label used by conditional statement GS when its
   predicate evaluates to false.  */

static inline void
gimple_cond_set_false_label (gimple gs, tree label)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  gimple_set_op (gs, 3, label);
}


/* Return the label used by conditional statement GS when its
   predicate evaluates to false.  */

static inline tree
gimple_cond_false_label (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_COND);
  return gimple_op (gs, 3);
}


/* Set the conditional COND_STMT to be of the form 'if (1 == 0)'.  */

static inline void
gimple_cond_make_false (gimple gs)
{
  gimple_cond_set_lhs (gs, boolean_true_node);
  gimple_cond_set_rhs (gs, boolean_false_node);
  gs->gsbase.subcode = EQ_EXPR;
}


/* Set the conditional COND_STMT to be of the form 'if (1 == 1)'.  */

static inline void
gimple_cond_make_true (gimple gs)
{
  gimple_cond_set_lhs (gs, boolean_true_node);
  gimple_cond_set_rhs (gs, boolean_true_node);
  gs->gsbase.subcode = EQ_EXPR;
}

/* Check if conditional statemente GS is of the form 'if (1 == 1)',
  'if (0 == 0)', 'if (1 != 0)' or 'if (0 != 1)' */

static inline bool
gimple_cond_true_p (const_gimple gs)
{
  tree lhs = gimple_cond_lhs (gs);
  tree rhs = gimple_cond_rhs (gs);
  enum tree_code code = gimple_cond_code (gs);

  if (lhs != boolean_true_node && lhs != boolean_false_node)
    return false;

  if (rhs != boolean_true_node && rhs != boolean_false_node)
    return false;

  if (code == NE_EXPR && lhs != rhs)
    return true;

  if (code == EQ_EXPR && lhs == rhs)
      return true;

  return false;
}

/* Check if conditional statement GS is of the form 'if (1 != 1)',
   'if (0 != 0)', 'if (1 == 0)' or 'if (0 == 1)' */

static inline bool
gimple_cond_false_p (const_gimple gs)
{
  tree lhs = gimple_cond_lhs (gs);
  tree rhs = gimple_cond_rhs (gs);
  enum tree_code code = gimple_cond_code (gs);

  if (lhs != boolean_true_node && lhs != boolean_false_node)
    return false;

  if (rhs != boolean_true_node && rhs != boolean_false_node)
    return false;

  if (code == NE_EXPR && lhs == rhs)
    return true;

  if (code == EQ_EXPR && lhs != rhs)
      return true;

  return false;
}

/* Check if conditional statement GS is of the form 'if (var != 0)' or
   'if (var == 1)' */

static inline bool
gimple_cond_single_var_p (gimple gs)
{
  if (gimple_cond_code (gs) == NE_EXPR
      && gimple_cond_rhs (gs) == boolean_false_node)
    return true;

  if (gimple_cond_code (gs) == EQ_EXPR
      && gimple_cond_rhs (gs) == boolean_true_node)
    return true;

  return false;
}

/* Set the code, LHS and RHS of GIMPLE_COND STMT from CODE, LHS and RHS.  */

static inline void
gimple_cond_set_condition (gimple stmt, enum tree_code code, tree lhs, tree rhs)
{
  gimple_cond_set_code (stmt, code);
  gimple_cond_set_lhs (stmt, lhs);
  gimple_cond_set_rhs (stmt, rhs);
}

/* Return the LABEL_DECL node used by GIMPLE_LABEL statement GS.  */

static inline tree
gimple_label_label (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_LABEL);
  return gimple_op (gs, 0);
}


/* Set LABEL to be the LABEL_DECL node used by GIMPLE_LABEL statement
   GS.  */

static inline void
gimple_label_set_label (gimple gs, tree label)
{
  GIMPLE_CHECK (gs, GIMPLE_LABEL);
  gimple_set_op (gs, 0, label);
}


/* Return the destination of the unconditional jump GS.  */

static inline tree
gimple_goto_dest (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_GOTO);
  return gimple_op (gs, 0);
}


/* Set DEST to be the destination of the unconditonal jump GS.  */

static inline void
gimple_goto_set_dest (gimple gs, tree dest)
{
  GIMPLE_CHECK (gs, GIMPLE_GOTO);
  gimple_set_op (gs, 0, dest);
}


/* Return the variables declared in the GIMPLE_BIND statement GS.  */

static inline tree
gimple_bind_vars (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_BIND);
  return gs->gimple_bind.vars;
}


/* Set VARS to be the set of variables declared in the GIMPLE_BIND
   statement GS.  */

static inline void
gimple_bind_set_vars (gimple gs, tree vars)
{
  GIMPLE_CHECK (gs, GIMPLE_BIND);
  gs->gimple_bind.vars = vars;
}


/* Append VARS to the set of variables declared in the GIMPLE_BIND
   statement GS.  */

static inline void
gimple_bind_append_vars (gimple gs, tree vars)
{
  GIMPLE_CHECK (gs, GIMPLE_BIND);
  gs->gimple_bind.vars = chainon (gs->gimple_bind.vars, vars);
}


/* Return the GIMPLE sequence contained in the GIMPLE_BIND statement GS.  */

static inline gimple_seq
gimple_bind_body (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_BIND);
  return gs->gimple_bind.body;
}


/* Set SEQ to be the GIMPLE sequence contained in the GIMPLE_BIND
   statement GS.  */

static inline void
gimple_bind_set_body (gimple gs, gimple_seq seq)
{
  GIMPLE_CHECK (gs, GIMPLE_BIND);
  gs->gimple_bind.body = seq;
}


/* Append a statement to the end of a GIMPLE_BIND's body.  */

static inline void
gimple_bind_add_stmt (gimple gs, gimple stmt)
{
  GIMPLE_CHECK (gs, GIMPLE_BIND);
  gimple_seq_add_stmt (&gs->gimple_bind.body, stmt);
}


/* Append a sequence of statements to the end of a GIMPLE_BIND's body.  */

static inline void
gimple_bind_add_seq (gimple gs, gimple_seq seq)
{
  GIMPLE_CHECK (gs, GIMPLE_BIND);
  gimple_seq_add_seq (&gs->gimple_bind.body, seq);
}


/* Return the TREE_BLOCK node associated with GIMPLE_BIND statement
   GS.  This is analogous to the BIND_EXPR_BLOCK field in trees.  */

static inline tree
gimple_bind_block (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_BIND);
  return gs->gimple_bind.block;
}


/* Set BLOCK to be the TREE_BLOCK node associated with GIMPLE_BIND
   statement GS.  */

static inline void
gimple_bind_set_block (gimple gs, tree block)
{
  GIMPLE_CHECK (gs, GIMPLE_BIND);
  gcc_assert (block == NULL_TREE || TREE_CODE (block) == BLOCK);
  gs->gimple_bind.block = block;
}


/* Return the number of input operands for GIMPLE_ASM GS.  */

static inline unsigned
gimple_asm_ninputs (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  return gs->gimple_asm.ni;
}


/* Return the number of output operands for GIMPLE_ASM GS.  */

static inline unsigned
gimple_asm_noutputs (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  return gs->gimple_asm.no;
}


/* Return the number of clobber operands for GIMPLE_ASM GS.  */

static inline unsigned
gimple_asm_nclobbers (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  return gs->gimple_asm.nc;
}

/* Return the number of label operands for GIMPLE_ASM GS.  */

static inline unsigned
gimple_asm_nlabels (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  return gs->gimple_asm.nl;
}

/* Return input operand INDEX of GIMPLE_ASM GS.  */

static inline tree
gimple_asm_input_op (const_gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.ni);
  return gimple_op (gs, index);
}

/* Return a pointer to input operand INDEX of GIMPLE_ASM GS.  */

static inline tree *
gimple_asm_input_op_ptr (const_gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.ni);
  return gimple_op_ptr (gs, index);
}


/* Set IN_OP to be input operand INDEX in GIMPLE_ASM GS.  */

static inline void
gimple_asm_set_input_op (gimple gs, unsigned index, tree in_op)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.ni);
  gcc_assert (TREE_CODE (in_op) == TREE_LIST);
  gimple_set_op (gs, index, in_op);
}


/* Return output operand INDEX of GIMPLE_ASM GS.  */

static inline tree
gimple_asm_output_op (const_gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.no);
  return gimple_op (gs, index + gs->gimple_asm.ni);
}

/* Return a pointer to output operand INDEX of GIMPLE_ASM GS.  */

static inline tree *
gimple_asm_output_op_ptr (const_gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.no);
  return gimple_op_ptr (gs, index + gs->gimple_asm.ni);
}


/* Set OUT_OP to be output operand INDEX in GIMPLE_ASM GS.  */

static inline void
gimple_asm_set_output_op (gimple gs, unsigned index, tree out_op)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.no);
  gcc_assert (TREE_CODE (out_op) == TREE_LIST);
  gimple_set_op (gs, index + gs->gimple_asm.ni, out_op);
}


/* Return clobber operand INDEX of GIMPLE_ASM GS.  */

static inline tree
gimple_asm_clobber_op (const_gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.nc);
  return gimple_op (gs, index + gs->gimple_asm.ni + gs->gimple_asm.no);
}


/* Set CLOBBER_OP to be clobber operand INDEX in GIMPLE_ASM GS.  */

static inline void
gimple_asm_set_clobber_op (gimple gs, unsigned index, tree clobber_op)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.nc);
  gcc_assert (TREE_CODE (clobber_op) == TREE_LIST);
  gimple_set_op (gs, index + gs->gimple_asm.ni + gs->gimple_asm.no, clobber_op);
}

/* Return label operand INDEX of GIMPLE_ASM GS.  */

static inline tree
gimple_asm_label_op (const_gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.nl);
  return gimple_op (gs, index + gs->gimple_asm.ni + gs->gimple_asm.nc);
}

/* Set LABEL_OP to be label operand INDEX in GIMPLE_ASM GS.  */

static inline void
gimple_asm_set_label_op (gimple gs, unsigned index, tree label_op)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  gcc_assert (index <= gs->gimple_asm.nl);
  gcc_assert (TREE_CODE (label_op) == TREE_LIST);
  gimple_set_op (gs, index + gs->gimple_asm.ni + gs->gimple_asm.nc, label_op);
}

/* Return the string representing the assembly instruction in
   GIMPLE_ASM GS.  */

static inline const char *
gimple_asm_string (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  return gs->gimple_asm.string;
}


/* Return true if GS is an asm statement marked volatile.  */

static inline bool
gimple_asm_volatile_p (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  return (gs->gsbase.subcode & GF_ASM_VOLATILE) != 0;
}


/* If VOLATLE_P is true, mark asm statement GS as volatile.  */

static inline void
gimple_asm_set_volatile (gimple gs, bool volatile_p)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  if (volatile_p)
    gs->gsbase.subcode |= GF_ASM_VOLATILE;
  else
    gs->gsbase.subcode &= ~GF_ASM_VOLATILE;
}


/* If INPUT_P is true, mark asm GS as an ASM_INPUT.  */

static inline void
gimple_asm_set_input (gimple gs, bool input_p)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  if (input_p)
    gs->gsbase.subcode |= GF_ASM_INPUT;
  else
    gs->gsbase.subcode &= ~GF_ASM_INPUT;
}


/* Return true if asm GS is an ASM_INPUT.  */

static inline bool
gimple_asm_input_p (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_ASM);
  return (gs->gsbase.subcode & GF_ASM_INPUT) != 0;
}


/* Return the types handled by GIMPLE_CATCH statement GS.  */

static inline tree
gimple_catch_types (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CATCH);
  return gs->gimple_catch.types;
}


/* Return a pointer to the types handled by GIMPLE_CATCH statement GS.  */

static inline tree *
gimple_catch_types_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CATCH);
  return &gs->gimple_catch.types;
}


/* Return the GIMPLE sequence representing the body of the handler of
   GIMPLE_CATCH statement GS.  */

static inline gimple_seq
gimple_catch_handler (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CATCH);
  return gs->gimple_catch.handler;
}


/* Return a pointer to the GIMPLE sequence representing the body of
   the handler of GIMPLE_CATCH statement GS.  */

static inline gimple_seq *
gimple_catch_handler_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_CATCH);
  return &gs->gimple_catch.handler;
}


/* Set T to be the set of types handled by GIMPLE_CATCH GS.  */

static inline void
gimple_catch_set_types (gimple gs, tree t)
{
  GIMPLE_CHECK (gs, GIMPLE_CATCH);
  gs->gimple_catch.types = t;
}


/* Set HANDLER to be the body of GIMPLE_CATCH GS.  */

static inline void
gimple_catch_set_handler (gimple gs, gimple_seq handler)
{
  GIMPLE_CHECK (gs, GIMPLE_CATCH);
  gs->gimple_catch.handler = handler;
}


/* Return the types handled by GIMPLE_EH_FILTER statement GS.  */

static inline tree
gimple_eh_filter_types (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_EH_FILTER);
  return gs->gimple_eh_filter.types;
}


/* Return a pointer to the types handled by GIMPLE_EH_FILTER statement
   GS.  */

static inline tree *
gimple_eh_filter_types_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_EH_FILTER);
  return &gs->gimple_eh_filter.types;
}


/* Return the sequence of statement to execute when GIMPLE_EH_FILTER
   statement fails.  */

static inline gimple_seq
gimple_eh_filter_failure (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_EH_FILTER);
  return gs->gimple_eh_filter.failure;
}


/* Set TYPES to be the set of types handled by GIMPLE_EH_FILTER GS.  */

static inline void
gimple_eh_filter_set_types (gimple gs, tree types)
{
  GIMPLE_CHECK (gs, GIMPLE_EH_FILTER);
  gs->gimple_eh_filter.types = types;
}


/* Set FAILURE to be the sequence of statements to execute on failure
   for GIMPLE_EH_FILTER GS.  */

static inline void
gimple_eh_filter_set_failure (gimple gs, gimple_seq failure)
{
  GIMPLE_CHECK (gs, GIMPLE_EH_FILTER);
  gs->gimple_eh_filter.failure = failure;
}

/* Get the function decl to be called by the MUST_NOT_THROW region.  */

static inline tree
gimple_eh_must_not_throw_fndecl (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_EH_MUST_NOT_THROW);
  return gs->gimple_eh_mnt.fndecl;
}

/* Set the function decl to be called by GS to DECL.  */

static inline void
gimple_eh_must_not_throw_set_fndecl (gimple gs, tree decl)
{
  GIMPLE_CHECK (gs, GIMPLE_EH_MUST_NOT_THROW);
  gs->gimple_eh_mnt.fndecl = decl;
}


/* GIMPLE_TRY accessors. */

/* Return the kind of try block represented by GIMPLE_TRY GS.  This is
   either GIMPLE_TRY_CATCH or GIMPLE_TRY_FINALLY.  */

static inline enum gimple_try_flags
gimple_try_kind (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_TRY);
  return (enum gimple_try_flags) (gs->gsbase.subcode & GIMPLE_TRY_KIND);
}


/* Set the kind of try block represented by GIMPLE_TRY GS.  */

static inline void
gimple_try_set_kind (gimple gs, enum gimple_try_flags kind)
{
  GIMPLE_CHECK (gs, GIMPLE_TRY);
  gcc_assert (kind == GIMPLE_TRY_CATCH || kind == GIMPLE_TRY_FINALLY);
  if (gimple_try_kind (gs) != kind)
    gs->gsbase.subcode = (unsigned int) kind;
}


/* Return the GIMPLE_TRY_CATCH_IS_CLEANUP flag.  */

static inline bool
gimple_try_catch_is_cleanup (const_gimple gs)
{
  gcc_assert (gimple_try_kind (gs) == GIMPLE_TRY_CATCH);
  return (gs->gsbase.subcode & GIMPLE_TRY_CATCH_IS_CLEANUP) != 0;
}


/* Return the sequence of statements used as the body for GIMPLE_TRY GS.  */

static inline gimple_seq
gimple_try_eval (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_TRY);
  return gs->gimple_try.eval;
}


/* Return the sequence of statements used as the cleanup body for
   GIMPLE_TRY GS.  */

static inline gimple_seq
gimple_try_cleanup (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_TRY);
  return gs->gimple_try.cleanup;
}


/* Set the GIMPLE_TRY_CATCH_IS_CLEANUP flag.  */

static inline void
gimple_try_set_catch_is_cleanup (gimple g, bool catch_is_cleanup)
{
  gcc_assert (gimple_try_kind (g) == GIMPLE_TRY_CATCH);
  if (catch_is_cleanup)
    g->gsbase.subcode |= GIMPLE_TRY_CATCH_IS_CLEANUP;
  else
    g->gsbase.subcode &= ~GIMPLE_TRY_CATCH_IS_CLEANUP;
}


/* Set EVAL to be the sequence of statements to use as the body for
   GIMPLE_TRY GS.  */

static inline void
gimple_try_set_eval (gimple gs, gimple_seq eval)
{
  GIMPLE_CHECK (gs, GIMPLE_TRY);
  gs->gimple_try.eval = eval;
}


/* Set CLEANUP to be the sequence of statements to use as the cleanup
   body for GIMPLE_TRY GS.  */

static inline void
gimple_try_set_cleanup (gimple gs, gimple_seq cleanup)
{
  GIMPLE_CHECK (gs, GIMPLE_TRY);
  gs->gimple_try.cleanup = cleanup;
}


/* Return the cleanup sequence for cleanup statement GS.  */

static inline gimple_seq
gimple_wce_cleanup (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_WITH_CLEANUP_EXPR);
  return gs->gimple_wce.cleanup;
}


/* Set CLEANUP to be the cleanup sequence for GS.  */

static inline void
gimple_wce_set_cleanup (gimple gs, gimple_seq cleanup)
{
  GIMPLE_CHECK (gs, GIMPLE_WITH_CLEANUP_EXPR);
  gs->gimple_wce.cleanup = cleanup;
}


/* Return the CLEANUP_EH_ONLY flag for a WCE tuple.  */

static inline bool
gimple_wce_cleanup_eh_only (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_WITH_CLEANUP_EXPR);
  return gs->gsbase.subcode != 0;
}


/* Set the CLEANUP_EH_ONLY flag for a WCE tuple.  */

static inline void
gimple_wce_set_cleanup_eh_only (gimple gs, bool eh_only_p)
{
  GIMPLE_CHECK (gs, GIMPLE_WITH_CLEANUP_EXPR);
  gs->gsbase.subcode = (unsigned int) eh_only_p;
}


/* Return the maximum number of arguments supported by GIMPLE_PHI GS.  */

static inline unsigned
gimple_phi_capacity (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_PHI);
  return gs->gimple_phi.capacity;
}


/* Return the number of arguments in GIMPLE_PHI GS.  This must always
   be exactly the number of incoming edges for the basic block holding
   GS.  */

static inline unsigned
gimple_phi_num_args (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_PHI);
  return gs->gimple_phi.nargs;
}


/* Return the SSA name created by GIMPLE_PHI GS.  */

static inline tree
gimple_phi_result (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_PHI);
  return gs->gimple_phi.result;
}

/* Return a pointer to the SSA name created by GIMPLE_PHI GS.  */

static inline tree *
gimple_phi_result_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_PHI);
  return &gs->gimple_phi.result;
}

/* Set RESULT to be the SSA name created by GIMPLE_PHI GS.  */

static inline void
gimple_phi_set_result (gimple gs, tree result)
{
  GIMPLE_CHECK (gs, GIMPLE_PHI);
  gs->gimple_phi.result = result;
}


/* Return the PHI argument corresponding to incoming edge INDEX for
   GIMPLE_PHI GS.  */

static inline struct phi_arg_d *
gimple_phi_arg (gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_PHI);
  gcc_assert (index <= gs->gimple_phi.capacity);
  return &(gs->gimple_phi.args[index]);
}

/* Set PHIARG to be the argument corresponding to incoming edge INDEX
   for GIMPLE_PHI GS.  */

static inline void
gimple_phi_set_arg (gimple gs, unsigned index, struct phi_arg_d * phiarg)
{
  GIMPLE_CHECK (gs, GIMPLE_PHI);
  gcc_assert (index <= gs->gimple_phi.nargs);
  memcpy (gs->gimple_phi.args + index, phiarg, sizeof (struct phi_arg_d));
}

/* Return the region number for GIMPLE_RESX GS.  */

static inline int
gimple_resx_region (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_RESX);
  return gs->gimple_eh_ctrl.region;
}

/* Set REGION to be the region number for GIMPLE_RESX GS.  */

static inline void
gimple_resx_set_region (gimple gs, int region)
{
  GIMPLE_CHECK (gs, GIMPLE_RESX);
  gs->gimple_eh_ctrl.region = region;
}

/* Return the region number for GIMPLE_EH_DISPATCH GS.  */

static inline int
gimple_eh_dispatch_region (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_EH_DISPATCH);
  return gs->gimple_eh_ctrl.region;
}

/* Set REGION to be the region number for GIMPLE_EH_DISPATCH GS.  */

static inline void
gimple_eh_dispatch_set_region (gimple gs, int region)
{
  GIMPLE_CHECK (gs, GIMPLE_EH_DISPATCH);
  gs->gimple_eh_ctrl.region = region;
}

/* Return the number of labels associated with the switch statement GS.  */

static inline unsigned
gimple_switch_num_labels (const_gimple gs)
{
  unsigned num_ops;
  GIMPLE_CHECK (gs, GIMPLE_SWITCH);
  num_ops = gimple_num_ops (gs);
  gcc_assert (num_ops > 1);
  return num_ops - 1;
}


/* Set NLABELS to be the number of labels for the switch statement GS.  */

static inline void
gimple_switch_set_num_labels (gimple g, unsigned nlabels)
{
  GIMPLE_CHECK (g, GIMPLE_SWITCH);
  gimple_set_num_ops (g, nlabels + 1);
}


/* Return the index variable used by the switch statement GS.  */

static inline tree
gimple_switch_index (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_SWITCH);
  return gimple_op (gs, 0);
}


/* Return a pointer to the index variable for the switch statement GS.  */

static inline tree *
gimple_switch_index_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_SWITCH);
  return gimple_op_ptr (gs, 0);
}


/* Set INDEX to be the index variable for switch statement GS.  */

static inline void
gimple_switch_set_index (gimple gs, tree index)
{
  GIMPLE_CHECK (gs, GIMPLE_SWITCH);
  gcc_assert (SSA_VAR_P (index) || CONSTANT_CLASS_P (index));
  gimple_set_op (gs, 0, index);
}


/* Return the label numbered INDEX.  The default label is 0, followed by any
   labels in a switch statement.  */

static inline tree
gimple_switch_label (const_gimple gs, unsigned index)
{
  GIMPLE_CHECK (gs, GIMPLE_SWITCH);
  gcc_assert (gimple_num_ops (gs) > index + 1);
  return gimple_op (gs, index + 1);
}

/* Set the label number INDEX to LABEL.  0 is always the default label.  */

static inline void
gimple_switch_set_label (gimple gs, unsigned index, tree label)
{
  GIMPLE_CHECK (gs, GIMPLE_SWITCH);
  gcc_assert (gimple_num_ops (gs) > index + 1);
  gcc_assert (label == NULL_TREE || TREE_CODE (label) == CASE_LABEL_EXPR);
  gimple_set_op (gs, index + 1, label);
}

/* Return the default label for a switch statement.  */

static inline tree
gimple_switch_default_label (const_gimple gs)
{
  return gimple_switch_label (gs, 0);
}

/* Set the default label for a switch statement.  */

static inline void
gimple_switch_set_default_label (gimple gs, tree label)
{
  gimple_switch_set_label (gs, 0, label);
}

/* Return true if GS is a GIMPLE_DEBUG statement.  */

static inline bool
is_gimple_debug (const_gimple gs)
{
  return gimple_code (gs) == GIMPLE_DEBUG;
}

/* Return true if S is a GIMPLE_DEBUG BIND statement.  */

static inline bool
gimple_debug_bind_p (const_gimple s)
{
  if (is_gimple_debug (s))
    return s->gsbase.subcode == GIMPLE_DEBUG_BIND;

  return false;
}

/* Return the variable bound in a GIMPLE_DEBUG bind statement.  */

static inline tree
gimple_debug_bind_get_var (gimple dbg)
{
  GIMPLE_CHECK (dbg, GIMPLE_DEBUG);
#ifdef ENABLE_CHECKING
  gcc_assert (gimple_debug_bind_p (dbg));
#endif
  return gimple_op (dbg, 0);
}

/* Return the value bound to the variable in a GIMPLE_DEBUG bind
   statement.  */

static inline tree
gimple_debug_bind_get_value (gimple dbg)
{
  GIMPLE_CHECK (dbg, GIMPLE_DEBUG);
#ifdef ENABLE_CHECKING
  gcc_assert (gimple_debug_bind_p (dbg));
#endif
  return gimple_op (dbg, 1);
}

/* Return a pointer to the value bound to the variable in a
   GIMPLE_DEBUG bind statement.  */

static inline tree *
gimple_debug_bind_get_value_ptr (gimple dbg)
{
  GIMPLE_CHECK (dbg, GIMPLE_DEBUG);
#ifdef ENABLE_CHECKING
  gcc_assert (gimple_debug_bind_p (dbg));
#endif
  return gimple_op_ptr (dbg, 1);
}

/* Set the variable bound in a GIMPLE_DEBUG bind statement.  */

static inline void
gimple_debug_bind_set_var (gimple dbg, tree var)
{
  GIMPLE_CHECK (dbg, GIMPLE_DEBUG);
#ifdef ENABLE_CHECKING
  gcc_assert (gimple_debug_bind_p (dbg));
#endif
  gimple_set_op (dbg, 0, var);
}

/* Set the value bound to the variable in a GIMPLE_DEBUG bind
   statement.  */

static inline void
gimple_debug_bind_set_value (gimple dbg, tree value)
{
  GIMPLE_CHECK (dbg, GIMPLE_DEBUG);
#ifdef ENABLE_CHECKING
  gcc_assert (gimple_debug_bind_p (dbg));
#endif
  gimple_set_op (dbg, 1, value);
}

/* The second operand of a GIMPLE_DEBUG_BIND, when the value was
   optimized away.  */
#define GIMPLE_DEBUG_BIND_NOVALUE NULL_TREE /* error_mark_node */

/* Remove the value bound to the variable in a GIMPLE_DEBUG bind
   statement.  */

static inline void
gimple_debug_bind_reset_value (gimple dbg)
{
  GIMPLE_CHECK (dbg, GIMPLE_DEBUG);
#ifdef ENABLE_CHECKING
  gcc_assert (gimple_debug_bind_p (dbg));
#endif
  gimple_set_op (dbg, 1, GIMPLE_DEBUG_BIND_NOVALUE);
}

/* Return true if the GIMPLE_DEBUG bind statement is bound to a
   value.  */

static inline bool
gimple_debug_bind_has_value_p (gimple dbg)
{
  GIMPLE_CHECK (dbg, GIMPLE_DEBUG);
#ifdef ENABLE_CHECKING
  gcc_assert (gimple_debug_bind_p (dbg));
#endif
  return gimple_op (dbg, 1) != GIMPLE_DEBUG_BIND_NOVALUE;
}

#undef GIMPLE_DEBUG_BIND_NOVALUE

/* Return the body for the OMP statement GS.  */

static inline gimple_seq
gimple_omp_body (gimple gs)
{
  return gs->omp.body;
}

/* Set BODY to be the body for the OMP statement GS.  */

static inline void
gimple_omp_set_body (gimple gs, gimple_seq body)
{
  gs->omp.body = body;
}


/* Return the name associated with OMP_CRITICAL statement GS.  */

static inline tree
gimple_omp_critical_name (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_CRITICAL);
  return gs->gimple_omp_critical.name;
}


/* Return a pointer to the name associated with OMP critical statement GS.  */

static inline tree *
gimple_omp_critical_name_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_CRITICAL);
  return &gs->gimple_omp_critical.name;
}


/* Set NAME to be the name associated with OMP critical statement GS.  */

static inline void
gimple_omp_critical_set_name (gimple gs, tree name)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_CRITICAL);
  gs->gimple_omp_critical.name = name;
}


/* Return the clauses associated with OMP_FOR GS.  */

static inline tree
gimple_omp_for_clauses (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  return gs->gimple_omp_for.clauses;
}


/* Return a pointer to the OMP_FOR GS.  */

static inline tree *
gimple_omp_for_clauses_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  return &gs->gimple_omp_for.clauses;
}


/* Set CLAUSES to be the list of clauses associated with OMP_FOR GS.  */

static inline void
gimple_omp_for_set_clauses (gimple gs, tree clauses)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gs->gimple_omp_for.clauses = clauses;
}


/* Get the collapse count of OMP_FOR GS.  */

static inline size_t
gimple_omp_for_collapse (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  return gs->gimple_omp_for.collapse;
}


/* Return the index variable for OMP_FOR GS.  */

static inline tree
gimple_omp_for_index (const_gimple gs, size_t i)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  return gs->gimple_omp_for.iter[i].index;
}


/* Return a pointer to the index variable for OMP_FOR GS.  */

static inline tree *
gimple_omp_for_index_ptr (gimple gs, size_t i)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  return &gs->gimple_omp_for.iter[i].index;
}


/* Set INDEX to be the index variable for OMP_FOR GS.  */

static inline void
gimple_omp_for_set_index (gimple gs, size_t i, tree index)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  gs->gimple_omp_for.iter[i].index = index;
}


/* Return the initial value for OMP_FOR GS.  */

static inline tree
gimple_omp_for_initial (const_gimple gs, size_t i)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  return gs->gimple_omp_for.iter[i].initial;
}


/* Return a pointer to the initial value for OMP_FOR GS.  */

static inline tree *
gimple_omp_for_initial_ptr (gimple gs, size_t i)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  return &gs->gimple_omp_for.iter[i].initial;
}


/* Set INITIAL to be the initial value for OMP_FOR GS.  */

static inline void
gimple_omp_for_set_initial (gimple gs, size_t i, tree initial)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  gs->gimple_omp_for.iter[i].initial = initial;
}


/* Return the final value for OMP_FOR GS.  */

static inline tree
gimple_omp_for_final (const_gimple gs, size_t i)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  return gs->gimple_omp_for.iter[i].final;
}


/* Return a pointer to the final value for OMP_FOR GS.  */

static inline tree *
gimple_omp_for_final_ptr (gimple gs, size_t i)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  return &gs->gimple_omp_for.iter[i].final;
}


/* Set FINAL to be the final value for OMP_FOR GS.  */

static inline void
gimple_omp_for_set_final (gimple gs, size_t i, tree final)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  gs->gimple_omp_for.iter[i].final = final;
}


/* Return the increment value for OMP_FOR GS.  */

static inline tree
gimple_omp_for_incr (const_gimple gs, size_t i)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  return gs->gimple_omp_for.iter[i].incr;
}


/* Return a pointer to the increment value for OMP_FOR GS.  */

static inline tree *
gimple_omp_for_incr_ptr (gimple gs, size_t i)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  return &gs->gimple_omp_for.iter[i].incr;
}


/* Set INCR to be the increment value for OMP_FOR GS.  */

static inline void
gimple_omp_for_set_incr (gimple gs, size_t i, tree incr)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  gs->gimple_omp_for.iter[i].incr = incr;
}


/* Return the sequence of statements to execute before the OMP_FOR
   statement GS starts.  */

static inline gimple_seq
gimple_omp_for_pre_body (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  return gs->gimple_omp_for.pre_body;
}


/* Set PRE_BODY to be the sequence of statements to execute before the
   OMP_FOR statement GS starts.  */

static inline void
gimple_omp_for_set_pre_body (gimple gs, gimple_seq pre_body)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gs->gimple_omp_for.pre_body = pre_body;
}


/* Return the clauses associated with OMP_PARALLEL GS.  */

static inline tree
gimple_omp_parallel_clauses (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_PARALLEL);
  return gs->gimple_omp_parallel.clauses;
}


/* Return a pointer to the clauses associated with OMP_PARALLEL GS.  */

static inline tree *
gimple_omp_parallel_clauses_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_PARALLEL);
  return &gs->gimple_omp_parallel.clauses;
}


/* Set CLAUSES to be the list of clauses associated with OMP_PARALLEL
   GS.  */

static inline void
gimple_omp_parallel_set_clauses (gimple gs, tree clauses)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_PARALLEL);
  gs->gimple_omp_parallel.clauses = clauses;
}


/* Return the child function used to hold the body of OMP_PARALLEL GS.  */

static inline tree
gimple_omp_parallel_child_fn (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_PARALLEL);
  return gs->gimple_omp_parallel.child_fn;
}

/* Return a pointer to the child function used to hold the body of
   OMP_PARALLEL GS.  */

static inline tree *
gimple_omp_parallel_child_fn_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_PARALLEL);
  return &gs->gimple_omp_parallel.child_fn;
}


/* Set CHILD_FN to be the child function for OMP_PARALLEL GS.  */

static inline void
gimple_omp_parallel_set_child_fn (gimple gs, tree child_fn)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_PARALLEL);
  gs->gimple_omp_parallel.child_fn = child_fn;
}


/* Return the artificial argument used to send variables and values
   from the parent to the children threads in OMP_PARALLEL GS.  */

static inline tree
gimple_omp_parallel_data_arg (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_PARALLEL);
  return gs->gimple_omp_parallel.data_arg;
}


/* Return a pointer to the data argument for OMP_PARALLEL GS.  */

static inline tree *
gimple_omp_parallel_data_arg_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_PARALLEL);
  return &gs->gimple_omp_parallel.data_arg;
}


/* Set DATA_ARG to be the data argument for OMP_PARALLEL GS.  */

static inline void
gimple_omp_parallel_set_data_arg (gimple gs, tree data_arg)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_PARALLEL);
  gs->gimple_omp_parallel.data_arg = data_arg;
}


/* Return the clauses associated with OMP_TASK GS.  */

static inline tree
gimple_omp_task_clauses (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return gs->gimple_omp_parallel.clauses;
}


/* Return a pointer to the clauses associated with OMP_TASK GS.  */

static inline tree *
gimple_omp_task_clauses_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return &gs->gimple_omp_parallel.clauses;
}


/* Set CLAUSES to be the list of clauses associated with OMP_TASK
   GS.  */

static inline void
gimple_omp_task_set_clauses (gimple gs, tree clauses)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  gs->gimple_omp_parallel.clauses = clauses;
}


/* Return the child function used to hold the body of OMP_TASK GS.  */

static inline tree
gimple_omp_task_child_fn (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return gs->gimple_omp_parallel.child_fn;
}

/* Return a pointer to the child function used to hold the body of
   OMP_TASK GS.  */

static inline tree *
gimple_omp_task_child_fn_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return &gs->gimple_omp_parallel.child_fn;
}


/* Set CHILD_FN to be the child function for OMP_TASK GS.  */

static inline void
gimple_omp_task_set_child_fn (gimple gs, tree child_fn)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  gs->gimple_omp_parallel.child_fn = child_fn;
}


/* Return the artificial argument used to send variables and values
   from the parent to the children threads in OMP_TASK GS.  */

static inline tree
gimple_omp_task_data_arg (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return gs->gimple_omp_parallel.data_arg;
}


/* Return a pointer to the data argument for OMP_TASK GS.  */

static inline tree *
gimple_omp_task_data_arg_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return &gs->gimple_omp_parallel.data_arg;
}


/* Set DATA_ARG to be the data argument for OMP_TASK GS.  */

static inline void
gimple_omp_task_set_data_arg (gimple gs, tree data_arg)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  gs->gimple_omp_parallel.data_arg = data_arg;
}


/* Return the clauses associated with OMP_TASK GS.  */

static inline tree
gimple_omp_taskreg_clauses (const_gimple gs)
{
  if (gimple_code (gs) != GIMPLE_OMP_PARALLEL)
    GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return gs->gimple_omp_parallel.clauses;
}


/* Return a pointer to the clauses associated with OMP_TASK GS.  */

static inline tree *
gimple_omp_taskreg_clauses_ptr (gimple gs)
{
  if (gimple_code (gs) != GIMPLE_OMP_PARALLEL)
    GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return &gs->gimple_omp_parallel.clauses;
}


/* Set CLAUSES to be the list of clauses associated with OMP_TASK
   GS.  */

static inline void
gimple_omp_taskreg_set_clauses (gimple gs, tree clauses)
{
  if (gimple_code (gs) != GIMPLE_OMP_PARALLEL)
    GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  gs->gimple_omp_parallel.clauses = clauses;
}


/* Return the child function used to hold the body of OMP_TASK GS.  */

static inline tree
gimple_omp_taskreg_child_fn (const_gimple gs)
{
  if (gimple_code (gs) != GIMPLE_OMP_PARALLEL)
    GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return gs->gimple_omp_parallel.child_fn;
}

/* Return a pointer to the child function used to hold the body of
   OMP_TASK GS.  */

static inline tree *
gimple_omp_taskreg_child_fn_ptr (gimple gs)
{
  if (gimple_code (gs) != GIMPLE_OMP_PARALLEL)
    GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return &gs->gimple_omp_parallel.child_fn;
}


/* Set CHILD_FN to be the child function for OMP_TASK GS.  */

static inline void
gimple_omp_taskreg_set_child_fn (gimple gs, tree child_fn)
{
  if (gimple_code (gs) != GIMPLE_OMP_PARALLEL)
    GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  gs->gimple_omp_parallel.child_fn = child_fn;
}


/* Return the artificial argument used to send variables and values
   from the parent to the children threads in OMP_TASK GS.  */

static inline tree
gimple_omp_taskreg_data_arg (const_gimple gs)
{
  if (gimple_code (gs) != GIMPLE_OMP_PARALLEL)
    GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return gs->gimple_omp_parallel.data_arg;
}


/* Return a pointer to the data argument for OMP_TASK GS.  */

static inline tree *
gimple_omp_taskreg_data_arg_ptr (gimple gs)
{
  if (gimple_code (gs) != GIMPLE_OMP_PARALLEL)
    GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return &gs->gimple_omp_parallel.data_arg;
}


/* Set DATA_ARG to be the data argument for OMP_TASK GS.  */

static inline void
gimple_omp_taskreg_set_data_arg (gimple gs, tree data_arg)
{
  if (gimple_code (gs) != GIMPLE_OMP_PARALLEL)
    GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  gs->gimple_omp_parallel.data_arg = data_arg;
}


/* Return the copy function used to hold the body of OMP_TASK GS.  */

static inline tree
gimple_omp_task_copy_fn (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return gs->gimple_omp_task.copy_fn;
}

/* Return a pointer to the copy function used to hold the body of
   OMP_TASK GS.  */

static inline tree *
gimple_omp_task_copy_fn_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return &gs->gimple_omp_task.copy_fn;
}


/* Set CHILD_FN to be the copy function for OMP_TASK GS.  */

static inline void
gimple_omp_task_set_copy_fn (gimple gs, tree copy_fn)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  gs->gimple_omp_task.copy_fn = copy_fn;
}


/* Return size of the data block in bytes in OMP_TASK GS.  */

static inline tree
gimple_omp_task_arg_size (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return gs->gimple_omp_task.arg_size;
}


/* Return a pointer to the data block size for OMP_TASK GS.  */

static inline tree *
gimple_omp_task_arg_size_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return &gs->gimple_omp_task.arg_size;
}


/* Set ARG_SIZE to be the data block size for OMP_TASK GS.  */

static inline void
gimple_omp_task_set_arg_size (gimple gs, tree arg_size)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  gs->gimple_omp_task.arg_size = arg_size;
}


/* Return align of the data block in bytes in OMP_TASK GS.  */

static inline tree
gimple_omp_task_arg_align (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return gs->gimple_omp_task.arg_align;
}


/* Return a pointer to the data block align for OMP_TASK GS.  */

static inline tree *
gimple_omp_task_arg_align_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  return &gs->gimple_omp_task.arg_align;
}


/* Set ARG_SIZE to be the data block align for OMP_TASK GS.  */

static inline void
gimple_omp_task_set_arg_align (gimple gs, tree arg_align)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_TASK);
  gs->gimple_omp_task.arg_align = arg_align;
}


/* Return the clauses associated with OMP_SINGLE GS.  */

static inline tree
gimple_omp_single_clauses (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_SINGLE);
  return gs->gimple_omp_single.clauses;
}


/* Return a pointer to the clauses associated with OMP_SINGLE GS.  */

static inline tree *
gimple_omp_single_clauses_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_SINGLE);
  return &gs->gimple_omp_single.clauses;
}


/* Set CLAUSES to be the clauses associated with OMP_SINGLE GS.  */

static inline void
gimple_omp_single_set_clauses (gimple gs, tree clauses)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_SINGLE);
  gs->gimple_omp_single.clauses = clauses;
}


/* Return the clauses associated with OMP_SECTIONS GS.  */

static inline tree
gimple_omp_sections_clauses (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_SECTIONS);
  return gs->gimple_omp_sections.clauses;
}


/* Return a pointer to the clauses associated with OMP_SECTIONS GS.  */

static inline tree *
gimple_omp_sections_clauses_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_SECTIONS);
  return &gs->gimple_omp_sections.clauses;
}


/* Set CLAUSES to be the set of clauses associated with OMP_SECTIONS
   GS.  */

static inline void
gimple_omp_sections_set_clauses (gimple gs, tree clauses)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_SECTIONS);
  gs->gimple_omp_sections.clauses = clauses;
}


/* Return the control variable associated with the GIMPLE_OMP_SECTIONS
   in GS.  */

static inline tree
gimple_omp_sections_control (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_SECTIONS);
  return gs->gimple_omp_sections.control;
}


/* Return a pointer to the clauses associated with the GIMPLE_OMP_SECTIONS
   GS.  */

static inline tree *
gimple_omp_sections_control_ptr (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_SECTIONS);
  return &gs->gimple_omp_sections.control;
}


/* Set CONTROL to be the set of clauses associated with the
   GIMPLE_OMP_SECTIONS in GS.  */

static inline void
gimple_omp_sections_set_control (gimple gs, tree control)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_SECTIONS);
  gs->gimple_omp_sections.control = control;
}


/* Set COND to be the condition code for OMP_FOR GS.  */

static inline void
gimple_omp_for_set_cond (gimple gs, size_t i, enum tree_code cond)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (TREE_CODE_CLASS (cond) == tcc_comparison);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  gs->gimple_omp_for.iter[i].cond = cond;
}


/* Return the condition code associated with OMP_FOR GS.  */

static inline enum tree_code
gimple_omp_for_cond (const_gimple gs, size_t i)
{
  GIMPLE_CHECK (gs, GIMPLE_OMP_FOR);
  gcc_assert (i < gs->gimple_omp_for.collapse);
  return gs->gimple_omp_for.iter[i].cond;
}


/* Set the value being stored in an atomic store.  */

static inline void
gimple_omp_atomic_store_set_val (gimple g, tree val)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_ATOMIC_STORE);
  g->gimple_omp_atomic_store.val = val;
}


/* Return the value being stored in an atomic store.  */

static inline tree
gimple_omp_atomic_store_val (const_gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_ATOMIC_STORE);
  return g->gimple_omp_atomic_store.val;
}


/* Return a pointer to the value being stored in an atomic store.  */

static inline tree *
gimple_omp_atomic_store_val_ptr (gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_ATOMIC_STORE);
  return &g->gimple_omp_atomic_store.val;
}


/* Set the LHS of an atomic load.  */

static inline void
gimple_omp_atomic_load_set_lhs (gimple g, tree lhs)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_ATOMIC_LOAD);
  g->gimple_omp_atomic_load.lhs = lhs;
}


/* Get the LHS of an atomic load.  */

static inline tree
gimple_omp_atomic_load_lhs (const_gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_ATOMIC_LOAD);
  return g->gimple_omp_atomic_load.lhs;
}


/* Return a pointer to the LHS of an atomic load.  */

static inline tree *
gimple_omp_atomic_load_lhs_ptr (gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_ATOMIC_LOAD);
  return &g->gimple_omp_atomic_load.lhs;
}


/* Set the RHS of an atomic load.  */

static inline void
gimple_omp_atomic_load_set_rhs (gimple g, tree rhs)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_ATOMIC_LOAD);
  g->gimple_omp_atomic_load.rhs = rhs;
}


/* Get the RHS of an atomic load.  */

static inline tree
gimple_omp_atomic_load_rhs (const_gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_ATOMIC_LOAD);
  return g->gimple_omp_atomic_load.rhs;
}


/* Return a pointer to the RHS of an atomic load.  */

static inline tree *
gimple_omp_atomic_load_rhs_ptr (gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_ATOMIC_LOAD);
  return &g->gimple_omp_atomic_load.rhs;
}


/* Get the definition of the control variable in a GIMPLE_OMP_CONTINUE.  */

static inline tree
gimple_omp_continue_control_def (const_gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_CONTINUE);
  return g->gimple_omp_continue.control_def;
}

/* The same as above, but return the address.  */

static inline tree *
gimple_omp_continue_control_def_ptr (gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_CONTINUE);
  return &g->gimple_omp_continue.control_def;
}

/* Set the definition of the control variable in a GIMPLE_OMP_CONTINUE.  */

static inline void
gimple_omp_continue_set_control_def (gimple g, tree def)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_CONTINUE);
  g->gimple_omp_continue.control_def = def;
}


/* Get the use of the control variable in a GIMPLE_OMP_CONTINUE.  */

static inline tree
gimple_omp_continue_control_use (const_gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_CONTINUE);
  return g->gimple_omp_continue.control_use;
}


/* The same as above, but return the address.  */

static inline tree *
gimple_omp_continue_control_use_ptr (gimple g)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_CONTINUE);
  return &g->gimple_omp_continue.control_use;
}


/* Set the use of the control variable in a GIMPLE_OMP_CONTINUE.  */

static inline void
gimple_omp_continue_set_control_use (gimple g, tree use)
{
  GIMPLE_CHECK (g, GIMPLE_OMP_CONTINUE);
  g->gimple_omp_continue.control_use = use;
}


/* Return a pointer to the return value for GIMPLE_RETURN GS.  */

static inline tree *
gimple_return_retval_ptr (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_RETURN);
  return gimple_op_ptr (gs, 0);
}

/* Return the return value for GIMPLE_RETURN GS.  */

static inline tree
gimple_return_retval (const_gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_RETURN);
  return gimple_op (gs, 0);
}


/* Set RETVAL to be the return value for GIMPLE_RETURN GS.  */

static inline void
gimple_return_set_retval (gimple gs, tree retval)
{
  GIMPLE_CHECK (gs, GIMPLE_RETURN);
  gimple_set_op (gs, 0, retval);
}


/* Returns true when the gimple statment STMT is any of the OpenMP types.  */

#define CASE_GIMPLE_OMP				\
    case GIMPLE_OMP_PARALLEL:			\
    case GIMPLE_OMP_TASK:			\
    case GIMPLE_OMP_FOR:			\
    case GIMPLE_OMP_SECTIONS:			\
    case GIMPLE_OMP_SECTIONS_SWITCH:		\
    case GIMPLE_OMP_SINGLE:			\
    case GIMPLE_OMP_SECTION:			\
    case GIMPLE_OMP_MASTER:			\
    case GIMPLE_OMP_ORDERED:			\
    case GIMPLE_OMP_CRITICAL:			\
    case GIMPLE_OMP_RETURN:			\
    case GIMPLE_OMP_ATOMIC_LOAD:		\
    case GIMPLE_OMP_ATOMIC_STORE:		\
    case GIMPLE_OMP_CONTINUE

static inline bool
is_gimple_omp (const_gimple stmt)
{
  switch (gimple_code (stmt))
    {
    CASE_GIMPLE_OMP:
      return true;
    default:
      return false;
    }
}


/* Returns TRUE if statement G is a GIMPLE_NOP.  */

static inline bool
gimple_nop_p (const_gimple g)
{
  return gimple_code (g) == GIMPLE_NOP;
}


/* Return true if GS is a GIMPLE_RESX.  */

static inline bool
is_gimple_resx (const_gimple gs)
{
  return gimple_code (gs) == GIMPLE_RESX;
}

/* Return the predictor of GIMPLE_PREDICT statement GS.  */

static inline enum br_predictor
gimple_predict_predictor (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_PREDICT);
  return (enum br_predictor) (gs->gsbase.subcode & ~GF_PREDICT_TAKEN);
}


/* Set the predictor of GIMPLE_PREDICT statement GS to PREDICT.  */

static inline void
gimple_predict_set_predictor (gimple gs, enum br_predictor predictor)
{
  GIMPLE_CHECK (gs, GIMPLE_PREDICT);
  gs->gsbase.subcode = (gs->gsbase.subcode & GF_PREDICT_TAKEN)
		       | (unsigned) predictor;
}


/* Return the outcome of GIMPLE_PREDICT statement GS.  */

static inline enum prediction
gimple_predict_outcome (gimple gs)
{
  GIMPLE_CHECK (gs, GIMPLE_PREDICT);
  return (gs->gsbase.subcode & GF_PREDICT_TAKEN) ? TAKEN : NOT_TAKEN;
}


/* Set the outcome of GIMPLE_PREDICT statement GS to OUTCOME.  */

static inline void
gimple_predict_set_outcome (gimple gs, enum prediction outcome)
{
  GIMPLE_CHECK (gs, GIMPLE_PREDICT);
  if (outcome == TAKEN)
    gs->gsbase.subcode |= GF_PREDICT_TAKEN;
  else
    gs->gsbase.subcode &= ~GF_PREDICT_TAKEN;
}


/* Return the type of the main expression computed by STMT.  Return
   void_type_node if the statement computes nothing.  */

static inline tree
gimple_expr_type (const_gimple stmt)
{
  enum gimple_code code = gimple_code (stmt);

  if (code == GIMPLE_ASSIGN || code == GIMPLE_CALL)
    {
      tree type;
      /* In general we want to pass out a type that can be substituted
         for both the RHS and the LHS types if there is a possibly
	 useless conversion involved.  That means returning the
	 original RHS type as far as we can reconstruct it.  */
      if (code == GIMPLE_CALL)
	type = gimple_call_return_type (stmt);
      else
	switch (gimple_assign_rhs_code (stmt))
	  {
	  case POINTER_PLUS_EXPR:
	    type = TREE_TYPE (gimple_assign_rhs1 (stmt));
	    break;

	  default:
	    /* As fallback use the type of the LHS.  */
	    type = TREE_TYPE (gimple_get_lhs (stmt));
	    break;
	  }
      return type;
    }
  else if (code == GIMPLE_COND)
    return boolean_type_node;
  else
    return void_type_node;
}


/* Return a new iterator pointing to GIMPLE_SEQ's first statement.  */

static inline gimple_stmt_iterator
gsi_start (gimple_seq seq)
{
  gimple_stmt_iterator i;

  i.ptr = gimple_seq_first (seq);
  i.seq = seq;
  i.bb = (i.ptr && i.ptr->stmt) ? gimple_bb (i.ptr->stmt) : NULL;

  return i;
}


/* Return a new iterator pointing to the first statement in basic block BB.  */

static inline gimple_stmt_iterator
gsi_start_bb (basic_block bb)
{
  gimple_stmt_iterator i;
  gimple_seq seq;

  seq = bb_seq (bb);
  i.ptr = gimple_seq_first (seq);
  i.seq = seq;
  i.bb = bb;

  return i;
}


/* Return a new iterator initially pointing to GIMPLE_SEQ's last statement.  */

static inline gimple_stmt_iterator
gsi_last (gimple_seq seq)
{
  gimple_stmt_iterator i;

  i.ptr = gimple_seq_last (seq);
  i.seq = seq;
  i.bb = (i.ptr && i.ptr->stmt) ? gimple_bb (i.ptr->stmt) : NULL;

  return i;
}


/* Return a new iterator pointing to the last statement in basic block BB.  */

static inline gimple_stmt_iterator
gsi_last_bb (basic_block bb)
{
  gimple_stmt_iterator i;
  gimple_seq seq;

  seq = bb_seq (bb);
  i.ptr = gimple_seq_last (seq);
  i.seq = seq;
  i.bb = bb;

  return i;
}


/* Return true if I is at the end of its sequence.  */

static inline bool
gsi_end_p (gimple_stmt_iterator i)
{
  return i.ptr == NULL;
}


/* Return true if I is one statement before the end of its sequence.  */

static inline bool
gsi_one_before_end_p (gimple_stmt_iterator i)
{
  return i.ptr != NULL && i.ptr->next == NULL;
}


/* Advance the iterator to the next gimple statement.  */

static inline void
gsi_next (gimple_stmt_iterator *i)
{
  i->ptr = i->ptr->next;
}

/* Advance the iterator to the previous gimple statement.  */

static inline void
gsi_prev (gimple_stmt_iterator *i)
{
  i->ptr = i->ptr->prev;
}

/* Return the current stmt.  */

static inline gimple
gsi_stmt (gimple_stmt_iterator i)
{
  return i.ptr->stmt;
}

/* Return a block statement iterator that points to the first non-label
   statement in block BB.  */

static inline gimple_stmt_iterator
gsi_after_labels (basic_block bb)
{
  gimple_stmt_iterator gsi = gsi_start_bb (bb);

  while (!gsi_end_p (gsi) && gimple_code (gsi_stmt (gsi)) == GIMPLE_LABEL)
    gsi_next (&gsi);

  return gsi;
}

/* Advance the iterator to the next non-debug gimple statement.  */

static inline void
gsi_next_nondebug (gimple_stmt_iterator *i)
{
  do
    {
      gsi_next (i);
    }
  while (!gsi_end_p (*i) && is_gimple_debug (gsi_stmt (*i)));
}

/* Advance the iterator to the next non-debug gimple statement.  */

static inline void
gsi_prev_nondebug (gimple_stmt_iterator *i)
{
  do
    {
      gsi_prev (i);
    }
  while (!gsi_end_p (*i) && is_gimple_debug (gsi_stmt (*i)));
}

/* Return a new iterator pointing to the first non-debug statement in
   basic block BB.  */

static inline gimple_stmt_iterator
gsi_start_nondebug_bb (basic_block bb)
{
  gimple_stmt_iterator i = gsi_start_bb (bb);

  if (!gsi_end_p (i) && is_gimple_debug (gsi_stmt (i)))
    gsi_next_nondebug (&i);

  return i;
}

/* Return a new iterator pointing to the last non-debug statement in
   basic block BB.  */

static inline gimple_stmt_iterator
gsi_last_nondebug_bb (basic_block bb)
{
  gimple_stmt_iterator i = gsi_last_bb (bb);

  if (!gsi_end_p (i) && is_gimple_debug (gsi_stmt (i)))
    gsi_prev_nondebug (&i);

  return i;
}

/* Return a pointer to the current stmt.

  NOTE: You may want to use gsi_replace on the iterator itself,
  as this performs additional bookkeeping that will not be done
  if you simply assign through a pointer returned by gsi_stmt_ptr.  */

static inline gimple *
gsi_stmt_ptr (gimple_stmt_iterator *i)
{
  return &i->ptr->stmt;
}


/* Return the basic block associated with this iterator.  */

static inline basic_block
gsi_bb (gimple_stmt_iterator i)
{
  return i.bb;
}


/* Return the sequence associated with this iterator.  */

static inline gimple_seq
gsi_seq (gimple_stmt_iterator i)
{
  return i.seq;
}


enum gsi_iterator_update
{
  GSI_NEW_STMT,		/* Only valid when single statement is added, move
			   iterator to it.  */
  GSI_SAME_STMT,	/* Leave the iterator at the same statement.  */
  GSI_CONTINUE_LINKING	/* Move iterator to whatever position is suitable
			   for linking other statements in the same
			   direction.  */
};

/* In gimple-iterator.c  */
gimple_stmt_iterator gsi_start_phis (basic_block);
gimple_seq gsi_split_seq_after (gimple_stmt_iterator);
gimple_seq gsi_split_seq_before (gimple_stmt_iterator *);
void gsi_replace (gimple_stmt_iterator *, gimple, bool);
void gsi_insert_before (gimple_stmt_iterator *, gimple,
			enum gsi_iterator_update);
void gsi_insert_before_without_update (gimple_stmt_iterator *, gimple,
                                       enum gsi_iterator_update);
void gsi_insert_seq_before (gimple_stmt_iterator *, gimple_seq,
                            enum gsi_iterator_update);
void gsi_insert_seq_before_without_update (gimple_stmt_iterator *, gimple_seq,
                                           enum gsi_iterator_update);
void gsi_insert_after (gimple_stmt_iterator *, gimple,
		       enum gsi_iterator_update);
void gsi_insert_after_without_update (gimple_stmt_iterator *, gimple,
                                      enum gsi_iterator_update);
void gsi_insert_seq_after (gimple_stmt_iterator *, gimple_seq,
			   enum gsi_iterator_update);
void gsi_insert_seq_after_without_update (gimple_stmt_iterator *, gimple_seq,
                                          enum gsi_iterator_update);
void gsi_remove (gimple_stmt_iterator *, bool);
gimple_stmt_iterator gsi_for_stmt (gimple);
void gsi_move_after (gimple_stmt_iterator *, gimple_stmt_iterator *);
void gsi_move_before (gimple_stmt_iterator *, gimple_stmt_iterator *);
void gsi_move_to_bb_end (gimple_stmt_iterator *, struct basic_block_def *);
void gsi_insert_on_edge (edge, gimple);
void gsi_insert_seq_on_edge (edge, gimple_seq);
basic_block gsi_insert_on_edge_immediate (edge, gimple);
basic_block gsi_insert_seq_on_edge_immediate (edge, gimple_seq);
void gsi_commit_one_edge_insert (edge, basic_block *);
void gsi_commit_edge_inserts (void);
gimple gimple_call_copy_skip_args (gimple, bitmap);


/* Convenience routines to walk all statements of a gimple function.
   Note that this is useful exclusively before the code is converted
   into SSA form.  Once the program is in SSA form, the standard
   operand interface should be used to analyze/modify statements.  */
struct walk_stmt_info
{
  /* Points to the current statement being walked.  */
  gimple_stmt_iterator gsi;

  /* Additional data that the callback functions may want to carry
     through the recursion.  */
  void *info;

  /* Pointer map used to mark visited tree nodes when calling
     walk_tree on each operand.  If set to NULL, duplicate tree nodes
     will be visited more than once.  */
  struct pointer_set_t *pset;

  /* Indicates whether the operand being examined may be replaced
     with something that matches is_gimple_val (if true) or something
     slightly more complicated (if false).  "Something" technically
     means the common subset of is_gimple_lvalue and is_gimple_rhs,
     but we never try to form anything more complicated than that, so
     we don't bother checking.

     Also note that CALLBACK should update this flag while walking the
     sub-expressions of a statement.  For instance, when walking the
     statement 'foo (&var)', the flag VAL_ONLY will initially be set
     to true, however, when walking &var, the operand of that
     ADDR_EXPR does not need to be a GIMPLE value.  */
  bool val_only;

  /* True if we are currently walking the LHS of an assignment.  */
  bool is_lhs;

  /* Optional.  Set to true by the callback functions if they made any
     changes.  */
  bool changed;

  /* True if we're interested in location information.  */
  bool want_locations;

  /* Operand returned by the callbacks.  This is set when calling
     walk_gimple_seq.  If the walk_stmt_fn or walk_tree_fn callback
     returns non-NULL, this field will contain the tree returned by
     the last callback.  */
  tree callback_result;
};

/* Callback for walk_gimple_stmt.  Called for every statement found
   during traversal.  The first argument points to the statement to
   walk.  The second argument is a flag that the callback sets to
   'true' if it the callback handled all the operands and
   sub-statements of the statement (the default value of this flag is
   'false').  The third argument is an anonymous pointer to data
   to be used by the callback.  */
typedef tree (*walk_stmt_fn) (gimple_stmt_iterator *, bool *,
			      struct walk_stmt_info *);

gimple walk_gimple_seq (gimple_seq, walk_stmt_fn, walk_tree_fn,
		        struct walk_stmt_info *);
tree walk_gimple_stmt (gimple_stmt_iterator *, walk_stmt_fn, walk_tree_fn,
		       struct walk_stmt_info *);
tree walk_gimple_op (gimple, walk_tree_fn, struct walk_stmt_info *);

#ifdef GATHER_STATISTICS
/* Enum and arrays used for allocation stats.  Keep in sync with
   gimple.c:gimple_alloc_kind_names.  */
enum gimple_alloc_kind
{
  gimple_alloc_kind_assign,	/* Assignments.  */
  gimple_alloc_kind_phi,	/* PHI nodes.  */
  gimple_alloc_kind_cond,	/* Conditionals.  */
  gimple_alloc_kind_seq,	/* Sequences.  */
  gimple_alloc_kind_rest,	/* Everything else.  */
  gimple_alloc_kind_all
};

extern int gimple_alloc_counts[];
extern int gimple_alloc_sizes[];

/* Return the allocation kind for a given stmt CODE.  */
static inline enum gimple_alloc_kind
gimple_alloc_kind (enum gimple_code code)
{
  switch (code)
    {
      case GIMPLE_ASSIGN:
	return gimple_alloc_kind_assign;
      case GIMPLE_PHI:
	return gimple_alloc_kind_phi;
      case GIMPLE_COND:
	return gimple_alloc_kind_cond;
      default:
	return gimple_alloc_kind_rest;
    }
}
#endif /* GATHER_STATISTICS */

extern void dump_gimple_statistics (void);

extern tree maybe_fold_and_comparisons (enum tree_code, tree, tree, 
					enum tree_code, tree, tree);
extern tree maybe_fold_or_comparisons (enum tree_code, tree, tree,
				       enum tree_code, tree, tree);

#endif  /* GCC_GIMPLE_H */
