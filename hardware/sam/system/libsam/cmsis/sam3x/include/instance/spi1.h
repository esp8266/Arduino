/* $asf_license$ */

#ifndef _SAM3XA_SPI1_INSTANCE_
#define _SAM3XA_SPI1_INSTANCE_

/* ========== Register definition for SPI1 peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_SPI1_CR              (0x4000C000U) /**< \brief (SPI1) Control Register */
#define REG_SPI1_MR              (0x4000C004U) /**< \brief (SPI1) Mode Register */
#define REG_SPI1_RDR             (0x4000C008U) /**< \brief (SPI1) Receive Data Register */
#define REG_SPI1_TDR             (0x4000C00CU) /**< \brief (SPI1) Transmit Data Register */
#define REG_SPI1_SR              (0x4000C010U) /**< \brief (SPI1) Status Register */
#define REG_SPI1_IER             (0x4000C014U) /**< \brief (SPI1) Interrupt Enable Register */
#define REG_SPI1_IDR             (0x4000C018U) /**< \brief (SPI1) Interrupt Disable Register */
#define REG_SPI1_IMR             (0x4000C01CU) /**< \brief (SPI1) Interrupt Mask Register */
#define REG_SPI1_CSR             (0x4000C030U) /**< \brief (SPI1) Chip Select Register */
#define REG_SPI1_WPMR            (0x4000C0E4U) /**< \brief (SPI1) Write Protection Control Register */
#define REG_SPI1_WPSR            (0x4000C0E8U) /**< \brief (SPI1) Write Protection Status Register */
#else
#define REG_SPI1_CR     (*(WoReg*)0x4000C000U) /**< \brief (SPI1) Control Register */
#define REG_SPI1_MR     (*(RwReg*)0x4000C004U) /**< \brief (SPI1) Mode Register */
#define REG_SPI1_RDR    (*(RoReg*)0x4000C008U) /**< \brief (SPI1) Receive Data Register */
#define REG_SPI1_TDR    (*(WoReg*)0x4000C00CU) /**< \brief (SPI1) Transmit Data Register */
#define REG_SPI1_SR     (*(RoReg*)0x4000C010U) /**< \brief (SPI1) Status Register */
#define REG_SPI1_IER    (*(WoReg*)0x4000C014U) /**< \brief (SPI1) Interrupt Enable Register */
#define REG_SPI1_IDR    (*(WoReg*)0x4000C018U) /**< \brief (SPI1) Interrupt Disable Register */
#define REG_SPI1_IMR    (*(RoReg*)0x4000C01CU) /**< \brief (SPI1) Interrupt Mask Register */
#define REG_SPI1_CSR    (*(RwReg*)0x4000C030U) /**< \brief (SPI1) Chip Select Register */
#define REG_SPI1_WPMR   (*(RwReg*)0x4000C0E4U) /**< \brief (SPI1) Write Protection Control Register */
#define REG_SPI1_WPSR   (*(RoReg*)0x4000C0E8U) /**< \brief (SPI1) Write Protection Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_SPI1_INSTANCE_ */
