/* Garbage collection for the GNU compiler.

   Copyright (C) 1998-2013 Free Software Foundation, Inc.

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

#ifndef GCC_GGC_H
#define GCC_GGC_H
#include "statistics.h"

/* Symbols are marked with `ggc' for `gcc gc' so as not to interfere with
   an external gc library that might be linked in.  */

/* Constants for general use.  */
extern const char empty_string[];	/* empty string */

/* Internal functions and data structures used by the GTY
   machinery, including the generated gt*.[hc] files.  */

#include "gtype-desc.h"

/* One of these applies its third parameter (with cookie in the fourth
   parameter) to each pointer in the object pointed to by the first
   parameter, using the second parameter.  */
typedef void (*gt_note_pointers) (void *, void *, gt_pointer_operator,
				  void *);

/* One of these is called before objects are re-ordered in memory.
   The first parameter is the original object, the second is the
   subobject that has had its pointers reordered, the third parameter
   can compute the new values of a pointer when given the cookie in
   the fourth parameter.  */
typedef void (*gt_handle_reorder) (void *, void *, gt_pointer_operator,
				   void *);

/* Used by the gt_pch_n_* routines.  Register an object in the hash table.  */
extern int gt_pch_note_object (void *, void *, gt_note_pointers);

/* Used by the gt_pch_n_* routines.  Register that an object has a reorder
   function.  */
extern void gt_pch_note_reorder (void *, void *, gt_handle_reorder);

/* Mark the object in the first parameter and anything it points to.  */
typedef void (*gt_pointer_walker) (void *);

/* Structures for the easy way to mark roots.
   In an array, terminated by having base == NULL.  */
struct ggc_root_tab {
  void *base;
  size_t nelt;
  size_t stride;
  gt_pointer_walker cb;
  gt_pointer_walker pchw;
};
#define LAST_GGC_ROOT_TAB { NULL, 0, 0, NULL, NULL }
/* Pointers to arrays of ggc_root_tab, terminated by NULL.  */
extern const struct ggc_root_tab * const gt_ggc_rtab[];
extern const struct ggc_root_tab * const gt_ggc_deletable_rtab[];
extern const struct ggc_root_tab * const gt_pch_cache_rtab[];
extern const struct ggc_root_tab * const gt_pch_scalar_rtab[];

/* Structure for hash table cache marking.  */
struct htab;
struct ggc_cache_tab {
  struct htab * *base;
  size_t nelt;
  size_t stride;
  gt_pointer_walker cb;
  gt_pointer_walker pchw;
  int (*marked_p) (const void *);
};
#define LAST_GGC_CACHE_TAB { NULL, 0, 0, NULL, NULL, NULL }
/* Pointers to arrays of ggc_cache_tab, terminated by NULL.  */
extern const struct ggc_cache_tab * const gt_ggc_cache_rtab[];

/* If EXPR is not NULL and previously unmarked, mark it and evaluate
   to true.  Otherwise evaluate to false.  */
#define ggc_test_and_set_mark(EXPR) \
  ((EXPR) != NULL && ((void *) (EXPR)) != (void *) 1 && ! ggc_set_mark (EXPR))

#define ggc_mark(EXPR)				\
  do {						\
    const void *const a__ = (EXPR);		\
    if (a__ != NULL && a__ != (void *) 1)	\
      ggc_set_mark (a__);			\
  } while (0)

/* Actually set the mark on a particular region of memory, but don't
   follow pointers.  This function is called by ggc_mark_*.  It
   returns zero if the object was not previously marked; nonzero if
   the object was already marked, or if, for any other reason,
   pointers in this data structure should not be traversed.  */
extern int ggc_set_mark	(const void *);

/* Return 1 if P has been marked, zero otherwise.
   P must have been allocated by the GC allocator; it mustn't point to
   static objects, stack variables, or memory allocated with malloc.  */
extern int ggc_marked_p	(const void *);

/* PCH and GGC handling for strings, mostly trivial.  */
extern void gt_pch_n_S (const void *);
extern void gt_ggc_m_S (const void *);

/* End of GTY machinery API.  */

/* Initialize the string pool.  */
extern void init_stringpool (void);

/* Initialize the garbage collector.  */
extern void init_ggc (void);

/* When true, identifier nodes are considered as GC roots.  When
   false, identifier nodes are treated like any other GC-allocated
   object, and the identifier hash table is treated as a weak
   hash.  */
extern bool ggc_protect_identifiers;

/* Write out all GCed objects to F.  */
extern void gt_pch_save (FILE *f);


/* Allocation.  */

/* The internal primitive.  */
extern void *ggc_internal_alloc_stat (size_t MEM_STAT_DECL)
  ATTRIBUTE_MALLOC;

extern size_t ggc_round_alloc_size (size_t requested_size);

#define ggc_internal_alloc(s) ggc_internal_alloc_stat (s MEM_STAT_INFO)

/* Allocates cleared memory.  */
extern void *ggc_internal_cleared_alloc_stat (size_t MEM_STAT_DECL)
  ATTRIBUTE_MALLOC;

/* Resize a block.  */
extern void *ggc_realloc_stat (void *, size_t MEM_STAT_DECL);

/* Free a block.  To be used when known for certain it's not reachable.  */
extern void ggc_free (void *);

extern void dump_ggc_loc_statistics (bool);

/* Reallocators.  */
#define GGC_RESIZEVEC(T, P, N) \
    ((T *) ggc_realloc_stat ((P), (N) * sizeof (T) MEM_STAT_INFO))

#define GGC_RESIZEVAR(T, P, N)                          \
    ((T *) ggc_realloc_stat ((P), (N) MEM_STAT_INFO))

static inline void *
ggc_internal_vec_alloc_stat (size_t s, size_t c MEM_STAT_DECL)
{
    return ggc_internal_alloc_stat (c * s PASS_MEM_STAT);
}

static inline void *
ggc_internal_cleared_vec_alloc_stat (size_t s, size_t c MEM_STAT_DECL)
{
    return ggc_internal_cleared_alloc_stat (c * s PASS_MEM_STAT);
}

#define ggc_internal_cleared_vec_alloc(s, c) \
    (ggc_internal_cleared_vec_alloc_stat ((s), (c) MEM_STAT_INFO))

static inline void *
ggc_alloc_atomic_stat (size_t s MEM_STAT_DECL)
{
    return ggc_internal_alloc_stat (s PASS_MEM_STAT);
}

#define ggc_alloc_atomic(S)  (ggc_alloc_atomic_stat ((S) MEM_STAT_INFO))

#define ggc_alloc_cleared_atomic(S)             \
    (ggc_internal_cleared_alloc_stat ((S) MEM_STAT_INFO))

extern void *ggc_cleared_alloc_htab_ignore_args (size_t, size_t)
  ATTRIBUTE_MALLOC;

extern void *ggc_cleared_alloc_ptr_array_two_args (size_t, size_t)
  ATTRIBUTE_MALLOC;

#define htab_create_ggc(SIZE, HASH, EQ, DEL) \
  htab_create_typed_alloc (SIZE, HASH, EQ, DEL,	\
			   ggc_cleared_alloc_htab_ignore_args,		\
			   ggc_cleared_alloc_ptr_array_two_args,	\
			   ggc_free)

#define splay_tree_new_ggc(COMPARE, ALLOC_TREE, ALLOC_NODE)		     \
  splay_tree_new_typed_alloc (COMPARE, NULL, NULL, &ALLOC_TREE, &ALLOC_NODE, \
			      &ggc_splay_dont_free, NULL)

extern void *ggc_splay_alloc (int, void *)
  ATTRIBUTE_MALLOC;

extern void ggc_splay_dont_free (void *, void *);

/* Allocate a gc-able string, and fill it with LENGTH bytes from CONTENTS.
   If LENGTH is -1, then CONTENTS is assumed to be a
   null-terminated string and the memory sized accordingly.  */
extern const char *ggc_alloc_string_stat (const char *contents, int length
                                          MEM_STAT_DECL);

#define ggc_alloc_string(c, l) ggc_alloc_string_stat (c, l MEM_STAT_INFO)

/* Make a copy of S, in GC-able memory.  */
#define ggc_strdup(S) ggc_alloc_string_stat ((S), -1 MEM_STAT_INFO)

/* Invoke the collector.  Garbage collection occurs only when this
   function is called, not during allocations.  */
extern void ggc_collect	(void);

/* Register an additional root table.  This can be useful for some
   plugins.  Does nothing if the passed pointer is NULL. */
extern void ggc_register_root_tab (const struct ggc_root_tab *);

/* Register an additional cache table.  This can be useful for some
   plugins.  Does nothing if the passed pointer is NULL. */
extern void ggc_register_cache_tab (const struct ggc_cache_tab *);

/* Read objects previously saved with gt_pch_save from F.  */
extern void gt_pch_restore (FILE *f);

/* Statistics.  */

/* Print allocation statistics.  */
extern void ggc_print_statistics (void);

extern void stringpool_statistics (void);

/* Heuristics.  */
extern void init_ggc_heuristics (void);

#define ggc_alloc_rtvec_sized(NELT)				\
  ggc_alloc_rtvec_def (sizeof (struct rtvec_def)		\
		       + ((NELT) - 1) * sizeof (rtx))		\

/* Memory statistics passing versions of some allocators.  Too few of them to
   make gengtype produce them, so just define the needed ones here.  */
static inline struct rtx_def *
ggc_alloc_rtx_def_stat (size_t s MEM_STAT_DECL)
{
  return (struct rtx_def *) ggc_internal_alloc_stat (s PASS_MEM_STAT);
}

static inline union tree_node *
ggc_alloc_tree_node_stat (size_t s MEM_STAT_DECL)
{
  return (union tree_node *) ggc_internal_alloc_stat (s PASS_MEM_STAT);
}

static inline union tree_node *
ggc_alloc_cleared_tree_node_stat (size_t s MEM_STAT_DECL)
{
  return (union tree_node *) ggc_internal_cleared_alloc_stat (s PASS_MEM_STAT);
}

static inline union gimple_statement_d *
ggc_alloc_cleared_gimple_statement_d_stat (size_t s MEM_STAT_DECL)
{
  return (union gimple_statement_d *)
    ggc_internal_cleared_alloc_stat (s PASS_MEM_STAT);
}

#endif
