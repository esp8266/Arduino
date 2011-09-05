/* %ATMEL_LICENCE% */

#ifndef _SAM3S8_UDP_INSTANCE_
#define _SAM3S8_UDP_INSTANCE_

/* ========== Register definition for UDP peripheral ========== */
#define REG_UDP_FRM_NUM  REG_ACCESS(RoReg, 0x40034000U) /**< \brief (UDP) Frame Number Register */
#define REG_UDP_GLB_STAT REG_ACCESS(RwReg, 0x40034004U) /**< \brief (UDP) Global State Register */
#define REG_UDP_FADDR    REG_ACCESS(RwReg, 0x40034008U) /**< \brief (UDP) Function Address Register */
#define REG_UDP_IER      REG_ACCESS(WoReg, 0x40034010U) /**< \brief (UDP) Interrupt Enable Register */
#define REG_UDP_IDR      REG_ACCESS(WoReg, 0x40034014U) /**< \brief (UDP) Interrupt Disable Register */
#define REG_UDP_IMR      REG_ACCESS(RoReg, 0x40034018U) /**< \brief (UDP) Interrupt Mask Register */
#define REG_UDP_ISR      REG_ACCESS(RoReg, 0x4003401CU) /**< \brief (UDP) Interrupt Status Register */
#define REG_UDP_ICR      REG_ACCESS(WoReg, 0x40034020U) /**< \brief (UDP) Interrupt Clear Register */
#define REG_UDP_RST_EP   REG_ACCESS(RwReg, 0x40034028U) /**< \brief (UDP) Reset Endpoint Register */
#define REG_UDP_CSR      REG_ACCESS(RwReg, 0x40034030U) /**< \brief (UDP) Endpoint Control and Status Register */
#define REG_UDP_FDR      REG_ACCESS(RwReg, 0x40034050U) /**< \brief (UDP) Endpoint FIFO Data Register */
#define REG_UDP_TXVC     REG_ACCESS(RwReg, 0x40034074U) /**< \brief (UDP) Transceiver Control Register */

#endif /* _SAM3S8_UDP_INSTANCE_ */
