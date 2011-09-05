/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_SPI0_INSTANCE_
#define _SAM3XA_SPI0_INSTANCE_

/* ========== Register definition for SPI0 peripheral ========== */
#define REG_SPI0_CR     REG_ACCESS(WoReg, 0x40008000U) /**< \brief (SPI0) Control Register */
#define REG_SPI0_MR     REG_ACCESS(RwReg, 0x40008004U) /**< \brief (SPI0) Mode Register */
#define REG_SPI0_RDR    REG_ACCESS(RoReg, 0x40008008U) /**< \brief (SPI0) Receive Data Register */
#define REG_SPI0_TDR    REG_ACCESS(WoReg, 0x4000800CU) /**< \brief (SPI0) Transmit Data Register */
#define REG_SPI0_SR     REG_ACCESS(RoReg, 0x40008010U) /**< \brief (SPI0) Status Register */
#define REG_SPI0_IER    REG_ACCESS(WoReg, 0x40008014U) /**< \brief (SPI0) Interrupt Enable Register */
#define REG_SPI0_IDR    REG_ACCESS(WoReg, 0x40008018U) /**< \brief (SPI0) Interrupt Disable Register */
#define REG_SPI0_IMR    REG_ACCESS(RoReg, 0x4000801CU) /**< \brief (SPI0) Interrupt Mask Register */
#define REG_SPI0_CSR    REG_ACCESS(RwReg, 0x40008030U) /**< \brief (SPI0) Chip Select Register */
#define REG_SPI0_WPMR   REG_ACCESS(RwReg, 0x400080E4U) /**< \brief (SPI0) Write Protection Control Register */
#define REG_SPI0_WPSR   REG_ACCESS(RoReg, 0x400080E8U) /**< \brief (SPI0) Write Protection Status Register */

#endif /* _SAM3XA_SPI0_INSTANCE_ */
