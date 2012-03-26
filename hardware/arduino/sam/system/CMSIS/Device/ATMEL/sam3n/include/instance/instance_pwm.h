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

#ifndef _SAM3N_PWM_INSTANCE_
#define _SAM3N_PWM_INSTANCE_

/* ========== Register definition for PWM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PWM_MR             (0x40020000U) /**< \brief (PWM) PWM Mode Register */
#define REG_PWM_ENA            (0x40020004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS            (0x40020008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR             (0x4002000CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER            (0x40020010U) /**< \brief (PWM) PWM Interrupt Enable Register */
#define REG_PWM_IDR            (0x40020014U) /**< \brief (PWM) PWM Interrupt Disable Register */
#define REG_PWM_IMR            (0x40020018U) /**< \brief (PWM) PWM Interrupt Mask Register */
#define REG_PWM_ISR            (0x4002001CU) /**< \brief (PWM) PWM Interrupt Status Register */
#define REG_PWM_CMR0           (0x40020200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0          (0x40020204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CPRD0          (0x40020208U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CCNT0          (0x4002020CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_CUPD0          (0x40020210U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 0) */
#define REG_PWM_CMR1           (0x40020220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1          (0x40020224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CPRD1          (0x40020228U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CCNT1          (0x4002022CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_CUPD1          (0x40020230U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 1) */
#define REG_PWM_CMR2           (0x40020240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2          (0x40020244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CPRD2          (0x40020248U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CCNT2          (0x4002024CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_CUPD2          (0x40020250U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 2) */
#define REG_PWM_CMR3           (0x40020260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3          (0x40020264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CPRD3          (0x40020268U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CCNT3          (0x4002026CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_CUPD3          (0x40020270U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 3) */
#else
#define REG_PWM_MR    (*(RwReg*)0x40020000U) /**< \brief (PWM) PWM Mode Register */
#define REG_PWM_ENA   (*(WoReg*)0x40020004U) /**< \brief (PWM) PWM Enable Register */
#define REG_PWM_DIS   (*(WoReg*)0x40020008U) /**< \brief (PWM) PWM Disable Register */
#define REG_PWM_SR    (*(RoReg*)0x4002000CU) /**< \brief (PWM) PWM Status Register */
#define REG_PWM_IER   (*(WoReg*)0x40020010U) /**< \brief (PWM) PWM Interrupt Enable Register */
#define REG_PWM_IDR   (*(WoReg*)0x40020014U) /**< \brief (PWM) PWM Interrupt Disable Register */
#define REG_PWM_IMR   (*(RoReg*)0x40020018U) /**< \brief (PWM) PWM Interrupt Mask Register */
#define REG_PWM_ISR   (*(RoReg*)0x4002001CU) /**< \brief (PWM) PWM Interrupt Status Register */
#define REG_PWM_CMR0  (*(RwReg*)0x40020200U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 0) */
#define REG_PWM_CDTY0 (*(RwReg*)0x40020204U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 0) */
#define REG_PWM_CPRD0 (*(RwReg*)0x40020208U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 0) */
#define REG_PWM_CCNT0 (*(RoReg*)0x4002020CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 0) */
#define REG_PWM_CUPD0 (*(WoReg*)0x40020210U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 0) */
#define REG_PWM_CMR1  (*(RwReg*)0x40020220U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 1) */
#define REG_PWM_CDTY1 (*(RwReg*)0x40020224U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 1) */
#define REG_PWM_CPRD1 (*(RwReg*)0x40020228U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 1) */
#define REG_PWM_CCNT1 (*(RoReg*)0x4002022CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 1) */
#define REG_PWM_CUPD1 (*(WoReg*)0x40020230U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 1) */
#define REG_PWM_CMR2  (*(RwReg*)0x40020240U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 2) */
#define REG_PWM_CDTY2 (*(RwReg*)0x40020244U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 2) */
#define REG_PWM_CPRD2 (*(RwReg*)0x40020248U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 2) */
#define REG_PWM_CCNT2 (*(RoReg*)0x4002024CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 2) */
#define REG_PWM_CUPD2 (*(WoReg*)0x40020250U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 2) */
#define REG_PWM_CMR3  (*(RwReg*)0x40020260U) /**< \brief (PWM) PWM Channel Mode Register (ch_num = 3) */
#define REG_PWM_CDTY3 (*(RwReg*)0x40020264U) /**< \brief (PWM) PWM Channel Duty Cycle Register (ch_num = 3) */
#define REG_PWM_CPRD3 (*(RwReg*)0x40020268U) /**< \brief (PWM) PWM Channel Period Register (ch_num = 3) */
#define REG_PWM_CCNT3 (*(RoReg*)0x4002026CU) /**< \brief (PWM) PWM Channel Counter Register (ch_num = 3) */
#define REG_PWM_CUPD3 (*(WoReg*)0x40020270U) /**< \brief (PWM) PWM Channel Update Register (ch_num = 3) */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3N_PWM_INSTANCE_ */
