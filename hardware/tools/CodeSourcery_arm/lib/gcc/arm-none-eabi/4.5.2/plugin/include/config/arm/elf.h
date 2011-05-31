/* Definitions of target machine for GNU compiler.
   For ARM with ELF obj format.
   Copyright (C) 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2004, 2005, 2007,
   2008 Free Software Foundation, Inc.
   Contributed by Philip Blundell <philb@gnu.org> and
   Catherine Moore <clm@cygnus.com>
   
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

#ifndef OBJECT_FORMAT_ELF
 #error elf.h included before elfos.h
#endif

#ifndef LOCAL_LABEL_PREFIX
#define LOCAL_LABEL_PREFIX "."
#endif

#ifndef SUBTARGET_CPP_SPEC
#define SUBTARGET_CPP_SPEC  "-D__ELF__"
#endif

#ifndef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS \
  { "subtarget_extra_asm_spec",	SUBTARGET_EXTRA_ASM_SPEC }, \
  { "subtarget_asm_float_spec", SUBTARGET_ASM_FLOAT_SPEC }, \
  SUBSUBTARGET_EXTRA_SPECS
#endif

#ifndef SUBTARGET_EXTRA_ASM_SPEC
#define SUBTARGET_EXTRA_ASM_SPEC ""
#endif

#ifndef SUBTARGET_ASM_FLOAT_SPEC
#define SUBTARGET_ASM_FLOAT_SPEC "\
%{mapcs-float:-mfloat}"
#endif

#undef SUBSUBTARGET_EXTRA_SPECS
#define SUBSUBTARGET_EXTRA_SPECS

#ifndef ASM_SPEC
#define ASM_SPEC "\
%{mbig-endian:-EB} \
%{mlittle-endian:-EL} \
%{mcpu=*:-mcpu=%*} \
%{march=*:-march=%*} \
%{mapcs-*:-mapcs-%*} \
%(subtarget_asm_float_spec) \
%{mthumb-interwork:-mthumb-interwork} \
%{msoft-float:-mfloat-abi=soft} %{mhard-float:-mfloat-abi=hard} \
%{mfloat-abi=*} %{mfpu=*} \
%(subtarget_extra_asm_spec)"
#endif

/* The ARM uses @ are a comment character so we need to redefine
   TYPE_OPERAND_FMT.  */
#undef  TYPE_OPERAND_FMT
#define TYPE_OPERAND_FMT	"%%%s"

/* We might need a ARM specific header to function declarations.  */
#undef  ASM_DECLARE_FUNCTION_NAME
#define ASM_DECLARE_FUNCTION_NAME(FILE, NAME, DECL)		\
  do								\
    {								\
      ARM_DECLARE_FUNCTION_NAME (FILE, NAME, DECL);		\
      ASM_OUTPUT_TYPE_DIRECTIVE (FILE, NAME, "function");	\
      ASM_DECLARE_RESULT (FILE, DECL_RESULT (DECL));		\
      ASM_OUTPUT_LABEL(FILE, NAME);				\
      ARM_OUTPUT_FN_UNWIND (FILE, TRUE);			\
    }								\
  while (0)

/* We might need an ARM specific trailer for function declarations.  */
#undef  ASM_DECLARE_FUNCTION_SIZE
#define ASM_DECLARE_FUNCTION_SIZE(FILE, FNAME, DECL)		\
  do								\
    {								\
      ARM_OUTPUT_FN_UNWIND (FILE, FALSE);			\
      if (!flag_inhibit_size_directive)				\
	ASM_OUTPUT_MEASURED_SIZE (FILE, FNAME);			\
    }								\
  while (0)

/* Define this macro if jump tables (for `tablejump' insns) should be
   output in the text section, along with the assembler instructions.
   Otherwise, the readonly data section is used.  */
/* We put ARM and Thumb-2 jump tables in the text section, because it makes
   the code more efficient, but for Thumb-1 it's better to put them out of
   band unless we are generating compressed tables.  */
#define JUMP_TABLES_IN_TEXT_SECTION					\
   (TARGET_32BIT || (TARGET_THUMB && (optimize_size || flag_pic)))

#ifndef LINK_SPEC
#define LINK_SPEC "%{mbig-endian:-EB} %{mlittle-endian:-EL} -X"
#endif
  
/* Run-time Target Specification.  */
#ifndef TARGET_VERSION
#define TARGET_VERSION fputs (" (ARM/elf)", stderr)
#endif

#ifndef TARGET_DEFAULT
#define TARGET_DEFAULT (MASK_APCS_FRAME)
#endif

#ifndef MULTILIB_DEFAULTS
#define MULTILIB_DEFAULTS \
  { "marm", "mlittle-endian", "msoft-float", "mno-thumb-interwork", "fno-leading-underscore" }
#endif

#define TARGET_ASM_FILE_START_APP_OFF true
#define TARGET_ASM_FILE_START_FILE_DIRECTIVE true


/* Output an element in the static constructor array.  */
#undef TARGET_ASM_CONSTRUCTOR
#define TARGET_ASM_CONSTRUCTOR arm_elf_asm_constructor

#undef TARGET_ASM_DESTRUCTOR
#define TARGET_ASM_DESTRUCTOR arm_elf_asm_destructor

/* For PIC code we need to explicitly specify (PLT) and (GOT) relocs.  */
#define NEED_PLT_RELOC	flag_pic
#define NEED_GOT_RELOC	flag_pic

/* The ELF assembler handles GOT addressing differently to NetBSD.  */
#define GOT_PCREL	0

/* Align output to a power of two.  Note ".align 0" is redundant,
   and also GAS will treat it as ".align 2" which we do not want.  */
#define ASM_OUTPUT_ALIGN(STREAM, POWER)			\
  do							\
    {							\
      if ((POWER) > 0)					\
	fprintf (STREAM, "\t.align\t%d\n", POWER);	\
    }							\
  while (0)

/* Horrible hack: We want to prevent some libgcc routines being included
   for some multilibs.  */
#if !(defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_6SM__))
#undef L_fixdfsi
#undef L_fixunsdfsi
#undef L_truncdfsf2
#undef L_fixsfsi
#undef L_fixunssfsi
#undef L_floatdidf
#undef L_floatdisf
#undef L_floatundidf
#undef L_floatundisf
#endif

