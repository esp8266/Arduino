/* Exported functions from emit-rtl.c
   Copyright (C) 2004-2013 Free Software Foundation, Inc.

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

#ifndef GCC_EMIT_RTL_H
#define GCC_EMIT_RTL_H

/* Set the alias set of MEM to SET.  */
extern void set_mem_alias_set (rtx, alias_set_type);

/* Set the alignment of MEM to ALIGN bits.  */
extern void set_mem_align (rtx, unsigned int);

/* Set the address space of MEM to ADDRSPACE.  */
extern void set_mem_addr_space (rtx, addr_space_t);

/* Set the expr for MEM to EXPR.  */
extern void set_mem_expr (rtx, tree);

/* Set the offset for MEM to OFFSET.  */
extern void set_mem_offset (rtx, HOST_WIDE_INT);

/* Clear the offset recorded for MEM.  */
extern void clear_mem_offset (rtx);

/* Set the size for MEM to SIZE.  */
extern void set_mem_size (rtx, HOST_WIDE_INT);

/* Clear the size recorded for MEM.  */
extern void clear_mem_size (rtx);

/* Set the attributes for MEM appropriate for a spill slot.  */
extern void set_mem_attrs_for_spill (rtx);
extern tree get_spill_slot_decl (bool);

/* Return a memory reference like MEMREF, but with its address changed to
   ADDR.  The caller is asserting that the actual piece of memory pointed
   to is the same, just the form of the address is being changed, such as
   by putting something into a register.  */
extern rtx replace_equiv_address (rtx, rtx);

/* Likewise, but the reference is not required to be valid.  */
extern rtx replace_equiv_address_nv (rtx, rtx);

extern rtx gen_blockage (void);
extern rtvec gen_rtvec (int, ...);
extern rtx copy_insn_1 (rtx);
extern rtx copy_insn (rtx);
extern rtx copy_delay_slot_insn (rtx);
extern rtx gen_int_mode (HOST_WIDE_INT, enum machine_mode);
extern rtx emit_copy_of_insn_after (rtx, rtx);
extern void set_reg_attrs_from_value (rtx, rtx);
extern void set_reg_attrs_for_parm (rtx, rtx);
extern void set_reg_attrs_for_decl_rtl (tree t, rtx x);
extern void adjust_reg_mode (rtx, enum machine_mode);
extern int mem_expr_equal_p (const_tree, const_tree);

extern bool need_atomic_barrier_p (enum memmodel, bool);

/* Return the first insn of the current sequence or current function.  */

static inline rtx
get_insns (void)
{
  return crtl->emit.x_first_insn;
}

/* Specify a new insn as the first in the chain.  */

static inline void
set_first_insn (rtx insn)
{
  gcc_checking_assert (!insn || !PREV_INSN (insn));
  crtl->emit.x_first_insn = insn;
}

/* Return the last insn emitted in current sequence or current function.  */

static inline rtx
get_last_insn (void)
{
  return crtl->emit.x_last_insn;
}

/* Specify a new insn as the last in the chain.  */

static inline void
set_last_insn (rtx insn)
{
  gcc_checking_assert (!insn || !NEXT_INSN (insn));
  crtl->emit.x_last_insn = insn;
}

/* Return a number larger than any instruction's uid in this function.  */

static inline int
get_max_uid (void)
{
  return crtl->emit.x_cur_insn_uid;
}
#endif /* GCC_EMIT_RTL_H */
