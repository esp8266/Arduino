/* Generated automatically by the program `genflags'
   from the machine description file `md'.  */

#ifndef GCC_INSN_FLAGS_H
#define GCC_INSN_FLAGS_H

#define HAVE_mulhisi3 (TARGET_DSP_MULTIPLY)
#define HAVE_maddhisi4 (TARGET_DSP_MULTIPLY)
#define HAVE_insv_zero (arm_arch_thumb2)
#define HAVE_insv_t2 (arm_arch_thumb2)
#define HAVE_andsi_notsi_si (TARGET_32BIT)
#define HAVE_bicsi3 (TARGET_THUMB1)
#define HAVE_andsi_not_shiftsi_si (TARGET_ARM)
#define HAVE_arm_ashldi3_1bit (TARGET_32BIT)
#define HAVE_arm_ashrdi3_1bit (TARGET_32BIT)
#define HAVE_arm_lshrdi3_1bit (TARGET_32BIT)
#define HAVE_unaligned_loadsi (unaligned_access)
#define HAVE_unaligned_loadhis (unaligned_access)
#define HAVE_unaligned_loadhiu (unaligned_access)
#define HAVE_unaligned_storesi (unaligned_access)
#define HAVE_unaligned_storehi (unaligned_access)
#define HAVE_extzv_t2 (arm_arch_thumb2)
#define HAVE_divsi3 (TARGET_IDIV)
#define HAVE_udivsi3 (TARGET_IDIV)
#define HAVE_one_cmpldi2 (TARGET_32BIT)
#define HAVE_thumb1_extendhisi2 (TARGET_THUMB1)
#define HAVE_thumb1_extendqisi2 (TARGET_THUMB1)
#define HAVE_pic_load_addr_32bit (TARGET_32BIT && flag_pic)
#define HAVE_pic_load_addr_thumb1 (TARGET_THUMB1 && flag_pic)
#define HAVE_pic_add_dot_plus_four (TARGET_THUMB)
#define HAVE_pic_add_dot_plus_eight (TARGET_ARM)
#define HAVE_tls_load_dot_plus_eight (TARGET_ARM)
#define HAVE_movmem12b (TARGET_THUMB1)
#define HAVE_movmem8b (TARGET_THUMB1)
#define HAVE_cbranchsi4_insn (TARGET_THUMB1)
#define HAVE_cbranchsi4_scratch (TARGET_THUMB1)
#define HAVE_cstoresi_nltu_thumb1 (TARGET_THUMB1)
#define HAVE_cstoresi_ltu_thumb1 (TARGET_THUMB1)
#define HAVE_thumb1_addsi3_addgeu (TARGET_THUMB1)
#define HAVE_blockage 1
#define HAVE_arm_casesi_internal (TARGET_ARM)
#define HAVE_thumb1_casesi_dispatch (TARGET_THUMB1)
#define HAVE_nop 1
#define HAVE_movcond (TARGET_ARM)
#define HAVE_sibcall_epilogue (TARGET_32BIT)
#define HAVE_stack_tie 1
#define HAVE_align_4 1
#define HAVE_align_8 1
#define HAVE_align_16 1
#define HAVE_align_32 1
#define HAVE_consttable_end 1
#define HAVE_consttable_1 (TARGET_THUMB1)
#define HAVE_consttable_2 (TARGET_THUMB1)
#define HAVE_consttable_4 1
#define HAVE_consttable_8 1
#define HAVE_consttable_16 1
#define HAVE_clzsi2 (TARGET_32BIT && arm_arch5)
#define HAVE_rbitsi2 (TARGET_32BIT && arm_arch_thumb2)
#define HAVE_prefetch (TARGET_32BIT && arm_arch5e)
#define HAVE_prologue_use 1
#define HAVE_arm_eh_return (TARGET_ARM)
#define HAVE_thumb_eh_return (TARGET_THUMB1)
#define HAVE_load_tp_hard (TARGET_HARD_TP)
#define HAVE_load_tp_soft (TARGET_SOFT_TP)
#define HAVE_tlscall (TARGET_GNU_TLS)
#define HAVE_cirrus_adddi3 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_cirrus_subdi3 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_muldi3 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_ashldi3_cirrus (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_cirrus_ashldi_const (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_cirrus_ashiftrtdi_const (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_cirrus_floatsisf2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_cirrus_floatsidf2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_floatdisf2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_floatdidf2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_cirrus_truncsfsi2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_cirrus_truncdfsi2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_MAVERICK)
#define HAVE_iwmmxt_iordi3 (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_xordi3 (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_anddi3 (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_nanddi3 (TARGET_REALLY_IWMMXT)
#define HAVE_movv2si_internal (TARGET_REALLY_IWMMXT)
#define HAVE_movv4hi_internal (TARGET_REALLY_IWMMXT)
#define HAVE_movv8qi_internal (TARGET_REALLY_IWMMXT)
#define HAVE_ssaddv8qi3 (TARGET_REALLY_IWMMXT)
#define HAVE_ssaddv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_ssaddv2si3 (TARGET_REALLY_IWMMXT)
#define HAVE_usaddv8qi3 (TARGET_REALLY_IWMMXT)
#define HAVE_usaddv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_usaddv2si3 (TARGET_REALLY_IWMMXT)
#define HAVE_sssubv8qi3 (TARGET_REALLY_IWMMXT)
#define HAVE_sssubv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_sssubv2si3 (TARGET_REALLY_IWMMXT)
#define HAVE_ussubv8qi3 (TARGET_REALLY_IWMMXT)
#define HAVE_ussubv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_ussubv2si3 (TARGET_REALLY_IWMMXT)
#define HAVE_smulv4hi3_highpart (TARGET_REALLY_IWMMXT)
#define HAVE_umulv4hi3_highpart (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wmacs (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wmacsz (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wmacu (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wmacuz (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_clrdi (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_uavgrndv8qi3 (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_uavgrndv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_uavgv8qi3 (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_uavgv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_psadbw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tinsrb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tinsrh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tinsrw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_textrmub (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_textrmsb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_textrmuh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_textrmsh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_textrmw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wshufh (TARGET_REALLY_IWMMXT)
#define HAVE_eqv8qi3 (TARGET_REALLY_IWMMXT)
#define HAVE_eqv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_eqv2si3 (TARGET_REALLY_IWMMXT)
#define HAVE_gtuv8qi3 (TARGET_REALLY_IWMMXT)
#define HAVE_gtuv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_gtuv2si3 (TARGET_REALLY_IWMMXT)
#define HAVE_gtv8qi3 (TARGET_REALLY_IWMMXT)
#define HAVE_gtv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_gtv2si3 (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wpackhss (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wpackwss (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wpackdss (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wpackhus (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wpackwus (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wpackdus (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckihb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckihh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckihw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckilb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckilh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckilw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckehub (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckehuh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckehuw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckehsb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckehsh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckehsw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckelub (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckeluh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckeluw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckelsb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckelsh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wunpckelsw (TARGET_REALLY_IWMMXT)
#define HAVE_rorv4hi3 (TARGET_REALLY_IWMMXT)
#define HAVE_rorv2si3 (TARGET_REALLY_IWMMXT)
#define HAVE_rordi3 (TARGET_REALLY_IWMMXT)
#define HAVE_ashrv4hi3_iwmmxt (TARGET_REALLY_IWMMXT)
#define HAVE_ashrv2si3_iwmmxt (TARGET_REALLY_IWMMXT)
#define HAVE_ashrdi3_iwmmxt (TARGET_REALLY_IWMMXT)
#define HAVE_lshrv4hi3_iwmmxt (TARGET_REALLY_IWMMXT)
#define HAVE_lshrv2si3_iwmmxt (TARGET_REALLY_IWMMXT)
#define HAVE_lshrdi3_iwmmxt (TARGET_REALLY_IWMMXT)
#define HAVE_ashlv4hi3_iwmmxt (TARGET_REALLY_IWMMXT)
#define HAVE_ashlv2si3_iwmmxt (TARGET_REALLY_IWMMXT)
#define HAVE_ashldi3_iwmmxt (TARGET_REALLY_IWMMXT)
#define HAVE_rorv4hi3_di (TARGET_REALLY_IWMMXT)
#define HAVE_rorv2si3_di (TARGET_REALLY_IWMMXT)
#define HAVE_rordi3_di (TARGET_REALLY_IWMMXT)
#define HAVE_ashrv4hi3_di (TARGET_REALLY_IWMMXT)
#define HAVE_ashrv2si3_di (TARGET_REALLY_IWMMXT)
#define HAVE_ashrdi3_di (TARGET_REALLY_IWMMXT)
#define HAVE_lshrv4hi3_di (TARGET_REALLY_IWMMXT)
#define HAVE_lshrv2si3_di (TARGET_REALLY_IWMMXT)
#define HAVE_lshrdi3_di (TARGET_REALLY_IWMMXT)
#define HAVE_ashlv4hi3_di (TARGET_REALLY_IWMMXT)
#define HAVE_ashlv2si3_di (TARGET_REALLY_IWMMXT)
#define HAVE_ashldi3_di (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wmadds (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wmaddu (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmia (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmiaph (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmiabb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmiatb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmiabt (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmiatt (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tbcstqi (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tbcsthi (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tbcstsi (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmovmskb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmovmskh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmovmskw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_waccb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wacch (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_waccw (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_walign (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmrc (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_tmcr (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wsadb (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wsadh (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wsadbz (TARGET_REALLY_IWMMXT)
#define HAVE_iwmmxt_wsadhz (TARGET_REALLY_IWMMXT)
#define HAVE_extendhfsf2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_FP16)
#define HAVE_truncsfhf2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_FP16)
#define HAVE_fixuns_truncsfsi2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_VFP)
#define HAVE_fixuns_truncdfsi2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_VFP_DOUBLE)
#define HAVE_floatunssisf2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_VFP)
#define HAVE_floatunssidf2 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_VFP_DOUBLE)
#define HAVE_tls_load_dot_plus_four (TARGET_THUMB2)
#define HAVE_thumb2_casesi_internal (TARGET_THUMB2 && !flag_pic)
#define HAVE_thumb2_casesi_internal_pic (TARGET_THUMB2 && flag_pic)
#define HAVE_thumb2_eh_return (TARGET_THUMB2)
#define HAVE_orsi_notsi_si (TARGET_THUMB2)
#define HAVE_vec_setv8qi_internal (TARGET_NEON)
#define HAVE_vec_setv4hi_internal (TARGET_NEON)
#define HAVE_vec_setv2si_internal (TARGET_NEON)
#define HAVE_vec_setv2sf_internal (TARGET_NEON)
#define HAVE_vec_setv16qi_internal (TARGET_NEON)
#define HAVE_vec_setv8hi_internal (TARGET_NEON)
#define HAVE_vec_setv4si_internal (TARGET_NEON)
#define HAVE_vec_setv4sf_internal (TARGET_NEON)
#define HAVE_vec_setv2di_internal (TARGET_NEON)
#define HAVE_vec_extractv8qi (TARGET_NEON)
#define HAVE_vec_extractv4hi (TARGET_NEON)
#define HAVE_vec_extractv2si (TARGET_NEON)
#define HAVE_vec_extractv2sf (TARGET_NEON)
#define HAVE_vec_extractv16qi (TARGET_NEON)
#define HAVE_vec_extractv8hi (TARGET_NEON)
#define HAVE_vec_extractv4si (TARGET_NEON)
#define HAVE_vec_extractv4sf (TARGET_NEON)
#define HAVE_vec_extractv2di (TARGET_NEON)
#define HAVE_adddi3_neon (TARGET_NEON)
#define HAVE_subdi3_neon (TARGET_NEON)
#define HAVE_mulv8qi3addv8qi_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv16qi3addv16qi_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv4hi3addv4hi_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv8hi3addv8hi_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv2si3addv2si_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv4si3addv4si_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv2sf3addv2sf_neon (TARGET_NEON && (!true || flag_unsafe_math_optimizations))
#define HAVE_mulv4sf3addv4sf_neon (TARGET_NEON && (!true || flag_unsafe_math_optimizations))
#define HAVE_mulv2di3addv2di_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv8qi3negv8qiaddv8qi_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv16qi3negv16qiaddv16qi_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv4hi3negv4hiaddv4hi_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv8hi3negv8hiaddv8hi_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv2si3negv2siaddv2si_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv4si3negv4siaddv4si_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_mulv2sf3negv2sfaddv2sf_neon (TARGET_NEON && (!true || flag_unsafe_math_optimizations))
#define HAVE_mulv4sf3negv4sfaddv4sf_neon (TARGET_NEON && (!true || flag_unsafe_math_optimizations))
#define HAVE_mulv2di3negv2diaddv2di_neon (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_iorv8qi3 (TARGET_NEON)
#define HAVE_iorv16qi3 (TARGET_NEON)
#define HAVE_iorv4hi3 (TARGET_NEON)
#define HAVE_iorv8hi3 (TARGET_NEON)
#define HAVE_iorv2si3 (TARGET_NEON)
#define HAVE_iorv4si3 (TARGET_NEON)
#define HAVE_iorv2sf3 (TARGET_NEON)
#define HAVE_iorv4sf3 (TARGET_NEON)
#define HAVE_iorv2di3 (TARGET_NEON)
#define HAVE_iordi3_neon (TARGET_NEON)
#define HAVE_andv8qi3 (TARGET_NEON)
#define HAVE_andv16qi3 (TARGET_NEON)
#define HAVE_andv4hi3 (TARGET_NEON)
#define HAVE_andv8hi3 (TARGET_NEON)
#define HAVE_andv2si3 (TARGET_NEON)
#define HAVE_andv4si3 (TARGET_NEON)
#define HAVE_andv2sf3 (TARGET_NEON)
#define HAVE_andv4sf3 (TARGET_NEON)
#define HAVE_andv2di3 (TARGET_NEON)
#define HAVE_anddi3_neon (TARGET_NEON)
#define HAVE_ornv8qi3_neon (TARGET_NEON)
#define HAVE_ornv16qi3_neon (TARGET_NEON)
#define HAVE_ornv4hi3_neon (TARGET_NEON)
#define HAVE_ornv8hi3_neon (TARGET_NEON)
#define HAVE_ornv2si3_neon (TARGET_NEON)
#define HAVE_ornv4si3_neon (TARGET_NEON)
#define HAVE_ornv2sf3_neon (TARGET_NEON)
#define HAVE_ornv4sf3_neon (TARGET_NEON)
#define HAVE_ornv2di3_neon (TARGET_NEON)
#define HAVE_orndi3_neon (TARGET_NEON)
#define HAVE_bicv8qi3_neon (TARGET_NEON)
#define HAVE_bicv16qi3_neon (TARGET_NEON)
#define HAVE_bicv4hi3_neon (TARGET_NEON)
#define HAVE_bicv8hi3_neon (TARGET_NEON)
#define HAVE_bicv2si3_neon (TARGET_NEON)
#define HAVE_bicv4si3_neon (TARGET_NEON)
#define HAVE_bicv2sf3_neon (TARGET_NEON)
#define HAVE_bicv4sf3_neon (TARGET_NEON)
#define HAVE_bicv2di3_neon (TARGET_NEON)
#define HAVE_bicdi3_neon (TARGET_NEON)
#define HAVE_xorv8qi3 (TARGET_NEON)
#define HAVE_xorv16qi3 (TARGET_NEON)
#define HAVE_xorv4hi3 (TARGET_NEON)
#define HAVE_xorv8hi3 (TARGET_NEON)
#define HAVE_xorv2si3 (TARGET_NEON)
#define HAVE_xorv4si3 (TARGET_NEON)
#define HAVE_xorv2sf3 (TARGET_NEON)
#define HAVE_xorv4sf3 (TARGET_NEON)
#define HAVE_xorv2di3 (TARGET_NEON)
#define HAVE_xordi3_neon (TARGET_NEON)
#define HAVE_one_cmplv8qi2 (TARGET_NEON)
#define HAVE_one_cmplv16qi2 (TARGET_NEON)
#define HAVE_one_cmplv4hi2 (TARGET_NEON)
#define HAVE_one_cmplv8hi2 (TARGET_NEON)
#define HAVE_one_cmplv2si2 (TARGET_NEON)
#define HAVE_one_cmplv4si2 (TARGET_NEON)
#define HAVE_one_cmplv2sf2 (TARGET_NEON)
#define HAVE_one_cmplv4sf2 (TARGET_NEON)
#define HAVE_one_cmplv2di2 (TARGET_NEON)
#define HAVE_absv8qi2 (TARGET_NEON)
#define HAVE_absv16qi2 (TARGET_NEON)
#define HAVE_absv4hi2 (TARGET_NEON)
#define HAVE_absv8hi2 (TARGET_NEON)
#define HAVE_absv2si2 (TARGET_NEON)
#define HAVE_absv4si2 (TARGET_NEON)
#define HAVE_absv2sf2 (TARGET_NEON)
#define HAVE_absv4sf2 (TARGET_NEON)
#define HAVE_negv8qi2 (TARGET_NEON)
#define HAVE_negv16qi2 (TARGET_NEON)
#define HAVE_negv4hi2 (TARGET_NEON)
#define HAVE_negv8hi2 (TARGET_NEON)
#define HAVE_negv2si2 (TARGET_NEON)
#define HAVE_negv4si2 (TARGET_NEON)
#define HAVE_negv2sf2 (TARGET_NEON)
#define HAVE_negv4sf2 (TARGET_NEON)
#define HAVE_vashlv8qi3 (TARGET_NEON)
#define HAVE_vashlv16qi3 (TARGET_NEON)
#define HAVE_vashlv4hi3 (TARGET_NEON)
#define HAVE_vashlv8hi3 (TARGET_NEON)
#define HAVE_vashlv2si3 (TARGET_NEON)
#define HAVE_vashlv4si3 (TARGET_NEON)
#define HAVE_ashlv8qi3_signed (TARGET_NEON)
#define HAVE_ashlv16qi3_signed (TARGET_NEON)
#define HAVE_ashlv4hi3_signed (TARGET_NEON)
#define HAVE_ashlv8hi3_signed (TARGET_NEON)
#define HAVE_ashlv2si3_signed (TARGET_NEON)
#define HAVE_ashlv4si3_signed (TARGET_NEON)
#define HAVE_ashlv2di3_signed (TARGET_NEON)
#define HAVE_ashlv8qi3_unsigned (TARGET_NEON)
#define HAVE_ashlv16qi3_unsigned (TARGET_NEON)
#define HAVE_ashlv4hi3_unsigned (TARGET_NEON)
#define HAVE_ashlv8hi3_unsigned (TARGET_NEON)
#define HAVE_ashlv2si3_unsigned (TARGET_NEON)
#define HAVE_ashlv4si3_unsigned (TARGET_NEON)
#define HAVE_ashlv2di3_unsigned (TARGET_NEON)
#define HAVE_widen_ssumv8qi3 (TARGET_NEON)
#define HAVE_widen_ssumv4hi3 (TARGET_NEON)
#define HAVE_widen_ssumv2si3 (TARGET_NEON)
#define HAVE_widen_usumv8qi3 (TARGET_NEON)
#define HAVE_widen_usumv4hi3 (TARGET_NEON)
#define HAVE_widen_usumv2si3 (TARGET_NEON)
#define HAVE_quad_halves_plusv4si (TARGET_NEON)
#define HAVE_quad_halves_sminv4si (TARGET_NEON)
#define HAVE_quad_halves_smaxv4si (TARGET_NEON)
#define HAVE_quad_halves_uminv4si (TARGET_NEON)
#define HAVE_quad_halves_umaxv4si (TARGET_NEON)
#define HAVE_quad_halves_plusv4sf (TARGET_NEON && flag_unsafe_math_optimizations)
#define HAVE_quad_halves_sminv4sf (TARGET_NEON && flag_unsafe_math_optimizations)
#define HAVE_quad_halves_smaxv4sf (TARGET_NEON && flag_unsafe_math_optimizations)
#define HAVE_quad_halves_plusv8hi (TARGET_NEON)
#define HAVE_quad_halves_sminv8hi (TARGET_NEON)
#define HAVE_quad_halves_smaxv8hi (TARGET_NEON)
#define HAVE_quad_halves_uminv8hi (TARGET_NEON)
#define HAVE_quad_halves_umaxv8hi (TARGET_NEON)
#define HAVE_quad_halves_plusv16qi (TARGET_NEON)
#define HAVE_quad_halves_sminv16qi (TARGET_NEON)
#define HAVE_quad_halves_smaxv16qi (TARGET_NEON)
#define HAVE_quad_halves_uminv16qi (TARGET_NEON)
#define HAVE_quad_halves_umaxv16qi (TARGET_NEON)
#define HAVE_neon_move_lo_quad_v2di (TARGET_NEON)
#define HAVE_neon_move_lo_quad_v2df (TARGET_NEON)
#define HAVE_neon_move_lo_quad_v16qi (TARGET_NEON)
#define HAVE_neon_move_lo_quad_v8hi (TARGET_NEON)
#define HAVE_neon_move_lo_quad_v4si (TARGET_NEON)
#define HAVE_neon_move_lo_quad_v4sf (TARGET_NEON)
#define HAVE_neon_move_hi_quad_v2di (TARGET_NEON)
#define HAVE_neon_move_hi_quad_v2df (TARGET_NEON)
#define HAVE_neon_move_hi_quad_v16qi (TARGET_NEON)
#define HAVE_neon_move_hi_quad_v8hi (TARGET_NEON)
#define HAVE_neon_move_hi_quad_v4si (TARGET_NEON)
#define HAVE_neon_move_hi_quad_v4sf (TARGET_NEON)
#define HAVE_reduc_splus_v2di (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vpadd_internalv8qi (TARGET_NEON)
#define HAVE_neon_vpadd_internalv4hi (TARGET_NEON)
#define HAVE_neon_vpadd_internalv2si (TARGET_NEON)
#define HAVE_neon_vpadd_internalv2sf (TARGET_NEON)
#define HAVE_neon_vpsminv8qi (TARGET_NEON)
#define HAVE_neon_vpsminv4hi (TARGET_NEON)
#define HAVE_neon_vpsminv2si (TARGET_NEON)
#define HAVE_neon_vpsminv2sf (TARGET_NEON)
#define HAVE_neon_vpsmaxv8qi (TARGET_NEON)
#define HAVE_neon_vpsmaxv4hi (TARGET_NEON)
#define HAVE_neon_vpsmaxv2si (TARGET_NEON)
#define HAVE_neon_vpsmaxv2sf (TARGET_NEON)
#define HAVE_neon_vpuminv8qi (TARGET_NEON)
#define HAVE_neon_vpuminv4hi (TARGET_NEON)
#define HAVE_neon_vpuminv2si (TARGET_NEON)
#define HAVE_neon_vpumaxv8qi (TARGET_NEON)
#define HAVE_neon_vpumaxv4hi (TARGET_NEON)
#define HAVE_neon_vpumaxv2si (TARGET_NEON)
#define HAVE_neon_vaddv8qi_unspec (TARGET_NEON)
#define HAVE_neon_vaddv16qi_unspec (TARGET_NEON)
#define HAVE_neon_vaddv4hi_unspec (TARGET_NEON)
#define HAVE_neon_vaddv8hi_unspec (TARGET_NEON)
#define HAVE_neon_vaddv2si_unspec (TARGET_NEON)
#define HAVE_neon_vaddv4si_unspec (TARGET_NEON)
#define HAVE_neon_vaddv2sf_unspec (TARGET_NEON)
#define HAVE_neon_vaddv4sf_unspec (TARGET_NEON)
#define HAVE_neon_vadddi_unspec (TARGET_NEON)
#define HAVE_neon_vaddv2di_unspec (TARGET_NEON)
#define HAVE_neon_vaddlv8qi (TARGET_NEON)
#define HAVE_neon_vaddlv4hi (TARGET_NEON)
#define HAVE_neon_vaddlv2si (TARGET_NEON)
#define HAVE_neon_vaddwv8qi (TARGET_NEON)
#define HAVE_neon_vaddwv4hi (TARGET_NEON)
#define HAVE_neon_vaddwv2si (TARGET_NEON)
#define HAVE_neon_vhaddv8qi (TARGET_NEON)
#define HAVE_neon_vhaddv16qi (TARGET_NEON)
#define HAVE_neon_vhaddv4hi (TARGET_NEON)
#define HAVE_neon_vhaddv8hi (TARGET_NEON)
#define HAVE_neon_vhaddv2si (TARGET_NEON)
#define HAVE_neon_vhaddv4si (TARGET_NEON)
#define HAVE_neon_vqaddv8qi (TARGET_NEON)
#define HAVE_neon_vqaddv16qi (TARGET_NEON)
#define HAVE_neon_vqaddv4hi (TARGET_NEON)
#define HAVE_neon_vqaddv8hi (TARGET_NEON)
#define HAVE_neon_vqaddv2si (TARGET_NEON)
#define HAVE_neon_vqaddv4si (TARGET_NEON)
#define HAVE_neon_vqadddi (TARGET_NEON)
#define HAVE_neon_vqaddv2di (TARGET_NEON)
#define HAVE_neon_vaddhnv8hi (TARGET_NEON)
#define HAVE_neon_vaddhnv4si (TARGET_NEON)
#define HAVE_neon_vaddhnv2di (TARGET_NEON)
#define HAVE_neon_vmulv8qi (TARGET_NEON)
#define HAVE_neon_vmulv16qi (TARGET_NEON)
#define HAVE_neon_vmulv4hi (TARGET_NEON)
#define HAVE_neon_vmulv8hi (TARGET_NEON)
#define HAVE_neon_vmulv2si (TARGET_NEON)
#define HAVE_neon_vmulv4si (TARGET_NEON)
#define HAVE_neon_vmulv2sf (TARGET_NEON)
#define HAVE_neon_vmulv4sf (TARGET_NEON)
#define HAVE_neon_vmlav8qi_unspec (TARGET_NEON)
#define HAVE_neon_vmlav16qi_unspec (TARGET_NEON)
#define HAVE_neon_vmlav4hi_unspec (TARGET_NEON)
#define HAVE_neon_vmlav8hi_unspec (TARGET_NEON)
#define HAVE_neon_vmlav2si_unspec (TARGET_NEON)
#define HAVE_neon_vmlav4si_unspec (TARGET_NEON)
#define HAVE_neon_vmlav2sf_unspec (TARGET_NEON)
#define HAVE_neon_vmlav4sf_unspec (TARGET_NEON)
#define HAVE_neon_vmlav2di_unspec (TARGET_NEON)
#define HAVE_neon_vmlalv8qi (TARGET_NEON)
#define HAVE_neon_vmlalv4hi (TARGET_NEON)
#define HAVE_neon_vmlalv2si (TARGET_NEON)
#define HAVE_neon_vmlsv8qi_unspec (TARGET_NEON)
#define HAVE_neon_vmlsv16qi_unspec (TARGET_NEON)
#define HAVE_neon_vmlsv4hi_unspec (TARGET_NEON)
#define HAVE_neon_vmlsv8hi_unspec (TARGET_NEON)
#define HAVE_neon_vmlsv2si_unspec (TARGET_NEON)
#define HAVE_neon_vmlsv4si_unspec (TARGET_NEON)
#define HAVE_neon_vmlsv2sf_unspec (TARGET_NEON)
#define HAVE_neon_vmlsv4sf_unspec (TARGET_NEON)
#define HAVE_neon_vmlsv2di_unspec (TARGET_NEON)
#define HAVE_neon_vmlslv8qi (TARGET_NEON)
#define HAVE_neon_vmlslv4hi (TARGET_NEON)
#define HAVE_neon_vmlslv2si (TARGET_NEON)
#define HAVE_neon_vqdmulhv4hi (TARGET_NEON)
#define HAVE_neon_vqdmulhv2si (TARGET_NEON)
#define HAVE_neon_vqdmulhv8hi (TARGET_NEON)
#define HAVE_neon_vqdmulhv4si (TARGET_NEON)
#define HAVE_neon_vqdmlalv4hi (TARGET_NEON)
#define HAVE_neon_vqdmlalv2si (TARGET_NEON)
#define HAVE_neon_vqdmlslv4hi (TARGET_NEON)
#define HAVE_neon_vqdmlslv2si (TARGET_NEON)
#define HAVE_neon_vmullv8qi (TARGET_NEON)
#define HAVE_neon_vmullv4hi (TARGET_NEON)
#define HAVE_neon_vmullv2si (TARGET_NEON)
#define HAVE_neon_vqdmullv4hi (TARGET_NEON)
#define HAVE_neon_vqdmullv2si (TARGET_NEON)
#define HAVE_neon_vsubv8qi_unspec (TARGET_NEON)
#define HAVE_neon_vsubv16qi_unspec (TARGET_NEON)
#define HAVE_neon_vsubv4hi_unspec (TARGET_NEON)
#define HAVE_neon_vsubv8hi_unspec (TARGET_NEON)
#define HAVE_neon_vsubv2si_unspec (TARGET_NEON)
#define HAVE_neon_vsubv4si_unspec (TARGET_NEON)
#define HAVE_neon_vsubv2sf_unspec (TARGET_NEON)
#define HAVE_neon_vsubv4sf_unspec (TARGET_NEON)
#define HAVE_neon_vsubdi_unspec (TARGET_NEON)
#define HAVE_neon_vsubv2di_unspec (TARGET_NEON)
#define HAVE_neon_vsublv8qi (TARGET_NEON)
#define HAVE_neon_vsublv4hi (TARGET_NEON)
#define HAVE_neon_vsublv2si (TARGET_NEON)
#define HAVE_neon_vsubwv8qi (TARGET_NEON)
#define HAVE_neon_vsubwv4hi (TARGET_NEON)
#define HAVE_neon_vsubwv2si (TARGET_NEON)
#define HAVE_neon_vqsubv8qi (TARGET_NEON)
#define HAVE_neon_vqsubv16qi (TARGET_NEON)
#define HAVE_neon_vqsubv4hi (TARGET_NEON)
#define HAVE_neon_vqsubv8hi (TARGET_NEON)
#define HAVE_neon_vqsubv2si (TARGET_NEON)
#define HAVE_neon_vqsubv4si (TARGET_NEON)
#define HAVE_neon_vqsubdi (TARGET_NEON)
#define HAVE_neon_vqsubv2di (TARGET_NEON)
#define HAVE_neon_vhsubv8qi (TARGET_NEON)
#define HAVE_neon_vhsubv16qi (TARGET_NEON)
#define HAVE_neon_vhsubv4hi (TARGET_NEON)
#define HAVE_neon_vhsubv8hi (TARGET_NEON)
#define HAVE_neon_vhsubv2si (TARGET_NEON)
#define HAVE_neon_vhsubv4si (TARGET_NEON)
#define HAVE_neon_vsubhnv8hi (TARGET_NEON)
#define HAVE_neon_vsubhnv4si (TARGET_NEON)
#define HAVE_neon_vsubhnv2di (TARGET_NEON)
#define HAVE_neon_vceqv8qi (TARGET_NEON)
#define HAVE_neon_vceqv16qi (TARGET_NEON)
#define HAVE_neon_vceqv4hi (TARGET_NEON)
#define HAVE_neon_vceqv8hi (TARGET_NEON)
#define HAVE_neon_vceqv2si (TARGET_NEON)
#define HAVE_neon_vceqv4si (TARGET_NEON)
#define HAVE_neon_vceqv2sf (TARGET_NEON)
#define HAVE_neon_vceqv4sf (TARGET_NEON)
#define HAVE_neon_vcgev8qi (TARGET_NEON)
#define HAVE_neon_vcgev16qi (TARGET_NEON)
#define HAVE_neon_vcgev4hi (TARGET_NEON)
#define HAVE_neon_vcgev8hi (TARGET_NEON)
#define HAVE_neon_vcgev2si (TARGET_NEON)
#define HAVE_neon_vcgev4si (TARGET_NEON)
#define HAVE_neon_vcgev2sf (TARGET_NEON)
#define HAVE_neon_vcgev4sf (TARGET_NEON)
#define HAVE_neon_vcgtv8qi (TARGET_NEON)
#define HAVE_neon_vcgtv16qi (TARGET_NEON)
#define HAVE_neon_vcgtv4hi (TARGET_NEON)
#define HAVE_neon_vcgtv8hi (TARGET_NEON)
#define HAVE_neon_vcgtv2si (TARGET_NEON)
#define HAVE_neon_vcgtv4si (TARGET_NEON)
#define HAVE_neon_vcgtv2sf (TARGET_NEON)
#define HAVE_neon_vcgtv4sf (TARGET_NEON)
#define HAVE_neon_vclev8qi (TARGET_NEON)
#define HAVE_neon_vclev16qi (TARGET_NEON)
#define HAVE_neon_vclev4hi (TARGET_NEON)
#define HAVE_neon_vclev8hi (TARGET_NEON)
#define HAVE_neon_vclev2si (TARGET_NEON)
#define HAVE_neon_vclev4si (TARGET_NEON)
#define HAVE_neon_vclev2sf (TARGET_NEON)
#define HAVE_neon_vclev4sf (TARGET_NEON)
#define HAVE_neon_vcltv8qi (TARGET_NEON)
#define HAVE_neon_vcltv16qi (TARGET_NEON)
#define HAVE_neon_vcltv4hi (TARGET_NEON)
#define HAVE_neon_vcltv8hi (TARGET_NEON)
#define HAVE_neon_vcltv2si (TARGET_NEON)
#define HAVE_neon_vcltv4si (TARGET_NEON)
#define HAVE_neon_vcltv2sf (TARGET_NEON)
#define HAVE_neon_vcltv4sf (TARGET_NEON)
#define HAVE_neon_vcagev2sf (TARGET_NEON)
#define HAVE_neon_vcagev4sf (TARGET_NEON)
#define HAVE_neon_vcagtv2sf (TARGET_NEON)
#define HAVE_neon_vcagtv4sf (TARGET_NEON)
#define HAVE_neon_vtstv8qi (TARGET_NEON)
#define HAVE_neon_vtstv16qi (TARGET_NEON)
#define HAVE_neon_vtstv4hi (TARGET_NEON)
#define HAVE_neon_vtstv8hi (TARGET_NEON)
#define HAVE_neon_vtstv2si (TARGET_NEON)
#define HAVE_neon_vtstv4si (TARGET_NEON)
#define HAVE_neon_vabdv8qi (TARGET_NEON)
#define HAVE_neon_vabdv16qi (TARGET_NEON)
#define HAVE_neon_vabdv4hi (TARGET_NEON)
#define HAVE_neon_vabdv8hi (TARGET_NEON)
#define HAVE_neon_vabdv2si (TARGET_NEON)
#define HAVE_neon_vabdv4si (TARGET_NEON)
#define HAVE_neon_vabdv2sf (TARGET_NEON)
#define HAVE_neon_vabdv4sf (TARGET_NEON)
#define HAVE_neon_vabdlv8qi (TARGET_NEON)
#define HAVE_neon_vabdlv4hi (TARGET_NEON)
#define HAVE_neon_vabdlv2si (TARGET_NEON)
#define HAVE_neon_vabav8qi (TARGET_NEON)
#define HAVE_neon_vabav16qi (TARGET_NEON)
#define HAVE_neon_vabav4hi (TARGET_NEON)
#define HAVE_neon_vabav8hi (TARGET_NEON)
#define HAVE_neon_vabav2si (TARGET_NEON)
#define HAVE_neon_vabav4si (TARGET_NEON)
#define HAVE_neon_vabalv8qi (TARGET_NEON)
#define HAVE_neon_vabalv4hi (TARGET_NEON)
#define HAVE_neon_vabalv2si (TARGET_NEON)
#define HAVE_neon_vmaxv8qi (TARGET_NEON)
#define HAVE_neon_vmaxv16qi (TARGET_NEON)
#define HAVE_neon_vmaxv4hi (TARGET_NEON)
#define HAVE_neon_vmaxv8hi (TARGET_NEON)
#define HAVE_neon_vmaxv2si (TARGET_NEON)
#define HAVE_neon_vmaxv4si (TARGET_NEON)
#define HAVE_neon_vmaxv2sf (TARGET_NEON)
#define HAVE_neon_vmaxv4sf (TARGET_NEON)
#define HAVE_neon_vminv8qi (TARGET_NEON)
#define HAVE_neon_vminv16qi (TARGET_NEON)
#define HAVE_neon_vminv4hi (TARGET_NEON)
#define HAVE_neon_vminv8hi (TARGET_NEON)
#define HAVE_neon_vminv2si (TARGET_NEON)
#define HAVE_neon_vminv4si (TARGET_NEON)
#define HAVE_neon_vminv2sf (TARGET_NEON)
#define HAVE_neon_vminv4sf (TARGET_NEON)
#define HAVE_neon_vpaddlv8qi (TARGET_NEON)
#define HAVE_neon_vpaddlv16qi (TARGET_NEON)
#define HAVE_neon_vpaddlv4hi (TARGET_NEON)
#define HAVE_neon_vpaddlv8hi (TARGET_NEON)
#define HAVE_neon_vpaddlv2si (TARGET_NEON)
#define HAVE_neon_vpaddlv4si (TARGET_NEON)
#define HAVE_neon_vpadalv8qi (TARGET_NEON)
#define HAVE_neon_vpadalv16qi (TARGET_NEON)
#define HAVE_neon_vpadalv4hi (TARGET_NEON)
#define HAVE_neon_vpadalv8hi (TARGET_NEON)
#define HAVE_neon_vpadalv2si (TARGET_NEON)
#define HAVE_neon_vpadalv4si (TARGET_NEON)
#define HAVE_neon_vpmaxv8qi (TARGET_NEON)
#define HAVE_neon_vpmaxv4hi (TARGET_NEON)
#define HAVE_neon_vpmaxv2si (TARGET_NEON)
#define HAVE_neon_vpmaxv2sf (TARGET_NEON)
#define HAVE_neon_vpminv8qi (TARGET_NEON)
#define HAVE_neon_vpminv4hi (TARGET_NEON)
#define HAVE_neon_vpminv2si (TARGET_NEON)
#define HAVE_neon_vpminv2sf (TARGET_NEON)
#define HAVE_neon_vrecpsv2sf (TARGET_NEON)
#define HAVE_neon_vrecpsv4sf (TARGET_NEON)
#define HAVE_neon_vrsqrtsv2sf (TARGET_NEON)
#define HAVE_neon_vrsqrtsv4sf (TARGET_NEON)
#define HAVE_neon_vqabsv8qi (TARGET_NEON)
#define HAVE_neon_vqabsv16qi (TARGET_NEON)
#define HAVE_neon_vqabsv4hi (TARGET_NEON)
#define HAVE_neon_vqabsv8hi (TARGET_NEON)
#define HAVE_neon_vqabsv2si (TARGET_NEON)
#define HAVE_neon_vqabsv4si (TARGET_NEON)
#define HAVE_neon_vqnegv8qi (TARGET_NEON)
#define HAVE_neon_vqnegv16qi (TARGET_NEON)
#define HAVE_neon_vqnegv4hi (TARGET_NEON)
#define HAVE_neon_vqnegv8hi (TARGET_NEON)
#define HAVE_neon_vqnegv2si (TARGET_NEON)
#define HAVE_neon_vqnegv4si (TARGET_NEON)
#define HAVE_neon_vclsv8qi (TARGET_NEON)
#define HAVE_neon_vclsv16qi (TARGET_NEON)
#define HAVE_neon_vclsv4hi (TARGET_NEON)
#define HAVE_neon_vclsv8hi (TARGET_NEON)
#define HAVE_neon_vclsv2si (TARGET_NEON)
#define HAVE_neon_vclsv4si (TARGET_NEON)
#define HAVE_clzv8qi2 (TARGET_NEON)
#define HAVE_clzv16qi2 (TARGET_NEON)
#define HAVE_clzv4hi2 (TARGET_NEON)
#define HAVE_clzv8hi2 (TARGET_NEON)
#define HAVE_clzv2si2 (TARGET_NEON)
#define HAVE_clzv4si2 (TARGET_NEON)
#define HAVE_popcountv8qi2 (TARGET_NEON)
#define HAVE_popcountv16qi2 (TARGET_NEON)
#define HAVE_neon_vrecpev2si (TARGET_NEON)
#define HAVE_neon_vrecpev2sf (TARGET_NEON)
#define HAVE_neon_vrecpev4si (TARGET_NEON)
#define HAVE_neon_vrecpev4sf (TARGET_NEON)
#define HAVE_neon_vrsqrtev2si (TARGET_NEON)
#define HAVE_neon_vrsqrtev2sf (TARGET_NEON)
#define HAVE_neon_vrsqrtev4si (TARGET_NEON)
#define HAVE_neon_vrsqrtev4sf (TARGET_NEON)
#define HAVE_neon_vget_lanev8qi_sext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev4hi_sext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev2si_sext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev2sf_sext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev8qi_zext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev4hi_zext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev2si_zext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev2sf_zext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev16qi_sext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev8hi_sext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev4si_sext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev4sf_sext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev16qi_zext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev8hi_zext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev4si_zext_internal (TARGET_NEON)
#define HAVE_neon_vget_lanev4sf_zext_internal (TARGET_NEON)
#define HAVE_neon_vdup_nv8qi (TARGET_NEON)
#define HAVE_neon_vdup_nv4hi (TARGET_NEON)
#define HAVE_neon_vdup_nv16qi (TARGET_NEON)
#define HAVE_neon_vdup_nv8hi (TARGET_NEON)
#define HAVE_neon_vdup_nv2si (TARGET_NEON)
#define HAVE_neon_vdup_nv2sf (TARGET_NEON)
#define HAVE_neon_vdup_nv4si (TARGET_NEON)
#define HAVE_neon_vdup_nv4sf (TARGET_NEON)
#define HAVE_neon_vdup_nv2di (TARGET_NEON)
#define HAVE_neon_vdup_lanev8qi_internal (TARGET_NEON)
#define HAVE_neon_vdup_lanev16qi_internal (TARGET_NEON)
#define HAVE_neon_vdup_lanev4hi_internal (TARGET_NEON)
#define HAVE_neon_vdup_lanev8hi_internal (TARGET_NEON)
#define HAVE_neon_vdup_lanev2si_internal (TARGET_NEON)
#define HAVE_neon_vdup_lanev4si_internal (TARGET_NEON)
#define HAVE_neon_vdup_lanev2sf_internal (TARGET_NEON)
#define HAVE_neon_vdup_lanev4sf_internal (TARGET_NEON)
#define HAVE_neon_vcombinev8qi (TARGET_NEON)
#define HAVE_neon_vcombinev4hi (TARGET_NEON)
#define HAVE_neon_vcombinev2si (TARGET_NEON)
#define HAVE_neon_vcombinev2sf (TARGET_NEON)
#define HAVE_neon_vcombinedi (TARGET_NEON)
#define HAVE_neon_vget_highv16qi (TARGET_NEON)
#define HAVE_neon_vget_highv8hi (TARGET_NEON)
#define HAVE_neon_vget_highv4si (TARGET_NEON)
#define HAVE_neon_vget_highv4sf (TARGET_NEON)
#define HAVE_neon_vget_highv2di (TARGET_NEON)
#define HAVE_neon_vget_lowv16qi (TARGET_NEON)
#define HAVE_neon_vget_lowv8hi (TARGET_NEON)
#define HAVE_neon_vget_lowv4si (TARGET_NEON)
#define HAVE_neon_vget_lowv4sf (TARGET_NEON)
#define HAVE_neon_vget_lowv2di (TARGET_NEON)
#define HAVE_neon_vcvtv2sf (TARGET_NEON)
#define HAVE_neon_vcvtv4sf (TARGET_NEON)
#define HAVE_neon_vcvtv2si (TARGET_NEON)
#define HAVE_neon_vcvtv4si (TARGET_NEON)
#define HAVE_neon_vcvt_nv2sf (TARGET_NEON)
#define HAVE_neon_vcvt_nv4sf (TARGET_NEON)
#define HAVE_neon_vcvt_nv2si (TARGET_NEON)
#define HAVE_neon_vcvt_nv4si (TARGET_NEON)
#define HAVE_neon_vmovnv8hi (TARGET_NEON)
#define HAVE_neon_vmovnv4si (TARGET_NEON)
#define HAVE_neon_vmovnv2di (TARGET_NEON)
#define HAVE_neon_vqmovnv8hi (TARGET_NEON)
#define HAVE_neon_vqmovnv4si (TARGET_NEON)
#define HAVE_neon_vqmovnv2di (TARGET_NEON)
#define HAVE_neon_vqmovunv8hi (TARGET_NEON)
#define HAVE_neon_vqmovunv4si (TARGET_NEON)
#define HAVE_neon_vqmovunv2di (TARGET_NEON)
#define HAVE_neon_vmovlv8qi (TARGET_NEON)
#define HAVE_neon_vmovlv4hi (TARGET_NEON)
#define HAVE_neon_vmovlv2si (TARGET_NEON)
#define HAVE_neon_vmul_lanev4hi (TARGET_NEON)
#define HAVE_neon_vmul_lanev2si (TARGET_NEON)
#define HAVE_neon_vmul_lanev2sf (TARGET_NEON)
#define HAVE_neon_vmul_lanev8hi (TARGET_NEON)
#define HAVE_neon_vmul_lanev4si (TARGET_NEON)
#define HAVE_neon_vmul_lanev4sf (TARGET_NEON)
#define HAVE_neon_vmull_lanev4hi (TARGET_NEON)
#define HAVE_neon_vmull_lanev2si (TARGET_NEON)
#define HAVE_neon_vqdmull_lanev4hi (TARGET_NEON)
#define HAVE_neon_vqdmull_lanev2si (TARGET_NEON)
#define HAVE_neon_vqdmulh_lanev8hi (TARGET_NEON)
#define HAVE_neon_vqdmulh_lanev4si (TARGET_NEON)
#define HAVE_neon_vqdmulh_lanev4hi (TARGET_NEON)
#define HAVE_neon_vqdmulh_lanev2si (TARGET_NEON)
#define HAVE_neon_vmla_lanev4hi (TARGET_NEON)
#define HAVE_neon_vmla_lanev2si (TARGET_NEON)
#define HAVE_neon_vmla_lanev2sf (TARGET_NEON)
#define HAVE_neon_vmla_lanev8hi (TARGET_NEON)
#define HAVE_neon_vmla_lanev4si (TARGET_NEON)
#define HAVE_neon_vmla_lanev4sf (TARGET_NEON)
#define HAVE_neon_vmlal_lanev4hi (TARGET_NEON)
#define HAVE_neon_vmlal_lanev2si (TARGET_NEON)
#define HAVE_neon_vqdmlal_lanev4hi (TARGET_NEON)
#define HAVE_neon_vqdmlal_lanev2si (TARGET_NEON)
#define HAVE_neon_vmls_lanev4hi (TARGET_NEON)
#define HAVE_neon_vmls_lanev2si (TARGET_NEON)
#define HAVE_neon_vmls_lanev2sf (TARGET_NEON)
#define HAVE_neon_vmls_lanev8hi (TARGET_NEON)
#define HAVE_neon_vmls_lanev4si (TARGET_NEON)
#define HAVE_neon_vmls_lanev4sf (TARGET_NEON)
#define HAVE_neon_vmlsl_lanev4hi (TARGET_NEON)
#define HAVE_neon_vmlsl_lanev2si (TARGET_NEON)
#define HAVE_neon_vqdmlsl_lanev4hi (TARGET_NEON)
#define HAVE_neon_vqdmlsl_lanev2si (TARGET_NEON)
#define HAVE_neon_vextv8qi (TARGET_NEON)
#define HAVE_neon_vextv16qi (TARGET_NEON)
#define HAVE_neon_vextv4hi (TARGET_NEON)
#define HAVE_neon_vextv8hi (TARGET_NEON)
#define HAVE_neon_vextv2si (TARGET_NEON)
#define HAVE_neon_vextv4si (TARGET_NEON)
#define HAVE_neon_vextv2sf (TARGET_NEON)
#define HAVE_neon_vextv4sf (TARGET_NEON)
#define HAVE_neon_vextdi (TARGET_NEON)
#define HAVE_neon_vextv2di (TARGET_NEON)
#define HAVE_neon_vrev64v8qi (TARGET_NEON)
#define HAVE_neon_vrev64v16qi (TARGET_NEON)
#define HAVE_neon_vrev64v4hi (TARGET_NEON)
#define HAVE_neon_vrev64v8hi (TARGET_NEON)
#define HAVE_neon_vrev64v2si (TARGET_NEON)
#define HAVE_neon_vrev64v4si (TARGET_NEON)
#define HAVE_neon_vrev64v2sf (TARGET_NEON)
#define HAVE_neon_vrev64v4sf (TARGET_NEON)
#define HAVE_neon_vrev64v2di (TARGET_NEON)
#define HAVE_neon_vrev32v8qi (TARGET_NEON)
#define HAVE_neon_vrev32v4hi (TARGET_NEON)
#define HAVE_neon_vrev32v16qi (TARGET_NEON)
#define HAVE_neon_vrev32v8hi (TARGET_NEON)
#define HAVE_neon_vrev16v8qi (TARGET_NEON)
#define HAVE_neon_vrev16v16qi (TARGET_NEON)
#define HAVE_neon_vbslv8qi_internal (TARGET_NEON)
#define HAVE_neon_vbslv16qi_internal (TARGET_NEON)
#define HAVE_neon_vbslv4hi_internal (TARGET_NEON)
#define HAVE_neon_vbslv8hi_internal (TARGET_NEON)
#define HAVE_neon_vbslv2si_internal (TARGET_NEON)
#define HAVE_neon_vbslv4si_internal (TARGET_NEON)
#define HAVE_neon_vbslv2sf_internal (TARGET_NEON)
#define HAVE_neon_vbslv4sf_internal (TARGET_NEON)
#define HAVE_neon_vbsldi_internal (TARGET_NEON)
#define HAVE_neon_vbslv2di_internal (TARGET_NEON)
#define HAVE_neon_vshlv8qi (TARGET_NEON)
#define HAVE_neon_vshlv16qi (TARGET_NEON)
#define HAVE_neon_vshlv4hi (TARGET_NEON)
#define HAVE_neon_vshlv8hi (TARGET_NEON)
#define HAVE_neon_vshlv2si (TARGET_NEON)
#define HAVE_neon_vshlv4si (TARGET_NEON)
#define HAVE_neon_vshldi (TARGET_NEON)
#define HAVE_neon_vshlv2di (TARGET_NEON)
#define HAVE_neon_vqshlv8qi (TARGET_NEON)
#define HAVE_neon_vqshlv16qi (TARGET_NEON)
#define HAVE_neon_vqshlv4hi (TARGET_NEON)
#define HAVE_neon_vqshlv8hi (TARGET_NEON)
#define HAVE_neon_vqshlv2si (TARGET_NEON)
#define HAVE_neon_vqshlv4si (TARGET_NEON)
#define HAVE_neon_vqshldi (TARGET_NEON)
#define HAVE_neon_vqshlv2di (TARGET_NEON)
#define HAVE_neon_vshr_nv8qi (TARGET_NEON)
#define HAVE_neon_vshr_nv16qi (TARGET_NEON)
#define HAVE_neon_vshr_nv4hi (TARGET_NEON)
#define HAVE_neon_vshr_nv8hi (TARGET_NEON)
#define HAVE_neon_vshr_nv2si (TARGET_NEON)
#define HAVE_neon_vshr_nv4si (TARGET_NEON)
#define HAVE_neon_vshr_ndi (TARGET_NEON)
#define HAVE_neon_vshr_nv2di (TARGET_NEON)
#define HAVE_neon_vshrn_nv8hi (TARGET_NEON)
#define HAVE_neon_vshrn_nv4si (TARGET_NEON)
#define HAVE_neon_vshrn_nv2di (TARGET_NEON)
#define HAVE_neon_vqshrn_nv8hi (TARGET_NEON)
#define HAVE_neon_vqshrn_nv4si (TARGET_NEON)
#define HAVE_neon_vqshrn_nv2di (TARGET_NEON)
#define HAVE_neon_vqshrun_nv8hi (TARGET_NEON)
#define HAVE_neon_vqshrun_nv4si (TARGET_NEON)
#define HAVE_neon_vqshrun_nv2di (TARGET_NEON)
#define HAVE_neon_vshl_nv8qi (TARGET_NEON)
#define HAVE_neon_vshl_nv16qi (TARGET_NEON)
#define HAVE_neon_vshl_nv4hi (TARGET_NEON)
#define HAVE_neon_vshl_nv8hi (TARGET_NEON)
#define HAVE_neon_vshl_nv2si (TARGET_NEON)
#define HAVE_neon_vshl_nv4si (TARGET_NEON)
#define HAVE_neon_vshl_ndi (TARGET_NEON)
#define HAVE_neon_vshl_nv2di (TARGET_NEON)
#define HAVE_neon_vqshl_nv8qi (TARGET_NEON)
#define HAVE_neon_vqshl_nv16qi (TARGET_NEON)
#define HAVE_neon_vqshl_nv4hi (TARGET_NEON)
#define HAVE_neon_vqshl_nv8hi (TARGET_NEON)
#define HAVE_neon_vqshl_nv2si (TARGET_NEON)
#define HAVE_neon_vqshl_nv4si (TARGET_NEON)
#define HAVE_neon_vqshl_ndi (TARGET_NEON)
#define HAVE_neon_vqshl_nv2di (TARGET_NEON)
#define HAVE_neon_vqshlu_nv8qi (TARGET_NEON)
#define HAVE_neon_vqshlu_nv16qi (TARGET_NEON)
#define HAVE_neon_vqshlu_nv4hi (TARGET_NEON)
#define HAVE_neon_vqshlu_nv8hi (TARGET_NEON)
#define HAVE_neon_vqshlu_nv2si (TARGET_NEON)
#define HAVE_neon_vqshlu_nv4si (TARGET_NEON)
#define HAVE_neon_vqshlu_ndi (TARGET_NEON)
#define HAVE_neon_vqshlu_nv2di (TARGET_NEON)
#define HAVE_neon_vshll_nv8qi (TARGET_NEON)
#define HAVE_neon_vshll_nv4hi (TARGET_NEON)
#define HAVE_neon_vshll_nv2si (TARGET_NEON)
#define HAVE_neon_vsra_nv8qi (TARGET_NEON)
#define HAVE_neon_vsra_nv16qi (TARGET_NEON)
#define HAVE_neon_vsra_nv4hi (TARGET_NEON)
#define HAVE_neon_vsra_nv8hi (TARGET_NEON)
#define HAVE_neon_vsra_nv2si (TARGET_NEON)
#define HAVE_neon_vsra_nv4si (TARGET_NEON)
#define HAVE_neon_vsra_ndi (TARGET_NEON)
#define HAVE_neon_vsra_nv2di (TARGET_NEON)
#define HAVE_neon_vsri_nv8qi (TARGET_NEON)
#define HAVE_neon_vsri_nv16qi (TARGET_NEON)
#define HAVE_neon_vsri_nv4hi (TARGET_NEON)
#define HAVE_neon_vsri_nv8hi (TARGET_NEON)
#define HAVE_neon_vsri_nv2si (TARGET_NEON)
#define HAVE_neon_vsri_nv4si (TARGET_NEON)
#define HAVE_neon_vsri_ndi (TARGET_NEON)
#define HAVE_neon_vsri_nv2di (TARGET_NEON)
#define HAVE_neon_vsli_nv8qi (TARGET_NEON)
#define HAVE_neon_vsli_nv16qi (TARGET_NEON)
#define HAVE_neon_vsli_nv4hi (TARGET_NEON)
#define HAVE_neon_vsli_nv8hi (TARGET_NEON)
#define HAVE_neon_vsli_nv2si (TARGET_NEON)
#define HAVE_neon_vsli_nv4si (TARGET_NEON)
#define HAVE_neon_vsli_ndi (TARGET_NEON)
#define HAVE_neon_vsli_nv2di (TARGET_NEON)
#define HAVE_neon_vtbl1v8qi (TARGET_NEON)
#define HAVE_neon_vtbl2v8qi (TARGET_NEON)
#define HAVE_neon_vtbl3v8qi (TARGET_NEON)
#define HAVE_neon_vtbl4v8qi (TARGET_NEON)
#define HAVE_neon_vtbx1v8qi (TARGET_NEON)
#define HAVE_neon_vtbx2v8qi (TARGET_NEON)
#define HAVE_neon_vtbx3v8qi (TARGET_NEON)
#define HAVE_neon_vtbx4v8qi (TARGET_NEON)
#define HAVE_neon_vtrnv8qi_internal (TARGET_NEON)
#define HAVE_neon_vtrnv16qi_internal (TARGET_NEON)
#define HAVE_neon_vtrnv4hi_internal (TARGET_NEON)
#define HAVE_neon_vtrnv8hi_internal (TARGET_NEON)
#define HAVE_neon_vtrnv2si_internal (TARGET_NEON)
#define HAVE_neon_vtrnv4si_internal (TARGET_NEON)
#define HAVE_neon_vtrnv2sf_internal (TARGET_NEON)
#define HAVE_neon_vtrnv4sf_internal (TARGET_NEON)
#define HAVE_neon_vzipv8qi_internal (TARGET_NEON)
#define HAVE_neon_vzipv16qi_internal (TARGET_NEON)
#define HAVE_neon_vzipv4hi_internal (TARGET_NEON)
#define HAVE_neon_vzipv8hi_internal (TARGET_NEON)
#define HAVE_neon_vzipv2si_internal (TARGET_NEON)
#define HAVE_neon_vzipv4si_internal (TARGET_NEON)
#define HAVE_neon_vzipv2sf_internal (TARGET_NEON)
#define HAVE_neon_vzipv4sf_internal (TARGET_NEON)
#define HAVE_neon_vuzpv8qi_internal (TARGET_NEON)
#define HAVE_neon_vuzpv16qi_internal (TARGET_NEON)
#define HAVE_neon_vuzpv4hi_internal (TARGET_NEON)
#define HAVE_neon_vuzpv8hi_internal (TARGET_NEON)
#define HAVE_neon_vuzpv2si_internal (TARGET_NEON)
#define HAVE_neon_vuzpv4si_internal (TARGET_NEON)
#define HAVE_neon_vuzpv2sf_internal (TARGET_NEON)
#define HAVE_neon_vuzpv4sf_internal (TARGET_NEON)
#define HAVE_neon_vld1v8qi (TARGET_NEON)
#define HAVE_neon_vld1v16qi (TARGET_NEON)
#define HAVE_neon_vld1v4hi (TARGET_NEON)
#define HAVE_neon_vld1v8hi (TARGET_NEON)
#define HAVE_neon_vld1v2si (TARGET_NEON)
#define HAVE_neon_vld1v4si (TARGET_NEON)
#define HAVE_neon_vld1v2sf (TARGET_NEON)
#define HAVE_neon_vld1v4sf (TARGET_NEON)
#define HAVE_neon_vld1di (TARGET_NEON)
#define HAVE_neon_vld1v2di (TARGET_NEON)
#define HAVE_neon_vld1_lanev8qi (TARGET_NEON)
#define HAVE_neon_vld1_lanev4hi (TARGET_NEON)
#define HAVE_neon_vld1_lanev2si (TARGET_NEON)
#define HAVE_neon_vld1_lanev2sf (TARGET_NEON)
#define HAVE_neon_vld1_lanedi (TARGET_NEON)
#define HAVE_neon_vld1_lanev16qi (TARGET_NEON)
#define HAVE_neon_vld1_lanev8hi (TARGET_NEON)
#define HAVE_neon_vld1_lanev4si (TARGET_NEON)
#define HAVE_neon_vld1_lanev4sf (TARGET_NEON)
#define HAVE_neon_vld1_lanev2di (TARGET_NEON)
#define HAVE_neon_vld1_dupv8qi (TARGET_NEON)
#define HAVE_neon_vld1_dupv4hi (TARGET_NEON)
#define HAVE_neon_vld1_dupv2si (TARGET_NEON)
#define HAVE_neon_vld1_dupv2sf (TARGET_NEON)
#define HAVE_neon_vld1_dupdi (TARGET_NEON)
#define HAVE_neon_vld1_dupv16qi (TARGET_NEON)
#define HAVE_neon_vld1_dupv8hi (TARGET_NEON)
#define HAVE_neon_vld1_dupv4si (TARGET_NEON)
#define HAVE_neon_vld1_dupv4sf (TARGET_NEON)
#define HAVE_neon_vld1_dupv2di (TARGET_NEON)
#define HAVE_neon_vst1v8qi (TARGET_NEON)
#define HAVE_neon_vst1v16qi (TARGET_NEON)
#define HAVE_neon_vst1v4hi (TARGET_NEON)
#define HAVE_neon_vst1v8hi (TARGET_NEON)
#define HAVE_neon_vst1v2si (TARGET_NEON)
#define HAVE_neon_vst1v4si (TARGET_NEON)
#define HAVE_neon_vst1v2sf (TARGET_NEON)
#define HAVE_neon_vst1v4sf (TARGET_NEON)
#define HAVE_neon_vst1di (TARGET_NEON)
#define HAVE_neon_vst1v2di (TARGET_NEON)
#define HAVE_neon_vst1_lanev8qi (TARGET_NEON)
#define HAVE_neon_vst1_lanev4hi (TARGET_NEON)
#define HAVE_neon_vst1_lanev2si (TARGET_NEON)
#define HAVE_neon_vst1_lanev2sf (TARGET_NEON)
#define HAVE_neon_vst1_lanedi (TARGET_NEON)
#define HAVE_neon_vst1_lanev16qi (TARGET_NEON)
#define HAVE_neon_vst1_lanev8hi (TARGET_NEON)
#define HAVE_neon_vst1_lanev4si (TARGET_NEON)
#define HAVE_neon_vst1_lanev4sf (TARGET_NEON)
#define HAVE_neon_vst1_lanev2di (TARGET_NEON)
#define HAVE_neon_vld2v8qi (TARGET_NEON)
#define HAVE_neon_vld2v4hi (TARGET_NEON)
#define HAVE_neon_vld2v2si (TARGET_NEON)
#define HAVE_neon_vld2v2sf (TARGET_NEON)
#define HAVE_neon_vld2di (TARGET_NEON)
#define HAVE_neon_vld2v16qi (TARGET_NEON)
#define HAVE_neon_vld2v8hi (TARGET_NEON)
#define HAVE_neon_vld2v4si (TARGET_NEON)
#define HAVE_neon_vld2v4sf (TARGET_NEON)
#define HAVE_neon_vld2_lanev8qi (TARGET_NEON)
#define HAVE_neon_vld2_lanev4hi (TARGET_NEON)
#define HAVE_neon_vld2_lanev2si (TARGET_NEON)
#define HAVE_neon_vld2_lanev2sf (TARGET_NEON)
#define HAVE_neon_vld2_lanev8hi (TARGET_NEON)
#define HAVE_neon_vld2_lanev4si (TARGET_NEON)
#define HAVE_neon_vld2_lanev4sf (TARGET_NEON)
#define HAVE_neon_vld2_dupv8qi (TARGET_NEON)
#define HAVE_neon_vld2_dupv4hi (TARGET_NEON)
#define HAVE_neon_vld2_dupv2si (TARGET_NEON)
#define HAVE_neon_vld2_dupv2sf (TARGET_NEON)
#define HAVE_neon_vld2_dupdi (TARGET_NEON)
#define HAVE_neon_vst2v8qi (TARGET_NEON)
#define HAVE_neon_vst2v4hi (TARGET_NEON)
#define HAVE_neon_vst2v2si (TARGET_NEON)
#define HAVE_neon_vst2v2sf (TARGET_NEON)
#define HAVE_neon_vst2di (TARGET_NEON)
#define HAVE_neon_vst2v16qi (TARGET_NEON)
#define HAVE_neon_vst2v8hi (TARGET_NEON)
#define HAVE_neon_vst2v4si (TARGET_NEON)
#define HAVE_neon_vst2v4sf (TARGET_NEON)
#define HAVE_neon_vst2_lanev8qi (TARGET_NEON)
#define HAVE_neon_vst2_lanev4hi (TARGET_NEON)
#define HAVE_neon_vst2_lanev2si (TARGET_NEON)
#define HAVE_neon_vst2_lanev2sf (TARGET_NEON)
#define HAVE_neon_vst2_lanev8hi (TARGET_NEON)
#define HAVE_neon_vst2_lanev4si (TARGET_NEON)
#define HAVE_neon_vst2_lanev4sf (TARGET_NEON)
#define HAVE_neon_vld3v8qi (TARGET_NEON)
#define HAVE_neon_vld3v4hi (TARGET_NEON)
#define HAVE_neon_vld3v2si (TARGET_NEON)
#define HAVE_neon_vld3v2sf (TARGET_NEON)
#define HAVE_neon_vld3di (TARGET_NEON)
#define HAVE_neon_vld3qav16qi (TARGET_NEON)
#define HAVE_neon_vld3qav8hi (TARGET_NEON)
#define HAVE_neon_vld3qav4si (TARGET_NEON)
#define HAVE_neon_vld3qav4sf (TARGET_NEON)
#define HAVE_neon_vld3qbv16qi (TARGET_NEON)
#define HAVE_neon_vld3qbv8hi (TARGET_NEON)
#define HAVE_neon_vld3qbv4si (TARGET_NEON)
#define HAVE_neon_vld3qbv4sf (TARGET_NEON)
#define HAVE_neon_vld3_lanev8qi (TARGET_NEON)
#define HAVE_neon_vld3_lanev4hi (TARGET_NEON)
#define HAVE_neon_vld3_lanev2si (TARGET_NEON)
#define HAVE_neon_vld3_lanev2sf (TARGET_NEON)
#define HAVE_neon_vld3_lanev8hi (TARGET_NEON)
#define HAVE_neon_vld3_lanev4si (TARGET_NEON)
#define HAVE_neon_vld3_lanev4sf (TARGET_NEON)
#define HAVE_neon_vld3_dupv8qi (TARGET_NEON)
#define HAVE_neon_vld3_dupv4hi (TARGET_NEON)
#define HAVE_neon_vld3_dupv2si (TARGET_NEON)
#define HAVE_neon_vld3_dupv2sf (TARGET_NEON)
#define HAVE_neon_vld3_dupdi (TARGET_NEON)
#define HAVE_neon_vst3v8qi (TARGET_NEON)
#define HAVE_neon_vst3v4hi (TARGET_NEON)
#define HAVE_neon_vst3v2si (TARGET_NEON)
#define HAVE_neon_vst3v2sf (TARGET_NEON)
#define HAVE_neon_vst3di (TARGET_NEON)
#define HAVE_neon_vst3qav16qi (TARGET_NEON)
#define HAVE_neon_vst3qav8hi (TARGET_NEON)
#define HAVE_neon_vst3qav4si (TARGET_NEON)
#define HAVE_neon_vst3qav4sf (TARGET_NEON)
#define HAVE_neon_vst3qbv16qi (TARGET_NEON)
#define HAVE_neon_vst3qbv8hi (TARGET_NEON)
#define HAVE_neon_vst3qbv4si (TARGET_NEON)
#define HAVE_neon_vst3qbv4sf (TARGET_NEON)
#define HAVE_neon_vst3_lanev8qi (TARGET_NEON)
#define HAVE_neon_vst3_lanev4hi (TARGET_NEON)
#define HAVE_neon_vst3_lanev2si (TARGET_NEON)
#define HAVE_neon_vst3_lanev2sf (TARGET_NEON)
#define HAVE_neon_vst3_lanev8hi (TARGET_NEON)
#define HAVE_neon_vst3_lanev4si (TARGET_NEON)
#define HAVE_neon_vst3_lanev4sf (TARGET_NEON)
#define HAVE_neon_vld4v8qi (TARGET_NEON)
#define HAVE_neon_vld4v4hi (TARGET_NEON)
#define HAVE_neon_vld4v2si (TARGET_NEON)
#define HAVE_neon_vld4v2sf (TARGET_NEON)
#define HAVE_neon_vld4di (TARGET_NEON)
#define HAVE_neon_vld4qav16qi (TARGET_NEON)
#define HAVE_neon_vld4qav8hi (TARGET_NEON)
#define HAVE_neon_vld4qav4si (TARGET_NEON)
#define HAVE_neon_vld4qav4sf (TARGET_NEON)
#define HAVE_neon_vld4qbv16qi (TARGET_NEON)
#define HAVE_neon_vld4qbv8hi (TARGET_NEON)
#define HAVE_neon_vld4qbv4si (TARGET_NEON)
#define HAVE_neon_vld4qbv4sf (TARGET_NEON)
#define HAVE_neon_vld4_lanev8qi (TARGET_NEON)
#define HAVE_neon_vld4_lanev4hi (TARGET_NEON)
#define HAVE_neon_vld4_lanev2si (TARGET_NEON)
#define HAVE_neon_vld4_lanev2sf (TARGET_NEON)
#define HAVE_neon_vld4_lanev8hi (TARGET_NEON)
#define HAVE_neon_vld4_lanev4si (TARGET_NEON)
#define HAVE_neon_vld4_lanev4sf (TARGET_NEON)
#define HAVE_neon_vld4_dupv8qi (TARGET_NEON)
#define HAVE_neon_vld4_dupv4hi (TARGET_NEON)
#define HAVE_neon_vld4_dupv2si (TARGET_NEON)
#define HAVE_neon_vld4_dupv2sf (TARGET_NEON)
#define HAVE_neon_vld4_dupdi (TARGET_NEON)
#define HAVE_neon_vst4v8qi (TARGET_NEON)
#define HAVE_neon_vst4v4hi (TARGET_NEON)
#define HAVE_neon_vst4v2si (TARGET_NEON)
#define HAVE_neon_vst4v2sf (TARGET_NEON)
#define HAVE_neon_vst4di (TARGET_NEON)
#define HAVE_neon_vst4qav16qi (TARGET_NEON)
#define HAVE_neon_vst4qav8hi (TARGET_NEON)
#define HAVE_neon_vst4qav4si (TARGET_NEON)
#define HAVE_neon_vst4qav4sf (TARGET_NEON)
#define HAVE_neon_vst4qbv16qi (TARGET_NEON)
#define HAVE_neon_vst4qbv8hi (TARGET_NEON)
#define HAVE_neon_vst4qbv4si (TARGET_NEON)
#define HAVE_neon_vst4qbv4sf (TARGET_NEON)
#define HAVE_neon_vst4_lanev8qi (TARGET_NEON)
#define HAVE_neon_vst4_lanev4hi (TARGET_NEON)
#define HAVE_neon_vst4_lanev2si (TARGET_NEON)
#define HAVE_neon_vst4_lanev2sf (TARGET_NEON)
#define HAVE_neon_vst4_lanev8hi (TARGET_NEON)
#define HAVE_neon_vst4_lanev4si (TARGET_NEON)
#define HAVE_neon_vst4_lanev4sf (TARGET_NEON)
#define HAVE_neon_vec_unpacks_lo_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacku_lo_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacks_lo_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacku_lo_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacks_lo_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacku_lo_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacks_hi_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacku_hi_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacks_hi_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacku_hi_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacks_hi_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_unpacku_hi_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_smult_lo_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_umult_lo_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_smult_lo_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_umult_lo_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_smult_lo_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_umult_lo_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_smult_hi_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_umult_hi_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_smult_hi_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_umult_hi_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_smult_hi_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_umult_hi_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_unpacks_v8qi (TARGET_NEON)
#define HAVE_neon_unpacku_v8qi (TARGET_NEON)
#define HAVE_neon_unpacks_v4hi (TARGET_NEON)
#define HAVE_neon_unpacku_v4hi (TARGET_NEON)
#define HAVE_neon_unpacks_v2si (TARGET_NEON)
#define HAVE_neon_unpacku_v2si (TARGET_NEON)
#define HAVE_neon_vec_smult_v8qi (TARGET_NEON)
#define HAVE_neon_vec_umult_v8qi (TARGET_NEON)
#define HAVE_neon_vec_smult_v4hi (TARGET_NEON)
#define HAVE_neon_vec_umult_v4hi (TARGET_NEON)
#define HAVE_neon_vec_smult_v2si (TARGET_NEON)
#define HAVE_neon_vec_umult_v2si (TARGET_NEON)
#define HAVE_vec_pack_trunc_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_pack_trunc_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_pack_trunc_v2di (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_pack_trunc_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_pack_trunc_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_neon_vec_pack_trunc_v2di (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_arm_sync_compare_and_swapsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_compare_and_swapqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_compare_and_swaphi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_lock_test_and_setsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_lock_test_and_setqi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_lock_test_and_sethi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_addsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_subsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_iorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_xorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_andsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_nandsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_addqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_subqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_iorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_xorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_andqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_addhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_subhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_iorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_xorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_andhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_nandqi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_new_nandhi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_addsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_subsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_iorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_xorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_andsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_nandsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_addqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_subqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_iorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_xorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_andqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_addhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_subhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_iorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_xorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_andhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_nandqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_arm_sync_old_nandhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_addqq3 (TARGET_32BIT)
#define HAVE_addhq3 (TARGET_32BIT)
#define HAVE_addsq3 (TARGET_32BIT)
#define HAVE_adduqq3 (TARGET_32BIT)
#define HAVE_adduhq3 (TARGET_32BIT)
#define HAVE_addusq3 (TARGET_32BIT)
#define HAVE_addha3 (TARGET_32BIT)
#define HAVE_addsa3 (TARGET_32BIT)
#define HAVE_adduha3 (TARGET_32BIT)
#define HAVE_addusa3 (TARGET_32BIT)
#define HAVE_addv4qq3 (TARGET_INT_SIMD)
#define HAVE_addv2hq3 (TARGET_INT_SIMD)
#define HAVE_addv2ha3 (TARGET_INT_SIMD)
#define HAVE_usaddv4uqq3 (TARGET_INT_SIMD)
#define HAVE_usaddv2uhq3 (TARGET_INT_SIMD)
#define HAVE_usadduqq3 (TARGET_INT_SIMD)
#define HAVE_usadduhq3 (TARGET_INT_SIMD)
#define HAVE_usaddv2uha3 (TARGET_INT_SIMD)
#define HAVE_usadduha3 (TARGET_INT_SIMD)
#define HAVE_ssaddv4qq3 (TARGET_INT_SIMD)
#define HAVE_ssaddv2hq3 (TARGET_INT_SIMD)
#define HAVE_ssaddqq3 (TARGET_INT_SIMD)
#define HAVE_ssaddhq3 (TARGET_INT_SIMD)
#define HAVE_ssaddv2ha3 (TARGET_INT_SIMD)
#define HAVE_ssaddha3 (TARGET_INT_SIMD)
#define HAVE_ssaddsq3 (TARGET_INT_SIMD)
#define HAVE_ssaddsa3 (TARGET_INT_SIMD)
#define HAVE_subqq3 (TARGET_32BIT)
#define HAVE_subhq3 (TARGET_32BIT)
#define HAVE_subsq3 (TARGET_32BIT)
#define HAVE_subuqq3 (TARGET_32BIT)
#define HAVE_subuhq3 (TARGET_32BIT)
#define HAVE_subusq3 (TARGET_32BIT)
#define HAVE_subha3 (TARGET_32BIT)
#define HAVE_subsa3 (TARGET_32BIT)
#define HAVE_subuha3 (TARGET_32BIT)
#define HAVE_subusa3 (TARGET_32BIT)
#define HAVE_subv4qq3 (TARGET_INT_SIMD)
#define HAVE_subv2hq3 (TARGET_INT_SIMD)
#define HAVE_subv2ha3 (TARGET_INT_SIMD)
#define HAVE_ussubv4uqq3 (TARGET_INT_SIMD)
#define HAVE_ussubv2uhq3 (TARGET_INT_SIMD)
#define HAVE_ussubuqq3 (TARGET_INT_SIMD)
#define HAVE_ussubuhq3 (TARGET_INT_SIMD)
#define HAVE_ussubv2uha3 (TARGET_INT_SIMD)
#define HAVE_ussubuha3 (TARGET_INT_SIMD)
#define HAVE_sssubv4qq3 (TARGET_INT_SIMD)
#define HAVE_sssubv2hq3 (TARGET_INT_SIMD)
#define HAVE_sssubqq3 (TARGET_INT_SIMD)
#define HAVE_sssubhq3 (TARGET_INT_SIMD)
#define HAVE_sssubv2ha3 (TARGET_INT_SIMD)
#define HAVE_sssubha3 (TARGET_INT_SIMD)
#define HAVE_sssubsq3 (TARGET_INT_SIMD)
#define HAVE_sssubsa3 (TARGET_INT_SIMD)
#define HAVE_ssmulsa3 (TARGET_32BIT && arm_arch6)
#define HAVE_usmulusa3 (TARGET_32BIT && arm_arch6)
#define HAVE_arm_ssatsihi_shift (TARGET_32BIT && arm_arch6)
#define HAVE_arm_usatsihi (TARGET_INT_SIMD)
#define HAVE_adddi3 1
#define HAVE_addsi3 1
#define HAVE_incscc (TARGET_32BIT)
#define HAVE_addsf3 (TARGET_32BIT && TARGET_HARD_FLOAT)
#define HAVE_adddf3 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_subdi3 1
#define HAVE_subsi3 1
#define HAVE_decscc (TARGET_32BIT)
#define HAVE_subsf3 (TARGET_32BIT && TARGET_HARD_FLOAT)
#define HAVE_subdf3 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_mulsi3 1
#define HAVE_maddsidi4 (TARGET_32BIT && arm_arch3m)
#define HAVE_mulsidi3 (TARGET_32BIT && arm_arch3m)
#define HAVE_umulsidi3 (TARGET_32BIT && arm_arch3m)
#define HAVE_umaddsidi4 (TARGET_32BIT && arm_arch3m)
#define HAVE_smulsi3_highpart (TARGET_32BIT && arm_arch3m)
#define HAVE_umulsi3_highpart (TARGET_32BIT && arm_arch3m)
#define HAVE_mulsf3 (TARGET_32BIT && TARGET_HARD_FLOAT)
#define HAVE_muldf3 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_divsf3 (TARGET_32BIT && TARGET_HARD_FLOAT && (TARGET_FPA || TARGET_VFP))
#define HAVE_divdf3 (TARGET_32BIT && TARGET_HARD_FLOAT && (TARGET_FPA || TARGET_VFP_DOUBLE))
#define HAVE_modsf3 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_FPA)
#define HAVE_moddf3 (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_FPA)
#define HAVE_anddi3 (TARGET_32BIT)
#define HAVE_andsi3 1
#define HAVE_insv (TARGET_ARM || arm_arch_thumb2)
#define HAVE_iordi3 (TARGET_32BIT)
#define HAVE_iorsi3 1
#define HAVE_xordi3 (TARGET_32BIT)
#define HAVE_xorsi3 1
#define HAVE_smaxsi3 (TARGET_32BIT)
#define HAVE_sminsi3 (TARGET_32BIT)
#define HAVE_umaxsi3 (TARGET_32BIT)
#define HAVE_uminsi3 (TARGET_32BIT)
#define HAVE_ashldi3 (TARGET_32BIT)
#define HAVE_ashlsi3 1
#define HAVE_ashrdi3 (TARGET_32BIT)
#define HAVE_ashrsi3 1
#define HAVE_lshrdi3 (TARGET_32BIT)
#define HAVE_lshrsi3 1
#define HAVE_rotlsi3 (TARGET_32BIT)
#define HAVE_rotrsi3 1
#define HAVE_extzv (TARGET_THUMB1 || arm_arch_thumb2)
#define HAVE_extv (arm_arch_thumb2)
#define HAVE_negdi2 1
#define HAVE_negsi2 1
#define HAVE_negsf2 (TARGET_32BIT && TARGET_HARD_FLOAT && (TARGET_FPA || TARGET_VFP))
#define HAVE_negdf2 (TARGET_32BIT && TARGET_HARD_FLOAT && (TARGET_FPA || TARGET_VFP_DOUBLE))
#define HAVE_abssi2 1
#define HAVE_abssf2 (TARGET_32BIT && TARGET_HARD_FLOAT)
#define HAVE_absdf2 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_sqrtsf2 (TARGET_32BIT && TARGET_HARD_FLOAT && (TARGET_FPA || TARGET_VFP))
#define HAVE_sqrtdf2 (TARGET_32BIT && TARGET_HARD_FLOAT && (TARGET_FPA || TARGET_VFP_DOUBLE))
#define HAVE_one_cmplsi2 1
#define HAVE_floatsihf2 1
#define HAVE_floatdihf2 1
#define HAVE_floatsisf2 (TARGET_32BIT && TARGET_HARD_FLOAT)
#define HAVE_floatsidf2 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_fix_trunchfsi2 1
#define HAVE_fix_trunchfdi2 1
#define HAVE_fix_truncsfsi2 (TARGET_32BIT && TARGET_HARD_FLOAT)
#define HAVE_fix_truncdfsi2 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_truncdfsf2 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_truncdfhf2 1
#define HAVE_zero_extendsidi2 (TARGET_32BIT)
#define HAVE_zero_extendqidi2 (TARGET_32BIT)
#define HAVE_extendsidi2 (TARGET_32BIT)
#define HAVE_zero_extendhisi2 1
#define HAVE_zero_extendqisi2 1
#define HAVE_extendhisi2 1
#define HAVE_extendhisi2_mem (TARGET_ARM)
#define HAVE_extendqihi2 (TARGET_ARM)
#define HAVE_extendqisi2 1
#define HAVE_extendsfdf2 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_extendhfdf2 1
#define HAVE_movdi 1
#define HAVE_movsi 1
#define HAVE_calculate_pic_address (flag_pic)
#define HAVE_builtin_setjmp_receiver (flag_pic)
#define HAVE_storehi (TARGET_ARM)
#define HAVE_storehi_bigend (TARGET_ARM)
#define HAVE_storeinthi (TARGET_ARM)
#define HAVE_storehi_single_op (TARGET_32BIT && arm_arch4)
#define HAVE_movhi 1
#define HAVE_movhi_bytes (TARGET_ARM)
#define HAVE_movhi_bigend (TARGET_ARM)
#define HAVE_thumb_movhi_clobber (TARGET_THUMB1)
#define HAVE_reload_outhi 1
#define HAVE_reload_inhi 1
#define HAVE_movqi 1
#define HAVE_movhf 1
#define HAVE_movsf 1
#define HAVE_movdf 1
#define HAVE_reload_outdf (TARGET_32BIT)
#define HAVE_movxf (TARGET_32BIT && TARGET_HARD_FLOAT && TARGET_FPA)
#define HAVE_load_multiple (TARGET_32BIT)
#define HAVE_store_multiple (TARGET_32BIT)
#define HAVE_movmemqi (TARGET_EITHER && !low_irq_latency)
#define HAVE_cbranchsi4 (TARGET_THUMB1 || TARGET_32BIT)
#define HAVE_cbranchqi4 (TARGET_THUMB1)
#define HAVE_cbranchsf4 (TARGET_32BIT && TARGET_HARD_FLOAT)
#define HAVE_cbranchdf4 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_cbranchdi4 (TARGET_32BIT)
#define HAVE_cbranch_cc (TARGET_32BIT)
#define HAVE_cstore_cc (TARGET_32BIT)
#define HAVE_cstoresi4 (TARGET_32BIT || TARGET_THUMB1)
#define HAVE_cstoresf4 (TARGET_32BIT && TARGET_HARD_FLOAT)
#define HAVE_cstoredf4 (TARGET_32BIT && TARGET_HARD_FLOAT && !TARGET_VFP_SINGLE)
#define HAVE_cstoredi4 (TARGET_32BIT)
#define HAVE_cstoresi_eq0_thumb1 (TARGET_THUMB1)
#define HAVE_cstoresi_ne0_thumb1 (TARGET_THUMB1)
#define HAVE_movsicc (TARGET_32BIT)
#define HAVE_movsfcc (TARGET_32BIT && TARGET_HARD_FLOAT)
#define HAVE_movdfcc (TARGET_32BIT && TARGET_HARD_FLOAT && (TARGET_FPA || TARGET_VFP_DOUBLE))
#define HAVE_jump 1
#define HAVE_call 1
#define HAVE_call_internal 1
#define HAVE_call_value 1
#define HAVE_call_value_internal 1
#define HAVE_sibcall (TARGET_32BIT)
#define HAVE_sibcall_value (TARGET_32BIT)
#define HAVE_return (TARGET_32BIT && USE_RETURN_INSN (FALSE))
#define HAVE_simple_return (TARGET_32BIT && use_simple_return_p ())
#define HAVE_return_addr_mask (TARGET_ARM)
#define HAVE_untyped_call 1
#define HAVE_untyped_return 1
#define HAVE_casesi (TARGET_32BIT || optimize_size || flag_pic)
#define HAVE_thumb1_casesi_internal_pic (TARGET_THUMB1)
#define HAVE_indirect_jump 1
#define HAVE_prologue 1
#define HAVE_epilogue 1
#define HAVE_eh_epilogue 1
#define HAVE_tablejump (TARGET_THUMB1)
#define HAVE_ctzsi2 (TARGET_32BIT && arm_arch_thumb2)
#define HAVE_eh_return 1
#define HAVE_arm_legacy_rev (TARGET_32BIT)
#define HAVE_thumb_legacy_rev (TARGET_THUMB)
#define HAVE_bswapsi2 1
#define HAVE_movv2di (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2DImode)))
#define HAVE_movv2si (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SImode)))
#define HAVE_movv4hi (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4HImode)))
#define HAVE_movv8qi (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8QImode)))
#define HAVE_movv2sf (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SFmode)))
#define HAVE_movv4si (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SImode)))
#define HAVE_movv8hi (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8HImode)))
#define HAVE_movv16qi (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V16QImode)))
#define HAVE_movv4sf (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SFmode)))
#define HAVE_addv2di3 ((TARGET_NEON && ((V2DImode != V2SFmode && V2DImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2DImode)))
#define HAVE_addv2si3 ((TARGET_NEON && ((V2SImode != V2SFmode && V2SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SImode)))
#define HAVE_addv4hi3 ((TARGET_NEON && ((V4HImode != V2SFmode && V4HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4HImode)))
#define HAVE_addv8qi3 ((TARGET_NEON && ((V8QImode != V2SFmode && V8QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8QImode)))
#define HAVE_addv2sf3 ((TARGET_NEON && ((V2SFmode != V2SFmode && V2SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SFmode)))
#define HAVE_addv4si3 ((TARGET_NEON && ((V4SImode != V2SFmode && V4SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SImode)))
#define HAVE_addv8hi3 ((TARGET_NEON && ((V8HImode != V2SFmode && V8HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8HImode)))
#define HAVE_addv16qi3 ((TARGET_NEON && ((V16QImode != V2SFmode && V16QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V16QImode)))
#define HAVE_addv4sf3 ((TARGET_NEON && ((V4SFmode != V2SFmode && V4SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SFmode)))
#define HAVE_subv2di3 ((TARGET_NEON && ((V2DImode != V2SFmode && V2DImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2DImode)))
#define HAVE_subv2si3 ((TARGET_NEON && ((V2SImode != V2SFmode && V2SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SImode)))
#define HAVE_subv4hi3 ((TARGET_NEON && ((V4HImode != V2SFmode && V4HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4HImode)))
#define HAVE_subv8qi3 ((TARGET_NEON && ((V8QImode != V2SFmode && V8QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8QImode)))
#define HAVE_subv2sf3 ((TARGET_NEON && ((V2SFmode != V2SFmode && V2SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SFmode)))
#define HAVE_subv4si3 ((TARGET_NEON && ((V4SImode != V2SFmode && V4SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SImode)))
#define HAVE_subv8hi3 ((TARGET_NEON && ((V8HImode != V2SFmode && V8HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8HImode)))
#define HAVE_subv16qi3 ((TARGET_NEON && ((V16QImode != V2SFmode && V16QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V16QImode)))
#define HAVE_subv4sf3 ((TARGET_NEON && ((V4SFmode != V2SFmode && V4SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SFmode)))
#define HAVE_mulv2si3 ((TARGET_NEON && ((V2SImode != V2SFmode && V2SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (V2SImode == V4HImode && TARGET_REALLY_IWMMXT))
#define HAVE_mulv4hi3 ((TARGET_NEON && ((V4HImode != V2SFmode && V4HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (V4HImode == V4HImode && TARGET_REALLY_IWMMXT))
#define HAVE_mulv8qi3 ((TARGET_NEON && ((V8QImode != V2SFmode && V8QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (V8QImode == V4HImode && TARGET_REALLY_IWMMXT))
#define HAVE_mulv2sf3 ((TARGET_NEON && ((V2SFmode != V2SFmode && V2SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (V2SFmode == V4HImode && TARGET_REALLY_IWMMXT))
#define HAVE_mulv4si3 ((TARGET_NEON && ((V4SImode != V2SFmode && V4SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (V4SImode == V4HImode && TARGET_REALLY_IWMMXT))
#define HAVE_mulv8hi3 ((TARGET_NEON && ((V8HImode != V2SFmode && V8HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (V8HImode == V4HImode && TARGET_REALLY_IWMMXT))
#define HAVE_mulv16qi3 ((TARGET_NEON && ((V16QImode != V2SFmode && V16QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (V16QImode == V4HImode && TARGET_REALLY_IWMMXT))
#define HAVE_mulv4sf3 ((TARGET_NEON && ((V4SFmode != V2SFmode && V4SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (V4SFmode == V4HImode && TARGET_REALLY_IWMMXT))
#define HAVE_sminv2si3 ((TARGET_NEON && ((V2SImode != V2SFmode && V2SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SImode)))
#define HAVE_sminv4hi3 ((TARGET_NEON && ((V4HImode != V2SFmode && V4HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4HImode)))
#define HAVE_sminv8qi3 ((TARGET_NEON && ((V8QImode != V2SFmode && V8QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8QImode)))
#define HAVE_sminv2sf3 ((TARGET_NEON && ((V2SFmode != V2SFmode && V2SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SFmode)))
#define HAVE_sminv4si3 ((TARGET_NEON && ((V4SImode != V2SFmode && V4SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SImode)))
#define HAVE_sminv8hi3 ((TARGET_NEON && ((V8HImode != V2SFmode && V8HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8HImode)))
#define HAVE_sminv16qi3 ((TARGET_NEON && ((V16QImode != V2SFmode && V16QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V16QImode)))
#define HAVE_sminv4sf3 ((TARGET_NEON && ((V4SFmode != V2SFmode && V4SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SFmode)))
#define HAVE_uminv2si3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SImode)))
#define HAVE_uminv4hi3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4HImode)))
#define HAVE_uminv8qi3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8QImode)))
#define HAVE_uminv4si3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SImode)))
#define HAVE_uminv8hi3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8HImode)))
#define HAVE_uminv16qi3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V16QImode)))
#define HAVE_smaxv2si3 ((TARGET_NEON && ((V2SImode != V2SFmode && V2SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SImode)))
#define HAVE_smaxv4hi3 ((TARGET_NEON && ((V4HImode != V2SFmode && V4HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4HImode)))
#define HAVE_smaxv8qi3 ((TARGET_NEON && ((V8QImode != V2SFmode && V8QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8QImode)))
#define HAVE_smaxv2sf3 ((TARGET_NEON && ((V2SFmode != V2SFmode && V2SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SFmode)))
#define HAVE_smaxv4si3 ((TARGET_NEON && ((V4SImode != V2SFmode && V4SImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SImode)))
#define HAVE_smaxv8hi3 ((TARGET_NEON && ((V8HImode != V2SFmode && V8HImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8HImode)))
#define HAVE_smaxv16qi3 ((TARGET_NEON && ((V16QImode != V2SFmode && V16QImode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V16QImode)))
#define HAVE_smaxv4sf3 ((TARGET_NEON && ((V4SFmode != V2SFmode && V4SFmode != V4SFmode) \
		    || flag_unsafe_math_optimizations)) \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SFmode)))
#define HAVE_umaxv2si3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V2SImode)))
#define HAVE_umaxv4hi3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4HImode)))
#define HAVE_umaxv8qi3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8QImode)))
#define HAVE_umaxv4si3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V4SImode)))
#define HAVE_umaxv8hi3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V8HImode)))
#define HAVE_umaxv16qi3 (TARGET_NEON \
   || (TARGET_REALLY_IWMMXT && VALID_IWMMXT_REG_MODE (V16QImode)))
#define HAVE_movti (TARGET_NEON)
#define HAVE_movei (TARGET_NEON)
#define HAVE_movoi (TARGET_NEON)
#define HAVE_movci (TARGET_NEON)
#define HAVE_movxi (TARGET_NEON)
#define HAVE_movmisalignv8qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_movmisalignv16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_movmisalignv4hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_movmisalignv8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_movmisalignv2si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_movmisalignv4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_movmisalignv2sf (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_movmisalignv4sf (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_movmisaligndi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_movmisalignv2di (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_setv8qi (TARGET_NEON)
#define HAVE_vec_setv16qi (TARGET_NEON)
#define HAVE_vec_setv4hi (TARGET_NEON)
#define HAVE_vec_setv8hi (TARGET_NEON)
#define HAVE_vec_setv2si (TARGET_NEON)
#define HAVE_vec_setv4si (TARGET_NEON)
#define HAVE_vec_setv2sf (TARGET_NEON)
#define HAVE_vec_setv4sf (TARGET_NEON)
#define HAVE_vec_setv2di (TARGET_NEON)
#define HAVE_vec_initv8qi (TARGET_NEON)
#define HAVE_vec_initv16qi (TARGET_NEON)
#define HAVE_vec_initv4hi (TARGET_NEON)
#define HAVE_vec_initv8hi (TARGET_NEON)
#define HAVE_vec_initv2si (TARGET_NEON)
#define HAVE_vec_initv4si (TARGET_NEON)
#define HAVE_vec_initv2sf (TARGET_NEON)
#define HAVE_vec_initv4sf (TARGET_NEON)
#define HAVE_vec_initv2di (TARGET_NEON)
#define HAVE_vashrv8qi3 (TARGET_NEON)
#define HAVE_vashrv16qi3 (TARGET_NEON)
#define HAVE_vashrv4hi3 (TARGET_NEON)
#define HAVE_vashrv8hi3 (TARGET_NEON)
#define HAVE_vashrv2si3 (TARGET_NEON)
#define HAVE_vashrv4si3 (TARGET_NEON)
#define HAVE_vlshrv8qi3 (TARGET_NEON)
#define HAVE_vlshrv16qi3 (TARGET_NEON)
#define HAVE_vlshrv4hi3 (TARGET_NEON)
#define HAVE_vlshrv8hi3 (TARGET_NEON)
#define HAVE_vlshrv2si3 (TARGET_NEON)
#define HAVE_vlshrv4si3 (TARGET_NEON)
#define HAVE_vec_shr_v8qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shr_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shr_v4hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shr_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shr_v2si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shr_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shr_v2sf (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shr_v4sf (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shr_v2di (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shl_v8qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shl_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shl_v4hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shl_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shl_v2si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shl_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shl_v2sf (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shl_v4sf (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_shl_v2di (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_move_hi_quad_v2di (TARGET_NEON)
#define HAVE_move_hi_quad_v2df (TARGET_NEON)
#define HAVE_move_hi_quad_v16qi (TARGET_NEON)
#define HAVE_move_hi_quad_v8hi (TARGET_NEON)
#define HAVE_move_hi_quad_v4si (TARGET_NEON)
#define HAVE_move_hi_quad_v4sf (TARGET_NEON)
#define HAVE_move_lo_quad_v2di (TARGET_NEON)
#define HAVE_move_lo_quad_v2df (TARGET_NEON)
#define HAVE_move_lo_quad_v16qi (TARGET_NEON)
#define HAVE_move_lo_quad_v8hi (TARGET_NEON)
#define HAVE_move_lo_quad_v4si (TARGET_NEON)
#define HAVE_move_lo_quad_v4sf (TARGET_NEON)
#define HAVE_reduc_splus_v8qi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_reduc_splus_v4hi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_reduc_splus_v2si (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_reduc_splus_v2sf (TARGET_NEON && (!true || flag_unsafe_math_optimizations))
#define HAVE_reduc_splus_v16qi (TARGET_NEON && (!false || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_splus_v8hi (TARGET_NEON && (!false || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_splus_v4si (TARGET_NEON && (!false || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_splus_v4sf (TARGET_NEON && (!true || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_uplus_v8qi (TARGET_NEON && (true || !BYTES_BIG_ENDIAN))
#define HAVE_reduc_uplus_v16qi (TARGET_NEON && (false || !BYTES_BIG_ENDIAN))
#define HAVE_reduc_uplus_v4hi (TARGET_NEON && (true || !BYTES_BIG_ENDIAN))
#define HAVE_reduc_uplus_v8hi (TARGET_NEON && (false || !BYTES_BIG_ENDIAN))
#define HAVE_reduc_uplus_v2si (TARGET_NEON && (true || !BYTES_BIG_ENDIAN))
#define HAVE_reduc_uplus_v4si (TARGET_NEON && (false || !BYTES_BIG_ENDIAN))
#define HAVE_reduc_uplus_v2di (TARGET_NEON && (false || !BYTES_BIG_ENDIAN))
#define HAVE_reduc_smin_v8qi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_reduc_smin_v4hi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_reduc_smin_v2si (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_reduc_smin_v2sf (TARGET_NEON && (!true || flag_unsafe_math_optimizations))
#define HAVE_reduc_smin_v16qi (TARGET_NEON && (!false || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_smin_v8hi (TARGET_NEON && (!false || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_smin_v4si (TARGET_NEON && (!false || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_smin_v4sf (TARGET_NEON && (!true || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_smax_v8qi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_reduc_smax_v4hi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_reduc_smax_v2si (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_reduc_smax_v2sf (TARGET_NEON && (!true || flag_unsafe_math_optimizations))
#define HAVE_reduc_smax_v16qi (TARGET_NEON && (!false || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_smax_v8hi (TARGET_NEON && (!false || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_smax_v4si (TARGET_NEON && (!false || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_smax_v4sf (TARGET_NEON && (!true || flag_unsafe_math_optimizations) \
   && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_umin_v8qi (TARGET_NEON)
#define HAVE_reduc_umin_v4hi (TARGET_NEON)
#define HAVE_reduc_umin_v2si (TARGET_NEON)
#define HAVE_reduc_umin_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_umin_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_umin_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_umax_v8qi (TARGET_NEON)
#define HAVE_reduc_umax_v4hi (TARGET_NEON)
#define HAVE_reduc_umax_v2si (TARGET_NEON)
#define HAVE_reduc_umax_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_umax_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_reduc_umax_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vcondv8qi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_vcondv16qi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_vcondv4hi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_vcondv8hi (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_vcondv2si (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_vcondv4si (TARGET_NEON && (!false || flag_unsafe_math_optimizations))
#define HAVE_vcondv2sf (TARGET_NEON && (!true || flag_unsafe_math_optimizations))
#define HAVE_vcondv4sf (TARGET_NEON && (!true || flag_unsafe_math_optimizations))
#define HAVE_vconduv8qi (TARGET_NEON)
#define HAVE_vconduv16qi (TARGET_NEON)
#define HAVE_vconduv4hi (TARGET_NEON)
#define HAVE_vconduv8hi (TARGET_NEON)
#define HAVE_vconduv2si (TARGET_NEON)
#define HAVE_vconduv4si (TARGET_NEON)
#define HAVE_neon_vaddv8qi (TARGET_NEON)
#define HAVE_neon_vaddv16qi (TARGET_NEON)
#define HAVE_neon_vaddv4hi (TARGET_NEON)
#define HAVE_neon_vaddv8hi (TARGET_NEON)
#define HAVE_neon_vaddv2si (TARGET_NEON)
#define HAVE_neon_vaddv4si (TARGET_NEON)
#define HAVE_neon_vaddv2sf (TARGET_NEON)
#define HAVE_neon_vaddv4sf (TARGET_NEON)
#define HAVE_neon_vadddi (TARGET_NEON)
#define HAVE_neon_vaddv2di (TARGET_NEON)
#define HAVE_neon_vmlav8qi (TARGET_NEON)
#define HAVE_neon_vmlav16qi (TARGET_NEON)
#define HAVE_neon_vmlav4hi (TARGET_NEON)
#define HAVE_neon_vmlav8hi (TARGET_NEON)
#define HAVE_neon_vmlav2si (TARGET_NEON)
#define HAVE_neon_vmlav4si (TARGET_NEON)
#define HAVE_neon_vmlav2sf (TARGET_NEON)
#define HAVE_neon_vmlav4sf (TARGET_NEON)
#define HAVE_neon_vmlsv8qi (TARGET_NEON)
#define HAVE_neon_vmlsv16qi (TARGET_NEON)
#define HAVE_neon_vmlsv4hi (TARGET_NEON)
#define HAVE_neon_vmlsv8hi (TARGET_NEON)
#define HAVE_neon_vmlsv2si (TARGET_NEON)
#define HAVE_neon_vmlsv4si (TARGET_NEON)
#define HAVE_neon_vmlsv2sf (TARGET_NEON)
#define HAVE_neon_vmlsv4sf (TARGET_NEON)
#define HAVE_neon_vsubv8qi (TARGET_NEON)
#define HAVE_neon_vsubv16qi (TARGET_NEON)
#define HAVE_neon_vsubv4hi (TARGET_NEON)
#define HAVE_neon_vsubv8hi (TARGET_NEON)
#define HAVE_neon_vsubv2si (TARGET_NEON)
#define HAVE_neon_vsubv4si (TARGET_NEON)
#define HAVE_neon_vsubv2sf (TARGET_NEON)
#define HAVE_neon_vsubv4sf (TARGET_NEON)
#define HAVE_neon_vsubdi (TARGET_NEON)
#define HAVE_neon_vsubv2di (TARGET_NEON)
#define HAVE_neon_vpaddv8qi (TARGET_NEON)
#define HAVE_neon_vpaddv4hi (TARGET_NEON)
#define HAVE_neon_vpaddv2si (TARGET_NEON)
#define HAVE_neon_vpaddv2sf (TARGET_NEON)
#define HAVE_neon_vabsv8qi (TARGET_NEON)
#define HAVE_neon_vabsv16qi (TARGET_NEON)
#define HAVE_neon_vabsv4hi (TARGET_NEON)
#define HAVE_neon_vabsv8hi (TARGET_NEON)
#define HAVE_neon_vabsv2si (TARGET_NEON)
#define HAVE_neon_vabsv4si (TARGET_NEON)
#define HAVE_neon_vabsv2sf (TARGET_NEON)
#define HAVE_neon_vabsv4sf (TARGET_NEON)
#define HAVE_neon_vnegv8qi (TARGET_NEON)
#define HAVE_neon_vnegv16qi (TARGET_NEON)
#define HAVE_neon_vnegv4hi (TARGET_NEON)
#define HAVE_neon_vnegv8hi (TARGET_NEON)
#define HAVE_neon_vnegv2si (TARGET_NEON)
#define HAVE_neon_vnegv4si (TARGET_NEON)
#define HAVE_neon_vnegv2sf (TARGET_NEON)
#define HAVE_neon_vnegv4sf (TARGET_NEON)
#define HAVE_neon_vclzv8qi (TARGET_NEON)
#define HAVE_neon_vclzv16qi (TARGET_NEON)
#define HAVE_neon_vclzv4hi (TARGET_NEON)
#define HAVE_neon_vclzv8hi (TARGET_NEON)
#define HAVE_neon_vclzv2si (TARGET_NEON)
#define HAVE_neon_vclzv4si (TARGET_NEON)
#define HAVE_neon_vcntv8qi (TARGET_NEON)
#define HAVE_neon_vcntv16qi (TARGET_NEON)
#define HAVE_neon_vmvnv8qi (TARGET_NEON)
#define HAVE_neon_vmvnv16qi (TARGET_NEON)
#define HAVE_neon_vmvnv4hi (TARGET_NEON)
#define HAVE_neon_vmvnv8hi (TARGET_NEON)
#define HAVE_neon_vmvnv2si (TARGET_NEON)
#define HAVE_neon_vmvnv4si (TARGET_NEON)
#define HAVE_neon_vget_lanev8qi (TARGET_NEON)
#define HAVE_neon_vget_lanev16qi (TARGET_NEON)
#define HAVE_neon_vget_lanev4hi (TARGET_NEON)
#define HAVE_neon_vget_lanev8hi (TARGET_NEON)
#define HAVE_neon_vget_lanev2si (TARGET_NEON)
#define HAVE_neon_vget_lanev4si (TARGET_NEON)
#define HAVE_neon_vget_lanev2sf (TARGET_NEON)
#define HAVE_neon_vget_lanev4sf (TARGET_NEON)
#define HAVE_neon_vget_lanedi (TARGET_NEON)
#define HAVE_neon_vget_lanev2di (TARGET_NEON)
#define HAVE_neon_vset_lanev8qi (TARGET_NEON)
#define HAVE_neon_vset_lanev16qi (TARGET_NEON)
#define HAVE_neon_vset_lanev4hi (TARGET_NEON)
#define HAVE_neon_vset_lanev8hi (TARGET_NEON)
#define HAVE_neon_vset_lanev2si (TARGET_NEON)
#define HAVE_neon_vset_lanev4si (TARGET_NEON)
#define HAVE_neon_vset_lanev2sf (TARGET_NEON)
#define HAVE_neon_vset_lanev4sf (TARGET_NEON)
#define HAVE_neon_vset_lanev2di (TARGET_NEON)
#define HAVE_neon_vset_lanedi (TARGET_NEON)
#define HAVE_neon_vcreatev8qi (TARGET_NEON)
#define HAVE_neon_vcreatev4hi (TARGET_NEON)
#define HAVE_neon_vcreatev2si (TARGET_NEON)
#define HAVE_neon_vcreatev2sf (TARGET_NEON)
#define HAVE_neon_vcreatedi (TARGET_NEON)
#define HAVE_neon_vdup_ndi (TARGET_NEON)
#define HAVE_neon_vdup_lanev8qi (TARGET_NEON)
#define HAVE_neon_vdup_lanev16qi (TARGET_NEON)
#define HAVE_neon_vdup_lanev4hi (TARGET_NEON)
#define HAVE_neon_vdup_lanev8hi (TARGET_NEON)
#define HAVE_neon_vdup_lanev2si (TARGET_NEON)
#define HAVE_neon_vdup_lanev4si (TARGET_NEON)
#define HAVE_neon_vdup_lanev2sf (TARGET_NEON)
#define HAVE_neon_vdup_lanev4sf (TARGET_NEON)
#define HAVE_neon_vdup_lanedi (TARGET_NEON)
#define HAVE_neon_vdup_lanev2di (TARGET_NEON)
#define HAVE_neon_vmul_nv4hi (TARGET_NEON)
#define HAVE_neon_vmul_nv2si (TARGET_NEON)
#define HAVE_neon_vmul_nv2sf (TARGET_NEON)
#define HAVE_neon_vmul_nv8hi (TARGET_NEON)
#define HAVE_neon_vmul_nv4si (TARGET_NEON)
#define HAVE_neon_vmul_nv4sf (TARGET_NEON)
#define HAVE_neon_vmull_nv4hi (TARGET_NEON)
#define HAVE_neon_vmull_nv2si (TARGET_NEON)
#define HAVE_neon_vqdmull_nv4hi (TARGET_NEON)
#define HAVE_neon_vqdmull_nv2si (TARGET_NEON)
#define HAVE_neon_vqdmulh_nv4hi (TARGET_NEON)
#define HAVE_neon_vqdmulh_nv2si (TARGET_NEON)
#define HAVE_neon_vqdmulh_nv8hi (TARGET_NEON)
#define HAVE_neon_vqdmulh_nv4si (TARGET_NEON)
#define HAVE_neon_vmla_nv4hi (TARGET_NEON)
#define HAVE_neon_vmla_nv2si (TARGET_NEON)
#define HAVE_neon_vmla_nv2sf (TARGET_NEON)
#define HAVE_neon_vmla_nv8hi (TARGET_NEON)
#define HAVE_neon_vmla_nv4si (TARGET_NEON)
#define HAVE_neon_vmla_nv4sf (TARGET_NEON)
#define HAVE_neon_vmlal_nv4hi (TARGET_NEON)
#define HAVE_neon_vmlal_nv2si (TARGET_NEON)
#define HAVE_neon_vqdmlal_nv4hi (TARGET_NEON)
#define HAVE_neon_vqdmlal_nv2si (TARGET_NEON)
#define HAVE_neon_vmls_nv4hi (TARGET_NEON)
#define HAVE_neon_vmls_nv2si (TARGET_NEON)
#define HAVE_neon_vmls_nv2sf (TARGET_NEON)
#define HAVE_neon_vmls_nv8hi (TARGET_NEON)
#define HAVE_neon_vmls_nv4si (TARGET_NEON)
#define HAVE_neon_vmls_nv4sf (TARGET_NEON)
#define HAVE_neon_vmlsl_nv4hi (TARGET_NEON)
#define HAVE_neon_vmlsl_nv2si (TARGET_NEON)
#define HAVE_neon_vqdmlsl_nv4hi (TARGET_NEON)
#define HAVE_neon_vqdmlsl_nv2si (TARGET_NEON)
#define HAVE_neon_vbslv8qi (TARGET_NEON)
#define HAVE_neon_vbslv16qi (TARGET_NEON)
#define HAVE_neon_vbslv4hi (TARGET_NEON)
#define HAVE_neon_vbslv8hi (TARGET_NEON)
#define HAVE_neon_vbslv2si (TARGET_NEON)
#define HAVE_neon_vbslv4si (TARGET_NEON)
#define HAVE_neon_vbslv2sf (TARGET_NEON)
#define HAVE_neon_vbslv4sf (TARGET_NEON)
#define HAVE_neon_vbsldi (TARGET_NEON)
#define HAVE_neon_vbslv2di (TARGET_NEON)
#define HAVE_neon_vtrnv8qi (TARGET_NEON)
#define HAVE_neon_vtrnv16qi (TARGET_NEON)
#define HAVE_neon_vtrnv4hi (TARGET_NEON)
#define HAVE_neon_vtrnv8hi (TARGET_NEON)
#define HAVE_neon_vtrnv2si (TARGET_NEON)
#define HAVE_neon_vtrnv4si (TARGET_NEON)
#define HAVE_neon_vtrnv2sf (TARGET_NEON)
#define HAVE_neon_vtrnv4sf (TARGET_NEON)
#define HAVE_neon_vzipv8qi (TARGET_NEON)
#define HAVE_neon_vzipv16qi (TARGET_NEON)
#define HAVE_neon_vzipv4hi (TARGET_NEON)
#define HAVE_neon_vzipv8hi (TARGET_NEON)
#define HAVE_neon_vzipv2si (TARGET_NEON)
#define HAVE_neon_vzipv4si (TARGET_NEON)
#define HAVE_neon_vzipv2sf (TARGET_NEON)
#define HAVE_neon_vzipv4sf (TARGET_NEON)
#define HAVE_neon_vuzpv8qi (TARGET_NEON)
#define HAVE_neon_vuzpv16qi (TARGET_NEON)
#define HAVE_neon_vuzpv4hi (TARGET_NEON)
#define HAVE_neon_vuzpv8hi (TARGET_NEON)
#define HAVE_neon_vuzpv2si (TARGET_NEON)
#define HAVE_neon_vuzpv4si (TARGET_NEON)
#define HAVE_neon_vuzpv2sf (TARGET_NEON)
#define HAVE_neon_vuzpv4sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv8qiv8qi (TARGET_NEON)
#define HAVE_neon_vreinterpretv8qiv4hi (TARGET_NEON)
#define HAVE_neon_vreinterpretv8qiv2si (TARGET_NEON)
#define HAVE_neon_vreinterpretv8qiv2sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv8qidi (TARGET_NEON)
#define HAVE_neon_vreinterpretv4hiv8qi (TARGET_NEON)
#define HAVE_neon_vreinterpretv4hiv4hi (TARGET_NEON)
#define HAVE_neon_vreinterpretv4hiv2si (TARGET_NEON)
#define HAVE_neon_vreinterpretv4hiv2sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv4hidi (TARGET_NEON)
#define HAVE_neon_vreinterpretv2siv8qi (TARGET_NEON)
#define HAVE_neon_vreinterpretv2siv4hi (TARGET_NEON)
#define HAVE_neon_vreinterpretv2siv2si (TARGET_NEON)
#define HAVE_neon_vreinterpretv2siv2sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv2sidi (TARGET_NEON)
#define HAVE_neon_vreinterpretv2sfv8qi (TARGET_NEON)
#define HAVE_neon_vreinterpretv2sfv4hi (TARGET_NEON)
#define HAVE_neon_vreinterpretv2sfv2si (TARGET_NEON)
#define HAVE_neon_vreinterpretv2sfv2sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv2sfdi (TARGET_NEON)
#define HAVE_neon_vreinterpretdiv8qi (TARGET_NEON)
#define HAVE_neon_vreinterpretdiv4hi (TARGET_NEON)
#define HAVE_neon_vreinterpretdiv2si (TARGET_NEON)
#define HAVE_neon_vreinterpretdiv2sf (TARGET_NEON)
#define HAVE_neon_vreinterpretdidi (TARGET_NEON)
#define HAVE_neon_vreinterpretv16qiv16qi (TARGET_NEON)
#define HAVE_neon_vreinterpretv16qiv8hi (TARGET_NEON)
#define HAVE_neon_vreinterpretv16qiv4si (TARGET_NEON)
#define HAVE_neon_vreinterpretv16qiv4sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv16qiv2di (TARGET_NEON)
#define HAVE_neon_vreinterpretv8hiv16qi (TARGET_NEON)
#define HAVE_neon_vreinterpretv8hiv8hi (TARGET_NEON)
#define HAVE_neon_vreinterpretv8hiv4si (TARGET_NEON)
#define HAVE_neon_vreinterpretv8hiv4sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv8hiv2di (TARGET_NEON)
#define HAVE_neon_vreinterpretv4siv16qi (TARGET_NEON)
#define HAVE_neon_vreinterpretv4siv8hi (TARGET_NEON)
#define HAVE_neon_vreinterpretv4siv4si (TARGET_NEON)
#define HAVE_neon_vreinterpretv4siv4sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv4siv2di (TARGET_NEON)
#define HAVE_neon_vreinterpretv4sfv16qi (TARGET_NEON)
#define HAVE_neon_vreinterpretv4sfv8hi (TARGET_NEON)
#define HAVE_neon_vreinterpretv4sfv4si (TARGET_NEON)
#define HAVE_neon_vreinterpretv4sfv4sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv4sfv2di (TARGET_NEON)
#define HAVE_neon_vreinterpretv2div16qi (TARGET_NEON)
#define HAVE_neon_vreinterpretv2div8hi (TARGET_NEON)
#define HAVE_neon_vreinterpretv2div4si (TARGET_NEON)
#define HAVE_neon_vreinterpretv2div4sf (TARGET_NEON)
#define HAVE_neon_vreinterpretv2div2di (TARGET_NEON)
#define HAVE_neon_vld3v16qi (TARGET_NEON)
#define HAVE_neon_vld3v8hi (TARGET_NEON)
#define HAVE_neon_vld3v4si (TARGET_NEON)
#define HAVE_neon_vld3v4sf (TARGET_NEON)
#define HAVE_neon_vst3v16qi (TARGET_NEON)
#define HAVE_neon_vst3v8hi (TARGET_NEON)
#define HAVE_neon_vst3v4si (TARGET_NEON)
#define HAVE_neon_vst3v4sf (TARGET_NEON)
#define HAVE_neon_vld4v16qi (TARGET_NEON)
#define HAVE_neon_vld4v8hi (TARGET_NEON)
#define HAVE_neon_vld4v4si (TARGET_NEON)
#define HAVE_neon_vld4v4sf (TARGET_NEON)
#define HAVE_neon_vst4v16qi (TARGET_NEON)
#define HAVE_neon_vst4v8hi (TARGET_NEON)
#define HAVE_neon_vst4v4si (TARGET_NEON)
#define HAVE_neon_vst4v4sf (TARGET_NEON)
#define HAVE_neon_vandv8qi (TARGET_NEON)
#define HAVE_neon_vandv16qi (TARGET_NEON)
#define HAVE_neon_vandv4hi (TARGET_NEON)
#define HAVE_neon_vandv8hi (TARGET_NEON)
#define HAVE_neon_vandv2si (TARGET_NEON)
#define HAVE_neon_vandv4si (TARGET_NEON)
#define HAVE_neon_vandv2sf (TARGET_NEON)
#define HAVE_neon_vandv4sf (TARGET_NEON)
#define HAVE_neon_vanddi (TARGET_NEON)
#define HAVE_neon_vandv2di (TARGET_NEON)
#define HAVE_neon_vorrv8qi (TARGET_NEON)
#define HAVE_neon_vorrv16qi (TARGET_NEON)
#define HAVE_neon_vorrv4hi (TARGET_NEON)
#define HAVE_neon_vorrv8hi (TARGET_NEON)
#define HAVE_neon_vorrv2si (TARGET_NEON)
#define HAVE_neon_vorrv4si (TARGET_NEON)
#define HAVE_neon_vorrv2sf (TARGET_NEON)
#define HAVE_neon_vorrv4sf (TARGET_NEON)
#define HAVE_neon_vorrdi (TARGET_NEON)
#define HAVE_neon_vorrv2di (TARGET_NEON)
#define HAVE_neon_veorv8qi (TARGET_NEON)
#define HAVE_neon_veorv16qi (TARGET_NEON)
#define HAVE_neon_veorv4hi (TARGET_NEON)
#define HAVE_neon_veorv8hi (TARGET_NEON)
#define HAVE_neon_veorv2si (TARGET_NEON)
#define HAVE_neon_veorv4si (TARGET_NEON)
#define HAVE_neon_veorv2sf (TARGET_NEON)
#define HAVE_neon_veorv4sf (TARGET_NEON)
#define HAVE_neon_veordi (TARGET_NEON)
#define HAVE_neon_veorv2di (TARGET_NEON)
#define HAVE_neon_vbicv8qi (TARGET_NEON)
#define HAVE_neon_vbicv16qi (TARGET_NEON)
#define HAVE_neon_vbicv4hi (TARGET_NEON)
#define HAVE_neon_vbicv8hi (TARGET_NEON)
#define HAVE_neon_vbicv2si (TARGET_NEON)
#define HAVE_neon_vbicv4si (TARGET_NEON)
#define HAVE_neon_vbicv2sf (TARGET_NEON)
#define HAVE_neon_vbicv4sf (TARGET_NEON)
#define HAVE_neon_vbicdi (TARGET_NEON)
#define HAVE_neon_vbicv2di (TARGET_NEON)
#define HAVE_neon_vornv8qi (TARGET_NEON)
#define HAVE_neon_vornv16qi (TARGET_NEON)
#define HAVE_neon_vornv4hi (TARGET_NEON)
#define HAVE_neon_vornv8hi (TARGET_NEON)
#define HAVE_neon_vornv2si (TARGET_NEON)
#define HAVE_neon_vornv4si (TARGET_NEON)
#define HAVE_neon_vornv2sf (TARGET_NEON)
#define HAVE_neon_vornv4sf (TARGET_NEON)
#define HAVE_neon_vorndi (TARGET_NEON)
#define HAVE_neon_vornv2di (TARGET_NEON)
#define HAVE_vec_unpacks_hi_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacku_hi_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacks_hi_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacku_hi_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacks_hi_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacku_hi_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacks_lo_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacku_lo_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacks_lo_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacku_lo_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacks_lo_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacku_lo_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_smult_lo_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_umult_lo_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_smult_lo_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_umult_lo_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_smult_lo_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_umult_lo_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_smult_hi_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_umult_hi_v16qi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_smult_hi_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_umult_hi_v8hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_smult_hi_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_widen_umult_hi_v4si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_unpacks_lo_v8qi (TARGET_NEON)
#define HAVE_vec_unpacku_lo_v8qi (TARGET_NEON)
#define HAVE_vec_unpacks_lo_v4hi (TARGET_NEON)
#define HAVE_vec_unpacku_lo_v4hi (TARGET_NEON)
#define HAVE_vec_unpacks_lo_v2si (TARGET_NEON)
#define HAVE_vec_unpacku_lo_v2si (TARGET_NEON)
#define HAVE_vec_unpacks_hi_v8qi (TARGET_NEON)
#define HAVE_vec_unpacku_hi_v8qi (TARGET_NEON)
#define HAVE_vec_unpacks_hi_v4hi (TARGET_NEON)
#define HAVE_vec_unpacku_hi_v4hi (TARGET_NEON)
#define HAVE_vec_unpacks_hi_v2si (TARGET_NEON)
#define HAVE_vec_unpacku_hi_v2si (TARGET_NEON)
#define HAVE_vec_widen_smult_hi_v8qi (TARGET_NEON)
#define HAVE_vec_widen_umult_hi_v8qi (TARGET_NEON)
#define HAVE_vec_widen_smult_hi_v4hi (TARGET_NEON)
#define HAVE_vec_widen_umult_hi_v4hi (TARGET_NEON)
#define HAVE_vec_widen_smult_hi_v2si (TARGET_NEON)
#define HAVE_vec_widen_umult_hi_v2si (TARGET_NEON)
#define HAVE_vec_widen_smult_lo_v8qi (TARGET_NEON)
#define HAVE_vec_widen_umult_lo_v8qi (TARGET_NEON)
#define HAVE_vec_widen_smult_lo_v4hi (TARGET_NEON)
#define HAVE_vec_widen_umult_lo_v4hi (TARGET_NEON)
#define HAVE_vec_widen_smult_lo_v2si (TARGET_NEON)
#define HAVE_vec_widen_umult_lo_v2si (TARGET_NEON)
#define HAVE_vec_pack_trunc_v4hi (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_pack_trunc_v2si (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_vec_pack_trunc_di (TARGET_NEON && !BYTES_BIG_ENDIAN)
#define HAVE_memory_barrier (TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_compare_and_swapsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_compare_and_swapqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_compare_and_swaphi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_lock_test_and_setsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_lock_test_and_setqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_lock_test_and_sethi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_addsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_subsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_iorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_xorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_andsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_nandsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_addqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_subqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_iorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_xorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_andqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_addhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_subhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_iorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_xorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_andhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_nandqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_nandhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_addsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_subsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_iorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_xorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_andsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_nandsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_addqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_subqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_iorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_xorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_andqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_addhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_subhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_iorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_xorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_andhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_nandqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_new_nandhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_addsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_subsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_iorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_xorsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_andsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_nandsi (TARGET_HAVE_LDREX && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_addqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_subqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_iorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_xorqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_andqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_addhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_subhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_iorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_xorhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_andhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_nandqi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_sync_old_nandhi (TARGET_HAVE_LDREXBHD && TARGET_HAVE_MEMORY_BARRIER)
#define HAVE_mulqq3 (TARGET_DSP_MULTIPLY && arm_arch_thumb2)
#define HAVE_mulhq3 (TARGET_DSP_MULTIPLY && arm_arch_thumb2)
#define HAVE_mulsq3 (TARGET_32BIT && arm_arch3m)
#define HAVE_mulsa3 (TARGET_32BIT && arm_arch3m)
#define HAVE_mulusa3 (TARGET_32BIT && arm_arch3m)
#define HAVE_mulha3 (TARGET_DSP_MULTIPLY && arm_arch_thumb2)
#define HAVE_muluha3 (TARGET_DSP_MULTIPLY)
#define HAVE_ssmulha3 (TARGET_32BIT && arm_arch6)
#define HAVE_usmuluha3 (TARGET_32BIT && arm_arch6)
extern rtx        gen_mulhisi3                          (rtx, rtx, rtx);
extern rtx        gen_maddhisi4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_insv_zero                         (rtx, rtx, rtx);
extern rtx        gen_insv_t2                           (rtx, rtx, rtx, rtx);
extern rtx        gen_andsi_notsi_si                    (rtx, rtx, rtx);
extern rtx        gen_bicsi3                            (rtx, rtx, rtx);
extern rtx        gen_andsi_not_shiftsi_si              (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_arm_ashldi3_1bit                  (rtx, rtx);
extern rtx        gen_arm_ashrdi3_1bit                  (rtx, rtx);
extern rtx        gen_arm_lshrdi3_1bit                  (rtx, rtx);
extern rtx        gen_unaligned_loadsi                  (rtx, rtx);
extern rtx        gen_unaligned_loadhis                 (rtx, rtx);
extern rtx        gen_unaligned_loadhiu                 (rtx, rtx);
extern rtx        gen_unaligned_storesi                 (rtx, rtx);
extern rtx        gen_unaligned_storehi                 (rtx, rtx);
extern rtx        gen_extzv_t2                          (rtx, rtx, rtx, rtx);
extern rtx        gen_divsi3                            (rtx, rtx, rtx);
extern rtx        gen_udivsi3                           (rtx, rtx, rtx);
extern rtx        gen_one_cmpldi2                       (rtx, rtx);
extern rtx        gen_thumb1_extendhisi2                (rtx, rtx);
extern rtx        gen_thumb1_extendqisi2                (rtx, rtx);
extern rtx        gen_pic_load_addr_32bit               (rtx, rtx);
extern rtx        gen_pic_load_addr_thumb1              (rtx, rtx);
extern rtx        gen_pic_add_dot_plus_four             (rtx, rtx, rtx);
extern rtx        gen_pic_add_dot_plus_eight            (rtx, rtx, rtx);
extern rtx        gen_tls_load_dot_plus_eight           (rtx, rtx, rtx);
static inline rtx gen_pic_offset_arm                    (rtx, rtx, rtx);
static inline rtx
gen_pic_offset_arm(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_movmem12b                         (rtx, rtx, rtx, rtx);
extern rtx        gen_movmem8b                          (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchsi4_insn                   (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchsi4_scratch                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_cstoresi_nltu_thumb1              (rtx, rtx, rtx);
extern rtx        gen_cstoresi_ltu_thumb1               (rtx, rtx, rtx);
extern rtx        gen_thumb1_addsi3_addgeu              (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_blockage                          (void);
extern rtx        gen_arm_casesi_internal               (rtx, rtx, rtx, rtx);
extern rtx        gen_thumb1_casesi_dispatch            (rtx);
extern rtx        gen_nop                               (void);
extern rtx        gen_movcond                           (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sibcall_epilogue                  (void);
extern rtx        gen_stack_tie                         (rtx, rtx);
extern rtx        gen_align_4                           (void);
extern rtx        gen_align_8                           (void);
extern rtx        gen_align_16                          (void);
extern rtx        gen_align_32                          (void);
extern rtx        gen_consttable_end                    (void);
extern rtx        gen_consttable_1                      (rtx);
extern rtx        gen_consttable_2                      (rtx);
extern rtx        gen_consttable_4                      (rtx);
extern rtx        gen_consttable_8                      (rtx);
extern rtx        gen_consttable_16                     (rtx);
extern rtx        gen_clzsi2                            (rtx, rtx);
extern rtx        gen_rbitsi2                           (rtx, rtx);
extern rtx        gen_prefetch                          (rtx, rtx, rtx);
extern rtx        gen_prologue_use                      (rtx);
extern rtx        gen_arm_eh_return                     (rtx);
extern rtx        gen_thumb_eh_return                   (rtx);
extern rtx        gen_load_tp_hard                      (rtx);
extern rtx        gen_load_tp_soft                      (void);
extern rtx        gen_tlscall                           (rtx, rtx);
extern rtx        gen_cirrus_adddi3                     (rtx, rtx, rtx);
extern rtx        gen_cirrus_subdi3                     (rtx, rtx, rtx);
extern rtx        gen_muldi3                            (rtx, rtx, rtx);
static inline rtx gen_cirrus_ashl_const                 (rtx, rtx, rtx);
static inline rtx
gen_cirrus_ashl_const(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_cirrus_ashiftrt_const             (rtx, rtx, rtx);
static inline rtx
gen_cirrus_ashiftrt_const(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_cirrus_ashlsi3                    (rtx, rtx, rtx);
static inline rtx
gen_cirrus_ashlsi3(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_ashldi3_cirrus                    (rtx, rtx, rtx);
extern rtx        gen_cirrus_ashldi_const               (rtx, rtx, rtx);
extern rtx        gen_cirrus_ashiftrtdi_const           (rtx, rtx, rtx);
extern rtx        gen_cirrus_floatsisf2                 (rtx, rtx);
extern rtx        gen_cirrus_floatsidf2                 (rtx, rtx);
extern rtx        gen_floatdisf2                        (rtx, rtx);
extern rtx        gen_floatdidf2                        (rtx, rtx);
extern rtx        gen_cirrus_truncsfsi2                 (rtx, rtx);
extern rtx        gen_cirrus_truncdfsi2                 (rtx, rtx);
extern rtx        gen_iwmmxt_iordi3                     (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_xordi3                     (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_anddi3                     (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_nanddi3                    (rtx, rtx, rtx);
extern rtx        gen_movv2si_internal                  (rtx, rtx);
extern rtx        gen_movv4hi_internal                  (rtx, rtx);
extern rtx        gen_movv8qi_internal                  (rtx, rtx);
extern rtx        gen_ssaddv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_ssaddv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_ssaddv2si3                        (rtx, rtx, rtx);
extern rtx        gen_usaddv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_usaddv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_usaddv2si3                        (rtx, rtx, rtx);
extern rtx        gen_sssubv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_sssubv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_sssubv2si3                        (rtx, rtx, rtx);
extern rtx        gen_ussubv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_ussubv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_ussubv2si3                        (rtx, rtx, rtx);
extern rtx        gen_smulv4hi3_highpart                (rtx, rtx, rtx);
extern rtx        gen_umulv4hi3_highpart                (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wmacs                      (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wmacsz                     (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wmacu                      (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wmacuz                     (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_clrdi                      (rtx);
extern rtx        gen_iwmmxt_uavgrndv8qi3               (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_uavgrndv4hi3               (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_uavgv8qi3                  (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_uavgv4hi3                  (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_psadbw                     (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tinsrb                     (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tinsrh                     (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tinsrw                     (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_textrmub                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_textrmsb                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_textrmuh                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_textrmsh                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_textrmw                    (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wshufh                     (rtx, rtx, rtx);
extern rtx        gen_eqv8qi3                           (rtx, rtx, rtx);
extern rtx        gen_eqv4hi3                           (rtx, rtx, rtx);
extern rtx        gen_eqv2si3                           (rtx, rtx, rtx);
extern rtx        gen_gtuv8qi3                          (rtx, rtx, rtx);
extern rtx        gen_gtuv4hi3                          (rtx, rtx, rtx);
extern rtx        gen_gtuv2si3                          (rtx, rtx, rtx);
extern rtx        gen_gtv8qi3                           (rtx, rtx, rtx);
extern rtx        gen_gtv4hi3                           (rtx, rtx, rtx);
extern rtx        gen_gtv2si3                           (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wpackhss                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wpackwss                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wpackdss                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wpackhus                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wpackwus                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wpackdus                   (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wunpckihb                  (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wunpckihh                  (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wunpckihw                  (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wunpckilb                  (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wunpckilh                  (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wunpckilw                  (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wunpckehub                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckehuh                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckehuw                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckehsb                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckehsh                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckehsw                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckelub                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckeluh                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckeluw                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckelsb                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckelsh                 (rtx, rtx);
extern rtx        gen_iwmmxt_wunpckelsw                 (rtx, rtx);
extern rtx        gen_rorv4hi3                          (rtx, rtx, rtx);
extern rtx        gen_rorv2si3                          (rtx, rtx, rtx);
extern rtx        gen_rordi3                            (rtx, rtx, rtx);
extern rtx        gen_ashrv4hi3_iwmmxt                  (rtx, rtx, rtx);
extern rtx        gen_ashrv2si3_iwmmxt                  (rtx, rtx, rtx);
extern rtx        gen_ashrdi3_iwmmxt                    (rtx, rtx, rtx);
extern rtx        gen_lshrv4hi3_iwmmxt                  (rtx, rtx, rtx);
extern rtx        gen_lshrv2si3_iwmmxt                  (rtx, rtx, rtx);
extern rtx        gen_lshrdi3_iwmmxt                    (rtx, rtx, rtx);
extern rtx        gen_ashlv4hi3_iwmmxt                  (rtx, rtx, rtx);
extern rtx        gen_ashlv2si3_iwmmxt                  (rtx, rtx, rtx);
extern rtx        gen_ashldi3_iwmmxt                    (rtx, rtx, rtx);
extern rtx        gen_rorv4hi3_di                       (rtx, rtx, rtx);
extern rtx        gen_rorv2si3_di                       (rtx, rtx, rtx);
extern rtx        gen_rordi3_di                         (rtx, rtx, rtx);
extern rtx        gen_ashrv4hi3_di                      (rtx, rtx, rtx);
extern rtx        gen_ashrv2si3_di                      (rtx, rtx, rtx);
extern rtx        gen_ashrdi3_di                        (rtx, rtx, rtx);
extern rtx        gen_lshrv4hi3_di                      (rtx, rtx, rtx);
extern rtx        gen_lshrv2si3_di                      (rtx, rtx, rtx);
extern rtx        gen_lshrdi3_di                        (rtx, rtx, rtx);
extern rtx        gen_ashlv4hi3_di                      (rtx, rtx, rtx);
extern rtx        gen_ashlv2si3_di                      (rtx, rtx, rtx);
extern rtx        gen_ashldi3_di                        (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wmadds                     (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wmaddu                     (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tmia                       (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tmiaph                     (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tmiabb                     (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tmiatb                     (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tmiabt                     (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tmiatt                     (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tbcstqi                    (rtx, rtx);
extern rtx        gen_iwmmxt_tbcsthi                    (rtx, rtx);
extern rtx        gen_iwmmxt_tbcstsi                    (rtx, rtx);
extern rtx        gen_iwmmxt_tmovmskb                   (rtx, rtx);
extern rtx        gen_iwmmxt_tmovmskh                   (rtx, rtx);
extern rtx        gen_iwmmxt_tmovmskw                   (rtx, rtx);
extern rtx        gen_iwmmxt_waccb                      (rtx, rtx);
extern rtx        gen_iwmmxt_wacch                      (rtx, rtx);
extern rtx        gen_iwmmxt_waccw                      (rtx, rtx);
extern rtx        gen_iwmmxt_walign                     (rtx, rtx, rtx, rtx);
extern rtx        gen_iwmmxt_tmrc                       (rtx, rtx);
extern rtx        gen_iwmmxt_tmcr                       (rtx, rtx);
extern rtx        gen_iwmmxt_wsadb                      (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wsadh                      (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wsadbz                     (rtx, rtx, rtx);
extern rtx        gen_iwmmxt_wsadhz                     (rtx, rtx, rtx);
extern rtx        gen_extendhfsf2                       (rtx, rtx);
extern rtx        gen_truncsfhf2                        (rtx, rtx);
extern rtx        gen_fixuns_truncsfsi2                 (rtx, rtx);
extern rtx        gen_fixuns_truncdfsi2                 (rtx, rtx);
extern rtx        gen_floatunssisf2                     (rtx, rtx);
extern rtx        gen_floatunssidf2                     (rtx, rtx);
extern rtx        gen_tls_load_dot_plus_four            (rtx, rtx, rtx, rtx);
extern rtx        gen_thumb2_casesi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_thumb2_casesi_internal_pic        (rtx, rtx, rtx, rtx);
extern rtx        gen_thumb2_eh_return                  (rtx);
extern rtx        gen_orsi_notsi_si                     (rtx, rtx, rtx);
extern rtx        gen_vec_setv8qi_internal              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_setv4hi_internal              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_setv2si_internal              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_setv2sf_internal              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_setv16qi_internal             (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_setv8hi_internal              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_setv4si_internal              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_setv4sf_internal              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_setv2di_internal              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_extractv8qi                   (rtx, rtx, rtx);
extern rtx        gen_vec_extractv4hi                   (rtx, rtx, rtx);
extern rtx        gen_vec_extractv2si                   (rtx, rtx, rtx);
extern rtx        gen_vec_extractv2sf                   (rtx, rtx, rtx);
extern rtx        gen_vec_extractv16qi                  (rtx, rtx, rtx);
extern rtx        gen_vec_extractv8hi                   (rtx, rtx, rtx);
extern rtx        gen_vec_extractv4si                   (rtx, rtx, rtx);
extern rtx        gen_vec_extractv4sf                   (rtx, rtx, rtx);
extern rtx        gen_vec_extractv2di                   (rtx, rtx, rtx);
extern rtx        gen_adddi3_neon                       (rtx, rtx, rtx);
extern rtx        gen_subdi3_neon                       (rtx, rtx, rtx);
extern rtx        gen_mulv8qi3addv8qi_neon              (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv16qi3addv16qi_neon            (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv4hi3addv4hi_neon              (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv8hi3addv8hi_neon              (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv2si3addv2si_neon              (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv4si3addv4si_neon              (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv2sf3addv2sf_neon              (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv4sf3addv4sf_neon              (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv2di3addv2di_neon              (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv8qi3negv8qiaddv8qi_neon       (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv16qi3negv16qiaddv16qi_neon    (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv4hi3negv4hiaddv4hi_neon       (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv8hi3negv8hiaddv8hi_neon       (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv2si3negv2siaddv2si_neon       (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv4si3negv4siaddv4si_neon       (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv2sf3negv2sfaddv2sf_neon       (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv4sf3negv4sfaddv4sf_neon       (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv2di3negv2diaddv2di_neon       (rtx, rtx, rtx, rtx);
extern rtx        gen_iorv8qi3                          (rtx, rtx, rtx);
extern rtx        gen_iorv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_iorv4hi3                          (rtx, rtx, rtx);
extern rtx        gen_iorv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_iorv2si3                          (rtx, rtx, rtx);
extern rtx        gen_iorv4si3                          (rtx, rtx, rtx);
extern rtx        gen_iorv2sf3                          (rtx, rtx, rtx);
extern rtx        gen_iorv4sf3                          (rtx, rtx, rtx);
extern rtx        gen_iorv2di3                          (rtx, rtx, rtx);
extern rtx        gen_iordi3_neon                       (rtx, rtx, rtx);
extern rtx        gen_andv8qi3                          (rtx, rtx, rtx);
extern rtx        gen_andv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_andv4hi3                          (rtx, rtx, rtx);
extern rtx        gen_andv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_andv2si3                          (rtx, rtx, rtx);
extern rtx        gen_andv4si3                          (rtx, rtx, rtx);
extern rtx        gen_andv2sf3                          (rtx, rtx, rtx);
extern rtx        gen_andv4sf3                          (rtx, rtx, rtx);
extern rtx        gen_andv2di3                          (rtx, rtx, rtx);
extern rtx        gen_anddi3_neon                       (rtx, rtx, rtx);
extern rtx        gen_ornv8qi3_neon                     (rtx, rtx, rtx);
extern rtx        gen_ornv16qi3_neon                    (rtx, rtx, rtx);
extern rtx        gen_ornv4hi3_neon                     (rtx, rtx, rtx);
extern rtx        gen_ornv8hi3_neon                     (rtx, rtx, rtx);
extern rtx        gen_ornv2si3_neon                     (rtx, rtx, rtx);
extern rtx        gen_ornv4si3_neon                     (rtx, rtx, rtx);
extern rtx        gen_ornv2sf3_neon                     (rtx, rtx, rtx);
extern rtx        gen_ornv4sf3_neon                     (rtx, rtx, rtx);
extern rtx        gen_ornv2di3_neon                     (rtx, rtx, rtx);
extern rtx        gen_orndi3_neon                       (rtx, rtx, rtx);
extern rtx        gen_bicv8qi3_neon                     (rtx, rtx, rtx);
extern rtx        gen_bicv16qi3_neon                    (rtx, rtx, rtx);
extern rtx        gen_bicv4hi3_neon                     (rtx, rtx, rtx);
extern rtx        gen_bicv8hi3_neon                     (rtx, rtx, rtx);
extern rtx        gen_bicv2si3_neon                     (rtx, rtx, rtx);
extern rtx        gen_bicv4si3_neon                     (rtx, rtx, rtx);
extern rtx        gen_bicv2sf3_neon                     (rtx, rtx, rtx);
extern rtx        gen_bicv4sf3_neon                     (rtx, rtx, rtx);
extern rtx        gen_bicv2di3_neon                     (rtx, rtx, rtx);
extern rtx        gen_bicdi3_neon                       (rtx, rtx, rtx);
extern rtx        gen_xorv8qi3                          (rtx, rtx, rtx);
extern rtx        gen_xorv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_xorv4hi3                          (rtx, rtx, rtx);
extern rtx        gen_xorv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_xorv2si3                          (rtx, rtx, rtx);
extern rtx        gen_xorv4si3                          (rtx, rtx, rtx);
extern rtx        gen_xorv2sf3                          (rtx, rtx, rtx);
extern rtx        gen_xorv4sf3                          (rtx, rtx, rtx);
extern rtx        gen_xorv2di3                          (rtx, rtx, rtx);
extern rtx        gen_xordi3_neon                       (rtx, rtx, rtx);
extern rtx        gen_one_cmplv8qi2                     (rtx, rtx);
extern rtx        gen_one_cmplv16qi2                    (rtx, rtx);
extern rtx        gen_one_cmplv4hi2                     (rtx, rtx);
extern rtx        gen_one_cmplv8hi2                     (rtx, rtx);
extern rtx        gen_one_cmplv2si2                     (rtx, rtx);
extern rtx        gen_one_cmplv4si2                     (rtx, rtx);
extern rtx        gen_one_cmplv2sf2                     (rtx, rtx);
extern rtx        gen_one_cmplv4sf2                     (rtx, rtx);
extern rtx        gen_one_cmplv2di2                     (rtx, rtx);
extern rtx        gen_absv8qi2                          (rtx, rtx);
extern rtx        gen_absv16qi2                         (rtx, rtx);
extern rtx        gen_absv4hi2                          (rtx, rtx);
extern rtx        gen_absv8hi2                          (rtx, rtx);
extern rtx        gen_absv2si2                          (rtx, rtx);
extern rtx        gen_absv4si2                          (rtx, rtx);
extern rtx        gen_absv2sf2                          (rtx, rtx);
extern rtx        gen_absv4sf2                          (rtx, rtx);
extern rtx        gen_negv8qi2                          (rtx, rtx);
extern rtx        gen_negv16qi2                         (rtx, rtx);
extern rtx        gen_negv4hi2                          (rtx, rtx);
extern rtx        gen_negv8hi2                          (rtx, rtx);
extern rtx        gen_negv2si2                          (rtx, rtx);
extern rtx        gen_negv4si2                          (rtx, rtx);
extern rtx        gen_negv2sf2                          (rtx, rtx);
extern rtx        gen_negv4sf2                          (rtx, rtx);
extern rtx        gen_vashlv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_vashlv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_vashlv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_vashlv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_vashlv2si3                        (rtx, rtx, rtx);
extern rtx        gen_vashlv4si3                        (rtx, rtx, rtx);
extern rtx        gen_ashlv8qi3_signed                  (rtx, rtx, rtx);
extern rtx        gen_ashlv16qi3_signed                 (rtx, rtx, rtx);
extern rtx        gen_ashlv4hi3_signed                  (rtx, rtx, rtx);
extern rtx        gen_ashlv8hi3_signed                  (rtx, rtx, rtx);
extern rtx        gen_ashlv2si3_signed                  (rtx, rtx, rtx);
extern rtx        gen_ashlv4si3_signed                  (rtx, rtx, rtx);
extern rtx        gen_ashlv2di3_signed                  (rtx, rtx, rtx);
extern rtx        gen_ashlv8qi3_unsigned                (rtx, rtx, rtx);
extern rtx        gen_ashlv16qi3_unsigned               (rtx, rtx, rtx);
extern rtx        gen_ashlv4hi3_unsigned                (rtx, rtx, rtx);
extern rtx        gen_ashlv8hi3_unsigned                (rtx, rtx, rtx);
extern rtx        gen_ashlv2si3_unsigned                (rtx, rtx, rtx);
extern rtx        gen_ashlv4si3_unsigned                (rtx, rtx, rtx);
extern rtx        gen_ashlv2di3_unsigned                (rtx, rtx, rtx);
extern rtx        gen_widen_ssumv8qi3                   (rtx, rtx, rtx);
extern rtx        gen_widen_ssumv4hi3                   (rtx, rtx, rtx);
extern rtx        gen_widen_ssumv2si3                   (rtx, rtx, rtx);
extern rtx        gen_widen_usumv8qi3                   (rtx, rtx, rtx);
extern rtx        gen_widen_usumv4hi3                   (rtx, rtx, rtx);
extern rtx        gen_widen_usumv2si3                   (rtx, rtx, rtx);
extern rtx        gen_quad_halves_plusv4si              (rtx, rtx);
extern rtx        gen_quad_halves_sminv4si              (rtx, rtx);
extern rtx        gen_quad_halves_smaxv4si              (rtx, rtx);
extern rtx        gen_quad_halves_uminv4si              (rtx, rtx);
extern rtx        gen_quad_halves_umaxv4si              (rtx, rtx);
extern rtx        gen_quad_halves_plusv4sf              (rtx, rtx);
extern rtx        gen_quad_halves_sminv4sf              (rtx, rtx);
extern rtx        gen_quad_halves_smaxv4sf              (rtx, rtx);
extern rtx        gen_quad_halves_plusv8hi              (rtx, rtx);
extern rtx        gen_quad_halves_sminv8hi              (rtx, rtx);
extern rtx        gen_quad_halves_smaxv8hi              (rtx, rtx);
extern rtx        gen_quad_halves_uminv8hi              (rtx, rtx);
extern rtx        gen_quad_halves_umaxv8hi              (rtx, rtx);
extern rtx        gen_quad_halves_plusv16qi             (rtx, rtx);
extern rtx        gen_quad_halves_sminv16qi             (rtx, rtx);
extern rtx        gen_quad_halves_smaxv16qi             (rtx, rtx);
extern rtx        gen_quad_halves_uminv16qi             (rtx, rtx);
extern rtx        gen_quad_halves_umaxv16qi             (rtx, rtx);
extern rtx        gen_neon_move_lo_quad_v2di            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_lo_quad_v2df            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_lo_quad_v16qi           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_lo_quad_v8hi            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_lo_quad_v4si            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_lo_quad_v4sf            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_hi_quad_v2di            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_hi_quad_v2df            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_hi_quad_v16qi           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_hi_quad_v8hi            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_hi_quad_v4si            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_move_hi_quad_v4sf            (rtx, rtx, rtx, rtx);
extern rtx        gen_reduc_splus_v2di                  (rtx, rtx);
extern rtx        gen_neon_vpadd_internalv8qi           (rtx, rtx, rtx);
extern rtx        gen_neon_vpadd_internalv4hi           (rtx, rtx, rtx);
extern rtx        gen_neon_vpadd_internalv2si           (rtx, rtx, rtx);
extern rtx        gen_neon_vpadd_internalv2sf           (rtx, rtx, rtx);
extern rtx        gen_neon_vpsminv8qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpsminv4hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpsminv2si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpsminv2sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpsmaxv8qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpsmaxv4hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpsmaxv2si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpsmaxv2sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpuminv8qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpuminv4hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpuminv2si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpumaxv8qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpumaxv4hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpumaxv2si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vaddv8qi_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vaddv16qi_unspec             (rtx, rtx, rtx);
extern rtx        gen_neon_vaddv4hi_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vaddv8hi_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vaddv2si_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vaddv4si_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vaddv2sf_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vaddv4sf_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vadddi_unspec                (rtx, rtx, rtx);
extern rtx        gen_neon_vaddv2di_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vaddlv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddlv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddlv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddwv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddwv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddwv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhaddv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhaddv16qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhaddv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhaddv8hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhaddv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhaddv4si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqaddv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqaddv16qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqaddv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqaddv8hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqaddv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqaddv4si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqadddi                      (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqaddv2di                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddhnv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddhnv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddhnv2di                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmulv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmulv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmulv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmulv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmulv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmulv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmulv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmulv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav8qi_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav16qi_unspec             (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav4hi_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav8hi_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav2si_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav4si_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav2sf_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav4sf_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav2di_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlalv8qi                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlalv4hi                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlalv2si                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv8qi_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv16qi_unspec             (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv4hi_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv8hi_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv2si_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv4si_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv2sf_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv4sf_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv2di_unspec              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlslv8qi                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlslv4hi                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlslv2si                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulhv4hi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulhv2si                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulhv8hi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulhv4si                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlalv4hi                  (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlalv2si                  (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlslv4hi                  (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlslv2si                  (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmullv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmullv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmullv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmullv4hi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmullv2si                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv8qi_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vsubv16qi_unspec             (rtx, rtx, rtx);
extern rtx        gen_neon_vsubv4hi_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vsubv8hi_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vsubv2si_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vsubv4si_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vsubv2sf_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vsubv4sf_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vsubdi_unspec                (rtx, rtx, rtx);
extern rtx        gen_neon_vsubv2di_unspec              (rtx, rtx, rtx);
extern rtx        gen_neon_vsublv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsublv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsublv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubwv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubwv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubwv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqsubv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqsubv16qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqsubv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqsubv8hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqsubv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqsubv4si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqsubdi                      (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqsubv2di                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhsubv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhsubv16qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhsubv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhsubv8hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhsubv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vhsubv4si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubhnv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubhnv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubhnv2di                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vceqv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vceqv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vceqv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vceqv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vceqv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vceqv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vceqv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vceqv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgev8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgev16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgev4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgev8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgev2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgev4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgev2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgev4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgtv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgtv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgtv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgtv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgtv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgtv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgtv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcgtv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vclev8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vclev16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vclev4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vclev8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vclev2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vclev4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vclev2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vclev4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcltv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcltv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcltv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcltv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcltv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcltv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcltv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcltv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcagev2sf                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcagev4sf                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcagtv2sf                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcagtv4sf                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtstv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtstv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtstv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtstv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtstv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtstv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdlv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdlv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabdlv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabav8qi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabav16qi                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabav4hi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabav8hi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabav2si                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabav4si                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabalv8qi                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabalv4hi                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabalv2si                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmaxv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmaxv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmaxv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmaxv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmaxv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmaxv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmaxv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmaxv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vminv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vminv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vminv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vminv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vminv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vminv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vminv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vminv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpaddlv8qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpaddlv16qi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vpaddlv4hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpaddlv8hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpaddlv2si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpaddlv4si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vpadalv8qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpadalv16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpadalv4hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpadalv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpadalv2si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpadalv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpmaxv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpmaxv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpmaxv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpmaxv2sf                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpminv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpminv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpminv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpminv2sf                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vrecpsv2sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vrecpsv4sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vrsqrtsv2sf                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vrsqrtsv4sf                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqabsv8qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqabsv16qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vqabsv4hi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqabsv8hi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqabsv2si                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqabsv4si                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqnegv8qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqnegv16qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vqnegv4hi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqnegv8hi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqnegv2si                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqnegv4si                    (rtx, rtx, rtx);
extern rtx        gen_neon_vclsv8qi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vclsv16qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vclsv4hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vclsv8hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vclsv2si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vclsv4si                     (rtx, rtx, rtx);
extern rtx        gen_clzv8qi2                          (rtx, rtx);
extern rtx        gen_clzv16qi2                         (rtx, rtx);
extern rtx        gen_clzv4hi2                          (rtx, rtx);
extern rtx        gen_clzv8hi2                          (rtx, rtx);
extern rtx        gen_clzv2si2                          (rtx, rtx);
extern rtx        gen_clzv4si2                          (rtx, rtx);
extern rtx        gen_popcountv8qi2                     (rtx, rtx);
extern rtx        gen_popcountv16qi2                    (rtx, rtx);
extern rtx        gen_neon_vrecpev2si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrecpev2sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrecpev4si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrecpev4sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrsqrtev2si                  (rtx, rtx, rtx);
extern rtx        gen_neon_vrsqrtev2sf                  (rtx, rtx, rtx);
extern rtx        gen_neon_vrsqrtev4si                  (rtx, rtx, rtx);
extern rtx        gen_neon_vrsqrtev4sf                  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev8qi_sext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev4hi_sext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev2si_sext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev2sf_sext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev8qi_zext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev4hi_zext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev2si_zext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev2sf_zext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev16qi_sext_internal (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev8hi_sext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev4si_sext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev4sf_sext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev16qi_zext_internal (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev8hi_zext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev4si_zext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev4sf_zext_internal  (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_nv8qi                   (rtx, rtx);
extern rtx        gen_neon_vdup_nv4hi                   (rtx, rtx);
extern rtx        gen_neon_vdup_nv16qi                  (rtx, rtx);
extern rtx        gen_neon_vdup_nv8hi                   (rtx, rtx);
extern rtx        gen_neon_vdup_nv2si                   (rtx, rtx);
extern rtx        gen_neon_vdup_nv2sf                   (rtx, rtx);
extern rtx        gen_neon_vdup_nv4si                   (rtx, rtx);
extern rtx        gen_neon_vdup_nv4sf                   (rtx, rtx);
extern rtx        gen_neon_vdup_nv2di                   (rtx, rtx);
extern rtx        gen_neon_vdup_lanev8qi_internal       (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev16qi_internal      (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev4hi_internal       (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev8hi_internal       (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev2si_internal       (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev4si_internal       (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev2sf_internal       (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev4sf_internal       (rtx, rtx, rtx);
extern rtx        gen_neon_vcombinev8qi                 (rtx, rtx, rtx);
extern rtx        gen_neon_vcombinev4hi                 (rtx, rtx, rtx);
extern rtx        gen_neon_vcombinev2si                 (rtx, rtx, rtx);
extern rtx        gen_neon_vcombinev2sf                 (rtx, rtx, rtx);
extern rtx        gen_neon_vcombinedi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vget_highv16qi               (rtx, rtx);
extern rtx        gen_neon_vget_highv8hi                (rtx, rtx);
extern rtx        gen_neon_vget_highv4si                (rtx, rtx);
extern rtx        gen_neon_vget_highv4sf                (rtx, rtx);
extern rtx        gen_neon_vget_highv2di                (rtx, rtx);
extern rtx        gen_neon_vget_lowv16qi                (rtx, rtx);
extern rtx        gen_neon_vget_lowv8hi                 (rtx, rtx);
extern rtx        gen_neon_vget_lowv4si                 (rtx, rtx);
extern rtx        gen_neon_vget_lowv4sf                 (rtx, rtx);
extern rtx        gen_neon_vget_lowv2di                 (rtx, rtx);
extern rtx        gen_neon_vcvtv2sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vcvtv4sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vcvtv2si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vcvtv4si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vcvt_nv2sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcvt_nv4sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcvt_nv2si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcvt_nv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmovnv8hi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vmovnv4si                    (rtx, rtx, rtx);
extern rtx        gen_neon_vmovnv2di                    (rtx, rtx, rtx);
extern rtx        gen_neon_vqmovnv8hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vqmovnv4si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vqmovnv2di                   (rtx, rtx, rtx);
extern rtx        gen_neon_vqmovunv8hi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vqmovunv4si                  (rtx, rtx, rtx);
extern rtx        gen_neon_vqmovunv2di                  (rtx, rtx, rtx);
extern rtx        gen_neon_vmovlv8qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vmovlv4hi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vmovlv2si                    (rtx, rtx, rtx);
extern rtx        gen_neon_vmul_lanev4hi                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_lanev2si                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_lanev2sf                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_lanev8hi                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_lanev4si                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_lanev4sf                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmull_lanev4hi               (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmull_lanev2si               (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmull_lanev4hi             (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmull_lanev2si             (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulh_lanev8hi             (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulh_lanev4si             (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulh_lanev4hi             (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulh_lanev2si             (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_lanev4hi                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_lanev2si                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_lanev2sf                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_lanev8hi                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_lanev4si                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_lanev4sf                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlal_lanev4hi               (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlal_lanev2si               (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlal_lanev4hi             (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlal_lanev2si             (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_lanev4hi                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_lanev2si                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_lanev2sf                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_lanev8hi                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_lanev4si                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_lanev4sf                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsl_lanev4hi               (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsl_lanev2si               (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlsl_lanev4hi             (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlsl_lanev2si             (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextdi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vextv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vrev64v8qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev64v16qi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vrev64v4hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev64v8hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev64v2si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev64v4si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev64v2sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev64v4sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev64v2di                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev32v8qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev32v4hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev32v16qi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vrev32v8hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev16v8qi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vrev16v16qi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vbslv8qi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv16qi_internal           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv4hi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv8hi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv2si_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv4si_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv2sf_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv4sf_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbsldi_internal              (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv2di_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshlv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshlv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshlv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshlv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshlv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshlv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshldi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshlv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlv16qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlv8hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlv4si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshldi                      (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlv2di                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshr_nv8qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshr_nv16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshr_nv4hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshr_nv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshr_nv2si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshr_nv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshr_ndi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshr_nv2di                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshrn_nv8hi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshrn_nv4si                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshrn_nv2di                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshrn_nv8hi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshrn_nv4si                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshrn_nv2di                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshrun_nv8hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshrun_nv4si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshrun_nv2di                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshl_nv8qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshl_nv16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshl_nv4hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshl_nv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshl_nv2si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshl_nv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshl_ndi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshl_nv2di                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshl_nv8qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshl_nv16qi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshl_nv4hi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshl_nv8hi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshl_nv2si                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshl_nv4si                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshl_ndi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshl_nv2di                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlu_nv8qi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlu_nv16qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlu_nv4hi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlu_nv8hi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlu_nv2si                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlu_nv4si                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlu_ndi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqshlu_nv2di                 (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshll_nv8qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshll_nv4hi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vshll_nv2si                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsra_nv8qi                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsra_nv16qi                  (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsra_nv4hi                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsra_nv8hi                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsra_nv2si                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsra_nv4si                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsra_ndi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsra_nv2di                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsri_nv8qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsri_nv16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsri_nv4hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsri_nv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsri_nv2si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsri_nv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsri_ndi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsri_nv2di                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsli_nv8qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsli_nv16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsli_nv4hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsli_nv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsli_nv2si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsli_nv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsli_ndi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsli_nv2di                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtbl1v8qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vtbl2v8qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vtbl3v8qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vtbl4v8qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vtbx1v8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtbx2v8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtbx3v8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtbx4v8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv8qi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv16qi_internal           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv4hi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv8hi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv2si_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv4si_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv2sf_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv4sf_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vzipv8qi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vzipv16qi_internal           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vzipv4hi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vzipv8hi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vzipv2si_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vzipv4si_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vzipv2sf_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vzipv4sf_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv8qi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv16qi_internal           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv4hi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv8hi_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv2si_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv4si_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv2sf_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv4sf_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1v8qi                     (rtx, rtx);
extern rtx        gen_neon_vld1v16qi                    (rtx, rtx);
extern rtx        gen_neon_vld1v4hi                     (rtx, rtx);
extern rtx        gen_neon_vld1v8hi                     (rtx, rtx);
extern rtx        gen_neon_vld1v2si                     (rtx, rtx);
extern rtx        gen_neon_vld1v4si                     (rtx, rtx);
extern rtx        gen_neon_vld1v2sf                     (rtx, rtx);
extern rtx        gen_neon_vld1v4sf                     (rtx, rtx);
extern rtx        gen_neon_vld1di                       (rtx, rtx);
extern rtx        gen_neon_vld1v2di                     (rtx, rtx);
extern rtx        gen_neon_vld1_lanev8qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_lanev4hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_lanev2si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_lanev2sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_lanedi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_lanev16qi               (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_lanev8hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_lanev4si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_lanev4sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_lanev2di                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld1_dupv8qi                 (rtx, rtx);
extern rtx        gen_neon_vld1_dupv4hi                 (rtx, rtx);
extern rtx        gen_neon_vld1_dupv2si                 (rtx, rtx);
extern rtx        gen_neon_vld1_dupv2sf                 (rtx, rtx);
extern rtx        gen_neon_vld1_dupdi                   (rtx, rtx);
extern rtx        gen_neon_vld1_dupv16qi                (rtx, rtx);
extern rtx        gen_neon_vld1_dupv8hi                 (rtx, rtx);
extern rtx        gen_neon_vld1_dupv4si                 (rtx, rtx);
extern rtx        gen_neon_vld1_dupv4sf                 (rtx, rtx);
extern rtx        gen_neon_vld1_dupv2di                 (rtx, rtx);
extern rtx        gen_neon_vst1v8qi                     (rtx, rtx);
extern rtx        gen_neon_vst1v16qi                    (rtx, rtx);
extern rtx        gen_neon_vst1v4hi                     (rtx, rtx);
extern rtx        gen_neon_vst1v8hi                     (rtx, rtx);
extern rtx        gen_neon_vst1v2si                     (rtx, rtx);
extern rtx        gen_neon_vst1v4si                     (rtx, rtx);
extern rtx        gen_neon_vst1v2sf                     (rtx, rtx);
extern rtx        gen_neon_vst1v4sf                     (rtx, rtx);
extern rtx        gen_neon_vst1di                       (rtx, rtx);
extern rtx        gen_neon_vst1v2di                     (rtx, rtx);
extern rtx        gen_neon_vst1_lanev8qi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst1_lanev4hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst1_lanev2si                (rtx, rtx, rtx);
extern rtx        gen_neon_vst1_lanev2sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vst1_lanedi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vst1_lanev16qi               (rtx, rtx, rtx);
extern rtx        gen_neon_vst1_lanev8hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst1_lanev4si                (rtx, rtx, rtx);
extern rtx        gen_neon_vst1_lanev4sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vst1_lanev2di                (rtx, rtx, rtx);
extern rtx        gen_neon_vld2v8qi                     (rtx, rtx);
extern rtx        gen_neon_vld2v4hi                     (rtx, rtx);
extern rtx        gen_neon_vld2v2si                     (rtx, rtx);
extern rtx        gen_neon_vld2v2sf                     (rtx, rtx);
extern rtx        gen_neon_vld2di                       (rtx, rtx);
extern rtx        gen_neon_vld2v16qi                    (rtx, rtx);
extern rtx        gen_neon_vld2v8hi                     (rtx, rtx);
extern rtx        gen_neon_vld2v4si                     (rtx, rtx);
extern rtx        gen_neon_vld2v4sf                     (rtx, rtx);
extern rtx        gen_neon_vld2_lanev8qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld2_lanev4hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld2_lanev2si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld2_lanev2sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld2_lanev8hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld2_lanev4si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld2_lanev4sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld2_dupv8qi                 (rtx, rtx);
extern rtx        gen_neon_vld2_dupv4hi                 (rtx, rtx);
extern rtx        gen_neon_vld2_dupv2si                 (rtx, rtx);
extern rtx        gen_neon_vld2_dupv2sf                 (rtx, rtx);
extern rtx        gen_neon_vld2_dupdi                   (rtx, rtx);
extern rtx        gen_neon_vst2v8qi                     (rtx, rtx);
extern rtx        gen_neon_vst2v4hi                     (rtx, rtx);
extern rtx        gen_neon_vst2v2si                     (rtx, rtx);
extern rtx        gen_neon_vst2v2sf                     (rtx, rtx);
extern rtx        gen_neon_vst2di                       (rtx, rtx);
extern rtx        gen_neon_vst2v16qi                    (rtx, rtx);
extern rtx        gen_neon_vst2v8hi                     (rtx, rtx);
extern rtx        gen_neon_vst2v4si                     (rtx, rtx);
extern rtx        gen_neon_vst2v4sf                     (rtx, rtx);
extern rtx        gen_neon_vst2_lanev8qi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst2_lanev4hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst2_lanev2si                (rtx, rtx, rtx);
extern rtx        gen_neon_vst2_lanev2sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vst2_lanev8hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst2_lanev4si                (rtx, rtx, rtx);
extern rtx        gen_neon_vst2_lanev4sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vld3v8qi                     (rtx, rtx);
extern rtx        gen_neon_vld3v4hi                     (rtx, rtx);
extern rtx        gen_neon_vld3v2si                     (rtx, rtx);
extern rtx        gen_neon_vld3v2sf                     (rtx, rtx);
extern rtx        gen_neon_vld3di                       (rtx, rtx);
extern rtx        gen_neon_vld3qav16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3qav8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3qav4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3qav4sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3qbv16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3qbv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3qbv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3qbv4sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3_lanev8qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3_lanev4hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3_lanev2si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3_lanev2sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3_lanev8hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3_lanev4si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3_lanev4sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld3_dupv8qi                 (rtx, rtx);
extern rtx        gen_neon_vld3_dupv4hi                 (rtx, rtx);
extern rtx        gen_neon_vld3_dupv2si                 (rtx, rtx);
extern rtx        gen_neon_vld3_dupv2sf                 (rtx, rtx);
extern rtx        gen_neon_vld3_dupdi                   (rtx, rtx);
extern rtx        gen_neon_vst3v8qi                     (rtx, rtx);
extern rtx        gen_neon_vst3v4hi                     (rtx, rtx);
extern rtx        gen_neon_vst3v2si                     (rtx, rtx);
extern rtx        gen_neon_vst3v2sf                     (rtx, rtx);
extern rtx        gen_neon_vst3di                       (rtx, rtx);
extern rtx        gen_neon_vst3qav16qi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vst3qav8hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst3qav4si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst3qav4sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst3qbv16qi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vst3qbv8hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst3qbv4si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst3qbv4sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst3_lanev8qi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst3_lanev4hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst3_lanev2si                (rtx, rtx, rtx);
extern rtx        gen_neon_vst3_lanev2sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vst3_lanev8hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst3_lanev4si                (rtx, rtx, rtx);
extern rtx        gen_neon_vst3_lanev4sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vld4v8qi                     (rtx, rtx);
extern rtx        gen_neon_vld4v4hi                     (rtx, rtx);
extern rtx        gen_neon_vld4v2si                     (rtx, rtx);
extern rtx        gen_neon_vld4v2sf                     (rtx, rtx);
extern rtx        gen_neon_vld4di                       (rtx, rtx);
extern rtx        gen_neon_vld4qav16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4qav8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4qav4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4qav4sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4qbv16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4qbv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4qbv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4qbv4sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4_lanev8qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4_lanev4hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4_lanev2si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4_lanev2sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4_lanev8hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4_lanev4si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4_lanev4sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vld4_dupv8qi                 (rtx, rtx);
extern rtx        gen_neon_vld4_dupv4hi                 (rtx, rtx);
extern rtx        gen_neon_vld4_dupv2si                 (rtx, rtx);
extern rtx        gen_neon_vld4_dupv2sf                 (rtx, rtx);
extern rtx        gen_neon_vld4_dupdi                   (rtx, rtx);
extern rtx        gen_neon_vst4v8qi                     (rtx, rtx);
extern rtx        gen_neon_vst4v4hi                     (rtx, rtx);
extern rtx        gen_neon_vst4v2si                     (rtx, rtx);
extern rtx        gen_neon_vst4v2sf                     (rtx, rtx);
extern rtx        gen_neon_vst4di                       (rtx, rtx);
extern rtx        gen_neon_vst4qav16qi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vst4qav8hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst4qav4si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst4qav4sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst4qbv16qi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vst4qbv8hi                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst4qbv4si                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst4qbv4sf                   (rtx, rtx, rtx);
extern rtx        gen_neon_vst4_lanev8qi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst4_lanev4hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst4_lanev2si                (rtx, rtx, rtx);
extern rtx        gen_neon_vst4_lanev2sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vst4_lanev8hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vst4_lanev4si                (rtx, rtx, rtx);
extern rtx        gen_neon_vst4_lanev4sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacks_lo_v16qi         (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacku_lo_v16qi         (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacks_lo_v8hi          (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacku_lo_v8hi          (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacks_lo_v4si          (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacku_lo_v4si          (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacks_hi_v16qi         (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacku_hi_v16qi         (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacks_hi_v8hi          (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacku_hi_v8hi          (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacks_hi_v4si          (rtx, rtx, rtx);
extern rtx        gen_neon_vec_unpacku_hi_v4si          (rtx, rtx, rtx);
extern rtx        gen_neon_vec_smult_lo_v16qi           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_umult_lo_v16qi           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_smult_lo_v8hi            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_umult_lo_v8hi            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_smult_lo_v4si            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_umult_lo_v4si            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_smult_hi_v16qi           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_umult_hi_v16qi           (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_smult_hi_v8hi            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_umult_hi_v8hi            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_smult_hi_v4si            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vec_umult_hi_v4si            (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_unpacks_v8qi                 (rtx, rtx);
extern rtx        gen_neon_unpacku_v8qi                 (rtx, rtx);
extern rtx        gen_neon_unpacks_v4hi                 (rtx, rtx);
extern rtx        gen_neon_unpacku_v4hi                 (rtx, rtx);
extern rtx        gen_neon_unpacks_v2si                 (rtx, rtx);
extern rtx        gen_neon_unpacku_v2si                 (rtx, rtx);
extern rtx        gen_neon_vec_smult_v8qi               (rtx, rtx, rtx);
extern rtx        gen_neon_vec_umult_v8qi               (rtx, rtx, rtx);
extern rtx        gen_neon_vec_smult_v4hi               (rtx, rtx, rtx);
extern rtx        gen_neon_vec_umult_v4hi               (rtx, rtx, rtx);
extern rtx        gen_neon_vec_smult_v2si               (rtx, rtx, rtx);
extern rtx        gen_neon_vec_umult_v2si               (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v8hi               (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v4si               (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v2di               (rtx, rtx, rtx);
extern rtx        gen_neon_vec_pack_trunc_v8hi          (rtx, rtx);
extern rtx        gen_neon_vec_pack_trunc_v4si          (rtx, rtx);
extern rtx        gen_neon_vec_pack_trunc_v2di          (rtx, rtx);
extern rtx        gen_arm_sync_compare_and_swapsi       (rtx, rtx, rtx, rtx);
extern rtx        gen_arm_sync_compare_and_swapqi       (rtx, rtx, rtx, rtx);
extern rtx        gen_arm_sync_compare_and_swaphi       (rtx, rtx, rtx, rtx);
extern rtx        gen_arm_sync_lock_test_and_setsi      (rtx, rtx, rtx);
extern rtx        gen_arm_sync_lock_test_and_setqi      (rtx, rtx, rtx);
extern rtx        gen_arm_sync_lock_test_and_sethi      (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_addsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_subsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_iorsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_xorsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_andsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_nandsi               (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_addqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_subqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_iorqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_xorqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_andqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_addhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_subhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_iorhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_xorhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_andhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_nandqi               (rtx, rtx, rtx);
extern rtx        gen_arm_sync_new_nandhi               (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_addsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_subsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_iorsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_xorsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_andsi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_nandsi               (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_addqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_subqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_iorqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_xorqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_andqi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_addhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_subhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_iorhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_xorhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_andhi                (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_nandqi               (rtx, rtx, rtx);
extern rtx        gen_arm_sync_old_nandhi               (rtx, rtx, rtx);
extern rtx        gen_addqq3                            (rtx, rtx, rtx);
extern rtx        gen_addhq3                            (rtx, rtx, rtx);
extern rtx        gen_addsq3                            (rtx, rtx, rtx);
extern rtx        gen_adduqq3                           (rtx, rtx, rtx);
extern rtx        gen_adduhq3                           (rtx, rtx, rtx);
extern rtx        gen_addusq3                           (rtx, rtx, rtx);
extern rtx        gen_addha3                            (rtx, rtx, rtx);
extern rtx        gen_addsa3                            (rtx, rtx, rtx);
extern rtx        gen_adduha3                           (rtx, rtx, rtx);
extern rtx        gen_addusa3                           (rtx, rtx, rtx);
extern rtx        gen_addv4qq3                          (rtx, rtx, rtx);
extern rtx        gen_addv2hq3                          (rtx, rtx, rtx);
extern rtx        gen_addv2ha3                          (rtx, rtx, rtx);
extern rtx        gen_usaddv4uqq3                       (rtx, rtx, rtx);
extern rtx        gen_usaddv2uhq3                       (rtx, rtx, rtx);
extern rtx        gen_usadduqq3                         (rtx, rtx, rtx);
extern rtx        gen_usadduhq3                         (rtx, rtx, rtx);
extern rtx        gen_usaddv2uha3                       (rtx, rtx, rtx);
extern rtx        gen_usadduha3                         (rtx, rtx, rtx);
extern rtx        gen_ssaddv4qq3                        (rtx, rtx, rtx);
extern rtx        gen_ssaddv2hq3                        (rtx, rtx, rtx);
extern rtx        gen_ssaddqq3                          (rtx, rtx, rtx);
extern rtx        gen_ssaddhq3                          (rtx, rtx, rtx);
extern rtx        gen_ssaddv2ha3                        (rtx, rtx, rtx);
extern rtx        gen_ssaddha3                          (rtx, rtx, rtx);
extern rtx        gen_ssaddsq3                          (rtx, rtx, rtx);
extern rtx        gen_ssaddsa3                          (rtx, rtx, rtx);
extern rtx        gen_subqq3                            (rtx, rtx, rtx);
extern rtx        gen_subhq3                            (rtx, rtx, rtx);
extern rtx        gen_subsq3                            (rtx, rtx, rtx);
extern rtx        gen_subuqq3                           (rtx, rtx, rtx);
extern rtx        gen_subuhq3                           (rtx, rtx, rtx);
extern rtx        gen_subusq3                           (rtx, rtx, rtx);
extern rtx        gen_subha3                            (rtx, rtx, rtx);
extern rtx        gen_subsa3                            (rtx, rtx, rtx);
extern rtx        gen_subuha3                           (rtx, rtx, rtx);
extern rtx        gen_subusa3                           (rtx, rtx, rtx);
extern rtx        gen_subv4qq3                          (rtx, rtx, rtx);
extern rtx        gen_subv2hq3                          (rtx, rtx, rtx);
extern rtx        gen_subv2ha3                          (rtx, rtx, rtx);
extern rtx        gen_ussubv4uqq3                       (rtx, rtx, rtx);
extern rtx        gen_ussubv2uhq3                       (rtx, rtx, rtx);
extern rtx        gen_ussubuqq3                         (rtx, rtx, rtx);
extern rtx        gen_ussubuhq3                         (rtx, rtx, rtx);
extern rtx        gen_ussubv2uha3                       (rtx, rtx, rtx);
extern rtx        gen_ussubuha3                         (rtx, rtx, rtx);
extern rtx        gen_sssubv4qq3                        (rtx, rtx, rtx);
extern rtx        gen_sssubv2hq3                        (rtx, rtx, rtx);
extern rtx        gen_sssubqq3                          (rtx, rtx, rtx);
extern rtx        gen_sssubhq3                          (rtx, rtx, rtx);
extern rtx        gen_sssubv2ha3                        (rtx, rtx, rtx);
extern rtx        gen_sssubha3                          (rtx, rtx, rtx);
extern rtx        gen_sssubsq3                          (rtx, rtx, rtx);
extern rtx        gen_sssubsa3                          (rtx, rtx, rtx);
extern rtx        gen_ssmulsa3                          (rtx, rtx, rtx);
extern rtx        gen_usmulusa3                         (rtx, rtx, rtx);
extern rtx        gen_arm_ssatsihi_shift                (rtx, rtx, rtx, rtx);
extern rtx        gen_arm_usatsihi                      (rtx, rtx);
extern rtx        gen_adddi3                            (rtx, rtx, rtx);
extern rtx        gen_addsi3                            (rtx, rtx, rtx);
extern rtx        gen_incscc                            (rtx, rtx, rtx, rtx);
extern rtx        gen_addsf3                            (rtx, rtx, rtx);
extern rtx        gen_adddf3                            (rtx, rtx, rtx);
extern rtx        gen_subdi3                            (rtx, rtx, rtx);
extern rtx        gen_subsi3                            (rtx, rtx, rtx);
extern rtx        gen_decscc                            (rtx, rtx, rtx, rtx);
extern rtx        gen_subsf3                            (rtx, rtx, rtx);
extern rtx        gen_subdf3                            (rtx, rtx, rtx);
extern rtx        gen_mulsi3                            (rtx, rtx, rtx);
extern rtx        gen_maddsidi4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_mulsidi3                          (rtx, rtx, rtx);
extern rtx        gen_umulsidi3                         (rtx, rtx, rtx);
extern rtx        gen_umaddsidi4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_smulsi3_highpart                  (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart                  (rtx, rtx, rtx);
extern rtx        gen_mulsf3                            (rtx, rtx, rtx);
extern rtx        gen_muldf3                            (rtx, rtx, rtx);
extern rtx        gen_divsf3                            (rtx, rtx, rtx);
extern rtx        gen_divdf3                            (rtx, rtx, rtx);
extern rtx        gen_modsf3                            (rtx, rtx, rtx);
extern rtx        gen_moddf3                            (rtx, rtx, rtx);
extern rtx        gen_anddi3                            (rtx, rtx, rtx);
extern rtx        gen_andsi3                            (rtx, rtx, rtx);
extern rtx        gen_insv                              (rtx, rtx, rtx, rtx);
extern rtx        gen_iordi3                            (rtx, rtx, rtx);
extern rtx        gen_iorsi3                            (rtx, rtx, rtx);
extern rtx        gen_xordi3                            (rtx, rtx, rtx);
extern rtx        gen_xorsi3                            (rtx, rtx, rtx);
extern rtx        gen_smaxsi3                           (rtx, rtx, rtx);
extern rtx        gen_sminsi3                           (rtx, rtx, rtx);
extern rtx        gen_umaxsi3                           (rtx, rtx, rtx);
extern rtx        gen_uminsi3                           (rtx, rtx, rtx);
extern rtx        gen_ashldi3                           (rtx, rtx, rtx);
extern rtx        gen_ashlsi3                           (rtx, rtx, rtx);
extern rtx        gen_ashrdi3                           (rtx, rtx, rtx);
extern rtx        gen_ashrsi3                           (rtx, rtx, rtx);
extern rtx        gen_lshrdi3                           (rtx, rtx, rtx);
extern rtx        gen_lshrsi3                           (rtx, rtx, rtx);
extern rtx        gen_rotlsi3                           (rtx, rtx, rtx);
extern rtx        gen_rotrsi3                           (rtx, rtx, rtx);
extern rtx        gen_extzv                             (rtx, rtx, rtx, rtx);
extern rtx        gen_extv                              (rtx, rtx, rtx, rtx);
extern rtx        gen_negdi2                            (rtx, rtx);
extern rtx        gen_negsi2                            (rtx, rtx);
extern rtx        gen_negsf2                            (rtx, rtx);
extern rtx        gen_negdf2                            (rtx, rtx);
extern rtx        gen_abssi2                            (rtx, rtx);
extern rtx        gen_abssf2                            (rtx, rtx);
extern rtx        gen_absdf2                            (rtx, rtx);
extern rtx        gen_sqrtsf2                           (rtx, rtx);
extern rtx        gen_sqrtdf2                           (rtx, rtx);
extern rtx        gen_one_cmplsi2                       (rtx, rtx);
extern rtx        gen_floatsihf2                        (rtx, rtx);
extern rtx        gen_floatdihf2                        (rtx, rtx);
extern rtx        gen_floatsisf2                        (rtx, rtx);
extern rtx        gen_floatsidf2                        (rtx, rtx);
extern rtx        gen_fix_trunchfsi2                    (rtx, rtx);
extern rtx        gen_fix_trunchfdi2                    (rtx, rtx);
extern rtx        gen_fix_truncsfsi2                    (rtx, rtx);
extern rtx        gen_fix_truncdfsi2                    (rtx, rtx);
extern rtx        gen_truncdfsf2                        (rtx, rtx);
extern rtx        gen_truncdfhf2                        (rtx, rtx);
extern rtx        gen_zero_extendsidi2                  (rtx, rtx);
extern rtx        gen_zero_extendqidi2                  (rtx, rtx);
extern rtx        gen_extendsidi2                       (rtx, rtx);
extern rtx        gen_zero_extendhisi2                  (rtx, rtx);
extern rtx        gen_zero_extendqisi2                  (rtx, rtx);
extern rtx        gen_extendhisi2                       (rtx, rtx);
extern rtx        gen_extendhisi2_mem                   (rtx, rtx);
extern rtx        gen_extendqihi2                       (rtx, rtx);
extern rtx        gen_extendqisi2                       (rtx, rtx);
extern rtx        gen_extendsfdf2                       (rtx, rtx);
extern rtx        gen_extendhfdf2                       (rtx, rtx);
extern rtx        gen_movdi                             (rtx, rtx);
extern rtx        gen_movsi                             (rtx, rtx);
extern rtx        gen_calculate_pic_address             (rtx, rtx, rtx);
extern rtx        gen_builtin_setjmp_receiver           (rtx);
extern rtx        gen_storehi                           (rtx, rtx);
extern rtx        gen_storehi_bigend                    (rtx, rtx);
extern rtx        gen_storeinthi                        (rtx, rtx);
extern rtx        gen_storehi_single_op                 (rtx, rtx);
extern rtx        gen_movhi                             (rtx, rtx);
extern rtx        gen_movhi_bytes                       (rtx, rtx);
extern rtx        gen_movhi_bigend                      (rtx, rtx);
extern rtx        gen_thumb_movhi_clobber               (rtx, rtx, rtx);
extern rtx        gen_reload_outhi                      (rtx, rtx, rtx);
extern rtx        gen_reload_inhi                       (rtx, rtx, rtx);
extern rtx        gen_movqi                             (rtx, rtx);
extern rtx        gen_movhf                             (rtx, rtx);
extern rtx        gen_movsf                             (rtx, rtx);
extern rtx        gen_movdf                             (rtx, rtx);
extern rtx        gen_reload_outdf                      (rtx, rtx, rtx);
extern rtx        gen_movxf                             (rtx, rtx);
extern rtx        gen_load_multiple                     (rtx, rtx, rtx);
extern rtx        gen_store_multiple                    (rtx, rtx, rtx);
extern rtx        gen_movmemqi                          (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchsi4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchqi4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchsf4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchdf4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchdi4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranch_cc                        (rtx, rtx, rtx, rtx);
extern rtx        gen_cstore_cc                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoresi4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoresf4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoredf4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoredi4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoresi_eq0_thumb1               (rtx, rtx);
extern rtx        gen_cstoresi_ne0_thumb1               (rtx, rtx);
extern rtx        gen_movsicc                           (rtx, rtx, rtx, rtx);
extern rtx        gen_movsfcc                           (rtx, rtx, rtx, rtx);
extern rtx        gen_movdfcc                           (rtx, rtx, rtx, rtx);
extern rtx        gen_jump                              (rtx);
#define GEN_CALL(A, B, C, D) gen_call ((A), (B), (C))
extern rtx        gen_call                              (rtx, rtx, rtx);
extern rtx        gen_call_internal                     (rtx, rtx, rtx);
#define GEN_CALL_VALUE(A, B, C, D, E) gen_call_value ((A), (B), (C), (D))
extern rtx        gen_call_value                        (rtx, rtx, rtx, rtx);
extern rtx        gen_call_value_internal               (rtx, rtx, rtx, rtx);
#define GEN_SIBCALL(A, B, C, D) gen_sibcall ((A), (B), (C))
extern rtx        gen_sibcall                           (rtx, rtx, rtx);
#define GEN_SIBCALL_VALUE(A, B, C, D, E) gen_sibcall_value ((A), (B), (C), (D))
extern rtx        gen_sibcall_value                     (rtx, rtx, rtx, rtx);
extern rtx        gen_return                            (void);
extern rtx        gen_simple_return                     (void);
extern rtx        gen_return_addr_mask                  (rtx);
extern rtx        gen_untyped_call                      (rtx, rtx, rtx);
extern rtx        gen_untyped_return                    (rtx, rtx);
extern rtx        gen_casesi                            (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_thumb1_casesi_internal_pic        (rtx, rtx, rtx, rtx);
extern rtx        gen_indirect_jump                     (rtx);
extern rtx        gen_prologue                          (void);
extern rtx        gen_epilogue                          (void);
extern rtx        gen_eh_epilogue                       (rtx, rtx, rtx);
extern rtx        gen_tablejump                         (rtx, rtx);
extern rtx        gen_ctzsi2                            (rtx, rtx);
extern rtx        gen_eh_return                         (rtx);
extern rtx        gen_arm_legacy_rev                    (rtx, rtx, rtx, rtx);
extern rtx        gen_thumb_legacy_rev                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_bswapsi2                          (rtx, rtx);
extern rtx        gen_movv2di                           (rtx, rtx);
extern rtx        gen_movv2si                           (rtx, rtx);
extern rtx        gen_movv4hi                           (rtx, rtx);
extern rtx        gen_movv8qi                           (rtx, rtx);
extern rtx        gen_movv2sf                           (rtx, rtx);
extern rtx        gen_movv4si                           (rtx, rtx);
extern rtx        gen_movv8hi                           (rtx, rtx);
extern rtx        gen_movv16qi                          (rtx, rtx);
extern rtx        gen_movv4sf                           (rtx, rtx);
extern rtx        gen_addv2di3                          (rtx, rtx, rtx);
extern rtx        gen_addv2si3                          (rtx, rtx, rtx);
extern rtx        gen_addv4hi3                          (rtx, rtx, rtx);
extern rtx        gen_addv8qi3                          (rtx, rtx, rtx);
extern rtx        gen_addv2sf3                          (rtx, rtx, rtx);
extern rtx        gen_addv4si3                          (rtx, rtx, rtx);
extern rtx        gen_addv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_addv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_addv4sf3                          (rtx, rtx, rtx);
extern rtx        gen_subv2di3                          (rtx, rtx, rtx);
extern rtx        gen_subv2si3                          (rtx, rtx, rtx);
extern rtx        gen_subv4hi3                          (rtx, rtx, rtx);
extern rtx        gen_subv8qi3                          (rtx, rtx, rtx);
extern rtx        gen_subv2sf3                          (rtx, rtx, rtx);
extern rtx        gen_subv4si3                          (rtx, rtx, rtx);
extern rtx        gen_subv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_subv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_subv4sf3                          (rtx, rtx, rtx);
extern rtx        gen_mulv2si3                          (rtx, rtx, rtx);
extern rtx        gen_mulv4hi3                          (rtx, rtx, rtx);
extern rtx        gen_mulv8qi3                          (rtx, rtx, rtx);
extern rtx        gen_mulv2sf3                          (rtx, rtx, rtx);
extern rtx        gen_mulv4si3                          (rtx, rtx, rtx);
extern rtx        gen_mulv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_mulv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_mulv4sf3                          (rtx, rtx, rtx);
extern rtx        gen_sminv2si3                         (rtx, rtx, rtx);
extern rtx        gen_sminv4hi3                         (rtx, rtx, rtx);
extern rtx        gen_sminv8qi3                         (rtx, rtx, rtx);
extern rtx        gen_sminv2sf3                         (rtx, rtx, rtx);
extern rtx        gen_sminv4si3                         (rtx, rtx, rtx);
extern rtx        gen_sminv8hi3                         (rtx, rtx, rtx);
extern rtx        gen_sminv16qi3                        (rtx, rtx, rtx);
extern rtx        gen_sminv4sf3                         (rtx, rtx, rtx);
extern rtx        gen_uminv2si3                         (rtx, rtx, rtx);
extern rtx        gen_uminv4hi3                         (rtx, rtx, rtx);
extern rtx        gen_uminv8qi3                         (rtx, rtx, rtx);
extern rtx        gen_uminv4si3                         (rtx, rtx, rtx);
extern rtx        gen_uminv8hi3                         (rtx, rtx, rtx);
extern rtx        gen_uminv16qi3                        (rtx, rtx, rtx);
extern rtx        gen_smaxv2si3                         (rtx, rtx, rtx);
extern rtx        gen_smaxv4hi3                         (rtx, rtx, rtx);
extern rtx        gen_smaxv8qi3                         (rtx, rtx, rtx);
extern rtx        gen_smaxv2sf3                         (rtx, rtx, rtx);
extern rtx        gen_smaxv4si3                         (rtx, rtx, rtx);
extern rtx        gen_smaxv8hi3                         (rtx, rtx, rtx);
extern rtx        gen_smaxv16qi3                        (rtx, rtx, rtx);
extern rtx        gen_smaxv4sf3                         (rtx, rtx, rtx);
extern rtx        gen_umaxv2si3                         (rtx, rtx, rtx);
extern rtx        gen_umaxv4hi3                         (rtx, rtx, rtx);
extern rtx        gen_umaxv8qi3                         (rtx, rtx, rtx);
extern rtx        gen_umaxv4si3                         (rtx, rtx, rtx);
extern rtx        gen_umaxv8hi3                         (rtx, rtx, rtx);
extern rtx        gen_umaxv16qi3                        (rtx, rtx, rtx);
extern rtx        gen_movti                             (rtx, rtx);
extern rtx        gen_movei                             (rtx, rtx);
extern rtx        gen_movoi                             (rtx, rtx);
extern rtx        gen_movci                             (rtx, rtx);
extern rtx        gen_movxi                             (rtx, rtx);
extern rtx        gen_movmisalignv8qi                   (rtx, rtx);
extern rtx        gen_movmisalignv16qi                  (rtx, rtx);
extern rtx        gen_movmisalignv4hi                   (rtx, rtx);
extern rtx        gen_movmisalignv8hi                   (rtx, rtx);
extern rtx        gen_movmisalignv2si                   (rtx, rtx);
extern rtx        gen_movmisalignv4si                   (rtx, rtx);
extern rtx        gen_movmisalignv2sf                   (rtx, rtx);
extern rtx        gen_movmisalignv4sf                   (rtx, rtx);
extern rtx        gen_movmisaligndi                     (rtx, rtx);
extern rtx        gen_movmisalignv2di                   (rtx, rtx);
extern rtx        gen_vec_setv8qi                       (rtx, rtx, rtx);
extern rtx        gen_vec_setv16qi                      (rtx, rtx, rtx);
extern rtx        gen_vec_setv4hi                       (rtx, rtx, rtx);
extern rtx        gen_vec_setv8hi                       (rtx, rtx, rtx);
extern rtx        gen_vec_setv2si                       (rtx, rtx, rtx);
extern rtx        gen_vec_setv4si                       (rtx, rtx, rtx);
extern rtx        gen_vec_setv2sf                       (rtx, rtx, rtx);
extern rtx        gen_vec_setv4sf                       (rtx, rtx, rtx);
extern rtx        gen_vec_setv2di                       (rtx, rtx, rtx);
extern rtx        gen_vec_initv8qi                      (rtx, rtx);
extern rtx        gen_vec_initv16qi                     (rtx, rtx);
extern rtx        gen_vec_initv4hi                      (rtx, rtx);
extern rtx        gen_vec_initv8hi                      (rtx, rtx);
extern rtx        gen_vec_initv2si                      (rtx, rtx);
extern rtx        gen_vec_initv4si                      (rtx, rtx);
extern rtx        gen_vec_initv2sf                      (rtx, rtx);
extern rtx        gen_vec_initv4sf                      (rtx, rtx);
extern rtx        gen_vec_initv2di                      (rtx, rtx);
extern rtx        gen_vashrv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_vashrv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_vashrv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_vashrv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_vashrv2si3                        (rtx, rtx, rtx);
extern rtx        gen_vashrv4si3                        (rtx, rtx, rtx);
extern rtx        gen_vlshrv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_vlshrv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_vlshrv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_vlshrv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_vlshrv2si3                        (rtx, rtx, rtx);
extern rtx        gen_vlshrv4si3                        (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v8qi                      (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v16qi                     (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v4hi                      (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v8hi                      (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v2si                      (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v4si                      (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v2sf                      (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v4sf                      (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v2di                      (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v8qi                      (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v16qi                     (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v4hi                      (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v8hi                      (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v2si                      (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v4si                      (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v2sf                      (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v4sf                      (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v2di                      (rtx, rtx, rtx);
extern rtx        gen_move_hi_quad_v2di                 (rtx, rtx);
extern rtx        gen_move_hi_quad_v2df                 (rtx, rtx);
extern rtx        gen_move_hi_quad_v16qi                (rtx, rtx);
extern rtx        gen_move_hi_quad_v8hi                 (rtx, rtx);
extern rtx        gen_move_hi_quad_v4si                 (rtx, rtx);
extern rtx        gen_move_hi_quad_v4sf                 (rtx, rtx);
extern rtx        gen_move_lo_quad_v2di                 (rtx, rtx);
extern rtx        gen_move_lo_quad_v2df                 (rtx, rtx);
extern rtx        gen_move_lo_quad_v16qi                (rtx, rtx);
extern rtx        gen_move_lo_quad_v8hi                 (rtx, rtx);
extern rtx        gen_move_lo_quad_v4si                 (rtx, rtx);
extern rtx        gen_move_lo_quad_v4sf                 (rtx, rtx);
extern rtx        gen_reduc_splus_v8qi                  (rtx, rtx);
extern rtx        gen_reduc_splus_v4hi                  (rtx, rtx);
extern rtx        gen_reduc_splus_v2si                  (rtx, rtx);
extern rtx        gen_reduc_splus_v2sf                  (rtx, rtx);
extern rtx        gen_reduc_splus_v16qi                 (rtx, rtx);
extern rtx        gen_reduc_splus_v8hi                  (rtx, rtx);
extern rtx        gen_reduc_splus_v4si                  (rtx, rtx);
extern rtx        gen_reduc_splus_v4sf                  (rtx, rtx);
extern rtx        gen_reduc_uplus_v8qi                  (rtx, rtx);
extern rtx        gen_reduc_uplus_v16qi                 (rtx, rtx);
extern rtx        gen_reduc_uplus_v4hi                  (rtx, rtx);
extern rtx        gen_reduc_uplus_v8hi                  (rtx, rtx);
extern rtx        gen_reduc_uplus_v2si                  (rtx, rtx);
extern rtx        gen_reduc_uplus_v4si                  (rtx, rtx);
extern rtx        gen_reduc_uplus_v2di                  (rtx, rtx);
extern rtx        gen_reduc_smin_v8qi                   (rtx, rtx);
extern rtx        gen_reduc_smin_v4hi                   (rtx, rtx);
extern rtx        gen_reduc_smin_v2si                   (rtx, rtx);
extern rtx        gen_reduc_smin_v2sf                   (rtx, rtx);
extern rtx        gen_reduc_smin_v16qi                  (rtx, rtx);
extern rtx        gen_reduc_smin_v8hi                   (rtx, rtx);
extern rtx        gen_reduc_smin_v4si                   (rtx, rtx);
extern rtx        gen_reduc_smin_v4sf                   (rtx, rtx);
extern rtx        gen_reduc_smax_v8qi                   (rtx, rtx);
extern rtx        gen_reduc_smax_v4hi                   (rtx, rtx);
extern rtx        gen_reduc_smax_v2si                   (rtx, rtx);
extern rtx        gen_reduc_smax_v2sf                   (rtx, rtx);
extern rtx        gen_reduc_smax_v16qi                  (rtx, rtx);
extern rtx        gen_reduc_smax_v8hi                   (rtx, rtx);
extern rtx        gen_reduc_smax_v4si                   (rtx, rtx);
extern rtx        gen_reduc_smax_v4sf                   (rtx, rtx);
extern rtx        gen_reduc_umin_v8qi                   (rtx, rtx);
extern rtx        gen_reduc_umin_v4hi                   (rtx, rtx);
extern rtx        gen_reduc_umin_v2si                   (rtx, rtx);
extern rtx        gen_reduc_umin_v16qi                  (rtx, rtx);
extern rtx        gen_reduc_umin_v8hi                   (rtx, rtx);
extern rtx        gen_reduc_umin_v4si                   (rtx, rtx);
extern rtx        gen_reduc_umax_v8qi                   (rtx, rtx);
extern rtx        gen_reduc_umax_v4hi                   (rtx, rtx);
extern rtx        gen_reduc_umax_v2si                   (rtx, rtx);
extern rtx        gen_reduc_umax_v16qi                  (rtx, rtx);
extern rtx        gen_reduc_umax_v8hi                   (rtx, rtx);
extern rtx        gen_reduc_umax_v4si                   (rtx, rtx);
extern rtx        gen_vcondv8qi                         (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv16qi                        (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4hi                         (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv8hi                         (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2si                         (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4si                         (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2sf                         (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4sf                         (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv8qi                        (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv16qi                       (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4hi                        (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv8hi                        (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv2si                        (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4si                        (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vadddi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vaddv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav8qi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav16qi                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav4hi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav8hi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav2si                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav4si                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav2sf                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlav4sf                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv8qi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv16qi                    (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv4hi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv8hi                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv2si                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv4si                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv2sf                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsv4sf                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubdi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vsubv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpaddv8qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpaddv4hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpaddv2si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vpaddv2sf                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vabsv8qi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vabsv16qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vabsv4hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vabsv8hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vabsv2si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vabsv4si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vabsv2sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vabsv4sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vnegv8qi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vnegv16qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vnegv4hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vnegv8hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vnegv2si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vnegv4si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vnegv2sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vnegv4sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vclzv8qi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vclzv16qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vclzv4hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vclzv8hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vclzv2si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vclzv4si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vcntv8qi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vcntv16qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vmvnv8qi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vmvnv16qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vmvnv4hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vmvnv8hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vmvnv2si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vmvnv4si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev8qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev16qi               (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev4hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev8hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev2si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev4si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev2sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev4sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanedi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vget_lanev2di                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanev8qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanev16qi               (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanev4hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanev8hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanev2si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanev4si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanev2sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanev4sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanev2di                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vset_lanedi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vcreatev8qi                  (rtx, rtx);
extern rtx        gen_neon_vcreatev4hi                  (rtx, rtx);
extern rtx        gen_neon_vcreatev2si                  (rtx, rtx);
extern rtx        gen_neon_vcreatev2sf                  (rtx, rtx);
extern rtx        gen_neon_vcreatedi                    (rtx, rtx);
extern rtx        gen_neon_vdup_ndi                     (rtx, rtx);
extern rtx        gen_neon_vdup_lanev8qi                (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev16qi               (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev4hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev8hi                (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev2si                (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev4si                (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev2sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev4sf                (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanedi                  (rtx, rtx, rtx);
extern rtx        gen_neon_vdup_lanev2di                (rtx, rtx, rtx);
extern rtx        gen_neon_vmul_nv4hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_nv2si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_nv2sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_nv8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_nv4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmul_nv4sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmull_nv4hi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmull_nv2si                  (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmull_nv4hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmull_nv2si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulh_nv4hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulh_nv2si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulh_nv8hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmulh_nv4si                (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_nv4hi                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_nv2si                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_nv2sf                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_nv8hi                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_nv4si                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmla_nv4sf                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlal_nv4hi                  (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlal_nv2si                  (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlal_nv4hi                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlal_nv2si                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_nv4hi                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_nv2si                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_nv2sf                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_nv8hi                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_nv4si                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmls_nv4sf                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsl_nv4hi                  (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vmlsl_nv2si                  (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlsl_nv4hi                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vqdmlsl_nv2si                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbsldi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbslv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv8qi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv16qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv4hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv8hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv2si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv4si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv2sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vtrnv4sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vzipv8qi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vzipv16qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vzipv4hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vzipv8hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vzipv2si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vzipv4si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vzipv2sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vzipv4sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv8qi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv16qi                    (rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv4hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv8hi                     (rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv2si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv4si                     (rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv2sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vuzpv4sf                     (rtx, rtx, rtx);
extern rtx        gen_neon_vreinterpretv8qiv8qi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv8qiv4hi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv8qiv2si         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv8qiv2sf         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv8qidi           (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4hiv8qi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4hiv4hi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4hiv2si         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4hiv2sf         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4hidi           (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2siv8qi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2siv4hi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2siv2si         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2siv2sf         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2sidi           (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2sfv8qi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2sfv4hi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2sfv2si         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2sfv2sf         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2sfdi           (rtx, rtx);
extern rtx        gen_neon_vreinterpretdiv8qi           (rtx, rtx);
extern rtx        gen_neon_vreinterpretdiv4hi           (rtx, rtx);
extern rtx        gen_neon_vreinterpretdiv2si           (rtx, rtx);
extern rtx        gen_neon_vreinterpretdiv2sf           (rtx, rtx);
extern rtx        gen_neon_vreinterpretdidi             (rtx, rtx);
extern rtx        gen_neon_vreinterpretv16qiv16qi       (rtx, rtx);
extern rtx        gen_neon_vreinterpretv16qiv8hi        (rtx, rtx);
extern rtx        gen_neon_vreinterpretv16qiv4si        (rtx, rtx);
extern rtx        gen_neon_vreinterpretv16qiv4sf        (rtx, rtx);
extern rtx        gen_neon_vreinterpretv16qiv2di        (rtx, rtx);
extern rtx        gen_neon_vreinterpretv8hiv16qi        (rtx, rtx);
extern rtx        gen_neon_vreinterpretv8hiv8hi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv8hiv4si         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv8hiv4sf         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv8hiv2di         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4siv16qi        (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4siv8hi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4siv4si         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4siv4sf         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4siv2di         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4sfv16qi        (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4sfv8hi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4sfv4si         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4sfv4sf         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv4sfv2di         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2div16qi        (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2div8hi         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2div4si         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2div4sf         (rtx, rtx);
extern rtx        gen_neon_vreinterpretv2div2di         (rtx, rtx);
extern rtx        gen_neon_vld3v16qi                    (rtx, rtx);
extern rtx        gen_neon_vld3v8hi                     (rtx, rtx);
extern rtx        gen_neon_vld3v4si                     (rtx, rtx);
extern rtx        gen_neon_vld3v4sf                     (rtx, rtx);
extern rtx        gen_neon_vst3v16qi                    (rtx, rtx);
extern rtx        gen_neon_vst3v8hi                     (rtx, rtx);
extern rtx        gen_neon_vst3v4si                     (rtx, rtx);
extern rtx        gen_neon_vst3v4sf                     (rtx, rtx);
extern rtx        gen_neon_vld4v16qi                    (rtx, rtx);
extern rtx        gen_neon_vld4v8hi                     (rtx, rtx);
extern rtx        gen_neon_vld4v4si                     (rtx, rtx);
extern rtx        gen_neon_vld4v4sf                     (rtx, rtx);
extern rtx        gen_neon_vst4v16qi                    (rtx, rtx);
extern rtx        gen_neon_vst4v8hi                     (rtx, rtx);
extern rtx        gen_neon_vst4v4si                     (rtx, rtx);
extern rtx        gen_neon_vst4v4sf                     (rtx, rtx);
extern rtx        gen_neon_vandv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vandv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vandv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vandv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vandv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vandv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vandv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vandv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vanddi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vandv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrdi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorrv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veorv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veorv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veorv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veorv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veorv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veorv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veorv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veorv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veordi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_veorv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicdi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vbicv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vornv8qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vornv16qi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vornv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vornv8hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vornv2si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vornv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vornv2sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vornv4sf                     (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vorndi                       (rtx, rtx, rtx, rtx);
extern rtx        gen_neon_vornv2di                     (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v16qi              (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v16qi              (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v8hi               (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v8hi               (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v4si               (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v4si               (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v16qi              (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v16qi              (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v8hi               (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v8hi               (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v4si               (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v4si               (rtx, rtx);
extern rtx        gen_vec_widen_smult_lo_v16qi          (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_lo_v16qi          (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_lo_v8hi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_lo_v8hi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_lo_v4si           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_lo_v4si           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_hi_v16qi          (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_hi_v16qi          (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_hi_v8hi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_hi_v8hi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_hi_v4si           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_hi_v4si           (rtx, rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v8qi               (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v8qi               (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v4hi               (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v4hi               (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v2si               (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v2si               (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v8qi               (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v8qi               (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v4hi               (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v4hi               (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v2si               (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v2si               (rtx, rtx);
extern rtx        gen_vec_widen_smult_hi_v8qi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_hi_v8qi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_hi_v4hi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_hi_v4hi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_hi_v2si           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_hi_v2si           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_lo_v8qi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_lo_v8qi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_lo_v4hi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_lo_v4hi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_lo_v2si           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_lo_v2si           (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v4hi               (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v2si               (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_di                 (rtx, rtx, rtx);
extern rtx        gen_memory_barrier                    (void);
extern rtx        gen_sync_compare_and_swapsi           (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_compare_and_swapqi           (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_compare_and_swaphi           (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_lock_test_and_setsi          (rtx, rtx, rtx);
extern rtx        gen_sync_lock_test_and_setqi          (rtx, rtx, rtx);
extern rtx        gen_sync_lock_test_and_sethi          (rtx, rtx, rtx);
extern rtx        gen_sync_addsi                        (rtx, rtx);
extern rtx        gen_sync_subsi                        (rtx, rtx);
extern rtx        gen_sync_iorsi                        (rtx, rtx);
extern rtx        gen_sync_xorsi                        (rtx, rtx);
extern rtx        gen_sync_andsi                        (rtx, rtx);
extern rtx        gen_sync_nandsi                       (rtx, rtx);
extern rtx        gen_sync_addqi                        (rtx, rtx);
extern rtx        gen_sync_subqi                        (rtx, rtx);
extern rtx        gen_sync_iorqi                        (rtx, rtx);
extern rtx        gen_sync_xorqi                        (rtx, rtx);
extern rtx        gen_sync_andqi                        (rtx, rtx);
extern rtx        gen_sync_addhi                        (rtx, rtx);
extern rtx        gen_sync_subhi                        (rtx, rtx);
extern rtx        gen_sync_iorhi                        (rtx, rtx);
extern rtx        gen_sync_xorhi                        (rtx, rtx);
extern rtx        gen_sync_andhi                        (rtx, rtx);
extern rtx        gen_sync_nandqi                       (rtx, rtx);
extern rtx        gen_sync_nandhi                       (rtx, rtx);
extern rtx        gen_sync_new_addsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_subsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_iorsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_xorsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_andsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_nandsi                   (rtx, rtx, rtx);
extern rtx        gen_sync_new_addqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_subqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_iorqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_xorqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_andqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_addhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_subhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_iorhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_xorhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_andhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_new_nandqi                   (rtx, rtx, rtx);
extern rtx        gen_sync_new_nandhi                   (rtx, rtx, rtx);
extern rtx        gen_sync_old_addsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_subsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_iorsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_xorsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_andsi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_nandsi                   (rtx, rtx, rtx);
extern rtx        gen_sync_old_addqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_subqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_iorqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_xorqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_andqi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_addhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_subhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_iorhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_xorhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_andhi                    (rtx, rtx, rtx);
extern rtx        gen_sync_old_nandqi                   (rtx, rtx, rtx);
extern rtx        gen_sync_old_nandhi                   (rtx, rtx, rtx);
extern rtx        gen_mulqq3                            (rtx, rtx, rtx);
extern rtx        gen_mulhq3                            (rtx, rtx, rtx);
extern rtx        gen_mulsq3                            (rtx, rtx, rtx);
extern rtx        gen_mulsa3                            (rtx, rtx, rtx);
extern rtx        gen_mulusa3                           (rtx, rtx, rtx);
extern rtx        gen_mulha3                            (rtx, rtx, rtx);
extern rtx        gen_muluha3                           (rtx, rtx, rtx);
extern rtx        gen_ssmulha3                          (rtx, rtx, rtx);
extern rtx        gen_usmuluha3                         (rtx, rtx, rtx);

#endif /* GCC_INSN_FLAGS_H */
