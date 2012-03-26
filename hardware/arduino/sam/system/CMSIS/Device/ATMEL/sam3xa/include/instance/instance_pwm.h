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

#ifndef _SAM3XA_PWM_INSTANCE_
#define _SAM3XA_PWM_INSTANCE_

/* ========== Register definition for PWM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PWM_CLK               (0x40094000U) /**< \brief (PWM) PWM Clock Register */
#define REG_PWM_ENA               (0x40094004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS               (0x40094008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR                (0x4009400CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER1              (0x40094010U) /**< \brief (PWM) PWM Interrupt Enable Register 1 */
#define REG_PWM_IDR1              (0x40094014U) /**< \brief (PWM) PWM Interrupt Disable Register 1 */
#define REG_PWM_IMR1              (0x40094018U) /**< \brief (PWM) PWM Interrupt Mask Register 1 */
#define REG_PWM_ISR1              (0x4009401CU) /**< \brief (PWM) PWM Interrupt Status Register 1 */
#define REG_PWM_SCM               (0x40094020U) /**< \brief (PWM) PWM Sync Channels Mode Register */
#define REG_PWM_SCUC              (0x40094028U) /**< \brief (PWM) PWM Sync Channels Update Control Register */
#define REG_PWM_SCUP              (0x4009402CU) /**< \brief (PWM) PWM Sync Channels Update Period Register */
#define REG_PWM_SCUPUPD           (0x40094030U) /**< \brief (PWM) PWM Sync Channels Update Period Update Register */
#define REG_PWM_IER2              (0x40094034U) /**< \brief (PWM) PWM Interrupt Enable Register 2 */
#define REG_PWM_IDR2              (0x40094038U) /**< \brief (PWM) PWM Interrupt Disable Register 2 */
#define REG_PWM_IMR2              (0x4009403CU) /**< \brief (PWM) PWM Interrupt Mask Register 2 */
#define REG_PWM_ISR2              (0x40094040U) /**< \brief (PWM) PWM Interrupt Status Register 2 */
#define REG_PWM_OOV               (0x40094044U) /**< \brief (PWM) PWM Output Override Value Register */
#define REG_PWM_OS                (0x40094048U) /**< \brief (PWM) PWM Output Selection Register */
#define REG_PWM_OSS               (0x4009404CU) /**< \brief (PWM) PWM Output Selection Set Register */
#define REG_PWM_OSC               (0x40094050U) /**< \brief (PWM) PWM Output Selection Clear Register */
#define REG_PWM_OSSUPD            (0x40094054U) /**< \brief (PWM) PWM Output Selection Set Update Register */
#define REG_PWM_OSCUPD            (0x40094058U) /**< \brief (PWM) PWM Output Selection Clear Update Register */
#define REG_PWM_FMR               (0x4009405CU) /**< \brief (PWM) PWM Fault Mode Register */
#define REG_PWM_FSR               (0x40094060U) /**< \brief (PWM) PWM Fault Status Register */
#define REG_PWM_FCR               (0x40094064U) /**< \brief (PWM) PWM Fault Clear Register */
#define REG_PWM_FPV               (0x40094068U) /**< \brief (PWM) PWM Fault Protection Value Register */
#define REG_PWM_FPE1              (0x4009406CU) /**< \brief (PWM) PWM Fault Protection Enable Register 1 */
#define REG_PWM_FPE2              (0x40094070U) /**< \brief (PWM) PWM Fault Protection Enable Register 2 */
#define REG_PWM_ELMR              (0x4009407CU) /**< \brief (PWM) PWM Event Line 0 Mode Register */
#define REG_PWM_SMMR              (0x400940B0U) /**< \brief (PWM) PWM Stepper Motor Mode Register */
#define REG_PWM_WPCR              (0x400940E4U) /**< \brief (PWM) PWM Write Protect Control Register */
#define REG_PWM_WPSR              (0x400940E8U) /**< \brief (PWM) PWM Write Protect Status Register */
#define REG_PWM_TPR               (0x40094108U) /**< \brief (PWM) Transmit Pointer Register */
#define REG_PWM_TCR               (0x4009410CU) /**< \brief (PWM) Transmit Counter Register */
#define REG_PWM_TNPR              (0x40094118U) /**< \brief (PWM) Transmit Next Pointer Register */
#define REG_PWM_TNCR              (0x4009411CU) /**< \brief (PWM) Transmit Next Counter Register */
#define REG_PWM_PTCR              (0x40094120U) /**< \brief (PWM) Transfer Control Register */
#define REG_PWM_PTSR              (0x40094124U) /**< \brief (PWM) Transfer Status Register */
#define REG_PWM_CMPV0             (0x40094130U) /**< \brief (PWM) PWM Comparison 0 Value Register */
#define REG_PWM_CMPVUPD0          (0x40094134U) /**< \brief (PWM) PWM Comparison 0 Value Update Register */
#define REG_PWM_CMPM0             (0x40094138U) /**< \brief (PWM) PWM Comparison 0 Mode Register */
#define REG_PWM_CMPMUPD0          (0x4009413CU) /**< \brief (PWM) PWM Comparison 0 Mode Update Register */
#define REG_PWM_CMPV1             (0x40094140U) /**< \brief (PWM) PWM Comparison 1 Value Register */
#define REG_PWM_CMPVUPD1          (0x40094144U) /**< \brief (PWM) PWM Comparison 1 Value Update Register */
#define REG_PWM_CMPM1             (0x40094148U) /**< \brief (PWM) PWM Comparison 1 Mode Register */
#define REG_PWM_CMPMUPD1          (0x4009414CU) /**< \brief (PWM) PWM Comparison 1 Mode Update Register */
#define REG_PWM_CMPV2             (0x40094150U) /**< \brief (PWM) PWM Comparison 2 Value Register */
#define REG_PWM_CMPVUPD2          (0x40094154U) /**< \brief (PWM) PWM Comparison 2 Value Update Register */
#define REG_PWM_CMPM2             (0x40094158U) /**< \brief (PWM) PWM Comparison 2 Mode Register */
#define REG_PWM_CMPMUPD2          (0x4009415CU) /**< \brief (PWM) PWM Comparison 2 Mode Update Register */
#define REG_PWM_CMPV3             (0x40094160U) /**< \brief (PWM) PWM Comparison 3 Value Register */
#define REG_PWM_CMPVUPD3          (0x40094164U) /**< \brief (PWM) PWM Comparison 3 Value Update Register */
#define REG_PWM_CMPM3             (0x40094168U) /**< \brief (PWM) PWM Comparison 3 Mode Register */
#define REG_PWM_CMPMUPD3          (0x4009416CU) /**< \brief (PWM) PWM Comparison 3 Mode Update Register */
#define REG_PWM_CMPV4             (0x40094170U) /**< \brief (PWM) PWM Comparison 4 Value Register */
#define REG_PWM_CMPVUPD4          (0x40094174U) /**< \brief (PWM) PWM Comparison 4 Value Update Register */
#define REG_PWM_CMPM4             (0x40094178U) /**< \brief (PWM) PWM Comparison 4 Mode Register */
#define REG_PWM_CMPMUPD4          (0x4009417CU) /**< \brief (PWM) PWM Comparison 4 Mode Update Register */
#define REG_PWM_CMPV5             (0x40094180U) /**< \brief (PWM) PWM Comparison 5 Value Register */
#define REG_PWM_CMPVUPD5          (0x40094184U) /**< \brief (PWM) PWM Comparison 5 Value Update Register */
#define REG_PWM_CMPM5             (0x40094188U) /**< \brief (PWM) PWM Comparison 5 Mode Register */
#define REG_PWM_CMPMUPD5          (0x4009418CU) /**< \brief (PWM) PWM Comparison 5 Mode Update Register */
#define REG_PWM_CMPV6             (0x40094190U) /**< \brief (PWM) PWM Comparison 6 Value Register */
#define REG_PWM_CMPVUPD6          (0x40094194U) /**< \brief (PWM) PWM Comparison 6 Value Update Register */
#define REG_PWM_CMPM6             (0x40094198U) /**< \brief (PWM) PWM Comparison 6 Mode Register */
#define REG_PWM_CMPMUPD6          (0x4009419CU) /**< \brief (PWM) PWM Comparison 6 Mode Update Register */
#define REG_PWM_CMPV7             (0x400941A0U) /**< \brief (PWM) PWM Comparison 7 Value Register */
#define REG_PWM_CMPVUPD7          (0x400941A4U) /**< \brief (PWM) PWM Comparison 7 Value Update Register */
#define REG_PWM_CMPM7             (0x400941A8U) /**< \brief (PWM) PWM Comparison 7 Mode Register */
#define REG_PWM_CMPMUPD7          (0x400941ACU) /**< \brief (PWM) PWM Comparison 7 Mode Update Register */
#define REG_PWM_CMR0              (0x40094200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0             (0x40094204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CDTYUPD0          (0x40094208U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 0) */
#define REG_PWM_CPRD0             (0x4009420CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CPRDUPD0          (0x40094210U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 0) */
#define REG_PWM_CCNT0             (0x40094214U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_DT0               (0x40094218U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 0) */
#define REG_PWM_DTUPD0            (0x4009421CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 0) */
#define REG_PWM_CMR1              (0x40094220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1             (0x40094224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CDTYUPD1          (0x40094228U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 1) */
#define REG_PWM_CPRD1             (0x4009422CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CPRDUPD1          (0x40094230U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 1) */
#define REG_PWM_CCNT1             (0x40094234U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_DT1               (0x40094238U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 1) */
#define REG_PWM_DTUPD1            (0x4009423CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 1) */
#define REG_PWM_CMR2              (0x40094240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2             (0x40094244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CDTYUPD2          (0x40094248U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 2) */
#define REG_PWM_CPRD2             (0x4009424CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CPRDUPD2          (0x40094250U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 2) */
#define REG_PWM_CCNT2             (0x40094254U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_DT2               (0x40094258U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 2) */
#define REG_PWM_DTUPD2            (0x4009425CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 2) */
#define REG_PWM_CMR3              (0x40094260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3             (0x40094264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CDTYUPD3          (0x40094268U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 3) */
#define REG_PWM_CPRD3             (0x4009426CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CPRDUPD3          (0x40094270U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 3) */
#define REG_PWM_CCNT3             (0x40094274U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_DT3               (0x40094278U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 3) */
#define REG_PWM_DTUPD3            (0x4009427CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 3) */
#define REG_PWM_CMR4              (0x40094280U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 4) */
#define REG_PWM_CDTY4             (0x40094284U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 4) */
#define REG_PWM_CDTYUPD4          (0x40094288U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 4) */
#define REG_PWM_CPRD4             (0x4009428CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 4) */
#define REG_PWM_CPRDUPD4          (0x40094290U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 4) */
#define REG_PWM_CCNT4             (0x40094294U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 4) */
#define REG_PWM_DT4               (0x40094298U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 4) */
#define REG_PWM_DTUPD4            (0x4009429CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 4) */
#define REG_PWM_CMR5              (0x400942A0U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 5) */
#define REG_PWM_CDTY5             (0x400942A4U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 5) */
#define REG_PWM_CDTYUPD5          (0x400942A8U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 5) */
#define REG_PWM_CPRD5             (0x400942ACU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 5) */
#define REG_PWM_CPRDUPD5          (0x400942B0U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 5) */
#define REG_PWM_CCNT5             (0x400942B4U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 5) */
#define REG_PWM_DT5               (0x400942B8U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 5) */
#define REG_PWM_DTUPD5            (0x400942BCU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 5) */
#define REG_PWM_CMR6              (0x400942C0U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 6) */
#define REG_PWM_CDTY6             (0x400942C4U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 6) */
#define REG_PWM_CDTYUPD6          (0x400942C8U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 6) */
#define REG_PWM_CPRD6             (0x400942CCU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 6) */
#define REG_PWM_CPRDUPD6          (0x400942D0U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 6) */
#define REG_PWM_CCNT6             (0x400942D4U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 6) */
#define REG_PWM_DT6               (0x400942D8U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 6) */
#define REG_PWM_DTUPD6            (0x400942DCU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 6) */
#define REG_PWM_CMR7              (0x400942E0U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 7) */
#define REG_PWM_CDTY7             (0x400942E4U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 7) */
#define REG_PWM_CDTYUPD7          (0x400942E8U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 7) */
#define REG_PWM_CPRD7             (0x400942ECU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 7) */
#define REG_PWM_CPRDUPD7          (0x400942F0U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 7) */
#define REG_PWM_CCNT7             (0x400942F4U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 7) */
#define REG_PWM_DT7               (0x400942F8U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 7) */
#define REG_PWM_DTUPD7            (0x400942FCU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 7) */
#else
#define REG_PWM_CLK      (*(RwReg*)0x40094000U) /**< \brief (PWM) PWM Clock Register */
#define REG_PWM_ENA      (*(WoReg*)0x40094004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS      (*(WoReg*)0x40094008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR       (*(RoReg*)0x4009400CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER1     (*(WoReg*)0x40094010U) /**< \brief (PWM) PWM Interrupt Enable Register 1 */
#define REG_PWM_IDR1     (*(WoReg*)0x40094014U) /**< \brief (PWM) PWM Interrupt Disable Register 1 */
#define REG_PWM_IMR1     (*(RoReg*)0x40094018U) /**< \brief (PWM) PWM Interrupt Mask Register 1 */
#define REG_PWM_ISR1     (*(RoReg*)0x4009401CU) /**< \brief (PWM) PWM Interrupt Status Register 1 */
#define REG_PWM_SCM      (*(RwReg*)0x40094020U) /**< \brief (PWM) PWM Sync Channels Mode Register */
#define REG_PWM_SCUC     (*(RwReg*)0x40094028U) /**< \brief (PWM) PWM Sync Channels Update Control Register */
#define REG_PWM_SCUP     (*(RwReg*)0x4009402CU) /**< \brief (PWM) PWM Sync Channels Update Period Register */
#define REG_PWM_SCUPUPD  (*(WoReg*)0x40094030U) /**< \brief (PWM) PWM Sync Channels Update Period Update Register */
#define REG_PWM_IER2     (*(WoReg*)0x40094034U) /**< \brief (PWM) PWM Interrupt Enable Register 2 */
#define REG_PWM_IDR2     (*(WoReg*)0x40094038U) /**< \brief (PWM) PWM Interrupt Disable Register 2 */
#define REG_PWM_IMR2     (*(RoReg*)0x4009403CU) /**< \brief (PWM) PWM Interrupt Mask Register 2 */
#define REG_PWM_ISR2     (*(RoReg*)0x40094040U) /**< \brief (PWM) PWM Interrupt Status Register 2 */
#define REG_PWM_OOV      (*(RwReg*)0x40094044U) /**< \brief (PWM) PWM Output Override Value Register */
#define REG_PWM_OS       (*(RwReg*)0x40094048U) /**< \brief (PWM) PWM Output Selection Register */
#define REG_PWM_OSS      (*(WoReg*)0x4009404CU) /**< \brief (PWM) PWM Output Selection Set Register */
#define REG_PWM_OSC      (*(WoReg*)0x40094050U) /**< \brief (PWM) PWM Output Selection Clear Register */
#define REG_PWM_OSSUPD   (*(WoReg*)0x40094054U) /**< \brief (PWM) PWM Output Selection Set Update Register */
#define REG_PWM_OSCUPD   (*(WoReg*)0x40094058U) /**< \brief (PWM) PWM Output Selection Clear Update Register */
#define REG_PWM_FMR      (*(RwReg*)0x4009405CU) /**< \brief (PWM) PWM Fault Mode Register */
#define REG_PWM_FSR      (*(RoReg*)0x40094060U) /**< \brief (PWM) PWM Fault Status Register */
#define REG_PWM_FCR      (*(WoReg*)0x40094064U) /**< \brief (PWM) PWM Fault Clear Register */
#define REG_PWM_FPV      (*(RwReg*)0x40094068U) /**< \brief (PWM) PWM Fault Protection Value Register */
#define REG_PWM_FPE1     (*(RwReg*)0x4009406CU) /**< \brief (PWM) PWM Fault Protection Enable Register 1 */
#define REG_PWM_FPE2     (*(RwReg*)0x40094070U) /**< \brief (PWM) PWM Fault Protection Enable Register 2 */
#define REG_PWM_ELMR     (*(RwReg*)0x4009407CU) /**< \brief (PWM) PWM Event Line 0 Mode Register */
#define REG_PWM_SMMR     (*(RwReg*)0x400940B0U) /**< \brief (PWM) PWM Stepper Motor Mode Register */
#define REG_PWM_WPCR     (*(WoReg*)0x400940E4U) /**< \brief (PWM) PWM Write Protect Control Register */
#define REG_PWM_WPSR     (*(RoReg*)0x400940E8U) /**< \brief (PWM) PWM Write Protect Status Register */
#define REG_PWM_TPR      (*(RwReg*)0x40094108U) /**< \brief (PWM) Transmit Pointer Register */
#define REG_PWM_TCR      (*(RwReg*)0x4009410CU) /**< \brief (PWM) Transmit Counter Register */
#define REG_PWM_TNPR     (*(RwReg*)0x40094118U) /**< \brief (PWM) Transmit Next Pointer Register */
#define REG_PWM_TNCR     (*(RwReg*)0x4009411CU) /**< \brief (PWM) Transmit Next Counter Register */
#define REG_PWM_PTCR     (*(WoReg*)0x40094120U) /**< \brief (PWM) Transfer Control Register */
#define REG_PWM_PTSR     (*(RoReg*)0x40094124U) /**< \brief (PWM) Transfer Status Register */
#define REG_PWM_CMPV0    (*(RwReg*)0x40094130U) /**< \brief (PWM) PWM Comparison 0 Value Register */
#define REG_PWM_CMPVUPD0 (*(WoReg*)0x40094134U) /**< \brief (PWM) PWM Comparison 0 Value Update Register */
#define REG_PWM_CMPM0    (*(RwReg*)0x40094138U) /**< \brief (PWM) PWM Comparison 0 Mode Register */
#define REG_PWM_CMPMUPD0 (*(WoReg*)0x4009413CU) /**< \brief (PWM) PWM Comparison 0 Mode Update Register */
#define REG_PWM_CMPV1    (*(RwReg*)0x40094140U) /**< \brief (PWM) PWM Comparison 1 Value Register */
#define REG_PWM_CMPVUPD1 (*(WoReg*)0x40094144U) /**< \brief (PWM) PWM Comparison 1 Value Update Register */
#define REG_PWM_CMPM1    (*(RwReg*)0x40094148U) /**< \brief (PWM) PWM Comparison 1 Mode Register */
#define REG_PWM_CMPMUPD1 (*(WoReg*)0x4009414CU) /**< \brief (PWM) PWM Comparison 1 Mode Update Register */
#define REG_PWM_CMPV2    (*(RwReg*)0x40094150U) /**< \brief (PWM) PWM Comparison 2 Value Register */
#define REG_PWM_CMPVUPD2 (*(WoReg*)0x40094154U) /**< \brief (PWM) PWM Comparison 2 Value Update Register */
#define REG_PWM_CMPM2    (*(RwReg*)0x40094158U) /**< \brief (PWM) PWM Comparison 2 Mode Register */
#define REG_PWM_CMPMUPD2 (*(WoReg*)0x4009415CU) /**< \brief (PWM) PWM Comparison 2 Mode Update Register */
#define REG_PWM_CMPV3    (*(RwReg*)0x40094160U) /**< \brief (PWM) PWM Comparison 3 Value Register */
#define REG_PWM_CMPVUPD3 (*(WoReg*)0x40094164U) /**< \brief (PWM) PWM Comparison 3 Value Update Register */
#define REG_PWM_CMPM3    (*(RwReg*)0x40094168U) /**< \brief (PWM) PWM Comparison 3 Mode Register */
#define REG_PWM_CMPMUPD3 (*(WoReg*)0x4009416CU) /**< \brief (PWM) PWM Comparison 3 Mode Update Register */
#define REG_PWM_CMPV4    (*(RwReg*)0x40094170U) /**< \brief (PWM) PWM Comparison 4 Value Register */
#define REG_PWM_CMPVUPD4 (*(WoReg*)0x40094174U) /**< \brief (PWM) PWM Comparison 4 Value Update Register */
#define REG_PWM_CMPM4    (*(RwReg*)0x40094178U) /**< \brief (PWM) PWM Comparison 4 Mode Register */
#define REG_PWM_CMPMUPD4 (*(WoReg*)0x4009417CU) /**< \brief (PWM) PWM Comparison 4 Mode Update Register */
#define REG_PWM_CMPV5    (*(RwReg*)0x40094180U) /**< \brief (PWM) PWM Comparison 5 Value Register */
#define REG_PWM_CMPVUPD5 (*(WoReg*)0x40094184U) /**< \brief (PWM) PWM Comparison 5 Value Update Register */
#define REG_PWM_CMPM5    (*(RwReg*)0x40094188U) /**< \brief (PWM) PWM Comparison 5 Mode Register */
#define REG_PWM_CMPMUPD5 (*(WoReg*)0x4009418CU) /**< \brief (PWM) PWM Comparison 5 Mode Update Register */
#define REG_PWM_CMPV6    (*(RwReg*)0x40094190U) /**< \brief (PWM) PWM Comparison 6 Value Register */
#define REG_PWM_CMPVUPD6 (*(WoReg*)0x40094194U) /**< \brief (PWM) PWM Comparison 6 Value Update Register */
#define REG_PWM_CMPM6    (*(RwReg*)0x40094198U) /**< \brief (PWM) PWM Comparison 6 Mode Register */
#define REG_PWM_CMPMUPD6 (*(WoReg*)0x4009419CU) /**< \brief (PWM) PWM Comparison 6 Mode Update Register */
#define REG_PWM_CMPV7    (*(RwReg*)0x400941A0U) /**< \brief (PWM) PWM Comparison 7 Value Register */
#define REG_PWM_CMPVUPD7 (*(WoReg*)0x400941A4U) /**< \brief (PWM) PWM Comparison 7 Value Update Register */
#define REG_PWM_CMPM7    (*(RwReg*)0x400941A8U) /**< \brief (PWM) PWM Comparison 7 Mode Register */
#define REG_PWM_CMPMUPD7 (*(WoReg*)0x400941ACU) /**< \brief (PWM) PWM Comparison 7 Mode Update Register */
#define REG_PWM_CMR0     (*(RwReg*)0x40094200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0    (*(RwReg*)0x40094204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CDTYUPD0 (*(WoReg*)0x40094208U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 0) */
#define REG_PWM_CPRD0    (*(RwReg*)0x4009420CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CPRDUPD0 (*(WoReg*)0x40094210U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 0) */
#define REG_PWM_CCNT0    (*(RoReg*)0x40094214U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_DT0      (*(RwReg*)0x40094218U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 0) */
#define REG_PWM_DTUPD0   (*(WoReg*)0x4009421CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 0) */
#define REG_PWM_CMR1     (*(RwReg*)0x40094220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1    (*(RwReg*)0x40094224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CDTYUPD1 (*(WoReg*)0x40094228U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 1) */
#define REG_PWM_CPRD1    (*(RwReg*)0x4009422CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CPRDUPD1 (*(WoReg*)0x40094230U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 1) */
#define REG_PWM_CCNT1    (*(RoReg*)0x40094234U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_DT1      (*(RwReg*)0x40094238U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 1) */
#define REG_PWM_DTUPD1   (*(WoReg*)0x4009423CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 1) */
#define REG_PWM_CMR2     (*(RwReg*)0x40094240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2    (*(RwReg*)0x40094244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CDTYUPD2 (*(WoReg*)0x40094248U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 2) */
#define REG_PWM_CPRD2    (*(RwReg*)0x4009424CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CPRDUPD2 (*(WoReg*)0x40094250U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 2) */
#define REG_PWM_CCNT2    (*(RoReg*)0x40094254U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_DT2      (*(RwReg*)0x40094258U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 2) */
#define REG_PWM_DTUPD2   (*(WoReg*)0x4009425CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 2) */
#define REG_PWM_CMR3     (*(RwReg*)0x40094260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3    (*(RwReg*)0x40094264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CDTYUPD3 (*(WoReg*)0x40094268U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 3) */
#define REG_PWM_CPRD3    (*(RwReg*)0x4009426CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CPRDUPD3 (*(WoReg*)0x40094270U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 3) */
#define REG_PWM_CCNT3    (*(RoReg*)0x40094274U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_DT3      (*(RwReg*)0x40094278U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 3) */
#define REG_PWM_DTUPD3   (*(WoReg*)0x4009427CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 3) */
#define REG_PWM_CMR4     (*(RwReg*)0x40094280U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 4) */
#define REG_PWM_CDTY4    (*(RwReg*)0x40094284U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 4) */
#define REG_PWM_CDTYUPD4 (*(WoReg*)0x40094288U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 4) */
#define REG_PWM_CPRD4    (*(RwReg*)0x4009428CU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 4) */
#define REG_PWM_CPRDUPD4 (*(WoReg*)0x40094290U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 4) */
#define REG_PWM_CCNT4    (*(RoReg*)0x40094294U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 4) */
#define REG_PWM_DT4      (*(RwReg*)0x40094298U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 4) */
#define REG_PWM_DTUPD4   (*(WoReg*)0x4009429CU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 4) */
#define REG_PWM_CMR5     (*(RwReg*)0x400942A0U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 5) */
#define REG_PWM_CDTY5    (*(RwReg*)0x400942A4U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 5) */
#define REG_PWM_CDTYUPD5 (*(WoReg*)0x400942A8U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 5) */
#define REG_PWM_CPRD5    (*(RwReg*)0x400942ACU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 5) */
#define REG_PWM_CPRDUPD5 (*(WoReg*)0x400942B0U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 5) */
#define REG_PWM_CCNT5    (*(RoReg*)0x400942B4U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 5) */
#define REG_PWM_DT5      (*(RwReg*)0x400942B8U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 5) */
#define REG_PWM_DTUPD5   (*(WoReg*)0x400942BCU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 5) */
#define REG_PWM_CMR6     (*(RwReg*)0x400942C0U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 6) */
#define REG_PWM_CDTY6    (*(RwReg*)0x400942C4U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 6) */
#define REG_PWM_CDTYUPD6 (*(WoReg*)0x400942C8U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 6) */
#define REG_PWM_CPRD6    (*(RwReg*)0x400942CCU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 6) */
#define REG_PWM_CPRDUPD6 (*(WoReg*)0x400942D0U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 6) */
#define REG_PWM_CCNT6    (*(RoReg*)0x400942D4U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 6) */
#define REG_PWM_DT6      (*(RwReg*)0x400942D8U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 6) */
#define REG_PWM_DTUPD6   (*(WoReg*)0x400942DCU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 6) */
#define REG_PWM_CMR7     (*(RwReg*)0x400942E0U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 7) */
#define REG_PWM_CDTY7    (*(RwReg*)0x400942E4U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 7) */
#define REG_PWM_CDTYUPD7 (*(WoReg*)0x400942E8U) /**< \brief (PWM) PWM Channel Duty Cycle Update Register (ch_num = 7) */
#define REG_PWM_CPRD7    (*(RwReg*)0x400942ECU) /**< \brief (PWM) PWM Channel Period Register (ch_num = 7) */
#define REG_PWM_CPRDUPD7 (*(WoReg*)0x400942F0U) /**< \brief (PWM) PWM Channel Period Update Register (ch_num = 7) */
#define REG_PWM_CCNT7    (*(RoReg*)0x400942F4U) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 7) */
#define REG_PWM_DT7      (*(RwReg*)0x400942F8U) /**< \brief (PWM) PWM Channel Dead Time Register (ch_num = 7) */
#define REG_PWM_DTUPD7   (*(WoReg*)0x400942FCU) /**< \brief (PWM) PWM Channel Dead Time Update Register (ch_num = 7) */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3XA_PWM_INSTANCE_ */
