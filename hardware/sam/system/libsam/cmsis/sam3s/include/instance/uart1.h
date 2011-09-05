/* %ATMEL_LICENCE% */

#ifndef _SAM3S_UART1_INSTANCE_
#define _SAM3S_UART1_INSTANCE_

/* ========== Register definition for UART1 peripheral ========== */
#define REG_UART1_CR   REG_ACCESS(WoReg, 0x400E0800U) /**< \brief (UART1) Control Register */
#define REG_UART1_MR   REG_ACCESS(RwReg, 0x400E0804U) /**< \brief (UART1) Mode Register */
#define REG_UART1_IER  REG_ACCESS(WoReg, 0x400E0808U) /**< \brief (UART1) Interrupt Enable Register */
#define REG_UART1_IDR  REG_ACCESS(WoReg, 0x400E080CU) /**< \brief (UART1) Interrupt Disable Register */
#define REG_UART1_IMR  REG_ACCESS(RoReg, 0x400E0810U) /**< \brief (UART1) Interrupt Mask Register */
#define REG_UART1_SR   REG_ACCESS(RoReg, 0x400E0814U) /**< \brief (UART1) Status Register */
#define REG_UART1_RHR  REG_ACCESS(RoReg, 0x400E0818U) /**< \brief (UART1) Receive Holding Register */
#define REG_UART1_THR  REG_ACCESS(WoReg, 0x400E081CU) /**< \brief (UART1) Transmit Holding Register */
#define REG_UART1_BRGR REG_ACCESS(RwReg, 0x400E0820U) /**< \brief (UART1) Baud Rate Generator Register */
#define REG_UART1_RPR  REG_ACCESS(RwReg, 0x400E0900U) /**< \brief (UART1) Receive Pointer Register */
#define REG_UART1_RCR  REG_ACCESS(RwReg, 0x400E0904U) /**< \brief (UART1) Receive Counter Register */
#define REG_UART1_TPR  REG_ACCESS(RwReg, 0x400E0908U) /**< \brief (UART1) Transmit Pointer Register */
#define REG_UART1_TCR  REG_ACCESS(RwReg, 0x400E090CU) /**< \brief (UART1) Transmit Counter Register */
#define REG_UART1_RNPR REG_ACCESS(RwReg, 0x400E0910U) /**< \brief (UART1) Receive Next Pointer Register */
#define REG_UART1_RNCR REG_ACCESS(RwReg, 0x400E0914U) /**< \brief (UART1) Receive Next Counter Register */
#define REG_UART1_TNPR REG_ACCESS(RwReg, 0x400E0918U) /**< \brief (UART1) Transmit Next Pointer Register */
#define REG_UART1_TNCR REG_ACCESS(RwReg, 0x400E091CU) /**< \brief (UART1) Transmit Next Counter Register */
#define REG_UART1_PTCR REG_ACCESS(WoReg, 0x400E0920U) /**< \brief (UART1) Transfer Control Register */
#define REG_UART1_PTSR REG_ACCESS(RoReg, 0x400E0924U) /**< \brief (UART1) Transfer Status Register */

#endif /* _SAM3S_UART1_INSTANCE_ */
