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

#ifndef _SAM3N_PWM_COMPONENT_
#define _SAM3N_PWM_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Pulse Width Modulation Controller */
/* ============================================================================= */
/** \addtogroup SAM3N_PWM Pulse Width Modulation Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief PwmCh_num hardware registers */
typedef struct {
  RwReg      PWM_CMR;        /**< \brief (PwmCh_num Offset: 0x0) PWM Channel Mode Register */
  RwReg      PWM_CDTY;       /**< \brief (PwmCh_num Offset: 0x4) PWM Channel Duty Cycle Register */
  RwReg      PWM_CPRD;       /**< \brief (PwmCh_num Offset: 0x8) PWM Channel Period Register */
  RwReg      PWM_CCNT;       /**< \brief (PwmCh_num Offset: 0xC) PWM Channel Counter Register */
  RwReg      PWM_CUPD;       /**< \brief (PwmCh_num Offset: 0x10) PWM Channel Update Register */
  RoReg      Reserved1[3];
} PwmCh_num;
/** \brief Pwm hardware registers */
#define PWMCH_NUM_NUMBER 4
typedef struct {
  RwReg      PWM_MR;         /**< \brief (Pwm Offset: 0x00) PWM Mode Register */
  WoReg      PWM_ENA;        /**< \brief (Pwm Offset: 0x04) PWM Enable Register */
  WoReg      PWM_DIS;        /**< \brief (Pwm Offset: 0x08) PWM Disable Register */
  RoReg      PWM_SR;         /**< \brief (Pwm Offset: 0x0C) PWM Status Register */
  WoReg      PWM_IER;        /**< \brief (Pwm Offset: 0x10) PWM Interrupt Enable Register */
  WoReg      PWM_IDR;        /**< \brief (Pwm Offset: 0x14) PWM Interrupt Disable Register */
  RoReg      PWM_IMR;        /**< \brief (Pwm Offset: 0x18) PWM Interrupt Mask Register */
  RoReg      PWM_ISR;        /**< \brief (Pwm Offset: 0x1C) PWM Interrupt Status Register */
  RoReg      Reserved1[120];
  PwmCh_num  PWM_CH_NUM[PWMCH_NUM_NUMBER]; /**< \brief (Pwm Offset: 0x200) ch_num = 0 .. 3 */
} Pwm;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- PWM_MR : (PWM Offset: 0x00) PWM Mode Register -------- */
#define PWM_MR_DIVA_Pos 0
#define PWM_MR_DIVA_Msk (0xffu << PWM_MR_DIVA_Pos) /**< \brief (PWM_MR) CLKA, CLKB Divide Factor */
#define   PWM_MR_DIVA_CLK_OFF (0x0u << 0) /**< \brief (PWM_MR) CLKA, CLKB clock is turned off */
#define   PWM_MR_DIVA_CLK_DIV1 (0x1u << 0) /**< \brief (PWM_MR) CLKA, CLKB clock is clock selected by PREA, PREB */
#define PWM_MR_PREA_Pos 8
#define PWM_MR_PREA_Msk (0xfu << PWM_MR_PREA_Pos) /**< \brief (PWM_MR)  */
#define   PWM_MR_PREA_MCK (0x0u << 8) /**< \brief (PWM_MR) Master Clock */
#define   PWM_MR_PREA_MCKDIV2 (0x1u << 8) /**< \brief (PWM_MR) Master Clock divided by 2 */
#define   PWM_MR_PREA_MCKDIV4 (0x2u << 8) /**< \brief (PWM_MR) Master Clock divided by 4 */
#define   PWM_MR_PREA_MCKDIV8 (0x3u << 8) /**< \brief (PWM_MR) Master Clock divided by 8 */
#define   PWM_MR_PREA_MCKDIV16 (0x4u << 8) /**< \brief (PWM_MR) Master Clock divided by 16 */
#define   PWM_MR_PREA_MCKDIV32 (0x5u << 8) /**< \brief (PWM_MR) Master Clock divided by 32 */
#define   PWM_MR_PREA_MCKDIV64 (0x6u << 8) /**< \brief (PWM_MR) Master Clock divided by 64 */
#define   PWM_MR_PREA_MCKDIV128 (0x7u << 8) /**< \brief (PWM_MR) Master Clock divided by 128 */
#define   PWM_MR_PREA_MCKDIV256 (0x8u << 8) /**< \brief (PWM_MR) Master Clock divided by 256 */
#define   PWM_MR_PREA_MCKDIV512 (0x9u << 8) /**< \brief (PWM_MR) Master Clock divided by 512 */
#define   PWM_MR_PREA_MCKDIV1024 (0xAu << 8) /**< \brief (PWM_MR) Master Clock divided by 1024 */
#define PWM_MR_DIVB_Pos 16
#define PWM_MR_DIVB_Msk (0xffu << PWM_MR_DIVB_Pos) /**< \brief (PWM_MR) CLKA, CLKB Divide Factor */
#define   PWM_MR_DIVB_CLK_OFF (0x0u << 16) /**< \brief (PWM_MR) CLKA, CLKB clock is turned off */
#define   PWM_MR_DIVB_CLK_DIV1 (0x1u << 16) /**< \brief (PWM_MR) CLKA, CLKB clock is clock selected by PREA, PREB */
#define PWM_MR_PREB_Pos 24
#define PWM_MR_PREB_Msk (0xfu << PWM_MR_PREB_Pos) /**< \brief (PWM_MR)  */
#define   PWM_MR_PREB_MCK (0x0u << 24) /**< \brief (PWM_MR) Master Clock */
#define   PWM_MR_PREB_MCKDIV2 (0x1u << 24) /**< \brief (PWM_MR) Master Clock divided by 2 */
#define   PWM_MR_PREB_MCKDIV4 (0x2u << 24) /**< \brief (PWM_MR) Master Clock divided by 4 */
#define   PWM_MR_PREB_MCKDIV8 (0x3u << 24) /**< \brief (PWM_MR) Master Clock divided by 8 */
#define   PWM_MR_PREB_MCKDIV16 (0x4u << 24) /**< \brief (PWM_MR) Master Clock divided by 16 */
#define   PWM_MR_PREB_MCKDIV32 (0x5u << 24) /**< \brief (PWM_MR) Master Clock divided by 32 */
#define   PWM_MR_PREB_MCKDIV64 (0x6u << 24) /**< \brief (PWM_MR) Master Clock divided by 64 */
#define   PWM_MR_PREB_MCKDIV128 (0x7u << 24) /**< \brief (PWM_MR) Master Clock divided by 128 */
#define   PWM_MR_PREB_MCKDIV256 (0x8u << 24) /**< \brief (PWM_MR) Master Clock divided by 256 */
#define   PWM_MR_PREB_MCKDIV512 (0x9u << 24) /**< \brief (PWM_MR) Master Clock divided by 512 */
#define   PWM_MR_PREB_MCKDIV1024 (0xAu << 24) /**< \brief (PWM_MR) Master Clock divided by 1024 */
/* -------- PWM_ENA : (PWM Offset: 0x04) PWM Enable Register -------- */
#define PWM_ENA_CHID0 (0x1u << 0) /**< \brief (PWM_ENA) Channel ID */
#define PWM_ENA_CHID1 (0x1u << 1) /**< \brief (PWM_ENA) Channel ID */
#define PWM_ENA_CHID2 (0x1u << 2) /**< \brief (PWM_ENA) Channel ID */
#define PWM_ENA_CHID3 (0x1u << 3) /**< \brief (PWM_ENA) Channel ID */
/* -------- PWM_DIS : (PWM Offset: 0x08) PWM Disable Register -------- */
#define PWM_DIS_CHID0 (0x1u << 0) /**< \brief (PWM_DIS) Channel ID */
#define PWM_DIS_CHID1 (0x1u << 1) /**< \brief (PWM_DIS) Channel ID */
#define PWM_DIS_CHID2 (0x1u << 2) /**< \brief (PWM_DIS) Channel ID */
#define PWM_DIS_CHID3 (0x1u << 3) /**< \brief (PWM_DIS) Channel ID */
/* -------- PWM_SR : (PWM Offset: 0x0C) PWM Status Register -------- */
#define PWM_SR_CHID0 (0x1u << 0) /**< \brief (PWM_SR) Channel ID */
#define PWM_SR_CHID1 (0x1u << 1) /**< \brief (PWM_SR) Channel ID */
#define PWM_SR_CHID2 (0x1u << 2) /**< \brief (PWM_SR) Channel ID */
#define PWM_SR_CHID3 (0x1u << 3) /**< \brief (PWM_SR) Channel ID */
/* -------- PWM_IER : (PWM Offset: 0x10) PWM Interrupt Enable Register -------- */
#define PWM_IER_CHID0 (0x1u << 0) /**< \brief (PWM_IER) Channel ID. */
#define PWM_IER_CHID1 (0x1u << 1) /**< \brief (PWM_IER) Channel ID. */
#define PWM_IER_CHID2 (0x1u << 2) /**< \brief (PWM_IER) Channel ID. */
#define PWM_IER_CHID3 (0x1u << 3) /**< \brief (PWM_IER) Channel ID. */
/* -------- PWM_IDR : (PWM Offset: 0x14) PWM Interrupt Disable Register -------- */
#define PWM_IDR_CHID0 (0x1u << 0) /**< \brief (PWM_IDR) Channel ID. */
#define PWM_IDR_CHID1 (0x1u << 1) /**< \brief (PWM_IDR) Channel ID. */
#define PWM_IDR_CHID2 (0x1u << 2) /**< \brief (PWM_IDR) Channel ID. */
#define PWM_IDR_CHID3 (0x1u << 3) /**< \brief (PWM_IDR) Channel ID. */
/* -------- PWM_IMR : (PWM Offset: 0x18) PWM Interrupt Mask Register -------- */
#define PWM_IMR_CHID0 (0x1u << 0) /**< \brief (PWM_IMR) Channel ID. */
#define PWM_IMR_CHID1 (0x1u << 1) /**< \brief (PWM_IMR) Channel ID. */
#define PWM_IMR_CHID2 (0x1u << 2) /**< \brief (PWM_IMR) Channel ID. */
#define PWM_IMR_CHID3 (0x1u << 3) /**< \brief (PWM_IMR) Channel ID. */
/* -------- PWM_ISR : (PWM Offset: 0x1C) PWM Interrupt Status Register -------- */
#define PWM_ISR_CHID0 (0x1u << 0) /**< \brief (PWM_ISR) Channel ID */
#define PWM_ISR_CHID1 (0x1u << 1) /**< \brief (PWM_ISR) Channel ID */
#define PWM_ISR_CHID2 (0x1u << 2) /**< \brief (PWM_ISR) Channel ID */
#define PWM_ISR_CHID3 (0x1u << 3) /**< \brief (PWM_ISR) Channel ID */
/* -------- PWM_CMR : (PWM Offset: N/A) PWM Channel Mode Register -------- */
#define PWM_CMR_CPRE_Pos 0
#define PWM_CMR_CPRE_Msk (0xfu << PWM_CMR_CPRE_Pos) /**< \brief (PWM_CMR) Channel Pre-scaler */
#define   PWM_CMR_CPRE_MCK (0x0u << 0) /**< \brief (PWM_CMR) Master Clock */
#define   PWM_CMR_CPRE_MCKDIV2 (0x1u << 0) /**< \brief (PWM_CMR) Master Clock divided by 2 */
#define   PWM_CMR_CPRE_MCKDIV4 (0x2u << 0) /**< \brief (PWM_CMR) Master Clock divided by 4 */
#define   PWM_CMR_CPRE_MCKDIV8 (0x3u << 0) /**< \brief (PWM_CMR) Master Clock divided by 8 */
#define   PWM_CMR_CPRE_MCKDIV16 (0x4u << 0) /**< \brief (PWM_CMR) Master Clock divided by 16 */
#define   PWM_CMR_CPRE_MCKDIV32 (0x5u << 0) /**< \brief (PWM_CMR) Master Clock divided by 32 */
#define   PWM_CMR_CPRE_MCKDIV64 (0x6u << 0) /**< \brief (PWM_CMR) Master Clock divided by 64 */
#define   PWM_CMR_CPRE_MCKDIV128 (0x7u << 0) /**< \brief (PWM_CMR) Master Clock divided by 128 */
#define   PWM_CMR_CPRE_MCKDIV256 (0x8u << 0) /**< \brief (PWM_CMR) Master Clock divided by 256 */
#define   PWM_CMR_CPRE_MCKDIV512 (0x9u << 0) /**< \brief (PWM_CMR) Master Clock divided by 512 */
#define   PWM_CMR_CPRE_MCKDIV1024 (0xAu << 0) /**< \brief (PWM_CMR) Master Clock divided by 1024 */
#define   PWM_CMR_CPRE_CLKA (0xBu << 0) /**< \brief (PWM_CMR) Clock A */
#define   PWM_CMR_CPRE_CLKB (0xCu << 0) /**< \brief (PWM_CMR) Clock B */
#define PWM_CMR_CALG (0x1u << 8) /**< \brief (PWM_CMR) Channel Alignment */
#define PWM_CMR_CPOL (0x1u << 9) /**< \brief (PWM_CMR) Channel Polarity */
#define PWM_CMR_CPD (0x1u << 10) /**< \brief (PWM_CMR) Channel Update Period */
/* -------- PWM_CDTY : (PWM Offset: N/A) PWM Channel Duty Cycle Register -------- */
#define PWM_CDTY_CDTY_Pos 0
#define PWM_CDTY_CDTY_Msk (0xffffffffu << PWM_CDTY_CDTY_Pos) /**< \brief (PWM_CDTY) Channel Duty Cycle */
#define PWM_CDTY_CDTY(value) ((PWM_CDTY_CDTY_Msk & ((value) << PWM_CDTY_CDTY_Pos)))
/* -------- PWM_CPRD : (PWM Offset: N/A) PWM Channel Period Register -------- */
#define PWM_CPRD_CPRD_Pos 0
#define PWM_CPRD_CPRD_Msk (0xffffffffu << PWM_CPRD_CPRD_Pos) /**< \brief (PWM_CPRD) Channel Period */
#define PWM_CPRD_CPRD(value) ((PWM_CPRD_CPRD_Msk & ((value) << PWM_CPRD_CPRD_Pos)))
/* -------- PWM_CCNT : (PWM Offset: N/A) PWM Channel Counter Register -------- */
#define PWM_CCNT_CNT_Pos 0
#define PWM_CCNT_CNT_Msk (0xffffffffu << PWM_CCNT_CNT_Pos) /**< \brief (PWM_CCNT) Channel Counter Register */
/* -------- PWM_CUPD : (PWM Offset: N/A) PWM Channel Update Register -------- */
#define PWM_CUPD_CUPD_Pos 0
#define PWM_CUPD_CUPD_Msk (0xffffffffu << PWM_CUPD_CUPD_Pos) /**< \brief (PWM_CUPD)  */
#define PWM_CUPD_CUPD(value) ((PWM_CUPD_CUPD_Msk & ((value) << PWM_CUPD_CUPD_Pos)))

/*@}*/


#endif /* _SAM3N_PWM_COMPONENT_ */
