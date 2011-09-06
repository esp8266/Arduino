/* $asf_license$ */

#ifndef _SAM3S_UART1_INSTANCE_
#define _SAM3S_UART1_INSTANCE_

/* ========== Register definition for UART1 peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_UART1_CR            (0x400E0800U) /**< \brief (UART1) Control Register */
#define REG_UART1_MR            (0x400E0804U) /**< \brief (UART1) Mode Register */
#define REG_UART1_IER           (0x400E0808U) /**< \brief (UART1) Interrupt Enable Register */
#define REG_UART1_IDR           (0x400E080CU) /**< \brief (UART1) Interrupt Disable Register */
#define REG_UART1_IMR           (0x400E0810U) /**< \brief (UART1) Interrupt Mask Register */
#define REG_UART1_SR            (0x400E0814U) /**< \brief (UART1) Status Register */
#define REG_UART1_RHR           (0x400E0818U) /**< \brief (UART1) Receive Holding Register */
#define REG_UART1_THR           (0x400E081CU) /**< \brief (UART1) Transmit Holding Register */
#define REG_UART1_BRGR          (0x400E0820U) /**< \brief (UART1) Baud Rate Generator Register */
#define REG_UART1_RPR           (0x400E0900U) /**< \brief (UART1) Receive Pointer Register */
#define REG_UART1_RCR           (0x400E0904U) /**< \brief (UART1) Receive Counter Register */
#define REG_UART1_TPR           (0x400E0908U) /**< \brief (UART1) Transmit Pointer Register */
#define REG_UART1_TCR           (0x400E090CU) /**< \brief (UART1) Transmit Counter Register */
#define REG_UART1_RNPR          (0x400E0910U) /**< \brief (UART1) Receive Next Pointer Register */
#define REG_UART1_RNCR          (0x400E0914U) /**< \brief (UART1) Receive Next Counter Register */
#define REG_UART1_TNPR          (0x400E0918U) /**< \brief (UART1) Transmit Next Pointer Register */
#define REG_UART1_TNCR          (0x400E091CU) /**< \brief (UART1) Transmit Next Counter Register */
#define REG_UART1_PTCR          (0x400E0920U) /**< \brief (UART1) Transfer Control Register */
#define REG_UART1_PTSR          (0x400E0924U) /**< \brief (UART1) Transfer Status Register */
#else
#define REG_UART1_CR   (*(WoReg*)0x400E0800U) /**< \brief (UART1) Control Register */
#define REG_UART1_MR   (*(RwReg*)0x400E0804U) /**< \brief (UART1) Mode Register */
#define REG_UART1_IER  (*(WoReg*)0x400E0808U) /**< \brief (UART1) Interrupt Enable Register */
#define REG_UART1_IDR  (*(WoReg*)0x400E080CU) /**< \brief (UART1) Interrupt Disable Register */
#define REG_UART1_IMR  (*(RoReg*)0x400E0810U) /**< \brief (UART1) Interrupt Mask Register */
#define REG_UART1_SR   (*(RoReg*)0x400E0814U) /**< \brief (UART1) Status Register */
#define REG_UART1_RHR  (*(RoReg*)0x400E0818U) /**< \brief (UART1) Receive Holding Register */
#define REG_UART1_THR  (*(WoReg*)0x400E081CU) /**< \brief (UART1) Transmit Holding Register */
#define REG_UART1_BRGR (*(RwReg*)0x400E0820U) /**< \brief (UART1) Baud Rate Generator Register */
#define REG_UART1_RPR  (*(RwReg*)0x400E0900U) /**< \brief (UART1) Receive Pointer Register */
#define REG_UART1_RCR  (*(RwReg*)0x400E0904U) /**< \brief (UART1) Receive Counter Register */
#define REG_UART1_TPR  (*(RwReg*)0x400E0908U) /**< \brief (UART1) Transmit Pointer Register */
#define REG_UART1_TCR  (*(RwReg*)0x400E090CU) /**< \brief (UART1) Transmit Counter Register */
#define REG_UART1_RNPR (*(RwReg*)0x400E0910U) /**< \brief (UART1) Receive Next Pointer Register */
#define REG_UART1_RNCR (*(RwReg*)0x400E0914U) /**< \brief (UART1) Receive Next Counter Register */
#define REG_UART1_TNPR (*(RwReg*)0x400E0918U) /**< \brief (UART1) Transmit Next Pointer Register */
#define REG_UART1_TNCR (*(RwReg*)0x400E091CU) /**< \brief (UART1) Transmit Next Counter Register */
#define REG_UART1_PTCR (*(WoReg*)0x400E0920U) /**< \brief (UART1) Transfer Control Register */
#define REG_UART1_PTSR (*(RoReg*)0x400E0924U) /**< \brief (UART1) Transfer Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3S_UART1_INSTANCE_ */
