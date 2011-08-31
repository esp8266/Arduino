/* %ATMEL_LICENCE% */

#ifndef _SAM3S_UART0_INSTANCE_
#define _SAM3S_UART0_INSTANCE_

/* ========== Register definition for UART0 peripheral ========== */
#define REG_UART0_CR   REG_ACCESS(WoReg, 0x400E0600U) /**< \brief (UART0) Control Register */
#define REG_UART0_MR   REG_ACCESS(RwReg, 0x400E0604U) /**< \brief (UART0) Mode Register */
#define REG_UART0_IER  REG_ACCESS(WoReg, 0x400E0608U) /**< \brief (UART0) Interrupt Enable Register */
#define REG_UART0_IDR  REG_ACCESS(WoReg, 0x400E060CU) /**< \brief (UART0) Interrupt Disable Register */
#define REG_UART0_IMR  REG_ACCESS(RoReg, 0x400E0610U) /**< \brief (UART0) Interrupt Mask Register */
#define REG_UART0_SR   REG_ACCESS(RoReg, 0x400E0614U) /**< \brief (UART0) Status Register */
#define REG_UART0_RHR  REG_ACCESS(RoReg, 0x400E0618U) /**< \brief (UART0) Receive Holding Register */
#define REG_UART0_THR  REG_ACCESS(WoReg, 0x400E061CU) /**< \brief (UART0) Transmit Holding Register */
#define REG_UART0_BRGR REG_ACCESS(RwReg, 0x400E0620U) /**< \brief (UART0) Baud Rate Generator Register */
#define REG_UART0_RPR  REG_ACCESS(RwReg, 0x400E0700U) /**< \brief (UART0) Receive Pointer Register */
#define REG_UART0_RCR  REG_ACCESS(RwReg, 0x400E0704U) /**< \brief (UART0) Receive Counter Register */
#define REG_UART0_TPR  REG_ACCESS(RwReg, 0x400E0708U) /**< \brief (UART0) Transmit Pointer Register */
#define REG_UART0_TCR  REG_ACCESS(RwReg, 0x400E070CU) /**< \brief (UART0) Transmit Counter Register */
#define REG_UART0_RNPR REG_ACCESS(RwReg, 0x400E0710U) /**< \brief (UART0) Receive Next Pointer Register */
#define REG_UART0_RNCR REG_ACCESS(RwReg, 0x400E0714U) /**< \brief (UART0) Receive Next Counter Register */
#define REG_UART0_TNPR REG_ACCESS(RwReg, 0x400E0718U) /**< \brief (UART0) Transmit Next Pointer Register */
#define REG_UART0_TNCR REG_ACCESS(RwReg, 0x400E071CU) /**< \brief (UART0) Transmit Next Counter Register */
#define REG_UART0_PTCR REG_ACCESS(WoReg, 0x400E0720U) /**< \brief (UART0) Transfer Control Register */
#define REG_UART0_PTSR REG_ACCESS(RoReg, 0x400E0724U) /**< \brief (UART0) Transfer Status Register */

#endif /* _SAM3S_UART0_INSTANCE_ */
