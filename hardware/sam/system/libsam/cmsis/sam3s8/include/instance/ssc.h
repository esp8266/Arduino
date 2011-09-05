/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_SSC_INSTANCE_
#define _SAM3S8_SSC_INSTANCE_

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
#define REG_SSC_RPR  REG_ACCESS(RwReg, 0x40004100U) /**< \brief (SSC) Receive Pointer Register */
#define REG_SSC_RCR  REG_ACCESS(RwReg, 0x40004104U) /**< \brief (SSC) Receive Counter Register */
#define REG_SSC_TPR  REG_ACCESS(RwReg, 0x40004108U) /**< \brief (SSC) Transmit Pointer Register */
#define REG_SSC_TCR  REG_ACCESS(RwReg, 0x4000410CU) /**< \brief (SSC) Transmit Counter Register */
#define REG_SSC_RNPR REG_ACCESS(RwReg, 0x40004110U) /**< \brief (SSC) Receive Next Pointer Register */
#define REG_SSC_RNCR REG_ACCESS(RwReg, 0x40004114U) /**< \brief (SSC) Receive Next Counter Register */
#define REG_SSC_TNPR REG_ACCESS(RwReg, 0x40004118U) /**< \brief (SSC) Transmit Next Pointer Register */
#define REG_SSC_TNCR REG_ACCESS(RwReg, 0x4000411CU) /**< \brief (SSC) Transmit Next Counter Register */
#define REG_SSC_PTCR REG_ACCESS(WoReg, 0x40004120U) /**< \brief (SSC) Transfer Control Register */
#define REG_SSC_PTSR REG_ACCESS(RoReg, 0x40004124U) /**< \brief (SSC) Transfer Status Register */

#endif /* _SAM3S8_SSC_INSTANCE_ */
