/* $asf_license$ */

#ifndef _SAM3XA_EFC1_INSTANCE_
#define _SAM3XA_EFC1_INSTANCE_

/* ========== Register definition for EFC1 peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_EFC1_FMR           (0x400E0C00U) /**< \brief (EFC1) EEFC Flash Mode Register */
#define REG_EFC1_FCR           (0x400E0C04U) /**< \brief (EFC1) EEFC Flash Command Register */
#define REG_EFC1_FSR           (0x400E0C08U) /**< \brief (EFC1) EEFC Flash Status Register */
#define REG_EFC1_FRR           (0x400E0C0CU) /**< \brief (EFC1) EEFC Flash Result Register */
#else
#define REG_EFC1_FMR  (*(RwReg*)0x400E0C00U) /**< \brief (EFC1) EEFC Flash Mode Register */
#define REG_EFC1_FCR  (*(WoReg*)0x400E0C04U) /**< \brief (EFC1) EEFC Flash Command Register */
#define REG_EFC1_FSR  (*(RoReg*)0x400E0C08U) /**< \brief (EFC1) EEFC Flash Status Register */
#define REG_EFC1_FRR  (*(RoReg*)0x400E0C0CU) /**< \brief (EFC1) EEFC Flash Result Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3XA_EFC1_INSTANCE_ */
