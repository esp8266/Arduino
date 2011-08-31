/* %ATMEL_LICENCE% */

#ifndef _SAM3S_TWI1_INSTANCE_
#define _SAM3S_TWI1_INSTANCE_

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
#define REG_TWI1_RPR  REG_ACCESS(RwReg, 0x4001C100U) /**< \brief (TWI1) Receive Pointer Register */
#define REG_TWI1_RCR  REG_ACCESS(RwReg, 0x4001C104U) /**< \brief (TWI1) Receive Counter Register */
#define REG_TWI1_TPR  REG_ACCESS(RwReg, 0x4001C108U) /**< \brief (TWI1) Transmit Pointer Register */
#define REG_TWI1_TCR  REG_ACCESS(RwReg, 0x4001C10CU) /**< \brief (TWI1) Transmit Counter Register */
#define REG_TWI1_RNPR REG_ACCESS(RwReg, 0x4001C110U) /**< \brief (TWI1) Receive Next Pointer Register */
#define REG_TWI1_RNCR REG_ACCESS(RwReg, 0x4001C114U) /**< \brief (TWI1) Receive Next Counter Register */
#define REG_TWI1_TNPR REG_ACCESS(RwReg, 0x4001C118U) /**< \brief (TWI1) Transmit Next Pointer Register */
#define REG_TWI1_TNCR REG_ACCESS(RwReg, 0x4001C11CU) /**< \brief (TWI1) Transmit Next Counter Register */
#define REG_TWI1_PTCR REG_ACCESS(WoReg, 0x4001C120U) /**< \brief (TWI1) Transfer Control Register */
#define REG_TWI1_PTSR REG_ACCESS(RoReg, 0x4001C124U) /**< \brief (TWI1) Transfer Status Register */

#endif /* _SAM3S_TWI1_INSTANCE_ */
