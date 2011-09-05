/* %ATMEL_LICENCE% */

#ifndef _SAM3N_TWI1_INSTANCE_
#define _SAM3N_TWI1_INSTANCE_

/* ========== Register definition for TWI1 peripheral ========== */
#define REG_TWI1_CR   REG_ACCESS(WoReg, 0x4001C000U) /**< \brief (TWI1) Control Register */
#define REG_TWI1_MMR  REG_ACCESS(RwReg, 0x4001C004U) /**< \brief (TWI1) Master Mode Register */
#define REG_TWI1_SMR  REG_ACCESS(RwReg, 0x4001C008U) /**< \brief (TWI1) Slave Mode Register */
#define REG_TWI1_IADR REG_ACCESS(RwReg, 0x4001C00CU) /**< \brief (TWI1) Internal Address Register */
#define REG_TWI1_CWGR REG_ACCESS(RwReg, 0x4001C010U) /**< \brief (TWI1) Clock Waveform Generator Register */
#define REG_TWI1_SR   REG_ACCESS(RoReg, 0x4001C020U) /**< \brief (TWI1) Status Register */
#define REG_TWI1_IER  REG_ACCESS(WoReg, 0x4001C024U) /**< \brief (TWI1) Interrupt Enable Register */
#define REG_TWI1_IDR  REG_ACCESS(WoReg, 0x4001C028U) /**< \brief (TWI1) Interrupt Disable Register */
#define REG_TWI1_IMR  REG_ACCESS(RoReg, 0x4001C02CU) /**< \brief (TWI1) Interrupt Mask Register */
#define REG_TWI1_RHR  REG_ACCESS(RoReg, 0x4001C030U) /**< \brief (TWI1) Receive Holding Register */
#define REG_TWI1_THR  REG_ACCESS(WoReg, 0x4001C034U) /**< \brief (TWI1) Transmit Holding Register */

#endif /* _SAM3N_TWI1_INSTANCE_ */
