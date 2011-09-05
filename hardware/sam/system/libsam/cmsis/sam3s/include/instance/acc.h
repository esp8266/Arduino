/* %ATMEL_LICENCE% */

#ifndef _SAM3S_ACC_INSTANCE_
#define _SAM3S_ACC_INSTANCE_

/* ========== Register definition for ACC peripheral ========== */
#define REG_ACC_CR   REG_ACCESS(WoReg, 0x40040000U) /**< \brief (ACC) Control Register */
#define REG_ACC_MR   REG_ACCESS(RwReg, 0x40040004U) /**< \brief (ACC) Mode Register */
#define REG_ACC_IER  REG_ACCESS(WoReg, 0x40040024U) /**< \brief (ACC) Interrupt Enable Register */
#define REG_ACC_IDR  REG_ACCESS(WoReg, 0x40040028U) /**< \brief (ACC) Interrupt Disable Register */
#define REG_ACC_IMR  REG_ACCESS(RoReg, 0x4004002CU) /**< \brief (ACC) Interrupt Mask Register */
#define REG_ACC_ISR  REG_ACCESS(RoReg, 0x40040030U) /**< \brief (ACC) Interrupt Status Register */
#define REG_ACC_ACR  REG_ACCESS(RwReg, 0x40040094U) /**< \brief (ACC) Analog Control Register */
#define REG_ACC_WPMR REG_ACCESS(RwReg, 0x400400E4U) /**< \brief (ACC) Write Protect Mode Register */
#define REG_ACC_WPSR REG_ACCESS(RoReg, 0x400400E8U) /**< \brief (ACC) Write Protect Status Register */

#endif /* _SAM3S_ACC_INSTANCE_ */
