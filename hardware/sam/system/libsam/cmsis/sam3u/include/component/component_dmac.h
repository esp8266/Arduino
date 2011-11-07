/* ----------------------------------------------------------------------------
 *         SAM Software Package License 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#ifndef _SAM3U_DMAC_COMPONENT_
#define _SAM3U_DMAC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR DMA Controller */
/* ============================================================================= */
/** \addtogroup SAM3U_DMAC DMA Controller */
/*@{*/

#ifndef __ASSEMBLY__
/** \brief DmacCh_num hardware registers */
typedef struct {
  RwReg       DMAC_SADDR;     /**< \brief (DmacCh_num Offset: 0x0) DMAC Channel Source Address Register */
  RwReg       DMAC_DADDR;     /**< \brief (DmacCh_num Offset: 0x4) DMAC Channel Destination Address Register */
  RwReg       DMAC_DSCR;      /**< \brief (DmacCh_num Offset: 0x8) DMAC Channel Descriptor Address Register */
  RwReg       DMAC_CTRLA;     /**< \brief (DmacCh_num Offset: 0xC) DMAC Channel Control A Register */
  RwReg       DMAC_CTRLB;     /**< \brief (DmacCh_num Offset: 0x10) DMAC Channel Control B Register */
  RwReg       DMAC_CFG;       /**< \brief (DmacCh_num Offset: 0x14) DMAC Channel Configuration Register */
  RoReg       Reserved1[4];
} DmacCh_num;
/** \brief Dmac hardware registers */
#define DMACCH_NUM_NUMBER 4
typedef struct {
  RwReg       DMAC_GCFG;      /**< \brief (Dmac Offset: 0x000) DMAC Global Configuration Register */
  RwReg       DMAC_EN;        /**< \brief (Dmac Offset: 0x004) DMAC Enable Register */
  RwReg       DMAC_SREQ;      /**< \brief (Dmac Offset: 0x008) DMAC Software Single Request Register */
  RwReg       DMAC_CREQ;      /**< \brief (Dmac Offset: 0x00C) DMAC Software Chunk Transfer Request Register */
  RwReg       DMAC_LAST;      /**< \brief (Dmac Offset: 0x010) DMAC Software Last Transfer Flag Register */
  RoReg       Reserved1[1];
  WoReg       DMAC_EBCIER;    /**< \brief (Dmac Offset: 0x018) DMAC Error, Chained Buffer transfer completed and Buffer transfer completed Interrupt Enable register. */
  WoReg       DMAC_EBCIDR;    /**< \brief (Dmac Offset: 0x01C) DMAC Error, Chained Buffer transfer completed and Buffer transfer completed Interrupt Disable register. */
  RoReg       DMAC_EBCIMR;    /**< \brief (Dmac Offset: 0x020) DMAC Error, Chained Buffer transfer completed and Buffer transfer completed Mask Register. */
  RoReg       DMAC_EBCISR;    /**< \brief (Dmac Offset: 0x024) DMAC Error, Chained Buffer transfer completed and Buffer transfer completed Status Register. */
  WoReg       DMAC_CHER;      /**< \brief (Dmac Offset: 0x028) DMAC Channel Handler Enable Register */
  WoReg       DMAC_CHDR;      /**< \brief (Dmac Offset: 0x02C) DMAC Channel Handler Disable Register */
  RoReg       DMAC_CHSR;      /**< \brief (Dmac Offset: 0x030) DMAC Channel Handler Status Register */
  RoReg       Reserved2[2];
  DmacCh_num  DMAC_CH_NUM[DMACCH_NUM_NUMBER]; /**< \brief (Dmac Offset: 0x3C) ch_num = 0 .. 3 */
} Dmac;
#endif /* __ASSEMBLY__ */
/* -------- DMAC_GCFG : (DMAC Offset: 0x000) DMAC Global Configuration Register -------- */
#define DMAC_GCFG_ARB_CFG (0x1u << 4) /**< \brief (DMAC_GCFG)  */
/* -------- DMAC_EN : (DMAC Offset: 0x004) DMAC Enable Register -------- */
#define DMAC_EN_ENABLE (0x1u << 0) /**< \brief (DMAC_EN)  */
/* -------- DMAC_SREQ : (DMAC Offset: 0x008) DMAC Software Single Request Register -------- */
#define DMAC_SREQ_SSREQ0 (0x1u << 0) /**< \brief (DMAC_SREQ)  */
#define DMAC_SREQ_DSREQ0 (0x1u << 1) /**< \brief (DMAC_SREQ)  */
#define DMAC_SREQ_SSREQ1 (0x1u << 2) /**< \brief (DMAC_SREQ)  */
#define DMAC_SREQ_DSREQ1 (0x1u << 3) /**< \brief (DMAC_SREQ)  */
#define DMAC_SREQ_SSREQ3 (0x1u << 6) /**< \brief (DMAC_SREQ)  */
#define DMAC_SREQ_DSREQ3 (0x1u << 7) /**< \brief (DMAC_SREQ)  */
/* -------- DMAC_CREQ : (DMAC Offset: 0x00C) DMAC Software Chunk Transfer Request Register -------- */
#define DMAC_CREQ_SCREQ0 (0x1u << 0) /**< \brief (DMAC_CREQ)  */
#define DMAC_CREQ_DCREQ0 (0x1u << 1) /**< \brief (DMAC_CREQ)  */
#define DMAC_CREQ_SCREQ1 (0x1u << 2) /**< \brief (DMAC_CREQ)  */
#define DMAC_CREQ_DCREQ1 (0x1u << 3) /**< \brief (DMAC_CREQ)  */
#define DMAC_CREQ_SCREQ3 (0x1u << 6) /**< \brief (DMAC_CREQ)  */
#define DMAC_CREQ_DCREQ3 (0x1u << 7) /**< \brief (DMAC_CREQ)  */
/* -------- DMAC_LAST : (DMAC Offset: 0x010) DMAC Software Last Transfer Flag Register -------- */
#define DMAC_LAST_SLAST0 (0x1u << 0) /**< \brief (DMAC_LAST)  */
#define DMAC_LAST_DLAST0 (0x1u << 1) /**< \brief (DMAC_LAST)  */
#define DMAC_LAST_SLAST1 (0x1u << 2) /**< \brief (DMAC_LAST)  */
#define DMAC_LAST_DLAST1 (0x1u << 3) /**< \brief (DMAC_LAST)  */
#define DMAC_LAST_SLAST2 (0x1u << 4) /**< \brief (DMAC_LAST)  */
#define DMAC_LAST_DLAST2 (0x1u << 5) /**< \brief (DMAC_LAST)  */
#define DMAC_LAST_SLAST3 (0x1u << 6) /**< \brief (DMAC_LAST)  */
#define DMAC_LAST_DLAST3 (0x1u << 7) /**< \brief (DMAC_LAST)  */
/* -------- DMAC_EBCIER : (DMAC Offset: 0x018) DMAC Error, Chained Buffer transfer completed and Buffer transfer completed Interrupt Enable register. -------- */
#define DMAC_EBCIER_BTC0 (0x1u << 0) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_BTC1 (0x1u << 1) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_BTC2 (0x1u << 2) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_BTC3 (0x1u << 3) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_CBTC0 (0x1u << 8) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_CBTC1 (0x1u << 9) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_CBTC2 (0x1u << 10) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_CBTC3 (0x1u << 11) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_ERR0 (0x1u << 16) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_ERR1 (0x1u << 17) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_ERR2 (0x1u << 18) /**< \brief (DMAC_EBCIER)  */
#define DMAC_EBCIER_ERR3 (0x1u << 19) /**< \brief (DMAC_EBCIER)  */
/* -------- DMAC_EBCIDR : (DMAC Offset: 0x01C) DMAC Error, Chained Buffer transfer completed and Buffer transfer completed Interrupt Disable register. -------- */
#define DMAC_EBCIDR_BTC0 (0x1u << 0) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_BTC1 (0x1u << 1) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_BTC2 (0x1u << 2) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_BTC3 (0x1u << 3) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_CBTC0 (0x1u << 8) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_CBTC1 (0x1u << 9) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_CBTC2 (0x1u << 10) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_CBTC3 (0x1u << 11) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_ERR0 (0x1u << 16) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_ERR1 (0x1u << 17) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_ERR2 (0x1u << 18) /**< \brief (DMAC_EBCIDR)  */
#define DMAC_EBCIDR_ERR3 (0x1u << 19) /**< \brief (DMAC_EBCIDR)  */
/* -------- DMAC_EBCIMR : (DMAC Offset: 0x020) DMAC Error, Chained Buffer transfer completed and Buffer transfer completed Mask Register. -------- */
#define DMAC_EBCIMR_BTC0 (0x1u << 0) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_BTC1 (0x1u << 1) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_BTC2 (0x1u << 2) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_BTC3 (0x1u << 3) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_CBTC0 (0x1u << 8) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_CBTC1 (0x1u << 9) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_CBTC2 (0x1u << 10) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_CBTC3 (0x1u << 11) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_ERR0 (0x1u << 16) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_ERR1 (0x1u << 17) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_ERR2 (0x1u << 18) /**< \brief (DMAC_EBCIMR)  */
#define DMAC_EBCIMR_ERR3 (0x1u << 19) /**< \brief (DMAC_EBCIMR)  */
/* -------- DMAC_EBCISR : (DMAC Offset: 0x024) DMAC Error, Chained Buffer transfer completed and Buffer transfer completed Status Register. -------- */
#define DMAC_EBCISR_BTC0 (0x1u << 0) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_BTC1 (0x1u << 1) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_BTC2 (0x1u << 2) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_BTC3 (0x1u << 3) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_CBTC0 (0x1u << 8) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_CBTC1 (0x1u << 9) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_CBTC2 (0x1u << 10) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_CBTC3 (0x1u << 11) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_ERR0 (0x1u << 16) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_ERR1 (0x1u << 17) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_ERR2 (0x1u << 18) /**< \brief (DMAC_EBCISR)  */
#define DMAC_EBCISR_ERR3 (0x1u << 19) /**< \brief (DMAC_EBCISR)  */
/* -------- DMAC_CHER : (DMAC Offset: 0x028) DMAC Channel Handler Enable Register -------- */
#define DMAC_CHER_ENA0 (0x1u << 0) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_ENA1 (0x1u << 1) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_ENA2 (0x1u << 2) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_ENA3 (0x1u << 3) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_SUSP0 (0x1u << 8) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_SUSP1 (0x1u << 9) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_SUSP2 (0x1u << 10) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_SUSP3 (0x1u << 11) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_KEEP0 (0x1u << 24) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_KEEP1 (0x1u << 25) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_KEEP2 (0x1u << 26) /**< \brief (DMAC_CHER)  */
#define DMAC_CHER_KEEP3 (0x1u << 27) /**< \brief (DMAC_CHER)  */
/* -------- DMAC_CHDR : (DMAC Offset: 0x02C) DMAC Channel Handler Disable Register -------- */
#define DMAC_CHDR_DIS0 (0x1u << 0) /**< \brief (DMAC_CHDR)  */
#define DMAC_CHDR_DIS1 (0x1u << 1) /**< \brief (DMAC_CHDR)  */
#define DMAC_CHDR_DIS2 (0x1u << 2) /**< \brief (DMAC_CHDR)  */
#define DMAC_CHDR_DIS3 (0x1u << 3) /**< \brief (DMAC_CHDR)  */
#define DMAC_CHDR_RES0 (0x1u << 8) /**< \brief (DMAC_CHDR)  */
#define DMAC_CHDR_RES1 (0x1u << 9) /**< \brief (DMAC_CHDR)  */
#define DMAC_CHDR_RES2 (0x1u << 10) /**< \brief (DMAC_CHDR)  */
#define DMAC_CHDR_RES3 (0x1u << 11) /**< \brief (DMAC_CHDR)  */
/* -------- DMAC_CHSR : (DMAC Offset: 0x030) DMAC Channel Handler Status Register -------- */
#define DMAC_CHSR_ENA0 (0x1u << 0) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_ENA1 (0x1u << 1) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_ENA2 (0x1u << 2) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_ENA3 (0x1u << 3) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_SUSP0 (0x1u << 8) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_SUSP1 (0x1u << 9) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_SUSP2 (0x1u << 10) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_SUSP3 (0x1u << 11) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_EMPT0 (0x1u << 16) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_EMPT1 (0x1u << 17) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_EMPT2 (0x1u << 18) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_EMPT3 (0x1u << 19) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_STAL0 (0x1u << 24) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_STAL1 (0x1u << 25) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_STAL2 (0x1u << 26) /**< \brief (DMAC_CHSR)  */
#define DMAC_CHSR_STAL3 (0x1u << 27) /**< \brief (DMAC_CHSR)  */
/* -------- DMAC_SADDR : (DMAC Offset: N/A) DMAC Channel Source Address Register -------- */
#define DMAC_SADDR_SADDRx_Pos 0
#define DMAC_SADDR_SADDRx_Msk (0xffffffffu << DMAC_SADDR_SADDRx_Pos) /**< \brief (DMAC_SADDR)  */
#define DMAC_SADDR_SADDRx(value) ((DMAC_SADDR_SADDRx_Msk & ((value) << DMAC_SADDR_SADDRx_Pos)))
/* -------- DMAC_DADDR : (DMAC Offset: N/A) DMAC Channel Destination Address Register -------- */
#define DMAC_DADDR_DADDRx_Pos 0
#define DMAC_DADDR_DADDRx_Msk (0xffffffffu << DMAC_DADDR_DADDRx_Pos) /**< \brief (DMAC_DADDR)  */
#define DMAC_DADDR_DADDRx(value) ((DMAC_DADDR_DADDRx_Msk & ((value) << DMAC_DADDR_DADDRx_Pos)))
/* -------- DMAC_DSCR : (DMAC Offset: N/A) DMAC Channel Descriptor Address Register -------- */
#define DMAC_DSCR_DSCRx_Pos 2
#define DMAC_DSCR_DSCRx_Msk (0x3fffffffu << DMAC_DSCR_DSCRx_Pos) /**< \brief (DMAC_DSCR)  */
#define DMAC_DSCR_DSCRx(value) ((DMAC_DSCR_DSCRx_Msk & ((value) << DMAC_DSCR_DSCRx_Pos)))
/* -------- DMAC_CTRLA : (DMAC Offset: N/A) DMAC Channel Control A Register -------- */
#define DMAC_CTRLA_BTSIZE_Pos 0
#define DMAC_CTRLA_BTSIZE_Msk (0xfffu << DMAC_CTRLA_BTSIZE_Pos) /**< \brief (DMAC_CTRLA)  */
#define DMAC_CTRLA_BTSIZE(value) ((DMAC_CTRLA_BTSIZE_Msk & ((value) << DMAC_CTRLA_BTSIZE_Pos)))
#define DMAC_CTRLA_SCSIZE (0x1u << 16) /**< \brief (DMAC_CTRLA)  */
#define DMAC_CTRLA_DCSIZE (0x1u << 20) /**< \brief (DMAC_CTRLA)  */
#define DMAC_CTRLA_SRC_WIDTH_Pos 24
#define DMAC_CTRLA_SRC_WIDTH_Msk (0x3u << DMAC_CTRLA_SRC_WIDTH_Pos) /**< \brief (DMAC_CTRLA)  */
#define DMAC_CTRLA_SRC_WIDTH(value) ((DMAC_CTRLA_SRC_WIDTH_Msk & ((value) << DMAC_CTRLA_SRC_WIDTH_Pos)))
#define DMAC_CTRLA_DST_WIDTH_Pos 28
#define DMAC_CTRLA_DST_WIDTH_Msk (0x3u << DMAC_CTRLA_DST_WIDTH_Pos) /**< \brief (DMAC_CTRLA)  */
#define DMAC_CTRLA_DST_WIDTH(value) ((DMAC_CTRLA_DST_WIDTH_Msk & ((value) << DMAC_CTRLA_DST_WIDTH_Pos)))
#define DMAC_CTRLA_DONE (0x1u << 31) /**< \brief (DMAC_CTRLA)  */
/* -------- DMAC_CTRLB : (DMAC Offset: N/A) DMAC Channel Control B Register -------- */
#define DMAC_CTRLB_SRC_DSCR (0x1u << 16) /**< \brief (DMAC_CTRLB)  */
#define DMAC_CTRLB_DST_DSCR (0x1u << 20) /**< \brief (DMAC_CTRLB)  */
#define DMAC_CTRLB_FC_Pos 21
#define DMAC_CTRLB_FC_Msk (0x3u << DMAC_CTRLB_FC_Pos) /**< \brief (DMAC_CTRLB)  */
#define DMAC_CTRLB_FC(value) ((DMAC_CTRLB_FC_Msk & ((value) << DMAC_CTRLB_FC_Pos)))
#define DMAC_CTRLB_SRC_INCR_Pos 24
#define DMAC_CTRLB_SRC_INCR_Msk (0x3u << DMAC_CTRLB_SRC_INCR_Pos) /**< \brief (DMAC_CTRLB)  */
#define DMAC_CTRLB_SRC_INCR(value) ((DMAC_CTRLB_SRC_INCR_Msk & ((value) << DMAC_CTRLB_SRC_INCR_Pos)))
#define DMAC_CTRLB_DST_INCR_Pos 28
#define DMAC_CTRLB_DST_INCR_Msk (0x3u << DMAC_CTRLB_DST_INCR_Pos) /**< \brief (DMAC_CTRLB)  */
#define DMAC_CTRLB_DST_INCR(value) ((DMAC_CTRLB_DST_INCR_Msk & ((value) << DMAC_CTRLB_DST_INCR_Pos)))
#define DMAC_CTRLB_IEN (0x1u << 30) /**< \brief (DMAC_CTRLB)  */
/* -------- DMAC_CFG : (DMAC Offset: N/A) DMAC Channel Configuration Register -------- */
#define DMAC_CFG_SRC_PER_Pos 0
#define DMAC_CFG_SRC_PER_Msk (0xfu << DMAC_CFG_SRC_PER_Pos) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_SRC_PER(value) ((DMAC_CFG_SRC_PER_Msk & ((value) << DMAC_CFG_SRC_PER_Pos)))
#define DMAC_CFG_DST_PER_Pos 4
#define DMAC_CFG_DST_PER_Msk (0xfu << DMAC_CFG_DST_PER_Pos) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_DST_PER(value) ((DMAC_CFG_DST_PER_Msk & ((value) << DMAC_CFG_DST_PER_Pos)))
#define DMAC_CFG_SRC_H2SEL (0x1u << 9) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_DST_H2SEL (0x1u << 13) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_SOD (0x1u << 16) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_LOCK_IF (0x1u << 20) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_LOCK_B (0x1u << 21) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_LOCK_IF_L (0x1u << 22) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_AHB_PROT_Pos 24
#define DMAC_CFG_AHB_PROT_Msk (0x7u << DMAC_CFG_AHB_PROT_Pos) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_AHB_PROT(value) ((DMAC_CFG_AHB_PROT_Msk & ((value) << DMAC_CFG_AHB_PROT_Pos)))
#define DMAC_CFG_FIFOCFG_Pos 28
#define DMAC_CFG_FIFOCFG_Msk (0x3u << DMAC_CFG_FIFOCFG_Pos) /**< \brief (DMAC_CFG)  */
#define DMAC_CFG_FIFOCFG(value) ((DMAC_CFG_FIFOCFG_Msk & ((value) << DMAC_CFG_FIFOCFG_Pos)))

/*@}*/


#endif /* _SAM3U_DMAC_COMPONENT_ */
