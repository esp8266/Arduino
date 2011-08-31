/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_SUPC_INSTANCE_
#define _SAM3S8_SUPC_INSTANCE_

/* ========== Register definition for SUPC peripheral ========== */
#define REG_SUPC_CR   REG_ACCESS(WoReg, 0x400E1410U) /**< \brief (SUPC) Supply Controller Control Register */
#define REG_SUPC_SMMR REG_ACCESS(RwReg, 0x400E1414U) /**< \brief (SUPC) Supply Controller Supply Monitor Mode Register */
#define REG_SUPC_MR   REG_ACCESS(RwReg, 0x400E1418U) /**< \brief (SUPC) Supply Controller Mode Register */
#define REG_SUPC_WUMR REG_ACCESS(RwReg, 0x400E141CU) /**< \brief (SUPC) Supply Controller Wake Up Mode Register */
#define REG_SUPC_WUIR REG_ACCESS(RwReg, 0x400E1420U) /**< \brief (SUPC) Supply Controller Wake Up Inputs Register */
#define REG_SUPC_SR   REG_ACCESS(RoReg, 0x400E1424U) /**< \brief (SUPC) Supply Controller Status Register */

#endif /* _SAM3S8_SUPC_INSTANCE_ */
