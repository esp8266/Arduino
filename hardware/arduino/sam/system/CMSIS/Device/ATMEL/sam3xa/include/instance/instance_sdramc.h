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

#ifndef _SAM3XA_SDRAMC_INSTANCE_
#define _SAM3XA_SDRAMC_INSTANCE_

/* ========== Register definition for SDRAMC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SDRAMC_MR            (0x400E0200U) /**< \brief (SDRAMC) SDRAMC Mode Register */
#define REG_SDRAMC_TR            (0x400E0204U) /**< \brief (SDRAMC) SDRAMC Refresh Timer Register */
#define REG_SDRAMC_CR            (0x400E0208U) /**< \brief (SDRAMC) SDRAMC Configuration Register */
#define REG_SDRAMC_LPR           (0x400E0210U) /**< \brief (SDRAMC) SDRAMC Low Power Register */
#define REG_SDRAMC_IER           (0x400E0214U) /**< \brief (SDRAMC) SDRAMC Interrupt Enable Register */
#define REG_SDRAMC_IDR           (0x400E0218U) /**< \brief (SDRAMC) SDRAMC Interrupt Disable Register */
#define REG_SDRAMC_IMR           (0x400E021CU) /**< \brief (SDRAMC) SDRAMC Interrupt Mask Register */
#define REG_SDRAMC_ISR           (0x400E0220U) /**< \brief (SDRAMC) SDRAMC Interrupt Status Register */
#define REG_SDRAMC_MDR           (0x400E0224U) /**< \brief (SDRAMC) SDRAMC Memory Device Register */
#define REG_SDRAMC_CR1           (0x400E0228U) /**< \brief (SDRAMC) SDRAMC Configuration Register 1 */
#define REG_SDRAMC_OCMS          (0x400E022CU) /**< \brief (SDRAMC) SDRAMC OCMS Register 1 */
#else
#define REG_SDRAMC_MR   (*(RwReg*)0x400E0200U) /**< \brief (SDRAMC) SDRAMC Mode Register */
#define REG_SDRAMC_TR   (*(RwReg*)0x400E0204U) /**< \brief (SDRAMC) SDRAMC Refresh Timer Register */
#define REG_SDRAMC_CR   (*(RwReg*)0x400E0208U) /**< \brief (SDRAMC) SDRAMC Configuration Register */
#define REG_SDRAMC_LPR  (*(RwReg*)0x400E0210U) /**< \brief (SDRAMC) SDRAMC Low Power Register */
#define REG_SDRAMC_IER  (*(WoReg*)0x400E0214U) /**< \brief (SDRAMC) SDRAMC Interrupt Enable Register */
#define REG_SDRAMC_IDR  (*(WoReg*)0x400E0218U) /**< \brief (SDRAMC) SDRAMC Interrupt Disable Register */
#define REG_SDRAMC_IMR  (*(RoReg*)0x400E021CU) /**< \brief (SDRAMC) SDRAMC Interrupt Mask Register */
#define REG_SDRAMC_ISR  (*(RoReg*)0x400E0220U) /**< \brief (SDRAMC) SDRAMC Interrupt Status Register */
#define REG_SDRAMC_MDR  (*(RwReg*)0x400E0224U) /**< \brief (SDRAMC) SDRAMC Memory Device Register */
#define REG_SDRAMC_CR1  (*(RwReg*)0x400E0228U) /**< \brief (SDRAMC) SDRAMC Configuration Register 1 */
#define REG_SDRAMC_OCMS (*(RwReg*)0x400E022CU) /**< \brief (SDRAMC) SDRAMC OCMS Register 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3XA_SDRAMC_INSTANCE_ */
