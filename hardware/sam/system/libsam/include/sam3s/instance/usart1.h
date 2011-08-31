/* %ATMEL_LICENCE% */

#ifndef _SAM3S_USART1_INSTANCE_
#define _SAM3S_USART1_INSTANCE_

/* ========== Register definition for USART1 peripheral ========== */
#define REG_USART1_CR   REG_ACCESS(WoReg, 0x40028000U) /**< \brief (USART1) Control Register */
#define REG_USART1_MR   REG_ACCESS(RwReg, 0x40028004U) /**< \brief (USART1) Mode Register */
#define REG_USART1_IER  REG_ACCESS(WoReg, 0x40028008U) /**< \brief (USART1) Interrupt Enable Register */
#define REG_USART1_IDR  REG_ACCESS(WoReg, 0x4002800CU) /**< \brief (USART1) Interrupt Disable Register */
#define REG_USART1_IMR  REG_ACCESS(RoReg, 0x40028010U) /**< \brief (USART1) Interrupt Mask Register */
#define REG_USART1_CSR  REG_ACCESS(RoReg, 0x40028014U) /**< \brief (USART1) Channel Status Register */
#define REG_USART1_RHR  REG_ACCESS(RoReg, 0x40028018U) /**< \brief (USART1) Receiver Holding Register */
#define REG_USART1_THR  REG_ACCESS(WoReg, 0x4002801CU) /**< \brief (USART1) Transmitter Holding Register */
#define REG_USART1_BRGR REG_ACCESS(RwReg, 0x40028020U) /**< \brief (USART1) Baud Rate Generator Register */
#define REG_USART1_RTOR REG_ACCESS(RwReg, 0x40028024U) /**< \brief (USART1) Receiver Time-out Register */
#define REG_USART1_TTGR REG_ACCESS(RwReg, 0x40028028U) /**< \brief (USART1) Transmitter Timeguard Register */
#define REG_USART1_FIDI REG_ACCESS(RwReg, 0x40028040U) /**< \brief (USART1) FI DI Ratio Register */
#define REG_USART1_NER  REG_ACCESS(RoReg, 0x40028044U) /**< \brief (USART1) Number of Errors Register */
#define REG_USART1_IF   REG_ACCESS(RwReg, 0x4002804CU) /**< \brief (USART1) IrDA Filter Register */
#define REG_USART1_MAN  REG_ACCESS(RwReg, 0x40028050U) /**< \brief (USART1) Manchester Encoder Decoder Register */
#define REG_USART1_WPMR REG_ACCESS(RwReg, 0x400280E4U) /**< \brief (USART1) Write Protect Mode Register */
#define REG_USART1_WPSR REG_ACCESS(RoReg, 0x400280E8U) /**< \brief (USART1) Write Protect Status Register */
#define REG_USART1_VERSION REG_ACCESS(RoReg, 0x400280FCU) /**< \brief (USART1) Version Register */
#define REG_USART1_RPR  REG_ACCESS(RwReg, 0x40028100U) /**< \brief (USART1) Receive Pointer Register */
#define REG_USART1_RCR  REG_ACCESS(RwReg, 0x40028104U) /**< \brief (USART1) Receive Counter Register */
#define REG_USART1_TPR  REG_ACCESS(RwReg, 0x40028108U) /**< \brief (USART1) Transmit Pointer Register */
#define REG_USART1_TCR  REG_ACCESS(RwReg, 0x4002810CU) /**< \brief (USART1) Transmit Counter Register */
#define REG_USART1_RNPR REG_ACCESS(RwReg, 0x40028110U) /**< \brief (USART1) Receive Next Pointer Register */
#define REG_USART1_RNCR REG_ACCESS(RwReg, 0x40028114U) /**< \brief (USART1) Receive Next Counter Register */
#define REG_USART1_TNPR REG_ACCESS(RwReg, 0x40028118U) /**< \brief (USART1) Transmit Next Pointer Register */
#define REG_USART1_TNCR REG_ACCESS(RwReg, 0x4002811CU) /**< \brief (USART1) Transmit Next Counter Register */
#define REG_USART1_PTCR REG_ACCESS(WoReg, 0x40028120U) /**< \brief (USART1) Transfer Control Register */
#define REG_USART1_PTSR REG_ACCESS(RoReg, 0x40028124U) /**< \brief (USART1) Transfer Status Register */

#endif /* _SAM3S_USART1_INSTANCE_ */
