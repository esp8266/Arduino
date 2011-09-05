/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_ADC_INSTANCE_
#define _SAM3XA_ADC_INSTANCE_

/* ========== Register definition for ADC peripheral ========== */
#define REG_ADC_CR      REG_ACCESS(WoReg, 0x400C0000U) /**< \brief (ADC) Control Register */
#define REG_ADC_MR      REG_ACCESS(RwReg, 0x400C0004U) /**< \brief (ADC) Mode Register */
#define REG_ADC_SEQR1   REG_ACCESS(RwReg, 0x400C0008U) /**< \brief (ADC) Channel Sequence Register 1 */
#define REG_ADC_SEQR2   REG_ACCESS(RwReg, 0x400C000CU) /**< \brief (ADC) Channel Sequence Register 2 */
#define REG_ADC_CHER    REG_ACCESS(WoReg, 0x400C0010U) /**< \brief (ADC) Channel Enable Register */
#define REG_ADC_CHDR    REG_ACCESS(WoReg, 0x400C0014U) /**< \brief (ADC) Channel Disable Register */
#define REG_ADC_CHSR    REG_ACCESS(RoReg, 0x400C0018U) /**< \brief (ADC) Channel Status Register */
#define REG_ADC_LCDR    REG_ACCESS(RoReg, 0x400C0020U) /**< \brief (ADC) Last Converted Data Register */
#define REG_ADC_IER     REG_ACCESS(WoReg, 0x400C0024U) /**< \brief (ADC) Interrupt Enable Register */
#define REG_ADC_IDR     REG_ACCESS(WoReg, 0x400C0028U) /**< \brief (ADC) Interrupt Disable Register */
#define REG_ADC_IMR     REG_ACCESS(RoReg, 0x400C002CU) /**< \brief (ADC) Interrupt Mask Register */
#define REG_ADC_ISR     REG_ACCESS(RoReg, 0x400C0030U) /**< \brief (ADC) Interrupt Status Register */
#define REG_ADC_OVER    REG_ACCESS(RoReg, 0x400C003CU) /**< \brief (ADC) Overrun Status Register */
#define REG_ADC_EMR     REG_ACCESS(RwReg, 0x400C0040U) /**< \brief (ADC) Extended Mode Register */
#define REG_ADC_CWR     REG_ACCESS(RwReg, 0x400C0044U) /**< \brief (ADC) Compare Window Register */
#define REG_ADC_CGR     REG_ACCESS(RwReg, 0x400C0048U) /**< \brief (ADC) Channel Gain Register */
#define REG_ADC_COR     REG_ACCESS(RwReg, 0x400C004CU) /**< \brief (ADC) Channel Offset Register */
#define REG_ADC_CDR     REG_ACCESS(RoReg, 0x400C0050U) /**< \brief (ADC) Channel Data Register */
#define REG_ADC_ACR     REG_ACCESS(RwReg, 0x400C0094U) /**< \brief (ADC) Analog Control Register */
#define REG_ADC_WPMR    REG_ACCESS(RwReg, 0x400C00E4U) /**< \brief (ADC) Write Protect Mode Register */
#define REG_ADC_WPSR    REG_ACCESS(RoReg, 0x400C00E8U) /**< \brief (ADC) Write Protect Status Register */
#define REG_ADC_RPR     REG_ACCESS(RwReg, 0x400C0100U) /**< \brief (ADC) Receive Pointer Register */
#define REG_ADC_RCR     REG_ACCESS(RwReg, 0x400C0104U) /**< \brief (ADC) Receive Counter Register */
#define REG_ADC_TPR     REG_ACCESS(RwReg, 0x400C0108U) /**< \brief (ADC) Transmit Pointer Register */
#define REG_ADC_TCR     REG_ACCESS(RwReg, 0x400C010CU) /**< \brief (ADC) Transmit Counter Register */
#define REG_ADC_RNPR    REG_ACCESS(RwReg, 0x400C0110U) /**< \brief (ADC) Receive Next Pointer Register */
#define REG_ADC_RNCR    REG_ACCESS(RwReg, 0x400C0114U) /**< \brief (ADC) Receive Next Counter Register */
#define REG_ADC_TNPR    REG_ACCESS(RwReg, 0x400C0118U) /**< \brief (ADC) Transmit Next Pointer Register */
#define REG_ADC_TNCR    REG_ACCESS(RwReg, 0x400C011CU) /**< \brief (ADC) Transmit Next Counter Register */
#define REG_ADC_PTCR    REG_ACCESS(WoReg, 0x400C0120U) /**< \brief (ADC) Transfer Control Register */
#define REG_ADC_PTSR    REG_ACCESS(RoReg, 0x400C0124U) /**< \brief (ADC) Transfer Status Register */

#endif /* _SAM3XA_ADC_INSTANCE_ */
