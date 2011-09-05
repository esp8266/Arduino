/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_PIOA_INSTANCE_
#define _SAM3S8_PIOA_INSTANCE_

/* ========== Register definition for PIOA peripheral ========== */
#define REG_PIOA_PER       REG_ACCESS(WoReg, 0x400E0E00U) /**< \brief (PIOA) PIO Enable Register */
#define REG_PIOA_PDR       REG_ACCESS(WoReg, 0x400E0E04U) /**< \brief (PIOA) PIO Disable Register */
#define REG_PIOA_PSR       REG_ACCESS(RoReg, 0x400E0E08U) /**< \brief (PIOA) PIO Status Register */
#define REG_PIOA_OER       REG_ACCESS(WoReg, 0x400E0E10U) /**< \brief (PIOA) Output Enable Register */
#define REG_PIOA_ODR       REG_ACCESS(WoReg, 0x400E0E14U) /**< \brief (PIOA) Output Disable Register */
#define REG_PIOA_OSR       REG_ACCESS(RoReg, 0x400E0E18U) /**< \brief (PIOA) Output Status Register */
#define REG_PIOA_IFER      REG_ACCESS(WoReg, 0x400E0E20U) /**< \brief (PIOA) Glitch Input Filter Enable Register */
#define REG_PIOA_IFDR      REG_ACCESS(WoReg, 0x400E0E24U) /**< \brief (PIOA) Glitch Input Filter Disable Register */
#define REG_PIOA_IFSR      REG_ACCESS(RoReg, 0x400E0E28U) /**< \brief (PIOA) Glitch Input Filter Status Register */
#define REG_PIOA_SODR      REG_ACCESS(WoReg, 0x400E0E30U) /**< \brief (PIOA) Set Output Data Register */
#define REG_PIOA_CODR      REG_ACCESS(WoReg, 0x400E0E34U) /**< \brief (PIOA) Clear Output Data Register */
#define REG_PIOA_ODSR      REG_ACCESS(RwReg, 0x400E0E38U) /**< \brief (PIOA) Output Data Status Register */
#define REG_PIOA_PDSR      REG_ACCESS(RoReg, 0x400E0E3CU) /**< \brief (PIOA) Pin Data Status Register */
#define REG_PIOA_IER       REG_ACCESS(WoReg, 0x400E0E40U) /**< \brief (PIOA) Interrupt Enable Register */
#define REG_PIOA_IDR       REG_ACCESS(WoReg, 0x400E0E44U) /**< \brief (PIOA) Interrupt Disable Register */
#define REG_PIOA_IMR       REG_ACCESS(RoReg, 0x400E0E48U) /**< \brief (PIOA) Interrupt Mask Register */
#define REG_PIOA_ISR       REG_ACCESS(RoReg, 0x400E0E4CU) /**< \brief (PIOA) Interrupt Status Register */
#define REG_PIOA_MDER      REG_ACCESS(WoReg, 0x400E0E50U) /**< \brief (PIOA) Multi-driver Enable Register */
#define REG_PIOA_MDDR      REG_ACCESS(WoReg, 0x400E0E54U) /**< \brief (PIOA) Multi-driver Disable Register */
#define REG_PIOA_MDSR      REG_ACCESS(RoReg, 0x400E0E58U) /**< \brief (PIOA) Multi-driver Status Register */
#define REG_PIOA_PUDR      REG_ACCESS(WoReg, 0x400E0E60U) /**< \brief (PIOA) Pull-up Disable Register */
#define REG_PIOA_PUER      REG_ACCESS(WoReg, 0x400E0E64U) /**< \brief (PIOA) Pull-up Enable Register */
#define REG_PIOA_PUSR      REG_ACCESS(RoReg, 0x400E0E68U) /**< \brief (PIOA) Pad Pull-up Status Register */
#define REG_PIOA_ABCDSR    REG_ACCESS(RwReg, 0x400E0E70U) /**< \brief (PIOA) Peripheral Select Register */
#define REG_PIOA_IFSCDR    REG_ACCESS(WoReg, 0x400E0E80U) /**< \brief (PIOA) Input Filter Slow Clock Disable Register */
#define REG_PIOA_IFSCER    REG_ACCESS(WoReg, 0x400E0E84U) /**< \brief (PIOA) Input Filter Slow Clock Enable Register */
#define REG_PIOA_IFSCSR    REG_ACCESS(RoReg, 0x400E0E88U) /**< \brief (PIOA) Input Filter Slow Clock Status Register */
#define REG_PIOA_SCDR      REG_ACCESS(RwReg, 0x400E0E8CU) /**< \brief (PIOA) Slow Clock Divider Debouncing Register */
#define REG_PIOA_PPDDR     REG_ACCESS(WoReg, 0x400E0E90U) /**< \brief (PIOA) Pad Pull-down Disable Register */
#define REG_PIOA_PPDER     REG_ACCESS(WoReg, 0x400E0E94U) /**< \brief (PIOA) Pad Pull-down Enable Register */
#define REG_PIOA_PPDSR     REG_ACCESS(RoReg, 0x400E0E98U) /**< \brief (PIOA) Pad Pull-down Status Register */
#define REG_PIOA_OWER      REG_ACCESS(WoReg, 0x400E0EA0U) /**< \brief (PIOA) Output Write Enable */
#define REG_PIOA_OWDR      REG_ACCESS(WoReg, 0x400E0EA4U) /**< \brief (PIOA) Output Write Disable */
#define REG_PIOA_OWSR      REG_ACCESS(RoReg, 0x400E0EA8U) /**< \brief (PIOA) Output Write Status Register */
#define REG_PIOA_AIMER     REG_ACCESS(WoReg, 0x400E0EB0U) /**< \brief (PIOA) Additional Interrupt Modes Enable Register */
#define REG_PIOA_AIMDR     REG_ACCESS(WoReg, 0x400E0EB4U) /**< \brief (PIOA) Additional Interrupt Modes Disables Register */
#define REG_PIOA_AIMMR     REG_ACCESS(RoReg, 0x400E0EB8U) /**< \brief (PIOA) Additional Interrupt Modes Mask Register */
#define REG_PIOA_ESR       REG_ACCESS(WoReg, 0x400E0EC0U) /**< \brief (PIOA) Edge Select Register */
#define REG_PIOA_LSR       REG_ACCESS(WoReg, 0x400E0EC4U) /**< \brief (PIOA) Level Select Register */
#define REG_PIOA_ELSR      REG_ACCESS(RoReg, 0x400E0EC8U) /**< \brief (PIOA) Edge/Level Status Register */
#define REG_PIOA_FELLSR    REG_ACCESS(WoReg, 0x400E0ED0U) /**< \brief (PIOA) Falling Edge/Low Level Select Register */
#define REG_PIOA_REHLSR    REG_ACCESS(WoReg, 0x400E0ED4U) /**< \brief (PIOA) Rising Edge/ High Level Select Register */
#define REG_PIOA_FRLHSR    REG_ACCESS(RoReg, 0x400E0ED8U) /**< \brief (PIOA) Fall/Rise - Low/High Status Register */
#define REG_PIOA_LOCKSR    REG_ACCESS(RoReg, 0x400E0EE0U) /**< \brief (PIOA) Lock Status */
#define REG_PIOA_WPMR      REG_ACCESS(RwReg, 0x400E0EE4U) /**< \brief (PIOA) Write Protect Mode Register */
#define REG_PIOA_WPSR      REG_ACCESS(RoReg, 0x400E0EE8U) /**< \brief (PIOA) Write Protect Status Register */
#define REG_PIOA_SCHMITT   REG_ACCESS(RwReg, 0x400E0F00U) /**< \brief (PIOA) Schmitt Trigger Register */
#define REG_PIOA_PCMR      REG_ACCESS(RwReg, 0x400E0F50U) /**< \brief (PIOA) Parallel Capture Mode Register */
#define REG_PIOA_PCIER     REG_ACCESS(WoReg, 0x400E0F54U) /**< \brief (PIOA) Parallel Capture Interrupt Enable Register */
#define REG_PIOA_PCIDR     REG_ACCESS(WoReg, 0x400E0F58U) /**< \brief (PIOA) Parallel Capture Interrupt Disable Register */
#define REG_PIOA_PCIMR     REG_ACCESS(RoReg, 0x400E0F5CU) /**< \brief (PIOA) Parallel Capture Interrupt Mask Register */
#define REG_PIOA_PCISR     REG_ACCESS(RoReg, 0x400E0F60U) /**< \brief (PIOA) Parallel Capture Interrupt Status Register */
#define REG_PIOA_PCRHR     REG_ACCESS(RoReg, 0x400E0F64U) /**< \brief (PIOA) Parallel Capture Reception Holding Register */
#define REG_PIOA_RPR       REG_ACCESS(RwReg, 0x400E0F68U) /**< \brief (PIOA) Receive Pointer Register */
#define REG_PIOA_RCR       REG_ACCESS(RwReg, 0x400E0F6CU) /**< \brief (PIOA) Receive Counter Register */
#define REG_PIOA_TPR       REG_ACCESS(RwReg, 0x400E0F70U) /**< \brief (PIOA) Transmit Pointer Register */
#define REG_PIOA_TCR       REG_ACCESS(RwReg, 0x400E0F74U) /**< \brief (PIOA) Transmit Counter Register */
#define REG_PIOA_RNPR      REG_ACCESS(RwReg, 0x400E0F78U) /**< \brief (PIOA) Receive Next Pointer Register */
#define REG_PIOA_RNCR      REG_ACCESS(RwReg, 0x400E0F7CU) /**< \brief (PIOA) Receive Next Counter Register */
#define REG_PIOA_TNPR      REG_ACCESS(RwReg, 0x400E0F80U) /**< \brief (PIOA) Transmit Next Pointer Register */
#define REG_PIOA_TNCR      REG_ACCESS(RwReg, 0x400E0F84U) /**< \brief (PIOA) Transmit Next Counter Register */
#define REG_PIOA_PTCR      REG_ACCESS(WoReg, 0x400E0F88U) /**< \brief (PIOA) Transfer Control Register */
#define REG_PIOA_PTSR      REG_ACCESS(RoReg, 0x400E0F8CU) /**< \brief (PIOA) Transfer Status Register */

#endif /* _SAM3S8_PIOA_INSTANCE_ */
