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

#ifndef _SAM3U_ADC12B_INSTANCE_
#define _SAM3U_ADC12B_INSTANCE_

/* ========== Register definition for ADC12B peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_ADC12B_CR              (0x400A8000U) /**< \brief (ADC12B) Control Register */
#define REG_ADC12B_MR              (0x400A8004U) /**< \brief (ADC12B) Mode Register */
#define REG_ADC12B_CHER            (0x400A8010U) /**< \brief (ADC12B) Channel Enable Register */
#define REG_ADC12B_CHDR            (0x400A8014U) /**< \brief (ADC12B) Channel Disable Register */
#define REG_ADC12B_CHSR            (0x400A8018U) /**< \brief (ADC12B) Channel Status Register */
#define REG_ADC12B_SR              (0x400A801CU) /**< \brief (ADC12B) Status Register */
#define REG_ADC12B_LCDR            (0x400A8020U) /**< \brief (ADC12B) Last Converted Data Register */
#define REG_ADC12B_IER             (0x400A8024U) /**< \brief (ADC12B) Interrupt Enable Register */
#define REG_ADC12B_IDR             (0x400A8028U) /**< \brief (ADC12B) Interrupt Disable Register */
#define REG_ADC12B_IMR             (0x400A802CU) /**< \brief (ADC12B) Interrupt Mask Register */
#define REG_ADC12B_CDR             (0x400A8030U) /**< \brief (ADC12B) Channel Data Register */
#define REG_ADC12B_ACR             (0x400A8064U) /**< \brief (ADC12B) Analog Control Register */
#define REG_ADC12B_EMR             (0x400A8068U) /**< \brief (ADC12B) Extended Mode Register */
#define REG_ADC12B_RPR             (0x400A8100U) /**< \brief (ADC12B) Receive Pointer Register */
#define REG_ADC12B_RCR             (0x400A8104U) /**< \brief (ADC12B) Receive Counter Register */
#define REG_ADC12B_RNPR            (0x400A8110U) /**< \brief (ADC12B) Receive Next Pointer Register */
#define REG_ADC12B_RNCR            (0x400A8114U) /**< \brief (ADC12B) Receive Next Counter Register */
#define REG_ADC12B_PTCR            (0x400A8120U) /**< \brief (ADC12B) Transfer Control Register */
#define REG_ADC12B_PTSR            (0x400A8124U) /**< \brief (ADC12B) Transfer Status Register */
#else
#define REG_ADC12B_CR     (*(WoReg*)0x400A8000U) /**< \brief (ADC12B) Control Register */
#define REG_ADC12B_MR     (*(RwReg*)0x400A8004U) /**< \brief (ADC12B) Mode Register */
#define REG_ADC12B_CHER   (*(WoReg*)0x400A8010U) /**< \brief (ADC12B) Channel Enable Register */
#define REG_ADC12B_CHDR   (*(WoReg*)0x400A8014U) /**< \brief (ADC12B) Channel Disable Register */
#define REG_ADC12B_CHSR   (*(RoReg*)0x400A8018U) /**< \brief (ADC12B) Channel Status Register */
#define REG_ADC12B_SR     (*(RoReg*)0x400A801CU) /**< \brief (ADC12B) Status Register */
#define REG_ADC12B_LCDR   (*(RoReg*)0x400A8020U) /**< \brief (ADC12B) Last Converted Data Register */
#define REG_ADC12B_IER    (*(WoReg*)0x400A8024U) /**< \brief (ADC12B) Interrupt Enable Register */
#define REG_ADC12B_IDR    (*(WoReg*)0x400A8028U) /**< \brief (ADC12B) Interrupt Disable Register */
#define REG_ADC12B_IMR    (*(RoReg*)0x400A802CU) /**< \brief (ADC12B) Interrupt Mask Register */
#define REG_ADC12B_CDR    (*(RoReg*)0x400A8030U) /**< \brief (ADC12B) Channel Data Register */
#define REG_ADC12B_ACR    (*(RwReg*)0x400A8064U) /**< \brief (ADC12B) Analog Control Register */
#define REG_ADC12B_EMR    (*(RwReg*)0x400A8068U) /**< \brief (ADC12B) Extended Mode Register */
#define REG_ADC12B_RPR    (*(RwReg*)0x400A8100U) /**< \brief (ADC12B) Receive Pointer Register */
#define REG_ADC12B_RCR    (*(RwReg*)0x400A8104U) /**< \brief (ADC12B) Receive Counter Register */
#define REG_ADC12B_RNPR   (*(RwReg*)0x400A8110U) /**< \brief (ADC12B) Receive Next Pointer Register */
#define REG_ADC12B_RNCR   (*(RwReg*)0x400A8114U) /**< \brief (ADC12B) Receive Next Counter Register */
#define REG_ADC12B_PTCR   (*(WoReg*)0x400A8120U) /**< \brief (ADC12B) Transfer Control Register */
#define REG_ADC12B_PTSR   (*(RoReg*)0x400A8124U) /**< \brief (ADC12B) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3U_ADC12B_INSTANCE_ */
