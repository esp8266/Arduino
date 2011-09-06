/* $asf_license$ */

#ifndef _SAM3S_SMC_INSTANCE_
#define _SAM3S_SMC_INSTANCE_

/* ========== Register definition for SMC peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_SMC_SETUP0          (0x400E0000U) /**< \brief (SMC) SMC Setup Register (CS_number = 0) */
#define REG_SMC_PULSE0          (0x400E0004U) /**< \brief (SMC) SMC Pulse Register (CS_number = 0) */
#define REG_SMC_CYCLE0          (0x400E0008U) /**< \brief (SMC) SMC Cycle Register (CS_number = 0) */
#define REG_SMC_MODE0           (0x400E000CU) /**< \brief (SMC) SMC Mode Register (CS_number = 0) */
#define REG_SMC_SETUP1          (0x400E0010U) /**< \brief (SMC) SMC Setup Register (CS_number = 1) */
#define REG_SMC_PULSE1          (0x400E0014U) /**< \brief (SMC) SMC Pulse Register (CS_number = 1) */
#define REG_SMC_CYCLE1          (0x400E0018U) /**< \brief (SMC) SMC Cycle Register (CS_number = 1) */
#define REG_SMC_MODE1           (0x400E001CU) /**< \brief (SMC) SMC Mode Register (CS_number = 1) */
#define REG_SMC_SETUP2          (0x400E0020U) /**< \brief (SMC) SMC Setup Register (CS_number = 2) */
#define REG_SMC_PULSE2          (0x400E0024U) /**< \brief (SMC) SMC Pulse Register (CS_number = 2) */
#define REG_SMC_CYCLE2          (0x400E0028U) /**< \brief (SMC) SMC Cycle Register (CS_number = 2) */
#define REG_SMC_MODE2           (0x400E002CU) /**< \brief (SMC) SMC Mode Register (CS_number = 2) */
#define REG_SMC_SETUP3          (0x400E0030U) /**< \brief (SMC) SMC Setup Register (CS_number = 3) */
#define REG_SMC_PULSE3          (0x400E0034U) /**< \brief (SMC) SMC Pulse Register (CS_number = 3) */
#define REG_SMC_CYCLE3          (0x400E0038U) /**< \brief (SMC) SMC Cycle Register (CS_number = 3) */
#define REG_SMC_MODE3           (0x400E003CU) /**< \brief (SMC) SMC Mode Register (CS_number = 3) */
#define REG_SMC_SETUP4          (0x400E0040U) /**< \brief (SMC) SMC Setup Register (CS_number = 4) */
#define REG_SMC_PULSE4          (0x400E0044U) /**< \brief (SMC) SMC Pulse Register (CS_number = 4) */
#define REG_SMC_CYCLE4          (0x400E0048U) /**< \brief (SMC) SMC Cycle Register (CS_number = 4) */
#define REG_SMC_MODE4           (0x400E004CU) /**< \brief (SMC) SMC Mode Register (CS_number = 4) */
#define REG_SMC_OCMS            (0x400E0080U) /**< \brief (SMC) SMC OCMS MODE Register */
#define REG_SMC_KEY1            (0x400E0084U) /**< \brief (SMC) SMC OCMS KEY1 Register */
#define REG_SMC_KEY2            (0x400E0088U) /**< \brief (SMC) SMC OCMS KEY2 Register */
#define REG_SMC_WPMR            (0x400E00E4U) /**< \brief (SMC) SMC Write Protect Mode Register */
#define REG_SMC_WPSR            (0x400E00E8U) /**< \brief (SMC) SMC Write Protect Status Register */
#else
#define REG_SMC_SETUP0 (*(RwReg*)0x400E0000U) /**< \brief (SMC) SMC Setup Register (CS_number = 0) */
#define REG_SMC_PULSE0 (*(RwReg*)0x400E0004U) /**< \brief (SMC) SMC Pulse Register (CS_number = 0) */
#define REG_SMC_CYCLE0 (*(RwReg*)0x400E0008U) /**< \brief (SMC) SMC Cycle Register (CS_number = 0) */
#define REG_SMC_MODE0  (*(RwReg*)0x400E000CU) /**< \brief (SMC) SMC Mode Register (CS_number = 0) */
#define REG_SMC_SETUP1 (*(RwReg*)0x400E0010U) /**< \brief (SMC) SMC Setup Register (CS_number = 1) */
#define REG_SMC_PULSE1 (*(RwReg*)0x400E0014U) /**< \brief (SMC) SMC Pulse Register (CS_number = 1) */
#define REG_SMC_CYCLE1 (*(RwReg*)0x400E0018U) /**< \brief (SMC) SMC Cycle Register (CS_number = 1) */
#define REG_SMC_MODE1  (*(RwReg*)0x400E001CU) /**< \brief (SMC) SMC Mode Register (CS_number = 1) */
#define REG_SMC_SETUP2 (*(RwReg*)0x400E0020U) /**< \brief (SMC) SMC Setup Register (CS_number = 2) */
#define REG_SMC_PULSE2 (*(RwReg*)0x400E0024U) /**< \brief (SMC) SMC Pulse Register (CS_number = 2) */
#define REG_SMC_CYCLE2 (*(RwReg*)0x400E0028U) /**< \brief (SMC) SMC Cycle Register (CS_number = 2) */
#define REG_SMC_MODE2  (*(RwReg*)0x400E002CU) /**< \brief (SMC) SMC Mode Register (CS_number = 2) */
#define REG_SMC_SETUP3 (*(RwReg*)0x400E0030U) /**< \brief (SMC) SMC Setup Register (CS_number = 3) */
#define REG_SMC_PULSE3 (*(RwReg*)0x400E0034U) /**< \brief (SMC) SMC Pulse Register (CS_number = 3) */
#define REG_SMC_CYCLE3 (*(RwReg*)0x400E0038U) /**< \brief (SMC) SMC Cycle Register (CS_number = 3) */
#define REG_SMC_MODE3  (*(RwReg*)0x400E003CU) /**< \brief (SMC) SMC Mode Register (CS_number = 3) */
#define REG_SMC_SETUP4 (*(RwReg*)0x400E0040U) /**< \brief (SMC) SMC Setup Register (CS_number = 4) */
#define REG_SMC_PULSE4 (*(RwReg*)0x400E0044U) /**< \brief (SMC) SMC Pulse Register (CS_number = 4) */
#define REG_SMC_CYCLE4 (*(RwReg*)0x400E0048U) /**< \brief (SMC) SMC Cycle Register (CS_number = 4) */
#define REG_SMC_MODE4  (*(RwReg*)0x400E004CU) /**< \brief (SMC) SMC Mode Register (CS_number = 4) */
#define REG_SMC_OCMS   (*(RwReg*)0x400E0080U) /**< \brief (SMC) SMC OCMS MODE Register */
#define REG_SMC_KEY1   (*(WoReg*)0x400E0084U) /**< \brief (SMC) SMC OCMS KEY1 Register */
#define REG_SMC_KEY2   (*(WoReg*)0x400E0088U) /**< \brief (SMC) SMC OCMS KEY2 Register */
#define REG_SMC_WPMR   (*(RwReg*)0x400E00E4U) /**< \brief (SMC) SMC Write Protect Mode Register */
#define REG_SMC_WPSR   (*(RoReg*)0x400E00E8U) /**< \brief (SMC) SMC Write Protect Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3S_SMC_INSTANCE_ */
