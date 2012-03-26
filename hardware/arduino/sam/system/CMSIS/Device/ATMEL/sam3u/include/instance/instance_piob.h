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

#ifndef _SAM3U_PIOB_INSTANCE_
#define _SAM3U_PIOB_INSTANCE_

/* ========== Register definition for PIOB peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PIOB_PER             (0x400E0E00U) /**< \brief (PIOB) PIO Enable Register */
#define REG_PIOB_PDR             (0x400E0E04U) /**< \brief (PIOB) PIO Disable Register */
#define REG_PIOB_PSR             (0x400E0E08U) /**< \brief (PIOB) PIO Status Register */
#define REG_PIOB_OER             (0x400E0E10U) /**< \brief (PIOB) Output Enable Register */
#define REG_PIOB_ODR             (0x400E0E14U) /**< \brief (PIOB) Output Disable Register */
#define REG_PIOB_OSR             (0x400E0E18U) /**< \brief (PIOB) Output Status Register */
#define REG_PIOB_IFER            (0x400E0E20U) /**< \brief (PIOB) Glitch Input Filter Enable Register */
#define REG_PIOB_IFDR            (0x400E0E24U) /**< \brief (PIOB) Glitch Input Filter Disable Register */
#define REG_PIOB_IFSR            (0x400E0E28U) /**< \brief (PIOB) Glitch Input Filter Status Register */
#define REG_PIOB_SODR            (0x400E0E30U) /**< \brief (PIOB) Set Output Data Register */
#define REG_PIOB_CODR            (0x400E0E34U) /**< \brief (PIOB) Clear Output Data Register */
#define REG_PIOB_ODSR            (0x400E0E38U) /**< \brief (PIOB) Output Data Status Register */
#define REG_PIOB_PDSR            (0x400E0E3CU) /**< \brief (PIOB) Pin Data Status Register */
#define REG_PIOB_IER             (0x400E0E40U) /**< \brief (PIOB) Interrupt Enable Register */
#define REG_PIOB_IDR             (0x400E0E44U) /**< \brief (PIOB) Interrupt Disable Register */
#define REG_PIOB_IMR             (0x400E0E48U) /**< \brief (PIOB) Interrupt Mask Register */
#define REG_PIOB_ISR             (0x400E0E4CU) /**< \brief (PIOB) Interrupt Status Register */
#define REG_PIOB_MDER            (0x400E0E50U) /**< \brief (PIOB) Multi-driver Enable Register */
#define REG_PIOB_MDDR            (0x400E0E54U) /**< \brief (PIOB) Multi-driver Disable Register */
#define REG_PIOB_MDSR            (0x400E0E58U) /**< \brief (PIOB) Multi-driver Status Register */
#define REG_PIOB_PUDR            (0x400E0E60U) /**< \brief (PIOB) Pull-up Disable Register */
#define REG_PIOB_PUER            (0x400E0E64U) /**< \brief (PIOB) Pull-up Enable Register */
#define REG_PIOB_PUSR            (0x400E0E68U) /**< \brief (PIOB) Pad Pull-up Status Register */
#define REG_PIOB_ABSR            (0x400E0E70U) /**< \brief (PIOB) Peripheral AB Select Register */
#define REG_PIOB_SCIFSR          (0x400E0E80U) /**< \brief (PIOB) System Clock Glitch Input Filter Select Register */
#define REG_PIOB_DIFSR           (0x400E0E84U) /**< \brief (PIOB) Debouncing Input Filter Select Register */
#define REG_PIOB_IFDGSR          (0x400E0E88U) /**< \brief (PIOB) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOB_SCDR            (0x400E0E8CU) /**< \brief (PIOB) Slow Clock Divider Debouncing Register */
#define REG_PIOB_OWER            (0x400E0EA0U) /**< \brief (PIOB) Output Write Enable */
#define REG_PIOB_OWDR            (0x400E0EA4U) /**< \brief (PIOB) Output Write Disable */
#define REG_PIOB_OWSR            (0x400E0EA8U) /**< \brief (PIOB) Output Write Status Register */
#define REG_PIOB_AIMER           (0x400E0EB0U) /**< \brief (PIOB) Additional Interrupt Modes Enable Register */
#define REG_PIOB_AIMDR           (0x400E0EB4U) /**< \brief (PIOB) Additional Interrupt Modes Disables Register */
#define REG_PIOB_AIMMR           (0x400E0EB8U) /**< \brief (PIOB) Additional Interrupt Modes Mask Register */
#define REG_PIOB_ESR             (0x400E0EC0U) /**< \brief (PIOB) Edge Select Register */
#define REG_PIOB_LSR             (0x400E0EC4U) /**< \brief (PIOB) Level Select Register */
#define REG_PIOB_ELSR            (0x400E0EC8U) /**< \brief (PIOB) Edge/Level Status Register */
#define REG_PIOB_FELLSR          (0x400E0ED0U) /**< \brief (PIOB) Falling Edge/Low Level Select Register */
#define REG_PIOB_REHLSR          (0x400E0ED4U) /**< \brief (PIOB) Rising Edge/ High Level Select Register */
#define REG_PIOB_FRLHSR          (0x400E0ED8U) /**< \brief (PIOB) Fall/Rise - Low/High Status Register */
#define REG_PIOB_LOCKSR          (0x400E0EE0U) /**< \brief (PIOB) Lock Status */
#define REG_PIOB_WPMR            (0x400E0EE4U) /**< \brief (PIOB) Write Protect Mode Register */
#define REG_PIOB_WPSR            (0x400E0EE8U) /**< \brief (PIOB) Write Protect Status Register */
#else
#define REG_PIOB_PER    (*(WoReg*)0x400E0E00U) /**< \brief (PIOB) PIO Enable Register */
#define REG_PIOB_PDR    (*(WoReg*)0x400E0E04U) /**< \brief (PIOB) PIO Disable Register */
#define REG_PIOB_PSR    (*(RoReg*)0x400E0E08U) /**< \brief (PIOB) PIO Status Register */
#define REG_PIOB_OER    (*(WoReg*)0x400E0E10U) /**< \brief (PIOB) Output Enable Register */
#define REG_PIOB_ODR    (*(WoReg*)0x400E0E14U) /**< \brief (PIOB) Output Disable Register */
#define REG_PIOB_OSR    (*(RoReg*)0x400E0E18U) /**< \brief (PIOB) Output Status Register */
#define REG_PIOB_IFER   (*(WoReg*)0x400E0E20U) /**< \brief (PIOB) Glitch Input Filter Enable Register */
#define REG_PIOB_IFDR   (*(WoReg*)0x400E0E24U) /**< \brief (PIOB) Glitch Input Filter Disable Register */
#define REG_PIOB_IFSR   (*(RoReg*)0x400E0E28U) /**< \brief (PIOB) Glitch Input Filter Status Register */
#define REG_PIOB_SODR   (*(WoReg*)0x400E0E30U) /**< \brief (PIOB) Set Output Data Register */
#define REG_PIOB_CODR   (*(WoReg*)0x400E0E34U) /**< \brief (PIOB) Clear Output Data Register */
#define REG_PIOB_ODSR   (*(RwReg*)0x400E0E38U) /**< \brief (PIOB) Output Data Status Register */
#define REG_PIOB_PDSR   (*(RoReg*)0x400E0E3CU) /**< \brief (PIOB) Pin Data Status Register */
#define REG_PIOB_IER    (*(WoReg*)0x400E0E40U) /**< \brief (PIOB) Interrupt Enable Register */
#define REG_PIOB_IDR    (*(WoReg*)0x400E0E44U) /**< \brief (PIOB) Interrupt Disable Register */
#define REG_PIOB_IMR    (*(RoReg*)0x400E0E48U) /**< \brief (PIOB) Interrupt Mask Register */
#define REG_PIOB_ISR    (*(RoReg*)0x400E0E4CU) /**< \brief (PIOB) Interrupt Status Register */
#define REG_PIOB_MDER   (*(WoReg*)0x400E0E50U) /**< \brief (PIOB) Multi-driver Enable Register */
#define REG_PIOB_MDDR   (*(WoReg*)0x400E0E54U) /**< \brief (PIOB) Multi-driver Disable Register */
#define REG_PIOB_MDSR   (*(RoReg*)0x400E0E58U) /**< \brief (PIOB) Multi-driver Status Register */
#define REG_PIOB_PUDR   (*(WoReg*)0x400E0E60U) /**< \brief (PIOB) Pull-up Disable Register */
#define REG_PIOB_PUER   (*(WoReg*)0x400E0E64U) /**< \brief (PIOB) Pull-up Enable Register */
#define REG_PIOB_PUSR   (*(RoReg*)0x400E0E68U) /**< \brief (PIOB) Pad Pull-up Status Register */
#define REG_PIOB_ABSR   (*(RwReg*)0x400E0E70U) /**< \brief (PIOB) Peripheral AB Select Register */
#define REG_PIOB_SCIFSR (*(WoReg*)0x400E0E80U) /**< \brief (PIOB) System Clock Glitch Input Filter Select Register */
#define REG_PIOB_DIFSR  (*(WoReg*)0x400E0E84U) /**< \brief (PIOB) Debouncing Input Filter Select Register */
#define REG_PIOB_IFDGSR (*(RoReg*)0x400E0E88U) /**< \brief (PIOB) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOB_SCDR   (*(RwReg*)0x400E0E8CU) /**< \brief (PIOB) Slow Clock Divider Debouncing Register */
#define REG_PIOB_OWER   (*(WoReg*)0x400E0EA0U) /**< \brief (PIOB) Output Write Enable */
#define REG_PIOB_OWDR   (*(WoReg*)0x400E0EA4U) /**< \brief (PIOB) Output Write Disable */
#define REG_PIOB_OWSR   (*(RoReg*)0x400E0EA8U) /**< \brief (PIOB) Output Write Status Register */
#define REG_PIOB_AIMER  (*(WoReg*)0x400E0EB0U) /**< \brief (PIOB) Additional Interrupt Modes Enable Register */
#define REG_PIOB_AIMDR  (*(WoReg*)0x400E0EB4U) /**< \brief (PIOB) Additional Interrupt Modes Disables Register */
#define REG_PIOB_AIMMR  (*(RoReg*)0x400E0EB8U) /**< \brief (PIOB) Additional Interrupt Modes Mask Register */
#define REG_PIOB_ESR    (*(WoReg*)0x400E0EC0U) /**< \brief (PIOB) Edge Select Register */
#define REG_PIOB_LSR    (*(WoReg*)0x400E0EC4U) /**< \brief (PIOB) Level Select Register */
#define REG_PIOB_ELSR   (*(RoReg*)0x400E0EC8U) /**< \brief (PIOB) Edge/Level Status Register */
#define REG_PIOB_FELLSR (*(WoReg*)0x400E0ED0U) /**< \brief (PIOB) Falling Edge/Low Level Select Register */
#define REG_PIOB_REHLSR (*(WoReg*)0x400E0ED4U) /**< \brief (PIOB) Rising Edge/ High Level Select Register */
#define REG_PIOB_FRLHSR (*(RoReg*)0x400E0ED8U) /**< \brief (PIOB) Fall/Rise - Low/High Status Register */
#define REG_PIOB_LOCKSR (*(RoReg*)0x400E0EE0U) /**< \brief (PIOB) Lock Status */
#define REG_PIOB_WPMR   (*(RwReg*)0x400E0EE4U) /**< \brief (PIOB) Write Protect Mode Register */
#define REG_PIOB_WPSR   (*(RoReg*)0x400E0EE8U) /**< \brief (PIOB) Write Protect Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3U_PIOB_INSTANCE_ */
