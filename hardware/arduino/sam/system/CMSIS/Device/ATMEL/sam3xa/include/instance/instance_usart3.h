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

#ifndef _SAM3XA_USART3_INSTANCE_
#define _SAM3XA_USART3_INSTANCE_

/* ========== Register definition for USART3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_USART3_CR          (0x400A4000U) /**< \brief (USART3) Control Register */
#define REG_USART3_MR          (0x400A4004U) /**< \brief (USART3) Mode Register */
#define REG_USART3_IER          (0x400A4008U) /**< \brief (USART3) Interrupt Enable Register */
#define REG_USART3_IDR          (0x400A400CU) /**< \brief (USART3) Interrupt Disable Register */
#define REG_USART3_IMR          (0x400A4010U) /**< \brief (USART3) Interrupt Mask Register */
#define REG_USART3_CSR          (0x400A4014U) /**< \brief (USART3) Channel Status Register */
#define REG_USART3_RHR          (0x400A4018U) /**< \brief (USART3) Receiver Holding Register */
#define REG_USART3_THR          (0x400A401CU) /**< \brief (USART3) Transmitter Holding Register */
#define REG_USART3_BRGR          (0x400A4020U) /**< \brief (USART3) Baud Rate Generator Register */
#define REG_USART3_RTOR          (0x400A4024U) /**< \brief (USART3) Receiver Time-out Register */
#define REG_USART3_TTGR          (0x400A4028U) /**< \brief (USART3) Transmitter Timeguard Register */
#define REG_USART3_FIDI          (0x400A4040U) /**< \brief (USART3) FI DI Ratio Register */
#define REG_USART3_NER          (0x400A4044U) /**< \brief (USART3) Number of Errors Register */
#define REG_USART3_IF          (0x400A404CU) /**< \brief (USART3) IrDA Filter Register */
#define REG_USART3_MAN          (0x400A4050U) /**< \brief (USART3) Manchester Encoder Decoder Register */
#define REG_USART3_LINMR          (0x400A4054U) /**< \brief (USART3) LIN Mode Register */
#define REG_USART3_LINIR          (0x400A4058U) /**< \brief (USART3) LIN Identifier Register */
#define REG_USART3_WPMR          (0x400A40E4U) /**< \brief (USART3) Write Protect Mode Register */
#define REG_USART3_WPSR          (0x400A40E8U) /**< \brief (USART3) Write Protect Status Register */
#define REG_USART3_RPR          (0x400A4100U) /**< \brief (USART3) Receive Pointer Register */
#define REG_USART3_RCR          (0x400A4104U) /**< \brief (USART3) Receive Counter Register */
#define REG_USART3_TPR          (0x400A4108U) /**< \brief (USART3) Transmit Pointer Register */
#define REG_USART3_TCR          (0x400A410CU) /**< \brief (USART3) Transmit Counter Register */
#define REG_USART3_RNPR          (0x400A4110U) /**< \brief (USART3) Receive Next Pointer Register */
#define REG_USART3_RNCR          (0x400A4114U) /**< \brief (USART3) Receive Next Counter Register */
#define REG_USART3_TNPR          (0x400A4118U) /**< \brief (USART3) Transmit Next Pointer Register */
#define REG_USART3_TNCR          (0x400A411CU) /**< \brief (USART3) Transmit Next Counter Register */
#define REG_USART3_PTCR          (0x400A4120U) /**< \brief (USART3) Transfer Control Register */
#define REG_USART3_PTSR          (0x400A4124U) /**< \brief (USART3) Transfer Status Register */
#else
#define REG_USART3_CR (*(WoReg*)0x400A4000U) /**< \brief (USART3) Control Register */
#define REG_USART3_MR (*(RwReg*)0x400A4004U) /**< \brief (USART3) Mode Register */
#define REG_USART3_IER (*(WoReg*)0x400A4008U) /**< \brief (USART3) Interrupt Enable Register */
#define REG_USART3_IDR (*(WoReg*)0x400A400CU) /**< \brief (USART3) Interrupt Disable Register */
#define REG_USART3_IMR (*(RoReg*)0x400A4010U) /**< \brief (USART3) Interrupt Mask Register */
#define REG_USART3_CSR (*(RoReg*)0x400A4014U) /**< \brief (USART3) Channel Status Register */
#define REG_USART3_RHR (*(RoReg*)0x400A4018U) /**< \brief (USART3) Receiver Holding Register */
#define REG_USART3_THR (*(WoReg*)0x400A401CU) /**< \brief (USART3) Transmitter Holding Register */
#define REG_USART3_BRGR (*(RwReg*)0x400A4020U) /**< \brief (USART3) Baud Rate Generator Register */
#define REG_USART3_RTOR (*(RwReg*)0x400A4024U) /**< \brief (USART3) Receiver Time-out Register */
#define REG_USART3_TTGR (*(RwReg*)0x400A4028U) /**< \brief (USART3) Transmitter Timeguard Register */
#define REG_USART3_FIDI (*(RwReg*)0x400A4040U) /**< \brief (USART3) FI DI Ratio Register */
#define REG_USART3_NER (*(RoReg*)0x400A4044U) /**< \brief (USART3) Number of Errors Register */
#define REG_USART3_IF (*(RwReg*)0x400A404CU) /**< \brief (USART3) IrDA Filter Register */
#define REG_USART3_MAN (*(RwReg*)0x400A4050U) /**< \brief (USART3) Manchester Encoder Decoder Register */
#define REG_USART3_LINMR (*(RwReg*)0x400A4054U) /**< \brief (USART3) LIN Mode Register */
#define REG_USART3_LINIR (*(RwReg*)0x400A4058U) /**< \brief (USART3) LIN Identifier Register */
#define REG_USART3_WPMR (*(RwReg*)0x400A40E4U) /**< \brief (USART3) Write Protect Mode Register */
#define REG_USART3_WPSR (*(RoReg*)0x400A40E8U) /**< \brief (USART3) Write Protect Status Register */
#define REG_USART3_RPR (*(RwReg*)0x400A4100U) /**< \brief (USART3) Receive Pointer Register */
#define REG_USART3_RCR (*(RwReg*)0x400A4104U) /**< \brief (USART3) Receive Counter Register */
#define REG_USART3_TPR (*(RwReg*)0x400A4108U) /**< \brief (USART3) Transmit Pointer Register */
#define REG_USART3_TCR (*(RwReg*)0x400A410CU) /**< \brief (USART3) Transmit Counter Register */
#define REG_USART3_RNPR (*(RwReg*)0x400A4110U) /**< \brief (USART3) Receive Next Pointer Register */
#define REG_USART3_RNCR (*(RwReg*)0x400A4114U) /**< \brief (USART3) Receive Next Counter Register */
#define REG_USART3_TNPR (*(RwReg*)0x400A4118U) /**< \brief (USART3) Transmit Next Pointer Register */
#define REG_USART3_TNCR (*(RwReg*)0x400A411CU) /**< \brief (USART3) Transmit Next Counter Register */
#define REG_USART3_PTCR (*(WoReg*)0x400A4120U) /**< \brief (USART3) Transfer Control Register */
#define REG_USART3_PTSR (*(RoReg*)0x400A4124U) /**< \brief (USART3) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3XA_USART3_INSTANCE_ */
