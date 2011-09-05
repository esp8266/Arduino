/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_PWM_INSTANCE_
#define _SAM3XA_PWM_INSTANCE_

/* ========== Register definition for PWM peripheral ========== */
#define REG_PWM_CLK      REG_ACCESS(RwReg, 0x40094000U) /**< \brief (PWM) PWM Clock Register */
#define REG_PWM_ENA      REG_ACCESS(WoReg, 0x40094004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS      REG_ACCESS(WoReg, 0x40094008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR       REG_ACCESS(RoReg, 0x4009400CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER1     REG_ACCESS(WoReg, 0x40094010U) /**< \brief (PWM) PWM Interrupt Enable Register 1 */
#define REG_PWM_IDR1     REG_ACCESS(WoReg, 0x40094014U) /**< \brief (PWM) PWM Interrupt Disable Register 1 */
#define REG_PWM_IMR1     REG_ACCESS(RoReg, 0x40094018U) /**< \brief (PWM) PWM Interrupt Mask Register 1 */
#define REG_PWM_ISR1     REG_ACCESS(RoReg, 0x4009401CU) /**< \brief (PWM) PWM Interrupt Status Register 1 */
#define REG_PWM_SCM      REG_ACCESS(RwReg, 0x40094020U) /**< \brief (PWM) PWM Sync Channels Mode Register */
#define REG_PWM_SCUC     REG_ACCESS(RwReg, 0x40094028U) /**< \brief (PWM) PWM Sync Channels Update Control Register */
#define REG_PWM_SCUP     REG_ACCESS(RwReg, 0x4009402CU) /**< \brief (PWM) PWM Sync Channels Update Period Register */
#define REG_PWM_SCUPUPD  REG_ACCESS(WoReg, 0x40094030U) /**< \brief (PWM) PWM Sync Channels Update Period Update Register */
#define REG_PWM_IER2     REG_ACCESS(WoReg, 0x40094034U) /**< \brief (PWM) PWM Interrupt Enable Register 2 */
#define REG_PWM_IDR2     REG_ACCESS(WoReg, 0x40094038U) /**< \brief (PWM) PWM Interrupt Disable Register 2 */
#define REG_PWM_IMR2     REG_ACCESS(RoReg, 0x4009403CU) /**< \brief (PWM) PWM Interrupt Mask Register 2 */
#define REG_PWM_ISR2     REG_ACCESS(RoReg, 0x40094040U) /**< \brief (PWM) PWM Interrupt Status Register 2 */
#define REG_PWM_OOV      REG_ACCESS(RwReg, 0x40094044U) /**< \brief (PWM) PWM Output Override Value Register */
#define REG_PWM_OS       REG_ACCESS(RwReg, 0x40094048U) /**< \brief (PWM) PWM Output Selection Register */
#define REG_PWM_OSS      REG_ACCESS(WoReg, 0x4009404CU) /**< \brief (PWM) PWM Output Selection Set Register */
#define REG_PWM_OSC      REG_ACCESS(WoReg, 0x40094050U) /**< \brief (PWM) PWM Output Selection Clear Register */
#define REG_PWM_OSSUPD   REG_ACCESS(WoReg, 0x40094054U) /**< \brief (PWM) PWM Output Selection Set Update Register */
#define REG_PWM_OSCUPD   REG_ACCESS(WoReg, 0x40094058U) /**< \brief (PWM) PWM Output Selection Clear Update Register */
#define REG_PWM_FMR      REG_ACCESS(RwReg, 0x4009405CU) /**< \brief (PWM) PWM Fault Mode Register */
#define REG_PWM_FSR      REG_ACCESS(RoReg, 0x40094060U) /**< \brief (PWM) PWM Fault Status Register */
#define REG_PWM_FCR      REG_ACCESS(WoReg, 0x40094064U) /**< \brief (PWM) PWM Fault Clear Register */
#define REG_PWM_FPV      REG_ACCESS(RwReg, 0x40094068U) /**< \brief (PWM) PWM Fault Protection Value Register */
#define REG_PWM_FPE1     REG_ACCESS(RwReg, 0x4009406CU) /**< \brief (PWM) PWM Fault Protection Enable Register 1 */
#define REG_PWM_FPE2     REG_ACCESS(RwReg, 0x40094070U) /**< \brief (PWM) PWM Fault Protection Enable Register 2 */
#define REG_PWM_ELMR     REG_ACCESS(RwReg, 0x4009407CU) /**< \brief (PWM) PWM Event Line 0 Mode Register */
#define REG_PWM_SMMR     REG_ACCESS(RwReg, 0x400940B0U) /**< \brief (PWM) PWM Stepper Motor Mode Register */
#define REG_PWM_WPCR     REG_ACCESS(WoReg, 0x400940E4U) /**< \brief (PWM) PWM Write Protect Control Register */
#define REG_PWM_WPSR     REG_ACCESS(RoReg, 0x400940E8U) /**< \brief (PWM) PWM Write Protect Status Register */
#define REG_PWM_RPR      REG_ACCESS(RwReg, 0x40094100U) /**< \brief (PWM) Receive Pointer Register */
#define REG_PWM_RCR      REG_ACCESS(RwReg, 0x40094104U) /**< \brief (PWM) Receive Counter Register */
#define REG_PWM_TPR      REG_ACCESS(RwReg, 0x40094108U) /**< \brief (PWM) Transmit Pointer Register */
#define REG_PWM_TCR      REG_ACCESS(RwReg, 0x4009410CU) /**< \brief (PWM) Transmit Counter Register */
#define REG_PWM_RNPR     REG_ACCESS(RwReg, 0x40094110U) /**< \brief (PWM) Receive Next Pointer Register */
#define REG_PWM_RNCR     REG_ACCESS(RwReg, 0x40094114U) /**< \brief (PWM) Receive Next Counter Register */
#define REG_PWM_TNPR     REG_ACCESS(RwReg, 0x40094118U) /**< \brief (PWM) Transmit Next Pointer Register */
#define REG_PWM_TNCR     REG_ACCESS(RwReg, 0x4009411CU) /**< \brief (PWM) Transmit Next Counter Register */
#define REG_PWM_PTCR     REG_ACCESS(WoReg, 0x40094120U) /**< \brief (PWM) Transfer Control Register */
#define REG_PWM_PTSR     REG_ACCESS(RoReg, 0x40094124U) /**< \brief (PWM) Transfer Status Register */
#define REG_PWM_CMPV0    REG_ACCESS(RwReg, 0x40094130U) /**< \brief (PWM) PWM Comparison 0 Value Register */
#define REG_PWM_CMPVUPD0 REG_ACCESS(WoReg, 0x40094134U) /**< \brief (PWM) PWM Comparison 0 Value Update Register */
#define REG_PWM_CMPM0    REG_ACCESS(RwReg, 0x40094138U) /**< \brief (PWM) PWM Comparison 0 Mode Register */
#define REG_PWM_CMPMUPD0 REG_ACCESS(WoReg, 0x4009413CU) /**< \brief (PWM) PWM Comparison 0 Mode Update Register */
#define REG_PWM_CMPV1    REG_ACCESS(RwReg, 0x40094140U) /**< \brief (PWM) PWM Comparison 1 Value Register */
#define REG_PWM_CMPVUPD1 REG_ACCESS(WoReg, 0x40094144U) /**< \brief (PWM) PWM Comparison 1 Value Update Register */
#define REG_PWM_CMPM1    REG_ACCESS(RwReg, 0x40094148U) /**< \brief (PWM) PWM Comparison 1 Mode Register */
#define REG_PWM_CMPMUPD1 REG_ACCESS(WoReg, 0x4009414CU) /**< \brief (PWM) PWM Comparison 1 Mode Update Register */
#define REG_PWM_CMPV2    REG_ACCESS(RwReg, 0x40094150U) /**< \brief (PWM) PWM Comparison 2 Value Register */
#define REG_PWM_CMPVUPD2 REG_ACCESS(WoReg, 0x40094154U) /**< \brief (PWM) PWM Comparison 2 Value Update Register */
#define REG_PWM_CMPM2    REG_ACCESS(RwReg, 0x40094158U) /**< \brief (PWM) PWM Comparison 2 Mode Register */
#define REG_PWM_CMPMUPD2 REG_ACCESS(WoReg, 0x4009415CU) /**< \brief (PWM) PWM Comparison 2 Mode Update Register */
#define REG_PWM_CMPV3    REG_ACCESS(RwReg, 0x40094160U) /**< \brief (PWM) PWM Comparison 3 Value Register */
#define REG_PWM_CMPVUPD3 REG_ACCESS(WoReg, 0x40094164U) /**< \brief (PWM) PWM Comparison 3 Value Update Register */
#define REG_PWM_CMPM3    REG_ACCESS(RwReg, 0x40094168U) /**< \brief (PWM) PWM Comparison 3 Mode Register */
#define REG_PWM_CMPMUPD3 REG_ACCESS(WoReg, 0x4009416CU) /**< \brief (PWM) PWM Comparison 3 Mode Update Register */
#define REG_PWM_CMPV4    REG_ACCESS(RwReg, 0x40094170U) /**< \brief (PWM) PWM Comparison 4 Value Register */
#define REG_PWM_CMPVUPD4 REG_ACCESS(WoReg, 0x40094174U) /**< \brief (PWM) PWM Comparison 4 Value Update Register */
#define REG_PWM_CMPM4    REG_ACCESS(RwReg, 0x40094178U) /**< \brief (PWM) PWM Comparison 4 Mode Register */
#define REG_PWM_CMPMUPD4 REG_ACCESS(WoReg, 0x4009417CU) /**< \brief (PWM) PWM Comparison 4 Mode Update Register */
#define REG_PWM_CMPV5    REG_ACCESS(RwReg, 0x40094180U) /**< \brief (PWM) PWM Comparison 5 Value Register */
#define REG_PWM_CMPVUPD5 REG_ACCESS(WoReg, 0x40094184U) /**< \brief (PWM) PWM Comparison 5 Value Update Register */
#define REG_PWM_CMPM5    REG_ACCESS(RwReg, 0x40094188U) /**< \brief (PWM) PWM Comparison 5 Mode Register */
#define REG_PWM_CMPMUPD5 REG_ACCESS(WoReg, 0x4009418CU) /**< \brief (PWM) PWM Comparison 5 Mode Update Register */
#define REG_PWM_CMPV6    REG_ACCESS(RwReg, 0x40094190U) /**< \brief (PWM) PWM Comparison 6 Value Register */
#define REG_PWM_CMPVUPD6 REG_ACCESS(WoReg, 0x40094194U) /**< \brief (PWM) PWM Comparison 6 Value Update Register */
#define REG_PWM_CMPM6    REG_ACCESS(RwReg, 0x40094198U) /**< \brief (PWM) PWM Comparison 6 Mode Register */
#define REG_PWM_CMPMUPD6 REG_ACCESS(WoReg, 0x4009419CU) /**< \brief (PWM) PWM Comparison 6 Mode Update Register */
#define REG_PWM_CMPV7    REG_ACCESS(RwReg, 0x400941A0U) /**< \brief (PWM) PWM Comparison 7 Value Register */
#define REG_PWM_CMPVUPD7 REG_ACCESS(WoReg, 0x400941A4U) /**< \brief (PWM) PWM Comparison 7 Value Update Register */
#define REG_PWM_CMPM7    REG_ACCESS(RwReg, 0x400941A8U) /**< \brief (PWM) PWM Comparison 7 Mode Register */
#define REG_PWM_CMPMUPD7 REG_ACCESS(WoReg, 0x400941ACU) /**< \brief (PWM) PWM Comparison 7 Mode Update Register */
#define REG_PWM_CMR0     REG_ACCESS(RwReg, 0x40094200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0    REG_ACCESS(RwReg, 0x40094204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CDTYUPD0 REG_ACCESS(WoReg, 0x40094208U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 0) */
#define REG_PWM_CPRD0    REG_ACCESS(RwReg, 0x4009420CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CPRDUPD0 REG_ACCESS(WoReg, 0x40094210U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 0) */
#define REG_PWM_CCNT0    REG_ACCESS(RoReg, 0x40094214U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_DT0      REG_ACCESS(RwReg, 0x40094218U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 0) */
#define REG_PWM_DTUPD0   REG_ACCESS(WoReg, 0x4009421CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 0) */
#define REG_PWM_CMR1     REG_ACCESS(RwReg, 0x40094220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1    REG_ACCESS(RwReg, 0x40094224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CDTYUPD1 REG_ACCESS(WoReg, 0x40094228U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 1) */
#define REG_PWM_CPRD1    REG_ACCESS(RwReg, 0x4009422CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CPRDUPD1 REG_ACCESS(WoReg, 0x40094230U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 1) */
#define REG_PWM_CCNT1    REG_ACCESS(RoReg, 0x40094234U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_DT1      REG_ACCESS(RwReg, 0x40094238U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 1) */
#define REG_PWM_DTUPD1   REG_ACCESS(WoReg, 0x4009423CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 1) */
#define REG_PWM_CMR2     REG_ACCESS(RwReg, 0x40094240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2    REG_ACCESS(RwReg, 0x40094244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CDTYUPD2 REG_ACCESS(WoReg, 0x40094248U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 2) */
#define REG_PWM_CPRD2    REG_ACCESS(RwReg, 0x4009424CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CPRDUPD2 REG_ACCESS(WoReg, 0x40094250U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 2) */
#define REG_PWM_CCNT2    REG_ACCESS(RoReg, 0x40094254U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_DT2      REG_ACCESS(RwReg, 0x40094258U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 2) */
#define REG_PWM_DTUPD2   REG_ACCESS(WoReg, 0x4009425CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 2) */
#define REG_PWM_CMR3     REG_ACCESS(RwReg, 0x40094260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3    REG_ACCESS(RwReg, 0x40094264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CDTYUPD3 REG_ACCESS(WoReg, 0x40094268U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 3) */
#define REG_PWM_CPRD3    REG_ACCESS(RwReg, 0x4009426CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CPRDUPD3 REG_ACCESS(WoReg, 0x40094270U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 3) */
#define REG_PWM_CCNT3    REG_ACCESS(RoReg, 0x40094274U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_DT3      REG_ACCESS(RwReg, 0x40094278U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 3) */
#define REG_PWM_DTUPD3   REG_ACCESS(WoReg, 0x4009427CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 3) */
#define REG_PWM_CMR4     REG_ACCESS(RwReg, 0x40094280U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 4) */
#define REG_PWM_CDTY4    REG_ACCESS(RwReg, 0x40094284U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 4) */
#define REG_PWM_CDTYUPD4 REG_ACCESS(WoReg, 0x40094288U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 4) */
#define REG_PWM_CPRD4    REG_ACCESS(RwReg, 0x4009428CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 4) */
#define REG_PWM_CPRDUPD4 REG_ACCESS(WoReg, 0x40094290U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 4) */
#define REG_PWM_CCNT4    REG_ACCESS(RoReg, 0x40094294U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 4) */
#define REG_PWM_DT4      REG_ACCESS(RwReg, 0x40094298U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 4) */
#define REG_PWM_DTUPD4   REG_ACCESS(WoReg, 0x4009429CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 4) */
#define REG_PWM_CMR5     REG_ACCESS(RwReg, 0x400942A0U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 5) */
#define REG_PWM_CDTY5    REG_ACCESS(RwReg, 0x400942A4U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 5) */
#define REG_PWM_CDTYUPD5 REG_ACCESS(WoReg, 0x400942A8U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 5) */
#define REG_PWM_CPRD5    REG_ACCESS(RwReg, 0x400942ACU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 5) */
#define REG_PWM_CPRDUPD5 REG_ACCESS(WoReg, 0x400942B0U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 5) */
#define REG_PWM_CCNT5    REG_ACCESS(RoReg, 0x400942B4U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 5) */
#define REG_PWM_DT5      REG_ACCESS(RwReg, 0x400942B8U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 5) */
#define REG_PWM_DTUPD5   REG_ACCESS(WoReg, 0x400942BCU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 5) */
#define REG_PWM_CMR6     REG_ACCESS(RwReg, 0x400942C0U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 6) */
#define REG_PWM_CDTY6    REG_ACCESS(RwReg, 0x400942C4U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 6) */
#define REG_PWM_CDTYUPD6 REG_ACCESS(WoReg, 0x400942C8U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 6) */
#define REG_PWM_CPRD6    REG_ACCESS(RwReg, 0x400942CCU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 6) */
#define REG_PWM_CPRDUPD6 REG_ACCESS(WoReg, 0x400942D0U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 6) */
#define REG_PWM_CCNT6    REG_ACCESS(RoReg, 0x400942D4U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 6) */
#define REG_PWM_DT6      REG_ACCESS(RwReg, 0x400942D8U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 6) */
#define REG_PWM_DTUPD6   REG_ACCESS(WoReg, 0x400942DCU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 6) */
#define REG_PWM_CMR7     REG_ACCESS(RwReg, 0x400942E0U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 7) */
#define REG_PWM_CDTY7    REG_ACCESS(RwReg, 0x400942E4U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 7) */
#define REG_PWM_CDTYUPD7 REG_ACCESS(WoReg, 0x400942E8U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 7) */
#define REG_PWM_CPRD7    REG_ACCESS(RwReg, 0x400942ECU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 7) */
#define REG_PWM_CPRDUPD7 REG_ACCESS(WoReg, 0x400942F0U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 7) */
#define REG_PWM_CCNT7    REG_ACCESS(RoReg, 0x400942F4U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 7) */
#define REG_PWM_DT7      REG_ACCESS(RwReg, 0x400942F8U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 7) */
#define REG_PWM_DTUPD7   REG_ACCESS(WoReg, 0x400942FCU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 7) */

#endif /* _SAM3XA_PWM_INSTANCE_ */
