/* $asf_license$ */

#ifndef _SAM3XA_SPI0_INSTANCE_
#define _SAM3XA_SPI0_INSTANCE_

/* ========== Register definition for SPI0 peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_SPI0_CR              (0x40008000U) /**< \brief (SPI0) Control Register */
#define REG_SPI0_MR              (0x40008004U) /**< \brief (SPI0) Mode Register */
#define REG_SPI0_RDR             (0x40008008U) /**< \brief (SPI0) Receive Data Register */
#define REG_SPI0_TDR             (0x4000800CU) /**< \brief (SPI0) Transmit Data Register */
#define REG_SPI0_SR              (0x40008010U) /**< \brief (SPI0) Status Register */
#define REG_SPI0_IER             (0x40008014U) /**< \brief (SPI0) Interrupt Enable Register */
#define REG_SPI0_IDR             (0x40008018U) /**< \brief (SPI0) Interrupt Disable Register */
#define REG_SPI0_IMR             (0x4000801CU) /**< \brief (SPI0) Interrupt Mask Register */
#define REG_SPI0_CSR             (0x40008030U) /**< \brief (SPI0) Chip Select Register */
#define REG_SPI0_WPMR            (0x400080E4U) /**< \brief (SPI0) Write Protection Control Register */
#define REG_SPI0_WPSR            (0x400080E8U) /**< \brief (SPI0) Write Protection Status Register */
#else
#define REG_SPI0_CR     (*(WoReg*)0x40008000U) /**< \brief (SPI0) Control Register */
#define REG_SPI0_MR     (*(RwReg*)0x40008004U) /**< \brief (SPI0) Mode Register */
#define REG_SPI0_RDR    (*(RoReg*)0x40008008U) /**< \brief (SPI0) Receive Data Register */
#define REG_SPI0_TDR    (*(WoReg*)0x4000800CU) /**< \brief (SPI0) Transmit Data Register */
#define REG_SPI0_SR     (*(RoReg*)0x40008010U) /**< \brief (SPI0) Status Register */
#define REG_SPI0_IER    (*(WoReg*)0x40008014U) /**< \brief (SPI0) Interrupt Enable Register */
#define REG_SPI0_IDR    (*(WoReg*)0x40008018U) /**< \brief (SPI0) Interrupt Disable Register */
#define REG_SPI0_IMR    (*(RoReg*)0x4000801CU) /**< \brief (SPI0) Interrupt Mask Register */
#define REG_SPI0_CSR    (*(RwReg*)0x40008030U) /**< \brief (SPI0) Chip Select Register */
#define REG_SPI0_WPMR   (*(RwReg*)0x400080E4U) /**< \brief (SPI0) Write Protection Control Register */
#define REG_SPI0_WPSR   (*(RoReg*)0x400080E8U) /**< \brief (SPI0) Write Protection Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_SPI0_INSTANCE_ */
