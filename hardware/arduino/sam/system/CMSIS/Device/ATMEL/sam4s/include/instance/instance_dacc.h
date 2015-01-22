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

#ifndef _SAM4S_DACC_INSTANCE_
#define _SAM4S_DACC_INSTANCE_

/* ========== Register definition for DACC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DACC_CR            (0x4003C000U) /**< \brief (DACC) Control Register */
#define REG_DACC_MR            (0x4003C004U) /**< \brief (DACC) Mode Register */
#define REG_DACC_CHER          (0x4003C010U) /**< \brief (DACC) Channel Enable Register */
#define REG_DACC_CHDR          (0x4003C014U) /**< \brief (DACC) Channel Disable Register */
#define REG_DACC_CHSR          (0x4003C018U) /**< \brief (DACC) Channel Status Register */
#define REG_DACC_CDR           (0x4003C020U) /**< \brief (DACC) Conversion Data Register */
#define REG_DACC_IER           (0x4003C024U) /**< \brief (DACC) Interrupt Enable Register */
#define REG_DACC_IDR           (0x4003C028U) /**< \brief (DACC) Interrupt Disable Register */
#define REG_DACC_IMR           (0x4003C02CU) /**< \brief (DACC) Interrupt Mask Register */
#define REG_DACC_ISR           (0x4003C030U) /**< \brief (DACC) Interrupt Status Register */
#define REG_DACC_ACR           (0x4003C094U) /**< \brief (DACC) Analog Current Register */
#define REG_DACC_WPMR          (0x4003C0E4U) /**< \brief (DACC) Write Protect Mode register */
#define REG_DACC_WPSR          (0x4003C0E8U) /**< \brief (DACC) Write Protect Status register */
#define REG_DACC_TPR           (0x4003C108U) /**< \brief (DACC) Transmit Pointer Register */
#define REG_DACC_TCR           (0x4003C10CU) /**< \brief (DACC) Transmit Counter Register */
#define REG_DACC_TNPR          (0x4003C118U) /**< \brief (DACC) Transmit Next Pointer Register */
#define REG_DACC_TNCR          (0x4003C11CU) /**< \brief (DACC) Transmit Next Counter Register */
#define REG_DACC_PTCR          (0x4003C120U) /**< \brief (DACC) Transfer Control Register */
#define REG_DACC_PTSR          (0x4003C124U) /**< \brief (DACC) Transfer Status Register */
#else
#define REG_DACC_CR   (*(WoReg*)0x4003C000U) /**< \brief (DACC) Control Register */
#define REG_DACC_MR   (*(RwReg*)0x4003C004U) /**< \brief (DACC) Mode Register */
#define REG_DACC_CHER (*(WoReg*)0x4003C010U) /**< \brief (DACC) Channel Enable Register */
#define REG_DACC_CHDR (*(WoReg*)0x4003C014U) /**< \brief (DACC) Channel Disable Register */
#define REG_DACC_CHSR (*(RoReg*)0x4003C018U) /**< \brief (DACC) Channel Status Register */
#define REG_DACC_CDR  (*(WoReg*)0x4003C020U) /**< \brief (DACC) Conversion Data Register */
#define REG_DACC_IER  (*(WoReg*)0x4003C024U) /**< \brief (DACC) Interrupt Enable Register */
#define REG_DACC_IDR  (*(WoReg*)0x4003C028U) /**< \brief (DACC) Interrupt Disable Register */
#define REG_DACC_IMR  (*(RoReg*)0x4003C02CU) /**< \brief (DACC) Interrupt Mask Register */
#define REG_DACC_ISR  (*(RoReg*)0x4003C030U) /**< \brief (DACC) Interrupt Status Register */
#define REG_DACC_ACR  (*(RwReg*)0x4003C094U) /**< \brief (DACC) Analog Current Register */
#define REG_DACC_WPMR (*(RwReg*)0x4003C0E4U) /**< \brief (DACC) Write Protect Mode register */
#define REG_DACC_WPSR (*(RoReg*)0x4003C0E8U) /**< \brief (DACC) Write Protect Status register */
#define REG_DACC_TPR  (*(RwReg*)0x4003C108U) /**< \brief (DACC) Transmit Pointer Register */
#define REG_DACC_TCR  (*(RwReg*)0x4003C10CU) /**< \brief (DACC) Transmit Counter Register */
#define REG_DACC_TNPR (*(RwReg*)0x4003C118U) /**< \brief (DACC) Transmit Next Pointer Register */
#define REG_DACC_TNCR (*(RwReg*)0x4003C11CU) /**< \brief (DACC) Transmit Next Counter Register */
#define REG_DACC_PTCR (*(WoReg*)0x4003C120U) /**< \brief (DACC) Transfer Control Register */
#define REG_DACC_PTSR (*(RoReg*)0x4003C124U) /**< \brief (DACC) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4S_DACC_INSTANCE_ */
