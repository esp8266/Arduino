/* %ATMEL_LICENCE% */

#ifndef _SAM3U_SSC_INSTANCE_
#define _SAM3U_SSC_INSTANCE_

/* ========== Register definition for SSC peripheral ========== */
#define REG_SSC_CR   REG_ACCESS(WoReg, 0x40004000U) /**< \brief (SSC) Control Register */
#define REG_SSC_CMR  REG_ACCESS(RwReg, 0x40004004U) /**< \brief (SSC) Clock Mode Register */
#define REG_SSC_RCMR REG_ACCESS(RwReg, 0x40004010U) /**< \brief (SSC) Receive Clock Mode Register */
#define REG_SSC_RFMR REG_ACCESS(RwReg, 0x40004014U) /**< \brief (SSC) Receive Frame Mode Register */
#define REG_SSC_TCMR REG_ACCESS(RwReg, 0x40004018U) /**< \brief (SSC) Transmit Clock Mode Register */
#define REG_SSC_TFMR REG_ACCESS(RwReg, 0x4000401CU) /**< \brief (SSC) Transmit Frame Mode Register */
#define REG_SSC_RHR  REG_ACCESS(RoReg, 0x40004020U) /**< \brief (SSC) Receive Holding Register */
#define REG_SSC_THR  REG_ACCESS(WoReg, 0x40004024U) /**< \brief (SSC) Transmit Holding Register */
#define REG_SSC_RSHR REG_ACCESS(RoReg, 0x40004030U) /**< \brief (SSC) Receive Sync. Holding Register */
#define REG_SSC_TSHR REG_ACCESS(RwReg, 0x40004034U) /**< \brief (SSC) Transmit Sync. Holding Register */
#define REG_SSC_RC0R REG_ACCESS(RwReg, 0x40004038U) /**< \brief (SSC) Receive Compare 0 Register */
#define REG_SSC_RC1R REG_ACCESS(RwReg, 0x4000403CU) /**< \brief (SSC) Receive Compare 1 Register */
#define REG_SSC_SR   REG_ACCESS(RoReg, 0x40004040U) /**< \brief (SSC) Status Register */
#define REG_SSC_IER  REG_ACCESS(WoReg, 0x40004044U) /**< \brief (SSC) Interrupt Enable Register */
#define REG_SSC_IDR  REG_ACCESS(WoReg, 0x40004048U) /**< \brief (SSC) Interrupt Disable Register */
#define REG_SSC_IMR  REG_ACCESS(RoReg, 0x4000404CU) /**< \brief (SSC) Interrupt Mask Register */
#define REG_SSC_WPMR REG_ACCESS(RwReg, 0x400040E4U) /**< \brief (SSC) Write Protect Mode Register */
#define REG_SSC_WPSR REG_ACCESS(RoReg, 0x400040E8U) /**< \brief (SSC) Write Protect Status Register */

#endif /* _SAM3U_SSC_INSTANCE_ */
