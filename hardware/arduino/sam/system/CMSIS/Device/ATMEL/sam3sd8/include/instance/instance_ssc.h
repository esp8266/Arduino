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

#ifndef _SAM3S8_SSC_INSTANCE_
#define _SAM3S8_SSC_INSTANCE_

/* ========== Register definition for SSC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SSC_CR            (0x40004000U) /**< \brief (SSC) Control Register */
#define REG_SSC_CMR           (0x40004004U) /**< \brief (SSC) Clock Mode Register */
#define REG_SSC_RCMR          (0x40004010U) /**< \brief (SSC) Receive Clock Mode Register */
#define REG_SSC_RFMR          (0x40004014U) /**< \brief (SSC) Receive Frame Mode Register */
#define REG_SSC_TCMR          (0x40004018U) /**< \brief (SSC) Transmit Clock Mode Register */
#define REG_SSC_TFMR          (0x4000401CU) /**< \brief (SSC) Transmit Frame Mode Register */
#define REG_SSC_RHR           (0x40004020U) /**< \brief (SSC) Receive Holding Register */
#define REG_SSC_THR           (0x40004024U) /**< \brief (SSC) Transmit Holding Register */
#define REG_SSC_RSHR          (0x40004030U) /**< \brief (SSC) Receive Sync. Holding Register */
#define REG_SSC_TSHR          (0x40004034U) /**< \brief (SSC) Transmit Sync. Holding Register */
#define REG_SSC_RC0R          (0x40004038U) /**< \brief (SSC) Receive Compare 0 Register */
#define REG_SSC_RC1R          (0x4000403CU) /**< \brief (SSC) Receive Compare 1 Register */
#define REG_SSC_SR            (0x40004040U) /**< \brief (SSC) Status Register */
#define REG_SSC_IER           (0x40004044U) /**< \brief (SSC) Interrupt Enable Register */
#define REG_SSC_IDR           (0x40004048U) /**< \brief (SSC) Interrupt Disable Register */
#define REG_SSC_IMR           (0x4000404CU) /**< \brief (SSC) Interrupt Mask Register */
#define REG_SSC_WPMR          (0x400040E4U) /**< \brief (SSC) Write Protect Mode Register */
#define REG_SSC_WPSR          (0x400040E8U) /**< \brief (SSC) Write Protect Status Register */
#define REG_SSC_RPR           (0x40004100U) /**< \brief (SSC) Receive Pointer Register */
#define REG_SSC_RCR           (0x40004104U) /**< \brief (SSC) Receive Counter Register */
#define REG_SSC_TPR           (0x40004108U) /**< \brief (SSC) Transmit Pointer Register */
#define REG_SSC_TCR           (0x4000410CU) /**< \brief (SSC) Transmit Counter Register */
#define REG_SSC_RNPR          (0x40004110U) /**< \brief (SSC) Receive Next Pointer Register */
#define REG_SSC_RNCR          (0x40004114U) /**< \brief (SSC) Receive Next Counter Register */
#define REG_SSC_TNPR          (0x40004118U) /**< \brief (SSC) Transmit Next Pointer Register */
#define REG_SSC_TNCR          (0x4000411CU) /**< \brief (SSC) Transmit Next Counter Register */
#define REG_SSC_PTCR          (0x40004120U) /**< \brief (SSC) Transfer Control Register */
#define REG_SSC_PTSR          (0x40004124U) /**< \brief (SSC) Transfer Status Register */
#else
#define REG_SSC_CR   (*(WoReg*)0x40004000U) /**< \brief (SSC) Control Register */
#define REG_SSC_CMR  (*(RwReg*)0x40004004U) /**< \brief (SSC) Clock Mode Register */
#define REG_SSC_RCMR (*(RwReg*)0x40004010U) /**< \brief (SSC) Receive Clock Mode Register */
#define REG_SSC_RFMR (*(RwReg*)0x40004014U) /**< \brief (SSC) Receive Frame Mode Register */
#define REG_SSC_TCMR (*(RwReg*)0x40004018U) /**< \brief (SSC) Transmit Clock Mode Register */
#define REG_SSC_TFMR (*(RwReg*)0x4000401CU) /**< \brief (SSC) Transmit Frame Mode Register */
#define REG_SSC_RHR  (*(RoReg*)0x40004020U) /**< \brief (SSC) Receive Holding Register */
#define REG_SSC_THR  (*(WoReg*)0x40004024U) /**< \brief (SSC) Transmit Holding Register */
#define REG_SSC_RSHR (*(RoReg*)0x40004030U) /**< \brief (SSC) Receive Sync. Holding Register */
#define REG_SSC_TSHR (*(RwReg*)0x40004034U) /**< \brief (SSC) Transmit Sync. Holding Register */
#define REG_SSC_RC0R (*(RwReg*)0x40004038U) /**< \brief (SSC) Receive Compare 0 Register */
#define REG_SSC_RC1R (*(RwReg*)0x4000403CU) /**< \brief (SSC) Receive Compare 1 Register */
#define REG_SSC_SR   (*(RoReg*)0x40004040U) /**< \brief (SSC) Status Register */
#define REG_SSC_IER  (*(WoReg*)0x40004044U) /**< \brief (SSC) Interrupt Enable Register */
#define REG_SSC_IDR  (*(WoReg*)0x40004048U) /**< \brief (SSC) Interrupt Disable Register */
#define REG_SSC_IMR  (*(RoReg*)0x4000404CU) /**< \brief (SSC) Interrupt Mask Register */
#define REG_SSC_WPMR (*(RwReg*)0x400040E4U) /**< \brief (SSC) Write Protect Mode Register */
#define REG_SSC_WPSR (*(RoReg*)0x400040E8U) /**< \brief (SSC) Write Protect Status Register */
#define REG_SSC_RPR  (*(RwReg*)0x40004100U) /**< \brief (SSC) Receive Pointer Register */
#define REG_SSC_RCR  (*(RwReg*)0x40004104U) /**< \brief (SSC) Receive Counter Register */
#define REG_SSC_TPR  (*(RwReg*)0x40004108U) /**< \brief (SSC) Transmit Pointer Register */
#define REG_SSC_TCR  (*(RwReg*)0x4000410CU) /**< \brief (SSC) Transmit Counter Register */
#define REG_SSC_RNPR (*(RwReg*)0x40004110U) /**< \brief (SSC) Receive Next Pointer Register */
#define REG_SSC_RNCR (*(RwReg*)0x40004114U) /**< \brief (SSC) Receive Next Counter Register */
#define REG_SSC_TNPR (*(RwReg*)0x40004118U) /**< \brief (SSC) Transmit Next Pointer Register */
#define REG_SSC_TNCR (*(RwReg*)0x4000411CU) /**< \brief (SSC) Transmit Next Counter Register */
#define REG_SSC_PTCR (*(WoReg*)0x40004120U) /**< \brief (SSC) Transfer Control Register */
#define REG_SSC_PTSR (*(RoReg*)0x40004124U) /**< \brief (SSC) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3S8_SSC_INSTANCE_ */
