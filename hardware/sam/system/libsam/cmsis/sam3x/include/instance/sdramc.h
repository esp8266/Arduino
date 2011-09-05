/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_SDRAMC_INSTANCE_
#define _SAM3XA_SDRAMC_INSTANCE_

/* ========== Register definition for SDRAMC peripheral ========== */
#define REG_SDRAMC_MR   REG_ACCESS(RwReg, 0x400E0200U) /**< \brief (SDRAMC) SDRAMC Mode Register */
#define REG_SDRAMC_TR   REG_ACCESS(RwReg, 0x400E0204U) /**< \brief (SDRAMC) SDRAMC Refresh Timer Register */
#define REG_SDRAMC_CR   REG_ACCESS(RwReg, 0x400E0208U) /**< \brief (SDRAMC) SDRAMC Configuration Register */
#define REG_SDRAMC_LPR  REG_ACCESS(RwReg, 0x400E0210U) /**< \brief (SDRAMC) SDRAMC Low Power Register */
#define REG_SDRAMC_IER  REG_ACCESS(WoReg, 0x400E0214U) /**< \brief (SDRAMC) SDRAMC Interrupt Enable Register */
#define REG_SDRAMC_IDR  REG_ACCESS(WoReg, 0x400E0218U) /**< \brief (SDRAMC) SDRAMC Interrupt Disable Register */
#define REG_SDRAMC_IMR  REG_ACCESS(RoReg, 0x400E021CU) /**< \brief (SDRAMC) SDRAMC Interrupt Mask Register */
#define REG_SDRAMC_ISR  REG_ACCESS(RoReg, 0x400E0220U) /**< \brief (SDRAMC) SDRAMC Interrupt Status Register */
#define REG_SDRAMC_MDR  REG_ACCESS(RwReg, 0x400E0224U) /**< \brief (SDRAMC) SDRAMC Memory Device Register */
#define REG_SDRAMC_CR1  REG_ACCESS(RwReg, 0x400E0228U) /**< \brief (SDRAMC) SDRAMC Configuration Register 1 */
#define REG_SDRAMC_OCMS REG_ACCESS(RwReg, 0x400E022CU) /**< \brief (SDRAMC) SDRAMC OCMS Register 1 */

#endif /* _SAM3XA_SDRAMC_INSTANCE_ */
