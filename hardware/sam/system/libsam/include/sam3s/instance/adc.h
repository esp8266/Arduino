/* %ATMEL_LICENCE% */

#ifndef _SAM3S_ADC_INSTANCE_
#define _SAM3S_ADC_INSTANCE_

/* ========== Register definition for ADC peripheral ========== */
#define REG_ADC_CR      REG_ACCESS(WoReg, 0x40038000U) /**< \brief (ADC) Control Register */
#define REG_ADC_MR      REG_ACCESS(RwReg, 0x40038004U) /**< \brief (ADC) Mode Register */
#define REG_ADC_SEQR1   REG_ACCESS(RwReg, 0x40038008U) /**< \brief (ADC) Channel Sequence Register 1 */
#define REG_ADC_SEQR2   REG_ACCESS(RwReg, 0x4003800CU) /**< \brief (ADC) Channel Sequence Register 2 */
#define REG_ADC_CHER    REG_ACCESS(WoReg, 0x40038010U) /**< \brief (ADC) Channel Enable Register */
#define REG_ADC_CHDR    REG_ACCESS(WoReg, 0x40038014U) /**< \brief (ADC) Channel Disable Register */
#define REG_ADC_CHSR    REG_ACCESS(RoReg, 0x40038018U) /**< \brief (ADC) Channel Status Register */
#define REG_ADC_LCDR    REG_ACCESS(RoReg, 0x40038020U) /**< \brief (ADC) Last Converted Data Register */
#define REG_ADC_IER     REG_ACCESS(WoReg, 0x40038024U) /**< \brief (ADC) Interrupt Enable Register */
#define REG_ADC_IDR     REG_ACCESS(WoReg, 0x40038028U) /**< \brief (ADC) Interrupt Disable Register */
#define REG_ADC_IMR     REG_ACCESS(RoReg, 0x4003802CU) /**< \brief (ADC) Interrupt Mask Register */
#define REG_ADC_ISR     REG_ACCESS(RoReg, 0x40038030U) /**< \brief (ADC) Interrupt Status Register */
#define REG_ADC_OVER    REG_ACCESS(RoReg, 0x4003803CU) /**< \brief (ADC) Overrun Status Register */
#define REG_ADC_EMR     REG_ACCESS(RwReg, 0x40038040U) /**< \brief (ADC) Extended Mode Register */
#define REG_ADC_CWR     REG_ACCESS(RwReg, 0x40038044U) /**< \brief (ADC) Compare Window Register */
#define REG_ADC_CGR     REG_ACCESS(RwReg, 0x40038048U) /**< \brief (ADC) Channel Gain Register */
#define REG_ADC_COR     REG_ACCESS(RwReg, 0x4003804CU) /**< \brief (ADC) Channel Offset Register */
#define REG_ADC_CDR     REG_ACCESS(RoReg, 0x40038050U) /**< \brief (ADC) Channel Data Register */
#define REG_ADC_ACR     REG_ACCESS(RwReg, 0x40038094U) /**< \brief (ADC) Analog Control Register */
#define REG_ADC_WPMR    REG_ACCESS(RwReg, 0x400380E4U) /**< \brief (ADC) Write Protect Mode Register */
#define REG_ADC_WPSR    REG_ACCESS(RoReg, 0x400380E8U) /**< \brief (ADC) Write Protect Status Register */
#define REG_ADC_RPR     REG_ACCESS(RwReg, 0x40038100U) /**< \brief (ADC) Receive Pointer Register */
#define REG_ADC_RCR     REG_ACCESS(RwReg, 0x40038104U) /**< \brief (ADC) Receive Counter Register */
#define REG_ADC_TPR     REG_ACCESS(RwReg, 0x40038108U) /**< \brief (ADC) Transmit Pointer Register */
#define REG_ADC_TCR     REG_ACCESS(RwReg, 0x4003810CU) /**< \brief (ADC) Transmit Counter Register */
#define REG_ADC_RNPR    REG_ACCESS(RwReg, 0x40038110U) /**< \brief (ADC) Receive Next Pointer Register */
#define REG_ADC_RNCR    REG_ACCESS(RwReg, 0x40038114U) /**< \brief (ADC) Receive Next Counter Register */
#define REG_ADC_TNPR    REG_ACCESS(RwReg, 0x40038118U) /**< \brief (ADC) Transmit Next Pointer Register */
#define REG_ADC_TNCR    REG_ACCESS(RwReg, 0x4003811CU) /**< \brief (ADC) Transmit Next Counter Register */
#define REG_ADC_PTCR    REG_ACCESS(WoReg, 0x40038120U) /**< \brief (ADC) Transfer Control Register */
#define REG_ADC_PTSR    REG_ACCESS(RoReg, 0x40038124U) /**< \brief (ADC) Transfer Status Register */

#endif /* _SAM3S_ADC_INSTANCE_ */
