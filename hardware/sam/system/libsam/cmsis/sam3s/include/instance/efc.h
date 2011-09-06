/* $asf_license$ */

#ifndef _SAM3S_EFC_INSTANCE_
#define _SAM3S_EFC_INSTANCE_

/* ========== Register definition for EFC peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_EFC_FMR           (0x400E0A00U) /**< \brief (EFC) EEFC Flash Mode Register */
#define REG_EFC_FCR           (0x400E0A04U) /**< \brief (EFC) EEFC Flash Command Register */
#define REG_EFC_FSR           (0x400E0A08U) /**< \brief (EFC) EEFC Flash Status Register */
#define REG_EFC_FRR           (0x400E0A0CU) /**< \brief (EFC) EEFC Flash Result Register */
#else
#define REG_EFC_FMR  (*(RwReg*)0x400E0A00U) /**< \brief (EFC) EEFC Flash Mode Register */
#define REG_EFC_FCR  (*(WoReg*)0x400E0A04U) /**< \brief (EFC) EEFC Flash Command Register */
#define REG_EFC_FSR  (*(RoReg*)0x400E0A08U) /**< \brief (EFC) EEFC Flash Status Register */
#define REG_EFC_FRR  (*(RoReg*)0x400E0A0CU) /**< \brief (EFC) EEFC Flash Result Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3S_EFC_INSTANCE_ */
