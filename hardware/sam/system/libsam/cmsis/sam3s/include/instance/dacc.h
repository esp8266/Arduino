/* $asf_license$ */

#ifndef _SAM3S_DACC_INSTANCE_
#define _SAM3S_DACC_INSTANCE_

/* ========== Register definition for DACC peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_DACC_CR            (0x4003C000U) /**< \brief (DACC) Control Register */
#define REG_DACC_MR            (0x4003C004U) /**< \brief (DACC) Mode Register */
#define REG_DACC_CHER          (0x4003C010U) /**< \brief (DACC) Channel Enable Register */
#define REG_DACC_CHDR          (0x4003C014U) /**< \brief (DACC) Channel Disable Register */
#define REG_DACC_CHSR          (0x4003C018U) /**< \brief (DACC) Channel Status Register */
#define REG_DACC_CDR           (0x4003C020U) /**< \brief (DACC) Conversion Data Register */
#define REG_DACC_IER           (0x4003C024U) /**< \brief (DACC) Interrupt Enable Register */
#define REG_DACC_IDR           (0x4003C028U) /**< \brief (DACC) Interrupt Disable Register */
#define REG_DACC_IMR           (0x4003C02CU) /**< \brief (DACC) Interrupt Mask Register */
#define REG_DACC_ISR           (0x4003C030U) /**< \brief (DACC) Interrupt Status Register */
#define REG_DACC_ACR           (0x4003C094U) /**< \brief (DACC) Analog Current Register */
#define REG_DACC_WPMR          (0x4003C0E4U) /**< \brief (DACC) Write Protect Mode register */
#define REG_DACC_WPSR          (0x4003C0E8U) /**< \brief (DACC) Write Protect Status register */
#define REG_DACC_RPR           (0x4003C100U) /**< \brief (DACC) Receive Pointer Register */
#define REG_DACC_RCR           (0x4003C104U) /**< \brief (DACC) Receive Counter Register */
#define REG_DACC_TPR           (0x4003C108U) /**< \brief (DACC) Transmit Pointer Register */
#define REG_DACC_TCR           (0x4003C10CU) /**< \brief (DACC) Transmit Counter Register */
#define REG_DACC_RNPR          (0x4003C110U) /**< \brief (DACC) Receive Next Pointer Register */
#define REG_DACC_RNCR          (0x4003C114U) /**< \brief (DACC) Receive Next Counter Register */
#define REG_DACC_TNPR          (0x4003C118U) /**< \brief (DACC) Transmit Next Pointer Register */
#define REG_DACC_TNCR          (0x4003C11CU) /**< \brief (DACC) Transmit Next Counter Register */
#define REG_DACC_PTCR          (0x4003C120U) /**< \brief (DACC) Transfer Control Register */
#define REG_DACC_PTSR          (0x4003C124U) /**< \brief (DACC) Transfer Status Register */
#else
#define REG_DACC_CR   (*(WoReg*)0x4003C000U) /**< \brief (DACC) Control Register */
#define REG_DACC_MR   (*(RwReg*)0x4003C004U) /**< \brief (DACC) Mode Register */
#define REG_DACC_CHER (*(WoReg*)0x4003C010U) /**< \brief (DACC) Channel Enable Register */
#define REG_DACC_CHDR (*(WoReg*)0x4003C014U) /**< \brief (DACC) Channel Disable Register */
#define REG_DACC_CHSR (*(RoReg*)0x4003C018U) /**< \brief (DACC) Channel Status Register */
#define REG_DACC_CDR  (*(WoReg*)0x4003C020U) /**< \brief (DACC) Conversion Data Register */
#define REG_DACC_IER  (*(WoReg*)0x4003C024U) /**< \brief (DACC) Interrupt Enable Register */
#define REG_DACC_IDR  (*(WoReg*)0x4003C028U) /**< \brief (DACC) Interrupt Disable Register */
#define REG_DACC_IMR  (*(RoReg*)0x4003C02CU) /**< \brief (DACC) Interrupt Mask Register */
#define REG_DACC_ISR  (*(RoReg*)0x4003C030U) /**< \brief (DACC) Interrupt Status Register */
#define REG_DACC_ACR  (*(RwReg*)0x4003C094U) /**< \brief (DACC) Analog Current Register */
#define REG_DACC_WPMR (*(RwReg*)0x4003C0E4U) /**< \brief (DACC) Write Protect Mode register */
#define REG_DACC_WPSR (*(RoReg*)0x4003C0E8U) /**< \brief (DACC) Write Protect Status register */
#define REG_DACC_RPR  (*(RwReg*)0x4003C100U) /**< \brief (DACC) Receive Pointer Register */
#define REG_DACC_RCR  (*(RwReg*)0x4003C104U) /**< \brief (DACC) Receive Counter Register */
#define REG_DACC_TPR  (*(RwReg*)0x4003C108U) /**< \brief (DACC) Transmit Pointer Register */
#define REG_DACC_TCR  (*(RwReg*)0x4003C10CU) /**< \brief (DACC) Transmit Counter Register */
#define REG_DACC_RNPR (*(RwReg*)0x4003C110U) /**< \brief (DACC) Receive Next Pointer Register */
#define REG_DACC_RNCR (*(RwReg*)0x4003C114U) /**< \brief (DACC) Receive Next Counter Register */
#define REG_DACC_TNPR (*(RwReg*)0x4003C118U) /**< \brief (DACC) Transmit Next Pointer Register */
#define REG_DACC_TNCR (*(RwReg*)0x4003C11CU) /**< \brief (DACC) Transmit Next Counter Register */
#define REG_DACC_PTCR (*(WoReg*)0x4003C120U) /**< \brief (DACC) Transfer Control Register */
#define REG_DACC_PTSR (*(RoReg*)0x4003C124U) /**< \brief (DACC) Transfer Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3S_DACC_INSTANCE_ */
