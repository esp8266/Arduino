/* %ATMEL_LICENCE% */

#ifndef _SAM3U_TWI0_INSTANCE_
#define _SAM3U_TWI0_INSTANCE_

/* ========== Register definition for TWI0 peripheral ========== */
#define REG_TWI0_CR   REG_ACCESS(WoReg, 0x40084000U) /**< \brief (TWI0) Control Register */
#define REG_TWI0_MMR  REG_ACCESS(RwReg, 0x40084004U) /**< \brief (TWI0) Master Mode Register */
#define REG_TWI0_SMR  REG_ACCESS(RwReg, 0x40084008U) /**< \brief (TWI0) Slave Mode Register */
#define REG_TWI0_IADR REG_ACCESS(RwReg, 0x4008400CU) /**< \brief (TWI0) Internal Address Register */
#define REG_TWI0_CWGR REG_ACCESS(RwReg, 0x40084010U) /**< \brief (TWI0) Clock Waveform Generator Register */
#define REG_TWI0_SR   REG_ACCESS(RoReg, 0x40084020U) /**< \brief (TWI0) Status Register */
#define REG_TWI0_IER  REG_ACCESS(WoReg, 0x40084024U) /**< \brief (TWI0) Interrupt Enable Register */
#define REG_TWI0_IDR  REG_ACCESS(WoReg, 0x40084028U) /**< \brief (TWI0) Interrupt Disable Register */
#define REG_TWI0_IMR  REG_ACCESS(RoReg, 0x4008402CU) /**< \brief (TWI0) Interrupt Mask Register */
#define REG_TWI0_RHR  REG_ACCESS(RoReg, 0x40084030U) /**< \brief (TWI0) Receive Holding Register */
#define REG_TWI0_THR  REG_ACCESS(WoReg, 0x40084034U) /**< \brief (TWI0) Transmit Holding Register */
#define REG_TWI0_RPR  REG_ACCESS(RwReg, 0x40084100U) /**< \brief (TWI0) Receive Pointer Register */
#define REG_TWI0_RCR  REG_ACCESS(RwReg, 0x40084104U) /**< \brief (TWI0) Receive Counter Register */
#define REG_TWI0_TPR  REG_ACCESS(RwReg, 0x40084108U) /**< \brief (TWI0) Transmit Pointer Register */
#define REG_TWI0_TCR  REG_ACCESS(RwReg, 0x4008410CU) /**< \brief (TWI0) Transmit Counter Register */
#define REG_TWI0_RNPR REG_ACCESS(RwReg, 0x40084110U) /**< \brief (TWI0) Receive Next Pointer Register */
#define REG_TWI0_RNCR REG_ACCESS(RwReg, 0x40084114U) /**< \brief (TWI0) Receive Next Counter Register */
#define REG_TWI0_TNPR REG_ACCESS(RwReg, 0x40084118U) /**< \brief (TWI0) Transmit Next Pointer Register */
#define REG_TWI0_TNCR REG_ACCESS(RwReg, 0x4008411CU) /**< \brief (TWI0) Transmit Next Counter Register */
#define REG_TWI0_PTCR REG_ACCESS(WoReg, 0x40084120U) /**< \brief (TWI0) Transfer Control Register */
#define REG_TWI0_PTSR REG_ACCESS(RoReg, 0x40084124U) /**< \brief (TWI0) Transfer Status Register */

#endif /* _SAM3U_TWI0_INSTANCE_ */
