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

#ifndef _SAM3U_UDPHS_COMPONENT_
#define _SAM3U_UDPHS_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR USB High Speed Device Port */
/* ============================================================================= */
/** \addtogroup SAM3U_UDPHS USB High Speed Device Port */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief UdphsDma hardware registers */
typedef struct {
  RwReg     UDPHS_DMANXTDSC;  /**< \brief (UdphsDma Offset: 0x0) UDPHS DMA Next Descriptor Address Register */
  RwReg     UDPHS_DMAADDRESS; /**< \brief (UdphsDma Offset: 0x4) UDPHS DMA Channel Address Register */
  RwReg     UDPHS_DMACONTROL; /**< \brief (UdphsDma Offset: 0x8) UDPHS DMA Channel Control Register */
  RwReg     UDPHS_DMASTATUS;  /**< \brief (UdphsDma Offset: 0xC) UDPHS DMA Channel Status Register */
} UdphsDma;
/** \brief UdphsEpt hardware registers */
typedef struct {
  RwReg     UDPHS_EPTCFG;     /**< \brief (UdphsEpt Offset: 0x0) UDPHS Endpoint Configuration Register */
  RwReg     UDPHS_EPTCTLENB;  /**< \brief (UdphsEpt Offset: 0x4) UDPHS Endpoint Control Enable Register */
  RwReg     UDPHS_EPTCTLDIS;  /**< \brief (UdphsEpt Offset: 0x8) UDPHS Endpoint Control Disable Register */
  RwReg     UDPHS_EPTCTL;     /**< \brief (UdphsEpt Offset: 0xC) UDPHS Endpoint Control Register */
  RoReg     Reserved1[1];
  RwReg     UDPHS_EPTSETSTA;  /**< \brief (UdphsEpt Offset: 0x14) UDPHS Endpoint Set Status Register */
  RwReg     UDPHS_EPTCLRSTA;  /**< \brief (UdphsEpt Offset: 0x18) UDPHS Endpoint Clear Status Register */
  RwReg     UDPHS_EPTSTA;     /**< \brief (UdphsEpt Offset: 0x1C) UDPHS Endpoint Status Register */
} UdphsEpt;
/** \brief Udphs hardware registers */
#define UDPHSEPT_NUMBER 7
#define UDPHSDMA_NUMBER 6
typedef struct {
  RwReg     UDPHS_CTRL;       /**< \brief (Udphs Offset: 0x00) UDPHS Control Register */
  RoReg     UDPHS_FNUM;       /**< \brief (Udphs Offset: 0x04) UDPHS Frame Number Register */
  RoReg     Reserved1[2];
  RwReg     UDPHS_IEN;        /**< \brief (Udphs Offset: 0x10) UDPHS Interrupt Enable Register */
  RoReg     UDPHS_INTSTA;     /**< \brief (Udphs Offset: 0x14) UDPHS Interrupt Status Register */
  WoReg     UDPHS_CLRINT;     /**< \brief (Udphs Offset: 0x18) UDPHS Clear Interrupt Register */
  WoReg     UDPHS_EPTRST;     /**< \brief (Udphs Offset: 0x1C) UDPHS Endpoints Reset Register */
  RoReg     Reserved2[48];
  RwReg     UDPHS_TST;        /**< \brief (Udphs Offset: 0xE0) UDPHS Test Register */
  RoReg     Reserved3[3];
  RoReg     UDPHS_IPNAME1;    /**< \brief (Udphs Offset: 0xF0) UDPHS Name1 Register */
  RoReg     UDPHS_IPNAME2;    /**< \brief (Udphs Offset: 0xF4) UDPHS Name2 Register */
  RoReg     UDPHS_IPFEATURES; /**< \brief (Udphs Offset: 0xF8) UDPHS Features Register */
  RoReg     Reserved4[1];
  UdphsEpt  UDPHS_EPT[UDPHSEPT_NUMBER]; /**< \brief (Udphs Offset: 0x100) endpoint = 0 .. 6 */
  RoReg     Reserved5[72];
  UdphsDma  UDPHS_DMA[UDPHSDMA_NUMBER]; /**< \brief (Udphs Offset: 0x300) channel = 0 .. 5 */
} Udphs;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- UDPHS_CTRL : (UDPHS Offset: 0x00) UDPHS Control Register -------- */
#define UDPHS_CTRL_DEV_ADDR_Pos 0
#define UDPHS_CTRL_DEV_ADDR_Msk (0x7fu << UDPHS_CTRL_DEV_ADDR_Pos) /**< \brief (UDPHS_CTRL) UDPHS Address */
#define UDPHS_CTRL_DEV_ADDR(value) ((UDPHS_CTRL_DEV_ADDR_Msk & ((value) << UDPHS_CTRL_DEV_ADDR_Pos)))
#define UDPHS_CTRL_FADDR_EN (0x1u << 7) /**< \brief (UDPHS_CTRL) Function Address Enable */
#define UDPHS_CTRL_EN_UDPHS (0x1u << 8) /**< \brief (UDPHS_CTRL) UDPHS Enable */
#define UDPHS_CTRL_DETACH (0x1u << 9) /**< \brief (UDPHS_CTRL) Detach Command */
#define UDPHS_CTRL_REWAKEUP (0x1u << 10) /**< \brief (UDPHS_CTRL) Send Remote Wake Up */
#define UDPHS_CTRL_PULLD_DIS (0x1u << 11) /**< \brief (UDPHS_CTRL) Pull-Down Disable */
/* -------- UDPHS_FNUM : (UDPHS Offset: 0x04) UDPHS Frame Number Register -------- */
#define UDPHS_FNUM_MICRO_FRAME_NUM_Pos 0
#define UDPHS_FNUM_MICRO_FRAME_NUM_Msk (0x7u << UDPHS_FNUM_MICRO_FRAME_NUM_Pos) /**< \brief (UDPHS_FNUM) Microframe Number */
#define UDPHS_FNUM_FRAME_NUMBER_Pos 3
#define UDPHS_FNUM_FRAME_NUMBER_Msk (0x7ffu << UDPHS_FNUM_FRAME_NUMBER_Pos) /**< \brief (UDPHS_FNUM) Frame Number as defined in the Packet Field Formats */
#define UDPHS_FNUM_FNUM_ERR (0x1u << 31) /**< \brief (UDPHS_FNUM) Frame Number CRC Error */
/* -------- UDPHS_IEN : (UDPHS Offset: 0x10) UDPHS Interrupt Enable Register -------- */
#define UDPHS_IEN_DET_SUSPD (0x1u << 1) /**< \brief (UDPHS_IEN) Suspend Interrupt Enable */
#define UDPHS_IEN_MICRO_SOF (0x1u << 2) /**< \brief (UDPHS_IEN) Micro-SOF Interrupt Enable */
#define UDPHS_IEN_INT_SOF (0x1u << 3) /**< \brief (UDPHS_IEN) SOF Interrupt Enable */
#define UDPHS_IEN_ENDRESET (0x1u << 4) /**< \brief (UDPHS_IEN) End Of Reset Interrupt Enable */
#define UDPHS_IEN_WAKE_UP (0x1u << 5) /**< \brief (UDPHS_IEN) Wake Up CPU Interrupt Enable */
#define UDPHS_IEN_ENDOFRSM (0x1u << 6) /**< \brief (UDPHS_IEN) End Of Resume Interrupt Enable */
#define UDPHS_IEN_UPSTR_RES (0x1u << 7) /**< \brief (UDPHS_IEN) Upstream Resume Interrupt Enable */
#define UDPHS_IEN_EPT_0 (0x1u << 8) /**< \brief (UDPHS_IEN) Endpoint 0 Interrupt Enable */
#define UDPHS_IEN_EPT_1 (0x1u << 9) /**< \brief (UDPHS_IEN) Endpoint 1 Interrupt Enable */
#define UDPHS_IEN_EPT_2 (0x1u << 10) /**< \brief (UDPHS_IEN) Endpoint 2 Interrupt Enable */
#define UDPHS_IEN_EPT_3 (0x1u << 11) /**< \brief (UDPHS_IEN) Endpoint 3 Interrupt Enable */
#define UDPHS_IEN_EPT_4 (0x1u << 12) /**< \brief (UDPHS_IEN) Endpoint 4 Interrupt Enable */
#define UDPHS_IEN_EPT_5 (0x1u << 13) /**< \brief (UDPHS_IEN) Endpoint 5 Interrupt Enable */
#define UDPHS_IEN_EPT_6 (0x1u << 14) /**< \brief (UDPHS_IEN) Endpoint 6 Interrupt Enable */
#define UDPHS_IEN_DMA_1 (0x1u << 25) /**< \brief (UDPHS_IEN) DMA Channel 1 Interrupt Enable */
#define UDPHS_IEN_DMA_2 (0x1u << 26) /**< \brief (UDPHS_IEN) DMA Channel 2 Interrupt Enable */
#define UDPHS_IEN_DMA_3 (0x1u << 27) /**< \brief (UDPHS_IEN) DMA Channel 3 Interrupt Enable */
#define UDPHS_IEN_DMA_4 (0x1u << 28) /**< \brief (UDPHS_IEN) DMA Channel 4 Interrupt Enable */
#define UDPHS_IEN_DMA_5 (0x1u << 29) /**< \brief (UDPHS_IEN) DMA Channel 5 Interrupt Enable */
#define UDPHS_IEN_DMA_6 (0x1u << 30) /**< \brief (UDPHS_IEN) DMA Channel 6 Interrupt Enable */
/* -------- UDPHS_INTSTA : (UDPHS Offset: 0x14) UDPHS Interrupt Status Register -------- */
#define UDPHS_INTSTA_SPEED (0x1u << 0) /**< \brief (UDPHS_INTSTA) Speed Status */
#define UDPHS_INTSTA_DET_SUSPD (0x1u << 1) /**< \brief (UDPHS_INTSTA) Suspend Interrupt */
#define UDPHS_INTSTA_MICRO_SOF (0x1u << 2) /**< \brief (UDPHS_INTSTA) Micro Start Of Frame Interrupt */
#define UDPHS_INTSTA_INT_SOF (0x1u << 3) /**< \brief (UDPHS_INTSTA) Start Of Frame Interrupt */
#define UDPHS_INTSTA_ENDRESET (0x1u << 4) /**< \brief (UDPHS_INTSTA) End Of Reset Interrupt */
#define UDPHS_INTSTA_WAKE_UP (0x1u << 5) /**< \brief (UDPHS_INTSTA) Wake Up CPU Interrupt */
#define UDPHS_INTSTA_ENDOFRSM (0x1u << 6) /**< \brief (UDPHS_INTSTA) End Of Resume Interrupt */
#define UDPHS_INTSTA_UPSTR_RES (0x1u << 7) /**< \brief (UDPHS_INTSTA) Upstream Resume Interrupt */
#define UDPHS_INTSTA_EPT_0 (0x1u << 8) /**< \brief (UDPHS_INTSTA) Endpoint 0 Interrupt */
#define UDPHS_INTSTA_EPT_1 (0x1u << 9) /**< \brief (UDPHS_INTSTA) Endpoint 1 Interrupt */
#define UDPHS_INTSTA_EPT_2 (0x1u << 10) /**< \brief (UDPHS_INTSTA) Endpoint 2 Interrupt */
#define UDPHS_INTSTA_EPT_3 (0x1u << 11) /**< \brief (UDPHS_INTSTA) Endpoint 3 Interrupt */
#define UDPHS_INTSTA_EPT_4 (0x1u << 12) /**< \brief (UDPHS_INTSTA) Endpoint 4 Interrupt */
#define UDPHS_INTSTA_EPT_5 (0x1u << 13) /**< \brief (UDPHS_INTSTA) Endpoint 5 Interrupt */
#define UDPHS_INTSTA_EPT_6 (0x1u << 14) /**< \brief (UDPHS_INTSTA) Endpoint 6 Interrupt */
#define UDPHS_INTSTA_DMA_1 (0x1u << 25) /**< \brief (UDPHS_INTSTA) DMA Channel 1 Interrupt */
#define UDPHS_INTSTA_DMA_2 (0x1u << 26) /**< \brief (UDPHS_INTSTA) DMA Channel 2 Interrupt */
#define UDPHS_INTSTA_DMA_3 (0x1u << 27) /**< \brief (UDPHS_INTSTA) DMA Channel 3 Interrupt */
#define UDPHS_INTSTA_DMA_4 (0x1u << 28) /**< \brief (UDPHS_INTSTA) DMA Channel 4 Interrupt */
#define UDPHS_INTSTA_DMA_5 (0x1u << 29) /**< \brief (UDPHS_INTSTA) DMA Channel 5 Interrupt */
#define UDPHS_INTSTA_DMA_6 (0x1u << 30) /**< \brief (UDPHS_INTSTA) DMA Channel 6 Interrupt */
/* -------- UDPHS_CLRINT : (UDPHS Offset: 0x18) UDPHS Clear Interrupt Register -------- */
#define UDPHS_CLRINT_DET_SUSPD (0x1u << 1) /**< \brief (UDPHS_CLRINT) Suspend Interrupt Clear */
#define UDPHS_CLRINT_MICRO_SOF (0x1u << 2) /**< \brief (UDPHS_CLRINT) Micro Start Of Frame Interrupt Clear */
#define UDPHS_CLRINT_INT_SOF (0x1u << 3) /**< \brief (UDPHS_CLRINT) Start Of Frame Interrupt Clear */
#define UDPHS_CLRINT_ENDRESET (0x1u << 4) /**< \brief (UDPHS_CLRINT) End Of Reset Interrupt Clear */
#define UDPHS_CLRINT_WAKE_UP (0x1u << 5) /**< \brief (UDPHS_CLRINT) Wake Up CPU Interrupt Clear */
#define UDPHS_CLRINT_ENDOFRSM (0x1u << 6) /**< \brief (UDPHS_CLRINT) End Of Resume Interrupt Clear */
#define UDPHS_CLRINT_UPSTR_RES (0x1u << 7) /**< \brief (UDPHS_CLRINT) Upstream Resume Interrupt Clear */
/* -------- UDPHS_EPTRST : (UDPHS Offset: 0x1C) UDPHS Endpoints Reset Register -------- */
#define UDPHS_EPTRST_EPT_0 (0x1u << 0) /**< \brief (UDPHS_EPTRST) Endpoint 0 Reset */
#define UDPHS_EPTRST_EPT_1 (0x1u << 1) /**< \brief (UDPHS_EPTRST) Endpoint 1 Reset */
#define UDPHS_EPTRST_EPT_2 (0x1u << 2) /**< \brief (UDPHS_EPTRST) Endpoint 2 Reset */
#define UDPHS_EPTRST_EPT_3 (0x1u << 3) /**< \brief (UDPHS_EPTRST) Endpoint 3 Reset */
#define UDPHS_EPTRST_EPT_4 (0x1u << 4) /**< \brief (UDPHS_EPTRST) Endpoint 4 Reset */
#define UDPHS_EPTRST_EPT_5 (0x1u << 5) /**< \brief (UDPHS_EPTRST) Endpoint 5 Reset */
#define UDPHS_EPTRST_EPT_6 (0x1u << 6) /**< \brief (UDPHS_EPTRST) Endpoint 6 Reset */
/* -------- UDPHS_TST : (UDPHS Offset: 0xE0) UDPHS Test Register -------- */
#define UDPHS_TST_SPEED_CFG_Pos 0
#define UDPHS_TST_SPEED_CFG_Msk (0x3u << UDPHS_TST_SPEED_CFG_Pos) /**< \brief (UDPHS_TST) Speed Configuration */
#define   UDPHS_TST_SPEED_CFG_NORMAL (0x0u << 0) /**< \brief (UDPHS_TST) Normal Mode: The macro is in Full Speed mode, ready to make a High Speed identification, if the host supports it and then to automatically switch to High Speed mode */
#define   UDPHS_TST_SPEED_CFG_HIGH_SPEED (0x2u << 0) /**< \brief (UDPHS_TST) Force High Speed: Set this value to force the hardware to work in High Speed mode. Only for debug or test purpose. */
#define   UDPHS_TST_SPEED_CFG_FULL_SPEED (0x3u << 0) /**< \brief (UDPHS_TST) Force Full Speed: Set this value to force the hardware to work only in Full Speed mode. In this configuration, the macro will not respond to a High Speed reset handshake. */
#define UDPHS_TST_TST_J (0x1u << 2) /**< \brief (UDPHS_TST) Test J Mode */
#define UDPHS_TST_TST_K (0x1u << 3) /**< \brief (UDPHS_TST) Test K Mode */
#define UDPHS_TST_TST_PKT (0x1u << 4) /**< \brief (UDPHS_TST) Test Packet Mode */
#define UDPHS_TST_OPMODE2 (0x1u << 5) /**< \brief (UDPHS_TST) OpMode2 */
/* -------- UDPHS_IPNAME1 : (UDPHS Offset: 0xF0) UDPHS Name1 Register -------- */
#define UDPHS_IPNAME1_IP_NAME1_Pos 0
#define UDPHS_IPNAME1_IP_NAME1_Msk (0xffffffffu << UDPHS_IPNAME1_IP_NAME1_Pos) /**< \brief (UDPHS_IPNAME1)  */
/* -------- UDPHS_IPNAME2 : (UDPHS Offset: 0xF4) UDPHS Name2 Register -------- */
#define UDPHS_IPNAME2_IP_NAME2_Pos 0
#define UDPHS_IPNAME2_IP_NAME2_Msk (0xffffffffu << UDPHS_IPNAME2_IP_NAME2_Pos) /**< \brief (UDPHS_IPNAME2)  */
/* -------- UDPHS_IPFEATURES : (UDPHS Offset: 0xF8) UDPHS Features Register -------- */
#define UDPHS_IPFEATURES_EPT_NBR_MAX_Pos 0
#define UDPHS_IPFEATURES_EPT_NBR_MAX_Msk (0xfu << UDPHS_IPFEATURES_EPT_NBR_MAX_Pos) /**< \brief (UDPHS_IPFEATURES) Max Number of Endpoints */
#define UDPHS_IPFEATURES_DMA_CHANNEL_NBR_Pos 4
#define UDPHS_IPFEATURES_DMA_CHANNEL_NBR_Msk (0x7u << UDPHS_IPFEATURES_DMA_CHANNEL_NBR_Pos) /**< \brief (UDPHS_IPFEATURES) Number of DMA Channels */
#define UDPHS_IPFEATURES_DMA_B_SIZ (0x1u << 7) /**< \brief (UDPHS_IPFEATURES) DMA Buffer Size */
#define UDPHS_IPFEATURES_DMA_FIFO_WORD_DEPTH_Pos 8
#define UDPHS_IPFEATURES_DMA_FIFO_WORD_DEPTH_Msk (0xfu << UDPHS_IPFEATURES_DMA_FIFO_WORD_DEPTH_Pos) /**< \brief (UDPHS_IPFEATURES) DMA FIFO Depth in Words */
#define UDPHS_IPFEATURES_FIFO_MAX_SIZE_Pos 12
#define UDPHS_IPFEATURES_FIFO_MAX_SIZE_Msk (0x7u << UDPHS_IPFEATURES_FIFO_MAX_SIZE_Pos) /**< \brief (UDPHS_IPFEATURES) DPRAM Size */
#define UDPHS_IPFEATURES_BW_DPRAM (0x1u << 15) /**< \brief (UDPHS_IPFEATURES) DPRAM Byte Write Capability */
#define UDPHS_IPFEATURES_DATAB16_8 (0x1u << 16) /**< \brief (UDPHS_IPFEATURES) UTMI DataBus16_8 */
#define UDPHS_IPFEATURES_ISO_EPT_1 (0x1u << 17) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_2 (0x1u << 18) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_3 (0x1u << 19) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_4 (0x1u << 20) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_5 (0x1u << 21) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_6 (0x1u << 22) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_7 (0x1u << 23) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_8 (0x1u << 24) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_9 (0x1u << 25) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_10 (0x1u << 26) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_11 (0x1u << 27) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_12 (0x1u << 28) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_13 (0x1u << 29) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_14 (0x1u << 30) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
#define UDPHS_IPFEATURES_ISO_EPT_15 (0x1u << 31) /**< \brief (UDPHS_IPFEATURES) Endpointx High Bandwidth Isochronous Capability */
/* -------- UDPHS_EPTCFG : (UDPHS Offset: N/A) UDPHS Endpoint Configuration Register -------- */
#define UDPHS_EPTCFG_EPT_SIZE_Pos 0
#define UDPHS_EPTCFG_EPT_SIZE_Msk (0x7u << UDPHS_EPTCFG_EPT_SIZE_Pos) /**< \brief (UDPHS_EPTCFG) Endpoint Size */
#define   UDPHS_EPTCFG_EPT_SIZE_8 (0x0u << 0) /**< \brief (UDPHS_EPTCFG) 8 bytes */
#define   UDPHS_EPTCFG_EPT_SIZE_16 (0x1u << 0) /**< \brief (UDPHS_EPTCFG) 16 bytes */
#define   UDPHS_EPTCFG_EPT_SIZE_32 (0x2u << 0) /**< \brief (UDPHS_EPTCFG) 32 bytes */
#define   UDPHS_EPTCFG_EPT_SIZE_64 (0x3u << 0) /**< \brief (UDPHS_EPTCFG) 64 bytes */
#define   UDPHS_EPTCFG_EPT_SIZE_128 (0x4u << 0) /**< \brief (UDPHS_EPTCFG) 128 bytes */
#define   UDPHS_EPTCFG_EPT_SIZE_256 (0x5u << 0) /**< \brief (UDPHS_EPTCFG) 256 bytes */
#define   UDPHS_EPTCFG_EPT_SIZE_512 (0x6u << 0) /**< \brief (UDPHS_EPTCFG) 512 bytes */
#define   UDPHS_EPTCFG_EPT_SIZE_1024 (0x7u << 0) /**< \brief (UDPHS_EPTCFG) 1024 bytes */
#define UDPHS_EPTCFG_EPT_DIR (0x1u << 3) /**< \brief (UDPHS_EPTCFG) Endpoint Direction */
#define UDPHS_EPTCFG_EPT_TYPE_Pos 4
#define UDPHS_EPTCFG_EPT_TYPE_Msk (0x3u << UDPHS_EPTCFG_EPT_TYPE_Pos) /**< \brief (UDPHS_EPTCFG) Endpoint Type */
#define   UDPHS_EPTCFG_EPT_TYPE_CTRL8 (0x0u << 4) /**< \brief (UDPHS_EPTCFG) Control endpoint */
#define   UDPHS_EPTCFG_EPT_TYPE_ISO (0x1u << 4) /**< \brief (UDPHS_EPTCFG) Isochronous endpoint */
#define   UDPHS_EPTCFG_EPT_TYPE_BULK (0x2u << 4) /**< \brief (UDPHS_EPTCFG) Bulk endpoint */
#define   UDPHS_EPTCFG_EPT_TYPE_INT (0x3u << 4) /**< \brief (UDPHS_EPTCFG) Interrupt endpoint */
#define UDPHS_EPTCFG_BK_NUMBER_Pos 6
#define UDPHS_EPTCFG_BK_NUMBER_Msk (0x3u << UDPHS_EPTCFG_BK_NUMBER_Pos) /**< \brief (UDPHS_EPTCFG) Number of Banks */
#define   UDPHS_EPTCFG_BK_NUMBER_0 (0x0u << 6) /**< \brief (UDPHS_EPTCFG) Zero bank, the endpoint is not mapped in memory */
#define   UDPHS_EPTCFG_BK_NUMBER_1 (0x1u << 6) /**< \brief (UDPHS_EPTCFG) One bank (bank 0) */
#define   UDPHS_EPTCFG_BK_NUMBER_2 (0x2u << 6) /**< \brief (UDPHS_EPTCFG) Double bank (Ping-Pong: bank0/bank1) */
#define   UDPHS_EPTCFG_BK_NUMBER_3 (0x3u << 6) /**< \brief (UDPHS_EPTCFG) Triple bank (bank0/bank1/bank2) */
#define UDPHS_EPTCFG_NB_TRANS_Pos 8
#define UDPHS_EPTCFG_NB_TRANS_Msk (0x3u << UDPHS_EPTCFG_NB_TRANS_Pos) /**< \brief (UDPHS_EPTCFG) Number Of Transaction per Microframe */
#define UDPHS_EPTCFG_NB_TRANS(value) ((UDPHS_EPTCFG_NB_TRANS_Msk & ((value) << UDPHS_EPTCFG_NB_TRANS_Pos)))
#define UDPHS_EPTCFG_EPT_MAPD (0x1u << 31) /**< \brief (UDPHS_EPTCFG) Endpoint Mapped */
/* -------- UDPHS_EPTCTLENB : (UDPHS Offset: N/A) UDPHS Endpoint Control Enable Register -------- */
#define UDPHS_EPTCTLENB_EPT_ENABL (0x1u << 0) /**< \brief (UDPHS_EPTCTLENB) Endpoint Enable */
#define UDPHS_EPTCTLENB_AUTO_VALID (0x1u << 1) /**< \brief (UDPHS_EPTCTLENB) Packet Auto-Valid Enable */
#define UDPHS_EPTCTLENB_INTDIS_DMA (0x1u << 3) /**< \brief (UDPHS_EPTCTLENB) Interrupts Disable DMA */
#define UDPHS_EPTCTLENB_NYET_DIS (0x1u << 4) /**< \brief (UDPHS_EPTCTLENB) NYET Disable (Only for High Speed Bulk OUT endpoints) */
#define UDPHS_EPTCTLENB_DATAX_RX (0x1u << 6) /**< \brief (UDPHS_EPTCTLENB) DATAx Interrupt Enable (Only for high bandwidth Isochronous OUT endpoints) */
#define UDPHS_EPTCTLENB_MDATA_RX (0x1u << 7) /**< \brief (UDPHS_EPTCTLENB) MDATA Interrupt Enable (Only for high bandwidth Isochronous OUT endpoints) */
#define UDPHS_EPTCTLENB_ERR_OVFLW (0x1u << 8) /**< \brief (UDPHS_EPTCTLENB) Overflow Error Interrupt Enable */
#define UDPHS_EPTCTLENB_RX_BK_RDY (0x1u << 9) /**< \brief (UDPHS_EPTCTLENB) Received OUT Data Interrupt Enable */
#define UDPHS_EPTCTLENB_TX_COMPLT (0x1u << 10) /**< \brief (UDPHS_EPTCTLENB) Transmitted IN Data Complete Interrupt Enable */
#define UDPHS_EPTCTLENB_TX_PK_RDY (0x1u << 11) /**< \brief (UDPHS_EPTCTLENB) TX Packet Ready/Transaction Error Interrupt Enable */
#define UDPHS_EPTCTLENB_ERR_TRANS (0x1u << 11) /**< \brief (UDPHS_EPTCTLENB) TX Packet Ready/Transaction Error Interrupt Enable */
#define UDPHS_EPTCTLENB_RX_SETUP (0x1u << 12) /**< \brief (UDPHS_EPTCTLENB) Received SETUP/Error Flow Interrupt Enable */
#define UDPHS_EPTCTLENB_ERR_FL_ISO (0x1u << 12) /**< \brief (UDPHS_EPTCTLENB) Received SETUP/Error Flow Interrupt Enable */
#define UDPHS_EPTCTLENB_STALL_SNT (0x1u << 13) /**< \brief (UDPHS_EPTCTLENB) Stall Sent /ISO CRC Error/Number of Transaction Error Interrupt Enable */
#define UDPHS_EPTCTLENB_ERR_CRISO (0x1u << 13) /**< \brief (UDPHS_EPTCTLENB) Stall Sent /ISO CRC Error/Number of Transaction Error Interrupt Enable */
#define UDPHS_EPTCTLENB_ERR_NBTRA (0x1u << 13) /**< \brief (UDPHS_EPTCTLENB) Stall Sent /ISO CRC Error/Number of Transaction Error Interrupt Enable */
#define UDPHS_EPTCTLENB_NAK_IN (0x1u << 14) /**< \brief (UDPHS_EPTCTLENB) NAKIN/Bank Flush Error Interrupt Enable */
#define UDPHS_EPTCTLENB_ERR_FLUSH (0x1u << 14) /**< \brief (UDPHS_EPTCTLENB) NAKIN/Bank Flush Error Interrupt Enable */
#define UDPHS_EPTCTLENB_NAK_OUT (0x1u << 15) /**< \brief (UDPHS_EPTCTLENB) NAKOUT Interrupt Enable */
#define UDPHS_EPTCTLENB_BUSY_BANK (0x1u << 18) /**< \brief (UDPHS_EPTCTLENB) Busy Bank Interrupt Enable */
#define UDPHS_EPTCTLENB_SHRT_PCKT (0x1u << 31) /**< \brief (UDPHS_EPTCTLENB) Short Packet Send/Short Packet Interrupt Enable */
/* -------- UDPHS_EPTCTLDIS : (UDPHS Offset: N/A) UDPHS Endpoint Control Disable Register -------- */
#define UDPHS_EPTCTLDIS_EPT_DISABL (0x1u << 0) /**< \brief (UDPHS_EPTCTLDIS) Endpoint Disable */
#define UDPHS_EPTCTLDIS_AUTO_VALID (0x1u << 1) /**< \brief (UDPHS_EPTCTLDIS) Packet Auto-Valid Disable */
#define UDPHS_EPTCTLDIS_INTDIS_DMA (0x1u << 3) /**< \brief (UDPHS_EPTCTLDIS) Interrupts Disable DMA */
#define UDPHS_EPTCTLDIS_NYET_DIS (0x1u << 4) /**< \brief (UDPHS_EPTCTLDIS) NYET Enable (Only for High Speed Bulk OUT endpoints) */
#define UDPHS_EPTCTLDIS_DATAX_RX (0x1u << 6) /**< \brief (UDPHS_EPTCTLDIS) DATAx Interrupt Disable (Only for High Bandwidth Isochronous OUT endpoints) */
#define UDPHS_EPTCTLDIS_MDATA_RX (0x1u << 7) /**< \brief (UDPHS_EPTCTLDIS) MDATA Interrupt Disable (Only for High Bandwidth Isochronous OUT endpoints) */
#define UDPHS_EPTCTLDIS_ERR_OVFLW (0x1u << 8) /**< \brief (UDPHS_EPTCTLDIS) Overflow Error Interrupt Disable */
#define UDPHS_EPTCTLDIS_RX_BK_RDY (0x1u << 9) /**< \brief (UDPHS_EPTCTLDIS) Received OUT Data Interrupt Disable */
#define UDPHS_EPTCTLDIS_TX_COMPLT (0x1u << 10) /**< \brief (UDPHS_EPTCTLDIS) Transmitted IN Data Complete Interrupt Disable */
#define UDPHS_EPTCTLDIS_TX_PK_RDY (0x1u << 11) /**< \brief (UDPHS_EPTCTLDIS) TX Packet Ready/Transaction Error Interrupt Disable */
#define UDPHS_EPTCTLDIS_ERR_TRANS (0x1u << 11) /**< \brief (UDPHS_EPTCTLDIS) TX Packet Ready/Transaction Error Interrupt Disable */
#define UDPHS_EPTCTLDIS_RX_SETUP (0x1u << 12) /**< \brief (UDPHS_EPTCTLDIS) Received SETUP/Error Flow Interrupt Disable */
#define UDPHS_EPTCTLDIS_ERR_FL_ISO (0x1u << 12) /**< \brief (UDPHS_EPTCTLDIS) Received SETUP/Error Flow Interrupt Disable */
#define UDPHS_EPTCTLDIS_STALL_SNT (0x1u << 13) /**< \brief (UDPHS_EPTCTLDIS) Stall Sent/ISO CRC Error/Number of Transaction Error Interrupt Disable */
#define UDPHS_EPTCTLDIS_ERR_CRISO (0x1u << 13) /**< \brief (UDPHS_EPTCTLDIS) Stall Sent/ISO CRC Error/Number of Transaction Error Interrupt Disable */
#define UDPHS_EPTCTLDIS_ERR_NBTRA (0x1u << 13) /**< \brief (UDPHS_EPTCTLDIS) Stall Sent/ISO CRC Error/Number of Transaction Error Interrupt Disable */
#define UDPHS_EPTCTLDIS_NAK_IN (0x1u << 14) /**< \brief (UDPHS_EPTCTLDIS) NAKIN/bank flush error Interrupt Disable */
#define UDPHS_EPTCTLDIS_ERR_FLUSH (0x1u << 14) /**< \brief (UDPHS_EPTCTLDIS) NAKIN/bank flush error Interrupt Disable */
#define UDPHS_EPTCTLDIS_NAK_OUT (0x1u << 15) /**< \brief (UDPHS_EPTCTLDIS) NAKOUT Interrupt Disable */
#define UDPHS_EPTCTLDIS_BUSY_BANK (0x1u << 18) /**< \brief (UDPHS_EPTCTLDIS) Busy Bank Interrupt Disable */
#define UDPHS_EPTCTLDIS_SHRT_PCKT (0x1u << 31) /**< \brief (UDPHS_EPTCTLDIS) Short Packet Interrupt Disable */
/* -------- UDPHS_EPTCTL : (UDPHS Offset: N/A) UDPHS Endpoint Control Register -------- */
#define UDPHS_EPTCTL_EPT_ENABL (0x1u << 0) /**< \brief (UDPHS_EPTCTL) Endpoint Enable */
#define UDPHS_EPTCTL_AUTO_VALID (0x1u << 1) /**< \brief (UDPHS_EPTCTL) Packet Auto-Valid Enabled (Not for CONTROL Endpoints) */
#define UDPHS_EPTCTL_INTDIS_DMA (0x1u << 3) /**< \brief (UDPHS_EPTCTL) Interrupt Disables DMA */
#define UDPHS_EPTCTL_NYET_DIS (0x1u << 4) /**< \brief (UDPHS_EPTCTL) NYET Disable (Only for High Speed Bulk OUT endpoints) */
#define UDPHS_EPTCTL_DATAX_RX (0x1u << 6) /**< \brief (UDPHS_EPTCTL) DATAx Interrupt Enabled (Only for High Bandwidth Isochronous OUT endpoints) */
#define UDPHS_EPTCTL_MDATA_RX (0x1u << 7) /**< \brief (UDPHS_EPTCTL) MDATA Interrupt Enabled (Only for High Bandwidth Isochronous OUT endpoints) */
#define UDPHS_EPTCTL_ERR_OVFLW (0x1u << 8) /**< \brief (UDPHS_EPTCTL) Overflow Error Interrupt Enabled */
#define UDPHS_EPTCTL_RX_BK_RDY (0x1u << 9) /**< \brief (UDPHS_EPTCTL) Received OUT Data Interrupt Enabled */
#define UDPHS_EPTCTL_TX_COMPLT (0x1u << 10) /**< \brief (UDPHS_EPTCTL) Transmitted IN Data Complete Interrupt Enabled */
#define UDPHS_EPTCTL_TX_PK_RDY (0x1u << 11) /**< \brief (UDPHS_EPTCTL) TX Packet Ready/Transaction Error Interrupt Enabled */
#define UDPHS_EPTCTL_ERR_TRANS (0x1u << 11) /**< \brief (UDPHS_EPTCTL) TX Packet Ready/Transaction Error Interrupt Enabled */
#define UDPHS_EPTCTL_RX_SETUP (0x1u << 12) /**< \brief (UDPHS_EPTCTL) Received SETUP/Error Flow Interrupt Enabled */
#define UDPHS_EPTCTL_ERR_FL_ISO (0x1u << 12) /**< \brief (UDPHS_EPTCTL) Received SETUP/Error Flow Interrupt Enabled */
#define UDPHS_EPTCTL_STALL_SNT (0x1u << 13) /**< \brief (UDPHS_EPTCTL) Stall Sent/ISO CRC Error/Number of Transaction Error Interrupt Enabled */
#define UDPHS_EPTCTL_ERR_CRISO (0x1u << 13) /**< \brief (UDPHS_EPTCTL) Stall Sent/ISO CRC Error/Number of Transaction Error Interrupt Enabled */
#define UDPHS_EPTCTL_ERR_NBTRA (0x1u << 13) /**< \brief (UDPHS_EPTCTL) Stall Sent/ISO CRC Error/Number of Transaction Error Interrupt Enabled */
#define UDPHS_EPTCTL_NAK_IN (0x1u << 14) /**< \brief (UDPHS_EPTCTL) NAKIN/Bank Flush Error Interrupt Enabled */
#define UDPHS_EPTCTL_ERR_FLUSH (0x1u << 14) /**< \brief (UDPHS_EPTCTL) NAKIN/Bank Flush Error Interrupt Enabled */
#define UDPHS_EPTCTL_NAK_OUT (0x1u << 15) /**< \brief (UDPHS_EPTCTL) NAKOUT Interrupt Enabled */
#define UDPHS_EPTCTL_BUSY_BANK (0x1u << 18) /**< \brief (UDPHS_EPTCTL) Busy Bank Interrupt Enabled */
#define UDPHS_EPTCTL_SHRT_PCKT (0x1u << 31) /**< \brief (UDPHS_EPTCTL) Short Packet Interrupt Enabled */
/* -------- UDPHS_EPTSETSTA : (UDPHS Offset: N/A) UDPHS Endpoint Set Status Register -------- */
#define UDPHS_EPTSETSTA_FRCESTALL (0x1u << 5) /**< \brief (UDPHS_EPTSETSTA) Stall Handshake Request Set */
#define UDPHS_EPTSETSTA_KILL_BANK (0x1u << 9) /**< \brief (UDPHS_EPTSETSTA) KILL Bank Set (for IN Endpoint) */
#define UDPHS_EPTSETSTA_TX_PK_RDY (0x1u << 11) /**< \brief (UDPHS_EPTSETSTA) TX Packet Ready Set */
/* -------- UDPHS_EPTCLRSTA : (UDPHS Offset: N/A) UDPHS Endpoint Clear Status Register -------- */
#define UDPHS_EPTCLRSTA_FRCESTALL (0x1u << 5) /**< \brief (UDPHS_EPTCLRSTA) Stall Handshake Request Clear */
#define UDPHS_EPTCLRSTA_TOGGLESQ (0x1u << 6) /**< \brief (UDPHS_EPTCLRSTA) Data Toggle Clear */
#define UDPHS_EPTCLRSTA_RX_BK_RDY (0x1u << 9) /**< \brief (UDPHS_EPTCLRSTA) Received OUT Data Clear */
#define UDPHS_EPTCLRSTA_TX_COMPLT (0x1u << 10) /**< \brief (UDPHS_EPTCLRSTA) Transmitted IN Data Complete Clear */
#define UDPHS_EPTCLRSTA_RX_SETUP (0x1u << 12) /**< \brief (UDPHS_EPTCLRSTA) Received SETUP/Error Flow Clear */
#define UDPHS_EPTCLRSTA_ERR_FL_ISO (0x1u << 12) /**< \brief (UDPHS_EPTCLRSTA) Received SETUP/Error Flow Clear */
#define UDPHS_EPTCLRSTA_STALL_SNT (0x1u << 13) /**< \brief (UDPHS_EPTCLRSTA) Stall Sent/Number of Transaction Error Clear */
#define UDPHS_EPTCLRSTA_ERR_NBTRA (0x1u << 13) /**< \brief (UDPHS_EPTCLRSTA) Stall Sent/Number of Transaction Error Clear */
#define UDPHS_EPTCLRSTA_NAK_IN (0x1u << 14) /**< \brief (UDPHS_EPTCLRSTA) NAKIN/Bank Flush Error Clear */
#define UDPHS_EPTCLRSTA_ERR_FLUSH (0x1u << 14) /**< \brief (UDPHS_EPTCLRSTA) NAKIN/Bank Flush Error Clear */
#define UDPHS_EPTCLRSTA_NAK_OUT (0x1u << 15) /**< \brief (UDPHS_EPTCLRSTA) NAKOUT Clear */
/* -------- UDPHS_EPTSTA : (UDPHS Offset: N/A) UDPHS Endpoint Status Register -------- */
#define UDPHS_EPTSTA_FRCESTALL (0x1u << 5) /**< \brief (UDPHS_EPTSTA) Stall Handshake Request */
#define UDPHS_EPTSTA_TOGGLESQ_STA_Pos 6
#define UDPHS_EPTSTA_TOGGLESQ_STA_Msk (0x3u << UDPHS_EPTSTA_TOGGLESQ_STA_Pos) /**< \brief (UDPHS_EPTSTA) Toggle Sequencing */
#define   UDPHS_EPTSTA_TOGGLESQ_STA_DATA0 (0x0u << 6) /**< \brief (UDPHS_EPTSTA) DATA0 */
#define   UDPHS_EPTSTA_TOGGLESQ_STA_DATA1 (0x1u << 6) /**< \brief (UDPHS_EPTSTA) DATA1 */
#define   UDPHS_EPTSTA_TOGGLESQ_STA_DATA2 (0x2u << 6) /**< \brief (UDPHS_EPTSTA) Data2 (only for High Bandwidth Isochronous Endpoint) */
#define   UDPHS_EPTSTA_TOGGLESQ_STA_MDATA (0x3u << 6) /**< \brief (UDPHS_EPTSTA) MData (only for High Bandwidth Isochronous Endpoint) */
#define UDPHS_EPTSTA_ERR_OVFLW (0x1u << 8) /**< \brief (UDPHS_EPTSTA) Overflow Error */
#define UDPHS_EPTSTA_RX_BK_RDY (0x1u << 9) /**< \brief (UDPHS_EPTSTA) Received OUT Data/KILL Bank */
#define UDPHS_EPTSTA_KILL_BANK (0x1u << 9) /**< \brief (UDPHS_EPTSTA) Received OUT Data/KILL Bank */
#define UDPHS_EPTSTA_TX_COMPLT (0x1u << 10) /**< \brief (UDPHS_EPTSTA) Transmitted IN Data Complete */
#define UDPHS_EPTSTA_TX_PK_RDY (0x1u << 11) /**< \brief (UDPHS_EPTSTA) TX Packet Ready/Transaction Error */
#define UDPHS_EPTSTA_ERR_TRANS (0x1u << 11) /**< \brief (UDPHS_EPTSTA) TX Packet Ready/Transaction Error */
#define UDPHS_EPTSTA_RX_SETUP (0x1u << 12) /**< \brief (UDPHS_EPTSTA) Received SETUP/Error Flow */
#define UDPHS_EPTSTA_ERR_FL_ISO (0x1u << 12) /**< \brief (UDPHS_EPTSTA) Received SETUP/Error Flow */
#define UDPHS_EPTSTA_STALL_SNT (0x1u << 13) /**< \brief (UDPHS_EPTSTA) Stall Sent/CRC ISO Error/Number of Transaction Error */
#define UDPHS_EPTSTA_ERR_CRISO (0x1u << 13) /**< \brief (UDPHS_EPTSTA) Stall Sent/CRC ISO Error/Number of Transaction Error */
#define UDPHS_EPTSTA_ERR_NBTRA (0x1u << 13) /**< \brief (UDPHS_EPTSTA) Stall Sent/CRC ISO Error/Number of Transaction Error */
#define UDPHS_EPTSTA_NAK_IN (0x1u << 14) /**< \brief (UDPHS_EPTSTA) NAK IN/Bank Flush Error */
#define UDPHS_EPTSTA_ERR_FLUSH (0x1u << 14) /**< \brief (UDPHS_EPTSTA) NAK IN/Bank Flush Error */
#define UDPHS_EPTSTA_NAK_OUT (0x1u << 15) /**< \brief (UDPHS_EPTSTA) NAK OUT */
#define UDPHS_EPTSTA_CURRENT_BANK_Pos 16
#define UDPHS_EPTSTA_CURRENT_BANK_Msk (0x3u << UDPHS_EPTSTA_CURRENT_BANK_Pos) /**< \brief (UDPHS_EPTSTA) Current Bank/Control Direction */
#define UDPHS_EPTSTA_CONTROL_DIR_Pos 16
#define UDPHS_EPTSTA_CONTROL_DIR_Msk (0x3u << UDPHS_EPTSTA_CONTROL_DIR_Pos) /**< \brief (UDPHS_EPTSTA) Current Bank/Control Direction */
#define UDPHS_EPTSTA_BUSY_BANK_STA_Pos 18
#define UDPHS_EPTSTA_BUSY_BANK_STA_Msk (0x3u << UDPHS_EPTSTA_BUSY_BANK_STA_Pos) /**< \brief (UDPHS_EPTSTA) Busy Bank Number */
#define   UDPHS_EPTSTA_BUSY_BANK_STA_1BUSYBANK (0x0u << 18) /**< \brief (UDPHS_EPTSTA) 1 busy bank */
#define   UDPHS_EPTSTA_BUSY_BANK_STA_2BUSYBANKS (0x1u << 18) /**< \brief (UDPHS_EPTSTA) 2 busy banks */
#define   UDPHS_EPTSTA_BUSY_BANK_STA_3BUSYBANKS (0x2u << 18) /**< \brief (UDPHS_EPTSTA) 3 busy banks */
#define UDPHS_EPTSTA_BYTE_COUNT_Pos 20
#define UDPHS_EPTSTA_BYTE_COUNT_Msk (0x7ffu << UDPHS_EPTSTA_BYTE_COUNT_Pos) /**< \brief (UDPHS_EPTSTA) UDPHS Byte Count */
#define UDPHS_EPTSTA_SHRT_PCKT (0x1u << 31) /**< \brief (UDPHS_EPTSTA) Short Packet */
/* -------- UDPHS_DMANXTDSC : (UDPHS Offset: N/A) UDPHS DMA Next Descriptor Address Register -------- */
#define UDPHS_DMANXTDSC_NXT_DSC_ADD_Pos 0
#define UDPHS_DMANXTDSC_NXT_DSC_ADD_Msk (0xffffffffu << UDPHS_DMANXTDSC_NXT_DSC_ADD_Pos) /**< \brief (UDPHS_DMANXTDSC)  */
#define UDPHS_DMANXTDSC_NXT_DSC_ADD(value) ((UDPHS_DMANXTDSC_NXT_DSC_ADD_Msk & ((value) << UDPHS_DMANXTDSC_NXT_DSC_ADD_Pos)))
/* -------- UDPHS_DMAADDRESS : (UDPHS Offset: N/A) UDPHS DMA Channel Address Register -------- */
#define UDPHS_DMAADDRESS_BUFF_ADD_Pos 0
#define UDPHS_DMAADDRESS_BUFF_ADD_Msk (0xffffffffu << UDPHS_DMAADDRESS_BUFF_ADD_Pos) /**< \brief (UDPHS_DMAADDRESS)  */
#define UDPHS_DMAADDRESS_BUFF_ADD(value) ((UDPHS_DMAADDRESS_BUFF_ADD_Msk & ((value) << UDPHS_DMAADDRESS_BUFF_ADD_Pos)))
/* -------- UDPHS_DMACONTROL : (UDPHS Offset: N/A) UDPHS DMA Channel Control Register -------- */
#define UDPHS_DMACONTROL_CHANN_ENB (0x1u << 0) /**< \brief (UDPHS_DMACONTROL)  */
#define UDPHS_DMACONTROL_LDNXT_DSC (0x1u << 1) /**< \brief (UDPHS_DMACONTROL) Load Next Channel Transfer Descriptor Enable (Command) */
#define UDPHS_DMACONTROL_END_TR_EN (0x1u << 2) /**< \brief (UDPHS_DMACONTROL) End of Transfer Enable (Control) */
#define UDPHS_DMACONTROL_END_B_EN (0x1u << 3) /**< \brief (UDPHS_DMACONTROL) End of Buffer Enable (Control) */
#define UDPHS_DMACONTROL_END_TR_IT (0x1u << 4) /**< \brief (UDPHS_DMACONTROL) End of Transfer Interrupt Enable */
#define UDPHS_DMACONTROL_END_BUFFIT (0x1u << 5) /**< \brief (UDPHS_DMACONTROL) End of Buffer Interrupt Enable */
#define UDPHS_DMACONTROL_DESC_LD_IT (0x1u << 6) /**< \brief (UDPHS_DMACONTROL) Descriptor Loaded Interrupt Enable */
#define UDPHS_DMACONTROL_BURST_LCK (0x1u << 7) /**< \brief (UDPHS_DMACONTROL) Burst Lock Enable */
#define UDPHS_DMACONTROL_BUFF_LENGTH_Pos 16
#define UDPHS_DMACONTROL_BUFF_LENGTH_Msk (0xffffu << UDPHS_DMACONTROL_BUFF_LENGTH_Pos) /**< \brief (UDPHS_DMACONTROL) Buffer Byte Length (Write-only) */
#define UDPHS_DMACONTROL_BUFF_LENGTH(value) ((UDPHS_DMACONTROL_BUFF_LENGTH_Msk & ((value) << UDPHS_DMACONTROL_BUFF_LENGTH_Pos)))
/* -------- UDPHS_DMASTATUS : (UDPHS Offset: N/A) UDPHS DMA Channel Status Register -------- */
#define UDPHS_DMASTATUS_CHANN_ENB (0x1u << 0) /**< \brief (UDPHS_DMASTATUS) Channel Enable Status */
#define UDPHS_DMASTATUS_CHANN_ACT (0x1u << 1) /**< \brief (UDPHS_DMASTATUS) Channel Active Status */
#define UDPHS_DMASTATUS_END_TR_ST (0x1u << 4) /**< \brief (UDPHS_DMASTATUS) End of Channel Transfer Status */
#define UDPHS_DMASTATUS_END_BF_ST (0x1u << 5) /**< \brief (UDPHS_DMASTATUS) End of Channel Buffer Status */
#define UDPHS_DMASTATUS_DESC_LDST (0x1u << 6) /**< \brief (UDPHS_DMASTATUS) Descriptor Loaded Status */
#define UDPHS_DMASTATUS_BUFF_COUNT_Pos 16
#define UDPHS_DMASTATUS_BUFF_COUNT_Msk (0xffffu << UDPHS_DMASTATUS_BUFF_COUNT_Pos) /**< \brief (UDPHS_DMASTATUS) Buffer Byte Count */
#define UDPHS_DMASTATUS_BUFF_COUNT(value) ((UDPHS_DMASTATUS_BUFF_COUNT_Msk & ((value) << UDPHS_DMASTATUS_BUFF_COUNT_Pos)))

/*@}*/


#endif /* _SAM3U_UDPHS_COMPONENT_ */
