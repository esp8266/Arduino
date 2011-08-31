/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_SUPC_INSTANCE_
#define _SAM3XA_SUPC_INSTANCE_

/* ========== Register definition for SUPC peripheral ========== */
#define REG_SUPC_CR   REG_ACCESS(WoReg, 0x400E1A10U) /**< \brief (SUPC) Supply Controller Control Register */
#define REG_SUPC_SMMR REG_ACCESS(RwReg, 0x400E1A14U) /**< \brief (SUPC) Supply Controller Supply Monitor Mode Register */
#define REG_SUPC_MR   REG_ACCESS(RwReg, 0x400E1A18U) /**< \brief (SUPC) Supply Controller Mode Register */
#define REG_SUPC_WUMR REG_ACCESS(RwReg, 0x400E1A1CU) /**< \brief (SUPC) Supply Controller Wake Up Mode Register */
#define REG_SUPC_WUIR REG_ACCESS(RwReg, 0x400E1A20U) /**< \brief (SUPC) Supply Controller Wake Up Inputs Register */
#define REG_SUPC_SR   REG_ACCESS(RoReg, 0x400E1A24U) /**< \brief (SUPC) Supply Controller Status Register */

#endif /* _SAM3XA_SUPC_INSTANCE_ */
