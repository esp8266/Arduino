/* Generated automatically by the program 'build/genpreds'
   from the machine description file '/home/igrokhotkov/xtensa/crosstool-NG/.build/src/gcc-4.8.2/gcc/config/xtensa/xtensa.md'.  */

#ifndef GCC_TM_PREDS_H
#define GCC_TM_PREDS_H

#ifdef HAVE_MACHINE_MODES
extern int general_operand (rtx, enum machine_mode);
extern int address_operand (rtx, enum machine_mode);
extern int register_operand (rtx, enum machine_mode);
extern int pmode_register_operand (rtx, enum machine_mode);
extern int scratch_operand (rtx, enum machine_mode);
extern int immediate_operand (rtx, enum machine_mode);
extern int const_int_operand (rtx, enum machine_mode);
extern int const_double_operand (rtx, enum machine_mode);
extern int nonimmediate_operand (rtx, enum machine_mode);
extern int nonmemory_operand (rtx, enum machine_mode);
extern int push_operand (rtx, enum machine_mode);
extern int pop_operand (rtx, enum machine_mode);
extern int memory_operand (rtx, enum machine_mode);
extern int indirect_operand (rtx, enum machine_mode);
extern int ordered_comparison_operator (rtx, enum machine_mode);
extern int comparison_operator (rtx, enum machine_mode);
extern int add_operand (rtx, enum machine_mode);
extern int addsubx_operand (rtx, enum machine_mode);
extern int arith_operand (rtx, enum machine_mode);
extern int nonimmed_operand (rtx, enum machine_mode);
extern int mem_operand (rtx, enum machine_mode);
extern int constantpool_operand (rtx, enum machine_mode);
extern int mask_operand (rtx, enum machine_mode);
extern int extui_fldsz_operand (rtx, enum machine_mode);
extern int sext_operand (rtx, enum machine_mode);
extern int sext_fldsz_operand (rtx, enum machine_mode);
extern int lsbitnum_operand (rtx, enum machine_mode);
extern int branch_operand (rtx, enum machine_mode);
extern int ubranch_operand (rtx, enum machine_mode);
extern int call_insn_operand (rtx, enum machine_mode);
extern int move_operand (rtx, enum machine_mode);
extern int const_float_1_operand (rtx, enum machine_mode);
extern int fpmem_offset_operand (rtx, enum machine_mode);
extern int branch_operator (rtx, enum machine_mode);
extern int ubranch_operator (rtx, enum machine_mode);
extern int boolean_operator (rtx, enum machine_mode);
extern int xtensa_cstoresi_operator (rtx, enum machine_mode);
extern int tls_symbol_operand (rtx, enum machine_mode);
#endif /* HAVE_MACHINE_MODES */

#define CONSTRAINT_NUM_DEFINED_P 1
enum constraint_num
{
  CONSTRAINT__UNKNOWN = 0,
  CONSTRAINT_a,
  CONSTRAINT_b,
  CONSTRAINT_d,
  CONSTRAINT_f,
  CONSTRAINT_q,
  CONSTRAINT_A,
  CONSTRAINT_B,
  CONSTRAINT_C,
  CONSTRAINT_D,
  CONSTRAINT_W,
  CONSTRAINT_I,
  CONSTRAINT_J,
  CONSTRAINT_K,
  CONSTRAINT_L,
  CONSTRAINT_M,
  CONSTRAINT_N,
  CONSTRAINT_O,
  CONSTRAINT_P,
  CONSTRAINT_R,
  CONSTRAINT_T,
  CONSTRAINT_U,
  CONSTRAINT__LIMIT
};

extern enum constraint_num lookup_constraint (const char *);
extern bool constraint_satisfied_p (rtx, enum constraint_num);

#define CONSTRAINT_LEN(c_,s_) 1

extern enum reg_class regclass_for_constraint (enum constraint_num);
#define REG_CLASS_FROM_CONSTRAINT(c_,s_) \
    regclass_for_constraint (lookup_constraint (s_))
#define REG_CLASS_FOR_CONSTRAINT(x_) \
    regclass_for_constraint (x_)

extern bool insn_const_int_ok_for_constraint (HOST_WIDE_INT, enum constraint_num);
#define CONST_OK_FOR_CONSTRAINT_P(v_,c_,s_) \
    insn_const_int_ok_for_constraint (v_, lookup_constraint (s_))

#define CONST_DOUBLE_OK_FOR_CONSTRAINT_P(v_,c_,s_) 0

#define EXTRA_CONSTRAINT_STR(v_,c_,s_) \
    constraint_satisfied_p (v_, lookup_constraint (s_))

#define EXTRA_MEMORY_CONSTRAINT(c_,s_) false

#define EXTRA_ADDRESS_CONSTRAINT(c_,s_) false

#endif /* tm-preds.h */
