/* %ATMEL_LICENCE% */

#ifndef _SAM3U_PWM_INSTANCE_
#define _SAM3U_PWM_INSTANCE_

/* ========== Register definition for PWM peripheral ========== */
#define REG_PWM_CLK      REG_ACCESS(RwReg, 0x4008C000U) /**< \brief (PWM) PWM Clock Register */
#define REG_PWM_ENA      REG_ACCESS(WoReg, 0x4008C004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS      REG_ACCESS(WoReg, 0x4008C008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR       REG_ACCESS(RoReg, 0x4008C00CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER1     REG_ACCESS(WoReg, 0x4008C010U) /**< \brief (PWM) PWM Interrupt Enable Register 1 */
#define REG_PWM_IDR1     REG_ACCESS(WoReg, 0x4008C014U) /**< \brief (PWM) PWM Interrupt Disable Register 1 */
#define REG_PWM_IMR1     REG_ACCESS(RoReg, 0x4008C018U) /**< \brief (PWM) PWM Interrupt Mask Register 1 */
#define REG_PWM_ISR1     REG_ACCESS(RoReg, 0x4008C01CU) /**< \brief (PWM) PWM Interrupt Status Register 1 */
#define REG_PWM_SCM      REG_ACCESS(RwReg, 0x4008C020U) /**< \brief (PWM) PWM Sync Channels Mode Register */
#define REG_PWM_SCUC     REG_ACCESS(RwReg, 0x4008C028U) /**< \brief (PWM) PWM Sync Channels Update Control Register */
#define REG_PWM_SCUP     REG_ACCESS(RwReg, 0x4008C02CU) /**< \brief (PWM) PWM Sync Channels Update Period Register */
#define REG_PWM_SCUPUPD  REG_ACCESS(WoReg, 0x4008C030U) /**< \brief (PWM) PWM Sync Channels Update Period Update Register */
#define REG_PWM_IER2     REG_ACCESS(WoReg, 0x4008C034U) /**< \brief (PWM) PWM Interrupt Enable Register 2 */
#define REG_PWM_IDR2     REG_ACCESS(WoReg, 0x4008C038U) /**< \brief (PWM) PWM Interrupt Disable Register 2 */
#define REG_PWM_IMR2     REG_ACCESS(RoReg, 0x4008C03CU) /**< \brief (PWM) PWM Interrupt Mask Register 2 */
#define REG_PWM_ISR2     REG_ACCESS(RoReg, 0x4008C040U) /**< \brief (PWM) PWM Interrupt Status Register 2 */
#define REG_PWM_OOV      REG_ACCESS(RwReg, 0x4008C044U) /**< \brief (PWM) PWM Output Override Value Register */
#define REG_PWM_OS       REG_ACCESS(RwReg, 0x4008C048U) /**< \brief (PWM) PWM Output Selection Register */
#define REG_PWM_OSS      REG_ACCESS(WoReg, 0x4008C04CU) /**< \brief (PWM) PWM Output Selection Set Register */
#define REG_PWM_OSC      REG_ACCESS(WoReg, 0x4008C050U) /**< \brief (PWM) PWM Output Selection Clear Register */
#define REG_PWM_OSSUPD   REG_ACCESS(WoReg, 0x4008C054U) /**< \brief (PWM) PWM Output Selection Set Update Register */
#define REG_PWM_OSCUPD   REG_ACCESS(WoReg, 0x4008C058U) /**< \brief (PWM) PWM Output Selection Clear Update Register */
#define REG_PWM_FMR      REG_ACCESS(RwReg, 0x4008C05CU) /**< \brief (PWM) PWM Fault Mode Register */
#define REG_PWM_FSR      REG_ACCESS(RoReg, 0x4008C060U) /**< \brief (PWM) PWM Fault Status Register */
#define REG_PWM_FCR      REG_ACCESS(WoReg, 0x4008C064U) /**< \brief (PWM) PWM Fault Clear Register */
#define REG_PWM_FPV      REG_ACCESS(RwReg, 0x4008C068U) /**< \brief (PWM) PWM Fault Protection Value Register */
#define REG_PWM_FPE      REG_ACCESS(RwReg, 0x4008C06CU) /**< \brief (PWM) PWM Fault Protection Enable Register */
#define REG_PWM_ELMR     REG_ACCESS(RwReg, 0x4008C07CU) /**< \brief (PWM) PWM Event Line 0 Mode Register */
#define REG_PWM_WPCR     REG_ACCESS(WoReg, 0x4008C0E4U) /**< \brief (PWM) PWM Write Protect Control Register */
#define REG_PWM_WPSR     REG_ACCESS(RoReg, 0x4008C0E8U) /**< \brief (PWM) PWM Write Protect Status Register */
#define REG_PWM_RPR      REG_ACCESS(RwReg, 0x4008C100U) /**< \brief (PWM) Receive Pointer Register */
#define REG_PWM_RCR      REG_ACCESS(RwReg, 0x4008C104U) /**< \brief (PWM) Receive Counter Register */
#define REG_PWM_TPR      REG_ACCESS(RwReg, 0x4008C108U) /**< \brief (PWM) Transmit Pointer Register */
#define REG_PWM_TCR      REG_ACCESS(RwReg, 0x4008C10CU) /**< \brief (PWM) Transmit Counter Register */
#define REG_PWM_RNPR     REG_ACCESS(RwReg, 0x4008C110U) /**< \brief (PWM) Receive Next Pointer Register */
#define REG_PWM_RNCR     REG_ACCESS(RwReg, 0x4008C114U) /**< \brief (PWM) Receive Next Counter Register */
#define REG_PWM_TNPR     REG_ACCESS(RwReg, 0x4008C118U) /**< \brief (PWM) Transmit Next Pointer Register */
#define REG_PWM_TNCR     REG_ACCESS(RwReg, 0x4008C11CU) /**< \brief (PWM) Transmit Next Counter Register */
#define REG_PWM_PTCR     REG_ACCESS(WoReg, 0x4008C120U) /**< \brief (PWM) Transfer Control Register */
#define REG_PWM_PTSR     REG_ACCESS(RoReg, 0x4008C124U) /**< \brief (PWM) Transfer Status Register */
#define REG_PWM_CMPV0    REG_ACCESS(RwReg, 0x4008C130U) /**< \brief (PWM) PWM Comparison 0 Value Register */
#define REG_PWM_CMPVUPD0 REG_ACCESS(WoReg, 0x4008C134U) /**< \brief (PWM) PWM Comparison 0 Value Update Register */
#define REG_PWM_CMPM0    REG_ACCESS(RwReg, 0x4008C138U) /**< \brief (PWM) PWM Comparison 0 Mode Register */
#define REG_PWM_CMPMUPD0 REG_ACCESS(WoReg, 0x4008C13CU) /**< \brief (PWM) PWM Comparison 0 Mode Update Register */
#define REG_PWM_CMPV1    REG_ACCESS(RwReg, 0x4008C140U) /**< \brief (PWM) PWM Comparison 1 Value Register */
#define REG_PWM_CMPVUPD1 REG_ACCESS(WoReg, 0x4008C144U) /**< \brief (PWM) PWM Comparison 1 Value Update Register */
#define REG_PWM_CMPM1    REG_ACCESS(RwReg, 0x4008C148U) /**< \brief (PWM) PWM Comparison 1 Mode Register */
#define REG_PWM_CMPMUPD1 REG_ACCESS(WoReg, 0x4008C14CU) /**< \brief (PWM) PWM Comparison 1 Mode Update Register */
#define REG_PWM_CMPV2    REG_ACCESS(RwReg, 0x4008C150U) /**< \brief (PWM) PWM Comparison 2 Value Register */
#define REG_PWM_CMPVUPD2 REG_ACCESS(WoReg, 0x4008C154U) /**< \brief (PWM) PWM Comparison 2 Value Update Register */
#define REG_PWM_CMPM2    REG_ACCESS(RwReg, 0x4008C158U) /**< \brief (PWM) PWM Comparison 2 Mode Register */
#define REG_PWM_CMPMUPD2 REG_ACCESS(WoReg, 0x4008C15CU) /**< \brief (PWM) PWM Comparison 2 Mode Update Register */
#define REG_PWM_CMPV3    REG_ACCESS(RwReg, 0x4008C160U) /**< \brief (PWM) PWM Comparison 3 Value Register */
#define REG_PWM_CMPVUPD3 REG_ACCESS(WoReg, 0x4008C164U) /**< \brief (PWM) PWM Comparison 3 Value Update Register */
#define REG_PWM_CMPM3    REG_ACCESS(RwReg, 0x4008C168U) /**< \brief (PWM) PWM Comparison 3 Mode Register */
#define REG_PWM_CMPMUPD3 REG_ACCESS(WoReg, 0x4008C16CU) /**< \brief (PWM) PWM Comparison 3 Mode Update Register */
#define REG_PWM_CMPV4    REG_ACCESS(RwReg, 0x4008C170U) /**< \brief (PWM) PWM Comparison 4 Value Register */
#define REG_PWM_CMPVUPD4 REG_ACCESS(WoReg, 0x4008C174U) /**< \brief (PWM) PWM Comparison 4 Value Update Register */
#define REG_PWM_CMPM4    REG_ACCESS(RwReg, 0x4008C178U) /**< \brief (PWM) PWM Comparison 4 Mode Register */
#define REG_PWM_CMPMUPD4 REG_ACCESS(WoReg, 0x4008C17CU) /**< \brief (PWM) PWM Comparison 4 Mode Update Register */
#define REG_PWM_CMPV5    REG_ACCESS(RwReg, 0x4008C180U) /**< \brief (PWM) PWM Comparison 5 Value Register */
#define REG_PWM_CMPVUPD5 REG_ACCESS(WoReg, 0x4008C184U) /**< \brief (PWM) PWM Comparison 5 Value Update Register */
#define REG_PWM_CMPM5    REG_ACCESS(RwReg, 0x4008C188U) /**< \brief (PWM) PWM Comparison 5 Mode Register */
#define REG_PWM_CMPMUPD5 REG_ACCESS(WoReg, 0x4008C18CU) /**< \brief (PWM) PWM Comparison 5 Mode Update Register */
#define REG_PWM_CMPV6    REG_ACCESS(RwReg, 0x4008C190U) /**< \brief (PWM) PWM Comparison 6 Value Register */
#define REG_PWM_CMPVUPD6 REG_ACCESS(WoReg, 0x4008C194U) /**< \brief (PWM) PWM Comparison 6 Value Update Register */
#define REG_PWM_CMPM6    REG_ACCESS(RwReg, 0x4008C198U) /**< \brief (PWM) PWM Comparison 6 Mode Register */
#define REG_PWM_CMPMUPD6 REG_ACCESS(WoReg, 0x4008C19CU) /**< \brief (PWM) PWM Comparison 6 Mode Update Register */
#define REG_PWM_CMPV7    REG_ACCESS(RwReg, 0x4008C1A0U) /**< \brief (PWM) PWM Comparison 7 Value Register */
#define REG_PWM_CMPVUPD7 REG_ACCESS(WoReg, 0x4008C1A4U) /**< \brief (PWM) PWM Comparison 7 Value Update Register */
#define REG_PWM_CMPM7    REG_ACCESS(RwReg, 0x4008C1A8U) /**< \brief (PWM) PWM Comparison 7 Mode Register */
#define REG_PWM_CMPMUPD7 REG_ACCESS(WoReg, 0x4008C1ACU) /**< \brief (PWM) PWM Comparison 7 Mode Update Register */
#define REG_PWM_CMR0     REG_ACCESS(RwReg, 0x4008C200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0    REG_ACCESS(RwReg, 0x4008C204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CDTYUPD0 REG_ACCESS(WoReg, 0x4008C208U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 0) */
#define REG_PWM_CPRD0    REG_ACCESS(RwReg, 0x4008C20CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CPRDUPD0 REG_ACCESS(WoReg, 0x4008C210U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 0) */
#define REG_PWM_CCNT0    REG_ACCESS(RoReg, 0x4008C214U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_DT0      REG_ACCESS(RwReg, 0x4008C218U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 0) */
#define REG_PWM_DTUPD0   REG_ACCESS(WoReg, 0x4008C21CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 0) */
#define REG_PWM_CMR1     REG_ACCESS(RwReg, 0x4008C220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1    REG_ACCESS(RwReg, 0x4008C224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CDTYUPD1 REG_ACCESS(WoReg, 0x4008C228U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 1) */
#define REG_PWM_CPRD1    REG_ACCESS(RwReg, 0x4008C22CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CPRDUPD1 REG_ACCESS(WoReg, 0x4008C230U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 1) */
#define REG_PWM_CCNT1    REG_ACCESS(RoReg, 0x4008C234U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_DT1      REG_ACCESS(RwReg, 0x4008C238U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 1) */
#define REG_PWM_DTUPD1   REG_ACCESS(WoReg, 0x4008C23CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 1) */
#define REG_PWM_CMR2     REG_ACCESS(RwReg, 0x4008C240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2    REG_ACCESS(RwReg, 0x4008C244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CDTYUPD2 REG_ACCESS(WoReg, 0x4008C248U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 2) */
#define REG_PWM_CPRD2    REG_ACCESS(RwReg, 0x4008C24CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CPRDUPD2 REG_ACCESS(WoReg, 0x4008C250U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 2) */
#define REG_PWM_CCNT2    REG_ACCESS(RoReg, 0x4008C254U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_DT2      REG_ACCESS(RwReg, 0x4008C258U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 2) */
#define REG_PWM_DTUPD2   REG_ACCESS(WoReg, 0x4008C25CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 2) */
#define REG_PWM_CMR3     REG_ACCESS(RwReg, 0x4008C260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3    REG_ACCESS(RwReg, 0x4008C264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CDTYUPD3 REG_ACCESS(WoReg, 0x4008C268U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 3) */
#define REG_PWM_CPRD3    REG_ACCESS(RwReg, 0x4008C26CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CPRDUPD3 REG_ACCESS(WoReg, 0x4008C270U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 3) */
#define REG_PWM_CCNT3    REG_ACCESS(RoReg, 0x4008C274U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_DT3      REG_ACCESS(RwReg, 0x4008C278U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 3) */
#define REG_PWM_DTUPD3   REG_ACCESS(WoReg, 0x4008C27CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 3) */

#endif /* _SAM3U_PWM_INSTANCE_ */
