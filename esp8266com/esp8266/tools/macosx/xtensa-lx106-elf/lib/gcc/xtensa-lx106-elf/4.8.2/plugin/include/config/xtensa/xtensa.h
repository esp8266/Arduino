/* Definitions of Tensilica's Xtensa target machine for GNU compiler.
   Copyright (C) 2001-2013 Free Software Foundation, Inc.
   Contributed by Bob Wilson (bwilson@tensilica.com) at Tensilica.

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

/* Get Xtensa configuration settings */
#include "xtensa-config.h"

/* External variables defined in xtensa.c.  */

extern unsigned xtensa_current_frame_size;

/* Macros used in the machine description to select various Xtensa
   configuration options.  */
#ifndef XCHAL_HAVE_MUL32_HIGH
#define XCHAL_HAVE_MUL32_HIGH 0
#endif
#ifndef XCHAL_HAVE_RELEASE_SYNC
#define XCHAL_HAVE_RELEASE_SYNC 0
#endif
#ifndef XCHAL_HAVE_S32C1I
#define XCHAL_HAVE_S32C1I 0
#endif
#ifndef XCHAL_HAVE_THREADPTR
#define XCHAL_HAVE_THREADPTR 0
#endif
#define TARGET_BIG_ENDIAN	XCHAL_HAVE_BE
#define TARGET_DENSITY		XCHAL_HAVE_DENSITY
#define TARGET_MAC16		XCHAL_HAVE_MAC16
#define TARGET_MUL16		XCHAL_HAVE_MUL16
#define TARGET_MUL32		XCHAL_HAVE_MUL32
#define TARGET_MUL32_HIGH	XCHAL_HAVE_MUL32_HIGH
#define TARGET_DIV32		XCHAL_HAVE_DIV32
#define TARGET_NSA		XCHAL_HAVE_NSA
#define TARGET_MINMAX		XCHAL_HAVE_MINMAX
#define TARGET_SEXT		XCHAL_HAVE_SEXT
#define TARGET_BOOLEANS		XCHAL_HAVE_BOOLEANS
#define TARGET_HARD_FLOAT	XCHAL_HAVE_FP
#define TARGET_HARD_FLOAT_DIV	XCHAL_HAVE_FP_DIV
#define TARGET_HARD_FLOAT_RECIP	XCHAL_HAVE_FP_RECIP
#define TARGET_HARD_FLOAT_SQRT	XCHAL_HAVE_FP_SQRT
#define TARGET_HARD_FLOAT_RSQRT	XCHAL_HAVE_FP_RSQRT
#define TARGET_ABS		XCHAL_HAVE_ABS
#define TARGET_ADDX		XCHAL_HAVE_ADDX
#define TARGET_RELEASE_SYNC	XCHAL_HAVE_RELEASE_SYNC
#define TARGET_S32C1I		XCHAL_HAVE_S32C1I
#define TARGET_ABSOLUTE_LITERALS XSHAL_USE_ABSOLUTE_LITERALS
#define TARGET_THREADPTR	XCHAL_HAVE_THREADPTR
#define TARGET_WINDOWED_ABI	(XSHAL_ABI == XTHAL_ABI_WINDOWED)

#define TARGET_DEFAULT \
  ((XCHAL_HAVE_L32R	? 0 : MASK_CONST16) |				\
   MASK_SERIALIZE_VOLATILE)

#ifndef HAVE_AS_TLS
#define HAVE_AS_TLS 0
#endif


/* Target CPU builtins.  */
#define TARGET_CPU_CPP_BUILTINS()					\
  do {									\
    builtin_assert ("cpu=xtensa");					\
    builtin_assert ("machine=xtensa");					\
    builtin_define ("__xtensa__");					\
    builtin_define ("__XTENSA__");					\
    builtin_define (TARGET_WINDOWED_ABI ?				\
		    "__XTENSA_WINDOWED_ABI__" : "__XTENSA_CALL0_ABI__");\
    builtin_define (TARGET_BIG_ENDIAN ? "__XTENSA_EB__" : "__XTENSA_EL__"); \
    if (!TARGET_HARD_FLOAT)						\
      builtin_define ("__XTENSA_SOFT_FLOAT__");				\
  } while (0)

#define CPP_SPEC " %(subtarget_cpp_spec) "

#ifndef SUBTARGET_CPP_SPEC
#define SUBTARGET_CPP_SPEC ""
#endif

#define EXTRA_SPECS							\
  { "subtarget_cpp_spec", SUBTARGET_CPP_SPEC },

/* Target machine storage layout */

/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields.  */
#define BITS_BIG_ENDIAN (TARGET_BIG_ENDIAN != 0)

/* Define this if most significant byte of a word is the lowest numbered.  */
#define BYTES_BIG_ENDIAN (TARGET_BIG_ENDIAN != 0)

/* Define this if most significant word of a multiword number is the lowest.  */
#define WORDS_BIG_ENDIAN (TARGET_BIG_ENDIAN != 0)

#define MAX_BITS_PER_WORD 32

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD 4
#define MIN_UNITS_PER_WORD 4

/* Width of a floating point register.  */
#define UNITS_PER_FPREG 4

/* Size in bits of various types on the target machine.  */
#define INT_TYPE_SIZE 32
#define SHORT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64
#define FLOAT_TYPE_SIZE 32
#define DOUBLE_TYPE_SIZE 64
#define LONG_DOUBLE_TYPE_SIZE 64

/* Allocation boundary (in *bits*) for storing pointers in memory.  */
#define POINTER_BOUNDARY 32

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */
#define PARM_BOUNDARY 32

/* Allocation boundary (in *bits*) for the code of a function.  */
#define FUNCTION_BOUNDARY 32

/* Alignment of field after 'int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 32

/* Every structure's size must be a multiple of this.  */
#define STRUCTURE_SIZE_BOUNDARY 8

/* There is no point aligning anything to a rounder boundary than this.  */
#define BIGGEST_ALIGNMENT 128

/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* Promote integer modes smaller than a word to SImode.  Set UNSIGNEDP
   for QImode, because there is no 8-bit load from memory with sign
   extension.  Otherwise, leave UNSIGNEDP alone, since Xtensa has 16-bit
   loads both with and without sign extension.  */
#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)				\
  do {									\
    if (GET_MODE_CLASS (MODE) == MODE_INT				\
	&& GET_MODE_SIZE (MODE) < UNITS_PER_WORD)			\
      {									\
	if ((MODE) == QImode)						\
	  (UNSIGNEDP) = 1;						\
	(MODE) = SImode;						\
      }									\
  } while (0)

/* Imitate the way many other C compilers handle alignment of
   bitfields and the structures that contain them.  */
#define PCC_BITFIELD_TYPE_MATTERS 1

/* Align string constants and constructors to at least a word boundary.
   The typical use of this macro is to increase alignment for string
   constants to be word aligned so that 'strcpy' calls that copy
   constants can be done inline.  */
#define CONSTANT_ALIGNMENT(EXP, ALIGN)					\
  ((TREE_CODE (EXP) == STRING_CST || TREE_CODE (EXP) == CONSTRUCTOR)	\
   && (ALIGN) < BITS_PER_WORD						\
	? BITS_PER_WORD							\
	: (ALIGN))

/* Align arrays, unions and records to at least a word boundary.
   One use of this macro is to increase alignment of medium-size
   data to make it all fit in fewer cache lines.  Another is to
   cause character arrays to be word-aligned so that 'strcpy' calls
   that copy constants to character arrays can be done inline.  */
#undef DATA_ALIGNMENT
#define DATA_ALIGNMENT(TYPE, ALIGN)					\
  ((((ALIGN) < BITS_PER_WORD)						\
    && (TREE_CODE (TYPE) == ARRAY_TYPE					\
	|| TREE_CODE (TYPE) == UNION_TYPE				\
	|| TREE_CODE (TYPE) == RECORD_TYPE)) ? BITS_PER_WORD : (ALIGN))

/* Operations between registers always perform the operation
   on the full register even if a narrower mode is specified.  */
#define WORD_REGISTER_OPERATIONS

/* Xtensa loads are zero-extended by default.  */
#define LOAD_EXTEND_OP(MODE) ZERO_EXTEND

/* Standard register usage.  */

/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers.

   The fake frame pointer and argument pointer will never appear in
   the generated code, since they will always be eliminated and replaced
   by either the stack pointer or the hard frame pointer.

   0 - 15	AR[0] - AR[15]
   16		FRAME_POINTER (fake = initial sp)
   17		ARG_POINTER (fake = initial sp + framesize)
   18		BR[0] for floating-point CC
   19 - 34	FR[0] - FR[15]
   35		MAC16 accumulator */

#define FIRST_PSEUDO_REGISTER 36

/* Return the stabs register number to use for REGNO.  */
#define DBX_REGISTER_NUMBER(REGNO) xtensa_dbx_register_number (REGNO)

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.  */
#define FIXED_REGISTERS							\
{									\
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,			\
  1, 1, 0,								\
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,			\
  0,									\
}

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you like.  */
#if TARGET_WINDOWED_ABI
#define CALL_USED_REGISTERS						\
{									\
  1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,			\
  1, 1, 1,								\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			\
  1,									\
}
#else
#define CALL_USED_REGISTERS						\
{									\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,			\
  1, 1, 1,								\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			\
  1,									\
}
#endif

/* For non-leaf procedures on Xtensa processors, the allocation order
   is as specified below by REG_ALLOC_ORDER.  For leaf procedures, we
   want to use the lowest numbered registers first to minimize
   register window overflows.  However, local-alloc is not smart
   enough to consider conflicts with incoming arguments.  If an
   incoming argument in a2 is live throughout the function and
   local-alloc decides to use a2, then the incoming argument must
   either be spilled or copied to another register.  To get around
   this, we define ADJUST_REG_ALLOC_ORDER to redefine
   reg_alloc_order for leaf functions such that lowest numbered
   registers are used first with the exception that the incoming
   argument registers are not used until after other register choices
   have been exhausted.  */

#if TARGET_WINDOWED_ABI
#define REG_ALLOC_ORDER \
{  8,  9, 10, 11, 12, 13, 14, 15,  7,  6,  5,  4,  3,  2, \
  18, \
  19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, \
   0,  1, 16, 17, \
  35, \
}
#else
#define REG_ALLOC_ORDER \
{  8,  9, 10, 11,  7,  6,  5,  4,  3,  2, 12, 13, 14, 15, \
  18, \
  19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, \
   0,  1, 16, 17, \
  35, \
}
#endif


#define ADJUST_REG_ALLOC_ORDER order_regs_for_local_alloc ()

/* For Xtensa, the only point of this is to prevent GCC from otherwise
   giving preference to call-used registers.  To minimize window
   overflows for the AR registers, we want to give preference to the
   lower-numbered AR registers.  For other register files, which are
   not windowed, we still prefer call-used registers, if there are any.  */
extern const char xtensa_leaf_regs[FIRST_PSEUDO_REGISTER];
#define LEAF_REGISTERS xtensa_leaf_regs

/* For Xtensa, no remapping is necessary, but this macro must be
   defined if LEAF_REGISTERS is defined.  */
#define LEAF_REG_REMAP(REGNO) (REGNO)

/* This must be declared if LEAF_REGISTERS is set.  */
extern int leaf_function;

/* Internal macros to classify a register number.  */

/* 16 address registers + fake registers */
#define GP_REG_FIRST 0
#define GP_REG_LAST  17
#define GP_REG_NUM   (GP_REG_LAST - GP_REG_FIRST + 1)

/* Coprocessor registers */
#define BR_REG_FIRST 18
#define BR_REG_LAST  18 
#define BR_REG_NUM   (BR_REG_LAST - BR_REG_FIRST + 1)

/* 16 floating-point registers */
#define FP_REG_FIRST 19
#define FP_REG_LAST  34
#define FP_REG_NUM   (FP_REG_LAST - FP_REG_FIRST + 1)

/* MAC16 accumulator */
#define ACC_REG_FIRST 35
#define ACC_REG_LAST 35
#define ACC_REG_NUM  (ACC_REG_LAST - ACC_REG_FIRST + 1)

#define GP_REG_P(REGNO) ((unsigned) ((REGNO) - GP_REG_FIRST) < GP_REG_NUM)
#define BR_REG_P(REGNO) ((unsigned) ((REGNO) - BR_REG_FIRST) < BR_REG_NUM)
#define FP_REG_P(REGNO) ((unsigned) ((REGNO) - FP_REG_FIRST) < FP_REG_NUM)
#define ACC_REG_P(REGNO) ((unsigned) ((REGNO) - ACC_REG_FIRST) < ACC_REG_NUM)

/* Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.  */
#define HARD_REGNO_NREGS(REGNO, MODE)					\
  (FP_REG_P (REGNO) ?							\
	((GET_MODE_SIZE (MODE) + UNITS_PER_FPREG - 1) / UNITS_PER_FPREG) : \
	((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD))

/* Value is 1 if hard register REGNO can hold a value of machine-mode
   MODE.  */
extern char xtensa_hard_regno_mode_ok[][FIRST_PSEUDO_REGISTER];

#define HARD_REGNO_MODE_OK(REGNO, MODE)					\
  xtensa_hard_regno_mode_ok[(int) (MODE)][(REGNO)]

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */
#define MODES_TIEABLE_P(MODE1, MODE2)					\
  ((GET_MODE_CLASS (MODE1) == MODE_FLOAT ||				\
    GET_MODE_CLASS (MODE1) == MODE_COMPLEX_FLOAT)			\
   == (GET_MODE_CLASS (MODE2) == MODE_FLOAT ||				\
       GET_MODE_CLASS (MODE2) == MODE_COMPLEX_FLOAT))

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM (GP_REG_FIRST + 1)

/* Base register for access to local variables of the function.  */
#define HARD_FRAME_POINTER_REGNUM (GP_REG_FIRST + \
				   (TARGET_WINDOWED_ABI ? 7 : 15))

/* The register number of the frame pointer register, which is used to
   access automatic variables in the stack frame.  For Xtensa, this
   register never appears in the output.  It is always eliminated to
   either the stack pointer or the hard frame pointer.  */
#define FRAME_POINTER_REGNUM (GP_REG_FIRST + 16)

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM (GP_REG_FIRST + 17)

/* For now we don't try to use the full set of boolean registers.  Without
   software pipelining of FP operations, there's not much to gain and it's
   a real pain to get them reloaded.  */
#define FPCC_REGNUM (BR_REG_FIRST + 0)

/* It is as good or better to call a constant function address than to
   call an address kept in a register.  */
#define NO_FUNCTION_CSE 1

/* Xtensa processors have "register windows".  GCC does not currently
   take advantage of the possibility for variable-sized windows; instead,
   we use a fixed window size of 8.  */

#if TARGET_WINDOWED_ABI
#define INCOMING_REGNO(OUT)						\
  ((GP_REG_P (OUT) &&							\
    ((unsigned) ((OUT) - GP_REG_FIRST) >= WINDOW_SIZE)) ?		\
   (OUT) - WINDOW_SIZE : (OUT))

#define OUTGOING_REGNO(IN)						\
  ((GP_REG_P (IN) &&							\
    ((unsigned) ((IN) - GP_REG_FIRST) < WINDOW_SIZE)) ?			\
   (IN) + WINDOW_SIZE : (IN))
#else
#define INCOMING_REGNO(OUT) (OUT)
#define OUTGOING_REGNO(IN) (IN)
#endif


/* Define the classes of registers for register constraints in the
   machine description.  */
enum reg_class
{
  NO_REGS,			/* no registers in set */
  BR_REGS,			/* coprocessor boolean registers */
  FP_REGS,			/* floating point registers */
  ACC_REG,			/* MAC16 accumulator */
  SP_REG,			/* sp register (aka a1) */
  RL_REGS,			/* preferred reload regs (not sp or fp) */
  GR_REGS,			/* integer registers except sp */
  AR_REGS,			/* all integer registers */
  ALL_REGS,			/* all registers */
  LIM_REG_CLASSES		/* max value + 1 */
};

#define N_REG_CLASSES (int) LIM_REG_CLASSES

#define GENERAL_REGS AR_REGS

/* An initializer containing the names of the register classes as C
   string constants.  These names are used in writing some of the
   debugging dumps.  */
#define REG_CLASS_NAMES							\
{									\
  "NO_REGS",								\
  "BR_REGS",								\
  "FP_REGS",								\
  "ACC_REG",								\
  "SP_REG",								\
  "RL_REGS",								\
  "GR_REGS",								\
  "AR_REGS",								\
  "ALL_REGS"								\
}

/* Contents of the register classes.  The Nth integer specifies the
   contents of class N.  The way the integer MASK is interpreted is
   that register R is in the class if 'MASK & (1 << R)' is 1.  */
#if TARGET_WINDOWED_ABI
#define REG_CLASS_CONTENTS \
{ \
  { 0x00000000, 0x00000000 }, /* no registers */ \
  { 0x00040000, 0x00000000 }, /* coprocessor boolean registers */ \
  { 0xfff80000, 0x00000007 }, /* floating-point registers */ \
  { 0x00000000, 0x00000008 }, /* MAC16 accumulator */ \
  { 0x00000002, 0x00000000 }, /* stack pointer register */ \
  { 0x0000ff7d, 0x00000000 }, /* preferred reload registers */ \
  { 0x0000fffd, 0x00000000 }, /* general-purpose registers */ \
  { 0x0003ffff, 0x00000000 }, /* integer registers */ \
  { 0xffffffff, 0x0000000f }  /* all registers */ \
}
#else
#define REG_CLASS_CONTENTS \
{ \
  { 0x00000000, 0x00000000 }, /* no registers */ \
  { 0x00040000, 0x00000000 }, /* coprocessor boolean registers */ \
  { 0xfff80000, 0x00000007 }, /* floating-point registers */ \
  { 0x00000000, 0x00000008 }, /* MAC16 accumulator */ \
  { 0x00000002, 0x00000000 }, /* stack pointer register */ \
  { 0x00007ffd, 0x00000000 }, /* preferred reload registers */ \
  { 0x0000fffd, 0x00000000 }, /* general-purpose registers */ \
  { 0x0003ffff, 0x00000000 }, /* integer registers */ \
  { 0xffffffff, 0x0000000f }  /* all registers */ \
}
#endif

/* A C expression whose value is a register class containing hard
   register REGNO.  In general there is more that one such class;
   choose a class which is "minimal", meaning that no smaller class
   also contains the register.  */
extern const enum reg_class xtensa_regno_to_class[FIRST_PSEUDO_REGISTER];

#define REGNO_REG_CLASS(REGNO) xtensa_regno_to_class[ (REGNO) ]

/* Use the Xtensa AR register file for base registers.
   No index registers.  */
#define BASE_REG_CLASS AR_REGS
#define INDEX_REG_CLASS NO_REGS

/* The small_register_classes_for_mode_p hook must always return true for
   Xtrnase, because all of the 16 AR registers may be explicitly used in
   the RTL, as either incoming or outgoing arguments.  */
#define TARGET_SMALL_REGISTER_CLASSES_FOR_MODE_P hook_bool_mode_true

/* Stack layout; function entry, exit and calling.  */

#define STACK_GROWS_DOWNWARD

/* Offset within stack frame to start allocating local variables at.  */
#define STARTING_FRAME_OFFSET						\
  crtl->outgoing_args_size

/* The ARG_POINTER and FRAME_POINTER are not real Xtensa registers, so
   they are eliminated to either the stack pointer or hard frame pointer.  */
#define ELIMINABLE_REGS							\
{{ ARG_POINTER_REGNUM,		STACK_POINTER_REGNUM},			\
 { ARG_POINTER_REGNUM,		HARD_FRAME_POINTER_REGNUM},		\
 { FRAME_POINTER_REGNUM,	STACK_POINTER_REGNUM},			\
 { FRAME_POINTER_REGNUM,	HARD_FRAME_POINTER_REGNUM}}

/* Specify the initial difference between the specified pair of registers.  */
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  do {									\
    compute_frame_size (get_frame_size ());				\
    switch (FROM)							\
      {									\
      case FRAME_POINTER_REGNUM:					\
        (OFFSET) = 0;							\
	break;								\
      case ARG_POINTER_REGNUM:						\
        (OFFSET) = xtensa_current_frame_size;				\
	break;								\
      default:								\
	gcc_unreachable ();						\
      }									\
  } while (0)

/* If defined, the maximum amount of space required for outgoing
   arguments will be computed and placed into the variable
   'crtl->outgoing_args_size'.  No space will be pushed
   onto the stack for each call; instead, the function prologue
   should increase the stack frame size by this amount.  */
#define ACCUMULATE_OUTGOING_ARGS 1

/* Offset from the argument pointer register to the first argument's
   address.  On some machines it may depend on the data type of the
   function.  If 'ARGS_GROW_DOWNWARD', this is the offset to the
   location above the first argument's address.  */
#define FIRST_PARM_OFFSET(FNDECL) 0

/* Align stack frames on 128 bits for Xtensa.  This is necessary for
   128-bit datatypes defined in TIE (e.g., for Vectra).  */
#define STACK_BOUNDARY 128

/* Use a fixed register window size of 8.  */
#define WINDOW_SIZE (TARGET_WINDOWED_ABI ? 8 : 0)

/* Symbolic macros for the registers used to return integer, floating
   point, and values of coprocessor and user-defined modes.  */
#define GP_RETURN (GP_REG_FIRST + 2 + WINDOW_SIZE)
#define GP_OUTGOING_RETURN (GP_REG_FIRST + 2)

/* Symbolic macros for the first/last argument registers.  */
#define GP_ARG_FIRST (GP_REG_FIRST + 2)
#define GP_ARG_LAST  (GP_REG_FIRST + 7)
#define GP_OUTGOING_ARG_FIRST (GP_REG_FIRST + 2 + WINDOW_SIZE)
#define GP_OUTGOING_ARG_LAST  (GP_REG_FIRST + 7 + WINDOW_SIZE)

#define MAX_ARGS_IN_REGISTERS 6

/* Don't worry about compatibility with PCC.  */
#define DEFAULT_PCC_STRUCT_RETURN 0

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which function arguments are sometimes passed.  This
   does *not* include implicit arguments such as the static chain and
   the structure-value address.  On many machines, no registers can be
   used for this purpose since all function arguments are pushed on
   the stack.  */
#define FUNCTION_ARG_REGNO_P(N)						\
  ((N) >= GP_OUTGOING_ARG_FIRST && (N) <= GP_OUTGOING_ARG_LAST)

/* Record the number of argument words seen so far, along with a flag to
   indicate whether these are incoming arguments.  (FUNCTION_INCOMING_ARG
   is used for both incoming and outgoing args, so a separate flag is
   needed.  */
typedef struct xtensa_args
{
  int arg_words;
  int incoming;
} CUMULATIVE_ARGS;

#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, INDIRECT, N_NAMED_ARGS) \
  init_cumulative_args (&CUM, 0)

#define INIT_CUMULATIVE_INCOMING_ARGS(CUM, FNTYPE, LIBNAME)		\
  init_cumulative_args (&CUM, 1)

/* Profiling Xtensa code is typically done with the built-in profiling
   feature of Tensilica's instruction set simulator, which does not
   require any compiler support.  Profiling code on a real (i.e.,
   non-simulated) Xtensa processor is currently only supported by
   GNU/Linux with glibc.  The glibc version of _mcount doesn't require
   counter variables.  The _mcount function needs the current PC and
   the current return address to identify an arc in the call graph.
   Pass the current return address as the first argument; the current
   PC is available as a0 in _mcount's register window.  Both of these
   values contain window size information in the two most significant
   bits; we assume that _mcount will mask off those bits.  The call to
   _mcount uses a window size of 8 to make sure that it doesn't clobber
   any incoming argument values.  */

#define NO_PROFILE_COUNTERS	1

#define FUNCTION_PROFILER(FILE, LABELNO) \
  do {									\
    fprintf (FILE, "\t%s\ta10, a0\n", TARGET_DENSITY ? "mov.n" : "mov"); \
    if (flag_pic)							\
      {									\
	fprintf (FILE, "\tmovi\ta%d, _mcount@PLT\n", WINDOW_SIZE);	\
	fprintf (FILE, "\tcallx%d\ta%d\n", WINDOW_SIZE, WINDOW_SIZE);	\
      }									\
    else								\
      fprintf (FILE, "\tcall%d\t_mcount\n", WINDOW_SIZE);		\
  } while (0)

/* Stack pointer value doesn't matter at exit.  */
#define EXIT_IGNORE_STACK 1

/* Size in bytes of the trampoline, as an integer.  Make sure this is
   a multiple of TRAMPOLINE_ALIGNMENT to avoid -Wpadded warnings.  */
#define TRAMPOLINE_SIZE (TARGET_WINDOWED_ABI ? \
			 (TARGET_CONST16 || TARGET_ABSOLUTE_LITERALS ? \
			  60 : 52) : \
			 (TARGET_CONST16 || TARGET_ABSOLUTE_LITERALS ? \
			  32 : 24))

/* Alignment required for trampolines, in bits.  */
#define TRAMPOLINE_ALIGNMENT 32

/* If defined, a C expression that produces the machine-specific code
   to setup the stack so that arbitrary frames can be accessed.

   On Xtensa, a stack back-trace must always begin from the stack pointer,
   so that the register overflow save area can be located.  However, the
   stack-walking code in GCC always begins from the hard_frame_pointer
   register, not the stack pointer.  The frame pointer is usually equal
   to the stack pointer, but the __builtin_return_address and
   __builtin_frame_address functions will not work if count > 0 and
   they are called from a routine that uses alloca.  These functions
   are not guaranteed to work at all if count > 0 so maybe that is OK.

   A nicer solution would be to allow the architecture-specific files to
   specify whether to start from the stack pointer or frame pointer.  That
   would also allow us to skip the machine->accesses_prev_frame stuff that
   we currently need to ensure that there is a frame pointer when these
   builtin functions are used.  */

#define SETUP_FRAME_ADDRESSES  xtensa_setup_frame_addresses

/* A C expression whose value is RTL representing the address in a
   stack frame where the pointer to the caller's frame is stored.
   Assume that FRAMEADDR is an RTL expression for the address of the
   stack frame itself.

   For Xtensa, there is no easy way to get the frame pointer if it is
   not equivalent to the stack pointer.  Moreover, the result of this
   macro is used for continuing to walk back up the stack, so it must
   return the stack pointer address.  Thus, there is some inconsistency
   here in that __builtin_frame_address will return the frame pointer
   when count == 0 and the stack pointer when count > 0.  */

#define DYNAMIC_CHAIN_ADDRESS(frame)					\
  gen_rtx_PLUS (Pmode, frame, GEN_INT (-3 * UNITS_PER_WORD))

/* Define this if the return address of a particular stack frame is
   accessed from the frame pointer of the previous stack frame.  */
#if TARGET_WINDOWED_ABI
#define RETURN_ADDR_IN_PREVIOUS_FRAME
#endif

/* A C expression whose value is RTL representing the value of the
   return address for the frame COUNT steps up from the current
   frame, after the prologue.  */
#define RETURN_ADDR_RTX  xtensa_return_addr

/* Addressing modes, and classification of registers for them.  */

/* C expressions which are nonzero if register number NUM is suitable
   for use as a base or index register in operand addresses.  */

#define REGNO_OK_FOR_INDEX_P(NUM) 0
#define REGNO_OK_FOR_BASE_P(NUM) \
  (GP_REG_P (NUM) || GP_REG_P ((unsigned) reg_renumber[NUM]))

/* C expressions that are nonzero if X (assumed to be a `reg' RTX) is
   valid for use as a base or index register.  */

#ifdef REG_OK_STRICT
#define REG_OK_STRICT_FLAG 1
#else
#define REG_OK_STRICT_FLAG 0
#endif

#define BASE_REG_P(X, STRICT)						\
  ((!(STRICT) && REGNO (X) >= FIRST_PSEUDO_REGISTER)			\
   || REGNO_OK_FOR_BASE_P (REGNO (X)))

#define REG_OK_FOR_INDEX_P(X) 0
#define REG_OK_FOR_BASE_P(X) BASE_REG_P (X, REG_OK_STRICT_FLAG)

/* Maximum number of registers that can appear in a valid memory address.  */
#define MAX_REGS_PER_ADDRESS 1

/* A C expression that is 1 if the RTX X is a constant which is a
   valid address.  This is defined to be the same as 'CONSTANT_P (X)',
   but rejecting CONST_DOUBLE.  */
#define CONSTANT_ADDRESS_P(X)						\
  ((GET_CODE (X) == LABEL_REF || GET_CODE (X) == SYMBOL_REF		\
    || GET_CODE (X) == CONST_INT || GET_CODE (X) == HIGH		\
    || (GET_CODE (X) == CONST)))

/* A C expression that is nonzero if X is a legitimate immediate
   operand on the target machine when generating position independent
   code.  */
#define LEGITIMATE_PIC_OPERAND_P(X)					\
  ((GET_CODE (X) != SYMBOL_REF						\
    || (SYMBOL_REF_LOCAL_P (X) && !SYMBOL_REF_EXTERNAL_P (X)))		\
   && GET_CODE (X) != LABEL_REF						\
   && GET_CODE (X) != CONST)

/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE (SImode)

/* Define this as 1 if 'char' should by default be signed; else as 0.  */
#define DEFAULT_SIGNED_CHAR 0

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX 4
#define MAX_MOVE_MAX 4

/* Prefer word-sized loads.  */
#define SLOW_BYTE_ACCESS 1

/* Shift instructions ignore all but the low-order few bits.  */
#define SHIFT_COUNT_TRUNCATED 1

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

#define CLZ_DEFINED_VALUE_AT_ZERO(MODE, VALUE)  ((VALUE) = 32, 1)
#define CTZ_DEFINED_VALUE_AT_ZERO(MODE, VALUE)  ((VALUE) = -1, 1)

/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode SImode

/* A function address in a call instruction is a word address (for
   indexing purposes) so give the MEM rtx a words's mode.  */
#define FUNCTION_MODE SImode

#define BRANCH_COST(speed_p, predictable_p) 3

/* How to refer to registers in assembler output.
   This sequence is indexed by compiler's hard-register-number (see above).  */
#define REGISTER_NAMES							\
{									\
  "a0",   "sp",   "a2",   "a3",   "a4",   "a5",   "a6",   "a7",		\
  "a8",   "a9",   "a10",  "a11",  "a12",  "a13",  "a14",  "a15",	\
  "fp",   "argp", "b0",							\
  "f0",   "f1",   "f2",   "f3",   "f4",   "f5",   "f6",   "f7",		\
  "f8",   "f9",   "f10",  "f11",  "f12",  "f13",  "f14",  "f15",	\
  "acc"									\
}

/* If defined, a C initializer for an array of structures containing a
   name and a register number.  This macro defines additional names
   for hard registers, thus allowing the 'asm' option in declarations
   to refer to registers using alternate names.  */
#define ADDITIONAL_REGISTER_NAMES					\
{									\
  { "a1",	 1 + GP_REG_FIRST }					\
}

#define PRINT_OPERAND(FILE, X, CODE) print_operand (FILE, X, CODE)
#define PRINT_OPERAND_ADDRESS(FILE, ADDR) print_operand_address (FILE, ADDR)

/* Globalizing directive for a label.  */
#define GLOBAL_ASM_OP "\t.global\t"

/* Declare an uninitialized external linkage data object.  */
#define ASM_OUTPUT_ALIGNED_BSS(FILE, DECL, NAME, SIZE, ALIGN) \
  asm_output_aligned_bss (FILE, DECL, NAME, SIZE, ALIGN)

/* This is how to output an element of a case-vector that is absolute.  */
#define ASM_OUTPUT_ADDR_VEC_ELT(STREAM, VALUE)				\
  fprintf (STREAM, "%s%sL%u\n", integer_asm_op (4, TRUE),		\
	   LOCAL_LABEL_PREFIX, VALUE)

/* This is how to output an element of a case-vector that is relative.
   This is used for pc-relative code.  */
#define ASM_OUTPUT_ADDR_DIFF_ELT(STREAM, BODY, VALUE, REL)		\
  do {									\
    fprintf (STREAM, "%s%sL%u-%sL%u\n",	integer_asm_op (4, TRUE),	\
	     LOCAL_LABEL_PREFIX, (VALUE),				\
	     LOCAL_LABEL_PREFIX, (REL));				\
  } while (0)

/* This is how to output an assembler line that says to advance the
   location counter to a multiple of 2**LOG bytes.  */
#define ASM_OUTPUT_ALIGN(STREAM, LOG)					\
  do {									\
    if ((LOG) != 0)							\
      fprintf (STREAM, "\t.align\t%d\n", 1 << (LOG));			\
  } while (0)

/* Indicate that jump tables go in the text section.  This is
   necessary when compiling PIC code.  */
#define JUMP_TABLES_IN_TEXT_SECTION (flag_pic)


/* Define the strings to put out for each section in the object file.  */
#define TEXT_SECTION_ASM_OP	"\t.text"
#define DATA_SECTION_ASM_OP	"\t.data"
#define BSS_SECTION_ASM_OP	"\t.section\t.bss"


/* Define output to appear before the constant pool.  */
#define ASM_OUTPUT_POOL_PROLOGUE(FILE, FUNNAME, FUNDECL, SIZE)          \
  do {									\
    if ((SIZE) > 0 || !TARGET_WINDOWED_ABI)				\
      {									\
	resolve_unique_section ((FUNDECL), 0, flag_function_sections);	\
	switch_to_section (function_section (FUNDECL));			\
	fprintf (FILE, "\t.literal_position\n");			\
      }									\
  } while (0)


/* A C statement (with or without semicolon) to output a constant in
   the constant pool, if it needs special treatment.  */
#define ASM_OUTPUT_SPECIAL_POOL_ENTRY(FILE, X, MODE, ALIGN, LABELNO, JUMPTO) \
  do {									\
    xtensa_output_literal (FILE, X, MODE, LABELNO);			\
    goto JUMPTO;							\
  } while (0)

/* How to start an assembler comment.  */
#define ASM_COMMENT_START "#"

/* Exception handling.  Xtensa uses much of the standard DWARF2 unwinding
   machinery, but the variable size register window save areas are too
   complicated to efficiently describe with CFI entries.  The CFA must
   still be specified in DWARF so that DW_AT_frame_base is set correctly
   for debugging.  */
#define INCOMING_RETURN_ADDR_RTX gen_rtx_REG (Pmode, 0)
#define DWARF_FRAME_RETURN_COLUMN DWARF_FRAME_REGNUM (0)
#define DWARF_FRAME_REGISTERS 16
#define EH_RETURN_DATA_REGNO(N) ((N) < 2 ? (N) + 2 : INVALID_REGNUM)
#define ASM_PREFERRED_EH_DATA_FORMAT(CODE, GLOBAL)			\
  (flag_pic								\
   ? (((GLOBAL) ? DW_EH_PE_indirect : 0)				\
      | DW_EH_PE_pcrel | DW_EH_PE_sdata4)				\
   : DW_EH_PE_absptr)

/* Emit a PC-relative relocation.  */
#define ASM_OUTPUT_DWARF_PCREL(FILE, SIZE, LABEL)			\
  do {									\
    fputs (integer_asm_op (SIZE, FALSE), FILE);				\
    assemble_name (FILE, LABEL);					\
    fputs ("@pcrel", FILE);						\
  } while (0)

/* Xtensa constant pool breaks the devices in crtstuff.c to control
   section in where code resides.  We have to write it as asm code.  Use
   a MOVI and let the assembler relax it -- for the .init and .fini
   sections, the assembler knows to put the literal in the right
   place.  */
#if defined(__XTENSA_WINDOWED_ABI__)
#define CRT_CALL_STATIC_FUNCTION(SECTION_OP, FUNC) \
    asm (SECTION_OP "\n\
	movi\ta8, " USER_LABEL_PREFIX #FUNC "\n\
	callx8\ta8\n" \
	TEXT_SECTION_ASM_OP);
#elif defined(__XTENSA_CALL0_ABI__)
#define CRT_CALL_STATIC_FUNCTION(SECTION_OP, FUNC) \
    asm (SECTION_OP "\n\
	movi\ta0, " USER_LABEL_PREFIX #FUNC "\n\
	callx0\ta0\n" \
	TEXT_SECTION_ASM_OP);
#endif
