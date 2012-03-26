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

#ifndef _SAM3U_USART2_INSTANCE_
#define _SAM3U_USART2_INSTANCE_

/* ========== Register definition for USART2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_USART2_CR          (0x40098000U) /**< \brief (USART2) Control Register */
#define REG_USART2_MR          (0x40098004U) /**< \brief (USART2) Mode Register */
#define REG_USART2_IER          (0x40098008U) /**< \brief (USART2) Interrupt Enable Register */
#define REG_USART2_IDR          (0x4009800CU) /**< \brief (USART2) Interrupt Disable Register */
#define REG_USART2_IMR          (0x40098010U) /**< \brief (USART2) Interrupt Mask Register */
#define REG_USART2_CSR          (0x40098014U) /**< \brief (USART2) Channel Status Register */
#define REG_USART2_RHR          (0x40098018U) /**< \brief (USART2) Receiver Holding Register */
#define REG_USART2_THR          (0x4009801CU) /**< \brief (USART2) Transmitter Holding Register */
#define REG_USART2_BRGR          (0x40098020U) /**< \brief (USART2) Baud Rate Generator Register */
#define REG_USART2_RTOR          (0x40098024U) /**< \brief (USART2) Receiver Time-out Register */
#define REG_USART2_TTGR          (0x40098028U) /**< \brief (USART2) Transmitter Timeguard Register */
#define REG_USART2_FIDI          (0x40098040U) /**< \brief (USART2) FI DI Ratio Register */
#define REG_USART2_NER          (0x40098044U) /**< \brief (USART2) Number of Errors Register */
#define REG_USART2_IF          (0x4009804CU) /**< \brief (USART2) IrDA Filter Register */
#define REG_USART2_MAN          (0x40098050U) /**< \brief (USART2) Manchester Encoder Decoder Register */
#define REG_USART2_WPMR          (0x400980E4U) /**< \brief (USART2) Write Protect Mode Register */
#define REG_USART2_WPSR          (0x400980E8U) /**< \brief (USART2) Write Protect Status Register */
#define REG_USART2_RPR          (0x40098100U) /**< \brief (USART2) Receive Pointer Register */
#define REG_USART2_RCR          (0x40098104U) /**< \brief (USART2) Receive Counter Register */
#define REG_USART2_TPR          (0x40098108U) /**< \brief (USART2) Transmit Pointer Register */
#define REG_USART2_TCR          (0x4009810CU) /**< \brief (USART2) Transmit Counter Register */
#define REG_USART2_RNPR          (0x40098110U) /**< \brief (USART2) Receive Next Pointer Register */
#define REG_USART2_RNCR          (0x40098114U) /**< \brief (USART2) Receive Next Counter Register */
#define REG_USART2_TNPR          (0x40098118U) /**< \brief (USART2) Transmit Next Pointer Register */
#define REG_USART2_TNCR          (0x4009811CU) /**< \brief (USART2) Transmit Next Counter Register */
#define REG_USART2_PTCR          (0x40098120U) /**< \brief (USART2) Transfer Control Register */
#define REG_USART2_PTSR          (0x40098124U) /**< \brief (USART2) Transfer Status Register */
#else
#define REG_USART2_CR (*(WoReg*)0x40098000U) /**< \brief (USART2) Control Register */
#define REG_USART2_MR (*(RwReg*)0x40098004U) /**< \brief (USART2) Mode Register */
#define REG_USART2_IER (*(WoReg*)0x40098008U) /**< \brief (USART2) Interrupt Enable Register */
#define REG_USART2_IDR (*(WoReg*)0x4009800CU) /**< \brief (USART2) Interrupt Disable Register */
#define REG_USART2_IMR (*(RoReg*)0x40098010U) /**< \brief (USART2) Interrupt Mask Register */
#define REG_USART2_CSR (*(RoReg*)0x40098014U) /**< \brief (USART2) Channel Status Register */
#define REG_USART2_RHR (*(RoReg*)0x40098018U) /**< \brief (USART2) Receiver Holding Register */
#define REG_USART2_THR (*(WoReg*)0x4009801CU) /**< \brief (USART2) Transmitter Holding Register */
#define REG_USART2_BRGR (*(RwReg*)0x40098020U) /**< \brief (USART2) Baud Rate Generator Register */
#define REG_USART2_RTOR (*(RwReg*)0x40098024U) /**< \brief (USART2) Receiver Time-out Register */
#define REG_USART2_TTGR (*(RwReg*)0x40098028U) /**< \brief (USART2) Transmitter Timeguard Register */
#define REG_USART2_FIDI (*(RwReg*)0x40098040U) /**< \brief (USART2) FI DI Ratio Register */
#define REG_USART2_NER (*(RoReg*)0x40098044U) /**< \brief (USART2) Number of Errors Register */
#define REG_USART2_IF (*(RwReg*)0x4009804CU) /**< \brief (USART2) IrDA Filter Register */
#define REG_USART2_MAN (*(RwReg*)0x40098050U) /**< \brief (USART2) Manchester Encoder Decoder Register */
#define REG_USART2_WPMR (*(RwReg*)0x400980E4U) /**< \brief (USART2) Write Protect Mode Register */
#define REG_USART2_WPSR (*(RoReg*)0x400980E8U) /**< \brief (USART2) Write Protect Status Register */
#define REG_USART2_RPR (*(RwReg*)0x40098100U) /**< \brief (USART2) Receive Pointer Register */
#define REG_USART2_RCR (*(RwReg*)0x40098104U) /**< \brief (USART2) Receive Counter Register */
#define REG_USART2_TPR (*(RwReg*)0x40098108U) /**< \brief (USART2) Transmit Pointer Register */
#define REG_USART2_TCR (*(RwReg*)0x4009810CU) /**< \brief (USART2) Transmit Counter Register */
#define REG_USART2_RNPR (*(RwReg*)0x40098110U) /**< \brief (USART2) Receive Next Pointer Register */
#define REG_USART2_RNCR (*(RwReg*)0x40098114U) /**< \brief (USART2) Receive Next Counter Register */
#define REG_USART2_TNPR (*(RwReg*)0x40098118U) /**< \brief (USART2) Transmit Next Pointer Register */
#define REG_USART2_TNCR (*(RwReg*)0x4009811CU) /**< \brief (USART2) Transmit Next Counter Register */
#define REG_USART2_PTCR (*(WoReg*)0x40098120U) /**< \brief (USART2) Transfer Control Register */
#define REG_USART2_PTSR (*(RoReg*)0x40098124U) /**< \brief (USART2) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3U_USART2_INSTANCE_ */
