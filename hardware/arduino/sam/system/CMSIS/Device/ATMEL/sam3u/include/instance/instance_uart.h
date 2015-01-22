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

#ifndef _SAM3U_UART_INSTANCE_
#define _SAM3U_UART_INSTANCE_

/* ========== Register definition for UART peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_UART_CR            (0x400E0600U) /**< \brief (UART) Control Register */
#define REG_UART_MR            (0x400E0604U) /**< \brief (UART) Mode Register */
#define REG_UART_IER           (0x400E0608U) /**< \brief (UART) Interrupt Enable Register */
#define REG_UART_IDR           (0x400E060CU) /**< \brief (UART) Interrupt Disable Register */
#define REG_UART_IMR           (0x400E0610U) /**< \brief (UART) Interrupt Mask Register */
#define REG_UART_SR            (0x400E0614U) /**< \brief (UART) Status Register */
#define REG_UART_RHR           (0x400E0618U) /**< \brief (UART) Receive Holding Register */
#define REG_UART_THR           (0x400E061CU) /**< \brief (UART) Transmit Holding Register */
#define REG_UART_BRGR          (0x400E0620U) /**< \brief (UART) Baud Rate Generator Register */
#define REG_UART_RPR           (0x400E0700U) /**< \brief (UART) Receive Pointer Register */
#define REG_UART_RCR           (0x400E0704U) /**< \brief (UART) Receive Counter Register */
#define REG_UART_TPR           (0x400E0708U) /**< \brief (UART) Transmit Pointer Register */
#define REG_UART_TCR           (0x400E070CU) /**< \brief (UART) Transmit Counter Register */
#define REG_UART_RNPR          (0x400E0710U) /**< \brief (UART) Receive Next Pointer Register */
#define REG_UART_RNCR          (0x400E0714U) /**< \brief (UART) Receive Next Counter Register */
#define REG_UART_TNPR          (0x400E0718U) /**< \brief (UART) Transmit Next Pointer Register */
#define REG_UART_TNCR          (0x400E071CU) /**< \brief (UART) Transmit Next Counter Register */
#define REG_UART_PTCR          (0x400E0720U) /**< \brief (UART) Transfer Control Register */
#define REG_UART_PTSR          (0x400E0724U) /**< \brief (UART) Transfer Status Register */
#else
#define REG_UART_CR   (*(WoReg*)0x400E0600U) /**< \brief (UART) Control Register */
#define REG_UART_MR   (*(RwReg*)0x400E0604U) /**< \brief (UART) Mode Register */
#define REG_UART_IER  (*(WoReg*)0x400E0608U) /**< \brief (UART) Interrupt Enable Register */
#define REG_UART_IDR  (*(WoReg*)0x400E060CU) /**< \brief (UART) Interrupt Disable Register */
#define REG_UART_IMR  (*(RoReg*)0x400E0610U) /**< \brief (UART) Interrupt Mask Register */
#define REG_UART_SR   (*(RoReg*)0x400E0614U) /**< \brief (UART) Status Register */
#define REG_UART_RHR  (*(RoReg*)0x400E0618U) /**< \brief (UART) Receive Holding Register */
#define REG_UART_THR  (*(WoReg*)0x400E061CU) /**< \brief (UART) Transmit Holding Register */
#define REG_UART_BRGR (*(RwReg*)0x400E0620U) /**< \brief (UART) Baud Rate Generator Register */
#define REG_UART_RPR  (*(RwReg*)0x400E0700U) /**< \brief (UART) Receive Pointer Register */
#define REG_UART_RCR  (*(RwReg*)0x400E0704U) /**< \brief (UART) Receive Counter Register */
#define REG_UART_TPR  (*(RwReg*)0x400E0708U) /**< \brief (UART) Transmit Pointer Register */
#define REG_UART_TCR  (*(RwReg*)0x400E070CU) /**< \brief (UART) Transmit Counter Register */
#define REG_UART_RNPR (*(RwReg*)0x400E0710U) /**< \brief (UART) Receive Next Pointer Register */
#define REG_UART_RNCR (*(RwReg*)0x400E0714U) /**< \brief (UART) Receive Next Counter Register */
#define REG_UART_TNPR (*(RwReg*)0x400E0718U) /**< \brief (UART) Transmit Next Pointer Register */
#define REG_UART_TNCR (*(RwReg*)0x400E071CU) /**< \brief (UART) Transmit Next Counter Register */
#define REG_UART_PTCR (*(WoReg*)0x400E0720U) /**< \brief (UART) Transfer Control Register */
#define REG_UART_PTSR (*(RoReg*)0x400E0724U) /**< \brief (UART) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3U_UART_INSTANCE_ */
