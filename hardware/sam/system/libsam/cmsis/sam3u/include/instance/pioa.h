/* %ATMEL_LICENCE% */

#ifndef _SAM3U_PIOA_INSTANCE_
#define _SAM3U_PIOA_INSTANCE_

/* ========== Register definition for PIOA peripheral ========== */
#define REG_PIOA_PER    REG_ACCESS(WoReg, 0x400E0C00U) /**< \brief (PIOA) PIO Enable Register */
#define REG_PIOA_PDR    REG_ACCESS(WoReg, 0x400E0C04U) /**< \brief (PIOA) PIO Disable Register */
#define REG_PIOA_PSR    REG_ACCESS(RoReg, 0x400E0C08U) /**< \brief (PIOA) PIO Status Register */
#define REG_PIOA_OER    REG_ACCESS(WoReg, 0x400E0C10U) /**< \brief (PIOA) Output Enable Register */
#define REG_PIOA_ODR    REG_ACCESS(WoReg, 0x400E0C14U) /**< \brief (PIOA) Output Disable Register */
#define REG_PIOA_OSR    REG_ACCESS(RoReg, 0x400E0C18U) /**< \brief (PIOA) Output Status Register */
#define REG_PIOA_IFER   REG_ACCESS(WoReg, 0x400E0C20U) /**< \brief (PIOA) Glitch Input Filter Enable Register */
#define REG_PIOA_IFDR   REG_ACCESS(WoReg, 0x400E0C24U) /**< \brief (PIOA) Glitch Input Filter Disable Register */
#define REG_PIOA_IFSR   REG_ACCESS(RoReg, 0x400E0C28U) /**< \brief (PIOA) Glitch Input Filter Status Register */
#define REG_PIOA_SODR   REG_ACCESS(WoReg, 0x400E0C30U) /**< \brief (PIOA) Set Output Data Register */
#define REG_PIOA_CODR   REG_ACCESS(WoReg, 0x400E0C34U) /**< \brief (PIOA) Clear Output Data Register */
#define REG_PIOA_ODSR   REG_ACCESS(RwReg, 0x400E0C38U) /**< \brief (PIOA) Output Data Status Register */
#define REG_PIOA_PDSR   REG_ACCESS(RoReg, 0x400E0C3CU) /**< \brief (PIOA) Pin Data Status Register */
#define REG_PIOA_IER    REG_ACCESS(WoReg, 0x400E0C40U) /**< \brief (PIOA) Interrupt Enable Register */
#define REG_PIOA_IDR    REG_ACCESS(WoReg, 0x400E0C44U) /**< \brief (PIOA) Interrupt Disable Register */
#define REG_PIOA_IMR    REG_ACCESS(RoReg, 0x400E0C48U) /**< \brief (PIOA) Interrupt Mask Register */
#define REG_PIOA_ISR    REG_ACCESS(RoReg, 0x400E0C4CU) /**< \brief (PIOA) Interrupt Status Register */
#define REG_PIOA_MDER   REG_ACCESS(WoReg, 0x400E0C50U) /**< \brief (PIOA) Multi-driver Enable Register */
#define REG_PIOA_MDDR   REG_ACCESS(WoReg, 0x400E0C54U) /**< \brief (PIOA) Multi-driver Disable Register */
#define REG_PIOA_MDSR   REG_ACCESS(RoReg, 0x400E0C58U) /**< \brief (PIOA) Multi-driver Status Register */
#define REG_PIOA_PUDR   REG_ACCESS(WoReg, 0x400E0C60U) /**< \brief (PIOA) Pull-up Disable Register */
#define REG_PIOA_PUER   REG_ACCESS(WoReg, 0x400E0C64U) /**< \brief (PIOA) Pull-up Enable Register */
#define REG_PIOA_PUSR   REG_ACCESS(RoReg, 0x400E0C68U) /**< \brief (PIOA) Pad Pull-up Status Register */
#define REG_PIOA_ABSR   REG_ACCESS(RwReg, 0x400E0C70U) /**< \brief (PIOA) Peripheral AB Select Register */
#define REG_PIOA_SCIFSR REG_ACCESS(WoReg, 0x400E0C80U) /**< \brief (PIOA) System Clock Glitch Input Filter Select Register */
#define REG_PIOA_DIFSR  REG_ACCESS(WoReg, 0x400E0C84U) /**< \brief (PIOA) Debouncing Input Filter Select Register */
#define REG_PIOA_IFDGSR REG_ACCESS(RoReg, 0x400E0C88U) /**< \brief (PIOA) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOA_SCDR   REG_ACCESS(RwReg, 0x400E0C8CU) /**< \brief (PIOA) Slow Clock Divider Debouncing Register */
#define REG_PIOA_OWER   REG_ACCESS(WoReg, 0x400E0CA0U) /**< \brief (PIOA) Output Write Enable */
#define REG_PIOA_OWDR   REG_ACCESS(WoReg, 0x400E0CA4U) /**< \brief (PIOA) Output Write Disable */
#define REG_PIOA_OWSR   REG_ACCESS(RoReg, 0x400E0CA8U) /**< \brief (PIOA) Output Write Status Register */
#define REG_PIOA_AIMER  REG_ACCESS(WoReg, 0x400E0CB0U) /**< \brief (PIOA) Additional Interrupt Modes Enable Register */
#define REG_PIOA_AIMDR  REG_ACCESS(WoReg, 0x400E0CB4U) /**< \brief (PIOA) Additional Interrupt Modes Disables Register */
#define REG_PIOA_AIMMR  REG_ACCESS(RoReg, 0x400E0CB8U) /**< \brief (PIOA) Additional Interrupt Modes Mask Register */
#define REG_PIOA_ESR    REG_ACCESS(WoReg, 0x400E0CC0U) /**< \brief (PIOA) Edge Select Register */
#define REG_PIOA_LSR    REG_ACCESS(WoReg, 0x400E0CC4U) /**< \brief (PIOA) Level Select Register */
#define REG_PIOA_ELSR   REG_ACCESS(RoReg, 0x400E0CC8U) /**< \brief (PIOA) Edge/Level Status Register */
#define REG_PIOA_FELLSR REG_ACCESS(WoReg, 0x400E0CD0U) /**< \brief (PIOA) Falling Edge/Low Level Select Register */
#define REG_PIOA_REHLSR REG_ACCESS(WoReg, 0x400E0CD4U) /**< \brief (PIOA) Rising Edge/ High Level Select Register */
#define REG_PIOA_FRLHSR REG_ACCESS(RoReg, 0x400E0CD8U) /**< \brief (PIOA) Fall/Rise - Low/High Status Register */
#define REG_PIOA_LOCKSR REG_ACCESS(RoReg, 0x400E0CE0U) /**< \brief (PIOA) Lock Status */
#define REG_PIOA_WPMR   REG_ACCESS(RwReg, 0x400E0CE4U) /**< \brief (PIOA) Write Protect Mode Register */
#define REG_PIOA_WPSR   REG_ACCESS(RoReg, 0x400E0CE8U) /**< \brief (PIOA) Write Protect Status Register */

#endif /* _SAM3U_PIOA_INSTANCE_ */
