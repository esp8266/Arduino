/* $asf_license$ */

#ifndef _SAM3S_RSTC_INSTANCE_
#define _SAM3S_RSTC_INSTANCE_

/* ========== Register definition for RSTC peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_RSTC_CR          (0x400E1400U) /**< \brief (RSTC) Control Register */
#define REG_RSTC_SR          (0x400E1404U) /**< \brief (RSTC) Status Register */
#define REG_RSTC_MR          (0x400E1408U) /**< \brief (RSTC) Mode Register */
#else
#define REG_RSTC_CR (*(WoReg*)0x400E1400U) /**< \brief (RSTC) Control Register */
#define REG_RSTC_SR (*(RoReg*)0x400E1404U) /**< \brief (RSTC) Status Register */
#define REG_RSTC_MR (*(RwReg*)0x400E1408U) /**< \brief (RSTC) Mode Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3S_RSTC_INSTANCE_ */
