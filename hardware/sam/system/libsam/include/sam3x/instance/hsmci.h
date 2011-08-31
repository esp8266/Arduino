/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_HSMCI_INSTANCE_
#define _SAM3XA_HSMCI_INSTANCE_

/* ========== Register definition for HSMCI peripheral ========== */
#define REG_HSMCI_CR        REG_ACCESS(WoReg, 0x40000000U) /**< \brief (HSMCI) Control Register */
#define REG_HSMCI_MR        REG_ACCESS(RwReg, 0x40000004U) /**< \brief (HSMCI) Mode Register */
#define REG_HSMCI_DTOR      REG_ACCESS(RwReg, 0x40000008U) /**< \brief (HSMCI) Data Timeout Register */
#define REG_HSMCI_SDCR      REG_ACCESS(RwReg, 0x4000000CU) /**< \brief (HSMCI) SD/SDIO Card Register */
#define REG_HSMCI_ARGR      REG_ACCESS(RwReg, 0x40000010U) /**< \brief (HSMCI) Argument Register */
#define REG_HSMCI_CMDR      REG_ACCESS(WoReg, 0x40000014U) /**< \brief (HSMCI) Command Register */
#define REG_HSMCI_BLKR      REG_ACCESS(RwReg, 0x40000018U) /**< \brief (HSMCI) Block Register */
#define REG_HSMCI_CSTOR     REG_ACCESS(RwReg, 0x4000001CU) /**< \brief (HSMCI) Completion Signal Timeout Register */
#define REG_HSMCI_RSPR      REG_ACCESS(RoReg, 0x40000020U) /**< \brief (HSMCI) Response Register */
#define REG_HSMCI_RDR       REG_ACCESS(RoReg, 0x40000030U) /**< \brief (HSMCI) Receive Data Register */
#define REG_HSMCI_TDR       REG_ACCESS(WoReg, 0x40000034U) /**< \brief (HSMCI) Transmit Data Register */
#define REG_HSMCI_SR        REG_ACCESS(RoReg, 0x40000040U) /**< \brief (HSMCI) Status Register */
#define REG_HSMCI_IER       REG_ACCESS(WoReg, 0x40000044U) /**< \brief (HSMCI) Interrupt Enable Register */
#define REG_HSMCI_IDR       REG_ACCESS(WoReg, 0x40000048U) /**< \brief (HSMCI) Interrupt Disable Register */
#define REG_HSMCI_IMR       REG_ACCESS(RoReg, 0x4000004CU) /**< \brief (HSMCI) Interrupt Mask Register */
#define REG_HSMCI_DMA       REG_ACCESS(RwReg, 0x40000050U) /**< \brief (HSMCI) DMA Configuration Register */
#define REG_HSMCI_CFG       REG_ACCESS(RwReg, 0x40000054U) /**< \brief (HSMCI) Configuration Register */
#define REG_HSMCI_WPMR      REG_ACCESS(RwReg, 0x400000E4U) /**< \brief (HSMCI) Write Protection Mode Register */
#define REG_HSMCI_WPSR      REG_ACCESS(RoReg, 0x400000E8U) /**< \brief (HSMCI) Write Protection Status Register */
#define REG_HSMCI_FIFO      REG_ACCESS(RwReg, 0x40000200U) /**< \brief (HSMCI) FIFO Memory Aperture0 */

#endif /* _SAM3XA_HSMCI_INSTANCE_ */
