/* %ATMEL_LICENCE% */

#ifndef _SAM3N_UART1_INSTANCE_
#define _SAM3N_UART1_INSTANCE_

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

#endif /* _SAM3N_UART1_INSTANCE_ */
