/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_DACC_INSTANCE_
#define _SAM3S8_DACC_INSTANCE_

/* ========== Register definition for DACC peripheral ========== */
#define REG_DACC_CR   REG_ACCESS(WoReg, 0x4003C000U) /**< \brief (DACC) Control Register */
#define REG_DACC_MR   REG_ACCESS(RwReg, 0x4003C004U) /**< \brief (DACC) Mode Register */
#define REG_DACC_CHER REG_ACCESS(WoReg, 0x4003C010U) /**< \brief (DACC) Channel Enable Register */
#define REG_DACC_CHDR REG_ACCESS(WoReg, 0x4003C014U) /**< \brief (DACC) Channel Disable Register */
#define REG_DACC_CHSR REG_ACCESS(RoReg, 0x4003C018U) /**< \brief (DACC) Channel Status Register */
#define REG_DACC_CDR  REG_ACCESS(WoReg, 0x4003C020U) /**< \brief (DACC) Conversion Data Register */
#define REG_DACC_IER  REG_ACCESS(WoReg, 0x4003C024U) /**< \brief (DACC) Interrupt Enable Register */
#define REG_DACC_IDR  REG_ACCESS(WoReg, 0x4003C028U) /**< \brief (DACC) Interrupt Disable Register */
#define REG_DACC_IMR  REG_ACCESS(RoReg, 0x4003C02CU) /**< \brief (DACC) Interrupt Mask Register */
#define REG_DACC_ISR  REG_ACCESS(RoReg, 0x4003C030U) /**< \brief (DACC) Interrupt Status Register */
#define REG_DACC_ACR  REG_ACCESS(RwReg, 0x4003C094U) /**< \brief (DACC) Analog Current Register */
#define REG_DACC_WPMR REG_ACCESS(RwReg, 0x4003C0E4U) /**< \brief (DACC) Write Protect Mode register */
#define REG_DACC_WPSR REG_ACCESS(RoReg, 0x4003C0E8U) /**< \brief (DACC) Write Protect Status register */
#define REG_DACC_RPR  REG_ACCESS(RwReg, 0x4003C100U) /**< \brief (DACC) Receive Pointer Register */
#define REG_DACC_RCR  REG_ACCESS(RwReg, 0x4003C104U) /**< \brief (DACC) Receive Counter Register */
#define REG_DACC_TPR  REG_ACCESS(RwReg, 0x4003C108U) /**< \brief (DACC) Transmit Pointer Register */
#define REG_DACC_TCR  REG_ACCESS(RwReg, 0x4003C10CU) /**< \brief (DACC) Transmit Counter Register */
#define REG_DACC_RNPR REG_ACCESS(RwReg, 0x4003C110U) /**< \brief (DACC) Receive Next Pointer Register */
#define REG_DACC_RNCR REG_ACCESS(RwReg, 0x4003C114U) /**< \brief (DACC) Receive Next Counter Register */
#define REG_DACC_TNPR REG_ACCESS(RwReg, 0x4003C118U) /**< \brief (DACC) Transmit Next Pointer Register */
#define REG_DACC_TNCR REG_ACCESS(RwReg, 0x4003C11CU) /**< \brief (DACC) Transmit Next Counter Register */
#define REG_DACC_PTCR REG_ACCESS(WoReg, 0x4003C120U) /**< \brief (DACC) Transfer Control Register */
#define REG_DACC_PTSR REG_ACCESS(RoReg, 0x4003C124U) /**< \brief (DACC) Transfer Status Register */

#endif /* _SAM3S8_DACC_INSTANCE_ */
