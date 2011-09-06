/* $asf_license$ */

#ifndef _SAM3N_UART0_INSTANCE_
#define _SAM3N_UART0_INSTANCE_

/* ========== Register definition for UART0 peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_UART0_CR            (0x400E0600U) /**< \brief (UART0) Control Register */
#define REG_UART0_MR            (0x400E0604U) /**< \brief (UART0) Mode Register */
#define REG_UART0_IER           (0x400E0608U) /**< \brief (UART0) Interrupt Enable Register */
#define REG_UART0_IDR           (0x400E060CU) /**< \brief (UART0) Interrupt Disable Register */
#define REG_UART0_IMR           (0x400E0610U) /**< \brief (UART0) Interrupt Mask Register */
#define REG_UART0_SR            (0x400E0614U) /**< \brief (UART0) Status Register */
#define REG_UART0_RHR           (0x400E0618U) /**< \brief (UART0) Receive Holding Register */
#define REG_UART0_THR           (0x400E061CU) /**< \brief (UART0) Transmit Holding Register */
#define REG_UART0_BRGR          (0x400E0620U) /**< \brief (UART0) Baud Rate Generator Register */
#define REG_UART0_RPR           (0x400E0700U) /**< \brief (UART0) Receive Pointer Register */
#define REG_UART0_RCR           (0x400E0704U) /**< \brief (UART0) Receive Counter Register */
#define REG_UART0_TPR           (0x400E0708U) /**< \brief (UART0) Transmit Pointer Register */
#define REG_UART0_TCR           (0x400E070CU) /**< \brief (UART0) Transmit Counter Register */
#define REG_UART0_RNPR          (0x400E0710U) /**< \brief (UART0) Receive Next Pointer Register */
#define REG_UART0_RNCR          (0x400E0714U) /**< \brief (UART0) Receive Next Counter Register */
#define REG_UART0_TNPR          (0x400E0718U) /**< \brief (UART0) Transmit Next Pointer Register */
#define REG_UART0_TNCR          (0x400E071CU) /**< \brief (UART0) Transmit Next Counter Register */
#define REG_UART0_PTCR          (0x400E0720U) /**< \brief (UART0) Transfer Control Register */
#define REG_UART0_PTSR          (0x400E0724U) /**< \brief (UART0) Transfer Status Register */
#else
#define REG_UART0_CR   (*(WoReg*)0x400E0600U) /**< \brief (UART0) Control Register */
#define REG_UART0_MR   (*(RwReg*)0x400E0604U) /**< \brief (UART0) Mode Register */
#define REG_UART0_IER  (*(WoReg*)0x400E0608U) /**< \brief (UART0) Interrupt Enable Register */
#define REG_UART0_IDR  (*(WoReg*)0x400E060CU) /**< \brief (UART0) Interrupt Disable Register */
#define REG_UART0_IMR  (*(RoReg*)0x400E0610U) /**< \brief (UART0) Interrupt Mask Register */
#define REG_UART0_SR   (*(RoReg*)0x400E0614U) /**< \brief (UART0) Status Register */
#define REG_UART0_RHR  (*(RoReg*)0x400E0618U) /**< \brief (UART0) Receive Holding Register */
#define REG_UART0_THR  (*(WoReg*)0x400E061CU) /**< \brief (UART0) Transmit Holding Register */
#define REG_UART0_BRGR (*(RwReg*)0x400E0620U) /**< \brief (UART0) Baud Rate Generator Register */
#define REG_UART0_RPR  (*(RwReg*)0x400E0700U) /**< \brief (UART0) Receive Pointer Register */
#define REG_UART0_RCR  (*(RwReg*)0x400E0704U) /**< \brief (UART0) Receive Counter Register */
#define REG_UART0_TPR  (*(RwReg*)0x400E0708U) /**< \brief (UART0) Transmit Pointer Register */
#define REG_UART0_TCR  (*(RwReg*)0x400E070CU) /**< \brief (UART0) Transmit Counter Register */
#define REG_UART0_RNPR (*(RwReg*)0x400E0710U) /**< \brief (UART0) Receive Next Pointer Register */
#define REG_UART0_RNCR (*(RwReg*)0x400E0714U) /**< \brief (UART0) Receive Next Counter Register */
#define REG_UART0_TNPR (*(RwReg*)0x400E0718U) /**< \brief (UART0) Transmit Next Pointer Register */
#define REG_UART0_TNCR (*(RwReg*)0x400E071CU) /**< \brief (UART0) Transmit Next Counter Register */
#define REG_UART0_PTCR (*(WoReg*)0x400E0720U) /**< \brief (UART0) Transfer Control Register */
#define REG_UART0_PTSR (*(RoReg*)0x400E0724U) /**< \brief (UART0) Transfer Status Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3N_UART0_INSTANCE_ */
