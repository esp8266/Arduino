/* %ATMEL_LICENCE% */

#ifndef _SAM3U_ADC_INSTANCE_
#define _SAM3U_ADC_INSTANCE_

/* ========== Register definition for ADC peripheral ========== */
#define REG_ADC_CR     REG_ACCESS(WoReg, 0x400AC000U) /**< \brief (ADC) Control Register */
#define REG_ADC_MR     REG_ACCESS(RwReg, 0x400AC004U) /**< \brief (ADC) Mode Register */
#define REG_ADC_CHER   REG_ACCESS(WoReg, 0x400AC010U) /**< \brief (ADC) Channel Enable Register */
#define REG_ADC_CHDR   REG_ACCESS(WoReg, 0x400AC014U) /**< \brief (ADC) Channel Disable Register */
#define REG_ADC_CHSR   REG_ACCESS(RoReg, 0x400AC018U) /**< \brief (ADC) Channel Status Register */
#define REG_ADC_SR     REG_ACCESS(RoReg, 0x400AC01CU) /**< \brief (ADC) Status Register */
#define REG_ADC_LCDR   REG_ACCESS(RoReg, 0x400AC020U) /**< \brief (ADC) Last Converted Data Register */
#define REG_ADC_IER    REG_ACCESS(WoReg, 0x400AC024U) /**< \brief (ADC) Interrupt Enable Register */
#define REG_ADC_IDR    REG_ACCESS(WoReg, 0x400AC028U) /**< \brief (ADC) Interrupt Disable Register */
#define REG_ADC_IMR    REG_ACCESS(RoReg, 0x400AC02CU) /**< \brief (ADC) Interrupt Mask Register */
#define REG_ADC_CDR    REG_ACCESS(RoReg, 0x400AC030U) /**< \brief (ADC) Channel Data Register */
#define REG_ADC_RPR    REG_ACCESS(RwReg, 0x400AC100U) /**< \brief (ADC) Receive Pointer Register */
#define REG_ADC_RCR    REG_ACCESS(RwReg, 0x400AC104U) /**< \brief (ADC) Receive Counter Register */
#define REG_ADC_TPR    REG_ACCESS(RwReg, 0x400AC108U) /**< \brief (ADC) Transmit Pointer Register */
#define REG_ADC_TCR    REG_ACCESS(RwReg, 0x400AC10CU) /**< \brief (ADC) Transmit Counter Register */
#define REG_ADC_RNPR   REG_ACCESS(RwReg, 0x400AC110U) /**< \brief (ADC) Receive Next Pointer Register */
#define REG_ADC_RNCR   REG_ACCESS(RwReg, 0x400AC114U) /**< \brief (ADC) Receive Next Counter Register */
#define REG_ADC_TNPR   REG_ACCESS(RwReg, 0x400AC118U) /**< \brief (ADC) Transmit Next Pointer Register */
#define REG_ADC_TNCR   REG_ACCESS(RwReg, 0x400AC11CU) /**< \brief (ADC) Transmit Next Counter Register */
#define REG_ADC_PTCR   REG_ACCESS(WoReg, 0x400AC120U) /**< \brief (ADC) Transfer Control Register */
#define REG_ADC_PTSR   REG_ACCESS(RoReg, 0x400AC124U) /**< \brief (ADC) Transfer Status Register */

#endif /* _SAM3U_ADC_INSTANCE_ */
