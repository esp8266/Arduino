/* %ATMEL_LICENCE% */

#ifndef _SAM3XA_UART_INSTANCE_
#define _SAM3XA_UART_INSTANCE_

/* ========== Register definition for UART peripheral ========== */
#define REG_UART_CR   REG_ACCESS(WoReg, 0x400E0800U) /**< \brief (UART) Control Register */
#define REG_UART_MR   REG_ACCESS(RwReg, 0x400E0804U) /**< \brief (UART) Mode Register */
#define REG_UART_IER  REG_ACCESS(WoReg, 0x400E0808U) /**< \brief (UART) Interrupt Enable Register */
#define REG_UART_IDR  REG_ACCESS(WoReg, 0x400E080CU) /**< \brief (UART) Interrupt Disable Register */
#define REG_UART_IMR  REG_ACCESS(RoReg, 0x400E0810U) /**< \brief (UART) Interrupt Mask Register */
#define REG_UART_SR   REG_ACCESS(RoReg, 0x400E0814U) /**< \brief (UART) Status Register */
#define REG_UART_RHR  REG_ACCESS(RoReg, 0x400E0818U) /**< \brief (UART) Receive Holding Register */
#define REG_UART_THR  REG_ACCESS(WoReg, 0x400E081CU) /**< \brief (UART) Transmit Holding Register */
#define REG_UART_BRGR REG_ACCESS(RwReg, 0x400E0820U) /**< \brief (UART) Baud Rate Generator Register */
#define REG_UART_RPR  REG_ACCESS(RwReg, 0x400E0900U) /**< \brief (UART) Receive Pointer Register */
#define REG_UART_RCR  REG_ACCESS(RwReg, 0x400E0904U) /**< \brief (UART) Receive Counter Register */
#define REG_UART_TPR  REG_ACCESS(RwReg, 0x400E0908U) /**< \brief (UART) Transmit Pointer Register */
#define REG_UART_TCR  REG_ACCESS(RwReg, 0x400E090CU) /**< \brief (UART) Transmit Counter Register */
#define REG_UART_RNPR REG_ACCESS(RwReg, 0x400E0910U) /**< \brief (UART) Receive Next Pointer Register */
#define REG_UART_RNCR REG_ACCESS(RwReg, 0x400E0914U) /**< \brief (UART) Receive Next Counter Register */
#define REG_UART_TNPR REG_ACCESS(RwReg, 0x400E0918U) /**< \brief (UART) Transmit Next Pointer Register */
#define REG_UART_TNCR REG_ACCESS(RwReg, 0x400E091CU) /**< \brief (UART) Transmit Next Counter Register */
#define REG_UART_PTCR REG_ACCESS(WoReg, 0x400E0920U) /**< \brief (UART) Transfer Control Register */
#define REG_UART_PTSR REG_ACCESS(RoReg, 0x400E0924U) /**< \brief (UART) Transfer Status Register */

#endif /* _SAM3XA_UART_INSTANCE_ */
