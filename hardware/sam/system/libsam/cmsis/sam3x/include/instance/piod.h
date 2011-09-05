/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_PIOD_INSTANCE_
#define _SAM3XA_PIOD_INSTANCE_

/* ========== Register definition for PIOD peripheral ========== */
#define REG_PIOD_PER    REG_ACCESS(WoReg, 0x400E1400U) /**< \brief (PIOD) PIO Enable Register */
#define REG_PIOD_PDR    REG_ACCESS(WoReg, 0x400E1404U) /**< \brief (PIOD) PIO Disable Register */
#define REG_PIOD_PSR    REG_ACCESS(RoReg, 0x400E1408U) /**< \brief (PIOD) PIO Status Register */
#define REG_PIOD_OER    REG_ACCESS(WoReg, 0x400E1410U) /**< \brief (PIOD) Output Enable Register */
#define REG_PIOD_ODR    REG_ACCESS(WoReg, 0x400E1414U) /**< \brief (PIOD) Output Disable Register */
#define REG_PIOD_OSR    REG_ACCESS(RoReg, 0x400E1418U) /**< \brief (PIOD) Output Status Register */
#define REG_PIOD_IFER   REG_ACCESS(WoReg, 0x400E1420U) /**< \brief (PIOD) Glitch Input Filter Enable Register */
#define REG_PIOD_IFDR   REG_ACCESS(WoReg, 0x400E1424U) /**< \brief (PIOD) Glitch Input Filter Disable Register */
#define REG_PIOD_IFSR   REG_ACCESS(RoReg, 0x400E1428U) /**< \brief (PIOD) Glitch Input Filter Status Register */
#define REG_PIOD_SODR   REG_ACCESS(WoReg, 0x400E1430U) /**< \brief (PIOD) Set Output Data Register */
#define REG_PIOD_CODR   REG_ACCESS(WoReg, 0x400E1434U) /**< \brief (PIOD) Clear Output Data Register */
#define REG_PIOD_ODSR   REG_ACCESS(RwReg, 0x400E1438U) /**< \brief (PIOD) Output Data Status Register */
#define REG_PIOD_PDSR   REG_ACCESS(RoReg, 0x400E143CU) /**< \brief (PIOD) Pin Data Status Register */
#define REG_PIOD_IER    REG_ACCESS(WoReg, 0x400E1440U) /**< \brief (PIOD) Interrupt Enable Register */
#define REG_PIOD_IDR    REG_ACCESS(WoReg, 0x400E1444U) /**< \brief (PIOD) Interrupt Disable Register */
#define REG_PIOD_IMR    REG_ACCESS(RoReg, 0x400E1448U) /**< \brief (PIOD) Interrupt Mask Register */
#define REG_PIOD_ISR    REG_ACCESS(RoReg, 0x400E144CU) /**< \brief (PIOD) Interrupt Status Register */
#define REG_PIOD_MDER   REG_ACCESS(WoReg, 0x400E1450U) /**< \brief (PIOD) Multi-driver Enable Register */
#define REG_PIOD_MDDR   REG_ACCESS(WoReg, 0x400E1454U) /**< \brief (PIOD) Multi-driver Disable Register */
#define REG_PIOD_MDSR   REG_ACCESS(RoReg, 0x400E1458U) /**< \brief (PIOD) Multi-driver Status Register */
#define REG_PIOD_PUDR   REG_ACCESS(WoReg, 0x400E1460U) /**< \brief (PIOD) Pull-up Disable Register */
#define REG_PIOD_PUER   REG_ACCESS(WoReg, 0x400E1464U) /**< \brief (PIOD) Pull-up Enable Register */
#define REG_PIOD_PUSR   REG_ACCESS(RoReg, 0x400E1468U) /**< \brief (PIOD) Pad Pull-up Status Register */
#define REG_PIOD_ABSR   REG_ACCESS(RwReg, 0x400E1470U) /**< \brief (PIOD) Peripheral AB Select Register */
#define REG_PIOD_SCIFSR REG_ACCESS(WoReg, 0x400E1480U) /**< \brief (PIOD) System Clock Glitch Input Filter Select Register */
#define REG_PIOD_DIFSR  REG_ACCESS(WoReg, 0x400E1484U) /**< \brief (PIOD) Debouncing Input Filter Select Register */
#define REG_PIOD_IFDGSR REG_ACCESS(RoReg, 0x400E1488U) /**< \brief (PIOD) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOD_SCDR   REG_ACCESS(RwReg, 0x400E148CU) /**< \brief (PIOD) Slow Clock Divider Debouncing Register */
#define REG_PIOD_OWER   REG_ACCESS(WoReg, 0x400E14A0U) /**< \brief (PIOD) Output Write Enable */
#define REG_PIOD_OWDR   REG_ACCESS(WoReg, 0x400E14A4U) /**< \brief (PIOD) Output Write Disable */
#define REG_PIOD_OWSR   REG_ACCESS(RoReg, 0x400E14A8U) /**< \brief (PIOD) Output Write Status Register */
#define REG_PIOD_AIMER  REG_ACCESS(WoReg, 0x400E14B0U) /**< \brief (PIOD) Additional Interrupt Modes Enable Register */
#define REG_PIOD_AIMDR  REG_ACCESS(WoReg, 0x400E14B4U) /**< \brief (PIOD) Additional Interrupt Modes Disables Register */
#define REG_PIOD_AIMMR  REG_ACCESS(RoReg, 0x400E14B8U) /**< \brief (PIOD) Additional Interrupt Modes Mask Register */
#define REG_PIOD_ESR    REG_ACCESS(WoReg, 0x400E14C0U) /**< \brief (PIOD) Edge Select Register */
#define REG_PIOD_LSR    REG_ACCESS(WoReg, 0x400E14C4U) /**< \brief (PIOD) Level Select Register */
#define REG_PIOD_ELSR   REG_ACCESS(RoReg, 0x400E14C8U) /**< \brief (PIOD) Edge/Level Status Register */
#define REG_PIOD_FELLSR REG_ACCESS(WoReg, 0x400E14D0U) /**< \brief (PIOD) Falling Edge/Low Level Select Register */
#define REG_PIOD_REHLSR REG_ACCESS(WoReg, 0x400E14D4U) /**< \brief (PIOD) Rising Edge/ High Level Select Register */
#define REG_PIOD_FRLHSR REG_ACCESS(RoReg, 0x400E14D8U) /**< \brief (PIOD) Fall/Rise - Low/High Status Register */
#define REG_PIOD_LOCKSR REG_ACCESS(RoReg, 0x400E14E0U) /**< \brief (PIOD) Lock Status */
#define REG_PIOD_WPMR   REG_ACCESS(RwReg, 0x400E14E4U) /**< \brief (PIOD) Write Protect Mode Register */
#define REG_PIOD_WPSR   REG_ACCESS(RoReg, 0x400E14E8U) /**< \brief (PIOD) Write Protect Status Register */

#endif /* _SAM3XA_PIOD_INSTANCE_ */
