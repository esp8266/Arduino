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

#ifndef _SAM3U_PWM_INSTANCE_
#define _SAM3U_PWM_INSTANCE_

/* ========== Register definition for PWM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PWM_CLK               (0x4008C000U) /**< \brief (PWM) PWM Clock Register */
#define REG_PWM_ENA               (0x4008C004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS               (0x4008C008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR                (0x4008C00CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER1              (0x4008C010U) /**< \brief (PWM) PWM Interrupt Enable Register 1 */
#define REG_PWM_IDR1              (0x4008C014U) /**< \brief (PWM) PWM Interrupt Disable Register 1 */
#define REG_PWM_IMR1              (0x4008C018U) /**< \brief (PWM) PWM Interrupt Mask Register 1 */
#define REG_PWM_ISR1              (0x4008C01CU) /**< \brief (PWM) PWM Interrupt Status Register 1 */
#define REG_PWM_SCM               (0x4008C020U) /**< \brief (PWM) PWM Sync Channels Mode Register */
#define REG_PWM_SCUC              (0x4008C028U) /**< \brief (PWM) PWM Sync Channels Update Control Register */
#define REG_PWM_SCUP              (0x4008C02CU) /**< \brief (PWM) PWM Sync Channels Update Period Register */
#define REG_PWM_SCUPUPD           (0x4008C030U) /**< \brief (PWM) PWM Sync Channels Update Period Update Register */
#define REG_PWM_IER2              (0x4008C034U) /**< \brief (PWM) PWM Interrupt Enable Register 2 */
#define REG_PWM_IDR2              (0x4008C038U) /**< \brief (PWM) PWM Interrupt Disable Register 2 */
#define REG_PWM_IMR2              (0x4008C03CU) /**< \brief (PWM) PWM Interrupt Mask Register 2 */
#define REG_PWM_ISR2              (0x4008C040U) /**< \brief (PWM) PWM Interrupt Status Register 2 */
#define REG_PWM_OOV               (0x4008C044U) /**< \brief (PWM) PWM Output Override Value Register */
#define REG_PWM_OS                (0x4008C048U) /**< \brief (PWM) PWM Output Selection Register */
#define REG_PWM_OSS               (0x4008C04CU) /**< \brief (PWM) PWM Output Selection Set Register */
#define REG_PWM_OSC               (0x4008C050U) /**< \brief (PWM) PWM Output Selection Clear Register */
#define REG_PWM_OSSUPD            (0x4008C054U) /**< \brief (PWM) PWM Output Selection Set Update Register */
#define REG_PWM_OSCUPD            (0x4008C058U) /**< \brief (PWM) PWM Output Selection Clear Update Register */
#define REG_PWM_FMR               (0x4008C05CU) /**< \brief (PWM) PWM Fault Mode Register */
#define REG_PWM_FSR               (0x4008C060U) /**< \brief (PWM) PWM Fault Status Register */
#define REG_PWM_FCR               (0x4008C064U) /**< \brief (PWM) PWM Fault Clear Register */
#define REG_PWM_FPV               (0x4008C068U) /**< \brief (PWM) PWM Fault Protection Value Register */
#define REG_PWM_FPE               (0x4008C06CU) /**< \brief (PWM) PWM Fault Protection Enable Register */
#define REG_PWM_ELMR              (0x4008C07CU) /**< \brief (PWM) PWM Event Line 0 Mode Register */
#define REG_PWM_WPCR              (0x4008C0E4U) /**< \brief (PWM) PWM Write Protect Control Register */
#define REG_PWM_WPSR              (0x4008C0E8U) /**< \brief (PWM) PWM Write Protect Status Register */
#define REG_PWM_TPR               (0x4008C108U) /**< \brief (PWM) Transmit Pointer Register */
#define REG_PWM_TCR               (0x4008C10CU) /**< \brief (PWM) Transmit Counter Register */
#define REG_PWM_TNPR              (0x4008C118U) /**< \brief (PWM) Transmit Next Pointer Register */
#define REG_PWM_TNCR              (0x4008C11CU) /**< \brief (PWM) Transmit Next Counter Register */
#define REG_PWM_PTCR              (0x4008C120U) /**< \brief (PWM) Transfer Control Register */
#define REG_PWM_PTSR              (0x4008C124U) /**< \brief (PWM) Transfer Status Register */
#define REG_PWM_CMPV0             (0x4008C130U) /**< \brief (PWM) PWM Comparison 0 Value Register */
#define REG_PWM_CMPVUPD0          (0x4008C134U) /**< \brief (PWM) PWM Comparison 0 Value Update Register */
#define REG_PWM_CMPM0             (0x4008C138U) /**< \brief (PWM) PWM Comparison 0 Mode Register */
#define REG_PWM_CMPMUPD0          (0x4008C13CU) /**< \brief (PWM) PWM Comparison 0 Mode Update Register */
#define REG_PWM_CMPV1             (0x4008C140U) /**< \brief (PWM) PWM Comparison 1 Value Register */
#define REG_PWM_CMPVUPD1          (0x4008C144U) /**< \brief (PWM) PWM Comparison 1 Value Update Register */
#define REG_PWM_CMPM1             (0x4008C148U) /**< \brief (PWM) PWM Comparison 1 Mode Register */
#define REG_PWM_CMPMUPD1          (0x4008C14CU) /**< \brief (PWM) PWM Comparison 1 Mode Update Register */
#define REG_PWM_CMPV2             (0x4008C150U) /**< \brief (PWM) PWM Comparison 2 Value Register */
#define REG_PWM_CMPVUPD2          (0x4008C154U) /**< \brief (PWM) PWM Comparison 2 Value Update Register */
#define REG_PWM_CMPM2             (0x4008C158U) /**< \brief (PWM) PWM Comparison 2 Mode Register */
#define REG_PWM_CMPMUPD2          (0x4008C15CU) /**< \brief (PWM) PWM Comparison 2 Mode Update Register */
#define REG_PWM_CMPV3             (0x4008C160U) /**< \brief (PWM) PWM Comparison 3 Value Register */
#define REG_PWM_CMPVUPD3          (0x4008C164U) /**< \brief (PWM) PWM Comparison 3 Value Update Register */
#define REG_PWM_CMPM3             (0x4008C168U) /**< \brief (PWM) PWM Comparison 3 Mode Register */
#define REG_PWM_CMPMUPD3          (0x4008C16CU) /**< \brief (PWM) PWM Comparison 3 Mode Update Register */
#define REG_PWM_CMPV4             (0x4008C170U) /**< \brief (PWM) PWM Comparison 4 Value Register */
#define REG_PWM_CMPVUPD4          (0x4008C174U) /**< \brief (PWM) PWM Comparison 4 Value Update Register */
#define REG_PWM_CMPM4             (0x4008C178U) /**< \brief (PWM) PWM Comparison 4 Mode Register */
#define REG_PWM_CMPMUPD4          (0x4008C17CU) /**< \brief (PWM) PWM Comparison 4 Mode Update Register */
#define REG_PWM_CMPV5             (0x4008C180U) /**< \brief (PWM) PWM Comparison 5 Value Register */
#define REG_PWM_CMPVUPD5          (0x4008C184U) /**< \brief (PWM) PWM Comparison 5 Value Update Register */
#define REG_PWM_CMPM5             (0x4008C188U) /**< \brief (PWM) PWM Comparison 5 Mode Register */
#define REG_PWM_CMPMUPD5          (0x4008C18CU) /**< \brief (PWM) PWM Comparison 5 Mode Update Register */
#define REG_PWM_CMPV6             (0x4008C190U) /**< \brief (PWM) PWM Comparison 6 Value Register */
#define REG_PWM_CMPVUPD6          (0x4008C194U) /**< \brief (PWM) PWM Comparison 6 Value Update Register */
#define REG_PWM_CMPM6             (0x4008C198U) /**< \brief (PWM) PWM Comparison 6 Mode Register */
#define REG_PWM_CMPMUPD6          (0x4008C19CU) /**< \brief (PWM) PWM Comparison 6 Mode Update Register */
#define REG_PWM_CMPV7             (0x4008C1A0U) /**< \brief (PWM) PWM Comparison 7 Value Register */
#define REG_PWM_CMPVUPD7          (0x4008C1A4U) /**< \brief (PWM) PWM Comparison 7 Value Update Register */
#define REG_PWM_CMPM7             (0x4008C1A8U) /**< \brief (PWM) PWM Comparison 7 Mode Register */
#define REG_PWM_CMPMUPD7          (0x4008C1ACU) /**< \brief (PWM) PWM Comparison 7 Mode Update Register */
#define REG_PWM_CMR0              (0x4008C200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0             (0x4008C204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CDTYUPD0          (0x4008C208U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 0) */
#define REG_PWM_CPRD0             (0x4008C20CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CPRDUPD0          (0x4008C210U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 0) */
#define REG_PWM_CCNT0             (0x4008C214U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_DT0               (0x4008C218U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 0) */
#define REG_PWM_DTUPD0            (0x4008C21CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 0) */
#define REG_PWM_CMR1              (0x4008C220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1             (0x4008C224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CDTYUPD1          (0x4008C228U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 1) */
#define REG_PWM_CPRD1             (0x4008C22CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CPRDUPD1          (0x4008C230U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 1) */
#define REG_PWM_CCNT1             (0x4008C234U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_DT1               (0x4008C238U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 1) */
#define REG_PWM_DTUPD1            (0x4008C23CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 1) */
#define REG_PWM_CMR2              (0x4008C240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2             (0x4008C244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CDTYUPD2          (0x4008C248U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 2) */
#define REG_PWM_CPRD2             (0x4008C24CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CPRDUPD2          (0x4008C250U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 2) */
#define REG_PWM_CCNT2             (0x4008C254U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_DT2               (0x4008C258U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 2) */
#define REG_PWM_DTUPD2            (0x4008C25CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 2) */
#define REG_PWM_CMR3              (0x4008C260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3             (0x4008C264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CDTYUPD3          (0x4008C268U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 3) */
#define REG_PWM_CPRD3             (0x4008C26CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CPRDUPD3          (0x4008C270U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 3) */
#define REG_PWM_CCNT3             (0x4008C274U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_DT3               (0x4008C278U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 3) */
#define REG_PWM_DTUPD3            (0x4008C27CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 3) */
#else
#define REG_PWM_CLK      (*(RwReg*)0x4008C000U) /**< \brief (PWM) PWM Clock Register */
#define REG_PWM_ENA      (*(WoReg*)0x4008C004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS      (*(WoReg*)0x4008C008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR       (*(RoReg*)0x4008C00CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER1     (*(WoReg*)0x4008C010U) /**< \brief (PWM) PWM Interrupt Enable Register 1 */
#define REG_PWM_IDR1     (*(WoReg*)0x4008C014U) /**< \brief (PWM) PWM Interrupt Disable Register 1 */
#define REG_PWM_IMR1     (*(RoReg*)0x4008C018U) /**< \brief (PWM) PWM Interrupt Mask Register 1 */
#define REG_PWM_ISR1     (*(RoReg*)0x4008C01CU) /**< \brief (PWM) PWM Interrupt Status Register 1 */
#define REG_PWM_SCM      (*(RwReg*)0x4008C020U) /**< \brief (PWM) PWM Sync Channels Mode Register */
#define REG_PWM_SCUC     (*(RwReg*)0x4008C028U) /**< \brief (PWM) PWM Sync Channels Update Control Register */
#define REG_PWM_SCUP     (*(RwReg*)0x4008C02CU) /**< \brief (PWM) PWM Sync Channels Update Period Register */
#define REG_PWM_SCUPUPD  (*(WoReg*)0x4008C030U) /**< \brief (PWM) PWM Sync Channels Update Period Update Register */
#define REG_PWM_IER2     (*(WoReg*)0x4008C034U) /**< \brief (PWM) PWM Interrupt Enable Register 2 */
#define REG_PWM_IDR2     (*(WoReg*)0x4008C038U) /**< \brief (PWM) PWM Interrupt Disable Register 2 */
#define REG_PWM_IMR2     (*(RoReg*)0x4008C03CU) /**< \brief (PWM) PWM Interrupt Mask Register 2 */
#define REG_PWM_ISR2     (*(RoReg*)0x4008C040U) /**< \brief (PWM) PWM Interrupt Status Register 2 */
#define REG_PWM_OOV      (*(RwReg*)0x4008C044U) /**< \brief (PWM) PWM Output Override Value Register */
#define REG_PWM_OS       (*(RwReg*)0x4008C048U) /**< \brief (PWM) PWM Output Selection Register */
#define REG_PWM_OSS      (*(WoReg*)0x4008C04CU) /**< \brief (PWM) PWM Output Selection Set Register */
#define REG_PWM_OSC      (*(WoReg*)0x4008C050U) /**< \brief (PWM) PWM Output Selection Clear Register */
#define REG_PWM_OSSUPD   (*(WoReg*)0x4008C054U) /**< \brief (PWM) PWM Output Selection Set Update Register */
#define REG_PWM_OSCUPD   (*(WoReg*)0x4008C058U) /**< \brief (PWM) PWM Output Selection Clear Update Register */
#define REG_PWM_FMR      (*(RwReg*)0x4008C05CU) /**< \brief (PWM) PWM Fault Mode Register */
#define REG_PWM_FSR      (*(RoReg*)0x4008C060U) /**< \brief (PWM) PWM Fault Status Register */
#define REG_PWM_FCR      (*(WoReg*)0x4008C064U) /**< \brief (PWM) PWM Fault Clear Register */
#define REG_PWM_FPV      (*(RwReg*)0x4008C068U) /**< \brief (PWM) PWM Fault Protection Value Register */
#define REG_PWM_FPE      (*(RwReg*)0x4008C06CU) /**< \brief (PWM) PWM Fault Protection Enable Register */
#define REG_PWM_ELMR     (*(RwReg*)0x4008C07CU) /**< \brief (PWM) PWM Event Line 0 Mode Register */
#define REG_PWM_WPCR     (*(WoReg*)0x4008C0E4U) /**< \brief (PWM) PWM Write Protect Control Register */
#define REG_PWM_WPSR     (*(RoReg*)0x4008C0E8U) /**< \brief (PWM) PWM Write Protect Status Register */
#define REG_PWM_TPR      (*(RwReg*)0x4008C108U) /**< \brief (PWM) Transmit Pointer Register */
#define REG_PWM_TCR      (*(RwReg*)0x4008C10CU) /**< \brief (PWM) Transmit Counter Register */
#define REG_PWM_TNPR     (*(RwReg*)0x4008C118U) /**< \brief (PWM) Transmit Next Pointer Register */
#define REG_PWM_TNCR     (*(RwReg*)0x4008C11CU) /**< \brief (PWM) Transmit Next Counter Register */
#define REG_PWM_PTCR     (*(WoReg*)0x4008C120U) /**< \brief (PWM) Transfer Control Register */
#define REG_PWM_PTSR     (*(RoReg*)0x4008C124U) /**< \brief (PWM) Transfer Status Register */
#define REG_PWM_CMPV0    (*(RwReg*)0x4008C130U) /**< \brief (PWM) PWM Comparison 0 Value Register */
#define REG_PWM_CMPVUPD0 (*(WoReg*)0x4008C134U) /**< \brief (PWM) PWM Comparison 0 Value Update Register */
#define REG_PWM_CMPM0    (*(RwReg*)0x4008C138U) /**< \brief (PWM) PWM Comparison 0 Mode Register */
#define REG_PWM_CMPMUPD0 (*(WoReg*)0x4008C13CU) /**< \brief (PWM) PWM Comparison 0 Mode Update Register */
#define REG_PWM_CMPV1    (*(RwReg*)0x4008C140U) /**< \brief (PWM) PWM Comparison 1 Value Register */
#define REG_PWM_CMPVUPD1 (*(WoReg*)0x4008C144U) /**< \brief (PWM) PWM Comparison 1 Value Update Register */
#define REG_PWM_CMPM1    (*(RwReg*)0x4008C148U) /**< \brief (PWM) PWM Comparison 1 Mode Register */
#define REG_PWM_CMPMUPD1 (*(WoReg*)0x4008C14CU) /**< \brief (PWM) PWM Comparison 1 Mode Update Register */
#define REG_PWM_CMPV2    (*(RwReg*)0x4008C150U) /**< \brief (PWM) PWM Comparison 2 Value Register */
#define REG_PWM_CMPVUPD2 (*(WoReg*)0x4008C154U) /**< \brief (PWM) PWM Comparison 2 Value Update Register */
#define REG_PWM_CMPM2    (*(RwReg*)0x4008C158U) /**< \brief (PWM) PWM Comparison 2 Mode Register */
#define REG_PWM_CMPMUPD2 (*(WoReg*)0x4008C15CU) /**< \brief (PWM) PWM Comparison 2 Mode Update Register */
#define REG_PWM_CMPV3    (*(RwReg*)0x4008C160U) /**< \brief (PWM) PWM Comparison 3 Value Register */
#define REG_PWM_CMPVUPD3 (*(WoReg*)0x4008C164U) /**< \brief (PWM) PWM Comparison 3 Value Update Register */
#define REG_PWM_CMPM3    (*(RwReg*)0x4008C168U) /**< \brief (PWM) PWM Comparison 3 Mode Register */
#define REG_PWM_CMPMUPD3 (*(WoReg*)0x4008C16CU) /**< \brief (PWM) PWM Comparison 3 Mode Update Register */
#define REG_PWM_CMPV4    (*(RwReg*)0x4008C170U) /**< \brief (PWM) PWM Comparison 4 Value Register */
#define REG_PWM_CMPVUPD4 (*(WoReg*)0x4008C174U) /**< \brief (PWM) PWM Comparison 4 Value Update Register */
#define REG_PWM_CMPM4    (*(RwReg*)0x4008C178U) /**< \brief (PWM) PWM Comparison 4 Mode Register */
#define REG_PWM_CMPMUPD4 (*(WoReg*)0x4008C17CU) /**< \brief (PWM) PWM Comparison 4 Mode Update Register */
#define REG_PWM_CMPV5    (*(RwReg*)0x4008C180U) /**< \brief (PWM) PWM Comparison 5 Value Register */
#define REG_PWM_CMPVUPD5 (*(WoReg*)0x4008C184U) /**< \brief (PWM) PWM Comparison 5 Value Update Register */
#define REG_PWM_CMPM5    (*(RwReg*)0x4008C188U) /**< \brief (PWM) PWM Comparison 5 Mode Register */
#define REG_PWM_CMPMUPD5 (*(WoReg*)0x4008C18CU) /**< \brief (PWM) PWM Comparison 5 Mode Update Register */
#define REG_PWM_CMPV6    (*(RwReg*)0x4008C190U) /**< \brief (PWM) PWM Comparison 6 Value Register */
#define REG_PWM_CMPVUPD6 (*(WoReg*)0x4008C194U) /**< \brief (PWM) PWM Comparison 6 Value Update Register */
#define REG_PWM_CMPM6    (*(RwReg*)0x4008C198U) /**< \brief (PWM) PWM Comparison 6 Mode Register */
#define REG_PWM_CMPMUPD6 (*(WoReg*)0x4008C19CU) /**< \brief (PWM) PWM Comparison 6 Mode Update Register */
#define REG_PWM_CMPV7    (*(RwReg*)0x4008C1A0U) /**< \brief (PWM) PWM Comparison 7 Value Register */
#define REG_PWM_CMPVUPD7 (*(WoReg*)0x4008C1A4U) /**< \brief (PWM) PWM Comparison 7 Value Update Register */
#define REG_PWM_CMPM7    (*(RwReg*)0x4008C1A8U) /**< \brief (PWM) PWM Comparison 7 Mode Register */
#define REG_PWM_CMPMUPD7 (*(WoReg*)0x4008C1ACU) /**< \brief (PWM) PWM Comparison 7 Mode Update Register */
#define REG_PWM_CMR0     (*(RwReg*)0x4008C200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0    (*(RwReg*)0x4008C204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CDTYUPD0 (*(WoReg*)0x4008C208U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 0) */
#define REG_PWM_CPRD0    (*(RwReg*)0x4008C20CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CPRDUPD0 (*(WoReg*)0x4008C210U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 0) */
#define REG_PWM_CCNT0    (*(RoReg*)0x4008C214U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_DT0      (*(RwReg*)0x4008C218U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 0) */
#define REG_PWM_DTUPD0   (*(WoReg*)0x4008C21CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 0) */
#define REG_PWM_CMR1     (*(RwReg*)0x4008C220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1    (*(RwReg*)0x4008C224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CDTYUPD1 (*(WoReg*)0x4008C228U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 1) */
#define REG_PWM_CPRD1    (*(RwReg*)0x4008C22CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CPRDUPD1 (*(WoReg*)0x4008C230U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 1) */
#define REG_PWM_CCNT1    (*(RoReg*)0x4008C234U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_DT1      (*(RwReg*)0x4008C238U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 1) */
#define REG_PWM_DTUPD1   (*(WoReg*)0x4008C23CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 1) */
#define REG_PWM_CMR2     (*(RwReg*)0x4008C240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2    (*(RwReg*)0x4008C244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CDTYUPD2 (*(WoReg*)0x4008C248U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 2) */
#define REG_PWM_CPRD2    (*(RwReg*)0x4008C24CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CPRDUPD2 (*(WoReg*)0x4008C250U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 2) */
#define REG_PWM_CCNT2    (*(RoReg*)0x4008C254U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_DT2      (*(RwReg*)0x4008C258U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 2) */
#define REG_PWM_DTUPD2   (*(WoReg*)0x4008C25CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 2) */
#define REG_PWM_CMR3     (*(RwReg*)0x4008C260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3    (*(RwReg*)0x4008C264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CDTYUPD3 (*(WoReg*)0x4008C268U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 3) */
#define REG_PWM_CPRD3    (*(RwReg*)0x4008C26CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CPRDUPD3 (*(WoReg*)0x4008C270U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 3) */
#define REG_PWM_CCNT3    (*(RoReg*)0x4008C274U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_DT3      (*(RwReg*)0x4008C278U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 3) */
#define REG_PWM_DTUPD3   (*(WoReg*)0x4008C27CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 3) */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3U_PWM_INSTANCE_ */
