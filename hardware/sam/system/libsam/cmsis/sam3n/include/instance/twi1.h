/* $asf_license$ */

#ifndef _SAM3N_TWI1_INSTANCE_
#define _SAM3N_TWI1_INSTANCE_

/* ========== Register definition for TWI1 peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_TWI1_CR            (0x4001C000U) /**< \brief (TWI1) Control Register */
#define REG_TWI1_MMR           (0x4001C004U) /**< \brief (TWI1) Master Mode Register */
#define REG_TWI1_SMR           (0x4001C008U) /**< \brief (TWI1) Slave Mode Register */
#define REG_TWI1_IADR          (0x4001C00CU) /**< \brief (TWI1) Internal Address Register */
#define REG_TWI1_CWGR          (0x4001C010U) /**< \brief (TWI1) Clock Waveform Generator Register */
#define REG_TWI1_SR            (0x4001C020U) /**< \brief (TWI1) Status Register */
#define REG_TWI1_IER           (0x4001C024U) /**< \brief (TWI1) Interrupt Enable Register */
#define REG_TWI1_IDR           (0x4001C028U) /**< \brief (TWI1) Interrupt Disable Register */
#define REG_TWI1_IMR           (0x4001C02CU) /**< \brief (TWI1) Interrupt Mask Register */
#define REG_TWI1_RHR           (0x4001C030U) /**< \brief (TWI1) Receive Holding Register */
#define REG_TWI1_THR           (0x4001C034U) /**< \brief (TWI1) Transmit Holding Register */
#else
#define REG_TWI1_CR   (*(WoReg*)0x4001C000U) /**< \brief (TWI1) Control Register */
#define REG_TWI1_MMR  (*(RwReg*)0x4001C004U) /**< \brief (TWI1) Master Mode Register */
#define REG_TWI1_SMR  (*(RwReg*)0x4001C008U) /**< \brief (TWI1) Slave Mode Register */
#define REG_TWI1_IADR (*(RwReg*)0x4001C00CU) /**< \brief (TWI1) Internal Address Register */
#define REG_TWI1_CWGR (*(RwReg*)0x4001C010U) /**< \brief (TWI1) Clock Waveform Generator Register */
#define REG_TWI1_SR   (*(RoReg*)0x4001C020U) /**< \brief (TWI1) Status Register */
#define REG_TWI1_IER  (*(WoReg*)0x4001C024U) /**< \brief (TWI1) Interrupt Enable Register */
#define REG_TWI1_IDR  (*(WoReg*)0x4001C028U) /**< \brief (TWI1) Interrupt Disable Register */
#define REG_TWI1_IMR  (*(RoReg*)0x4001C02CU) /**< \brief (TWI1) Interrupt Mask Register */
#define REG_TWI1_RHR  (*(RoReg*)0x4001C030U) /**< \brief (TWI1) Receive Holding Register */
#define REG_TWI1_THR  (*(WoReg*)0x4001C034U) /**< \brief (TWI1) Transmit Holding Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3N_TWI1_INSTANCE_ */
