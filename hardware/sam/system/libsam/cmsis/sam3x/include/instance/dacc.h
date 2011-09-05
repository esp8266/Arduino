/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_DACC_INSTANCE_
#define _SAM3XA_DACC_INSTANCE_

/* ========== Register definition for DACC peripheral ========== */
#define REG_DACC_CR   REG_ACCESS(WoReg, 0x400C8000U) /**< \brief (DACC) Control Register */
#define REG_DACC_MR   REG_ACCESS(RwReg, 0x400C8004U) /**< \brief (DACC) Mode Register */
#define REG_DACC_CHER REG_ACCESS(WoReg, 0x400C8010U) /**< \brief (DACC) Channel Enable Register */
#define REG_DACC_CHDR REG_ACCESS(WoReg, 0x400C8014U) /**< \brief (DACC) Channel Disable Register */
#define REG_DACC_CHSR REG_ACCESS(RoReg, 0x400C8018U) /**< \brief (DACC) Channel Status Register */
#define REG_DACC_CDR  REG_ACCESS(WoReg, 0x400C8020U) /**< \brief (DACC) Conversion Data Register */
#define REG_DACC_IER  REG_ACCESS(WoReg, 0x400C8024U) /**< \brief (DACC) Interrupt Enable Register */
#define REG_DACC_IDR  REG_ACCESS(WoReg, 0x400C8028U) /**< \brief (DACC) Interrupt Disable Register */
#define REG_DACC_IMR  REG_ACCESS(RoReg, 0x400C802CU) /**< \brief (DACC) Interrupt Mask Register */
#define REG_DACC_ISR  REG_ACCESS(RoReg, 0x400C8030U) /**< \brief (DACC) Interrupt Status Register */
#define REG_DACC_ACR  REG_ACCESS(RwReg, 0x400C8094U) /**< \brief (DACC) Analog Current Register */
#define REG_DACC_WPMR REG_ACCESS(RwReg, 0x400C80E4U) /**< \brief (DACC) Write Protect Mode register */
#define REG_DACC_WPSR REG_ACCESS(RoReg, 0x400C80E8U) /**< \brief (DACC) Write Protect Status register */
#define REG_DACC_RPR  REG_ACCESS(RwReg, 0x400C8100U) /**< \brief (DACC) Receive Pointer Register */
#define REG_DACC_RCR  REG_ACCESS(RwReg, 0x400C8104U) /**< \brief (DACC) Receive Counter Register */
#define REG_DACC_TPR  REG_ACCESS(RwReg, 0x400C8108U) /**< \brief (DACC) Transmit Pointer Register */
#define REG_DACC_TCR  REG_ACCESS(RwReg, 0x400C810CU) /**< \brief (DACC) Transmit Counter Register */
#define REG_DACC_RNPR REG_ACCESS(RwReg, 0x400C8110U) /**< \brief (DACC) Receive Next Pointer Register */
#define REG_DACC_RNCR REG_ACCESS(RwReg, 0x400C8114U) /**< \brief (DACC) Receive Next Counter Register */
#define REG_DACC_TNPR REG_ACCESS(RwReg, 0x400C8118U) /**< \brief (DACC) Transmit Next Pointer Register */
#define REG_DACC_TNCR REG_ACCESS(RwReg, 0x400C811CU) /**< \brief (DACC) Transmit Next Counter Register */
#define REG_DACC_PTCR REG_ACCESS(WoReg, 0x400C8120U) /**< \brief (DACC) Transfer Control Register */
#define REG_DACC_PTSR REG_ACCESS(RoReg, 0x400C8124U) /**< \brief (DACC) Transfer Status Register */

#endif /* _SAM3XA_DACC_INSTANCE_ */
