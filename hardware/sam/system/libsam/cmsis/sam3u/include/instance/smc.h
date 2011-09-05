/* %ATMEL_LICENCE% */

#ifndef _SAM3U_SMC_INSTANCE_
#define _SAM3U_SMC_INSTANCE_

/* ========== Register definition for SMC peripheral ========== */
#define REG_SMC_CFG      REG_ACCESS(RwReg, 0x400E0000U) /**< \brief (SMC) SMC NFC Configuration Register */
#define REG_SMC_CTRL     REG_ACCESS(WoReg, 0x400E0004U) /**< \brief (SMC) SMC NFC Control Register */
#define REG_SMC_SR       REG_ACCESS(RoReg, 0x400E0008U) /**< \brief (SMC) SMC NFC Status Register */
#define REG_SMC_IER      REG_ACCESS(WoReg, 0x400E000CU) /**< \brief (SMC) SMC NFC Interrupt Enable Register */
#define REG_SMC_IDR      REG_ACCESS(WoReg, 0x400E0010U) /**< \brief (SMC) SMC NFC Interrupt Disable Register */
#define REG_SMC_IMR      REG_ACCESS(RoReg, 0x400E0014U) /**< \brief (SMC) SMC NFC Interrupt Mask Register */
#define REG_SMC_ADDR     REG_ACCESS(RwReg, 0x400E0018U) /**< \brief (SMC) SMC NFC Address Cycle Zero Register */
#define REG_SMC_BANK     REG_ACCESS(RwReg, 0x400E001CU) /**< \brief (SMC) SMC Bank Address Register */
#define REG_SMC_ECC_CTRL REG_ACCESS(WoReg, 0x400E0020U) /**< \brief (SMC) SMC ECC Control Register */
#define REG_SMC_ECC_MD   REG_ACCESS(RwReg, 0x400E0024U) /**< \brief (SMC) SMC ECC Mode Register */
#define REG_SMC_ECC_SR1  REG_ACCESS(RoReg, 0x400E0028U) /**< \brief (SMC) SMC ECC Status 1 Register */
#define REG_SMC_ECC_PR0  REG_ACCESS(RoReg, 0x400E002CU) /**< \brief (SMC) SMC ECC Parity 0 Register */
#define REG_SMC_ECC_PR1  REG_ACCESS(RoReg, 0x400E0030U) /**< \brief (SMC) SMC ECC parity 1 Register */
#define REG_SMC_ECC_SR2  REG_ACCESS(RoReg, 0x400E0034U) /**< \brief (SMC) SMC ECC status 2 Register */
#define REG_SMC_ECC_PR2  REG_ACCESS(RoReg, 0x400E0038U) /**< \brief (SMC) SMC ECC parity 2 Register */
#define REG_SMC_ECC_PR3  REG_ACCESS(RoReg, 0x400E003CU) /**< \brief (SMC) SMC ECC parity 3 Register */
#define REG_SMC_ECC_PR4  REG_ACCESS(RoReg, 0x400E0040U) /**< \brief (SMC) SMC ECC parity 4 Register */
#define REG_SMC_ECC_PR5  REG_ACCESS(RoReg, 0x400E0044U) /**< \brief (SMC) SMC ECC parity 5 Register */
#define REG_SMC_ECC_PR6  REG_ACCESS(RoReg, 0x400E0048U) /**< \brief (SMC) SMC ECC parity 6 Register */
#define REG_SMC_ECC_PR7  REG_ACCESS(RoReg, 0x400E004CU) /**< \brief (SMC) SMC ECC parity 7 Register */
#define REG_SMC_ECC_PR8  REG_ACCESS(RoReg, 0x400E0050U) /**< \brief (SMC) SMC ECC parity 8 Register */
#define REG_SMC_ECC_PR9  REG_ACCESS(RoReg, 0x400E0054U) /**< \brief (SMC) SMC ECC parity 9 Register */
#define REG_SMC_ECC_PR10 REG_ACCESS(RoReg, 0x400E0058U) /**< \brief (SMC) SMC ECC parity 10 Register */
#define REG_SMC_ECC_PR11 REG_ACCESS(RoReg, 0x400E005CU) /**< \brief (SMC) SMC ECC parity 11 Register */
#define REG_SMC_ECC_PR12 REG_ACCESS(RoReg, 0x400E0060U) /**< \brief (SMC) SMC ECC parity 12 Register */
#define REG_SMC_ECC_PR13 REG_ACCESS(RoReg, 0x400E0064U) /**< \brief (SMC) SMC ECC parity 13 Register */
#define REG_SMC_ECC_PR14 REG_ACCESS(RoReg, 0x400E0068U) /**< \brief (SMC) SMC ECC parity 14 Register */
#define REG_SMC_ECC_PR15 REG_ACCESS(RoReg, 0x400E006CU) /**< \brief (SMC) SMC ECC parity 15 Register */
#define REG_SMC_SETUP0   REG_ACCESS(RwReg, 0x400E0070U) /**< \brief (SMC) SMC Setup Register (CS_number = 0) */
#define REG_SMC_PULSE0   REG_ACCESS(RwReg, 0x400E0074U) /**< \brief (SMC) SMC Pulse Register (CS_number = 0) */
#define REG_SMC_CYCLE0   REG_ACCESS(RwReg, 0x400E0078U) /**< \brief (SMC) SMC Cycle Register (CS_number = 0) */
#define REG_SMC_TIMINGS0 REG_ACCESS(RwReg, 0x400E007CU) /**< \brief (SMC) SMC Timings Register (CS_number = 0) */
#define REG_SMC_MODE0    REG_ACCESS(RwReg, 0x400E0080U) /**< \brief (SMC) SMC Mode Register (CS_number = 0) */
#define REG_SMC_SETUP1   REG_ACCESS(RwReg, 0x400E0084U) /**< \brief (SMC) SMC Setup Register (CS_number = 1) */
#define REG_SMC_PULSE1   REG_ACCESS(RwReg, 0x400E0088U) /**< \brief (SMC) SMC Pulse Register (CS_number = 1) */
#define REG_SMC_CYCLE1   REG_ACCESS(RwReg, 0x400E008CU) /**< \brief (SMC) SMC Cycle Register (CS_number = 1) */
#define REG_SMC_TIMINGS1 REG_ACCESS(RwReg, 0x400E0090U) /**< \brief (SMC) SMC Timings Register (CS_number = 1) */
#define REG_SMC_MODE1    REG_ACCESS(RwReg, 0x400E0094U) /**< \brief (SMC) SMC Mode Register (CS_number = 1) */
#define REG_SMC_SETUP2   REG_ACCESS(RwReg, 0x400E0098U) /**< \brief (SMC) SMC Setup Register (CS_number = 2) */
#define REG_SMC_PULSE2   REG_ACCESS(RwReg, 0x400E009CU) /**< \brief (SMC) SMC Pulse Register (CS_number = 2) */
#define REG_SMC_CYCLE2   REG_ACCESS(RwReg, 0x400E00A0U) /**< \brief (SMC) SMC Cycle Register (CS_number = 2) */
#define REG_SMC_TIMINGS2 REG_ACCESS(RwReg, 0x400E00A4U) /**< \brief (SMC) SMC Timings Register (CS_number = 2) */
#define REG_SMC_MODE2    REG_ACCESS(RwReg, 0x400E00A8U) /**< \brief (SMC) SMC Mode Register (CS_number = 2) */
#define REG_SMC_SETUP3   REG_ACCESS(RwReg, 0x400E00ACU) /**< \brief (SMC) SMC Setup Register (CS_number = 3) */
#define REG_SMC_PULSE3   REG_ACCESS(RwReg, 0x400E00B0U) /**< \brief (SMC) SMC Pulse Register (CS_number = 3) */
#define REG_SMC_CYCLE3   REG_ACCESS(RwReg, 0x400E00B4U) /**< \brief (SMC) SMC Cycle Register (CS_number = 3) */
#define REG_SMC_TIMINGS3 REG_ACCESS(RwReg, 0x400E00B8U) /**< \brief (SMC) SMC Timings Register (CS_number = 3) */
#define REG_SMC_MODE3    REG_ACCESS(RwReg, 0x400E00BCU) /**< \brief (SMC) SMC Mode Register (CS_number = 3) */
#define REG_SMC_OCMS     REG_ACCESS(RwReg, 0x400E0110U) /**< \brief (SMC) SMC OCMS Register */
#define REG_SMC_KEY1     REG_ACCESS(WoReg, 0x400E0114U) /**< \brief (SMC) SMC OCMS KEY1 Register */
#define REG_SMC_KEY2     REG_ACCESS(WoReg, 0x400E0118U) /**< \brief (SMC) SMC OCMS KEY2 Register */
#define REG_SMC_WPCR     REG_ACCESS(WoReg, 0x400E01E4U) /**< \brief (SMC) Write Protection Control Register */
#define REG_SMC_WPSR     REG_ACCESS(RoReg, 0x400E01E8U) /**< \brief (SMC) Write Protection Status Register */

#endif /* _SAM3U_SMC_INSTANCE_ */
