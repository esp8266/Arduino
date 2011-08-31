/* %ATMEL_LICENCE% */

#ifndef _SAM3U_ADC12B_INSTANCE_
#define _SAM3U_ADC12B_INSTANCE_

/* ========== Register definition for ADC12B peripheral ========== */
#define REG_ADC12B_CR     REG_ACCESS(WoReg, 0x400A8000U) /**< \brief (ADC12B) Control Register */
#define REG_ADC12B_MR     REG_ACCESS(RwReg, 0x400A8004U) /**< \brief (ADC12B) Mode Register */
#define REG_ADC12B_CHER   REG_ACCESS(WoReg, 0x400A8010U) /**< \brief (ADC12B) Channel Enable Register */
#define REG_ADC12B_CHDR   REG_ACCESS(WoReg, 0x400A8014U) /**< \brief (ADC12B) Channel Disable Register */
#define REG_ADC12B_CHSR   REG_ACCESS(RoReg, 0x400A8018U) /**< \brief (ADC12B) Channel Status Register */
#define REG_ADC12B_SR     REG_ACCESS(RoReg, 0x400A801CU) /**< \brief (ADC12B) Status Register */
#define REG_ADC12B_LCDR   REG_ACCESS(RoReg, 0x400A8020U) /**< \brief (ADC12B) Last Converted Data Register */
#define REG_ADC12B_IER    REG_ACCESS(WoReg, 0x400A8024U) /**< \brief (ADC12B) Interrupt Enable Register */
#define REG_ADC12B_IDR    REG_ACCESS(WoReg, 0x400A8028U) /**< \brief (ADC12B) Interrupt Disable Register */
#define REG_ADC12B_IMR    REG_ACCESS(RoReg, 0x400A802CU) /**< \brief (ADC12B) Interrupt Mask Register */
#define REG_ADC12B_CDR    REG_ACCESS(RoReg, 0x400A8030U) /**< \brief (ADC12B) Channel Data Register */
#define REG_ADC12B_ACR    REG_ACCESS(RwReg, 0x400A8064U) /**< \brief (ADC12B) Analog Control Register */
#define REG_ADC12B_EMR    REG_ACCESS(RwReg, 0x400A8068U) /**< \brief (ADC12B) Extended Mode Register */
#define REG_ADC12B_RPR    REG_ACCESS(RwReg, 0x400A8100U) /**< \brief (ADC12B) Receive Pointer Register */
#define REG_ADC12B_RCR    REG_ACCESS(RwReg, 0x400A8104U) /**< \brief (ADC12B) Receive Counter Register */
#define REG_ADC12B_TPR    REG_ACCESS(RwReg, 0x400A8108U) /**< \brief (ADC12B) Transmit Pointer Register */
#define REG_ADC12B_TCR    REG_ACCESS(RwReg, 0x400A810CU) /**< \brief (ADC12B) Transmit Counter Register */
#define REG_ADC12B_RNPR   REG_ACCESS(RwReg, 0x400A8110U) /**< \brief (ADC12B) Receive Next Pointer Register */
#define REG_ADC12B_RNCR   REG_ACCESS(RwReg, 0x400A8114U) /**< \brief (ADC12B) Receive Next Counter Register */
#define REG_ADC12B_TNPR   REG_ACCESS(RwReg, 0x400A8118U) /**< \brief (ADC12B) Transmit Next Pointer Register */
#define REG_ADC12B_TNCR   REG_ACCESS(RwReg, 0x400A811CU) /**< \brief (ADC12B) Transmit Next Counter Register */
#define REG_ADC12B_PTCR   REG_ACCESS(WoReg, 0x400A8120U) /**< \brief (ADC12B) Transfer Control Register */
#define REG_ADC12B_PTSR   REG_ACCESS(RoReg, 0x400A8124U) /**< \brief (ADC12B) Transfer Status Register */

#endif /* _SAM3U_ADC12B_INSTANCE_ */
