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

#ifndef _SAM3U_USART3_INSTANCE_
#define _SAM3U_USART3_INSTANCE_

/* ========== Register definition for USART3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_USART3_CR          (0x4009C000U) /**< \brief (USART3) Control Register */
#define REG_USART3_MR          (0x4009C004U) /**< \brief (USART3) Mode Register */
#define REG_USART3_IER          (0x4009C008U) /**< \brief (USART3) Interrupt Enable Register */
#define REG_USART3_IDR          (0x4009C00CU) /**< \brief (USART3) Interrupt Disable Register */
#define REG_USART3_IMR          (0x4009C010U) /**< \brief (USART3) Interrupt Mask Register */
#define REG_USART3_CSR          (0x4009C014U) /**< \brief (USART3) Channel Status Register */
#define REG_USART3_RHR          (0x4009C018U) /**< \brief (USART3) Receiver Holding Register */
#define REG_USART3_THR          (0x4009C01CU) /**< \brief (USART3) Transmitter Holding Register */
#define REG_USART3_BRGR          (0x4009C020U) /**< \brief (USART3) Baud Rate Generator Register */
#define REG_USART3_RTOR          (0x4009C024U) /**< \brief (USART3) Receiver Time-out Register */
#define REG_USART3_TTGR          (0x4009C028U) /**< \brief (USART3) Transmitter Timeguard Register */
#define REG_USART3_FIDI          (0x4009C040U) /**< \brief (USART3) FI DI Ratio Register */
#define REG_USART3_NER          (0x4009C044U) /**< \brief (USART3) Number of Errors Register */
#define REG_USART3_IF          (0x4009C04CU) /**< \brief (USART3) IrDA Filter Register */
#define REG_USART3_MAN          (0x4009C050U) /**< \brief (USART3) Manchester Encoder Decoder Register */
#define REG_USART3_WPMR          (0x4009C0E4U) /**< \brief (USART3) Write Protect Mode Register */
#define REG_USART3_WPSR          (0x4009C0E8U) /**< \brief (USART3) Write Protect Status Register */
#define REG_USART3_RPR          (0x4009C100U) /**< \brief (USART3) Receive Pointer Register */
#define REG_USART3_RCR          (0x4009C104U) /**< \brief (USART3) Receive Counter Register */
#define REG_USART3_TPR          (0x4009C108U) /**< \brief (USART3) Transmit Pointer Register */
#define REG_USART3_TCR          (0x4009C10CU) /**< \brief (USART3) Transmit Counter Register */
#define REG_USART3_RNPR          (0x4009C110U) /**< \brief (USART3) Receive Next Pointer Register */
#define REG_USART3_RNCR          (0x4009C114U) /**< \brief (USART3) Receive Next Counter Register */
#define REG_USART3_TNPR          (0x4009C118U) /**< \brief (USART3) Transmit Next Pointer Register */
#define REG_USART3_TNCR          (0x4009C11CU) /**< \brief (USART3) Transmit Next Counter Register */
#define REG_USART3_PTCR          (0x4009C120U) /**< \brief (USART3) Transfer Control Register */
#define REG_USART3_PTSR          (0x4009C124U) /**< \brief (USART3) Transfer Status Register */
#else
#define REG_USART3_CR (*(WoReg*)0x4009C000U) /**< \brief (USART3) Control Register */
#define REG_USART3_MR (*(RwReg*)0x4009C004U) /**< \brief (USART3) Mode Register */
#define REG_USART3_IER (*(WoReg*)0x4009C008U) /**< \brief (USART3) Interrupt Enable Register */
#define REG_USART3_IDR (*(WoReg*)0x4009C00CU) /**< \brief (USART3) Interrupt Disable Register */
#define REG_USART3_IMR (*(RoReg*)0x4009C010U) /**< \brief (USART3) Interrupt Mask Register */
#define REG_USART3_CSR (*(RoReg*)0x4009C014U) /**< \brief (USART3) Channel Status Register */
#define REG_USART3_RHR (*(RoReg*)0x4009C018U) /**< \brief (USART3) Receiver Holding Register */
#define REG_USART3_THR (*(WoReg*)0x4009C01CU) /**< \brief (USART3) Transmitter Holding Register */
#define REG_USART3_BRGR (*(RwReg*)0x4009C020U) /**< \brief (USART3) Baud Rate Generator Register */
#define REG_USART3_RTOR (*(RwReg*)0x4009C024U) /**< \brief (USART3) Receiver Time-out Register */
#define REG_USART3_TTGR (*(RwReg*)0x4009C028U) /**< \brief (USART3) Transmitter Timeguard Register */
#define REG_USART3_FIDI (*(RwReg*)0x4009C040U) /**< \brief (USART3) FI DI Ratio Register */
#define REG_USART3_NER (*(RoReg*)0x4009C044U) /**< \brief (USART3) Number of Errors Register */
#define REG_USART3_IF (*(RwReg*)0x4009C04CU) /**< \brief (USART3) IrDA Filter Register */
#define REG_USART3_MAN (*(RwReg*)0x4009C050U) /**< \brief (USART3) Manchester Encoder Decoder Register */
#define REG_USART3_WPMR (*(RwReg*)0x4009C0E4U) /**< \brief (USART3) Write Protect Mode Register */
#define REG_USART3_WPSR (*(RoReg*)0x4009C0E8U) /**< \brief (USART3) Write Protect Status Register */
#define REG_USART3_RPR (*(RwReg*)0x4009C100U) /**< \brief (USART3) Receive Pointer Register */
#define REG_USART3_RCR (*(RwReg*)0x4009C104U) /**< \brief (USART3) Receive Counter Register */
#define REG_USART3_TPR (*(RwReg*)0x4009C108U) /**< \brief (USART3) Transmit Pointer Register */
#define REG_USART3_TCR (*(RwReg*)0x4009C10CU) /**< \brief (USART3) Transmit Counter Register */
#define REG_USART3_RNPR (*(RwReg*)0x4009C110U) /**< \brief (USART3) Receive Next Pointer Register */
#define REG_USART3_RNCR (*(RwReg*)0x4009C114U) /**< \brief (USART3) Receive Next Counter Register */
#define REG_USART3_TNPR (*(RwReg*)0x4009C118U) /**< \brief (USART3) Transmit Next Pointer Register */
#define REG_USART3_TNCR (*(RwReg*)0x4009C11CU) /**< \brief (USART3) Transmit Next Counter Register */
#define REG_USART3_PTCR (*(WoReg*)0x4009C120U) /**< \brief (USART3) Transfer Control Register */
#define REG_USART3_PTSR (*(RoReg*)0x4009C124U) /**< \brief (USART3) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3U_USART3_INSTANCE_ */
