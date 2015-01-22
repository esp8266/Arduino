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

#ifndef _SAM3XA_PIOE_INSTANCE_
#define _SAM3XA_PIOE_INSTANCE_

/* ========== Register definition for PIOE peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PIOE_PER             (0x400E1600U) /**< \brief (PIOE) PIO Enable Register */
#define REG_PIOE_PDR             (0x400E1604U) /**< \brief (PIOE) PIO Disable Register */
#define REG_PIOE_PSR             (0x400E1608U) /**< \brief (PIOE) PIO Status Register */
#define REG_PIOE_OER             (0x400E1610U) /**< \brief (PIOE) Output Enable Register */
#define REG_PIOE_ODR             (0x400E1614U) /**< \brief (PIOE) Output Disable Register */
#define REG_PIOE_OSR             (0x400E1618U) /**< \brief (PIOE) Output Status Register */
#define REG_PIOE_IFER            (0x400E1620U) /**< \brief (PIOE) Glitch Input Filter Enable Register */
#define REG_PIOE_IFDR            (0x400E1624U) /**< \brief (PIOE) Glitch Input Filter Disable Register */
#define REG_PIOE_IFSR            (0x400E1628U) /**< \brief (PIOE) Glitch Input Filter Status Register */
#define REG_PIOE_SODR            (0x400E1630U) /**< \brief (PIOE) Set Output Data Register */
#define REG_PIOE_CODR            (0x400E1634U) /**< \brief (PIOE) Clear Output Data Register */
#define REG_PIOE_ODSR            (0x400E1638U) /**< \brief (PIOE) Output Data Status Register */
#define REG_PIOE_PDSR            (0x400E163CU) /**< \brief (PIOE) Pin Data Status Register */
#define REG_PIOE_IER             (0x400E1640U) /**< \brief (PIOE) Interrupt Enable Register */
#define REG_PIOE_IDR             (0x400E1644U) /**< \brief (PIOE) Interrupt Disable Register */
#define REG_PIOE_IMR             (0x400E1648U) /**< \brief (PIOE) Interrupt Mask Register */
#define REG_PIOE_ISR             (0x400E164CU) /**< \brief (PIOE) Interrupt Status Register */
#define REG_PIOE_MDER            (0x400E1650U) /**< \brief (PIOE) Multi-driver Enable Register */
#define REG_PIOE_MDDR            (0x400E1654U) /**< \brief (PIOE) Multi-driver Disable Register */
#define REG_PIOE_MDSR            (0x400E1658U) /**< \brief (PIOE) Multi-driver Status Register */
#define REG_PIOE_PUDR            (0x400E1660U) /**< \brief (PIOE) Pull-up Disable Register */
#define REG_PIOE_PUER            (0x400E1664U) /**< \brief (PIOE) Pull-up Enable Register */
#define REG_PIOE_PUSR            (0x400E1668U) /**< \brief (PIOE) Pad Pull-up Status Register */
#define REG_PIOE_ABSR            (0x400E1670U) /**< \brief (PIOE) Peripheral AB Select Register */
#define REG_PIOE_SCIFSR          (0x400E1680U) /**< \brief (PIOE) System Clock Glitch Input Filter Select Register */
#define REG_PIOE_DIFSR           (0x400E1684U) /**< \brief (PIOE) Debouncing Input Filter Select Register */
#define REG_PIOE_IFDGSR          (0x400E1688U) /**< \brief (PIOE) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOE_SCDR            (0x400E168CU) /**< \brief (PIOE) Slow Clock Divider Debouncing Register */
#define REG_PIOE_OWER            (0x400E16A0U) /**< \brief (PIOE) Output Write Enable */
#define REG_PIOE_OWDR            (0x400E16A4U) /**< \brief (PIOE) Output Write Disable */
#define REG_PIOE_OWSR            (0x400E16A8U) /**< \brief (PIOE) Output Write Status Register */
#define REG_PIOE_AIMER           (0x400E16B0U) /**< \brief (PIOE) Additional Interrupt Modes Enable Register */
#define REG_PIOE_AIMDR           (0x400E16B4U) /**< \brief (PIOE) Additional Interrupt Modes Disables Register */
#define REG_PIOE_AIMMR           (0x400E16B8U) /**< \brief (PIOE) Additional Interrupt Modes Mask Register */
#define REG_PIOE_ESR             (0x400E16C0U) /**< \brief (PIOE) Edge Select Register */
#define REG_PIOE_LSR             (0x400E16C4U) /**< \brief (PIOE) Level Select Register */
#define REG_PIOE_ELSR            (0x400E16C8U) /**< \brief (PIOE) Edge/Level Status Register */
#define REG_PIOE_FELLSR          (0x400E16D0U) /**< \brief (PIOE) Falling Edge/Low Level Select Register */
#define REG_PIOE_REHLSR          (0x400E16D4U) /**< \brief (PIOE) Rising Edge/ High Level Select Register */
#define REG_PIOE_FRLHSR          (0x400E16D8U) /**< \brief (PIOE) Fall/Rise - Low/High Status Register */
#define REG_PIOE_LOCKSR          (0x400E16E0U) /**< \brief (PIOE) Lock Status */
#define REG_PIOE_WPMR            (0x400E16E4U) /**< \brief (PIOE) Write Protect Mode Register */
#define REG_PIOE_WPSR            (0x400E16E8U) /**< \brief (PIOE) Write Protect Status Register */
#else
#define REG_PIOE_PER    (*(WoReg*)0x400E1600U) /**< \brief (PIOE) PIO Enable Register */
#define REG_PIOE_PDR    (*(WoReg*)0x400E1604U) /**< \brief (PIOE) PIO Disable Register */
#define REG_PIOE_PSR    (*(RoReg*)0x400E1608U) /**< \brief (PIOE) PIO Status Register */
#define REG_PIOE_OER    (*(WoReg*)0x400E1610U) /**< \brief (PIOE) Output Enable Register */
#define REG_PIOE_ODR    (*(WoReg*)0x400E1614U) /**< \brief (PIOE) Output Disable Register */
#define REG_PIOE_OSR    (*(RoReg*)0x400E1618U) /**< \brief (PIOE) Output Status Register */
#define REG_PIOE_IFER   (*(WoReg*)0x400E1620U) /**< \brief (PIOE) Glitch Input Filter Enable Register */
#define REG_PIOE_IFDR   (*(WoReg*)0x400E1624U) /**< \brief (PIOE) Glitch Input Filter Disable Register */
#define REG_PIOE_IFSR   (*(RoReg*)0x400E1628U) /**< \brief (PIOE) Glitch Input Filter Status Register */
#define REG_PIOE_SODR   (*(WoReg*)0x400E1630U) /**< \brief (PIOE) Set Output Data Register */
#define REG_PIOE_CODR   (*(WoReg*)0x400E1634U) /**< \brief (PIOE) Clear Output Data Register */
#define REG_PIOE_ODSR   (*(RwReg*)0x400E1638U) /**< \brief (PIOE) Output Data Status Register */
#define REG_PIOE_PDSR   (*(RoReg*)0x400E163CU) /**< \brief (PIOE) Pin Data Status Register */
#define REG_PIOE_IER    (*(WoReg*)0x400E1640U) /**< \brief (PIOE) Interrupt Enable Register */
#define REG_PIOE_IDR    (*(WoReg*)0x400E1644U) /**< \brief (PIOE) Interrupt Disable Register */
#define REG_PIOE_IMR    (*(RoReg*)0x400E1648U) /**< \brief (PIOE) Interrupt Mask Register */
#define REG_PIOE_ISR    (*(RoReg*)0x400E164CU) /**< \brief (PIOE) Interrupt Status Register */
#define REG_PIOE_MDER   (*(WoReg*)0x400E1650U) /**< \brief (PIOE) Multi-driver Enable Register */
#define REG_PIOE_MDDR   (*(WoReg*)0x400E1654U) /**< \brief (PIOE) Multi-driver Disable Register */
#define REG_PIOE_MDSR   (*(RoReg*)0x400E1658U) /**< \brief (PIOE) Multi-driver Status Register */
#define REG_PIOE_PUDR   (*(WoReg*)0x400E1660U) /**< \brief (PIOE) Pull-up Disable Register */
#define REG_PIOE_PUER   (*(WoReg*)0x400E1664U) /**< \brief (PIOE) Pull-up Enable Register */
#define REG_PIOE_PUSR   (*(RoReg*)0x400E1668U) /**< \brief (PIOE) Pad Pull-up Status Register */
#define REG_PIOE_ABSR   (*(RwReg*)0x400E1670U) /**< \brief (PIOE) Peripheral AB Select Register */
#define REG_PIOE_SCIFSR (*(WoReg*)0x400E1680U) /**< \brief (PIOE) System Clock Glitch Input Filter Select Register */
#define REG_PIOE_DIFSR  (*(WoReg*)0x400E1684U) /**< \brief (PIOE) Debouncing Input Filter Select Register */
#define REG_PIOE_IFDGSR (*(RoReg*)0x400E1688U) /**< \brief (PIOE) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOE_SCDR   (*(RwReg*)0x400E168CU) /**< \brief (PIOE) Slow Clock Divider Debouncing Register */
#define REG_PIOE_OWER   (*(WoReg*)0x400E16A0U) /**< \brief (PIOE) Output Write Enable */
#define REG_PIOE_OWDR   (*(WoReg*)0x400E16A4U) /**< \brief (PIOE) Output Write Disable */
#define REG_PIOE_OWSR   (*(RoReg*)0x400E16A8U) /**< \brief (PIOE) Output Write Status Register */
#define REG_PIOE_AIMER  (*(WoReg*)0x400E16B0U) /**< \brief (PIOE) Additional Interrupt Modes Enable Register */
#define REG_PIOE_AIMDR  (*(WoReg*)0x400E16B4U) /**< \brief (PIOE) Additional Interrupt Modes Disables Register */
#define REG_PIOE_AIMMR  (*(RoReg*)0x400E16B8U) /**< \brief (PIOE) Additional Interrupt Modes Mask Register */
#define REG_PIOE_ESR    (*(WoReg*)0x400E16C0U) /**< \brief (PIOE) Edge Select Register */
#define REG_PIOE_LSR    (*(WoReg*)0x400E16C4U) /**< \brief (PIOE) Level Select Register */
#define REG_PIOE_ELSR   (*(RoReg*)0x400E16C8U) /**< \brief (PIOE) Edge/Level Status Register */
#define REG_PIOE_FELLSR (*(WoReg*)0x400E16D0U) /**< \brief (PIOE) Falling Edge/Low Level Select Register */
#define REG_PIOE_REHLSR (*(WoReg*)0x400E16D4U) /**< \brief (PIOE) Rising Edge/ High Level Select Register */
#define REG_PIOE_FRLHSR (*(RoReg*)0x400E16D8U) /**< \brief (PIOE) Fall/Rise - Low/High Status Register */
#define REG_PIOE_LOCKSR (*(RoReg*)0x400E16E0U) /**< \brief (PIOE) Lock Status */
#define REG_PIOE_WPMR   (*(RwReg*)0x400E16E4U) /**< \brief (PIOE) Write Protect Mode Register */
#define REG_PIOE_WPSR   (*(RoReg*)0x400E16E8U) /**< \brief (PIOE) Write Protect Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3XA_PIOE_INSTANCE_ */
