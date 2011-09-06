/* $asf_license$ */

#ifndef _SAM3XA_WDT_INSTANCE_
#define _SAM3XA_WDT_INSTANCE_

/* ========== Register definition for WDT peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_WDT_CR          (0x400E1A50U) /**< \brief (WDT) Control Register */
#define REG_WDT_MR          (0x400E1A54U) /**< \brief (WDT) Mode Register */
#define REG_WDT_SR          (0x400E1A58U) /**< \brief (WDT) Status Register */
#else
#define REG_WDT_CR (*(WoReg*)0x400E1A50U) /**< \brief (WDT) Control Register */
#define REG_WDT_MR (*(RwReg*)0x400E1A54U) /**< \brief (WDT) Mode Register */
#define REG_WDT_SR (*(RoReg*)0x400E1A58U) /**< \brief (WDT) Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_WDT_INSTANCE_ */
