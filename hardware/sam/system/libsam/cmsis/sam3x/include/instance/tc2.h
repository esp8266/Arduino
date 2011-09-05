/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_TC2_INSTANCE_
#define _SAM3XA_TC2_INSTANCE_

/* ========== Register definition for TC2 peripheral ========== */
#define REG_TC2_CCR0  REG_ACCESS(WoReg, 0x40088000U) /**< \brief (TC2) Channel Control Register (channel = 0) */
#define REG_TC2_CMR0  REG_ACCESS(RwReg, 0x40088004U) /**< \brief (TC2) Channel Mode Register (channel = 0) */
#define REG_TC2_SMMR0 REG_ACCESS(RwReg, 0x40088008U) /**< \brief (TC2) Stepper Motor Mode Register (channel = 0) */
#define REG_TC2_CV0   REG_ACCESS(RoReg, 0x40088010U) /**< \brief (TC2) Counter Value (channel = 0) */
#define REG_TC2_RA0   REG_ACCESS(RwReg, 0x40088014U) /**< \brief (TC2) Register A (channel = 0) */
#define REG_TC2_RB0   REG_ACCESS(RwReg, 0x40088018U) /**< \brief (TC2) Register B (channel = 0) */
#define REG_TC2_RC0   REG_ACCESS(RwReg, 0x4008801CU) /**< \brief (TC2) Register C (channel = 0) */
#define REG_TC2_SR0   REG_ACCESS(RoReg, 0x40088020U) /**< \brief (TC2) Status Register (channel = 0) */
#define REG_TC2_IER0  REG_ACCESS(WoReg, 0x40088024U) /**< \brief (TC2) Interrupt Enable Register (channel = 0) */
#define REG_TC2_IDR0  REG_ACCESS(WoReg, 0x40088028U) /**< \brief (TC2) Interrupt Disable Register (channel = 0) */
#define REG_TC2_IMR0  REG_ACCESS(RoReg, 0x4008802CU) /**< \brief (TC2) Interrupt Mask Register (channel = 0) */
#define REG_TC2_CCR1  REG_ACCESS(WoReg, 0x40088040U) /**< \brief (TC2) Channel Control Register (channel = 1) */
#define REG_TC2_CMR1  REG_ACCESS(RwReg, 0x40088044U) /**< \brief (TC2) Channel Mode Register (channel = 1) */
#define REG_TC2_SMMR1 REG_ACCESS(RwReg, 0x40088048U) /**< \brief (TC2) Stepper Motor Mode Register (channel = 1) */
#define REG_TC2_CV1   REG_ACCESS(RoReg, 0x40088050U) /**< \brief (TC2) Counter Value (channel = 1) */
#define REG_TC2_RA1   REG_ACCESS(RwReg, 0x40088054U) /**< \brief (TC2) Register A (channel = 1) */
#define REG_TC2_RB1   REG_ACCESS(RwReg, 0x40088058U) /**< \brief (TC2) Register B (channel = 1) */
#define REG_TC2_RC1   REG_ACCESS(RwReg, 0x4008805CU) /**< \brief (TC2) Register C (channel = 1) */
#define REG_TC2_SR1   REG_ACCESS(RoReg, 0x40088060U) /**< \brief (TC2) Status Register (channel = 1) */
#define REG_TC2_IER1  REG_ACCESS(WoReg, 0x40088064U) /**< \brief (TC2) Interrupt Enable Register (channel = 1) */
#define REG_TC2_IDR1  REG_ACCESS(WoReg, 0x40088068U) /**< \brief (TC2) Interrupt Disable Register (channel = 1) */
#define REG_TC2_IMR1  REG_ACCESS(RoReg, 0x4008806CU) /**< \brief (TC2) Interrupt Mask Register (channel = 1) */
#define REG_TC2_CCR2  REG_ACCESS(WoReg, 0x40088080U) /**< \brief (TC2) Channel Control Register (channel = 2) */
#define REG_TC2_CMR2  REG_ACCESS(RwReg, 0x40088084U) /**< \brief (TC2) Channel Mode Register (channel = 2) */
#define REG_TC2_SMMR2 REG_ACCESS(RwReg, 0x40088088U) /**< \brief (TC2) Stepper Motor Mode Register (channel = 2) */
#define REG_TC2_CV2   REG_ACCESS(RoReg, 0x40088090U) /**< \brief (TC2) Counter Value (channel = 2) */
#define REG_TC2_RA2   REG_ACCESS(RwReg, 0x40088094U) /**< \brief (TC2) Register A (channel = 2) */
#define REG_TC2_RB2   REG_ACCESS(RwReg, 0x40088098U) /**< \brief (TC2) Register B (channel = 2) */
#define REG_TC2_RC2   REG_ACCESS(RwReg, 0x4008809CU) /**< \brief (TC2) Register C (channel = 2) */
#define REG_TC2_SR2   REG_ACCESS(RoReg, 0x400880A0U) /**< \brief (TC2) Status Register (channel = 2) */
#define REG_TC2_IER2  REG_ACCESS(WoReg, 0x400880A4U) /**< \brief (TC2) Interrupt Enable Register (channel = 2) */
#define REG_TC2_IDR2  REG_ACCESS(WoReg, 0x400880A8U) /**< \brief (TC2) Interrupt Disable Register (channel = 2) */
#define REG_TC2_IMR2  REG_ACCESS(RoReg, 0x400880ACU) /**< \brief (TC2) Interrupt Mask Register (channel = 2) */
#define REG_TC2_BCR   REG_ACCESS(WoReg, 0x400880C0U) /**< \brief (TC2) Block Control Register */
#define REG_TC2_BMR   REG_ACCESS(RwReg, 0x400880C4U) /**< \brief (TC2) Block Mode Register */
#define REG_TC2_QIER  REG_ACCESS(WoReg, 0x400880C8U) /**< \brief (TC2) QDEC Interrupt Enable Register */
#define REG_TC2_QIDR  REG_ACCESS(WoReg, 0x400880CCU) /**< \brief (TC2) QDEC Interrupt Disable Register */
#define REG_TC2_QIMR  REG_ACCESS(RoReg, 0x400880D0U) /**< \brief (TC2) QDEC Interrupt Mask Register */
#define REG_TC2_QISR  REG_ACCESS(RoReg, 0x400880D4U) /**< \brief (TC2) QDEC Interrupt Status Register */
#define REG_TC2_FMR   REG_ACCESS(RwReg, 0x400880D8U) /**< \brief (TC2) Fault Mode Register */
#define REG_TC2_WPMR  REG_ACCESS(RwReg, 0x400880E4U) /**< \brief (TC2) Write Protect Mode Register */

#endif /* _SAM3XA_TC2_INSTANCE_ */
