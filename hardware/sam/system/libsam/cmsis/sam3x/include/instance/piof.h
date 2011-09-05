/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_PIOF_INSTANCE_
#define _SAM3XA_PIOF_INSTANCE_

/* ========== Register definition for PIOF peripheral ========== */
#define REG_PIOF_PER    REG_ACCESS(WoReg, 0x400E1800U) /**< \brief (PIOF) PIO Enable Register */
#define REG_PIOF_PDR    REG_ACCESS(WoReg, 0x400E1804U) /**< \brief (PIOF) PIO Disable Register */
#define REG_PIOF_PSR    REG_ACCESS(RoReg, 0x400E1808U) /**< \brief (PIOF) PIO Status Register */
#define REG_PIOF_OER    REG_ACCESS(WoReg, 0x400E1810U) /**< \brief (PIOF) Output Enable Register */
#define REG_PIOF_ODR    REG_ACCESS(WoReg, 0x400E1814U) /**< \brief (PIOF) Output Disable Register */
#define REG_PIOF_OSR    REG_ACCESS(RoReg, 0x400E1818U) /**< \brief (PIOF) Output Status Register */
#define REG_PIOF_IFER   REG_ACCESS(WoReg, 0x400E1820U) /**< \brief (PIOF) Glitch Input Filter Enable Register */
#define REG_PIOF_IFDR   REG_ACCESS(WoReg, 0x400E1824U) /**< \brief (PIOF) Glitch Input Filter Disable Register */
#define REG_PIOF_IFSR   REG_ACCESS(RoReg, 0x400E1828U) /**< \brief (PIOF) Glitch Input Filter Status Register */
#define REG_PIOF_SODR   REG_ACCESS(WoReg, 0x400E1830U) /**< \brief (PIOF) Set Output Data Register */
#define REG_PIOF_CODR   REG_ACCESS(WoReg, 0x400E1834U) /**< \brief (PIOF) Clear Output Data Register */
#define REG_PIOF_ODSR   REG_ACCESS(RwReg, 0x400E1838U) /**< \brief (PIOF) Output Data Status Register */
#define REG_PIOF_PDSR   REG_ACCESS(RoReg, 0x400E183CU) /**< \brief (PIOF) Pin Data Status Register */
#define REG_PIOF_IER    REG_ACCESS(WoReg, 0x400E1840U) /**< \brief (PIOF) Interrupt Enable Register */
#define REG_PIOF_IDR    REG_ACCESS(WoReg, 0x400E1844U) /**< \brief (PIOF) Interrupt Disable Register */
#define REG_PIOF_IMR    REG_ACCESS(RoReg, 0x400E1848U) /**< \brief (PIOF) Interrupt Mask Register */
#define REG_PIOF_ISR    REG_ACCESS(RoReg, 0x400E184CU) /**< \brief (PIOF) Interrupt Status Register */
#define REG_PIOF_MDER   REG_ACCESS(WoReg, 0x400E1850U) /**< \brief (PIOF) Multi-driver Enable Register */
#define REG_PIOF_MDDR   REG_ACCESS(WoReg, 0x400E1854U) /**< \brief (PIOF) Multi-driver Disable Register */
#define REG_PIOF_MDSR   REG_ACCESS(RoReg, 0x400E1858U) /**< \brief (PIOF) Multi-driver Status Register */
#define REG_PIOF_PUDR   REG_ACCESS(WoReg, 0x400E1860U) /**< \brief (PIOF) Pull-up Disable Register */
#define REG_PIOF_PUER   REG_ACCESS(WoReg, 0x400E1864U) /**< \brief (PIOF) Pull-up Enable Register */
#define REG_PIOF_PUSR   REG_ACCESS(RoReg, 0x400E1868U) /**< \brief (PIOF) Pad Pull-up Status Register */
#define REG_PIOF_ABSR   REG_ACCESS(RwReg, 0x400E1870U) /**< \brief (PIOF) Peripheral AB Select Register */
#define REG_PIOF_SCIFSR REG_ACCESS(WoReg, 0x400E1880U) /**< \brief (PIOF) System Clock Glitch Input Filter Select Register */
#define REG_PIOF_DIFSR  REG_ACCESS(WoReg, 0x400E1884U) /**< \brief (PIOF) Debouncing Input Filter Select Register */
#define REG_PIOF_IFDGSR REG_ACCESS(RoReg, 0x400E1888U) /**< \brief (PIOF) Glitch or Debouncing Input Filter Clock Selection Status Register */
#define REG_PIOF_SCDR   REG_ACCESS(RwReg, 0x400E188CU) /**< \brief (PIOF) Slow Clock Divider Debouncing Register */
#define REG_PIOF_OWER   REG_ACCESS(WoReg, 0x400E18A0U) /**< \brief (PIOF) Output Write Enable */
#define REG_PIOF_OWDR   REG_ACCESS(WoReg, 0x400E18A4U) /**< \brief (PIOF) Output Write Disable */
#define REG_PIOF_OWSR   REG_ACCESS(RoReg, 0x400E18A8U) /**< \brief (PIOF) Output Write Status Register */
#define REG_PIOF_AIMER  REG_ACCESS(WoReg, 0x400E18B0U) /**< \brief (PIOF) Additional Interrupt Modes Enable Register */
#define REG_PIOF_AIMDR  REG_ACCESS(WoReg, 0x400E18B4U) /**< \brief (PIOF) Additional Interrupt Modes Disables Register */
#define REG_PIOF_AIMMR  REG_ACCESS(RoReg, 0x400E18B8U) /**< \brief (PIOF) Additional Interrupt Modes Mask Register */
#define REG_PIOF_ESR    REG_ACCESS(WoReg, 0x400E18C0U) /**< \brief (PIOF) Edge Select Register */
#define REG_PIOF_LSR    REG_ACCESS(WoReg, 0x400E18C4U) /**< \brief (PIOF) Level Select Register */
#define REG_PIOF_ELSR   REG_ACCESS(RoReg, 0x400E18C8U) /**< \brief (PIOF) Edge/Level Status Register */
#define REG_PIOF_FELLSR REG_ACCESS(WoReg, 0x400E18D0U) /**< \brief (PIOF) Falling Edge/Low Level Select Register */
#define REG_PIOF_REHLSR REG_ACCESS(WoReg, 0x400E18D4U) /**< \brief (PIOF) Rising Edge/ High Level Select Register */
#define REG_PIOF_FRLHSR REG_ACCESS(RoReg, 0x400E18D8U) /**< \brief (PIOF) Fall/Rise - Low/High Status Register */
#define REG_PIOF_LOCKSR REG_ACCESS(RoReg, 0x400E18E0U) /**< \brief (PIOF) Lock Status */
#define REG_PIOF_WPMR   REG_ACCESS(RwReg, 0x400E18E4U) /**< \brief (PIOF) Write Protect Mode Register */
#define REG_PIOF_WPSR   REG_ACCESS(RoReg, 0x400E18E8U) /**< \brief (PIOF) Write Protect Status Register */

#endif /* _SAM3XA_PIOF_INSTANCE_ */
