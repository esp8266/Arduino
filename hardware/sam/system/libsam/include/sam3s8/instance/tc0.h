/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_TC0_INSTANCE_
#define _SAM3S8_TC0_INSTANCE_

/* ========== Register definition for TC0 peripheral ========== */
#define REG_TC0_CCR0  REG_ACCESS(WoReg, 0x40010000U) /**< \brief (TC0) Channel Control Register (channel = 0) */
#define REG_TC0_CMR0  REG_ACCESS(RwReg, 0x40010004U) /**< \brief (TC0) Channel Mode Register (channel = 0) */
#define REG_TC0_SMMR0 REG_ACCESS(RwReg, 0x40010008U) /**< \brief (TC0) Stepper Motor Mode Register (channel = 0) */
#define REG_TC0_CV0   REG_ACCESS(RoReg, 0x40010010U) /**< \brief (TC0) Counter Value (channel = 0) */
#define REG_TC0_RA0   REG_ACCESS(RwReg, 0x40010014U) /**< \brief (TC0) Register A (channel = 0) */
#define REG_TC0_RB0   REG_ACCESS(RwReg, 0x40010018U) /**< \brief (TC0) Register B (channel = 0) */
#define REG_TC0_RC0   REG_ACCESS(RwReg, 0x4001001CU) /**< \brief (TC0) Register C (channel = 0) */
#define REG_TC0_SR0   REG_ACCESS(RoReg, 0x40010020U) /**< \brief (TC0) Status Register (channel = 0) */
#define REG_TC0_IER0  REG_ACCESS(WoReg, 0x40010024U) /**< \brief (TC0) Interrupt Enable Register (channel = 0) */
#define REG_TC0_IDR0  REG_ACCESS(WoReg, 0x40010028U) /**< \brief (TC0) Interrupt Disable Register (channel = 0) */
#define REG_TC0_IMR0  REG_ACCESS(RoReg, 0x4001002CU) /**< \brief (TC0) Interrupt Mask Register (channel = 0) */
#define REG_TC0_CCR1  REG_ACCESS(WoReg, 0x40010040U) /**< \brief (TC0) Channel Control Register (channel = 1) */
#define REG_TC0_CMR1  REG_ACCESS(RwReg, 0x40010044U) /**< \brief (TC0) Channel Mode Register (channel = 1) */
#define REG_TC0_SMMR1 REG_ACCESS(RwReg, 0x40010048U) /**< \brief (TC0) Stepper Motor Mode Register (channel = 1) */
#define REG_TC0_CV1   REG_ACCESS(RoReg, 0x40010050U) /**< \brief (TC0) Counter Value (channel = 1) */
#define REG_TC0_RA1   REG_ACCESS(RwReg, 0x40010054U) /**< \brief (TC0) Register A (channel = 1) */
#define REG_TC0_RB1   REG_ACCESS(RwReg, 0x40010058U) /**< \brief (TC0) Register B (channel = 1) */
#define REG_TC0_RC1   REG_ACCESS(RwReg, 0x4001005CU) /**< \brief (TC0) Register C (channel = 1) */
#define REG_TC0_SR1   REG_ACCESS(RoReg, 0x40010060U) /**< \brief (TC0) Status Register (channel = 1) */
#define REG_TC0_IER1  REG_ACCESS(WoReg, 0x40010064U) /**< \brief (TC0) Interrupt Enable Register (channel = 1) */
#define REG_TC0_IDR1  REG_ACCESS(WoReg, 0x40010068U) /**< \brief (TC0) Interrupt Disable Register (channel = 1) */
#define REG_TC0_IMR1  REG_ACCESS(RoReg, 0x4001006CU) /**< \brief (TC0) Interrupt Mask Register (channel = 1) */
#define REG_TC0_CCR2  REG_ACCESS(WoReg, 0x40010080U) /**< \brief (TC0) Channel Control Register (channel = 2) */
#define REG_TC0_CMR2  REG_ACCESS(RwReg, 0x40010084U) /**< \brief (TC0) Channel Mode Register (channel = 2) */
#define REG_TC0_SMMR2 REG_ACCESS(RwReg, 0x40010088U) /**< \brief (TC0) Stepper Motor Mode Register (channel = 2) */
#define REG_TC0_CV2   REG_ACCESS(RoReg, 0x40010090U) /**< \brief (TC0) Counter Value (channel = 2) */
#define REG_TC0_RA2   REG_ACCESS(RwReg, 0x40010094U) /**< \brief (TC0) Register A (channel = 2) */
#define REG_TC0_RB2   REG_ACCESS(RwReg, 0x40010098U) /**< \brief (TC0) Register B (channel = 2) */
#define REG_TC0_RC2   REG_ACCESS(RwReg, 0x4001009CU) /**< \brief (TC0) Register C (channel = 2) */
#define REG_TC0_SR2   REG_ACCESS(RoReg, 0x400100A0U) /**< \brief (TC0) Status Register (channel = 2) */
#define REG_TC0_IER2  REG_ACCESS(WoReg, 0x400100A4U) /**< \brief (TC0) Interrupt Enable Register (channel = 2) */
#define REG_TC0_IDR2  REG_ACCESS(WoReg, 0x400100A8U) /**< \brief (TC0) Interrupt Disable Register (channel = 2) */
#define REG_TC0_IMR2  REG_ACCESS(RoReg, 0x400100ACU) /**< \brief (TC0) Interrupt Mask Register (channel = 2) */
#define REG_TC0_BCR   REG_ACCESS(WoReg, 0x400100C0U) /**< \brief (TC0) Block Control Register */
#define REG_TC0_BMR   REG_ACCESS(RwReg, 0x400100C4U) /**< \brief (TC0) Block Mode Register */
#define REG_TC0_QIER  REG_ACCESS(WoReg, 0x400100C8U) /**< \brief (TC0) QDEC Interrupt Enable Register */
#define REG_TC0_QIDR  REG_ACCESS(WoReg, 0x400100CCU) /**< \brief (TC0) QDEC Interrupt Disable Register */
#define REG_TC0_QIMR  REG_ACCESS(RoReg, 0x400100D0U) /**< \brief (TC0) QDEC Interrupt Mask Register */
#define REG_TC0_QISR  REG_ACCESS(RoReg, 0x400100D4U) /**< \brief (TC0) QDEC Interrupt Status Register */
#define REG_TC0_FMR   REG_ACCESS(RwReg, 0x400100D8U) /**< \brief (TC0) Fault Mode Register */
#define REG_TC0_WPMR  REG_ACCESS(RwReg, 0x400100E4U) /**< \brief (TC0) Write Protect Mode Register */

#endif /* _SAM3S8_TC0_INSTANCE_ */
