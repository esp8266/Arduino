/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_USART2_INSTANCE_
#define _SAM3S8_USART2_INSTANCE_

/* ========== Register definition for USART2 peripheral ========== */
#define REG_USART2_CR   REG_ACCESS(WoReg, 0x4002C000U) /**< \brief (USART2) Control Register */
#define REG_USART2_MR   REG_ACCESS(RwReg, 0x4002C004U) /**< \brief (USART2) Mode Register */
#define REG_USART2_IER  REG_ACCESS(WoReg, 0x4002C008U) /**< \brief (USART2) Interrupt Enable Register */
#define REG_USART2_IDR  REG_ACCESS(WoReg, 0x4002C00CU) /**< \brief (USART2) Interrupt Disable Register */
#define REG_USART2_IMR  REG_ACCESS(RoReg, 0x4002C010U) /**< \brief (USART2) Interrupt Mask Register */
#define REG_USART2_CSR  REG_ACCESS(RoReg, 0x4002C014U) /**< \brief (USART2) Channel Status Register */
#define REG_USART2_RHR  REG_ACCESS(RoReg, 0x4002C018U) /**< \brief (USART2) Receiver Holding Register */
#define REG_USART2_THR  REG_ACCESS(WoReg, 0x4002C01CU) /**< \brief (USART2) Transmitter Holding Register */
#define REG_USART2_BRGR REG_ACCESS(RwReg, 0x4002C020U) /**< \brief (USART2) Baud Rate Generator Register */
#define REG_USART2_RTOR REG_ACCESS(RwReg, 0x4002C024U) /**< \brief (USART2) Receiver Time-out Register */
#define REG_USART2_TTGR REG_ACCESS(RwReg, 0x4002C028U) /**< \brief (USART2) Transmitter Timeguard Register */
#define REG_USART2_FIDI REG_ACCESS(RwReg, 0x4002C040U) /**< \brief (USART2) FI DI Ratio Register */
#define REG_USART2_NER  REG_ACCESS(RoReg, 0x4002C044U) /**< \brief (USART2) Number of Errors Register */
#define REG_USART2_IF   REG_ACCESS(RwReg, 0x4002C04CU) /**< \brief (USART2) IrDA Filter Register */
#define REG_USART2_MAN  REG_ACCESS(RwReg, 0x4002C050U) /**< \brief (USART2) Manchester Encoder Decoder Register */
#define REG_USART2_WPMR REG_ACCESS(RwReg, 0x4002C0E4U) /**< \brief (USART2) Write Protect Mode Register */
#define REG_USART2_WPSR REG_ACCESS(RoReg, 0x4002C0E8U) /**< \brief (USART2) Write Protect Status Register */
#define REG_USART2_VERSION REG_ACCESS(RoReg, 0x4002C0FCU) /**< \brief (USART2) Version Register */
#define REG_USART2_RPR  REG_ACCESS(RwReg, 0x4002C100U) /**< \brief (USART2) Receive Pointer Register */
#define REG_USART2_RCR  REG_ACCESS(RwReg, 0x4002C104U) /**< \brief (USART2) Receive Counter Register */
#define REG_USART2_TPR  REG_ACCESS(RwReg, 0x4002C108U) /**< \brief (USART2) Transmit Pointer Register */
#define REG_USART2_TCR  REG_ACCESS(RwReg, 0x4002C10CU) /**< \brief (USART2) Transmit Counter Register */
#define REG_USART2_RNPR REG_ACCESS(RwReg, 0x4002C110U) /**< \brief (USART2) Receive Next Pointer Register */
#define REG_USART2_RNCR REG_ACCESS(RwReg, 0x4002C114U) /**< \brief (USART2) Receive Next Counter Register */
#define REG_USART2_TNPR REG_ACCESS(RwReg, 0x4002C118U) /**< \brief (USART2) Transmit Next Pointer Register */
#define REG_USART2_TNCR REG_ACCESS(RwReg, 0x4002C11CU) /**< \brief (USART2) Transmit Next Counter Register */
#define REG_USART2_PTCR REG_ACCESS(WoReg, 0x4002C120U) /**< \brief (USART2) Transfer Control Register */
#define REG_USART2_PTSR REG_ACCESS(RoReg, 0x4002C124U) /**< \brief (USART2) Transfer Status Register */

#endif /* _SAM3S8_USART2_INSTANCE_ */
