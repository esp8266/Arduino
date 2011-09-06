/* $asf_license$ */

#ifndef _SAM3S8_UDP_INSTANCE_
#define _SAM3S8_UDP_INSTANCE_

/* ========== Register definition for UDP peripheral ========== */
#ifdef __ASSEMBLY__
#define REG_UDP_FRM_NUM           (0x40034000U) /**< \brief (UDP) Frame Number Register */
#define REG_UDP_GLB_STAT          (0x40034004U) /**< \brief (UDP) Global State Register */
#define REG_UDP_FADDR             (0x40034008U) /**< \brief (UDP) Function Address Register */
#define REG_UDP_IER               (0x40034010U) /**< \brief (UDP) Interrupt Enable Register */
#define REG_UDP_IDR               (0x40034014U) /**< \brief (UDP) Interrupt Disable Register */
#define REG_UDP_IMR               (0x40034018U) /**< \brief (UDP) Interrupt Mask Register */
#define REG_UDP_ISR               (0x4003401CU) /**< \brief (UDP) Interrupt Status Register */
#define REG_UDP_ICR               (0x40034020U) /**< \brief (UDP) Interrupt Clear Register */
#define REG_UDP_RST_EP            (0x40034028U) /**< \brief (UDP) Reset Endpoint Register */
#define REG_UDP_CSR               (0x40034030U) /**< \brief (UDP) Endpoint Control and Status Register */
#define REG_UDP_FDR               (0x40034050U) /**< \brief (UDP) Endpoint FIFO Data Register */
#define REG_UDP_TXVC              (0x40034074U) /**< \brief (UDP) Transceiver Control Register */
#else
#define REG_UDP_FRM_NUM  (*(RoReg*)0x40034000U) /**< \brief (UDP) Frame Number Register */
#define REG_UDP_GLB_STAT (*(RwReg*)0x40034004U) /**< \brief (UDP) Global State Register */
#define REG_UDP_FADDR    (*(RwReg*)0x40034008U) /**< \brief (UDP) Function Address Register */
#define REG_UDP_IER      (*(WoReg*)0x40034010U) /**< \brief (UDP) Interrupt Enable Register */
#define REG_UDP_IDR      (*(WoReg*)0x40034014U) /**< \brief (UDP) Interrupt Disable Register */
#define REG_UDP_IMR      (*(RoReg*)0x40034018U) /**< \brief (UDP) Interrupt Mask Register */
#define REG_UDP_ISR      (*(RoReg*)0x4003401CU) /**< \brief (UDP) Interrupt Status Register */
#define REG_UDP_ICR      (*(WoReg*)0x40034020U) /**< \brief (UDP) Interrupt Clear Register */
#define REG_UDP_RST_EP   (*(RwReg*)0x40034028U) /**< \brief (UDP) Reset Endpoint Register */
#define REG_UDP_CSR      (*(RwReg*)0x40034030U) /**< \brief (UDP) Endpoint Control and Status Register */
#define REG_UDP_FDR      (*(RwReg*)0x40034050U) /**< \brief (UDP) Endpoint FIFO Data Register */
#define REG_UDP_TXVC     (*(RwReg*)0x40034074U) /**< \brief (UDP) Transceiver Control Register */
#endif /* __ASSEMBLY__ */

#endif /* _SAM3S8_UDP_INSTANCE_ */
