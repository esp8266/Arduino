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

#ifndef _SAM3U_ADC12B_COMPONENT_
#define _SAM3U_ADC12B_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Analog-to-Digital-Converter 12bits */
/* ============================================================================= */
/** \addtogroup SAM3U_ADC12B Analog-to-Digital-Converter 12bits */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Adc12b hardware registers */
typedef struct {
  WoReg ADC12B_CR;     /**< \brief (Adc12b Offset: 0x00) Control Register */
  RwReg ADC12B_MR;     /**< \brief (Adc12b Offset: 0x04) Mode Register */
  RoReg Reserved1[2];
  WoReg ADC12B_CHER;   /**< \brief (Adc12b Offset: 0x10) Channel Enable Register */
  WoReg ADC12B_CHDR;   /**< \brief (Adc12b Offset: 0x14) Channel Disable Register */
  RoReg ADC12B_CHSR;   /**< \brief (Adc12b Offset: 0x18) Channel Status Register */
  RoReg ADC12B_SR;     /**< \brief (Adc12b Offset: 0x1C) Status Register */
  RoReg ADC12B_LCDR;   /**< \brief (Adc12b Offset: 0x20) Last Converted Data Register */
  WoReg ADC12B_IER;    /**< \brief (Adc12b Offset: 0x24) Interrupt Enable Register */
  WoReg ADC12B_IDR;    /**< \brief (Adc12b Offset: 0x28) Interrupt Disable Register */
  RoReg ADC12B_IMR;    /**< \brief (Adc12b Offset: 0x2C) Interrupt Mask Register */
  RoReg ADC12B_CDR[8]; /**< \brief (Adc12b Offset: 0x30) Channel Data Register */
  RoReg Reserved2[5];
  RwReg ADC12B_ACR;    /**< \brief (Adc12b Offset: 0x64) Analog Control Register */
  RwReg ADC12B_EMR;    /**< \brief (Adc12b Offset: 0x68) Extended Mode Register */
  RoReg Reserved3[37];
  RwReg ADC12B_RPR;    /**< \brief (Adc12b Offset: 0x100) Receive Pointer Register */
  RwReg ADC12B_RCR;    /**< \brief (Adc12b Offset: 0x104) Receive Counter Register */
  RoReg Reserved4[2];
  RwReg ADC12B_RNPR;   /**< \brief (Adc12b Offset: 0x110) Receive Next Pointer Register */
  RwReg ADC12B_RNCR;   /**< \brief (Adc12b Offset: 0x114) Receive Next Counter Register */
  RoReg Reserved5[2];
  WoReg ADC12B_PTCR;   /**< \brief (Adc12b Offset: 0x120) Transfer Control Register */
  RoReg ADC12B_PTSR;   /**< \brief (Adc12b Offset: 0x124) Transfer Status Register */
} Adc12b;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- ADC12B_CR : (ADC12B Offset: 0x00) Control Register -------- */
#define ADC12B_CR_SWRST (0x1u << 0) /**< \brief (ADC12B_CR) Software Reset */
#define ADC12B_CR_START (0x1u << 1) /**< \brief (ADC12B_CR) Start Conversion */
/* -------- ADC12B_MR : (ADC12B Offset: 0x04) Mode Register -------- */
#define ADC12B_MR_TRGEN (0x1u << 0) /**< \brief (ADC12B_MR) Trigger Enable */
#define   ADC12B_MR_TRGEN_DIS (0x0u << 0) /**< \brief (ADC12B_MR) Hardware triggers are disabled. Starting a conversion is only possible by software. */
#define   ADC12B_MR_TRGEN_EN (0x1u << 0) /**< \brief (ADC12B_MR) Hardware trigger selected by TRGSEL field is enabled. */
#define ADC12B_MR_TRGSEL_Pos 1
#define ADC12B_MR_TRGSEL_Msk (0x7u << ADC12B_MR_TRGSEL_Pos) /**< \brief (ADC12B_MR) Trigger Selection */
#define ADC12B_MR_TRGSEL(value) ((ADC12B_MR_TRGSEL_Msk & ((value) << ADC12B_MR_TRGSEL_Pos)))
#define   ADC12B_MR_TRGSEL_ADC_TRIG0 (0x0u << 1) /**< \brief (ADC12B_MR) External trigger */
#define   ADC12B_MR_TRGSEL_ADC_TRIG1 (0x1u << 1) /**< \brief (ADC12B_MR) TIO Output of the Timer Counter Channel 0 */
#define   ADC12B_MR_TRGSEL_ADC_TRIG2 (0x2u << 1) /**< \brief (ADC12B_MR) TIO Output of the Timer Counter Channel 1 */
#define   ADC12B_MR_TRGSEL_ADC_TRIG3 (0x3u << 1) /**< \brief (ADC12B_MR) TIO Output of the Timer Counter Channel 2 */
#define   ADC12B_MR_TRGSEL_ADC_TRIG4 (0x4u << 1) /**< \brief (ADC12B_MR) PWM Event Line 0 */
#define   ADC12B_MR_TRGSEL_ADC_TRIG5 (0x5u << 1) /**< \brief (ADC12B_MR) PWM Event Line 1 */
#define ADC12B_MR_LOWRES (0x1u << 4) /**< \brief (ADC12B_MR) Resolution */
#define   ADC12B_MR_LOWRES_BITS_12 (0x0u << 4) /**< \brief (ADC12B_MR) 12-bit resolution */
#define   ADC12B_MR_LOWRES_BITS_10 (0x1u << 4) /**< \brief (ADC12B_MR) 10-bit resolution */
#define ADC12B_MR_SLEEP (0x1u << 5) /**< \brief (ADC12B_MR) Sleep Mode */
#define   ADC12B_MR_SLEEP_NORMAL (0x0u << 5) /**< \brief (ADC12B_MR) Normal Mode: The ADC Core and reference voltage circuitry are kept ON between conversions */
#define   ADC12B_MR_SLEEP_SLEEP (0x1u << 5) /**< \brief (ADC12B_MR) Sleep Mode: The ADC Core and reference voltage circuitry are OFF between conversions */
#define ADC12B_MR_PRESCAL_Pos 8
#define ADC12B_MR_PRESCAL_Msk (0xffu << ADC12B_MR_PRESCAL_Pos) /**< \brief (ADC12B_MR) Prescaler Rate Selection */
#define ADC12B_MR_PRESCAL(value) ((ADC12B_MR_PRESCAL_Msk & ((value) << ADC12B_MR_PRESCAL_Pos)))
#define ADC12B_MR_STARTUP_Pos 16
#define ADC12B_MR_STARTUP_Msk (0xffu << ADC12B_MR_STARTUP_Pos) /**< \brief (ADC12B_MR) Start Up Time */
#define ADC12B_MR_STARTUP(value) ((ADC12B_MR_STARTUP_Msk & ((value) << ADC12B_MR_STARTUP_Pos)))
#define ADC12B_MR_SHTIM_Pos 24
#define ADC12B_MR_SHTIM_Msk (0xfu << ADC12B_MR_SHTIM_Pos) /**< \brief (ADC12B_MR) Sample & Hold Time */
#define ADC12B_MR_SHTIM(value) ((ADC12B_MR_SHTIM_Msk & ((value) << ADC12B_MR_SHTIM_Pos)))
/* -------- ADC12B_CHER : (ADC12B Offset: 0x10) Channel Enable Register -------- */
#define ADC12B_CHER_CH0 (0x1u << 0) /**< \brief (ADC12B_CHER) Channel 0 Enable */
#define ADC12B_CHER_CH1 (0x1u << 1) /**< \brief (ADC12B_CHER) Channel 1 Enable */
#define ADC12B_CHER_CH2 (0x1u << 2) /**< \brief (ADC12B_CHER) Channel 2 Enable */
#define ADC12B_CHER_CH3 (0x1u << 3) /**< \brief (ADC12B_CHER) Channel 3 Enable */
#define ADC12B_CHER_CH4 (0x1u << 4) /**< \brief (ADC12B_CHER) Channel 4 Enable */
#define ADC12B_CHER_CH5 (0x1u << 5) /**< \brief (ADC12B_CHER) Channel 5 Enable */
#define ADC12B_CHER_CH6 (0x1u << 6) /**< \brief (ADC12B_CHER) Channel 6 Enable */
#define ADC12B_CHER_CH7 (0x1u << 7) /**< \brief (ADC12B_CHER) Channel 7 Enable */
/* -------- ADC12B_CHDR : (ADC12B Offset: 0x14) Channel Disable Register -------- */
#define ADC12B_CHDR_CH0 (0x1u << 0) /**< \brief (ADC12B_CHDR)  */
#define ADC12B_CHDR_CH1 (0x1u << 1) /**< \brief (ADC12B_CHDR)  */
#define ADC12B_CHDR_CH2 (0x1u << 2) /**< \brief (ADC12B_CHDR)  */
#define ADC12B_CHDR_CH3 (0x1u << 3) /**< \brief (ADC12B_CHDR)  */
#define ADC12B_CHDR_CH4 (0x1u << 4) /**< \brief (ADC12B_CHDR)  */
#define ADC12B_CHDR_CH5 (0x1u << 5) /**< \brief (ADC12B_CHDR)  */
#define ADC12B_CHDR_CH6 (0x1u << 6) /**< \brief (ADC12B_CHDR)  */
#define ADC12B_CHDR_CH7 (0x1u << 7) /**< \brief (ADC12B_CHDR)  */
/* -------- ADC12B_CHSR : (ADC12B Offset: 0x18) Channel Status Register -------- */
#define ADC12B_CHSR_CH0 (0x1u << 0) /**< \brief (ADC12B_CHSR) Channel 0 Status */
#define ADC12B_CHSR_CH1 (0x1u << 1) /**< \brief (ADC12B_CHSR) Channel 1 Status */
#define ADC12B_CHSR_CH2 (0x1u << 2) /**< \brief (ADC12B_CHSR) Channel 2 Status */
#define ADC12B_CHSR_CH3 (0x1u << 3) /**< \brief (ADC12B_CHSR) Channel 3 Status */
#define ADC12B_CHSR_CH4 (0x1u << 4) /**< \brief (ADC12B_CHSR) Channel 4 Status */
#define ADC12B_CHSR_CH5 (0x1u << 5) /**< \brief (ADC12B_CHSR) Channel 5 Status */
#define ADC12B_CHSR_CH6 (0x1u << 6) /**< \brief (ADC12B_CHSR) Channel 6 Status */
#define ADC12B_CHSR_CH7 (0x1u << 7) /**< \brief (ADC12B_CHSR) Channel 7 Status */
/* -------- ADC12B_SR : (ADC12B Offset: 0x1C) Status Register -------- */
#define ADC12B_SR_EOC0 (0x1u << 0) /**< \brief (ADC12B_SR) End of Conversion 0 */
#define ADC12B_SR_EOC1 (0x1u << 1) /**< \brief (ADC12B_SR) End of Conversion 1 */
#define ADC12B_SR_EOC2 (0x1u << 2) /**< \brief (ADC12B_SR) End of Conversion 2 */
#define ADC12B_SR_EOC3 (0x1u << 3) /**< \brief (ADC12B_SR) End of Conversion 3 */
#define ADC12B_SR_EOC4 (0x1u << 4) /**< \brief (ADC12B_SR) End of Conversion 4 */
#define ADC12B_SR_EOC5 (0x1u << 5) /**< \brief (ADC12B_SR) End of Conversion 5 */
#define ADC12B_SR_EOC6 (0x1u << 6) /**< \brief (ADC12B_SR) End of Conversion 6 */
#define ADC12B_SR_EOC7 (0x1u << 7) /**< \brief (ADC12B_SR) End of Conversion 7 */
#define ADC12B_SR_OVRE0 (0x1u << 8) /**< \brief (ADC12B_SR) Overrun Error 0 */
#define ADC12B_SR_OVRE1 (0x1u << 9) /**< \brief (ADC12B_SR) Overrun Error 1 */
#define ADC12B_SR_OVRE2 (0x1u << 10) /**< \brief (ADC12B_SR) Overrun Error 2 */
#define ADC12B_SR_OVRE3 (0x1u << 11) /**< \brief (ADC12B_SR) Overrun Error 3 */
#define ADC12B_SR_OVRE4 (0x1u << 12) /**< \brief (ADC12B_SR) Overrun Error 4 */
#define ADC12B_SR_OVRE5 (0x1u << 13) /**< \brief (ADC12B_SR) Overrun Error 5 */
#define ADC12B_SR_OVRE6 (0x1u << 14) /**< \brief (ADC12B_SR) Overrun Error 6 */
#define ADC12B_SR_OVRE7 (0x1u << 15) /**< \brief (ADC12B_SR) Overrun Error 7 */
#define ADC12B_SR_DRDY (0x1u << 16) /**< \brief (ADC12B_SR) Data Ready */
#define ADC12B_SR_GOVRE (0x1u << 17) /**< \brief (ADC12B_SR) General Overrun Error */
#define ADC12B_SR_ENDRX (0x1u << 18) /**< \brief (ADC12B_SR) End of RX Buffer */
#define ADC12B_SR_RXBUFF (0x1u << 19) /**< \brief (ADC12B_SR) RX Buffer Full */
/* -------- ADC12B_LCDR : (ADC12B Offset: 0x20) Last Converted Data Register -------- */
#define ADC12B_LCDR_LDATA_Pos 0
#define ADC12B_LCDR_LDATA_Msk (0xfffu << ADC12B_LCDR_LDATA_Pos) /**< \brief (ADC12B_LCDR) Last Data Converted */
/* -------- ADC12B_IER : (ADC12B Offset: 0x24) Interrupt Enable Register -------- */
#define ADC12B_IER_EOC0 (0x1u << 0) /**< \brief (ADC12B_IER) End of Conversion Interrupt Enable 0 */
#define ADC12B_IER_EOC1 (0x1u << 1) /**< \brief (ADC12B_IER) End of Conversion Interrupt Enable 1 */
#define ADC12B_IER_EOC2 (0x1u << 2) /**< \brief (ADC12B_IER) End of Conversion Interrupt Enable 2 */
#define ADC12B_IER_EOC3 (0x1u << 3) /**< \brief (ADC12B_IER) End of Conversion Interrupt Enable 3 */
#define ADC12B_IER_EOC4 (0x1u << 4) /**< \brief (ADC12B_IER) End of Conversion Interrupt Enable 4 */
#define ADC12B_IER_EOC5 (0x1u << 5) /**< \brief (ADC12B_IER) End of Conversion Interrupt Enable 5 */
#define ADC12B_IER_EOC6 (0x1u << 6) /**< \brief (ADC12B_IER) End of Conversion Interrupt Enable 6 */
#define ADC12B_IER_EOC7 (0x1u << 7) /**< \brief (ADC12B_IER) End of Conversion Interrupt Enable 7 */
#define ADC12B_IER_OVRE0 (0x1u << 8) /**< \brief (ADC12B_IER) Overrun Error Interrupt Enable 0 */
#define ADC12B_IER_OVRE1 (0x1u << 9) /**< \brief (ADC12B_IER) Overrun Error Interrupt Enable 1 */
#define ADC12B_IER_OVRE2 (0x1u << 10) /**< \brief (ADC12B_IER) Overrun Error Interrupt Enable 2 */
#define ADC12B_IER_OVRE3 (0x1u << 11) /**< \brief (ADC12B_IER) Overrun Error Interrupt Enable 3 */
#define ADC12B_IER_OVRE4 (0x1u << 12) /**< \brief (ADC12B_IER) Overrun Error Interrupt Enable 4 */
#define ADC12B_IER_OVRE5 (0x1u << 13) /**< \brief (ADC12B_IER) Overrun Error Interrupt Enable 5 */
#define ADC12B_IER_OVRE6 (0x1u << 14) /**< \brief (ADC12B_IER) Overrun Error Interrupt Enable 6 */
#define ADC12B_IER_OVRE7 (0x1u << 15) /**< \brief (ADC12B_IER) Overrun Error Interrupt Enable 7 */
#define ADC12B_IER_DRDY (0x1u << 16) /**< \brief (ADC12B_IER) Data Ready Interrupt Enable */
#define ADC12B_IER_GOVRE (0x1u << 17) /**< \brief (ADC12B_IER) General Overrun Error Interrupt Enable */
#define ADC12B_IER_ENDRX (0x1u << 18) /**< \brief (ADC12B_IER) End of Receive Buffer Interrupt Enable */
#define ADC12B_IER_RXBUFF (0x1u << 19) /**< \brief (ADC12B_IER) Receive Buffer Full Interrupt Enable */
/* -------- ADC12B_IDR : (ADC12B Offset: 0x28) Interrupt Disable Register -------- */
#define ADC12B_IDR_EOC0 (0x1u << 0) /**< \brief (ADC12B_IDR) End of Conversion Interrupt Disable 0 */
#define ADC12B_IDR_EOC1 (0x1u << 1) /**< \brief (ADC12B_IDR) End of Conversion Interrupt Disable 1 */
#define ADC12B_IDR_EOC2 (0x1u << 2) /**< \brief (ADC12B_IDR) End of Conversion Interrupt Disable 2 */
#define ADC12B_IDR_EOC3 (0x1u << 3) /**< \brief (ADC12B_IDR) End of Conversion Interrupt Disable 3 */
#define ADC12B_IDR_EOC4 (0x1u << 4) /**< \brief (ADC12B_IDR) End of Conversion Interrupt Disable 4 */
#define ADC12B_IDR_EOC5 (0x1u << 5) /**< \brief (ADC12B_IDR) End of Conversion Interrupt Disable 5 */
#define ADC12B_IDR_EOC6 (0x1u << 6) /**< \brief (ADC12B_IDR) End of Conversion Interrupt Disable 6 */
#define ADC12B_IDR_EOC7 (0x1u << 7) /**< \brief (ADC12B_IDR) End of Conversion Interrupt Disable 7 */
#define ADC12B_IDR_OVRE0 (0x1u << 8) /**< \brief (ADC12B_IDR) Overrun Error Interrupt Disable 0 */
#define ADC12B_IDR_OVRE1 (0x1u << 9) /**< \brief (ADC12B_IDR) Overrun Error Interrupt Disable 1 */
#define ADC12B_IDR_OVRE2 (0x1u << 10) /**< \brief (ADC12B_IDR) Overrun Error Interrupt Disable 2 */
#define ADC12B_IDR_OVRE3 (0x1u << 11) /**< \brief (ADC12B_IDR) Overrun Error Interrupt Disable 3 */
#define ADC12B_IDR_OVRE4 (0x1u << 12) /**< \brief (ADC12B_IDR) Overrun Error Interrupt Disable 4 */
#define ADC12B_IDR_OVRE5 (0x1u << 13) /**< \brief (ADC12B_IDR) Overrun Error Interrupt Disable 5 */
#define ADC12B_IDR_OVRE6 (0x1u << 14) /**< \brief (ADC12B_IDR) Overrun Error Interrupt Disable 6 */
#define ADC12B_IDR_OVRE7 (0x1u << 15) /**< \brief (ADC12B_IDR) Overrun Error Interrupt Disable 7 */
#define ADC12B_IDR_DRDY (0x1u << 16) /**< \brief (ADC12B_IDR) Data Ready Interrupt Disable */
#define ADC12B_IDR_GOVRE (0x1u << 17) /**< \brief (ADC12B_IDR) General Overrun Error Interrupt Disable */
#define ADC12B_IDR_ENDRX (0x1u << 18) /**< \brief (ADC12B_IDR) End of Receive Buffer Interrupt Disable */
#define ADC12B_IDR_RXBUFF (0x1u << 19) /**< \brief (ADC12B_IDR) Receive Buffer Full Interrupt Disable */
/* -------- ADC12B_IMR : (ADC12B Offset: 0x2C) Interrupt Mask Register -------- */
#define ADC12B_IMR_EOC0 (0x1u << 0) /**< \brief (ADC12B_IMR) End of Conversion Interrupt Mask 0 */
#define ADC12B_IMR_EOC1 (0x1u << 1) /**< \brief (ADC12B_IMR) End of Conversion Interrupt Mask 1 */
#define ADC12B_IMR_EOC2 (0x1u << 2) /**< \brief (ADC12B_IMR) End of Conversion Interrupt Mask 2 */
#define ADC12B_IMR_EOC3 (0x1u << 3) /**< \brief (ADC12B_IMR) End of Conversion Interrupt Mask 3 */
#define ADC12B_IMR_EOC4 (0x1u << 4) /**< \brief (ADC12B_IMR) End of Conversion Interrupt Mask 4 */
#define ADC12B_IMR_EOC5 (0x1u << 5) /**< \brief (ADC12B_IMR) End of Conversion Interrupt Mask 5 */
#define ADC12B_IMR_EOC6 (0x1u << 6) /**< \brief (ADC12B_IMR) End of Conversion Interrupt Mask 6 */
#define ADC12B_IMR_EOC7 (0x1u << 7) /**< \brief (ADC12B_IMR) End of Conversion Interrupt Mask 7 */
#define ADC12B_IMR_OVRE0 (0x1u << 8) /**< \brief (ADC12B_IMR) Overrun Error Interrupt Mask 0 */
#define ADC12B_IMR_OVRE1 (0x1u << 9) /**< \brief (ADC12B_IMR) Overrun Error Interrupt Mask 1 */
#define ADC12B_IMR_OVRE2 (0x1u << 10) /**< \brief (ADC12B_IMR) Overrun Error Interrupt Mask 2 */
#define ADC12B_IMR_OVRE3 (0x1u << 11) /**< \brief (ADC12B_IMR) Overrun Error Interrupt Mask 3 */
#define ADC12B_IMR_OVRE4 (0x1u << 12) /**< \brief (ADC12B_IMR) Overrun Error Interrupt Mask 4 */
#define ADC12B_IMR_OVRE5 (0x1u << 13) /**< \brief (ADC12B_IMR) Overrun Error Interrupt Mask 5 */
#define ADC12B_IMR_OVRE6 (0x1u << 14) /**< \brief (ADC12B_IMR) Overrun Error Interrupt Mask 6 */
#define ADC12B_IMR_OVRE7 (0x1u << 15) /**< \brief (ADC12B_IMR) Overrun Error Interrupt Mask 7 */
#define ADC12B_IMR_DRDY (0x1u << 16) /**< \brief (ADC12B_IMR) Data Ready Interrupt Mask */
#define ADC12B_IMR_GOVRE (0x1u << 17) /**< \brief (ADC12B_IMR) General Overrun Error Interrupt Mask */
#define ADC12B_IMR_ENDRX (0x1u << 18) /**< \brief (ADC12B_IMR) End of Receive Buffer Interrupt Mask */
#define ADC12B_IMR_RXBUFF (0x1u << 19) /**< \brief (ADC12B_IMR) Receive Buffer Full Interrupt Mask */
/* -------- ADC12B_CDR[8] : (ADC12B Offset: 0x30) Channel Data Register -------- */
#define ADC12B_CDR_DATA_Pos 0
#define ADC12B_CDR_DATA_Msk (0xfffu << ADC12B_CDR_DATA_Pos) /**< \brief (ADC12B_CDR[8]) Converted Data */
/* -------- ADC12B_ACR : (ADC12B Offset: 0x64) Analog Control Register -------- */
#define ADC12B_ACR_GAIN_Pos 0
#define ADC12B_ACR_GAIN_Msk (0x3u << ADC12B_ACR_GAIN_Pos) /**< \brief (ADC12B_ACR) Input Gain */
#define ADC12B_ACR_GAIN(value) ((ADC12B_ACR_GAIN_Msk & ((value) << ADC12B_ACR_GAIN_Pos)))
#define ADC12B_ACR_IBCTL_Pos 8
#define ADC12B_ACR_IBCTL_Msk (0x3u << ADC12B_ACR_IBCTL_Pos) /**< \brief (ADC12B_ACR) Bias Current Control */
#define ADC12B_ACR_IBCTL(value) ((ADC12B_ACR_IBCTL_Msk & ((value) << ADC12B_ACR_IBCTL_Pos)))
#define ADC12B_ACR_DIFF (0x1u << 16) /**< \brief (ADC12B_ACR) Differential Mode */
#define ADC12B_ACR_OFFSET (0x1u << 17) /**< \brief (ADC12B_ACR) Input OFFSET */
/* -------- ADC12B_EMR : (ADC12B Offset: 0x68) Extended Mode Register -------- */
#define ADC12B_EMR_OFFMODES (0x1u << 0) /**< \brief (ADC12B_EMR) Off Mode if Sleep Bit (ADC12B_MR) = 1 */
#define ADC12B_EMR_OFF_MODE_STARTUP_TIME_Pos 16
#define ADC12B_EMR_OFF_MODE_STARTUP_TIME_Msk (0xffu << ADC12B_EMR_OFF_MODE_STARTUP_TIME_Pos) /**< \brief (ADC12B_EMR) Startup Time */
#define ADC12B_EMR_OFF_MODE_STARTUP_TIME(value) ((ADC12B_EMR_OFF_MODE_STARTUP_TIME_Msk & ((value) << ADC12B_EMR_OFF_MODE_STARTUP_TIME_Pos)))
/* -------- ADC12B_RPR : (ADC12B Offset: 0x100) Receive Pointer Register -------- */
#define ADC12B_RPR_RXPTR_Pos 0
#define ADC12B_RPR_RXPTR_Msk (0xffffffffu << ADC12B_RPR_RXPTR_Pos) /**< \brief (ADC12B_RPR) Receive Pointer Register */
#define ADC12B_RPR_RXPTR(value) ((ADC12B_RPR_RXPTR_Msk & ((value) << ADC12B_RPR_RXPTR_Pos)))
/* -------- ADC12B_RCR : (ADC12B Offset: 0x104) Receive Counter Register -------- */
#define ADC12B_RCR_RXCTR_Pos 0
#define ADC12B_RCR_RXCTR_Msk (0xffffu << ADC12B_RCR_RXCTR_Pos) /**< \brief (ADC12B_RCR) Receive Counter Register */
#define ADC12B_RCR_RXCTR(value) ((ADC12B_RCR_RXCTR_Msk & ((value) << ADC12B_RCR_RXCTR_Pos)))
/* -------- ADC12B_RNPR : (ADC12B Offset: 0x110) Receive Next Pointer Register -------- */
#define ADC12B_RNPR_RXNPTR_Pos 0
#define ADC12B_RNPR_RXNPTR_Msk (0xffffffffu << ADC12B_RNPR_RXNPTR_Pos) /**< \brief (ADC12B_RNPR) Receive Next Pointer */
#define ADC12B_RNPR_RXNPTR(value) ((ADC12B_RNPR_RXNPTR_Msk & ((value) << ADC12B_RNPR_RXNPTR_Pos)))
/* -------- ADC12B_RNCR : (ADC12B Offset: 0x114) Receive Next Counter Register -------- */
#define ADC12B_RNCR_RXNCTR_Pos 0
#define ADC12B_RNCR_RXNCTR_Msk (0xffffu << ADC12B_RNCR_RXNCTR_Pos) /**< \brief (ADC12B_RNCR) Receive Next Counter */
#define ADC12B_RNCR_RXNCTR(value) ((ADC12B_RNCR_RXNCTR_Msk & ((value) << ADC12B_RNCR_RXNCTR_Pos)))
/* -------- ADC12B_PTCR : (ADC12B Offset: 0x120) Transfer Control Register -------- */
#define ADC12B_PTCR_RXTEN (0x1u << 0) /**< \brief (ADC12B_PTCR) Receiver Transfer Enable */
#define ADC12B_PTCR_RXTDIS (0x1u << 1) /**< \brief (ADC12B_PTCR) Receiver Transfer Disable */
#define ADC12B_PTCR_TXTEN (0x1u << 8) /**< \brief (ADC12B_PTCR) Transmitter Transfer Enable */
#define ADC12B_PTCR_TXTDIS (0x1u << 9) /**< \brief (ADC12B_PTCR) Transmitter Transfer Disable */
/* -------- ADC12B_PTSR : (ADC12B Offset: 0x124) Transfer Status Register -------- */
#define ADC12B_PTSR_RXTEN (0x1u << 0) /**< \brief (ADC12B_PTSR) Receiver Transfer Enable */
#define ADC12B_PTSR_TXTEN (0x1u << 8) /**< \brief (ADC12B_PTSR) Transmitter Transfer Enable */

/*@}*/


#endif /* _SAM3U_ADC12B_COMPONENT_ */
