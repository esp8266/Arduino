/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef _SAM3U_ADC_COMPONENT_
#define _SAM3U_ADC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Analog-to-digital Converter */
/* ============================================================================= */
/** \addtogroup SAM3U_ADC Analog-to-digital Converter */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Adc hardware registers */
typedef struct {
  WoReg ADC_CR;        /**< \brief (Adc Offset: 0x00) Control Register */
  RwReg ADC_MR;        /**< \brief (Adc Offset: 0x04) Mode Register */
  RoReg Reserved1[2];
  WoReg ADC_CHER;      /**< \brief (Adc Offset: 0x10) Channel Enable Register */
  WoReg ADC_CHDR;      /**< \brief (Adc Offset: 0x14) Channel Disable Register */
  RoReg ADC_CHSR;      /**< \brief (Adc Offset: 0x18) Channel Status Register */
  RoReg ADC_SR;        /**< \brief (Adc Offset: 0x1C) Status Register */
  RoReg ADC_LCDR;      /**< \brief (Adc Offset: 0x20) Last Converted Data Register */
  WoReg ADC_IER;       /**< \brief (Adc Offset: 0x24) Interrupt Enable Register */
  WoReg ADC_IDR;       /**< \brief (Adc Offset: 0x28) Interrupt Disable Register */
  RoReg ADC_IMR;       /**< \brief (Adc Offset: 0x2C) Interrupt Mask Register */
  RoReg ADC_CDR[8];    /**< \brief (Adc Offset: 0x30) Channel Data Register */
  RoReg Reserved2[44];
  RwReg ADC_RPR;       /**< \brief (Adc Offset: 0x100) Receive Pointer Register */
  RwReg ADC_RCR;       /**< \brief (Adc Offset: 0x104) Receive Counter Register */
  RoReg Reserved3[2];
  RwReg ADC_RNPR;      /**< \brief (Adc Offset: 0x110) Receive Next Pointer Register */
  RwReg ADC_RNCR;      /**< \brief (Adc Offset: 0x114) Receive Next Counter Register */
  RoReg Reserved4[2];
  WoReg ADC_PTCR;      /**< \brief (Adc Offset: 0x120) Transfer Control Register */
  RoReg ADC_PTSR;      /**< \brief (Adc Offset: 0x124) Transfer Status Register */
} Adc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- ADC_CR : (ADC Offset: 0x00) Control Register -------- */
#define ADC_CR_SWRST (0x1u << 0) /**< \brief (ADC_CR) Software Reset */
#define ADC_CR_START (0x1u << 1) /**< \brief (ADC_CR) Start Conversion */
/* -------- ADC_MR : (ADC Offset: 0x04) Mode Register -------- */
#define ADC_MR_TRGEN (0x1u << 0) /**< \brief (ADC_MR) Trigger Enable */
#define   ADC_MR_TRGEN_DIS (0x0u << 0) /**< \brief (ADC_MR) Hardware triggers are disabled. Starting a conversion is only possible by software. */
#define   ADC_MR_TRGEN_EN (0x1u << 0) /**< \brief (ADC_MR) Hardware trigger selected by TRGSEL field is enabled. */
#define ADC_MR_TRGSEL_Pos 1
#define ADC_MR_TRGSEL_Msk (0x7u << ADC_MR_TRGSEL_Pos) /**< \brief (ADC_MR) Trigger Selection */
#define ADC_MR_TRGSEL(value) ((ADC_MR_TRGSEL_Msk & ((value) << ADC_MR_TRGSEL_Pos)))
#define   ADC_MR_TRGSEL_ADC_TRIG0 (0x6u << 1) /**< \brief (ADC_MR) External trigger */
#define   ADC_MR_TRGSEL_ADC_TRIG1 (0x0u << 1) /**< \brief (ADC_MR) TIO Output of the Timer Counter Channel 0 */
#define   ADC_MR_TRGSEL_ADC_TRIG2 (0x1u << 1) /**< \brief (ADC_MR) TIO Output of the Timer Counter Channel 1 */
#define   ADC_MR_TRGSEL_ADC_TRIG3 (0x2u << 1) /**< \brief (ADC_MR) TIO Output of the Timer Counter Channel 2 */
#define   ADC_MR_TRGSEL_ADC_TRIG4 (0x3u << 1) /**< \brief (ADC_MR) PWM Event Line 0 */
#define   ADC_MR_TRGSEL_ADC_TRIG5 (0x4u << 1) /**< \brief (ADC_MR) PWM Event Line 1 */
#define ADC_MR_LOWRES (0x1u << 4) /**< \brief (ADC_MR) Resolution */
#define   ADC_MR_LOWRES_BITS_10 (0x0u << 4) /**< \brief (ADC_MR) 10-bit resolution */
#define   ADC_MR_LOWRES_BITS_8 (0x1u << 4) /**< \brief (ADC_MR) 8-bit resolution */
#define ADC_MR_SLEEP (0x1u << 5) /**< \brief (ADC_MR) Sleep Mode */
#define   ADC_MR_SLEEP_NORMAL (0x0u << 5) /**< \brief (ADC_MR) Normal Mode: The ADC Core and reference voltage circuitry are kept ON between conversions */
#define   ADC_MR_SLEEP_SLEEP (0x1u << 5) /**< \brief (ADC_MR) Sleep Mode: The ADC Core and reference voltage circuitry are OFF between conversions */
#define ADC_MR_PRESCAL_Pos 8
#define ADC_MR_PRESCAL_Msk (0xffu << ADC_MR_PRESCAL_Pos) /**< \brief (ADC_MR) Prescaler Rate Selection */
#define ADC_MR_PRESCAL(value) ((ADC_MR_PRESCAL_Msk & ((value) << ADC_MR_PRESCAL_Pos)))
#define ADC_MR_STARTUP_Pos 16
#define ADC_MR_STARTUP_Msk (0x7fu << ADC_MR_STARTUP_Pos) /**< \brief (ADC_MR) Start Up Time */
#define ADC_MR_STARTUP(value) ((ADC_MR_STARTUP_Msk & ((value) << ADC_MR_STARTUP_Pos)))
#define ADC_MR_SHTIM_Pos 24
#define ADC_MR_SHTIM_Msk (0xfu << ADC_MR_SHTIM_Pos) /**< \brief (ADC_MR) Sample & Hold Time */
#define ADC_MR_SHTIM(value) ((ADC_MR_SHTIM_Msk & ((value) << ADC_MR_SHTIM_Pos)))
/* -------- ADC_CHER : (ADC Offset: 0x10) Channel Enable Register -------- */
#define ADC_CHER_CH0 (0x1u << 0) /**< \brief (ADC_CHER) Channel 0 Enable */
#define ADC_CHER_CH1 (0x1u << 1) /**< \brief (ADC_CHER) Channel 1 Enable */
#define ADC_CHER_CH2 (0x1u << 2) /**< \brief (ADC_CHER) Channel 2 Enable */
#define ADC_CHER_CH3 (0x1u << 3) /**< \brief (ADC_CHER) Channel 3 Enable */
#define ADC_CHER_CH4 (0x1u << 4) /**< \brief (ADC_CHER) Channel 4 Enable */
#define ADC_CHER_CH5 (0x1u << 5) /**< \brief (ADC_CHER) Channel 5 Enable */
#define ADC_CHER_CH6 (0x1u << 6) /**< \brief (ADC_CHER) Channel 6 Enable */
#define ADC_CHER_CH7 (0x1u << 7) /**< \brief (ADC_CHER) Channel 7 Enable */
/* -------- ADC_CHDR : (ADC Offset: 0x14) Channel Disable Register -------- */
#define ADC_CHDR_CH0 (0x1u << 0) /**< \brief (ADC_CHDR) Channel 0 Disable */
#define ADC_CHDR_CH1 (0x1u << 1) /**< \brief (ADC_CHDR) Channel 1 Disable */
#define ADC_CHDR_CH2 (0x1u << 2) /**< \brief (ADC_CHDR) Channel 2 Disable */
#define ADC_CHDR_CH3 (0x1u << 3) /**< \brief (ADC_CHDR) Channel 3 Disable */
#define ADC_CHDR_CH4 (0x1u << 4) /**< \brief (ADC_CHDR) Channel 4 Disable */
#define ADC_CHDR_CH5 (0x1u << 5) /**< \brief (ADC_CHDR) Channel 5 Disable */
#define ADC_CHDR_CH6 (0x1u << 6) /**< \brief (ADC_CHDR) Channel 6 Disable */
#define ADC_CHDR_CH7 (0x1u << 7) /**< \brief (ADC_CHDR) Channel 7 Disable */
/* -------- ADC_CHSR : (ADC Offset: 0x18) Channel Status Register -------- */
#define ADC_CHSR_CH0 (0x1u << 0) /**< \brief (ADC_CHSR) Channel 0 Status */
#define ADC_CHSR_CH1 (0x1u << 1) /**< \brief (ADC_CHSR) Channel 1 Status */
#define ADC_CHSR_CH2 (0x1u << 2) /**< \brief (ADC_CHSR) Channel 2 Status */
#define ADC_CHSR_CH3 (0x1u << 3) /**< \brief (ADC_CHSR) Channel 3 Status */
#define ADC_CHSR_CH4 (0x1u << 4) /**< \brief (ADC_CHSR) Channel 4 Status */
#define ADC_CHSR_CH5 (0x1u << 5) /**< \brief (ADC_CHSR) Channel 5 Status */
#define ADC_CHSR_CH6 (0x1u << 6) /**< \brief (ADC_CHSR) Channel 6 Status */
#define ADC_CHSR_CH7 (0x1u << 7) /**< \brief (ADC_CHSR) Channel 7 Status */
/* -------- ADC_SR : (ADC Offset: 0x1C) Status Register -------- */
#define ADC_SR_EOC0 (0x1u << 0) /**< \brief (ADC_SR) End of Conversion 0 */
#define ADC_SR_EOC1 (0x1u << 1) /**< \brief (ADC_SR) End of Conversion 1 */
#define ADC_SR_EOC2 (0x1u << 2) /**< \brief (ADC_SR) End of Conversion 2 */
#define ADC_SR_EOC3 (0x1u << 3) /**< \brief (ADC_SR) End of Conversion 3 */
#define ADC_SR_EOC4 (0x1u << 4) /**< \brief (ADC_SR) End of Conversion 4 */
#define ADC_SR_EOC5 (0x1u << 5) /**< \brief (ADC_SR) End of Conversion 5 */
#define ADC_SR_EOC6 (0x1u << 6) /**< \brief (ADC_SR) End of Conversion 6 */
#define ADC_SR_EOC7 (0x1u << 7) /**< \brief (ADC_SR) End of Conversion 7 */
#define ADC_SR_OVRE0 (0x1u << 8) /**< \brief (ADC_SR) Overrun Error 0 */
#define ADC_SR_OVRE1 (0x1u << 9) /**< \brief (ADC_SR) Overrun Error 1 */
#define ADC_SR_OVRE2 (0x1u << 10) /**< \brief (ADC_SR) Overrun Error 2 */
#define ADC_SR_OVRE3 (0x1u << 11) /**< \brief (ADC_SR) Overrun Error 3 */
#define ADC_SR_OVRE4 (0x1u << 12) /**< \brief (ADC_SR) Overrun Error 4 */
#define ADC_SR_OVRE5 (0x1u << 13) /**< \brief (ADC_SR) Overrun Error 5 */
#define ADC_SR_OVRE6 (0x1u << 14) /**< \brief (ADC_SR) Overrun Error 6 */
#define ADC_SR_OVRE7 (0x1u << 15) /**< \brief (ADC_SR) Overrun Error 7 */
#define ADC_SR_DRDY (0x1u << 16) /**< \brief (ADC_SR) Data Ready */
#define ADC_SR_GOVRE (0x1u << 17) /**< \brief (ADC_SR) General Overrun Error */
#define ADC_SR_ENDRX (0x1u << 18) /**< \brief (ADC_SR) End of RX Buffer */
#define ADC_SR_RXBUFF (0x1u << 19) /**< \brief (ADC_SR) RX Buffer Full */
/* -------- ADC_LCDR : (ADC Offset: 0x20) Last Converted Data Register -------- */
#define ADC_LCDR_LDATA_Pos 0
#define ADC_LCDR_LDATA_Msk (0x3ffu << ADC_LCDR_LDATA_Pos) /**< \brief (ADC_LCDR) Last Data Converted */
/* -------- ADC_IER : (ADC Offset: 0x24) Interrupt Enable Register -------- */
#define ADC_IER_EOC0 (0x1u << 0) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 0 */
#define ADC_IER_EOC1 (0x1u << 1) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 1 */
#define ADC_IER_EOC2 (0x1u << 2) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 2 */
#define ADC_IER_EOC3 (0x1u << 3) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 3 */
#define ADC_IER_EOC4 (0x1u << 4) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 4 */
#define ADC_IER_EOC5 (0x1u << 5) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 5 */
#define ADC_IER_EOC6 (0x1u << 6) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 6 */
#define ADC_IER_EOC7 (0x1u << 7) /**< \brief (ADC_IER) End of Conversion Interrupt Enable 7 */
#define ADC_IER_OVRE0 (0x1u << 8) /**< \brief (ADC_IER) Overrun Error Interrupt Enable 0 */
#define ADC_IER_OVRE1 (0x1u << 9) /**< \brief (ADC_IER) Overrun Error Interrupt Enable 1 */
#define ADC_IER_OVRE2 (0x1u << 10) /**< \brief (ADC_IER) Overrun Error Interrupt Enable 2 */
#define ADC_IER_OVRE3 (0x1u << 11) /**< \brief (ADC_IER) Overrun Error Interrupt Enable 3 */
#define ADC_IER_OVRE4 (0x1u << 12) /**< \brief (ADC_IER) Overrun Error Interrupt Enable 4 */
#define ADC_IER_OVRE5 (0x1u << 13) /**< \brief (ADC_IER) Overrun Error Interrupt Enable 5 */
#define ADC_IER_OVRE6 (0x1u << 14) /**< \brief (ADC_IER) Overrun Error Interrupt Enable 6 */
#define ADC_IER_OVRE7 (0x1u << 15) /**< \brief (ADC_IER) Overrun Error Interrupt Enable 7 */
#define ADC_IER_DRDY (0x1u << 16) /**< \brief (ADC_IER) Data Ready Interrupt Enable */
#define ADC_IER_GOVRE (0x1u << 17) /**< \brief (ADC_IER) General Overrun Error Interrupt Enable */
#define ADC_IER_ENDRX (0x1u << 18) /**< \brief (ADC_IER) End of Receive Buffer Interrupt Enable */
#define ADC_IER_RXBUFF (0x1u << 19) /**< \brief (ADC_IER) Receive Buffer Full Interrupt Enable */
/* -------- ADC_IDR : (ADC Offset: 0x28) Interrupt Disable Register -------- */
#define ADC_IDR_EOC0 (0x1u << 0) /**< \brief (ADC_IDR) End of Conversion Interrupt Disable 0 */
#define ADC_IDR_EOC1 (0x1u << 1) /**< \brief (ADC_IDR) End of Conversion Interrupt Disable 1 */
#define ADC_IDR_EOC2 (0x1u << 2) /**< \brief (ADC_IDR) End of Conversion Interrupt Disable 2 */
#define ADC_IDR_EOC3 (0x1u << 3) /**< \brief (ADC_IDR) End of Conversion Interrupt Disable 3 */
#define ADC_IDR_EOC4 (0x1u << 4) /**< \brief (ADC_IDR) End of Conversion Interrupt Disable 4 */
#define ADC_IDR_EOC5 (0x1u << 5) /**< \brief (ADC_IDR) End of Conversion Interrupt Disable 5 */
#define ADC_IDR_EOC6 (0x1u << 6) /**< \brief (ADC_IDR) End of Conversion Interrupt Disable 6 */
#define ADC_IDR_EOC7 (0x1u << 7) /**< \brief (ADC_IDR) End of Conversion Interrupt Disable 7 */
#define ADC_IDR_OVRE0 (0x1u << 8) /**< \brief (ADC_IDR) Overrun Error Interrupt Disable 0 */
#define ADC_IDR_OVRE1 (0x1u << 9) /**< \brief (ADC_IDR) Overrun Error Interrupt Disable 1 */
#define ADC_IDR_OVRE2 (0x1u << 10) /**< \brief (ADC_IDR) Overrun Error Interrupt Disable 2 */
#define ADC_IDR_OVRE3 (0x1u << 11) /**< \brief (ADC_IDR) Overrun Error Interrupt Disable 3 */
#define ADC_IDR_OVRE4 (0x1u << 12) /**< \brief (ADC_IDR) Overrun Error Interrupt Disable 4 */
#define ADC_IDR_OVRE5 (0x1u << 13) /**< \brief (ADC_IDR) Overrun Error Interrupt Disable 5 */
#define ADC_IDR_OVRE6 (0x1u << 14) /**< \brief (ADC_IDR) Overrun Error Interrupt Disable 6 */
#define ADC_IDR_OVRE7 (0x1u << 15) /**< \brief (ADC_IDR) Overrun Error Interrupt Disable 7 */
#define ADC_IDR_DRDY (0x1u << 16) /**< \brief (ADC_IDR) Data Ready Interrupt Disable */
#define ADC_IDR_GOVRE (0x1u << 17) /**< \brief (ADC_IDR) General Overrun Error Interrupt Disable */
#define ADC_IDR_ENDRX (0x1u << 18) /**< \brief (ADC_IDR) End of Receive Buffer Interrupt Disable */
#define ADC_IDR_RXBUFF (0x1u << 19) /**< \brief (ADC_IDR) Receive Buffer Full Interrupt Disable */
/* -------- ADC_IMR : (ADC Offset: 0x2C) Interrupt Mask Register -------- */
#define ADC_IMR_EOC0 (0x1u << 0) /**< \brief (ADC_IMR) End of Conversion Interrupt Mask 0 */
#define ADC_IMR_EOC1 (0x1u << 1) /**< \brief (ADC_IMR) End of Conversion Interrupt Mask 1 */
#define ADC_IMR_EOC2 (0x1u << 2) /**< \brief (ADC_IMR) End of Conversion Interrupt Mask 2 */
#define ADC_IMR_EOC3 (0x1u << 3) /**< \brief (ADC_IMR) End of Conversion Interrupt Mask 3 */
#define ADC_IMR_EOC4 (0x1u << 4) /**< \brief (ADC_IMR) End of Conversion Interrupt Mask 4 */
#define ADC_IMR_EOC5 (0x1u << 5) /**< \brief (ADC_IMR) End of Conversion Interrupt Mask 5 */
#define ADC_IMR_EOC6 (0x1u << 6) /**< \brief (ADC_IMR) End of Conversion Interrupt Mask 6 */
#define ADC_IMR_EOC7 (0x1u << 7) /**< \brief (ADC_IMR) End of Conversion Interrupt Mask 7 */
#define ADC_IMR_OVRE0 (0x1u << 8) /**< \brief (ADC_IMR) Overrun Error Interrupt Mask 0 */
#define ADC_IMR_OVRE1 (0x1u << 9) /**< \brief (ADC_IMR) Overrun Error Interrupt Mask 1 */
#define ADC_IMR_OVRE2 (0x1u << 10) /**< \brief (ADC_IMR) Overrun Error Interrupt Mask 2 */
#define ADC_IMR_OVRE3 (0x1u << 11) /**< \brief (ADC_IMR) Overrun Error Interrupt Mask 3 */
#define ADC_IMR_OVRE4 (0x1u << 12) /**< \brief (ADC_IMR) Overrun Error Interrupt Mask 4 */
#define ADC_IMR_OVRE5 (0x1u << 13) /**< \brief (ADC_IMR) Overrun Error Interrupt Mask 5 */
#define ADC_IMR_OVRE6 (0x1u << 14) /**< \brief (ADC_IMR) Overrun Error Interrupt Mask 6 */
#define ADC_IMR_OVRE7 (0x1u << 15) /**< \brief (ADC_IMR) Overrun Error Interrupt Mask 7 */
#define ADC_IMR_DRDY (0x1u << 16) /**< \brief (ADC_IMR) Data Ready Interrupt Mask */
#define ADC_IMR_GOVRE (0x1u << 17) /**< \brief (ADC_IMR) General Overrun Error Interrupt Mask */
#define ADC_IMR_ENDRX (0x1u << 18) /**< \brief (ADC_IMR) End of Receive Buffer Interrupt Mask */
#define ADC_IMR_RXBUFF (0x1u << 19) /**< \brief (ADC_IMR) Receive Buffer Full Interrupt Mask */
/* -------- ADC_CDR[8] : (ADC Offset: 0x30) Channel Data Register -------- */
#define ADC_CDR_DATA_Pos 0
#define ADC_CDR_DATA_Msk (0x3ffu << ADC_CDR_DATA_Pos) /**< \brief (ADC_CDR[8]) Converted Data */
/* -------- ADC_RPR : (ADC Offset: 0x100) Receive Pointer Register -------- */
#define ADC_RPR_RXPTR_Pos 0
#define ADC_RPR_RXPTR_Msk (0xffffffffu << ADC_RPR_RXPTR_Pos) /**< \brief (ADC_RPR) Receive Pointer Register */
#define ADC_RPR_RXPTR(value) ((ADC_RPR_RXPTR_Msk & ((value) << ADC_RPR_RXPTR_Pos)))
/* -------- ADC_RCR : (ADC Offset: 0x104) Receive Counter Register -------- */
#define ADC_RCR_RXCTR_Pos 0
#define ADC_RCR_RXCTR_Msk (0xffffu << ADC_RCR_RXCTR_Pos) /**< \brief (ADC_RCR) Receive Counter Register */
#define ADC_RCR_RXCTR(value) ((ADC_RCR_RXCTR_Msk & ((value) << ADC_RCR_RXCTR_Pos)))
/* -------- ADC_RNPR : (ADC Offset: 0x110) Receive Next Pointer Register -------- */
#define ADC_RNPR_RXNPTR_Pos 0
#define ADC_RNPR_RXNPTR_Msk (0xffffffffu << ADC_RNPR_RXNPTR_Pos) /**< \brief (ADC_RNPR) Receive Next Pointer */
#define ADC_RNPR_RXNPTR(value) ((ADC_RNPR_RXNPTR_Msk & ((value) << ADC_RNPR_RXNPTR_Pos)))
/* -------- ADC_RNCR : (ADC Offset: 0x114) Receive Next Counter Register -------- */
#define ADC_RNCR_RXNCTR_Pos 0
#define ADC_RNCR_RXNCTR_Msk (0xffffu << ADC_RNCR_RXNCTR_Pos) /**< \brief (ADC_RNCR) Receive Next Counter */
#define ADC_RNCR_RXNCTR(value) ((ADC_RNCR_RXNCTR_Msk & ((value) << ADC_RNCR_RXNCTR_Pos)))
/* -------- ADC_PTCR : (ADC Offset: 0x120) Transfer Control Register -------- */
#define ADC_PTCR_RXTEN (0x1u << 0) /**< \brief (ADC_PTCR) Receiver Transfer Enable */
#define ADC_PTCR_RXTDIS (0x1u << 1) /**< \brief (ADC_PTCR) Receiver Transfer Disable */
#define ADC_PTCR_TXTEN (0x1u << 8) /**< \brief (ADC_PTCR) Transmitter Transfer Enable */
#define ADC_PTCR_TXTDIS (0x1u << 9) /**< \brief (ADC_PTCR) Transmitter Transfer Disable */
/* -------- ADC_PTSR : (ADC Offset: 0x124) Transfer Status Register -------- */
#define ADC_PTSR_RXTEN (0x1u << 0) /**< \brief (ADC_PTSR) Receiver Transfer Enable */
#define ADC_PTSR_TXTEN (0x1u << 8) /**< \brief (ADC_PTSR) Transmitter Transfer Enable */

/*@}*/


#endif /* _SAM3U_ADC_COMPONENT_ */
