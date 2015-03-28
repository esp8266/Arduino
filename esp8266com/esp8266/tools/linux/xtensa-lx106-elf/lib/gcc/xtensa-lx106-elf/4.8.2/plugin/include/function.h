/* Structure for saving state for a nested function.
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

#ifndef GCC_FUNCTION_H
#define GCC_FUNCTION_H

#include "hashtab.h"
#include "vec.h"
#include "machmode.h"
#include "tm.h"			/* For CUMULATIVE_ARGS.  */
#include "hard-reg-set.h"	/* For HARD_REG_SET in struct rtl_data. */
#include "input.h"		/* For location_t.  */

/* Stack of pending (incomplete) sequences saved by `start_sequence'.
   Each element describes one pending sequence.
   The main insn-chain is saved in the last element of the chain,
   unless the chain is empty.  */

struct GTY(()) sequence_stack {
  /* First and last insns in the chain of the saved sequence.  */
  rtx first;
  rtx last;
  struct sequence_stack *next;
};

struct GTY(()) emit_status {
  /* This is reset to LAST_VIRTUAL_REGISTER + 1 at the start of each function.
     After rtl generation, it is 1 plus the largest register number used.  */
  int x_reg_rtx_no;

  /* Lowest label number in current function.  */
  int x_first_label_num;

  /* The ends of the doubly-linked chain of rtl for the current function.
     Both are reset to null at the start of rtl generation for the function.

     start_sequence saves both of these on `sequence_stack' and then starts
     a new, nested sequence of insns.  */
  rtx x_first_insn;
  rtx x_last_insn;

  /* Stack of pending (incomplete) sequences saved by `start_sequence'.
     Each element describes one pending sequence.
     The main insn-chain is saved in the last element of the chain,
     unless the chain is empty.  */
  struct sequence_stack *sequence_stack;

  /* INSN_UID for next insn emitted.
     Reset to 1 for each function compiled.  */
  int x_cur_insn_uid;

  /* INSN_UID for next debug insn emitted.  Only used if
     --param min-nondebug-insn-uid=<value> is given with nonzero value.  */
  int x_cur_debug_insn_uid;

  /* The length of the regno_pointer_align, regno_decl, and x_regno_reg_rtx
     vectors.  Since these vectors are needed during the expansion phase when
     the total number of registers in the function is not yet known, the
     vectors are copied and made bigger when necessary.  */
  int regno_pointer_align_length;

  /* Indexed by pseudo register number, if nonzero gives the known alignment
     for that pseudo (if REG_POINTER is set in x_regno_reg_rtx).
     Allocated in parallel with x_regno_reg_rtx.  */
  unsigned char * GTY((skip)) regno_pointer_align;
};


/* Indexed by register number, gives an rtx for that register (and only
   that register).  For pseudo registers, it is the unique rtx for
   that pseudo.  For hard registers, it is an rtx of the mode specified
   by reg_raw_mode.

   FIXME: We could put it into emit_status struct, but gengtype is not
   able to deal with length attribute nested in top level structures.  */

extern GTY ((length ("crtl->emit.x_reg_rtx_no"))) rtx * regno_reg_rtx;

/* For backward compatibility... eventually these should all go away.  */
#define reg_rtx_no (crtl->emit.x_reg_rtx_no)
#define seq_stack (crtl->emit.sequence_stack)

#define REGNO_POINTER_ALIGN(REGNO) (crtl->emit.regno_pointer_align[REGNO])

struct GTY(()) expr_status {
  /* Number of units that we should eventually pop off the stack.
     These are the arguments to function calls that have already returned.  */
  int x_pending_stack_adjust;

  /* Under some ABIs, it is the caller's responsibility to pop arguments
     pushed for function calls.  A naive implementation would simply pop
     the arguments immediately after each call.  However, if several
     function calls are made in a row, it is typically cheaper to pop
     all the arguments after all of the calls are complete since a
     single pop instruction can be used.  Therefore, GCC attempts to
     defer popping the arguments until absolutely necessary.  (For
     example, at the end of a conditional, the arguments must be popped,
     since code outside the conditional won't know whether or not the
     arguments need to be popped.)

     When INHIBIT_DEFER_POP is nonzero, however, the compiler does not
     attempt to defer pops.  Instead, the stack is popped immediately
     after each call.  Rather then setting this variable directly, use
     NO_DEFER_POP and OK_DEFER_POP.  */
  int x_inhibit_defer_pop;

  /* If PREFERRED_STACK_BOUNDARY and PUSH_ROUNDING are defined, the stack
     boundary can be momentarily unaligned while pushing the arguments.
     Record the delta since last aligned boundary here in order to get
     stack alignment in the nested function calls working right.  */
  int x_stack_pointer_delta;

  /* Nonzero means __builtin_saveregs has already been done in this function.
     The value is the pseudoreg containing the value __builtin_saveregs
     returned.  */
  rtx x_saveregs_value;

  /* Similarly for __builtin_apply_args.  */
  rtx x_apply_args_value;

  /* List of labels that must never be deleted.  */
  rtx x_forced_labels;
};

typedef struct call_site_record_d *call_site_record;

/* RTL representation of exception handling.  */
struct GTY(()) rtl_eh {
  rtx ehr_stackadj;
  rtx ehr_handler;
  rtx ehr_label;

  rtx sjlj_fc;
  rtx sjlj_exit_after;

  vec<uchar, va_gc> *action_record_data;

  vec<call_site_record, va_gc> *call_site_record_v[2];
};

#define pending_stack_adjust (crtl->expr.x_pending_stack_adjust)
#define inhibit_defer_pop (crtl->expr.x_inhibit_defer_pop)
#define saveregs_value (crtl->expr.x_saveregs_value)
#define apply_args_value (crtl->expr.x_apply_args_value)
#define forced_labels (crtl->expr.x_forced_labels)
#define stack_pointer_delta (crtl->expr.x_stack_pointer_delta)

struct gimple_df;
struct temp_slot;
typedef struct temp_slot *temp_slot_p;
struct call_site_record_d;
struct dw_fde_struct;

struct ipa_opt_pass_d;
typedef struct ipa_opt_pass_d *ipa_opt_pass;


struct GTY(()) varasm_status {
  /* If we're using a per-function constant pool, this is it.  */
  struct rtx_constant_pool *pool;

  /* Number of tree-constants deferred during the expansion of this
     function.  */
  unsigned int deferred_constants;
};

/* Information mainlined about RTL representation of incoming arguments.  */
struct GTY(()) incoming_args {
  /* Number of bytes of args popped by function being compiled on its return.
     Zero if no bytes are to be popped.
     May affect compilation of return insn or of function epilogue.  */
  int pops_args;

  /* If function's args have a fixed size, this is that size, in bytes.
     Otherwise, it is -1.
     May affect compilation of return insn or of function epilogue.  */
  int size;

  /* # bytes the prologue should push and pretend that the caller pushed them.
     The prologue must do this, but only if parms can be passed in
     registers.  */
  int pretend_args_size;

  /* This is the offset from the arg pointer to the place where the first
     anonymous arg can be found, if there is one.  */
  rtx arg_offset_rtx;

  /* Quantities of various kinds of registers
     used for the current function's args.  */
  CUMULATIVE_ARGS info;

  /* The arg pointer hard register, or the pseudo into which it was copied.  */
  rtx internal_arg_pointer;
};

/* Data for function partitioning.  */
struct GTY(()) function_subsections {
  /* Assembly labels for the hot and cold text sections, to
     be used by debugger functions for determining the size of text
     sections.  */

  const char *hot_section_label;
  const char *cold_section_label;
  const char *hot_section_end_label;
  const char *cold_section_end_label;
};

/* Describe an empty area of space in the stack frame.  These can be chained
   into a list; this is used to keep track of space wasted for alignment
   reasons.  */
struct GTY(()) frame_space
{
  struct frame_space *next;

  HOST_WIDE_INT start;
  HOST_WIDE_INT length;
};

/* Datastructures maintained for currently processed function in RTL form.  */
struct GTY(()) rtl_data {
  struct expr_status expr;
  struct emit_status emit;
  struct varasm_status varasm;
  struct incoming_args args;
  struct function_subsections subsections;
  struct rtl_eh eh;

  /* For function.c  */

  /* # of bytes of outgoing arguments.  If ACCUMULATE_OUTGOING_ARGS is
     defined, the needed space is pushed by the prologue.  */
  int outgoing_args_size;

  /* If nonzero, an RTL expression for the location at which the current
     function returns its result.  If the current function returns its
     result in a register, current_function_return_rtx will always be
     the hard register containing the result.  */
  rtx return_rtx;

  /* Vector of initial-value pairs.  Each pair consists of a pseudo
     register of approprite mode that stores the initial value a hard
     register REGNO, and that hard register itself.  */
  /* ??? This could be a VEC but there is currently no way to define an
	 opaque VEC type.  */
  struct initial_value_struct *hard_reg_initial_vals;

  /* A variable living at the top of the frame that holds a known value.
     Used for detecting stack clobbers.  */
  tree stack_protect_guard;

  /* List (chain of EXPR_LIST) of labels heading the current handlers for
     nonlocal gotos.  */
  rtx x_nonlocal_goto_handler_labels;

  /* Label that will go on function epilogue.
     Jumping to this label serves as a "return" instruction
     on machines which require execution of the epilogue on all returns.  */
  rtx x_return_label;

  /* Label that will go on the end of function epilogue.
     Jumping to this label serves as a "naked return" instruction
     on machines which require execution of the epilogue on all returns.  */
  rtx x_naked_return_label;

  /* List (chain of EXPR_LISTs) of all stack slots in this function.
     Made for the sake of unshare_all_rtl.  */
  rtx x_stack_slot_list;

  /* List of empty areas in the stack frame.  */
  struct frame_space *frame_space_list;

  /* Place after which to insert the tail_recursion_label if we need one.  */
  rtx x_stack_check_probe_note;

  /* Location at which to save the argument pointer if it will need to be
     referenced.  There are two cases where this is done: if nonlocal gotos
     exist, or if vars stored at an offset from the argument pointer will be
     needed by inner routines.  */
  rtx x_arg_pointer_save_area;

  /* Dynamic Realign Argument Pointer used for realigning stack.  */
  rtx drap_reg;

  /* Offset to end of allocated area of stack frame.
     If stack grows down, this is the address of the last stack slot allocated.
     If stack grows up, this is the address for the next slot.  */
  HOST_WIDE_INT x_frame_offset;

  /* Insn after which register parms and SAVE_EXPRs are born, if nonopt.  */
  rtx x_parm_birth_insn;

  /* List of all used temporaries allocated, by level.  */
  vec<temp_slot_p, va_gc> *x_used_temp_slots;

  /* List of available temp slots.  */
  struct temp_slot *x_avail_temp_slots;

  /* Current nesting level for temporaries.  */
  int x_temp_slot_level;

  /* The largest alignment needed on the stack, including requirement
     for outgoing stack alignment.  */
  unsigned int stack_alignment_needed;

  /* Preferred alignment of the end of stack frame, which is preferred
     to call other functions.  */
  unsigned int preferred_stack_boundary;

  /* The minimum alignment of parameter stack.  */
  unsigned int parm_stack_boundary;

  /* The largest alignment of slot allocated on the stack.  */
  unsigned int max_used_stack_slot_alignment;

  /* The stack alignment estimated before reload, with consideration of
     following factors:
     1. Alignment of local stack variables (max_used_stack_slot_alignment)
     2. Alignment requirement to call other functions
        (preferred_stack_boundary)
     3. Alignment of non-local stack variables but might be spilled in
        local stack.  */
  unsigned int stack_alignment_estimated;

  /* For reorg.  */

  /* Nonzero if function being compiled called builtin_return_addr or
     builtin_frame_address with nonzero count.  */
  bool accesses_prior_frames;

  /* Nonzero if the function calls __builtin_eh_return.  */
  bool calls_eh_return;

  /* Nonzero if function saves all registers, e.g. if it has a nonlocal
     label that can reach the exit block via non-exceptional paths. */
  bool saves_all_registers;

  /* Nonzero if function being compiled has nonlocal gotos to parent
     function.  */
  bool has_nonlocal_goto;

  /* Nonzero if function being compiled has an asm statement.  */
  bool has_asm_statement;

  /* This bit is used by the exception handling logic.  It is set if all
     calls (if any) are sibling calls.  Such functions do not have to
     have EH tables generated, as they cannot throw.  A call to such a
     function, however, should be treated as throwing if any of its callees
     can throw.  */
  bool all_throwers_are_sibcalls;

  /* Nonzero if stack limit checking should be enabled in the current
     function.  */
  bool limit_stack;

  /* Nonzero if profiling code should be generated.  */
  bool profile;

  /* Nonzero if the current function uses the constant pool.  */
  bool uses_const_pool;

  /* Nonzero if the current function uses pic_offset_table_rtx.  */
  bool uses_pic_offset_table;

  /* Nonzero if the current function needs an lsda for exception handling.  */
  bool uses_eh_lsda;

  /* Set when the tail call has been produced.  */
  bool tail_call_emit;

  /* Nonzero if code to initialize arg_pointer_save_area has been emitted.  */
  bool arg_pointer_save_area_init;

  /* Nonzero if current function must be given a frame pointer.
     Set in reload1.c or lra-eliminations.c if anything is allocated
     on the stack there.  */
  bool frame_pointer_needed;

  /* When set, expand should optimize for speed.  */
  bool maybe_hot_insn_p;

  /* Nonzero if function stack realignment is needed.  This flag may be
     set twice: before and after reload.  It is set before reload wrt
     stack alignment estimation before reload.  It will be changed after
     reload if by then criteria of stack realignment is different.
     The value set after reload is the accurate one and is finalized.  */
  bool stack_realign_needed;

  /* Nonzero if function stack realignment is tried.  This flag is set
     only once before reload.  It affects register elimination.  This
     is used to generate DWARF debug info for stack variables.  */
  bool stack_realign_tried;

  /* Nonzero if function being compiled needs dynamic realigned
     argument pointer (drap) if stack needs realigning.  */
  bool need_drap;

  /* Nonzero if function stack realignment estimation is done, namely
     stack_realign_needed flag has been set before reload wrt estimated
     stack alignment info.  */
  bool stack_realign_processed;

  /* Nonzero if function stack realignment has been finalized, namely
     stack_realign_needed flag has been set and finalized after reload.  */
  bool stack_realign_finalized;

  /* True if dbr_schedule has already been called for this function.  */
  bool dbr_scheduled_p;

  /* True if current function can not throw.  Unlike
     TREE_NOTHROW (current_function_decl) it is set even for overwritable
     function where currently compiled version of it is nothrow.  */
  bool nothrow;

  /* True if we performed shrink-wrapping for the current function.  */
  bool shrink_wrapped;

  /* Nonzero if function being compiled doesn't modify the stack pointer
     (ignoring the prologue and epilogue).  This is only valid after
     pass_stack_ptr_mod has run.  */
  bool sp_is_unchanging;

  /* Nonzero if function being compiled doesn't contain any calls
     (ignoring the prologue and epilogue).  This is set prior to
     local register allocation and is valid for the remaining
     compiler passes.  */
  bool is_leaf;

  /* Nonzero if the function being compiled is a leaf function which only
     uses leaf registers.  This is valid after reload (specifically after
     sched2) and is useful only if the port defines LEAF_REGISTERS.  */
  bool uses_only_leaf_regs;

  /* Like regs_ever_live, but 1 if a reg is set or clobbered from an
     asm.  Unlike regs_ever_live, elements of this array corresponding
     to eliminable regs (like the frame pointer) are set if an asm
     sets them.  */
  HARD_REG_SET asm_clobbers;
};

#define return_label (crtl->x_return_label)
#define naked_return_label (crtl->x_naked_return_label)
#define stack_slot_list (crtl->x_stack_slot_list)
#define parm_birth_insn (crtl->x_parm_birth_insn)
#define frame_offset (crtl->x_frame_offset)
#define stack_check_probe_note (crtl->x_stack_check_probe_note)
#define arg_pointer_save_area (crtl->x_arg_pointer_save_area)
#define used_temp_slots (crtl->x_used_temp_slots)
#define avail_temp_slots (crtl->x_avail_temp_slots)
#define temp_slot_level (crtl->x_temp_slot_level)
#define nonlocal_goto_handler_labels (crtl->x_nonlocal_goto_handler_labels)
#define frame_pointer_needed (crtl->frame_pointer_needed)
#define stack_realign_fp (crtl->stack_realign_needed && !crtl->need_drap)
#define stack_realign_drap (crtl->stack_realign_needed && crtl->need_drap)

extern GTY(()) struct rtl_data x_rtl;

/* Accessor to RTL datastructures.  We keep them statically allocated now since
   we never keep multiple functions.  For threaded compiler we might however
   want to do differently.  */
#define crtl (&x_rtl)

struct GTY(()) stack_usage
{
  /* # of bytes of static stack space allocated by the function.  */
  HOST_WIDE_INT static_stack_size;

  /* # of bytes of dynamic stack space allocated by the function.  This is
     meaningful only if has_unbounded_dynamic_stack_size is zero.  */
  HOST_WIDE_INT dynamic_stack_size;

  /* # of bytes of space pushed onto the stack after the prologue.  If
     !ACCUMULATE_OUTGOING_ARGS, it contains the outgoing arguments.  */
  int pushed_stack_size;

  /* Nonzero if the amount of stack space allocated dynamically cannot
     be bounded at compile-time.  */
  unsigned int has_unbounded_dynamic_stack_size : 1;
};

#define current_function_static_stack_size (cfun->su->static_stack_size)
#define current_function_dynamic_stack_size (cfun->su->dynamic_stack_size)
#define current_function_pushed_stack_size (cfun->su->pushed_stack_size)
#define current_function_has_unbounded_dynamic_stack_size \
  (cfun->su->has_unbounded_dynamic_stack_size)
#define current_function_allocates_dynamic_stack_space    \
  (current_function_dynamic_stack_size != 0               \
   || current_function_has_unbounded_dynamic_stack_size)

/* This structure can save all the important global and static variables
   describing the status of the current function.  */

struct GTY(()) function {
  struct eh_status *eh;

  /* The control flow graph for this function.  */
  struct control_flow_graph *cfg;

  /* GIMPLE body for this function.  */
  gimple_seq gimple_body;

  /* SSA and dataflow information.  */
  struct gimple_df *gimple_df;

  /* The loops in this function.  */
  struct loops *x_current_loops;

  /* The stack usage of this function.  */
  struct stack_usage *su;

  /* Value histograms attached to particular statements.  */
  htab_t GTY((skip)) value_histograms;

  /* For function.c.  */

  /* Points to the FUNCTION_DECL of this function.  */
  tree decl;

  /* A PARM_DECL that should contain the static chain for this function.
     It will be initialized at the beginning of the function.  */
  tree static_chain_decl;

  /* An expression that contains the non-local goto save area.  The first
     word is the saved frame pointer and the second is the saved stack
     pointer.  */
  tree nonlocal_goto_save_area;

  /* Vector of function local variables, functions, types and constants.  */
  vec<tree, va_gc> *local_decls;

  /* For md files.  */

  /* tm.h can use this to store whatever it likes.  */
  struct machine_function * GTY ((maybe_undef)) machine;

  /* Language-specific code can use this to store whatever it likes.  */
  struct language_function * language;

  /* Used types hash table.  */
  htab_t GTY ((param_is (union tree_node))) used_types_hash;

  /* Dwarf2 Frame Description Entry, containing the Call Frame Instructions
     used for unwinding.  Only set when either dwarf2 unwinding or dwarf2
     debugging is enabled.  */
  struct dw_fde_struct *fde;

  /* Last statement uid.  */
  int last_stmt_uid;

  /* Function sequence number for profiling, debugging, etc.  */
  int funcdef_no;

  /* Line number of the start of the function for debugging purposes.  */
  location_t function_start_locus;

  /* Line number of the end of the function.  */
  location_t function_end_locus;

  /* Properties used by the pass manager.  */
  unsigned int curr_properties;
  unsigned int last_verified;

  /* Non-null if the function does something that would prevent it from
     being copied; this applies to both versioning and inlining.  Set to
     a string describing the reason for failure.  */
  const char * GTY((skip)) cannot_be_copied_reason;

  /* Collected bit flags.  */

  /* Number of units of general registers that need saving in stdarg
     function.  What unit is depends on the backend, either it is number
     of bytes, or it can be number of registers.  */
  unsigned int va_list_gpr_size : 8;

  /* Number of units of floating point registers that need saving in stdarg
     function.  */
  unsigned int va_list_fpr_size : 8;

  /* Nonzero if function being compiled can call setjmp.  */
  unsigned int calls_setjmp : 1;

  /* Nonzero if function being compiled can call alloca,
     either as a subroutine or builtin.  */
  unsigned int calls_alloca : 1;

  /* Nonzero if function being compiled receives nonlocal gotos
     from nested functions.  */
  unsigned int has_nonlocal_label : 1;

  /* Nonzero if we've set cannot_be_copied_reason.  I.e. if
     (cannot_be_copied_set && !cannot_be_copied_reason), the function
     can in fact be copied.  */
  unsigned int cannot_be_copied_set : 1;

  /* Nonzero if current function uses stdarg.h or equivalent.  */
  unsigned int stdarg : 1;

  unsigned int after_inlining : 1;
  unsigned int always_inline_functions_inlined : 1;

  /* Nonzero if function being compiled can throw synchronous non-call
     exceptions.  */
  unsigned int can_throw_non_call_exceptions : 1;

  /* Nonzero if instructions that may throw exceptions but don't otherwise
     contribute to the execution of the program can be deleted.  */
  unsigned int can_delete_dead_exceptions : 1;

  /* Fields below this point are not set for abstract functions; see
     allocate_struct_function.  */

  /* Nonzero if function being compiled needs to be given an address
     where the value should be stored.  */
  unsigned int returns_struct : 1;

  /* Nonzero if function being compiled needs to
     return the address of where it has put a structure value.  */
  unsigned int returns_pcc_struct : 1;

  /* Nonzero if this function has local DECL_HARD_REGISTER variables.
     In this case code motion has to be done more carefully.  */
  unsigned int has_local_explicit_reg_vars : 1;

  /* Nonzero if the current function is a thunk, i.e., a lightweight
     function implemented by the output_mi_thunk hook) that just
     adjusts one of its arguments and forwards to another
     function.  */
  unsigned int is_thunk : 1;
};

/* Add the decl D to the local_decls list of FUN.  */

static inline void
add_local_decl (struct function *fun, tree d)
{
  vec_safe_push (fun->local_decls, d);
}

#define FOR_EACH_LOCAL_DECL(FUN, I, D)		\
  FOR_EACH_VEC_SAFE_ELT_REVERSE ((FUN)->local_decls, I, D)

/* If va_list_[gf]pr_size is set to this, it means we don't know how
   many units need to be saved.  */
#define VA_LIST_MAX_GPR_SIZE	255
#define VA_LIST_MAX_FPR_SIZE	255

/* The function currently being compiled.  */
extern GTY(()) struct function *cfun;

/* In order to ensure that cfun is not set directly, we redefine it so
   that it is not an lvalue.  Rather than assign to cfun, use
   push_cfun or set_cfun.  */
#define cfun (cfun + 0)

/* Nonzero if we've already converted virtual regs to hard regs.  */
extern int virtuals_instantiated;

/* Nonzero if at least one trampoline has been created.  */
extern int trampolines_created;

struct GTY(()) types_used_by_vars_entry {
  tree type;
  tree var_decl;
};

/* Hash table making the relationship between a global variable
   and the types it references in its initializer. The key of the
   entry is a referenced type, and the value is the DECL of the global
   variable. types_use_by_vars_do_hash and types_used_by_vars_eq below are
   the hash and equality functions to use for this hash table.  */
extern GTY((param_is (struct types_used_by_vars_entry))) htab_t
  types_used_by_vars_hash;

hashval_t types_used_by_vars_do_hash (const void*);
int types_used_by_vars_eq (const void *, const void *);
void types_used_by_var_decl_insert (tree type, tree var_decl);

/* During parsing of a global variable, this vector contains the types
   referenced by the global variable.  */
extern GTY(()) vec<tree, va_gc> *types_used_by_cur_var_decl;

/* cfun shouldn't be set directly; use one of these functions instead.  */
extern void set_cfun (struct function *new_cfun);
extern void push_cfun (struct function *new_cfun);
extern void pop_cfun (void);
extern void instantiate_decl_rtl (rtx x);

/* For backward compatibility... eventually these should all go away.  */
#define current_function_funcdef_no (cfun->funcdef_no)

#define current_loops (cfun->x_current_loops)
#define dom_computed (cfun->cfg->x_dom_computed)
#define n_bbs_in_dom_tree (cfun->cfg->x_n_bbs_in_dom_tree)
#define VALUE_HISTOGRAMS(fun) (fun)->value_histograms

/* Identify BLOCKs referenced by more than one NOTE_INSN_BLOCK_{BEG,END},
   and create duplicate blocks.  */
extern void reorder_blocks (void);

/* Set BLOCK_NUMBER for all the blocks in FN.  */
extern void number_blocks (tree);

extern void clear_block_marks (tree);
extern tree blocks_nreverse (tree);
extern tree block_chainon (tree, tree);

/* Return size needed for stack frame based on slots so far allocated.
   This size counts from zero.  It is not rounded to STACK_BOUNDARY;
   the caller may have to do that.  */
extern HOST_WIDE_INT get_frame_size (void);

/* Issue an error message and return TRUE if frame OFFSET overflows in
   the signed target pointer arithmetics for function FUNC.  Otherwise
   return FALSE.  */
extern bool frame_offset_overflow (HOST_WIDE_INT, tree);

/* A pointer to a function to create target specific, per-function
   data structures.  */
extern struct machine_function * (*init_machine_status) (void);

/* Save and restore status information for a nested function.  */
extern void free_after_parsing (struct function *);
extern void free_after_compilation (struct function *);

extern void init_varasm_status (void);

#ifdef RTX_CODE
extern void diddle_return_value (void (*)(rtx, void*), void*);
extern void clobber_return_register (void);
#endif

extern rtx get_arg_pointer_save_area (void);

/* Returns the name of the current function.  */
extern const char *fndecl_name (tree);
extern const char *function_name (struct function *);
extern const char *current_function_name (void);

extern void do_warn_unused_parameter (tree);

extern bool pass_by_reference (CUMULATIVE_ARGS *, enum machine_mode,
			       tree, bool);
extern bool reference_callee_copied (CUMULATIVE_ARGS *, enum machine_mode,
				     tree, bool);

extern void used_types_insert (tree);

extern int get_next_funcdef_no (void);
extern int get_last_funcdef_no (void);

#ifdef HAVE_simple_return
extern bool requires_stack_frame_p (rtx, HARD_REG_SET, HARD_REG_SET);
#endif                        

extern rtx get_hard_reg_initial_val (enum machine_mode, unsigned int);
extern rtx has_hard_reg_initial_val (enum machine_mode, unsigned int);
extern rtx get_hard_reg_initial_reg (rtx);
extern bool initial_value_entry (int i, rtx *, rtx *);

/* Called from gimple_expand_cfg.  */
extern unsigned int emit_initial_value_sets (void);

/* In predict.c */
extern bool optimize_function_for_size_p (struct function *);
extern bool optimize_function_for_speed_p (struct function *);

#endif  /* GCC_FUNCTION_H */
