/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_PIOC_INSTANCE_
#define _SAM3XA_PIOC_INSTANCE_

/* ========== Register definition for PIOC peripheral ========== */
#define REG_PIOC_PER    REG_ACCESS(WoReg, 0x400E1200U) /**< \brief (PIOC) PIO Enable Register */
#define REG_PIOC_PDR    REG_ACCESS(WoReg, 0x400E1204U) /**< \brief (PIOC) PIO Disable Register */
#define REG_PIOC_PSR    REG_ACCESS(RoReg, 0x400E1208U) /**< \brief (PIOC) PIO Status Register */
#define REG_PIOC_OER    REG_ACCESS(WoReg, 0x400E1210U) /**< \brief (PIOC) Output Enable Register */
#define REG_PIOC_ODR    REG_ACCESS(WoReg, 0x400E1214U) /**< \brief (PIOC) Output Disable Register */
#define REG_PIOC_OSR    REG_ACCESS(RoReg, 0x400E1218U) /**< \brief (PIOC) Output Status Register */
#define REG_PIOC_IFER   REG_ACCESS(WoReg, 0x400E1220U) /**< \brief (PIOC) Glitch Input Filter Enable Register */
#define REG_PIOC_IFDR   REG_ACCESS(WoReg, 0x400E1224U) /**< \brief (PIOC) Glitch Input Filter Disable Register */
#define REG_PIOC_IFSR   REG_ACCESS(RoReg, 0x400E1228U) /**< \brief (PIOC) Glitch Input Filter Status Register */
#define REG_PIOC_SODR   REG_ACCESS(WoReg, 0x400E1230U) /**< \brief (PIOC) Set Output Data Register */
#define REG_PIOC_CODR   REG_ACCESS(WoReg, 0x400E1234U) /**< \brief (PIOC) Clear Output Data Register */
#define REG_PIOC_ODSR   REG_ACCESS(RwReg, 0x400E1238U) /**< \brief (PIOC) Output Data Status Register */
#define REG_PIOC_PDSR   REG_ACCESS(RoReg, 0x400E123CU) /**< \brief (PIOC) Pin Data Status Register */
#define REG_PIOC_IER    REG_ACCESS(WoReg, 0x400E1240U) /**< \brief (PIOC) Interrupt Enable Register */
#define REG_PIOC_IDR    REG_ACCESS(WoReg, 0x400E1244U) /**< \brief (PIOC) Interrupt Disable Register */
#define REG_PIOC_IMR    REG_ACCESS(RoReg, 0x400E1248U) /**< \brief (PIOC) Interrupt Mask Register */
#define REG_PIOC_ISR    REG_ACCESS(RoReg, 0x400E124CU) /**< \brief (PIOC) Interrupt Status Register */
#define REG_PIOC_MDER   REG_ACCESS(WoReg, 0x400E1250U) /**< \brief (PIOC) Multi-driver Enable Register */
#define REG_PIOC_MDDR   REG_ACCESS(WoReg, 0x400E1254U) /**< \brief (PIOC) Multi-driver Disable Register */
#define REG_PIOC_MDSR   REG_ACCESS(RoReg, 0x400E1258U) /**< \brief (PIOC) Multi-driver Status Register */
#define REG_PIOC_PUDR   REG_ACCESS(WoReg, 0x400E1260U) /**< \brief (PIOC) Pull-up Disable Register */
#define REG_PIOC_PUER   REG_ACCESS(WoReg, 0x400E1264U) /**< \brief (PIOC) Pull-up Enable Register */
#define REG_PIOC_PUSR   REG_ACCESS(RoReg, 0x400E1268U) /**< \brief (PIOC) Pad Pull-up Status Register */
#define REG_PIOC_ABSR   REG_ACCESS(RwReg, 0x400E1270U) /**< \brief (PIOC) Peripheral AB Select Register */
#define REG_PIOC_SCIFSR REG_ACCESS(WoReg, 0x400E1280U) /**< \brief (PIOC) System Clock Glitch Input Filter Select Register */
#define REG_PIOC_DIFSR  REG_ACCESS(WoReg, 0x400E1284U) /**< \brief (PIOC) Debouncing Input Filter Select Register */
#define REG_PIOC_IFDGSR REG_ACCESS(RoReg, 0x400E1288U) /**< \brief (PIOC) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOC_SCDR   REG_ACCESS(RwReg, 0x400E128CU) /**< \brief (PIOC) Slow Clock Divider Debouncing Register */
#define REG_PIOC_OWER   REG_ACCESS(WoReg, 0x400E12A0U) /**< \brief (PIOC) Output Write Enable */
#define REG_PIOC_OWDR   REG_ACCESS(WoReg, 0x400E12A4U) /**< \brief (PIOC) Output Write Disable */
#define REG_PIOC_OWSR   REG_ACCESS(RoReg, 0x400E12A8U) /**< \brief (PIOC) Output Write Status Register */
#define REG_PIOC_AIMER  REG_ACCESS(WoReg, 0x400E12B0U) /**< \brief (PIOC) Additional Interrupt Modes Enable Register */
#define REG_PIOC_AIMDR  REG_ACCESS(WoReg, 0x400E12B4U) /**< \brief (PIOC) Additional Interrupt Modes Disables Register */
#define REG_PIOC_AIMMR  REG_ACCESS(RoReg, 0x400E12B8U) /**< \brief (PIOC) Additional Interrupt Modes Mask Register */
#define REG_PIOC_ESR    REG_ACCESS(WoReg, 0x400E12C0U) /**< \brief (PIOC) Edge Select Register */
#define REG_PIOC_LSR    REG_ACCESS(WoReg, 0x400E12C4U) /**< \brief (PIOC) Level Select Register */
#define REG_PIOC_ELSR   REG_ACCESS(RoReg, 0x400E12C8U) /**< \brief (PIOC) Edge/Level Status Register */
#define REG_PIOC_FELLSR REG_ACCESS(WoReg, 0x400E12D0U) /**< \brief (PIOC) Falling Edge/Low Level Select Register */
#define REG_PIOC_REHLSR REG_ACCESS(WoReg, 0x400E12D4U) /**< \brief (PIOC) Rising Edge/ High Level Select Register */
#define REG_PIOC_FRLHSR REG_ACCESS(RoReg, 0x400E12D8U) /**< \brief (PIOC) Fall/Rise - Low/High Status Register */
#define REG_PIOC_LOCKSR REG_ACCESS(RoReg, 0x400E12E0U) /**< \brief (PIOC) Lock Status */
#define REG_PIOC_WPMR   REG_ACCESS(RwReg, 0x400E12E4U) /**< \brief (PIOC) Write Protect Mode Register */
#define REG_PIOC_WPSR   REG_ACCESS(RoReg, 0x400E12E8U) /**< \brief (PIOC) Write Protect Status Register */

#endif /* _SAM3XA_PIOC_INSTANCE_ */
