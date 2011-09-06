/* $asf_license$ */

#ifndef _SAM3XA_SUPC_INSTANCE_
#define _SAM3XA_SUPC_INSTANCE_

/* ========== Register definition for SUPC peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_SUPC_CR            (0x400E1A10U) /**< \brief (SUPC) Supply Controller Control Register */
#define REG_SUPC_SMMR          (0x400E1A14U) /**< \brief (SUPC) Supply Controller Supply Monitor Mode Register */
#define REG_SUPC_MR            (0x400E1A18U) /**< \brief (SUPC) Supply Controller Mode Register */
#define REG_SUPC_WUMR          (0x400E1A1CU) /**< \brief (SUPC) Supply Controller Wake Up Mode Register */
#define REG_SUPC_WUIR          (0x400E1A20U) /**< \brief (SUPC) Supply Controller Wake Up Inputs Register */
#define REG_SUPC_SR            (0x400E1A24U) /**< \brief (SUPC) Supply Controller Status Register */
#else
#define REG_SUPC_CR   (*(WoReg*)0x400E1A10U) /**< \brief (SUPC) Supply Controller Control Register */
#define REG_SUPC_SMMR (*(RwReg*)0x400E1A14U) /**< \brief (SUPC) Supply Controller Supply Monitor Mode Register */
#define REG_SUPC_MR   (*(RwReg*)0x400E1A18U) /**< \brief (SUPC) Supply Controller Mode Register */
#define REG_SUPC_WUMR (*(RwReg*)0x400E1A1CU) /**< \brief (SUPC) Supply Controller Wake Up Mode Register */
#define REG_SUPC_WUIR (*(RwReg*)0x400E1A20U) /**< \brief (SUPC) Supply Controller Wake Up Inputs Register */
#define REG_SUPC_SR   (*(RoReg*)0x400E1A24U) /**< \brief (SUPC) Supply Controller Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_SUPC_INSTANCE_ */
