/* $asf_license$ */

#ifndef _SAM3XA_RSTC_INSTANCE_
#define _SAM3XA_RSTC_INSTANCE_

/* ========== Register definition for RSTC peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_RSTC_CR          (0x400E1A00U) /**< \brief (RSTC) Control Register */
#define REG_RSTC_SR          (0x400E1A04U) /**< \brief (RSTC) Status Register */
#define REG_RSTC_MR          (0x400E1A08U) /**< \brief (RSTC) Mode Register */
#else
#define REG_RSTC_CR (*(WoReg*)0x400E1A00U) /**< \brief (RSTC) Control Register */
#define REG_RSTC_SR (*(RoReg*)0x400E1A04U) /**< \brief (RSTC) Status Register */
#define REG_RSTC_MR (*(RwReg*)0x400E1A08U) /**< \brief (RSTC) Mode Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_RSTC_INSTANCE_ */
