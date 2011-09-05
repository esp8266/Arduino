/* %ATMEL_LICENCE% */

#ifndef _SAM3S_USART0_INSTANCE_
#define _SAM3S_USART0_INSTANCE_

/* ========== Register definition for USART0 peripheral ========== */
#define REG_USART0_CR   REG_ACCESS(WoReg, 0x40024000U) /**< \brief (USART0) Control Register */
#define REG_USART0_MR   REG_ACCESS(RwReg, 0x40024004U) /**< \brief (USART0) Mode Register */
#define REG_USART0_IER  REG_ACCESS(WoReg, 0x40024008U) /**< \brief (USART0) Interrupt Enable Register */
#define REG_USART0_IDR  REG_ACCESS(WoReg, 0x4002400CU) /**< \brief (USART0) Interrupt Disable Register */
#define REG_USART0_IMR  REG_ACCESS(RoReg, 0x40024010U) /**< \brief (USART0) Interrupt Mask Register */
#define REG_USART0_CSR  REG_ACCESS(RoReg, 0x40024014U) /**< \brief (USART0) Channel Status Register */
#define REG_USART0_RHR  REG_ACCESS(RoReg, 0x40024018U) /**< \brief (USART0) Receiver Holding Register */
#define REG_USART0_THR  REG_ACCESS(WoReg, 0x4002401CU) /**< \brief (USART0) Transmitter Holding Register */
#define REG_USART0_BRGR REG_ACCESS(RwReg, 0x40024020U) /**< \brief (USART0) Baud Rate Generator Register */
#define REG_USART0_RTOR REG_ACCESS(RwReg, 0x40024024U) /**< \brief (USART0) Receiver Time-out Register */
#define REG_USART0_TTGR REG_ACCESS(RwReg, 0x40024028U) /**< \brief (USART0) Transmitter Timeguard Register */
#define REG_USART0_FIDI REG_ACCESS(RwReg, 0x40024040U) /**< \brief (USART0) FI DI Ratio Register */
#define REG_USART0_NER  REG_ACCESS(RoReg, 0x40024044U) /**< \brief (USART0) Number of Errors Register */
#define REG_USART0_IF   REG_ACCESS(RwReg, 0x4002404CU) /**< \brief (USART0) IrDA Filter Register */
#define REG_USART0_MAN  REG_ACCESS(RwReg, 0x40024050U) /**< \brief (USART0) Manchester Encoder Decoder Register */
#define REG_USART0_WPMR REG_ACCESS(RwReg, 0x400240E4U) /**< \brief (USART0) Write Protect Mode Register */
#define REG_USART0_WPSR REG_ACCESS(RoReg, 0x400240E8U) /**< \brief (USART0) Write Protect Status Register */
#define REG_USART0_VERSION REG_ACCESS(RoReg, 0x400240FCU) /**< \brief (USART0) Version Register */
#define REG_USART0_RPR  REG_ACCESS(RwReg, 0x40024100U) /**< \brief (USART0) Receive Pointer Register */
#define REG_USART0_RCR  REG_ACCESS(RwReg, 0x40024104U) /**< \brief (USART0) Receive Counter Register */
#define REG_USART0_TPR  REG_ACCESS(RwReg, 0x40024108U) /**< \brief (USART0) Transmit Pointer Register */
#define REG_USART0_TCR  REG_ACCESS(RwReg, 0x4002410CU) /**< \brief (USART0) Transmit Counter Register */
#define REG_USART0_RNPR REG_ACCESS(RwReg, 0x40024110U) /**< \brief (USART0) Receive Next Pointer Register */
#define REG_USART0_RNCR REG_ACCESS(RwReg, 0x40024114U) /**< \brief (USART0) Receive Next Counter Register */
#define REG_USART0_TNPR REG_ACCESS(RwReg, 0x40024118U) /**< \brief (USART0) Transmit Next Pointer Register */
#define REG_USART0_TNCR REG_ACCESS(RwReg, 0x4002411CU) /**< \brief (USART0) Transmit Next Counter Register */
#define REG_USART0_PTCR REG_ACCESS(WoReg, 0x40024120U) /**< \brief (USART0) Transfer Control Register */
#define REG_USART0_PTSR REG_ACCESS(RoReg, 0x40024124U) /**< \brief (USART0) Transfer Status Register */

#endif /* _SAM3S_USART0_INSTANCE_ */
