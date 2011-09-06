/* $asf_license$ */

#ifndef _SAM3XA_EFC0_INSTANCE_
#define _SAM3XA_EFC0_INSTANCE_

/* ========== Register definition for EFC0 peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_EFC0_FMR           (0x400E0A00U) /**< \brief (EFC0) EEFC Flash Mode Register */
#define REG_EFC0_FCR           (0x400E0A04U) /**< \brief (EFC0) EEFC Flash Command Register */
#define REG_EFC0_FSR           (0x400E0A08U) /**< \brief (EFC0) EEFC Flash Status Register */
#define REG_EFC0_FRR           (0x400E0A0CU) /**< \brief (EFC0) EEFC Flash Result Register */
#else
#define REG_EFC0_FMR  (*(RwReg*)0x400E0A00U) /**< \brief (EFC0) EEFC Flash Mode Register */
#define REG_EFC0_FCR  (*(WoReg*)0x400E0A04U) /**< \brief (EFC0) EEFC Flash Command Register */
#define REG_EFC0_FSR  (*(RoReg*)0x400E0A08U) /**< \brief (EFC0) EEFC Flash Status Register */
#define REG_EFC0_FRR  (*(RoReg*)0x400E0A0CU) /**< \brief (EFC0) EEFC Flash Result Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_EFC0_INSTANCE_ */
