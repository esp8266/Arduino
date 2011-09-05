/* %ATMEL_LICENCE% */

#ifndef _SAM3S_PWM_INSTANCE_
#define _SAM3S_PWM_INSTANCE_

/* ========== Register definition for PWM peripheral ========== */
#define REG_PWM_CLK      REG_ACCESS(RwReg, 0x40020000U) /**< \brief (PWM) PWM Clock Register */
#define REG_PWM_ENA      REG_ACCESS(WoReg, 0x40020004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS      REG_ACCESS(WoReg, 0x40020008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR       REG_ACCESS(RoReg, 0x4002000CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER1     REG_ACCESS(WoReg, 0x40020010U) /**< \brief (PWM) PWM Interrupt Enable Register 1 */
#define REG_PWM_IDR1     REG_ACCESS(WoReg, 0x40020014U) /**< \brief (PWM) PWM Interrupt Disable Register 1 */
#define REG_PWM_IMR1     REG_ACCESS(RoReg, 0x40020018U) /**< \brief (PWM) PWM Interrupt Mask Register 1 */
#define REG_PWM_ISR1     REG_ACCESS(RoReg, 0x4002001CU) /**< \brief (PWM) PWM Interrupt Status Register 1 */
#define REG_PWM_SCM      REG_ACCESS(RwReg, 0x40020020U) /**< \brief (PWM) PWM Sync Channels Mode Register */
#define REG_PWM_SCUC     REG_ACCESS(RwReg, 0x40020028U) /**< \brief (PWM) PWM Sync Channels Update Control Register */
#define REG_PWM_SCUP     REG_ACCESS(RwReg, 0x4002002CU) /**< \brief (PWM) PWM Sync Channels Update Period Register */
#define REG_PWM_SCUPUPD  REG_ACCESS(WoReg, 0x40020030U) /**< \brief (PWM) PWM Sync Channels Update Period Update Register */
#define REG_PWM_IER2     REG_ACCESS(WoReg, 0x40020034U) /**< \brief (PWM) PWM Interrupt Enable Register 2 */
#define REG_PWM_IDR2     REG_ACCESS(WoReg, 0x40020038U) /**< \brief (PWM) PWM Interrupt Disable Register 2 */
#define REG_PWM_IMR2     REG_ACCESS(RoReg, 0x4002003CU) /**< \brief (PWM) PWM Interrupt Mask Register 2 */
#define REG_PWM_ISR2     REG_ACCESS(RoReg, 0x40020040U) /**< \brief (PWM) PWM Interrupt Status Register 2 */
#define REG_PWM_OOV      REG_ACCESS(RwReg, 0x40020044U) /**< \brief (PWM) PWM Output Override Value Register */
#define REG_PWM_OS       REG_ACCESS(RwReg, 0x40020048U) /**< \brief (PWM) PWM Output Selection Register */
#define REG_PWM_OSS      REG_ACCESS(WoReg, 0x4002004CU) /**< \brief (PWM) PWM Output Selection Set Register */
#define REG_PWM_OSC      REG_ACCESS(WoReg, 0x40020050U) /**< \brief (PWM) PWM Output Selection Clear Register */
#define REG_PWM_OSSUPD   REG_ACCESS(WoReg, 0x40020054U) /**< \brief (PWM) PWM Output Selection Set Update Register */
#define REG_PWM_OSCUPD   REG_ACCESS(WoReg, 0x40020058U) /**< \brief (PWM) PWM Output Selection Clear Update Register */
#define REG_PWM_FMR      REG_ACCESS(RwReg, 0x4002005CU) /**< \brief (PWM) PWM Fault Mode Register */
#define REG_PWM_FSR      REG_ACCESS(RoReg, 0x40020060U) /**< \brief (PWM) PWM Fault Status Register */
#define REG_PWM_FCR      REG_ACCESS(WoReg, 0x40020064U) /**< \brief (PWM) PWM Fault Clear Register */
#define REG_PWM_FPV      REG_ACCESS(RwReg, 0x40020068U) /**< \brief (PWM) PWM Fault Protection Value Register */
#define REG_PWM_FPE      REG_ACCESS(RwReg, 0x4002006CU) /**< \brief (PWM) PWM Fault Protection Enable Register */
#define REG_PWM_ELMR     REG_ACCESS(RwReg, 0x4002007CU) /**< \brief (PWM) PWM Event Line 0 Mode Register */
#define REG_PWM_SMMR     REG_ACCESS(RwReg, 0x400200B0U) /**< \brief (PWM) PWM Stepper Motor Mode Register */
#define REG_PWM_WPCR     REG_ACCESS(WoReg, 0x400200E4U) /**< \brief (PWM) PWM Write Protect Control Register */
#define REG_PWM_WPSR     REG_ACCESS(RoReg, 0x400200E8U) /**< \brief (PWM) PWM Write Protect Status Register */
#define REG_PWM_RPR      REG_ACCESS(RwReg, 0x40020100U) /**< \brief (PWM) Receive Pointer Register */
#define REG_PWM_RCR      REG_ACCESS(RwReg, 0x40020104U) /**< \brief (PWM) Receive Counter Register */
#define REG_PWM_TPR      REG_ACCESS(RwReg, 0x40020108U) /**< \brief (PWM) Transmit Pointer Register */
#define REG_PWM_TCR      REG_ACCESS(RwReg, 0x4002010CU) /**< \brief (PWM) Transmit Counter Register */
#define REG_PWM_RNPR     REG_ACCESS(RwReg, 0x40020110U) /**< \brief (PWM) Receive Next Pointer Register */
#define REG_PWM_RNCR     REG_ACCESS(RwReg, 0x40020114U) /**< \brief (PWM) Receive Next Counter Register */
#define REG_PWM_TNPR     REG_ACCESS(RwReg, 0x40020118U) /**< \brief (PWM) Transmit Next Pointer Register */
#define REG_PWM_TNCR     REG_ACCESS(RwReg, 0x4002011CU) /**< \brief (PWM) Transmit Next Counter Register */
#define REG_PWM_PTCR     REG_ACCESS(WoReg, 0x40020120U) /**< \brief (PWM) Transfer Control Register */
#define REG_PWM_PTSR     REG_ACCESS(RoReg, 0x40020124U) /**< \brief (PWM) Transfer Status Register */
#define REG_PWM_CMPV0    REG_ACCESS(RwReg, 0x40020130U) /**< \brief (PWM) PWM Comparison 0 Value Register */
#define REG_PWM_CMPVUPD0 REG_ACCESS(WoReg, 0x40020134U) /**< \brief (PWM) PWM Comparison 0 Value Update Register */
#define REG_PWM_CMPM0    REG_ACCESS(RwReg, 0x40020138U) /**< \brief (PWM) PWM Comparison 0 Mode Register */
#define REG_PWM_CMPMUPD0 REG_ACCESS(WoReg, 0x4002013CU) /**< \brief (PWM) PWM Comparison 0 Mode Update Register */
#define REG_PWM_CMPV1    REG_ACCESS(RwReg, 0x40020140U) /**< \brief (PWM) PWM Comparison 1 Value Register */
#define REG_PWM_CMPVUPD1 REG_ACCESS(WoReg, 0x40020144U) /**< \brief (PWM) PWM Comparison 1 Value Update Register */
#define REG_PWM_CMPM1    REG_ACCESS(RwReg, 0x40020148U) /**< \brief (PWM) PWM Comparison 1 Mode Register */
#define REG_PWM_CMPMUPD1 REG_ACCESS(WoReg, 0x4002014CU) /**< \brief (PWM) PWM Comparison 1 Mode Update Register */
#define REG_PWM_CMPV2    REG_ACCESS(RwReg, 0x40020150U) /**< \brief (PWM) PWM Comparison 2 Value Register */
#define REG_PWM_CMPVUPD2 REG_ACCESS(WoReg, 0x40020154U) /**< \brief (PWM) PWM Comparison 2 Value Update Register */
#define REG_PWM_CMPM2    REG_ACCESS(RwReg, 0x40020158U) /**< \brief (PWM) PWM Comparison 2 Mode Register */
#define REG_PWM_CMPMUPD2 REG_ACCESS(WoReg, 0x4002015CU) /**< \brief (PWM) PWM Comparison 2 Mode Update Register */
#define REG_PWM_CMPV3    REG_ACCESS(RwReg, 0x40020160U) /**< \brief (PWM) PWM Comparison 3 Value Register */
#define REG_PWM_CMPVUPD3 REG_ACCESS(WoReg, 0x40020164U) /**< \brief (PWM) PWM Comparison 3 Value Update Register */
#define REG_PWM_CMPM3    REG_ACCESS(RwReg, 0x40020168U) /**< \brief (PWM) PWM Comparison 3 Mode Register */
#define REG_PWM_CMPMUPD3 REG_ACCESS(WoReg, 0x4002016CU) /**< \brief (PWM) PWM Comparison 3 Mode Update Register */
#define REG_PWM_CMPV4    REG_ACCESS(RwReg, 0x40020170U) /**< \brief (PWM) PWM Comparison 4 Value Register */
#define REG_PWM_CMPVUPD4 REG_ACCESS(WoReg, 0x40020174U) /**< \brief (PWM) PWM Comparison 4 Value Update Register */
#define REG_PWM_CMPM4    REG_ACCESS(RwReg, 0x40020178U) /**< \brief (PWM) PWM Comparison 4 Mode Register */
#define REG_PWM_CMPMUPD4 REG_ACCESS(WoReg, 0x4002017CU) /**< \brief (PWM) PWM Comparison 4 Mode Update Register */
#define REG_PWM_CMPV5    REG_ACCESS(RwReg, 0x40020180U) /**< \brief (PWM) PWM Comparison 5 Value Register */
#define REG_PWM_CMPVUPD5 REG_ACCESS(WoReg, 0x40020184U) /**< \brief (PWM) PWM Comparison 5 Value Update Register */
#define REG_PWM_CMPM5    REG_ACCESS(RwReg, 0x40020188U) /**< \brief (PWM) PWM Comparison 5 Mode Register */
#define REG_PWM_CMPMUPD5 REG_ACCESS(WoReg, 0x4002018CU) /**< \brief (PWM) PWM Comparison 5 Mode Update Register */
#define REG_PWM_CMPV6    REG_ACCESS(RwReg, 0x40020190U) /**< \brief (PWM) PWM Comparison 6 Value Register */
#define REG_PWM_CMPVUPD6 REG_ACCESS(WoReg, 0x40020194U) /**< \brief (PWM) PWM Comparison 6 Value Update Register */
#define REG_PWM_CMPM6    REG_ACCESS(RwReg, 0x40020198U) /**< \brief (PWM) PWM Comparison 6 Mode Register */
#define REG_PWM_CMPMUPD6 REG_ACCESS(WoReg, 0x4002019CU) /**< \brief (PWM) PWM Comparison 6 Mode Update Register */
#define REG_PWM_CMPV7    REG_ACCESS(RwReg, 0x400201A0U) /**< \brief (PWM) PWM Comparison 7 Value Register */
#define REG_PWM_CMPVUPD7 REG_ACCESS(WoReg, 0x400201A4U) /**< \brief (PWM) PWM Comparison 7 Value Update Register */
#define REG_PWM_CMPM7    REG_ACCESS(RwReg, 0x400201A8U) /**< \brief (PWM) PWM Comparison 7 Mode Register */
#define REG_PWM_CMPMUPD7 REG_ACCESS(WoReg, 0x400201ACU) /**< \brief (PWM) PWM Comparison 7 Mode Update Register */
#define REG_PWM_CMR0     REG_ACCESS(RwReg, 0x40020200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0    REG_ACCESS(RwReg, 0x40020204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CDTYUPD0 REG_ACCESS(WoReg, 0x40020208U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 0) */
#define REG_PWM_CPRD0    REG_ACCESS(RwReg, 0x4002020CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CPRDUPD0 REG_ACCESS(WoReg, 0x40020210U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 0) */
#define REG_PWM_CCNT0    REG_ACCESS(RoReg, 0x40020214U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_DT0      REG_ACCESS(RwReg, 0x40020218U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 0) */
#define REG_PWM_DTUPD0   REG_ACCESS(WoReg, 0x4002021CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 0) */
#define REG_PWM_CMR1     REG_ACCESS(RwReg, 0x40020220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1    REG_ACCESS(RwReg, 0x40020224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CDTYUPD1 REG_ACCESS(WoReg, 0x40020228U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 1) */
#define REG_PWM_CPRD1    REG_ACCESS(RwReg, 0x4002022CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CPRDUPD1 REG_ACCESS(WoReg, 0x40020230U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 1) */
#define REG_PWM_CCNT1    REG_ACCESS(RoReg, 0x40020234U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_DT1      REG_ACCESS(RwReg, 0x40020238U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 1) */
#define REG_PWM_DTUPD1   REG_ACCESS(WoReg, 0x4002023CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 1) */
#define REG_PWM_CMR2     REG_ACCESS(RwReg, 0x40020240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2    REG_ACCESS(RwReg, 0x40020244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CDTYUPD2 REG_ACCESS(WoReg, 0x40020248U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 2) */
#define REG_PWM_CPRD2    REG_ACCESS(RwReg, 0x4002024CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CPRDUPD2 REG_ACCESS(WoReg, 0x40020250U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 2) */
#define REG_PWM_CCNT2    REG_ACCESS(RoReg, 0x40020254U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_DT2      REG_ACCESS(RwReg, 0x40020258U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 2) */
#define REG_PWM_DTUPD2   REG_ACCESS(WoReg, 0x4002025CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 2) */
#define REG_PWM_CMR3     REG_ACCESS(RwReg, 0x40020260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3    REG_ACCESS(RwReg, 0x40020264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CDTYUPD3 REG_ACCESS(WoReg, 0x40020268U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 3) */
#define REG_PWM_CPRD3    REG_ACCESS(RwReg, 0x4002026CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CPRDUPD3 REG_ACCESS(WoReg, 0x40020270U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 3) */
#define REG_PWM_CCNT3    REG_ACCESS(RoReg, 0x40020274U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_DT3      REG_ACCESS(RwReg, 0x40020278U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 3) */
#define REG_PWM_DTUPD3   REG_ACCESS(WoReg, 0x4002027CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 3) */

#endif /* _SAM3S_PWM_INSTANCE_ */
