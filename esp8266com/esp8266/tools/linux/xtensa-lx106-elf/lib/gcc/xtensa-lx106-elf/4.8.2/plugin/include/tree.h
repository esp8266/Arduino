/* Definitions for the ubiquitous 'tree' type for GNU compilers.
   Copyright (C) 1989-2013 Free Software Foundation, Inc.

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

#ifndef GCC_TREE_H
#define GCC_TREE_H

#include "hashtab.h"
#include "machmode.h"
#include "input.h"
#include "statistics.h"
#include "vec.h"
#include "double-int.h"
#include "real.h"
#include "fixed-value.h"
#include "alias.h"
#include "flags.h"

/* Codes of tree nodes */

#define DEFTREECODE(SYM, STRING, TYPE, NARGS)   SYM,
#define END_OF_BASE_TREE_CODES LAST_AND_UNUSED_TREE_CODE,

enum tree_code {
#include "all-tree.def"
MAX_TREE_CODES
};

#undef DEFTREECODE
#undef END_OF_BASE_TREE_CODES

extern unsigned char tree_contains_struct[MAX_TREE_CODES][64];
#define CODE_CONTAINS_STRUCT(CODE, STRUCT) (tree_contains_struct[(CODE)][(STRUCT)])

/* Macros for initializing `tree_contains_struct'.  */
#define MARK_TS_BASE(C)					\
  do {							\
    tree_contains_struct[C][TS_BASE] = 1;		\
  } while (0)

#define MARK_TS_TYPED(C)				\
  do {							\
    MARK_TS_BASE (C);					\
    tree_contains_struct[C][TS_TYPED] = 1;		\
  } while (0)

#define MARK_TS_COMMON(C)				\
  do {							\
    MARK_TS_TYPED (C);					\
    tree_contains_struct[C][TS_COMMON] = 1;		\
  } while (0)

#define MARK_TS_TYPE_COMMON(C)				\
  do {							\
    MARK_TS_COMMON (C);					\
    tree_contains_struct[C][TS_TYPE_COMMON] = 1;	\
  } while (0)

#define MARK_TS_TYPE_WITH_LANG_SPECIFIC(C)		\
  do {							\
    MARK_TS_TYPE_COMMON (C);				\
    tree_contains_struct[C][TS_TYPE_WITH_LANG_SPECIFIC] = 1;	\
  } while (0)

#define MARK_TS_DECL_MINIMAL(C)				\
  do {							\
    MARK_TS_COMMON (C);					\
    tree_contains_struct[C][TS_DECL_MINIMAL] = 1;	\
  } while (0)

#define MARK_TS_DECL_COMMON(C)				\
  do {							\
    MARK_TS_DECL_MINIMAL (C);				\
    tree_contains_struct[C][TS_DECL_COMMON] = 1;	\
  } while (0)

#define MARK_TS_DECL_WRTL(C)				\
  do {							\
    MARK_TS_DECL_COMMON (C);				\
    tree_contains_struct[C][TS_DECL_WRTL] = 1;		\
  } while (0)

#define MARK_TS_DECL_WITH_VIS(C)			\
  do {							\
    MARK_TS_DECL_WRTL (C);				\
    tree_contains_struct[C][TS_DECL_WITH_VIS] = 1;	\
  } while (0)

#define MARK_TS_DECL_NON_COMMON(C)			\
  do {							\
    MARK_TS_DECL_WITH_VIS (C);				\
    tree_contains_struct[C][TS_DECL_NON_COMMON] = 1;	\
  } while (0)

/* Number of language-independent tree codes.  */
#define NUM_TREE_CODES ((int) LAST_AND_UNUSED_TREE_CODE)

/* Tree code classes.  */

/* Each tree_code has an associated code class represented by a
   TREE_CODE_CLASS.  */

enum tree_code_class {
  tcc_exceptional, /* An exceptional code (fits no category).  */
  tcc_constant,    /* A constant.  */
  /* Order of tcc_type and tcc_declaration is important.  */
  tcc_type,        /* A type object code.  */
  tcc_declaration, /* A declaration (also serving as variable refs).  */
  tcc_reference,   /* A reference to storage.  */
  tcc_comparison,  /* A comparison expression.  */
  tcc_unary,       /* A unary arithmetic expression.  */
  tcc_binary,      /* A binary arithmetic expression.  */
  tcc_statement,   /* A statement expression, which have side effects
		      but usually no interesting value.  */
  tcc_vl_exp,      /* A function call or other expression with a
		      variable-length operand vector.  */
  tcc_expression   /* Any other expression.  */
};

/* Each tree code class has an associated string representation.
   These must correspond to the tree_code_class entries.  */

extern const char *const tree_code_class_strings[];

/* Returns the string representing CLASS.  */

#define TREE_CODE_CLASS_STRING(CLASS)\
        tree_code_class_strings[(int) (CLASS)]

extern const enum tree_code_class tree_code_type[];
#define TREE_CODE_CLASS(CODE)	tree_code_type[(int) (CODE)]

/* Nonzero if CODE represents an exceptional code.  */

#define EXCEPTIONAL_CLASS_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_exceptional)

/* Nonzero if CODE represents a constant.  */

#define CONSTANT_CLASS_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_constant)

/* Nonzero if CODE represents a type.  */

#define TYPE_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_type)

/* Nonzero if CODE represents a declaration.  */

#define DECL_P(CODE)\
        (TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_declaration)

/* Nonzero if DECL represents a VAR_DECL or FUNCTION_DECL.  */

#define VAR_OR_FUNCTION_DECL_P(DECL)\
  (TREE_CODE (DECL) == VAR_DECL || TREE_CODE (DECL) == FUNCTION_DECL)

/* Nonzero if CODE represents a INDIRECT_REF.  Keep these checks in
   ascending code order.  */

#define INDIRECT_REF_P(CODE)\
  (TREE_CODE (CODE) == INDIRECT_REF)

/* Nonzero if CODE represents a reference.  */

#define REFERENCE_CLASS_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_reference)

/* Nonzero if CODE represents a comparison.  */

#define COMPARISON_CLASS_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_comparison)

/* Nonzero if CODE represents a unary arithmetic expression.  */

#define UNARY_CLASS_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_unary)

/* Nonzero if CODE represents a binary arithmetic expression.  */

#define BINARY_CLASS_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_binary)

/* Nonzero if CODE represents a statement expression.  */

#define STATEMENT_CLASS_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_statement)

/* Nonzero if CODE represents a function call-like expression with a
   variable-length operand vector.  */

#define VL_EXP_CLASS_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_vl_exp)

/* Nonzero if CODE represents any other expression.  */

#define EXPRESSION_CLASS_P(CODE)\
	(TREE_CODE_CLASS (TREE_CODE (CODE)) == tcc_expression)

/* Returns nonzero iff CODE represents a type or declaration.  */

#define IS_TYPE_OR_DECL_P(CODE)\
	(TYPE_P (CODE) || DECL_P (CODE))

/* Returns nonzero iff CLASS is the tree-code class of an
   expression.  */

#define IS_EXPR_CODE_CLASS(CLASS)\
	((CLASS) >= tcc_reference && (CLASS) <= tcc_expression)

/* Returns nonzero iff NODE is an expression of some kind.  */

#define EXPR_P(NODE) IS_EXPR_CODE_CLASS (TREE_CODE_CLASS (TREE_CODE (NODE)))

/* Number of argument-words in each kind of tree-node.  */

extern const unsigned char tree_code_length[];
#define TREE_CODE_LENGTH(CODE)	tree_code_length[(int) (CODE)]

/* Names of tree components.  */

extern const char *const tree_code_name[];

/* When procesing aliases on symtab level, we need the declaration of target.
   For this reason we need to queue aliases and process them after all declarations
   has been produced.  */

typedef struct GTY(()) alias_pair
{
  tree decl;
  tree target;  
} alias_pair;

/* Define gc'd vector type.  */

extern GTY(()) vec<alias_pair, va_gc> *alias_pairs;


/* Classify which part of the compiler has defined a given builtin function.
   Note that we assume below that this is no more than two bits.  */
enum built_in_class
{
  NOT_BUILT_IN = 0,
  BUILT_IN_FRONTEND,
  BUILT_IN_MD,
  BUILT_IN_NORMAL
};

/* Last marker used for LTO stremaing of built_in_class.  We can not add it
   to the enum since we need the enumb to fit in 2 bits.  */
#define BUILT_IN_LAST (BUILT_IN_NORMAL + 1)

/* Names for the above.  */
extern const char *const built_in_class_names[4];

/* Codes that identify the various built in functions
   so that expand_call can identify them quickly.  */

#define DEF_BUILTIN(ENUM, N, C, T, LT, B, F, NA, AT, IM, COND) ENUM,
enum built_in_function
{
#include "builtins.def"

  /* Complex division routines in libgcc.  These are done via builtins
     because emit_library_call_value can't handle complex values.  */
  BUILT_IN_COMPLEX_MUL_MIN,
  BUILT_IN_COMPLEX_MUL_MAX
    = BUILT_IN_COMPLEX_MUL_MIN
      + MAX_MODE_COMPLEX_FLOAT
      - MIN_MODE_COMPLEX_FLOAT,

  BUILT_IN_COMPLEX_DIV_MIN,
  BUILT_IN_COMPLEX_DIV_MAX
    = BUILT_IN_COMPLEX_DIV_MIN
      + MAX_MODE_COMPLEX_FLOAT
      - MIN_MODE_COMPLEX_FLOAT,

  /* Upper bound on non-language-specific builtins.  */
  END_BUILTINS
};
#undef DEF_BUILTIN

/* Names for the above.  */
extern const char * built_in_names[(int) END_BUILTINS];

/* Helper macros for math builtins.  */

#define BUILTIN_EXP10_P(FN) \
 ((FN) == BUILT_IN_EXP10 || (FN) == BUILT_IN_EXP10F || (FN) == BUILT_IN_EXP10L \
  || (FN) == BUILT_IN_POW10 || (FN) == BUILT_IN_POW10F || (FN) == BUILT_IN_POW10L)

#define BUILTIN_EXPONENT_P(FN) (BUILTIN_EXP10_P (FN) \
  || (FN) == BUILT_IN_EXP || (FN) == BUILT_IN_EXPF || (FN) == BUILT_IN_EXPL \
  || (FN) == BUILT_IN_EXP2 || (FN) == BUILT_IN_EXP2F || (FN) == BUILT_IN_EXP2L)

#define BUILTIN_SQRT_P(FN) \
 ((FN) == BUILT_IN_SQRT || (FN) == BUILT_IN_SQRTF || (FN) == BUILT_IN_SQRTL)

#define BUILTIN_CBRT_P(FN) \
 ((FN) == BUILT_IN_CBRT || (FN) == BUILT_IN_CBRTF || (FN) == BUILT_IN_CBRTL)

#define BUILTIN_ROOT_P(FN) (BUILTIN_SQRT_P (FN) || BUILTIN_CBRT_P (FN))

#define CASE_FLT_FN(FN) case FN: case FN##F: case FN##L
#define CASE_FLT_FN_REENT(FN) case FN##_R: case FN##F_R: case FN##L_R
#define CASE_INT_FN(FN) case FN: case FN##L: case FN##LL

/* In an OMP_CLAUSE node.  */

/* Number of operands and names for each clause.  */
extern unsigned const char omp_clause_num_ops[];
extern const char * const omp_clause_code_name[];

/* Clause codes.  Do not reorder, as this is used to index into the tables
   omp_clause_num_ops and omp_clause_code_name.  */
enum omp_clause_code
{
  /* Clause zero is special-cased inside the parser
     (c_parser_omp_variable_list).  */
  OMP_CLAUSE_ERROR = 0,

  /* OpenMP clause: private (variable_list).  */
  OMP_CLAUSE_PRIVATE,

  /* OpenMP clause: shared (variable_list).  */
  OMP_CLAUSE_SHARED,

  /* OpenMP clause: firstprivate (variable_list).  */
  OMP_CLAUSE_FIRSTPRIVATE,

  /* OpenMP clause: lastprivate (variable_list).  */
  OMP_CLAUSE_LASTPRIVATE,

  /* OpenMP clause: reduction (operator:variable_list).
     OMP_CLAUSE_REDUCTION_CODE: The tree_code of the operator.
     Operand 1: OMP_CLAUSE_REDUCTION_INIT: Stmt-list to initialize the var.
     Operand 2: OMP_CLAUSE_REDUCTION_MERGE: Stmt-list to merge private var
                into the shared one.
     Operand 3: OMP_CLAUSE_REDUCTION_PLACEHOLDER: A dummy VAR_DECL
                placeholder used in OMP_CLAUSE_REDUCTION_{INIT,MERGE}.  */
  OMP_CLAUSE_REDUCTION,

  /* OpenMP clause: copyin (variable_list).  */
  OMP_CLAUSE_COPYIN,

  /* OpenMP clause: copyprivate (variable_list).  */
  OMP_CLAUSE_COPYPRIVATE,

  /* OpenMP clause: if (scalar-expression).  */
  OMP_CLAUSE_IF,

  /* OpenMP clause: num_threads (integer-expression).  */
  OMP_CLAUSE_NUM_THREADS,

  /* OpenMP clause: schedule.  */
  OMP_CLAUSE_SCHEDULE,

  /* OpenMP clause: nowait.  */
  OMP_CLAUSE_NOWAIT,

  /* OpenMP clause: ordered.  */
  OMP_CLAUSE_ORDERED,

  /* OpenMP clause: default.  */
  OMP_CLAUSE_DEFAULT,

  /* OpenMP clause: collapse (constant-integer-expression).  */
  OMP_CLAUSE_COLLAPSE,

  /* OpenMP clause: untied.  */
  OMP_CLAUSE_UNTIED,

  /* OpenMP clause: final (scalar-expression).  */
  OMP_CLAUSE_FINAL,

  /* OpenMP clause: mergeable.  */
  OMP_CLAUSE_MERGEABLE
};

/* The definition of tree nodes fills the next several pages.  */

/* A tree node can represent a data type, a variable, an expression
   or a statement.  Each node has a TREE_CODE which says what kind of
   thing it represents.  Some common codes are:
   INTEGER_TYPE -- represents a type of integers.
   ARRAY_TYPE -- represents a type of pointer.
   VAR_DECL -- represents a declared variable.
   INTEGER_CST -- represents a constant integer value.
   PLUS_EXPR -- represents a sum (an expression).

   As for the contents of a tree node: there are some fields
   that all nodes share.  Each TREE_CODE has various special-purpose
   fields as well.  The fields of a node are never accessed directly,
   always through accessor macros.  */

/* Every kind of tree node starts with this structure,
   so all nodes have these fields.

   See the accessor macros, defined below, for documentation of the
   fields, and the table below which connects the fields and the
   accessor macros.  */

struct GTY(()) tree_base {
  ENUM_BITFIELD(tree_code) code : 16;

  unsigned side_effects_flag : 1;
  unsigned constant_flag : 1;
  unsigned addressable_flag : 1;
  unsigned volatile_flag : 1;
  unsigned readonly_flag : 1;
  unsigned asm_written_flag: 1;
  unsigned nowarning_flag : 1;
  unsigned visited : 1;

  unsigned used_flag : 1;
  unsigned nothrow_flag : 1;
  unsigned static_flag : 1;
  unsigned public_flag : 1;
  unsigned private_flag : 1;
  unsigned protected_flag : 1;
  unsigned deprecated_flag : 1;
  unsigned default_def_flag : 1;

  union {
    /* The bits in the following structure should only be used with
       accessor macros that constrain inputs with tree checking.  */
    struct {
      unsigned lang_flag_0 : 1;
      unsigned lang_flag_1 : 1;
      unsigned lang_flag_2 : 1;
      unsigned lang_flag_3 : 1;
      unsigned lang_flag_4 : 1;
      unsigned lang_flag_5 : 1;
      unsigned lang_flag_6 : 1;
      unsigned saturating_flag : 1;

      unsigned unsigned_flag : 1;
      unsigned packed_flag : 1;
      unsigned user_align : 1;
      unsigned nameless_flag : 1;
      unsigned spare0 : 4;

      unsigned spare1 : 8;

      /* This field is only used with TREE_TYPE nodes; the only reason it is
	 present in tree_base instead of tree_type is to save space.  The size
	 of the field must be large enough to hold addr_space_t values.  */
      unsigned address_space : 8;
    } bits;
    /* The following fields are present in tree_base to save space.  The
       nodes using them do not require any of the flags above and so can
       make better use of the 4-byte sized word.  */
    /* VEC length.  This field is only used with TREE_VEC.  */
    int length;
    /* SSA version number.  This field is only used with SSA_NAME.  */
    unsigned int version;
  } GTY((skip(""))) u;
};

/* The following table lists the uses of each of the above flags and
   for which types of nodes they are defined.

   addressable_flag:

       TREE_ADDRESSABLE in
           VAR_DECL, PARM_DECL, RESULT_DECL, FUNCTION_DECL, LABEL_DECL
           SSA_NAME
           all types
           CONSTRUCTOR, IDENTIFIER_NODE
           STMT_EXPR

       CALL_EXPR_TAILCALL in
           CALL_EXPR

       CASE_LOW_SEEN in
           CASE_LABEL_EXPR

       PREDICT_EXPR_OUTCOME in
	   PREDICT_EXPR

   static_flag:

       TREE_STATIC in
           VAR_DECL, FUNCTION_DECL
           CONSTRUCTOR

       TREE_NO_TRAMPOLINE in
           ADDR_EXPR

       BINFO_VIRTUAL_P in
           TREE_BINFO

       TREE_SYMBOL_REFERENCED in
           IDENTIFIER_NODE

       CLEANUP_EH_ONLY in
           TARGET_EXPR, WITH_CLEANUP_EXPR

       TRY_CATCH_IS_CLEANUP in
           TRY_CATCH_EXPR

       ASM_INPUT_P in
           ASM_EXPR

       TYPE_REF_CAN_ALIAS_ALL in
           POINTER_TYPE, REFERENCE_TYPE

       CASE_HIGH_SEEN in
           CASE_LABEL_EXPR

       ENUM_IS_SCOPED in
	   ENUMERAL_TYPE

       TRANSACTION_EXPR_OUTER in
	   TRANSACTION_EXPR

   public_flag:

       TREE_OVERFLOW in
           INTEGER_CST, REAL_CST, COMPLEX_CST, VECTOR_CST

       TREE_PUBLIC in
           VAR_DECL, FUNCTION_DECL
           IDENTIFIER_NODE

       ASM_VOLATILE_P in
           ASM_EXPR

       CALL_EXPR_VA_ARG_PACK in
           CALL_EXPR

       TYPE_CACHED_VALUES_P in
           all types

       SAVE_EXPR_RESOLVED_P in
           SAVE_EXPR

       OMP_CLAUSE_LASTPRIVATE_FIRSTPRIVATE in
           OMP_CLAUSE_LASTPRIVATE

       OMP_CLAUSE_PRIVATE_DEBUG in
           OMP_CLAUSE_PRIVATE

       TRANSACTION_EXPR_RELAXED in
	   TRANSACTION_EXPR

   private_flag:

       TREE_PRIVATE in
           all decls

       CALL_EXPR_RETURN_SLOT_OPT in
           CALL_EXPR

       OMP_SECTION_LAST in
           OMP_SECTION

       OMP_PARALLEL_COMBINED in
           OMP_PARALLEL

       OMP_CLAUSE_PRIVATE_OUTER_REF in
	   OMP_CLAUSE_PRIVATE

       TYPE_REF_IS_RVALUE in
	   REFERENCE_TYPE

       ENUM_IS_OPAQUE in
	   ENUMERAL_TYPE

   protected_flag:

       TREE_PROTECTED in
           BLOCK
           all decls

       CALL_FROM_THUNK_P and
       CALL_ALLOCA_FOR_VAR_P in
           CALL_EXPR

   side_effects_flag:

       TREE_SIDE_EFFECTS in
           all expressions
           all decls
           all constants

       FORCED_LABEL in
           LABEL_DECL

   volatile_flag:

       TREE_THIS_VOLATILE in
           all expressions
           all decls

       TYPE_VOLATILE in
           all types

   readonly_flag:

       TREE_READONLY in
           all expressions
           all decls

       TYPE_READONLY in
           all types

   constant_flag:

       TREE_CONSTANT in
           all expressions
           all decls
           all constants

       TYPE_SIZES_GIMPLIFIED in
           all types

   unsigned_flag:

       TYPE_UNSIGNED in
           all types

       DECL_UNSIGNED in
           all decls

   asm_written_flag:

       TREE_ASM_WRITTEN in
           VAR_DECL, FUNCTION_DECL, TYPE_DECL
           RECORD_TYPE, UNION_TYPE, QUAL_UNION_TYPE
           BLOCK, STRING_CST

       SSA_NAME_OCCURS_IN_ABNORMAL_PHI in
           SSA_NAME

   used_flag:

       TREE_USED in
           all expressions
           all decls
           IDENTIFIER_NODE

   nothrow_flag:

       TREE_NOTHROW in
           CALL_EXPR
           FUNCTION_DECL

       TYPE_ALIGN_OK in
           all types

       TREE_THIS_NOTRAP in
          INDIRECT_REF, MEM_REF, TARGET_MEM_REF, ARRAY_REF, ARRAY_RANGE_REF

       SSA_NAME_IN_FREELIST in
          SSA_NAME

   deprecated_flag:

       TREE_DEPRECATED in
           all decls
	   all types

       IDENTIFIER_TRANSPARENT_ALIAS in
           IDENTIFIER_NODE

   visited:

       TREE_VISITED in
           all trees (used liberally by many passes)

   saturating_flag:

       TYPE_SATURATING in
           all types

       VAR_DECL_IS_VIRTUAL_OPERAND in
	   VAR_DECL

   nowarning_flag:

       TREE_NO_WARNING in
           all expressions
           all decls

       TYPE_ARTIFICIAL in
           all types

   default_def_flag:

       TYPE_VECTOR_OPAQUE in
	   VECTOR_TYPE

       SSA_NAME_IS_DEFAULT_DEF in
           SSA_NAME

       DECL_NONLOCAL_FRAME in
	   VAR_DECL
*/

struct GTY(()) tree_typed {
  struct tree_base base;
  tree type;
};

struct GTY(()) tree_common {
  struct tree_typed typed;
  tree chain;
};

#undef DEFTREESTRUCT
#define DEFTREESTRUCT(ENUM, NAME) ENUM,
enum tree_node_structure_enum {
#include "treestruct.def"
  LAST_TS_ENUM
};
#undef DEFTREESTRUCT

/* Define accessors for the fields that all tree nodes have
   (though some fields are not used for all kinds of nodes).  */

/* The tree-code says what kind of node it is.
   Codes are defined in tree.def.  */
#define TREE_CODE(NODE) ((enum tree_code) (NODE)->base.code)
#define TREE_SET_CODE(NODE, VALUE) ((NODE)->base.code = (VALUE))

/* When checking is enabled, errors will be generated if a tree node
   is accessed incorrectly. The macros die with a fatal error.  */
#if defined ENABLE_TREE_CHECKING && (GCC_VERSION >= 2007)

#define TREE_CHECK(T, CODE) \
(tree_check ((T), __FILE__, __LINE__, __FUNCTION__, (CODE)))

#define TREE_NOT_CHECK(T, CODE) \
(tree_not_check ((T), __FILE__, __LINE__, __FUNCTION__, (CODE)))

#define TREE_CHECK2(T, CODE1, CODE2) \
(tree_check2 ((T), __FILE__, __LINE__, __FUNCTION__, (CODE1), (CODE2)))

#define TREE_NOT_CHECK2(T, CODE1, CODE2) \
(tree_not_check2 ((T), __FILE__, __LINE__, __FUNCTION__, (CODE1), (CODE2)))

#define TREE_CHECK3(T, CODE1, CODE2, CODE3) \
(tree_check3 ((T), __FILE__, __LINE__, __FUNCTION__, (CODE1), (CODE2), (CODE3)))

#define TREE_NOT_CHECK3(T, CODE1, CODE2, CODE3) \
(tree_not_check3 ((T), __FILE__, __LINE__, __FUNCTION__, \
                               (CODE1), (CODE2), (CODE3)))

#define TREE_CHECK4(T, CODE1, CODE2, CODE3, CODE4) \
(tree_check4 ((T), __FILE__, __LINE__, __FUNCTION__, \
                           (CODE1), (CODE2), (CODE3), (CODE4)))

#define TREE_NOT_CHECK4(T, CODE1, CODE2, CODE3, CODE4) \
(tree_not_check4 ((T), __FILE__, __LINE__, __FUNCTION__, \
                               (CODE1), (CODE2), (CODE3), (CODE4)))

#define TREE_CHECK5(T, CODE1, CODE2, CODE3, CODE4, CODE5) \
(tree_check5 ((T), __FILE__, __LINE__, __FUNCTION__, \
                           (CODE1), (CODE2), (CODE3), (CODE4), (CODE5)))

#define TREE_NOT_CHECK5(T, CODE1, CODE2, CODE3, CODE4, CODE5) \
(tree_not_check5 ((T), __FILE__, __LINE__, __FUNCTION__, \
                               (CODE1), (CODE2), (CODE3), (CODE4), (CODE5)))

#define CONTAINS_STRUCT_CHECK(T, STRUCT) \
(contains_struct_check ((T), (STRUCT), __FILE__, __LINE__, __FUNCTION__))

#define TREE_CLASS_CHECK(T, CLASS) \
(tree_class_check ((T), (CLASS), __FILE__, __LINE__, __FUNCTION__))

#define TREE_RANGE_CHECK(T, CODE1, CODE2) \
(tree_range_check ((T), (CODE1), (CODE2), __FILE__, __LINE__, __FUNCTION__))

#define OMP_CLAUSE_SUBCODE_CHECK(T, CODE) \
(omp_clause_subcode_check ((T), (CODE), __FILE__, __LINE__, __FUNCTION__))

#define OMP_CLAUSE_RANGE_CHECK(T, CODE1, CODE2) \
(omp_clause_range_check ((T), (CODE1), (CODE2), \
                                      __FILE__, __LINE__, __FUNCTION__))

/* These checks have to be special cased.  */
#define EXPR_CHECK(T) \
(expr_check ((T), __FILE__, __LINE__, __FUNCTION__))

/* These checks have to be special cased.  */
#define NON_TYPE_CHECK(T) \
(non_type_check ((T), __FILE__, __LINE__, __FUNCTION__))

#define TREE_VEC_ELT_CHECK(T, I) \
(*(CONST_CAST2 (tree *, typeof (T)*, \
     tree_vec_elt_check ((T), (I), __FILE__, __LINE__, __FUNCTION__))))

#define OMP_CLAUSE_ELT_CHECK(T, I) \
(*(omp_clause_elt_check ((T), (I), __FILE__, __LINE__, __FUNCTION__)))

/* Special checks for TREE_OPERANDs.  */
#define TREE_OPERAND_CHECK(T, I) \
(*(CONST_CAST2 (tree*, typeof (T)*, \
     tree_operand_check ((T), (I), __FILE__, __LINE__, __FUNCTION__))))

#define TREE_OPERAND_CHECK_CODE(T, CODE, I) \
(*(tree_operand_check_code ((T), (CODE), (I), \
                                         __FILE__, __LINE__, __FUNCTION__)))

/* Nodes are chained together for many purposes.
   Types are chained together to record them for being output to the debugger
   (see the function `chain_type').
   Decls in the same scope are chained together to record the contents
   of the scope.
   Statement nodes for successive statements used to be chained together.
   Often lists of things are represented by TREE_LIST nodes that
   are chained together.  */

#define TREE_CHAIN(NODE) \
(CONTAINS_STRUCT_CHECK (NODE, TS_COMMON)->common.chain)

/* In all nodes that are expressions, this is the data type of the expression.
   In POINTER_TYPE nodes, this is the type that the pointer points to.
   In ARRAY_TYPE nodes, this is the type of the elements.
   In VECTOR_TYPE nodes, this is the type of the elements.  */
#define TREE_TYPE(NODE) \
(CONTAINS_STRUCT_CHECK (NODE, TS_TYPED)->typed.type)

extern void tree_contains_struct_check_failed (const_tree,
					       const enum tree_node_structure_enum,
					       const char *, int, const char *)
  ATTRIBUTE_NORETURN;

extern void tree_check_failed (const_tree, const char *, int, const char *,
			       ...) ATTRIBUTE_NORETURN;
extern void tree_not_check_failed (const_tree, const char *, int, const char *,
				   ...) ATTRIBUTE_NORETURN;
extern void tree_class_check_failed (const_tree, const enum tree_code_class,
				     const char *, int, const char *)
    ATTRIBUTE_NORETURN;
extern void tree_range_check_failed (const_tree, const char *, int,
				     const char *, enum tree_code,
				     enum tree_code)
    ATTRIBUTE_NORETURN;
extern void tree_not_class_check_failed (const_tree,
					 const enum tree_code_class,
					 const char *, int, const char *)
    ATTRIBUTE_NORETURN;
extern void tree_vec_elt_check_failed (int, int, const char *,
				       int, const char *)
    ATTRIBUTE_NORETURN;
extern void phi_node_elt_check_failed (int, int, const char *,
				       int, const char *)
    ATTRIBUTE_NORETURN;
extern void tree_operand_check_failed (int, const_tree,
				       const char *, int, const char *)
    ATTRIBUTE_NORETURN;
extern void omp_clause_check_failed (const_tree, const char *, int,
				     const char *, enum omp_clause_code)
    ATTRIBUTE_NORETURN;
extern void omp_clause_operand_check_failed (int, const_tree, const char *,
				             int, const char *)
    ATTRIBUTE_NORETURN;
extern void omp_clause_range_check_failed (const_tree, const char *, int,
			       const char *, enum omp_clause_code,
			       enum omp_clause_code)
    ATTRIBUTE_NORETURN;

#else /* not ENABLE_TREE_CHECKING, or not gcc */

#define CONTAINS_STRUCT_CHECK(T, ENUM)          (T)
#define TREE_CHECK(T, CODE)			(T)
#define TREE_NOT_CHECK(T, CODE)			(T)
#define TREE_CHECK2(T, CODE1, CODE2)		(T)
#define TREE_NOT_CHECK2(T, CODE1, CODE2)	(T)
#define TREE_CHECK3(T, CODE1, CODE2, CODE3)	(T)
#define TREE_NOT_CHECK3(T, CODE1, CODE2, CODE3)	(T)
#define TREE_CHECK4(T, CODE1, CODE2, CODE3, CODE4) (T)
#define TREE_NOT_CHECK4(T, CODE1, CODE2, CODE3, CODE4) (T)
#define TREE_CHECK5(T, CODE1, CODE2, CODE3, CODE4, CODE5) (T)
#define TREE_NOT_CHECK5(T, CODE1, CODE2, CODE3, CODE4, CODE5) (T)
#define TREE_CLASS_CHECK(T, CODE)		(T)
#define TREE_RANGE_CHECK(T, CODE1, CODE2)	(T)
#define EXPR_CHECK(T)				(T)
#define NON_TYPE_CHECK(T)			(T)
#define TREE_VEC_ELT_CHECK(T, I)		((T)->vec.a[I])
#define TREE_OPERAND_CHECK(T, I)		((T)->exp.operands[I])
#define TREE_OPERAND_CHECK_CODE(T, CODE, I)	((T)->exp.operands[I])
#define OMP_CLAUSE_ELT_CHECK(T, i)	        ((T)->omp_clause.ops[i])
#define OMP_CLAUSE_RANGE_CHECK(T, CODE1, CODE2)	(T)
#define OMP_CLAUSE_SUBCODE_CHECK(T, CODE)	(T)

#define TREE_CHAIN(NODE) ((NODE)->common.chain)
#define TREE_TYPE(NODE) ((NODE)->typed.type)

#endif

#define TREE_BLOCK(NODE)		(tree_block (NODE))
#define TREE_SET_BLOCK(T, B)		(tree_set_block ((T), (B)))

#include "tree-check.h"

#define TYPE_CHECK(T)		TREE_CLASS_CHECK (T, tcc_type)
#define DECL_MINIMAL_CHECK(T)   CONTAINS_STRUCT_CHECK (T, TS_DECL_MINIMAL)
#define DECL_COMMON_CHECK(T)    CONTAINS_STRUCT_CHECK (T, TS_DECL_COMMON)
#define DECL_WRTL_CHECK(T)      CONTAINS_STRUCT_CHECK (T, TS_DECL_WRTL)
#define DECL_WITH_VIS_CHECK(T)  CONTAINS_STRUCT_CHECK (T, TS_DECL_WITH_VIS)
#define DECL_NON_COMMON_CHECK(T) CONTAINS_STRUCT_CHECK (T, TS_DECL_NON_COMMON)
#define CST_CHECK(T)		TREE_CLASS_CHECK (T, tcc_constant)
#define STMT_CHECK(T)		TREE_CLASS_CHECK (T, tcc_statement)
#define VL_EXP_CHECK(T)		TREE_CLASS_CHECK (T, tcc_vl_exp)
#define FUNC_OR_METHOD_CHECK(T)	TREE_CHECK2 (T, FUNCTION_TYPE, METHOD_TYPE)
#define PTR_OR_REF_CHECK(T)	TREE_CHECK2 (T, POINTER_TYPE, REFERENCE_TYPE)

#define RECORD_OR_UNION_CHECK(T)	\
  TREE_CHECK3 (T, RECORD_TYPE, UNION_TYPE, QUAL_UNION_TYPE)
#define NOT_RECORD_OR_UNION_CHECK(T) \
  TREE_NOT_CHECK3 (T, RECORD_TYPE, UNION_TYPE, QUAL_UNION_TYPE)

#define NUMERICAL_TYPE_CHECK(T)					\
  TREE_CHECK5 (T, INTEGER_TYPE, ENUMERAL_TYPE, BOOLEAN_TYPE, REAL_TYPE,	\
	       FIXED_POINT_TYPE)

/* Here is how primitive or already-canonicalized types' hash codes
   are made.  */
#define TYPE_HASH(TYPE) (TYPE_UID (TYPE))

/* A simple hash function for an arbitrary tree node.  This must not be
   used in hash tables which are saved to a PCH.  */
#define TREE_HASH(NODE) ((size_t) (NODE) & 0777777)

/* Tests if CODE is a conversion expr (NOP_EXPR or CONVERT_EXPR).  */
#define CONVERT_EXPR_CODE_P(CODE)				\
  ((CODE) == NOP_EXPR || (CODE) == CONVERT_EXPR)

/* Similarly, but accept an expressions instead of a tree code.  */
#define CONVERT_EXPR_P(EXP)	CONVERT_EXPR_CODE_P (TREE_CODE (EXP))

/* Generate case for NOP_EXPR, CONVERT_EXPR.  */

#define CASE_CONVERT						\
  case NOP_EXPR:						\
  case CONVERT_EXPR

/* Given an expression as a tree, strip any conversion that generates
   no instruction.  Accepts both tree and const_tree arguments since
   we are not modifying the tree itself.  */

#define STRIP_NOPS(EXP) \
  (EXP) = tree_strip_nop_conversions (CONST_CAST_TREE (EXP))

/* Like STRIP_NOPS, but don't let the signedness change either.  */

#define STRIP_SIGN_NOPS(EXP) \
  (EXP) = tree_strip_sign_nop_conversions (CONST_CAST_TREE (EXP))

/* Like STRIP_NOPS, but don't alter the TREE_TYPE either.  */

#define STRIP_TYPE_NOPS(EXP) \
  while ((CONVERT_EXPR_P (EXP)					\
	  || TREE_CODE (EXP) == NON_LVALUE_EXPR)		\
	 && TREE_OPERAND (EXP, 0) != error_mark_node		\
	 && (TREE_TYPE (EXP)					\
	     == TREE_TYPE (TREE_OPERAND (EXP, 0))))		\
    (EXP) = TREE_OPERAND (EXP, 0)

/* Remove unnecessary type conversions according to
   tree_ssa_useless_type_conversion.  */

#define STRIP_USELESS_TYPE_CONVERSION(EXP) \
  (EXP) = tree_ssa_strip_useless_type_conversions (EXP)

/* Nonzero if TYPE represents an integral type.  Note that we do not
   include COMPLEX types here.  Keep these checks in ascending code
   order.  */

#define INTEGRAL_TYPE_P(TYPE)  \
  (TREE_CODE (TYPE) == ENUMERAL_TYPE  \
   || TREE_CODE (TYPE) == BOOLEAN_TYPE \
   || TREE_CODE (TYPE) == INTEGER_TYPE)

/* Nonzero if TYPE represents a non-saturating fixed-point type.  */

#define NON_SAT_FIXED_POINT_TYPE_P(TYPE) \
  (TREE_CODE (TYPE) == FIXED_POINT_TYPE && !TYPE_SATURATING (TYPE))

/* Nonzero if TYPE represents a saturating fixed-point type.  */

#define SAT_FIXED_POINT_TYPE_P(TYPE) \
  (TREE_CODE (TYPE) == FIXED_POINT_TYPE && TYPE_SATURATING (TYPE))

/* Nonzero if TYPE represents a fixed-point type.  */

#define FIXED_POINT_TYPE_P(TYPE)	(TREE_CODE (TYPE) == FIXED_POINT_TYPE)

/* Nonzero if TYPE represents a scalar floating-point type.  */

#define SCALAR_FLOAT_TYPE_P(TYPE) (TREE_CODE (TYPE) == REAL_TYPE)

/* Nonzero if TYPE represents a complex floating-point type.  */

#define COMPLEX_FLOAT_TYPE_P(TYPE)	\
  (TREE_CODE (TYPE) == COMPLEX_TYPE	\
   && TREE_CODE (TREE_TYPE (TYPE)) == REAL_TYPE)

/* Nonzero if TYPE represents a vector integer type.  */
                
#define VECTOR_INTEGER_TYPE_P(TYPE)                   \
             (TREE_CODE (TYPE) == VECTOR_TYPE      \
                 && TREE_CODE (TREE_TYPE (TYPE)) == INTEGER_TYPE)


/* Nonzero if TYPE represents a vector floating-point type.  */

#define VECTOR_FLOAT_TYPE_P(TYPE)	\
  (TREE_CODE (TYPE) == VECTOR_TYPE	\
   && TREE_CODE (TREE_TYPE (TYPE)) == REAL_TYPE)

/* Nonzero if TYPE represents a floating-point type, including complex
   and vector floating-point types.  The vector and complex check does
   not use the previous two macros to enable early folding.  */

#define FLOAT_TYPE_P(TYPE)			\
  (SCALAR_FLOAT_TYPE_P (TYPE)			\
   || ((TREE_CODE (TYPE) == COMPLEX_TYPE 	\
        || TREE_CODE (TYPE) == VECTOR_TYPE)	\
       && SCALAR_FLOAT_TYPE_P (TREE_TYPE (TYPE))))

/* Nonzero if TYPE represents a decimal floating-point type.  */
#define DECIMAL_FLOAT_TYPE_P(TYPE)		\
  (SCALAR_FLOAT_TYPE_P (TYPE)			\
   && DECIMAL_FLOAT_MODE_P (TYPE_MODE (TYPE)))

/* Nonzero if TYPE is a record or union type.  */
#define RECORD_OR_UNION_TYPE_P(TYPE)		\
  (TREE_CODE (TYPE) == RECORD_TYPE		\
   || TREE_CODE (TYPE) == UNION_TYPE		\
   || TREE_CODE (TYPE) == QUAL_UNION_TYPE)

/* Nonzero if TYPE represents an aggregate (multi-component) type.
   Keep these checks in ascending code order.  */

#define AGGREGATE_TYPE_P(TYPE) \
  (TREE_CODE (TYPE) == ARRAY_TYPE || RECORD_OR_UNION_TYPE_P (TYPE))

/* Nonzero if TYPE represents a pointer or reference type.
   (It should be renamed to INDIRECT_TYPE_P.)  Keep these checks in
   ascending code order.  */

#define POINTER_TYPE_P(TYPE) \
  (TREE_CODE (TYPE) == POINTER_TYPE || TREE_CODE (TYPE) == REFERENCE_TYPE)

/* Nonzero if TYPE represents a pointer to function.  */
#define FUNCTION_POINTER_TYPE_P(TYPE) \
  (POINTER_TYPE_P (TYPE) && TREE_CODE (TREE_TYPE (TYPE)) == FUNCTION_TYPE)

/* Nonzero if this type is a complete type.  */
#define COMPLETE_TYPE_P(NODE) (TYPE_SIZE (NODE) != NULL_TREE)

/* Nonzero if this type is the (possibly qualified) void type.  */
#define VOID_TYPE_P(NODE) (TREE_CODE (NODE) == VOID_TYPE)

/* Nonzero if this type is complete or is cv void.  */
#define COMPLETE_OR_VOID_TYPE_P(NODE) \
  (COMPLETE_TYPE_P (NODE) || VOID_TYPE_P (NODE))

/* Nonzero if this type is complete or is an array with unspecified bound.  */
#define COMPLETE_OR_UNBOUND_ARRAY_TYPE_P(NODE) \
  (COMPLETE_TYPE_P (TREE_CODE (NODE) == ARRAY_TYPE ? TREE_TYPE (NODE) : (NODE)))


/* Define many boolean fields that all tree nodes have.  */

/* In VAR_DECL, PARM_DECL and RESULT_DECL nodes, nonzero means address
   of this is needed.  So it cannot be in a register.
   In a FUNCTION_DECL it has no meaning.
   In LABEL_DECL nodes, it means a goto for this label has been seen
   from a place outside all binding contours that restore stack levels.
   In an artificial SSA_NAME that points to a stack partition with at least
   two variables, it means that at least one variable has TREE_ADDRESSABLE.
   In ..._TYPE nodes, it means that objects of this type must be fully
   addressable.  This means that pieces of this object cannot go into
   register parameters, for example.  If this a function type, this
   means that the value must be returned in memory.
   In CONSTRUCTOR nodes, it means object constructed must be in memory.
   In IDENTIFIER_NODEs, this means that some extern decl for this name
   had its address taken.  That matters for inline functions.
   In a STMT_EXPR, it means we want the result of the enclosed expression.  */
#define TREE_ADDRESSABLE(NODE) ((NODE)->base.addressable_flag)

/* Set on a CALL_EXPR if the call is in a tail position, ie. just before the
   exit of a function.  Calls for which this is true are candidates for tail
   call optimizations.  */
#define CALL_EXPR_TAILCALL(NODE) \
  (CALL_EXPR_CHECK(NODE)->base.addressable_flag)

/* Used as a temporary field on a CASE_LABEL_EXPR to indicate that the
   CASE_LOW operand has been processed.  */
#define CASE_LOW_SEEN(NODE) \
  (CASE_LABEL_EXPR_CHECK (NODE)->base.addressable_flag)

#define PREDICT_EXPR_OUTCOME(NODE) \
  ((enum prediction) (PREDICT_EXPR_CHECK(NODE)->base.addressable_flag))
#define SET_PREDICT_EXPR_OUTCOME(NODE, OUTCOME) \
  (PREDICT_EXPR_CHECK(NODE)->base.addressable_flag = (int) OUTCOME)
#define PREDICT_EXPR_PREDICTOR(NODE) \
  ((enum br_predictor)tree_low_cst (TREE_OPERAND (PREDICT_EXPR_CHECK (NODE), 0), 0))

/* In a VAR_DECL, nonzero means allocate static storage.
   In a FUNCTION_DECL, nonzero if function has been defined.
   In a CONSTRUCTOR, nonzero means allocate static storage.  */
#define TREE_STATIC(NODE) ((NODE)->base.static_flag)

/* In an ADDR_EXPR, nonzero means do not use a trampoline.  */
#define TREE_NO_TRAMPOLINE(NODE) (ADDR_EXPR_CHECK (NODE)->base.static_flag)

/* In a TARGET_EXPR or WITH_CLEANUP_EXPR, means that the pertinent cleanup
   should only be executed if an exception is thrown, not on normal exit
   of its scope.  */
#define CLEANUP_EH_ONLY(NODE) ((NODE)->base.static_flag)

/* In a TRY_CATCH_EXPR, means that the handler should be considered a
   separate cleanup in honor_protect_cleanup_actions.  */
#define TRY_CATCH_IS_CLEANUP(NODE) \
  (TRY_CATCH_EXPR_CHECK (NODE)->base.static_flag)

/* Used as a temporary field on a CASE_LABEL_EXPR to indicate that the
   CASE_HIGH operand has been processed.  */
#define CASE_HIGH_SEEN(NODE) \
  (CASE_LABEL_EXPR_CHECK (NODE)->base.static_flag)

/* Used to mark scoped enums.  */
#define ENUM_IS_SCOPED(NODE) (ENUMERAL_TYPE_CHECK (NODE)->base.static_flag)

/* Determines whether an ENUMERAL_TYPE has defined the list of constants. */
#define ENUM_IS_OPAQUE(NODE) (ENUMERAL_TYPE_CHECK (NODE)->base.private_flag)

/* In an expr node (usually a conversion) this means the node was made
   implicitly and should not lead to any sort of warning.  In a decl node,
   warnings concerning the decl should be suppressed.  This is used at
   least for used-before-set warnings, and it set after one warning is
   emitted.  */
#define TREE_NO_WARNING(NODE) ((NODE)->base.nowarning_flag)

/* Used to indicate that this TYPE represents a compiler-generated entity.  */
#define TYPE_ARTIFICIAL(NODE) (TYPE_CHECK (NODE)->base.nowarning_flag)

/* In an IDENTIFIER_NODE, this means that assemble_name was called with
   this string as an argument.  */
#define TREE_SYMBOL_REFERENCED(NODE) \
  (IDENTIFIER_NODE_CHECK (NODE)->base.static_flag)

/* Nonzero in a pointer or reference type means the data pointed to
   by this type can alias anything.  */
#define TYPE_REF_CAN_ALIAS_ALL(NODE) \
  (PTR_OR_REF_CHECK (NODE)->base.static_flag)

/* In an INTEGER_CST, REAL_CST, COMPLEX_CST, or VECTOR_CST, this means
   there was an overflow in folding.  */

#define TREE_OVERFLOW(NODE) (CST_CHECK (NODE)->base.public_flag)

/* TREE_OVERFLOW can only be true for EXPR of CONSTANT_CLASS_P.  */

#define TREE_OVERFLOW_P(EXPR) \
 (CONSTANT_CLASS_P (EXPR) && TREE_OVERFLOW (EXPR))

/* In a VAR_DECL, FUNCTION_DECL, NAMESPACE_DECL or TYPE_DECL,
   nonzero means name is to be accessible from outside this translation unit.
   In an IDENTIFIER_NODE, nonzero means an external declaration
   accessible from outside this translation unit was previously seen
   for this name in an inner scope.  */
#define TREE_PUBLIC(NODE) ((NODE)->base.public_flag)

/* In a _TYPE, indicates whether TYPE_CACHED_VALUES contains a vector
   of cached values, or is something else.  */
#define TYPE_CACHED_VALUES_P(NODE) (TYPE_CHECK(NODE)->base.public_flag)

/* In a SAVE_EXPR, indicates that the original expression has already
   been substituted with a VAR_DECL that contains the value.  */
#define SAVE_EXPR_RESOLVED_P(NODE) \
  (SAVE_EXPR_CHECK (NODE)->base.public_flag)

/* Set on a CALL_EXPR if this stdarg call should be passed the argument
   pack.  */
#define CALL_EXPR_VA_ARG_PACK(NODE) \
  (CALL_EXPR_CHECK(NODE)->base.public_flag)

/* In any expression, decl, or constant, nonzero means it has side effects or
   reevaluation of the whole expression could produce a different value.
   This is set if any subexpression is a function call, a side effect or a
   reference to a volatile variable.  In a ..._DECL, this is set only if the
   declaration said `volatile'.  This will never be set for a constant.  */
#define TREE_SIDE_EFFECTS(NODE) \
  (NON_TYPE_CHECK (NODE)->base.side_effects_flag)

/* In a LABEL_DECL, nonzero means this label had its address taken
   and therefore can never be deleted and is a jump target for
   computed gotos.  */
#define FORCED_LABEL(NODE) (LABEL_DECL_CHECK (NODE)->base.side_effects_flag)

/* Nonzero means this expression is volatile in the C sense:
   its address should be of type `volatile WHATEVER *'.
   In other words, the declared item is volatile qualified.
   This is used in _DECL nodes and _REF nodes.
   On a FUNCTION_DECL node, this means the function does not
   return normally.  This is the same effect as setting
   the attribute noreturn on the function in C.

   In a ..._TYPE node, means this type is volatile-qualified.
   But use TYPE_VOLATILE instead of this macro when the node is a type,
   because eventually we may make that a different bit.

   If this bit is set in an expression, so is TREE_SIDE_EFFECTS.  */
#define TREE_THIS_VOLATILE(NODE) ((NODE)->base.volatile_flag)

/* Nonzero means this node will not trap.  In an INDIRECT_REF, means
   accessing the memory pointed to won't generate a trap.  However,
   this only applies to an object when used appropriately: it doesn't
   mean that writing a READONLY mem won't trap.

   In ARRAY_REF and ARRAY_RANGE_REF means that we know that the index
   (or slice of the array) always belongs to the range of the array.
   I.e. that the access will not trap, provided that the access to
   the base to the array will not trap.  */
#define TREE_THIS_NOTRAP(NODE) \
  (TREE_CHECK5 (NODE, INDIRECT_REF, MEM_REF, TARGET_MEM_REF, ARRAY_REF,	\
		ARRAY_RANGE_REF)->base.nothrow_flag)

/* In a VAR_DECL, PARM_DECL or FIELD_DECL, or any kind of ..._REF node,
   nonzero means it may not be the lhs of an assignment.
   Nonzero in a FUNCTION_DECL means this function should be treated
   as "const" function (can only read its arguments).  */
#define TREE_READONLY(NODE) (NON_TYPE_CHECK (NODE)->base.readonly_flag)

/* Value of expression is constant.  Always on in all ..._CST nodes.  May
   also appear in an expression or decl where the value is constant.  */
#define TREE_CONSTANT(NODE) (NON_TYPE_CHECK (NODE)->base.constant_flag)

/* Nonzero if NODE, a type, has had its sizes gimplified.  */
#define TYPE_SIZES_GIMPLIFIED(NODE) \
  (TYPE_CHECK (NODE)->base.constant_flag)

/* In a decl (most significantly a FIELD_DECL), means an unsigned field.  */
#define DECL_UNSIGNED(NODE) \
  (DECL_COMMON_CHECK (NODE)->base.u.bits.unsigned_flag)

/* In integral and pointer types, means an unsigned type.  */
#define TYPE_UNSIGNED(NODE) (TYPE_CHECK (NODE)->base.u.bits.unsigned_flag)

/* True if overflow wraps around for the given integral type.  That
   is, TYPE_MAX + 1 == TYPE_MIN.  */
#define TYPE_OVERFLOW_WRAPS(TYPE) \
  (TYPE_UNSIGNED (TYPE) || flag_wrapv)

/* True if overflow is undefined for the given integral type.  We may
   optimize on the assumption that values in the type never overflow.

   IMPORTANT NOTE: Any optimization based on TYPE_OVERFLOW_UNDEFINED
   must issue a warning based on warn_strict_overflow.  In some cases
   it will be appropriate to issue the warning immediately, and in
   other cases it will be appropriate to simply set a flag and let the
   caller decide whether a warning is appropriate or not.  */
#define TYPE_OVERFLOW_UNDEFINED(TYPE) \
  (!TYPE_UNSIGNED (TYPE) && !flag_wrapv && !flag_trapv && flag_strict_overflow)

/* True if overflow for the given integral type should issue a
   trap.  */
#define TYPE_OVERFLOW_TRAPS(TYPE) \
  (!TYPE_UNSIGNED (TYPE) && flag_trapv)

/* True if pointer types have undefined overflow.  */
#define POINTER_TYPE_OVERFLOW_UNDEFINED (flag_strict_overflow)

/* Nonzero in a VAR_DECL or STRING_CST means assembler code has been written.
   Nonzero in a FUNCTION_DECL means that the function has been compiled.
   This is interesting in an inline function, since it might not need
   to be compiled separately.
   Nonzero in a RECORD_TYPE, UNION_TYPE, QUAL_UNION_TYPE, ENUMERAL_TYPE
   or TYPE_DECL if the debugging info for the type has been written.
   In a BLOCK node, nonzero if reorder_blocks has already seen this block.
   In an SSA_NAME node, nonzero if the SSA_NAME occurs in an abnormal
   PHI node.  */
#define TREE_ASM_WRITTEN(NODE) ((NODE)->base.asm_written_flag)

/* Nonzero in a _DECL if the name is used in its scope.
   Nonzero in an expr node means inhibit warning if value is unused.
   In IDENTIFIER_NODEs, this means that some extern decl for this name
   was used.
   In a BLOCK, this means that the block contains variables that are used.  */
#define TREE_USED(NODE) ((NODE)->base.used_flag)

/* In a FUNCTION_DECL, nonzero means a call to the function cannot
   throw an exception.  In a CALL_EXPR, nonzero means the call cannot
   throw.  We can't easily check the node type here as the C++
   frontend also uses this flag (for AGGR_INIT_EXPR).  */
#define TREE_NOTHROW(NODE) ((NODE)->base.nothrow_flag)

/* In a CALL_EXPR, means that it's safe to use the target of the call
   expansion as the return slot for a call that returns in memory.  */
#define CALL_EXPR_RETURN_SLOT_OPT(NODE) \
  (CALL_EXPR_CHECK (NODE)->base.private_flag)

/* In a RESULT_DECL, PARM_DECL and VAR_DECL, means that it is
   passed by invisible reference (and the TREE_TYPE is a pointer to the true
   type).  */
#define DECL_BY_REFERENCE(NODE) \
  (TREE_CHECK3 (NODE, VAR_DECL, PARM_DECL, \
		RESULT_DECL)->decl_common.decl_by_reference_flag)

/* In VAR_DECL and PARM_DECL, set when the decl has been used except for
   being set.  */
#define DECL_READ_P(NODE) \
  (TREE_CHECK2 (NODE, VAR_DECL, PARM_DECL)->decl_common.decl_read_flag)

/* In VAR_DECL or RESULT_DECL, set when significant code movement precludes
   attempting to share the stack slot with some other variable.  */
#define DECL_NONSHAREABLE(NODE) \
  (TREE_CHECK2 (NODE, VAR_DECL, \
		RESULT_DECL)->decl_common.decl_nonshareable_flag)

/* In a CALL_EXPR, means that the call is the jump from a thunk to the
   thunked-to function.  */
#define CALL_FROM_THUNK_P(NODE) (CALL_EXPR_CHECK (NODE)->base.protected_flag)

/* In a CALL_EXPR, if the function being called is BUILT_IN_ALLOCA, means that
   it has been built for the declaration of a variable-sized object.  */
#define CALL_ALLOCA_FOR_VAR_P(NODE) \
  (CALL_EXPR_CHECK (NODE)->base.protected_flag)

/* In a type, nonzero means that all objects of the type are guaranteed by the
   language or front-end to be properly aligned, so we can indicate that a MEM
   of this type is aligned at least to the alignment of the type, even if it
   doesn't appear that it is.  We see this, for example, in object-oriented
   languages where a tag field may show this is an object of a more-aligned
   variant of the more generic type.

   In an SSA_NAME node, nonzero if the SSA_NAME node is on the SSA_NAME
   freelist.  */
#define TYPE_ALIGN_OK(NODE) (TYPE_CHECK (NODE)->base.nothrow_flag)

/* Used in classes in C++.  */
#define TREE_PRIVATE(NODE) ((NODE)->base.private_flag)
/* Used in classes in C++. */
#define TREE_PROTECTED(NODE) ((NODE)->base.protected_flag)

/* True if reference type NODE is a C++ rvalue reference.  */
#define TYPE_REF_IS_RVALUE(NODE) \
  (REFERENCE_TYPE_CHECK (NODE)->base.private_flag)

/* Nonzero in a _DECL if the use of the name is defined as a
   deprecated feature by __attribute__((deprecated)).  */
#define TREE_DEPRECATED(NODE) \
  ((NODE)->base.deprecated_flag)

/* Nonzero in an IDENTIFIER_NODE if the name is a local alias, whose
   uses are to be substituted for uses of the TREE_CHAINed identifier.  */
#define IDENTIFIER_TRANSPARENT_ALIAS(NODE) \
  (IDENTIFIER_NODE_CHECK (NODE)->base.deprecated_flag)

/* In fixed-point types, means a saturating type.  */
#define TYPE_SATURATING(NODE) (TYPE_CHECK (NODE)->base.u.bits.saturating_flag)

/* These flags are available for each language front end to use internally.  */
#define TREE_LANG_FLAG_0(NODE) \
  (TREE_NOT_CHECK2(NODE, TREE_VEC, SSA_NAME)->base.u.bits.lang_flag_0)
#define TREE_LANG_FLAG_1(NODE) \
  (TREE_NOT_CHECK2(NODE, TREE_VEC, SSA_NAME)->base.u.bits.lang_flag_1)
#define TREE_LANG_FLAG_2(NODE) \
  (TREE_NOT_CHECK2(NODE, TREE_VEC, SSA_NAME)->base.u.bits.lang_flag_2)
#define TREE_LANG_FLAG_3(NODE) \
  (TREE_NOT_CHECK2(NODE, TREE_VEC, SSA_NAME)->base.u.bits.lang_flag_3)
#define TREE_LANG_FLAG_4(NODE) \
  (TREE_NOT_CHECK2(NODE, TREE_VEC, SSA_NAME)->base.u.bits.lang_flag_4)
#define TREE_LANG_FLAG_5(NODE) \
  (TREE_NOT_CHECK2(NODE, TREE_VEC, SSA_NAME)->base.u.bits.lang_flag_5)
#define TREE_LANG_FLAG_6(NODE) \
  (TREE_NOT_CHECK2(NODE, TREE_VEC, SSA_NAME)->base.u.bits.lang_flag_6)

/* Define additional fields and accessors for nodes representing constants.  */

/* In an INTEGER_CST node.  These two together make a 2-word integer.
   If the data type is signed, the value is sign-extended to 2 words
   even though not all of them may really be in use.
   In an unsigned constant shorter than 2 words, the extra bits are 0.  */
#define TREE_INT_CST(NODE) (INTEGER_CST_CHECK (NODE)->int_cst.int_cst)
#define TREE_INT_CST_LOW(NODE) (TREE_INT_CST (NODE).low)
#define TREE_INT_CST_HIGH(NODE) (TREE_INT_CST (NODE).high)

#define INT_CST_LT(A, B)				\
  (TREE_INT_CST_HIGH (A) < TREE_INT_CST_HIGH (B)	\
   || (TREE_INT_CST_HIGH (A) == TREE_INT_CST_HIGH (B)	\
       && TREE_INT_CST_LOW (A) < TREE_INT_CST_LOW (B)))

#define INT_CST_LT_UNSIGNED(A, B)				\
  (((unsigned HOST_WIDE_INT) TREE_INT_CST_HIGH (A)		\
    < (unsigned HOST_WIDE_INT) TREE_INT_CST_HIGH (B))		\
   || (((unsigned HOST_WIDE_INT) TREE_INT_CST_HIGH (A)		\
	== (unsigned HOST_WIDE_INT) TREE_INT_CST_HIGH (B))	\
       && TREE_INT_CST_LOW (A) < TREE_INT_CST_LOW (B)))

struct GTY(()) tree_int_cst {
  struct tree_typed typed;
  double_int int_cst;
};

/* In a REAL_CST node.  struct real_value is an opaque entity, with
   manipulators defined in real.h.  We don't want tree.h depending on
   real.h and transitively on tm.h.  */
struct real_value;

#define TREE_REAL_CST_PTR(NODE) (REAL_CST_CHECK (NODE)->real_cst.real_cst_ptr)
#define TREE_REAL_CST(NODE) (*TREE_REAL_CST_PTR (NODE))

struct GTY(()) tree_real_cst {
  struct tree_typed typed;
  struct real_value * real_cst_ptr;
};

/* In a FIXED_CST node.  */
struct fixed_value;

#define TREE_FIXED_CST_PTR(NODE) \
  (FIXED_CST_CHECK (NODE)->fixed_cst.fixed_cst_ptr)
#define TREE_FIXED_CST(NODE) (*TREE_FIXED_CST_PTR (NODE))

struct GTY(()) tree_fixed_cst {
  struct tree_typed typed;
  struct fixed_value * fixed_cst_ptr;
};

/* In a STRING_CST */
/* In C terms, this is sizeof, not strlen.  */
#define TREE_STRING_LENGTH(NODE) (STRING_CST_CHECK (NODE)->string.length)
#define TREE_STRING_POINTER(NODE) \
  ((const char *)(STRING_CST_CHECK (NODE)->string.str))

struct GTY(()) tree_string {
  struct tree_typed typed;
  int length;
  char str[1];
};

/* In a COMPLEX_CST node.  */
#define TREE_REALPART(NODE) (COMPLEX_CST_CHECK (NODE)->complex.real)
#define TREE_IMAGPART(NODE) (COMPLEX_CST_CHECK (NODE)->complex.imag)

struct GTY(()) tree_complex {
  struct tree_typed typed;
  tree real;
  tree imag;
};

/* In a VECTOR_CST node.  */
#define VECTOR_CST_NELTS(NODE) (TYPE_VECTOR_SUBPARTS (TREE_TYPE (NODE)))
#define VECTOR_CST_ELTS(NODE) (VECTOR_CST_CHECK (NODE)->vector.elts)
#define VECTOR_CST_ELT(NODE,IDX) (VECTOR_CST_CHECK (NODE)->vector.elts[IDX])

struct GTY(()) tree_vector {
  struct tree_typed typed;
  tree GTY ((length ("TYPE_VECTOR_SUBPARTS (TREE_TYPE ((tree)&%h))"))) elts[1];
};

#include "symtab.h"

/* Define fields and accessors for some special-purpose tree nodes.  */

#define IDENTIFIER_LENGTH(NODE) \
  (IDENTIFIER_NODE_CHECK (NODE)->identifier.id.len)
#define IDENTIFIER_POINTER(NODE) \
  ((const char *) IDENTIFIER_NODE_CHECK (NODE)->identifier.id.str)
#define IDENTIFIER_HASH_VALUE(NODE) \
  (IDENTIFIER_NODE_CHECK (NODE)->identifier.id.hash_value)

/* Translate a hash table identifier pointer to a tree_identifier
   pointer, and vice versa.  */

#define HT_IDENT_TO_GCC_IDENT(NODE) \
  ((tree) ((char *) (NODE) - sizeof (struct tree_common)))
#define GCC_IDENT_TO_HT_IDENT(NODE) (&((struct tree_identifier *) (NODE))->id)

struct GTY(()) tree_identifier {
  struct tree_common common;
  struct ht_identifier id;
};

/* In a TREE_LIST node.  */
#define TREE_PURPOSE(NODE) (TREE_LIST_CHECK (NODE)->list.purpose)
#define TREE_VALUE(NODE) (TREE_LIST_CHECK (NODE)->list.value)

struct GTY(()) tree_list {
  struct tree_common common;
  tree purpose;
  tree value;
};

/* In a TREE_VEC node.  */
#define TREE_VEC_LENGTH(NODE) (TREE_VEC_CHECK (NODE)->base.u.length)
#define TREE_VEC_END(NODE) \
  ((void) TREE_VEC_CHECK (NODE), &((NODE)->vec.a[(NODE)->vec.base.u.length]))

#define TREE_VEC_ELT(NODE,I) TREE_VEC_ELT_CHECK (NODE, I)

struct GTY(()) tree_vec {
  struct tree_common common;
  tree GTY ((length ("TREE_VEC_LENGTH ((tree)&%h)"))) a[1];
};

/* In a CONSTRUCTOR node.  */
#define CONSTRUCTOR_ELTS(NODE) (CONSTRUCTOR_CHECK (NODE)->constructor.elts)
#define CONSTRUCTOR_ELT(NODE,IDX) \
  (&(*CONSTRUCTOR_ELTS (NODE))[IDX])
#define CONSTRUCTOR_NELTS(NODE) \
  (vec_safe_length (CONSTRUCTOR_ELTS (NODE)))

/* Iterate through the vector V of CONSTRUCTOR_ELT elements, yielding the
   value of each element (stored within VAL). IX must be a scratch variable
   of unsigned integer type.  */
#define FOR_EACH_CONSTRUCTOR_VALUE(V, IX, VAL) \
  for (IX = 0; (IX >= vec_safe_length (V)) \
	       ? false \
	       : ((VAL = (*(V))[IX].value), \
	       true); \
       (IX)++)

/* Iterate through the vector V of CONSTRUCTOR_ELT elements, yielding both
   the value of each element (stored within VAL) and its index (stored
   within INDEX). IX must be a scratch variable of unsigned integer type.  */
#define FOR_EACH_CONSTRUCTOR_ELT(V, IX, INDEX, VAL) \
  for (IX = 0; (IX >= vec_safe_length (V)) \
	       ? false \
	       : (((void) (VAL = (*V)[IX].value)), \
		  (INDEX = (*V)[IX].index), \
		  true); \
       (IX)++)

/* Append a new constructor element to V, with the specified INDEX and VAL.  */
#define CONSTRUCTOR_APPEND_ELT(V, INDEX, VALUE) \
  do { \
    constructor_elt _ce___ = {INDEX, VALUE}; \
    vec_safe_push ((V), _ce___); \
  } while (0)

/* True if NODE, a FIELD_DECL, is to be processed as a bitfield for
   constructor output purposes.  */
#define CONSTRUCTOR_BITFIELD_P(NODE) \
  (DECL_BIT_FIELD (FIELD_DECL_CHECK (NODE)) && DECL_MODE (NODE) != BLKmode)

/* True if NODE is a clobber right hand side, an expression of indeterminate
   value that clobbers the LHS in a copy instruction.  We use a volatile
   empty CONSTRUCTOR for this, as it matches most of the necessary semantic.
   In particular the volatile flag causes us to not prematurely remove
   such clobber instructions.  */
#define TREE_CLOBBER_P(NODE) \
  (TREE_CODE (NODE) == CONSTRUCTOR && TREE_THIS_VOLATILE (NODE))

/* A single element of a CONSTRUCTOR. VALUE holds the actual value of the
   element. INDEX can optionally design the position of VALUE: in arrays,
   it is the index where VALUE has to be placed; in structures, it is the
   FIELD_DECL of the member.  */
typedef struct GTY(()) constructor_elt_d {
  tree index;
  tree value;
} constructor_elt;


struct GTY(()) tree_constructor {
  struct tree_typed typed;
  vec<constructor_elt, va_gc> *elts;
};

/* Define fields and accessors for some nodes that represent expressions.  */

/* Nonzero if NODE is an empty statement (NOP_EXPR <0>).  */
#define IS_EMPTY_STMT(NODE)	(TREE_CODE (NODE) == NOP_EXPR \
				 && VOID_TYPE_P (TREE_TYPE (NODE)) \
				 && integer_zerop (TREE_OPERAND (NODE, 0)))

/* In ordinary expression nodes.  */
#define TREE_OPERAND_LENGTH(NODE) tree_operand_length (NODE)
#define TREE_OPERAND(NODE, I) TREE_OPERAND_CHECK (NODE, I)

/* In a tcc_vl_exp node, operand 0 is an INT_CST node holding the operand
   length.  Its value includes the length operand itself; that is,
   the minimum valid length is 1.
   Note that we have to bypass the use of TREE_OPERAND to access
   that field to avoid infinite recursion in expanding the macros.  */
#define VL_EXP_OPERAND_LENGTH(NODE) \
  ((int)TREE_INT_CST_LOW (VL_EXP_CHECK (NODE)->exp.operands[0]))

/* Nonzero if is_gimple_debug() may possibly hold.  */
#define MAY_HAVE_DEBUG_STMTS    (flag_var_tracking_assignments)

/* In a LOOP_EXPR node.  */
#define LOOP_EXPR_BODY(NODE) TREE_OPERAND_CHECK_CODE (NODE, LOOP_EXPR, 0)

/* The source location of this expression.  Non-tree_exp nodes such as
   decls and constants can be shared among multiple locations, so
   return nothing.  */
#define EXPR_LOCATION(NODE) \
  (CAN_HAVE_LOCATION_P ((NODE)) ? (NODE)->exp.locus : UNKNOWN_LOCATION)
#define SET_EXPR_LOCATION(NODE, LOCUS) EXPR_CHECK ((NODE))->exp.locus = (LOCUS)
#define EXPR_HAS_LOCATION(NODE) (LOCATION_LOCUS (EXPR_LOCATION (NODE))	\
  != UNKNOWN_LOCATION)
/* The location to be used in a diagnostic about this expression.  Do not
   use this macro if the location will be assigned to other expressions.  */
#define EXPR_LOC_OR_HERE(NODE) (EXPR_HAS_LOCATION (NODE) \
				? (NODE)->exp.locus : input_location)
#define EXPR_LOC_OR_LOC(NODE, LOCUS) (EXPR_HAS_LOCATION (NODE) \
				      ? (NODE)->exp.locus : (LOCUS))
#define EXPR_FILENAME(NODE) LOCATION_FILE (EXPR_CHECK ((NODE))->exp.locus)
#define EXPR_LINENO(NODE) LOCATION_LINE (EXPR_CHECK (NODE)->exp.locus)

/* True if a tree is an expression or statement that can have a
   location.  */
#define CAN_HAVE_LOCATION_P(NODE) ((NODE) && EXPR_P (NODE))

extern void protected_set_expr_location (tree, location_t);

/* In a TARGET_EXPR node.  */
#define TARGET_EXPR_SLOT(NODE) TREE_OPERAND_CHECK_CODE (NODE, TARGET_EXPR, 0)
#define TARGET_EXPR_INITIAL(NODE) TREE_OPERAND_CHECK_CODE (NODE, TARGET_EXPR, 1)
#define TARGET_EXPR_CLEANUP(NODE) TREE_OPERAND_CHECK_CODE (NODE, TARGET_EXPR, 2)

/* DECL_EXPR accessor. This gives access to the DECL associated with
   the given declaration statement.  */
#define DECL_EXPR_DECL(NODE)    TREE_OPERAND (DECL_EXPR_CHECK (NODE), 0)

#define EXIT_EXPR_COND(NODE)	     TREE_OPERAND (EXIT_EXPR_CHECK (NODE), 0)

/* COMPOUND_LITERAL_EXPR accessors.  */
#define COMPOUND_LITERAL_EXPR_DECL_EXPR(NODE)		\
  TREE_OPERAND (COMPOUND_LITERAL_EXPR_CHECK (NODE), 0)
#define COMPOUND_LITERAL_EXPR_DECL(NODE)			\
  DECL_EXPR_DECL (COMPOUND_LITERAL_EXPR_DECL_EXPR (NODE))

/* SWITCH_EXPR accessors. These give access to the condition, body and
   original condition type (before any compiler conversions)
   of the switch statement, respectively.  */
#define SWITCH_COND(NODE)       TREE_OPERAND (SWITCH_EXPR_CHECK (NODE), 0)
#define SWITCH_BODY(NODE)       TREE_OPERAND (SWITCH_EXPR_CHECK (NODE), 1)
#define SWITCH_LABELS(NODE)     TREE_OPERAND (SWITCH_EXPR_CHECK (NODE), 2)

/* CASE_LABEL_EXPR accessors. These give access to the high and low values
   of a case label, respectively.  */
#define CASE_LOW(NODE)          	TREE_OPERAND (CASE_LABEL_EXPR_CHECK (NODE), 0)
#define CASE_HIGH(NODE)         	TREE_OPERAND (CASE_LABEL_EXPR_CHECK (NODE), 1)
#define CASE_LABEL(NODE)		TREE_OPERAND (CASE_LABEL_EXPR_CHECK (NODE), 2)
#define CASE_CHAIN(NODE)		TREE_OPERAND (CASE_LABEL_EXPR_CHECK (NODE), 3)

/* The operands of a TARGET_MEM_REF.  Operands 0 and 1 have to match
   corresponding MEM_REF operands.  */
#define TMR_BASE(NODE) (TREE_OPERAND (TARGET_MEM_REF_CHECK (NODE), 0))
#define TMR_OFFSET(NODE) (TREE_OPERAND (TARGET_MEM_REF_CHECK (NODE), 1))
#define TMR_INDEX(NODE) (TREE_OPERAND (TARGET_MEM_REF_CHECK (NODE), 2))
#define TMR_STEP(NODE) (TREE_OPERAND (TARGET_MEM_REF_CHECK (NODE), 3))
#define TMR_INDEX2(NODE) (TREE_OPERAND (TARGET_MEM_REF_CHECK (NODE), 4))

/* The operands of a BIND_EXPR.  */
#define BIND_EXPR_VARS(NODE) (TREE_OPERAND (BIND_EXPR_CHECK (NODE), 0))
#define BIND_EXPR_BODY(NODE) (TREE_OPERAND (BIND_EXPR_CHECK (NODE), 1))
#define BIND_EXPR_BLOCK(NODE) (TREE_OPERAND (BIND_EXPR_CHECK (NODE), 2))

/* GOTO_EXPR accessor. This gives access to the label associated with
   a goto statement.  */
#define GOTO_DESTINATION(NODE)  TREE_OPERAND ((NODE), 0)

/* ASM_EXPR accessors. ASM_STRING returns a STRING_CST for the
   instruction (e.g., "mov x, y"). ASM_OUTPUTS, ASM_INPUTS, and
   ASM_CLOBBERS represent the outputs, inputs, and clobbers for the
   statement.  */
#define ASM_STRING(NODE)        TREE_OPERAND (ASM_EXPR_CHECK (NODE), 0)
#define ASM_OUTPUTS(NODE)       TREE_OPERAND (ASM_EXPR_CHECK (NODE), 1)
#define ASM_INPUTS(NODE)        TREE_OPERAND (ASM_EXPR_CHECK (NODE), 2)
#define ASM_CLOBBERS(NODE)      TREE_OPERAND (ASM_EXPR_CHECK (NODE), 3)
#define ASM_LABELS(NODE)	TREE_OPERAND (ASM_EXPR_CHECK (NODE), 4)
/* Nonzero if we want to create an ASM_INPUT instead of an
   ASM_OPERAND with no operands.  */
#define ASM_INPUT_P(NODE) (ASM_EXPR_CHECK (NODE)->base.static_flag)
#define ASM_VOLATILE_P(NODE) (ASM_EXPR_CHECK (NODE)->base.public_flag)

/* COND_EXPR accessors.  */
#define COND_EXPR_COND(NODE)	(TREE_OPERAND (COND_EXPR_CHECK (NODE), 0))
#define COND_EXPR_THEN(NODE)	(TREE_OPERAND (COND_EXPR_CHECK (NODE), 1))
#define COND_EXPR_ELSE(NODE)	(TREE_OPERAND (COND_EXPR_CHECK (NODE), 2))

/* Accessors for the chains of recurrences.  */
#define CHREC_VAR(NODE)           TREE_OPERAND (POLYNOMIAL_CHREC_CHECK (NODE), 0)
#define CHREC_LEFT(NODE)          TREE_OPERAND (POLYNOMIAL_CHREC_CHECK (NODE), 1)
#define CHREC_RIGHT(NODE)         TREE_OPERAND (POLYNOMIAL_CHREC_CHECK (NODE), 2)
#define CHREC_VARIABLE(NODE)      TREE_INT_CST_LOW (CHREC_VAR (NODE))

/* LABEL_EXPR accessor. This gives access to the label associated with
   the given label expression.  */
#define LABEL_EXPR_LABEL(NODE)  TREE_OPERAND (LABEL_EXPR_CHECK (NODE), 0)

/* VDEF_EXPR accessors are specified in tree-flow.h, along with the other
   accessors for SSA operands.  */

/* CATCH_EXPR accessors.  */
#define CATCH_TYPES(NODE)	TREE_OPERAND (CATCH_EXPR_CHECK (NODE), 0)
#define CATCH_BODY(NODE)	TREE_OPERAND (CATCH_EXPR_CHECK (NODE), 1)

/* EH_FILTER_EXPR accessors.  */
#define EH_FILTER_TYPES(NODE)	TREE_OPERAND (EH_FILTER_EXPR_CHECK (NODE), 0)
#define EH_FILTER_FAILURE(NODE)	TREE_OPERAND (EH_FILTER_EXPR_CHECK (NODE), 1)

/* OBJ_TYPE_REF accessors.  */
#define OBJ_TYPE_REF_EXPR(NODE)	  TREE_OPERAND (OBJ_TYPE_REF_CHECK (NODE), 0)
#define OBJ_TYPE_REF_OBJECT(NODE) TREE_OPERAND (OBJ_TYPE_REF_CHECK (NODE), 1)
#define OBJ_TYPE_REF_TOKEN(NODE)  TREE_OPERAND (OBJ_TYPE_REF_CHECK (NODE), 2)

/* ASSERT_EXPR accessors.  */
#define ASSERT_EXPR_VAR(NODE)	TREE_OPERAND (ASSERT_EXPR_CHECK (NODE), 0)
#define ASSERT_EXPR_COND(NODE)	TREE_OPERAND (ASSERT_EXPR_CHECK (NODE), 1)

/* CALL_EXPR accessors.
 */
#define CALL_EXPR_FN(NODE) TREE_OPERAND (CALL_EXPR_CHECK (NODE), 1)
#define CALL_EXPR_STATIC_CHAIN(NODE) TREE_OPERAND (CALL_EXPR_CHECK (NODE), 2)
#define CALL_EXPR_ARG(NODE, I) TREE_OPERAND (CALL_EXPR_CHECK (NODE), (I) + 3)
#define call_expr_nargs(NODE) (VL_EXP_OPERAND_LENGTH(NODE) - 3)

/* CALL_EXPR_ARGP returns a pointer to the argument vector for NODE.
   We can't use &CALL_EXPR_ARG (NODE, 0) because that will complain if
   the argument count is zero when checking is enabled.  Instead, do
   the pointer arithmetic to advance past the 3 fixed operands in a
   CALL_EXPR.  That produces a valid pointer to just past the end of the
   operand array, even if it's not valid to dereference it.  */
#define CALL_EXPR_ARGP(NODE) \
  (&(TREE_OPERAND (CALL_EXPR_CHECK (NODE), 0)) + 3)

/* TM directives and accessors.  */
#define TRANSACTION_EXPR_BODY(NODE) \
  TREE_OPERAND (TRANSACTION_EXPR_CHECK (NODE), 0)
#define TRANSACTION_EXPR_OUTER(NODE) \
  (TRANSACTION_EXPR_CHECK (NODE)->base.static_flag)
#define TRANSACTION_EXPR_RELAXED(NODE) \
  (TRANSACTION_EXPR_CHECK (NODE)->base.public_flag)

/* OpenMP directive and clause accessors.  */

#define OMP_BODY(NODE) \
  TREE_OPERAND (TREE_RANGE_CHECK (NODE, OMP_PARALLEL, OMP_CRITICAL), 0)
#define OMP_CLAUSES(NODE) \
  TREE_OPERAND (TREE_RANGE_CHECK (NODE, OMP_PARALLEL, OMP_SINGLE), 1)

#define OMP_PARALLEL_BODY(NODE)    TREE_OPERAND (OMP_PARALLEL_CHECK (NODE), 0)
#define OMP_PARALLEL_CLAUSES(NODE) TREE_OPERAND (OMP_PARALLEL_CHECK (NODE), 1)

#define OMP_TASK_BODY(NODE)	   TREE_OPERAND (OMP_TASK_CHECK (NODE), 0)
#define OMP_TASK_CLAUSES(NODE)	   TREE_OPERAND (OMP_TASK_CHECK (NODE), 1)

#define OMP_TASKREG_CHECK(NODE)	  TREE_RANGE_CHECK (NODE, OMP_PARALLEL, OMP_TASK)
#define OMP_TASKREG_BODY(NODE)    TREE_OPERAND (OMP_TASKREG_CHECK (NODE), 0)
#define OMP_TASKREG_CLAUSES(NODE) TREE_OPERAND (OMP_TASKREG_CHECK (NODE), 1)

#define OMP_FOR_BODY(NODE)	   TREE_OPERAND (OMP_FOR_CHECK (NODE), 0)
#define OMP_FOR_CLAUSES(NODE)	   TREE_OPERAND (OMP_FOR_CHECK (NODE), 1)
#define OMP_FOR_INIT(NODE)	   TREE_OPERAND (OMP_FOR_CHECK (NODE), 2)
#define OMP_FOR_COND(NODE)	   TREE_OPERAND (OMP_FOR_CHECK (NODE), 3)
#define OMP_FOR_INCR(NODE)	   TREE_OPERAND (OMP_FOR_CHECK (NODE), 4)
#define OMP_FOR_PRE_BODY(NODE)	   TREE_OPERAND (OMP_FOR_CHECK (NODE), 5)

#define OMP_SECTIONS_BODY(NODE)    TREE_OPERAND (OMP_SECTIONS_CHECK (NODE), 0)
#define OMP_SECTIONS_CLAUSES(NODE) TREE_OPERAND (OMP_SECTIONS_CHECK (NODE), 1)

#define OMP_SECTION_BODY(NODE)	   TREE_OPERAND (OMP_SECTION_CHECK (NODE), 0)

#define OMP_SINGLE_BODY(NODE)	   TREE_OPERAND (OMP_SINGLE_CHECK (NODE), 0)
#define OMP_SINGLE_CLAUSES(NODE)   TREE_OPERAND (OMP_SINGLE_CHECK (NODE), 1)

#define OMP_MASTER_BODY(NODE)	   TREE_OPERAND (OMP_MASTER_CHECK (NODE), 0)

#define OMP_ORDERED_BODY(NODE)	   TREE_OPERAND (OMP_ORDERED_CHECK (NODE), 0)

#define OMP_CRITICAL_BODY(NODE)    TREE_OPERAND (OMP_CRITICAL_CHECK (NODE), 0)
#define OMP_CRITICAL_NAME(NODE)    TREE_OPERAND (OMP_CRITICAL_CHECK (NODE), 1)

#define OMP_CLAUSE_CHAIN(NODE)     TREE_CHAIN (OMP_CLAUSE_CHECK (NODE))
#define OMP_CLAUSE_DECL(NODE)      					\
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_RANGE_CHECK (OMP_CLAUSE_CHECK (NODE),	\
					      OMP_CLAUSE_PRIVATE,	\
	                                      OMP_CLAUSE_COPYPRIVATE), 0)
#define OMP_CLAUSE_HAS_LOCATION(NODE) \
  (LOCATION_LOCUS ((OMP_CLAUSE_CHECK (NODE))->omp_clause.locus)		\
  != UNKNOWN_LOCATION)
#define OMP_CLAUSE_LOCATION(NODE)  (OMP_CLAUSE_CHECK (NODE))->omp_clause.locus

/* True on an OMP_SECTION statement that was the last lexical member.
   This status is meaningful in the implementation of lastprivate.  */
#define OMP_SECTION_LAST(NODE) \
  (OMP_SECTION_CHECK (NODE)->base.private_flag)

/* True on an OMP_PARALLEL statement if it represents an explicit
   combined parallel work-sharing constructs.  */
#define OMP_PARALLEL_COMBINED(NODE) \
  (OMP_PARALLEL_CHECK (NODE)->base.private_flag)

/* True on a PRIVATE clause if its decl is kept around for debugging
   information only and its DECL_VALUE_EXPR is supposed to point
   to what it has been remapped to.  */
#define OMP_CLAUSE_PRIVATE_DEBUG(NODE) \
  (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_PRIVATE)->base.public_flag)

/* True on a PRIVATE clause if ctor needs access to outer region's
   variable.  */
#define OMP_CLAUSE_PRIVATE_OUTER_REF(NODE) \
  TREE_PRIVATE (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_PRIVATE))

/* True on a LASTPRIVATE clause if a FIRSTPRIVATE clause for the same
   decl is present in the chain.  */
#define OMP_CLAUSE_LASTPRIVATE_FIRSTPRIVATE(NODE) \
  (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_LASTPRIVATE)->base.public_flag)
#define OMP_CLAUSE_LASTPRIVATE_STMT(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE,			\
						OMP_CLAUSE_LASTPRIVATE),\
		      1)
#define OMP_CLAUSE_LASTPRIVATE_GIMPLE_SEQ(NODE) \
  (OMP_CLAUSE_CHECK (NODE))->omp_clause.gimple_reduction_init

#define OMP_CLAUSE_FINAL_EXPR(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_FINAL), 0)
#define OMP_CLAUSE_IF_EXPR(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_IF), 0)
#define OMP_CLAUSE_NUM_THREADS_EXPR(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_NUM_THREADS),0)
#define OMP_CLAUSE_SCHEDULE_CHUNK_EXPR(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_SCHEDULE), 0)

#define OMP_CLAUSE_COLLAPSE_EXPR(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_COLLAPSE), 0)
#define OMP_CLAUSE_COLLAPSE_ITERVAR(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_COLLAPSE), 1)
#define OMP_CLAUSE_COLLAPSE_COUNT(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_COLLAPSE), 2)

#define OMP_CLAUSE_REDUCTION_CODE(NODE)	\
  (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_REDUCTION)->omp_clause.subcode.reduction_code)
#define OMP_CLAUSE_REDUCTION_INIT(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_REDUCTION), 1)
#define OMP_CLAUSE_REDUCTION_MERGE(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_REDUCTION), 2)
#define OMP_CLAUSE_REDUCTION_GIMPLE_INIT(NODE) \
  (OMP_CLAUSE_CHECK (NODE))->omp_clause.gimple_reduction_init
#define OMP_CLAUSE_REDUCTION_GIMPLE_MERGE(NODE) \
  (OMP_CLAUSE_CHECK (NODE))->omp_clause.gimple_reduction_merge
#define OMP_CLAUSE_REDUCTION_PLACEHOLDER(NODE) \
  OMP_CLAUSE_OPERAND (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_REDUCTION), 3)

enum omp_clause_schedule_kind
{
  OMP_CLAUSE_SCHEDULE_STATIC,
  OMP_CLAUSE_SCHEDULE_DYNAMIC,
  OMP_CLAUSE_SCHEDULE_GUIDED,
  OMP_CLAUSE_SCHEDULE_AUTO,
  OMP_CLAUSE_SCHEDULE_RUNTIME
};

#define OMP_CLAUSE_SCHEDULE_KIND(NODE) \
  (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_SCHEDULE)->omp_clause.subcode.schedule_kind)

enum omp_clause_default_kind
{
  OMP_CLAUSE_DEFAULT_UNSPECIFIED,
  OMP_CLAUSE_DEFAULT_SHARED,
  OMP_CLAUSE_DEFAULT_NONE,
  OMP_CLAUSE_DEFAULT_PRIVATE,
  OMP_CLAUSE_DEFAULT_FIRSTPRIVATE
};

#define OMP_CLAUSE_DEFAULT_KIND(NODE) \
  (OMP_CLAUSE_SUBCODE_CHECK (NODE, OMP_CLAUSE_DEFAULT)->omp_clause.subcode.default_kind)

struct GTY(()) tree_exp {
  struct tree_typed typed;
  location_t locus;
  tree GTY ((special ("tree_exp"),
	     desc ("TREE_CODE ((tree) &%0)")))
    operands[1];
};

/* SSA_NAME accessors.  */

/* Returns the IDENTIFIER_NODE giving the SSA name a name or NULL_TREE
   if there is no name associated with it.  */
#define SSA_NAME_IDENTIFIER(NODE)				\
  (SSA_NAME_CHECK (NODE)->ssa_name.var != NULL_TREE		\
   ? (TREE_CODE ((NODE)->ssa_name.var) == IDENTIFIER_NODE	\
      ? (NODE)->ssa_name.var					\
      : DECL_NAME ((NODE)->ssa_name.var))			\
   : NULL_TREE)

/* Returns the variable being referenced.  This can be NULL_TREE for
   temporaries not associated with any user variable.
   Once released, this is the only field that can be relied upon.  */
#define SSA_NAME_VAR(NODE)					\
  (SSA_NAME_CHECK (NODE)->ssa_name.var == NULL_TREE		\
   || TREE_CODE ((NODE)->ssa_name.var) == IDENTIFIER_NODE	\
   ? NULL_TREE : (NODE)->ssa_name.var)

#define SET_SSA_NAME_VAR_OR_IDENTIFIER(NODE,VAR) \
  do { SSA_NAME_CHECK (NODE)->ssa_name.var = (VAR); } while (0)

/* Returns the statement which defines this SSA name.  */
#define SSA_NAME_DEF_STMT(NODE)	SSA_NAME_CHECK (NODE)->ssa_name.def_stmt

/* Returns the SSA version number of this SSA name.  Note that in
   tree SSA, version numbers are not per variable and may be recycled.  */
#define SSA_NAME_VERSION(NODE)	SSA_NAME_CHECK (NODE)->base.u.version

/* Nonzero if this SSA name occurs in an abnormal PHI.  SSA_NAMES are
   never output, so we can safely use the ASM_WRITTEN_FLAG for this
   status bit.  */
#define SSA_NAME_OCCURS_IN_ABNORMAL_PHI(NODE) \
    SSA_NAME_CHECK (NODE)->base.asm_written_flag

/* Nonzero if this SSA_NAME expression is currently on the free list of
   SSA_NAMES.  Using NOTHROW_FLAG seems reasonably safe since throwing
   has no meaning for an SSA_NAME.  */
#define SSA_NAME_IN_FREE_LIST(NODE) \
    SSA_NAME_CHECK (NODE)->base.nothrow_flag

/* Nonzero if this SSA_NAME is the default definition for the
   underlying symbol.  A default SSA name is created for symbol S if
   the very first reference to S in the function is a read operation.
   Default definitions are always created by an empty statement and
   belong to no basic block.  */
#define SSA_NAME_IS_DEFAULT_DEF(NODE) \
    SSA_NAME_CHECK (NODE)->base.default_def_flag

/* Attributes for SSA_NAMEs for pointer-type variables.  */
#define SSA_NAME_PTR_INFO(N) \
    SSA_NAME_CHECK (N)->ssa_name.ptr_info

/* Defined in tree-flow.h.  */
struct ptr_info_def;

/* Immediate use linking structure.  This structure is used for maintaining
   a doubly linked list of uses of an SSA_NAME.  */
typedef struct GTY(()) ssa_use_operand_d {
  struct ssa_use_operand_d* GTY((skip(""))) prev;
  struct ssa_use_operand_d* GTY((skip(""))) next;
  /* Immediate uses for a given SSA name are maintained as a cyclic
     list.  To recognize the root of this list, the location field
     needs to point to the original SSA name.  Since statements and
     SSA names are of different data types, we need this union.  See
     the explanation in struct immediate_use_iterator_d.  */
  union { gimple stmt; tree ssa_name; } GTY((skip(""))) loc;
  tree *GTY((skip(""))) use;
} ssa_use_operand_t;

/* Return the immediate_use information for an SSA_NAME. */
#define SSA_NAME_IMM_USE_NODE(NODE) SSA_NAME_CHECK (NODE)->ssa_name.imm_uses

struct GTY(()) tree_ssa_name {
  struct tree_typed typed;

  /* _DECL wrapped by this SSA name.  */
  tree var;

  /* Statement that defines this SSA name.  */
  gimple def_stmt;

  /* Pointer attributes used for alias analysis.  */
  struct ptr_info_def *ptr_info;

  /* Immediate uses list for this SSA_NAME.  */
  struct ssa_use_operand_d imm_uses;
};

struct GTY(()) phi_arg_d {
  /* imm_use MUST be the first element in struct because we do some
     pointer arithmetic with it.  See phi_arg_index_from_use.  */
  struct ssa_use_operand_d imm_use;
  tree def;
  location_t locus;
};


#define OMP_CLAUSE_CODE(NODE)					\
	(OMP_CLAUSE_CHECK (NODE))->omp_clause.code

#define OMP_CLAUSE_SET_CODE(NODE, CODE)				\
	((OMP_CLAUSE_CHECK (NODE))->omp_clause.code = (CODE))

#define OMP_CLAUSE_CODE(NODE)					\
	(OMP_CLAUSE_CHECK (NODE))->omp_clause.code

#define OMP_CLAUSE_OPERAND(NODE, I)				\
	OMP_CLAUSE_ELT_CHECK (NODE, I)

struct GTY(()) tree_omp_clause {
  struct tree_common common;
  location_t locus;
  enum omp_clause_code code;
  union omp_clause_subcode {
    enum omp_clause_default_kind  default_kind;
    enum omp_clause_schedule_kind schedule_kind;
    enum tree_code                reduction_code;
  } GTY ((skip)) subcode;

  /* The gimplification of OMP_CLAUSE_REDUCTION_{INIT,MERGE} for omp-low's
     usage.  */
  gimple_seq gimple_reduction_init;
  gimple_seq gimple_reduction_merge;

  tree GTY ((length ("omp_clause_num_ops[OMP_CLAUSE_CODE ((tree)&%h)]"))) ops[1];
};


/* In a BLOCK node.  */
#define BLOCK_VARS(NODE) (BLOCK_CHECK (NODE)->block.vars)
#define BLOCK_NONLOCALIZED_VARS(NODE) \
  (BLOCK_CHECK (NODE)->block.nonlocalized_vars)
#define BLOCK_NUM_NONLOCALIZED_VARS(NODE) \
  vec_safe_length (BLOCK_NONLOCALIZED_VARS (NODE))
#define BLOCK_NONLOCALIZED_VAR(NODE,N) (*BLOCK_NONLOCALIZED_VARS (NODE))[N]
#define BLOCK_SUBBLOCKS(NODE) (BLOCK_CHECK (NODE)->block.subblocks)
#define BLOCK_SUPERCONTEXT(NODE) (BLOCK_CHECK (NODE)->block.supercontext)
#define BLOCK_CHAIN(NODE) (BLOCK_CHECK (NODE)->block.chain)
#define BLOCK_ABSTRACT_ORIGIN(NODE) (BLOCK_CHECK (NODE)->block.abstract_origin)
#define BLOCK_ABSTRACT(NODE) (BLOCK_CHECK (NODE)->block.abstract_flag)

/* True if BLOCK has the same ranges as its BLOCK_SUPERCONTEXT.  */
#define BLOCK_SAME_RANGE(NODE) (BLOCK_CHECK (NODE)->base.u.bits.nameless_flag)

/* An index number for this block.  These values are not guaranteed to
   be unique across functions -- whether or not they are depends on
   the debugging output format in use.  */
#define BLOCK_NUMBER(NODE) (BLOCK_CHECK (NODE)->block.block_num)

/* If block reordering splits a lexical block into discontiguous
   address ranges, we'll make a copy of the original block.

   Note that this is logically distinct from BLOCK_ABSTRACT_ORIGIN.
   In that case, we have one source block that has been replicated
   (through inlining or unrolling) into many logical blocks, and that
   these logical blocks have different physical variables in them.

   In this case, we have one logical block split into several
   non-contiguous address ranges.  Most debug formats can't actually
   represent this idea directly, so we fake it by creating multiple
   logical blocks with the same variables in them.  However, for those
   that do support non-contiguous regions, these allow the original
   logical block to be reconstructed, along with the set of address
   ranges.

   One of the logical block fragments is arbitrarily chosen to be
   the ORIGIN.  The other fragments will point to the origin via
   BLOCK_FRAGMENT_ORIGIN; the origin itself will have this pointer
   be null.  The list of fragments will be chained through
   BLOCK_FRAGMENT_CHAIN from the origin.  */

#define BLOCK_FRAGMENT_ORIGIN(NODE) (BLOCK_CHECK (NODE)->block.fragment_origin)
#define BLOCK_FRAGMENT_CHAIN(NODE) (BLOCK_CHECK (NODE)->block.fragment_chain)

/* For an inlined function, this gives the location where it was called
   from.  This is only set in the top level block, which corresponds to the
   inlined function scope.  This is used in the debug output routines.  */

#define BLOCK_SOURCE_LOCATION(NODE) (BLOCK_CHECK (NODE)->block.locus)

struct GTY(()) tree_block {
  struct tree_base base;
  tree chain;

  unsigned abstract_flag : 1;
  unsigned block_num : 31;

  location_t locus;

  tree vars;
  vec<tree, va_gc> *nonlocalized_vars;

  tree subblocks;
  tree supercontext;
  tree abstract_origin;
  tree fragment_origin;
  tree fragment_chain;
};

/* Define fields and accessors for nodes representing data types.  */

/* See tree.def for documentation of the use of these fields.
   Look at the documentation of the various ..._TYPE tree codes.

   Note that the type.values, type.minval, and type.maxval fields are
   overloaded and used for different macros in different kinds of types.
   Each macro must check to ensure the tree node is of the proper kind of
   type.  Note also that some of the front-ends also overload these fields,
   so they must be checked as well.  */

#define TYPE_UID(NODE) (TYPE_CHECK (NODE)->type_common.uid)
#define TYPE_SIZE(NODE) (TYPE_CHECK (NODE)->type_common.size)
#define TYPE_SIZE_UNIT(NODE) (TYPE_CHECK (NODE)->type_common.size_unit)
#define TYPE_POINTER_TO(NODE) (TYPE_CHECK (NODE)->type_common.pointer_to)
#define TYPE_REFERENCE_TO(NODE) (TYPE_CHECK (NODE)->type_common.reference_to)
#define TYPE_PRECISION(NODE) (TYPE_CHECK (NODE)->type_common.precision)
#define TYPE_NAME(NODE) (TYPE_CHECK (NODE)->type_common.name)
#define TYPE_NEXT_VARIANT(NODE) (TYPE_CHECK (NODE)->type_common.next_variant)
#define TYPE_MAIN_VARIANT(NODE) (TYPE_CHECK (NODE)->type_common.main_variant)
#define TYPE_CONTEXT(NODE) (TYPE_CHECK (NODE)->type_common.context)

/* Vector types need to check target flags to determine type.  */
extern enum machine_mode vector_type_mode (const_tree);
#define TYPE_MODE(NODE) \
  (TREE_CODE (TYPE_CHECK (NODE)) == VECTOR_TYPE \
   ? vector_type_mode (NODE) : (NODE)->type_common.mode)
#define SET_TYPE_MODE(NODE, MODE) \
  (TYPE_CHECK (NODE)->type_common.mode = (MODE))

/* The "canonical" type for this type node, which is used by frontends to
   compare the type for equality with another type.  If two types are
   equal (based on the semantics of the language), then they will have
   equivalent TYPE_CANONICAL entries.

   As a special case, if TYPE_CANONICAL is NULL_TREE, and thus
   TYPE_STRUCTURAL_EQUALITY_P is true, then it cannot
   be used for comparison against other types.  Instead, the type is
   said to require structural equality checks, described in
   TYPE_STRUCTURAL_EQUALITY_P.

   For unqualified aggregate and function types the middle-end relies on
   TYPE_CANONICAL to tell whether two variables can be assigned
   to each other without a conversion.  The middle-end also makes sure
   to assign the same alias-sets to the type partition with equal
   TYPE_CANONICAL of their unqualified variants.  */
#define TYPE_CANONICAL(NODE) (TYPE_CHECK (NODE)->type_common.canonical)
/* Indicates that the type node requires structural equality
   checks.  The compiler will need to look at the composition of the
   type to determine whether it is equal to another type, rather than
   just comparing canonical type pointers.  For instance, we would need
   to look at the return and parameter types of a FUNCTION_TYPE
   node.  */
#define TYPE_STRUCTURAL_EQUALITY_P(NODE) (TYPE_CANONICAL (NODE) == NULL_TREE)
/* Sets the TYPE_CANONICAL field to NULL_TREE, indicating that the
   type node requires structural equality.  */
#define SET_TYPE_STRUCTURAL_EQUALITY(NODE) (TYPE_CANONICAL (NODE) = NULL_TREE)

#define TYPE_IBIT(NODE) (GET_MODE_IBIT (TYPE_MODE (NODE)))
#define TYPE_FBIT(NODE) (GET_MODE_FBIT (TYPE_MODE (NODE)))

/* The (language-specific) typed-based alias set for this type.
   Objects whose TYPE_ALIAS_SETs are different cannot alias each
   other.  If the TYPE_ALIAS_SET is -1, no alias set has yet been
   assigned to this type.  If the TYPE_ALIAS_SET is 0, objects of this
   type can alias objects of any type.  */
#define TYPE_ALIAS_SET(NODE) (TYPE_CHECK (NODE)->type_common.alias_set)

/* Nonzero iff the typed-based alias set for this type has been
   calculated.  */
#define TYPE_ALIAS_SET_KNOWN_P(NODE) \
  (TYPE_CHECK (NODE)->type_common.alias_set != -1)

/* A TREE_LIST of IDENTIFIER nodes of the attributes that apply
   to this type.  */
#define TYPE_ATTRIBUTES(NODE) (TYPE_CHECK (NODE)->type_common.attributes)

/* The alignment necessary for objects of this type.
   The value is an int, measured in bits.  */
#define TYPE_ALIGN(NODE) (TYPE_CHECK (NODE)->type_common.align)

/* 1 if the alignment for this type was requested by "aligned" attribute,
   0 if it is the default for this type.  */
#define TYPE_USER_ALIGN(NODE) (TYPE_CHECK (NODE)->base.u.bits.user_align)

/* The alignment for NODE, in bytes.  */
#define TYPE_ALIGN_UNIT(NODE) (TYPE_ALIGN (NODE) / BITS_PER_UNIT)

/* If your language allows you to declare types, and you want debug info
   for them, then you need to generate corresponding TYPE_DECL nodes.
   These "stub" TYPE_DECL nodes have no name, and simply point at the
   type node.  You then set the TYPE_STUB_DECL field of the type node
   to point back at the TYPE_DECL node.  This allows the debug routines
   to know that the two nodes represent the same type, so that we only
   get one debug info record for them.  */
#define TYPE_STUB_DECL(NODE) (TREE_CHAIN (TYPE_CHECK (NODE)))

/* In a RECORD_TYPE, UNION_TYPE, QUAL_UNION_TYPE or ARRAY_TYPE, it means
   the type has BLKmode only because it lacks the alignment required for
   its size.  */
#define TYPE_NO_FORCE_BLK(NODE) \
  (TYPE_CHECK (NODE)->type_common.no_force_blk_flag)

/* Nonzero in a type considered volatile as a whole.  */
#define TYPE_VOLATILE(NODE) (TYPE_CHECK (NODE)->base.volatile_flag)

/* Means this type is const-qualified.  */
#define TYPE_READONLY(NODE) (TYPE_CHECK (NODE)->base.readonly_flag)

/* If nonzero, this type is `restrict'-qualified, in the C sense of
   the term.  */
#define TYPE_RESTRICT(NODE) (TYPE_CHECK (NODE)->type_common.restrict_flag)

/* If nonzero, type's name shouldn't be emitted into debug info.  */
#define TYPE_NAMELESS(NODE) (TYPE_CHECK (NODE)->base.u.bits.nameless_flag)

/* The address space the type is in.  */
#define TYPE_ADDR_SPACE(NODE) (TYPE_CHECK (NODE)->base.u.bits.address_space)

/* There is a TYPE_QUAL value for each type qualifier.  They can be
   combined by bitwise-or to form the complete set of qualifiers for a
   type.  */
enum cv_qualifier
  {
    TYPE_UNQUALIFIED   = 0x0,
    TYPE_QUAL_CONST    = 0x1,
    TYPE_QUAL_VOLATILE = 0x2,
    TYPE_QUAL_RESTRICT = 0x4
  };

/* Encode/decode the named memory support as part of the qualifier.  If more
   than 8 qualifiers are added, these macros need to be adjusted.  */
#define ENCODE_QUAL_ADDR_SPACE(NUM) ((NUM & 0xFF) << 8)
#define DECODE_QUAL_ADDR_SPACE(X) (((X) >> 8) & 0xFF)

/* Return all qualifiers except for the address space qualifiers.  */
#define CLEAR_QUAL_ADDR_SPACE(X) ((X) & ~0xFF00)

/* Only keep the address space out of the qualifiers and discard the other
   qualifiers.  */
#define KEEP_QUAL_ADDR_SPACE(X) ((X) & 0xFF00)

/* The set of type qualifiers for this type.  */
#define TYPE_QUALS(NODE)					\
  ((int) ((TYPE_READONLY (NODE) * TYPE_QUAL_CONST)		\
	  | (TYPE_VOLATILE (NODE) * TYPE_QUAL_VOLATILE)		\
	  | (TYPE_RESTRICT (NODE) * TYPE_QUAL_RESTRICT)		\
	  | (ENCODE_QUAL_ADDR_SPACE (TYPE_ADDR_SPACE (NODE)))))

/* The same as TYPE_QUALS without the address space qualifications.  */
#define TYPE_QUALS_NO_ADDR_SPACE(NODE)				\
  ((int) ((TYPE_READONLY (NODE) * TYPE_QUAL_CONST)		\
	  | (TYPE_VOLATILE (NODE) * TYPE_QUAL_VOLATILE)		\
	  | (TYPE_RESTRICT (NODE) * TYPE_QUAL_RESTRICT)))

/* These flags are available for each language front end to use internally.  */
#define TYPE_LANG_FLAG_0(NODE) (TYPE_CHECK (NODE)->type_common.lang_flag_0)
#define TYPE_LANG_FLAG_1(NODE) (TYPE_CHECK (NODE)->type_common.lang_flag_1)
#define TYPE_LANG_FLAG_2(NODE) (TYPE_CHECK (NODE)->type_common.lang_flag_2)
#define TYPE_LANG_FLAG_3(NODE) (TYPE_CHECK (NODE)->type_common.lang_flag_3)
#define TYPE_LANG_FLAG_4(NODE) (TYPE_CHECK (NODE)->type_common.lang_flag_4)
#define TYPE_LANG_FLAG_5(NODE) (TYPE_CHECK (NODE)->type_common.lang_flag_5)
#define TYPE_LANG_FLAG_6(NODE) (TYPE_CHECK (NODE)->type_common.lang_flag_6)

/* Used to keep track of visited nodes in tree traversals.  This is set to
   0 by copy_node and make_node.  */
#define TREE_VISITED(NODE) ((NODE)->base.visited)

/* If set in an ARRAY_TYPE, indicates a string type (for languages
   that distinguish string from array of char).
   If set in a INTEGER_TYPE, indicates a character type.  */
#define TYPE_STRING_FLAG(NODE) (TYPE_CHECK (NODE)->type_common.string_flag)

/* For a VECTOR_TYPE, this is the number of sub-parts of the vector.  */
#define TYPE_VECTOR_SUBPARTS(VECTOR_TYPE) \
  (((unsigned HOST_WIDE_INT) 1) \
   << VECTOR_TYPE_CHECK (VECTOR_TYPE)->type_common.precision)

/* Set precision to n when we have 2^n sub-parts of the vector.  */
#define SET_TYPE_VECTOR_SUBPARTS(VECTOR_TYPE, X) \
  (VECTOR_TYPE_CHECK (VECTOR_TYPE)->type_common.precision = exact_log2 (X))

/* Nonzero in a VECTOR_TYPE if the frontends should not emit warnings
   about missing conversions to other vector types of the same size.  */
#define TYPE_VECTOR_OPAQUE(NODE) \
  (VECTOR_TYPE_CHECK (NODE)->base.default_def_flag)

/* Indicates that objects of this type must be initialized by calling a
   function when they are created.  */
#define TYPE_NEEDS_CONSTRUCTING(NODE) \
  (TYPE_CHECK (NODE)->type_common.needs_constructing_flag)

/* Indicates that a UNION_TYPE object should be passed the same way that
   the first union alternative would be passed, or that a RECORD_TYPE
   object should be passed the same way that the first (and only) member
   would be passed.  */
#define TYPE_TRANSPARENT_AGGR(NODE) \
  (RECORD_OR_UNION_CHECK (NODE)->type_common.transparent_aggr_flag)

/* For an ARRAY_TYPE, indicates that it is not permitted to take the
   address of a component of the type.  This is the counterpart of
   DECL_NONADDRESSABLE_P for arrays, see the definition of this flag.  */
#define TYPE_NONALIASED_COMPONENT(NODE) \
  (ARRAY_TYPE_CHECK (NODE)->type_common.transparent_aggr_flag)

/* Indicated that objects of this type should be laid out in as
   compact a way as possible.  */
#define TYPE_PACKED(NODE) (TYPE_CHECK (NODE)->base.u.bits.packed_flag)

/* Used by type_contains_placeholder_p to avoid recomputation.
   Values are: 0 (unknown), 1 (false), 2 (true).  Never access
   this field directly.  */
#define TYPE_CONTAINS_PLACEHOLDER_INTERNAL(NODE) \
  (TYPE_CHECK (NODE)->type_common.contains_placeholder_bits)

/* The debug output functions use the symtab union field to store
   information specific to the debugging format.  The different debug
   output hooks store different types in the union field.  These three
   macros are used to access different fields in the union.  The debug
   hooks are responsible for consistently using only a specific
   macro.  */

/* Symtab field as an integer.  Used by stabs generator in dbxout.c to
   hold the type's number in the generated stabs.  */
#define TYPE_SYMTAB_ADDRESS(NODE) \
  (TYPE_CHECK (NODE)->type_common.symtab.address)

/* Symtab field as a string.  Used by COFF generator in sdbout.c to
   hold struct/union type tag names.  */
#define TYPE_SYMTAB_POINTER(NODE) \
  (TYPE_CHECK (NODE)->type_common.symtab.pointer)

/* Symtab field as a pointer to a DWARF DIE.  Used by DWARF generator
   in dwarf2out.c to point to the DIE generated for the type.  */
#define TYPE_SYMTAB_DIE(NODE) \
  (TYPE_CHECK (NODE)->type_common.symtab.die)

/* The garbage collector needs to know the interpretation of the
   symtab field.  These constants represent the different types in the
   union.  */

#define TYPE_SYMTAB_IS_ADDRESS (0)
#define TYPE_SYMTAB_IS_POINTER (1)
#define TYPE_SYMTAB_IS_DIE (2)

struct die_struct;

struct GTY(()) tree_type_common {
  struct tree_common common;
  tree size;
  tree size_unit;
  tree attributes;
  unsigned int uid;

  unsigned int precision : 10;
  unsigned no_force_blk_flag : 1;
  unsigned needs_constructing_flag : 1;
  unsigned transparent_aggr_flag : 1;
  unsigned restrict_flag : 1;
  unsigned contains_placeholder_bits : 2;

  ENUM_BITFIELD(machine_mode) mode : 8;

  unsigned string_flag : 1;
  unsigned lang_flag_0 : 1;
  unsigned lang_flag_1 : 1;
  unsigned lang_flag_2 : 1;
  unsigned lang_flag_3 : 1;
  unsigned lang_flag_4 : 1;
  unsigned lang_flag_5 : 1;
  unsigned lang_flag_6 : 1;

  unsigned int align;
  alias_set_type alias_set;
  tree pointer_to;
  tree reference_to;
  union tree_type_symtab {
    int GTY ((tag ("TYPE_SYMTAB_IS_ADDRESS"))) address;
    const char * GTY ((tag ("TYPE_SYMTAB_IS_POINTER"))) pointer;
    struct die_struct * GTY ((tag ("TYPE_SYMTAB_IS_DIE"))) die;
  } GTY ((desc ("debug_hooks->tree_type_symtab_field"))) symtab;
  tree name;
  tree next_variant;
  tree main_variant;
  tree context;
  tree canonical;
};

#define TYPE_LANG_SPECIFIC(NODE) \
  (TYPE_CHECK (NODE)->type_with_lang_specific.lang_specific)

struct GTY(()) tree_type_with_lang_specific {
  struct tree_type_common common;
  /* Points to a structure whose details depend on the language in use.  */
  struct lang_type *lang_specific;
};

#define TYPE_VALUES(NODE) (ENUMERAL_TYPE_CHECK (NODE)->type_non_common.values)
#define TYPE_DOMAIN(NODE) (ARRAY_TYPE_CHECK (NODE)->type_non_common.values)
#define TYPE_FIELDS(NODE) \
  (RECORD_OR_UNION_CHECK (NODE)->type_non_common.values)
#define TYPE_CACHED_VALUES(NODE) (TYPE_CHECK(NODE)->type_non_common.values)
#define TYPE_ARG_TYPES(NODE) \
  (FUNC_OR_METHOD_CHECK (NODE)->type_non_common.values)
#define TYPE_VALUES_RAW(NODE) (TYPE_CHECK(NODE)->type_non_common.values)

#define TYPE_METHODS(NODE) \
  (RECORD_OR_UNION_CHECK (NODE)->type_non_common.maxval)
#define TYPE_VFIELD(NODE) \
  (RECORD_OR_UNION_CHECK (NODE)->type_non_common.minval)
#define TYPE_METHOD_BASETYPE(NODE) \
  (FUNC_OR_METHOD_CHECK (NODE)->type_non_common.maxval)
#define TYPE_OFFSET_BASETYPE(NODE) \
  (OFFSET_TYPE_CHECK (NODE)->type_non_common.maxval)
#define TYPE_MAXVAL(NODE) (TYPE_CHECK (NODE)->type_non_common.maxval)
#define TYPE_MINVAL(NODE) (TYPE_CHECK (NODE)->type_non_common.minval)
#define TYPE_NEXT_PTR_TO(NODE) \
  (POINTER_TYPE_CHECK (NODE)->type_non_common.minval)
#define TYPE_NEXT_REF_TO(NODE) \
  (REFERENCE_TYPE_CHECK (NODE)->type_non_common.minval)
#define TYPE_MIN_VALUE(NODE) \
  (NUMERICAL_TYPE_CHECK (NODE)->type_non_common.minval)
#define TYPE_MAX_VALUE(NODE) \
  (NUMERICAL_TYPE_CHECK (NODE)->type_non_common.maxval)

/* If non-NULL, this is an upper bound of the size (in bytes) of an
   object of the given ARRAY_TYPE_NON_COMMON.  This allows temporaries to be
   allocated.  */
#define TYPE_ARRAY_MAX_SIZE(ARRAY_TYPE) \
  (ARRAY_TYPE_CHECK (ARRAY_TYPE)->type_non_common.maxval)

/* For record and union types, information about this type, as a base type
   for itself.  */
#define TYPE_BINFO(NODE) (RECORD_OR_UNION_CHECK(NODE)->type_non_common.binfo)

/* For non record and union types, used in a language-dependent way.  */
#define TYPE_LANG_SLOT_1(NODE) \
  (NOT_RECORD_OR_UNION_CHECK(NODE)->type_non_common.binfo)

struct GTY(()) tree_type_non_common {
  struct tree_type_with_lang_specific with_lang_specific;
  tree values;
  tree minval;
  tree maxval;
  tree binfo;
};

/* Define accessor macros for information about type inheritance
   and basetypes.

   A "basetype" means a particular usage of a data type for inheritance
   in another type.  Each such basetype usage has its own "binfo"
   object to describe it.  The binfo object is a TREE_VEC node.

   Inheritance is represented by the binfo nodes allocated for a
   given type.  For example, given types C and D, such that D is
   inherited by C, 3 binfo nodes will be allocated: one for describing
   the binfo properties of C, similarly one for D, and one for
   describing the binfo properties of D as a base type for C.
   Thus, given a pointer to class C, one can get a pointer to the binfo
   of D acting as a basetype for C by looking at C's binfo's basetypes.  */

/* BINFO specific flags.  */

/* Nonzero means that the derivation chain is via a `virtual' declaration.  */
#define BINFO_VIRTUAL_P(NODE) (TREE_BINFO_CHECK (NODE)->base.static_flag)

/* Flags for language dependent use.  */
#define BINFO_MARKED(NODE) TREE_LANG_FLAG_0(TREE_BINFO_CHECK(NODE))
#define BINFO_FLAG_1(NODE) TREE_LANG_FLAG_1(TREE_BINFO_CHECK(NODE))
#define BINFO_FLAG_2(NODE) TREE_LANG_FLAG_2(TREE_BINFO_CHECK(NODE))
#define BINFO_FLAG_3(NODE) TREE_LANG_FLAG_3(TREE_BINFO_CHECK(NODE))
#define BINFO_FLAG_4(NODE) TREE_LANG_FLAG_4(TREE_BINFO_CHECK(NODE))
#define BINFO_FLAG_5(NODE) TREE_LANG_FLAG_5(TREE_BINFO_CHECK(NODE))
#define BINFO_FLAG_6(NODE) TREE_LANG_FLAG_6(TREE_BINFO_CHECK(NODE))

/* The actual data type node being inherited in this basetype.  */
#define BINFO_TYPE(NODE) TREE_TYPE (TREE_BINFO_CHECK(NODE))

/* The offset where this basetype appears in its containing type.
   BINFO_OFFSET slot holds the offset (in bytes)
   from the base of the complete object to the base of the part of the
   object that is allocated on behalf of this `type'.
   This is always 0 except when there is multiple inheritance.  */

#define BINFO_OFFSET(NODE) (TREE_BINFO_CHECK(NODE)->binfo.offset)
#define BINFO_OFFSET_ZEROP(NODE) (integer_zerop (BINFO_OFFSET (NODE)))

/* The virtual function table belonging to this basetype.  Virtual
   function tables provide a mechanism for run-time method dispatching.
   The entries of a virtual function table are language-dependent.  */

#define BINFO_VTABLE(NODE) (TREE_BINFO_CHECK(NODE)->binfo.vtable)

/* The virtual functions in the virtual function table.  This is
   a TREE_LIST that is used as an initial approximation for building
   a virtual function table for this basetype.  */
#define BINFO_VIRTUALS(NODE) (TREE_BINFO_CHECK(NODE)->binfo.virtuals)

/* A vector of binfos for the direct basetypes inherited by this
   basetype.

   If this basetype describes type D as inherited in C, and if the
   basetypes of D are E and F, then this vector contains binfos for
   inheritance of E and F by C.  */
#define BINFO_BASE_BINFOS(NODE) (&TREE_BINFO_CHECK(NODE)->binfo.base_binfos)

/* The number of basetypes for NODE.  */
#define BINFO_N_BASE_BINFOS(NODE) (BINFO_BASE_BINFOS (NODE)->length ())

/* Accessor macro to get to the Nth base binfo of this binfo.  */
#define BINFO_BASE_BINFO(NODE,N) \
 ((*BINFO_BASE_BINFOS (NODE))[(N)])
#define BINFO_BASE_ITERATE(NODE,N,B) \
 (BINFO_BASE_BINFOS (NODE)->iterate ((N), &(B)))
#define BINFO_BASE_APPEND(NODE,T) \
 (BINFO_BASE_BINFOS (NODE)->quick_push ((T)))

/* For a BINFO record describing a virtual base class, i.e., one where
   TREE_VIA_VIRTUAL is set, this field assists in locating the virtual
   base.  The actual contents are language-dependent.  In the C++
   front-end this field is an INTEGER_CST giving an offset into the
   vtable where the offset to the virtual base can be found.  */
#define BINFO_VPTR_FIELD(NODE) (TREE_BINFO_CHECK(NODE)->binfo.vptr_field)

/* Indicates the accesses this binfo has to its bases. The values are
   access_public_node, access_protected_node or access_private_node.
   If this array is not present, public access is implied.  */
#define BINFO_BASE_ACCESSES(NODE) (TREE_BINFO_CHECK(NODE)->binfo.base_accesses)

#define BINFO_BASE_ACCESS(NODE,N) \
  (*BINFO_BASE_ACCESSES (NODE))[(N)]
#define BINFO_BASE_ACCESS_APPEND(NODE,T) \
  BINFO_BASE_ACCESSES (NODE)->quick_push ((T))

/* The index in the VTT where this subobject's sub-VTT can be found.
   NULL_TREE if there is no sub-VTT.  */
#define BINFO_SUBVTT_INDEX(NODE) (TREE_BINFO_CHECK(NODE)->binfo.vtt_subvtt)

/* The index in the VTT where the vptr for this subobject can be
   found.  NULL_TREE if there is no secondary vptr in the VTT.  */
#define BINFO_VPTR_INDEX(NODE) (TREE_BINFO_CHECK(NODE)->binfo.vtt_vptr)

/* The BINFO_INHERITANCE_CHAIN points at the binfo for the base
   inheriting this base for non-virtual bases. For virtual bases it
   points either to the binfo for which this is a primary binfo, or to
   the binfo of the most derived type.  */
#define BINFO_INHERITANCE_CHAIN(NODE) \
	(TREE_BINFO_CHECK(NODE)->binfo.inheritance)

struct GTY (()) tree_binfo {
  struct tree_common common;

  tree offset;
  tree vtable;
  tree virtuals;
  tree vptr_field;
  vec<tree, va_gc> *base_accesses;
  tree inheritance;

  tree vtt_subvtt;
  tree vtt_vptr;

  vec<tree, va_gc> base_binfos;
};


/* Define fields and accessors for nodes representing declared names.  */

/* Nonzero if DECL represents an SSA name or a variable that can possibly
   have an associated SSA name.  */
#define SSA_VAR_P(DECL)							\
	(TREE_CODE (DECL) == VAR_DECL					\
	 || TREE_CODE (DECL) == PARM_DECL				\
	 || TREE_CODE (DECL) == RESULT_DECL				\
	 || TREE_CODE (DECL) == SSA_NAME)



/* Enumerate visibility settings.  */
#ifndef SYMBOL_VISIBILITY_DEFINED
#define SYMBOL_VISIBILITY_DEFINED
enum symbol_visibility
{
  VISIBILITY_DEFAULT,
  VISIBILITY_PROTECTED,
  VISIBILITY_HIDDEN,
  VISIBILITY_INTERNAL
};
#endif

struct function;

#define DECL_CHAIN(NODE) (TREE_CHAIN (DECL_MINIMAL_CHECK (NODE)))

/* This is the name of the object as written by the user.
   It is an IDENTIFIER_NODE.  */
#define DECL_NAME(NODE) (DECL_MINIMAL_CHECK (NODE)->decl_minimal.name)

/* Every ..._DECL node gets a unique number.  */
#define DECL_UID(NODE) (DECL_MINIMAL_CHECK (NODE)->decl_minimal.uid)

/* DEBUG_EXPR_DECLs get negative UID numbers, to catch erroneous
   uses.  */
#define DEBUG_TEMP_UID(NODE) (-DECL_UID (TREE_CHECK ((NODE), DEBUG_EXPR_DECL)))

/* Every ..._DECL node gets a unique number that stays the same even
   when the decl is copied by the inliner once it is set.  */
#define DECL_PT_UID(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.pt_uid == -1u \
   ? (NODE)->decl_minimal.uid : (NODE)->decl_common.pt_uid)
/* Initialize the ..._DECL node pt-uid to the decls uid.  */
#define SET_DECL_PT_UID(NODE, UID) \
  (DECL_COMMON_CHECK (NODE)->decl_common.pt_uid = (UID))
/* Whether the ..._DECL node pt-uid has been initialized and thus needs to
   be preserved when copyin the decl.  */
#define DECL_PT_UID_SET_P(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.pt_uid != -1u)

/* These two fields describe where in the source code the declaration
   was.  If the declaration appears in several places (as for a C
   function that is declared first and then defined later), this
   information should refer to the definition.  */
#define DECL_SOURCE_LOCATION(NODE) \
  (DECL_MINIMAL_CHECK (NODE)->decl_minimal.locus)
#define DECL_SOURCE_FILE(NODE) LOCATION_FILE (DECL_SOURCE_LOCATION (NODE))
#define DECL_SOURCE_LINE(NODE) LOCATION_LINE (DECL_SOURCE_LOCATION (NODE))
#define DECL_SOURCE_COLUMN(NODE) LOCATION_COLUMN (DECL_SOURCE_LOCATION (NODE))
#define DECL_IS_BUILTIN(DECL) \
  (LOCATION_LOCUS (DECL_SOURCE_LOCATION (DECL)) <= BUILTINS_LOCATION)

/*  For FIELD_DECLs, this is the RECORD_TYPE, UNION_TYPE, or
    QUAL_UNION_TYPE node that the field is a member of.  For VAR_DECL,
    PARM_DECL, FUNCTION_DECL, LABEL_DECL, RESULT_DECL, and CONST_DECL
    nodes, this points to either the FUNCTION_DECL for the containing
    function, the RECORD_TYPE or UNION_TYPE for the containing type, or
    NULL_TREE or a TRANSLATION_UNIT_DECL if the given decl has "file
    scope".  In particular, for VAR_DECLs which are virtual table pointers
    (they have DECL_VIRTUAL set), we use DECL_CONTEXT to determine the type
    they belong to.  */
#define DECL_CONTEXT(NODE) (DECL_MINIMAL_CHECK (NODE)->decl_minimal.context)
#define DECL_FIELD_CONTEXT(NODE) \
  (FIELD_DECL_CHECK (NODE)->decl_minimal.context)

/* If nonzero, decl's name shouldn't be emitted into debug info.  */
#define DECL_NAMELESS(NODE) (DECL_MINIMAL_CHECK (NODE)->base.u.bits.nameless_flag)

struct GTY(()) tree_decl_minimal {
  struct tree_common common;
  location_t locus;
  unsigned int uid;
  tree name;
  tree context;
};


/* For any sort of a ..._DECL node, this points to the original (abstract)
   decl node which this decl is an inlined/cloned instance of, or else it
   is NULL indicating that this decl is not an instance of some other decl.

   The C front-end also uses this in a nested declaration of an inline
   function, to point back to the definition.  */
#define DECL_ABSTRACT_ORIGIN(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.abstract_origin)

/* Like DECL_ABSTRACT_ORIGIN, but returns NODE if there's no abstract
   origin.  This is useful when setting the DECL_ABSTRACT_ORIGIN.  */
#define DECL_ORIGIN(NODE) \
  (DECL_ABSTRACT_ORIGIN (NODE) ? DECL_ABSTRACT_ORIGIN (NODE) : (NODE))

/* Nonzero for any sort of ..._DECL node means this decl node represents an
   inline instance of some original (abstract) decl from an inline function;
   suppress any warnings about shadowing some other variable.  FUNCTION_DECL
   nodes can also have their abstract origin set to themselves.  */
#define DECL_FROM_INLINE(NODE) \
  (DECL_ABSTRACT_ORIGIN (NODE) != NULL_TREE \
   && DECL_ABSTRACT_ORIGIN (NODE) != (NODE))

/* In a DECL this is the field where attributes are stored.  */
#define DECL_ATTRIBUTES(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.attributes)

/* For a FUNCTION_DECL, holds the tree of BINDINGs.
   For a TRANSLATION_UNIT_DECL, holds the namespace's BLOCK.
   For a VAR_DECL, holds the initial value.
   For a PARM_DECL, used for DECL_ARG_TYPE--default
   values for parameters are encoded in the type of the function,
   not in the PARM_DECL slot.
   For a FIELD_DECL, this is used for enumeration values and the C
   frontend uses it for temporarily storing bitwidth of bitfields.

   ??? Need to figure out some way to check this isn't a PARM_DECL.  */
#define DECL_INITIAL(NODE) (DECL_COMMON_CHECK (NODE)->decl_common.initial)

/* Holds the size of the datum, in bits, as a tree expression.
   Need not be constant.  */
#define DECL_SIZE(NODE) (DECL_COMMON_CHECK (NODE)->decl_common.size)
/* Likewise for the size in bytes.  */
#define DECL_SIZE_UNIT(NODE) (DECL_COMMON_CHECK (NODE)->decl_common.size_unit)
/* Holds the alignment required for the datum, in bits.  */
#define DECL_ALIGN(NODE) (DECL_COMMON_CHECK (NODE)->decl_common.align)
/* The alignment of NODE, in bytes.  */
#define DECL_ALIGN_UNIT(NODE) (DECL_ALIGN (NODE) / BITS_PER_UNIT)
/* Set if the alignment of this DECL has been set by the user, for
   example with an 'aligned' attribute.  */
#define DECL_USER_ALIGN(NODE) \
  (DECL_COMMON_CHECK (NODE)->base.u.bits.user_align)
/* Holds the machine mode corresponding to the declaration of a variable or
   field.  Always equal to TYPE_MODE (TREE_TYPE (decl)) except for a
   FIELD_DECL.  */
#define DECL_MODE(NODE) (DECL_COMMON_CHECK (NODE)->decl_common.mode)

/* For FUNCTION_DECL, if it is built-in, this identifies which built-in
   operation it is.  Note, however, that this field is overloaded, with
   DECL_BUILT_IN_CLASS as the discriminant, so the latter must always be
   checked before any access to the former.  */
#define DECL_FUNCTION_CODE(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.function_code)
#define DECL_DEBUG_EXPR_IS_FROM(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.debug_expr_is_from)

#define DECL_FUNCTION_PERSONALITY(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.personality)

/* Nonzero for a given ..._DECL node means that the name of this node should
   be ignored for symbolic debug purposes.  For a TYPE_DECL, this means that
   the associated type should be ignored.  For a FUNCTION_DECL, the body of
   the function should also be ignored.  */
#define DECL_IGNORED_P(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.ignored_flag)

/* Nonzero for a given ..._DECL node means that this node represents an
   "abstract instance" of the given declaration (e.g. in the original
   declaration of an inline function).  When generating symbolic debugging
   information, we mustn't try to generate any address information for nodes
   marked as "abstract instances" because we don't actually generate
   any code or allocate any data space for such instances.  */
#define DECL_ABSTRACT(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.abstract_flag)

/* Language-specific decl information.  */
#define DECL_LANG_SPECIFIC(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_specific)

/* In a VAR_DECL or FUNCTION_DECL, nonzero means external reference:
   do not allocate storage, and refer to a definition elsewhere.  Note that
   this does not necessarily imply the entity represented by NODE
   has no program source-level definition in this translation unit.  For
   example, for a FUNCTION_DECL, DECL_SAVED_TREE may be non-NULL and
   DECL_EXTERNAL may be true simultaneously; that can be the case for
   a C99 "extern inline" function.  */
#define DECL_EXTERNAL(NODE) (DECL_COMMON_CHECK (NODE)->decl_common.decl_flag_1)

/* Nonzero in a ..._DECL means this variable is ref'd from a nested function.
   For VAR_DECL nodes, PARM_DECL nodes, and FUNCTION_DECL nodes.

   For LABEL_DECL nodes, nonzero if nonlocal gotos to the label are permitted.

   Also set in some languages for variables, etc., outside the normal
   lexical scope, such as class instance variables.  */
#define DECL_NONLOCAL(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.nonlocal_flag)

/* Used in VAR_DECLs to indicate that the variable is a vtable.
   Used in FIELD_DECLs for vtable pointers.
   Used in FUNCTION_DECLs to indicate that the function is virtual.  */
#define DECL_VIRTUAL_P(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.virtual_flag)

/* Used to indicate that this DECL represents a compiler-generated entity.  */
#define DECL_ARTIFICIAL(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.artificial_flag)

/* Additional flags for language-specific uses.  */
#define DECL_LANG_FLAG_0(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_flag_0)
#define DECL_LANG_FLAG_1(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_flag_1)
#define DECL_LANG_FLAG_2(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_flag_2)
#define DECL_LANG_FLAG_3(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_flag_3)
#define DECL_LANG_FLAG_4(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_flag_4)
#define DECL_LANG_FLAG_5(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_flag_5)
#define DECL_LANG_FLAG_6(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_flag_6)
#define DECL_LANG_FLAG_7(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_flag_7)
#define DECL_LANG_FLAG_8(NODE) \
  (DECL_COMMON_CHECK (NODE)->decl_common.lang_flag_8)

/* Nonzero for a scope which is equal to file scope.  */
#define SCOPE_FILE_SCOPE_P(EXP)	\
  (! (EXP) || TREE_CODE (EXP) == TRANSLATION_UNIT_DECL)
/* Nonzero for a decl which is at file scope.  */
#define DECL_FILE_SCOPE_P(EXP) SCOPE_FILE_SCOPE_P (DECL_CONTEXT (EXP))
/* Nonzero for a type which is at file scope.  */
#define TYPE_FILE_SCOPE_P(EXP) SCOPE_FILE_SCOPE_P (TYPE_CONTEXT (EXP))

/* Nonzero for a decl that is decorated using attribute used.
   This indicates to compiler tools that this decl needs to be preserved.  */
#define DECL_PRESERVE_P(DECL) \
  DECL_COMMON_CHECK (DECL)->decl_common.preserve_flag

/* For function local variables of COMPLEX and VECTOR types,
   indicates that the variable is not aliased, and that all
   modifications to the variable have been adjusted so that
   they are killing assignments.  Thus the variable may now
   be treated as a GIMPLE register, and use real instead of
   virtual ops in SSA form.  */
#define DECL_GIMPLE_REG_P(DECL) \
  DECL_COMMON_CHECK (DECL)->decl_common.gimple_reg_flag

struct GTY(()) tree_decl_common {
  struct tree_decl_minimal common;
  tree size;

  ENUM_BITFIELD(machine_mode) mode : 8;

  unsigned nonlocal_flag : 1;
  unsigned virtual_flag : 1;
  unsigned ignored_flag : 1;
  unsigned abstract_flag : 1;
  unsigned artificial_flag : 1;
  unsigned preserve_flag: 1;
  unsigned debug_expr_is_from : 1;

  unsigned lang_flag_0 : 1;
  unsigned lang_flag_1 : 1;
  unsigned lang_flag_2 : 1;
  unsigned lang_flag_3 : 1;
  unsigned lang_flag_4 : 1;
  unsigned lang_flag_5 : 1;
  unsigned lang_flag_6 : 1;
  unsigned lang_flag_7 : 1;
  unsigned lang_flag_8 : 1;

  /* In LABEL_DECL, this is DECL_ERROR_ISSUED.
     In VAR_DECL and PARM_DECL, this is DECL_REGISTER.  */
  unsigned decl_flag_0 : 1;
  /* In FIELD_DECL, this is DECL_BIT_FIELD
     In VAR_DECL and FUNCTION_DECL, this is DECL_EXTERNAL.
     In TYPE_DECL, this is TYPE_DECL_SUPPRESS_DEBUG.  */
  unsigned decl_flag_1 : 1;
  /* In FIELD_DECL, this is DECL_NONADDRESSABLE_P
     In VAR_DECL, PARM_DECL and RESULT_DECL, this is
     DECL_HAS_VALUE_EXPR_P.  */
  unsigned decl_flag_2 : 1;
  /* 1 bit unused.  */
  unsigned decl_flag_3 : 1;
  /* Logically, these two would go in a theoretical base shared by var and
     parm decl. */
  unsigned gimple_reg_flag : 1;
  /* In VAR_DECL, PARM_DECL and RESULT_DECL, this is DECL_BY_REFERENCE.  */
  unsigned decl_by_reference_flag : 1;
  /* In a VAR_DECL and PARM_DECL, this is DECL_READ_P.  */
  unsigned decl_read_flag : 1;
  /* In a VAR_DECL or RESULT_DECL, this is DECL_NONSHAREABLE.  */
  unsigned decl_nonshareable_flag : 1;

  /* DECL_OFFSET_ALIGN, used only for FIELD_DECLs.  */
  unsigned int off_align : 8;

  /* 24 bits unused.  */

  /* DECL_ALIGN.  It should have the same size as TYPE_ALIGN.  */
  unsigned int align;

  /* UID for points-to sets, stable over copying from inlining.  */
  unsigned int pt_uid;

  tree size_unit;
  tree initial;
  tree attributes;
  tree abstract_origin;

  /* Points to a structure whose details depend on the language in use.  */
  struct lang_decl *lang_specific;
};

extern tree decl_value_expr_lookup (tree);
extern void decl_value_expr_insert (tree, tree);

/* In a VAR_DECL or PARM_DECL, the location at which the value may be found,
   if transformations have made this more complicated than evaluating the
   decl itself.  This should only be used for debugging; once this field has
   been set, the decl itself may not legitimately appear in the function.  */
#define DECL_HAS_VALUE_EXPR_P(NODE) \
  (TREE_CHECK3 (NODE, VAR_DECL, PARM_DECL, RESULT_DECL) \
   ->decl_common.decl_flag_2)
#define DECL_VALUE_EXPR(NODE) \
  (decl_value_expr_lookup (DECL_WRTL_CHECK (NODE)))
#define SET_DECL_VALUE_EXPR(NODE, VAL) \
  (decl_value_expr_insert (DECL_WRTL_CHECK (NODE), VAL))

/* Holds the RTL expression for the value of a variable or function.
   This value can be evaluated lazily for functions, variables with
   static storage duration, and labels.  */
#define DECL_RTL(NODE)					\
  (DECL_WRTL_CHECK (NODE)->decl_with_rtl.rtl		\
   ? (NODE)->decl_with_rtl.rtl					\
   : (make_decl_rtl (NODE), (NODE)->decl_with_rtl.rtl))

/* Set the DECL_RTL for NODE to RTL.  */
#define SET_DECL_RTL(NODE, RTL) set_decl_rtl (NODE, RTL)

/* Returns nonzero if NODE is a tree node that can contain RTL.  */
#define HAS_RTL_P(NODE) (CODE_CONTAINS_STRUCT (TREE_CODE (NODE), TS_DECL_WRTL))

/* Returns nonzero if the DECL_RTL for NODE has already been set.  */
#define DECL_RTL_SET_P(NODE) \
  (HAS_RTL_P (NODE) && DECL_WRTL_CHECK (NODE)->decl_with_rtl.rtl != NULL)

/* Copy the RTL from NODE1 to NODE2.  If the RTL was not set for
   NODE1, it will not be set for NODE2; this is a lazy copy.  */
#define COPY_DECL_RTL(NODE1, NODE2) \
  (DECL_WRTL_CHECK (NODE2)->decl_with_rtl.rtl \
   = DECL_WRTL_CHECK (NODE1)->decl_with_rtl.rtl)

/* The DECL_RTL for NODE, if it is set, or NULL, if it is not set.  */
#define DECL_RTL_IF_SET(NODE) (DECL_RTL_SET_P (NODE) ? DECL_RTL (NODE) : NULL)

#if (GCC_VERSION >= 2007)
#define DECL_RTL_KNOWN_SET(decl) __extension__				\
({  tree const __d = (decl);						\
    gcc_checking_assert (DECL_RTL_SET_P (__d));				\
    /* Dereference it so the compiler knows it can't be NULL even	\
       without assertion checking.  */					\
    &*DECL_RTL_IF_SET (__d); })
#else
#define DECL_RTL_KNOWN_SET(decl) (&*DECL_RTL_IF_SET (decl))
#endif

/* In VAR_DECL and PARM_DECL nodes, nonzero means declared `register'.  */
#define DECL_REGISTER(NODE) (DECL_WRTL_CHECK (NODE)->decl_common.decl_flag_0)

struct GTY(()) tree_decl_with_rtl {
  struct tree_decl_common common;
  rtx rtl;
};

/* In a FIELD_DECL, this is the field position, counting in bytes, of the
   DECL_OFFSET_ALIGN-bit-sized word containing the bit closest to the beginning
   of the structure.  */
#define DECL_FIELD_OFFSET(NODE) (FIELD_DECL_CHECK (NODE)->field_decl.offset)

/* In a FIELD_DECL, this is the offset, in bits, of the first bit of the
   field from DECL_FIELD_OFFSET.  This field may be nonzero even for fields
   that are not bit fields (since DECL_OFFSET_ALIGN may be larger than the
   natural alignment of the field's type).  */
#define DECL_FIELD_BIT_OFFSET(NODE) \
  (FIELD_DECL_CHECK (NODE)->field_decl.bit_offset)

/* In a FIELD_DECL, this indicates whether the field was a bit-field and
   if so, the type that was originally specified for it.
   TREE_TYPE may have been modified (in finish_struct).  */
#define DECL_BIT_FIELD_TYPE(NODE) \
  (FIELD_DECL_CHECK (NODE)->field_decl.bit_field_type)

/* In a FIELD_DECL of a RECORD_TYPE, this is a pointer to the storage
   representative FIELD_DECL.  */
#define DECL_BIT_FIELD_REPRESENTATIVE(NODE) \
  (FIELD_DECL_CHECK (NODE)->field_decl.qualifier)

/* For a FIELD_DECL in a QUAL_UNION_TYPE, records the expression, which
   if nonzero, indicates that the field occupies the type.  */
#define DECL_QUALIFIER(NODE) (FIELD_DECL_CHECK (NODE)->field_decl.qualifier)

/* For FIELD_DECLs, off_align holds the number of low-order bits of
   DECL_FIELD_OFFSET which are known to be always zero.
   DECL_OFFSET_ALIGN thus returns the alignment that DECL_FIELD_OFFSET
   has.  */
#define DECL_OFFSET_ALIGN(NODE) \
  (((unsigned HOST_WIDE_INT)1) << FIELD_DECL_CHECK (NODE)->decl_common.off_align)

/* Specify that DECL_ALIGN(NODE) is a multiple of X.  */
#define SET_DECL_OFFSET_ALIGN(NODE, X) \
  (FIELD_DECL_CHECK (NODE)->decl_common.off_align = ffs_hwi (X) - 1)

/* For FIELD_DECLS, DECL_FCONTEXT is the *first* baseclass in
   which this FIELD_DECL is defined.  This information is needed when
   writing debugging information about vfield and vbase decls for C++.  */
#define DECL_FCONTEXT(NODE) (FIELD_DECL_CHECK (NODE)->field_decl.fcontext)

/* In a FIELD_DECL, indicates this field should be bit-packed.  */
#define DECL_PACKED(NODE) (FIELD_DECL_CHECK (NODE)->base.u.bits.packed_flag)

/* Nonzero in a FIELD_DECL means it is a bit field, and must be accessed
   specially.  */
#define DECL_BIT_FIELD(NODE) (FIELD_DECL_CHECK (NODE)->decl_common.decl_flag_1)

/* Used in a FIELD_DECL to indicate that we cannot form the address of
   this component.  This makes it possible for Type-Based Alias Analysis
   to disambiguate accesses to this field with indirect accesses using
   the field's type:

     struct S { int i; } s;
     int *p;

   If the flag is set on 'i', TBAA computes that s.i and *p never conflict.

   From the implementation's viewpoint, the alias set of the type of the
   field 'i' (int) will not be recorded as a subset of that of the type of
   's' (struct S) in record_component_aliases.  The counterpart is that
   accesses to s.i must not be given the alias set of the type of 'i'
   (int) but instead directly that of the type of 's' (struct S).  */
#define DECL_NONADDRESSABLE_P(NODE) \
  (FIELD_DECL_CHECK (NODE)->decl_common.decl_flag_2)

struct GTY(()) tree_field_decl {
  struct tree_decl_common common;

  tree offset;
  tree bit_field_type;
  tree qualifier;
  tree bit_offset;
  tree fcontext;
};

/* A numeric unique identifier for a LABEL_DECL.  The UID allocation is
   dense, unique within any one function, and may be used to index arrays.
   If the value is -1, then no UID has been assigned.  */
#define LABEL_DECL_UID(NODE) \
  (LABEL_DECL_CHECK (NODE)->label_decl.label_decl_uid)

/* In a LABEL_DECL, the EH region number for which the label is the
   post_landing_pad.  */
#define EH_LANDING_PAD_NR(NODE) \
  (LABEL_DECL_CHECK (NODE)->label_decl.eh_landing_pad_nr)

/* In LABEL_DECL nodes, nonzero means that an error message about
   jumping into such a binding contour has been printed for this label.  */
#define DECL_ERROR_ISSUED(NODE) \
  (LABEL_DECL_CHECK (NODE)->decl_common.decl_flag_0)

struct GTY(()) tree_label_decl {
  struct tree_decl_with_rtl common;
  int label_decl_uid;
  int eh_landing_pad_nr;
};

struct GTY(()) tree_result_decl {
  struct tree_decl_with_rtl common;
};

struct GTY(()) tree_const_decl {
  struct tree_decl_common common;
};

/* For a PARM_DECL, records the data type used to pass the argument,
   which may be different from the type seen in the program.  */
#define DECL_ARG_TYPE(NODE) (PARM_DECL_CHECK (NODE)->decl_common.initial)

/* For PARM_DECL, holds an RTL for the stack slot or register
   where the data was actually passed.  */
#define DECL_INCOMING_RTL(NODE) \
  (PARM_DECL_CHECK (NODE)->parm_decl.incoming_rtl)

struct GTY(()) tree_parm_decl {
  struct tree_decl_with_rtl common;
  rtx incoming_rtl;
};


/* Nonzero for a given ..._DECL node means that no warnings should be
   generated just because this node is unused.  */
#define DECL_IN_SYSTEM_HEADER(NODE) \
  (in_system_header_at (DECL_SOURCE_LOCATION (NODE)))

/* Used to indicate that the linkage status of this DECL is not yet known,
   so it should not be output now.  */
#define DECL_DEFER_OUTPUT(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.defer_output)

/* In a VAR_DECL that's static,
   nonzero if the space is in the text section.  */
#define DECL_IN_TEXT_SECTION(NODE) \
  (VAR_DECL_CHECK (NODE)->decl_with_vis.in_text_section)

/* In a VAR_DECL that's static,
   nonzero if it belongs to the global constant pool.  */
#define DECL_IN_CONSTANT_POOL(NODE) \
  (VAR_DECL_CHECK (NODE)->decl_with_vis.in_constant_pool)

/* Nonzero for a given ..._DECL node means that this node should be
   put in .common, if possible.  If a DECL_INITIAL is given, and it
   is not error_mark_node, then the decl cannot be put in .common.  */
#define DECL_COMMON(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.common_flag)

/* In a VAR_DECL, nonzero if the decl is a register variable with
   an explicit asm specification.  */
#define DECL_HARD_REGISTER(NODE)  \
  (VAR_DECL_CHECK (NODE)->decl_with_vis.hard_register)

  /* Used to indicate that this DECL has weak linkage.  */
#define DECL_WEAK(NODE) (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.weak_flag)

/* Used to indicate that the DECL is a dllimport.  */
#define DECL_DLLIMPORT_P(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.dllimport_flag)

/* Used in a DECL to indicate that, even if it TREE_PUBLIC, it need
   not be put out unless it is needed in this translation unit.
   Entities like this are shared across translation units (like weak
   entities), but are guaranteed to be generated by any translation
   unit that needs them, and therefore need not be put out anywhere
   where they are not needed.  DECL_COMDAT is just a hint to the
   back-end; it is up to front-ends which set this flag to ensure
   that there will never be any harm, other than bloat, in putting out
   something which is DECL_COMDAT.  */
#define DECL_COMDAT(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.comdat_flag)

#define DECL_COMDAT_GROUP(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.comdat_group)

/* Used in TREE_PUBLIC decls to indicate that copies of this DECL in
   multiple translation units should be merged.  */
#define DECL_ONE_ONLY(NODE) (DECL_COMDAT_GROUP (NODE) != NULL_TREE)

/* The name of the object as the assembler will see it (but before any
   translations made by ASM_OUTPUT_LABELREF).  Often this is the same
   as DECL_NAME.  It is an IDENTIFIER_NODE.  */
#define DECL_ASSEMBLER_NAME(NODE) decl_assembler_name (NODE)

/* Return true if NODE is a NODE that can contain a DECL_ASSEMBLER_NAME.
   This is true of all DECL nodes except FIELD_DECL.  */
#define HAS_DECL_ASSEMBLER_NAME_P(NODE) \
  (CODE_CONTAINS_STRUCT (TREE_CODE (NODE), TS_DECL_WITH_VIS))

/* Returns nonzero if the DECL_ASSEMBLER_NAME for NODE has been set.  If zero,
   the NODE might still have a DECL_ASSEMBLER_NAME -- it just hasn't been set
   yet.  */
#define DECL_ASSEMBLER_NAME_SET_P(NODE) \
  (HAS_DECL_ASSEMBLER_NAME_P (NODE) \
   && DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.assembler_name != NULL_TREE)

/* Set the DECL_ASSEMBLER_NAME for NODE to NAME.  */
#define SET_DECL_ASSEMBLER_NAME(NODE, NAME) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.assembler_name = (NAME))

/* Copy the DECL_ASSEMBLER_NAME from DECL1 to DECL2.  Note that if DECL1's
   DECL_ASSEMBLER_NAME has not yet been set, using this macro will not cause
   the DECL_ASSEMBLER_NAME of either DECL to be set.  In other words, the
   semantics of using this macro, are different than saying:

     SET_DECL_ASSEMBLER_NAME(DECL2, DECL_ASSEMBLER_NAME (DECL1))

   which will try to set the DECL_ASSEMBLER_NAME for DECL1.  */

#define COPY_DECL_ASSEMBLER_NAME(DECL1, DECL2)				\
  (DECL_ASSEMBLER_NAME_SET_P (DECL1)					\
   ? (void) SET_DECL_ASSEMBLER_NAME (DECL2,				\
				     DECL_ASSEMBLER_NAME (DECL1))	\
   : (void) 0)

/* Records the section name in a section attribute.  Used to pass
   the name from decl_attributes to make_function_rtl and make_decl_rtl.  */
#define DECL_SECTION_NAME(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.section_name)

/* Nonzero in a decl means that the gimplifier has seen (or placed)
   this variable in a BIND_EXPR.  */
#define DECL_SEEN_IN_BIND_EXPR_P(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.seen_in_bind_expr)

/* Value of the decls's visibility attribute */
#define DECL_VISIBILITY(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.visibility)

/* Nonzero means that the decl had its visibility specified rather than
   being inferred.  */
#define DECL_VISIBILITY_SPECIFIED(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.visibility_specified)

/* In a VAR_DECL, the model to use if the data should be allocated from
   thread-local storage.  */
#define DECL_TLS_MODEL(NODE) (VAR_DECL_CHECK (NODE)->decl_with_vis.tls_model)

/* In a VAR_DECL, nonzero if the data should be allocated from
   thread-local storage.  */
#define DECL_THREAD_LOCAL_P(NODE) \
  (VAR_DECL_CHECK (NODE)->decl_with_vis.tls_model >= TLS_MODEL_REAL)

/* In a non-local VAR_DECL with static storage duration, true if the
   variable has an initialization priority.  If false, the variable
   will be initialized at the DEFAULT_INIT_PRIORITY.  */
#define DECL_HAS_INIT_PRIORITY_P(NODE) \
  (VAR_DECL_CHECK (NODE)->decl_with_vis.init_priority_p)

/* Specify whether the section name was set by user or by
   compiler via -ffunction-sections.  */
#define DECL_HAS_IMPLICIT_SECTION_NAME_P(NODE) \
  (DECL_WITH_VIS_CHECK (NODE)->decl_with_vis.implicit_section_name_p)

struct GTY(()) tree_decl_with_vis {
 struct tree_decl_with_rtl common;
 tree assembler_name;
 tree section_name;
 tree comdat_group;

 /* Belong to VAR_DECL exclusively.  */
 unsigned defer_output : 1;
 unsigned hard_register : 1;
 unsigned common_flag : 1;
 unsigned in_text_section : 1;
 unsigned in_constant_pool : 1;
 unsigned dllimport_flag : 1;
 /* Don't belong to VAR_DECL exclusively.  */
 unsigned weak_flag : 1;
 /* When SECTION_NAME is implied by -ffunction-section.  */
 unsigned implicit_section_name_p : 1;

 unsigned seen_in_bind_expr : 1;
 unsigned comdat_flag : 1;
 ENUM_BITFIELD(symbol_visibility) visibility : 2;
 unsigned visibility_specified : 1;
 /* Belongs to VAR_DECL exclusively.  */
 ENUM_BITFIELD(tls_model) tls_model : 3;

 /* Belong to FUNCTION_DECL exclusively.  */
 unsigned init_priority_p : 1;
 /* Used by C++ only.  Might become a generic decl flag.  */
 unsigned shadowed_for_var_p : 1;
 /* 14 unused bits. */
};

extern tree decl_debug_expr_lookup (tree);
extern void decl_debug_expr_insert (tree, tree);
/* For VAR_DECL, this is set to either an expression that it was split
   from (if DECL_DEBUG_EXPR_IS_FROM is true), otherwise a tree_list of
   subexpressions that it was split into.  */
#define DECL_DEBUG_EXPR(NODE) \
  (decl_debug_expr_lookup (VAR_DECL_CHECK (NODE)))

#define SET_DECL_DEBUG_EXPR(NODE, VAL) \
  (decl_debug_expr_insert (VAR_DECL_CHECK (NODE), VAL))

/* An initialization priority.  */
typedef unsigned short priority_type;

extern priority_type decl_init_priority_lookup (tree);
extern priority_type decl_fini_priority_lookup (tree);
extern void decl_init_priority_insert (tree, priority_type);
extern void decl_fini_priority_insert (tree, priority_type);

/* For a VAR_DECL or FUNCTION_DECL the initialization priority of
   NODE.  */
#define DECL_INIT_PRIORITY(NODE) \
  (decl_init_priority_lookup (NODE))
/* Set the initialization priority for NODE to VAL.  */
#define SET_DECL_INIT_PRIORITY(NODE, VAL) \
  (decl_init_priority_insert (NODE, VAL))

/* For a FUNCTION_DECL the finalization priority of NODE.  */
#define DECL_FINI_PRIORITY(NODE) \
  (decl_fini_priority_lookup (NODE))
/* Set the finalization priority for NODE to VAL.  */
#define SET_DECL_FINI_PRIORITY(NODE, VAL) \
  (decl_fini_priority_insert (NODE, VAL))

/* The initialization priority for entities for which no explicit
   initialization priority has been specified.  */
#define DEFAULT_INIT_PRIORITY 65535

/* The maximum allowed initialization priority.  */
#define MAX_INIT_PRIORITY 65535

/* The largest priority value reserved for use by system runtime
   libraries.  */
#define MAX_RESERVED_INIT_PRIORITY 100

/* In a VAR_DECL, nonzero if this is a global variable for VOPs.  */
#define VAR_DECL_IS_VIRTUAL_OPERAND(NODE) \
  (VAR_DECL_CHECK (NODE)->base.u.bits.saturating_flag)

/* In a VAR_DECL, nonzero if this is a non-local frame structure.  */
#define DECL_NONLOCAL_FRAME(NODE)  \
  (VAR_DECL_CHECK (NODE)->base.default_def_flag)

struct GTY(()) tree_var_decl {
  struct tree_decl_with_vis common;
};


/* This field is used to reference anything in decl.result and is meant only
   for use by the garbage collector.  */
#define DECL_RESULT_FLD(NODE) \
  (DECL_NON_COMMON_CHECK (NODE)->decl_non_common.result)

/* The DECL_VINDEX is used for FUNCTION_DECLS in two different ways.
   Before the struct containing the FUNCTION_DECL is laid out,
   DECL_VINDEX may point to a FUNCTION_DECL in a base class which
   is the FUNCTION_DECL which this FUNCTION_DECL will replace as a virtual
   function.  When the class is laid out, this pointer is changed
   to an INTEGER_CST node which is suitable for use as an index
   into the virtual function table.
   C++ also uses this field in namespaces, hence the DECL_NON_COMMON_CHECK.  */
#define DECL_VINDEX(NODE) \
  (DECL_NON_COMMON_CHECK (NODE)->decl_non_common.vindex)

struct GTY(())
 tree_decl_non_common {
  struct tree_decl_with_vis common;
  /* C++ uses this in namespaces.  */
  tree saved_tree;
  /* C++ uses this in templates.  */
  tree arguments;
  /* Almost all FE's use this.  */
  tree result;
  /* C++ uses this in namespaces and function_decls.  */
  tree vindex;
};

/* In FUNCTION_DECL, holds the decl for the return value.  */
#define DECL_RESULT(NODE) (FUNCTION_DECL_CHECK (NODE)->decl_non_common.result)

/* In a FUNCTION_DECL, nonzero if the function cannot be inlined.  */
#define DECL_UNINLINABLE(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.uninlinable)

/* In a FUNCTION_DECL, the saved representation of the body of the
   entire function.  */
#define DECL_SAVED_TREE(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->decl_non_common.saved_tree)

/* Nonzero in a FUNCTION_DECL means this function should be treated
   as if it were a malloc, meaning it returns a pointer that is
   not an alias.  */
#define DECL_IS_MALLOC(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.malloc_flag)

/* Nonzero in a FUNCTION_DECL means this function should be treated as
   C++ operator new, meaning that it returns a pointer for which we
   should not use type based aliasing.  */
#define DECL_IS_OPERATOR_NEW(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.operator_new_flag)

/* Nonzero in a FUNCTION_DECL means this function may return more
   than once.  */
#define DECL_IS_RETURNS_TWICE(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.returns_twice_flag)

/* Nonzero in a FUNCTION_DECL means this function should be treated
   as "pure" function (like const function, but may read global memory).  */
#define DECL_PURE_P(NODE) (FUNCTION_DECL_CHECK (NODE)->function_decl.pure_flag)

/* Nonzero only if one of TREE_READONLY or DECL_PURE_P is nonzero AND
   the const or pure function may not terminate.  When this is nonzero
   for a const or pure function, it can be dealt with by cse passes
   but cannot be removed by dce passes since you are not allowed to
   change an infinite looping program into one that terminates without
   error.  */
#define DECL_LOOPING_CONST_OR_PURE_P(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.looping_const_or_pure_flag)

/* Nonzero in a FUNCTION_DECL means this function should be treated
   as "novops" function (function that does not read global memory,
   but may have arbitrary side effects).  */
#define DECL_IS_NOVOPS(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.novops_flag)

/* Used in FUNCTION_DECLs to indicate that they should be run automatically
   at the beginning or end of execution.  */
#define DECL_STATIC_CONSTRUCTOR(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.static_ctor_flag)

#define DECL_STATIC_DESTRUCTOR(NODE) \
(FUNCTION_DECL_CHECK (NODE)->function_decl.static_dtor_flag)

/* Used in FUNCTION_DECLs to indicate that function entry and exit should
   be instrumented with calls to support routines.  */
#define DECL_NO_INSTRUMENT_FUNCTION_ENTRY_EXIT(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.no_instrument_function_entry_exit)

/* Used in FUNCTION_DECLs to indicate that limit-stack-* should be
   disabled in this function.  */
#define DECL_NO_LIMIT_STACK(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.no_limit_stack)

/* In a FUNCTION_DECL indicates that a static chain is needed.  */
#define DECL_STATIC_CHAIN(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.regdecl_flag)

/* Nonzero for a decl that cgraph has decided should be inlined into
   at least one call site.  It is not meaningful to look at this
   directly; always use cgraph_function_possibly_inlined_p.  */
#define DECL_POSSIBLY_INLINED(DECL) \
  FUNCTION_DECL_CHECK (DECL)->function_decl.possibly_inlined

/* Nonzero in a FUNCTION_DECL means that this function was declared inline,
   such as via the `inline' keyword in C/C++.  This flag controls the linkage
   semantics of 'inline'  */
#define DECL_DECLARED_INLINE_P(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.declared_inline_flag)

/* Nonzero in a FUNCTION_DECL means this function should not get
   -Winline warnings.  */
#define DECL_NO_INLINE_WARNING_P(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.no_inline_warning_flag)

/* Nonzero if a FUNCTION_CODE is a TM load/store.  */
#define BUILTIN_TM_LOAD_STORE_P(FN) \
  ((FN) >= BUILT_IN_TM_STORE_1 && (FN) <= BUILT_IN_TM_LOAD_RFW_LDOUBLE)

/* Nonzero if a FUNCTION_CODE is a TM load.  */
#define BUILTIN_TM_LOAD_P(FN) \
  ((FN) >= BUILT_IN_TM_LOAD_1 && (FN) <= BUILT_IN_TM_LOAD_RFW_LDOUBLE)

/* Nonzero if a FUNCTION_CODE is a TM store.  */
#define BUILTIN_TM_STORE_P(FN) \
  ((FN) >= BUILT_IN_TM_STORE_1 && (FN) <= BUILT_IN_TM_STORE_WAW_LDOUBLE)

#define CASE_BUILT_IN_TM_LOAD(FN)	\
  case BUILT_IN_TM_LOAD_##FN:		\
  case BUILT_IN_TM_LOAD_RAR_##FN:	\
  case BUILT_IN_TM_LOAD_RAW_##FN:	\
  case BUILT_IN_TM_LOAD_RFW_##FN

#define CASE_BUILT_IN_TM_STORE(FN)	\
  case BUILT_IN_TM_STORE_##FN:		\
  case BUILT_IN_TM_STORE_WAR_##FN:	\
  case BUILT_IN_TM_STORE_WAW_##FN

/* Nonzero in a FUNCTION_DECL that should be always inlined by the inliner
   disregarding size and cost heuristics.  This is equivalent to using
   the always_inline attribute without the required diagnostics if the
   function cannot be inlined.  */
#define DECL_DISREGARD_INLINE_LIMITS(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.disregard_inline_limits)

extern vec<tree, va_gc> **decl_debug_args_lookup (tree);
extern vec<tree, va_gc> **decl_debug_args_insert (tree);

/* Nonzero if a FUNCTION_DECL has DEBUG arguments attached to it.  */
#define DECL_HAS_DEBUG_ARGS_P(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.has_debug_args_flag)

/* For FUNCTION_DECL, this holds a pointer to a structure ("struct function")
   that describes the status of this function.  */
#define DECL_STRUCT_FUNCTION(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->function_decl.f)

/* In a FUNCTION_DECL, nonzero means a built in function.  */
#define DECL_BUILT_IN(NODE) (DECL_BUILT_IN_CLASS (NODE) != NOT_BUILT_IN)

/* For a builtin function, identify which part of the compiler defined it.  */
#define DECL_BUILT_IN_CLASS(NODE) \
   (FUNCTION_DECL_CHECK (NODE)->function_decl.built_in_class)

/* In FUNCTION_DECL, a chain of ..._DECL nodes.
   VAR_DECL and PARM_DECL reserve the arguments slot for language-specific
   uses.  */
#define DECL_ARGUMENTS(NODE) \
  (FUNCTION_DECL_CHECK (NODE)->decl_non_common.arguments)
#define DECL_ARGUMENT_FLD(NODE) \
  (DECL_NON_COMMON_CHECK (NODE)->decl_non_common.arguments)

/* In FUNCTION_DECL, the function specific target options to use when compiling
   this function.  */
#define DECL_FUNCTION_SPECIFIC_TARGET(NODE) \
   (FUNCTION_DECL_CHECK (NODE)->function_decl.function_specific_target)

/* In FUNCTION_DECL, the function specific optimization options to use when
   compiling this function.  */
#define DECL_FUNCTION_SPECIFIC_OPTIMIZATION(NODE) \
   (FUNCTION_DECL_CHECK (NODE)->function_decl.function_specific_optimization)

/* In FUNCTION_DECL, this is set if this function has other versions generated
   using "target" attributes.  The default version is the one which does not
   have any "target" attribute set. */
#define DECL_FUNCTION_VERSIONED(NODE)\
   (FUNCTION_DECL_CHECK (NODE)->function_decl.versioned_function)

/* FUNCTION_DECL inherits from DECL_NON_COMMON because of the use of the
   arguments/result/saved_tree fields by front ends.   It was either inherit
   FUNCTION_DECL from non_common, or inherit non_common from FUNCTION_DECL,
   which seemed a bit strange.  */

struct GTY(()) tree_function_decl {
  struct tree_decl_non_common common;

  struct function *f;

  /* The personality function. Used for stack unwinding. */
  tree personality;

  /* Function specific options that are used by this function.  */
  tree function_specific_target;	/* target options */
  tree function_specific_optimization;	/* optimization options */

  /* In a FUNCTION_DECL for which DECL_BUILT_IN holds, this is
     DECL_FUNCTION_CODE.  Otherwise unused.
     ???  The bitfield needs to be able to hold all target function
	  codes as well.  */
  ENUM_BITFIELD(built_in_function) function_code : 11;
  ENUM_BITFIELD(built_in_class) built_in_class : 2;

  unsigned static_ctor_flag : 1;
  unsigned static_dtor_flag : 1;
  unsigned uninlinable : 1;

  unsigned possibly_inlined : 1;
  unsigned novops_flag : 1;
  unsigned returns_twice_flag : 1;
  unsigned malloc_flag : 1;
  unsigned operator_new_flag : 1;
  unsigned declared_inline_flag : 1;
  unsigned regdecl_flag : 1;
  unsigned no_inline_warning_flag : 1;

  unsigned no_instrument_function_entry_exit : 1;
  unsigned no_limit_stack : 1;
  unsigned disregard_inline_limits : 1;
  unsigned pure_flag : 1;
  unsigned looping_const_or_pure_flag : 1;
  unsigned has_debug_args_flag : 1;
  unsigned tm_clone_flag : 1;
  unsigned versioned_function : 1;
  /* No bits left.  */
};

/* The source language of the translation-unit.  */
#define TRANSLATION_UNIT_LANGUAGE(NODE) \
  (TRANSLATION_UNIT_DECL_CHECK (NODE)->translation_unit_decl.language)

/* TRANSLATION_UNIT_DECL inherits from DECL_MINIMAL.  */

struct GTY(()) tree_translation_unit_decl {
  struct tree_decl_common common;
  /* Source language of this translation unit.  Used for DWARF output.  */
  const char * GTY((skip(""))) language;
  /* TODO: Non-optimization used to build this translation unit.  */
  /* TODO: Root of a partial DWARF tree for global types and decls.  */
};

/* A vector of all translation-units.  */
extern GTY (()) vec<tree, va_gc> *all_translation_units;

/* For a TYPE_DECL, holds the "original" type.  (TREE_TYPE has the copy.) */
#define DECL_ORIGINAL_TYPE(NODE) \
  (TYPE_DECL_CHECK (NODE)->decl_non_common.result)

/* In a TYPE_DECL nonzero means the detail info about this type is not dumped
   into stabs.  Instead it will generate cross reference ('x') of names.
   This uses the same flag as DECL_EXTERNAL.  */
#define TYPE_DECL_SUPPRESS_DEBUG(NODE) \
  (TYPE_DECL_CHECK (NODE)->decl_common.decl_flag_1)

/* Getter of the imported declaration associated to the
   IMPORTED_DECL node.  */
#define IMPORTED_DECL_ASSOCIATED_DECL(NODE) \
(DECL_INITIAL (IMPORTED_DECL_CHECK (NODE)))

struct GTY(()) tree_type_decl {
  struct tree_decl_non_common common;

};

/* A STATEMENT_LIST chains statements together in GENERIC and GIMPLE.
   To reduce overhead, the nodes containing the statements are not trees.
   This avoids the overhead of tree_common on all linked list elements.

   Use the interface in tree-iterator.h to access this node.  */

#define STATEMENT_LIST_HEAD(NODE) \
  (STATEMENT_LIST_CHECK (NODE)->stmt_list.head)
#define STATEMENT_LIST_TAIL(NODE) \
  (STATEMENT_LIST_CHECK (NODE)->stmt_list.tail)

struct GTY ((chain_next ("%h.next"), chain_prev ("%h.prev"))) tree_statement_list_node
 {
  struct tree_statement_list_node *prev;
  struct tree_statement_list_node *next;
  tree stmt;
};

struct GTY(()) tree_statement_list
 {
  struct tree_typed typed;
  struct tree_statement_list_node *head;
  struct tree_statement_list_node *tail;
};


/* Optimization options used by a function.  */

struct GTY(()) tree_optimization_option {
  struct tree_common common;

  /* The optimization options used by the user.  */
  struct cl_optimization opts;

  /* Target optabs for this set of optimization options.  This is of
     type `struct target_optabs *'.  */
  unsigned char *GTY ((atomic)) optabs;

  /* The value of this_target_optabs against which the optabs above were
     generated.  */
  struct target_optabs *GTY ((skip)) base_optabs;
};

#define TREE_OPTIMIZATION(NODE) \
  (&OPTIMIZATION_NODE_CHECK (NODE)->optimization.opts)

#define TREE_OPTIMIZATION_OPTABS(NODE) \
  (OPTIMIZATION_NODE_CHECK (NODE)->optimization.optabs)

#define TREE_OPTIMIZATION_BASE_OPTABS(NODE) \
  (OPTIMIZATION_NODE_CHECK (NODE)->optimization.base_optabs)

/* Return a tree node that encapsulates the current optimization options.  */
extern tree build_optimization_node (void);

extern void init_tree_optimization_optabs (tree);

/* Target options used by a function.  */

struct GTY(()) tree_target_option {
  struct tree_common common;

  /* The optimization options used by the user.  */
  struct cl_target_option opts;
};

#define TREE_TARGET_OPTION(NODE) \
  (&TARGET_OPTION_NODE_CHECK (NODE)->target_option.opts)

/* Return a tree node that encapsulates the current target options.  */
extern tree build_target_option_node (void);


/* Define the overall contents of a tree node.
   It may be any of the structures declared above
   for various types of node.  */

union GTY ((ptr_alias (union lang_tree_node),
	    desc ("tree_node_structure (&%h)"), variable_size)) tree_node {
  struct tree_base GTY ((tag ("TS_BASE"))) base;
  struct tree_typed GTY ((tag ("TS_TYPED"))) typed;
  struct tree_common GTY ((tag ("TS_COMMON"))) common;
  struct tree_int_cst GTY ((tag ("TS_INT_CST"))) int_cst;
  struct tree_real_cst GTY ((tag ("TS_REAL_CST"))) real_cst;
  struct tree_fixed_cst GTY ((tag ("TS_FIXED_CST"))) fixed_cst;
  struct tree_vector GTY ((tag ("TS_VECTOR"))) vector;
  struct tree_string GTY ((tag ("TS_STRING"))) string;
  struct tree_complex GTY ((tag ("TS_COMPLEX"))) complex;
  struct tree_identifier GTY ((tag ("TS_IDENTIFIER"))) identifier;
  struct tree_decl_minimal GTY((tag ("TS_DECL_MINIMAL"))) decl_minimal;
  struct tree_decl_common GTY ((tag ("TS_DECL_COMMON"))) decl_common;
  struct tree_decl_with_rtl GTY ((tag ("TS_DECL_WRTL"))) decl_with_rtl;
  struct tree_decl_non_common  GTY ((tag ("TS_DECL_NON_COMMON"))) decl_non_common;
  struct tree_parm_decl  GTY  ((tag ("TS_PARM_DECL"))) parm_decl;
  struct tree_decl_with_vis GTY ((tag ("TS_DECL_WITH_VIS"))) decl_with_vis;
  struct tree_var_decl GTY ((tag ("TS_VAR_DECL"))) var_decl;
  struct tree_field_decl GTY ((tag ("TS_FIELD_DECL"))) field_decl;
  struct tree_label_decl GTY ((tag ("TS_LABEL_DECL"))) label_decl;
  struct tree_result_decl GTY ((tag ("TS_RESULT_DECL"))) result_decl;
  struct tree_const_decl GTY ((tag ("TS_CONST_DECL"))) const_decl;
  struct tree_type_decl GTY ((tag ("TS_TYPE_DECL"))) type_decl;
  struct tree_function_decl GTY ((tag ("TS_FUNCTION_DECL"))) function_decl;
  struct tree_translation_unit_decl GTY ((tag ("TS_TRANSLATION_UNIT_DECL")))
    translation_unit_decl;
  struct tree_type_common GTY ((tag ("TS_TYPE_COMMON"))) type_common;
  struct tree_type_with_lang_specific GTY ((tag ("TS_TYPE_WITH_LANG_SPECIFIC")))
    type_with_lang_specific;
  struct tree_type_non_common GTY ((tag ("TS_TYPE_NON_COMMON")))
    type_non_common;
  struct tree_list GTY ((tag ("TS_LIST"))) list;
  struct tree_vec GTY ((tag ("TS_VEC"))) vec;
  struct tree_exp GTY ((tag ("TS_EXP"))) exp;
  struct tree_ssa_name GTY ((tag ("TS_SSA_NAME"))) ssa_name;
  struct tree_block GTY ((tag ("TS_BLOCK"))) block;
  struct tree_binfo GTY ((tag ("TS_BINFO"))) binfo;
  struct tree_statement_list GTY ((tag ("TS_STATEMENT_LIST"))) stmt_list;
  struct tree_constructor GTY ((tag ("TS_CONSTRUCTOR"))) constructor;
  struct tree_omp_clause GTY ((tag ("TS_OMP_CLAUSE"))) omp_clause;
  struct tree_optimization_option GTY ((tag ("TS_OPTIMIZATION"))) optimization;
  struct tree_target_option GTY ((tag ("TS_TARGET_OPTION"))) target_option;
};

#if defined ENABLE_TREE_CHECKING && (GCC_VERSION >= 2007)

inline tree
tree_check (tree __t, const char *__f, int __l, const char *__g, tree_code __c)
{
  if (TREE_CODE (__t) != __c)
    tree_check_failed (__t, __f, __l, __g, __c, 0);
  return __t;
}

inline tree
tree_not_check (tree __t, const char *__f, int __l, const char *__g,
                enum tree_code __c)
{
  if (TREE_CODE (__t) == __c)
    tree_not_check_failed (__t, __f, __l, __g, __c, 0);
  return __t;
}

inline tree
tree_check2 (tree __t, const char *__f, int __l, const char *__g,
             enum tree_code __c1, enum tree_code __c2)
{
  if (TREE_CODE (__t) != __c1
      && TREE_CODE (__t) != __c2)
    tree_check_failed (__t, __f, __l, __g, __c1, __c2, 0);
  return __t;
}

inline tree
tree_not_check2 (tree __t, const char *__f, int __l, const char *__g,
                 enum tree_code __c1, enum tree_code __c2)
{
  if (TREE_CODE (__t) == __c1
      || TREE_CODE (__t) == __c2)
    tree_not_check_failed (__t, __f, __l, __g, __c1, __c2, 0);
  return __t;
}

inline tree
tree_check3 (tree __t, const char *__f, int __l, const char *__g,
             enum tree_code __c1, enum tree_code __c2, enum tree_code __c3)
{
  if (TREE_CODE (__t) != __c1
      && TREE_CODE (__t) != __c2
      && TREE_CODE (__t) != __c3)
    tree_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, 0);
  return __t;
}

inline tree
tree_not_check3 (tree __t, const char *__f, int __l, const char *__g,
                 enum tree_code __c1, enum tree_code __c2, enum tree_code __c3)
{
  if (TREE_CODE (__t) == __c1
      || TREE_CODE (__t) == __c2
      || TREE_CODE (__t) == __c3)
    tree_not_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, 0);
  return __t;
}

inline tree
tree_check4 (tree __t, const char *__f, int __l, const char *__g,
             enum tree_code __c1, enum tree_code __c2, enum tree_code __c3,
             enum tree_code __c4)
{
  if (TREE_CODE (__t) != __c1
      && TREE_CODE (__t) != __c2
      && TREE_CODE (__t) != __c3
      && TREE_CODE (__t) != __c4)
    tree_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, __c4, 0);
  return __t;
}

inline tree
tree_not_check4 (tree __t, const char *__f, int __l, const char *__g,
                 enum tree_code __c1, enum tree_code __c2, enum tree_code __c3,
                 enum tree_code __c4)
{
  if (TREE_CODE (__t) == __c1
      || TREE_CODE (__t) == __c2
      || TREE_CODE (__t) == __c3
      || TREE_CODE (__t) == __c4)
    tree_not_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, __c4, 0);
  return __t;
}

inline tree
tree_check5 (tree __t, const char *__f, int __l, const char *__g,
             enum tree_code __c1, enum tree_code __c2, enum tree_code __c3,
             enum tree_code __c4, enum tree_code __c5)
{
  if (TREE_CODE (__t) != __c1
      && TREE_CODE (__t) != __c2
      && TREE_CODE (__t) != __c3
      && TREE_CODE (__t) != __c4
      && TREE_CODE (__t) != __c5)
    tree_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, __c4, __c5, 0);
  return __t;
}

inline tree
tree_not_check5 (tree __t, const char *__f, int __l, const char *__g,
                 enum tree_code __c1, enum tree_code __c2, enum tree_code __c3,
                 enum tree_code __c4, enum tree_code __c5)
{
  if (TREE_CODE (__t) == __c1
      || TREE_CODE (__t) == __c2
      || TREE_CODE (__t) == __c3
      || TREE_CODE (__t) == __c4
      || TREE_CODE (__t) == __c5)
    tree_not_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, __c4, __c5, 0);
  return __t;
}

inline tree
contains_struct_check (tree __t, const enum tree_node_structure_enum __s,
                       const char *__f, int __l, const char *__g)
{
  if (tree_contains_struct[TREE_CODE(__t)][__s] != 1)
      tree_contains_struct_check_failed (__t, __s, __f, __l, __g);
  return __t;
}

inline tree
tree_class_check (tree __t, const enum tree_code_class __class,
                  const char *__f, int __l, const char *__g)
{
  if (TREE_CODE_CLASS (TREE_CODE(__t)) != __class)
    tree_class_check_failed (__t, __class, __f, __l, __g);
  return __t;
}

inline tree
tree_range_check (tree __t,
                  enum tree_code __code1, enum tree_code __code2,
                  const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) < __code1 || TREE_CODE (__t) > __code2)
    tree_range_check_failed (__t, __f, __l, __g, __code1, __code2);
  return __t;
}

inline tree
omp_clause_subcode_check (tree __t, enum omp_clause_code __code,
                          const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != OMP_CLAUSE)
    tree_check_failed (__t, __f, __l, __g, OMP_CLAUSE, 0);
  if (__t->omp_clause.code != __code)
    omp_clause_check_failed (__t, __f, __l, __g, __code);
  return __t;
}

inline tree
omp_clause_range_check (tree __t,
                        enum omp_clause_code __code1,
                        enum omp_clause_code __code2,
                        const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != OMP_CLAUSE)
    tree_check_failed (__t, __f, __l, __g, OMP_CLAUSE, 0);
  if ((int) __t->omp_clause.code < (int) __code1
      || (int) __t->omp_clause.code > (int) __code2)
    omp_clause_range_check_failed (__t, __f, __l, __g, __code1, __code2);
  return __t;
}

/* These checks have to be special cased.  */

inline tree
expr_check (tree __t, const char *__f, int __l, const char *__g)
{
  char const __c = TREE_CODE_CLASS (TREE_CODE (__t));
  if (!IS_EXPR_CODE_CLASS (__c))
    tree_class_check_failed (__t, tcc_expression, __f, __l, __g);
  return __t;
}

/* These checks have to be special cased.  */

inline tree
non_type_check (tree __t, const char *__f, int __l, const char *__g)
{
  if (TYPE_P (__t))
    tree_not_class_check_failed (__t, tcc_type, __f, __l, __g);
  return __t;
}

inline tree *
tree_vec_elt_check (tree __t, int __i,
                    const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != TREE_VEC)
    tree_check_failed (__t, __f, __l, __g, TREE_VEC, 0);
  if (__i < 0 || __i >= __t->base.u.length)
    tree_vec_elt_check_failed (__i, __t->base.u.length, __f, __l, __g);
  return &CONST_CAST_TREE (__t)->vec.a[__i];
}

inline tree *
omp_clause_elt_check (tree __t, int __i,
                      const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != OMP_CLAUSE)
    tree_check_failed (__t, __f, __l, __g, OMP_CLAUSE, 0);
  if (__i < 0 || __i >= omp_clause_num_ops [__t->omp_clause.code])
    omp_clause_operand_check_failed (__i, __t, __f, __l, __g);
  return &__t->omp_clause.ops[__i];
}

inline const_tree
tree_check (const_tree __t, const char *__f, int __l, const char *__g,
	    tree_code __c)
{
  if (TREE_CODE (__t) != __c)
    tree_check_failed (__t, __f, __l, __g, __c, 0);
  return __t;
}

inline const_tree
tree_not_check (const_tree __t, const char *__f, int __l, const char *__g,
                enum tree_code __c)
{
  if (TREE_CODE (__t) == __c)
    tree_not_check_failed (__t, __f, __l, __g, __c, 0);
  return __t;
}

inline const_tree
tree_check2 (const_tree __t, const char *__f, int __l, const char *__g,
             enum tree_code __c1, enum tree_code __c2)
{
  if (TREE_CODE (__t) != __c1
      && TREE_CODE (__t) != __c2)
    tree_check_failed (__t, __f, __l, __g, __c1, __c2, 0);
  return __t;
}

inline const_tree
tree_not_check2 (const_tree __t, const char *__f, int __l, const char *__g,
                 enum tree_code __c1, enum tree_code __c2)
{
  if (TREE_CODE (__t) == __c1
      || TREE_CODE (__t) == __c2)
    tree_not_check_failed (__t, __f, __l, __g, __c1, __c2, 0);
  return __t;
}

inline const_tree
tree_check3 (const_tree __t, const char *__f, int __l, const char *__g,
             enum tree_code __c1, enum tree_code __c2, enum tree_code __c3)
{
  if (TREE_CODE (__t) != __c1
      && TREE_CODE (__t) != __c2
      && TREE_CODE (__t) != __c3)
    tree_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, 0);
  return __t;
}

inline const_tree
tree_not_check3 (const_tree __t, const char *__f, int __l, const char *__g,
                 enum tree_code __c1, enum tree_code __c2, enum tree_code __c3)
{
  if (TREE_CODE (__t) == __c1
      || TREE_CODE (__t) == __c2
      || TREE_CODE (__t) == __c3)
    tree_not_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, 0);
  return __t;
}

inline const_tree
tree_check4 (const_tree __t, const char *__f, int __l, const char *__g,
             enum tree_code __c1, enum tree_code __c2, enum tree_code __c3,
             enum tree_code __c4)
{
  if (TREE_CODE (__t) != __c1
      && TREE_CODE (__t) != __c2
      && TREE_CODE (__t) != __c3
      && TREE_CODE (__t) != __c4)
    tree_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, __c4, 0);
  return __t;
}

inline const_tree
tree_not_check4 (const_tree __t, const char *__f, int __l, const char *__g,
                 enum tree_code __c1, enum tree_code __c2, enum tree_code __c3,
                 enum tree_code __c4)
{
  if (TREE_CODE (__t) == __c1
      || TREE_CODE (__t) == __c2
      || TREE_CODE (__t) == __c3
      || TREE_CODE (__t) == __c4)
    tree_not_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, __c4, 0);
  return __t;
}

inline const_tree
tree_check5 (const_tree __t, const char *__f, int __l, const char *__g,
             enum tree_code __c1, enum tree_code __c2, enum tree_code __c3,
             enum tree_code __c4, enum tree_code __c5)
{
  if (TREE_CODE (__t) != __c1
      && TREE_CODE (__t) != __c2
      && TREE_CODE (__t) != __c3
      && TREE_CODE (__t) != __c4
      && TREE_CODE (__t) != __c5)
    tree_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, __c4, __c5, 0);
  return __t;
}

inline const_tree
tree_not_check5 (const_tree __t, const char *__f, int __l, const char *__g,
                 enum tree_code __c1, enum tree_code __c2, enum tree_code __c3,
                 enum tree_code __c4, enum tree_code __c5)
{
  if (TREE_CODE (__t) == __c1
      || TREE_CODE (__t) == __c2
      || TREE_CODE (__t) == __c3
      || TREE_CODE (__t) == __c4
      || TREE_CODE (__t) == __c5)
    tree_not_check_failed (__t, __f, __l, __g, __c1, __c2, __c3, __c4, __c5, 0);
  return __t;
}

inline const_tree
contains_struct_check (const_tree __t, const enum tree_node_structure_enum __s,
                       const char *__f, int __l, const char *__g)
{
  if (tree_contains_struct[TREE_CODE(__t)][__s] != 1)
      tree_contains_struct_check_failed (__t, __s, __f, __l, __g);
  return __t;
}

inline const_tree
tree_class_check (const_tree __t, const enum tree_code_class __class,
                  const char *__f, int __l, const char *__g)
{
  if (TREE_CODE_CLASS (TREE_CODE(__t)) != __class)
    tree_class_check_failed (__t, __class, __f, __l, __g);
  return __t;
}

inline const_tree
tree_range_check (const_tree __t,
                  enum tree_code __code1, enum tree_code __code2,
                  const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) < __code1 || TREE_CODE (__t) > __code2)
    tree_range_check_failed (__t, __f, __l, __g, __code1, __code2);
  return __t;
}

inline const_tree
omp_clause_subcode_check (const_tree __t, enum omp_clause_code __code,
                          const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != OMP_CLAUSE)
    tree_check_failed (__t, __f, __l, __g, OMP_CLAUSE, 0);
  if (__t->omp_clause.code != __code)
    omp_clause_check_failed (__t, __f, __l, __g, __code);
  return __t;
}

inline const_tree
omp_clause_range_check (const_tree __t,
                        enum omp_clause_code __code1,
                        enum omp_clause_code __code2,
                        const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != OMP_CLAUSE)
    tree_check_failed (__t, __f, __l, __g, OMP_CLAUSE, 0);
  if ((int) __t->omp_clause.code < (int) __code1
      || (int) __t->omp_clause.code > (int) __code2)
    omp_clause_range_check_failed (__t, __f, __l, __g, __code1, __code2);
  return __t;
}

inline const_tree
expr_check (const_tree __t, const char *__f, int __l, const char *__g)
{
  char const __c = TREE_CODE_CLASS (TREE_CODE (__t));
  if (!IS_EXPR_CODE_CLASS (__c))
    tree_class_check_failed (__t, tcc_expression, __f, __l, __g);
  return __t;
}

inline const_tree
non_type_check (const_tree __t, const char *__f, int __l, const char *__g)
{
  if (TYPE_P (__t))
    tree_not_class_check_failed (__t, tcc_type, __f, __l, __g);
  return __t;
}

inline const_tree *
tree_vec_elt_check (const_tree __t, int __i,
                    const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != TREE_VEC)
    tree_check_failed (__t, __f, __l, __g, TREE_VEC, 0);
  if (__i < 0 || __i >= __t->base.u.length)
    tree_vec_elt_check_failed (__i, __t->base.u.length, __f, __l, __g);
  return CONST_CAST (const_tree *, &__t->vec.a[__i]);
  //return &__t->vec.a[__i];
}

inline const_tree *
omp_clause_elt_check (const_tree __t, int __i,
                      const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != OMP_CLAUSE)
    tree_check_failed (__t, __f, __l, __g, OMP_CLAUSE, 0);
  if (__i < 0 || __i >= omp_clause_num_ops [__t->omp_clause.code])
    omp_clause_operand_check_failed (__i, __t, __f, __l, __g);
  return CONST_CAST (const_tree *, &__t->omp_clause.ops[__i]);
}

#endif

/* Compute the number of operands in an expression node NODE.  For
   tcc_vl_exp nodes like CALL_EXPRs, this is stored in the node itself,
   otherwise it is looked up from the node's code.  */
static inline int
tree_operand_length (const_tree node)
{
  if (VL_EXP_CLASS_P (node))
    return VL_EXP_OPERAND_LENGTH (node);
  else
    return TREE_CODE_LENGTH (TREE_CODE (node));
}

#if defined ENABLE_TREE_CHECKING && (GCC_VERSION >= 2007)

/* Special checks for TREE_OPERANDs.  */
inline tree *
tree_operand_check (tree __t, int __i,
                    const char *__f, int __l, const char *__g)
{
  const_tree __u = EXPR_CHECK (__t);
  if (__i < 0 || __i >= TREE_OPERAND_LENGTH (__u))
    tree_operand_check_failed (__i, __u, __f, __l, __g);
  return &CONST_CAST_TREE (__u)->exp.operands[__i];
}

inline tree *
tree_operand_check_code (tree __t, enum tree_code __code, int __i,
                         const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != __code)
    tree_check_failed (__t, __f, __l, __g, __code, 0);
  if (__i < 0 || __i >= TREE_OPERAND_LENGTH (__t))
    tree_operand_check_failed (__i, __t, __f, __l, __g);
  return &__t->exp.operands[__i];
}

inline const_tree *
tree_operand_check (const_tree __t, int __i,
                    const char *__f, int __l, const char *__g)
{
  const_tree __u = EXPR_CHECK (__t);
  if (__i < 0 || __i >= TREE_OPERAND_LENGTH (__u))
    tree_operand_check_failed (__i, __u, __f, __l, __g);
  return CONST_CAST (const_tree *, &__u->exp.operands[__i]);
}

inline const_tree *
tree_operand_check_code (const_tree __t, enum tree_code __code, int __i,
                         const char *__f, int __l, const char *__g)
{
  if (TREE_CODE (__t) != __code)
    tree_check_failed (__t, __f, __l, __g, __code, 0);
  if (__i < 0 || __i >= TREE_OPERAND_LENGTH (__t))
    tree_operand_check_failed (__i, __t, __f, __l, __g);
  return CONST_CAST (const_tree *, &__t->exp.operands[__i]);
}

#endif


/* Standard named or nameless data types of the C compiler.  */

enum tree_index
{
  TI_ERROR_MARK,
  TI_INTQI_TYPE,
  TI_INTHI_TYPE,
  TI_INTSI_TYPE,
  TI_INTDI_TYPE,
  TI_INTTI_TYPE,

  TI_UINTQI_TYPE,
  TI_UINTHI_TYPE,
  TI_UINTSI_TYPE,
  TI_UINTDI_TYPE,
  TI_UINTTI_TYPE,

  TI_UINT16_TYPE,
  TI_UINT32_TYPE,
  TI_UINT64_TYPE,

  TI_INTEGER_ZERO,
  TI_INTEGER_ONE,
  TI_INTEGER_THREE,
  TI_INTEGER_MINUS_ONE,
  TI_NULL_POINTER,

  TI_SIZE_ZERO,
  TI_SIZE_ONE,

  TI_BITSIZE_ZERO,
  TI_BITSIZE_ONE,
  TI_BITSIZE_UNIT,

  TI_PUBLIC,
  TI_PROTECTED,
  TI_PRIVATE,

  TI_BOOLEAN_FALSE,
  TI_BOOLEAN_TRUE,

  TI_COMPLEX_INTEGER_TYPE,
  TI_COMPLEX_FLOAT_TYPE,
  TI_COMPLEX_DOUBLE_TYPE,
  TI_COMPLEX_LONG_DOUBLE_TYPE,

  TI_FLOAT_TYPE,
  TI_DOUBLE_TYPE,
  TI_LONG_DOUBLE_TYPE,

  TI_FLOAT_PTR_TYPE,
  TI_DOUBLE_PTR_TYPE,
  TI_LONG_DOUBLE_PTR_TYPE,
  TI_INTEGER_PTR_TYPE,

  TI_VOID_TYPE,
  TI_PTR_TYPE,
  TI_CONST_PTR_TYPE,
  TI_SIZE_TYPE,
  TI_PID_TYPE,
  TI_PTRDIFF_TYPE,
  TI_VA_LIST_TYPE,
  TI_VA_LIST_GPR_COUNTER_FIELD,
  TI_VA_LIST_FPR_COUNTER_FIELD,
  TI_BOOLEAN_TYPE,
  TI_FILEPTR_TYPE,

  TI_DFLOAT32_TYPE,
  TI_DFLOAT64_TYPE,
  TI_DFLOAT128_TYPE,
  TI_DFLOAT32_PTR_TYPE,
  TI_DFLOAT64_PTR_TYPE,
  TI_DFLOAT128_PTR_TYPE,

  TI_VOID_LIST_NODE,

  TI_MAIN_IDENTIFIER,

  TI_SAT_SFRACT_TYPE,
  TI_SAT_FRACT_TYPE,
  TI_SAT_LFRACT_TYPE,
  TI_SAT_LLFRACT_TYPE,
  TI_SAT_USFRACT_TYPE,
  TI_SAT_UFRACT_TYPE,
  TI_SAT_ULFRACT_TYPE,
  TI_SAT_ULLFRACT_TYPE,
  TI_SFRACT_TYPE,
  TI_FRACT_TYPE,
  TI_LFRACT_TYPE,
  TI_LLFRACT_TYPE,
  TI_USFRACT_TYPE,
  TI_UFRACT_TYPE,
  TI_ULFRACT_TYPE,
  TI_ULLFRACT_TYPE,
  TI_SAT_SACCUM_TYPE,
  TI_SAT_ACCUM_TYPE,
  TI_SAT_LACCUM_TYPE,
  TI_SAT_LLACCUM_TYPE,
  TI_SAT_USACCUM_TYPE,
  TI_SAT_UACCUM_TYPE,
  TI_SAT_ULACCUM_TYPE,
  TI_SAT_ULLACCUM_TYPE,
  TI_SACCUM_TYPE,
  TI_ACCUM_TYPE,
  TI_LACCUM_TYPE,
  TI_LLACCUM_TYPE,
  TI_USACCUM_TYPE,
  TI_UACCUM_TYPE,
  TI_ULACCUM_TYPE,
  TI_ULLACCUM_TYPE,
  TI_QQ_TYPE,
  TI_HQ_TYPE,
  TI_SQ_TYPE,
  TI_DQ_TYPE,
  TI_TQ_TYPE,
  TI_UQQ_TYPE,
  TI_UHQ_TYPE,
  TI_USQ_TYPE,
  TI_UDQ_TYPE,
  TI_UTQ_TYPE,
  TI_SAT_QQ_TYPE,
  TI_SAT_HQ_TYPE,
  TI_SAT_SQ_TYPE,
  TI_SAT_DQ_TYPE,
  TI_SAT_TQ_TYPE,
  TI_SAT_UQQ_TYPE,
  TI_SAT_UHQ_TYPE,
  TI_SAT_USQ_TYPE,
  TI_SAT_UDQ_TYPE,
  TI_SAT_UTQ_TYPE,
  TI_HA_TYPE,
  TI_SA_TYPE,
  TI_DA_TYPE,
  TI_TA_TYPE,
  TI_UHA_TYPE,
  TI_USA_TYPE,
  TI_UDA_TYPE,
  TI_UTA_TYPE,
  TI_SAT_HA_TYPE,
  TI_SAT_SA_TYPE,
  TI_SAT_DA_TYPE,
  TI_SAT_TA_TYPE,
  TI_SAT_UHA_TYPE,
  TI_SAT_USA_TYPE,
  TI_SAT_UDA_TYPE,
  TI_SAT_UTA_TYPE,

  TI_OPTIMIZATION_DEFAULT,
  TI_OPTIMIZATION_CURRENT,
  TI_TARGET_OPTION_DEFAULT,
  TI_TARGET_OPTION_CURRENT,
  TI_CURRENT_TARGET_PRAGMA,
  TI_CURRENT_OPTIMIZE_PRAGMA,

  TI_MAX
};

extern GTY(()) tree global_trees[TI_MAX];

#define error_mark_node			global_trees[TI_ERROR_MARK]

#define intQI_type_node			global_trees[TI_INTQI_TYPE]
#define intHI_type_node			global_trees[TI_INTHI_TYPE]
#define intSI_type_node			global_trees[TI_INTSI_TYPE]
#define intDI_type_node			global_trees[TI_INTDI_TYPE]
#define intTI_type_node			global_trees[TI_INTTI_TYPE]

#define unsigned_intQI_type_node	global_trees[TI_UINTQI_TYPE]
#define unsigned_intHI_type_node	global_trees[TI_UINTHI_TYPE]
#define unsigned_intSI_type_node	global_trees[TI_UINTSI_TYPE]
#define unsigned_intDI_type_node	global_trees[TI_UINTDI_TYPE]
#define unsigned_intTI_type_node	global_trees[TI_UINTTI_TYPE]

#define uint16_type_node		global_trees[TI_UINT16_TYPE]
#define uint32_type_node		global_trees[TI_UINT32_TYPE]
#define uint64_type_node		global_trees[TI_UINT64_TYPE]

#define integer_zero_node		global_trees[TI_INTEGER_ZERO]
#define integer_one_node		global_trees[TI_INTEGER_ONE]
#define integer_three_node              global_trees[TI_INTEGER_THREE]
#define integer_minus_one_node		global_trees[TI_INTEGER_MINUS_ONE]
#define size_zero_node			global_trees[TI_SIZE_ZERO]
#define size_one_node			global_trees[TI_SIZE_ONE]
#define bitsize_zero_node		global_trees[TI_BITSIZE_ZERO]
#define bitsize_one_node		global_trees[TI_BITSIZE_ONE]
#define bitsize_unit_node		global_trees[TI_BITSIZE_UNIT]

/* Base access nodes.  */
#define access_public_node		global_trees[TI_PUBLIC]
#define access_protected_node	        global_trees[TI_PROTECTED]
#define access_private_node		global_trees[TI_PRIVATE]

#define null_pointer_node		global_trees[TI_NULL_POINTER]

#define float_type_node			global_trees[TI_FLOAT_TYPE]
#define double_type_node		global_trees[TI_DOUBLE_TYPE]
#define long_double_type_node		global_trees[TI_LONG_DOUBLE_TYPE]

#define float_ptr_type_node		global_trees[TI_FLOAT_PTR_TYPE]
#define double_ptr_type_node		global_trees[TI_DOUBLE_PTR_TYPE]
#define long_double_ptr_type_node	global_trees[TI_LONG_DOUBLE_PTR_TYPE]
#define integer_ptr_type_node		global_trees[TI_INTEGER_PTR_TYPE]

#define complex_integer_type_node	global_trees[TI_COMPLEX_INTEGER_TYPE]
#define complex_float_type_node		global_trees[TI_COMPLEX_FLOAT_TYPE]
#define complex_double_type_node	global_trees[TI_COMPLEX_DOUBLE_TYPE]
#define complex_long_double_type_node	global_trees[TI_COMPLEX_LONG_DOUBLE_TYPE]

#define void_type_node			global_trees[TI_VOID_TYPE]
/* The C type `void *'.  */
#define ptr_type_node			global_trees[TI_PTR_TYPE]
/* The C type `const void *'.  */
#define const_ptr_type_node		global_trees[TI_CONST_PTR_TYPE]
/* The C type `size_t'.  */
#define size_type_node                  global_trees[TI_SIZE_TYPE]
#define pid_type_node                   global_trees[TI_PID_TYPE]
#define ptrdiff_type_node		global_trees[TI_PTRDIFF_TYPE]
#define va_list_type_node		global_trees[TI_VA_LIST_TYPE]
#define va_list_gpr_counter_field	global_trees[TI_VA_LIST_GPR_COUNTER_FIELD]
#define va_list_fpr_counter_field	global_trees[TI_VA_LIST_FPR_COUNTER_FIELD]
/* The C type `FILE *'.  */
#define fileptr_type_node		global_trees[TI_FILEPTR_TYPE]

#define boolean_type_node		global_trees[TI_BOOLEAN_TYPE]
#define boolean_false_node		global_trees[TI_BOOLEAN_FALSE]
#define boolean_true_node		global_trees[TI_BOOLEAN_TRUE]

/* The decimal floating point types. */
#define dfloat32_type_node              global_trees[TI_DFLOAT32_TYPE]
#define dfloat64_type_node              global_trees[TI_DFLOAT64_TYPE]
#define dfloat128_type_node             global_trees[TI_DFLOAT128_TYPE]
#define dfloat32_ptr_type_node          global_trees[TI_DFLOAT32_PTR_TYPE]
#define dfloat64_ptr_type_node          global_trees[TI_DFLOAT64_PTR_TYPE]
#define dfloat128_ptr_type_node         global_trees[TI_DFLOAT128_PTR_TYPE]

/* The fixed-point types.  */
#define sat_short_fract_type_node       global_trees[TI_SAT_SFRACT_TYPE]
#define sat_fract_type_node             global_trees[TI_SAT_FRACT_TYPE]
#define sat_long_fract_type_node        global_trees[TI_SAT_LFRACT_TYPE]
#define sat_long_long_fract_type_node   global_trees[TI_SAT_LLFRACT_TYPE]
#define sat_unsigned_short_fract_type_node \
					global_trees[TI_SAT_USFRACT_TYPE]
#define sat_unsigned_fract_type_node    global_trees[TI_SAT_UFRACT_TYPE]
#define sat_unsigned_long_fract_type_node \
					global_trees[TI_SAT_ULFRACT_TYPE]
#define sat_unsigned_long_long_fract_type_node \
					global_trees[TI_SAT_ULLFRACT_TYPE]
#define short_fract_type_node           global_trees[TI_SFRACT_TYPE]
#define fract_type_node                 global_trees[TI_FRACT_TYPE]
#define long_fract_type_node            global_trees[TI_LFRACT_TYPE]
#define long_long_fract_type_node       global_trees[TI_LLFRACT_TYPE]
#define unsigned_short_fract_type_node  global_trees[TI_USFRACT_TYPE]
#define unsigned_fract_type_node        global_trees[TI_UFRACT_TYPE]
#define unsigned_long_fract_type_node   global_trees[TI_ULFRACT_TYPE]
#define unsigned_long_long_fract_type_node \
					global_trees[TI_ULLFRACT_TYPE]
#define sat_short_accum_type_node       global_trees[TI_SAT_SACCUM_TYPE]
#define sat_accum_type_node             global_trees[TI_SAT_ACCUM_TYPE]
#define sat_long_accum_type_node        global_trees[TI_SAT_LACCUM_TYPE]
#define sat_long_long_accum_type_node   global_trees[TI_SAT_LLACCUM_TYPE]
#define sat_unsigned_short_accum_type_node \
					global_trees[TI_SAT_USACCUM_TYPE]
#define sat_unsigned_accum_type_node    global_trees[TI_SAT_UACCUM_TYPE]
#define sat_unsigned_long_accum_type_node \
					global_trees[TI_SAT_ULACCUM_TYPE]
#define sat_unsigned_long_long_accum_type_node \
					global_trees[TI_SAT_ULLACCUM_TYPE]
#define short_accum_type_node           global_trees[TI_SACCUM_TYPE]
#define accum_type_node                 global_trees[TI_ACCUM_TYPE]
#define long_accum_type_node            global_trees[TI_LACCUM_TYPE]
#define long_long_accum_type_node       global_trees[TI_LLACCUM_TYPE]
#define unsigned_short_accum_type_node  global_trees[TI_USACCUM_TYPE]
#define unsigned_accum_type_node        global_trees[TI_UACCUM_TYPE]
#define unsigned_long_accum_type_node   global_trees[TI_ULACCUM_TYPE]
#define unsigned_long_long_accum_type_node \
					global_trees[TI_ULLACCUM_TYPE]
#define qq_type_node                    global_trees[TI_QQ_TYPE]
#define hq_type_node                    global_trees[TI_HQ_TYPE]
#define sq_type_node                    global_trees[TI_SQ_TYPE]
#define dq_type_node                    global_trees[TI_DQ_TYPE]
#define tq_type_node                    global_trees[TI_TQ_TYPE]
#define uqq_type_node                   global_trees[TI_UQQ_TYPE]
#define uhq_type_node                   global_trees[TI_UHQ_TYPE]
#define usq_type_node                   global_trees[TI_USQ_TYPE]
#define udq_type_node                   global_trees[TI_UDQ_TYPE]
#define utq_type_node                   global_trees[TI_UTQ_TYPE]
#define sat_qq_type_node                global_trees[TI_SAT_QQ_TYPE]
#define sat_hq_type_node                global_trees[TI_SAT_HQ_TYPE]
#define sat_sq_type_node                global_trees[TI_SAT_SQ_TYPE]
#define sat_dq_type_node                global_trees[TI_SAT_DQ_TYPE]
#define sat_tq_type_node                global_trees[TI_SAT_TQ_TYPE]
#define sat_uqq_type_node               global_trees[TI_SAT_UQQ_TYPE]
#define sat_uhq_type_node               global_trees[TI_SAT_UHQ_TYPE]
#define sat_usq_type_node               global_trees[TI_SAT_USQ_TYPE]
#define sat_udq_type_node               global_trees[TI_SAT_UDQ_TYPE]
#define sat_utq_type_node               global_trees[TI_SAT_UTQ_TYPE]
#define ha_type_node                    global_trees[TI_HA_TYPE]
#define sa_type_node                    global_trees[TI_SA_TYPE]
#define da_type_node                    global_trees[TI_DA_TYPE]
#define ta_type_node                    global_trees[TI_TA_TYPE]
#define uha_type_node                   global_trees[TI_UHA_TYPE]
#define usa_type_node                   global_trees[TI_USA_TYPE]
#define uda_type_node                   global_trees[TI_UDA_TYPE]
#define uta_type_node                   global_trees[TI_UTA_TYPE]
#define sat_ha_type_node                global_trees[TI_SAT_HA_TYPE]
#define sat_sa_type_node                global_trees[TI_SAT_SA_TYPE]
#define sat_da_type_node                global_trees[TI_SAT_DA_TYPE]
#define sat_ta_type_node                global_trees[TI_SAT_TA_TYPE]
#define sat_uha_type_node               global_trees[TI_SAT_UHA_TYPE]
#define sat_usa_type_node               global_trees[TI_SAT_USA_TYPE]
#define sat_uda_type_node               global_trees[TI_SAT_UDA_TYPE]
#define sat_uta_type_node               global_trees[TI_SAT_UTA_TYPE]

/* The node that should be placed at the end of a parameter list to
   indicate that the function does not take a variable number of
   arguments.  The TREE_VALUE will be void_type_node and there will be
   no TREE_CHAIN.  Language-independent code should not assume
   anything else about this node.  */
#define void_list_node                  global_trees[TI_VOID_LIST_NODE]

#define main_identifier_node		global_trees[TI_MAIN_IDENTIFIER]
#define MAIN_NAME_P(NODE) \
  (IDENTIFIER_NODE_CHECK (NODE) == main_identifier_node)

/* Optimization options (OPTIMIZATION_NODE) to use for default and current
   functions.  */
#define optimization_default_node	global_trees[TI_OPTIMIZATION_DEFAULT]
#define optimization_current_node	global_trees[TI_OPTIMIZATION_CURRENT]

/* Default/current target options (TARGET_OPTION_NODE).  */
#define target_option_default_node	global_trees[TI_TARGET_OPTION_DEFAULT]
#define target_option_current_node	global_trees[TI_TARGET_OPTION_CURRENT]

/* Default tree list option(), optimize() pragmas to be linked into the
   attribute list.  */
#define current_target_pragma		global_trees[TI_CURRENT_TARGET_PRAGMA]
#define current_optimize_pragma		global_trees[TI_CURRENT_OPTIMIZE_PRAGMA]

/* An enumeration of the standard C integer types.  These must be
   ordered so that shorter types appear before longer ones, and so
   that signed types appear before unsigned ones, for the correct
   functioning of interpret_integer() in c-lex.c.  */
enum integer_type_kind
{
  itk_char,
  itk_signed_char,
  itk_unsigned_char,
  itk_short,
  itk_unsigned_short,
  itk_int,
  itk_unsigned_int,
  itk_long,
  itk_unsigned_long,
  itk_long_long,
  itk_unsigned_long_long,
  itk_int128,
  itk_unsigned_int128,
  itk_none
};

typedef enum integer_type_kind integer_type_kind;

/* The standard C integer types.  Use integer_type_kind to index into
   this array.  */
extern GTY(()) tree integer_types[itk_none];

#define char_type_node			integer_types[itk_char]
#define signed_char_type_node		integer_types[itk_signed_char]
#define unsigned_char_type_node		integer_types[itk_unsigned_char]
#define short_integer_type_node		integer_types[itk_short]
#define short_unsigned_type_node	integer_types[itk_unsigned_short]
#define integer_type_node		integer_types[itk_int]
#define unsigned_type_node		integer_types[itk_unsigned_int]
#define long_integer_type_node		integer_types[itk_long]
#define long_unsigned_type_node		integer_types[itk_unsigned_long]
#define long_long_integer_type_node	integer_types[itk_long_long]
#define long_long_unsigned_type_node	integer_types[itk_unsigned_long_long]
#define int128_integer_type_node	integer_types[itk_int128]
#define int128_unsigned_type_node	integer_types[itk_unsigned_int128]

/* A pointer-to-function member type looks like:

     struct {
       __P __pfn;
       ptrdiff_t __delta;
     };

   If __pfn is NULL, it is a NULL pointer-to-member-function.

   (Because the vtable is always the first thing in the object, we
   don't need its offset.)  If the function is virtual, then PFN is
   one plus twice the index into the vtable; otherwise, it is just a
   pointer to the function.

   Unfortunately, using the lowest bit of PFN doesn't work in
   architectures that don't impose alignment requirements on function
   addresses, or that use the lowest bit to tell one ISA from another,
   for example.  For such architectures, we use the lowest bit of
   DELTA instead of the lowest bit of the PFN, and DELTA will be
   multiplied by 2.  */

enum ptrmemfunc_vbit_where_t
{
  ptrmemfunc_vbit_in_pfn,
  ptrmemfunc_vbit_in_delta
};

#define NULL_TREE (tree) NULL

/* True if NODE is an erroneous expression.  */

#define error_operand_p(NODE)					\
  ((NODE) == error_mark_node					\
   || ((NODE) && TREE_TYPE ((NODE)) == error_mark_node))

extern tree decl_assembler_name (tree);
extern bool decl_assembler_name_equal (tree decl, const_tree asmname);
extern hashval_t decl_assembler_name_hash (const_tree asmname);

/* Compute the number of bytes occupied by 'node'.  This routine only
   looks at TREE_CODE and, if the code is TREE_VEC, TREE_VEC_LENGTH.  */

extern size_t tree_size (const_tree);

/* Compute the number of bytes occupied by a tree with code CODE.  This
   function cannot be used for TREE_VEC codes, which are of variable
   length.  */
extern size_t tree_code_size (enum tree_code);

/* Allocate and return a new UID from the DECL_UID namespace.  */
extern int allocate_decl_uid (void);

/* Lowest level primitive for allocating a node.
   The TREE_CODE is the only argument.  Contents are initialized
   to zero except for a few of the common fields.  */

extern tree make_node_stat (enum tree_code MEM_STAT_DECL);
#define make_node(t) make_node_stat (t MEM_STAT_INFO)

/* Make a copy of a node, with all the same contents.  */

extern tree copy_node_stat (tree MEM_STAT_DECL);
#define copy_node(t) copy_node_stat (t MEM_STAT_INFO)

/* Make a copy of a chain of TREE_LIST nodes.  */

extern tree copy_list (tree);

/* Make a CASE_LABEL_EXPR.  */

extern tree build_case_label (tree, tree, tree);

/* Make a BINFO.  */
extern tree make_tree_binfo_stat (unsigned MEM_STAT_DECL);
#define make_tree_binfo(t) make_tree_binfo_stat (t MEM_STAT_INFO)

/* Make a TREE_VEC.  */

extern tree make_tree_vec_stat (int MEM_STAT_DECL);
#define make_tree_vec(t) make_tree_vec_stat (t MEM_STAT_INFO)

/* Return the (unique) IDENTIFIER_NODE node for a given name.
   The name is supplied as a char *.  */

extern tree get_identifier (const char *);

#if GCC_VERSION >= 3000
#define get_identifier(str) \
  (__builtin_constant_p (str)				\
    ? get_identifier_with_length ((str), strlen (str))  \
    : get_identifier (str))
#endif


/* Identical to get_identifier, except that the length is assumed
   known.  */

extern tree get_identifier_with_length (const char *, size_t);

/* If an identifier with the name TEXT (a null-terminated string) has
   previously been referred to, return that node; otherwise return
   NULL_TREE.  */

extern tree maybe_get_identifier (const char *);

/* Construct various types of nodes.  */

extern tree build_nt (enum tree_code, ...);
extern tree build_nt_call_vec (tree, vec<tree, va_gc> *);

extern tree build0_stat (enum tree_code, tree MEM_STAT_DECL);
#define build0(c,t) build0_stat (c,t MEM_STAT_INFO)
extern tree build1_stat (enum tree_code, tree, tree MEM_STAT_DECL);
#define build1(c,t1,t2) build1_stat (c,t1,t2 MEM_STAT_INFO)
extern tree build2_stat (enum tree_code, tree, tree, tree MEM_STAT_DECL);
#define build2(c,t1,t2,t3) build2_stat (c,t1,t2,t3 MEM_STAT_INFO)
extern tree build3_stat (enum tree_code, tree, tree, tree, tree MEM_STAT_DECL);
#define build3(c,t1,t2,t3,t4) build3_stat (c,t1,t2,t3,t4 MEM_STAT_INFO)
extern tree build4_stat (enum tree_code, tree, tree, tree, tree,
			 tree MEM_STAT_DECL);
#define build4(c,t1,t2,t3,t4,t5) build4_stat (c,t1,t2,t3,t4,t5 MEM_STAT_INFO)
extern tree build5_stat (enum tree_code, tree, tree, tree, tree, tree,
			 tree MEM_STAT_DECL);
#define build5(c,t1,t2,t3,t4,t5,t6) build5_stat (c,t1,t2,t3,t4,t5,t6 MEM_STAT_INFO)

/* _loc versions of build[1-5].  */

static inline tree
build1_stat_loc (location_t loc, enum tree_code code, tree type,
		 tree arg1 MEM_STAT_DECL)
{
  tree t = build1_stat (code, type, arg1 PASS_MEM_STAT);
  if (CAN_HAVE_LOCATION_P (t))
    SET_EXPR_LOCATION (t, loc);
  return t;
}
#define build1_loc(l,c,t1,t2) build1_stat_loc (l,c,t1,t2 MEM_STAT_INFO)

static inline tree
build2_stat_loc (location_t loc, enum tree_code code, tree type, tree arg0,
		 tree arg1 MEM_STAT_DECL)
{
  tree t = build2_stat (code, type, arg0, arg1 PASS_MEM_STAT);
  if (CAN_HAVE_LOCATION_P (t))
    SET_EXPR_LOCATION (t, loc);
  return t;
}
#define build2_loc(l,c,t1,t2,t3) build2_stat_loc (l,c,t1,t2,t3 MEM_STAT_INFO)

static inline tree
build3_stat_loc (location_t loc, enum tree_code code, tree type, tree arg0,
		 tree arg1, tree arg2 MEM_STAT_DECL)
{
  tree t = build3_stat (code, type, arg0, arg1, arg2 PASS_MEM_STAT);
  if (CAN_HAVE_LOCATION_P (t))
    SET_EXPR_LOCATION (t, loc);
  return t;
}
#define build3_loc(l,c,t1,t2,t3,t4) \
  build3_stat_loc (l,c,t1,t2,t3,t4 MEM_STAT_INFO)

static inline tree
build4_stat_loc (location_t loc, enum tree_code code, tree type, tree arg0,
		 tree arg1, tree arg2, tree arg3 MEM_STAT_DECL)
{
  tree t = build4_stat (code, type, arg0, arg1, arg2, arg3 PASS_MEM_STAT);
  if (CAN_HAVE_LOCATION_P (t))
    SET_EXPR_LOCATION (t, loc);
  return t;
}
#define build4_loc(l,c,t1,t2,t3,t4,t5) \
  build4_stat_loc (l,c,t1,t2,t3,t4,t5 MEM_STAT_INFO)

static inline tree
build5_stat_loc (location_t loc, enum tree_code code, tree type, tree arg0,
		 tree arg1, tree arg2, tree arg3, tree arg4 MEM_STAT_DECL)
{
  tree t = build5_stat (code, type, arg0, arg1, arg2, arg3,
			arg4 PASS_MEM_STAT);
  if (CAN_HAVE_LOCATION_P (t))
    SET_EXPR_LOCATION (t, loc);
  return t;
}
#define build5_loc(l,c,t1,t2,t3,t4,t5,t6) \
  build5_stat_loc (l,c,t1,t2,t3,t4,t5,t6 MEM_STAT_INFO)

extern tree build_var_debug_value_stat (tree, tree MEM_STAT_DECL);
#define build_var_debug_value(t1,t2) \
  build_var_debug_value_stat (t1,t2 MEM_STAT_INFO)

/* Constructs double_int from tree CST.  */

static inline double_int
tree_to_double_int (const_tree cst)
{
  return TREE_INT_CST (cst);
}

extern tree double_int_to_tree (tree, double_int);
extern bool double_int_fits_to_tree_p (const_tree, double_int);
extern tree force_fit_type_double (tree, double_int, int, bool);

/* Create an INT_CST node with a CST value zero extended.  */

static inline tree
build_int_cstu (tree type, unsigned HOST_WIDE_INT cst)
{
  return double_int_to_tree (type, double_int::from_uhwi (cst));
}

extern tree build_int_cst (tree, HOST_WIDE_INT);
extern tree build_int_cst_type (tree, HOST_WIDE_INT);
extern tree build_int_cst_wide (tree, unsigned HOST_WIDE_INT, HOST_WIDE_INT);
extern tree make_vector_stat (unsigned MEM_STAT_DECL);
#define make_vector(n) make_vector_stat (n MEM_STAT_INFO)
extern tree build_vector_stat (tree, tree * MEM_STAT_DECL);
#define build_vector(t,v) build_vector_stat (t, v MEM_STAT_INFO)
extern tree build_vector_from_ctor (tree, vec<constructor_elt, va_gc> *);
extern tree build_vector_from_val (tree, tree);
extern tree build_constructor (tree, vec<constructor_elt, va_gc> *);
extern tree build_constructor_single (tree, tree, tree);
extern tree build_constructor_from_list (tree, tree);
extern tree build_real_from_int_cst (tree, const_tree);
extern tree build_complex (tree, tree, tree);
extern tree build_one_cst (tree);
extern tree build_zero_cst (tree);
extern tree build_string (int, const char *);
extern tree build_tree_list_stat (tree, tree MEM_STAT_DECL);
#define build_tree_list(t,q) build_tree_list_stat(t,q MEM_STAT_INFO)
extern tree build_tree_list_vec_stat (const vec<tree, va_gc> *MEM_STAT_DECL);
#define build_tree_list_vec(v) build_tree_list_vec_stat (v MEM_STAT_INFO)
extern tree build_decl_stat (location_t, enum tree_code,
			     tree, tree MEM_STAT_DECL);
extern tree build_fn_decl (const char *, tree);
#define build_decl(l,c,t,q) build_decl_stat (l,c,t,q MEM_STAT_INFO)
extern tree build_translation_unit_decl (tree);
extern tree build_block (tree, tree, tree, tree);
extern tree build_empty_stmt (location_t);
extern tree build_omp_clause (location_t, enum omp_clause_code);

extern tree build_vl_exp_stat (enum tree_code, int MEM_STAT_DECL);
#define build_vl_exp(c,n) build_vl_exp_stat (c,n MEM_STAT_INFO)

extern tree build_call_nary (tree, tree, int, ...);
extern tree build_call_valist (tree, tree, int, va_list);
#define build_call_array(T1,T2,N,T3)\
   build_call_array_loc (UNKNOWN_LOCATION, T1, T2, N, T3)
extern tree build_call_array_loc (location_t, tree, tree, int, const tree *);
extern tree build_call_vec (tree, tree, vec<tree, va_gc> *);

/* Construct various nodes representing data types.  */

extern tree make_signed_type (int);
extern tree make_unsigned_type (int);
extern tree signed_or_unsigned_type_for (int, tree);
extern tree signed_type_for (tree);
extern tree unsigned_type_for (tree);
extern tree truth_type_for (tree);
extern void initialize_sizetypes (void);
extern void fixup_unsigned_type (tree);
extern tree build_pointer_type_for_mode (tree, enum machine_mode, bool);
extern tree build_pointer_type (tree);
extern tree build_reference_type_for_mode (tree, enum machine_mode, bool);
extern tree build_reference_type (tree);
extern tree build_vector_type_for_mode (tree, enum machine_mode);
extern tree build_vector_type (tree innertype, int nunits);
extern tree build_opaque_vector_type (tree innertype, int nunits);
extern tree build_type_no_quals (tree);
extern tree build_index_type (tree);
extern tree build_array_type (tree, tree);
extern tree build_nonshared_array_type (tree, tree);
extern tree build_array_type_nelts (tree, unsigned HOST_WIDE_INT);
extern tree build_function_type (tree, tree);
extern tree build_function_type_list (tree, ...);
extern tree build_function_decl_skip_args (tree, bitmap, bool);
extern tree build_varargs_function_type_list (tree, ...);
extern tree build_function_type_array (tree, int, tree *);
extern tree build_varargs_function_type_array (tree, int, tree *);
#define build_function_type_vec(RET, V) \
  build_function_type_array (RET, vec_safe_length (V), vec_safe_address (V))
#define build_varargs_function_type_vec(RET, V) \
  build_varargs_function_type_array (RET, vec_safe_length (V), \
				     vec_safe_address (V))
extern tree build_method_type_directly (tree, tree, tree);
extern tree build_method_type (tree, tree);
extern tree build_offset_type (tree, tree);
extern tree build_complex_type (tree);
extern tree array_type_nelts (const_tree);
extern bool in_array_bounds_p (tree);
extern bool range_in_array_bounds_p (tree);

extern tree value_member (tree, tree);
extern tree purpose_member (const_tree, tree);
extern bool vec_member (const_tree, vec<tree, va_gc> *);
extern tree chain_index (int, tree);

extern int attribute_list_equal (const_tree, const_tree);
extern int attribute_list_contained (const_tree, const_tree);
extern int tree_int_cst_equal (const_tree, const_tree);
extern int tree_int_cst_lt (const_tree, const_tree);
extern int tree_int_cst_compare (const_tree, const_tree);
extern int host_integerp (const_tree, int)
#ifndef ENABLE_TREE_CHECKING
  ATTRIBUTE_PURE /* host_integerp is pure only when checking is disabled.  */
#endif
  ;
extern HOST_WIDE_INT tree_low_cst (const_tree, int);
#if !defined ENABLE_TREE_CHECKING && (GCC_VERSION >= 4003)
extern inline __attribute__ ((__gnu_inline__)) HOST_WIDE_INT
tree_low_cst (const_tree t, int pos)
{
  gcc_assert (host_integerp (t, pos));
  return TREE_INT_CST_LOW (t);
}
#endif
extern HOST_WIDE_INT size_low_cst (const_tree);
extern int tree_int_cst_sgn (const_tree);
extern int tree_int_cst_sign_bit (const_tree);
extern unsigned int tree_int_cst_min_precision (tree, bool);
extern bool tree_expr_nonnegative_p (tree);
extern bool tree_expr_nonnegative_warnv_p (tree, bool *);
extern bool may_negate_without_overflow_p (const_tree);
extern tree strip_array_types (tree);
extern tree excess_precision_type (tree);
extern bool valid_constant_size_p (const_tree);

/* Construct various nodes representing fract or accum data types.  */

extern tree make_fract_type (int, int, int);
extern tree make_accum_type (int, int, int);

#define make_signed_fract_type(P) make_fract_type (P, 0, 0)
#define make_unsigned_fract_type(P) make_fract_type (P, 1, 0)
#define make_sat_signed_fract_type(P) make_fract_type (P, 0, 1)
#define make_sat_unsigned_fract_type(P) make_fract_type (P, 1, 1)
#define make_signed_accum_type(P) make_accum_type (P, 0, 0)
#define make_unsigned_accum_type(P) make_accum_type (P, 1, 0)
#define make_sat_signed_accum_type(P) make_accum_type (P, 0, 1)
#define make_sat_unsigned_accum_type(P) make_accum_type (P, 1, 1)

#define make_or_reuse_signed_fract_type(P) \
		make_or_reuse_fract_type (P, 0, 0)
#define make_or_reuse_unsigned_fract_type(P) \
		make_or_reuse_fract_type (P, 1, 0)
#define make_or_reuse_sat_signed_fract_type(P) \
		make_or_reuse_fract_type (P, 0, 1)
#define make_or_reuse_sat_unsigned_fract_type(P) \
		make_or_reuse_fract_type (P, 1, 1)
#define make_or_reuse_signed_accum_type(P) \
		make_or_reuse_accum_type (P, 0, 0)
#define make_or_reuse_unsigned_accum_type(P) \
		make_or_reuse_accum_type (P, 1, 0)
#define make_or_reuse_sat_signed_accum_type(P) \
		make_or_reuse_accum_type (P, 0, 1)
#define make_or_reuse_sat_unsigned_accum_type(P) \
		make_or_reuse_accum_type (P, 1, 1)

/* From expmed.c.  Since rtl.h is included after tree.h, we can't
   put the prototype here.  Rtl.h does declare the prototype if
   tree.h had been included.  */

extern tree make_tree (tree, rtx);

/* Return a type like TTYPE except that its TYPE_ATTRIBUTES
   is ATTRIBUTE.

   Such modified types already made are recorded so that duplicates
   are not made.  */

extern tree build_type_attribute_variant (tree, tree);
extern tree build_decl_attribute_variant (tree, tree);
extern tree build_type_attribute_qual_variant (tree, tree, int);

/* Return 0 if the attributes for two types are incompatible, 1 if they
   are compatible, and 2 if they are nearly compatible (which causes a
   warning to be generated).  */
extern int comp_type_attributes (const_tree, const_tree);

/* Structure describing an attribute and a function to handle it.  */
struct attribute_spec
{
  /* The name of the attribute (without any leading or trailing __),
     or NULL to mark the end of a table of attributes.  */
  const char *name;
  /* The minimum length of the list of arguments of the attribute.  */
  int min_length;
  /* The maximum length of the list of arguments of the attribute
     (-1 for no maximum).  */
  int max_length;
  /* Whether this attribute requires a DECL.  If it does, it will be passed
     from types of DECLs, function return types and array element types to
     the DECLs, function types and array types respectively; but when
     applied to a type in any other circumstances, it will be ignored with
     a warning.  (If greater control is desired for a given attribute,
     this should be false, and the flags argument to the handler may be
     used to gain greater control in that case.)  */
  bool decl_required;
  /* Whether this attribute requires a type.  If it does, it will be passed
     from a DECL to the type of that DECL.  */
  bool type_required;
  /* Whether this attribute requires a function (or method) type.  If it does,
     it will be passed from a function pointer type to the target type,
     and from a function return type (which is not itself a function
     pointer type) to the function type.  */
  bool function_type_required;
  /* Function to handle this attribute.  NODE points to the node to which
     the attribute is to be applied.  If a DECL, it should be modified in
     place; if a TYPE, a copy should be created.  NAME is the name of the
     attribute (possibly with leading or trailing __).  ARGS is the TREE_LIST
     of the arguments (which may be NULL).  FLAGS gives further information
     about the context of the attribute.  Afterwards, the attributes will
     be added to the DECL_ATTRIBUTES or TYPE_ATTRIBUTES, as appropriate,
     unless *NO_ADD_ATTRS is set to true (which should be done on error,
     as well as in any other cases when the attributes should not be added
     to the DECL or TYPE).  Depending on FLAGS, any attributes to be
     applied to another type or DECL later may be returned;
     otherwise the return value should be NULL_TREE.  This pointer may be
     NULL if no special handling is required beyond the checks implied
     by the rest of this structure.  */
  tree (*handler) (tree *node, tree name, tree args,
		   int flags, bool *no_add_attrs);
  /* Specifies if attribute affects type's identity.  */
  bool affects_type_identity;
};

/* Flags that may be passed in the third argument of decl_attributes, and
   to handler functions for attributes.  */
enum attribute_flags
{
  /* The type passed in is the type of a DECL, and any attributes that
     should be passed in again to be applied to the DECL rather than the
     type should be returned.  */
  ATTR_FLAG_DECL_NEXT = 1,
  /* The type passed in is a function return type, and any attributes that
     should be passed in again to be applied to the function type rather
     than the return type should be returned.  */
  ATTR_FLAG_FUNCTION_NEXT = 2,
  /* The type passed in is an array element type, and any attributes that
     should be passed in again to be applied to the array type rather
     than the element type should be returned.  */
  ATTR_FLAG_ARRAY_NEXT = 4,
  /* The type passed in is a structure, union or enumeration type being
     created, and should be modified in place.  */
  ATTR_FLAG_TYPE_IN_PLACE = 8,
  /* The attributes are being applied by default to a library function whose
     name indicates known behavior, and should be silently ignored if they
     are not in fact compatible with the function type.  */
  ATTR_FLAG_BUILT_IN = 16,
  /* A given attribute has been parsed as a C++-11 attribute.  */
  ATTR_FLAG_CXX11 = 32
};

/* Default versions of target-overridable functions.  */

extern tree merge_decl_attributes (tree, tree);
extern tree merge_type_attributes (tree, tree);

/* This function is a private implementation detail of lookup_attribute()
   and you should never call it directly.  */
extern tree private_lookup_attribute (const char *, size_t, tree);

/* Given an attribute name ATTR_NAME and a list of attributes LIST,
   return a pointer to the attribute's list element if the attribute
   is part of the list, or NULL_TREE if not found.  If the attribute
   appears more than once, this only returns the first occurrence; the
   TREE_CHAIN of the return value should be passed back in if further
   occurrences are wanted.  ATTR_NAME must be in the form 'text' (not
   '__text__').  */

static inline tree
lookup_attribute (const char *attr_name, tree list)
{
  gcc_checking_assert (attr_name[0] != '_');  
  /* In most cases, list is NULL_TREE.  */
  if (list == NULL_TREE)
    return NULL_TREE;
  else
    /* Do the strlen() before calling the out-of-line implementation.
       In most cases attr_name is a string constant, and the compiler
       will optimize the strlen() away.  */
    return private_lookup_attribute (attr_name, strlen (attr_name), list);
}

/* This function is a private implementation detail of
   is_attribute_p() and you should never call it directly.  */
extern bool private_is_attribute_p (const char *, size_t, const_tree);

/* Given an identifier node IDENT and a string ATTR_NAME, return true
   if the identifier node is a valid attribute name for the string.
   ATTR_NAME must be in the form 'text' (not '__text__').  IDENT could
   be the identifier for 'text' or for '__text__'.  */

static inline bool
is_attribute_p (const char *attr_name, const_tree ident)
{
  gcc_checking_assert (attr_name[0] != '_');
  /* Do the strlen() before calling the out-of-line implementation.
     In most cases attr_name is a string constant, and the compiler
     will optimize the strlen() away.  */
  return private_is_attribute_p (attr_name, strlen (attr_name), ident);
}

/* Remove any instances of attribute ATTR_NAME in LIST and return the
   modified list.  ATTR_NAME must be in the form 'text' (not
   '__text__').  */

extern tree remove_attribute (const char *, tree);

/* Given two attributes lists, return a list of their union.  */

extern tree merge_attributes (tree, tree);

#if TARGET_DLLIMPORT_DECL_ATTRIBUTES
/* Given two Windows decl attributes lists, possibly including
   dllimport, return a list of their union .  */
extern tree merge_dllimport_decl_attributes (tree, tree);

/* Handle a "dllimport" or "dllexport" attribute.  */
extern tree handle_dll_attribute (tree *, tree, tree, int, bool *);
#endif

/* Check whether CAND is suitable to be returned from get_qualified_type
   (BASE, TYPE_QUALS).  */

extern bool check_qualified_type (const_tree, const_tree, int);

/* Return a version of the TYPE, qualified as indicated by the
   TYPE_QUALS, if one exists.  If no qualified version exists yet,
   return NULL_TREE.  */

extern tree get_qualified_type (tree, int);

/* Like get_qualified_type, but creates the type if it does not
   exist.  This function never returns NULL_TREE.  */

extern tree build_qualified_type (tree, int);

/* Create a variant of type T with alignment ALIGN.  */

extern tree build_aligned_type (tree, unsigned int);

/* Like build_qualified_type, but only deals with the `const' and
   `volatile' qualifiers.  This interface is retained for backwards
   compatibility with the various front-ends; new code should use
   build_qualified_type instead.  */

#define build_type_variant(TYPE, CONST_P, VOLATILE_P)			\
  build_qualified_type ((TYPE),						\
			((CONST_P) ? TYPE_QUAL_CONST : 0)		\
			| ((VOLATILE_P) ? TYPE_QUAL_VOLATILE : 0))

/* Make a copy of a type node.  */

extern tree build_distinct_type_copy (tree);
extern tree build_variant_type_copy (tree);

/* Finish up a builtin RECORD_TYPE. Give it a name and provide its
   fields. Optionally specify an alignment, and then lay it out.  */

extern void finish_builtin_struct (tree, const char *,
							 tree, tree);

/* Given a ..._TYPE node, calculate the TYPE_SIZE, TYPE_SIZE_UNIT,
   TYPE_ALIGN and TYPE_MODE fields.  If called more than once on one
   node, does nothing except for the first time.  */

extern void layout_type (tree);

/* These functions allow a front-end to perform a manual layout of a
   RECORD_TYPE.  (For instance, if the placement of subsequent fields
   depends on the placement of fields so far.)  Begin by calling
   start_record_layout.  Then, call place_field for each of the
   fields.  Then, call finish_record_layout.  See layout_type for the
   default way in which these functions are used.  */

typedef struct record_layout_info_s
{
  /* The RECORD_TYPE that we are laying out.  */
  tree t;
  /* The offset into the record so far, in bytes, not including bits in
     BITPOS.  */
  tree offset;
  /* The last known alignment of SIZE.  */
  unsigned int offset_align;
  /* The bit position within the last OFFSET_ALIGN bits, in bits.  */
  tree bitpos;
  /* The alignment of the record so far, in bits.  */
  unsigned int record_align;
  /* The alignment of the record so far, ignoring #pragma pack and
     __attribute__ ((packed)), in bits.  */
  unsigned int unpacked_align;
  /* The previous field laid out.  */
  tree prev_field;
  /* The static variables (i.e., class variables, as opposed to
     instance variables) encountered in T.  */
  vec<tree, va_gc> *pending_statics;
  /* Bits remaining in the current alignment group */
  int remaining_in_alignment;
  /* True if we've seen a packed field that didn't have normal
     alignment anyway.  */
  int packed_maybe_necessary;
} *record_layout_info;

extern record_layout_info start_record_layout (tree);
extern tree bit_from_pos (tree, tree);
extern tree byte_from_pos (tree, tree);
extern void pos_from_bit (tree *, tree *, unsigned int, tree);
extern void normalize_offset (tree *, tree *, unsigned int);
extern tree rli_size_unit_so_far (record_layout_info);
extern tree rli_size_so_far (record_layout_info);
extern void normalize_rli (record_layout_info);
extern void place_field (record_layout_info, tree);
extern void compute_record_mode (tree);
extern void finish_record_layout (record_layout_info, int);

/* Given a hashcode and a ..._TYPE node (for which the hashcode was made),
   return a canonicalized ..._TYPE node, so that duplicates are not made.
   How the hash code is computed is up to the caller, as long as any two
   callers that could hash identical-looking type nodes agree.  */

extern tree type_hash_canon (unsigned int, tree);

/* Given a VAR_DECL, PARM_DECL, RESULT_DECL or FIELD_DECL node,
   calculates the DECL_SIZE, DECL_SIZE_UNIT, DECL_ALIGN and DECL_MODE
   fields.  Call this only once for any given decl node.

   Second argument is the boundary that this field can be assumed to
   be starting at (in bits).  Zero means it can be assumed aligned
   on any boundary that may be needed.  */

extern void layout_decl (tree, unsigned);

/* Given a VAR_DECL, PARM_DECL or RESULT_DECL, clears the results of
   a previous call to layout_decl and calls it again.  */

extern void relayout_decl (tree);

/* Return the mode for data of a given size SIZE and mode class CLASS.
   If LIMIT is nonzero, then don't use modes bigger than MAX_FIXED_MODE_SIZE.
   The value is BLKmode if no other mode is found.  This is like
   mode_for_size, but is passed a tree.  */

extern enum machine_mode mode_for_size_tree (const_tree, enum mode_class, int);

/* Return an expr equal to X but certainly not valid as an lvalue.  */

#define non_lvalue(T) non_lvalue_loc (UNKNOWN_LOCATION, T)
extern tree non_lvalue_loc (location_t, tree);

extern tree convert (tree, tree);
extern unsigned int expr_align (const_tree);
extern tree expr_first (tree);
extern tree expr_last (tree);
extern tree size_in_bytes (const_tree);
extern HOST_WIDE_INT int_size_in_bytes (const_tree);
extern HOST_WIDE_INT max_int_size_in_bytes (const_tree);
extern tree tree_expr_size (const_tree);
extern tree bit_position (const_tree);
extern HOST_WIDE_INT int_bit_position (const_tree);
extern tree byte_position (const_tree);
extern HOST_WIDE_INT int_byte_position (const_tree);

/* Define data structures, macros, and functions for handling sizes
   and the various types used to represent sizes.  */

enum size_type_kind
{
  stk_sizetype,		/* Normal representation of sizes in bytes.  */
  stk_ssizetype,	/* Signed representation of sizes in bytes.  */
  stk_bitsizetype,	/* Normal representation of sizes in bits.  */
  stk_sbitsizetype,	/* Signed representation of sizes in bits.  */
  stk_type_kind_last
};

extern GTY(()) tree sizetype_tab[(int) stk_type_kind_last];

#define sizetype sizetype_tab[(int) stk_sizetype]
#define bitsizetype sizetype_tab[(int) stk_bitsizetype]
#define ssizetype sizetype_tab[(int) stk_ssizetype]
#define sbitsizetype sizetype_tab[(int) stk_sbitsizetype]

extern tree size_int_kind (HOST_WIDE_INT, enum size_type_kind);
#define size_binop(CODE,T1,T2)\
   size_binop_loc (UNKNOWN_LOCATION, CODE, T1, T2)
extern tree size_binop_loc (location_t, enum tree_code, tree, tree);
#define size_diffop(T1,T2)\
   size_diffop_loc (UNKNOWN_LOCATION, T1, T2)
extern tree size_diffop_loc (location_t, tree, tree);

#define size_int(L) size_int_kind (L, stk_sizetype)
#define ssize_int(L) size_int_kind (L, stk_ssizetype)
#define bitsize_int(L) size_int_kind (L, stk_bitsizetype)
#define sbitsize_int(L) size_int_kind (L, stk_sbitsizetype)

#define round_up(T,N) round_up_loc (UNKNOWN_LOCATION, T, N)
extern tree round_up_loc (location_t, tree, int);
#define round_down(T,N) round_down_loc (UNKNOWN_LOCATION, T, N)
extern tree round_down_loc (location_t, tree, int);
extern void finalize_size_functions (void);

/* Type for sizes of data-type.  */

#define BITS_PER_UNIT_LOG \
  ((BITS_PER_UNIT > 1) + (BITS_PER_UNIT > 2) + (BITS_PER_UNIT > 4) \
   + (BITS_PER_UNIT > 8) + (BITS_PER_UNIT > 16) + (BITS_PER_UNIT > 32) \
   + (BITS_PER_UNIT > 64) + (BITS_PER_UNIT > 128) + (BITS_PER_UNIT > 256))

/* If nonzero, an upper limit on alignment of structure fields, in bits,  */
extern unsigned int maximum_field_alignment;

/* Concatenate two lists (chains of TREE_LIST nodes) X and Y
   by making the last node in X point to Y.
   Returns X, except if X is 0 returns Y.  */

extern tree chainon (tree, tree);

/* Make a new TREE_LIST node from specified PURPOSE, VALUE and CHAIN.  */

extern tree tree_cons_stat (tree, tree, tree MEM_STAT_DECL);
#define tree_cons(t,q,w) tree_cons_stat (t,q,w MEM_STAT_INFO)

/* Return the last tree node in a chain.  */

extern tree tree_last (tree);

/* Reverse the order of elements in a chain, and return the new head.  */

extern tree nreverse (tree);

/* Returns the length of a chain of nodes
   (number of chain pointers to follow before reaching a null pointer).  */

extern int list_length (const_tree);

/* Returns the number of FIELD_DECLs in a type.  */

extern int fields_length (const_tree);

/* Returns the first FIELD_DECL in a type.  */

extern tree first_field (const_tree);

/* Given an initializer INIT, return TRUE if INIT is zero or some
   aggregate of zeros.  Otherwise return FALSE.  */

extern bool initializer_zerop (const_tree);

/* Given a CONSTRUCTOR CTOR, return the element values as a vector.  */

extern vec<tree, va_gc> *ctor_to_vec (tree);

extern bool categorize_ctor_elements (const_tree, HOST_WIDE_INT *,
				      HOST_WIDE_INT *, bool *);

extern bool complete_ctor_at_level_p (const_tree, HOST_WIDE_INT, const_tree);

/* integer_zerop (tree x) is nonzero if X is an integer constant of value 0.  */

extern int integer_zerop (const_tree);

/* integer_onep (tree x) is nonzero if X is an integer constant of value 1.  */

extern int integer_onep (const_tree);

/* integer_all_onesp (tree x) is nonzero if X is an integer constant
   all of whose significant bits are 1.  */

extern int integer_all_onesp (const_tree);

/* integer_pow2p (tree x) is nonzero is X is an integer constant with
   exactly one bit 1.  */

extern int integer_pow2p (const_tree);

/* integer_nonzerop (tree x) is nonzero if X is an integer constant
   with a nonzero value.  */

extern int integer_nonzerop (const_tree);

extern bool cst_and_fits_in_hwi (const_tree);
extern tree num_ending_zeros (const_tree);

/* fixed_zerop (tree x) is nonzero if X is a fixed-point constant of
   value 0.  */

extern int fixed_zerop (const_tree);

/* staticp (tree x) is nonzero if X is a reference to data allocated
   at a fixed address in memory.  Returns the outermost data.  */

extern tree staticp (tree);

/* save_expr (EXP) returns an expression equivalent to EXP
   but it can be used multiple times within context CTX
   and only evaluate EXP once.  */

extern tree save_expr (tree);

/* Look inside EXPR and into any simple arithmetic operations.  Return
   the innermost non-arithmetic node.  */

extern tree skip_simple_arithmetic (tree);

/* Return which tree structure is used by T.  */

enum tree_node_structure_enum tree_node_structure (const_tree);

/* Return true if EXP contains a PLACEHOLDER_EXPR, i.e. if it represents a
   size or offset that depends on a field within a record.  */

extern bool contains_placeholder_p (const_tree);

/* This macro calls the above function but short-circuits the common
   case of a constant to save time.  Also check for null.  */

#define CONTAINS_PLACEHOLDER_P(EXP) \
  ((EXP) != 0 && ! TREE_CONSTANT (EXP) && contains_placeholder_p (EXP))

/* Return true if any part of the structure of TYPE involves a PLACEHOLDER_EXPR
   directly.  This includes size, bounds, qualifiers (for QUAL_UNION_TYPE) and
   field positions.  */

extern bool type_contains_placeholder_p (tree);

/* Given a tree EXP, find all occurrences of references to fields
   in a PLACEHOLDER_EXPR and place them in vector REFS without
   duplicates.  Also record VAR_DECLs and CONST_DECLs.  Note that
   we assume here that EXP contains only arithmetic expressions
   or CALL_EXPRs with PLACEHOLDER_EXPRs occurring only in their
   argument list.  */

extern void find_placeholder_in_expr (tree, vec<tree> *);

/* This macro calls the above function but short-circuits the common
   case of a constant to save time and also checks for NULL.  */

#define FIND_PLACEHOLDER_IN_EXPR(EXP, V) \
do {					 \
  if((EXP) && !TREE_CONSTANT (EXP))	 \
    find_placeholder_in_expr (EXP, V);	 \
} while (0)

/* Given a tree EXP, a FIELD_DECL F, and a replacement value R,
   return a tree with all occurrences of references to F in a
   PLACEHOLDER_EXPR replaced by R.  Also handle VAR_DECLs and
   CONST_DECLs.  Note that we assume here that EXP contains only
   arithmetic expressions or CALL_EXPRs with PLACEHOLDER_EXPRs
   occurring only in their argument list.  */

extern tree substitute_in_expr (tree, tree, tree);

/* This macro calls the above function but short-circuits the common
   case of a constant to save time and also checks for NULL.  */

#define SUBSTITUTE_IN_EXPR(EXP, F, R) \
  ((EXP) == 0 || TREE_CONSTANT (EXP) ? (EXP) : substitute_in_expr (EXP, F, R))

/* Similar, but look for a PLACEHOLDER_EXPR in EXP and find a replacement
   for it within OBJ, a tree that is an object or a chain of references.  */

extern tree substitute_placeholder_in_expr (tree, tree);

/* This macro calls the above function but short-circuits the common
   case of a constant to save time and also checks for NULL.  */

#define SUBSTITUTE_PLACEHOLDER_IN_EXPR(EXP, OBJ) \
  ((EXP) == 0 || TREE_CONSTANT (EXP) ? (EXP)	\
   : substitute_placeholder_in_expr (EXP, OBJ))

/* variable_size (EXP) is like save_expr (EXP) except that it
   is for the special case of something that is part of a
   variable size for a data type.  It makes special arrangements
   to compute the value at the right time when the data type
   belongs to a function parameter.  */

extern tree variable_size (tree);

/* stabilize_reference (EXP) returns a reference equivalent to EXP
   but it can be used multiple times
   and only evaluate the subexpressions once.  */

extern tree stabilize_reference (tree);

/* Subroutine of stabilize_reference; this is called for subtrees of
   references.  Any expression with side-effects must be put in a SAVE_EXPR
   to ensure that it is only evaluated once.  */

extern tree stabilize_reference_1 (tree);

/* Return EXP, stripped of any conversions to wider types
   in such a way that the result of converting to type FOR_TYPE
   is the same as if EXP were converted to FOR_TYPE.
   If FOR_TYPE is 0, it signifies EXP's type.  */

extern tree get_unwidened (tree, tree);

/* Return OP or a simpler expression for a narrower value
   which can be sign-extended or zero-extended to give back OP.
   Store in *UNSIGNEDP_PTR either 1 if the value should be zero-extended
   or 0 if the value should be sign-extended.  */

extern tree get_narrower (tree, int *);

/* Return true if T is an expression that get_inner_reference handles.  */

static inline bool
handled_component_p (const_tree t)
{
  switch (TREE_CODE (t))
    {
    case COMPONENT_REF:
    case BIT_FIELD_REF:
    case ARRAY_REF:
    case ARRAY_RANGE_REF:
    case REALPART_EXPR:
    case IMAGPART_EXPR:
    case VIEW_CONVERT_EXPR:
      return true;

    default:
      return false;
    }
}

/* Given an expression EXP that is a handled_component_p,
   look for the ultimate containing object, which is returned and specify
   the access position and size.  */

extern tree get_inner_reference (tree, HOST_WIDE_INT *, HOST_WIDE_INT *,
				 tree *, enum machine_mode *, int *, int *,
				 bool);

/* Return a tree of sizetype representing the size, in bytes, of the element
   of EXP, an ARRAY_REF or an ARRAY_RANGE_REF.  */

extern tree array_ref_element_size (tree);

bool array_at_struct_end_p (tree);

/* Return a tree representing the lower bound of the array mentioned in
   EXP, an ARRAY_REF or an ARRAY_RANGE_REF.  */

extern tree array_ref_low_bound (tree);

/* Return a tree representing the upper bound of the array mentioned in
   EXP, an ARRAY_REF or an ARRAY_RANGE_REF.  */

extern tree array_ref_up_bound (tree);

/* Return a tree representing the offset, in bytes, of the field referenced
   by EXP.  This does not include any offset in DECL_FIELD_BIT_OFFSET.  */

extern tree component_ref_field_offset (tree);

/* Given a DECL or TYPE, return the scope in which it was declared, or
   NUL_TREE if there is no containing scope.  */

extern tree get_containing_scope (const_tree);

/* Return the FUNCTION_DECL which provides this _DECL with its context,
   or zero if none.  */
extern tree decl_function_context (const_tree);

/* Return the RECORD_TYPE, UNION_TYPE, or QUAL_UNION_TYPE which provides
   this _DECL with its context, or zero if none.  */
extern tree decl_type_context (const_tree);

/* Return 1 if EXPR is the real constant zero.  */
extern int real_zerop (const_tree);

/* Declare commonly used variables for tree structure.  */

/* Nonzero means lvalues are limited to those valid in pedantic ANSI C.
   Zero means allow extended lvalues.  */

extern int pedantic_lvalues;

/* Points to the FUNCTION_DECL of the function whose body we are reading.  */

extern GTY(()) tree current_function_decl;

/* Nonzero means a FUNC_BEGIN label was emitted.  */
extern GTY(()) const char * current_function_func_begin_label;

/* Iterator for going through the function arguments.  */
typedef struct {
  tree next;			/* TREE_LIST pointing to the next argument */
} function_args_iterator;

/* Initialize the iterator I with arguments from function FNDECL  */

static inline void
function_args_iter_init (function_args_iterator *i, const_tree fntype)
{
  i->next = TYPE_ARG_TYPES (fntype);
}

/* Return a pointer that holds the next argument if there are more arguments to
   handle, otherwise return NULL.  */

static inline tree *
function_args_iter_cond_ptr (function_args_iterator *i)
{
  return (i->next) ? &TREE_VALUE (i->next) : NULL;
}

/* Return the next argument if there are more arguments to handle, otherwise
   return NULL.  */

static inline tree
function_args_iter_cond (function_args_iterator *i)
{
  return (i->next) ? TREE_VALUE (i->next) : NULL_TREE;
}

/* Advance to the next argument.  */
static inline void
function_args_iter_next (function_args_iterator *i)
{
  gcc_assert (i->next != NULL_TREE);
  i->next = TREE_CHAIN (i->next);
}

/* We set BLOCK_SOURCE_LOCATION only to inlined function entry points.  */

static inline bool
inlined_function_outer_scope_p (const_tree block)
{
 return LOCATION_LOCUS (BLOCK_SOURCE_LOCATION (block)) != UNKNOWN_LOCATION;
}

/* Loop over all function arguments of FNTYPE.  In each iteration, PTR is set
   to point to the next tree element.  ITER is an instance of
   function_args_iterator used to iterate the arguments.  */
#define FOREACH_FUNCTION_ARGS_PTR(FNTYPE, PTR, ITER)			\
  for (function_args_iter_init (&(ITER), (FNTYPE));			\
       (PTR = function_args_iter_cond_ptr (&(ITER))) != NULL;		\
       function_args_iter_next (&(ITER)))

/* Loop over all function arguments of FNTYPE.  In each iteration, TREE is set
   to the next tree element.  ITER is an instance of function_args_iterator
   used to iterate the arguments.  */
#define FOREACH_FUNCTION_ARGS(FNTYPE, TREE, ITER)			\
  for (function_args_iter_init (&(ITER), (FNTYPE));			\
       (TREE = function_args_iter_cond (&(ITER))) != NULL_TREE;		\
       function_args_iter_next (&(ITER)))



/* In tree.c */
extern unsigned crc32_string (unsigned, const char *);
extern unsigned crc32_byte (unsigned, char);
extern unsigned crc32_unsigned (unsigned, unsigned);
extern void clean_symbol_name (char *);
extern tree get_file_function_name (const char *);
extern tree get_callee_fndecl (const_tree);
extern int type_num_arguments (const_tree);
extern bool associative_tree_code (enum tree_code);
extern bool commutative_tree_code (enum tree_code);
extern bool commutative_ternary_tree_code (enum tree_code);
extern tree upper_bound_in_type (tree, tree);
extern tree lower_bound_in_type (tree, tree);
extern int operand_equal_for_phi_arg_p (const_tree, const_tree);
extern tree create_artificial_label (location_t);
extern const char *get_name (tree);
extern bool stdarg_p (const_tree);
extern bool prototype_p (tree);
extern bool is_typedef_decl (tree x);
extern bool typedef_variant_p (tree);
extern bool auto_var_in_fn_p (const_tree, const_tree);
extern tree build_low_bits_mask (tree, unsigned);
extern tree tree_strip_nop_conversions (tree);
extern tree tree_strip_sign_nop_conversions (tree);
extern const_tree strip_invariant_refs (const_tree);
extern tree lhd_gcc_personality (void);
extern void assign_assembler_name_if_neeeded (tree);
extern void warn_deprecated_use (tree, tree);


/* In cgraph.c */
extern void change_decl_assembler_name (tree, tree);

/* In gimplify.c */
extern tree unshare_expr (tree);
extern tree unshare_expr_without_location (tree);

/* In stmt.c */

extern void expand_label (tree);
extern void expand_goto (tree);

extern rtx expand_stack_save (void);
extern void expand_stack_restore (tree);
extern void expand_return (tree);

/* In tree-eh.c */
extern void using_eh_for_cleanups (void);

extern bool tree_could_trap_p (tree);
extern bool operation_could_trap_helper_p (enum tree_code, bool, bool, bool,
					   bool, tree, bool *);
extern bool operation_could_trap_p (enum tree_code, bool, bool, tree);
extern bool tree_could_throw_p (tree);

/* Compare and hash for any structure which begins with a canonical
   pointer.  Assumes all pointers are interchangeable, which is sort
   of already assumed by gcc elsewhere IIRC.  */

static inline int
struct_ptr_eq (const void *a, const void *b)
{
  const void * const * x = (const void * const *) a;
  const void * const * y = (const void * const *) b;
  return *x == *y;
}

static inline hashval_t
struct_ptr_hash (const void *a)
{
  const void * const * x = (const void * const *) a;
  return (intptr_t)*x >> 4;
}

/* In fold-const.c */

/* Non-zero if we are folding constants inside an initializer; zero
   otherwise.  */
extern int folding_initializer;

/* Convert between trees and native memory representation.  */
extern int native_encode_expr (const_tree, unsigned char *, int);
extern tree native_interpret_expr (tree, const unsigned char *, int);

/* Fold constants as much as possible in an expression.
   Returns the simplified expression.
   Acts only on the top level of the expression;
   if the argument itself cannot be simplified, its
   subexpressions are not changed.  */

extern tree fold (tree);
#define fold_unary(CODE,T1,T2)\
   fold_unary_loc (UNKNOWN_LOCATION, CODE, T1, T2)
extern tree fold_unary_loc (location_t, enum tree_code, tree, tree);
#define fold_unary_ignore_overflow(CODE,T1,T2)\
   fold_unary_ignore_overflow_loc (UNKNOWN_LOCATION, CODE, T1, T2)
extern tree fold_unary_ignore_overflow_loc (location_t, enum tree_code, tree, tree);
#define fold_binary(CODE,T1,T2,T3)\
   fold_binary_loc (UNKNOWN_LOCATION, CODE, T1, T2, T3)
extern tree fold_binary_loc (location_t, enum tree_code, tree, tree, tree);
#define fold_ternary(CODE,T1,T2,T3,T4)\
   fold_ternary_loc (UNKNOWN_LOCATION, CODE, T1, T2, T3, T4)
extern tree fold_ternary_loc (location_t, enum tree_code, tree, tree, tree, tree);
#define fold_build1(c,t1,t2)\
   fold_build1_stat_loc (UNKNOWN_LOCATION, c, t1, t2 MEM_STAT_INFO)
#define fold_build1_loc(l,c,t1,t2)\
   fold_build1_stat_loc (l, c, t1, t2 MEM_STAT_INFO)
extern tree fold_build1_stat_loc (location_t, enum tree_code, tree,
				  tree MEM_STAT_DECL);
#define fold_build2(c,t1,t2,t3)\
   fold_build2_stat_loc (UNKNOWN_LOCATION, c, t1, t2, t3 MEM_STAT_INFO)
#define fold_build2_loc(l,c,t1,t2,t3)\
   fold_build2_stat_loc (l, c, t1, t2, t3 MEM_STAT_INFO)
extern tree fold_build2_stat_loc (location_t, enum tree_code, tree, tree,
				  tree MEM_STAT_DECL);
#define fold_build3(c,t1,t2,t3,t4)\
   fold_build3_stat_loc (UNKNOWN_LOCATION, c, t1, t2, t3, t4 MEM_STAT_INFO)
#define fold_build3_loc(l,c,t1,t2,t3,t4)\
   fold_build3_stat_loc (l, c, t1, t2, t3, t4 MEM_STAT_INFO)
extern tree fold_build3_stat_loc (location_t, enum tree_code, tree, tree, tree,
				  tree MEM_STAT_DECL);
extern tree fold_build1_initializer_loc (location_t, enum tree_code, tree, tree);
extern tree fold_build2_initializer_loc (location_t, enum tree_code, tree, tree, tree);
extern tree fold_build3_initializer_loc (location_t, enum tree_code, tree, tree, tree, tree);
#define fold_build_call_array(T1,T2,N,T4)\
   fold_build_call_array_loc (UNKNOWN_LOCATION, T1, T2, N, T4)
extern tree fold_build_call_array_loc (location_t, tree, tree, int, tree *);
#define fold_build_call_array_initializer(T1,T2,N,T4)\
   fold_build_call_array_initializer_loc (UNKNOWN_LOCATION, T1, T2, N, T4)
extern tree fold_build_call_array_initializer_loc (location_t, tree, tree, int, tree *);
extern bool fold_convertible_p (const_tree, const_tree);
#define fold_convert(T1,T2)\
   fold_convert_loc(UNKNOWN_LOCATION, T1, T2)
extern tree fold_convert_loc (location_t, tree, tree);
extern tree fold_single_bit_test (location_t, enum tree_code, tree, tree, tree);
extern tree fold_ignored_result (tree);
extern tree fold_abs_const (tree, tree);
extern tree fold_indirect_ref_1 (location_t, tree, tree);
extern void fold_defer_overflow_warnings (void);
extern void fold_undefer_overflow_warnings (bool, const_gimple, int);
extern void fold_undefer_and_ignore_overflow_warnings (void);
extern bool fold_deferring_overflow_warnings_p (void);
extern tree fold_fma (location_t, tree, tree, tree, tree);

enum operand_equal_flag
{
  OEP_ONLY_CONST = 1,
  OEP_PURE_SAME = 2,
  OEP_CONSTANT_ADDRESS_OF = 4
};

extern int operand_equal_p (const_tree, const_tree, unsigned int);
extern int multiple_of_p (tree, const_tree, const_tree);
#define omit_one_operand(T1,T2,T3)\
   omit_one_operand_loc (UNKNOWN_LOCATION, T1, T2, T3)
extern tree omit_one_operand_loc (location_t, tree, tree, tree);
#define omit_two_operands(T1,T2,T3,T4)\
   omit_two_operands_loc (UNKNOWN_LOCATION, T1, T2, T3, T4)
extern tree omit_two_operands_loc (location_t, tree, tree, tree, tree);
#define invert_truthvalue(T)\
   invert_truthvalue_loc(UNKNOWN_LOCATION, T)
extern tree invert_truthvalue_loc (location_t, tree);
extern tree fold_truth_not_expr (location_t, tree);
extern tree fold_unary_to_constant (enum tree_code, tree, tree);
extern tree fold_binary_to_constant (enum tree_code, tree, tree, tree);
extern tree fold_read_from_constant_string (tree);
extern tree int_const_binop (enum tree_code, const_tree, const_tree);
#define build_fold_addr_expr(T)\
        build_fold_addr_expr_loc (UNKNOWN_LOCATION, (T))
extern tree build_fold_addr_expr_loc (location_t, tree);
#define build_fold_addr_expr_with_type(T,TYPE)\
        build_fold_addr_expr_with_type_loc (UNKNOWN_LOCATION, (T), TYPE)
extern tree build_fold_addr_expr_with_type_loc (location_t, tree, tree);
extern tree fold_build_cleanup_point_expr (tree type, tree expr);
extern tree fold_strip_sign_ops (tree);
#define build_fold_indirect_ref(T)\
        build_fold_indirect_ref_loc (UNKNOWN_LOCATION, T)
extern tree build_fold_indirect_ref_loc (location_t, tree);
#define fold_indirect_ref(T)\
        fold_indirect_ref_loc (UNKNOWN_LOCATION, T)
extern tree fold_indirect_ref_loc (location_t, tree);
extern tree build_simple_mem_ref_loc (location_t, tree);
#define build_simple_mem_ref(T)\
	build_simple_mem_ref_loc (UNKNOWN_LOCATION, T)
extern double_int mem_ref_offset (const_tree);
extern tree reference_alias_ptr_type (const_tree);
extern tree build_invariant_address (tree, tree, HOST_WIDE_INT);
extern tree constant_boolean_node (bool, tree);
extern tree div_if_zero_remainder (enum tree_code, const_tree, const_tree);

extern bool tree_swap_operands_p (const_tree, const_tree, bool);
extern enum tree_code swap_tree_comparison (enum tree_code);

extern bool ptr_difference_const (tree, tree, HOST_WIDE_INT *);
extern enum tree_code invert_tree_comparison (enum tree_code, bool);

extern bool tree_expr_nonzero_p (tree);
extern bool tree_unary_nonzero_warnv_p (enum tree_code, tree, tree, bool *);
extern bool tree_binary_nonzero_warnv_p (enum tree_code, tree, tree, tree op1,
                                         bool *);
extern bool tree_single_nonzero_warnv_p (tree, bool *);
extern bool tree_unary_nonnegative_warnv_p (enum tree_code, tree, tree, bool *);
extern bool tree_binary_nonnegative_warnv_p (enum tree_code, tree, tree, tree,
                                             bool *);
extern bool tree_single_nonnegative_warnv_p (tree t, bool *strict_overflow_p);
extern bool tree_invalid_nonnegative_warnv_p (tree t, bool *strict_overflow_p);
extern bool tree_call_nonnegative_warnv_p (tree, tree, tree, tree, bool *);

extern bool tree_expr_nonzero_warnv_p (tree, bool *);

extern bool fold_real_zero_addition_p (const_tree, const_tree, int);
extern tree combine_comparisons (location_t, enum tree_code, enum tree_code,
				 enum tree_code, tree, tree, tree);
extern void debug_fold_checksum (const_tree);

/* Return nonzero if CODE is a tree code that represents a truth value.  */
static inline bool
truth_value_p (enum tree_code code)
{
  return (TREE_CODE_CLASS (code) == tcc_comparison
	  || code == TRUTH_AND_EXPR || code == TRUTH_ANDIF_EXPR
	  || code == TRUTH_OR_EXPR || code == TRUTH_ORIF_EXPR
	  || code == TRUTH_XOR_EXPR || code == TRUTH_NOT_EXPR);
}

/* Return whether TYPE is a type suitable for an offset for
   a POINTER_PLUS_EXPR.  */
static inline bool
ptrofftype_p (tree type)
{
  return (INTEGRAL_TYPE_P (type)
	  && TYPE_PRECISION (type) == TYPE_PRECISION (sizetype)
	  && TYPE_UNSIGNED (type) == TYPE_UNSIGNED (sizetype));
}

/* Return OFF converted to a pointer offset type suitable as offset for
   POINTER_PLUS_EXPR.  Use location LOC for this conversion.  */
static inline tree
convert_to_ptrofftype_loc (location_t loc, tree off)
{
  return fold_convert_loc (loc, sizetype, off);
}
#define convert_to_ptrofftype(t) convert_to_ptrofftype_loc (UNKNOWN_LOCATION, t)

/* Build and fold a POINTER_PLUS_EXPR at LOC offsetting PTR by OFF.  */
static inline tree
fold_build_pointer_plus_loc (location_t loc, tree ptr, tree off)
{
  return fold_build2_loc (loc, POINTER_PLUS_EXPR, TREE_TYPE (ptr),
			  ptr, fold_convert_loc (loc, sizetype, off));
}
#define fold_build_pointer_plus(p,o) \
	fold_build_pointer_plus_loc (UNKNOWN_LOCATION, p, o)

/* Build and fold a POINTER_PLUS_EXPR at LOC offsetting PTR by OFF.  */
static inline tree
fold_build_pointer_plus_hwi_loc (location_t loc, tree ptr, HOST_WIDE_INT off)
{
  return fold_build2_loc (loc, POINTER_PLUS_EXPR, TREE_TYPE (ptr),
			  ptr, size_int (off));
}
#define fold_build_pointer_plus_hwi(p,o) \
	fold_build_pointer_plus_hwi_loc (UNKNOWN_LOCATION, p, o)

/* In builtins.c */
extern bool avoid_folding_inline_builtin (tree);
extern tree fold_call_expr (location_t, tree, bool);
extern tree fold_builtin_fputs (location_t, tree, tree, bool, bool, tree);
extern tree fold_builtin_strcpy (location_t, tree, tree, tree, tree);
extern tree fold_builtin_strncpy (location_t, tree, tree, tree, tree, tree);
extern tree fold_builtin_memory_chk (location_t, tree, tree, tree, tree, tree, tree, bool,
				     enum built_in_function);
extern tree fold_builtin_stxcpy_chk (location_t, tree, tree, tree, tree, tree, bool,
				     enum built_in_function);
extern tree fold_builtin_stxncpy_chk (location_t, tree, tree, tree, tree, tree, bool,
				      enum built_in_function);
extern tree fold_builtin_snprintf_chk (location_t, tree, tree, enum built_in_function);
extern bool fold_builtin_next_arg (tree, bool);
extern enum built_in_function builtin_mathfn_code (const_tree);
extern tree fold_builtin_call_array (location_t, tree, tree, int, tree *);
extern tree build_call_expr_loc_array (location_t, tree, int, tree *);
extern tree build_call_expr_loc_vec (location_t, tree, vec<tree, va_gc> *);
extern tree build_call_expr_loc (location_t, tree, int, ...);
extern tree build_call_expr (tree, int, ...);
extern tree mathfn_built_in (tree, enum built_in_function fn);
extern tree c_strlen (tree, int);
extern tree std_gimplify_va_arg_expr (tree, tree, gimple_seq *, gimple_seq *);
extern tree build_va_arg_indirect_ref (tree);
extern tree build_string_literal (int, const char *);
extern bool validate_arglist (const_tree, ...);
extern rtx builtin_memset_read_str (void *, HOST_WIDE_INT, enum machine_mode);
extern bool is_builtin_fn (tree);
extern bool get_object_alignment_1 (tree, unsigned int *,
				    unsigned HOST_WIDE_INT *);
extern unsigned int get_object_alignment (tree);
extern bool get_pointer_alignment_1 (tree, unsigned int *,
				     unsigned HOST_WIDE_INT *);
extern unsigned int get_pointer_alignment (tree);
extern tree fold_call_stmt (gimple, bool);
extern tree gimple_fold_builtin_snprintf_chk (gimple, tree, enum built_in_function);
extern tree make_range (tree, int *, tree *, tree *, bool *);
extern tree make_range_step (location_t, enum tree_code, tree, tree, tree,
			     tree *, tree *, int *, bool *);
extern tree build_range_check (location_t, tree, tree, int, tree, tree);
extern bool merge_ranges (int *, tree *, tree *, int, tree, tree, int,
			  tree, tree);
extern void set_builtin_user_assembler_name (tree decl, const char *asmspec);
extern bool is_simple_builtin (tree);
extern bool is_inexpensive_builtin (tree);

/* In convert.c */
extern tree strip_float_extensions (tree);

/* In tree.c */
extern int really_constant_p (const_tree);
extern bool decl_address_invariant_p (const_tree);
extern bool decl_address_ip_invariant_p (const_tree);
extern bool int_fits_type_p (const_tree, const_tree);
#ifndef GENERATOR_FILE
extern void get_type_static_bounds (const_tree, mpz_t, mpz_t);
#endif
extern bool variably_modified_type_p (tree, tree);
extern int tree_log2 (const_tree);
extern int tree_floor_log2 (const_tree);
extern int simple_cst_equal (const_tree, const_tree);
extern hashval_t iterative_hash_expr (const_tree, hashval_t);
extern hashval_t iterative_hash_exprs_commutative (const_tree,
                                                   const_tree, hashval_t);
extern hashval_t iterative_hash_host_wide_int (HOST_WIDE_INT, hashval_t);
extern hashval_t iterative_hash_hashval_t (hashval_t, hashval_t);
extern hashval_t iterative_hash_host_wide_int (HOST_WIDE_INT, hashval_t);
extern int compare_tree_int (const_tree, unsigned HOST_WIDE_INT);
extern int type_list_equal (const_tree, const_tree);
extern int chain_member (const_tree, const_tree);
extern tree type_hash_lookup (unsigned int, tree);
extern void type_hash_add (unsigned int, tree);
extern int simple_cst_list_equal (const_tree, const_tree);
extern void dump_tree_statistics (void);
extern void recompute_tree_invariant_for_addr_expr (tree);
extern bool needs_to_live_in_memory (const_tree);
extern tree reconstruct_complex_type (tree, tree);

extern int real_onep (const_tree);
extern int real_twop (const_tree);
extern int real_minus_onep (const_tree);
extern void init_ttree (void);
extern void build_common_tree_nodes (bool, bool);
extern void build_common_builtin_nodes (void);
extern tree build_nonstandard_integer_type (unsigned HOST_WIDE_INT, int);
extern tree build_range_type (tree, tree, tree);
extern tree build_nonshared_range_type (tree, tree, tree);
extern bool subrange_type_for_debug_p (const_tree, tree *, tree *);
extern HOST_WIDE_INT int_cst_value (const_tree);
extern HOST_WIDEST_INT widest_int_cst_value (const_tree);

extern tree tree_block (tree);
extern void tree_set_block (tree, tree);
extern location_t *block_nonartificial_location (tree);
extern location_t tree_nonartificial_location (tree);

extern tree block_ultimate_origin (const_tree);

extern tree get_binfo_at_offset (tree, HOST_WIDE_INT, tree);
extern tree get_ref_base_and_extent (tree, HOST_WIDE_INT *,
				     HOST_WIDE_INT *, HOST_WIDE_INT *);
extern bool contains_bitfld_component_ref_p (const_tree);

/* In tree-nested.c */
extern tree build_addr (tree, tree);

/* In function.c */
extern void expand_main_function (void);
extern void expand_function_end (void);
extern void expand_function_start (tree);
extern void stack_protect_prologue (void);
extern void stack_protect_epilogue (void);
extern void init_dummy_function_start (void);
extern void expand_dummy_function_end (void);
extern void allocate_struct_function (tree, bool);
extern void push_struct_function (tree fndecl);
extern void init_function_start (tree);
extern bool use_register_for_decl (const_tree);
extern void generate_setjmp_warnings (void);
extern void init_temp_slots (void);
extern void free_temp_slots (void);
extern void pop_temp_slots (void);
extern void push_temp_slots (void);
extern void preserve_temp_slots (rtx);
extern int aggregate_value_p (const_tree, const_tree);
extern void push_function_context (void);
extern void pop_function_context (void);
extern gimple_seq gimplify_parameters (void);

/* In print-rtl.c */
#ifdef BUFSIZ
extern void print_rtl (FILE *, const_rtx);
#endif

/* In print-tree.c */
extern void debug_tree (tree);
extern void debug_vec_tree (vec<tree, va_gc> *);
#ifdef BUFSIZ
extern void dump_addr (FILE*, const char *, const void *);
extern void print_node (FILE *, const char *, tree, int);
extern void print_vec_tree (FILE *, const char *, vec<tree, va_gc> *, int);
extern void print_node_brief (FILE *, const char *, const_tree, int);
extern void indent_to (FILE *, int);
#endif

/* In tree-inline.c:  */
extern bool debug_find_tree (tree, tree);
/* This is in tree-inline.c since the routine uses
   data structures from the inliner.  */
extern tree unsave_expr_now (tree);
extern tree build_duplicate_type (tree);

/* In calls.c */

/* Nonzero if this is a call to a function whose return value depends
   solely on its arguments, has no side effects, and does not read
   global memory.  This corresponds to TREE_READONLY for function
   decls.  */
#define ECF_CONST		  (1 << 0)
/* Nonzero if this is a call to "pure" function (like const function,
   but may read memory.  This corresponds to DECL_PURE_P for function
   decls.  */
#define ECF_PURE		  (1 << 1)
/* Nonzero if this is ECF_CONST or ECF_PURE but cannot be proven to no
   infinite loop.  This corresponds to DECL_LOOPING_CONST_OR_PURE_P
   for function decls.*/
#define ECF_LOOPING_CONST_OR_PURE (1 << 2)
/* Nonzero if this call will never return.  */
#define ECF_NORETURN		  (1 << 3)
/* Nonzero if this is a call to malloc or a related function.  */
#define ECF_MALLOC		  (1 << 4)
/* Nonzero if it is plausible that this is a call to alloca.  */
#define ECF_MAY_BE_ALLOCA	  (1 << 5)
/* Nonzero if this is a call to a function that won't throw an exception.  */
#define ECF_NOTHROW		  (1 << 6)
/* Nonzero if this is a call to setjmp or a related function.  */
#define ECF_RETURNS_TWICE	  (1 << 7)
/* Nonzero if this call replaces the current stack frame.  */
#define ECF_SIBCALL		  (1 << 8)
/* Function does not read or write memory (but may have side effects, so
   it does not necessarily fit ECF_CONST).  */
#define ECF_NOVOPS		  (1 << 9)
/* The function does not lead to calls within current function unit.  */
#define ECF_LEAF		  (1 << 10)
/* Nonzero if this call does not affect transactions.  */
#define ECF_TM_PURE		  (1 << 11)
/* Nonzero if this call is into the transaction runtime library.  */
#define ECF_TM_BUILTIN		  (1 << 12)

extern int flags_from_decl_or_type (const_tree);
extern int call_expr_flags (const_tree);
extern void set_call_expr_flags (tree, int);

/* Call argument flags.  */

/* Nonzero if the argument is not dereferenced recursively, thus only
   directly reachable memory is read or written.  */
#define EAF_DIRECT		(1 << 0)
/* Nonzero if memory reached by the argument is not clobbered.  */
#define EAF_NOCLOBBER		(1 << 1)
/* Nonzero if the argument does not escape.  */
#define EAF_NOESCAPE		(1 << 2)
/* Nonzero if the argument is not used by the function.  */
#define EAF_UNUSED		(1 << 3)

/* Call return flags.  */

/* Mask for the argument number that is returned.  Lower two bits of
   the return flags, encodes argument slots zero to three.  */
#define ERF_RETURN_ARG_MASK	(3)
/* Nonzero if the return value is equal to the argument number
   flags & ERF_RETURN_ARG_MASK.  */
#define ERF_RETURNS_ARG		(1 << 2)
/* Nonzero if the return value does not alias with anything.  Functions
   with the malloc attribute have this set on their return value.  */
#define ERF_NOALIAS		(1 << 3)

extern int setjmp_call_p (const_tree);
extern bool gimple_alloca_call_p (const_gimple);
extern bool alloca_call_p (const_tree);
extern bool must_pass_in_stack_var_size (enum machine_mode, const_tree);
extern bool must_pass_in_stack_var_size_or_pad (enum machine_mode, const_tree);

/* In attribs.c.  */

extern const struct attribute_spec *lookup_attribute_spec (const_tree);
extern const struct attribute_spec *lookup_scoped_attribute_spec (const_tree,
								  const_tree);

extern void init_attributes (void);

/* Process the attributes listed in ATTRIBUTES and install them in *NODE,
   which is either a DECL (including a TYPE_DECL) or a TYPE.  If a DECL,
   it should be modified in place; if a TYPE, a copy should be created
   unless ATTR_FLAG_TYPE_IN_PLACE is set in FLAGS.  FLAGS gives further
   information, in the form of a bitwise OR of flags in enum attribute_flags
   from tree.h.  Depending on these flags, some attributes may be
   returned to be applied at a later stage (for example, to apply
   a decl attribute to the declaration rather than to its type).  */
extern tree decl_attributes (tree *, tree, int);

extern bool cxx11_attribute_p (const_tree);

extern tree get_attribute_name (const_tree);

extern tree get_attribute_namespace (const_tree);

extern void apply_tm_attr (tree, tree);

/* In stor-layout.c */
extern void set_min_and_max_values_for_integral_type (tree, int, bool);
extern void fixup_signed_type (tree);
extern void internal_reference_types (void);
extern unsigned int update_alignment_for_field (record_layout_info, tree,
                                                unsigned int);
/* varasm.c */
extern tree tree_output_constant_def (tree);
extern void make_decl_rtl (tree);
extern rtx make_decl_rtl_for_debug (tree);
extern void make_decl_one_only (tree, tree);
extern int supports_one_only (void);
extern void resolve_unique_section (tree, int, int);
extern void mark_referenced (tree);
extern void mark_decl_referenced (tree);
extern void notice_global_symbol (tree);
extern void set_user_assembler_name (tree, const char *);
extern void process_pending_assemble_externals (void);
extern bool decl_replaceable_p (tree);
extern bool decl_binds_to_current_def_p (tree);
extern enum tls_model decl_default_tls_model (const_tree);

/* Declare DECL to be a weak symbol.  */
extern void declare_weak (tree);
/* Merge weak status.  */
extern void merge_weak (tree, tree);
/* Make one symbol an alias for another.  */
extern void assemble_alias (tree, tree);

/* Return nonzero if VALUE is a valid constant-valued expression
   for use in initializing a static variable; one that can be an
   element of a "constant" initializer.

   Return null_pointer_node if the value is absolute;
   if it is relocatable, return the variable that determines the relocation.
   We assume that VALUE has been folded as much as possible;
   therefore, we do not need to check for such things as
   arithmetic-combinations of integers.  */
extern tree initializer_constant_valid_p (tree, tree);

/* Return true if VALUE is a valid constant-valued expression
   for use in initializing a static bit-field; one that can be
   an element of a "constant" initializer.  */
extern bool initializer_constant_valid_for_bitfield_p (tree);

/* Whether a constructor CTOR is a valid static constant initializer if all
   its elements are.  This used to be internal to initializer_constant_valid_p
   and has been exposed to let other functions like categorize_ctor_elements
   evaluate the property while walking a constructor for other purposes.  */

extern bool constructor_static_from_elts_p (const_tree);

/* In stmt.c */
extern void expand_computed_goto (tree);
extern bool parse_output_constraint (const char **, int, int, int,
				     bool *, bool *, bool *);
extern bool parse_input_constraint (const char **, int, int, int, int,
				    const char * const *, bool *, bool *);
extern void expand_asm_stmt (gimple);
extern tree resolve_asm_operand_names (tree, tree, tree, tree);
#ifdef HARD_CONST
/* Silly ifdef to avoid having all includers depend on hard-reg-set.h.  */
extern tree tree_overlaps_hard_reg_set (tree, HARD_REG_SET *);
#endif


/* In tree-inline.c  */

/* The type of a set of already-visited pointers.  Functions for creating
   and manipulating it are declared in pointer-set.h */
struct pointer_set_t;

/* The type of a callback function for walking over tree structure.  */

typedef tree (*walk_tree_fn) (tree *, int *, void *);

/* The type of a callback function that represents a custom walk_tree.  */

typedef tree (*walk_tree_lh) (tree *, int *, tree (*) (tree *, int *, void *),
			      void *, struct pointer_set_t*);

extern tree walk_tree_1 (tree*, walk_tree_fn, void*, struct pointer_set_t*,
			 walk_tree_lh);
extern tree walk_tree_without_duplicates_1 (tree*, walk_tree_fn, void*,
					    walk_tree_lh);
#define walk_tree(a,b,c,d) \
	walk_tree_1 (a, b, c, d, NULL)
#define walk_tree_without_duplicates(a,b,c) \
	walk_tree_without_duplicates_1 (a, b, c, NULL)

/* In emit-rtl.c */
/* Assign the RTX to declaration.  */

extern void set_decl_rtl (tree, rtx);
extern void set_decl_incoming_rtl (tree, rtx, bool);

/* Enum and arrays used for tree allocation stats.
   Keep in sync with tree.c:tree_node_kind_names.  */
typedef enum
{
  d_kind,
  t_kind,
  b_kind,
  s_kind,
  r_kind,
  e_kind,
  c_kind,
  id_kind,
  vec_kind,
  binfo_kind,
  ssa_name_kind,
  constr_kind,
  x_kind,
  lang_decl,
  lang_type,
  omp_clause_kind,
  all_kinds
} tree_node_kind;

extern int tree_node_counts[];
extern int tree_node_sizes[];

/* True if we are in gimple form and the actions of the folders need to
   be restricted.  False if we are not in gimple form and folding is not
   restricted to creating gimple expressions.  */
extern bool in_gimple_form;

/* In gimple.c.  */
extern tree get_base_address (tree t);
extern void mark_addressable (tree);

/* In tree.c.  */

struct GTY(()) tree_map_base {
  tree from;
};

extern int tree_map_base_eq (const void *, const void *);
extern unsigned int tree_map_base_hash (const void *);
extern int tree_map_base_marked_p (const void *);
extern bool list_equal_p (const_tree, const_tree);

/* Map from a tree to another tree.  */

struct GTY(()) tree_map {
  struct tree_map_base base;
  unsigned int hash;
  tree to;
};

#define tree_map_eq tree_map_base_eq
extern unsigned int tree_map_hash (const void *);
#define tree_map_marked_p tree_map_base_marked_p

/* Map from a decl tree to another tree.  */

struct GTY(()) tree_decl_map {
  struct tree_map_base base;
  tree to;
};

#define tree_decl_map_eq tree_map_base_eq
extern unsigned int tree_decl_map_hash (const void *);
#define tree_decl_map_marked_p tree_map_base_marked_p

/* Map from a tree to an int.  */

struct GTY(()) tree_int_map {
  struct tree_map_base base;
  unsigned int to;
};

#define tree_int_map_eq tree_map_base_eq
#define tree_int_map_hash tree_map_base_hash
#define tree_int_map_marked_p tree_map_base_marked_p

/* Map from a tree to initialization/finalization priorities.  */

struct GTY(()) tree_priority_map {
  struct tree_map_base base;
  priority_type init;
  priority_type fini;
};

#define tree_priority_map_eq tree_map_base_eq
#define tree_priority_map_hash tree_map_base_hash
#define tree_priority_map_marked_p tree_map_base_marked_p

/* Map from a decl tree to a tree vector.  */

struct GTY(()) tree_vec_map {
  struct tree_map_base base;
  vec<tree, va_gc> *to;
};

#define tree_vec_map_eq tree_map_base_eq
#define tree_vec_map_hash tree_decl_map_hash
#define tree_vec_map_marked_p tree_map_base_marked_p

/* In tree-ssa.c */

tree target_for_debug_bind (tree);

/* In tree-ssa-address.c.  */
extern tree tree_mem_ref_addr (tree, tree);
extern void copy_ref_info (tree, tree);

/* In tree-vrp.c */
extern bool ssa_name_nonnegative_p (const_tree);

/* In tree-object-size.c.  */
extern void init_object_sizes (void);
extern void fini_object_sizes (void);
extern unsigned HOST_WIDE_INT compute_builtin_object_size (tree, int);

/* In expr.c.  */

/* Determine whether the LEN bytes can be moved by using several move
   instructions.  Return nonzero if a call to move_by_pieces should
   succeed.  */
extern int can_move_by_pieces (unsigned HOST_WIDE_INT, unsigned int);

/* Is it an ADDR_EXPR of a DECL that's not in memory?  */
extern bool addr_expr_of_non_mem_decl_p (tree);

extern unsigned HOST_WIDE_INT highest_pow2_factor (const_tree);
extern tree build_personality_function (const char *);

/* In trans-mem.c.  */
extern tree build_tm_abort_call (location_t, bool);
extern bool is_tm_safe (const_tree);
extern bool is_tm_pure (const_tree);
extern bool is_tm_may_cancel_outer (tree);
extern bool is_tm_ending_fndecl (tree);
extern void record_tm_replacement (tree, tree);
extern void tm_malloc_replacement (tree);

static inline bool
is_tm_safe_or_pure (const_tree x)
{
  return is_tm_safe (x) || is_tm_pure (x);
}

/* In tree-inline.c.  */

void init_inline_once (void);

/* Abstract iterators for CALL_EXPRs.  These static inline definitions
   have to go towards the end of tree.h so that union tree_node is fully
   defined by this point.  */

/* Structure containing iterator state.  */
typedef struct call_expr_arg_iterator_d {
  tree t;	/* the call_expr */
  int n;	/* argument count */
  int i;	/* next argument index */
} call_expr_arg_iterator;

typedef struct const_call_expr_arg_iterator_d {
  const_tree t;	/* the call_expr */
  int n;	/* argument count */
  int i;	/* next argument index */
} const_call_expr_arg_iterator;

/* Initialize the abstract argument list iterator object ITER with the
   arguments from CALL_EXPR node EXP.  */
static inline void
init_call_expr_arg_iterator (tree exp, call_expr_arg_iterator *iter)
{
  iter->t = exp;
  iter->n = call_expr_nargs (exp);
  iter->i = 0;
}

static inline void
init_const_call_expr_arg_iterator (const_tree exp, const_call_expr_arg_iterator *iter)
{
  iter->t = exp;
  iter->n = call_expr_nargs (exp);
  iter->i = 0;
}

/* Return the next argument from abstract argument list iterator object ITER,
   and advance its state.  Return NULL_TREE if there are no more arguments.  */
static inline tree
next_call_expr_arg (call_expr_arg_iterator *iter)
{
  tree result;
  if (iter->i >= iter->n)
    return NULL_TREE;
  result = CALL_EXPR_ARG (iter->t, iter->i);
  iter->i++;
  return result;
}

static inline const_tree
next_const_call_expr_arg (const_call_expr_arg_iterator *iter)
{
  const_tree result;
  if (iter->i >= iter->n)
    return NULL_TREE;
  result = CALL_EXPR_ARG (iter->t, iter->i);
  iter->i++;
  return result;
}

/* Initialize the abstract argument list iterator object ITER, then advance
   past and return the first argument.  Useful in for expressions, e.g.
     for (arg = first_call_expr_arg (exp, &iter); arg;
          arg = next_call_expr_arg (&iter))   */
static inline tree
first_call_expr_arg (tree exp, call_expr_arg_iterator *iter)
{
  init_call_expr_arg_iterator (exp, iter);
  return next_call_expr_arg (iter);
}

static inline const_tree
first_const_call_expr_arg (const_tree exp, const_call_expr_arg_iterator *iter)
{
  init_const_call_expr_arg_iterator (exp, iter);
  return next_const_call_expr_arg (iter);
}

/* Test whether there are more arguments in abstract argument list iterator
   ITER, without changing its state.  */
static inline bool
more_call_expr_args_p (const call_expr_arg_iterator *iter)
{
  return (iter->i < iter->n);
}

static inline bool
more_const_call_expr_args_p (const const_call_expr_arg_iterator *iter)
{
  return (iter->i < iter->n);
}

/* Iterate through each argument ARG of CALL_EXPR CALL, using variable ITER
   (of type call_expr_arg_iterator) to hold the iteration state.  */
#define FOR_EACH_CALL_EXPR_ARG(arg, iter, call)			\
  for ((arg) = first_call_expr_arg ((call), &(iter)); (arg);	\
       (arg) = next_call_expr_arg (&(iter)))

#define FOR_EACH_CONST_CALL_EXPR_ARG(arg, iter, call)			\
  for ((arg) = first_const_call_expr_arg ((call), &(iter)); (arg);	\
       (arg) = next_const_call_expr_arg (&(iter)))

/* Return true if tree node T is a language-specific node.  */
static inline bool
is_lang_specific (tree t)
{
  return TREE_CODE (t) == LANG_TYPE || TREE_CODE (t) >= NUM_TREE_CODES;
}

/* In gimple-low.c.  */
extern bool block_may_fallthru (const_tree);


/* Functional interface to the builtin functions.  */

/* The builtin_info structure holds the FUNCTION_DECL of the standard builtin
   function, and a flag that says if the function is available implicitly, or
   whether the user has to code explicit calls to __builtin_<xxx>.  */

typedef struct GTY(()) builtin_info_type_d {
  tree decl[(int)END_BUILTINS];
  bool implicit_p[(int)END_BUILTINS];
} builtin_info_type;

extern GTY(()) builtin_info_type builtin_info;

/* Valid builtin number.  */
#define BUILTIN_VALID_P(FNCODE) \
  (IN_RANGE ((int)FNCODE, ((int)BUILT_IN_NONE) + 1, ((int) END_BUILTINS) - 1))

/* Return the tree node for an explicit standard builtin function or NULL.  */
static inline tree
builtin_decl_explicit (enum built_in_function fncode)
{
  gcc_checking_assert (BUILTIN_VALID_P (fncode));

  return builtin_info.decl[(size_t)fncode];
}

/* Return the tree node for an implicit builtin function or NULL.  */
static inline tree
builtin_decl_implicit (enum built_in_function fncode)
{
  size_t uns_fncode = (size_t)fncode;
  gcc_checking_assert (BUILTIN_VALID_P (fncode));

  if (!builtin_info.implicit_p[uns_fncode])
    return NULL_TREE;

  return builtin_info.decl[uns_fncode];
}

/* Set explicit builtin function nodes and whether it is an implicit
   function.  */

static inline void
set_builtin_decl (enum built_in_function fncode, tree decl, bool implicit_p)
{
  size_t ufncode = (size_t)fncode;

  gcc_checking_assert (BUILTIN_VALID_P (fncode)
		       && (decl != NULL_TREE || !implicit_p));

  builtin_info.decl[ufncode] = decl;
  builtin_info.implicit_p[ufncode] = implicit_p;
}

/* Set the implicit flag for a builtin function.  */

static inline void
set_builtin_decl_implicit_p (enum built_in_function fncode, bool implicit_p)
{
  size_t uns_fncode = (size_t)fncode;

  gcc_checking_assert (BUILTIN_VALID_P (fncode)
		       && builtin_info.decl[uns_fncode] != NULL_TREE);

  builtin_info.implicit_p[uns_fncode] = implicit_p;
}

/* Return whether the standard builtin function can be used as an explicit
   function.  */

static inline bool
builtin_decl_explicit_p (enum built_in_function fncode)
{
  gcc_checking_assert (BUILTIN_VALID_P (fncode));
  return (builtin_info.decl[(size_t)fncode] != NULL_TREE);
}

/* Return whether the standard builtin function can be used implicitly.  */

static inline bool
builtin_decl_implicit_p (enum built_in_function fncode)
{
  size_t uns_fncode = (size_t)fncode;

  gcc_checking_assert (BUILTIN_VALID_P (fncode));
  return (builtin_info.decl[uns_fncode] != NULL_TREE
	  && builtin_info.implicit_p[uns_fncode]);
}

#endif  /* GCC_TREE_H  */
