/* Definitions of target machine for GNU compiler, for ARM.
   Copyright (C) 1991, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
   2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.
   Contributed by Pieter `Tiggr' Schoenmakers (rcpieter@win.tue.nl)
   and Martin Simmons (@harleqn.co.uk).
   More major hacks by Richard Earnshaw (rearnsha@arm.com)
   Minor hacks by Nick Clifton (nickc@cygnus.com)

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef GCC_ARM_H
#define GCC_ARM_H

/* We can't use enum machine_mode inside a generator file because it
   hasn't been created yet; we shouldn't be using any code that
   needs the real definition though, so this ought to be safe.  */
#ifdef GENERATOR_FILE
#define MACHMODE int
#else
#include "insn-modes.h"
#define MACHMODE enum machine_mode
#endif

#include "config/vxworks-dummy.h"

/* The architecture define.  */
extern char arm_arch_name[];

/* Target CPU builtins.  */
#define TARGET_CPU_CPP_BUILTINS()			\
  do							\
    {							\
	/* Define __arm__ even when in thumb mode, for	\
	   consistency with armcc.  */			\
	builtin_define ("__arm__");			\
	builtin_define ("__APCS_32__");			\
	if (TARGET_THUMB)				\
	  builtin_define ("__thumb__");			\
	if (TARGET_THUMB2)				\
	  builtin_define ("__thumb2__");		\
							\
	if (TARGET_BIG_END)				\
	  {						\
	    builtin_define ("__ARMEB__");		\
	    if (TARGET_THUMB)				\
	      builtin_define ("__THUMBEB__");		\
	    if (TARGET_LITTLE_WORDS)			\
	      builtin_define ("__ARMWEL__");		\
	  }						\
        else						\
	  {						\
	    builtin_define ("__ARMEL__");		\
	    if (TARGET_THUMB)				\
	      builtin_define ("__THUMBEL__");		\
	  }						\
							\
	if (TARGET_SOFT_FLOAT)				\
	  builtin_define ("__SOFTFP__");		\
							\
	if (TARGET_VFP)					\
	  builtin_define ("__VFP_FP__");		\
							\
	if (TARGET_NEON)				\
	  builtin_define ("__ARM_NEON__");		\
							\
	/* Add a define for interworking.		\
	   Needed when building libgcc.a.  */		\
	if (arm_cpp_interwork)				\
	  builtin_define ("__THUMB_INTERWORK__");	\
							\
	builtin_assert ("cpu=arm");			\
	builtin_assert ("machine=arm");			\
							\
	builtin_define (arm_arch_name);			\
	if (arm_arch_cirrus)				\
	  builtin_define ("__MAVERICK__");		\
	if (arm_arch_xscale)				\
	  builtin_define ("__XSCALE__");		\
	if (arm_arch_iwmmxt)				\
	  builtin_define ("__IWMMXT__");		\
	if (TARGET_AAPCS_BASED)				\
 	  {						\
 	    if (arm_pcs_default == ARM_PCS_AAPCS_VFP)	\
 	      builtin_define ("__ARM_PCS_VFP");		\
 	    else if (arm_pcs_default == ARM_PCS_AAPCS)	\
 	      builtin_define ("__ARM_PCS");		\
 	    builtin_define ("__ARM_EABI__");		\
 	  }						\
	if (low_irq_latency)				\
	  builtin_define ("__low_irq_latency__");	\
	if (TARGET_IDIV)				\
	  builtin_define ("__ARM_ARCH_EXT_IDIV__");	\
    } while (0)

/* The various ARM cores.  */
enum processor_type
{
#define ARM_CORE(NAME, IDENT, ARCH, FLAGS, COSTS) \
  IDENT,
#include "arm-cores.def"
#undef ARM_CORE
  /* Used to indicate that no processor has been specified.  */
  arm_none
};

enum target_cpus
{
#define ARM_CORE(NAME, IDENT, ARCH, FLAGS, COSTS) \
  TARGET_CPU_##IDENT,
#include "arm-cores.def"
#undef ARM_CORE
  TARGET_CPU_generic
};

/* The processor for which instructions should be scheduled.  */
extern enum processor_type arm_tune;

enum arm_sync_generator_tag
  {
    arm_sync_generator_omn,
    arm_sync_generator_omrn
  };

/* Wrapper to pass around a polymorphic pointer to a sync instruction
   generator and.  */
struct arm_sync_generator
{
  enum arm_sync_generator_tag op;
  union
  {
    rtx (* omn) (rtx, rtx, rtx);
    rtx (* omrn) (rtx, rtx, rtx, rtx);
  } u;
};

typedef enum arm_cond_code
{
  ARM_EQ = 0, ARM_NE, ARM_CS, ARM_CC, ARM_MI, ARM_PL, ARM_VS, ARM_VC,
  ARM_HI, ARM_LS, ARM_GE, ARM_LT, ARM_GT, ARM_LE, ARM_AL, ARM_NV
}
arm_cc;

extern arm_cc arm_current_cc;

#define ARM_INVERSE_CONDITION_CODE(X)  ((arm_cc) (((int)X) ^ 1))

extern int arm_target_label;
extern int arm_ccfsm_state;
extern GTY(()) rtx arm_target_insn;
/* The label of the current constant pool.  */
extern rtx pool_vector_label;
/* Set to 1 when a return insn is output, this means that the epilogue
   is not needed.  */
extern int return_used_this_function;
/* Callback to output language specific object attributes.  */
extern void (*arm_lang_output_object_attributes_hook)(void);

/* Just in case configure has failed to define anything.  */
#ifndef TARGET_CPU_DEFAULT
#define TARGET_CPU_DEFAULT TARGET_CPU_generic
#endif


#undef  CPP_SPEC
#define CPP_SPEC "%(subtarget_cpp_spec)					\
%{msoft-float:%{mhard-float:						\
	%e-msoft-float and -mhard_float may not be used together}}	\
%{mbig-endian:%{mlittle-endian:						\
	%e-mbig-endian and -mlittle-endian may not be used together}}"

#ifndef CC1_SPEC
#define CC1_SPEC ""
#endif

/* This macro defines names of additional specifications to put in the specs
   that can be used in various specifications like CC1_SPEC.  Its definition
   is an initializer with a subgrouping for each command option.

   Each subgrouping contains a string constant, that defines the
   specification name, and a string constant that used by the GCC driver
   program.

   Do not define this macro if it does not need to do anything.  */
#define EXTRA_SPECS						\
  { "subtarget_cpp_spec",	SUBTARGET_CPP_SPEC },           \
  SUBTARGET_EXTRA_SPECS

#ifndef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS
#endif

#ifndef SUBTARGET_CPP_SPEC
#define SUBTARGET_CPP_SPEC      ""
#endif

/* Run-time Target Specification.  */
#ifndef TARGET_VERSION
#define TARGET_VERSION fputs (" (ARM/generic)", stderr);
#endif

#define TARGET_SOFT_FLOAT		(arm_float_abi == ARM_FLOAT_ABI_SOFT)
/* Use hardware floating point instructions. */
#define TARGET_HARD_FLOAT		(arm_float_abi != ARM_FLOAT_ABI_SOFT)
/* Use hardware floating point calling convention.  */
#define TARGET_HARD_FLOAT_ABI		(arm_float_abi == ARM_FLOAT_ABI_HARD)
#define TARGET_FPA		(arm_fpu_desc->model == ARM_FP_MODEL_FPA)
#define TARGET_MAVERICK		(arm_fpu_desc->model == ARM_FP_MODEL_MAVERICK)
#define TARGET_VFP		(arm_fpu_desc->model == ARM_FP_MODEL_VFP)
#define TARGET_IWMMXT			(arm_arch_iwmmxt)
#define TARGET_REALLY_IWMMXT		(TARGET_IWMMXT && TARGET_32BIT)
#define TARGET_IWMMXT_ABI (TARGET_32BIT && arm_abi == ARM_ABI_IWMMXT)
#define TARGET_ARM                      (! TARGET_THUMB)
#define TARGET_EITHER			1 /* (TARGET_ARM | TARGET_THUMB) */
#define TARGET_BACKTRACE	        (leaf_function_p () \
				         ? TARGET_TPCS_LEAF_FRAME \
				         : TARGET_TPCS_FRAME)
#define TARGET_LDRD			(arm_arch5e && ARM_DOUBLEWORD_ALIGN)
#define TARGET_AAPCS_BASED \
    (arm_abi != ARM_ABI_APCS && arm_abi != ARM_ABI_ATPCS)

#define TARGET_HARD_TP			(target_thread_pointer == TP_CP15)
#define TARGET_SOFT_TP			(target_thread_pointer == TP_SOFT)
#define TARGET_ARM_TLS			(target_tls_dialect == TLS_ARM)
#define TARGET_GNU_TLS			(target_tls_dialect == TLS_GNU)

/* Only 16-bit thumb code.  */
#define TARGET_THUMB1			(TARGET_THUMB && !arm_arch_thumb2)
/* Arm or Thumb-2 32-bit code.  */
#define TARGET_32BIT			(TARGET_ARM || arm_arch_thumb2)
/* 32-bit Thumb-2 code.  */
#define TARGET_THUMB2			(TARGET_THUMB && arm_arch_thumb2)
/* Thumb-1 only.  */
#define TARGET_THUMB1_ONLY		(TARGET_THUMB1 && !arm_arch_notm)
/* FPA emulator without LFM.  */
#define TARGET_FPA_EMU2			(TARGET_FPA && arm_fpu_desc->rev == 2)

/* The following two macros concern the ability to execute coprocessor
   instructions for VFPv3 or NEON.  TARGET_VFP3/TARGET_VFPD32 are currently
   only ever tested when we know we are generating for VFP hardware; we need
   to be more careful with TARGET_NEON as noted below.  */

/* FPU is has the full VFPv3/NEON register file of 32 D registers.  */
#define TARGET_VFPD32 (TARGET_VFP && arm_fpu_desc->regs == VFP_REG_D32)

/* FPU supports VFPv3 instructions.  */
#define TARGET_VFP3 (TARGET_VFP && arm_fpu_desc->rev >= 3)

/* FPU only supports VFP single-precision instructions.  */
#define TARGET_VFP_SINGLE (TARGET_VFP && arm_fpu_desc->regs == VFP_REG_SINGLE)

/* FPU supports VFP double-precision instructions.  */
#define TARGET_VFP_DOUBLE (TARGET_VFP && arm_fpu_desc->regs != VFP_REG_SINGLE)

/* FPU supports half-precision floating-point with NEON element load/store.  */
#define TARGET_NEON_FP16 \
  (TARGET_VFP && arm_fpu_desc->neon && arm_fpu_desc->fp16)

/* FPU supports VFP half-precision floating-point.  */
#define TARGET_FP16 (TARGET_VFP && arm_fpu_desc->fp16)

/* FPU supports Neon instructions.  The setting of this macro gets
   revealed via __ARM_NEON__ so we add extra guards upon TARGET_32BIT
   and TARGET_HARD_FLOAT to ensure that NEON instructions are
   available.  */
#define TARGET_NEON (TARGET_32BIT && TARGET_HARD_FLOAT \
		     && TARGET_VFP && arm_fpu_desc->neon)

/* "DSP" multiply instructions, eg. SMULxy.  */
#define TARGET_DSP_MULTIPLY \
  (TARGET_32BIT && arm_arch5e && (arm_arch_notm || arm_arch7em))
/* Integer SIMD instructions, and extend-accumulate instructions.  */
#define TARGET_INT_SIMD \
  (TARGET_32BIT && arm_arch6 && (arm_arch_notm || arm_arch7em))

/* Should MOVW/MOVT be used in preference to a constant pool.  */
#define TARGET_USE_MOVT \
  (arm_arch_thumb2 && !optimize_size && !current_tune->prefer_constant_pool)

/* We could use unified syntax for arm mode, but for now we just use it
   for Thumb-2.  */
#define TARGET_UNIFIED_ASM TARGET_THUMB2

/* Nonzero if this chip provides the DMB instruction.  */
#define TARGET_HAVE_DMB		(arm_arch7)

/* Nonzero if this chip implements a memory barrier via CP15.  */
#define TARGET_HAVE_DMB_MCR	(arm_arch6k && ! TARGET_HAVE_DMB)

/* Nonzero if this chip implements a memory barrier instruction.  */
#define TARGET_HAVE_MEMORY_BARRIER (TARGET_HAVE_DMB || TARGET_HAVE_DMB_MCR)

/* Nonzero if this chip supports ldrex and strex */
#define TARGET_HAVE_LDREX	((arm_arch6 && TARGET_ARM) || arm_arch7)

/* Nonzero if this chip supports ldrex{bhd} and strex{bhd}.  */
#define TARGET_HAVE_LDREXBHD	((arm_arch6k && TARGET_ARM) || arm_arch7)

/* Nonzero if integer division instructions supported.  */
#define TARGET_IDIV		((TARGET_ARM && arm_arch_arm_hwdiv) \
				 || (TARGET_THUMB2 && arm_arch_thumb_hwdiv))

/* True iff the full BPABI is being used.  If TARGET_BPABI is true,
   then TARGET_AAPCS_BASED must be true -- but the converse does not
   hold.  TARGET_BPABI implies the use of the BPABI runtime library,
   etc., in addition to just the AAPCS calling conventions.  */
#ifndef TARGET_BPABI
#define TARGET_BPABI false
#endif

/* Support for a compile-time default CPU, et cetera.  The rules are:
   --with-arch is ignored if -march or -mcpu are specified.
   --with-cpu is ignored if -march or -mcpu are specified, and is overridden
    by --with-arch.
   --with-tune is ignored if -mtune or -mcpu are specified (but not affected
     by -march).
   --with-float is ignored if -mhard-float, -msoft-float or -mfloat-abi are
   specified.
   --with-fpu is ignored if -mfpu is specified.
   --with-abi is ignored is -mabi is specified.  */
#define OPTION_DEFAULT_SPECS \
  {"arch", "%{!march=*:%{!mcpu=*:-march=%(VALUE)}}" }, \
  {"cpu", "%{!march=*:%{!mcpu=*:-mcpu=%(VALUE)}}" }, \
  {"tune", "%{!mcpu=*:%{!mtune=*:-mtune=%(VALUE)}}" }, \
  {"float", \
    "%{!msoft-float:%{!mhard-float:%{!mfloat-abi=*:-mfloat-abi=%(VALUE)}}}" }, \
  {"fpu", "%{!mfpu=*:-mfpu=%(VALUE)}"}, \
  {"abi", "%{!mabi=*:-mabi=%(VALUE)}"}, \
  {"mode", "%{!marm:%{!mthumb:-m%(VALUE)}}"},

/* Which floating point model to use.  */
enum arm_fp_model
{
  ARM_FP_MODEL_UNKNOWN,
  /* FPA model (Hardware or software).  */
  ARM_FP_MODEL_FPA,
  /* Cirrus Maverick floating point model.  */
  ARM_FP_MODEL_MAVERICK,
  /* VFP floating point model.  */
  ARM_FP_MODEL_VFP
};

enum vfp_reg_type
{
  VFP_NONE = 0,
  VFP_REG_D16,
  VFP_REG_D32,
  VFP_REG_SINGLE
};

extern const struct arm_fpu_desc
{
  const char *name;
  enum arm_fp_model model;
  int rev;
  enum vfp_reg_type regs;
  int neon;
  int fp16;
} *arm_fpu_desc;

/* Which floating point hardware to schedule for.  */
extern int arm_fpu_attr;

enum float_abi_type
{
  ARM_FLOAT_ABI_SOFT,
  ARM_FLOAT_ABI_SOFTFP,
  ARM_FLOAT_ABI_HARD
};

extern enum float_abi_type arm_float_abi;

#ifndef TARGET_DEFAULT_FLOAT_ABI
#define TARGET_DEFAULT_FLOAT_ABI ARM_FLOAT_ABI_SOFT
#endif

/* Which __fp16 format to use.
   The enumeration values correspond to the numbering for the
   Tag_ABI_FP_16bit_format attribute.
 */
enum arm_fp16_format_type
{
  ARM_FP16_FORMAT_NONE = 0,
  ARM_FP16_FORMAT_IEEE = 1,
  ARM_FP16_FORMAT_ALTERNATIVE = 2
};

extern enum arm_fp16_format_type arm_fp16_format;
#define LARGEST_EXPONENT_IS_NORMAL(bits) \
    ((bits) == 16 && arm_fp16_format == ARM_FP16_FORMAT_ALTERNATIVE)

/* Which ABI to use.  */
enum arm_abi_type
{
  ARM_ABI_APCS,
  ARM_ABI_ATPCS,
  ARM_ABI_AAPCS,
  ARM_ABI_IWMMXT,
  ARM_ABI_AAPCS_LINUX
};

extern enum arm_abi_type arm_abi;

#ifndef ARM_DEFAULT_ABI
#define ARM_DEFAULT_ABI ARM_ABI_APCS
#endif

/* Which thread pointer access sequence to use.  */
enum arm_tp_type {
  TP_AUTO,
  TP_SOFT,
  TP_CP15
};

enum arm_tls_type {
  TLS_ARM,
  TLS_GNU
};

extern enum arm_tp_type target_thread_pointer;
extern enum arm_tls_type target_tls_dialect;

/* Nonzero if this chip supports the ARM Architecture 3M extensions.  */
extern int arm_arch3m;

/* Nonzero if this chip supports the ARM Architecture 4 extensions.  */
extern int arm_arch4;

/* Nonzero if this chip supports the ARM Architecture 4T extensions.  */
extern int arm_arch4t;

/* Nonzero if this chip supports the ARM Architecture 5 extensions.  */
extern int arm_arch5;

/* Nonzero if this chip supports the ARM Architecture 5E extensions.  */
extern int arm_arch5e;

/* Nonzero if this chip supports the ARM Architecture 6 extensions.  */
extern int arm_arch6;

/* Nonzero if this chip supports the ARM Architecture 6k extensions.  */
extern int arm_arch6k;

/* Nonzero if this chip supports the ARM Architecture 7 extensions.  */
extern int arm_arch7;

/* Nonzero if instructions not present in the 'M' profile can be used.  */
extern int arm_arch_notm;

/* Nonzero if instructions present in ARMv7E-M can be used.  */
extern int arm_arch7em;

/* Nonzero if this chip can benefit from load scheduling.  */
extern int arm_ld_sched;

/* Nonzero if generating thumb code.  */
extern int thumb_code;

/* Nonzero if this chip is a StrongARM.  */
extern int arm_tune_strongarm;

/* Nonzero if this chip is a Cirrus variant.  */
extern int arm_arch_cirrus;

/* Nonzero if this chip supports Intel XScale with Wireless MMX technology.  */
extern int arm_arch_iwmmxt;

/* Nonzero if this chip is an XScale.  */
extern int arm_arch_xscale;

/* Nonzero if tuning for XScale.  */
extern int arm_tune_xscale;

/* Nonzero if tuning for stores via the write buffer.  */
extern int arm_tune_wbuf;

/* Nonzero if tuning for Cortex-A5.  */
extern int arm_tune_cortex_a5;

/* Nonzero if tuning for Cortex-A9.  */
extern int arm_tune_cortex_a9;

/* Nonzero if we should define __THUMB_INTERWORK__ in the
   preprocessor.
   XXX This is a bit of a hack, it's intended to help work around
   problems in GLD which doesn't understand that armv5t code is
   interworking clean.  */
extern int arm_cpp_interwork;

/* Nonzero if chip supports Thumb 2.  */
extern int arm_arch_thumb2;

/* Nonzero if chip supports integer division instruction in ARM mode.  */
extern int arm_arch_arm_hwdiv;

/* Nonzero if chip supports integer division instruction in Thumb mode.  */
extern int arm_arch_thumb_hwdiv;

/* Nonzero if we should minimize interrupt latency of the
   generated code.  */
extern int low_irq_latency;

#ifndef TARGET_DEFAULT
#define TARGET_DEFAULT  (MASK_APCS_FRAME)
#endif

/* The frame pointer register used in gcc has nothing to do with debugging;
   that is controlled by the APCS-FRAME option.  */
#define CAN_DEBUG_WITHOUT_FP

#define OVERRIDE_OPTIONS  arm_override_options ()

#define OPTIMIZATION_OPTIONS(LEVEL,SIZE)		\
	arm_optimization_options ((LEVEL), (SIZE))

/* Nonzero if PIC code requires explicit qualifiers to generate
   PLT and GOT relocs rather than the assembler doing so implicitly.
   Subtargets can override these if required.  */
#ifndef NEED_GOT_RELOC
#define NEED_GOT_RELOC	0
#endif
#ifndef NEED_PLT_RELOC
#define NEED_PLT_RELOC	0
#endif

/* Nonzero if we need to refer to the GOT with a PC-relative
   offset.  In other words, generate

   .word	_GLOBAL_OFFSET_TABLE_ - [. - (.Lxx + 8)]

   rather than

   .word	_GLOBAL_OFFSET_TABLE_ - (.Lxx + 8)

   The default is true, which matches NetBSD.  Subtargets can
   override this if required.  */
#ifndef GOT_PCREL
#define GOT_PCREL   1
#endif

/* Target machine storage Layout.  */


/* Define this macro if it is advisable to hold scalars in registers
   in a wider mode than that declared by the program.  In such cases,
   the value is constrained to be within the bounds of the declared
   type, but kept valid in the wider mode.  The signedness of the
   extension may differ from that of the type.  */

/* It is far faster to zero extend chars than to sign extend them */

#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)	\
  if (GET_MODE_CLASS (MODE) == MODE_INT		\
      && GET_MODE_SIZE (MODE) < 4)      	\
    {						\
      if (MODE == QImode)			\
	UNSIGNEDP = 1;				\
      else if (MODE == HImode)			\
	UNSIGNEDP = 1;				\
      (MODE) = SImode;				\
    }

/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields.  */
#define BITS_BIG_ENDIAN  0

/* Define this if most significant byte of a word is the lowest numbered.
   Most ARM processors are run in little endian mode, so that is the default.
   If you want to have it run-time selectable, change the definition in a
   cover file to be TARGET_BIG_ENDIAN.  */
#define BYTES_BIG_ENDIAN  (TARGET_BIG_END != 0)

/* Define this if most significant word of a multiword number is the lowest
   numbered.
   This is always false, even when in big-endian mode.  */
#define WORDS_BIG_ENDIAN  (BYTES_BIG_ENDIAN && ! TARGET_LITTLE_WORDS)

/* LIBGCC2_WORDS_BIG_ENDIAN has to be a constant, so we define this based
   on processor pre-defineds when compiling libgcc2.c.  */
#if defined(__ARMEB__) && !defined(__ARMWEL__)
#define LIBGCC2_WORDS_BIG_ENDIAN 1
#else
#define LIBGCC2_WORDS_BIG_ENDIAN 0
#endif

/* Define this if most significant word of doubles is the lowest numbered.
   The rules are different based on whether or not we use FPA-format,
   VFP-format or some other floating point co-processor's format doubles.  */
#define FLOAT_WORDS_BIG_ENDIAN (arm_float_words_big_endian ())

#define UNITS_PER_WORD	4

/* Use the option -mvectorize-with-neon-quad to override the use of doubleword
   registers when autovectorizing for Neon, at least until multiple vector
   widths are supported properly by the middle-end.  */
#define UNITS_PER_SIMD_WORD(MODE) \
  (TARGET_NEON ? (TARGET_NEON_VECTORIZE_QUAD ? 16 : 8) : UNITS_PER_WORD)

/* True if natural alignment is used for doubleword types.  */
#define ARM_DOUBLEWORD_ALIGN	TARGET_AAPCS_BASED

#define DOUBLEWORD_ALIGNMENT 64

#define PARM_BOUNDARY  	32

#define STACK_BOUNDARY  (ARM_DOUBLEWORD_ALIGN ? DOUBLEWORD_ALIGNMENT : 32)

#define PREFERRED_STACK_BOUNDARY \
    (arm_abi == ARM_ABI_ATPCS ? 64 : STACK_BOUNDARY)

#define FUNCTION_BOUNDARY  ((TARGET_THUMB && optimize_size) ? 16 : 32)

/* The lowest bit is used to indicate Thumb-mode functions, so the
   vbit must go into the delta field of pointers to member
   functions.  */
#define TARGET_PTRMEMFUNC_VBIT_LOCATION ptrmemfunc_vbit_in_delta

#define EMPTY_FIELD_BOUNDARY  32

#define BIGGEST_ALIGNMENT (ARM_DOUBLEWORD_ALIGN ? DOUBLEWORD_ALIGNMENT : 32)

/* XXX Blah -- this macro is used directly by libobjc.  Since it
   supports no vector modes, cut out the complexity and fall back
   on BIGGEST_FIELD_ALIGNMENT.  */
#ifdef IN_TARGET_LIBS
#define BIGGEST_FIELD_ALIGNMENT 64
#endif

/* Make strings word-aligned so strcpy from constants will be faster.  */
#define CONSTANT_ALIGNMENT_FACTOR (TARGET_THUMB || ! arm_tune_xscale ? 1 : 2)

#define CONSTANT_ALIGNMENT(EXP, ALIGN)				\
   ((TREE_CODE (EXP) == STRING_CST				\
     && !optimize_size						\
     && (ALIGN) < BITS_PER_WORD * CONSTANT_ALIGNMENT_FACTOR)	\
    ? BITS_PER_WORD * CONSTANT_ALIGNMENT_FACTOR : (ALIGN))

/* Align definitions of arrays, unions and structures so that
   initializations and copies can be made more efficient.  This is not
   ABI-changing, so it only affects places where we can see the
   definition. Increasing the alignment tends to introduce padding,
   so don't do this when optimizing for size/conserving stack space. */
#define ARM_EXPAND_ALIGNMENT(COND, EXP, ALIGN)				\
  (((COND) && ((ALIGN) < BITS_PER_WORD)					\
    && (TREE_CODE (EXP) == ARRAY_TYPE					\
	|| TREE_CODE (EXP) == UNION_TYPE				\
	|| TREE_CODE (EXP) == RECORD_TYPE)) ? BITS_PER_WORD : (ALIGN))

/* Align global data. */
#define DATA_ALIGNMENT(EXP, ALIGN)			\
  ARM_EXPAND_ALIGNMENT(!optimize_size, EXP, ALIGN)

/* Similarly, make sure that objects on the stack are sensibly aligned.  */
#define LOCAL_ALIGNMENT(EXP, ALIGN)				\
  ARM_EXPAND_ALIGNMENT(!flag_conserve_stack, EXP, ALIGN)

/* Setting STRUCTURE_SIZE_BOUNDARY to 32 produces more efficient code, but the
   value set in previous versions of this toolchain was 8, which produces more
   compact structures.  The command line option -mstructure_size_boundary=<n>
   can be used to change this value.  For compatibility with the ARM SDK
   however the value should be left at 32.  ARM SDT Reference Manual (ARM DUI
   0020D) page 2-20 says "Structures are aligned on word boundaries".
   The AAPCS specifies a value of 8.  */
#define STRUCTURE_SIZE_BOUNDARY arm_structure_size_boundary
extern int arm_structure_size_boundary;

/* This is the value used to initialize arm_structure_size_boundary.  If a
   particular arm target wants to change the default value it should change
   the definition of this macro, not STRUCTURE_SIZE_BOUNDARY.  See netbsd.h
   for an example of this.  */
#ifndef DEFAULT_STRUCTURE_SIZE_BOUNDARY
#define DEFAULT_STRUCTURE_SIZE_BOUNDARY 32
#endif

/* Nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* wchar_t is unsigned under the AAPCS.  */
#ifndef WCHAR_TYPE
#define WCHAR_TYPE (TARGET_AAPCS_BASED ? "unsigned int" : "int")

#define WCHAR_TYPE_SIZE BITS_PER_WORD
#endif

#ifndef SIZE_TYPE
#define SIZE_TYPE (TARGET_AAPCS_BASED ? "unsigned int" : "long unsigned int")
#endif

#ifndef PTRDIFF_TYPE
#define PTRDIFF_TYPE (TARGET_AAPCS_BASED ? "int" : "long int")
#endif

/* AAPCS requires that structure alignment is affected by bitfields.  */
#ifndef PCC_BITFIELD_TYPE_MATTERS
#define PCC_BITFIELD_TYPE_MATTERS TARGET_AAPCS_BASED
#endif


/* Standard register usage.  */

/* Register allocation in ARM Procedure Call Standard (as used on RISCiX):
   (S - saved over call).

	r0	   *	argument word/integer result
	r1-r3		argument word

	r4-r8	     S	register variable
	r9	     S	(rfp) register variable (real frame pointer)

	r10  	   F S	(sl) stack limit (used by -mapcs-stack-check)
	r11 	   F S	(fp) argument pointer
	r12		(ip) temp workspace
	r13  	   F S	(sp) lower end of current stack frame
	r14		(lr) link address/workspace
	r15	   F	(pc) program counter

	f0		floating point result
	f1-f3		floating point scratch

	f4-f7	     S	floating point variable

	cc		This is NOT a real register, but is used internally
	                to represent things that use or set the condition
			codes.
	sfp             This isn't either.  It is used during rtl generation
	                since the offset between the frame pointer and the
			auto's isn't known until after register allocation.
	afp		Nor this, we only need this because of non-local
	                goto.  Without it fp appears to be used and the
			elimination code won't get rid of sfp.  It tracks
			fp exactly at all times.

   *: See CONDITIONAL_REGISTER_USAGE  */

/*
  	mvf0		Cirrus floating point result
	mvf1-mvf3	Cirrus floating point scratch
	mvf4-mvf15   S	Cirrus floating point variable.  */

/*	s0-s15		VFP scratch (aka d0-d7).
	s16-s31	      S	VFP variable (aka d8-d15).
	vfpcc		Not a real register.  Represents the VFP condition
			code flags.  */

/* The stack backtrace structure is as follows:
  fp points to here:  |  save code pointer  |      [fp]
                      |  return link value  |      [fp, #-4]
                      |  return sp value    |      [fp, #-8]
                      |  return fp value    |      [fp, #-12]
                     [|  saved r10 value    |]
                     [|  saved r9 value     |]
                     [|  saved r8 value     |]
                     [|  saved r7 value     |]
                     [|  saved r6 value     |]
                     [|  saved r5 value     |]
                     [|  saved r4 value     |]
                     [|  saved r3 value     |]
                     [|  saved r2 value     |]
                     [|  saved r1 value     |]
                     [|  saved r0 value     |]
                     [|  saved f7 value     |]     three words
                     [|  saved f6 value     |]     three words
                     [|  saved f5 value     |]     three words
                     [|  saved f4 value     |]     three words
  r0-r3 are not normally saved in a C function.  */

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.  */
#define FIXED_REGISTERS \
{                       \
  0,0,0,0,0,0,0,0,	\
  0,0,0,0,0,1,0,1,	\
  0,0,0,0,0,0,0,0,	\
  1,1,1,		\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,		\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1,1,1,1,1,1,1,1,	\
  1			\
}

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you like.
   The CC is not preserved over function calls on the ARM 6, so it is
   easier to assume this for all.  SFP is preserved, since FP is.  */
#define CALL_USED_REGISTERS  \
{                            \
  1,1,1,1,0,0,0,0,	     \
  0,0,0,0,1,1,1,1,	     \
  1,1,1,1,0,0,0,0,	     \
  1,1,1,		     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,		     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1,1,1,1,1,1,1,1,	     \
  1			     \
}

#ifndef SUBTARGET_CONDITIONAL_REGISTER_USAGE
#define SUBTARGET_CONDITIONAL_REGISTER_USAGE
#endif

#define CONDITIONAL_REGISTER_USAGE				\
{								\
  int regno;							\
								\
  if (TARGET_SOFT_FLOAT || TARGET_THUMB1 || !TARGET_FPA)	\
    {								\
      for (regno = FIRST_FPA_REGNUM;				\
	   regno <= LAST_FPA_REGNUM; ++regno)			\
	fixed_regs[regno] = call_used_regs[regno] = 1;		\
    }								\
								\
  if (TARGET_THUMB1 && optimize_size)				\
    {                                                           \
      /* When optimizing for size on Thumb-1, it's better not	\
        to use the HI regs, because of the overhead of		\
        stacking them.  */                                      \
      for (regno = FIRST_HI_REGNUM;				\
	   regno <= LAST_HI_REGNUM; ++regno)			\
	fixed_regs[regno] = call_used_regs[regno] = 1;		\
    }								\
								\
  /* The link register can be clobbered by any branch insn,	\
     but we have no way to track that at present, so mark	\
     it as unavailable.  */					\
  if (TARGET_THUMB1)						\
    fixed_regs[LR_REGNUM] = call_used_regs[LR_REGNUM] = 1;	\
								\
  if (TARGET_32BIT && TARGET_HARD_FLOAT)			\
    {								\
      if (TARGET_MAVERICK)					\
	{							\
	  for (regno = FIRST_FPA_REGNUM;			\
	       regno <= LAST_FPA_REGNUM; ++ regno)		\
	    fixed_regs[regno] = call_used_regs[regno] = 1;	\
	  for (regno = FIRST_CIRRUS_FP_REGNUM;			\
	       regno <= LAST_CIRRUS_FP_REGNUM; ++ regno)	\
	    {							\
	      fixed_regs[regno] = 0;				\
	      call_used_regs[regno] = regno < FIRST_CIRRUS_FP_REGNUM + 4; \
	    }							\
	}							\
      if (TARGET_VFP)						\
	{							\
	  /* VFPv3 registers are disabled when earlier VFP	\
	     versions are selected due to the definition of	\
	     LAST_VFP_REGNUM.  */				\
	  for (regno = FIRST_VFP_REGNUM;			\
	       regno <= LAST_VFP_REGNUM; ++ regno)		\
	    {							\
	      fixed_regs[regno] = 0;				\
	      call_used_regs[regno] = regno < FIRST_VFP_REGNUM + 16 \
	      	|| regno >= FIRST_VFP_REGNUM + 32;		\
	    }							\
	}							\
    }								\
								\
  if (TARGET_REALLY_IWMMXT)					\
    {								\
      regno = FIRST_IWMMXT_GR_REGNUM;				\
      /* The 2002/10/09 revision of the XScale ABI has wCG0     \
         and wCG1 as call-preserved registers.  The 2002/11/21  \
         revision changed this so that all wCG registers are    \
         scratch registers.  */					\
      for (regno = FIRST_IWMMXT_GR_REGNUM;			\
	   regno <= LAST_IWMMXT_GR_REGNUM; ++ regno)		\
	fixed_regs[regno] = 0;					\
      /* The XScale ABI has wR0 - wR9 as scratch registers,     \
	 the rest as call-preserved registers.  */		\
      for (regno = FIRST_IWMMXT_REGNUM;				\
	   regno <= LAST_IWMMXT_REGNUM; ++ regno)		\
	{							\
	  fixed_regs[regno] = 0;				\
	  call_used_regs[regno] = regno < FIRST_IWMMXT_REGNUM + 10; \
	}							\
    }								\
								\
  if ((unsigned) PIC_OFFSET_TABLE_REGNUM != INVALID_REGNUM)	\
    {								\
      fixed_regs[PIC_OFFSET_TABLE_REGNUM] = 1;			\
      call_used_regs[PIC_OFFSET_TABLE_REGNUM] = 1;		\
    }								\
  else if (TARGET_APCS_STACK)					\
    {								\
      fixed_regs[10]     = 1;					\
      call_used_regs[10] = 1;					\
    }								\
  /* -mcaller-super-interworking reserves r11 for calls to	\
     _interwork_r11_call_via_rN().  Making the register global	\
     is an easy way of ensuring that it remains valid for all	\
     calls.  */							\
  if (TARGET_APCS_FRAME || TARGET_CALLER_INTERWORKING		\
      || TARGET_TPCS_FRAME || TARGET_TPCS_LEAF_FRAME)		\
    {								\
      fixed_regs[ARM_HARD_FRAME_POINTER_REGNUM] = 1;		\
      call_used_regs[ARM_HARD_FRAME_POINTER_REGNUM] = 1;	\
      if (TARGET_CALLER_INTERWORKING)				\
	global_regs[ARM_HARD_FRAME_POINTER_REGNUM] = 1;		\
    }								\
  SUBTARGET_CONDITIONAL_REGISTER_USAGE				\
}

/* These are a couple of extensions to the formats accepted
   by asm_fprintf:
     %@ prints out ASM_COMMENT_START
     %r prints out REGISTER_PREFIX reg_names[arg]  */
#define ASM_FPRINTF_EXTENSIONS(FILE, ARGS, P)		\
  case '@':						\
    fputs (ASM_COMMENT_START, FILE);			\
    break;						\
							\
  case 'r':						\
    fputs (REGISTER_PREFIX, FILE);			\
    fputs (reg_names [va_arg (ARGS, int)], FILE);	\
    break;

/* Round X up to the nearest word.  */
#define ROUND_UP_WORD(X) (((X) + 3) & ~3)

/* Convert fron bytes to ints.  */
#define ARM_NUM_INTS(X) (((X) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* The number of (integer) registers required to hold a quantity of type MODE.
   Also used for VFP registers.  */
#define ARM_NUM_REGS(MODE)				\
  ARM_NUM_INTS (GET_MODE_SIZE (MODE))

/* The number of (integer) registers required to hold a quantity of TYPE MODE.  */
#define ARM_NUM_REGS2(MODE, TYPE)                   \
  ARM_NUM_INTS ((MODE) == BLKmode ? 		\
  int_size_in_bytes (TYPE) : GET_MODE_SIZE (MODE))

/* The number of (integer) argument register available.  */
#define NUM_ARG_REGS		4

/* And similarly for the VFP.  */
#define NUM_VFP_ARG_REGS	16

/* Return the register number of the N'th (integer) argument.  */
#define ARG_REGISTER(N) 	(N - 1)

/* Specify the registers used for certain standard purposes.
   The values of these macros are register numbers.  */

/* The number of the last argument register.  */
#define LAST_ARG_REGNUM 	ARG_REGISTER (NUM_ARG_REGS)

/* The numbers of the Thumb register ranges.  */
#define FIRST_LO_REGNUM  	0
#define LAST_LO_REGNUM  	7
#define FIRST_HI_REGNUM		8
#define LAST_HI_REGNUM		11

#ifndef TARGET_UNWIND_INFO
/* We use sjlj exceptions for backwards compatibility.  */
#define MUST_USE_SJLJ_EXCEPTIONS 1
#endif

/* Use r0 and r1 to pass exception handling information.  */
#define EH_RETURN_DATA_REGNO(N) (((N) < 2) ? N : INVALID_REGNUM)

/* The register that holds the return address in exception handlers.  */
#define ARM_EH_STACKADJ_REGNUM	2
#define EH_RETURN_STACKADJ_RTX	gen_rtx_REG (SImode, ARM_EH_STACKADJ_REGNUM)

/* The native (Norcroft) Pascal compiler for the ARM passes the static chain
   as an invisible last argument (possible since varargs don't exist in
   Pascal), so the following is not true.  */
#define STATIC_CHAIN_REGNUM	12

/* Define this to be where the real frame pointer is if it is not possible to
   work out the offset between the frame pointer and the automatic variables
   until after register allocation has taken place.  FRAME_POINTER_REGNUM
   should point to a special register that we will make sure is eliminated.

   For the Thumb we have another problem.  The TPCS defines the frame pointer
   as r11, and GCC believes that it is always possible to use the frame pointer
   as base register for addressing purposes.  (See comments in
   find_reloads_address()).  But - the Thumb does not allow high registers,
   including r11, to be used as base address registers.  Hence our problem.

   The solution used here, and in the old thumb port is to use r7 instead of
   r11 as the hard frame pointer and to have special code to generate
   backtrace structures on the stack (if required to do so via a command line
   option) using r11.  This is the only 'user visible' use of r11 as a frame
   pointer.  */
#define ARM_HARD_FRAME_POINTER_REGNUM	11
#define THUMB_HARD_FRAME_POINTER_REGNUM	 7

#define HARD_FRAME_POINTER_REGNUM		\
  (TARGET_ARM					\
   ? ARM_HARD_FRAME_POINTER_REGNUM		\
   : THUMB_HARD_FRAME_POINTER_REGNUM)

#define FP_REGNUM	                HARD_FRAME_POINTER_REGNUM

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM	SP_REGNUM

/* ARM floating pointer registers.  */
#define FIRST_FPA_REGNUM 	16
#define LAST_FPA_REGNUM  	23
#define IS_FPA_REGNUM(REGNUM) \
  (((REGNUM) >= FIRST_FPA_REGNUM) && ((REGNUM) <= LAST_FPA_REGNUM))

#define FIRST_IWMMXT_GR_REGNUM	43
#define LAST_IWMMXT_GR_REGNUM	46
#define FIRST_IWMMXT_REGNUM	47
#define LAST_IWMMXT_REGNUM	62
#define IS_IWMMXT_REGNUM(REGNUM) \
  (((REGNUM) >= FIRST_IWMMXT_REGNUM) && ((REGNUM) <= LAST_IWMMXT_REGNUM))
#define IS_IWMMXT_GR_REGNUM(REGNUM) \
  (((REGNUM) >= FIRST_IWMMXT_GR_REGNUM) && ((REGNUM) <= LAST_IWMMXT_GR_REGNUM))

/* Base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM	25

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM	26

#define FIRST_CIRRUS_FP_REGNUM	27
#define LAST_CIRRUS_FP_REGNUM	42
#define IS_CIRRUS_REGNUM(REGNUM) \
  (((REGNUM) >= FIRST_CIRRUS_FP_REGNUM) && ((REGNUM) <= LAST_CIRRUS_FP_REGNUM))

#define FIRST_VFP_REGNUM	63
#define D7_VFP_REGNUM		78  /* Registers 77 and 78 == VFP reg D7.  */
#define LAST_VFP_REGNUM	\
  (TARGET_VFPD32 ? LAST_HI_VFP_REGNUM : LAST_LO_VFP_REGNUM)

#define IS_VFP_REGNUM(REGNUM) \
  (((REGNUM) >= FIRST_VFP_REGNUM) && ((REGNUM) <= LAST_VFP_REGNUM))

/* VFP registers are split into two types: those defined by VFP versions < 3
   have D registers overlaid on consecutive pairs of S registers. VFP version 3
   defines 16 new D registers (d16-d31) which, for simplicity and correctness
   in various parts of the backend, we implement as "fake" single-precision
   registers (which would be S32-S63, but cannot be used in that way).  The
   following macros define these ranges of registers.  */
#define LAST_LO_VFP_REGNUM	94
#define FIRST_HI_VFP_REGNUM	95
#define LAST_HI_VFP_REGNUM	126

#define VFP_REGNO_OK_FOR_SINGLE(REGNUM) \
  ((REGNUM) <= LAST_LO_VFP_REGNUM)

/* DFmode values are only valid in even register pairs.  */
#define VFP_REGNO_OK_FOR_DOUBLE(REGNUM) \
  ((((REGNUM) - FIRST_VFP_REGNUM) & 1) == 0)

/* Neon Quad values must start at a multiple of four registers.  */
#define NEON_REGNO_OK_FOR_QUAD(REGNUM) \
  ((((REGNUM) - FIRST_VFP_REGNUM) & 3) == 0)

/* Neon structures of vectors must be in even register pairs and there
   must be enough registers available.  Because of various patterns
   requiring quad registers, we require them to start at a multiple of
   four.  */
#define NEON_REGNO_OK_FOR_NREGS(REGNUM, N) \
  ((((REGNUM) - FIRST_VFP_REGNUM) & 3) == 0 \
   && (LAST_VFP_REGNUM - (REGNUM) >= 2 * (N) - 1))

/* The number of hard registers is 16 ARM + 8 FPA + 1 CC + 1 SFP + 1 AFP.  */
/* + 16 Cirrus registers take us up to 43.  */
/* Intel Wireless MMX Technology registers add 16 + 4 more.  */
/* VFP (VFP3) adds 32 (64) + 1 more.  */
#define FIRST_PSEUDO_REGISTER   128

#define DBX_REGISTER_NUMBER(REGNO) arm_dbx_register_number (REGNO)

/* Value should be nonzero if functions must have frame pointers.
   Zero means the frame pointer need not be set up (and parms may be accessed
   via the stack pointer) in functions that seem suitable.
   If we have to have a frame pointer we might as well make use of it.
   APCS says that the frame pointer does not need to be pushed in leaf
   functions, or simple tail call functions.  */

#ifndef SUBTARGET_FRAME_POINTER_REQUIRED
#define SUBTARGET_FRAME_POINTER_REQUIRED 0
#endif

/* Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.
   This is ordinarily the length in words of a value of mode MODE
   but can be less for certain modes in special long registers.

   On the ARM regs are UNITS_PER_WORD bits wide; FPA regs can hold any FP
   mode.  */
#define HARD_REGNO_NREGS(REGNO, MODE)  	\
  ((TARGET_32BIT			\
    && REGNO >= FIRST_FPA_REGNUM	\
    && REGNO != FRAME_POINTER_REGNUM	\
    && REGNO != ARG_POINTER_REGNUM)	\
    && !IS_VFP_REGNUM (REGNO)		\
   ? 1 : ARM_NUM_REGS (MODE))

/* Return true if REGNO is suitable for holding a quantity of type MODE.  */
#define HARD_REGNO_MODE_OK(REGNO, MODE)					\
  arm_hard_regno_mode_ok ((REGNO), (MODE))

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */
#define MODES_TIEABLE_P(MODE1, MODE2)  \
  (GET_MODE_CLASS (MODE1) == GET_MODE_CLASS (MODE2))

#define VALID_IWMMXT_REG_MODE(MODE) \
 (arm_vector_mode_supported_p (MODE) || (MODE) == DImode || (MODE) == SImode)

/* Modes valid for Neon D registers.  */
#define VALID_NEON_DREG_MODE(MODE) \
  ((MODE) == V2SImode || (MODE) == V4HImode || (MODE) == V8QImode \
   || (MODE) == V2SFmode || (MODE) == DImode)

/* Modes valid for Neon Q registers.  */
#define VALID_NEON_QREG_MODE(MODE) \
  ((MODE) == V4SImode || (MODE) == V8HImode || (MODE) == V16QImode \
   || (MODE) == V4SFmode || (MODE) == V2DImode)

/* Structure modes valid for Neon registers.  */
#define VALID_NEON_STRUCT_MODE(MODE) \
  ((MODE) == TImode || (MODE) == EImode || (MODE) == OImode \
   || (MODE) == CImode || (MODE) == XImode)

/* The order in which register should be allocated.  It is good to use ip
   since no saving is required (though calls clobber it) and it never contains
   function parameters.  It is quite good to use lr since other calls may
   clobber it anyway.  Allocate r0 through r3 in reverse order since r3 is
   least likely to contain a function parameter; in addition results are
   returned in r0.
   For VFP/VFPv3, allocate D16-D31 first, then caller-saved registers (D0-D7),
   then D8-D15.  The reason for doing this is to attempt to reduce register
   pressure when both single- and double-precision registers are used in a
   function.  */

#define REG_ALLOC_ORDER				\
{						\
     3,  2,  1,  0, 12, 14,  4,  5,		\
     6,  7,  8, 10,  9, 11, 13, 15,		\
    16, 17, 18, 19, 20, 21, 22, 23,		\
    27, 28, 29, 30, 31, 32, 33, 34,		\
    35, 36, 37, 38, 39, 40, 41, 42,		\
    43, 44, 45, 46, 47, 48, 49, 50,		\
    51, 52, 53, 54, 55, 56, 57, 58,		\
    59, 60, 61, 62,				\
    24, 25, 26,					\
    95,  96,  97,  98,  99, 100, 101, 102,	\
   103, 104, 105, 106, 107, 108, 109, 110,	\
   111, 112, 113, 114, 115, 116, 117, 118,	\
   119, 120, 121, 122, 123, 124, 125, 126,	\
    78,  77,  76,  75,  74,  73,  72,  71,	\
    70,  69,  68,  67,  66,  65,  64,  63,	\
    79,  80,  81,  82,  83,  84,  85,  86,	\
    87,  88,  89,  90,  91,  92,  93,  94,	\
   127						\
}

/* Use different register alloc ordering for Thumb.  */
#define ADJUST_REG_ALLOC_ORDER arm_order_regs_for_local_alloc ()

/* Tell IRA to use the order we define rather than messing it up with its
   own cost calculations.  */
#define HONOR_REG_ALLOC_ORDER

/* Interrupt functions can only use registers that have already been
   saved by the prologue, even if they would normally be
   call-clobbered.  */
#define HARD_REGNO_RENAME_OK(SRC, DST)					\
	(! IS_INTERRUPT (cfun->machine->func_type) ||			\
	 df_regs_ever_live_p (DST))

/* Register and constant classes.  */

/* Register classes: used to be simple, just all ARM regs or all FPA regs
   Now that the Thumb is involved it has become more complicated.  */
enum reg_class
{
  NO_REGS,
  FPA_REGS,
  CIRRUS_REGS,
  VFP_D0_D7_REGS,
  VFP_LO_REGS,
  VFP_HI_REGS,
  VFP_REGS,
  IWMMXT_GR_REGS,
  IWMMXT_REGS,
  LO_REGS,
  STACK_REG,
  BASE_REGS,
  HI_REGS,
  CC_REG,
  VFPCC_REG,
  GENERAL_REGS,
  CORE_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};

#define N_REG_CLASSES  (int) LIM_REG_CLASSES

/* Give names of register classes as strings for dump file.  */
#define REG_CLASS_NAMES  \
{			\
  "NO_REGS",		\
  "FPA_REGS",		\
  "CIRRUS_REGS",	\
  "VFP_D0_D7_REGS",	\
  "VFP_LO_REGS",	\
  "VFP_HI_REGS",	\
  "VFP_REGS",		\
  "IWMMXT_GR_REGS",	\
  "IWMMXT_REGS",	\
  "LO_REGS",		\
  "STACK_REG",		\
  "BASE_REGS",		\
  "HI_REGS",		\
  "CC_REG",		\
  "VFPCC_REG",		\
  "GENERAL_REGS",	\
  "CORE_REGS",		\
  "ALL_REGS",		\
}

/* Define which registers fit in which classes.
   This is an initializer for a vector of HARD_REG_SET
   of length N_REG_CLASSES.  */
#define REG_CLASS_CONTENTS						\
{									\
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* NO_REGS  */	\
  { 0x00FF0000, 0x00000000, 0x00000000, 0x00000000 }, /* FPA_REGS */	\
  { 0xF8000000, 0x000007FF, 0x00000000, 0x00000000 }, /* CIRRUS_REGS */	\
  { 0x00000000, 0x80000000, 0x00007FFF, 0x00000000 }, /* VFP_D0_D7_REGS  */ \
  { 0x00000000, 0x80000000, 0x7FFFFFFF, 0x00000000 }, /* VFP_LO_REGS  */ \
  { 0x00000000, 0x00000000, 0x80000000, 0x7FFFFFFF }, /* VFP_HI_REGS  */ \
  { 0x00000000, 0x80000000, 0xFFFFFFFF, 0x7FFFFFFF }, /* VFP_REGS  */	\
  { 0x00000000, 0x00007800, 0x00000000, 0x00000000 }, /* IWMMXT_GR_REGS */ \
  { 0x00000000, 0x7FFF8000, 0x00000000, 0x00000000 }, /* IWMMXT_REGS */	\
  { 0x000000FF, 0x00000000, 0x00000000, 0x00000000 }, /* LO_REGS */	\
  { 0x00002000, 0x00000000, 0x00000000, 0x00000000 }, /* STACK_REG */	\
  { 0x000020FF, 0x00000000, 0x00000000, 0x00000000 }, /* BASE_REGS */	\
  { 0x0000DF00, 0x00000000, 0x00000000, 0x00000000 }, /* HI_REGS */	\
  { 0x01000000, 0x00000000, 0x00000000, 0x00000000 }, /* CC_REG */	\
  { 0x00000000, 0x00000000, 0x00000000, 0x80000000 }, /* VFPCC_REG */	\
  { 0x0200DFFF, 0x00000000, 0x00000000, 0x00000000 }, /* GENERAL_REGS */ \
  { 0x0200FFFF, 0x00000000, 0x00000000, 0x00000000 }, /* CORE_REGS */	\
  { 0xFAFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF }  /* ALL_REGS */	\
}

/* Any of the VFP register classes.  */
#define IS_VFP_CLASS(X) \
  ((X) == VFP_D0_D7_REGS || (X) == VFP_LO_REGS \
   || (X) == VFP_HI_REGS || (X) == VFP_REGS)

/* The same information, inverted:
   Return the class number of the smallest class containing
   reg number REGNO.  This could be a conditional expression
   or could index an array.  */
#define REGNO_REG_CLASS(REGNO)  arm_regno_class (REGNO)

/* The following macro defines cover classes for Integrated Register
   Allocator.  Cover classes is a set of non-intersected register
   classes covering all hard registers used for register allocation
   purpose.  Any move between two registers of a cover class should be
   cheaper than load or store of the registers.  The macro value is
   array of register classes with LIM_REG_CLASSES used as the end
   marker.  */

#define IRA_COVER_CLASSES						     \
{									     \
  GENERAL_REGS, FPA_REGS, CIRRUS_REGS, VFP_REGS, IWMMXT_GR_REGS, IWMMXT_REGS,\
  LIM_REG_CLASSES							     \
}

/* FPA registers can't do subreg as all values are reformatted to internal
   precision.  VFP registers may only be accessed in the mode they
   were set.  */
#define CANNOT_CHANGE_MODE_CLASS(FROM, TO, CLASS)	\
  (GET_MODE_SIZE (FROM) != GET_MODE_SIZE (TO)		\
   ? reg_classes_intersect_p (FPA_REGS, (CLASS))	\
     || reg_classes_intersect_p (VFP_REGS, (CLASS))	\
   : 0)

/* We need to define this for LO_REGS on Thumb-1.  Otherwise we can end up
   using r0-r4 for function arguments, r7 for the stack frame and don't have
   enough left over to do doubleword arithmetic.  For Thumb-2 all the
   potentially problematic instructions accept high registers so this is not
   necessary.  Care needs to be taken to avoid adding new Thumb-2 patterns
   that require many low registers.  */
#define CLASS_LIKELY_SPILLED_P(CLASS)	\
    ((TARGET_THUMB1 && (CLASS) == LO_REGS)	\
     || (CLASS) == CC_REG)

/* The class value for index registers, and the one for base regs.  */
#define INDEX_REG_CLASS  (TARGET_THUMB1 ? LO_REGS : GENERAL_REGS)
#define BASE_REG_CLASS   (TARGET_THUMB1 ? LO_REGS : CORE_REGS)

/* For the Thumb the high registers cannot be used as base registers
   when addressing quantities in QI or HI mode; if we don't know the
   mode, then we must be conservative.  */
#define MODE_BASE_REG_CLASS(MODE)					\
    (TARGET_ARM || (TARGET_THUMB2 && !optimize_size) ? CORE_REGS :	\
     (((MODE) == SImode) ? BASE_REGS : LO_REGS))

/* For Thumb we can not support SP+reg addressing, so we return LO_REGS
   instead of BASE_REGS.  */
#define MODE_BASE_REG_REG_CLASS(MODE) BASE_REG_CLASS

/* When SMALL_REGISTER_CLASSES is nonzero, the compiler allows
   registers explicitly used in the rtl to be used as spill registers
   but prevents the compiler from extending the lifetime of these
   registers.  */
#define SMALL_REGISTER_CLASSES   TARGET_THUMB1

/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
   In general this is just CLASS, but for the Thumb core registers and
   immediate constants we prefer a LO_REGS class or a subset.  */
#define PREFERRED_RELOAD_CLASS(X, CLASS)		\
  (TARGET_32BIT ? (CLASS) :				\
   ((CLASS) == GENERAL_REGS || (CLASS) == HI_REGS	\
    || (CLASS) == NO_REGS || (CLASS) == STACK_REG	\
    || (CLASS) == CORE_REGS				\
   ? LO_REGS : (CLASS)))

/* Must leave BASE_REGS reloads alone */
#define THUMB_SECONDARY_INPUT_RELOAD_CLASS(CLASS, MODE, X)		\
  ((CLASS) != LO_REGS && (CLASS) != BASE_REGS				\
   ? ((true_regnum (X) == -1 ? LO_REGS					\
       : (true_regnum (X) + HARD_REGNO_NREGS (0, MODE) > 8) ? LO_REGS	\
       : NO_REGS)) 							\
   : NO_REGS)

#define THUMB_SECONDARY_OUTPUT_RELOAD_CLASS(CLASS, MODE, X)		\
  ((CLASS) != LO_REGS && (CLASS) != BASE_REGS				\
   ? ((true_regnum (X) == -1 ? LO_REGS					\
       : (true_regnum (X) + HARD_REGNO_NREGS (0, MODE) > 8) ? LO_REGS	\
       : NO_REGS)) 							\
   : NO_REGS)

/* Return the register class of a scratch register needed to copy IN into
   or out of a register in CLASS in MODE.  If it can be done directly,
   NO_REGS is returned.  */
#define SECONDARY_OUTPUT_RELOAD_CLASS(CLASS, MODE, X)		\
  /* Restrict which direct reloads are allowed for VFP/iWMMXt regs.  */ \
  ((TARGET_VFP && TARGET_HARD_FLOAT				\
    && IS_VFP_CLASS (CLASS))					\
   ? coproc_secondary_reload_class (MODE, X, FALSE)		\
   : (TARGET_IWMMXT && (CLASS) == IWMMXT_REGS)			\
   ? coproc_secondary_reload_class (MODE, X, TRUE)		\
   : TARGET_32BIT						\
   ? (((MODE) == HImode && ! arm_arch4 && true_regnum (X) == -1) \
    ? GENERAL_REGS : NO_REGS)					\
   : THUMB_SECONDARY_OUTPUT_RELOAD_CLASS (CLASS, MODE, X))

/* If we need to load shorts byte-at-a-time, then we need a scratch.  */
#define SECONDARY_INPUT_RELOAD_CLASS(CLASS, MODE, X)		\
  /* Restrict which direct reloads are allowed for VFP/iWMMXt regs.  */ \
  ((TARGET_VFP && TARGET_HARD_FLOAT				\
    && IS_VFP_CLASS (CLASS))					\
    ? coproc_secondary_reload_class (MODE, X, FALSE) :		\
    (TARGET_IWMMXT && (CLASS) == IWMMXT_REGS) ?			\
    coproc_secondary_reload_class (MODE, X, TRUE) :		\
  /* Cannot load constants into Cirrus registers.  */		\
   (TARGET_MAVERICK && TARGET_HARD_FLOAT			\
     && (CLASS) == CIRRUS_REGS					\
     && (CONSTANT_P (X) || GET_CODE (X) == SYMBOL_REF))		\
    ? GENERAL_REGS :						\
  (TARGET_32BIT ?						\
   (((CLASS) == IWMMXT_REGS || (CLASS) == IWMMXT_GR_REGS)	\
      && CONSTANT_P (X))					\
   ? GENERAL_REGS :						\
   (((MODE) == HImode && ! arm_arch4				\
     && (GET_CODE (X) == MEM					\
	 || ((GET_CODE (X) == REG || GET_CODE (X) == SUBREG)	\
	     && true_regnum (X) == -1)))			\
    ? GENERAL_REGS : NO_REGS)					\
   : THUMB_SECONDARY_INPUT_RELOAD_CLASS (CLASS, MODE, X)))

/* Try a machine-dependent way of reloading an illegitimate address
   operand.  If we find one, push the reload and jump to WIN.  This
   macro is used in only one place: `find_reloads_address' in reload.c.

   For the ARM, we wish to handle large displacements off a base
   register by splitting the addend across a MOV and the mem insn.
   This can cut the number of reloads needed.  */
#define ARM_LEGITIMIZE_RELOAD_ADDRESS(X, MODE, OPNUM, TYPE, IND, WIN)	   \
  do									   \
    {									   \
      if (GET_CODE (X) == PLUS						   \
	  && GET_CODE (XEXP (X, 0)) == REG				   \
	  && REGNO (XEXP (X, 0)) < FIRST_PSEUDO_REGISTER		   \
	  && REG_MODE_OK_FOR_BASE_P (XEXP (X, 0), MODE)			   \
	  && GET_CODE (XEXP (X, 1)) == CONST_INT)			   \
	{								   \
	  HOST_WIDE_INT val = INTVAL (XEXP (X, 1));			   \
	  HOST_WIDE_INT low, high;					   \
									   \
	  if (MODE == DImode || (MODE == DFmode && TARGET_SOFT_FLOAT))	   \
	    {								   \
	      if (TARGET_NEON && (val & 0x3) != 0)			   \
		break;							   \
	      low = ((val & 0xf) ^ 0x8) - 0x8;			   	   \
	    }								   \
	  else if (TARGET_MAVERICK && TARGET_HARD_FLOAT)		   \
	    /* Need to be careful, -256 is not a valid offset.  */	   \
	    low = val >= 0 ? (val & 0xff) : -((-val) & 0xff);		   \
	  else if (TARGET_REALLY_IWMMXT && MODE == SImode)		   \
	    /* Need to be careful, -1024 is not a valid offset.  */	   \
	    low = val >= 0 ? (val & 0x3ff) : -((-val) & 0x3ff);		   \
	  else if (MODE == SImode					   \
		   || (MODE == SFmode && TARGET_SOFT_FLOAT)		   \
		   || ((MODE == HImode || MODE == QImode) && ! arm_arch4)) \
	    /* Need to be careful, -4096 is not a valid offset.  */	   \
	    low = val >= 0 ? (val & 0xfff) : -((-val) & 0xfff);		   \
	  else if ((MODE == HImode || MODE == QImode) && arm_arch4)	   \
	    /* Need to be careful, -256 is not a valid offset.  */	   \
	    low = val >= 0 ? (val & 0xff) : -((-val) & 0xff);		   \
	  else if (GET_MODE_CLASS (MODE) == MODE_FLOAT			   \
		   && TARGET_HARD_FLOAT && TARGET_FPA)			   \
	    /* Need to be careful, -1024 is not a valid offset.  */	   \
	    low = val >= 0 ? (val & 0x3ff) : -((-val) & 0x3ff);		   \
	  else								   \
	    break;							   \
									   \
	  high = ((((val - low) & (unsigned HOST_WIDE_INT) 0xffffffff)	   \
		   ^ (unsigned HOST_WIDE_INT) 0x80000000)		   \
		  - (unsigned HOST_WIDE_INT) 0x80000000);		   \
	  /* Check for overflow or zero */				   \
	  if (low == 0 || high == 0 || (high + low != val))		   \
	    break;							   \
									   \
	  /* Reload the high part into a base reg; leave the low part	   \
	     in the mem.  */						   \
	  X = gen_rtx_PLUS (GET_MODE (X),				   \
			    gen_rtx_PLUS (GET_MODE (X), XEXP (X, 0),	   \
					  GEN_INT (high)),		   \
			    GEN_INT (low));				   \
	  push_reload (XEXP (X, 0), NULL_RTX, &XEXP (X, 0), NULL,	   \
		       MODE_BASE_REG_CLASS (MODE), GET_MODE (X), 	   \
		       VOIDmode, 0, 0, OPNUM, TYPE);			   \
	  goto WIN;							   \
	}								   \
    }									   \
  while (0)

/* XXX If an HImode FP+large_offset address is converted to an HImode
   SP+large_offset address, then reload won't know how to fix it.  It sees
   only that SP isn't valid for HImode, and so reloads the SP into an index
   register, but the resulting address is still invalid because the offset
   is too big.  We fix it here instead by reloading the entire address.  */
/* We could probably achieve better results by defining PROMOTE_MODE to help
   cope with the variances between the Thumb's signed and unsigned byte and
   halfword load instructions.  */
/* ??? This should be safe for thumb2, but we may be able to do better.  */
#define THUMB_LEGITIMIZE_RELOAD_ADDRESS(X, MODE, OPNUM, TYPE, IND_L, WIN)     \
do {									      \
  rtx new_x = thumb_legitimize_reload_address (&X, MODE, OPNUM, TYPE, IND_L); \
  if (new_x)								      \
    {									      \
      X = new_x;							      \
      goto WIN;								      \
    }									      \
} while (0)

#define LEGITIMIZE_RELOAD_ADDRESS(X, MODE, OPNUM, TYPE, IND_LEVELS, WIN)   \
  if (TARGET_ARM)							   \
    ARM_LEGITIMIZE_RELOAD_ADDRESS (X, MODE, OPNUM, TYPE, IND_LEVELS, WIN); \
  else									   \
    THUMB_LEGITIMIZE_RELOAD_ADDRESS (X, MODE, OPNUM, TYPE, IND_LEVELS, WIN)

/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.
   ARM regs are UNITS_PER_WORD bits while FPA regs can hold any FP mode */
#define CLASS_MAX_NREGS(CLASS, MODE)  \
  (((CLASS) == FPA_REGS || (CLASS) == CIRRUS_REGS) ? 1 : ARM_NUM_REGS (MODE))

/* If defined, gives a class of registers that cannot be used as the
   operand of a SUBREG that changes the mode of the object illegally.  */

/* Moves between FPA_REGS and GENERAL_REGS are two memory insns.
   Moves between VFP_REGS and GENERAL_REGS are a single insn, but
   it is typically more expensive than a single memory access.  We set
   the cost to less than two memory accesses so that floating
   point to integer conversion does not go through memory.  */
#define REGISTER_MOVE_COST(MODE, FROM, TO)		\
  (TARGET_32BIT ?						\
   ((FROM) == FPA_REGS && (TO) != FPA_REGS ? 20 :	\
    (FROM) != FPA_REGS && (TO) == FPA_REGS ? 20 :	\
    IS_VFP_CLASS (FROM) && !IS_VFP_CLASS (TO) ? 15 :	\
    !IS_VFP_CLASS (FROM) && IS_VFP_CLASS (TO) ? 15 :	\
    (FROM) == IWMMXT_REGS && (TO) != IWMMXT_REGS ? 4 :  \
    (FROM) != IWMMXT_REGS && (TO) == IWMMXT_REGS ? 4 :  \
    (FROM) == IWMMXT_GR_REGS || (TO) == IWMMXT_GR_REGS ? 20 :  \
    (FROM) == CIRRUS_REGS && (TO) != CIRRUS_REGS ? 20 :	\
    (FROM) != CIRRUS_REGS && (TO) == CIRRUS_REGS ? 20 :	\
   2)							\
   :							\
   ((FROM) == HI_REGS || (TO) == HI_REGS) ? 4 : 2)

/* Stack layout; function entry, exit and calling.  */

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */
#define STACK_GROWS_DOWNWARD  1

/* Define this to nonzero if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
#define FRAME_GROWS_DOWNWARD 1

/* The amount of scratch space needed by _interwork_{r7,r11}_call_via_rN().
   When present, it is one word in size, and sits at the top of the frame,
   between the soft frame pointer and either r7 or r11.

   We only need _interwork_rM_call_via_rN() for -mcaller-super-interworking,
   and only then if some outgoing arguments are passed on the stack.  It would
   be tempting to also check whether the stack arguments are passed by indirect
   calls, but there seems to be no reason in principle why a post-reload pass
   couldn't convert a direct call into an indirect one.  */
#define CALLER_INTERWORKING_SLOT_SIZE			\
  (TARGET_CALLER_INTERWORKING				\
   && crtl->outgoing_args_size != 0		\
   ? UNITS_PER_WORD : 0)

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */
#define STARTING_FRAME_OFFSET  0

/* If we generate an insn to push BYTES bytes,
   this says how many the stack pointer really advances by.  */
/* The push insns do not do this rounding implicitly.
   So don't define this.  */
/* #define PUSH_ROUNDING(NPUSHED)  ROUND_UP_WORD (NPUSHED) */

/* Define this if the maximum size of all the outgoing args is to be
   accumulated and pushed during the prologue.  The amount can be
   found in the variable crtl->outgoing_args_size.  */
#define ACCUMULATE_OUTGOING_ARGS 1

/* Offset of first parameter from the argument pointer register value.  */
#define FIRST_PARM_OFFSET(FNDECL)  (TARGET_ARM ? 4 : 0)

/* Value is the number of byte of arguments automatically
   popped when returning from a subroutine call.
   FUNDECL is the declaration node of the function (as a tree),
   FUNTYPE is the data type of the function (as a tree),
   or for a library call it is an identifier node for the subroutine name.
   SIZE is the number of bytes of arguments passed on the stack.

   On the ARM, the caller does not pop any of its arguments that were passed
   on the stack.  */
#define RETURN_POPS_ARGS(FUNDECL, FUNTYPE, SIZE)  0

/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */
#define LIBCALL_VALUE(MODE)  						\
  (TARGET_AAPCS_BASED ? aapcs_libcall_value (MODE)			\
   : (TARGET_32BIT && TARGET_HARD_FLOAT_ABI && TARGET_FPA		\
      && GET_MODE_CLASS (MODE) == MODE_FLOAT)				\
   ? gen_rtx_REG (MODE, FIRST_FPA_REGNUM)				\
   : TARGET_32BIT && TARGET_HARD_FLOAT_ABI && TARGET_MAVERICK		\
     && GET_MODE_CLASS (MODE) == MODE_FLOAT				\
   ? gen_rtx_REG (MODE, FIRST_CIRRUS_FP_REGNUM) 			\
   : TARGET_IWMMXT_ABI && arm_vector_mode_supported_p (MODE)    	\
   ? gen_rtx_REG (MODE, FIRST_IWMMXT_REGNUM) 				\
   : gen_rtx_REG (MODE, ARG_REGISTER (1)))

/* 1 if REGNO is a possible register number for a function value.  */
#define FUNCTION_VALUE_REGNO_P(REGNO)				\
  ((REGNO) == ARG_REGISTER (1)					\
   || (TARGET_AAPCS_BASED && TARGET_32BIT 			\
       && TARGET_VFP && TARGET_HARD_FLOAT			\
       && (REGNO) == FIRST_VFP_REGNUM)				\
   || (TARGET_32BIT && ((REGNO) == FIRST_CIRRUS_FP_REGNUM)	\
       && TARGET_HARD_FLOAT_ABI && TARGET_MAVERICK)		\
   || ((REGNO) == FIRST_IWMMXT_REGNUM && TARGET_IWMMXT_ABI)	\
   || (TARGET_32BIT && ((REGNO) == FIRST_FPA_REGNUM)		\
       && TARGET_HARD_FLOAT_ABI && TARGET_FPA))

/* Amount of memory needed for an untyped call to save all possible return
   registers.  */
#define APPLY_RESULT_SIZE arm_apply_result_size()

/* Define DEFAULT_PCC_STRUCT_RETURN to 1 if all structure and union return
   values must be in memory.  On the ARM, they need only do so if larger
   than a word, or if they contain elements offset from zero in the struct.  */
#define DEFAULT_PCC_STRUCT_RETURN 0

/* These bits describe the different types of function supported
   by the ARM backend.  They are exclusive.  i.e. a function cannot be both a
   normal function and an interworked function, for example.  Knowing the
   type of a function is important for determining its prologue and
   epilogue sequences.
   Note value 7 is currently unassigned.  Also note that the interrupt
   function types all have bit 2 set, so that they can be tested for easily.
   Note that 0 is deliberately chosen for ARM_FT_UNKNOWN so that when the
   machine_function structure is initialized (to zero) func_type will
   default to unknown.  This will force the first use of arm_current_func_type
   to call arm_compute_func_type.  */
#define ARM_FT_UNKNOWN		 0 /* Type has not yet been determined.  */
#define ARM_FT_NORMAL		 1 /* Your normal, straightforward function.  */
#define ARM_FT_INTERWORKED	 2 /* A function that supports interworking.  */
#define ARM_FT_ISR		 4 /* An interrupt service routine.  */
#define ARM_FT_FIQ		 5 /* A fast interrupt service routine.  */
#define ARM_FT_EXCEPTION	 6 /* An ARM exception handler (subcase of ISR).  */

#define ARM_FT_TYPE_MASK	((1 << 3) - 1)

/* In addition functions can have several type modifiers,
   outlined by these bit masks:  */
#define ARM_FT_INTERRUPT	(1 << 2) /* Note overlap with FT_ISR and above.  */
#define ARM_FT_NAKED		(1 << 3) /* No prologue or epilogue.  */
#define ARM_FT_VOLATILE		(1 << 4) /* Does not return.  */
#define ARM_FT_NESTED		(1 << 5) /* Embedded inside another func.  */
#define ARM_FT_STACKALIGN	(1 << 6) /* Called with misaligned stack.  */

/* Some macros to test these flags.  */
#define ARM_FUNC_TYPE(t)	(t & ARM_FT_TYPE_MASK)
#define IS_INTERRUPT(t)		(t & ARM_FT_INTERRUPT)
#define IS_VOLATILE(t)     	(t & ARM_FT_VOLATILE)
#define IS_NAKED(t)        	(t & ARM_FT_NAKED)
#define IS_NESTED(t)       	(t & ARM_FT_NESTED)
#define IS_STACKALIGN(t)       	(t & ARM_FT_STACKALIGN)


/* Structure used to hold the function stack frame layout.  Offsets are
   relative to the stack pointer on function entry.  Positive offsets are
   in the direction of stack growth.
   Only soft_frame is used in thumb mode.  */

typedef struct GTY(()) arm_stack_offsets
{
  int saved_args;	/* ARG_POINTER_REGNUM.  */
  int frame;		/* ARM_HARD_FRAME_POINTER_REGNUM.  */
  int saved_regs;
  int soft_frame;	/* FRAME_POINTER_REGNUM.  */
  int locals_base;	/* THUMB_HARD_FRAME_POINTER_REGNUM.  */
  int outgoing_args;	/* STACK_POINTER_REGNUM.  */
  unsigned int saved_regs_mask;
}
arm_stack_offsets;

/* A C structure for machine-specific, per-function data.
   This is added to the cfun structure.  */
typedef struct GTY(()) machine_function
{
  /* Additional stack adjustment in __builtin_eh_throw.  */
  rtx eh_epilogue_sp_ofs;
  /* Records if LR has to be saved for far jumps.  */
  int far_jump_used;
  /* Records if ARG_POINTER was ever live.  */
  int arg_pointer_live;
  /* Records if the save of LR has been eliminated.  */
  int lr_save_eliminated;
  /* The size of the stack frame.  Only valid after reload.  */
  arm_stack_offsets stack_offsets;
  /* Records the type of the current function.  */
  unsigned long func_type;
  /* Record if the function has a variable argument list.  */
  int uses_anonymous_args;
  /* Records if sibcalls are blocked because an argument
     register is needed to preserve stack alignment.  */
  int sibcall_blocked;
  /* The PIC register for this function.  This might be a pseudo.  */
  rtx pic_reg;
  /* Labels for per-function Thumb call-via stubs.  One per potential calling
     register.  We can never call via LR or PC.  We can call via SP if a
     trampoline happens to be on the top of the stack.  */
  rtx call_via[14];
  /* Set to 1 when a return insn is output, this means that the epilogue
     is not needed.  */
  int return_used_this_function;
}
machine_function;

/* As in the machine_function, a global set of call-via labels, for code 
   that is in text_section.  */
extern GTY(()) rtx thumb_call_via_label[14];

/* The number of potential ways of assigning to a co-processor.  */
#define ARM_NUM_COPROC_SLOTS 1

/* Enumeration of procedure calling standard variants.  We don't really 
   support all of these yet.  */
enum arm_pcs
{
  ARM_PCS_AAPCS,	/* Base standard AAPCS.  */
  ARM_PCS_AAPCS_VFP,	/* Use VFP registers for floating point values.  */
  ARM_PCS_AAPCS_IWMMXT, /* Use iWMMXT registers for vectors.  */
  /* This must be the last AAPCS variant.  */
  ARM_PCS_AAPCS_LOCAL,	/* Private call within this compilation unit.  */
  ARM_PCS_ATPCS,	/* ATPCS.  */
  ARM_PCS_APCS,		/* APCS (legacy Linux etc).  */
  ARM_PCS_UNKNOWN
};

/* Default procedure calling standard of current compilation unit. */
extern enum arm_pcs arm_pcs_default;

/* A C type for declaring a variable that is used as the first argument of
   `FUNCTION_ARG' and other related values.  */
typedef struct
{
  /* This is the number of registers of arguments scanned so far.  */
  int nregs;
  /* This is the number of iWMMXt register arguments scanned so far.  */
  int iwmmxt_nregs;
  int named_count;
  int nargs;
  /* Which procedure call variant to use for this call.  */
  enum arm_pcs pcs_variant;

  /* AAPCS related state tracking.  */
  int aapcs_arg_processed;  /* No need to lay out this argument again.  */
  int aapcs_cprc_slot;      /* Index of co-processor rules to handle
			       this argument, or -1 if using core
			       registers.  */
  int aapcs_ncrn;
  int aapcs_next_ncrn;
  rtx aapcs_reg;	    /* Register assigned to this argument.  */
  int aapcs_partial;	    /* How many bytes are passed in regs (if
			       split between core regs and stack.
			       Zero otherwise.  */
  int aapcs_cprc_failed[ARM_NUM_COPROC_SLOTS];
  int can_split;	    /* Argument can be split between core regs
			       and the stack.  */
  /* Private data for tracking VFP register allocation */
  unsigned aapcs_vfp_regs_free;
  unsigned aapcs_vfp_reg_alloc;
  int aapcs_vfp_rcount;
  MACHMODE aapcs_vfp_rmode;
} CUMULATIVE_ARGS;

/* Define where to put the arguments to a function.
   Value is zero to push the argument on the stack,
   or a hard register in which to store the argument.

   MODE is the argument's machine mode.
   TYPE is the data type of the argument (as a tree).
    This is null for libcalls where that information may
    not be available.
   CUM is a variable of type CUMULATIVE_ARGS which gives info about
    the preceding args and about the function being called.
   NAMED is nonzero if this argument is a named parameter
    (otherwise it is an extra parameter matching an ellipsis).

   On the ARM, normally the first 16 bytes are passed in registers r0-r3; all
   other arguments are passed on the stack.  If (NAMED == 0) (which happens
   only in assign_parms, since TARGET_SETUP_INCOMING_VARARGS is
   defined), say it is passed in the stack (function_prologue will
   indeed make it pass in the stack if necessary).  */
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
  arm_function_arg (&(CUM), (MODE), (TYPE), (NAMED))

#define FUNCTION_ARG_PADDING(MODE, TYPE) \
  (arm_pad_arg_upward (MODE, TYPE) ? upward : downward)

#define BLOCK_REG_PADDING(MODE, TYPE, FIRST) \
  (arm_pad_reg_upward (MODE, TYPE, FIRST) ? upward : downward)

/* For AAPCS, padding should never be below the argument. For other ABIs,
 * mimic the default.  */
#define PAD_VARARGS_DOWN \
  ((TARGET_AAPCS_BASED) ? 0 : BYTES_BIG_ENDIAN)

/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.
   On the ARM, the offset starts at 0.  */
#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS) \
  arm_init_cumulative_args (&(CUM), (FNTYPE), (LIBNAME), (FNDECL))

/* Update the data in CUM to advance over an argument
   of mode MODE and data type TYPE.
   (TYPE is null for libcalls where that information may not be available.)  */
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)	\
  arm_function_arg_advance (&(CUM), (MODE), (TYPE), (NAMED))

/* If defined, a C expression that gives the alignment boundary, in bits, of an
   argument with the specified mode and type.  If it is not defined,
   `PARM_BOUNDARY' is used for all arguments.  */
#define FUNCTION_ARG_BOUNDARY(MODE,TYPE) \
   ((ARM_DOUBLEWORD_ALIGN && arm_needs_doubleword_align (MODE, TYPE)) \
   ? DOUBLEWORD_ALIGNMENT \
   : PARM_BOUNDARY )

/* 1 if N is a possible register number for function argument passing.
   On the ARM, r0-r3 are used to pass args.  */
#define FUNCTION_ARG_REGNO_P(REGNO)					\
   (IN_RANGE ((REGNO), 0, 3)						\
    || (TARGET_AAPCS_BASED && TARGET_VFP && TARGET_HARD_FLOAT		\
	&& IN_RANGE ((REGNO), FIRST_VFP_REGNUM, FIRST_VFP_REGNUM + 15))	\
    || (TARGET_IWMMXT_ABI						\
	&& IN_RANGE ((REGNO), FIRST_IWMMXT_REGNUM, FIRST_IWMMXT_REGNUM + 9)))


/* If your target environment doesn't prefix user functions with an
   underscore, you may wish to re-define this to prevent any conflicts.  */
#ifndef ARM_MCOUNT_NAME
#define ARM_MCOUNT_NAME "*mcount"
#endif

/* Call the function profiler with a given profile label.  The Acorn
   compiler puts this BEFORE the prolog but gcc puts it afterwards.
   On the ARM the full profile code will look like:
	.data
	LP1
		.word	0
	.text
		mov	ip, lr
		bl	mcount
		.word	LP1

   profile_function() in final.c outputs the .data section, FUNCTION_PROFILER
   will output the .text section.

   The ``mov ip,lr'' seems like a good idea to stick with cc convention.
   ``prof'' doesn't seem to mind about this!

   Note - this version of the code is designed to work in both ARM and
   Thumb modes.  */
#ifndef ARM_FUNCTION_PROFILER
#define ARM_FUNCTION_PROFILER(STREAM, LABELNO)  	\
{							\
  char temp[20];					\
  rtx sym;						\
							\
  asm_fprintf (STREAM, "\tmov\t%r, %r\n\tbl\t",		\
	   IP_REGNUM, LR_REGNUM);			\
  assemble_name (STREAM, ARM_MCOUNT_NAME);		\
  fputc ('\n', STREAM);					\
  ASM_GENERATE_INTERNAL_LABEL (temp, "LP", LABELNO);	\
  sym = gen_rtx_SYMBOL_REF (Pmode, temp);		\
  assemble_aligned_integer (UNITS_PER_WORD, sym);	\
}
#endif

#ifdef THUMB_FUNCTION_PROFILER
#define FUNCTION_PROFILER(STREAM, LABELNO)		\
  if (TARGET_ARM)					\
    ARM_FUNCTION_PROFILER (STREAM, LABELNO)		\
  else							\
    THUMB_FUNCTION_PROFILER (STREAM, LABELNO)
#else
#define FUNCTION_PROFILER(STREAM, LABELNO)		\
    ARM_FUNCTION_PROFILER (STREAM, LABELNO)
#endif

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.

   On the ARM, the function epilogue recovers the stack pointer from the
   frame.  */
#define EXIT_IGNORE_STACK 1

#define EPILOGUE_USES(REGNO) ((REGNO) == LR_REGNUM)

/* Determine if the epilogue should be output as RTL.
   You should override this if you define FUNCTION_EXTRA_EPILOGUE.  */
#define USE_RETURN_INSN(ISCOND)				\
  ((TARGET_32BIT && !low_irq_latency) ? use_return_insn (ISCOND, NULL) : 0)

/* Definitions for register eliminations.

   This is an array of structures.  Each structure initializes one pair
   of eliminable registers.  The "from" register number is given first,
   followed by "to".  Eliminations of the same "from" register are listed
   in order of preference.

   We have two registers that can be eliminated on the ARM.  First, the
   arg pointer register can often be eliminated in favor of the stack
   pointer register.  Secondly, the pseudo frame pointer register can always
   be eliminated; it is replaced with either the stack or the real frame
   pointer.  Note we have to use {ARM|THUMB}_HARD_FRAME_POINTER_REGNUM
   because the definition of HARD_FRAME_POINTER_REGNUM is not a constant.  */

#define ELIMINABLE_REGS						\
{{ ARG_POINTER_REGNUM,        STACK_POINTER_REGNUM            },\
 { ARG_POINTER_REGNUM,        FRAME_POINTER_REGNUM            },\
 { ARG_POINTER_REGNUM,        ARM_HARD_FRAME_POINTER_REGNUM   },\
 { ARG_POINTER_REGNUM,        THUMB_HARD_FRAME_POINTER_REGNUM },\
 { FRAME_POINTER_REGNUM,      STACK_POINTER_REGNUM            },\
 { FRAME_POINTER_REGNUM,      ARM_HARD_FRAME_POINTER_REGNUM   },\
 { FRAME_POINTER_REGNUM,      THUMB_HARD_FRAME_POINTER_REGNUM }}

/* Define the offset between two registers, one to be eliminated, and the
   other its replacement, at the start of a routine.  */
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  if (TARGET_ARM)							\
    (OFFSET) = arm_compute_initial_elimination_offset (FROM, TO);	\
  else									\
    (OFFSET) = thumb_compute_initial_elimination_offset (FROM, TO)

/* Special case handling of the location of arguments passed on the stack.  */
#define DEBUGGER_ARG_OFFSET(value, addr) value ? value : arm_debugger_arg_offset (value, addr)

/* Initialize data used by insn expanders.  This is called from insn_emit,
   once for every function before code is generated.  */
#define INIT_EXPANDERS  arm_init_expanders ()

/* Length in units of the trampoline for entering a nested function.  */
#define TRAMPOLINE_SIZE  (TARGET_32BIT ? 16 : 20)

/* Alignment required for a trampoline in bits.  */
#define TRAMPOLINE_ALIGNMENT  32

/* Addressing modes, and classification of registers for them.  */
#define HAVE_POST_INCREMENT   1
#define HAVE_PRE_INCREMENT    TARGET_32BIT
#define HAVE_POST_DECREMENT   TARGET_32BIT
#define HAVE_PRE_DECREMENT    TARGET_32BIT
#define HAVE_PRE_MODIFY_DISP  TARGET_32BIT
#define HAVE_POST_MODIFY_DISP TARGET_32BIT
#define HAVE_PRE_MODIFY_REG   TARGET_32BIT
#define HAVE_POST_MODIFY_REG  TARGET_32BIT

/* Macros to check register numbers against specific register classes.  */

/* These assume that REGNO is a hard or pseudo reg number.
   They give nonzero only if REGNO is a hard reg of the suitable class
   or a pseudo reg currently allocated to a suitable hard reg.
   Since they use reg_renumber, they are safe only once reg_renumber
   has been allocated, which happens in local-alloc.c.  */
#define TEST_REGNO(R, TEST, VALUE) \
  ((R TEST VALUE) || ((unsigned) reg_renumber[R] TEST VALUE))

/* Don't allow the pc to be used.  */
#define ARM_REGNO_OK_FOR_BASE_P(REGNO)			\
  (TEST_REGNO (REGNO, <, PC_REGNUM)			\
   || TEST_REGNO (REGNO, ==, FRAME_POINTER_REGNUM)	\
   || TEST_REGNO (REGNO, ==, ARG_POINTER_REGNUM))

#define THUMB1_REGNO_MODE_OK_FOR_BASE_P(REGNO, MODE)		\
  (TEST_REGNO (REGNO, <=, LAST_LO_REGNUM)			\
   || (GET_MODE_SIZE (MODE) >= 4				\
       && TEST_REGNO (REGNO, ==, STACK_POINTER_REGNUM)))

#define REGNO_MODE_OK_FOR_BASE_P(REGNO, MODE)		\
  (TARGET_THUMB1					\
   ? THUMB1_REGNO_MODE_OK_FOR_BASE_P (REGNO, MODE)	\
   : ARM_REGNO_OK_FOR_BASE_P (REGNO))

/* Nonzero if X can be the base register in a reg+reg addressing mode.
   For Thumb, we can not use SP + reg, so reject SP.  */
#define REGNO_MODE_OK_FOR_REG_BASE_P(X, MODE)	\
  REGNO_MODE_OK_FOR_BASE_P (X, QImode)

/* For ARM code, we don't care about the mode, but for Thumb, the index
   must be suitable for use in a QImode load.  */
#define REGNO_OK_FOR_INDEX_P(REGNO)	\
  (REGNO_MODE_OK_FOR_BASE_P (REGNO, QImode) \
   && !TEST_REGNO (REGNO, ==, STACK_POINTER_REGNUM))

/* Maximum number of registers that can appear in a valid memory address.
   Shifts in addresses can't be by a register.  */
#define MAX_REGS_PER_ADDRESS 2

/* Recognize any constant value that is a valid address.  */
/* XXX We can address any constant, eventually...  */
/* ??? Should the TARGET_ARM here also apply to thumb2?  */
#define CONSTANT_ADDRESS_P(X)  			\
  (GET_CODE (X) == SYMBOL_REF 			\
   && (CONSTANT_POOL_ADDRESS_P (X)		\
       || (TARGET_ARM && optimize > 0 && SYMBOL_REF_FLAG (X))))

/* True if SYMBOL + OFFSET constants must refer to something within
   SYMBOL's section.  */
#define ARM_OFFSETS_MUST_BE_WITHIN_SECTIONS_P 0

/* Nonzero if all target requires all absolute relocations be R_ARM_ABS32.  */
#ifndef TARGET_DEFAULT_WORD_RELOCATIONS
#define TARGET_DEFAULT_WORD_RELOCATIONS 0
#endif

/* Nonzero if the constant value X is a legitimate general operand.
   It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE.

   On the ARM, allow any integer (invalid ones are removed later by insn
   patterns), nice doubles and symbol_refs which refer to the function's
   constant pool XXX.

   When generating pic allow anything.  */
#define ARM_LEGITIMATE_CONSTANT_P(X)	(flag_pic || ! label_mentioned_p (X))

#define THUMB_LEGITIMATE_CONSTANT_P(X)	\
 (   GET_CODE (X) == CONST_INT		\
  || GET_CODE (X) == CONST_DOUBLE	\
  || CONSTANT_ADDRESS_P (X)		\
  || flag_pic)

#define LEGITIMATE_CONSTANT_P(X)			\
  (!arm_cannot_force_const_mem (X)			\
   && (TARGET_32BIT ? ARM_LEGITIMATE_CONSTANT_P (X)	\
		    : THUMB_LEGITIMATE_CONSTANT_P (X)))

#ifndef SUBTARGET_NAME_ENCODING_LENGTHS
#define SUBTARGET_NAME_ENCODING_LENGTHS
#endif

/* This is a C fragment for the inside of a switch statement.
   Each case label should return the number of characters to
   be stripped from the start of a function's name, if that
   name starts with the indicated character.  */
#define ARM_NAME_ENCODING_LENGTHS		\
  case '*':  return 1;				\
  SUBTARGET_NAME_ENCODING_LENGTHS

/* This is how to output a reference to a user-level label named NAME.
   `assemble_name' uses this.  */
#undef  ASM_OUTPUT_LABELREF
#define ASM_OUTPUT_LABELREF(FILE, NAME)		\
   arm_asm_output_labelref (FILE, NAME)

/* Output IT instructions for conditionally executed Thumb-2 instructions.  */
#define ASM_OUTPUT_OPCODE(STREAM, PTR)	\
  if (TARGET_THUMB2)			\
    thumb2_asm_output_opcode (STREAM);

/* The EABI specifies that constructors should go in .init_array.
   Other targets use .ctors for compatibility.  */
#ifndef ARM_EABI_CTORS_SECTION_OP
#define ARM_EABI_CTORS_SECTION_OP \
  "\t.section\t.init_array,\"aw\",%init_array"
#endif
#ifndef ARM_EABI_DTORS_SECTION_OP
#define ARM_EABI_DTORS_SECTION_OP \
  "\t.section\t.fini_array,\"aw\",%fini_array"
#endif
#define ARM_CTORS_SECTION_OP \
  "\t.section\t.ctors,\"aw\",%progbits"
#define ARM_DTORS_SECTION_OP \
  "\t.section\t.dtors,\"aw\",%progbits"

/* Define CTORS_SECTION_ASM_OP.  */
#undef CTORS_SECTION_ASM_OP
#undef DTORS_SECTION_ASM_OP
#ifndef IN_LIBGCC2
# define CTORS_SECTION_ASM_OP \
   (TARGET_AAPCS_BASED ? ARM_EABI_CTORS_SECTION_OP : ARM_CTORS_SECTION_OP)
# define DTORS_SECTION_ASM_OP \
   (TARGET_AAPCS_BASED ? ARM_EABI_DTORS_SECTION_OP : ARM_DTORS_SECTION_OP)
#else /* !defined (IN_LIBGCC2) */
/* In libgcc, CTORS_SECTION_ASM_OP must be a compile-time constant,
   so we cannot use the definition above.  */
# ifdef __ARM_EABI__
/* The .ctors section is not part of the EABI, so we do not define
   CTORS_SECTION_ASM_OP when in libgcc; that prevents crtstuff
   from trying to use it.  We do define it when doing normal
   compilation, as .init_array can be used instead of .ctors.  */
/* There is no need to emit begin or end markers when using
   init_array; the dynamic linker will compute the size of the
   array itself based on special symbols created by the static
   linker.  However, we do need to arrange to set up
   exception-handling here.  */
#   define CTOR_LIST_BEGIN asm (ARM_EABI_CTORS_SECTION_OP)
#   define CTOR_LIST_END /* empty */
#   define DTOR_LIST_BEGIN asm (ARM_EABI_DTORS_SECTION_OP)
#   define DTOR_LIST_END /* empty */
# else /* !defined (__ARM_EABI__) */
#   define CTORS_SECTION_ASM_OP ARM_CTORS_SECTION_OP
#   define DTORS_SECTION_ASM_OP ARM_DTORS_SECTION_OP
# endif /* !defined (__ARM_EABI__) */
#endif /* !defined (IN_LIBCC2) */

/* True if the operating system can merge entities with vague linkage
   (e.g., symbols in COMDAT group) during dynamic linking.  */
#ifndef TARGET_ARM_DYNAMIC_VAGUE_LINKAGE_P
#define TARGET_ARM_DYNAMIC_VAGUE_LINKAGE_P true
#endif

#define ARM_OUTPUT_FN_UNWIND(F, PROLOGUE) arm_output_fn_unwind (F, PROLOGUE)

#ifdef TARGET_UNWIND_INFO
#define ARM_EABI_UNWIND_TABLES \
  ((!USING_SJLJ_EXCEPTIONS && flag_exceptions) || flag_unwind_tables)
#else
#define ARM_EABI_UNWIND_TABLES 0
#endif

/* The macros REG_OK_FOR..._P assume that the arg is a REG rtx
   and check its validity for a certain class.
   We have two alternate definitions for each of them.
   The usual definition accepts all pseudo regs; the other rejects
   them unless they have been allocated suitable hard regs.
   The symbol REG_OK_STRICT causes the latter definition to be used.
   Thumb-2 has the same restrictions as arm.  */
#ifndef REG_OK_STRICT

#define ARM_REG_OK_FOR_BASE_P(X)		\
  (REGNO (X) <= LAST_ARM_REGNUM			\
   || REGNO (X) >= FIRST_PSEUDO_REGISTER	\
   || REGNO (X) == FRAME_POINTER_REGNUM		\
   || REGNO (X) == ARG_POINTER_REGNUM)

#define ARM_REG_OK_FOR_INDEX_P(X)		\
  ((REGNO (X) <= LAST_ARM_REGNUM		\
    && REGNO (X) != STACK_POINTER_REGNUM)	\
   || REGNO (X) >= FIRST_PSEUDO_REGISTER	\
   || REGNO (X) == FRAME_POINTER_REGNUM		\
   || REGNO (X) == ARG_POINTER_REGNUM)

#define THUMB1_REG_MODE_OK_FOR_BASE_P(X, MODE)	\
  (REGNO (X) <= LAST_LO_REGNUM			\
   || REGNO (X) >= FIRST_PSEUDO_REGISTER	\
   || (GET_MODE_SIZE (MODE) >= 4		\
       && (REGNO (X) == STACK_POINTER_REGNUM	\
	   || (X) == hard_frame_pointer_rtx	\
	   || (X) == arg_pointer_rtx)))

#define REG_STRICT_P 0

#else /* REG_OK_STRICT */

#define ARM_REG_OK_FOR_BASE_P(X) 		\
  ARM_REGNO_OK_FOR_BASE_P (REGNO (X))

#define ARM_REG_OK_FOR_INDEX_P(X) 		\
  ARM_REGNO_OK_FOR_INDEX_P (REGNO (X))

#define THUMB1_REG_MODE_OK_FOR_BASE_P(X, MODE)	\
  THUMB1_REGNO_MODE_OK_FOR_BASE_P (REGNO (X), MODE)

#define REG_STRICT_P 1

#endif /* REG_OK_STRICT */

/* Now define some helpers in terms of the above.  */

#define REG_MODE_OK_FOR_BASE_P(X, MODE)		\
  (TARGET_THUMB1				\
   ? THUMB1_REG_MODE_OK_FOR_BASE_P (X, MODE)	\
   : ARM_REG_OK_FOR_BASE_P (X))

/* For 16-bit Thumb, a valid index register is anything that can be used in
   a byte load instruction.  */
#define THUMB1_REG_OK_FOR_INDEX_P(X) \
  THUMB1_REG_MODE_OK_FOR_BASE_P (X, QImode)

/* Nonzero if X is a hard reg that can be used as an index
   or if it is a pseudo reg.  On the Thumb, the stack pointer
   is not suitable.  */
#define REG_OK_FOR_INDEX_P(X)			\
  (TARGET_THUMB1				\
   ? THUMB1_REG_OK_FOR_INDEX_P (X)		\
   : ARM_REG_OK_FOR_INDEX_P (X))

/* Nonzero if X can be the base register in a reg+reg addressing mode.
   For Thumb, we can not use SP + reg, so reject SP.  */
#define REG_MODE_OK_FOR_REG_BASE_P(X, MODE)	\
  REG_OK_FOR_INDEX_P (X)

#define ARM_BASE_REGISTER_RTX_P(X)  \
  (GET_CODE (X) == REG && ARM_REG_OK_FOR_BASE_P (X))

#define ARM_INDEX_REGISTER_RTX_P(X)  \
  (GET_CODE (X) == REG && ARM_REG_OK_FOR_INDEX_P (X))

/* Define this for compatibility reasons. */
#define HANDLE_PRAGMA_PACK_PUSH_POP

/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE Pmode

#define CASE_VECTOR_PC_RELATIVE (TARGET_THUMB2				\
				 || (TARGET_THUMB1			\
				     && (optimize_size || flag_pic)))

#define CASE_VECTOR_SHORTEN_MODE(min, max, body)			\
  (TARGET_THUMB1							\
   ? (min >= 0 && max < 512						\
      ? (ADDR_DIFF_VEC_FLAGS (body).offset_unsigned = 1, QImode)	\
      : min >= -256 && max < 256					\
      ? (ADDR_DIFF_VEC_FLAGS (body).offset_unsigned = 0, QImode)	\
      : min >= 0 && max < 8192						\
      ? (ADDR_DIFF_VEC_FLAGS (body).offset_unsigned = 1, HImode)	\
      : min >= -4096 && max < 4096					\
      ? (ADDR_DIFF_VEC_FLAGS (body).offset_unsigned = 0, HImode)	\
      : SImode)								\
   : ((min < 0 || max >= 0x2000 || !TARGET_THUMB2) ? SImode		\
      : (max >= 0x200) ? HImode						\
      : QImode))

/* signed 'char' is most compatible, but RISC OS wants it unsigned.
   unsigned is probably best, but may break some code.  */
#ifndef DEFAULT_SIGNED_CHAR
#define DEFAULT_SIGNED_CHAR  0
#endif

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX 4

#undef  MOVE_RATIO
#define MOVE_RATIO(speed) (arm_tune_xscale ? 4 : 2)

/* Define if operations between registers always perform the operation
   on the full register even if a narrower mode is specified.  */
#define WORD_REGISTER_OPERATIONS

/* Define if loading in MODE, an integral mode narrower than BITS_PER_WORD
   will either zero-extend or sign-extend.  The value of this macro should
   be the code that says which one of the two operations is implicitly
   done, UNKNOWN if none.  */
#define LOAD_EXTEND_OP(MODE)						\
  (TARGET_THUMB ? ZERO_EXTEND :						\
   ((arm_arch4 || (MODE) == QImode) ? ZERO_EXTEND			\
    : ((BYTES_BIG_ENDIAN && (MODE) == HImode) ? SIGN_EXTEND : UNKNOWN)))

/* Nonzero if access to memory by bytes is slow and undesirable.  */
#define SLOW_BYTE_ACCESS 0

#define SLOW_UNALIGNED_ACCESS(MODE, ALIGN) 1

/* Immediate shift counts are truncated by the output routines (or was it
   the assembler?).  Shift counts in a register are truncated by ARM.  Note
   that the native compiler puts too large (> 32) immediate shift counts
   into a register and shifts by the register, letting the ARM decide what
   to do instead of doing that itself.  */
/* This is all wrong.  Defining SHIFT_COUNT_TRUNCATED tells combine that
   code like (X << (Y % 32)) for register X, Y is equivalent to (X << Y).
   On the arm, Y in a register is used modulo 256 for the shift. Only for
   rotates is modulo 32 used.  */
/* #define SHIFT_COUNT_TRUNCATED 1 */

/* All integers have the same format so truncation is easy.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC)  1

/* Calling from registers is a massive pain.  */
#define NO_FUNCTION_CSE 1

/* The machine modes of pointers and functions */
#define Pmode  SImode
#define FUNCTION_MODE  Pmode

#define ARM_FRAME_RTX(X)					\
  (   (X) == frame_pointer_rtx || (X) == stack_pointer_rtx	\
   || (X) == arg_pointer_rtx)

/* Moves to and from memory are quite expensive */
#define MEMORY_MOVE_COST(M, CLASS, IN)			\
  (TARGET_32BIT ? 10 :					\
   ((GET_MODE_SIZE (M) < 4 ? 8 : 2 * GET_MODE_SIZE (M))	\
    * (CLASS == LO_REGS ? 1 : 2)))

/* Try to generate sequences that don't involve branches, we can then use
   conditional instructions */
#define BRANCH_COST(speed_p, predictable_p) \
  (current_tune->branch_cost (speed_p, predictable_p))


/* Position Independent Code.  */
/* We decide which register to use based on the compilation options and
   the assembler in use; this is more general than the APCS restriction of
   using sb (r9) all the time.  */
extern unsigned arm_pic_register;

/* The register number of the register used to address a table of static
   data addresses in memory.  */
#define PIC_OFFSET_TABLE_REGNUM arm_pic_register

/* We can't directly access anything that contains a symbol,
   nor can we indirect via the constant pool.  One exception is
   UNSPEC_TLS, which is always PIC.  */
#define LEGITIMATE_PIC_OPERAND_P(X)					\
	(!(symbol_mentioned_p (X)					\
	   || label_mentioned_p (X)					\
	   || (GET_CODE (X) == SYMBOL_REF				\
	       && CONSTANT_POOL_ADDRESS_P (X)				\
	       && (symbol_mentioned_p (get_pool_constant (X))		\
		   || label_mentioned_p (get_pool_constant (X)))))	\
	 || tls_mentioned_p (X))

/* We need to know when we are making a constant pool; this determines
   whether data needs to be in the GOT or can be referenced via a GOT
   offset.  */
extern int making_const_table;

/* Handle pragmas for compatibility with Intel's compilers.  */
/* Also abuse this to register additional C specific EABI attributes.  */
#define REGISTER_TARGET_PRAGMAS() do {					\
  c_register_pragma (0, "long_calls", arm_pr_long_calls);		\
  c_register_pragma (0, "no_long_calls", arm_pr_no_long_calls);		\
  c_register_pragma (0, "long_calls_off", arm_pr_long_calls_off);	\
  arm_lang_object_attributes_init(); \
} while (0)

/* Condition code information.  */
/* Given a comparison code (EQ, NE, etc.) and the first operand of a COMPARE,
   return the mode to be used for the comparison.  */

#define SELECT_CC_MODE(OP, X, Y)  arm_select_cc_mode (OP, X, Y)

#define REVERSIBLE_CC_MODE(MODE) 1

#define REVERSE_CONDITION(CODE,MODE) \
  (((MODE) == CCFPmode || (MODE) == CCFPEmode) \
   ? reverse_condition_maybe_unordered (code) \
   : reverse_condition (code))

#define CANONICALIZE_COMPARISON(CODE, OP0, OP1)				\
  (CODE) = arm_canonicalize_comparison (CODE, &(OP0), &(OP1))

/* The arm5 clz instruction returns 32.  */
#define CLZ_DEFINED_VALUE_AT_ZERO(MODE, VALUE)  ((VALUE) = 32, 1)
#define CTZ_DEFINED_VALUE_AT_ZERO(MODE, VALUE)  ((VALUE) = 32, 1)

#undef  ASM_APP_OFF
#define ASM_APP_OFF (TARGET_THUMB1 ? "\t.code\t16\n" : \
		     TARGET_THUMB2 ? "\t.thumb\n" : "")

/* Output a push or a pop instruction (only used when profiling).
   We can't push STATIC_CHAIN_REGNUM (r12) directly with Thumb-1.  We know
   that ASM_OUTPUT_REG_PUSH will be matched with ASM_OUTPUT_REG_POP, and
   that r7 isn't used by the function profiler, so we can use it as a
   scratch reg.  WARNING: This isn't safe in the general case!  It may be
   sensitive to future changes in final.c:profile_function.  */
#define ASM_OUTPUT_REG_PUSH(STREAM, REGNO)		\
  do							\
    {							\
      if (TARGET_ARM)					\
	asm_fprintf (STREAM,"\tstmfd\t%r!,{%r}\n",	\
		     STACK_POINTER_REGNUM, REGNO);	\
      else if (TARGET_THUMB1				\
	       && (REGNO) == STATIC_CHAIN_REGNUM)	\
	{						\
	  asm_fprintf (STREAM, "\tpush\t{r7}\n");	\
	  asm_fprintf (STREAM, "\tmov\tr7, %r\n", REGNO);\
	  asm_fprintf (STREAM, "\tpush\t{r7}\n");	\
	}						\
      else						\
	asm_fprintf (STREAM, "\tpush {%r}\n", REGNO);	\
    } while (0)


/* See comment for ASM_OUTPUT_REG_PUSH concerning Thumb-1 issue.  */
#define ASM_OUTPUT_REG_POP(STREAM, REGNO)		\
  do							\
    {							\
      if (TARGET_ARM)					\
	asm_fprintf (STREAM, "\tldmfd\t%r!,{%r}\n",	\
		     STACK_POINTER_REGNUM, REGNO);	\
      else if (TARGET_THUMB1				\
	       && (REGNO) == STATIC_CHAIN_REGNUM)	\
	{						\
	  asm_fprintf (STREAM, "\tpop\t{r7}\n");	\
	  asm_fprintf (STREAM, "\tmov\t%r, r7\n", REGNO);\
	  asm_fprintf (STREAM, "\tpop\t{r7}\n");	\
	}						\
      else						\
	asm_fprintf (STREAM, "\tpop {%r}\n", REGNO);	\
    } while (0)

/* Jump table alignment is explicit in ASM_OUTPUT_CASE_LABEL.  */
#define ADDR_VEC_ALIGN(JUMPTABLE) 0

/* This is how to output a label which precedes a jumptable.  Since
   Thumb instructions are 2 bytes, we may need explicit alignment here.  */
#undef  ASM_OUTPUT_CASE_LABEL
#define ASM_OUTPUT_CASE_LABEL(FILE, PREFIX, NUM, JUMPTABLE)		\
  do									\
    {									\
      if (TARGET_THUMB && GET_MODE (PATTERN (JUMPTABLE)) == SImode)	\
        ASM_OUTPUT_ALIGN (FILE, 2);					\
      (*targetm.asm_out.internal_label) (FILE, PREFIX, NUM);		\
    }									\
  while (0)

/* Make sure subsequent insns are aligned after a TBB.  */
#define ASM_OUTPUT_CASE_END(FILE, NUM, JUMPTABLE)	\
  do							\
    {							\
      if (GET_MODE (PATTERN (JUMPTABLE)) == QImode)	\
	ASM_OUTPUT_ALIGN (FILE, 1);			\
    }							\
  while (0)

#define ARM_DECLARE_FUNCTION_NAME(STREAM, NAME, DECL) 	\
  do							\
    {							\
      if (TARGET_THUMB) 				\
        {						\
          if (is_called_in_ARM_mode (DECL)		\
	      || (TARGET_THUMB1 && !TARGET_THUMB1_ONLY	\
		  && cfun->is_thunk))	\
            fprintf (STREAM, "\t.code 32\n") ;		\
          else if (TARGET_THUMB1)			\
           fprintf (STREAM, "\t.code\t16\n\t.thumb_func\n") ;	\
          else						\
           fprintf (STREAM, "\t.thumb\n\t.thumb_func\n") ;	\
        }						\
      if (TARGET_POKE_FUNCTION_NAME)			\
        arm_poke_function_name (STREAM, (const char *) NAME);	\
    }							\
  while (0)

/* For aliases of functions we use .thumb_set instead.  */
#define ASM_OUTPUT_DEF_FROM_DECLS(FILE, DECL1, DECL2)		\
  do						   		\
    {								\
      const char *const LABEL1 = XSTR (XEXP (DECL_RTL (decl), 0), 0); \
      const char *const LABEL2 = IDENTIFIER_POINTER (DECL2);	\
								\
      if (TARGET_THUMB && TREE_CODE (DECL1) == FUNCTION_DECL)	\
	{							\
	  fprintf (FILE, "\t.thumb_set ");			\
	  assemble_name (FILE, LABEL1);			   	\
	  fprintf (FILE, ",");			   		\
	  assemble_name (FILE, LABEL2);		   		\
	  fprintf (FILE, "\n");					\
	}							\
      else							\
	ASM_OUTPUT_DEF (FILE, LABEL1, LABEL2);			\
    }								\
  while (0)

#ifdef HAVE_GAS_MAX_SKIP_P2ALIGN
/* To support -falign-* switches we need to use .p2align so
   that alignment directives in code sections will be padded
   with no-op instructions, rather than zeroes.  */
#define ASM_OUTPUT_MAX_SKIP_ALIGN(FILE, LOG, MAX_SKIP)		\
  if ((LOG) != 0)						\
    {								\
      if ((MAX_SKIP) == 0)					\
        fprintf ((FILE), "\t.p2align %d\n", (int) (LOG));	\
      else							\
        fprintf ((FILE), "\t.p2align %d,,%d\n",			\
                 (int) (LOG), (int) (MAX_SKIP));		\
    }
#endif

/* Add two bytes to the length of conditionally executed Thumb-2
   instructions for the IT instruction.  */
#define ADJUST_INSN_LENGTH(insn, length) \
  if (TARGET_THUMB2 && GET_CODE (PATTERN (insn)) == COND_EXEC) \
    length += 2;

/* Only perform branch elimination (by making instructions conditional) if
   we're optimizing.  For Thumb-2 check if any IT instructions need
   outputting.  */
#define FINAL_PRESCAN_INSN(INSN, OPVEC, NOPERANDS)	\
  if (TARGET_ARM && optimize)				\
    arm_final_prescan_insn (INSN);			\
  else if (TARGET_THUMB2)				\
    thumb2_final_prescan_insn (INSN);			\
  else if (TARGET_THUMB1)				\
    thumb1_final_prescan_insn (INSN)

#define PRINT_OPERAND_PUNCT_VALID_P(CODE)	\
  (CODE == '@' || CODE == '|' || CODE == '.'	\
   || CODE == '(' || CODE == ')' || CODE == '#'	\
   || (TARGET_32BIT && (CODE == '?'))		\
   || (TARGET_THUMB2 && (CODE == '!'))		\
   || (TARGET_THUMB && (CODE == '_')))

/* Output an operand of an instruction.  */
#define PRINT_OPERAND(STREAM, X, CODE)  \
  arm_print_operand (STREAM, X, CODE)

#define ARM_SIGN_EXTEND(x)  ((HOST_WIDE_INT)			\
  (HOST_BITS_PER_WIDE_INT <= 32 ? (unsigned HOST_WIDE_INT) (x)	\
   : ((((unsigned HOST_WIDE_INT)(x)) & (unsigned HOST_WIDE_INT) 0xffffffff) |\
      ((((unsigned HOST_WIDE_INT)(x)) & (unsigned HOST_WIDE_INT) 0x80000000) \
       ? ((~ (unsigned HOST_WIDE_INT) 0)			\
	  & ~ (unsigned HOST_WIDE_INT) 0xffffffff)		\
       : 0))))

/* Output the address of an operand.  */
#define ARM_PRINT_OPERAND_ADDRESS(STREAM, X)				\
{									\
    int is_minus = GET_CODE (X) == MINUS;				\
									\
    if (GET_CODE (X) == REG)						\
      asm_fprintf (STREAM, "[%r, #0]", REGNO (X));			\
    else if (GET_CODE (X) == PLUS || is_minus)				\
      {									\
	rtx base = XEXP (X, 0);						\
	rtx index = XEXP (X, 1);					\
	HOST_WIDE_INT offset = 0;					\
	if (GET_CODE (base) != REG					\
	    || (GET_CODE (index) == REG && REGNO (index) == SP_REGNUM))	\
	  {								\
	    /* Ensure that BASE is a register.  */			\
            /* (one of them must be).  */				\
	    /* Also ensure the SP is not used as in index register.  */ \
	    rtx temp = base;						\
	    base = index;						\
	    index = temp;						\
	  }								\
	switch (GET_CODE (index))					\
	  {								\
	  case CONST_INT:						\
	    offset = INTVAL (index);					\
	    if (is_minus)						\
	      offset = -offset;						\
	    asm_fprintf (STREAM, "[%r, #%wd]",				\
		         REGNO (base), offset);				\
	    break;							\
									\
	  case REG:							\
	    asm_fprintf (STREAM, "[%r, %s%r]",				\
		     REGNO (base), is_minus ? "-" : "",			\
		     REGNO (index));					\
	    break;							\
									\
	  case MULT:							\
	  case ASHIFTRT:						\
	  case LSHIFTRT:						\
	  case ASHIFT:							\
	  case ROTATERT:						\
	  {								\
	    asm_fprintf (STREAM, "[%r, %s%r",				\
		         REGNO (base), is_minus ? "-" : "",		\
                         REGNO (XEXP (index, 0)));			\
	    arm_print_operand (STREAM, index, 'S');			\
	    fputs ("]", STREAM);					\
	    break;							\
	  }								\
									\
	  default:							\
	    gcc_unreachable ();						\
	}								\
    }									\
  else if (GET_CODE (X) == PRE_INC || GET_CODE (X) == POST_INC		\
	   || GET_CODE (X) == PRE_DEC || GET_CODE (X) == POST_DEC)	\
    {									\
      extern enum machine_mode output_memory_reference_mode;		\
									\
      gcc_assert (GET_CODE (XEXP (X, 0)) == REG);			\
									\
      if (GET_CODE (X) == PRE_DEC || GET_CODE (X) == PRE_INC)		\
	asm_fprintf (STREAM, "[%r, #%s%d]!",				\
		     REGNO (XEXP (X, 0)),				\
		     GET_CODE (X) == PRE_DEC ? "-" : "",		\
		     GET_MODE_SIZE (output_memory_reference_mode));	\
      else								\
	asm_fprintf (STREAM, "[%r], #%s%d",				\
		     REGNO (XEXP (X, 0)),				\
		     GET_CODE (X) == POST_DEC ? "-" : "",		\
		     GET_MODE_SIZE (output_memory_reference_mode));	\
    }									\
  else if (GET_CODE (X) == PRE_MODIFY)					\
    {									\
      asm_fprintf (STREAM, "[%r, ", REGNO (XEXP (X, 0)));		\
      if (GET_CODE (XEXP (XEXP (X, 1), 1)) == CONST_INT)		\
	asm_fprintf (STREAM, "#%wd]!", 					\
		     INTVAL (XEXP (XEXP (X, 1), 1)));			\
      else								\
	asm_fprintf (STREAM, "%r]!", 					\
		     REGNO (XEXP (XEXP (X, 1), 1)));			\
    }									\
  else if (GET_CODE (X) == POST_MODIFY)					\
    {									\
      asm_fprintf (STREAM, "[%r], ", REGNO (XEXP (X, 0)));		\
      if (GET_CODE (XEXP (XEXP (X, 1), 1)) == CONST_INT)		\
	asm_fprintf (STREAM, "#%wd", 					\
		     INTVAL (XEXP (XEXP (X, 1), 1)));			\
      else								\
	asm_fprintf (STREAM, "%r", 					\
		     REGNO (XEXP (XEXP (X, 1), 1)));			\
    }									\
  else output_addr_const (STREAM, X);					\
}

#define THUMB_PRINT_OPERAND_ADDRESS(STREAM, X)		\
{							\
  if (GET_CODE (X) == REG)				\
    asm_fprintf (STREAM, "[%r]", REGNO (X));		\
  else if (GET_CODE (X) == POST_INC)			\
    asm_fprintf (STREAM, "%r!", REGNO (XEXP (X, 0)));	\
  else if (GET_CODE (X) == PLUS)			\
    {							\
      gcc_assert (GET_CODE (XEXP (X, 0)) == REG);	\
      if (GET_CODE (XEXP (X, 1)) == CONST_INT)		\
	asm_fprintf (STREAM, "[%r, #%wd]", 		\
		     REGNO (XEXP (X, 0)),		\
		     INTVAL (XEXP (X, 1)));		\
      else						\
	asm_fprintf (STREAM, "[%r, %r]",		\
		     REGNO (XEXP (X, 0)),		\
		     REGNO (XEXP (X, 1)));		\
    }							\
  else							\
    output_addr_const (STREAM, X);			\
}

#define PRINT_OPERAND_ADDRESS(STREAM, X)	\
  if (TARGET_32BIT)				\
    ARM_PRINT_OPERAND_ADDRESS (STREAM, X)	\
  else						\
    THUMB_PRINT_OPERAND_ADDRESS (STREAM, X)

#define OUTPUT_ADDR_CONST_EXTRA(file, x, fail)		\
  if (arm_output_addr_const_extra (file, x) == FALSE)	\
    goto fail

/* A C expression whose value is RTL representing the value of the return
   address for the frame COUNT steps up from the current frame.  */

#define RETURN_ADDR_RTX(COUNT, FRAME) \
  arm_return_addr (COUNT, FRAME)

#define RETURN_ADDR_REGNUM LR_REGNUM

/* Mask of the bits in the PC that contain the real return address
   when running in 26-bit mode.  */
#define RETURN_ADDR_MASK26 (0x03fffffc)

/* Pick up the return address upon entry to a procedure. Used for
   dwarf2 unwind information.  This also enables the table driven
   mechanism.  */
#define INCOMING_RETURN_ADDR_RTX	gen_rtx_REG (Pmode, LR_REGNUM)
#define DWARF_FRAME_RETURN_COLUMN	DWARF_FRAME_REGNUM (LR_REGNUM)

/* Used to mask out junk bits from the return address, such as
   processor state, interrupt status, condition codes and the like.  */
#define MASK_RETURN_ADDR \
  /* If we are generating code for an ARM2/ARM3 machine or for an ARM6	\
     in 26 bit mode, the condition codes must be masked out of the	\
     return address.  This does not apply to ARM6 and later processors	\
     when running in 32 bit mode.  */					\
  ((arm_arch4 || TARGET_THUMB)						\
   ? (gen_int_mode ((unsigned long)0xffffffff, Pmode))			\
   : arm_gen_return_addr_mask ())


/* Neon defines builtins from ARM_BUILTIN_MAX upwards, though they don't have
   symbolic names defined here (which would require too much duplication).
   FIXME?  */
enum arm_builtins
{
  ARM_BUILTIN_GETWCX,
  ARM_BUILTIN_SETWCX,

  ARM_BUILTIN_WZERO,

  ARM_BUILTIN_WAVG2BR,
  ARM_BUILTIN_WAVG2HR,
  ARM_BUILTIN_WAVG2B,
  ARM_BUILTIN_WAVG2H,

  ARM_BUILTIN_WACCB,
  ARM_BUILTIN_WACCH,
  ARM_BUILTIN_WACCW,

  ARM_BUILTIN_WMACS,
  ARM_BUILTIN_WMACSZ,
  ARM_BUILTIN_WMACU,
  ARM_BUILTIN_WMACUZ,

  ARM_BUILTIN_WSADB,
  ARM_BUILTIN_WSADBZ,
  ARM_BUILTIN_WSADH,
  ARM_BUILTIN_WSADHZ,

  ARM_BUILTIN_WALIGN,

  ARM_BUILTIN_TMIA,
  ARM_BUILTIN_TMIAPH,
  ARM_BUILTIN_TMIABB,
  ARM_BUILTIN_TMIABT,
  ARM_BUILTIN_TMIATB,
  ARM_BUILTIN_TMIATT,

  ARM_BUILTIN_TMOVMSKB,
  ARM_BUILTIN_TMOVMSKH,
  ARM_BUILTIN_TMOVMSKW,

  ARM_BUILTIN_TBCSTB,
  ARM_BUILTIN_TBCSTH,
  ARM_BUILTIN_TBCSTW,

  ARM_BUILTIN_WMADDS,
  ARM_BUILTIN_WMADDU,

  ARM_BUILTIN_WPACKHSS,
  ARM_BUILTIN_WPACKWSS,
  ARM_BUILTIN_WPACKDSS,
  ARM_BUILTIN_WPACKHUS,
  ARM_BUILTIN_WPACKWUS,
  ARM_BUILTIN_WPACKDUS,

  ARM_BUILTIN_WADDB,
  ARM_BUILTIN_WADDH,
  ARM_BUILTIN_WADDW,
  ARM_BUILTIN_WADDSSB,
  ARM_BUILTIN_WADDSSH,
  ARM_BUILTIN_WADDSSW,
  ARM_BUILTIN_WADDUSB,
  ARM_BUILTIN_WADDUSH,
  ARM_BUILTIN_WADDUSW,
  ARM_BUILTIN_WSUBB,
  ARM_BUILTIN_WSUBH,
  ARM_BUILTIN_WSUBW,
  ARM_BUILTIN_WSUBSSB,
  ARM_BUILTIN_WSUBSSH,
  ARM_BUILTIN_WSUBSSW,
  ARM_BUILTIN_WSUBUSB,
  ARM_BUILTIN_WSUBUSH,
  ARM_BUILTIN_WSUBUSW,

  ARM_BUILTIN_WAND,
  ARM_BUILTIN_WANDN,
  ARM_BUILTIN_WOR,
  ARM_BUILTIN_WXOR,

  ARM_BUILTIN_WCMPEQB,
  ARM_BUILTIN_WCMPEQH,
  ARM_BUILTIN_WCMPEQW,
  ARM_BUILTIN_WCMPGTUB,
  ARM_BUILTIN_WCMPGTUH,
  ARM_BUILTIN_WCMPGTUW,
  ARM_BUILTIN_WCMPGTSB,
  ARM_BUILTIN_WCMPGTSH,
  ARM_BUILTIN_WCMPGTSW,

  ARM_BUILTIN_TEXTRMSB,
  ARM_BUILTIN_TEXTRMSH,
  ARM_BUILTIN_TEXTRMSW,
  ARM_BUILTIN_TEXTRMUB,
  ARM_BUILTIN_TEXTRMUH,
  ARM_BUILTIN_TEXTRMUW,
  ARM_BUILTIN_TINSRB,
  ARM_BUILTIN_TINSRH,
  ARM_BUILTIN_TINSRW,

  ARM_BUILTIN_WMAXSW,
  ARM_BUILTIN_WMAXSH,
  ARM_BUILTIN_WMAXSB,
  ARM_BUILTIN_WMAXUW,
  ARM_BUILTIN_WMAXUH,
  ARM_BUILTIN_WMAXUB,
  ARM_BUILTIN_WMINSW,
  ARM_BUILTIN_WMINSH,
  ARM_BUILTIN_WMINSB,
  ARM_BUILTIN_WMINUW,
  ARM_BUILTIN_WMINUH,
  ARM_BUILTIN_WMINUB,

  ARM_BUILTIN_WMULUM,
  ARM_BUILTIN_WMULSM,
  ARM_BUILTIN_WMULUL,

  ARM_BUILTIN_PSADBH,
  ARM_BUILTIN_WSHUFH,

  ARM_BUILTIN_WSLLH,
  ARM_BUILTIN_WSLLW,
  ARM_BUILTIN_WSLLD,
  ARM_BUILTIN_WSRAH,
  ARM_BUILTIN_WSRAW,
  ARM_BUILTIN_WSRAD,
  ARM_BUILTIN_WSRLH,
  ARM_BUILTIN_WSRLW,
  ARM_BUILTIN_WSRLD,
  ARM_BUILTIN_WRORH,
  ARM_BUILTIN_WRORW,
  ARM_BUILTIN_WRORD,
  ARM_BUILTIN_WSLLHI,
  ARM_BUILTIN_WSLLWI,
  ARM_BUILTIN_WSLLDI,
  ARM_BUILTIN_WSRAHI,
  ARM_BUILTIN_WSRAWI,
  ARM_BUILTIN_WSRADI,
  ARM_BUILTIN_WSRLHI,
  ARM_BUILTIN_WSRLWI,
  ARM_BUILTIN_WSRLDI,
  ARM_BUILTIN_WRORHI,
  ARM_BUILTIN_WRORWI,
  ARM_BUILTIN_WRORDI,

  ARM_BUILTIN_WUNPCKIHB,
  ARM_BUILTIN_WUNPCKIHH,
  ARM_BUILTIN_WUNPCKIHW,
  ARM_BUILTIN_WUNPCKILB,
  ARM_BUILTIN_WUNPCKILH,
  ARM_BUILTIN_WUNPCKILW,

  ARM_BUILTIN_WUNPCKEHSB,
  ARM_BUILTIN_WUNPCKEHSH,
  ARM_BUILTIN_WUNPCKEHSW,
  ARM_BUILTIN_WUNPCKEHUB,
  ARM_BUILTIN_WUNPCKEHUH,
  ARM_BUILTIN_WUNPCKEHUW,
  ARM_BUILTIN_WUNPCKELSB,
  ARM_BUILTIN_WUNPCKELSH,
  ARM_BUILTIN_WUNPCKELSW,
  ARM_BUILTIN_WUNPCKELUB,
  ARM_BUILTIN_WUNPCKELUH,
  ARM_BUILTIN_WUNPCKELUW,

  ARM_BUILTIN_THREAD_POINTER,

  ARM_BUILTIN_NEON_BASE,

  ARM_BUILTIN_MAX = ARM_BUILTIN_NEON_BASE  /* FIXME: Wrong!  */
};

/* Do not emit .note.GNU-stack by default.  */
#ifndef NEED_INDICATE_EXEC_STACK
#define NEED_INDICATE_EXEC_STACK	0
#endif

#endif /* ! GCC_ARM_H */
