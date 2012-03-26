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

#ifndef _SAM3N_DACC_COMPONENT_
#define _SAM3N_DACC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Digital-to-Analog Converter Controller */
/* ============================================================================= */
/** \addtogroup SAM3N_DACC Digital-to-Analog Converter Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Dacc hardware registers */
typedef struct {
  WoReg DACC_CR;       /**< \brief (Dacc Offset: 0x00) Control Register */
  RwReg DACC_MR;       /**< \brief (Dacc Offset: 0x04) Mode Register */
  WoReg DACC_CDR;      /**< \brief (Dacc Offset: 0x08) Conversion Data Register */
  WoReg DACC_IER;      /**< \brief (Dacc Offset: 0x0C) Interrupt Enable Register */
  WoReg DACC_IDR;      /**< \brief (Dacc Offset: 0x10) Interrupt Disable Register */
  RoReg DACC_IMR;      /**< \brief (Dacc Offset: 0x14) Interrupt Mask Register */
  RoReg DACC_ISR;      /**< \brief (Dacc Offset: 0x18) Interrupt Status Register */
  RoReg Reserved1[50];
  RwReg DACC_WPMR;     /**< \brief (Dacc Offset: 0xE4) Write Protect Mode Register */
  RoReg DACC_WPSR;     /**< \brief (Dacc Offset: 0xE8) Write Protect Status Register */
  RoReg Reserved2[7];
  RwReg DACC_TPR;      /**< \brief (Dacc Offset: 0x108) Transmit Pointer Register */
  RwReg DACC_TCR;      /**< \brief (Dacc Offset: 0x10C) Transmit Counter Register */
  RoReg Reserved3[2];
  RwReg DACC_TNPR;     /**< \brief (Dacc Offset: 0x118) Transmit Next Pointer Register */
  RwReg DACC_TNCR;     /**< \brief (Dacc Offset: 0x11C) Transmit Next Counter Register */
  WoReg DACC_PTCR;     /**< \brief (Dacc Offset: 0x120) Transfer Control Register */
  RoReg DACC_PTSR;     /**< \brief (Dacc Offset: 0x124) Transfer Status Register */
} Dacc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- DACC_CR : (DACC Offset: 0x00) Control Register -------- */
#define DACC_CR_SWRST (0x1u << 0) /**< \brief (DACC_CR) Software Reset */
/* -------- DACC_MR : (DACC Offset: 0x04) Mode Register -------- */
#define DACC_MR_TRGEN (0x1u << 0) /**< \brief (DACC_MR) Trigger Enable */
#define DACC_MR_TRGSEL_Pos 1
#define DACC_MR_TRGSEL_Msk (0x7u << DACC_MR_TRGSEL_Pos) /**< \brief (DACC_MR) Trigger Selection */
#define   DACC_MR_TRGSEL_TRGSEL0 (0x0u << 1) /**< \brief (DACC_MR) External trigger */
#define   DACC_MR_TRGSEL_TRGSEL1 (0x1u << 1) /**< \brief (DACC_MR) TIO Output of the Timer Counter Channel 0 */
#define   DACC_MR_TRGSEL_TRGSEL2 (0x2u << 1) /**< \brief (DACC_MR) TIO Output of the Timer Counter Channel 1 */
#define   DACC_MR_TRGSEL_TRGSEL3 (0x3u << 1) /**< \brief (DACC_MR) TIO Output of the Timer Counter Channel 2 */
#define DACC_MR_DACEN (0x1u << 4) /**< \brief (DACC_MR) DAC enable */
#define DACC_MR_WORD (0x1u << 5) /**< \brief (DACC_MR) Word Transfer */
#define DACC_MR_STARTUP_Pos 8
#define DACC_MR_STARTUP_Msk (0xffu << DACC_MR_STARTUP_Pos) /**< \brief (DACC_MR) Startup Time Selection */
#define DACC_MR_STARTUP(value) ((DACC_MR_STARTUP_Msk & ((value) << DACC_MR_STARTUP_Pos)))
#define DACC_MR_CLKDIV_Pos 16
#define DACC_MR_CLKDIV_Msk (0xffffu << DACC_MR_CLKDIV_Pos) /**< \brief (DACC_MR) DAC Clock Divider for Internal Trigger */
#define DACC_MR_CLKDIV(value) ((DACC_MR_CLKDIV_Msk & ((value) << DACC_MR_CLKDIV_Pos)))
/* -------- DACC_CDR : (DACC Offset: 0x08) Conversion Data Register -------- */
#define DACC_CDR_DATA_Pos 0
#define DACC_CDR_DATA_Msk (0xffffffffu << DACC_CDR_DATA_Pos) /**< \brief (DACC_CDR) Data to Convert */
#define DACC_CDR_DATA(value) ((DACC_CDR_DATA_Msk & ((value) << DACC_CDR_DATA_Pos)))
/* -------- DACC_IER : (DACC Offset: 0x0C) Interrupt Enable Register -------- */
#define DACC_IER_TXRDY (0x1u << 0) /**< \brief (DACC_IER) Transmission Ready Interrupt Enable */
#define DACC_IER_ENDTX (0x1u << 1) /**< \brief (DACC_IER) End of PDC Interrupt Enable */
#define DACC_IER_TXBUFE (0x1u << 2) /**< \brief (DACC_IER) Buffer Empty Interrupt Enable */
/* -------- DACC_IDR : (DACC Offset: 0x10) Interrupt Disable Register -------- */
#define DACC_IDR_TXRDY (0x1u << 0) /**< \brief (DACC_IDR) Transmission Ready Interrupt Disable */
#define DACC_IDR_ENDTX (0x1u << 1) /**< \brief (DACC_IDR) End of PDC Interrupt Disable */
#define DACC_IDR_TXBUFE (0x1u << 2) /**< \brief (DACC_IDR) Buffer Empty Interrupt Disable */
/* -------- DACC_IMR : (DACC Offset: 0x14) Interrupt Mask Register -------- */
#define DACC_IMR_TXRDY (0x1u << 0) /**< \brief (DACC_IMR) Transmission Ready Interrupt Mask */
#define DACC_IMR_ENDTX (0x1u << 1) /**< \brief (DACC_IMR) End of PDC Interrupt Mask */
#define DACC_IMR_TXBUFE (0x1u << 2) /**< \brief (DACC_IMR) Buffer Empty Interrupt Mask */
/* -------- DACC_ISR : (DACC Offset: 0x18) Interrupt Status Register -------- */
#define DACC_ISR_TXRDY (0x1u << 0) /**< \brief (DACC_ISR) Transmission Ready Interrupt Flag */
#define DACC_ISR_ENDTX (0x1u << 1) /**< \brief (DACC_ISR) End of PDC Interrupt Flag */
#define DACC_ISR_TXBUFE (0x1u << 2) /**< \brief (DACC_ISR) Buffer Empty Interrupt Flag */
/* -------- DACC_WPMR : (DACC Offset: 0xE4) Write Protect Mode Register -------- */
#define DACC_WPMR_WPEN (0x1u << 0) /**< \brief (DACC_WPMR) Write Protect Enable */
#define DACC_WPMR_WPKEY_Pos 8
#define DACC_WPMR_WPKEY_Msk (0xffffffu << DACC_WPMR_WPKEY_Pos) /**< \brief (DACC_WPMR) Write Protect KEY */
#define DACC_WPMR_WPKEY(value) ((DACC_WPMR_WPKEY_Msk & ((value) << DACC_WPMR_WPKEY_Pos)))
/* -------- DACC_WPSR : (DACC Offset: 0xE8) Write Protect Status Register -------- */
#define DACC_WPSR_WPROTERR (0x1u << 0) /**< \brief (DACC_WPSR) Write protection error */
#define DACC_WPSR_WPROTADDR_Pos 8
#define DACC_WPSR_WPROTADDR_Msk (0xffu << DACC_WPSR_WPROTADDR_Pos) /**< \brief (DACC_WPSR) Write protection error address */
/* -------- DACC_TPR : (DACC Offset: 0x108) Transmit Pointer Register -------- */
#define DACC_TPR_TXPTR_Pos 0
#define DACC_TPR_TXPTR_Msk (0xffffffffu << DACC_TPR_TXPTR_Pos) /**< \brief (DACC_TPR) Transmit Counter Register */
#define DACC_TPR_TXPTR(value) ((DACC_TPR_TXPTR_Msk & ((value) << DACC_TPR_TXPTR_Pos)))
/* -------- DACC_TCR : (DACC Offset: 0x10C) Transmit Counter Register -------- */
#define DACC_TCR_TXCTR_Pos 0
#define DACC_TCR_TXCTR_Msk (0xffffu << DACC_TCR_TXCTR_Pos) /**< \brief (DACC_TCR) Transmit Counter Register */
#define DACC_TCR_TXCTR(value) ((DACC_TCR_TXCTR_Msk & ((value) << DACC_TCR_TXCTR_Pos)))
/* -------- DACC_TNPR : (DACC Offset: 0x118) Transmit Next Pointer Register -------- */
#define DACC_TNPR_TXNPTR_Pos 0
#define DACC_TNPR_TXNPTR_Msk (0xffffffffu << DACC_TNPR_TXNPTR_Pos) /**< \brief (DACC_TNPR) Transmit Next Pointer */
#define DACC_TNPR_TXNPTR(value) ((DACC_TNPR_TXNPTR_Msk & ((value) << DACC_TNPR_TXNPTR_Pos)))
/* -------- DACC_TNCR : (DACC Offset: 0x11C) Transmit Next Counter Register -------- */
#define DACC_TNCR_TXNCTR_Pos 0
#define DACC_TNCR_TXNCTR_Msk (0xffffu << DACC_TNCR_TXNCTR_Pos) /**< \brief (DACC_TNCR) Transmit Counter Next */
#define DACC_TNCR_TXNCTR(value) ((DACC_TNCR_TXNCTR_Msk & ((value) << DACC_TNCR_TXNCTR_Pos)))
/* -------- DACC_PTCR : (DACC Offset: 0x120) Transfer Control Register -------- */
#define DACC_PTCR_RXTEN (0x1u << 0) /**< \brief (DACC_PTCR) Receiver Transfer Enable */
#define DACC_PTCR_RXTDIS (0x1u << 1) /**< \brief (DACC_PTCR) Receiver Transfer Disable */
#define DACC_PTCR_TXTEN (0x1u << 8) /**< \brief (DACC_PTCR) Transmitter Transfer Enable */
#define DACC_PTCR_TXTDIS (0x1u << 9) /**< \brief (DACC_PTCR) Transmitter Transfer Disable */
/* -------- DACC_PTSR : (DACC Offset: 0x124) Transfer Status Register -------- */
#define DACC_PTSR_RXTEN (0x1u << 0) /**< \brief (DACC_PTSR) Receiver Transfer Enable */
#define DACC_PTSR_TXTEN (0x1u << 8) /**< \brief (DACC_PTSR) Transmitter Transfer Enable */

/*@}*/


#endif /* _SAM3N_DACC_COMPONENT_ */
