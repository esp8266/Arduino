/* $asf_license$ */

#ifndef _SAM3XA_SDRAMC_INSTANCE_
#define _SAM3XA_SDRAMC_INSTANCE_

/* ========== Register definition for SDRAMC peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_SDRAMC_MR            (0x400E0200U) /**< \brief (SDRAMC) SDRAMC Mode Register */
#define REG_SDRAMC_TR            (0x400E0204U) /**< \brief (SDRAMC) SDRAMC Refresh Timer Register */
#define REG_SDRAMC_CR            (0x400E0208U) /**< \brief (SDRAMC) SDRAMC Configuration Register */
#define REG_SDRAMC_LPR           (0x400E0210U) /**< \brief (SDRAMC) SDRAMC Low Power Register */
#define REG_SDRAMC_IER           (0x400E0214U) /**< \brief (SDRAMC) SDRAMC Interrupt Enable Register */
#define REG_SDRAMC_IDR           (0x400E0218U) /**< \brief (SDRAMC) SDRAMC Interrupt Disable Register */
#define REG_SDRAMC_IMR           (0x400E021CU) /**< \brief (SDRAMC) SDRAMC Interrupt Mask Register */
#define REG_SDRAMC_ISR           (0x400E0220U) /**< \brief (SDRAMC) SDRAMC Interrupt Status Register */
#define REG_SDRAMC_MDR           (0x400E0224U) /**< \brief (SDRAMC) SDRAMC Memory Device Register */
#define REG_SDRAMC_CR1           (0x400E0228U) /**< \brief (SDRAMC) SDRAMC Configuration Register 1 */
#define REG_SDRAMC_OCMS          (0x400E022CU) /**< \brief (SDRAMC) SDRAMC OCMS Register 1 */
#else
#define REG_SDRAMC_MR   (*(RwReg*)0x400E0200U) /**< \brief (SDRAMC) SDRAMC Mode Register */
#define REG_SDRAMC_TR   (*(RwReg*)0x400E0204U) /**< \brief (SDRAMC) SDRAMC Refresh Timer Register */
#define REG_SDRAMC_CR   (*(RwReg*)0x400E0208U) /**< \brief (SDRAMC) SDRAMC Configuration Register */
#define REG_SDRAMC_LPR  (*(RwReg*)0x400E0210U) /**< \brief (SDRAMC) SDRAMC Low Power Register */
#define REG_SDRAMC_IER  (*(WoReg*)0x400E0214U) /**< \brief (SDRAMC) SDRAMC Interrupt Enable Register */
#define REG_SDRAMC_IDR  (*(WoReg*)0x400E0218U) /**< \brief (SDRAMC) SDRAMC Interrupt Disable Register */
#define REG_SDRAMC_IMR  (*(RoReg*)0x400E021CU) /**< \brief (SDRAMC) SDRAMC Interrupt Mask Register */
#define REG_SDRAMC_ISR  (*(RoReg*)0x400E0220U) /**< \brief (SDRAMC) SDRAMC Interrupt Status Register */
#define REG_SDRAMC_MDR  (*(RwReg*)0x400E0224U) /**< \brief (SDRAMC) SDRAMC Memory Device Register */
#define REG_SDRAMC_CR1  (*(RwReg*)0x400E0228U) /**< \brief (SDRAMC) SDRAMC Configuration Register 1 */
#define REG_SDRAMC_OCMS (*(RwReg*)0x400E022CU) /**< \brief (SDRAMC) SDRAMC OCMS Register 1 */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_SDRAMC_INSTANCE_ */
