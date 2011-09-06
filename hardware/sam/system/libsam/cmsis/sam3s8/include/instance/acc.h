/* $asf_license$ */

#ifndef _SAM3S8_ACC_INSTANCE_
#define _SAM3S8_ACC_INSTANCE_

/* ========== Register definition for ACC peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_ACC_CR            (0x40040000U) /**< \brief (ACC) Control Register */
#define REG_ACC_MR            (0x40040004U) /**< \brief (ACC) Mode Register */
#define REG_ACC_IER           (0x40040024U) /**< \brief (ACC) Interrupt Enable Register */
#define REG_ACC_IDR           (0x40040028U) /**< \brief (ACC) Interrupt Disable Register */
#define REG_ACC_IMR           (0x4004002CU) /**< \brief (ACC) Interrupt Mask Register */
#define REG_ACC_ISR           (0x40040030U) /**< \brief (ACC) Interrupt Status Register */
#define REG_ACC_ACR           (0x40040094U) /**< \brief (ACC) Analog Control Register */
#define REG_ACC_WPMR          (0x400400E4U) /**< \brief (ACC) Write Protect Mode Register */
#define REG_ACC_WPSR          (0x400400E8U) /**< \brief (ACC) Write Protect Status Register */
#else
#define REG_ACC_CR   (*(WoReg*)0x40040000U) /**< \brief (ACC) Control Register */
#define REG_ACC_MR   (*(RwReg*)0x40040004U) /**< \brief (ACC) Mode Register */
#define REG_ACC_IER  (*(WoReg*)0x40040024U) /**< \brief (ACC) Interrupt Enable Register */
#define REG_ACC_IDR  (*(WoReg*)0x40040028U) /**< \brief (ACC) Interrupt Disable Register */
#define REG_ACC_IMR  (*(RoReg*)0x4004002CU) /**< \brief (ACC) Interrupt Mask Register */
#define REG_ACC_ISR  (*(RoReg*)0x40040030U) /**< \brief (ACC) Interrupt Status Register */
#define REG_ACC_ACR  (*(RwReg*)0x40040094U) /**< \brief (ACC) Analog Control Register */
#define REG_ACC_WPMR (*(RwReg*)0x400400E4U) /**< \brief (ACC) Write Protect Mode Register */
#define REG_ACC_WPSR (*(RoReg*)0x400400E8U) /**< \brief (ACC) Write Protect Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3S8_ACC_INSTANCE_ */
