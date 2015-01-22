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

#ifndef _SAM3S8_USART2_INSTANCE_
#define _SAM3S8_USART2_INSTANCE_

/* ========== Register definition for USART2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_USART2_CR            (0x4002C000U) /**< \brief (USART2) Control Register */
#define REG_USART2_MR            (0x4002C004U) /**< \brief (USART2) Mode Register */
#define REG_USART2_IER           (0x4002C008U) /**< \brief (USART2) Interrupt Enable Register */
#define REG_USART2_IDR           (0x4002C00CU) /**< \brief (USART2) Interrupt Disable Register */
#define REG_USART2_IMR           (0x4002C010U) /**< \brief (USART2) Interrupt Mask Register */
#define REG_USART2_CSR           (0x4002C014U) /**< \brief (USART2) Channel Status Register */
#define REG_USART2_RHR           (0x4002C018U) /**< \brief (USART2) Receiver Holding Register */
#define REG_USART2_THR           (0x4002C01CU) /**< \brief (USART2) Transmitter Holding Register */
#define REG_USART2_BRGR          (0x4002C020U) /**< \brief (USART2) Baud Rate Generator Register */
#define REG_USART2_RTOR          (0x4002C024U) /**< \brief (USART2) Receiver Time-out Register */
#define REG_USART2_TTGR          (0x4002C028U) /**< \brief (USART2) Transmitter Timeguard Register */
#define REG_USART2_FIDI          (0x4002C040U) /**< \brief (USART2) FI DI Ratio Register */
#define REG_USART2_NER           (0x4002C044U) /**< \brief (USART2) Number of Errors Register */
#define REG_USART2_IF            (0x4002C04CU) /**< \brief (USART2) IrDA Filter Register */
#define REG_USART2_MAN           (0x4002C050U) /**< \brief (USART2) Manchester Encoder Decoder Register */
#define REG_USART2_WPMR          (0x4002C0E4U) /**< \brief (USART2) Write Protect Mode Register */
#define REG_USART2_WPSR          (0x4002C0E8U) /**< \brief (USART2) Write Protect Status Register */
#define REG_USART2_VERSION          (0x4002C0FCU) /**< \brief (USART2) Version Register */
#define REG_USART2_RPR           (0x4002C100U) /**< \brief (USART2) Receive Pointer Register */
#define REG_USART2_RCR           (0x4002C104U) /**< \brief (USART2) Receive Counter Register */
#define REG_USART2_TPR           (0x4002C108U) /**< \brief (USART2) Transmit Pointer Register */
#define REG_USART2_TCR           (0x4002C10CU) /**< \brief (USART2) Transmit Counter Register */
#define REG_USART2_RNPR          (0x4002C110U) /**< \brief (USART2) Receive Next Pointer Register */
#define REG_USART2_RNCR          (0x4002C114U) /**< \brief (USART2) Receive Next Counter Register */
#define REG_USART2_TNPR          (0x4002C118U) /**< \brief (USART2) Transmit Next Pointer Register */
#define REG_USART2_TNCR          (0x4002C11CU) /**< \brief (USART2) Transmit Next Counter Register */
#define REG_USART2_PTCR          (0x4002C120U) /**< \brief (USART2) Transfer Control Register */
#define REG_USART2_PTSR          (0x4002C124U) /**< \brief (USART2) Transfer Status Register */
#else
#define REG_USART2_CR   (*(WoReg*)0x4002C000U) /**< \brief (USART2) Control Register */
#define REG_USART2_MR   (*(RwReg*)0x4002C004U) /**< \brief (USART2) Mode Register */
#define REG_USART2_IER  (*(WoReg*)0x4002C008U) /**< \brief (USART2) Interrupt Enable Register */
#define REG_USART2_IDR  (*(WoReg*)0x4002C00CU) /**< \brief (USART2) Interrupt Disable Register */
#define REG_USART2_IMR  (*(RoReg*)0x4002C010U) /**< \brief (USART2) Interrupt Mask Register */
#define REG_USART2_CSR  (*(RoReg*)0x4002C014U) /**< \brief (USART2) Channel Status Register */
#define REG_USART2_RHR  (*(RoReg*)0x4002C018U) /**< \brief (USART2) Receiver Holding Register */
#define REG_USART2_THR  (*(WoReg*)0x4002C01CU) /**< \brief (USART2) Transmitter Holding Register */
#define REG_USART2_BRGR (*(RwReg*)0x4002C020U) /**< \brief (USART2) Baud Rate Generator Register */
#define REG_USART2_RTOR (*(RwReg*)0x4002C024U) /**< \brief (USART2) Receiver Time-out Register */
#define REG_USART2_TTGR (*(RwReg*)0x4002C028U) /**< \brief (USART2) Transmitter Timeguard Register */
#define REG_USART2_FIDI (*(RwReg*)0x4002C040U) /**< \brief (USART2) FI DI Ratio Register */
#define REG_USART2_NER  (*(RoReg*)0x4002C044U) /**< \brief (USART2) Number of Errors Register */
#define REG_USART2_IF   (*(RwReg*)0x4002C04CU) /**< \brief (USART2) IrDA Filter Register */
#define REG_USART2_MAN  (*(RwReg*)0x4002C050U) /**< \brief (USART2) Manchester Encoder Decoder Register */
#define REG_USART2_WPMR (*(RwReg*)0x4002C0E4U) /**< \brief (USART2) Write Protect Mode Register */
#define REG_USART2_WPSR (*(RoReg*)0x4002C0E8U) /**< \brief (USART2) Write Protect Status Register */
#define REG_USART2_VERSION (*(RoReg*)0x4002C0FCU) /**< \brief (USART2) Version Register */
#define REG_USART2_RPR  (*(RwReg*)0x4002C100U) /**< \brief (USART2) Receive Pointer Register */
#define REG_USART2_RCR  (*(RwReg*)0x4002C104U) /**< \brief (USART2) Receive Counter Register */
#define REG_USART2_TPR  (*(RwReg*)0x4002C108U) /**< \brief (USART2) Transmit Pointer Register */
#define REG_USART2_TCR  (*(RwReg*)0x4002C10CU) /**< \brief (USART2) Transmit Counter Register */
#define REG_USART2_RNPR (*(RwReg*)0x4002C110U) /**< \brief (USART2) Receive Next Pointer Register */
#define REG_USART2_RNCR (*(RwReg*)0x4002C114U) /**< \brief (USART2) Receive Next Counter Register */
#define REG_USART2_TNPR (*(RwReg*)0x4002C118U) /**< \brief (USART2) Transmit Next Pointer Register */
#define REG_USART2_TNCR (*(RwReg*)0x4002C11CU) /**< \brief (USART2) Transmit Next Counter Register */
#define REG_USART2_PTCR (*(WoReg*)0x4002C120U) /**< \brief (USART2) Transfer Control Register */
#define REG_USART2_PTSR (*(RoReg*)0x4002C124U) /**< \brief (USART2) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3S8_USART2_INSTANCE_ */
