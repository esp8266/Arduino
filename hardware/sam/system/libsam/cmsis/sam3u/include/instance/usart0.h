/* $asf_license$ */

#ifndef _SAM3U_USART0_INSTANCE_
#define _SAM3U_USART0_INSTANCE_

/* ========== Register definition for USART0 peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_USART0_CR          (0x40090000U) /**< \brief (USART0) Control Register */
#define REG_USART0_MR          (0x40090004U) /**< \brief (USART0) Mode Register */
#define REG_USART0_IER          (0x40090008U) /**< \brief (USART0) Interrupt Enable Register */
#define REG_USART0_IDR          (0x4009000CU) /**< \brief (USART0) Interrupt Disable Register */
#define REG_USART0_IMR          (0x40090010U) /**< \brief (USART0) Interrupt Mask Register */
#define REG_USART0_CSR          (0x40090014U) /**< \brief (USART0) Channel Status Register */
#define REG_USART0_RHR          (0x40090018U) /**< \brief (USART0) Receiver Holding Register */
#define REG_USART0_THR          (0x4009001CU) /**< \brief (USART0) Transmitter Holding Register */
#define REG_USART0_BRGR          (0x40090020U) /**< \brief (USART0) Baud Rate Generator Register */
#define REG_USART0_RTOR          (0x40090024U) /**< \brief (USART0) Receiver Time-out Register */
#define REG_USART0_TTGR          (0x40090028U) /**< \brief (USART0) Transmitter Timeguard Register */
#define REG_USART0_FIDI          (0x40090040U) /**< \brief (USART0) FI DI Ratio Register */
#define REG_USART0_NER          (0x40090044U) /**< \brief (USART0) Number of Errors Register */
#define REG_USART0_IF          (0x4009004CU) /**< \brief (USART0) IrDA Filter Register */
#define REG_USART0_MAN          (0x40090050U) /**< \brief (USART0) Manchester Encoder Decoder Register */
#define REG_USART0_WPMR          (0x400900E4U) /**< \brief (USART0) Write Protect Mode Register */
#define REG_USART0_WPSR          (0x400900E8U) /**< \brief (USART0) Write Protect Status Register */
#define REG_USART0_RPR          (0x40090100U) /**< \brief (USART0) Receive Pointer Register */
#define REG_USART0_RCR          (0x40090104U) /**< \brief (USART0) Receive Counter Register */
#define REG_USART0_TPR          (0x40090108U) /**< \brief (USART0) Transmit Pointer Register */
#define REG_USART0_TCR          (0x4009010CU) /**< \brief (USART0) Transmit Counter Register */
#define REG_USART0_RNPR          (0x40090110U) /**< \brief (USART0) Receive Next Pointer Register */
#define REG_USART0_RNCR          (0x40090114U) /**< \brief (USART0) Receive Next Counter Register */
#define REG_USART0_TNPR          (0x40090118U) /**< \brief (USART0) Transmit Next Pointer Register */
#define REG_USART0_TNCR          (0x4009011CU) /**< \brief (USART0) Transmit Next Counter Register */
#define REG_USART0_PTCR          (0x40090120U) /**< \brief (USART0) Transfer Control Register */
#define REG_USART0_PTSR          (0x40090124U) /**< \brief (USART0) Transfer Status Register */
#else
#define REG_USART0_CR (*(WoReg*)0x40090000U) /**< \brief (USART0) Control Register */
#define REG_USART0_MR (*(RwReg*)0x40090004U) /**< \brief (USART0) Mode Register */
#define REG_USART0_IER (*(WoReg*)0x40090008U) /**< \brief (USART0) Interrupt Enable Register */
#define REG_USART0_IDR (*(WoReg*)0x4009000CU) /**< \brief (USART0) Interrupt Disable Register */
#define REG_USART0_IMR (*(RoReg*)0x40090010U) /**< \brief (USART0) Interrupt Mask Register */
#define REG_USART0_CSR (*(RoReg*)0x40090014U) /**< \brief (USART0) Channel Status Register */
#define REG_USART0_RHR (*(RoReg*)0x40090018U) /**< \brief (USART0) Receiver Holding Register */
#define REG_USART0_THR (*(WoReg*)0x4009001CU) /**< \brief (USART0) Transmitter Holding Register */
#define REG_USART0_BRGR (*(RwReg*)0x40090020U) /**< \brief (USART0) Baud Rate Generator Register */
#define REG_USART0_RTOR (*(RwReg*)0x40090024U) /**< \brief (USART0) Receiver Time-out Register */
#define REG_USART0_TTGR (*(RwReg*)0x40090028U) /**< \brief (USART0) Transmitter Timeguard Register */
#define REG_USART0_FIDI (*(RwReg*)0x40090040U) /**< \brief (USART0) FI DI Ratio Register */
#define REG_USART0_NER (*(RoReg*)0x40090044U) /**< \brief (USART0) Number of Errors Register */
#define REG_USART0_IF (*(RwReg*)0x4009004CU) /**< \brief (USART0) IrDA Filter Register */
#define REG_USART0_MAN (*(RwReg*)0x40090050U) /**< \brief (USART0) Manchester Encoder Decoder Register */
#define REG_USART0_WPMR (*(RwReg*)0x400900E4U) /**< \brief (USART0) Write Protect Mode Register */
#define REG_USART0_WPSR (*(RoReg*)0x400900E8U) /**< \brief (USART0) Write Protect Status Register */
#define REG_USART0_RPR (*(RwReg*)0x40090100U) /**< \brief (USART0) Receive Pointer Register */
#define REG_USART0_RCR (*(RwReg*)0x40090104U) /**< \brief (USART0) Receive Counter Register */
#define REG_USART0_TPR (*(RwReg*)0x40090108U) /**< \brief (USART0) Transmit Pointer Register */
#define REG_USART0_TCR (*(RwReg*)0x4009010CU) /**< \brief (USART0) Transmit Counter Register */
#define REG_USART0_RNPR (*(RwReg*)0x40090110U) /**< \brief (USART0) Receive Next Pointer Register */
#define REG_USART0_RNCR (*(RwReg*)0x40090114U) /**< \brief (USART0) Receive Next Counter Register */
#define REG_USART0_TNPR (*(RwReg*)0x40090118U) /**< \brief (USART0) Transmit Next Pointer Register */
#define REG_USART0_TNCR (*(RwReg*)0x4009011CU) /**< \brief (USART0) Transmit Next Counter Register */
#define REG_USART0_PTCR (*(WoReg*)0x40090120U) /**< \brief (USART0) Transfer Control Register */
#define REG_USART0_PTSR (*(RoReg*)0x40090124U) /**< \brief (USART0) Transfer Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3U_USART0_INSTANCE_ */
