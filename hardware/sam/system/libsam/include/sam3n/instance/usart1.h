/* %ATMEL_LICENCE% */

#ifndef _SAM3N_USART1_INSTANCE_
#define _SAM3N_USART1_INSTANCE_

/* ========== Register definition for USART1 peripheral ========== */
#define REG_USART1_CR REG_ACCESS(WoReg, 0x40028000U) /**< \brief (USART1) Control Register */
#define REG_USART1_MR REG_ACCESS(RwReg, 0x40028004U) /**< \brief (USART1) Mode Register */
#define REG_USART1_IER REG_ACCESS(WoReg, 0x40028008U) /**< \brief (USART1) Interrupt Enable Register */
#define REG_USART1_IDR REG_ACCESS(WoReg, 0x4002800CU) /**< \brief (USART1) Interrupt Disable Register */
#define REG_USART1_IMR REG_ACCESS(RoReg, 0x40028010U) /**< \brief (USART1) Interrupt Mask Register */
#define REG_USART1_CSR REG_ACCESS(RoReg, 0x40028014U) /**< \brief (USART1) Channel Status Register */
#define REG_USART1_RHR REG_ACCESS(RoReg, 0x40028018U) /**< \brief (USART1) Receiver Holding Register */
#define REG_USART1_THR REG_ACCESS(WoReg, 0x4002801CU) /**< \brief (USART1) Transmitter Holding Register */
#define REG_USART1_BRGR REG_ACCESS(RwReg, 0x40028020U) /**< \brief (USART1) Baud Rate Generator Register */
#define REG_USART1_RTOR REG_ACCESS(RwReg, 0x40028024U) /**< \brief (USART1) Receiver Time-out Register */
#define REG_USART1_TTGR REG_ACCESS(RwReg, 0x40028028U) /**< \brief (USART1) Transmitter Timeguard Register */
#define REG_USART1_FIDI REG_ACCESS(RwReg, 0x40028040U) /**< \brief (USART1) FI DI Ratio Register */
#define REG_USART1_NER REG_ACCESS(RoReg, 0x40028044U) /**< \brief (USART1) Number of Errors Register */
#define REG_USART1_IF REG_ACCESS(RwReg, 0x4002804CU) /**< \brief (USART1) IrDA Filter Register */
#define REG_USART1_WPMR REG_ACCESS(RwReg, 0x400280E4U) /**< \brief (USART1) Write Protect Mode Register */
#define REG_USART1_WPSR REG_ACCESS(RoReg, 0x400280E8U) /**< \brief (USART1) Write Protect Status Register */

#endif /* _SAM3N_USART1_INSTANCE_ */
