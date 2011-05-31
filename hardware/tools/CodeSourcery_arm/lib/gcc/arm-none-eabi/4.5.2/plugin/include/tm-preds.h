/* Generated automatically by the program 'build/genpreds'
   from the machine description file '/scratch/janisjo/arm-eabi-lite/src/gcc-4.5-2011.03/gcc/config/arm/arm.md'.  */

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
extern int s_register_operand (rtx, enum machine_mode);
extern int arm_hard_register_operand (rtx, enum machine_mode);
extern int low_register_operand (rtx, enum machine_mode);
extern int low_reg_or_int_operand (rtx, enum machine_mode);
extern int arm_general_register_operand (rtx, enum machine_mode);
extern int f_register_operand (rtx, enum machine_mode);
extern int vfp_register_operand (rtx, enum machine_mode);
extern int subreg_lowpart_operator (rtx, enum machine_mode);
extern int reg_or_int_operand (rtx, enum machine_mode);
extern int arm_immediate_operand (rtx, enum machine_mode);
extern int arm_immediate_di_operand (rtx, enum machine_mode);
extern int arm_neg_immediate_operand (rtx, enum machine_mode);
extern int arm_not_immediate_operand (rtx, enum machine_mode);
extern int const0_operand (rtx, enum machine_mode);
extern int arm_rhs_operand (rtx, enum machine_mode);
extern int arm_rhsm_operand (rtx, enum machine_mode);
extern int arm_add_operand (rtx, enum machine_mode);
extern int arm_addimm_operand (rtx, enum machine_mode);
extern int arm_not_operand (rtx, enum machine_mode);
extern int arm_di_operand (rtx, enum machine_mode);
extern int offsettable_memory_operand (rtx, enum machine_mode);
extern int call_memory_operand (rtx, enum machine_mode);
extern int arm_reload_memory_operand (rtx, enum machine_mode);
extern int arm_float_rhs_operand (rtx, enum machine_mode);
extern int arm_float_add_operand (rtx, enum machine_mode);
extern int vfp_compare_operand (rtx, enum machine_mode);
extern int arm_float_compare_operand (rtx, enum machine_mode);
extern int index_operand (rtx, enum machine_mode);
extern int shiftable_operator (rtx, enum machine_mode);
extern int plusminus_operator (rtx, enum machine_mode);
extern int logical_binary_operator (rtx, enum machine_mode);
extern int shift_operator (rtx, enum machine_mode);
extern int sat_shift_operator (rtx, enum machine_mode);
extern int thumb_16bit_operator (rtx, enum machine_mode);
extern int equality_operator (rtx, enum machine_mode);
extern int arm_comparison_operator (rtx, enum machine_mode);
extern int lt_ge_comparison_operator (rtx, enum machine_mode);
extern int minmax_operator (rtx, enum machine_mode);
extern int cc_register (rtx, enum machine_mode);
extern int dominant_cc_register (rtx, enum machine_mode);
extern int arm_extendqisi_mem_op (rtx, enum machine_mode);
extern int arm_reg_or_extendqisi_mem_op (rtx, enum machine_mode);
extern int power_of_two_operand (rtx, enum machine_mode);
extern int nonimmediate_di_operand (rtx, enum machine_mode);
extern int di_operand (rtx, enum machine_mode);
extern int nonimmediate_soft_df_operand (rtx, enum machine_mode);
extern int soft_df_operand (rtx, enum machine_mode);
extern int const_shift_count (rtx, enum machine_mode);
extern int load_multiple_operation (rtx, enum machine_mode);
extern int store_multiple_operation (rtx, enum machine_mode);
extern int multi_register_push (rtx, enum machine_mode);
extern int thumb1_cmp_operand (rtx, enum machine_mode);
extern int thumb1_cmpneg_operand (rtx, enum machine_mode);
extern int thumb_cbrch_target_operand (rtx, enum machine_mode);
extern int cirrus_register_operand (rtx, enum machine_mode);
extern int cirrus_fp_register (rtx, enum machine_mode);
extern int cirrus_shift_const (rtx, enum machine_mode);
extern int const_multiple_of_8_operand (rtx, enum machine_mode);
extern int imm_for_neon_mov_operand (rtx, enum machine_mode);
extern int imm_for_neon_logic_operand (rtx, enum machine_mode);
extern int imm_for_neon_inv_logic_operand (rtx, enum machine_mode);
extern int neon_logic_op2 (rtx, enum machine_mode);
extern int neon_inv_logic_op2 (rtx, enum machine_mode);
extern int neon_lane_number (rtx, enum machine_mode);
extern int cmpdi_operand (rtx, enum machine_mode);
extern int arm_sync_memory_operand (rtx, enum machine_mode);
extern int vect_par_constant_high (rtx, enum machine_mode);
extern int vect_par_constant_low (rtx, enum machine_mode);
#endif /* HAVE_MACHINE_MODES */

#define CONSTRAINT_NUM_DEFINED_P 1
enum constraint_num
{
  CONSTRAINT__UNKNOWN = 0,
  CONSTRAINT_f,
  CONSTRAINT_t,
  CONSTRAINT_v,
  CONSTRAINT_w,
  CONSTRAINT_x,
  CONSTRAINT_y,
  CONSTRAINT_z,
  CONSTRAINT_l,
  CONSTRAINT_h,
  CONSTRAINT_j,
  CONSTRAINT_k,
  CONSTRAINT_b,
  CONSTRAINT_c,
  CONSTRAINT_I,
  CONSTRAINT_J,
  CONSTRAINT_K,
  CONSTRAINT_L,
  CONSTRAINT_M,
  CONSTRAINT_N,
  CONSTRAINT_O,
  CONSTRAINT_Pa,
  CONSTRAINT_Pb,
  CONSTRAINT_Ps,
  CONSTRAINT_Pt,
  CONSTRAINT_Pv,
  CONSTRAINT_G,
  CONSTRAINT_H,
  CONSTRAINT_D0,
  CONSTRAINT_Dz,
  CONSTRAINT_Da,
  CONSTRAINT_Db,
  CONSTRAINT_Dc,
  CONSTRAINT_Di,
  CONSTRAINT_Dn,
  CONSTRAINT_Dl,
  CONSTRAINT_DL,
  CONSTRAINT_Dv,
  CONSTRAINT_Dy,
  CONSTRAINT_Ut,
  CONSTRAINT_Uv,
  CONSTRAINT_Uy,
  CONSTRAINT_Un,
  CONSTRAINT_Um,
  CONSTRAINT_Us,
  CONSTRAINT_Uq,
  CONSTRAINT_Q,
  CONSTRAINT__LIMIT
};

extern enum constraint_num lookup_constraint (const char *);
extern bool constraint_satisfied_p (rtx, enum constraint_num);

static inline size_t
insn_constraint_len (char fc, const char *str ATTRIBUTE_UNUSED)
{
  switch (fc)
    {
    case 'D': return 2;
    case 'P': return 2;
    case 'U': return 2;
    default: break;
    }
  return 1;
}

#define CONSTRAINT_LEN(c_,s_) insn_constraint_len (c_,s_)

extern enum reg_class regclass_for_constraint (enum constraint_num);
#define REG_CLASS_FROM_CONSTRAINT(c_,s_) \
    regclass_for_constraint (lookup_constraint (s_))
#define REG_CLASS_FOR_CONSTRAINT(x_) \
    regclass_for_constraint (x_)

extern bool insn_const_int_ok_for_constraint (HOST_WIDE_INT, enum constraint_num);
#define CONST_OK_FOR_CONSTRAINT_P(v_,c_,s_) \
    insn_const_int_ok_for_constraint (v_, lookup_constraint (s_))

#define CONST_DOUBLE_OK_FOR_CONSTRAINT_P(v_,c_,s_) \
    constraint_satisfied_p (v_, lookup_constraint (s_))

#define EXTRA_CONSTRAINT_STR(v_,c_,s_) \
    constraint_satisfied_p (v_, lookup_constraint (s_))

extern bool insn_extra_memory_constraint (enum constraint_num);
#define EXTRA_MEMORY_CONSTRAINT(c_,s_) insn_extra_memory_constraint (lookup_constraint (s_))

#define EXTRA_ADDRESS_CONSTRAINT(c_,s_) false

#endif /* tm-preds.h */
