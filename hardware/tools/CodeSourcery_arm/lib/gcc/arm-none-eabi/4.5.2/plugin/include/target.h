/* Data structure definitions for a generic GCC target.
   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.

   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!  */


/* This file contains a data structure that describes a GCC target.
   At present it is incomplete, but in future it should grow to
   contain most or all target machine and target O/S specific
   information.

   This structure has its initializer declared in target-def.h in the
   form of large macro TARGET_INITIALIZER that expands to many smaller
   macros.

   The smaller macros each initialize one component of the structure,
   and each has a default.  Each target should have a file that
   includes target.h and target-def.h, and overrides any inappropriate
   defaults by undefining the relevant macro and defining a suitable
   replacement.  That file should then contain the definition of
   "targetm" like so:

   struct gcc_target targetm = TARGET_INITIALIZER;

   Doing things this way allows us to bring together everything that
   defines a GCC target.  By supplying a default that is appropriate
   to most targets, we can easily add new items without needing to
   edit dozens of target configuration files.  It should also allow us
   to gradually reduce the amount of conditional compilation that is
   scattered throughout GCC.  */

#ifndef GCC_TARGET_H
#define GCC_TARGET_H

#include "tm.h"
#include "insn-modes.h"

/* Types used by the record_gcc_switches() target function.  */
typedef enum
{
  SWITCH_TYPE_PASSED,		/* A switch passed on the command line.  */
  SWITCH_TYPE_ENABLED,		/* An option that is currently enabled.  */
  SWITCH_TYPE_DESCRIPTIVE,	/* Descriptive text, not a switch or option.  */
  SWITCH_TYPE_LINE_START,	/* Please emit any necessary text at the start of a line.  */
  SWITCH_TYPE_LINE_END		/* Please emit a line terminator.  */
}
print_switch_type;

typedef int (* print_switch_fn_type) (print_switch_type, const char *);

/* An example implementation for ELF targets.  Defined in varasm.c  */
extern int elf_record_gcc_switches (print_switch_type type, const char *);

/* Some places still assume that all pointer or address modes are the
   standard Pmode and ptr_mode.  These optimizations become invalid if
   the target actually supports multiple different modes.  For now,
   we disable such optimizations on such targets, using this function.  */
extern bool target_default_pointer_address_modes_p (void);

struct stdarg_info;
struct spec_info_def;

/* The struct used by the secondary_reload target hook.  */
typedef struct secondary_reload_info
{
  /* icode is actually an enum insn_code, but we don't want to force every
     file that includes target.h to include optabs.h .  */
  int icode;
  int extra_cost; /* Cost for using (a) scratch register(s) to be taken
		     into account by copy_cost.  */
  /* The next two members are for the use of the backward
     compatibility hook.  */
  struct secondary_reload_info *prev_sri;
  int t_icode; /* Actually an enum insn_code - see above.  */
} secondary_reload_info;

/* This is defined in sched-int.h .  */
struct _dep;

/* This is defined in ddg.h .  */
struct ddg;

/* Assembler instructions for creating various kinds of integer object.  */

struct asm_int_op
{
  const char *hi;
  const char *si;
  const char *di;
  const char *ti;
};

/* The target structure.  This holds all the backend hooks.  */

struct gcc_target
{
  /* Functions that output assembler for the target.  */
  struct asm_out
  {
    /* Opening and closing parentheses for asm expression grouping.  */
    const char *open_paren, *close_paren;

    /* Assembler instructions for creating various kinds of integer object.  */
    const char *byte_op;
    struct asm_int_op aligned_op, unaligned_op;

    /* Try to output the assembler code for an integer object whose
       value is given by X.  SIZE is the size of the object in bytes and
       ALIGNED_P indicates whether it is aligned.  Return true if
       successful.  Only handles cases for which BYTE_OP, ALIGNED_OP
       and UNALIGNED_OP are NULL.  */
    bool (* integer) (rtx x, unsigned int size, int aligned_p);

    /* Output code that will globalize a label.  */
    void (* globalize_label) (FILE *, const char *);

    /* Output code that will globalize a declaration.  */
    void (* globalize_decl_name) (FILE *, tree);

    /* Output code that will emit a label for unwind info, if this
       target requires such labels.  Second argument is the decl the
       unwind info is associated with, third is a boolean: true if
       this is for exception handling, fourth is a boolean: true if
       this is only a placeholder for an omitted FDE.  */
    void (* unwind_label) (FILE *, tree, int, int);

    /* Output code that will emit a label to divide up the exception
       table.  */
    void (* except_table_label) (FILE *);

    /* Emit any directives required to unwind this instruction.  */
    void (* unwind_emit) (FILE *, rtx);

    /* Output an internal label.  */
    void (* internal_label) (FILE *, const char *, unsigned long);

    /* Emit a ttype table reference to a typeinfo object.  */
    bool (* ttype) (rtx);

    /* Emit an assembler directive to set visibility for the symbol
       associated with the tree decl.  */
    void (* visibility) (tree, int);

    /* Output the assembler code for entry to a function.  */
    void (* function_prologue) (FILE *, HOST_WIDE_INT);

    /* Output the assembler code for end of prologue.  */
    void (* function_end_prologue) (FILE *);

    /* Output the assembler code for start of epilogue.  */
    void (* function_begin_epilogue) (FILE *);

    /* Output the assembler code for function exit.  */
    void (* function_epilogue) (FILE *, HOST_WIDE_INT);

    /* Initialize target-specific sections.  */
    void (* init_sections) (void);

    /* Tell assembler to change to section NAME with attributes FLAGS.
       If DECL is non-NULL, it is the VAR_DECL or FUNCTION_DECL with
       which this section is associated.  */
    void (* named_section) (const char *name, unsigned int flags, tree decl);

    /* Return a mask describing how relocations should be treated when
       selecting sections.  Bit 1 should be set if global relocations
       should be placed in a read-write section; bit 0 should be set if
       local relocations should be placed in a read-write section.  */
    int (*reloc_rw_mask) (void);

    /* Return a section for EXP.  It may be a DECL or a constant.  RELOC
       is nonzero if runtime relocations must be applied; bit 1 will be
       set if the runtime relocations require non-local name resolution.
       ALIGN is the required alignment of the data.  */
    section *(* select_section) (tree, int, unsigned HOST_WIDE_INT);

    /* Return a section for X.  MODE is X's mode and ALIGN is its
       alignment in bits.  */
    section *(* select_rtx_section) (enum machine_mode, rtx,
				     unsigned HOST_WIDE_INT);

    /* Select a unique section name for DECL.  RELOC is the same as
       for SELECT_SECTION.  */
    void (* unique_section) (tree, int);

    /* Return the readonly data section associated with function DECL.  */
    section *(* function_rodata_section) (tree);

    /* Output a constructor for a symbol with a given priority.  */
    void (* constructor) (rtx, int);

    /* Output a destructor for a symbol with a given priority.  */
    void (* destructor) (rtx, int);

    /* Output the assembler code for a thunk function.  THUNK_DECL is the
       declaration for the thunk function itself, FUNCTION is the decl for
       the target function.  DELTA is an immediate constant offset to be
       added to THIS.  If VCALL_OFFSET is nonzero, the word at
       *(*this + vcall_offset) should be added to THIS.  */
    void (* output_mi_thunk) (FILE *file, tree thunk_decl,
			      HOST_WIDE_INT delta, HOST_WIDE_INT vcall_offset,
			      tree function_decl);

    /* Determine whether output_mi_thunk would succeed.  */
    /* ??? Ideally, this hook would not exist, and success or failure
       would be returned from output_mi_thunk directly.  But there's
       too much undo-able setup involved in invoking output_mi_thunk.
       Could be fixed by making output_mi_thunk emit rtl instead of
       text to the output file.  */
    bool (* can_output_mi_thunk) (const_tree thunk_decl, HOST_WIDE_INT delta,
				  HOST_WIDE_INT vcall_offset,
				  const_tree function_decl);

    /* Output any boilerplate text needed at the beginning of a
       translation unit.  */
    void (*file_start) (void);

    /* Output any boilerplate text needed at the end of a
       translation unit.  */
    void (*file_end) (void);

    /* Output any boilerplate text needed at the beginning of an
       LTO output stream.  */
    void (*lto_start) (void);

    /* Output any boilerplate text needed at the end of an
       LTO output stream.  */
    void (*lto_end) (void);

    /* Output any boilerplace text needed at the end of a
       translation unit before debug and unwind info is emitted.  */
    void (*code_end) (void);

    /* Output an assembler pseudo-op to declare a library function name
       external.  */
    void (*external_libcall) (rtx);

    /* Output an assembler directive to mark decl live. This instructs
	linker to not dead code strip this symbol.  */
    void (*mark_decl_preserved) (const char *);

    /* Output a record of the command line switches that have been passed.  */
    print_switch_fn_type record_gcc_switches;
    /* The name of the section that the example ELF implementation of
       record_gcc_switches will use to store the information.  Target
       specific versions of record_gcc_switches may or may not use
       this information.  */
    const char * record_gcc_switches_section;

    /* Output the definition of a section anchor.  */
    void (*output_anchor) (rtx);

    /* Output a DTP-relative reference to a TLS symbol.  */
    void (*output_dwarf_dtprel) (FILE *file, int size, rtx x);

    /* Some target machines need to postscan each insn after it is output.  */
    void (*final_postscan_insn) (FILE *, rtx, rtx *, int);

    /* Emit the trampoline template.  This hook may be NULL.  */
    void (*trampoline_template) (FILE *);
  } asm_out;

  /* Functions relating to instruction scheduling.  */
  struct sched
  {
    /* Given the current cost, COST, of an insn, INSN, calculate and
       return a new cost based on its relationship to DEP_INSN through
       the dependence LINK.  The default is to make no adjustment.  */
    int (* adjust_cost) (rtx insn, rtx link, rtx dep_insn, int cost);

    /* Adjust the priority of an insn as you see fit.  Returns the new
       priority.  */
    int (* adjust_priority) (rtx, int);

    /* Function which returns the maximum number of insns that can be
       scheduled in the same machine cycle.  This must be constant
       over an entire compilation.  The default is 1.  */
    int (* issue_rate) (void);

    /* Calculate how much this insn affects how many more insns we
       can emit this cycle.  Default is they all cost the same.  */
    int (* variable_issue) (FILE *, int, rtx, int);

    /* Initialize machine-dependent scheduling code.  */
    void (* md_init) (FILE *, int, int);

    /* Finalize machine-dependent scheduling code.  */
    void (* md_finish) (FILE *, int);

    /* Initialize machine-dependent function wide scheduling code.  */
    void (* md_init_global) (FILE *, int, int);

    /* Finalize machine-dependent function wide scheduling code.  */
    void (* md_finish_global) (FILE *, int);

    /* Reorder insns in a machine-dependent fashion, in two different
       places.  Default does nothing.  */
    int (* reorder) (FILE *, int, rtx *, int *, int);
    int (* reorder2) (FILE *, int, rtx *, int *, int);

    /* The following member value is a pointer to a function called
       after evaluation forward dependencies of insns in chain given
       by two parameter values (head and tail correspondingly).  */
    void (* dependencies_evaluation_hook) (rtx, rtx);

    /* The values of the following four members are pointers to
       functions used to simplify the automaton descriptions.
       dfa_pre_cycle_insn and dfa_post_cycle_insn give functions
       returning insns which are used to change the pipeline hazard
       recognizer state when the new simulated processor cycle
       correspondingly starts and finishes.  The function defined by
       init_dfa_pre_cycle_insn and init_dfa_post_cycle_insn are used
       to initialize the corresponding insns.  The default values of
       the members result in not changing the automaton state when
       the new simulated processor cycle correspondingly starts and
       finishes.  */
    void (* init_dfa_pre_cycle_insn) (void);
    rtx (* dfa_pre_cycle_insn) (void);
    void (* init_dfa_post_cycle_insn) (void);
    rtx (* dfa_post_cycle_insn) (void);

    /* The values of the following two members are pointers to
       functions used to simplify the automaton descriptions.
       dfa_pre_advance_cycle and dfa_post_advance_cycle are getting called
       immediately before and after cycle is advanced.  */
    void (* dfa_pre_advance_cycle) (void);
    void (* dfa_post_advance_cycle) (void);

    /* The following member value is a pointer to a function returning value
       which defines how many insns in queue `ready' will we try for
       multi-pass scheduling.  If the member value is nonzero and the
       function returns positive value, the DFA based scheduler will make
       multi-pass scheduling for the first cycle.  In other words, we will
       try to choose ready insn which permits to start maximum number of
       insns on the same cycle.  */
    int (* first_cycle_multipass_dfa_lookahead) (void);

    /* The following member value is pointer to a function controlling
       what insns from the ready insn queue will be considered for the
       multipass insn scheduling.  If the hook returns zero for insn
       passed as the parameter, the insn will be not chosen to be
       issued.  */
    int (* first_cycle_multipass_dfa_lookahead_guard) (rtx);

    /* The following member value is pointer to a function called by
       the insn scheduler before issuing insn passed as the third
       parameter on given cycle.  If the hook returns nonzero, the
       insn is not issued on given processors cycle.  Instead of that,
       the processor cycle is advanced.  If the value passed through
       the last parameter is zero, the insn ready queue is not sorted
       on the new cycle start as usually.  The first parameter passes
       file for debugging output.  The second one passes the scheduler
       verbose level of the debugging output.  The forth and the fifth
       parameter values are correspondingly processor cycle on which
       the previous insn has been issued and the current processor
       cycle.  */
    int (* dfa_new_cycle) (FILE *, int, rtx, int, int, int *);

    /* The following member value is a pointer to a function called by the
       insn scheduler.  It should return true if there exists a dependence
       which is considered costly by the target, between the insn
       DEP_PRO (&_DEP), and the insn DEP_CON (&_DEP).  The first parameter is
       the dep that represents the dependence between the two insns.  The
       second argument is the cost of the dependence as estimated by
       the scheduler.  The last argument is the distance in cycles
       between the already scheduled insn (first parameter) and the
       second insn (second parameter).  */
    bool (* is_costly_dependence) (struct _dep *_dep, int, int);

    /* Given the current cost, COST, of an insn, INSN, calculate and
       return a new cost based on its relationship to DEP_INSN through the
       dependence of type DEP_TYPE.  The default is to make no adjustment.  */
    int (* adjust_cost_2) (rtx insn, int, rtx dep_insn, int cost, int dw);

    /* The following member value is a pointer to a function called
       by the insn scheduler. This hook is called to notify the backend
       that new instructions were emitted.  */
    void (* h_i_d_extended) (void);

    /* Next 5 functions are for multi-point scheduling.  */

    /* Allocate memory for scheduler context.  */
    void *(* alloc_sched_context) (void);

    /* Fills the context from the local machine scheduler context.  */
    void (* init_sched_context) (void *, bool);

    /* Sets local machine scheduler context to a saved value.  */
    void (* set_sched_context) (void *);

    /* Clears a scheduler context so it becomes like after init.  */
    void (* clear_sched_context) (void *);

    /* Frees the scheduler context.  */
    void (* free_sched_context) (void *);

    /* The following member value is a pointer to a function called
       by the insn scheduler.
       The first parameter is an instruction, the second parameter is the type
       of the requested speculation, and the third parameter is a pointer to the
       speculative pattern of the corresponding type (set if return value == 1).
       It should return
       -1, if there is no pattern, that will satisfy the requested speculation
       type,
       0, if current pattern satisfies the requested speculation type,
       1, if pattern of the instruction should be changed to the newly
       generated one.  */
    int (* speculate_insn) (rtx, int, rtx *);

    /* The following member value is a pointer to a function called
       by the insn scheduler.  It should return true if the check instruction
       passed as the parameter needs a recovery block.  */
    bool (* needs_block_p) (int);

    /* The following member value is a pointer to a function called
       by the insn scheduler.  It should return a pattern for the check
       instruction.
       The first parameter is a speculative instruction, the second parameter
       is the label of the corresponding recovery block (or null, if it is a
       simple check).  If the mutation of the check is requested (e.g. from
       ld.c to chk.a), the third parameter is true - in this case the first
       parameter is the previous check.  */
    rtx (* gen_spec_check) (rtx, rtx, int);

    /* The following member value is a pointer to a function controlling
       what insns from the ready insn queue will be considered for the
       multipass insn scheduling.  If the hook returns zero for the insn
       passed as the parameter, the insn will not be chosen to be
       issued.  This hook is used to discard speculative instructions,
       that stand at the first position of the ready list.  */
    bool (* first_cycle_multipass_dfa_lookahead_guard_spec) (const_rtx);

    /* The following member value is a pointer to a function that provides
       information about the speculation capabilities of the target.
       The parameter is a pointer to spec_info variable.  */
    void (* set_sched_flags) (struct spec_info_def *);

    /* Return speculation types of the instruction passed as the parameter.  */
    int (* get_insn_spec_ds) (rtx);

    /* Return speculation types that are checked for the instruction passed as
       the parameter.  */
    int (* get_insn_checked_ds) (rtx);

    /* Return bool if rtx scanning should just skip current layer and
       advance to the inner rtxes.  */
    bool (* skip_rtx_p) (const_rtx);

    /* The following member value is a pointer to a function that provides
       information about the target resource-based lower bound which is
       used by the swing modulo scheduler.  The parameter is a pointer
       to ddg variable.  */
    int (* sms_res_mii) (struct ddg *);
  } sched;

  /* Functions relating to vectorization.  */
  struct vectorize
  {
    /* The following member value is a pointer to a function called
       by the vectorizer, and return the decl of the target builtin
       function.  */
    tree (* builtin_mask_for_load) (void);

    /* Returns a code for builtin that realizes vectorized version of
       function, or NULL_TREE if not available.  */
    tree (* builtin_vectorized_function) (tree, tree, tree);

    /* Returns a code for builtin that realizes vectorized version of
       conversion, or NULL_TREE if not available.  */
    tree (* builtin_conversion) (unsigned, tree);

    /* Target builtin that implements vector widening multiplication.
       builtin_mul_widen_eve computes the element-by-element products
       for the even elements, and builtin_mul_widen_odd computes the
       element-by-element products for the odd elements.  */
    tree (* builtin_mul_widen_even) (tree);
    tree (* builtin_mul_widen_odd) (tree);

    /* Returns the cost to be added to the overheads involved with
       executing the vectorized version of a loop.  */
    int (*builtin_vectorization_cost) (bool);

    /* Return true if vector alignment is reachable (by peeling N
       iterations) for the given type.  */
    bool (* vector_alignment_reachable) (const_tree, bool);

    /* Target builtin that implements vector permute.  */
    tree (* builtin_vec_perm) (tree, tree*);

    /* Return true if a vector created for builtin_vec_perm is valid.  */
    bool (* builtin_vec_perm_ok) (tree, tree);

    /* Return true if the target supports misaligned store/load of a
       specific factor denoted in the third parameter.  The last parameter
       is true if the access is defined in a packed struct.  */
    bool (* builtin_support_vector_misalignment) (enum machine_mode,
                                                  const_tree, int, bool);
  } vectorize;

  /* The initial value of target_flags.  */
  int default_target_flags;

  /* Allow target specific overriding of option settings after options have
     been changed by an attribute or pragma or when it is reset at the
     end of the code affected by an attribute or pragma.  */
  void (* override_options_after_change) (void);

  /* Handle target switch CODE (an OPT_* value).  ARG is the argument
     passed to the switch; it is NULL if no argument was.  VALUE is the
     value of ARG if CODE specifies a UInteger option, otherwise it is
     1 if the positive form of the switch was used and 0 if the negative
     form was.  Return true if the switch was valid.  */
  bool (* handle_option) (size_t code, const char *arg, int value);

  /* Display extra, target specific information in response to a
     --target-help switch.  */
  void (* target_help) (void);

  /* Return machine mode for filter value.  */
  enum machine_mode (* eh_return_filter_mode) (void);

  /* Return machine mode for libgcc expanded cmp instructions.  */
  enum machine_mode (* libgcc_cmp_return_mode) (void);

  /* Return machine mode for libgcc expanded shift instructions.  */
  enum machine_mode (* libgcc_shift_count_mode) (void);

  /* Return machine mode to be used for _Unwind_Word type.  */
  enum machine_mode (* unwind_word_mode) (void);

  /* Given two decls, merge their attributes and return the result.  */
  tree (* merge_decl_attributes) (tree, tree);

  /* Given two types, merge their attributes and return the result.  */
  tree (* merge_type_attributes) (tree, tree);

  /* Table of machine attributes and functions to handle them.
     Ignored if NULL.  */
  const struct attribute_spec *attribute_table;

  /* Return zero if the attributes on TYPE1 and TYPE2 are incompatible,
     one if they are compatible and two if they are nearly compatible
     (which causes a warning to be generated).  */
  int (* comp_type_attributes) (const_tree type1, const_tree type2);

  /* Assign default attributes to the newly defined TYPE.  */
  void (* set_default_type_attributes) (tree type);

  /* Insert attributes on the newly created DECL.  */
  void (* insert_attributes) (tree decl, tree *attributes);

  /* Return true if FNDECL (which has at least one machine attribute)
     can be inlined despite its machine attributes, false otherwise.  */
  bool (* function_attribute_inlinable_p) (const_tree fndecl);

  /* Return true if bitfields in RECORD_TYPE should follow the
     Microsoft Visual C++ bitfield layout rules.  */
  bool (* ms_bitfield_layout_p) (const_tree record_type);

  /* True if the target supports decimal floating point.  */
  bool (* decimal_float_supported_p) (void);

  /* True if the target supports fixed-point.  */
  bool (* fixed_point_supported_p) (void);

  /* Return true if anonymous bitfields affect structure alignment.  */
  bool (* align_anon_bitfield) (void);

  /* Return true if volatile bitfields should use the narrowest type possible.
     Return false if they should use the container type.  */
  bool (* narrow_volatile_bitfield) (void);

  /* Set up target-specific built-in functions.  */
  void (* init_builtins) (void);

  /* Initialize (if INITIALIZE_P is true) and return the target-specific
     built-in function decl for CODE.
     Return NULL if that is not possible.  Return error_mark_node if CODE
     is outside of the range of valid target builtin function codes.  */
  tree (* builtin_decl) (unsigned code, bool initialize_p);

  /* Expand a target-specific builtin.  */
  rtx (* expand_builtin) (tree exp, rtx target, rtx subtarget,
			  enum machine_mode mode, int ignore);

  /* Select a replacement for a target-specific builtin.  This is done
     *before* regular type checking, and so allows the target to
     implement a crude form of function overloading.  The result is a
     complete expression that implements the operation.  PARAMS really
     has type VEC(tree,gc)*, but we don't want to include tree.h
     here.  */
  tree (*resolve_overloaded_builtin) (unsigned int /*location_t*/,
      				      tree decl, void *params);

  /* Fold a target-specific builtin.  */
  tree (* fold_builtin) (tree fndecl, tree arglist, bool ignore);

  /* Returns a code for a target-specific builtin that implements
     reciprocal of the function, or NULL_TREE if not available.  */
  tree (* builtin_reciprocal) (unsigned, bool, bool);

  /* For a vendor-specific TYPE, return a pointer to a statically-allocated
     string containing the C++ mangling for TYPE.  In all other cases, return
     NULL.  */
  const char * (* mangle_type) (const_tree type);

  /* Make any adjustments to libfunc names needed for this target.  */
  void (* init_libfuncs) (void);

  /* Given a decl, a section name, and whether the decl initializer
     has relocs, choose attributes for the section.  */
  /* ??? Should be merged with SELECT_SECTION and UNIQUE_SECTION.  */
  unsigned int (* section_type_flags) (tree, const char *, int);

  /* True if new jumps cannot be created, to replace existing ones or
     not, at the current point in the compilation.  */
  bool (* cannot_modify_jumps_p) (void);

  /* Return a register class for which branch target register
     optimizations should be applied.  */
  enum reg_class (* branch_target_register_class) (void);

  /* Return true if branch target register optimizations should include
     callee-saved registers that are not already live during the current
     function.  AFTER_PE_GEN is true if prologues and epilogues have
     already been generated.  */
  bool (* branch_target_register_callee_saved) (bool after_pe_gen);

  /* Return true if the target supports conditional execution.  */
  bool (* have_conditional_execution) (void);

  /* True if the constant X cannot be placed in the constant pool.  */
  bool (* cannot_force_const_mem) (rtx);

  /* True if the insn X cannot be duplicated.  */
  bool (* cannot_copy_insn_p) (rtx);

  /* True if X is considered to be commutative.  */
  bool (* commutative_p) (const_rtx, int);

  /* Given an invalid address X for a given machine mode, try machine-specific
     ways to make it legitimate.  Return X or an invalid address on failure.  */
  rtx (* legitimize_address) (rtx, rtx, enum machine_mode);

  /* Given an address RTX, undo the effects of LEGITIMIZE_ADDRESS.  */
  rtx (* delegitimize_address) (rtx);

  /* Given an address RTX, say whether it is valid.  */
  bool (* legitimate_address_p) (enum machine_mode, rtx, bool);

  /* True if the given constant can be put into an object_block.  */
  bool (* use_blocks_for_constant_p) (enum machine_mode, const_rtx);

  /* The minimum and maximum byte offsets for anchored addresses.  */
  HOST_WIDE_INT min_anchor_offset;
  HOST_WIDE_INT max_anchor_offset;

  /* True if section anchors can be used to access the given symbol.  */
  bool (* use_anchors_for_symbol_p) (const_rtx);

  /* True if it is OK to do sibling call optimization for the specified
     call expression EXP.  DECL will be the called function, or NULL if
     this is an indirect call.  */
  bool (*function_ok_for_sibcall) (tree decl, tree exp);

  /* Establish appropriate back-end context for processing the function
     FNDECL.  The argument might be NULL to indicate processing at top
     level, outside of any function scope.  */
  void (*set_current_function) (tree fndecl);

  /* True if EXP should be placed in a "small data" section.  */
  bool (* in_small_data_p) (const_tree);

  /* True if EXP names an object for which name resolution must resolve
     to the current executable or shared library.  */
  bool (* binds_local_p) (const_tree);

  /* Modify and return the identifier of a DECL's external name,
     originally identified by ID, as required by the target,
    (eg, append @nn to windows32 stdcall function names).
     The default is to return ID without modification. */
   tree (* mangle_decl_assembler_name) (tree decl, tree  id);

  /* Do something target-specific to record properties of the DECL into
     the associated SYMBOL_REF.  */
  void (* encode_section_info) (tree, rtx, int);

  /* Undo the effects of encode_section_info on the symbol string.  */
  const char * (* strip_name_encoding) (const char *);

  /* If shift optabs for MODE are known to always truncate the shift count,
     return the mask that they apply.  Return 0 otherwise.  */
  unsigned HOST_WIDE_INT (* shift_truncation_mask) (enum machine_mode mode);

  /* Return the number of divisions in the given MODE that should be present,
     so that it is profitable to turn the division into a multiplication by
     the reciprocal.  */
  unsigned int (* min_divisions_for_recip_mul) (enum machine_mode mode);

  /* If the representation of integral MODE is such that values are
     always sign-extended to a wider mode MODE_REP then return
     SIGN_EXTEND.  Return UNKNOWN otherwise.  */
  /* Note that the return type ought to be RTX_CODE, but that's not
     necessarily defined at this point.  */
  int (* mode_rep_extended) (enum machine_mode mode,
			     enum machine_mode mode_rep);

  /* True if MODE is valid for a pointer in __attribute__((mode("MODE"))).  */
  bool (* valid_pointer_mode) (enum machine_mode mode);

  /* Support for named address spaces.  */
  struct addr_space {
    /* MODE to use for a pointer into another address space.  */
    enum machine_mode (* pointer_mode) (addr_space_t);

    /* MODE to use for an address in another address space.  */
    enum machine_mode (* address_mode) (addr_space_t);

    /* True if MODE is valid for a pointer in __attribute__((mode("MODE")))
       in another address space.  */
    bool (* valid_pointer_mode) (enum machine_mode, addr_space_t);

    /* True if an address is a valid memory address to a given named address
       space for a given mode.  */
    bool (* legitimate_address_p) (enum machine_mode, rtx, bool, addr_space_t);

    /* Return an updated address to convert an invalid pointer to a named
       address space to a valid one.  If NULL_RTX is returned use machine
       independent methods to make the address valid.  */
    rtx (* legitimize_address) (rtx, rtx, enum machine_mode, addr_space_t);

    /* True if one named address space is a subset of another named address. */
    bool (* subset_p) (addr_space_t, addr_space_t);

    /* Function to convert an rtl expression from one address space to
       another.  */
    rtx (* convert) (rtx, tree, tree);

  } addr_space;

  /* True if MODE is valid for the target.  By "valid", we mean able to
     be manipulated in non-trivial ways.  In particular, this means all
     the arithmetic is supported.  */
  bool (* scalar_mode_supported_p) (enum machine_mode mode);

  /* Similarly for vector modes.  "Supported" here is less strict.  At
     least some operations are supported; need to check optabs or builtins
     for further details.  */
  bool (* vector_mode_supported_p) (enum machine_mode mode);

  /* Compute a (partial) cost for rtx X.  Return true if the complete
     cost has been computed, and false if subexpressions should be
     scanned.  In either case, *TOTAL contains the cost result.  */
  /* Note that CODE and OUTER_CODE ought to be RTX_CODE, but that's
     not necessarily defined at this point.  */
  bool (* rtx_costs) (rtx x, int code, int outer_code, int *total, bool speed);

  /* Compute the cost of X, used as an address.  Never called with
     invalid addresses.  */
  int (* address_cost) (rtx x, bool speed);

  /* Return where to allocate pseudo for a given hard register initial
     value.  */
  rtx (* allocate_initial_value) (rtx x);

  /* Return nonzero if evaluating UNSPEC[_VOLATILE] X might cause a trap.
     FLAGS has the same meaning as in rtlanal.c: may_trap_p_1.  */
  int (* unspec_may_trap_p) (const_rtx x, unsigned flags);

  /* Given a register, this hook should return a parallel of registers
     to represent where to find the register pieces.  Define this hook
     if the register and its mode are represented in Dwarf in
     non-contiguous locations, or if the register should be
     represented in more than one register in Dwarf.  Otherwise, this
     hook should return NULL_RTX.  */
  rtx (* dwarf_register_span) (rtx);

  /* If expand_builtin_init_dwarf_reg_sizes needs to fill in table
     entries not corresponding directly to registers below
     FIRST_PSEUDO_REGISTER, this hook should generate the necessary
     code, given the address of the table.  */
  void (* init_dwarf_reg_sizes_extra) (tree);

  /* Fetch the fixed register(s) which hold condition codes, for
     targets where it makes sense to look for duplicate assignments to
     the condition codes.  This should return true if there is such a
     register, false otherwise.  The arguments should be set to the
     fixed register numbers.  Up to two condition code registers are
     supported.  If there is only one for this target, the int pointed
     at by the second argument should be set to -1.  */
  bool (* fixed_condition_code_regs) (unsigned int *, unsigned int *);

  /* If two condition code modes are compatible, return a condition
     code mode which is compatible with both, such that a comparison
     done in the returned mode will work for both of the original
     modes.  If the condition code modes are not compatible, return
     VOIDmode.  */
  enum machine_mode (* cc_modes_compatible) (enum machine_mode,
					     enum machine_mode);

  /* Do machine-dependent code transformations.  Called just before
     delayed-branch scheduling.  */
  void (* machine_dependent_reorg) (void);

  /* Create the __builtin_va_list type.  */
  tree (* build_builtin_va_list) (void);

  /* Get the cfun/fndecl calling abi __builtin_va_list type.  */
  tree (* fn_abi_va_list) (tree);

  /* Get the __builtin_va_list type dependent on input type.  */
  tree (* canonical_va_list_type) (tree);

  /* Expand the __builtin_va_start builtin.  */
  void (* expand_builtin_va_start) (tree valist, rtx nextarg);

  /* Gimplifies a VA_ARG_EXPR.  */
  tree (* gimplify_va_arg_expr) (tree valist, tree type, gimple_seq *pre_p,
				 gimple_seq *post_p);

  /* Validity-checking routines for PCH files, target-specific.
     get_pch_validity returns a pointer to the data to be stored,
     and stores the size in its argument.  pch_valid_p gets the same
     information back and returns NULL if the PCH is valid,
     or an error message if not.
  */
  void * (* get_pch_validity) (size_t *);
  const char * (* pch_valid_p) (const void *, size_t);

  /* If nonnull, this function checks whether a PCH file with the
     given set of target flags can be used.  It returns NULL if so,
     otherwise it returns an error message.  */
  const char *(*check_pch_target_flags) (int);

  /* True if the compiler should give an enum type only as many
     bytes as it takes to represent the range of possible values of
     that type.  */
  bool (* default_short_enums) (void);

  /* This target hook returns an rtx that is used to store the address
     of the current frame into the built-in setjmp buffer.  */
  rtx (* builtin_setjmp_frame_value) (void);

  /* This target hook should add STRING_CST trees for any hard regs
     the port wishes to automatically clobber for an asm.  */
  tree (* md_asm_clobbers) (tree, tree, tree);

  /* This target hook allows the backend to specify a calling convention
     in the debug information.  This function actually returns an
     enum dwarf_calling_convention, but because of forward declarations
     and not wanting to include dwarf2.h everywhere target.h is included
     the function is being declared as an int.  */
  int (* dwarf_calling_convention) (const_tree);

  /* This target hook allows the backend to emit frame-related insns that
     contain UNSPECs or UNSPEC_VOLATILEs.  The call frame debugging info
     engine will invoke it on insns of the form
       (set (reg) (unspec [...] UNSPEC_INDEX))
     and
       (set (reg) (unspec_volatile [...] UNSPECV_INDEX))
     to let the backend emit the call frame instructions.  */
  void (* dwarf_handle_frame_unspec) (const char *, rtx, int);

  /* Perform architecture specific checking of statements gimplified
     from VA_ARG_EXPR.  STMT is the statement.  Returns true if the statement
     doesn't need to be checked for va_list references.  */
  bool (* stdarg_optimize_hook) (struct stdarg_info *ai, const_gimple stmt);

  /* This target hook allows the operating system to override the DECL
     that represents the external variable that contains the stack
     protection guard variable.  The type of this DECL is ptr_type_node.  */
  tree (* stack_protect_guard) (void);

  /* This target hook allows the operating system to override the CALL_EXPR
     that is invoked when a check vs the guard variable fails.  */
  tree (* stack_protect_fail) (void);

  /* Returns NULL if target supports the insn within a doloop block,
     otherwise it returns an error message.  */
  const char * (*invalid_within_doloop) (const_rtx);

  /* DECL is a variable or function with __attribute__((dllimport))
     specified.  Use this hook if the target needs to add extra validation
     checks to  handle_dll_attribute ().  */
  bool (* valid_dllimport_attribute_p) (const_tree decl);

  /* If non-zero, align constant anchors in CSE to a multiple of this
     value.  */
  unsigned HOST_WIDE_INT const_anchor;

  /* Functions relating to calls - argument passing, returns, etc.  */
  struct calls {
    enum machine_mode (*promote_function_mode) (const_tree type,
						enum machine_mode mode,
						int *punsignedp,
						const_tree fntype,
						int for_return);
    bool (*promote_prototypes) (const_tree fntype);
    rtx (*struct_value_rtx) (tree fndecl, int incoming);
    bool (*return_in_memory) (const_tree type, const_tree fndecl);
    bool (*return_in_msb) (const_tree type);

    /* Return true if a parameter must be passed by reference.  TYPE may
       be null if this is a libcall.  CA may be null if this query is
       from __builtin_va_arg.  */
    bool (*pass_by_reference) (CUMULATIVE_ARGS *ca, enum machine_mode mode,
			       const_tree type, bool named_arg);

    rtx (*expand_builtin_saveregs) (void);
    /* Returns pretend_argument_size.  */
    void (*setup_incoming_varargs) (CUMULATIVE_ARGS *ca, enum machine_mode mode,
				    tree type, int *pretend_arg_size,
				    int second_time);
    bool (*strict_argument_naming) (CUMULATIVE_ARGS *ca);
    /* Returns true if we should use
       targetm.calls.setup_incoming_varargs() and/or
       targetm.calls.strict_argument_naming().  */
    bool (*pretend_outgoing_varargs_named) (CUMULATIVE_ARGS *ca);

    /* Given a complex type T, return true if a parameter of type T
       should be passed as two scalars.  */
    bool (* split_complex_arg) (const_tree type);

    /* Return true if type T, mode MODE, may not be passed in registers,
       but must be passed on the stack.  */
    /* ??? This predicate should be applied strictly after pass-by-reference.
       Need audit to verify that this is the case.  */
    bool (* must_pass_in_stack) (enum machine_mode mode, const_tree t);

    /* Return true if type TYPE, mode MODE, which is passed by reference,
       should have the object copy generated by the callee rather than
       the caller.  It is never called for TYPE requiring constructors.  */
    bool (* callee_copies) (CUMULATIVE_ARGS *ca, enum machine_mode mode,
			    const_tree type, bool named);

    /* Return zero for arguments passed entirely on the stack or entirely
       in registers.  If passed in both, return the number of bytes passed
       in registers; the balance is therefore passed on the stack.  */
    int (* arg_partial_bytes) (CUMULATIVE_ARGS *ca, enum machine_mode mode,
			       tree type, bool named);

    /* Return the diagnostic message string if function without a prototype
       is not allowed for this 'val' argument; NULL otherwise. */
    const char *(*invalid_arg_for_unprototyped_fn) (const_tree typelist,
					     	    const_tree funcdecl,
						    const_tree val);

    /* Return an rtx for the return value location of the function
       specified by FN_DECL_OR_TYPE with a return type of RET_TYPE.  */
    rtx (*function_value) (const_tree ret_type, const_tree fn_decl_or_type,
			   bool outgoing);

    /* Return the rtx for the result of a libcall of mode MODE,
       calling the function FN_NAME.  */
    rtx (*libcall_value) (enum machine_mode, const_rtx);

    /* Return mode and signedness to use when an libcall argument or
       result in the given mode is promoted.  */
    enum machine_mode (*promote_libcall_mode) (enum machine_mode, int *,
					       const_tree, int);

    /* Return an rtx for the argument pointer incoming to the
       current function.  */
    rtx (*internal_arg_pointer) (void);

    /* Update the current function stack boundary if needed.  */
    void (*update_stack_boundary) (void);

    /* Handle stack alignment and return an rtx for Dynamic Realign
       Argument Pointer if necessary.  */
    rtx (*get_drap_rtx) (void);

    /* Return true if all function parameters should be spilled to the
       stack.  */
    bool (*allocate_stack_slots_for_args) (void);

    /* Return an rtx for the static chain for FNDECL.  If INCOMING_P is true,
       then it should be for the callee; otherwise for the caller.  */
    rtx (*static_chain) (const_tree fndecl, bool incoming_p);

    /* Fill in the trampoline at MEM with a call to FNDECL and a
       static chain value of CHAIN.  */
    void (*trampoline_init) (rtx mem, tree fndecl, rtx chain);

    /* Adjust the address of the trampoline in a target-specific way.  */
    rtx (*trampoline_adjust_address) (rtx addr);
  } calls;

  /* Return the diagnostic message string if conversion from FROMTYPE
     to TOTYPE is not allowed, NULL otherwise.  */
  const char *(*invalid_conversion) (const_tree fromtype, const_tree totype);

  /* Return the diagnostic message string if the unary operation OP is
     not permitted on TYPE, NULL otherwise.  */
  const char *(*invalid_unary_op) (int op, const_tree type);

  /* Return the diagnostic message string if the binary operation OP
     is not permitted on TYPE1 and TYPE2, NULL otherwise.  */
  const char *(*invalid_binary_op) (int op, const_tree type1, const_tree type2);

  /* Return the diagnostic message string if TYPE is not valid as a
     function parameter type, NULL otherwise.  */
  const char *(*invalid_parameter_type) (const_tree type);

  /* Return the diagnostic message string if TYPE is not valid as a
     function return type, NULL otherwise.  */
  const char *(*invalid_return_type) (const_tree type);

  /* If values of TYPE are promoted to some other type when used in
     expressions (analogous to the integer promotions), return that type,
     or NULL_TREE otherwise.  */
  tree (*promoted_type) (const_tree type);

  /* Convert EXPR to TYPE, if target-specific types with special conversion
     rules are involved.  Return the converted expression, or NULL to apply
     the standard conversion rules.  */
  tree (*convert_to_type) (tree type, tree expr);

  /* Return the array of IRA cover classes for the current target.  */
  const enum reg_class *(*ira_cover_classes) (void);

  /* Return the class for a secondary reload, and fill in extra information.  */
  enum reg_class (*secondary_reload) (bool, rtx, enum reg_class,
				      enum machine_mode,
				      secondary_reload_info *);

  /* This target hook allows the backend to perform additional
     processing while initializing for variable expansion.  */
  void (* expand_to_rtl_hook) (void);

  /* This target hook allows the backend to perform additional
     instantiations on rtx that are not actually in insns yet,
     but will be later.  */
  void (* instantiate_decls) (void);

  /* Return true if is OK to use a hard register REGNO as scratch register
     in peephole2.  */
  bool (* hard_regno_scratch_ok) (unsigned int regno);

  /* Return the smallest number of different values for which it is best to
     use a jump-table instead of a tree of conditional branches.  */
  unsigned int (* case_values_threshold) (void);

  /* Retutn true if a function must have and use a frame pointer.  */
  bool (* frame_pointer_required) (void);

  /* Returns true if the compiler is allowed to try to replace register number
     from-reg with register number to-reg.  */
  bool (* can_eliminate) (const int, const int);

  /* Functions specific to the C family of frontends.  */
  struct c {
    /* Return machine mode for non-standard suffix
       or VOIDmode if non-standard suffixes are unsupported.  */
    enum machine_mode (*mode_for_suffix) (char);
  } c;

  /* Functions specific to the C++ frontend.  */
  struct cxx {
    /* Return the integer type used for guard variables.  */
    tree (*guard_type) (void);
    /* Return true if only the low bit of the guard should be tested.  */
    bool (*guard_mask_bit) (void);
    /* Returns the size of the array cookie for an array of type.  */
    tree (*get_cookie_size) (tree);
    /* Returns true if the element size should be stored in the
       array cookie.  */
    bool (*cookie_has_size) (void);
    /* Allows backends to perform additional processing when
       deciding if a class should be exported or imported.  */
    int (*import_export_class) (tree, int);
    /* Returns true if constructors and destructors return "this".  */
    bool (*cdtor_returns_this) (void);
    /* Returns true if the key method for a class can be an inline
       function, so long as it is not declared inline in the class
       itself.  Returning true is the behavior required by the Itanium
       C++ ABI.  */
    bool (*key_method_may_be_inline) (void);
    /* DECL is a virtual table, virtual table table, typeinfo object,
       or other similar implicit class data object that will be
       emitted with external linkage in this translation unit.  No ELF
       visibility has been explicitly specified.  If the target needs
       to specify a visibility other than that of the containing class,
       use this hook to set DECL_VISIBILITY and
       DECL_VISIBILITY_SPECIFIED.  */
    void (*determine_class_data_visibility) (tree decl);
    /* Returns true (the default) if virtual tables and other
       similar implicit class data objects are always COMDAT if they
       have external linkage.  If this hook returns false, then
       class data for classes whose virtual table will be emitted in
       only one translation unit will not be COMDAT.  */
    bool (*class_data_always_comdat) (void);
    /* Returns true (the default) if the RTTI for the basic types,
       which is always defined in the C++ runtime, should be COMDAT;
       false if it should not be COMDAT.  */
    bool (*library_rtti_comdat) (void);
    /* Returns true if __aeabi_atexit should be used to register static
       destructors.  */
    bool (*use_aeabi_atexit) (void);
    /* Returns true if target may use atexit in the same manner as
    __cxa_atexit  to register static destructors.  */
    bool (*use_atexit_for_cxa_atexit) (void);
    /* TYPE is a C++ class (i.e., RECORD_TYPE or UNION_TYPE) that
       has just been defined.  Use this hook to make adjustments to the
       class  (eg, tweak visibility or perform any other required
       target modifications).  */
    void (*adjust_class_at_definition) (tree type);
  } cxx;

  /* Functions and data for emulated TLS support.  */
  struct emutls {
    /* Name of the address and common functions.  */
    const char *get_address;
    const char *register_common;

    /* Prefixes for proxy variable and template.  */
    const char *var_section;
    const char *tmpl_section;

    /* Prefixes for proxy variable and template.  */
    const char *var_prefix;
    const char *tmpl_prefix;

    /* Function to generate field definitions of the proxy variable.  */
    tree (*var_fields) (tree, tree *);

    /* Function to initialize a proxy variable.  */
    tree (*var_init) (tree, tree, tree);

    /* Whether we are allowed to alter the usual alignment of the
       proxy variable.  */
    bool var_align_fixed;

    /* Whether we can emit debug information for TLS vars.  */
    bool debug_form_tls_address;
  } emutls;

  struct target_option_hooks {
    /* Function to validate the attribute((option(...))) strings or NULL.  If
       the option is validated, it is assumed that DECL_FUNCTION_SPECIFIC will
       be filled in in the function decl node.  */
    bool (*valid_attribute_p) (tree, tree, tree, int);

    /* Function to save any extra target state in the target options
       structure.  */
    void (*save) (struct cl_target_option *);

    /* Function to restore any extra target state from the target options
       structure.  */
    void (*restore) (struct cl_target_option *);

    /* Function to print any extra target state from the target options
       structure.  */
    void (*print) (FILE *, int, struct cl_target_option *);

    /* Function to parse arguments to be validated for #pragma option, and to
       change the state if the options are valid.  If the first argument is
       NULL, the second argument specifies the default options to use.  Return
       true if the options are valid, and set the current state.  */
    bool (*pragma_parse) (tree, tree);

    /* Function to determine if one function can inline another function.  */
    bool (*can_inline_p) (tree, tree);
  } target_option;

  /* Functions related to alignment.  */
  struct target_align_hooks {
    /* Function that returns the log2 of the instruction alignment in bytes.  */
    int (*align_insns) (void);
  } target_align;

  /* For targets that need to mark extra registers as live on entry to
     the function, they should define this target hook and set their
     bits in the bitmap passed in. */
  void (*live_on_entry) (bitmap);

  /* Return false if warnings about missing return statements or suspect
     noreturn attributes should be suppressed for the current function.  */
  bool (*warn_func_result) (void);

  /* True if unwinding tables should be generated by default.  */
  bool unwind_tables_default;

  /* Leave the boolean fields at the end.  */

  /* True if arbitrary sections are supported.  */
  bool have_named_sections;

  /* True if we can create zeroed data by switching to a BSS section
     and then using ASM_OUTPUT_SKIP to allocate the space.  */
  bool have_switchable_bss_sections;

  /* True if "native" constructors and destructors are supported,
     false if we're using collect2 for the job.  */
  bool have_ctors_dtors;

  /* True if thread-local storage is supported.  */
  bool have_tls;

  /* True if a small readonly data section is supported.  */
  bool have_srodata_section;

  /* True if EH frame info sections should be zero-terminated.  */
  bool terminate_dw2_eh_frame_info;

  /* True if #NO_APP should be emitted at the beginning of
     assembly output.  */
  bool file_start_app_off;

  /* True if output_file_directive should be called for main_input_filename
     at the beginning of assembly output.  */
  bool file_start_file_directive;

  /* True if #pragma extern_prefix is to be supported.  */
  bool handle_pragma_extern_prefix;

  /* True if the target is allowed to reorder memory accesses unless
     synchronization is explicitly requested.  */
  bool relaxed_ordering;

  /* Returns true if we should generate exception tables for use with the
     ARM EABI.  The effects the encoding of function exception specifications.
   */
  bool arm_eabi_unwinder;

  /* Leave the boolean fields at the end.  */
};

extern struct gcc_target targetm;

struct gcc_targetcm {
  /* Handle target switch CODE (an OPT_* value).  ARG is the argument
     passed to the switch; it is NULL if no argument was.  VALUE is the
     value of ARG if CODE specifies a UInteger option, otherwise it is
     1 if the positive form of the switch was used and 0 if the negative
     form was.  Return true if the switch was valid.  */
  bool (*handle_c_option) (size_t code, const char *arg, int value);
};

/* Each target can provide their own.  */
extern struct gcc_targetcm targetcm;

#endif /* GCC_TARGET_H */
