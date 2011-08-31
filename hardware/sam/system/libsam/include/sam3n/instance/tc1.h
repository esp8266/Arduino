/* %ATMEL_LICENCE% */

#ifndef _SAM3N_TC1_INSTANCE_
#define _SAM3N_TC1_INSTANCE_

/* ========== Register definition for TC1 peripheral ========== */
#define REG_TC1_CCR0  REG_ACCESS(WoReg, 0x40014000U) /**< \brief (TC1) Channel Control Register (channel = 0) */
#define REG_TC1_CMR0  REG_ACCESS(RwReg, 0x40014004U) /**< \brief (TC1) Channel Mode Register (channel = 0) */
#define REG_TC1_SMMR0 REG_ACCESS(RwReg, 0x40014008U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 0) */
#define REG_TC1_CV0   REG_ACCESS(RoReg, 0x40014010U) /**< \brief (TC1) Counter Value (channel = 0) */
#define REG_TC1_RA0   REG_ACCESS(RwReg, 0x40014014U) /**< \brief (TC1) Register A (channel = 0) */
#define REG_TC1_RB0   REG_ACCESS(RwReg, 0x40014018U) /**< \brief (TC1) Register B (channel = 0) */
#define REG_TC1_RC0   REG_ACCESS(RwReg, 0x4001401CU) /**< \brief (TC1) Register C (channel = 0) */
#define REG_TC1_SR0   REG_ACCESS(RoReg, 0x40014020U) /**< \brief (TC1) Status Register (channel = 0) */
#define REG_TC1_IER0  REG_ACCESS(WoReg, 0x40014024U) /**< \brief (TC1) Interrupt Enable Register (channel = 0) */
#define REG_TC1_IDR0  REG_ACCESS(WoReg, 0x40014028U) /**< \brief (TC1) Interrupt Disable Register (channel = 0) */
#define REG_TC1_IMR0  REG_ACCESS(RoReg, 0x4001402CU) /**< \brief (TC1) Interrupt Mask Register (channel = 0) */
#define REG_TC1_CCR1  REG_ACCESS(WoReg, 0x40014040U) /**< \brief (TC1) Channel Control Register (channel = 1) */
#define REG_TC1_CMR1  REG_ACCESS(RwReg, 0x40014044U) /**< \brief (TC1) Channel Mode Register (channel = 1) */
#define REG_TC1_SMMR1 REG_ACCESS(RwReg, 0x40014048U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 1) */
#define REG_TC1_CV1   REG_ACCESS(RoReg, 0x40014050U) /**< \brief (TC1) Counter Value (channel = 1) */
#define REG_TC1_RA1   REG_ACCESS(RwReg, 0x40014054U) /**< \brief (TC1) Register A (channel = 1) */
#define REG_TC1_RB1   REG_ACCESS(RwReg, 0x40014058U) /**< \brief (TC1) Register B (channel = 1) */
#define REG_TC1_RC1   REG_ACCESS(RwReg, 0x4001405CU) /**< \brief (TC1) Register C (channel = 1) */
#define REG_TC1_SR1   REG_ACCESS(RoReg, 0x40014060U) /**< \brief (TC1) Status Register (channel = 1) */
#define REG_TC1_IER1  REG_ACCESS(WoReg, 0x40014064U) /**< \brief (TC1) Interrupt Enable Register (channel = 1) */
#define REG_TC1_IDR1  REG_ACCESS(WoReg, 0x40014068U) /**< \brief (TC1) Interrupt Disable Register (channel = 1) */
#define REG_TC1_IMR1  REG_ACCESS(RoReg, 0x4001406CU) /**< \brief (TC1) Interrupt Mask Register (channel = 1) */
#define REG_TC1_CCR2  REG_ACCESS(WoReg, 0x40014080U) /**< \brief (TC1) Channel Control Register (channel = 2) */
#define REG_TC1_CMR2  REG_ACCESS(RwReg, 0x40014084U) /**< \brief (TC1) Channel Mode Register (channel = 2) */
#define REG_TC1_SMMR2 REG_ACCESS(RwReg, 0x40014088U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 2) */
#define REG_TC1_CV2   REG_ACCESS(RoReg, 0x40014090U) /**< \brief (TC1) Counter Value (channel = 2) */
#define REG_TC1_RA2   REG_ACCESS(RwReg, 0x40014094U) /**< \brief (TC1) Register A (channel = 2) */
#define REG_TC1_RB2   REG_ACCESS(RwReg, 0x40014098U) /**< \brief (TC1) Register B (channel = 2) */
#define REG_TC1_RC2   REG_ACCESS(RwReg, 0x4001409CU) /**< \brief (TC1) Register C (channel = 2) */
#define REG_TC1_SR2   REG_ACCESS(RoReg, 0x400140A0U) /**< \brief (TC1) Status Register (channel = 2) */
#define REG_TC1_IER2  REG_ACCESS(WoReg, 0x400140A4U) /**< \brief (TC1) Interrupt Enable Register (channel = 2) */
#define REG_TC1_IDR2  REG_ACCESS(WoReg, 0x400140A8U) /**< \brief (TC1) Interrupt Disable Register (channel = 2) */
#define REG_TC1_IMR2  REG_ACCESS(RoReg, 0x400140ACU) /**< \brief (TC1) Interrupt Mask Register (channel = 2) */
#define REG_TC1_BCR   REG_ACCESS(WoReg, 0x400140C0U) /**< \brief (TC1) Block Control Register */
#define REG_TC1_BMR   REG_ACCESS(RwReg, 0x400140C4U) /**< \brief (TC1) Block Mode Register */
#define REG_TC1_QIER  REG_ACCESS(WoReg, 0x400140C8U) /**< \brief (TC1) QDEC Interrupt Enable Register */
#define REG_TC1_QIDR  REG_ACCESS(WoReg, 0x400140CCU) /**< \brief (TC1) QDEC Interrupt Disable Register */
#define REG_TC1_QIMR  REG_ACCESS(RoReg, 0x400140D0U) /**< \brief (TC1) QDEC Interrupt Mask Register */
#define REG_TC1_QISR  REG_ACCESS(RoReg, 0x400140D4U) /**< \brief (TC1) QDEC Interrupt Status Register */
#define REG_TC1_WPMR  REG_ACCESS(RwReg, 0x400140E4U) /**< \brief (TC1) Write Protect Mode Register */

#endif /* _SAM3N_TC1_INSTANCE_ */
