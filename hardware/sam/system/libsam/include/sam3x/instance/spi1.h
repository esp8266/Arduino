/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_SPI1_INSTANCE_
#define _SAM3XA_SPI1_INSTANCE_

/* ========== Register definition for SPI1 peripheral ========== */
#define REG_SPI1_CR     REG_ACCESS(WoReg, 0x4000C000U) /**< \brief (SPI1) Control Register */
#define REG_SPI1_MR     REG_ACCESS(RwReg, 0x4000C004U) /**< \brief (SPI1) Mode Register */
#define REG_SPI1_RDR    REG_ACCESS(RoReg, 0x4000C008U) /**< \brief (SPI1) Receive Data Register */
#define REG_SPI1_TDR    REG_ACCESS(WoReg, 0x4000C00CU) /**< \brief (SPI1) Transmit Data Register */
#define REG_SPI1_SR     REG_ACCESS(RoReg, 0x4000C010U) /**< \brief (SPI1) Status Register */
#define REG_SPI1_IER    REG_ACCESS(WoReg, 0x4000C014U) /**< \brief (SPI1) Interrupt Enable Register */
#define REG_SPI1_IDR    REG_ACCESS(WoReg, 0x4000C018U) /**< \brief (SPI1) Interrupt Disable Register */
#define REG_SPI1_IMR    REG_ACCESS(RoReg, 0x4000C01CU) /**< \brief (SPI1) Interrupt Mask Register */
#define REG_SPI1_CSR    REG_ACCESS(RwReg, 0x4000C030U) /**< \brief (SPI1) Chip Select Register */
#define REG_SPI1_WPMR   REG_ACCESS(RwReg, 0x4000C0E4U) /**< \brief (SPI1) Write Protection Control Register */
#define REG_SPI1_WPSR   REG_ACCESS(RoReg, 0x4000C0E8U) /**< \brief (SPI1) Write Protection Status Register */

#endif /* _SAM3XA_SPI1_INSTANCE_ */
