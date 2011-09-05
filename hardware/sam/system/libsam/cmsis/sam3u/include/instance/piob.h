/* %ATMEL_LICENCE% */

#ifndef _SAM3U_PIOB_INSTANCE_
#define _SAM3U_PIOB_INSTANCE_

/* ========== Register definition for PIOB peripheral ========== */
#define REG_PIOB_PER    REG_ACCESS(WoReg, 0x400E0E00U) /**< \brief (PIOB) PIO Enable Register */
#define REG_PIOB_PDR    REG_ACCESS(WoReg, 0x400E0E04U) /**< \brief (PIOB) PIO Disable Register */
#define REG_PIOB_PSR    REG_ACCESS(RoReg, 0x400E0E08U) /**< \brief (PIOB) PIO Status Register */
#define REG_PIOB_OER    REG_ACCESS(WoReg, 0x400E0E10U) /**< \brief (PIOB) Output Enable Register */
#define REG_PIOB_ODR    REG_ACCESS(WoReg, 0x400E0E14U) /**< \brief (PIOB) Output Disable Register */
#define REG_PIOB_OSR    REG_ACCESS(RoReg, 0x400E0E18U) /**< \brief (PIOB) Output Status Register */
#define REG_PIOB_IFER   REG_ACCESS(WoReg, 0x400E0E20U) /**< \brief (PIOB) Glitch Input Filter Enable Register */
#define REG_PIOB_IFDR   REG_ACCESS(WoReg, 0x400E0E24U) /**< \brief (PIOB) Glitch Input Filter Disable Register */
#define REG_PIOB_IFSR   REG_ACCESS(RoReg, 0x400E0E28U) /**< \brief (PIOB) Glitch Input Filter Status Register */
#define REG_PIOB_SODR   REG_ACCESS(WoReg, 0x400E0E30U) /**< \brief (PIOB) Set Output Data Register */
#define REG_PIOB_CODR   REG_ACCESS(WoReg, 0x400E0E34U) /**< \brief (PIOB) Clear Output Data Register */
#define REG_PIOB_ODSR   REG_ACCESS(RwReg, 0x400E0E38U) /**< \brief (PIOB) Output Data Status Register */
#define REG_PIOB_PDSR   REG_ACCESS(RoReg, 0x400E0E3CU) /**< \brief (PIOB) Pin Data Status Register */
#define REG_PIOB_IER    REG_ACCESS(WoReg, 0x400E0E40U) /**< \brief (PIOB) Interrupt Enable Register */
#define REG_PIOB_IDR    REG_ACCESS(WoReg, 0x400E0E44U) /**< \brief (PIOB) Interrupt Disable Register */
#define REG_PIOB_IMR    REG_ACCESS(RoReg, 0x400E0E48U) /**< \brief (PIOB) Interrupt Mask Register */
#define REG_PIOB_ISR    REG_ACCESS(RoReg, 0x400E0E4CU) /**< \brief (PIOB) Interrupt Status Register */
#define REG_PIOB_MDER   REG_ACCESS(WoReg, 0x400E0E50U) /**< \brief (PIOB) Multi-driver Enable Register */
#define REG_PIOB_MDDR   REG_ACCESS(WoReg, 0x400E0E54U) /**< \brief (PIOB) Multi-driver Disable Register */
#define REG_PIOB_MDSR   REG_ACCESS(RoReg, 0x400E0E58U) /**< \brief (PIOB) Multi-driver Status Register */
#define REG_PIOB_PUDR   REG_ACCESS(WoReg, 0x400E0E60U) /**< \brief (PIOB) Pull-up Disable Register */
#define REG_PIOB_PUER   REG_ACCESS(WoReg, 0x400E0E64U) /**< \brief (PIOB) Pull-up Enable Register */
#define REG_PIOB_PUSR   REG_ACCESS(RoReg, 0x400E0E68U) /**< \brief (PIOB) Pad Pull-up Status Register */
#define REG_PIOB_ABSR   REG_ACCESS(RwReg, 0x400E0E70U) /**< \brief (PIOB) Peripheral AB Select Register */
#define REG_PIOB_SCIFSR REG_ACCESS(WoReg, 0x400E0E80U) /**< \brief (PIOB) System Clock Glitch Input Filter Select Register */
#define REG_PIOB_DIFSR  REG_ACCESS(WoReg, 0x400E0E84U) /**< \brief (PIOB) Debouncing Input Filter Select Register */
#define REG_PIOB_IFDGSR REG_ACCESS(RoReg, 0x400E0E88U) /**< \brief (PIOB) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOB_SCDR   REG_ACCESS(RwReg, 0x400E0E8CU) /**< \brief (PIOB) Slow Clock Divider Debouncing Register */
#define REG_PIOB_OWER   REG_ACCESS(WoReg, 0x400E0EA0U) /**< \brief (PIOB) Output Write Enable */
#define REG_PIOB_OWDR   REG_ACCESS(WoReg, 0x400E0EA4U) /**< \brief (PIOB) Output Write Disable */
#define REG_PIOB_OWSR   REG_ACCESS(RoReg, 0x400E0EA8U) /**< \brief (PIOB) Output Write Status Register */
#define REG_PIOB_AIMER  REG_ACCESS(WoReg, 0x400E0EB0U) /**< \brief (PIOB) Additional Interrupt Modes Enable Register */
#define REG_PIOB_AIMDR  REG_ACCESS(WoReg, 0x400E0EB4U) /**< \brief (PIOB) Additional Interrupt Modes Disables Register */
#define REG_PIOB_AIMMR  REG_ACCESS(RoReg, 0x400E0EB8U) /**< \brief (PIOB) Additional Interrupt Modes Mask Register */
#define REG_PIOB_ESR    REG_ACCESS(WoReg, 0x400E0EC0U) /**< \brief (PIOB) Edge Select Register */
#define REG_PIOB_LSR    REG_ACCESS(WoReg, 0x400E0EC4U) /**< \brief (PIOB) Level Select Register */
#define REG_PIOB_ELSR   REG_ACCESS(RoReg, 0x400E0EC8U) /**< \brief (PIOB) Edge/Level Status Register */
#define REG_PIOB_FELLSR REG_ACCESS(WoReg, 0x400E0ED0U) /**< \brief (PIOB) Falling Edge/Low Level Select Register */
#define REG_PIOB_REHLSR REG_ACCESS(WoReg, 0x400E0ED4U) /**< \brief (PIOB) Rising Edge/ High Level Select Register */
#define REG_PIOB_FRLHSR REG_ACCESS(RoReg, 0x400E0ED8U) /**< \brief (PIOB) Fall/Rise - Low/High Status Register */
#define REG_PIOB_LOCKSR REG_ACCESS(RoReg, 0x400E0EE0U) /**< \brief (PIOB) Lock Status */
#define REG_PIOB_WPMR   REG_ACCESS(RwReg, 0x400E0EE4U) /**< \brief (PIOB) Write Protect Mode Register */
#define REG_PIOB_WPSR   REG_ACCESS(RoReg, 0x400E0EE8U) /**< \brief (PIOB) Write Protect Status Register */

#endif /* _SAM3U_PIOB_INSTANCE_ */
