/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_TWI0_INSTANCE_
#define _SAM3S8_TWI0_INSTANCE_

/* ========== Register definition for TWI0 peripheral ========== */
#define REG_TWI0_CR   REG_ACCESS(WoReg, 0x40018000U) /**< \brief (TWI0) Control Register */
#define REG_TWI0_MMR  REG_ACCESS(RwReg, 0x40018004U) /**< \brief (TWI0) Master Mode Register */
#define REG_TWI0_SMR  REG_ACCESS(RwReg, 0x40018008U) /**< \brief (TWI0) Slave Mode Register */
#define REG_TWI0_IADR REG_ACCESS(RwReg, 0x4001800CU) /**< \brief (TWI0) Internal Address Register */
#define REG_TWI0_CWGR REG_ACCESS(RwReg, 0x40018010U) /**< \brief (TWI0) Clock Waveform Generator Register */
#define REG_TWI0_SR   REG_ACCESS(RoReg, 0x40018020U) /**< \brief (TWI0) Status Register */
#define REG_TWI0_IER  REG_ACCESS(WoReg, 0x40018024U) /**< \brief (TWI0) Interrupt Enable Register */
#define REG_TWI0_IDR  REG_ACCESS(WoReg, 0x40018028U) /**< \brief (TWI0) Interrupt Disable Register */
#define REG_TWI0_IMR  REG_ACCESS(RoReg, 0x4001802CU) /**< \brief (TWI0) Interrupt Mask Register */
#define REG_TWI0_RHR  REG_ACCESS(RoReg, 0x40018030U) /**< \brief (TWI0) Receive Holding Register */
#define REG_TWI0_THR  REG_ACCESS(WoReg, 0x40018034U) /**< \brief (TWI0) Transmit Holding Register */
#define REG_TWI0_RPR  REG_ACCESS(RwReg, 0x40018100U) /**< \brief (TWI0) Receive Pointer Register */
#define REG_TWI0_RCR  REG_ACCESS(RwReg, 0x40018104U) /**< \brief (TWI0) Receive Counter Register */
#define REG_TWI0_TPR  REG_ACCESS(RwReg, 0x40018108U) /**< \brief (TWI0) Transmit Pointer Register */
#define REG_TWI0_TCR  REG_ACCESS(RwReg, 0x4001810CU) /**< \brief (TWI0) Transmit Counter Register */
#define REG_TWI0_RNPR REG_ACCESS(RwReg, 0x40018110U) /**< \brief (TWI0) Receive Next Pointer Register */
#define REG_TWI0_RNCR REG_ACCESS(RwReg, 0x40018114U) /**< \brief (TWI0) Receive Next Counter Register */
#define REG_TWI0_TNPR REG_ACCESS(RwReg, 0x40018118U) /**< \brief (TWI0) Transmit Next Pointer Register */
#define REG_TWI0_TNCR REG_ACCESS(RwReg, 0x4001811CU) /**< \brief (TWI0) Transmit Next Counter Register */
#define REG_TWI0_PTCR REG_ACCESS(WoReg, 0x40018120U) /**< \brief (TWI0) Transfer Control Register */
#define REG_TWI0_PTSR REG_ACCESS(RoReg, 0x40018124U) /**< \brief (TWI0) Transfer Status Register */

#endif /* _SAM3S8_TWI0_INSTANCE_ */
