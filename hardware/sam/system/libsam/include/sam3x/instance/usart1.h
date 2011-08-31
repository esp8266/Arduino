/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_USART1_INSTANCE_
#define _SAM3XA_USART1_INSTANCE_

/* ========== Register definition for USART1 peripheral ========== */
#define REG_USART1_CR REG_ACCESS(WoReg, 0x4009C000U) /**< \brief (USART1) Control Register */
#define REG_USART1_MR REG_ACCESS(RwReg, 0x4009C004U) /**< \brief (USART1) Mode Register */
#define REG_USART1_IER REG_ACCESS(WoReg, 0x4009C008U) /**< \brief (USART1) Interrupt Enable Register */
#define REG_USART1_IDR REG_ACCESS(WoReg, 0x4009C00CU) /**< \brief (USART1) Interrupt Disable Register */
#define REG_USART1_IMR REG_ACCESS(RoReg, 0x4009C010U) /**< \brief (USART1) Interrupt Mask Register */
#define REG_USART1_CSR REG_ACCESS(RoReg, 0x4009C014U) /**< \brief (USART1) Channel Status Register */
#define REG_USART1_RHR REG_ACCESS(RoReg, 0x4009C018U) /**< \brief (USART1) Receiver Holding Register */
#define REG_USART1_THR REG_ACCESS(WoReg, 0x4009C01CU) /**< \brief (USART1) Transmitter Holding Register */
#define REG_USART1_BRGR REG_ACCESS(RwReg, 0x4009C020U) /**< \brief (USART1) Baud Rate Generator Register */
#define REG_USART1_RTOR REG_ACCESS(RwReg, 0x4009C024U) /**< \brief (USART1) Receiver Time-out Register */
#define REG_USART1_TTGR REG_ACCESS(RwReg, 0x4009C028U) /**< \brief (USART1) Transmitter Timeguard Register */
#define REG_USART1_FIDI REG_ACCESS(RwReg, 0x4009C040U) /**< \brief (USART1) FI DI Ratio Register */
#define REG_USART1_NER REG_ACCESS(RoReg, 0x4009C044U) /**< \brief (USART1) Number of Errors Register */
#define REG_USART1_IF REG_ACCESS(RwReg, 0x4009C04CU) /**< \brief (USART1) IrDA Filter Register */
#define REG_USART1_MAN REG_ACCESS(RwReg, 0x4009C050U) /**< \brief (USART1) Manchester Encoder Decoder Register */
#define REG_USART1_LINMR REG_ACCESS(RwReg, 0x4009C054U) /**< \brief (USART1) LIN Mode Register */
#define REG_USART1_LINIR REG_ACCESS(RwReg, 0x4009C058U) /**< \brief (USART1) LIN Identifier Register */
#define REG_USART1_WPMR REG_ACCESS(RwReg, 0x4009C0E4U) /**< \brief (USART1) Write Protect Mode Register */
#define REG_USART1_WPSR REG_ACCESS(RoReg, 0x4009C0E8U) /**< \brief (USART1) Write Protect Status Register */
#define REG_USART1_RPR REG_ACCESS(RwReg, 0x4009C100U) /**< \brief (USART1) Receive Pointer Register */
#define REG_USART1_RCR REG_ACCESS(RwReg, 0x4009C104U) /**< \brief (USART1) Receive Counter Register */
#define REG_USART1_TPR REG_ACCESS(RwReg, 0x4009C108U) /**< \brief (USART1) Transmit Pointer Register */
#define REG_USART1_TCR REG_ACCESS(RwReg, 0x4009C10CU) /**< \brief (USART1) Transmit Counter Register */
#define REG_USART1_RNPR REG_ACCESS(RwReg, 0x4009C110U) /**< \brief (USART1) Receive Next Pointer Register */
#define REG_USART1_RNCR REG_ACCESS(RwReg, 0x4009C114U) /**< \brief (USART1) Receive Next Counter Register */
#define REG_USART1_TNPR REG_ACCESS(RwReg, 0x4009C118U) /**< \brief (USART1) Transmit Next Pointer Register */
#define REG_USART1_TNCR REG_ACCESS(RwReg, 0x4009C11CU) /**< \brief (USART1) Transmit Next Counter Register */
#define REG_USART1_PTCR REG_ACCESS(WoReg, 0x4009C120U) /**< \brief (USART1) Transfer Control Register */
#define REG_USART1_PTSR REG_ACCESS(RoReg, 0x4009C124U) /**< \brief (USART1) Transfer Status Register */

#endif /* _SAM3XA_USART1_INSTANCE_ */
