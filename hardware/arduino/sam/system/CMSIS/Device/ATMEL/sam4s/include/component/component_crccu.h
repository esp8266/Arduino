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

#ifndef _SAM4S_CRCCU_COMPONENT_
#define _SAM4S_CRCCU_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Cyclic Redundancy Check Calculation Unit */
/* ============================================================================= */
/** \addtogroup SAM4S_CRCCU Cyclic Redundancy Check Calculation Unit */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Crccu hardware registers */
typedef struct {
  RwReg CRCCU_DSCR;    /**< \brief (Crccu Offset: 0x00000000) CRCCU Descriptor Base Register */
  RoReg Reserved1[1];
  WoReg CRCCU_DMA_EN;  /**< \brief (Crccu Offset: 0x00000008) CRCCU DMA Enable Register */
  WoReg CRCCU_DMA_DIS; /**< \brief (Crccu Offset: 0x0000000C) CRCCU DMA Disable Register */
  RoReg CRCCU_DMA_SR;  /**< \brief (Crccu Offset: 0x00000010) CRCCU DMA Status Register */
  WoReg CRCCU_DMA_IER; /**< \brief (Crccu Offset: 0x00000014) CRCCU DMA Interrupt Enable Register */
  WoReg CRCCU_DMA_IDR; /**< \brief (Crccu Offset: 0x00000018) CRCCU DMA Interrupt Disable Register */
  RoReg CRCCU_DMA_IMR; /**< \brief (Crccu Offset: 0x0000001C) CRCCU DMA Interrupt Mask Register */
  RoReg CRCCU_DMA_ISR; /**< \brief (Crccu Offset: 0x00000020) CRCCU DMA Interrupt Status Register */
  RoReg Reserved2[4];
  WoReg CRCCU_CR;      /**< \brief (Crccu Offset: 0x00000034) CRCCU Control Register */
  RwReg CRCCU_MR;      /**< \brief (Crccu Offset: 0x00000038) CRCCU Mode Register */
  RoReg CRCCU_SR;      /**< \brief (Crccu Offset: 0x0000003C) CRCCU Status Register */
  WoReg CRCCU_IER;     /**< \brief (Crccu Offset: 0x00000040) CRCCU Interrupt Enable Register */
  WoReg CRCCU_IDR;     /**< \brief (Crccu Offset: 0x00000044) CRCCU Interrupt Disable Register */
  RoReg CRCCU_IMR;     /**< \brief (Crccu Offset: 0x00000048) CRCCU Interrupt Mask Register */
  RoReg CRCCU_ISR;     /**< \brief (Crccu Offset: 0x0000004C) CRCCU Interrupt Status Register */
} Crccu;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- CRCCU_DSCR : (CRCCU Offset: 0x00000000) CRCCU Descriptor Base Register -------- */
#define CRCCU_DSCR_DSCR_Pos 9
#define CRCCU_DSCR_DSCR_Msk (0x7fffffu << CRCCU_DSCR_DSCR_Pos) /**< \brief (CRCCU_DSCR) Descriptor Base Address */
#define CRCCU_DSCR_DSCR(value) ((CRCCU_DSCR_DSCR_Msk & ((value) << CRCCU_DSCR_DSCR_Pos)))
/* -------- CRCCU_DMA_EN : (CRCCU Offset: 0x00000008) CRCCU DMA Enable Register -------- */
#define CRCCU_DMA_EN_DMAEN (0x1u << 0) /**< \brief (CRCCU_DMA_EN) DMA Enable Register */
/* -------- CRCCU_DMA_DIS : (CRCCU Offset: 0x0000000C) CRCCU DMA Disable Register -------- */
#define CRCCU_DMA_DIS_DMADIS (0x1u << 0) /**< \brief (CRCCU_DMA_DIS) DMA Disable Register */
/* -------- CRCCU_DMA_SR : (CRCCU Offset: 0x00000010) CRCCU DMA Status Register -------- */
#define CRCCU_DMA_SR_DMASR (0x1u << 0) /**< \brief (CRCCU_DMA_SR) DMA Status Register */
/* -------- CRCCU_DMA_IER : (CRCCU Offset: 0x00000014) CRCCU DMA Interrupt Enable Register -------- */
#define CRCCU_DMA_IER_DMAIER (0x1u << 0) /**< \brief (CRCCU_DMA_IER) Interrupt Enable register */
/* -------- CRCCU_DMA_IDR : (CRCCU Offset: 0x00000018) CRCCU DMA Interrupt Disable Register -------- */
#define CRCCU_DMA_IDR_DMAIDR (0x1u << 0) /**< \brief (CRCCU_DMA_IDR) Interrupt Disable register */
/* -------- CRCCU_DMA_IMR : (CRCCU Offset: 0x0000001C) CRCCU DMA Interrupt Mask Register -------- */
#define CRCCU_DMA_IMR_DMAIMR (0x1u << 0) /**< \brief (CRCCU_DMA_IMR) Interrupt Mask Register */
/* -------- CRCCU_DMA_ISR : (CRCCU Offset: 0x00000020) CRCCU DMA Interrupt Status Register -------- */
#define CRCCU_DMA_ISR_DMAISR (0x1u << 0) /**< \brief (CRCCU_DMA_ISR) Interrupt Status register */
/* -------- CRCCU_CR : (CRCCU Offset: 0x00000034) CRCCU Control Register -------- */
#define CRCCU_CR_RESET (0x1u << 0) /**< \brief (CRCCU_CR) CRC Computation Reset */
/* -------- CRCCU_MR : (CRCCU Offset: 0x00000038) CRCCU Mode Register -------- */
#define CRCCU_MR_ENABLE (0x1u << 0) /**< \brief (CRCCU_MR) CRC Enable */
#define CRCCU_MR_COMPARE (0x1u << 1) /**< \brief (CRCCU_MR) CRC Compare */
#define CRCCU_MR_PTYPE_Pos 2
#define CRCCU_MR_PTYPE_Msk (0x3u << CRCCU_MR_PTYPE_Pos) /**< \brief (CRCCU_MR) Primitive Polynomial */
#define   CRCCU_MR_PTYPE_CCITT8023 (0x0u << 2) /**< \brief (CRCCU_MR) Polynom 0x04C11DB7 */
#define   CRCCU_MR_PTYPE_CASTAGNOLI (0x1u << 2) /**< \brief (CRCCU_MR) Polynom 0x1EDC6F41 */
#define   CRCCU_MR_PTYPE_CCITT16 (0x2u << 2) /**< \brief (CRCCU_MR) Polynom 0x1021 */
#define CRCCU_MR_DIVIDER_Pos 4
#define CRCCU_MR_DIVIDER_Msk (0xfu << CRCCU_MR_DIVIDER_Pos) /**< \brief (CRCCU_MR) Request Divider */
#define CRCCU_MR_DIVIDER(value) ((CRCCU_MR_DIVIDER_Msk & ((value) << CRCCU_MR_DIVIDER_Pos)))
/* -------- CRCCU_SR : (CRCCU Offset: 0x0000003C) CRCCU Status Register -------- */
#define CRCCU_SR_CRC_Pos 0
#define CRCCU_SR_CRC_Msk (0xffffffffu << CRCCU_SR_CRC_Pos) /**< \brief (CRCCU_SR) Cyclic Redundancy Check Value */
/* -------- CRCCU_IER : (CRCCU Offset: 0x00000040) CRCCU Interrupt Enable Register -------- */
#define CRCCU_IER_ERRIER (0x1u << 0) /**< \brief (CRCCU_IER) CRC Error Interrupt Enable */
/* -------- CRCCU_IDR : (CRCCU Offset: 0x00000044) CRCCU Interrupt Disable Register -------- */
#define CRCCU_IDR_ERRIDR (0x1u << 0) /**< \brief (CRCCU_IDR) CRC Error Interrupt Disable */
/* -------- CRCCU_IMR : (CRCCU Offset: 0x00000048) CRCCU Interrupt Mask Register -------- */
#define CRCCU_IMR_ERRIMR (0x1u << 0) /**< \brief (CRCCU_IMR) CRC Error Interrupt Mask */
/* -------- CRCCU_ISR : (CRCCU Offset: 0x0000004C) CRCCU Interrupt Status Register -------- */
#define CRCCU_ISR_ERRISR (0x1u << 0) /**< \brief (CRCCU_ISR) CRC Error Interrupt Status */

/*@}*/


#endif /* _SAM4S_CRCCU_COMPONENT_ */
