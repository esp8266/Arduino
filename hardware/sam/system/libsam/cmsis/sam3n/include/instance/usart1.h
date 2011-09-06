/* $asf_license$ */

#ifndef _SAM3N_USART1_INSTANCE_
#define _SAM3N_USART1_INSTANCE_

/* ========== Register definition for USART1 peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_USART1_CR          (0x40028000U) /**< \brief (USART1) Control Register */
#define REG_USART1_MR          (0x40028004U) /**< \brief (USART1) Mode Register */
#define REG_USART1_IER          (0x40028008U) /**< \brief (USART1) Interrupt Enable Register */
#define REG_USART1_IDR          (0x4002800CU) /**< \brief (USART1) Interrupt Disable Register */
#define REG_USART1_IMR          (0x40028010U) /**< \brief (USART1) Interrupt Mask Register */
#define REG_USART1_CSR          (0x40028014U) /**< \brief (USART1) Channel Status Register */
#define REG_USART1_RHR          (0x40028018U) /**< \brief (USART1) Receiver Holding Register */
#define REG_USART1_THR          (0x4002801CU) /**< \brief (USART1) Transmitter Holding Register */
#define REG_USART1_BRGR          (0x40028020U) /**< \brief (USART1) Baud Rate Generator Register */
#define REG_USART1_RTOR          (0x40028024U) /**< \brief (USART1) Receiver Time-out Register */
#define REG_USART1_TTGR          (0x40028028U) /**< \brief (USART1) Transmitter Timeguard Register */
#define REG_USART1_FIDI          (0x40028040U) /**< \brief (USART1) FI DI Ratio Register */
#define REG_USART1_NER          (0x40028044U) /**< \brief (USART1) Number of Errors Register */
#define REG_USART1_IF          (0x4002804CU) /**< \brief (USART1) IrDA Filter Register */
#define REG_USART1_WPMR          (0x400280E4U) /**< \brief (USART1) Write Protect Mode Register */
#define REG_USART1_WPSR          (0x400280E8U) /**< \brief (USART1) Write Protect Status Register */
#else
#define REG_USART1_CR (*(WoReg*)0x40028000U) /**< \brief (USART1) Control Register */
#define REG_USART1_MR (*(RwReg*)0x40028004U) /**< \brief (USART1) Mode Register */
#define REG_USART1_IER (*(WoReg*)0x40028008U) /**< \brief (USART1) Interrupt Enable Register */
#define REG_USART1_IDR (*(WoReg*)0x4002800CU) /**< \brief (USART1) Interrupt Disable Register */
#define REG_USART1_IMR (*(RoReg*)0x40028010U) /**< \brief (USART1) Interrupt Mask Register */
#define REG_USART1_CSR (*(RoReg*)0x40028014U) /**< \brief (USART1) Channel Status Register */
#define REG_USART1_RHR (*(RoReg*)0x40028018U) /**< \brief (USART1) Receiver Holding Register */
#define REG_USART1_THR (*(WoReg*)0x4002801CU) /**< \brief (USART1) Transmitter Holding Register */
#define REG_USART1_BRGR (*(RwReg*)0x40028020U) /**< \brief (USART1) Baud Rate Generator Register */
#define REG_USART1_RTOR (*(RwReg*)0x40028024U) /**< \brief (USART1) Receiver Time-out Register */
#define REG_USART1_TTGR (*(RwReg*)0x40028028U) /**< \brief (USART1) Transmitter Timeguard Register */
#define REG_USART1_FIDI (*(RwReg*)0x40028040U) /**< \brief (USART1) FI DI Ratio Register */
#define REG_USART1_NER (*(RoReg*)0x40028044U) /**< \brief (USART1) Number of Errors Register */
#define REG_USART1_IF (*(RwReg*)0x4002804CU) /**< \brief (USART1) IrDA Filter Register */
#define REG_USART1_WPMR (*(RwReg*)0x400280E4U) /**< \brief (USART1) Write Protect Mode Register */
#define REG_USART1_WPSR (*(RoReg*)0x400280E8U) /**< \brief (USART1) Write Protect Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3N_USART1_INSTANCE_ */
