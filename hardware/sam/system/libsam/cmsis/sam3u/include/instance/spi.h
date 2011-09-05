/* %ATMEL_LICENCE% */

#ifndef _SAM3U_SPI_INSTANCE_
#define _SAM3U_SPI_INSTANCE_

/* ========== Register definition for SPI peripheral ========== */
#define REG_SPI_CR     REG_ACCESS(WoReg, 0x40008000U) /**< \brief (SPI) Control Register */
#define REG_SPI_MR     REG_ACCESS(RwReg, 0x40008004U) /**< \brief (SPI) Mode Register */
#define REG_SPI_RDR    REG_ACCESS(RoReg, 0x40008008U) /**< \brief (SPI) Receive Data Register */
#define REG_SPI_TDR    REG_ACCESS(WoReg, 0x4000800CU) /**< \brief (SPI) Transmit Data Register */
#define REG_SPI_SR     REG_ACCESS(RoReg, 0x40008010U) /**< \brief (SPI) Status Register */
#define REG_SPI_IER    REG_ACCESS(WoReg, 0x40008014U) /**< \brief (SPI) Interrupt Enable Register */
#define REG_SPI_IDR    REG_ACCESS(WoReg, 0x40008018U) /**< \brief (SPI) Interrupt Disable Register */
#define REG_SPI_IMR    REG_ACCESS(RoReg, 0x4000801CU) /**< \brief (SPI) Interrupt Mask Register */
#define REG_SPI_CSR    REG_ACCESS(RwReg, 0x40008030U) /**< \brief (SPI) Chip Select Register */
#define REG_SPI_WPMR   REG_ACCESS(RwReg, 0x400080E4U) /**< \brief (SPI) Write Protection Control Register */
#define REG_SPI_WPSR   REG_ACCESS(RoReg, 0x400080E8U) /**< \brief (SPI) Write Protection Status Register */

#endif /* _SAM3U_SPI_INSTANCE_ */
