/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_USART3_INSTANCE_
#define _SAM3XA_USART3_INSTANCE_

/* ========== Register definition for USART3 peripheral ========== */
#define REG_USART3_CR REG_ACCESS(WoReg, 0x400A4000U) /**< \brief (USART3) Control Register */
#define REG_USART3_MR REG_ACCESS(RwReg, 0x400A4004U) /**< \brief (USART3) Mode Register */
#define REG_USART3_IER REG_ACCESS(WoReg, 0x400A4008U) /**< \brief (USART3) Interrupt Enable Register */
#define REG_USART3_IDR REG_ACCESS(WoReg, 0x400A400CU) /**< \brief (USART3) Interrupt Disable Register */
#define REG_USART3_IMR REG_ACCESS(RoReg, 0x400A4010U) /**< \brief (USART3) Interrupt Mask Register */
#define REG_USART3_CSR REG_ACCESS(RoReg, 0x400A4014U) /**< \brief (USART3) Channel Status Register */
#define REG_USART3_RHR REG_ACCESS(RoReg, 0x400A4018U) /**< \brief (USART3) Receiver Holding Register */
#define REG_USART3_THR REG_ACCESS(WoReg, 0x400A401CU) /**< \brief (USART3) Transmitter Holding Register */
#define REG_USART3_BRGR REG_ACCESS(RwReg, 0x400A4020U) /**< \brief (USART3) Baud Rate Generator Register */
#define REG_USART3_RTOR REG_ACCESS(RwReg, 0x400A4024U) /**< \brief (USART3) Receiver Time-out Register */
#define REG_USART3_TTGR REG_ACCESS(RwReg, 0x400A4028U) /**< \brief (USART3) Transmitter Timeguard Register */
#define REG_USART3_FIDI REG_ACCESS(RwReg, 0x400A4040U) /**< \brief (USART3) FI DI Ratio Register */
#define REG_USART3_NER REG_ACCESS(RoReg, 0x400A4044U) /**< \brief (USART3) Number of Errors Register */
#define REG_USART3_IF REG_ACCESS(RwReg, 0x400A404CU) /**< \brief (USART3) IrDA Filter Register */
#define REG_USART3_MAN REG_ACCESS(RwReg, 0x400A4050U) /**< \brief (USART3) Manchester Encoder Decoder Register */
#define REG_USART3_LINMR REG_ACCESS(RwReg, 0x400A4054U) /**< \brief (USART3) LIN Mode Register */
#define REG_USART3_LINIR REG_ACCESS(RwReg, 0x400A4058U) /**< \brief (USART3) LIN Identifier Register */
#define REG_USART3_WPMR REG_ACCESS(RwReg, 0x400A40E4U) /**< \brief (USART3) Write Protect Mode Register */
#define REG_USART3_WPSR REG_ACCESS(RoReg, 0x400A40E8U) /**< \brief (USART3) Write Protect Status Register */
#define REG_USART3_RPR REG_ACCESS(RwReg, 0x400A4100U) /**< \brief (USART3) Receive Pointer Register */
#define REG_USART3_RCR REG_ACCESS(RwReg, 0x400A4104U) /**< \brief (USART3) Receive Counter Register */
#define REG_USART3_TPR REG_ACCESS(RwReg, 0x400A4108U) /**< \brief (USART3) Transmit Pointer Register */
#define REG_USART3_TCR REG_ACCESS(RwReg, 0x400A410CU) /**< \brief (USART3) Transmit Counter Register */
#define REG_USART3_RNPR REG_ACCESS(RwReg, 0x400A4110U) /**< \brief (USART3) Receive Next Pointer Register */
#define REG_USART3_RNCR REG_ACCESS(RwReg, 0x400A4114U) /**< \brief (USART3) Receive Next Counter Register */
#define REG_USART3_TNPR REG_ACCESS(RwReg, 0x400A4118U) /**< \brief (USART3) Transmit Next Pointer Register */
#define REG_USART3_TNCR REG_ACCESS(RwReg, 0x400A411CU) /**< \brief (USART3) Transmit Next Counter Register */
#define REG_USART3_PTCR REG_ACCESS(WoReg, 0x400A4120U) /**< \brief (USART3) Transfer Control Register */
#define REG_USART3_PTSR REG_ACCESS(RoReg, 0x400A4124U) /**< \brief (USART3) Transfer Status Register */

#endif /* _SAM3XA_USART3_INSTANCE_ */
