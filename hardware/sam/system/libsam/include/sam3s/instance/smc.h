/* %ATMEL_LICENCE% */

#ifndef _SAM3S_SMC_INSTANCE_
#define _SAM3S_SMC_INSTANCE_

/* ========== Register definition for SMC peripheral ========== */
#define REG_SMC_SETUP0 REG_ACCESS(RwReg, 0x400E0000U) /**< \brief (SMC) SMC Setup Register (CS_number = 0) */
#define REG_SMC_PULSE0 REG_ACCESS(RwReg, 0x400E0004U) /**< \brief (SMC) SMC Pulse Register (CS_number = 0) */
#define REG_SMC_CYCLE0 REG_ACCESS(RwReg, 0x400E0008U) /**< \brief (SMC) SMC Cycle Register (CS_number = 0) */
#define REG_SMC_MODE0  REG_ACCESS(RwReg, 0x400E000CU) /**< \brief (SMC) SMC Mode Register (CS_number = 0) */
#define REG_SMC_SETUP1 REG_ACCESS(RwReg, 0x400E0010U) /**< \brief (SMC) SMC Setup Register (CS_number = 1) */
#define REG_SMC_PULSE1 REG_ACCESS(RwReg, 0x400E0014U) /**< \brief (SMC) SMC Pulse Register (CS_number = 1) */
#define REG_SMC_CYCLE1 REG_ACCESS(RwReg, 0x400E0018U) /**< \brief (SMC) SMC Cycle Register (CS_number = 1) */
#define REG_SMC_MODE1  REG_ACCESS(RwReg, 0x400E001CU) /**< \brief (SMC) SMC Mode Register (CS_number = 1) */
#define REG_SMC_SETUP2 REG_ACCESS(RwReg, 0x400E0020U) /**< \brief (SMC) SMC Setup Register (CS_number = 2) */
#define REG_SMC_PULSE2 REG_ACCESS(RwReg, 0x400E0024U) /**< \brief (SMC) SMC Pulse Register (CS_number = 2) */
#define REG_SMC_CYCLE2 REG_ACCESS(RwReg, 0x400E0028U) /**< \brief (SMC) SMC Cycle Register (CS_number = 2) */
#define REG_SMC_MODE2  REG_ACCESS(RwReg, 0x400E002CU) /**< \brief (SMC) SMC Mode Register (CS_number = 2) */
#define REG_SMC_SETUP3 REG_ACCESS(RwReg, 0x400E0030U) /**< \brief (SMC) SMC Setup Register (CS_number = 3) */
#define REG_SMC_PULSE3 REG_ACCESS(RwReg, 0x400E0034U) /**< \brief (SMC) SMC Pulse Register (CS_number = 3) */
#define REG_SMC_CYCLE3 REG_ACCESS(RwReg, 0x400E0038U) /**< \brief (SMC) SMC Cycle Register (CS_number = 3) */
#define REG_SMC_MODE3  REG_ACCESS(RwReg, 0x400E003CU) /**< \brief (SMC) SMC Mode Register (CS_number = 3) */
#define REG_SMC_SETUP4 REG_ACCESS(RwReg, 0x400E0040U) /**< \brief (SMC) SMC Setup Register (CS_number = 4) */
#define REG_SMC_PULSE4 REG_ACCESS(RwReg, 0x400E0044U) /**< \brief (SMC) SMC Pulse Register (CS_number = 4) */
#define REG_SMC_CYCLE4 REG_ACCESS(RwReg, 0x400E0048U) /**< \brief (SMC) SMC Cycle Register (CS_number = 4) */
#define REG_SMC_MODE4  REG_ACCESS(RwReg, 0x400E004CU) /**< \brief (SMC) SMC Mode Register (CS_number = 4) */
#define REG_SMC_OCMS   REG_ACCESS(RwReg, 0x400E0080U) /**< \brief (SMC) SMC OCMS MODE Register */
#define REG_SMC_KEY1   REG_ACCESS(WoReg, 0x400E0084U) /**< \brief (SMC) SMC OCMS KEY1 Register */
#define REG_SMC_KEY2   REG_ACCESS(WoReg, 0x400E0088U) /**< \brief (SMC) SMC OCMS KEY2 Register */
#define REG_SMC_WPMR   REG_ACCESS(RwReg, 0x400E00E4U) /**< \brief (SMC) SMC Write Protect Mode Register */
#define REG_SMC_WPSR   REG_ACCESS(RoReg, 0x400E00E8U) /**< \brief (SMC) SMC Write Protect Status Register */

#endif /* _SAM3S_SMC_INSTANCE_ */
