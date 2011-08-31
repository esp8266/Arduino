/* %ATMEL_LICENCE% */

#ifndef _SAM3U_USART3_INSTANCE_
#define _SAM3U_USART3_INSTANCE_

/* ========== Register definition for USART3 peripheral ========== */
#define REG_USART3_CR REG_ACCESS(WoReg, 0x4009C000U) /**< \brief (USART3) Control Register */
#define REG_USART3_MR REG_ACCESS(RwReg, 0x4009C004U) /**< \brief (USART3) Mode Register */
#define REG_USART3_IER REG_ACCESS(WoReg, 0x4009C008U) /**< \brief (USART3) Interrupt Enable Register */
#define REG_USART3_IDR REG_ACCESS(WoReg, 0x4009C00CU) /**< \brief (USART3) Interrupt Disable Register */
#define REG_USART3_IMR REG_ACCESS(RoReg, 0x4009C010U) /**< \brief (USART3) Interrupt Mask Register */
#define REG_USART3_CSR REG_ACCESS(RoReg, 0x4009C014U) /**< \brief (USART3) Channel Status Register */
#define REG_USART3_RHR REG_ACCESS(RoReg, 0x4009C018U) /**< \brief (USART3) Receiver Holding Register */
#define REG_USART3_THR REG_ACCESS(WoReg, 0x4009C01CU) /**< \brief (USART3) Transmitter Holding Register */
#define REG_USART3_BRGR REG_ACCESS(RwReg, 0x4009C020U) /**< \brief (USART3) Baud Rate Generator Register */
#define REG_USART3_RTOR REG_ACCESS(RwReg, 0x4009C024U) /**< \brief (USART3) Receiver Time-out Register */
#define REG_USART3_TTGR REG_ACCESS(RwReg, 0x4009C028U) /**< \brief (USART3) Transmitter Timeguard Register */
#define REG_USART3_FIDI REG_ACCESS(RwReg, 0x4009C040U) /**< \brief (USART3) FI DI Ratio Register */
#define REG_USART3_NER REG_ACCESS(RoReg, 0x4009C044U) /**< \brief (USART3) Number of Errors Register */
#define REG_USART3_IF REG_ACCESS(RwReg, 0x4009C04CU) /**< \brief (USART3) IrDA Filter Register */
#define REG_USART3_MAN REG_ACCESS(RwReg, 0x4009C050U) /**< \brief (USART3) Manchester Encoder Decoder Register */
#define REG_USART3_WPMR REG_ACCESS(RwReg, 0x4009C0E4U) /**< \brief (USART3) Write Protect Mode Register */
#define REG_USART3_WPSR REG_ACCESS(RoReg, 0x4009C0E8U) /**< \brief (USART3) Write Protect Status Register */
#define REG_USART3_RPR REG_ACCESS(RwReg, 0x4009C100U) /**< \brief (USART3) Receive Pointer Register */
#define REG_USART3_RCR REG_ACCESS(RwReg, 0x4009C104U) /**< \brief (USART3) Receive Counter Register */
#define REG_USART3_TPR REG_ACCESS(RwReg, 0x4009C108U) /**< \brief (USART3) Transmit Pointer Register */
#define REG_USART3_TCR REG_ACCESS(RwReg, 0x4009C10CU) /**< \brief (USART3) Transmit Counter Register */
#define REG_USART3_RNPR REG_ACCESS(RwReg, 0x4009C110U) /**< \brief (USART3) Receive Next Pointer Register */
#define REG_USART3_RNCR REG_ACCESS(RwReg, 0x4009C114U) /**< \brief (USART3) Receive Next Counter Register */
#define REG_USART3_TNPR REG_ACCESS(RwReg, 0x4009C118U) /**< \brief (USART3) Transmit Next Pointer Register */
#define REG_USART3_TNCR REG_ACCESS(RwReg, 0x4009C11CU) /**< \brief (USART3) Transmit Next Counter Register */
#define REG_USART3_PTCR REG_ACCESS(WoReg, 0x4009C120U) /**< \brief (USART3) Transfer Control Register */
#define REG_USART3_PTSR REG_ACCESS(RoReg, 0x4009C124U) /**< \brief (USART3) Transfer Status Register */

#endif /* _SAM3U_USART3_INSTANCE_ */
