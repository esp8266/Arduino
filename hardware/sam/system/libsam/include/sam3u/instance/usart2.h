/* %ATMEL_LICENCE% */

#ifndef _SAM3U_USART2_INSTANCE_
#define _SAM3U_USART2_INSTANCE_

/* ========== Register definition for USART2 peripheral ========== */
#define REG_USART2_CR REG_ACCESS(WoReg, 0x40098000U) /**< \brief (USART2) Control Register */
#define REG_USART2_MR REG_ACCESS(RwReg, 0x40098004U) /**< \brief (USART2) Mode Register */
#define REG_USART2_IER REG_ACCESS(WoReg, 0x40098008U) /**< \brief (USART2) Interrupt Enable Register */
#define REG_USART2_IDR REG_ACCESS(WoReg, 0x4009800CU) /**< \brief (USART2) Interrupt Disable Register */
#define REG_USART2_IMR REG_ACCESS(RoReg, 0x40098010U) /**< \brief (USART2) Interrupt Mask Register */
#define REG_USART2_CSR REG_ACCESS(RoReg, 0x40098014U) /**< \brief (USART2) Channel Status Register */
#define REG_USART2_RHR REG_ACCESS(RoReg, 0x40098018U) /**< \brief (USART2) Receiver Holding Register */
#define REG_USART2_THR REG_ACCESS(WoReg, 0x4009801CU) /**< \brief (USART2) Transmitter Holding Register */
#define REG_USART2_BRGR REG_ACCESS(RwReg, 0x40098020U) /**< \brief (USART2) Baud Rate Generator Register */
#define REG_USART2_RTOR REG_ACCESS(RwReg, 0x40098024U) /**< \brief (USART2) Receiver Time-out Register */
#define REG_USART2_TTGR REG_ACCESS(RwReg, 0x40098028U) /**< \brief (USART2) Transmitter Timeguard Register */
#define REG_USART2_FIDI REG_ACCESS(RwReg, 0x40098040U) /**< \brief (USART2) FI DI Ratio Register */
#define REG_USART2_NER REG_ACCESS(RoReg, 0x40098044U) /**< \brief (USART2) Number of Errors Register */
#define REG_USART2_IF REG_ACCESS(RwReg, 0x4009804CU) /**< \brief (USART2) IrDA Filter Register */
#define REG_USART2_MAN REG_ACCESS(RwReg, 0x40098050U) /**< \brief (USART2) Manchester Encoder Decoder Register */
#define REG_USART2_WPMR REG_ACCESS(RwReg, 0x400980E4U) /**< \brief (USART2) Write Protect Mode Register */
#define REG_USART2_WPSR REG_ACCESS(RoReg, 0x400980E8U) /**< \brief (USART2) Write Protect Status Register */
#define REG_USART2_RPR REG_ACCESS(RwReg, 0x40098100U) /**< \brief (USART2) Receive Pointer Register */
#define REG_USART2_RCR REG_ACCESS(RwReg, 0x40098104U) /**< \brief (USART2) Receive Counter Register */
#define REG_USART2_TPR REG_ACCESS(RwReg, 0x40098108U) /**< \brief (USART2) Transmit Pointer Register */
#define REG_USART2_TCR REG_ACCESS(RwReg, 0x4009810CU) /**< \brief (USART2) Transmit Counter Register */
#define REG_USART2_RNPR REG_ACCESS(RwReg, 0x40098110U) /**< \brief (USART2) Receive Next Pointer Register */
#define REG_USART2_RNCR REG_ACCESS(RwReg, 0x40098114U) /**< \brief (USART2) Receive Next Counter Register */
#define REG_USART2_TNPR REG_ACCESS(RwReg, 0x40098118U) /**< \brief (USART2) Transmit Next Pointer Register */
#define REG_USART2_TNCR REG_ACCESS(RwReg, 0x4009811CU) /**< \brief (USART2) Transmit Next Counter Register */
#define REG_USART2_PTCR REG_ACCESS(WoReg, 0x40098120U) /**< \brief (USART2) Transfer Control Register */
#define REG_USART2_PTSR REG_ACCESS(RoReg, 0x40098124U) /**< \brief (USART2) Transfer Status Register */

#endif /* _SAM3U_USART2_INSTANCE_ */
